#!/bin/bash

# `esd2vector/scripts/scrontab-gsi/scrontab_lib.sh`
# =================================================
# Shared functions for the grid scrontab pipeline (submitter.sh, downloader.sh, cleaner.sh).
# Source this file; do not execute directly.
# NOTE: under scrontab, each script sources ${SCT_ENV_FILE} (default ~/.e2t_env) before this file;
#       that env file must export ALICE_ROOT, ALICE_PHYSICS, E2T_ROOT_DIR, E2T_SCT_DIR
#       and put alien.py, aliroot and sqlite3 on PATH.

# -- environment -- #

scrontab_require_env() {
    local var_name
    for var_name in "$@"; do
        if [[ -z ${!var_name:-} ]]; then
            echo "FATAL: missing environment variable ${var_name}"
            exit 1
        fi
    done
}

# Check environment and define `JALIEN_USERNAME`, `E2T_USER_SCT_DIR`, `JALIEN_HOME`
# (`E2T_OUTPUT_DIR` defaults to `${E2T_ROOT_DIR}/output`)
scrontab_init() {
    export E2T_OUTPUT_DIR="${E2T_ROOT_DIR}/output"
    mkdir -p "${E2T_OUTPUT_DIR}"
    export JALIEN_USERNAME
    JALIEN_USERNAME=$(alien.py whoami)
    if [[ -z ${JALIEN_USERNAME} ]]; then
        echo "FATAL: could not determine grid username, check your alien token"
        exit 1
    fi
    export E2T_USER_SCT_DIR="${E2T_ROOT_DIR}/scrontab-gsi/${JALIEN_USERNAME}"
    mkdir -p "${E2T_USER_SCT_DIR}" "${E2T_ROOT_DIR}/scrontab-gsi/locks"
    export JALIEN_HOME="/alice/cern.ch/user/${JALIEN_USERNAME:0:1}/${JALIEN_USERNAME}"
}

# Take a per-user non-blocking lock; exits 0 if another tick of the same script still holds it.
# usage: scrontab_acquire_lock <name>
scrontab_acquire_lock() {
    local lock_file="${E2T_ROOT_DIR}/scrontab-gsi/locks/${JALIEN_USERNAME}_$1.lock"
    exec 9>"${lock_file}"
    if ! flock -n 9; then
        log_msg "scrontab_lib" "lock" "previous $1 tick still running, exiting"
        exit 0
    fi
}

# -- logging -- #

log_msg() {
    local script_name=$1; shift
    local context=$1; shift
    echo -e "${script_name}(${JALIEN_USERNAME:-?}) :: ${context} :: $(date '+%I:%M:%S %p @ %d-%b-%Y') :: $*"
}

# -- state operations -- #

# Single entry point for sqlite3: waits instead of failing when another scrontab process holds the DB.
# NOTE: output columns are pipe-separated (sqlite3 default); parse with `IFS='|' read -r ...`
db_exec() {
    local db_file=$1; shift
    sqlite3 -cmd ".timeout 15000" "${db_file}" "$@"
}

db_ensure_exists() {
    local db_file=$1
    db_exec "${db_file}" "
        CREATE TABLE IF NOT EXISTS subjobs (
            subjob_id      INTEGER PRIMARY KEY,
            run_number     INTEGER NOT NULL,
            sim_set        TEXT NOT NULL DEFAULT '',   -- mc: e.g. 'A1.8'; rd: '' or xml chunk tag, e.g. 'xml2'
            masterjob_id   INTEGER NOT NULL,
            outdir_number  TEXT NOT NULL,              -- basename of the remote output dir, e.g. '017'
            remote_dir     TEXT NOT NULL,
            local_file     TEXT NOT NULL,
            state          TEXT NOT NULL,              -- SUBMITTED, RESUBMITTED, DONE, DOWNLOADING, DOWNLOADED, CLEANED, GIVEN_UP, DOWNLOAD_FAILED
            n_retries      INTEGER NOT NULL DEFAULT 0, -- grid resubmissions
            n_dl_retries   INTEGER NOT NULL DEFAULT 0, -- download attempts
            updated_at     TEXT NOT NULL
        );
        CREATE INDEX IF NOT EXISTS idx_state ON subjobs(state);
        CREATE INDEX IF NOT EXISTS idx_rn    ON subjobs(run_number);
        CREATE TABLE IF NOT EXISTS killed_masterjobs (
            masterjob_id INTEGER PRIMARY KEY
        );
        PRAGMA journal_mode = WAL;
    "
}

db_print() {
    db_exec "$1" "SELECT * FROM subjobs;"
}

# -- quota checks -- #

check_jquota() {
    local max_running_time=${1:-8000000}
    local total_running
    total_running=$(alien.py jquota list "${JALIEN_USERNAME}" 2>/dev/null | grep "totalRunningTimeLast24h" | awk '{print $3}' || true)
    if [[ ! ${total_running} =~ ^[0-9]+$ ]]; then
        log_msg "scrontab_lib" "jquota" "WARNING: could not parse jquota output, assuming quota is fine"
        return 0
    fi
    [[ ${total_running} -le ${max_running_time} ]]
}

