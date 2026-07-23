#!/bin/bash

# `esd2vector/scripts/scrontab-gsi/downloader.sh`
# ===============================================
# Phase 3: download the output of DONE subjobs and verify the transfers (size + md5).
# Failed downloads are retried on later ticks, up to MAX_DL_RETRIES times, then marked DOWNLOAD_FAILED.
# Single-pass "tick" meant to be scheduled with scrontab.
#
# Ideal usage:
# 1. `source .env`
# 2. - `scrontab downloader_rd.scrontab`
#    - `scrontab downloader_mc.scrontab`

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
if [[ $# -ne 1 || ( $1 != "mc" && $1 != "rd" ) ]]; then echo "usage: ./downloader.sh <mc|rd>"; exit 1; fi
DATA_KIND=$1

# hardcoded params
SCRIPT_NAME=$(basename "$0")
MAX_DL_RETRIES=3

# don't overlap with a previous tick that's still running
scrontab_acquire_lock "downloader_${DATA_KIND}"

# check db
DB="${E2T_ROOT_DIR}/scrontab-gsi/${JALIEN_USERNAME}_${DATA_KIND}.db"
db_ensure_exists "${DB}"

# text file containing src,dst pairs
ALIEN_CP_INPUT_FILE="${E2T_USER_SCT_DIR}/to_download_${DATA_KIND}.txt"

# requeue entries stranded in DOWNLOADING by a previous crash
db_exec "${DB}" "UPDATE subjobs SET state='DONE', updated_at=datetime('now') WHERE state='DOWNLOADING';"

# -- 1. claim subjobs that are DONE -- #

log_msg "${SCRIPT_NAME}" "downloader" "checking for DONE subjobs"
mapfile -t claimed_rows < <(db_exec "${DB}" "
    UPDATE    subjobs
    SET       state='DOWNLOADING', updated_at=datetime('now')
    WHERE     state='DONE'
    RETURNING subjob_id, remote_dir, local_file;
")

if [[ ${#claimed_rows[@]} -eq 0 ]]; then
    log_msg "${SCRIPT_NAME}" "downloader" "nothing to download"
    exit 0
fi
log_msg "${SCRIPT_NAME}" "downloader" "there are ${#claimed_rows[@]} files to download"

# -- 2. create file that contains src,dst pairs -- #

: > "${ALIEN_CP_INPUT_FILE}" # create or empty it
for row in "${claimed_rows[@]}"; do
    IFS='|' read -r _ remote_dir local_file <<< "${row}"
    mkdir -p "$(dirname "${local_file}")"
    echo "alien://${remote_dir}/AnalysisResults.root file://${local_file}" >> "${ALIEN_CP_INPUT_FILE}"
done

# -- 3. batch download -- #

alien.py cp -retry 1 -input "${ALIEN_CP_INPUT_FILE}" ||
    log_msg "${SCRIPT_NAME}" "downloader" "WARNING: alien.py cp reported errors, verifying files anyway"

# -- 4. verify each transfer -- #

for row in "${claimed_rows[@]}"; do
    IFS='|' read -r subjob_id remote_dir local_file <<< "${row}"

    remote_md5=""
    if [[ -s ${local_file} ]]; then # check size is > 0 bytes
        remote_md5=$(alien.py md5sum "${remote_dir}/AnalysisResults.root" 2>/dev/null | awk '{print $1}' || true)
    fi

    if [[ -n ${remote_md5} ]] && echo "${remote_md5}  ${local_file}" | md5sum -c --status; then
        db_exec "${DB}" "UPDATE subjobs SET state='DOWNLOADED', updated_at=datetime('now') WHERE subjob_id=${subjob_id};"
    else
        rm -f "${local_file}"
        db_exec "${DB}" "
            UPDATE subjobs
            SET    n_dl_retries = n_dl_retries + 1,
                   state = CASE WHEN n_dl_retries + 1 >= ${MAX_DL_RETRIES} THEN 'DOWNLOAD_FAILED' ELSE 'DONE' END,
                   updated_at = datetime('now')
            WHERE  subjob_id=${subjob_id};
        "
        log_msg "${SCRIPT_NAME}" "downloader" "WARNING: verification failed for ${local_file} (subjob ${subjob_id})"
    fi
done

log_msg "${SCRIPT_NAME}" "downloader" "tick finished"
