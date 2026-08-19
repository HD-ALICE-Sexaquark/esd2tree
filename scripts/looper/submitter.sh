#!/bin/bash

# `esd2vector/scripts/looper/submitter.sh`
# ========================================
# Single-pass script that submits GRID jobs for every run number (or job tag) in an input text file,
# and adds them to the DB.

set -euo pipefail
shopt -s nullglob

# environment
if [[ -z ${E2T_ROOT_DIR:-} ]]; then echo "error: missing env. var. E2T_ROOT_DIR"; exit 1; fi
source "${E2T_ROOT_DIR}/scripts/looper/looper_lib.sh"
looper_require_env LOOPER_DIR JALIEN_TOKEN_CERT JALIEN_TOKEN_KEY ALIROOT_RELEASE ALIPHYSICS_RELEASE GRID_ABORQUEZ_HOME_DIR

# command-line argument
if [[ $# -ne 2 || ( $1 != "mc" && $1 != "rd" ) || ! -f $2 ]]; then
    echo "usage: ./submitter.sh <mc|rd> <file>"
    echo "       where the plain text <file> is filled in each line with:"
    echo "       - for mc: either <run number> or <sim set>_<run number>"
    echo "       - for rd: either <run number> or <run number>_<xml chunk>"
    echo "       it's important that the two formats don't mix within a same file,"
    echo "       the reading mode is detected upon reading the first line"
    exit 1
fi
data_kind=$1
run_numbers_list=$2

# >> generate remaining env. vars.
looper_init "${data_kind}"
looper_require_env GRID_USERNAME LOOPER_USER_DIR LOOPER_LOGS_DIR GRID_HOME_DIR

# hardcoded params
script_name=$(basename "$0" .sh)
# -- mc related
mc_signal_channels=("A" "D" "H")
mc_sexaquark_masses=("1.73" "1.8" "1.87" "1.94" "2.01")

# db state file
db_file="${LOOPER_DIR}/${GRID_USERNAME}_${data_kind}.db"
db_ensure_exists "${db_file}"

# user settings
stage_analysis_files "${E2T_ROOT_DIR}" "${LOOPER_USER_DIR}"
cd "${LOOPER_USER_DIR}"

# Submit a single masterjob and register its subjobs in the state db file.
# usage: submit_masterjob <data_kind> <grid_input_dir> <production_name> <run_number> \
#                         <tag> <grid_working_dir> <local_xml> <local_dir>
submit_masterjob() {
    local data_kind=$1 grid_input_dir=$2 production_name=$3 run_number=$4
    local tag=$5 grid_working_dir=$6 local_xml=$7 local_dir=$8

    # define job tag
    local job_tag="${run_number}_${tag}"
    if [[ ${data_kind} == "mc" ]]; then job_tag="${tag}_${run_number}"; fi

    # skip if already tracked
    local n_tracked
    n_tracked=$(db_count_where "${db_file}" "run_number=${run_number} AND tag='${tag}'")
    if [[ ${n_tracked} -gt 0 ]]; then
        log_msg "${script_name}" "${job_tag}" "job tag already tracked, skipping"
        return 0
    fi

    # check quotas
    if ! check_quotas "${script_name}" "${db_file}"; then
        log_msg "${script_name}" "${job_tag}" "fatal :: quota exceeded, exiting, try again later"
        exit 1
    fi

    # handle tidentity macros
    local include_tidentity=false # temporary, while i wait for ilya to clean his storage... u_u
    if [[ ${data_kind} == "mc" ]]; then include_tidentity=false; fi

    # submit jobs via aliroot
    local aliroot_command aliroot_logfile
    # -- prepare command
    # <mode> <input_path> <production_name> <run_number> <n_dirs> <limit_n_events> <include_tidentity> <grid_working_dir> <custom_xml>
    aliroot_command=$(build_analysis_command "grid" \
                                             "${grid_input_dir}" \
                                             "${production_name}" \
                                             "${run_number}" \
                                             0 \
                                             0 \
                                             ${include_tidentity} \
                                             "${grid_working_dir}" \
                                             "${local_xml}")
    log_msg "${script_name}" "${job_tag}" "executing: ${aliroot_command}"
    # -- prepare log file
    aliroot_logfile="${LOOPER_LOGS_DIR}/analysis/${job_tag}.log"
    log_msg "${script_name}" "${job_tag}" "log: ${aliroot_logfile}"
    # -- execute command
    ${aliroot_command} 2>&1 | tee "${aliroot_logfile}" || true

    # extract masterjob id
    # near the end of the log file, there should be a line with this pattern:
    # ```
    # THE JOB ID IS: 3671972601
    # ```
    local masterjob_id
    masterjob_id=$(grep -oP 'THE JOB ID IS:\s*\K\d+' "${aliroot_logfile}" | tail -n 1 || true) # only digits are output
    if [[ ! ${masterjob_id} =~ ^[0-9]+$ ]]; then
        log_msg "${script_name}" "${tag}" "submission failed, no masterjob_id found"
        return 0
    fi
    log_msg "${script_name}" "${job_tag}" "submitted masterjob with id=${masterjob_id}"

    # wait until the subjobs appear (hardcoded: one attempt per 30 secs, 10 attempts, 5 min total)
    # example output of `alien.py ps -m`:
    # ```
    #     aborquez 3525965811    W                         TaskEsd2Tree.sh
    #     aborquez 3525965812    D                         TaskEsd2Tree.sh
    # ```
    local subjobs_arr=()
    for (( attempt = 0; attempt < 10; attempt++ )); do
        sleep 30 # seconds
        mapfile -t subjobs_arr < <(alien.py ps -m "${masterjob_id}" 2>/dev/null | awk '{ print $2 }' || true)
        [[ ${#subjobs_arr[@]} -gt 0 ]] && break
    done
    if [[ ${#subjobs_arr[@]} -eq 0 ]]; then
        log_msg "${script_name}" "${job_tag}" "couldn't get subjobs, skipping"
        return 0
    fi

    # register subjobs in the db state file
    local subjob_id remote_dir outdir_number local_file
    for subjob_id in "${subjobs_arr[@]}"; do
        # `alien.py jobInfo -jdl <job_id>` prints job information as a json object
        remote_dir=$(alien.py jobInfo -jdl "${subjob_id}" | jq -r '.["OutputDir"]' || true)
        if [[ -z ${remote_dir} || ${remote_dir} == "null" ]]; then
            log_msg "${script_name}" "${job_tag}" "warning :: no OutputDir for subjob ${subjob_id}, not tracking it"
            continue
        fi
        # define local file suffix:
        # - for mc, keep the output dir number as is
        # - for data, count
        outdir_number=$(basename "${remote_dir}")
        if [[ ${data_kind} == "mc" ]]; then
            local_file="${local_dir}/AnalysisResults_${outdir_number}.root"
        else
            local_file="${local_dir}/AnalysisResults_$(printf "%02i" "${tag}").${outdir_number}.root"
        fi
        db_insert_row "${db_file}" \
                      "${subjob_id}" "${run_number}" "${tag}" "${masterjob_id}" "${remote_dir}" "${local_file}" "SUBMITTED" 0
    done
    log_msg "${script_name}" "${job_tag}" "registered ${#subjobs_arr[@]} subjobs of masterjob ${masterjob_id}"
}

# === main === #

# determine reading mode by extracting first line
read_tags_instead_of_rn=0
if [[ $(head -1 "${run_numbers_list}") == *_* ]]; then
    read_tags_instead_of_rn=1
    log_msg "${script_name}" "submitter/init" "reading mode set to job tags"
else
    log_msg "${script_name}" "submitter/init" "reading mode set to run numbers"
fi

# loop over run numbers
while IFS= read -r line_from_rn_list; do

    # determine run number (needed here before setting production number)
    if [[ ${read_tags_instead_of_rn} -eq 0 ]]; then
        # <run number> was written
        run_number=${line_from_rn_list}
    else
        if [[ ${data_kind} == "mc" ]]; then
            # <sim set>_<run number> was written
            sim_set=${line_from_rn_list%_*}
            run_number=${line_from_rn_list#*_}
        else
            # <run number>_<xml chunk> was written
            run_number=${line_from_rn_list%_*}
            xml_chunk=${line_from_rn_list#*_}
        fi
    fi

    # resolve production
    production_path=$(resolve_production_path "${data_kind}" "${run_number}")
    if [[ -z ${production_path} ]]; then
        log_msg "${script_name}" "${run_number}" "fatal :: could not resolve production"
        exit 1
    fi
    production_name=$(basename "${production_path}") # e.g. LHC23l1a3, LHC18q
    small_prod_name=${production_name#LHC}           # remove 'LHC', e.g. 23l1a3, 18q
    log_msg "${script_name}" "${run_number}" "production path: ${production_path} (small: ${small_prod_name})"

    if [[ ${data_kind} == "mc" ]]; then
        if [[ ${read_tags_instead_of_rn} -eq 0 ]]; then
            for signal_channel in "${mc_signal_channels[@]}"; do
                for sexaquark_mass in "${mc_sexaquark_masses[@]}"; do
                    sim_set="${signal_channel}${sexaquark_mass}"
                    # submit_masterjob <data_kind> <grid_input_dir> <production_name> <run_number> \
                    #                  <tag> <grid_working_dir> <local_xml> <local_dir>
                    submit_masterjob "mc" \
                                    "${production_path}/${sim_set}" \
                                    "${production_name}" \
                                    "${run_number}" \
                                    "${sim_set}" \
                                    "work/Esd2Vector/mc_${small_prod_name}" \
                                    "" \
                                    "${E2T_ROOT_DIR}/output/grid_mc_${small_prod_name}/${sim_set}/${run_number}"
                done # end of masses loop
            done # end of channels loop
        else
            # read_tags_instead_of_rn == 1
            # submit_masterjob <data_kind> <grid_input_dir> <production_name> <run_number> \
            #                  <tag> <grid_working_dir> <local_xml> <local_dir>
            submit_masterjob "mc" \
                             "${production_path}/${sim_set}" \
                             "${production_name}" \
                             "${run_number}" \
                             "${sim_set}" \
                             "work/Esd2Vector/mc_${small_prod_name}" \
                             "" \
                             "${E2T_ROOT_DIR}/output/grid_mc_${small_prod_name}/${sim_set}/${run_number}"
        fi
    else
        # data kind == "rd"
        if [[ ${read_tags_instead_of_rn} -eq 0 ]]; then
            # get xml files
            xml_files_arr=()
            local_xml_rn_dir="${E2T_ROOT_DIR}/xml/${production_name}/${run_number}" # relative to E2T_ROOT_DIR
            if [[ -d ${local_xml_rn_dir} ]]; then
                mapfile -t xml_files_arr < <(find "${local_xml_rn_dir}" -name "*.xml" | sort -V)
            else
                log_msg "${script_name}" "${run_number}" "warning :: couldn't find xml dir for ${production_name}/${run_number}, skipping run number"
                continue
            fi
            # submit jobs per each xml file
            for index in "${!xml_files_arr[@]}"; do
                xml_chunk=$(( index + 1 ))
                local_xml_file="${xml_files_arr[${index}]}"
                remote_xml_file="${local_xml_file#*/xml/}" # remove everything behind and including '/xml/'
                remote_xml_file="${GRID_ABORQUEZ_HOME_DIR}/xml/${production_name}/${run_number}/$(basename "${local_xml_file}")"
                # submit_masterjob <data_kind> <grid_input_dir> <production_name> <run_number> \
                #                  <tag> <grid_working_dir> <local_xml> <local_dir>
                submit_masterjob "rd" \
                                 "${production_path}" \
                                 "${production_name}" \
                                 "${run_number}" \
                                 "${xml_chunk}" \
                                 "work/Esd2Vector/rd_${small_prod_name}" \
                                 "${remote_xml_file}" \
                                 "${E2T_ROOT_DIR}/output/grid_rd_${small_prod_name}/${run_number}"
            done # end of loop over xml files
        else
            # read_tags_instead_of_rn == 1
            remote_xml_file="${GRID_ABORQUEZ_HOME_DIR}/xml/${production_name}/${run_number}/${line_from_rn_list}.xml"
            # submit_masterjob <data_kind> <grid_input_dir> <production_name> <run_number> \
            #                  <tag> <grid_working_dir> <local_xml> <local_dir>
            submit_masterjob "rd" \
                             "${production_path}" \
                             "${production_name}" \
                             "${run_number}" \
                             "${xml_chunk}" \
                             "work/Esd2Vector/rd_${small_prod_name}" \
                             "${remote_xml_file}" \
                             "${E2T_ROOT_DIR}/output/grid_rd_${small_prod_name}/${run_number}"
        fi
    fi
done < "${run_numbers_list}" # end of loop over run numbers

log_msg "${script_name}" "submitter/done" "finished submitting all run numbers from ${run_numbers_list}"
log_msg "${script_name}" "submitter/done" "continue with resubmitter.sh"
