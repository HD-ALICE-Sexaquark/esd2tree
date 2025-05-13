#!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "ERROR :: exec.sh :: USAGE: bash exec.sh <CONFIG_FILE>"
    exit 1
fi

CONFIG_FILE=$1

source ${CONFIG_FILE}
echo "INFO :: exec.sh :: CONFIG_FILE          = ${CONFIG_FILE}"
echo "INFO :: exec.sh :: ATTEMPT_NAME         = ${ATTEMPT_NAME}"
echo "INFO :: exec.sh :: MODE                 = ${MODE}"
echo "INFO :: exec.sh :: INPUT_PATH           = ${INPUT_PATH}"
echo "INFO :: exec.sh :: PRODUCTION_NAME      = ${PRODUCTION_NAME}"
echo "INFO :: exec.sh :: SIMULATION_SET       = ${SIMULATION_SET}"
echo "INFO :: exec.sh :: RUN_NUMBER           = ${RUN_NUMBER}"
echo "INFO :: exec.sh :: LOCAL_N_DIRS         = ${LOCAL_N_DIRS}"
echo "INFO :: exec.sh :: LOCAL_LIMIT_N_EVENTS = ${LOCAL_LIMIT_N_EVENTS}"
echo "INFO :: exec.sh :: GRID_TEST_MODE       = ${GRID_TEST_MODE}"
echo "INFO :: exec.sh :: GRID_WORKING_DIR     = ${GRID_WORKING_DIR}"
echo "INFO :: exec.sh :: GRID_CUSTOM_SPLIT    = ${GRID_CUSTOM_SPLIT}"
echo "INFO :: exec.sh :: GRID_CUSTOM_PATTERN  = ${GRID_CUSTOM_PATTERN}"

ATTEMPT_DIR=attempts/${ATTEMPT_NAME}
mkdir -p ${ATTEMPT_DIR}

cp AliAnalysisTaskEsd2Tree.cxx ${ATTEMPT_DIR}/
cp AliAnalysisTaskEsd2Tree.h ${ATTEMPT_DIR}/
cp AddTaskEsd2Tree.C ${ATTEMPT_DIR}/
cp runAnalysis.C ${ATTEMPT_DIR}/

cd ${ATTEMPT_DIR}

ANALYSIS_OPTIONS="("
ANALYSIS_OPTIONS+="\"${MODE}\","
ANALYSIS_OPTIONS+="\"${INPUT_PATH}\","
ANALYSIS_OPTIONS+="\"${PRODUCTION_NAME}\","
ANALYSIS_OPTIONS+="${RUN_NUMBER},"
ANALYSIS_OPTIONS+="${LOCAL_N_DIRS},"
ANALYSIS_OPTIONS+="${LOCAL_LIMIT_N_EVENTS},"
ANALYSIS_OPTIONS+="${GRID_TEST_MODE},"
ANALYSIS_OPTIONS+="\"${GRID_WORKING_DIR}\","
ANALYSIS_OPTIONS+="${GRID_CUSTOM_SPLIT},"
ANALYSIS_OPTIONS+="\"${GRID_CUSTOM_PATTERN}\""
ANALYSIS_OPTIONS+=")"

ALIROOT_COMMAND='aliroot -l -b -q runAnalysis.C'${ANALYSIS_OPTIONS}''
echo ${ALIROOT_COMMAND}
${ALIROOT_COMMAND} 2>&1 | tee analysis.log

cd ..
