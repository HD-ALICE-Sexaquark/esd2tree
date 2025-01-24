#!/bin/bash

# IMPORTANT: this file should always be sourced by another script

if [[ -z ${ANALYSIS_DIR} ]]; then echo "ERROR :: config_file.sh :: Missing env. var. ANALYSIS_DIR" ; exit 1; fi

export ATTEMPT_NAME="grid_data_18r"
export MODE="grid"
export INPUT_PATH="/alice/data/2018"
export LOCAL_N_DIRS=1
export GRID_TEST_MODE=0
export IS_MC=0
export PRODUCTION_NAME="LHC18r"
export RUN_NUMBERS_LIST="${ANALYSIS_DIR}/doc/LHC18r_pass3_rn_GRID.txt"
export SIMULATION_SET=""
export CHOOSE_N_EVENTS=0
