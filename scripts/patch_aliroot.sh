#!/bin/bash

# `patch_aliroot.sh` -- quick utility to patch AliRoot, before it's built

set -euo pipefail

if [[ -z ${E2R_ROOT_DIR:-} ]]; then echo "error: missing env. var. E2R_ROOT_DIR"; exit 1; fi

if [[ $# -ne 1 ]]; then echo "usage: $0 <path aliroot>"; exit 1; fi
aliroot_path=$1

cp -v "${E2R_ROOT_DIR}/scripts/AliAnalysisAlien_patch.cxx" "${aliroot_path}/ANALYSIS/ANALYSISaliceBase/AliAnalysisAlien.cxx"
