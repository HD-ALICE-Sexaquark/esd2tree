#!/bin/bash

# `esd2vector/scripts/looper/looper_lib.sh`
# =========================================
# Shared functions for the grid looper pipeline (`submitter.sh`, `downloader.sh`, `cleaner.sh`).
# Source this file; do not execute directly.
# The pipeline state lives in a sqlite3 db file, one row per subjob.

# === environment === #

looper_require_env() {
    local var_name
    for var_name in "$@"; do
        if [[ -z ${!var_name:-} ]]; then
            echo "FATAL: missing environment variable ${var_name}"
            exit 1
        fi
    done
}

# Define `GRID_USERNAME`, `LOOPER_USER_DIR`, `LOOPER_LOGS_DIR`, `GRID_HOME_DIR`
looper_init() {
    local data_kind=$1
    export GRID_USERNAME
    GRID_USERNAME=$(alien.py whoami || true)
    if [[ -z ${GRID_USERNAME} ]]; then
        echo "FATAL: could not determine grid username, check your alien token"
        exit 1
    fi

    export LOOPER_USER_DIR="${LOOPER_DIR}/${GRID_USERNAME}.${data_kind}"
    export LOOPER_LOGS_DIR="${LOOPER_USER_DIR}/logs"
    mkdir -p "${LOOPER_USER_DIR}" "${LOOPER_LOGS_DIR}/analysis"

    export GRID_HOME_DIR="/alice/cern.ch/user/${GRID_USERNAME:0:1}/${GRID_USERNAME}"
}

# === logging === #

now_cool() {
    date '+%I:%M:%S %p @ %d-%b-%Y'
}

log_msg() {
    local script_name=$1; shift
    local context=$1; shift
    echo -e "${script_name}(${GRID_USERNAME:-?}) :: ${context} :: $(now_cool) :: $*" | tee -a "${LOOPER_LOGS_DIR}/${script_name}.log" >&2
}

# === helpers === #

# Minutes between two date strings; echoes -1 if either can't be parsed.
# usage: minutes_between <start> <end>
minutes_between() {
    local start_secs end_secs
    if ! start_secs=$(date -d "$1" +%s 2>/dev/null) || ! end_secs=$(date -d "$2" +%s 2>/dev/null); then
        echo "-1"
        return 0
    fi
    echo $(( (end_secs - start_secs) / 60 ))
}

# usage:
# to_kb "512 MB"      # = 524288
# echo "2 GB" | to_kb # = 2097152
# to_kb "1.5 GB"      # = 1572864
to_kb() {
    local input="${1:-$(cat)}"
    local value unit
    read -r value unit <<< "$input"
    unit="${unit%[Bb]}" # "MB" -> "M", "KB" -> "K", "GB" -> "G", "B" -> ""
    numfmt --from=iec --to-unit=1024 "${value}${unit}"
}

# === db state file operations === #

db_exec() {
    local db_file=$1; shift
    sqlite3 -cmd ".timeout 30000" "${db_file}" "$@"
}

# The state file is an sqlite3 db file, one row per subjob:
#     $1  subjob_id
#     $2  run_number
#     $3  tag
#     $4  masterjob_id
#     $5  remote_dir
#     $6  local_file
#     $7  state          SUBMITTED, RESUBMITTED, DONE, DOWNLOADING, DOWNLOADED, CLEANED, GIVEN_UP, DL_FAILED
#     $8  n_retries      grid resubmissions
#     $9  updated_at
db_ensure_exists() {
    local db_file=$1
    db_exec "${db_file}" "
        CREATE TABLE IF NOT EXISTS grid_jobs (
            subjob_id      INTEGER PRIMARY KEY,
            run_number     INTEGER NOT NULL,
            tag            TEXT NOT NULL DEFAULT '',   -- for mc: sim_set e.g. 'A1.8'; for rd: xml chunk e.g. '2'
            masterjob_id   INTEGER NOT NULL,
            remote_dir     TEXT NOT NULL,
            local_file     TEXT NOT NULL,
            state          TEXT NOT NULL,              -- SUBMITTED, RESUBMITTED, DONE, DOWNLOADING, DOWNLOADED, CLEANED, GIVEN_UP, DL_FAILED
            n_retries      INTEGER NOT NULL DEFAULT 0, -- grid resubmissions
            updated_at     TEXT NOT NULL
        );
        PRAGMA journal_mode = WAL;
    "
}

