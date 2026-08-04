#!/bin/bash

# `esd2vector/scripts/looper/cleaner.sh`
# ======================================
# Phase 4: Delete remote dirs of DOWNLOADED entries.
# Safe to run at any time. The script loops forever.
#
# Ideal usage: (requires .env)
# `./cleaner.sh mc`
# `./cleaner.sh rd`

set -euo pipefail

# environment
if [[ -z ${E2T_ROOT_DIR:-} ]]; then echo "error: missing env. var. E2T_ROOT_DIR"; exit 1; fi
source "${E2T_ROOT_DIR}/scripts/looper/looper_lib.sh"
looper_require_env LOOPER_DIR JALIEN_TOKEN_CERT JALIEN_TOKEN_KEY ALIROOT_RELEASE ALIPHYSICS_RELEASE

# command-line argument
if [[ $# -ne 1 || ( $1 != "mc" && $1 != "rd" ) ]]; then echo "usage: ./cleaner.sh <mc|rd>"; exit 1; fi
data_kind=$1

# >> generate remaining env. vars.
looper_init "${data_kind}"
looper_require_env GRID_USERNAME LOOPER_USER_DIR LOOPER_LOGS_DIR GRID_HOME_DIR

# hardcoded params
script_name=$(basename "$0" .sh)
poll_interval=300 # 5 minutes

# check state file
db_file="${LOOPER_DIR}/${GRID_USERNAME}_${data_kind}.db"
db_ensure_exists "${db_file}"

# === main eternal loop === #

while true; do

    # query subjobs that are DOWNLOADED or DL_FAILED
    log_msg "${script_name}" "query" "checking for DOWNLOADED or DL_FAILED subjobs"
    mapfile -t downloaded_subjobs < <(db_get_prop_where "${db_file}" "subjob_id" "state IN ('DOWNLOADED','DL_FAILED')")
    n_jobs_finished=${#downloaded_subjobs[@]}
    if [[ ${n_jobs_finished} -eq 0 ]]; then
        log_msg "${script_name}" "query" "nothing to clean, polling back in ${poll_interval} seconds"
        sleep "${poll_interval}"
        continue
    fi
    log_msg "${script_name}" "query" "there are ${n_jobs_finished} jobs to clean"

    # collect subjob ids and remote dirs
    remote_dirs=()
    for subjob_id in "${downloaded_subjobs[@]}"; do
        remote_dir=$(db_get_prop_where "${db_file}" "remote_dir" "subjob_id=${subjob_id}")
        remote_dirs+=("alien://${remote_dir}")
    done
    log_msg "${script_name}" "cleaner" "removing ${#remote_dirs[@]} remote dirs"

    # remove all remote dirs in a single call
    alien.py rm -rf "${remote_dirs[@]}" || true

    # update state
    for subjob_id in "${downloaded_subjobs[@]}"; do
        db_update_where "${db_file}" "state='CLEANED'" "subjob_id=${subjob_id}"
    done

    log_msg "${script_name}" "looper" "polling back in ${poll_interval} seconds"
    sleep "${poll_interval}"
done
