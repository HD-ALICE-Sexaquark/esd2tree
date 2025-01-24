#!/bin/bash

# IMPORTANT: this file should always be sourced by another script

if [[ -z ${SIMULATION_SET} ]]; then echo "ERROR :: config_file.sh :: Missing env. var. SIMULATION_SET" ; exit 1; fi
if [[ -z ${LOCAL_SIMS_DIR} ]]; then echo "ERROR :: config_file.sh :: Missing env. var. LOCAL_SIMS_DIR" ; exit 1; fi
if [[ -z ${ANALYSIS_DIR} ]]; then echo "ERROR :: config_file.sh :: Missing env. var. ANALYSIS_DIR" ; exit 1; fi

export ATTEMPT_NAME="local_signalMC_${SIMULATION_SET}_15o_full"
export MODE="local"
export INPUT_PATH="${LOCAL_SIMS_DIR}"
export LOCAL_N_DIRS=6
export GRID_TEST_MODE=0
export IS_MC=1
export PRODUCTION_NAME="LHC23l1b3"
export RUN_NUMBERS_LIST="${ANALYSIS_DIR}/doc/LHC15o_pass2_rn.txt"
export CHOOSE_N_EVENTS=0
