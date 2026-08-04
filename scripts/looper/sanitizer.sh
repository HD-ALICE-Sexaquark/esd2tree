#!/bin/bash

# `esd2vector/scripts/looper/sanitizer.sh`
# ========================================
# Single-pass script that syncs jobs' states in GRID with jobs' state in DB.
# It will give another chance to the stale RESUBMITTED or GIVEN_UP jobs,
# by setting them to SUBMITTED(n_retries=0) all over again.
#
# Ideal usage: (requires .env)
# `./sanitizer.sh mc`
# `./sanitizer.sh rd`

set -euo pipefail
shopt -s nullglob

# environment
if [[ -z ${E2T_ROOT_DIR:-} ]]; then echo "error: missing env. var. E2T_ROOT_DIR"; exit 1; fi
source "${E2T_ROOT_DIR}/scripts/looper/looper_lib.sh"
looper_require_env LOOPER_DIR JALIEN_TOKEN_CERT JALIEN_TOKEN_KEY ALIROOT_RELEASE ALIPHYSICS_RELEASE GRID_ABORQUEZ_HOME_DIR

# command-line argument
if [[ $# -ne 1 || ( $1 != "mc" && $1 != "rd" ) ]]; then
    echo "usage: ./sanitizer.sh <mc|rd>"
    exit 1
fi
data_kind=$1

# >> generate remaining env. vars.
looper_init "${data_kind}"
looper_require_env GRID_USERNAME LOOPER_USER_DIR LOOPER_LOGS_DIR GRID_HOME_DIR

# hardcoded params
script_name=$(basename "$0" .sh)

# db state file
db_file="${LOOPER_DIR}/${GRID_USERNAME}_${data_kind}.db"
db_ensure_exists "${db_file}"

# user settings
cd "${LOOPER_USER_DIR}"

# === main === #

# query unique masterjob_ids that contain SUBMITTED or RESUBMITTED subjobs
mapfile -t active_masterjobs < <(db_exec "${db_file}" -noheader -list \
    "SELECT DISTINCT masterjob_id FROM grid_jobs WHERE state IN ('SUBMITTED', 'RESUBMITTED', 'GIVEN_UP');")
n_pending=${#active_masterjobs[@]}
if [[ ${n_pending} -eq 0 ]]; then
    log_msg "${script_name}" "sanitizer" "no pending jobs to poll"
    exit 1
fi
log_msg "${script_name}" "sanitizer" "polling ${n_pending} pending masterjobs"

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
        [[ ${state_in_db} == "SUBMITTED" || ${state_in_db} == "RESUBMITTED" || ${state_in_db} == "GIVEN_UP" ]] || continue

        if [[ ${state_in_grid} == "R" ]]; then
            # job is running, do nothing
            continue
        elif [[ ${state_in_grid} == "D" ]]; then
            # sync done jobs
            db_update_where "${db_file}" "state='DONE'" "subjob_id=${subjob_id}"
            log_msg "${script_name}" "sanitizer" "updated subjob_id=${subjob_id} to DONE in DB"
        else
            # give another chance to the rest (errors, stale, etc)
            db_update_where "${db_file}" "state='SUBMITTED',n_retries=0" "subjob_id=${subjob_id}"
            log_msg "${script_name}" "sanitizer" "updated subjob_id=${subjob_id} to SUBMITTED in DB"
        fi
    done
done

log_msg "${script_name}" "sanitizer" "done"
