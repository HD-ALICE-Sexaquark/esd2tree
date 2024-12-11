# Datasets

## Run Numbers

Choosing the run numbers with `Central Barrel Tracking_hadron PID`. In detail:

- `LHC15o_pass2_rn.txt` (total: 144)
- `LHC15o_pass2_TEST.txt` (total: 4)
- `LHC18q_pass3_rn.txt` (total: 135)
- `LHC18r_pass3_rn.txt` (total: 90)
- `LHC18qr_pass3_rn.txt` (total: 225)
- `LHC18qr_pass3_rn_TEST.txt` (total: 4)
- `LHC18qr_pass3_rn_EXTRA.txt` (total: 232)
  - Added 7 extra run numbers! Only valid in data and gen. purpose MC
  - Reference: https://twiki.cern.ch/twiki/bin/viewauth/ALICE/AliDPGRunList18r

## Dedicated MC

- **Dedicated for anti-sexaquark and anti-neutron studies**

- `LHC23l1`

  - Anchored to: `LHC18qr_pass3`, `LHC15o_pass2`
  - Transport: Geant3
  - Jira Ticket: [ALIROOT-8862](https://alice.its.cern.ch/jira/browse/ALIROOT-8862)

## General Purpose MC

- `LHC20e3a`

  - Anchored to: `LHC18qr_pass3`
  - Generator: HIJING
  - Transport: Geant3
  - Jira Ticket: [ALIROOT-8462](https://alice.its.cern.ch/jira/browse/ALIROOT-8462)

- `LHC20j6a`

  - Anchored to: `LHC15o_pass2`
  - Generator: HIJING
  - Transport: Geant3
  - Jira Ticket: [ALIROOT-8568](https://alice.its.cern.ch/jira/browse/ALIROOT-8568)

## Data

- `LHC18qr`

  - Collision system: Pb-Pb, 5.02 TeV
  - Pass: `pass3`

- `LHC15o`

  - Collision system: Pb-Pb, 5.02 TeV
  - Pass: `pass2`
