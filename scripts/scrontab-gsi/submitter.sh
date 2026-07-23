#!/bin/bash

# `esd2vector/scripts/scrontab-gsi/submitter.sh`
# ==============================================
# Phase 1: submit grid jobs for every run number in the RN list not yet tracked in the sqlite3 DB.
# Phase 2: one monitoring pass over tracked jobs.
# - DONE jobs are marked as such.
# - Jobs in ERROR are resubmitted only once.
# - Jobs WAITING (or in an unknown state) for more than MAX_WAIT_MINUTES are resubmitted only once.
# Single-pass "tick" meant to be scheduled with scrontab; it runs indefinitely tick after tick:
# append run numbers to the RN list at any time and the next tick picks them up.
#
# Ideal usage:
# 1. `source .env`
# 2. - `scrontab submitter_rd.scrontab`
#    - `scrontab submitter_mc.scrontab`

set -euo pipefail

# environment
if [[ -z ${E2T_ROOT_DIR} ]]; then echo "error: missing env. var. E2T_ROOT_DIR"; exit 1; fi
source "${E2T_ROOT_DIR}/scrontab-gsi/looper_lib.sh"
# >> gsi-related
scrontab_require_env LUSTRE_HOME TMP TMPDIR
# >> alice-related
scrontab_require_env ALIENV_CVMFSDIR JALIEN_TOKEN_CERT JALIEN_TOKEN_KEY ALIPHYSICS_RELEASE
eval "$(${ALIENV_CVMFSDIR}/bin/alienv.safe printenv VO_ALICE@AliPhysics::${ALIPHYSICS_RELEASE})"
# >> mine
scrontab_require_env JALIEN_BORQUEZ_HOME
# >> generate remaining env. vars.
scrontab_init
scrontab_require_env E2T_OUTPUT_DIR JALIEN_USERNAME E2T_USER_SCT_DIR JALIEN_HOME

