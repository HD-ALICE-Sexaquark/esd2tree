#!/bin/bash

set -euo pipefail

# environment
if [[ -z ${E2T_ROOT_DIR:-} ]]; then echo "error: missing env. var. E2T_ROOT_DIR" ; exit 1; fi
if [[ -z ${GRID_HOME_DIR:-} ]]; then echo "error: missing env. var. GRID_HOME_DIR" ; exit 1; fi

# command-line arguments
if [[ $# -ne 1 ]]; then echo "usage: ./exec.sh <config_file>"; exit 1; fi
config_file=$1

source "${config_file}"
echo "exec.sh :: from config_file : ${config_file}"
echo "exec.sh ::   MODE                 = \"${MODE}\""
echo "exec.sh ::   INPUT_PATH           = \"${INPUT_PATH}\""
echo "exec.sh ::   PRODUCTION_NAME      = \"${PRODUCTION_NAME}\""
echo "exec.sh ::   RUN_NUMBER           = ${RUN_NUMBER}"
echo "exec.sh ::   LOCAL_N_DIRS         = ${LOCAL_N_DIRS}"
echo "exec.sh ::   LOCAL_LIMIT_N_EVENTS = ${LOCAL_LIMIT_N_EVENTS}"
echo "exec.sh ::   GRID_WORKING_DIR     = \"${GRID_WORKING_DIR}\""
echo "exec.sh ::   ATTEMPT_NAME         = ${ATTEMPT_NAME}"

current_dir=${PWD}

arr_custom_xml=("")
if [[ ${MODE} == "grid" && ${PRODUCTION_NAME} =~ ^LHC1 ]]; then # data real?
    cd "${E2T_ROOT_DIR}"
    xml_rn_dir=xml/${PRODUCTION_NAME}/${RUN_NUMBER} # relative to E2T_ROOT_DIR
    if [[ -d ${xml_rn_dir} ]]; then # run number big?
        mapfile -t arr_custom_xml < <(find "${xml_rn_dir}" -name "*.xml")
    fi
    cd "${current_dir}"
fi

for index in "${!arr_custom_xml[@]}"; do

    custom_xml=""
    attempt_suffix=""
    if [[ ${arr_custom_xml[${index}]} != "" ]];then
        custom_xml=${GRID_HOME_DIR}/${arr_custom_xml[${index}]} # mirrored at grid
        attempt_suffix="_$((index + 1))"
    fi

    attempt_dir=${E2T_ROOT_DIR}/task/attempts/${ATTEMPT_NAME}_${RUN_NUMBER}${attempt_suffix}
    mkdir -p "${attempt_dir}"

    echo "exec.sh ::   >> GRID_CUSTOM_XML   = \"${custom_xml}\""
    echo "exec.sh ::   >> ATTEMPT_DIR       = ${attempt_dir}"

    cd "${attempt_dir}"

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
    cp "${E2T_ROOT_DIR}/common/POD_InjectedSexa.hpp" .
    cp "${E2T_ROOT_DIR}/common/POD_Event.hpp" .
    cp "${E2T_ROOT_DIR}/common/Math.hpp" .
    cp "${E2T_ROOT_DIR}/common/Framework_TeeTree.hpp" .
    cp "${E2T_ROOT_DIR}/common/Cuts_E2T.hpp" .
    cp "${E2T_ROOT_DIR}/common/Constants.hpp" .

    cp "${E2T_ROOT_DIR}/tidentity/macros/AddTask_marsland_TIdentityPID.C" .
    cp "${E2T_ROOT_DIR}/tidentity/macros/AliAnalysisTaskTIdentityPID.cxx" .
    cp "${E2T_ROOT_DIR}/tidentity/macros/AliAnalysisTaskTIdentityPID.h" .
    cp "${E2T_ROOT_DIR}/tidentity/macros/Config_marsland_TIdentityPID.C" .
    cp "${E2T_ROOT_DIR}/tidentity/macros/AddTaskFilteredTreeLocal.C" .

    analysis_options="("
    analysis_options+="\"${MODE}\","
    analysis_options+="\"${INPUT_PATH}\","
    analysis_options+="\"${PRODUCTION_NAME}\","
    analysis_options+="${RUN_NUMBER},"
    analysis_options+="${LOCAL_N_DIRS},"
    analysis_options+="${LOCAL_LIMIT_N_EVENTS},"
    analysis_options+="true,"
    analysis_options+="\"${GRID_WORKING_DIR}\","
    analysis_options+="\"${custom_xml}\""
    analysis_options+=")"

    aliroot_command="aliroot -l -b -q RunTask.C${analysis_options}"
    echo "${aliroot_command}"
    ${aliroot_command} 2>&1 | tee analysis.log
done

# go back
cd "${current_dir}"