db_print() {
    local db_file=$1
    db_exec "${db_file}" "SELECT * FROM grid_jobs;"
}

db_insert_row() {
    local db_file=$1
    local subjob_id=$2
    local run_number=$3
    local tag=$4
    local masterjob_id=$5
    local remote_dir=$6
    local local_file=$7
    local state=$8
    local n_sub_retries=${9:-0}
    db_exec "${db_file}" "
        INSERT INTO grid_jobs VALUES(${subjob_id}, ${run_number}, '${tag}', ${masterjob_id}, '${remote_dir}', '${local_file}', '${state}', ${n_sub_retries}, datetime('now'));
    "
}

db_count_where() {
    local db_file=$1
    local where=$2
    db_exec "${db_file}" "
        SELECT COUNT(*) FROM grid_jobs WHERE ${where};
    "
}

db_update_where() {
    local db_file=$1
    local update_what=$2
    local where=$3
    db_exec "${db_file}" "
        UPDATE grid_jobs SET ${update_what}, updated_at=datetime('now') WHERE ${where};
    "
}

db_increase_val_where() {
    local db_file=$1
    local num_property=$2
    local where=$3
    local by_how_much=${4:-1}
    db_exec "${db_file}" "
        UPDATE grid_jobs SET ${num_property}=${num_property}+${by_how_much}, updated_at=datetime('now') WHERE ${where};
    "
}

db_get_prop_where() {
    local db_file=$1
    local properties=$2
    local where=$3
    db_exec "${db_file}" "
        SELECT ${properties} FROM grid_jobs WHERE ${where};
    "
}

db_get_unique_prop_where() {
    local db_file=$1
    local properties=$2
    local where=$3
    db_exec "${db_file}" "
        SELECT DISTINCT ${properties} FROM grid_jobs WHERE ${where};
    "
}

# === quota checks === #

# hardcoded settings
avg_subjob_size_kb=512000     # average AnalysisResults.root size per subjob
avg_subjobs_per_masterjob=150
fquota_margin_pct=10          # safety headroom in %
jquota_margin_pct=15          # safety headroom in %

check_quotas() {
    local script_name=$1
    local db_file=$2
    local n_new_subjobs=${3:-${avg_subjobs_per_masterjob}}
    # return 0 if both quotas are within limits; non-zero otherwise
    check_jquota "${script_name}" && check_fquota "${script_name}" "${db_file}" "${n_new_subjobs}"
}

check_jquota() {
    local script_name=$1
    local jquota_output
    jquota_output=$(alien.py jquota list "${GRID_USERNAME}" || true)
    local max_running_time max_cpu_cost limit_running_time limit_cpu_cost running_time cpu_cost
    max_running_time=$(awk '/maxTotalRunningTime/ { print $3 }' <<< "${jquota_output}")
    max_cpu_cost=$(awk '/maxTotalCpuCost/ { print $3 }' <<< "${jquota_output}")
    limit_running_time=$((max_running_time * (100 - jquota_margin_pct) / 100))
    limit_cpu_cost=$((max_cpu_cost * (100 - jquota_margin_pct) / 100))
    running_time=$(awk '/totalRunningTimeLast24h/ { print $3 }' <<< "${jquota_output}")
    cpu_cost=$(awk '/totalCpuCostLast24h/ { print $3 }' <<< "${jquota_output}")
    if [[ ! ${max_running_time} =~ ^[0-9]+$ || ! ${max_cpu_cost} =~ ^[0-9]+$ ||
          ! ${running_time} =~ ^[0-9]+$ || ! ${cpu_cost} =~ ^[0-9]+$ ]]; then
        log_msg "${script_name}" "check_jquota" "warning :: could not parse jquota output, assuming quota is fine"
        return 0
    fi
    # return 0 if running time and cpu cost are within limits
    [[ ${running_time} -le ${limit_running_time} && ${cpu_cost} -le ${limit_cpu_cost} ]]
}

