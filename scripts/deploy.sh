#!/bin/bash

# `esd2vector/scripts/deploy.sh` -- Deploy files to GSI server.

set -euo pipefail

print_usage() { echo "usage: ./deploy.sh gsi"; }

# check environment (1)
if [[ -z ${E2T_ROOT_DIR:-} ]]; then echo "error: missing env. var. E2T_ROOT_DIR"; exit 1; fi

# command-line arguments
if [[ $# -ne 1 ]]; then print_usage; exit 1; fi
server=$1
[[ " gsi " == *" ${server:-} "* ]] || { echo "error: invalid server"; exit 1; }

# check environment (2) based on server opt
server_upper=${server^^} # uppercase
host_var="${server_upper}_USER_AND_HOST"
path_var="E2T_AT_${server_upper}_PATH"
[[ -z ${!host_var:-} ]] && { echo "error: missing env. var. ${host_var}"; exit 1; }
[[ -z ${!path_var:-} ]] && { echo "error: missing env. var. ${path_var}"; exit 1; }
e2t_remote_user_and_host=${!host_var}
e2t_remote_path=${!path_var}

# ensure remote directories exists
ssh "${e2t_remote_user_and_host}" mkdir -p "${e2t_remote_path}"

rsync -avzR --delete \
      "${E2T_ROOT_DIR}"/./run_lists/ \
      "${E2T_ROOT_DIR}"/./xml/ \
      "${E2T_ROOT_DIR}"/./scripts/scrontab-gsi/ \
      "${E2T_ROOT_DIR}"/./task/RunTask.C \
      "${E2T_ROOT_DIR}"/./task/AliTaskEsd2Tree.h \
      "${E2T_ROOT_DIR}"/./task/AliTaskEsd2Tree.cxx \
      "${E2T_ROOT_DIR}"/./task/AliTaskEsd2Tree_LinkDef.h \
      "${E2T_ROOT_DIR}"/./task/AddTaskEsd2Tree.C \
      "${E2T_ROOT_DIR}"/./common/Schema_Events.hpp \
      "${E2T_ROOT_DIR}"/./common/POD_Track.hpp \
      "${E2T_ROOT_DIR}"/./common/POD_PreFoundLambda.hpp \
      "${E2T_ROOT_DIR}"/./common/POD_McParticle.hpp \
      "${E2T_ROOT_DIR}"/./common/POD_InjectedSexa.hpp \
      "${E2T_ROOT_DIR}"/./common/POD_Event.hpp \
      "${E2T_ROOT_DIR}"/./common/Math.hpp \
      "${E2T_ROOT_DIR}"/./common/Framework_TeeTree.hpp \
      "${E2T_ROOT_DIR}"/./common/Cuts_E2T.hpp \
      "${E2T_ROOT_DIR}"/./common/Constants.hpp \
      "${E2T_ROOT_DIR}"/./tidentity/macros/AddTask_marsland_TIdentityPID.C \
      "${E2T_ROOT_DIR}"/./tidentity/macros/AliAnalysisTaskTIdentityPID.cxx \
      "${E2T_ROOT_DIR}"/./tidentity/macros/AliAnalysisTaskTIdentityPID.h \
      "${E2T_ROOT_DIR}"/./tidentity/macros/Config_marsland_TIdentityPID.C \
      "${E2T_ROOT_DIR}"/./tidentity/macros/AddTaskFilteredTreeLocal.C \
      "${e2t_remote_user_and_host}":"${e2t_remote_path}"/

# create marker file
ssh "${e2t_remote_user_and_host}" touch "${e2t_remote_path}/CONTENT-DEPLOYED"
