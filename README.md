# esd2tree_hdib

**AliPhysics** analysis task `AliTaskEsd2Tree`, converts `AliESDs.root` files into derived trees.

## Requirements

The ALICE offline framework: AliRoot + AliPhysics. Specifically,
- At lxplus or GRID or CVMFS: the release tag `VO_ALICE@AliPhysics::vAN-20260616_O2-1`
- Locally, achieved a working version in Ubuntu 22.04 with:
   - aliBuild
   - https://github.com/HD-ALICE-Sexaquark/AliRoot (fork; use branch: borquez/alien-patch)
   - https://github.com/HD-ALICE-Sexaquark/alidist (fork; use branch: aborquez/grid)
   - `aliBuild build AliPhysics --defaults grid -j <N>`

## Environment

For basic local usage:
- All env. variables that come with `alienv enter AliRoot/latest,AliPhysics/latest`
- `ALICE_DATA` : (Reference: https://alisw.github.io/git-advanced/#how-to-use-large-data-files-for-analysis)

To use the bash scripts: `LOCAL_SIMS_DIR`, `LOCAL_DATA_DIR`, `E2T_ROOT_DIR`, `E2T_SCRIPTS_DIR`, `E2T_TASK_DIR`,
`E2T_OUTPUT_DIR`, `E2T_SLURM_DIR`.

To deploy to the PI server with `scripts/deploy.sh`: `E2TH_ROOT_DIR`, `PI_USER_AND_HOST`, `E2TH_AT_PI_PATH`.

In addition, to use `scripts/looper/` and submit jobs to the Grid: `LOOPER_DIR`, `JALIEN_TOKEN_CERT`,
`JALIEN_TOKEN_KEY`, `GRID_XXX_HOME_DIR`.

## Input

For real data: valid `AliESDs.root` files.

For monte-carlo: `AliESDs.root` + `Kinematics.root` + `galice.root` files.

## Output

A file named `AnalysisResults.root`, containing:

- `TList` "QA". Notable histograms:
   - `TH1D` Centrality
   - `TH1D` CentralityINT7
   - `TH1D` Tracks_Bookkeeping (empty; booked only because downstream T2DS requires it)
   - `TH1D` PreFoundLambdas_Bookkeeping
- `TTree` "Events" (all branches of `common/Schema_Events.hpp` except `InjectedSexa`; `Track` and, in MC, `Track_McEntry`
  are always empty). Where each row is a distinct selected event, which contains vectors of:
   - `POD::Event` (as defined in `common/POD_Event.hpp`)
   - `POD::Track` (as defined in `common/POD_Track.hpp`), always empty
   - `POD::PreFoundLambda` (as defined in `common/POD_PreFoundLambda.hpp`)
