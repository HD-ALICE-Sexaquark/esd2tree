#!/bin/bash

# !! Don't execute this script directly, it is meant to be used by `submit_batch_data.sh` !!

#SBATCH --partition=main
#SBATCH --time=4:00:00
#SBATCH --mem-per-cpu=4000

# check environment
if [[ -z ${LUSTRE_HOME} ]]; then echo "missing LUSTRE_HOME"; exit 1; fi
if [[ -z ${JALIEN_TOKEN_CERT} ]]; then echo "missing JALIEN_TOKEN_CERT"; exit 1; fi
if [[ -z ${JALIEN_TOKEN_KEY} ]]; then echo "missing JALIEN_TOKEN_KEY"; exit 1; fi
if [[ -z ${E2V_OUTPUT_DIR} ]]; then echo "missing E2V_OUTPUT_DIR"; exit 1; fi
if [[ -z ${E2V_TASK_DIR} ]]; then echo "missing E2V_TASK_DIR"; exit 1; fi
# -- batch options
if [[ -z ${MODE} ]]; then echo "missing MODE"; exit 1; fi
if [[ -z ${PRODUCTION_NAME} ]]; then echo "missing PRODUCTION_NAME"; exit 1; fi
if [[ -z ${GRID_TEST_MODE} ]]; then echo "missing GRID_TEST_MODE"; exit 1; fi
if [[ -z ${YEAR_2DIG} ]]; then echo "missing YEAR_2DIG"; exit 1; fi
if [[ -z ${PASS_NUMBER} ]]; then echo "missing PASS_NUMBER"; exit 1; fi
# -- per run number options
if [[ -z ${RUN_NUMBER} ]]; then echo "missing RUN_NUMBER"; exit 1; fi
if [[ -z ${UNROLLED_DN} ]]; then echo "missing UNROLLED_DN"; exit 1; fi
if [[ -z ${UNROLLED_N_DIRS} ]]; then echo "missing UNROLLED_N_DIRS"; exit 1; fi

# load software within container
export ALIBUILD_WORK_DIR="/opt/alice/sw"
eval "$(alienv shell-helper)"
eval "$(alienv printenv AliRoot/latest,AliPhysics/latest)"
export ALICE_DATA=/opt/alice/analysis-data

# convert space-separated strings into arrays (NOTE: because Slurm)
all_dn=(${UNROLLED_DN})
all_n_dirs=(${UNROLLED_N_DIRS})

# extract dn info from job index
dir_number=${all_dn[${SLURM_ARRAY_TASK_ID}]}
n_dirs=${all_n_dirs[${SLURM_ARRAY_TASK_ID}]}

input_path="/alice/data/20${YEAR_2DIG}/${PRODUCTION_NAME}"
custom_pattern="/pass${PASS_NUMBER}/${YEAR_2DIG}000${RUN_NUMBER}${dir_number}*/AliESDs.root"

# prepare output dir
this_job_outdir=${E2V_OUTPUT_DIR}/${PRODUCTION_NAME}/${RUN_NUMBER}/${dir_number}
mkdir -p ${this_job_outdir}

ANALYSIS_OPTIONS="("
ANALYSIS_OPTIONS+="\"${MODE}\","
ANALYSIS_OPTIONS+="\"${input_path}\","
ANALYSIS_OPTIONS+="\"${PRODUCTION_NAME}\","
ANALYSIS_OPTIONS+="${RUN_NUMBER},"
ANALYSIS_OPTIONS+="${n_dirs},"
ANALYSIS_OPTIONS+="0," # LOCAL_LIMIT_N_EVENTS, not used
ANALYSIS_OPTIONS+="${GRID_TEST_MODE},"
ANALYSIS_OPTIONS+="\"\"," # GRID_WORKING_DIR, not used
ANALYSIS_OPTIONS+="0," # GRID_CUSTOM_SPLIT, not used
ANALYSIS_OPTIONS+="\"${custom_pattern}\""
ANALYSIS_OPTIONS+=")"

cp ${E2V_TASK_DIR}/AliAnalysisTaskEsd2Vector.cxx ${this_job_outdir}/
cp ${E2V_TASK_DIR}/AliAnalysisTaskEsd2Vector.h ${this_job_outdir}/
cp ${E2V_TASK_DIR}/AliAnalysisTaskEsd2Vector_Const.h ${this_job_outdir}/
cp ${E2V_TASK_DIR}/AliAnalysisTaskEsd2Vector_Cuts.h ${this_job_outdir}/
cp ${E2V_TASK_DIR}/AddTaskEsd2Vector.C ${this_job_outdir}/
cp ${E2V_TASK_DIR}/runAnalysis.C ${this_job_outdir}/

cd ${this_job_outdir} || exit

ALIROOT_COMMAND="aliroot -l -b -q runAnalysis.C${ANALYSIS_OPTIONS}"
echo ${ALIROOT_COMMAND}
${ALIROOT_COMMAND}

# end of job: delete all files that are not `AnalysisResults.root`
find ${this_job_outdir} -maxdepth 1 -type f ! -name 'AnalysisResults.root' -delete
