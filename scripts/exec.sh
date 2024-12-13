#!/bin/bash

# USAGE: bash exec.sh <CONFIG_FILE> <SIMULATION_SET (optional)>

CONFIG_FILE=$1
export SIMULATION_SET=$2

if [[ -z ${CONFIG_FILE} ]]; then
    echo "USAGE :: bash exec.sh <CONFIG_FILE> <SIMULATION_SET (optional)>"
    exit 1
fi

source ${CONFIG_FILE}
echo "INFO :: exec.sh :: CONFIG_FILE      = ${CONFIG_FILE}"
echo "INFO :: exec.sh :: SIMULATION_SET   = ${SIMULATION_SET}"
echo "INFO :: exec.sh :: ATTEMPT_NAME     = ${ATTEMPT_NAME}"
echo "INFO :: exec.sh :: MODE             = ${MODE}"
echo "INFO :: exec.sh :: LOCAL_INPUT_PATH = ${LOCAL_INPUT_PATH}"
echo "INFO :: exec.sh :: LOCAL_N_DIRS     = ${LOCAL_N_DIRS}"
echo "INFO :: exec.sh :: GRID_TEST_MODE   = ${GRID_TEST_MODE}"
echo "INFO :: exec.sh :: IS_MC            = ${IS_MC}"
echo "INFO :: exec.sh :: PRODUCTION_NAME  = ${PRODUCTION_NAME}"
echo "INFO :: exec.sh :: RUN_NUMBERS_LIST = ${RUN_NUMBERS_LIST}"
echo "INFO :: exec.sh :: CHOOSE_N_EVENTS  = ${CHOOSE_N_EVENTS}"

ATTEMPT_DIR=attempts/${ATTEMPT_NAME}
mkdir -p ${ATTEMPT_DIR}

cp AliAnalysisTaskEsd2Tree.cxx ${ATTEMPT_DIR}/
cp AliAnalysisTaskEsd2Tree.h ${ATTEMPT_DIR}/
cp AddTask_Esd2Tree.C ${ATTEMPT_DIR}/
cp runAnalysis.C ${ATTEMPT_DIR}/

cd ${ATTEMPT_DIR}

ANALYSIS_OPTIONS="("
ANALYSIS_OPTIONS+="\"${MODE}\","
ANALYSIS_OPTIONS+="\"${LOCAL_INPUT_PATH}\","
ANALYSIS_OPTIONS+="${LOCAL_N_DIRS},"
ANALYSIS_OPTIONS+="${GRID_TEST_MODE},"
ANALYSIS_OPTIONS+="${IS_MC},"
ANALYSIS_OPTIONS+="\"${PRODUCTION_NAME}\","
ANALYSIS_OPTIONS+="\"${RUN_NUMBERS_LIST}\","
ANALYSIS_OPTIONS+="\"${SIMULATION_SET}\","
ANALYSIS_OPTIONS+="${CHOOSE_N_EVENTS}"
ANALYSIS_OPTIONS+=")"

ALIROOT_COMMAND='aliroot -l -b -q runAnalysis.C'${ANALYSIS_OPTIONS}''
echo ${ALIROOT_COMMAND}
${ALIROOT_COMMAND} 2>&1 | tee analysis.log

rm -v AliAnalysisTaskEsd2Tree*
rm -v AddTask_Esd2Tree.C
rm -v runAnalysis.C

cd ..
