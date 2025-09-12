#!/bin/bash

if [[ -z ${E2V_ROOT_DIR} ]]; then echo "missing E2V_ROOT_DIR"; exit 1; fi
if [[ -z ${E2V_OUTPUT_DIR} ]]; then echo "missing E2V_OUTPUT_DIR"; exit 1; fi
mkdir -p "${E2V_OUTPUT_DIR}"
if [[ -z ${E2V_SLURM_DIR} ]]; then echo "missing E2V_SLURM_DIR"; exit 1; fi
mkdir -p "${E2V_SLURM_DIR}"
if [[ -z ${LUSTRE_HOME} ]]; then echo "missing LUSTRE_HOME"; exit 1; fi
if [[ -z ${JALIEN_TOKEN_CERT} ]]; then echo "missing JALIEN_TOKEN_CERT"; exit 1; fi
if [[ -z ${JALIEN_TOKEN_KEY} ]]; then echo "missing JALIEN_TOKEN_KEY"; exit 1; fi

if [[ $# -ne 1 ]]; then echo "usage: ./submit_batch_data.sh [LHC15o,LHC18q,LHC18r]"; exit 1; fi

# hardcoded options
export MODE="grid"
export PRODUCTION_NAME="$1"
export GRID_TEST_MODE=1
export MAX_PARALLEL_JOBS=500

# determine year
export YEAR_2DIG=${PRODUCTION_NAME:3:2}

# determine pass number
export PASS_NUMBER=2
if [[ ${YEAR_2DIG} -eq 18 ]]; then PASS_NUMBER=3; fi

# find rn file
run_numbers_file=${E2V_ROOT_DIR}/doc/${PRODUCTION_NAME}_pass${PASS_NUMBER}_rn.txt

# define strings (NOTE: not arrays, because Slurm)

export UNROLLED_RUNS=""
export UNROLLED_DN=""
export UNROLLED_N_DIRS=""
n_total_jobs=0

# loop over run numbers
while read -r line1; do

    run_number=${line1}
    n_jobs_per_rn=0 # debug

    # find dn file
    dir_numbers_file=${E2V_ROOT_DIR}/doc/dir_numbers/${PRODUCTION_NAME}/${run_number}.txt

    while read -r line2; do
        dn=${line2% *}
        n_dirs=${line2#* }

        # skip finished dn
        if [[ -e ${E2V_OUTPUT_DIR}/${PRODUCTION_NAME}/${run_number}/AnalysisResults_${dn}.root ]]; then continue; fi

        UNROLLED_RUNS+="${run_number} "
        UNROLLED_DN+="${dn} "
        UNROLLED_N_DIRS+="${n_dirs} "

        n_total_jobs=$((n_total_jobs + 1))
        n_jobs_per_rn=$((n_jobs_per_rn + 1)) # debug
    done < "${dir_numbers_file}"

    echo "submit_batch_data @ farm-gsi :: RN ${run_number} = ${n_jobs_per_rn} remaining jobs" # debug
done < "${run_numbers_file}"

array_max=$((n_total_jobs - 1))
mkdir -p "${E2V_SLURM_DIR}/tmp"

if [[ ${n_total_jobs} -gt 0 ]]; then
    sbatch \
        --singularity-container="${LUSTRE_HOME}/containers/aliroot+aliphysics.sif" \
        --output="${E2V_SLURM_DIR}/tmp/%A_%a.log" \
        --array="0-${array_max}%${MAX_PARALLEL_JOBS}" \
        -- submit_single_data.sh
fi

echo "submit_batch_data @ farm-gsi :: a total of ${n_total_jobs} jobs have been submitted"
