#!/bin/bash

JALIEN_USERNAME=$(alien.py whoami)
if [[ -z ${ANALYSIS_DIR} ]]; then echo "mc/looper.sh(${JALIEN_USERNAME}):: ANALYSIS_DIR is not defined"; exit 1; fi
if [[ -z ${OUTPUT_DIR} ]]; then echo "mc/looper.sh(${JALIEN_USERNAME}):: OUTPUT_DIR is not defined"; exit 1; fi

LOOPER_DIR=${ANALYSIS_DIR}/scripts_mc/${JALIEN_USERNAME}
mkdir -p ${LOOPER_DIR}

### STAGE 0a: update analysis files ###

rm -v ${LOOPER_DIR}/AddTaskEsd2Tree.C
rm -v ${LOOPER_DIR}/AliAnalysisTaskEsd2Tree*
rm -v ${LOOPER_DIR}/*Analysis.C
rm -v ${LOOPER_DIR}/TaskEsd2Tree*

cp -v ${ANALYSIS_DIR}/task/AliAnalysisTaskEsd2Tree.cxx ${LOOPER_DIR}/
cp -v ${ANALYSIS_DIR}/task/AliAnalysisTaskEsd2Tree.h ${LOOPER_DIR}/
cp -v ${ANALYSIS_DIR}/task/AddTaskEsd2Tree.C ${LOOPER_DIR}/
cp -v ${ANALYSIS_DIR}/task/runAnalysis.C ${LOOPER_DIR}/

RUN_NUMBERS_LIST="${ANALYSIS_DIR}/scripts_mc/RN_${JALIEN_USERNAME}.txt"

while IFS= read -r LOOPER_RUN_NUMBER; do

    echo "mc/looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NUMBER}:: starting run number loop ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"

    ### STAGE 0b: determine production ###

    LOOPER_PRODUCTION_PATH=""
    if [[ -z ${LOOPER_PRODUCTION_PATH} ]]; then
        LOOPER_PRODUCTION_PATH=$(grep -q ${LOOPER_RUN_NUMBER} ${ANALYSIS_DIR}/doc/LHC15o_pass2_rn.txt && echo "2023/LHC23l1b3")
    fi
    if [[ -z ${LOOPER_PRODUCTION_PATH} ]]; then
        LOOPER_PRODUCTION_PATH=$(grep -q ${LOOPER_RUN_NUMBER} ${ANALYSIS_DIR}/doc/LHC18qr_pass3_rn.txt && echo "2023/LHC23l1a3")
    fi
    LOOPER_PRODUCTION_NAME=$(echo ${LOOPER_PRODUCTION_PATH} | awk -F/ '{print $2}')
    echo "mc/looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NUMBER}:: determined production ${LOOPER_PRODUCTION_NAME} ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"

    ### STAGE 0c: can we submit a job? ###

    LOOPER_JQUOTA=$(alien.py jquota list ${JALIEN_USERNAME})
    LOOPER_TotalRunningTimeLast24H=$(echo "${LOOPER_JQUOTA}" | grep "totalRunningTimeLast24h" | awk '{print $3}')
    if [[ ${LOOPER_TotalRunningTimeLast24H} -gt 8000000 ]]; then
        echo "mc/looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NUMBER}:: the total running time in the last 24 hours is ${LOOPER_TotalRunningTimeLast24H} > 8000000, I will not submit more jobs"
        echo "mc/looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NUMBER}:: NOTE: make sure to update the RN list before initiating looper again!!"
        break
    fi

    ### STAGE 1: submit jobs ###

    cd ${LOOPER_DIR}

    # for signal_channel in "A" "D" "E" "H"; do
    for signal_channel in "A" "D"; do
    for sexaquark_mass in "1.73" "1.8" "1.87" "1.94" "2.01"; do
        LOOPER_SIMSET="${signal_channel}${sexaquark_mass}"

        ANALYSIS_MODE="grid"
        ANALYSIS_INPUT_PATH="/alice/sim/${LOOPER_PRODUCTION_PATH}/${LOOPER_SIMSET}"
        ANALYSIS_PRODUCTION_NAME="${LOOPER_PRODUCTION_NAME}"
        ANALYSIS_RUN_NUMBER="${LOOPER_RUN_NUMBER}"
        ANALYSIS_LOCAL_N_DIRS=1 # not used
        ANALYSIS_LOCAL_LIMIT_N_EVENTS=0 # not used
        ANALYSIS_GRID_TEST_MODE=0
        ANALYSIS_GRID_WORKING_DIR="work/Esd2Tree/MC_${LOOPER_PRODUCTION_NAME}_${LOOPER_SIMSET}"
        ANALYSIS_GRID_CUSTOM_SPLIT=0 # not used
        ANALYSIS_GRID_CUSTOM_PATTERN="" # not used

        ANALYSIS_OPTIONS="("
        ANALYSIS_OPTIONS+="\"${ANALYSIS_MODE}\","
        ANALYSIS_OPTIONS+="\"${ANALYSIS_INPUT_PATH}\","
        ANALYSIS_OPTIONS+="\"${ANALYSIS_PRODUCTION_NAME}\","
        ANALYSIS_OPTIONS+="${ANALYSIS_RUN_NUMBER},"
        ANALYSIS_OPTIONS+="${ANALYSIS_LOCAL_N_DIRS},"
        ANALYSIS_OPTIONS+="${ANALYSIS_LOCAL_LIMIT_N_EVENTS},"
        ANALYSIS_OPTIONS+="${ANALYSIS_GRID_TEST_MODE},"
        ANALYSIS_OPTIONS+="\"${ANALYSIS_GRID_WORKING_DIR}\","
        ANALYSIS_OPTIONS+="${ANALYSIS_GRID_CUSTOM_SPLIT},"
        ANALYSIS_OPTIONS+="\"${ANALYSIS_GRID_CUSTOM_PATTERN}\""
        ANALYSIS_OPTIONS+=")"

        ALIROOT_COMMAND='aliroot -l -b -q runAnalysis.C'${ANALYSIS_OPTIONS}''
        echo "mc/looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NUMBER}_${LOOPER_SIMSET}:: running ${ALIROOT_COMMAND} ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"
        ${ALIROOT_COMMAND} 2>&1 | tee ${LOOPER_DIR}/${LOOPER_RUN_NUMBER}_${LOOPER_SIMSET}.log

    done # end of loop over sexaquark mass
    done # end of loop over signal channel

    cd ${ANALYSIS_DIR}/scripts_mc
    echo "mc/looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NUMBER}:: jobs submitted, now lets wait 2.5 min ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"

    ### STAGE 2: wait for jobs to finish ###

    while true; do
        sleep 150 # wait 2.5 minutes
        echo "mc/looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NUMBER}:: checking now how many jobs are still running ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"
        N_JOBS_RUNNING=$(alien.py ps -f r | wc -l)
        if [[ ${N_JOBS_RUNNING} -eq 0 ]]; then
            echo "looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NUMBER}:: all jobs are done ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"
            alien.py fquota list ${JALIEN_USERNAME} &> ${LOOPER_DIR}/${LOOPER_RUN_NUMBER}.fquota
            break
        else
            echo "mc/looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NUMBER}:: ${N_JOBS_RUNNING} jobs are still running, checking again in 5 min ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"
        fi
        sleep 150 # wait 2.5 minutes
    done

    ### STAGE 3: download files ###

    echo "mc/looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NUMBER}:: starting to download files ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"
    # for signal_channel in "A" "D" "E" "H"; do
    for signal_channel in "A" "D"; do
    for sexaquark_mass in "1.73" "1.8" "1.87" "1.94" "2.01"; do
        LOOPER_SIMSET="${signal_channel}${sexaquark_mass}"
        LOOPER_GRID_WORKING_DIR="work/Esd2Tree/MC_${LOOPER_PRODUCTION_NAME}_${LOOPER_SIMSET}"

        for LOOPER_FILE in $(alien.py find alien:///alice/cern.ch/user/${JALIEN_USERNAME:0:1}/${JALIEN_USERNAME}/${LOOPER_GRID_WORKING_DIR}/output/${LOOPER_RUN_NUMBER} "AnalysisResults.root"); do
            # example: /alice/cern.ch/user/a/aborquez/work/Esd2Tree/MC_LHC23l1a3_A1.8/output/246087/001/AnalysisResults.root
            # from path, extract directory number
            GRID_DN=$(basename $(dirname ${LOOPER_FILE}))
            GRID_DN=${GRID_DN%/} # remove trailing '/'
            echo "mc/looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NUMBER}_${LOOPER_SIMSET}:: getting file alien://${LOOPER_FILE}"
            alien.py cp alien://${LOOPER_FILE} file://${OUTPUT_DIR}/grid_mc_${LOOPER_PRODUCTION_NAME/LHC/}/${LOOPER_SIMSET}/${LOOPER_RUN_NUMBER}/${GRID_DN}/AnalysisResults.root
        done

    done # end of loop over sexaquark mass
    done # end of loop over signal channel

    ### STAGE 4: clean up ###

    echo "mc/looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NUMBER}:: starting to clean up ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"
    alien.py rm -rf /alice/cern.ch/user/${JALIEN_USERNAME:0:1}/${JALIEN_USERNAME}/work/Esd2Tree/*

done < "${RUN_NUMBERS_LIST}" # end of loop over RN

echo "mc/looper.sh(${JALIEN_USERNAME}):: my job is done. ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"
