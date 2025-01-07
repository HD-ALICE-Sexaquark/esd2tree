#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "ERROR :: task_batch.sh :: wrong number of arguments"
    echo "ERROR :: task_batch.sh :: USAGE: 1. set ANALYSIS_DIR, ATASK_DIR, OUTPUT_DIR and SLURM_LOGS_DIR"
    echo "ERROR :: task_batch.sh ::        2. \`./task_batch.sh <config_file>\`"
    exit 1
fi

if [[ -z ${ANALYSIS_DIR} || -z ${ATASK_DIR} || -z ${OUTPUT_DIR} || -z ${SLURM_LOGS_DIR} ]]; then
    echo "ERROR :: task_batch.sh :: make sure to set:"
    echo "ERROR :: task_batch.sh :: - ANALYSIS_DIR   : where the main analysis repository is"
    echo "ERROR :: task_batch.sh :: - ATASK_DIR      : where AliAnalysisTaskEsd2Tree is"
    echo "ERROR :: task_batch.sh :: - OUTPUT_DIR     : where the output will be stored"
    echo "ERROR :: task_batch.sh :: - SLURM_LOGS_DIR : where the slurm logs will be stored"
    exit 1
fi

echo "INFO :: task_batch.sh :: starting..."

# OPTIONS
CONFIG_FILE=$1
source ${CONFIG_FILE}
echo "INFO :: task_batch.sh :: Reading CONFIG_FILE : ${CONFIG_FILE}"
echo "INFO :: task_batch.sh :: >> ATTEMPT_NAME     = ${ATTEMPT_NAME}"
echo "INFO :: task_batch.sh :: >> MODE             = ${MODE}"
echo "INFO :: task_batch.sh :: >> INPUT_PATH       = ${INPUT_PATH}"
echo "INFO :: task_batch.sh :: >> LOCAL_N_DIRS     = ${LOCAL_N_DIRS}"
echo "INFO :: task_batch.sh :: >> GRID_TEST_MODE   = ${GRID_TEST_MODE}"
echo "INFO :: task_batch.sh :: >> IS_MC            = ${IS_MC}"
echo "INFO :: task_batch.sh :: >> PRODUCTION_NAME  = ${PRODUCTION_NAME}"
echo "INFO :: task_batch.sh :: >> RUN_NUMBERS_LIST = ${RUN_NUMBERS_LIST}"
echo "INFO :: task_batch.sh :: >> SIMULATION_SET   = ${SIMULATION_SET}"
echo "INFO :: task_batch.sh :: >> CHOOSE_N_EVENTS  = ${CHOOSE_N_EVENTS}"

SBATCH_SETUP="--partition=main"
SBATCH_SETUP+=" --output=${SLURM_LOGS_DIR}/slurm-%J.out"
SBATCH_SETUP+=" --error=${SLURM_LOGS_DIR}/slurm-%J.err"
SBATCH_SETUP+=" --time=12:00:00"
SBATCH_SETUP+=" --mem-per-cpu=4000" # in MB

while IFS= read -r RUN_NUMBER; do
    N_JOBS=$(wc -l < ${ANALYSIS_DIR}/doc/dir_numbers/DN_${PRODUCTION_NAME}_${RUN_NUMBER}.txt)
    echo "INFO :: task_batch.sh :: RUN_NUMBER = ${RUN_NUMBER}, N_JOBS = ${N_JOBS}"
    for (( i=1; i<=${N_JOBS}; i++ )); do
        export LOCAL_N_DIRS=${i}

        export TASK_OUTPUT_DIR=${OUTPUT_DIR}/${ATTEMPT_NAME}/${RUN_NUMBER}/${RUN_NUMBER}_${i}
        mkdir -p ${TASK_OUTPUT_DIR}

        echo ${RUN_NUMBER} > ${TASK_OUTPUT_DIR}/rn.txt

        cp ${ATASK_DIR}/AliAnalysisTaskEsd2Tree.cxx ${TASK_OUTPUT_DIR}/AliAnalysisTaskEsd2Tree.cxx
        cp ${ATASK_DIR}/AliAnalysisTaskEsd2Tree.h ${TASK_OUTPUT_DIR}/AliAnalysisTaskEsd2Tree.h
        cp ${ATASK_DIR}/runAnalysis.C ${TASK_OUTPUT_DIR}/runAnalysis.C
        cp ${ATASK_DIR}/AddTaskEsd2Tree.C ${TASK_OUTPUT_DIR}/AddTaskEsd2Tree.C

        JOB_NAME="--job-name=${ATTEMPT_NAME}_${RUN_NUMBER}_${i}"
        echo -n "INFO :: task_batch.sh :: "
        sbatch ${SBATCH_SETUP} ${JOB_NAME} ${ANALYSIS_DIR}/scripts/task_single.sh
    done
done < "${RUN_NUMBERS_LIST}"

echo "INFO :: task_batch.sh :: all jobs submitted."
