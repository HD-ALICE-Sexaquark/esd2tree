#!/bin/bash

# `esd2vector/scripts/farm-pi/slurm_wrapper.sh`
# =============================================
# Send jobs to the Slurm farm. Exclusive for MC.
#
# Expected input layout:
#   LOCAL_SIMS_DIR / LHC23l1a3 / <channel><mass> / <run number> / <dir number> / <files>.root
#   LOCAL_SIMS_DIR / LHC23l1b3 / <channel><mass> / <run number> / <dir number> / <files>.root
#   LOCAL_SIMS_DIR / LHC26h    / signal          / <run number> / <dir number> / <files>.root
#
# Output for LHC23l1a3+LHC23l1b3:
#   E2T_ROOT_DIR / output / local_mc_<short prod name>_<channel><mass> / AnalysisResults_<run number>.root
#
# Output for LHC26h:
#   E2T_ROOT_DIR / output / local_mc_<short prod name> / AnalysisResults_<run number>.root

set -euo pipefail
shopt -s nullglob

# hardcoded options
export MODE="local"
export MAX_N_DN_DIRS=25 # max number of <dir number> dirs to add; gets overriden for LHC26h
max_parallel_jobs=48

print_usage() {
    echo "usage: ./slurm_wrapper.sh <PRODUCTION NAME> <CHANNELS> <MASSES> [max_n_rn_dirs]"
    echo "where: PRODUCTION NAME : LHC23l1a3, LHC23l1b3, LHC26h"
    echo "       CHANNELS        : comma-separated reaction channels (e.g. \"A,D\"), or \"\" for none"
    echo "                         required non-empty for LHC23l1a3 and LHC23l1b3"
    echo "       MASSES          : comma-separated injected masses (e.g. \"1.73,1.8\"), or \"\" for none"
    echo "                         required non-empty for LHC23l1a3 and LHC23l1b3"
    echo "       [max_n_rn_dirs] : (optional; default = 0) max TOTAL jobs to submit across all channel/mass pairs; zero means no limit"
    echo "examples: ./slurm_wrapper.sh LHC23l1a3 A,D,H 1.73"
    echo "          ./slurm_wrapper.sh LHC26h \"\" \"\" 25"
}

# check environment
if [[ -z ${LOCAL_SIMS_DIR:-} ]]; then echo "error: missing env. var. LOCAL_SIMS_DIR"; exit 1; fi
if [[ -z ${E2T_ROOT_DIR:-} ]]; then echo "error: missing env. var. E2T_ROOT_DIR"; exit 1; fi

# command-line arguments
if [[ $# -lt 3 ]]; then print_usage; exit 1; fi
# >> derive production name
export PRODUCTION_NAME="$1"
# >> derive reaction channel + injected masses pairs; derive production settings
reaction_channels=()
injected_masses=()
input_path="${LOCAL_SIMS_DIR}/${PRODUCTION_NAME}"
if [[ ${PRODUCTION_NAME} =~ ^LHC23l1[ab]3 ]]; then
    IFS=',' read -ra reaction_channels <<< "$2"
    IFS=',' read -ra injected_masses <<< "$3"
    if [[ ${#reaction_channels[@]} -eq 0 || ${#injected_masses[@]} -eq 0 ]]; then
        print_usage
        exit 1
    fi
elif [[ "${PRODUCTION_NAME}" == "LHC26h" ]]; then
    MAX_N_DN_DIRS=50
else
    print_usage
    exit 1
fi
# >> derive max run number dirs to analyze
max_n_rn_dirs=${4:-0}  # 0 = no limit

# define arrays
channels_arr=()
masses_arr=()
run_numbers_arr=()

if [[ ${PRODUCTION_NAME} != "LHC26h" ]]; then
    for r_channel in "${reaction_channels[@]}"; do
        for s_mass in "${injected_masses[@]}"; do
            for rn_dir in "${input_path}/${r_channel}${s_mass}"/*/; do
                channels_arr+=("${r_channel}")
                masses_arr+=("${s_mass}")
                run_numbers_arr+=("$(basename "${rn_dir}")")
            done
        done
    done
else
    for rn_dir in "${input_path}/signal"/*/; do
        run_numbers_arr+=("$(basename "${rn_dir}")")
    done
fi

# truncate run number jobs
if (( max_n_rn_dirs > 0 && ${#run_numbers_arr[@]} > max_n_rn_dirs )); then
    run_numbers_arr=("${run_numbers_arr[@]:0:max_n_rn_dirs}")
    if [[ ${PRODUCTION_NAME} != "LHC26h" ]]; then
        channels_arr=("${channels_arr[@]:0:max_n_rn_dirs}")
        masses_arr=("${masses_arr[@]:0:max_n_rn_dirs}")
    fi
fi

# join arrays into strings before exporting, because Slurm
export CHANNELS_STR="${channels_arr[*]}"
export MASSES_STR="${masses_arr[*]}"
export RUN_NUMBERS_STR="${run_numbers_arr[*]}"

n_total_jobs=${#run_numbers_arr[@]}
if (( n_total_jobs == 0 )); then
    echo "error: no run number directories found"
    exit 1
fi
array_max=$((n_total_jobs - 1))

mkdir -p "${E2T_ROOT_DIR}/slurm/tmp"

sbatch \
    --output="${E2T_ROOT_DIR}/slurm/tmp/%A_%a.log" \
    --array="0-${array_max}%${max_parallel_jobs}" \
    -- "${E2T_ROOT_DIR}/scripts/farm-pi/slurm_exec.sh"

echo "$0 @ ${HOSTNAME} :: a total of ${n_total_jobs} jobs have been submitted"
