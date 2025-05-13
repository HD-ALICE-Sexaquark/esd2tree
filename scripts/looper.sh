#!/bin/bash

JALIEN_USERNAME=$(alien.py whoami)
if [[ -z ${ANALYSIS_DIR} ]]; then echo "looper.sh(${JALIEN_USERNAME}):: ANALYSIS_DIR is not defined"; exit 1; fi
if [[ -z ${OUTPUT_DIR} ]]; then echo "looper.sh(${JALIEN_USERNAME}):: OUTPUT_DIR is not defined"; exit 1; fi

LOOPER_DIR=${ANALYSIS_DIR}/scripts/${JALIEN_USERNAME}
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

RUN_NUMBERS_LIST="${ANALYSIS_DIR}/scripts/RN_${JALIEN_USERNAME}.txt"

LOOPER_PREVIOUS_RN=
LOOPER_COUNTER=0
while IFS= read -r line; do
    LOOPER_RUN_NUMBER=$(echo ${line} | awk -F, '{print $1}')
    LOOPER_RUN_NAME=${LOOPER_RUN_NUMBER}

    ### STAGE 0b: determine custom options ###

    LOOPER_CUSTOM_SPLIT=$(echo ${line} | awk -F, '{print $2}')
    if [[ ! -z ${LOOPER_CUSTOM_SPLIT} ]]; then
        echo "looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NAME}:: detected custom split, set to ${LOOPER_CUSTOM_SPLIT}"
    fi

    if [[ ${LOOPER_RUN_NUMBER} -ne ${LOOPER_PREVIOUS_RN} ]]; then LOOPER_COUNTER=0; fi
    LOOPER_CUSTOM_PATTERN=$(echo ${line} | awk -F, '{print $3}')
    if [[ ! -z ${LOOPER_CUSTOM_PATTERN} ]]; then
        LOOPER_RUN_NAME=${LOOPER_RUN_NUMBER}_${LOOPER_COUNTER}
        echo "looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NAME}:: detected custom data pattern, set to ${LOOPER_CUSTOM_PATTERN}"
    fi

    echo "looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NAME}:: starting run number loop ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"

    ### STAGE 0c: determine production ###

    LOOPER_PRODUCTION_PATH=""
    if [[ -z ${LOOPER_PRODUCTION_PATH} ]]; then
        LOOPER_PRODUCTION_PATH=$(grep -q ${LOOPER_RUN_NUMBER} ${ANALYSIS_DIR}/doc/LHC15o_pass2_rn.txt && echo "2015/LHC15o")
    fi
    if [[ -z ${LOOPER_PRODUCTION_PATH} ]]; then
        LOOPER_PRODUCTION_PATH=$(grep -q ${LOOPER_RUN_NUMBER} ${ANALYSIS_DIR}/doc/LHC18q_pass3_rn.txt && echo "2018/LHC18q")
    fi
    if [[ -z ${LOOPER_PRODUCTION_PATH} ]]; then
        LOOPER_PRODUCTION_PATH=$(grep -q ${LOOPER_RUN_NUMBER} ${ANALYSIS_DIR}/doc/LHC18r_pass3_rn_EXTRA.txt && echo "2018/LHC18r")
    fi
    LOOPER_PRODUCTION_NAME=$(echo ${LOOPER_PRODUCTION_PATH} | awk -F/ '{print $2}')
    echo "looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NAME}:: determined production ${LOOPER_PRODUCTION_NAME} ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"

    ### STAGE 0d: can we submit a job? ###

    LOOPER_JQUOTA=$(alien.py jquota list ${JALIEN_USERNAME})
    LOOPER_TotalRunningTimeLast24H=$(echo "${LOOPER_JQUOTA}" | grep "totalRunningTimeLast24h" | awk '{print $3}')
    if [[ ${LOOPER_TotalRunningTimeLast24H} -gt 8000000 ]]; then
        echo "looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NAME}:: the total running time in the last 24 hours is ${LOOPER_TotalRunningTimeLast24H} > 8000000, I will not submit more jobs"
        echo "looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NAME}:: NOTE: make sure to update the RN list before initiating looper again!!"
        break
    fi

    ### STAGE 1: submit jobs ###

    cd ${LOOPER_DIR}

    ANALYSIS_MODE="grid"
    ANALYSIS_INPUT_PATH="/alice/data/${LOOPER_PRODUCTION_PATH}"
    ANALYSIS_PRODUCTION_NAME="${LOOPER_PRODUCTION_NAME}"
    ANALYSIS_RUN_NUMBER=${LOOPER_RUN_NUMBER}
    ANALYSIS_LOCAL_N_DIRS=1 # not used
    ANALYSIS_LOCAL_LIMIT_N_EVENTS=0 # not used
    ANALYSIS_GRID_TEST_MODE=0
    ANALYSIS_GRID_WORKING_DIR="work/Esd2Tree/Data_${LOOPER_PRODUCTION_NAME}"
    ANALYSIS_GRID_CUSTOM_SPLIT=0 # default
    if [[ ! -z ${LOOPER_CUSTOM_SPLIT} ]]; then ANALYSIS_GRID_CUSTOM_SPLIT=${LOOPER_CUSTOM_SPLIT}; fi
    ANALYSIS_GRID_CUSTOM_PATTERN="" # default
    if [[ ! -z ${LOOPER_CUSTOM_PATTERN} ]]; then ANALYSIS_GRID_CUSTOM_PATTERN=${LOOPER_CUSTOM_PATTERN}; fi

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
    echo "looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NAME}:: running ${ALIROOT_COMMAND} ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"
    ${ALIROOT_COMMAND} 2>&1 | tee ${LOOPER_DIR}/000${LOOPER_RUN_NAME}.log

    cd ${ANALYSIS_DIR}/scripts
    echo "looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NAME}:: jobs submitted, now lets wait 2.5 min ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"

    ### STAGE 2: wait for jobs to finish ###

    while true; do
        sleep 150 # wait 2.5 minutes
        echo "looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NAME}:: checking now how many jobs are still runninng ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"
        N_JOBS_RUNNING=$(alien.py ps -f r | wc -l)
        if [[ ${N_JOBS_RUNNING} -eq 0 ]]; then
            echo "looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NAME}:: all jobs are done ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"
            alien.py fquota list ${JALIEN_USERNAME} &> ${LOOPER_DIR}/000${LOOPER_RUN_NAME}.fquota
            break
        else
            echo "looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NAME}:: ${N_JOBS_RUNNING} jobs are still running, checking again in 5 min ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"
        fi
        sleep 150 # wait 2.5 minutes
    done

    ### STAGE 3: download files ###

    echo "looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NAME}:: starting to download files ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"
    for LOOPER_FILE in $(alien.py find alien:///alice/cern.ch/user/${JALIEN_USERNAME:0:1}/${JALIEN_USERNAME}/${ANALYSIS_GRID_WORKING_DIR}/output/000${LOOPER_RUN_NUMBER} "AnalysisResults.root"); do
        # example: /alice/cern.ch/user/a/aborquez/work/Esd2Tree/Data_LHC15o/output/000246087/1415/AnalysisResults.root
        # from path, extract directory number
        GRID_DN=$(basename $(dirname ${LOOPER_FILE}))
        GRID_DN=${GRID_DN%/} # remove trailing '/'
        echo "looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NAME}:: getting file alien://${LOOPER_FILE}"
        alien.py cp alien://${LOOPER_FILE} file://${OUTPUT_DIR}/grid_data_${LOOPER_PRODUCTION_NAME/LHC/}/${LOOPER_RUN_NAME}/${GRID_DN}/AnalysisResults.root
    done

    ### STAGE 4: clean up ###

    echo "looper.sh(${JALIEN_USERNAME})::${LOOPER_RUN_NAME}:: starting to clean up ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"
    alien.py rm -rf /alice/cern.ch/user/${JALIEN_USERNAME:0:1}/${JALIEN_USERNAME}/work/Esd2Tree/*

    LOOPER_COUNTER=$((LOOPER_COUNTER+1))
    LOOPER_PREVIOUS_RN=${LOOPER_RUN_NUMBER}
done < "${RUN_NUMBERS_LIST}" # end of loop over RN

echo "looper.sh(${JALIEN_USERNAME}):: my job is done. ($(date "+%I:%M:%S %p @ %d-%b-%Y"))"
