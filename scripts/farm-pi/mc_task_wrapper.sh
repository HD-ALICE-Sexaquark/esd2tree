#!/bin/bash

# `esd2vector/scripts/farm-pi/mc_task_wrapper.sh` -- Send MC jobs to the Slurm farm.

set -euo pipefail
shopt -s nullglob

# hardcoded options #

export MODE="local"
export LOCAL_N_DIRS=6 # gets overriden for 26h
MAX_PARALLEL_JOBS=45
# reaction_channels=("A") # "A" "D" "H"
# injected_masses=(1.8) # (1.73 1.8 1.87 1.94 2.01)
rmasses_pairs=("A1.73" "A2.01" "D1.8" "H1.8")

print_usage() {
    echo "usage: ./mc_task_wrapper.sh <production_name> [max_rn]";
    echo "       where:";
    echo "       <production_name> : LHC23l1a3, LHC23l1b3, LHC26h";
    echo "       [max_rn]          : if 0 = no limit";
}

# check environment
if [[ -z ${LOCAL_SIMS_DIR:-} ]]; then echo "error: missing env. var. LOCAL_SIMS_DIR"; exit 1; fi
if [[ -z ${E2T_ROOT_DIR:-} ]]; then echo "error: missing env. var. E2T_ROOT_DIR"; exit 1; fi

# command-line arguments
if [[ $# -lt 1 || $# -gt 2 ]]; then print_usage; exit 1; fi
export PRODUCTION_NAME="$1"
MAX_RN=${2:-0}  # 0 = no limit

# validate input args
if [[ "${PRODUCTION_NAME}" != "LHC23l1a3" && "${PRODUCTION_NAME}" != "LHC23l1b3" && "${PRODUCTION_NAME}" != "LHC26h" ]]; then print_usage; exit 1; fi
if [[ "${PRODUCTION_NAME}" == "LHC26h" ]]; then
    LOCAL_N_DIRS=50
fi

# array accumulation
channels=()
masses=()
run_numbers=()

input_path=${LOCAL_SIMS_DIR}/${PRODUCTION_NAME}

if [[ ${PRODUCTION_NAME} != "LHC26h" ]]; then
    for r_pair in "${rmasses_pairs[@]}"; do
    # for r_channel in "${reaction_channels[@]}"; do
    # for s_mass in "${injected_masses[@]}"; do
        for rn_dir in "${input_path}/${r_pair}"/*/; do
            channels+=("${r_pair:0:1}")
            masses+=("${r_pair:1:4}")
            run_numbers+=("$(basename "${rn_dir}")")
        done
    # done
    # done
    done
else
    for rn_dir in "${input_path}"/signal/*/; do
        run_numbers+=("$(basename "${rn_dir}")")
    done
fi

# truncate run number jobs
if (( MAX_RN > 0 && ${#run_numbers[@]} > MAX_RN )); then
    run_numbers=("${run_numbers[@]:0:MAX_RN}")
    if [[ ${PRODUCTION_NAME} != "LHC26h" ]]; then
        channels=("${channels[@]:0:MAX_RN}")
        masses=("${masses[@]:0:MAX_RN}")
    fi
fi

# join arrays into strings before exporting, because Slurm
export CHANNELS_STR="${channels[*]}"
export MASSES_STR="${masses[*]}"
export RUN_NUMBERS_STR="${run_numbers[*]}"

n_total_jobs=${#run_numbers[@]}
if (( n_total_jobs == 0 )); then
    echo "error: no run number directories found under ${input_path}"
    exit 1
fi
array_max=$((n_total_jobs - 1))

mkdir -p "${E2T_ROOT_DIR}/slurm/tmp"

sbatch \
    --output="${E2T_ROOT_DIR}/slurm/tmp/%A_%a.log" \
    --array="0-${array_max}%${MAX_PARALLEL_JOBS}" \
    -- "${E2T_ROOT_DIR}/scripts/farm-pi/mc_task_exec.sh"

echo "$0 @ ${HOSTNAME} :: a total of ${n_total_jobs} jobs have been submitted"