# command-line argument
if [[ $# -ne 1 || ( $1 != "mc" && $1 != "rd" ) ]]; then echo "usage: ./submitter.sh <mc|rd>"; exit 1; fi
DATA_KIND=$1

# hardcoded params
SCRIPT_NAME=$(basename "$0")
MAX_SUBMIT_RETRIES=2
MAX_WAIT_MINUTES=60           # tolerance before resubmitting a job stuck in WAITING/unknown
MAX_POLLS_SUBJOBS_APPPEAR=10  # polls of 30 seconds each, waiting for subjobs to appear
# -- mc related
MC_SIGNAL_CHANNELS=("A" "D" "H")
MC_SEXAQUARK_MASSES=("1.73" "1.8" "1.87" "1.94" "2.01")

# don't overlap with a previous tick that's still running
scrontab_acquire_lock "submitter_${DATA_KIND}"

# check db
DB="${E2T_ROOT_DIR}/scrontab-gsi/${JALIEN_USERNAME}_${DATA_KIND}.db"
db_ensure_exists "${DB}"

# input
RUN_NUMBERS_LIST="${E2T_ROOT_DIR}/scrontab-gsi/${JALIEN_USERNAME}_RN.txt"
if [[ ! -f ${RUN_NUMBERS_LIST} ]]; then
    log_msg "${SCRIPT_NAME}" "init" "run numbers list not found: ${RUN_NUMBERS_LIST}"
    exit 1
fi

# stage analysis files lazily, only when this tick actually submits something
STAGING_DIR="${E2T_ROOT_DIR}/scripts/${JALIEN_USERNAME}"
mkdir -p "${STAGING_DIR}/tmp"
staged=0
ensure_staged() {
    if [[ ${staged} -eq 0 ]]; then
        stage_analysis_files "${STAGING_DIR}"
        cd "${STAGING_DIR}"
        staged=1
    fi
}

n_new_subjobs=0

# Submit a single masterjob and record its subjobs in the DB.
# usage: submit_one <run_number> <sim_set> <input_path> <production_name> \
#                   <grid_working_dir> <include_tidentity> <custom_xml> <local_dir> <local_prefix>
submit_one() {
    local run_number=$1 sim_set=$2 input_path=$3 production_name=$4
    local grid_working_dir=$5 include_tidentity=$6 custom_xml=$7
    local local_dir=$8 local_prefix=$9

    local tag="${run_number}${sim_set:+_${sim_set}}"

    # skip if already tracked
    local n_tracked
    n_tracked=$(db_exec "${DB}" "SELECT COUNT(*) FROM subjobs WHERE run_number=${run_number} AND sim_set='${sim_set}';")
    if [[ ${n_tracked} -gt 0 ]]; then
        log_msg "${SCRIPT_NAME}" "${tag}" "already tracked in DB, skipping"
        return 0
    fi

    ensure_staged

    local aliroot_command
    aliroot_command=$(build_analysis_command "grid" "${input_path}" "${production_name}" "${run_number}" \
                                             0 0 "${include_tidentity}" "${grid_working_dir}" "${custom_xml}")
    local log_file
    log_file="${STAGING_DIR}/tmp/${tag}_$(date +%s).log"

    # submit jobs (tolerate failures: a broken submission only skips this entry)
    log_msg "${SCRIPT_NAME}" "${tag}" "executing: ${aliroot_command}"
    ${aliroot_command} 2>&1 | tee "${log_file}" || true

    # find masterjob id
    local masterjob_id
    masterjob_id=$(grep "THE JOB ID IS:" "${log_file}" | tail -n1 | awk '{ print $NF }' || true)
    if [[ ! ${masterjob_id} =~ ^[0-9]+$ ]]; then
        log_msg "${SCRIPT_NAME}" "${tag}" "submission failed, no masterjob_id found"
        return 0
    fi
    log_msg "${SCRIPT_NAME}" "${tag}" "submitted masterjob_id ${masterjob_id}"
    rm "${log_file}"

    # wait until the subjobs appear
    # example output of `alien.py ps -m`:
    # ```
    #     aborquez 3525965811    W                         TaskEsd2Tree.sh
    #     aborquez 3525965812    D                         TaskEsd2Tree.sh
    # ```
    local subjobs_arr=()
    local attempt
    for (( attempt = 0; attempt < MAX_POLLS_SUBJOBS_APPPEAR; attempt++ )); do
        mapfile -t subjobs_arr < <(alien.py ps -m "${masterjob_id}" 2>/dev/null |
                                   awk -v m="${masterjob_id}" '$2 ~ /^[0-9]+$/ && $2 != m { print $2 }' || true)
        [[ ${#subjobs_arr[@]} -gt 0 ]] && break
        sleep 30 # seconds
    done
    if [[ ${#subjobs_arr[@]} -eq 0 ]]; then
        log_msg "${SCRIPT_NAME}" "${tag}" "couldn't get subjobs of masterjob ${masterjob_id}, skipping"
        return 0
    fi

    # record subjobs in DB
    local subjob_id remote_dir outdir_number local_file
    for subjob_id in "${subjobs_arr[@]}"; do
        # example output of `alien.py ps -jdl <job_id>`:
        # ```
        # ...
        # OutputDir = "/alice/cern.ch/user/a/aborquez/work/E2T/rd_18q/295585/output/017";
        # ...
        # ```
        remote_dir=$(alien.py ps -jdl "${subjob_id}" 2>/dev/null | sed -n 's/.*OutputDir = "\(.*\)";/\1/p' || true)
        if [[ -z ${remote_dir} ]]; then
            log_msg "${SCRIPT_NAME}" "${tag}" "WARNING: no OutputDir for subjob ${subjob_id}, not tracking it"
            continue
        fi
        outdir_number=$(basename "${remote_dir}")
        local_file="${local_dir}/AnalysisResults_${outdir_number}${local_prefix}.root"
        db_exec "${DB}" "
            INSERT OR IGNORE INTO subjobs
            (subjob_id, run_number, sim_set, masterjob_id, outdir_number, remote_dir, local_file, state, updated_at)
            VALUES (${subjob_id}, ${run_number}, '${sim_set}', ${masterjob_id}, '${outdir_number}',
                    '${remote_dir}', '${local_file}', 'SUBMITTED', datetime('now'));
        "
        n_new_subjobs=$(( n_new_subjobs + 1 ))
    done
    log_msg "${SCRIPT_NAME}" "${tag}" "recorded ${#subjobs_arr[@]} subjobs of masterjob ${masterjob_id}"
}

# -- 1. submit jobs for all run numbers in the list that are not yet tracked -- #

mapfile -t run_numbers < <(grep -vE '^\s*(#|$)' "${RUN_NUMBERS_LIST}")
quota_exceeded=0

# loop over run numbers
for run_number in "${run_numbers[@]}"; do
    run_number=${run_number//[[:space:]]/}

    # resolve production
    production_path=$(resolve_production_path "${DATA_KIND}" "${run_number}")
    if [[ -z ${production_path} ]]; then
        log_msg "${SCRIPT_NAME}" "${run_number}" "could not resolve production, skipping"
        continue
    fi
    production_name=$(basename "${production_path}") # e.g. LHC23l1a3, LHC18q
    red_prod_name=${production_name#LHC}             # reduced, removed LHC, e.g. 23l1a3, 18q
    log_msg "${SCRIPT_NAME}" "${run_number}" "production path: ${production_path}"

    # check quota
    if ! check_jquota; then
        log_msg "${SCRIPT_NAME}" "${run_number}" "jquota exceeded, stopping submissions"
        quota_exceeded=1
        break
    fi
    if ! check_fquota; then
        log_msg "${SCRIPT_NAME}" "${run_number}" "fquota exceeded, stopping submissions"
        quota_exceeded=1
        break
    fi

    if [[ ${DATA_KIND} == "mc" ]]; then
        # loop over channels+masses pairs
        for signal_channel in "${MC_SIGNAL_CHANNELS[@]}"; do
            for sexaquark_mass in "${MC_SEXAQUARK_MASSES[@]}"; do
                sim_set="${signal_channel}${sexaquark_mass}"
                submit_one "${run_number}" "${sim_set}" \
                           "${production_path}/${sim_set}" "${production_name}" \
                           "work/E2T/mc_${red_prod_name}_${sim_set}" 0 "" \
                           "${E2T_OUTPUT_DIR}/grid_mc_${red_prod_name}/${sim_set}/${run_number}" ""
            done # end of masses loop
        done # end of channels loop
    else
        # big run numbers have pre-generated XML chunks (see `scripts/generate_xml.sh`), mirrored in the grid home dir
        xml_rn_dir="${E2T_ROOT_DIR}/xml/${production_name}/${run_number}"
        local_dir="${E2T_OUTPUT_DIR}/grid_rd_${red_prod_name}/${run_number}"
        if [[ -d ${xml_rn_dir} ]]; then
            mapfile -t xml_files < <(find "${xml_rn_dir}" -name "*.xml" | sort)
            for index in "${!xml_files[@]}"; do
                chunk=$(( index + 1 ))
                custom_xml="${JALIEN_BORQUEZ_HOME}/xml/${production_name}/${run_number}/$(basename "${xml_files[${index}]}")"
                submit_one "${run_number}" "xml${chunk}" \
                           "${production_path}" "${production_name}" \
                           "work/E2T/rd_${red_prod_name}" 1 "${custom_xml}" \
                           "${local_dir}" "_${chunk}"
            done
        else
            submit_one "${run_number}" "" \
                       "${production_path}" "${production_name}" \
                       "work/E2T/rd_${red_prod_name}" 1 "" \
                       "${local_dir}" ""
        fi
    fi

done # end of loop over run numbers

if [[ ${quota_exceeded} -eq 1 ]]; then
    log_msg "${SCRIPT_NAME}" "done" "could not submit all run numbers from ${RUN_NUMBERS_LIST}, because quota exceeded"
else
    log_msg "${SCRIPT_NAME}" "done" "finished submitting all run numbers from ${RUN_NUMBERS_LIST}"
fi
log_msg "${SCRIPT_NAME}" "done" "n_new_subjobs = ${n_new_subjobs}"

# -- 2. one monitoring pass: update job states, resubmitting each failing/stale job only once -- #

resubmit_subjob() {
    local subjob_id=$1
    alien.py resubmit "${subjob_id}"
    db_exec "${DB}" "UPDATE subjobs SET state='RESUBMITTED', n_retries=n_retries+1, updated_at=datetime('now') WHERE subjob_id=${subjob_id};"
    log_msg "${SCRIPT_NAME}" "resubmitter" "resubmitted subjob ${subjob_id}"
}

retry_or_give_up() {
    local subjob_id=$1 n_retries=$2
    if [[ ${n_retries} -lt ${MAX_SUBMIT_RETRIES} ]]; then
        resubmit_subjob "${subjob_id}"
    else
        db_exec "${DB}" "UPDATE subjobs SET state='GIVEN_UP', updated_at=datetime('now') WHERE subjob_id=${subjob_id};"
        log_msg "${SCRIPT_NAME}" "resubmitter" "gave up on subjob ${subjob_id}"
    fi
}

# Check the last trace timestamp of a subjob; resubmit/give up if stale.
# example output of `alien.py ps -trace <job_id>`:
# ```
# 2026-02-16 20:01:32+0100 [state     ]: Job state transition to WAITING
# 2026-02-16 20:01:32+0100 [trace     ]: Job inserted by aliendb9.cern.ch [Masterjob is 3525953703]
# ```
check_stale() {
    local subjob_id=$1 n_retries=$2
    local time_last_trace
    time_last_trace=$(alien.py ps -trace "${subjob_id}" 2>/dev/null | tail -n1 | awk '{ printf "%s %s", $1, $2 }' || true)
    if [[ -z ${time_last_trace} ]]; then
        # assume stale if no trace
        retry_or_give_up "${subjob_id}" "${n_retries}"
        return 0
    fi
    local minutes_passed
    minutes_passed=$(minutes_between "${time_last_trace}" "$(date)")
    if [[ ${minutes_passed} -ge 0 && ${minutes_passed} -gt ${MAX_WAIT_MINUTES} ]]; then
        retry_or_give_up "${subjob_id}" "${n_retries}"
    fi
}

n_pending=$(db_exec "${DB}" "SELECT COUNT(*) FROM subjobs WHERE state IN ('SUBMITTED','RESUBMITTED');")
if [[ ${n_pending} -eq 0 ]]; then
    log_msg "${SCRIPT_NAME}" "resubmitter" "no pending jobs to poll"
    exit 0
fi

n_total=$(db_exec "${DB}" "SELECT COUNT(*) FROM subjobs;")
log_msg "${SCRIPT_NAME}" "resubmitter" "polling ${n_pending} pending jobs (of ${n_total} tracked)"

mapfile -t active_masterjobs < <(db_exec "${DB}" "SELECT DISTINCT masterjob_id FROM subjobs WHERE state IN ('SUBMITTED','RESUBMITTED');")

# get subjobs info for each masterjob
ps_lines=()
for masterjob_id in "${active_masterjobs[@]}"; do
    # example output of `alien.py ps`:
    # ```
    #     aborquez 3525965811    W                         TaskEsd2Vector.sh
    #     aborquez 3525965812    D                         TaskEsd2Vector.sh
    #     aborquez 3525965818   EE                         TaskEsd2Vector.sh
    # alihyperloop 3668154550    D                      hyperloop_merge10.sh # to verify this, you need to add -a
    # ```
    mapfile -t tmp_lines < <(alien.py ps -m "${masterjob_id}" 2>/dev/null || true)
    ps_lines+=("${tmp_lines[@]}")
done

for line in "${ps_lines[@]}"; do
    read -r _ subjob_id state_in_grid _ <<< "${line}"
    [[ ${subjob_id} =~ ^[0-9]+$ ]] || continue

    row=$(db_exec "${DB}" "SELECT state, n_retries FROM subjobs WHERE subjob_id=${subjob_id};")
    [[ -z ${row} ]] && continue # not tracked (e.g. the masterjob itself)

    IFS='|' read -r db_state n_retries <<< "${row}"
    [[ ${db_state} == "SUBMITTED" || ${db_state} == "RESUBMITTED" ]] || continue # exclude the ones that are above these stages

    if [[ ${state_in_grid} == "R" ]]; then
        # job is running, do nothing
        continue
    elif [[ ${state_in_grid} == "D" ]]; then
        # handle done jobs; DONE means "done at grid"
        db_exec "${DB}" "UPDATE subjobs SET state='DONE', updated_at=datetime('now') WHERE subjob_id=${subjob_id};"
    elif [[ ${state_in_grid} == *E ]]; then
        # handle jobs in error
        retry_or_give_up "${subjob_id}" "${n_retries}"
    else
        # handle jobs that have been WAITING (or in an unknown state) for long periods
        check_stale "${subjob_id}" "${n_retries}"
    fi
done

log_msg "${SCRIPT_NAME}" "resubmitter" "tick finished"
