#!/bin/bash

# `esd2vector/scripts/farm-pi/mc_task_exec.sh`
# ============================================
# NOTE: don't execute this script directly, it is meant to be used by `esd2vector/scripts/farm-pi/mc_task_wrapper.sh`

#SBATCH --partition=main
#SBATCH --time=2:00:00
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=4000

set -euo pipefail

# check environment
if [[ -z ${LOCAL_SIMS_DIR:-} ]]; then echo "error: missing env. var. LOCAL_SIMS_DIR"; exit 1; fi
if [[ -z ${E2R_ROOT_DIR:-} ]]; then echo "error: missing env. var. E2R_ROOT_DIR"; exit 1; fi
# -- batch options
if [[ -z ${MODE:-} ]]; then echo "error: missing env. var. MODE"; exit 1; fi
if [[ -z ${LOCAL_N_DIRS:-} ]]; then echo "error: missing env. var. LOCAL_N_DIRS"; exit 1; fi
if [[ -z ${PRODUCTION_NAME:-} ]]; then echo "error: missing env. var. PRODUCTION_NAME"; exit 1; fi
# -- per run number options
if [[ -z ${RUN_NUMBERS_STR:-} ]]; then echo "error: missing env. var. RUN_NUMBERS_STR"; exit 1; fi

# define input path
reaction_channel=""
injected_mass=""
if [[ ${PRODUCTION_NAME} != "LHC26h" ]]; then
    # -- check env.
    if [[ -z ${CHANNELS_STR:-} ]]; then echo "error: missing env. var. CHANNELS_STR"; exit 1; fi
    if [[ -z ${MASSES_STR:-} ]]; then echo "error: missing env. var. MASSES_STR"; exit 1; fi
    # -- get reaction channel
    read -ra CHANNELS_ARR <<< "${CHANNELS_STR}" # string -> array (because Slurm)
    reaction_channel=${CHANNELS_ARR[${SLURM_ARRAY_TASK_ID}]}
    # -- get injected anti-sexaquark mass
    read -ra MASSES_ARR <<< "${MASSES_STR}" # string -> array (because Slurm)
    injected_mass=${MASSES_ARR[${SLURM_ARRAY_TASK_ID}]}
    input_path=${LOCAL_SIMS_DIR}/${PRODUCTION_NAME}/${reaction_channel}${injected_mass}
else
    input_path=${LOCAL_SIMS_DIR}/${PRODUCTION_NAME}/signal
fi

# get run number
read -ra RUN_NUMBERS_ARR <<< "${RUN_NUMBERS_STR}" # string -> array (because Slurm)
run_number=${RUN_NUMBERS_ARR[${SLURM_ARRAY_TASK_ID}]}

# prepare output dirs
red_prod_name=${PRODUCTION_NAME/LHC/} # remove 'LHC'
simset_outdir=${E2R_ROOT_DIR}/output/${MODE}_mc_${red_prod_name}/${reaction_channel}${injected_mass}
rn_outdir=${simset_outdir}/${run_number}
mkdir -p "${rn_outdir}"

cd "${rn_outdir}"

# copy task files
cp "${E2R_ROOT_DIR}/task/RunTask.C" .
cp "${E2R_ROOT_DIR}/task/AddTaskEsd2Vector.C" .
cp "${E2R_ROOT_DIR}/task/AliTaskEsd2Vector.cxx" .
cp "${E2R_ROOT_DIR}/task/AliTaskEsd2Vector.h" .
cp "${E2R_ROOT_DIR}/task/AliTaskEsd2Vector_LinkDef.h" .
cp "${E2R_ROOT_DIR}/common/Constants.hpp" .
cp "${E2R_ROOT_DIR}/common/Framework_TeeTree.hpp" .
cp "${E2R_ROOT_DIR}/common/Schema_Events.hpp" .
cp "${E2R_ROOT_DIR}/common/E2R_Cuts.hpp" .
cp "${E2R_ROOT_DIR}/common/POD_Event.hpp" .
cp "${E2R_ROOT_DIR}/common/POD_InjectedSexa.hpp" .
cp "${E2R_ROOT_DIR}/common/POD_McParticle.hpp" .
cp "${E2R_ROOT_DIR}/common/POD_PreFoundLambda.hpp" .
cp "${E2R_ROOT_DIR}/common/POD_Track.hpp" .

# main command #

analysis_options="("
analysis_options+="\"${MODE}\","
analysis_options+="\"${input_path}\","
analysis_options+="\"${PRODUCTION_NAME}\","
analysis_options+="${run_number},"
analysis_options+="${LOCAL_N_DIRS}"
analysis_options+=")"

aliroot_command="aliroot -l -b -q RunTask.C${analysis_options}"
echo "${aliroot_command}"
${aliroot_command}

# move output file and clean intermediate dir
mv -v "${rn_outdir}/AnalysisResults.root" "${simset_outdir}/AnalysisResults_${run_number}.root"
cd "${simset_outdir}"
rm -rfv "${rn_outdir}"
