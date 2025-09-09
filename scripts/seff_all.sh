#!/bin/bash

# Print a summary of all the slurm efficiency reports obtained via `seff ${JOBID}` #
# This script is meant to be run after all the jobs have finished                  #
# It will print out the following:                                                 #
# - number of jobs that completed, failed, or timed out                            #
# - the maximum and average memory and time used by the jobs                       #

function convert_to_mb() {
    local size=$1
    echo $size | numfmt --from=iec --to-unit=1048576
}

function convert_time_to_seconds() {
    local time=$1
    echo $time | awk -F: '{ print ($1 * 3600) + ($2 * 60) + $3 }'
}

function convert_seconds_to_human_readable() {
    local seconds=$1
    printf '%02d:%02d:%02d' $((seconds/3600)) $((seconds%3600/60)) $((seconds%60))
}

N_COMPLETED=0
N_FAILED=0
N_TIMEOUT=0

MAX_MEM=0
AVG_MEM=0
MAX_TIME=0
AVG_TIME=0

if [[ -z ${E2V_SLURM_DIR} ]]; then echo "missing E2V_SLURM_DIR"; exit 1; fi
JOBIDS=()
mapfile -t JOBIDS < <(basename -s .out ${E2V_SLURM_DIR}/*.out)

for ((i=0; i<${#JOBIDS[@]}; i++)); do

    # progress bar
    if [[ $((i + 1)) -lt ${#JOBIDS[@]} ]]; then
        echo -ne "Processing $((i + 1)) / ${#JOBIDS[@]}\r"
    else
        echo "Processing $((i + 1)) / ${#JOBIDS[@]}"
    fi

    JOBID=${JOBIDS[$i]}
    SEFF_OUT=$(seff ${JOBID})

    # state
    STATE=$(echo ${SEFF_OUT} | grep -oP 'State: \K\w+')
    if [[ ${STATE} == "COMPLETED" ]]; then
        N_COMPLETED=$((N_COMPLETED + 1))
    elif [[ ${STATE} == "FAILED" ]]; then
        if [[ ${N_FAILED} -eq 0 ]]; then
            echo "${JOBID}" > failed_jobs.txt
        else
            echo "${JOBID}" >> failed_jobs.txt
        fi
        N_FAILED=$((N_FAILED + 1))
    elif [[ ${STATE} == "TIMEOUT" ]]; then
        N_TIMEOUT=$((N_TIMEOUT + 1))
        if [[ ${N_TIMEOUT} -eq 0 ]]; then
            echo "${JOBID}" > timeout_jobs.txt
        else
            echo "${JOBID}" >> timeout_jobs.txt
        fi
    fi

    # memory
    JOB_MEM=$(echo ${SEFF_OUT} | grep -oP 'Memory Utilized: \K[0-9.]+ [A-Z]+' | sed 's/ //' | sed 's/B//')
    JOB_MEM=$(convert_to_mb $JOB_MEM)
    if [[ ${JOB_MEM} -gt ${MAX_MEM} ]]; then
        MAX_MEM=${JOB_MEM}
    fi
    AVG_MEM=$((AVG_MEM + JOB_MEM))

    # time
    JOB_TIME=$(echo ${SEFF_OUT} | grep -oP 'Job Wall-clock time: \K[0-9:]+')
    JOB_TIME=$(convert_time_to_seconds $JOB_TIME)
    if [[ ${JOB_TIME} -gt ${MAX_TIME} ]]; then
        MAX_TIME=${JOB_TIME}
    fi
    AVG_TIME=$((AVG_TIME + JOB_TIME))
done

AVG_MEM=$((AVG_MEM / ${#JOBIDS[@]}))
AVG_TIME=$((AVG_TIME / ${#JOBIDS[@]}))

echo ""
echo "Summary of All Slurm Efficiency Reports"
echo "======================================="
echo ">> N Total     = ${#JOBIDS[@]}"
echo ">> N Completed = ${N_COMPLETED}"
echo ">> N Failed    = ${N_FAILED}"
echo ">> N Timeout   = ${N_TIMEOUT}"
echo ">> Max Memory  = ${MAX_MEM} MB"
echo ">> Avg Memory  = ${AVG_MEM} MB"
echo ">> Max Time    = $(convert_seconds_to_human_readable ${MAX_TIME})"
echo ">> Avg Time    = $(convert_seconds_to_human_readable ${AVG_TIME})"
echo ""
