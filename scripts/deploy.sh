#!/bin/bash

# `esd2tree_hdib/scripts/deploy.sh` -- deploy files to PI server

set -euo pipefail

# check environment
[[ -z ${E2TH_ROOT_DIR:-} ]] && { echo "error: missing env. var. E2TH_ROOT_DIR"; exit 1; }
[[ -z ${PI_USER_AND_HOST:-} ]] && { echo "error: missing env. var. PI_USER_AND_HOST"; exit 1; }
[[ -z ${E2TH_AT_PI_PATH:-} ]] && { echo "error: missing env. var. E2TH_AT_PI_PATH"; exit 1; }

# ensure remote directories exists
ssh "${PI_USER_AND_HOST}" mkdir -p "${E2TH_AT_PI_PATH}"

rsync -avzR --delete \
      "${E2TH_ROOT_DIR}"/./run_lists/ \
      "${E2TH_ROOT_DIR}"/./scripts/farm-pi/ \
      "${E2TH_ROOT_DIR}"/./task/RunTask.C \
      "${E2TH_ROOT_DIR}"/./task/AliTaskEsd2Tree.h \
      "${E2TH_ROOT_DIR}"/./task/AliTaskEsd2Tree.cxx \
      "${E2TH_ROOT_DIR}"/./task/AliTaskEsd2Tree_LinkDef.h \
      "${E2TH_ROOT_DIR}"/./task/AddTaskEsd2Tree.C \
      "${E2TH_ROOT_DIR}"/./common/Schema_Events.hpp \
      "${E2TH_ROOT_DIR}"/./common/POD_Track.hpp \
      "${E2TH_ROOT_DIR}"/./common/POD_PreFoundLambda.hpp \
      "${E2TH_ROOT_DIR}"/./common/POD_McParticle.hpp \
      "${E2TH_ROOT_DIR}"/./common/POD_InjectedSexa.hpp \
      "${E2TH_ROOT_DIR}"/./common/POD_Event.hpp \
      "${E2TH_ROOT_DIR}"/./common/Math.hpp \
      "${E2TH_ROOT_DIR}"/./common/Framework_TeeTree.hpp \
      "${E2TH_ROOT_DIR}"/./common/Cuts_E2T.hpp \
      "${E2TH_ROOT_DIR}"/./common/Constants.hpp \
      "${PI_USER_AND_HOST}":"${E2TH_AT_PI_PATH}"/

# create marker file
ssh "${PI_USER_AND_HOST}" touch "${E2TH_AT_PI_PATH}/CONTENT-DEPLOYED"
