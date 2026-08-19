#!/bin/bash

# `esd2vector/scripts/looper/resubmitter.sh`
# ==========================================
# Iterative script that monitors tracked jobs:
# - DONE jobs are marked as such.
# - Jobs in ERROR are resubmitted, up to `max_submit_retries` times, then given up on.
# - Jobs WAITING (or in an unknown state) for more than `max_wait_minutes` get the same treatment.
#
# Ideal usage: (requires .env)
# `./resubmitter.sh mc`
# `./resubmitter.sh rd`

set -euo pipefail
shopt -s nullglob

# environment
if [[ -z ${E2T_ROOT_DIR:-} ]]; then echo "error: missing env. var. E2T_ROOT_DIR"; exit 1; fi
source "${E2T_ROOT_DIR}/scripts/looper/looper_lib.sh"
looper_require_env LOOPER_DIR JALIEN_TOKEN_CERT JALIEN_TOKEN_KEY ALIROOT_RELEASE ALIPHYSICS_RELEASE GRID_ABORQUEZ_HOME_DIR

# command-line argument
if [[ $# -ne 1 || ( $1 != "mc" && $1 != "rd" ) ]]; then
    echo "usage: ./resubmitter.sh <mc|rd> <file>"
    exit 1
fi
data_kind=$1

# >> generate remaining env. vars.
looper_init "${data_kind}"
looper_require_env GRID_USERNAME LOOPER_USER_DIR LOOPER_LOGS_DIR GRID_HOME_DIR

# hardcoded params
script_name=$(basename "$0" .sh)
poll_interval=900 # 15 minutes
max_submit_retries=2
max_wait_minutes=120 # tolerance before resubmitting a job stuck in WAITING/unknown

# db state file
db_file="${LOOPER_DIR}/${GRID_USERNAME}_${data_kind}.db"
db_ensure_exists "${db_file}"

# user settings
stage_analysis_files "${E2T_ROOT_DIR}" "${LOOPER_USER_DIR}"
cd "${LOOPER_USER_DIR}"

retry_or_give_up() {
    local subjob_id=$1 n_retries=$2
    if [[ ${n_retries} -lt ${max_submit_retries} ]]; then
        alien.py resubmit "${subjob_id}" || true
        db_update_where "${db_file}" "state='RESUBMITTED'" "subjob_id=${subjob_id}"
        db_increase_val_where "${db_file}" "n_retries" "subjob_id=${subjob_id}" 1
        log_msg "${script_name}" "retry_or_give_up" "resubmitted subjob ${subjob_id}"
    else
        db_update_where "${db_file}" "state='GIVEN_UP'" "subjob_id=${subjob_id}"
        log_msg "${script_name}" "retry_or_give_up" "gave up on subjob ${subjob_id}"
    fi
}

# === main eternal loop === #

while true; do

    log_msg "${script_name}" "resubmitter/init" "starting new poll"

    # === monitoring pass: update job states + resubmit each failing/stale job === #

    # query unique masterjob_ids that contain SUBMITTED or RESUBMITTED subjobs
    mapfile -t active_masterjobs < <(db_exec "${db_file}" -noheader -list \
        "SELECT DISTINCT masterjob_id FROM grid_jobs WHERE state IN ('SUBMITTED', 'RESUBMITTED');")
    n_pending=${#active_masterjobs[@]}
    if [[ ${n_pending} -eq 0 ]]; then
        log_msg "${script_name}" "resubmitter" "no pending jobs to poll, polling back in $((poll_interval/60)) minutes"
        sleep ${poll_interval}
        continue
    fi
    log_msg "${script_name}" "resubmitter" "polling ${n_pending} pending jobs"

    # loop over masterjobs
    # optimization: grid handshake per masterjob instead of per subjob
    for masterjob_id in "${active_masterjobs[@]}"; do
        # example output of `alien.py ps`:
        # ```
        #     aborquez 3525965811    W                      TaskEsd2Tree.sh
        #     aborquez 3525965812    D                      TaskEsd2Tree.sh
        #     aborquez 3525965818   EE                      TaskEsd2Tree.sh
        #     aborquez 3525965820  EIB                      TaskEsd2Tree.sh
        # ```
        # loop over subjobs that belong to this masterjob
        mapfile -t ps_lines < <(alien.py ps -m "${masterjob_id}" 2>/dev/null || true)
        for line in "${ps_lines[@]}"; do
            read -r _ subjob_id state_in_grid _ <<< "${line}"

            # exclude the ones that are above these stages
            state_in_db=$(db_get_prop_where "${db_file}" "state" "subjob_id=${subjob_id}")
            [[ ${state_in_db} == "SUBMITTED" || ${state_in_db} == "RESUBMITTED" ]] || continue
            n_retries=$(db_get_prop_where "${db_file}" "n_retries" "subjob_id=${subjob_id}")

            if [[ ${state_in_grid} == "R" ]]; then
                # job is running, do nothing
                continue
            elif [[ ${state_in_grid} == "D" ]]; then
                # handle jobs done at grid
                db_update_where "${db_file}" "state='DONE'" "subjob_id=${subjob_id}"
                continue
            fi

            # from here on, the job either failed or may be stale, so a resubmission could follow

            # check quotas; specifically, if fquota allows 1 more subjob
            if ! check_quotas "${script_name}" "${db_file}" 1; then
                log_msg "${script_name}" "check_quotas" "warning :: skipping subjob ${subjob_id}"
                continue
            fi

            if [[ ${state_in_grid} == E* ]]; then
                # handle jobs in error
                retry_or_give_up "${subjob_id}" "${n_retries}"
            else
                # handle jobs that have been WAITING (or in an unknown state) for long periods
                # - assume stale if no trace
                # - assume stale if passed a certain threshold
                # example output of `alien.py ps -trace <job_id>`:
                # ```
                # 2026-02-16 20:01:32+0100 [state     ]: Job state transition to WAITING
                # 2026-02-16 20:01:32+0100 [trace     ]: Job inserted by aliendb9.cern.ch [Masterjob is 3525953703]
                # ```
                # expected date format: 2026-07-26 07:33:10+0200
                time_last_trace=$(alien.py ps -trace "${subjob_id}" | tail -1 | awk '{ printf "%s %s", $1, $2 }' || true)
                if [[ -z ${time_last_trace} ]]; then
                    retry_or_give_up "${subjob_id}" "${n_retries}"
                else
                    minutes_passed=$(minutes_between "${time_last_trace}" "$(date)")
                    if [[ ${minutes_passed} -ge 0 && ${minutes_passed} -gt ${max_wait_minutes} ]]; then
                        retry_or_give_up "${subjob_id}" "${n_retries}"
                    fi
                fi
            fi

        done
    done

    log_msg "${script_name}" "resubmitter/done" "polling back in $((poll_interval/60)) minutes"
    sleep ${poll_interval}
done
