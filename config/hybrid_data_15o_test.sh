#!/bin/bash

# IMPORTANT: this file should always be sourced by another script

if [[ -z ${ANALYSIS_DIR} ]]; then echo "ERROR :: config_file.sh :: Missing env. var. ANALYSIS_DIR" ; exit 1; fi

export ATTEMPT_NAME="hybrid_data_15o_test"
export MODE="hybrid"
export INPUT_PATH="/alice/data/2015"
export LOCAL_N_DIRS=1
export GRID_TEST_MODE=0
export IS_MC=0
export PRODUCTION_NAME="LHC15o"
export RUN_NUMBERS_LIST="${ANALYSIS_DIR}/doc/LHC15o_pass2_rn_TEST2.txt"
export SIMULATION_SET=""
export CHOOSE_N_EVENTS=0
