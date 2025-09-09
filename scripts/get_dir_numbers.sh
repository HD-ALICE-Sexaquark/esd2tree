#!/bin/bash

# USAGE: ./get_dir_numbers.sh <run_numbers_file>
# EXAMPLE: ./get_dir_numbers.sh ${E2V_ROOT_DIR}/doc/LHC18r_pass3_rn_TEST.txt
# NOTE: use "18q" or "18r" instead of the "18qr" files, as they could mess the output filenames

if [[ -z ${E2V_ROOT_DIR} ]]; then echo "missing E2V_ROOT_DIR" ; exit 1; fi

output_path="${E2V_ROOT_DIR}/doc/dir_numbers"
mkdir -p ${output_path}

rn_file="$1"
production_name=$(basename ${rn_file} | cut -d'_' -f1)
mkdir -p ${output_path}/${production_name}
pass_number=$(basename ${rn_file} | cut -d'_' -f2)
year="20$(echo ${production_name} | cut -c4-5)"

echo "rn_file         = ${rn_file}"
echo "production_name = ${production_name}"
echo "pass_number     = ${pass_number}"
echo "year            = ${year}"

alien_path="alien:///alice/data/${year}/${production_name}"

# read and loop over a run numbers file
while IFS= read -r line; do
    run_number=${line}
    echo "reading rn      = ${run_number}"
    single_file=${output_path}/${production_name}/${run_number}
    alien.py ls ${alien_path}/000${run_number}/${pass_number} > ${single_file}.tmp
    sed -i "/000${run_number}/!d" ${single_file}.tmp
    sed -i "s/.*000${run_number}//" ${single_file}.tmp
    sed -i 's/.$//' ${single_file}.tmp
    cut -c1-6 ${single_file}.tmp | sort | uniq -c | awk '{print $2, $1}' > ${single_file}.txt
    rm ${single_file}.tmp
done < ${rn_file}
