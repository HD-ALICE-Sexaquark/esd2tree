#!/bin/bash

set -euo pipefail

if [[ -z ${E2R_ROOT_DIR:-} ]]; then echo "error: missing env. var. E2R_ROOT_DIR"; exit 1; fi
if [[ $# -ne 1 ]]; then echo "usage: ./generate_xml.sh <rn_file>"; exit 1; fi

rn_file=$1

# hardcoded
max_total_files=15000
delta=7500

pass_subdir="pass3"
if [[ $(basename "${rn_file}") == "LHC15o_pass2_rn.txt" ]]; then
    prod_path="/alice/data/2015/LHC15o"
    pass_subdir="pass2"
elif [[ $(basename "${rn_file}") == "LHC18q_pass3_rn.txt" ]]; then
    prod_path="/alice/data/2018/LHC18q"
elif [[ $(basename "${rn_file}") == "LHC18r_pass3_rn_EXTRA.txt" ]]; then
    prod_path="/alice/data/2018/LHC18r"
else
    echo "error: you need to choose a valid rn file"
    exit 1
fi
echo "prod_path     = ${prod_path}"
echo "pass_subdir   = ${pass_subdir}"
echo ""

while read -r rn; do
    n_total_files=$(alien.py find -c "${prod_path}/000${rn}/${pass_subdir}" "*/AliESDs.root" | tail -1 | awk '{print $2}')
    if [[ ${n_total_files} -le ${max_total_files} ]]; then continue; fi

    xml_rn_dir="${E2R_ROOT_DIR}/xml/$(basename "${prod_path}")/${rn}"
    mkdir -p "${xml_rn_dir}"

    n_divisions=$((n_total_files / delta))
    if [[ $((n_total_files % delta)) -gt 0 ]]; then n_divisions=$((n_divisions + 1)); fi
    echo "rn            = ${rn}"
    echo "n_total_files = ${n_total_files}"
    echo "delta         = ${delta}"
    echo "n_divisions   = ${n_divisions}"

    for ((i = 0 ; i < n_divisions ; i++)); do
        offset=$((i * delta))
        alien.py find -l ${delta} -o ${offset} -f -x- "${prod_path}/000${rn}/${pass_subdir}" "*/AliESDs.root" > "${xml_rn_dir}/${rn}_$((i + 1)).xml"
    done
    echo ""
done < "${rn_file}"
