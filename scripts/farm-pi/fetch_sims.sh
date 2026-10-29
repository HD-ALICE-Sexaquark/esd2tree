#!/bin/bash

# `esd2tree/scripts/farm-pi/fetch_sims.sh`
# ========================================
# Copy the dedicated MC productions from GSI into ${LOCAL_SIMS_DIR}, as they are there, one uncompressed tar per dir:
#   LOCAL_SIMS_DIR / <output_tag> / <run number> / <run number>_<dir number>.tar
# where <output_tag> is `LHC26h2/<variant>` or `LHC23l1ab4_<A|D|H>/<variant>`, and <variant> is `signal` or `embedded`.
# The tars are unpacked later, per job, by `slurm_exec.sh`.
# NOTE: meant to run at PI, inside tmux
# NOTE: only finished tars are copied (GSI writes `.tar.part` first, then renames it), and partial transfers are kept
#       hidden in `.rsync-partial/`, so any `.tar` here is complete; nothing is ever deleted; the same command resumes,
#       retries, and tops up while GSI is still producing

set -euo pipefail
shopt -s nullglob

# hardcoded options
n_dir_numbers=25 # per run number, as in `simulations/scripts/farm-gsi/submit.sh`

print_usage() {
    echo "usage: ./fetch_sims.sh [<output_tag> ...]"
    echo "where: <output_tag> : LHC26h2/<variant> or LHC23l1ab4_<A|D|H>/<variant>, with <variant> = signal, embedded"
    echo "                      (none = everything in the output dir at GSI)"
    echo "examples: ./fetch_sims.sh"
    echo "          ./fetch_sims.sh LHC26h2/signal LHC26h2/embedded"
}

# check environment
if [[ -z ${LOCAL_SIMS_DIR:-} ]]; then echo "error: missing env. var. LOCAL_SIMS_DIR"; exit 1; fi
if [[ -z ${GSI_USER_AND_HOST:-} ]]; then echo "error: missing env. var. GSI_USER_AND_HOST"; exit 1; fi
if [[ -z ${FSSR_AT_GSI_PATH:-} ]]; then echo "error: missing env. var. FSSR_AT_GSI_PATH"; exit 1; fi
sims_at_gsi_path=${FSSR_OUTPUT_AT_GSI_PATH:-${FSSR_AT_GSI_PATH}/output}

# command-line arguments
for output_tag in "$@"; do
    [[ ${output_tag} =~ ^(LHC26h2|LHC23l1ab4_[ADH])/(signal|embedded)$ ]] || { echo "error: invalid output tag: ${output_tag}"; print_usage; exit 1; }
done

# NOTE: no `-z`, ROOT files are already compressed
rsync_opts=(
    -av
    --partial-dir=.rsync-partial
    --prune-empty-dirs
    --include='*/'
    --include='*.tar'
    --exclude='*'
)

mkdir -p "${LOCAL_SIMS_DIR}"
if [[ $# -eq 0 ]]; then
    rsync "${rsync_opts[@]}" "${GSI_USER_AND_HOST}:${sims_at_gsi_path}/" "${LOCAL_SIMS_DIR}/"
    tag_dirs=("${LOCAL_SIMS_DIR}"/*/*/)
else
    tag_dirs=()
    for output_tag in "$@"; do
        mkdir -p "${LOCAL_SIMS_DIR}/${output_tag}"
        rsync "${rsync_opts[@]}" "${GSI_USER_AND_HOST}:${sims_at_gsi_path}/${output_tag}/" "${LOCAL_SIMS_DIR}/${output_tag}/"
        tag_dirs+=("${LOCAL_SIMS_DIR}/${output_tag}/")
    done
fi

# summary, per output tag: tars against the expected ones of the run numbers present
for tag_dir in "${tag_dirs[@]}"; do
    output_tag=${tag_dir#"${LOCAL_SIMS_DIR}/"}
    output_tag=${output_tag%/}
    n_runs=$(find "${tag_dir}" -mindepth 1 -maxdepth 1 -type d ! -name '.*' | wc -l)
    n_tars=$(find "${tag_dir}" -mindepth 2 -maxdepth 2 -type f -name '*.tar' | wc -l)
    echo "$0 @ ${HOSTNAME} :: ${output_tag} :: ${n_tars} of $((n_runs * n_dir_numbers)) tars, in ${n_runs} run numbers"
done
