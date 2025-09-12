#!/bin/bash

# !! Don't execute this script directly, it is meant to be used by `submit_batch_data.sh` !!

#SBATCH --partition=main
#SBATCH --time=4:00:00
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=2000

# check environment
if [[ -z ${LUSTRE_HOME} ]]; then echo "missing LUSTRE_HOME"; exit 1; fi
if [[ -z ${JALIEN_TOKEN_CERT} ]]; then echo "missing JALIEN_TOKEN_CERT"; exit 1; fi
if [[ -z ${JALIEN_TOKEN_KEY} ]]; then echo "missing JALIEN_TOKEN_KEY"; exit 1; fi
if [[ -z ${E2V_OUTPUT_DIR} ]]; then echo "missing E2V_OUTPUT_DIR"; exit 1; fi
if [[ -z ${E2V_TASK_DIR} ]]; then echo "missing E2V_TASK_DIR"; exit 1; fi
if [[ -z ${E2V_SLURM_DIR} ]]; then echo "missing E2V_SLURM_DIR"; exit 1; fi
# -- batch options
if [[ -z ${MODE} ]]; then echo "missing MODE"; exit 1; fi
if [[ -z ${PRODUCTION_NAME} ]]; then echo "missing PRODUCTION_NAME"; exit 1; fi
if [[ -z ${GRID_TEST_MODE} ]]; then echo "missing GRID_TEST_MODE"; exit 1; fi
if [[ -z ${YEAR_2DIG} ]]; then echo "missing YEAR_2DIG"; exit 1; fi
if [[ -z ${PASS_NUMBER} ]]; then echo "missing PASS_NUMBER"; exit 1; fi
# -- per run number options
if [[ -z ${UNROLLED_RUNS} ]]; then echo "missing UNROLLED_RUNS"; exit 1; fi
if [[ -z ${UNROLLED_DN} ]]; then echo "missing UNROLLED_DN"; exit 1; fi
if [[ -z ${UNROLLED_N_DIRS} ]]; then echo "missing UNROLLED_N_DIRS"; exit 1; fi

# convert space-separated strings into arrays (NOTE: because Slurm)
all_runs=(${UNROLLED_RUNS})
all_dn=(${UNROLLED_DN})
all_n_dirs=(${UNROLLED_N_DIRS})

# extract rn+dn info from job index
run_number=${all_runs[${SLURM_ARRAY_TASK_ID}]}
dir_number=${all_dn[${SLURM_ARRAY_TASK_ID}]}
n_dirs=${all_n_dirs[${SLURM_ARRAY_TASK_ID}]}

# log hack (https://unix.stackexchange.com/a/585453)
tmp_logfile=${E2V_SLURM_DIR}/tmp/${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.log # = ${E2V_SLURM_DIR}/tmp/%A_%a.log
slurm_subdir=${E2V_SLURM_DIR}/${PRODUCTION_NAME}/${run_number}
mkdir -p "${slurm_subdir}"
ln -f "${tmp_logfile}" "${slurm_subdir}/${SLURM_ARRAY_JOB_ID}_${SLURM_ARRAY_TASK_ID}.log"

# load software within container
export ALIBUILD_WORK_DIR="/opt/alice/sw"
eval "$(alienv shell-helper)"
eval "$(alienv printenv AliRoot/latest,AliPhysics/latest)"
export ALICE_DATA=/opt/alice/analysis-data

input_path="/alice/data/20${YEAR_2DIG}/${PRODUCTION_NAME}"
custom_pattern="/pass${PASS_NUMBER}/${YEAR_2DIG}000${run_number}${dir_number}*/AliESDs.root"

# prepare output dirs
rn_output_dir=${E2V_OUTPUT_DIR}/${PRODUCTION_NAME}/${run_number}
mkdir -p "${rn_output_dir}"
dn_output_dir=${rn_output_dir}/${dir_number} # temporary dir, will be removed later
mkdir -p "${dn_output_dir}"

ANALYSIS_OPTIONS="("
ANALYSIS_OPTIONS+="\"${MODE}\","
ANALYSIS_OPTIONS+="\"${input_path}\","
ANALYSIS_OPTIONS+="\"${PRODUCTION_NAME}\","
ANALYSIS_OPTIONS+="${run_number},"
ANALYSIS_OPTIONS+="${n_dirs},"
ANALYSIS_OPTIONS+="0," # LOCAL_LIMIT_N_EVENTS, not used
ANALYSIS_OPTIONS+="${GRID_TEST_MODE},"
ANALYSIS_OPTIONS+="\"\"," # GRID_WORKING_DIR, not used
ANALYSIS_OPTIONS+="0," # GRID_CUSTOM_SPLIT, not used
ANALYSIS_OPTIONS+="\"${custom_pattern}\""
ANALYSIS_OPTIONS+=")"

cp "${E2V_TASK_DIR}/AliAnalysisTaskEsd2Vector.cxx" "${dn_output_dir}/"
cp "${E2V_TASK_DIR}/AliAnalysisTaskEsd2Vector.h" "${dn_output_dir}/"
cp "${E2V_TASK_DIR}/AliAnalysisTaskEsd2Vector_Const.h" "${dn_output_dir}/"
cp "${E2V_TASK_DIR}/AliAnalysisTaskEsd2Vector_Cuts.h" "${dn_output_dir}/"
cp "${E2V_TASK_DIR}/AddTaskEsd2Vector.C" "${dn_output_dir}/"
cp "${E2V_TASK_DIR}/runAnalysis.C" "${dn_output_dir}/"

cd "${dn_output_dir}" || exit

ALIROOT_COMMAND="aliroot -l -b -q runAnalysis.C${ANALYSIS_OPTIONS}"
echo "${ALIROOT_COMMAND}"
${ALIROOT_COMMAND}

if [[ ! -e ${dn_output_dir}/AnalysisResults.root ]]; then echo "error: output file AnalysisResults.root couldn't be created"; exit 1; fi

cd "${rn_output_dir}" || exit
mv -v "${dn_output_dir}/AnalysisResults.root" "${rn_output_dir}/AnalysisResults_${dir_number}.root"
rm -rfv "${dn_output_dir}"
rm "${tmp_logfile}"
