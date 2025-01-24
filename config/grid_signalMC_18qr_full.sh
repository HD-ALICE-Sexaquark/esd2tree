#!/bin/bash

# IMPORTANT: this file should always be sourced by another script

if [[ -z ${SIMULATION_SET} ]]; then echo "ERROR :: config_file.sh :: Missing env. var. SIMULATION_SET" ; exit 1; fi
if [[ -z ${ANALYSIS_DIR} ]]; then echo "ERROR :: config_file.sh :: Missing env. var. ANALYSIS_DIR" ; exit 1; fi

export ATTEMPT_NAME="grid_signalMC_${SIMULATION_SET}_18qr_full"
export MODE="grid"
export INPUT_PATH=""
export LOCAL_N_DIRS=0
export GRID_TEST_MODE=0
export IS_MC=1
export PRODUCTION_NAME="LHC23l1a3"
export RUN_NUMBERS_LIST="${ANALYSIS_DIR}/doc/LHC18qr_pass3_rn.txt"
export CHOOSE_N_EVENTS=0
