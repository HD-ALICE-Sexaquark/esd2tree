#!/bin/bash

# !! Don't execute this script directly, it is meant to be used by `task_batch.sh` !!

#SBATCH --partition=main
#SBATCH --time=1:00:00
#SBATCH --mem-per-cpu=4000

if [[ -z ${LOCAL_SIMS_DIR} ]]; then echo "missing LOCAL_SIMS_DIR"; exit 1; fi
if [[ -z ${OUTPUT_DIR} ]]; then echo "missing OUTPUT_DIR"; exit 1; fi
if [[ -z ${ATASK_DIR} ]]; then echo "missing ATASK_DIR"; exit 1; fi

# convert space-separated strings into arrays
# NOTE: needed by Slurm
all_channels=(${UNROLLED_CHANNELS})
all_masses=(${UNROLLED_MASSES})
all_runs=(${UNROLLED_RUNS})

reaction_channel=${all_channels[${SLURM_ARRAY_TASK_ID}]}
injected_mass=${all_masses[${SLURM_ARRAY_TASK_ID}]}
input_path=${LOCAL_SIMS_DIR}/${PRODUCTION_NAME}/${reaction_channel}${injected_mass}

run_number=${all_runs[${SLURM_ARRAY_TASK_ID}]}

this_job_outdir=${OUTPUT_DIR}/${MODE}_${PRODUCTION_NAME}/${reaction_channel}${injected_mass}/${run_number}
mkdir -p ${this_job_outdir}

ANALYSIS_OPTIONS="("
ANALYSIS_OPTIONS+="\"${MODE}\","
ANALYSIS_OPTIONS+="\"${input_path}\","
ANALYSIS_OPTIONS+="\"${PRODUCTION_NAME}\","
ANALYSIS_OPTIONS+="${run_number},"
ANALYSIS_OPTIONS+="${LOCAL_N_DIRS},"
ANALYSIS_OPTIONS+="${LOCAL_LIMIT_N_EVENTS},"
ANALYSIS_OPTIONS+="${GRID_TEST_MODE},"
ANALYSIS_OPTIONS+="\"${GRID_WORKING_DIR}\","
ANALYSIS_OPTIONS+="${GRID_CUSTOM_SPLIT},"
ANALYSIS_OPTIONS+="\"${GRID_CUSTOM_PATTERN}\""
ANALYSIS_OPTIONS+=")"

cp ${ATASK_DIR}/AliAnalysisTaskEsd2Vector.cxx ${this_job_outdir}/
cp ${ATASK_DIR}/AliAnalysisTaskEsd2Vector.h ${this_job_outdir}/
cp ${ATASK_DIR}/AliAnalysisTaskEsd2Vector_Const.h ${this_job_outdir}/
cp ${ATASK_DIR}/AliAnalysisTaskEsd2Vector_Cuts.h ${this_job_outdir}/
cp ${ATASK_DIR}/AddTaskEsd2Vector.C ${this_job_outdir}/
cp ${ATASK_DIR}/runAnalysis.C ${this_job_outdir}/

cd ${this_job_outdir} || exit

ALIROOT_COMMAND="aliroot -l -b -q runAnalysis.C${ANALYSIS_OPTIONS}"
echo ${ALIROOT_COMMAND}
${ALIROOT_COMMAND} 2>&1 | tee analysis.log
