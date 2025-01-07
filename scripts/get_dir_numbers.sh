#!/bin/bash

# USAGE: ./get_dir_numbers.sh <run_numbers_file>
# EXAMPLE: ./get_dir_numbers.sh ${ANALYSIS_DIR}/doc/LHC18r_pass3_rn_TEST.txt
# NOTE: use "18q" or "18r" instead of the "18qr" files, as they could mess the output filenames

if [[ -z ${ANALYSIS_DIR} ]]; then echo "ERROR :: config_file.sh :: Missing env. var. ANALYSIS_DIR" ; exit 1; fi

GDN_OUTPUT_PATH="${ANALYSIS_DIR}/doc/dir_numbers"
mkdir -p ${GDN_OUTPUT_PATH}

GDN_RN_FILE="$1"
GDN_PRODUCTION_NAME=$(basename ${GDN_RN_FILE} | cut -d'_' -f1)
GDN_PASS_NUMBER=$(basename ${GDN_RN_FILE} | cut -d'_' -f2)
GDN_YEAR="20$(echo ${GDN_PRODUCTION_NAME} | cut -c4-5)"

echo "GDN_RN_FILE=${GDN_RN_FILE}"
echo "GDN_PRODUCTION_NAME=${GDN_PRODUCTION_NAME}"
echo "GDN_PASS_NUMBER=${GDN_PASS_NUMBER}"
echo "GDN_YEAR=${GDN_YEAR}"

GDN_ALIEN_PATH="alien:///alice/data/${GDN_YEAR}/${GDN_PRODUCTION_NAME}"

# read and loop over a run numbers file
while IFS= read -r line; do
    GDN_RUN_NUMBER=${line}
    SINGLE_FILE=${GDN_OUTPUT_PATH}/DN_${GDN_PRODUCTION_NAME}_${GDN_RUN_NUMBER}
    alien.py ls ${GDN_ALIEN_PATH}/000${GDN_RUN_NUMBER}/${GDN_PASS_NUMBER} > ${SINGLE_FILE}.tmp
    sed -i "/000${GDN_RUN_NUMBER}/!d" ${SINGLE_FILE}.tmp
    sed -i "s/.*000${GDN_RUN_NUMBER}//" ${SINGLE_FILE}.tmp
    sed -i 's/.$//' ${SINGLE_FILE}.tmp
    awk '{printf "%s ", $0} NR % 50 == 0 {print ""}' ${SINGLE_FILE}.tmp > ${SINGLE_FILE}.txt
    echo "" >> ${SINGLE_FILE}.txt
    rm ${SINGLE_FILE}.tmp
done < ${GDN_RN_FILE}