# echoes the value of an exactly-matching key
# usage: fquota_field <fquota_output> <key>
fquota_field() {
    # output of `alien.py fquota list <grid username>`:
    # FQuota: user: aborquez
    # totalSize               : 484.3 GB (23.65% of max)
    # maxTotalSize            : 2 TB
    # tmpIncreasedTotalSize   : 0 B
    # nbFiles                 : 2838 (0.946% of max)
    # maxNbFiles              : 300000
    # tmpIncreasedNbFiles     : 0
    awk -v key="$2" -F' *: *' '$1 ~ "^[[:space:]]*"key"[[:space:]]*$" { print $2; exit }' <<< "$1"
}

check_fquota() {
    local script_name=$1
    local db_file=$2
    local n_new_subjobs=${3:-${avg_subjobs_per_masterjob}}
    # get fquota for current user, output example shown above in `fquota_field`
    local fquota_output
    fquota_output=$(alien.py fquota list "${GRID_USERNAME}" || true)
    # define vars
    local used max limit pending reserved requested projected
    local nb_files max_files limit_files projected_files
    used=$(fquota_field "${fquota_output}" totalSize | awk '{print $1,$2}' | to_kb)
    max=$(fquota_field "${fquota_output}" maxTotalSize | to_kb)
    nb_files=$(fquota_field "${fquota_output}" nbFiles | awk '{print $1}')
    max_files=$(fquota_field "${fquota_output}" maxNbFiles)
    if [[ -z ${used} || -z ${max} || -z ${nb_files} || -z ${max_files} ||
          ! ${used} =~ ^[0-9]+$ || ! ${max} =~ ^[0-9]+$ || ! ${nb_files} =~ ^[0-9]+$ || ! ${max_files} =~ ^[0-9]+$ ]]; then
        log_msg "${script_name}" "check_fquota" "warning :: could not parse fquota output, assuming usage is outside limits"
        return 1
    fi
    pending=$(db_count_where "${db_file}" "state IN ('SUBMITTED','RESUBMITTED')") # n pending subjobs, whose storage hasn't been accounted yet in fquota
    # estimate storage
    limit=$((max * (100 - fquota_margin_pct) / 100))
    reserved=$((pending * avg_subjob_size_kb))
    requested=$((n_new_subjobs * avg_subjob_size_kb))
    projected=$((used + reserved + requested))
    # estimate n files
    limit_files=$((max_files * (100 - fquota_margin_pct) / 100))
    projected_files=$((nb_files + pending + n_new_subjobs))
    # return 0 if projected storage is less than limit
    [[ ${projected} -le ${limit} && ${projected_files} -le ${limit_files} ]]
}

# === production resolution === #

# usage: resolve_production_path <mc|rd> <run_number>
# Echoes the grid production path, or nothing if the run number is not in any known list.
# For MC, it doesn't include the simulation sets, aka, the reaction+mass pair.
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
    fi
    echo "${production_path}"
}

# === file staging === #

# Copy into ${dest_dir} everything `RunTask.C` needs.
stage_analysis_files() {
    local src_dir=$1 dest_dir=$2
    mkdir -p "${dest_dir}"
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
        cp "${src_dir}/${file}" "${dest_dir}/"
    done
}

# === analysis command builder === #

# usage: build_analysis_command <mode> <input_path> <production_name> <run_number> \
#                               <n_dirs> <limit_n_events> <include_tidentity> <grid_working_dir> <custom_xml>
build_analysis_command() {
    local opts="("
    opts+="\"$1\","   # Mode
    opts+="\"$2\","   # InputPath
    opts+="\"$3\","   # ProductionName
    opts+="$4,"       # RunNumber
    opts+="${5},"     # Local_NDirs
    opts+="${6},"     # Local_LimitToNEvents
    opts+="${7},"     # include_tidentity
    opts+="\"${8}\"," # Grid_WorkingDir
    opts+="\"${9}\""  # Grid_CustomXML
    opts+=")"
    echo "aliroot -l -b -q RunTask.C${opts}"
}
