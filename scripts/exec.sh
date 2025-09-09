#!/bin/bash

if [[ -z ${E2V_TASK_DIR} ]]; then echo "missing E2V_TASK_DIR" ; exit 1; fi
if [[ $# -ne 1 ]]; then echo "usage: ./exec.sh <config_file>"; exit 1; fi
config_file=$1

source ${config_file}
echo "exec.sh :: from config_file : ${config_file}"
echo "exec.sh ::   MODE                 = \"${MODE}\""
echo "exec.sh ::   INPUT_PATH           = \"${INPUT_PATH}\""
echo "exec.sh ::   PRODUCTION_NAME      = \"${PRODUCTION_NAME}\""
echo "exec.sh ::   RUN_NUMBER           = ${RUN_NUMBER}"
echo "exec.sh ::   LOCAL_N_DIRS         = ${LOCAL_N_DIRS}"
echo "exec.sh ::   LOCAL_LIMIT_N_EVENTS = ${LOCAL_LIMIT_N_EVENTS}"
echo "exec.sh ::   GRID_TEST_MODE       = ${GRID_TEST_MODE}"
echo "exec.sh ::   GRID_WORKING_DIR     = \"${GRID_WORKING_DIR}\""
echo "exec.sh ::   GRID_CUSTOM_SPLIT    = ${GRID_CUSTOM_SPLIT}"
echo "exec.sh ::   GRID_CUSTOM_PATTERN  = \"${GRID_CUSTOM_PATTERN}\""
echo "exec.sh ::   ATTEMPT_NAME         = ${ATTEMPT_NAME}"

attempt_dir=${E2V_TASK_DIR}/attempts/${ATTEMPT_NAME}
mkdir -p ${attempt_dir}

cp ${E2V_TASK_DIR}/AliAnalysisTaskEsd2Vector.cxx ${attempt_dir}/
cp ${E2V_TASK_DIR}/AliAnalysisTaskEsd2Vector.h ${attempt_dir}/
cp ${E2V_TASK_DIR}/AliAnalysisTaskEsd2Vector_Const.h ${attempt_dir}/
cp ${E2V_TASK_DIR}/AliAnalysisTaskEsd2Vector_Cuts.h ${attempt_dir}/
cp ${E2V_TASK_DIR}/AddTaskEsd2Vector.C ${attempt_dir}/
cp ${E2V_TASK_DIR}/runAnalysis.C ${attempt_dir}/

cd ${attempt_dir} || exit

analysis_options="("
analysis_options+="\"${MODE}\","
analysis_options+="\"${INPUT_PATH}\","
analysis_options+="\"${PRODUCTION_NAME}\","
analysis_options+="${RUN_NUMBER},"
analysis_options+="${LOCAL_N_DIRS},"
analysis_options+="${LOCAL_LIMIT_N_EVENTS},"
analysis_options+="${GRID_TEST_MODE},"
analysis_options+="\"${GRID_WORKING_DIR}\","
analysis_options+="${GRID_CUSTOM_SPLIT},"
analysis_options+="\"${GRID_CUSTOM_PATTERN}\""
analysis_options+=")"

aliroot_command="aliroot -l -b -q runAnalysis.C${analysis_options}"
echo ${aliroot_command}
${aliroot_command} 2>&1 | tee analysis.log

cd ${E2V_TASK_DIR} || exit
