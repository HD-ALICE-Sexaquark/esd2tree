#!/bin/bash

if [[ -z ${E2V_ROOT_DIR} ]]; then echo "missing E2V_ROOT_DIR"; exit 1; fi
if [[ -z ${E2V_SLURM_DIR} ]]; then echo "missing E2V_SLURM_DIR"; exit 1; fi
if [[ -z ${LUSTRE_HOME} ]]; then echo "missing LUSTRE_HOME"; exit 1; fi
if [[ -z ${JALIEN_TOKEN_CERT} ]]; then echo "missing JALIEN_TOKEN_CERT"; exit 1; fi
if [[ -z ${JALIEN_TOKEN_KEY} ]]; then echo "missing JALIEN_TOKEN_KEY"; exit 1; fi

if [[ $# -ne 1 ]]; then echo "usage: ./submit_batch_data.sh [LHC15o,LHC18q,LHC18r]"; exit 1; fi

# hardcoded options
export MODE="grid"
export PRODUCTION_NAME="$1"
export GRID_TEST_MODE=1

# determine year
export YEAR_2DIG=${PRODUCTION_NAME:2:2}

# determine pass number
export PASS_NUMBER=2
if [[ YEAR_2DIG -eq 18 ]]; then PASS_NUMBER=3; fi

# find rn file
run_numbers_file=${E2V_ROOT_DIR}/doc/${PRODUCTION_NAME}_pass${PASS_NUMBER}_rn.txt

# define strings (NOTE: not arrays, because Slurm)
export UNROLLED_DN=""
export UNROLLED_N_DIRS=""

while read -r rn; do

    n_total_jobs=0

    # find dn file
    dir_numbers_file=${E2V_ROOT_DIR}/doc/dir_numbers/${PRODUCTION_NAME}/${rn}.txt

    while read -r line; do
        dn=${line% *}
        n_dirs=${line#* }

        UNROLLED_DN+="${dn} "
        UNROLLED_N_DIRS+="${n_dirs} "
        n_total_jobs=$((n_total_jobs + 1))
    done < ${dir_numbers_file}

    export RUN_NUMBER=${rn}
    echo "submit_batch_data @ farm-gsi :: submitting ${n_total_jobs} jobs for rn #${RUN_NUMBER}"
    array_max=$((n_total_jobs - 1))
    # sbatch \
    #    --singularity-container=${LUSTRE_HOME}/containers/aliroot+aliphysics.sif \
    #    --output=${E2V_SLURM_DIR}/${PRODUCTION_NAME}_${rn}_%a.log \
    #    --array=0-${array_max} \
    #    submit_single_data.sh ${rn}

done < ${run_numbers_file}
