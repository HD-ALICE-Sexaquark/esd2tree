#!/bin/bash

# `esd2tree/scripts/farm-pi/slurm_wrapper.sh`
# ===========================================
# Send jobs to the Slurm farm, one per run number, for one output tag of the dedicated MC: LHC26h2 (h-dibaryons) or
# LHC23l1ab4 (antisexaquark reactions, one channel per output tag). Runs whose output already exists are skipped, so the
# same command also resubmits the failed ones.
#
# Expected input layout, as copied from GSI by `fetch_sims.sh`:
#   LOCAL_SIMS_DIR / <output_tag> / <run number> / <run number>_<dir number>.tar
#
# Output:
#   E2T_OUTPUT_DIR / local_mc_<short_tag> / <variant> / AnalysisResults_<run number>.root
#
# where <output_tag> is `LHC26h2/<variant>` or `LHC23l1ab4_<A|D|H>/<variant>`, <variant> is `signal` or `embedded`, and
# <short_tag> is what comes before the variant, without `LHC` (e.g. `26h2`, `23l1ab4_A`)

set -euo pipefail
shopt -s nullglob

# hardcoded options
# NOTE: always "local" mode, as `RunTask.C` is called by `slurm_exec.sh`; the GRID is `scripts/looper/`'s
export MAX_N_DN_DIRS=25 # max number of <dir number> dirs to add; = all of them, as produced by `farm-gsi/submit.sh`
max_parallel_jobs=48

print_usage() {
    echo "usage: ./slurm_wrapper.sh <output_tag> [max_n_runs]"
    echo "where: output_tag   : LHC26h2/<variant> or LHC23l1ab4_<A|D|H>/<variant>, with <variant> = signal, embedded"
    echo "       [max_n_runs] : (optional; default = 0) max TOTAL jobs (= run numbers) to submit; zero means no limit"
    echo "examples: ./slurm_wrapper.sh LHC26h2/signal"
    echo "          ./slurm_wrapper.sh LHC23l1ab4_A/embedded 1"
}

# check environment
if [[ -z ${LOCAL_SIMS_DIR:-} ]]; then echo "error: missing env. var. LOCAL_SIMS_DIR"; exit 1; fi
if [[ -z ${E2T_ROOT_DIR:-} ]]; then echo "error: missing env. var. E2T_ROOT_DIR"; exit 1; fi
export E2T_OUTPUT_DIR=${E2T_OUTPUT_DIR:-${E2T_ROOT_DIR}/output}
export E2T_SLURM_DIR=${E2T_SLURM_DIR:-${E2T_ROOT_DIR}/slurm}

# command-line arguments
if [[ $# -lt 1 ]]; then print_usage; exit 1; fi
# >> derive output tag, production and variant
export OUTPUT_TAG="$1"
[[ ${OUTPUT_TAG} =~ ^(LHC26h2|LHC23l1ab4_[ADH])/(signal|embedded)$ ]] || { echo "error: invalid output tag"; print_usage; exit 1; }
export PRODUCTION_NAME=${OUTPUT_TAG%%[_/]*} # e.g. LHC23l1ab4_A/signal -> LHC23l1ab4
export VARIANT=${OUTPUT_TAG#*/}             # e.g. LHC23l1ab4_A/signal -> signal
# >> derive max run number dirs to analyze
max_n_runs=${2:-0} # 0 = no limit

# define output dir
short_tag=${OUTPUT_TAG%/*} # e.g. LHC23l1ab4_A/signal -> LHC23l1ab4_A
export OUTPUT_DIR=${E2T_OUTPUT_DIR}/local_mc_${short_tag#LHC}/${VARIANT}

# define arrays
# -- skip the run numbers already done
run_numbers_arr=()
n_skipped=0
for rn_dir in "${LOCAL_SIMS_DIR}/${OUTPUT_TAG}"/*/; do
    run_number=$(basename "${rn_dir}")
    if [[ -e ${OUTPUT_DIR}/AnalysisResults_${run_number}.root ]]; then
        n_skipped=$((n_skipped + 1))
        continue
    fi
    run_numbers_arr+=("${run_number}")
done

# truncate run number jobs
if (( max_n_runs > 0 && ${#run_numbers_arr[@]} > max_n_runs )); then
    run_numbers_arr=("${run_numbers_arr[@]:0:max_n_runs}")
fi

# count the run numbers with missing tars (still being produced or copied, or failed at GSI)
# -- NOTE: once its output exists, a run number is never submitted again, so these tars wouldn't be read
n_incomplete=0
for run_number in "${run_numbers_arr[@]}"; do
    tar_files=("${LOCAL_SIMS_DIR}/${OUTPUT_TAG}/${run_number}/${run_number}"_*.tar)
    if (( ${#tar_files[@]} < MAX_N_DN_DIRS )); then n_incomplete=$((n_incomplete + 1)); fi
done

# join arrays into strings before exporting, because Slurm
export RUN_NUMBERS_STR="${run_numbers_arr[*]}"

n_total_jobs=${#run_numbers_arr[@]}
if (( n_total_jobs == 0 && n_skipped == 0 )); then
    echo "error: no run number directories found in ${LOCAL_SIMS_DIR}/${OUTPUT_TAG}"
    exit 1
elif (( n_total_jobs == 0 )); then
    echo "$0 @ ${HOSTNAME} :: nothing to submit, the ${n_skipped} run numbers of ${OUTPUT_TAG} are done"
    exit 0
fi
array_max=$((n_total_jobs - 1))

mkdir -p "${E2T_SLURM_DIR}/tmp" "${OUTPUT_DIR}"

sbatch \
    --output="${E2T_SLURM_DIR}/tmp/%A_%a.log" \
    --job-name="e2t_${OUTPUT_TAG//\//_}" \
    --array="0-${array_max}%${max_parallel_jobs}" \
    -- "${E2T_ROOT_DIR}/scripts/farm-pi/slurm_exec.sh"

echo "$0 @ ${HOSTNAME} :: a total of ${n_total_jobs} jobs of ${OUTPUT_TAG} have been submitted (${n_skipped} run numbers already done)"
if (( n_incomplete > 0 )); then
    echo "$0 @ ${HOSTNAME} :: warning: ${n_incomplete} of them have fewer than ${MAX_N_DN_DIRS} tars; to redo one later, delete its output"
fi
