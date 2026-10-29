#!/bin/bash

# `esd2tree/scripts/farm-pi/slurm_exec.sh`
# ========================================
# NOTE: don't execute this script directly, it is meant to be used by `esd2tree/scripts/farm-pi/slurm_wrapper.sh`
# NOTE: unpacks the tars of its run number into node-local storage (~25 x 14 MB), runs there, and copies back only the
#       output file
# NOTE: time and memory limits are an educated guess, not measured (25 dirs per job: 20 events each in `signal`, 2
#       HIJING events each in `embedded`)

#SBATCH --partition=main
#SBATCH --time=2:00:00
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=4000

set -euo pipefail
shopt -s nullglob

# check environment
if [[ -z ${LOCAL_SIMS_DIR:-} ]]; then echo "error: missing env. var. LOCAL_SIMS_DIR"; exit 1; fi
if [[ -z ${E2T_ROOT_DIR:-} ]]; then echo "error: missing env. var. E2T_ROOT_DIR"; exit 1; fi
# -- batch options
if [[ -z ${MAX_N_DN_DIRS:-} ]]; then echo "error: missing env. var. MAX_N_DN_DIRS"; exit 1; fi
if [[ -z ${OUTPUT_TAG:-} ]]; then echo "error: missing env. var. OUTPUT_TAG"; exit 1; fi
if [[ -z ${PRODUCTION_NAME:-} ]]; then echo "error: missing env. var. PRODUCTION_NAME"; exit 1; fi
if [[ -z ${VARIANT:-} ]]; then echo "error: missing env. var. VARIANT"; exit 1; fi
if [[ -z ${OUTPUT_DIR:-} ]]; then echo "error: missing env. var. OUTPUT_DIR"; exit 1; fi
# -- per run number options
if [[ -z ${RUN_NUMBERS_STR:-} ]]; then echo "error: missing env. var. RUN_NUMBERS_STR"; exit 1; fi
# -- slurm vars
if [[ -z ${SLURM_ARRAY_JOB_ID:-} ]]; then echo "error: missing env. var. SLURM_ARRAY_JOB_ID"; exit 1; fi
if [[ -z ${SLURM_ARRAY_TASK_ID:-} ]]; then echo "error: missing env. var. SLURM_ARRAY_TASK_ID"; exit 1; fi

# get run number
read -ra RUN_NUMBERS_ARR <<< "${RUN_NUMBERS_STR}" # string -> array (because Slurm)
run_number=${RUN_NUMBERS_ARR[${SLURM_ARRAY_TASK_ID}]}

# work in node-local storage, removed at exit
work_dir=$(mktemp -d "${TMPDIR:-/tmp}/e2t_${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.XXXXXX")
trap 'rm -rf "${work_dir}"' EXIT

# unpack the tars into the layout `RunTask.C` expects: <input path> / <run number> / <dir number> / <files>.root
# NOTE: `RunTask.C` takes the variant from the last directory of the input path
input_path=${work_dir}/${VARIANT}
n_tars=0
for tar_file in "${LOCAL_SIMS_DIR}/${OUTPUT_TAG}/${run_number}/${run_number}"_*.tar; do
    dir_number=$(basename "${tar_file}" .tar)
    dir_number=${dir_number##*_}
    mkdir -p "${input_path}/${run_number}/${dir_number}"
    tar -xf "${tar_file}" -C "${input_path}/${run_number}/${dir_number}"
    n_tars=$((n_tars + 1))
done
if (( n_tars == 0 )); then echo "error: no tars found for ${OUTPUT_TAG}/${run_number}"; exit 1; fi
echo "$0 @ ${HOSTNAME} :: unpacked ${n_tars} tars of ${OUTPUT_TAG}/${run_number} into ${input_path}"

run_dir=${work_dir}/run
mkdir -p "${run_dir}"
cd "${run_dir}"

# copy task files
cp "${E2T_ROOT_DIR}/task/RunTask.C" .
cp "${E2T_ROOT_DIR}/task/AliTaskEsd2Tree.h" .
cp "${E2T_ROOT_DIR}/task/AliTaskEsd2Tree.cxx" .
cp "${E2T_ROOT_DIR}/task/AliTaskEsd2Tree_LinkDef.h" .
cp "${E2T_ROOT_DIR}/task/AddTaskEsd2Tree.C" .
cp "${E2T_ROOT_DIR}/common/Schema_Events.hpp" .
cp "${E2T_ROOT_DIR}/common/POD_Track.hpp" .
cp "${E2T_ROOT_DIR}/common/POD_PreFoundLambda.hpp" .
cp "${E2T_ROOT_DIR}/common/POD_McParticle.hpp" .
cp "${E2T_ROOT_DIR}/common/POD_Event.hpp" .
cp "${E2T_ROOT_DIR}/common/Math.hpp" .
cp "${E2T_ROOT_DIR}/common/Framework_TeeTree.hpp" .
cp "${E2T_ROOT_DIR}/common/Cuts_E2T.hpp" .
cp "${E2T_ROOT_DIR}/common/Constants.hpp" .

# main command #
# NOTE: always "local" mode

analysis_options="\"local\","
analysis_options+="\"${input_path}\","
analysis_options+="\"${PRODUCTION_NAME}\","
analysis_options+="${run_number},"
analysis_options+="${MAX_N_DN_DIRS}"

aliroot_command="aliroot -l -b -q RunTask.C(${analysis_options})"
echo "${aliroot_command}"
${aliroot_command}

# copy the output file back
# -- NOTE: as `.part` first, so a job killed while copying leaves no truncated output (which `slurm_wrapper.sh` would
#    take as done)
output_file=${OUTPUT_DIR}/AnalysisResults_${run_number}.root
mkdir -p "${OUTPUT_DIR}"
cp -v "${run_dir}/AnalysisResults.root" "${output_file}.part"
mv -v "${output_file}.part" "${output_file}"
