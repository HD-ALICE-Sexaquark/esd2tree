#!/bin/bash

# `esd2tree_hdib/scripts/farm-pi/slurm_wrapper.sh`
# ================================================
# Send jobs to the Slurm farm, one per run number. Exclusive for the (anti)h-dibaryon MC, LHC26h2.
#
# Expected input layout:
#   LOCAL_SIMS_DIR / LHC26h2 / <variant> / <run number> / <dir number> / <files>.root
#
# Output:
#   E2T_ROOT_DIR / output / local_mc_26h2 / <variant> / AnalysisResults_<run number>.root
#
# where <variant> is `signal` or `embedded`

set -euo pipefail
shopt -s nullglob

# hardcoded options
export MODE="local"
export PRODUCTION_NAME="LHC26h2"
export MAX_N_DN_DIRS=60 # max number of <dir number> dirs to add; = all of them, as produced by `hdibaryon_submit.sh`
max_parallel_jobs=48

print_usage() {
    echo "usage: ./slurm_wrapper.sh <VARIANT> [max_n_rn_dirs]"
    echo "where: VARIANT         : signal, embedded"
    echo "       [max_n_rn_dirs] : (optional; default = 0) max TOTAL jobs (= run numbers) to submit; zero means no limit"
    echo "examples: ./slurm_wrapper.sh signal"
    echo "          ./slurm_wrapper.sh embedded 25"
}

# check environment
if [[ -z ${LOCAL_SIMS_DIR:-} ]]; then echo "error: missing env. var. LOCAL_SIMS_DIR"; exit 1; fi
if [[ -z ${E2T_ROOT_DIR:-} ]]; then echo "error: missing env. var. E2T_ROOT_DIR"; exit 1; fi

# command-line arguments
if [[ $# -lt 1 ]]; then print_usage; exit 1; fi
# >> derive variant
export VARIANT="$1"
[[ " signal embedded " == *" ${VARIANT} "* ]] || { echo "error: invalid variant"; print_usage; exit 1; }
# >> derive max run number dirs to analyze
max_n_rn_dirs=${2:-0}  # 0 = no limit

# define arrays
run_numbers_arr=()
for rn_dir in "${LOCAL_SIMS_DIR}/${PRODUCTION_NAME}/${VARIANT}"/*/; do
    run_numbers_arr+=("$(basename "${rn_dir}")")
done

# truncate run number jobs
if (( max_n_rn_dirs > 0 && ${#run_numbers_arr[@]} > max_n_rn_dirs )); then
    run_numbers_arr=("${run_numbers_arr[@]:0:max_n_rn_dirs}")
fi

# join arrays into strings before exporting, because Slurm
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
