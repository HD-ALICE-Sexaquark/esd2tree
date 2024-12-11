#!/bin/bash

# IMPORTANT: this file should always be sourced by another script

if [[ -z ${SIMS_DIR} ]]; then echo "ERROR :: config_file.sh :: Missing env. var. SIMS_DIR" ; exit 1; fi
if [[ -z ${ANALYSIS_DIR} ]]; then echo "ERROR :: config_file.sh :: Missing env. var. ANALYSIS_DIR" ; exit 1; fi

export ATTEMPT_NAME="local_bkgMC_18qr_test"
export MODE="local"
export LOCAL_INPUT_PATH="${SIMS_DIR}"
export LOCAL_N_DIRS=20
export GRID_TEST_MODE=0
export IS_MC=1
export PRODUCTION_NAME="LHC20e3a"
export RUN_NUMBERS_LIST="${ANALYSIS_DIR}/doc/LHC18qr_pass3_rn_TEST.txt"
export CHOOSE_N_EVENTS=0
