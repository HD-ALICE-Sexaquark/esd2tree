#!/bin/bash

# `esd2vector/scripts/looper/downloader.sh`
# =========================================
# Phase 3: Download the output of DONE subjobs and verify the transfers (size + md5).
#          If succesful, mark them as DOWNLOADED.
#          Failed downloads are retried in-call.
#
# Ideal usage: (requires .env)
# `./downloader.sh mc`
# `./downloader.sh rd`

set -euo pipefail

# environment
if [[ -z ${E2T_ROOT_DIR:-} ]]; then echo "error: missing env. var. E2T_ROOT_DIR"; exit 1; fi
source "${E2T_ROOT_DIR}/scripts/looper/looper_lib.sh"
looper_require_env LOOPER_DIR JALIEN_TOKEN_CERT JALIEN_TOKEN_KEY ALIROOT_RELEASE ALIPHYSICS_RELEASE

# command-line argument
if [[ $# -ne 1 || ( $1 != "mc" && $1 != "rd" ) ]]; then echo "usage: ./downloader.sh <mc|rd>"; exit 1; fi
data_kind=$1

# >> generate remaining env. vars.
looper_init "${data_kind}"
looper_require_env GRID_USERNAME LOOPER_USER_DIR LOOPER_LOGS_DIR GRID_HOME_DIR

# hardcoded params
script_name=$(basename "$0" .sh)
poll_interval=600 # 10 minutes
max_dl_retries=3

# check state file
db_file="${LOOPER_DIR}/${GRID_USERNAME}_${data_kind}.db"
db_ensure_exists "${db_file}"

# === main eternal loop === #

log_msg "${script_name}" "init" "starting on ${db_file}, polling every ${poll_interval} seconds"

while true; do

    # query subjobs that are DONE
    log_msg "${script_name}" "query" "checking for DONE subjobs"
    mapfile -t rows < <(db_get_prop_where "${db_file}" "subjob_id,remote_dir,local_file" "state='DONE'")
    n_jobs2download=${#rows[@]}
    if [[ ${n_jobs2download} -eq 0 ]]; then
        log_msg "${script_name}" "query" "nothing to download, polling back in ${poll_interval} seconds"
        sleep "${poll_interval}"
        continue
    fi
    log_msg "${script_name}" "query" "there are ${n_jobs2download} files to download"

    for row in "${rows[@]}"; do
        IFS='|' read -r subjob_id remote_dir local_file <<< "${row}"
        mkdir -p "$(dirname "${local_file}")"

        # start download
        db_update_where "${db_file}" "state='DOWNLOADING'" "subjob_id=${subjob_id}"
        alien.py cp -retry ${max_dl_retries} "alien://${remote_dir}/AnalysisResults.root" "file://${local_file}" ||
            log_msg "${script_name}" "download" "warning :: alien.py cp reported errors, verifying files anyway"

        # verify each transfer
        remote_md5=""
        if [[ -s ${local_file} ]]; then # check size is > 0 bytes
            remote_md5=$(alien.py md5sum "alien://${remote_dir}/AnalysisResults.root" | awk '{print $1}' || true)
        fi

        if [[ -n ${remote_md5} ]] && echo "${remote_md5}  ${local_file}" | md5sum -c --status; then
            db_update_where "${db_file}" "state='DOWNLOADED'" "subjob_id=${subjob_id}"
            continue
        fi

        rm -f "${local_file}"
        log_msg "${script_name}" "verify" "warning :: download failed for ${local_file} (subjob=${subjob_id}), moving on"
        db_update_where "${db_file}" "state='DL_FAILED'" "subjob_id=${subjob_id}"
    done

    log_msg "${script_name}" "done" "polling back in ${poll_interval} seconds"
    sleep "${poll_interval}"
done
