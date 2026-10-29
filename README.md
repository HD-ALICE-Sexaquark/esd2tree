# esd2tree

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

It runs only at the PI server, deployed there by `scripts/deploy.sh`: the dedicated MC through its Slurm farm
(`scripts/farm-pi/`), the real data and the general-purpose MC on the Grid (`scripts/looper/`). Steps in
`docs/E2T_REPRO.md`.

For basic usage:
- All env. variables that come with `alienv enter AliRoot/latest,AliPhysics/latest`
- `ALICE_DATA` : (Reference: https://alisw.github.io/git-advanced/#how-to-use-large-data-files-for-analysis)
- `E2T_ROOT_DIR` : main repository dir

Optional, with their defaults:
- `E2T_OUTPUT_DIR` : `${E2T_ROOT_DIR}/output`, output of `scripts/farm-pi/` and `scripts/looper/`
- `E2T_SLURM_DIR` : `${E2T_ROOT_DIR}/slurm`, logs of `scripts/farm-pi/`

To deploy to the PI server with `scripts/deploy.sh` (from the laptop): `PI_USER_AND_HOST`, `E2T_AT_PI_PATH`.

To fetch the dedicated MC from GSI with `scripts/farm-pi/fetch_sims.sh`, and run on it with
`scripts/farm-pi/slurm_wrapper.sh`: `LOCAL_SIMS_DIR`, `GSI_USER_AND_HOST`, `FSSR_AT_GSI_PATH` (as in `~/simulations`;
optional: `FSSR_OUTPUT_AT_GSI_PATH`, default `${FSSR_AT_GSI_PATH}/output`).

To submit jobs to the Grid with `scripts/looper/` (or `scripts/exec.sh`), and to use `scripts/generate_xml.sh`:
`LOOPER_DIR`, `JALIEN_TOKEN_CERT`, `JALIEN_TOKEN_KEY`, `GRID_ABORQUEZ_HOME_DIR` (the Grid home dir that holds `xml/`).

## Input

For real data: valid `AliESDs.root` files.

For monte-carlo: `AliESDs.root` + `Kinematics.root` + `galice.root` files.

Supported productions, as `RunTask.C`'s `ProductionName` (any other name stops it):
- real data: `LHC15o`, `LHC18q`, `LHC18r`
- general-purpose MC: `LHC20e3a`, `LHC20e3b`, `LHC20j6a`, `LHC20j6b`
- antisexaquark MC: `LHC23l1ab4`, with `InputPath` ending in `/signal` or `/embedded`
- h-dibaryon MC: `LHC26h2`, with `InputPath` ending in `/signal` or `/embedded`

The antisexaquark real-data trees were produced by tag `rd-sexa-2026.10.12` (commit `f1ba831`), and are not produced
again: `POD::Event`, `POD::Track`, the track selection and the event selection stay as they were there.

## Output

A file named `AnalysisResults.root`, containing:

- `TList` "QA". Notable histograms:
   - `TH1D` Centrality
   - `TH1D` CentralityINT7
   - `TH1D` Tracks_Bookkeeping (empty when tracks aren't processed; booked anyway because downstream T2DS requires it)
   - `TH1D` PreFoundLambdas_Bookkeeping
- `TTree` "Events" (all branches of `common/Schema_Events.hpp`). Where each row is a distinct selected event, which
  contains vectors of:
   - `POD::Event` (as defined in `common/POD_Event.hpp`)
   - `POD::Track` (as defined in `common/POD_Track.hpp`), filled only for general-purpose and antisexaquark MC, empty
     otherwise
   - `POD::PreFoundLambda` (as defined in `common/POD_PreFoundLambda.hpp`)
   - in MC: `POD::MC::Event`, `POD::McParticle` (as defined in `common/POD_McParticle.hpp`), and the indices of each
     track's and each pre-found (anti)lambda daughter's `McParticle`