check_fquota() {
    local fquota_output
    fquota_output=$(alien.py fquota list "${JALIEN_USERNAME}" 2>/dev/null || true)
    local usage_pct files_pct
    usage_pct=$(echo "${fquota_output}" | grep "totalSize" | grep -oE '[0-9.]+%' | tr -d '%' || true)
    files_pct=$(echo "${fquota_output}" | grep "nbFiles" | grep -oE '[0-9.]+%' | tr -d '%' || true)
    if [[ -z ${usage_pct} || -z ${files_pct} ]]; then
        log_msg "scrontab_lib" "fquota" "WARNING: could not parse fquota output, assuming quota is fine"
        return 0
    fi
    awk -v s="${usage_pct}" -v f="${files_pct}" 'BEGIN { exit (s > 95 || f > 95) }'
}

# -- production resolution -- #

# usage: resolve_production_path <mc|rd> <run_number>
# Echoes the grid production path, or nothing if the run number is not in any known list.
# NOTE: globs also cover the *_EXTRA and *.hadron_pid list variants.
resolve_production_path() {
    local data_kind=$1
    local run_number=$2
    local lists_dir="${E2T_ROOT_DIR}/run_lists"
    local production_path=""
    if grep -qsw "${run_number}" "${lists_dir}"/LHC15o_pass2_rn*.txt; then
        if [[ ${data_kind} == "mc" ]]; then
            production_path="/alice/sim/2023/LHC23l1b3";
        else
            production_path="/alice/data/2015/LHC15o";
        fi
    elif grep -qsw "${run_number}" "${lists_dir}"/LHC18q_pass3_rn*.txt; then
        if [[ ${data_kind} == "mc" ]]; then
            production_path="/alice/sim/2023/LHC23l1a3";
        else
            production_path="/alice/data/2018/LHC18q";
        fi
    elif grep -qsw "${run_number}" "${lists_dir}"/LHC18r_pass3_rn*.txt; then
        if [[ ${data_kind} == "mc" ]]; then
            production_path="/alice/sim/2023/LHC23l1a3";
        else
            production_path="/alice/data/2018/LHC18r";
        fi
    elif [[ ${data_kind} == "mc" ]] && grep -qsw "${run_number}" "${lists_dir}"/LHC18qr_pass3_rn.txt; then
        # NOTE: as intended, LHC18qr*.txt contains all the run numbers that were generated for LHC23l1a3
        production_path="/alice/sim/2023/LHC23l1a3"
    fi
    echo "${production_path}"
}

# -- file staging -- #

# Copy into ${dest_dir} everything `RunTask.C` needs.
stage_analysis_files() {
    local dest_dir=$1
    mkdir -p "${dest_dir}"
    rm -rf "${dest_dir:?}"/*
    local file
    for file in task/RunTask.C \
                task/AliTaskEsd2Tree.h \
                task/AliTaskEsd2Tree.cxx \
                task/AliTaskEsd2Tree_LinkDef.h \
                task/AddTaskEsd2Tree.C \
                common/Schema_Events.hpp \
                common/POD_Track.hpp \
                common/POD_PreFoundLambda.hpp \
                common/POD_McParticle.hpp \
                common/POD_InjectedSexa.hpp \
                common/POD_Event.hpp \
                common/Math.hpp \
                common/Framework_TeeTree.hpp \
                common/Cuts_E2T.hpp \
                common/Constants.hpp \
                tidentity/macros/AddTask_marsland_TIdentityPID.C \
                tidentity/macros/AliAnalysisTaskTIdentityPID.cxx \
                tidentity/macros/AliAnalysisTaskTIdentityPID.h \
                tidentity/macros/Config_marsland_TIdentityPID.C \
                tidentity/macros/AddTaskFilteredTreeLocal.C; do
        cp -v "${E2T_ROOT_DIR}/${file}" "${dest_dir}/"
    done
}

# -- analysis command builder -- #

# usage: build_analysis_command <mode> <input_path> <production_name> <run_number> \
#                               <n_dirs> <limit_n_events> <include_tidentity> <grid_working_dir> <custom_xml>
# (signature must match `task/RunTask.C`)
build_analysis_command() {
    local opts="("
    opts+="\"$1\","     # Mode
    opts+="\"$2\","     # InputPath
    opts+="\"$3\","     # ProductionName
    opts+="$4,"         # RunNumber
    opts+="${5:-1},"    # Local_NDirs
    opts+="${6:-0},"    # Local_LimitToNEvents
    opts+="${7:-0},"    # include_tidentity
    opts+="\"${8:-}\"," # Grid_WorkingDir
    opts+="\"${9:-}\""  # Grid_CustomXML
    opts+=")"
    echo "aliroot -l -b -q RunTask.C${opts}"
}

# -- utilities -- #

# Minutes between two date strings; echoes -1 if either can't be parsed.
minutes_between() {
    local start_secs end_secs
    if ! start_secs=$(date -d "$1" +%s 2>/dev/null) || ! end_secs=$(date -d "$2" +%s 2>/dev/null); then
        echo "-1"
        return 0
    fi
    echo $(( (end_secs - start_secs) / 60 ))
}
