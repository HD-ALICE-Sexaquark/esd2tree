#!/bin/bash

# IMPORTANT: this file should always be sourced by another script

if [[ -z ${LOCAL_DATA_DIR} ]]; then echo "ERROR :: config_file.sh :: Missing env. var. LOCAL_DATA_DIR" ; exit 1; fi
if [[ -z ${ANALYSIS_DIR} ]]; then echo "ERROR :: config_file.sh :: Missing env. var. ANALYSIS_DIR" ; exit 1; fi

export ATTEMPT_NAME="local_data_18q_test"
export MODE="local"
export INPUT_PATH="${LOCAL_DATA_DIR}/2018"
export LOCAL_N_DIRS=13
export GRID_TEST_MODE=0
export IS_MC=0
export PRODUCTION_NAME="LHC18q"
export RUN_NUMBERS_LIST="${ANALYSIS_DIR}/doc/LHC18q_pass3_rn_TEST.txt"
export CHOOSE_N_EVENTS=100
