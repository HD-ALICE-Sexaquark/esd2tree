#!/bin/bash

# `esd2tree_hdib/scripts/farm-pi/slurm_exec.sh`
# =============================================
# NOTE: don't execute this script directly, it is meant to be used by `esd2tree_hdib/scripts/farm-pi/slurm_wrapper.sh`
# NOTE: time and memory limits are an educated guess, not measured (LHC26h2 `signal`: 60 files x 20 events per job)

#SBATCH --partition=main
#SBATCH --time=2:00:00
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=4000

set -euo pipefail

# check environment
if [[ -z ${LOCAL_SIMS_DIR:-} ]]; then echo "error: missing env. var. LOCAL_SIMS_DIR"; exit 1; fi
if [[ -z ${E2T_ROOT_DIR:-} ]]; then echo "error: missing env. var. E2T_ROOT_DIR"; exit 1; fi
# -- batch options
if [[ -z ${MODE:-} ]]; then echo "error: missing env. var. MODE"; exit 1; fi
if [[ -z ${MAX_N_DN_DIRS:-} ]]; then echo "error: missing env. var. MAX_N_DN_DIRS"; exit 1; fi
if [[ -z ${PRODUCTION_NAME:-} ]]; then echo "error: missing env. var. PRODUCTION_NAME"; exit 1; fi
if [[ -z ${VARIANT:-} ]]; then echo "error: missing env. var. VARIANT"; exit 1; fi
# -- per run number options
if [[ -z ${RUN_NUMBERS_STR:-} ]]; then echo "error: missing env. var. RUN_NUMBERS_STR"; exit 1; fi
# -- slurm var
if [[ -z ${SLURM_ARRAY_TASK_ID:-} ]]; then echo "error: missing env. var. SLURM_ARRAY_TASK_ID"; exit 1; fi

# define input path
# NOTE: `RunTask.C` takes the variant from its last directory
input_path=${LOCAL_SIMS_DIR}/${PRODUCTION_NAME}/${VARIANT}

# get run number
read -ra RUN_NUMBERS_ARR <<< "${RUN_NUMBERS_STR}" # string -> array (because Slurm)
run_number=${RUN_NUMBERS_ARR[${SLURM_ARRAY_TASK_ID}]}

# prepare output dirs
short_prod_name=${PRODUCTION_NAME/LHC/} # remove 'LHC'
simset_outdir=${E2T_ROOT_DIR}/output/${MODE}_mc_${short_prod_name}/${VARIANT}
rn_outdir=${simset_outdir}/${run_number}
mkdir -p "${rn_outdir}"

cd "${rn_outdir}"

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

analysis_options="\"${MODE}\","
analysis_options+="\"${input_path}\","
analysis_options+="\"${PRODUCTION_NAME}\","
analysis_options+="${run_number},"
analysis_options+="${MAX_N_DN_DIRS}"

aliroot_command="aliroot -l -b -q RunTask.C(${analysis_options})"
echo "${aliroot_command}"
${aliroot_command}

# move output file and clean intermediate dir
mv -v "${rn_outdir}/AnalysisResults.root" "${simset_outdir}/AnalysisResults_${run_number}.root"
cd "${simset_outdir}"
rm -rfv "${rn_outdir}"
