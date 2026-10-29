#!/bin/bash

# `esd2tree/scripts/exec.sh`
# ==========================
# Run `RunTask.C` interactively over one run number, in "local" mode only (the GRID is `scripts/looper/`'s), with the
# options of a config file, e.g. one of `config/`. It runs in, and writes into:
#   E2T_ROOT_DIR / task / attempts / <config name, without .cfg>_<run number> /
# NOTE: for the dedicated MC, as copied from GSI by `farm-pi/fetch_sims.sh` (<run number>_<dir number>.tar), the first
#       LOCAL_N_DIRS tars of the run number are unpacked first, as `farm-pi/slurm_exec.sh` does, into a temporary dir
#       that is removed at exit

set -euo pipefail
shopt -s nullglob

# environment
if [[ -z ${E2T_ROOT_DIR:-} ]]; then echo "error: missing env. var. E2T_ROOT_DIR" ; exit 1; fi

# command-line arguments
if [[ $# -ne 1 ]]; then echo "usage: ./exec.sh <config_file>"; exit 1; fi
config_file=$1

source "${config_file}"
echo "exec.sh :: from config_file : ${config_file}"
echo "exec.sh ::   INPUT_PATH           = \"${INPUT_PATH}\""
echo "exec.sh ::   PRODUCTION_NAME      = \"${PRODUCTION_NAME}\""
echo "exec.sh ::   RUN_NUMBER           = ${RUN_NUMBER}"
echo "exec.sh ::   LOCAL_N_DIRS         = ${LOCAL_N_DIRS}"
echo "exec.sh ::   LOCAL_LIMIT_N_EVENTS = ${LOCAL_LIMIT_N_EVENTS}"

current_dir=${PWD}

# unpack the tars, if any, into the layout `RunTask.C` expects: <input path> / <run number> / <dir number> / <files>.root
# NOTE: `RunTask.C` takes the variant from the last directory of the input path
input_path=${INPUT_PATH}
tar_files=("${INPUT_PATH}/${RUN_NUMBER}/${RUN_NUMBER}"_*.tar)
if (( ${#tar_files[@]} > 0 )); then
    if (( LOCAL_N_DIRS > 0 )); then tar_files=("${tar_files[@]:0:LOCAL_N_DIRS}"); fi
    work_dir=$(mktemp -d "${TMPDIR:-/tmp}/e2t_exec.XXXXXX")
    trap 'rm -rf "${work_dir}"' EXIT
    input_path=${work_dir}/$(basename "${INPUT_PATH}")
    for tar_file in "${tar_files[@]}"; do
        dir_number=$(basename "${tar_file}" .tar)
        dir_number=${dir_number##*_}
        mkdir -p "${input_path}/${RUN_NUMBER}/${dir_number}"
        tar -xf "${tar_file}" -C "${input_path}/${RUN_NUMBER}/${dir_number}"
    done
    echo "exec.sh ::   >> unpacked ${#tar_files[@]} tars into ${input_path}"
fi

attempt_dir=${E2T_ROOT_DIR}/task/attempts/$(basename "${config_file}" .cfg)_${RUN_NUMBER}
mkdir -p "${attempt_dir}"
echo "exec.sh ::   >> ATTEMPT_DIR       = ${attempt_dir}"

cd "${attempt_dir}"

# copy task files
cp "${E2T_ROOT_DIR}/task/RunTask.C" .
cp "${E2T_ROOT_DIR}/task/AliTaskEsd2Tree.h" .
cp "${E2T_ROOT_DIR}/task/AliTaskEsd2Tree.cxx" .
cp "${E2T_ROOT_DIR}/task/AliTaskEsd2Tree_LinkDef.h" .
cp "${E2T_ROOT_DIR}/task/AddTaskEsd2Tree.C" .
cp "${E2T_ROOT_DIR}/common/Constants.hpp" .
cp "${E2T_ROOT_DIR}/common/Cuts_E2T.hpp" .
cp "${E2T_ROOT_DIR}/common/Framework_TeeTree.hpp" .
cp "${E2T_ROOT_DIR}/common/Math.hpp" .
cp "${E2T_ROOT_DIR}/common/POD_Event.hpp" .
cp "${E2T_ROOT_DIR}/common/POD_McParticle.hpp" .
cp "${E2T_ROOT_DIR}/common/POD_PreFoundLambda.hpp" .
cp "${E2T_ROOT_DIR}/common/POD_Track.hpp" .
cp "${E2T_ROOT_DIR}/common/Schema_Events.hpp" .

analysis_options="\"local\","
analysis_options+="\"${input_path}\","
analysis_options+="\"${PRODUCTION_NAME}\","
analysis_options+="${RUN_NUMBER},"
analysis_options+="${LOCAL_N_DIRS},"
analysis_options+="${LOCAL_LIMIT_N_EVENTS}"

aliroot_command="aliroot -l -b -q RunTask.C(${analysis_options})"
echo "${aliroot_command}"
${aliroot_command} 2>&1 | tee analysis.log

# go back
cd "${current_dir}"
