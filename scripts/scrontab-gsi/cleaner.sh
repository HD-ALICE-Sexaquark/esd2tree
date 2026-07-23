#!/bin/bash

# `esd2vector/scripts/scrontab-gsi/cleaner.sh`
# ============================================
# Phase 4: delete remote dirs of DOWNLOADED entries.
# Safe to run at any time; single-pass "tick" meant to be scheduled with scrontab.
#
# Ideal usage:
# 1. `source .env`
# 2. - `scrontab cleaner_rd.scrontab`
#    - `scrontab cleaner_mc.scrontab`

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
if [[ $# -ne 1 || ( $1 != "mc" && $1 != "rd" ) ]]; then echo "usage: ./cleaner.sh <mc|rd>"; exit 1; fi
DATA_KIND=$1

# hardcoded params
SCRIPT_NAME=$(basename "$0")

# don't overlap with a previous tick that's still running
scrontab_acquire_lock "cleaner_${DATA_KIND}"

# check db
DB="${E2T_ROOT_DIR}/scrontab-gsi/${JALIEN_USERNAME}_${DATA_KIND}.db"
db_ensure_exists "${DB}"

# -- 1. list subjobs that are DOWNLOADED -- #

log_msg "${SCRIPT_NAME}" "cleaner" "checking for DOWNLOADED entries"
mapfile -t downloaded_rows < <(db_exec "${DB}" "SELECT subjob_id, remote_dir FROM subjobs WHERE state='DOWNLOADED';")

if [[ ${#downloaded_rows[@]} -gt 0 ]]; then

    # -- 2. collect subjob ids and remote dirs -- #

    subjob_ids=()
    remote_dirs=()
    for row in "${downloaded_rows[@]}"; do
        IFS='|' read -r subjob_id remote_dir <<< "${row}"
        subjob_ids+=("${subjob_id}")
        remote_dirs+=("alien://${remote_dir}")
    done
    mapfile -t remote_dirs < <(printf '%s\n' "${remote_dirs[@]}" | sort -u)

    # -- 3. remove all remote dirs in a single call -- #

    log_msg "${SCRIPT_NAME}" "cleaner" "removing ${#remote_dirs[@]} remote dirs"
    alien.py rm -rf "${remote_dirs[@]}" 2>/dev/null ||
        log_msg "${SCRIPT_NAME}" "cleaner" "WARNING: remote rm failed for some dirs (may already be gone)"

    # -- 4. update state to CLEANED -- #

    db_exec "${DB}" "
        UPDATE subjobs
        SET    state='CLEANED', updated_at=datetime('now')
        WHERE  subjob_id IN ($(IFS=,; echo "${subjob_ids[*]}"));
    "
fi

log_msg "${SCRIPT_NAME}" "cleaner" "tick finished"
