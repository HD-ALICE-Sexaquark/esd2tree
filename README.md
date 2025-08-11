# esd2vector

First step.
**[AliRoot](https://github.com/alisw/AliRoot)**+**[AliPhysics](https://github.com/alisw/AliPhysics)** macros that convert `AliESDs.root` files into ROOT trees with a simple vector format.

## Debug

```c++
Events->Scan("RunNumber:DirNumber:EventNumber:Centrality:MagneticField:PV_TrueXv:PV_TrueYv:PV_TrueZv:IsGenPileup:IsSBCPileup:PV_NContributors:PV_Dispersion:PV_Xv:PV_Yv:PV_Zv:PV_CovMatrix:SPD_PV_Zv:SPD_PV_ZvErr:NTracks:NTPCClusters:IsMB:IsHighMultV0:IsHighMultSPD:IsCentral:IsSemiCentral")
Events->Scan("RunNumber:DirNumber:EventNumber:PV_Xv:PV_Yv:PV_Zv:PV_CovMatrix")
Events->Scan("RunNumber:DirNumber:EventNumber:PV_Xv:PV_Yv:PV_Zv")
Events->Scan("RunNumber:DirNumber:EventNumber:PV_Xv:PV_Yv:PV_Zv:SPD_PV_Zv:SPD_PV_ZvErr")

Events->Scan("MC_PdgCode:MC_Mother_McEntry:MC_Px:MC_Py:MC_Pz:MC_Xv:MC_Yv:MC_Zv:MC_Status:MC_IsOOBPileup:MC_Generator:MC_IsPrimary:MC_IsSecFromMat:MC_IsSecFromWeak")

Events->Scan("Sexaquark_Px:Sexaquark_Py:Sexaquark_Pz:Nucleon_Px:Nucleon_Py:Nucleon_Pz:ReactionID")

Events->Scan("Track_Px:Track_Py:Track_Pz:Track_X:Track_Y:Track_Z:Track_Charge:Track_Alpha:Track_Snp:Track_Tgl:Track_Signed1Pt")
Events->Scan("Track_SigmaY2:Track_SigmaZY:Track_SigmaZ2:Track_SigmaSnpY:Track_SigmaSnpZ:Track_SigmaSnp2:Track_SigmaTglY:Track_SigmaTglZ:Track_SigmaTglSnp:Track_SigmaTgl2:Track_Sigma1PtY:Track_Sigma1PtZ:Track_Sigma1PtSnp:Track_Sigma1PtTgl:Track_Sigma1Pt2:Track_NSigmaPion")
Events->Scan("Track_NSigmaKaon:Track_NSigmaProton:Track_DCAxy:Track_DCAz:Track_NTPCClusters:Track_NCrossedRows:Track_NFindableClusters:Track_NSharedClusters:Track_Chi2overNcls:Track_IsKinkDaughter:Track_McEntry")

Events->Scan("RunNumber:DirNumberB:EventNumber:PV_Xv:PV_Yv:PV_Zv:PV_CovMatrix")
```

## Output Format

The output consists of a ROOT file named `AnalysisResults.root` with a tree called `Events`.

To list all branches of the output `TFile`: `root -l -b -q AnalysisResults.root -e "Events->GetListOfLeaves()->Print()"`

```c++
// event properties
unsigned int         RunNumber
unsigned int         DirNumber
unsigned int         DirNumberB // ^1
unsigned int         EventNumber
float                Centrality
float                MagneticField
int                  PV_NContributors
float                PV_Dispersion
float                PV_Xv
float                PV_Yv
float                PV_Zv
std::array<float, 6> PV_CovMatrix
float                SPD_PV_Zv
float                SPD_PV_ZvErr
unsigned int         NTracks
int                  NTPCClusters
bool                 IsMB
bool                 IsHighMultV0
bool                 IsHighMultSPD
bool                 IsCentral
bool                 IsSemiCentral
// track properties
std::vector<float>   Track_X
std::vector<float>   Track_Y
std::vector<float>   Track_Z
std::vector<float>   Track_Px
std::vector<float>   Track_Py
std::vector<float>   Track_Pz
std::vector<int>     Track_Charge
std::vector<float>   Track_NSigmaPion
std::vector<float>   Track_NSigmaKaon
std::vector<float>   Track_NSigmaProton
std::vector<float>   Track_DCAxy
std::vector<float>   Track_DCAz
std::vector<float>   Track_SignalTPC // ^2
std::vector<float>   Track_IsKinkDaughter
std::vector<float>   Track_Alpha
std::vector<float>   Track_Snp
std::vector<float>   Track_Tgl
std::vector<float>   Track_Signed1Pt
std::vector<float>   Track_SigmaY2
std::vector<float>   Track_SigmaZY
std::vector<float>   Track_SigmaZ2
std::vector<float>   Track_SigmaSnpY
std::vector<float>   Track_SigmaSnpZ
std::vector<float>   Track_SigmaSnp2
std::vector<float>   Track_SigmaTglY
std::vector<float>   Track_SigmaTglZ
std::vector<float>   Track_SigmaTglSnp
std::vector<float>   Track_SigmaTgl2
std::vector<float>   Track_Sigma1PtY
std::vector<float>   Track_Sigma1PtZ
std::vector<float>   Track_Sigma1PtSnp
std::vector<float>   Track_Sigma1PtTgl
std::vector<float>   Track_Sigma1Pt2

// ^1: available only for data
// ^2: available only for newer versions
```

If the processed data were MC simulations, it will also contain:

```c++
// event properties
float                      MC_PV_Xv
float                      MC_PV_Yv
float                      MC_PV_Zv
bool                       MC_IsGenPileup
bool                       MC_IsSBCPileup
// mc properties
std::vector<int>           MC_PdgCode
std::vector<long>          MC_Mother_McEntry
std::vector<float>         MC_X
std::vector<float>         MC_Y
std::vector<float>         MC_Z
std::vector<float>         MC_Px
std::vector<float>         MC_Py
std::vector<float>         MC_Pz
std::vector<int>  MC_Status
std::vector<bool>          MC_IsOOBPileup
std::vector<int>           MC_Generator
std::vector<bool>          MC_IsPrimary
std::vector<bool>          MC_IsSecFromMat
std::vector<bool>          MC_IsSecFromWeak
// track properties
std::vector<long>          Track_McEntry
```

And if the processed MC were dedicated signal simulations, the output file will also contain these branches:

```c++
std::vector<int> ReactionID
std::vector<float>        Sexaquark_Px
std::vector<float>        Sexaquark_Py
std::vector<float>        Sexaquark_Pz
std::vector<float>        Nucleon_Px
std::vector<float>        Nucleon_Py
std::vector<float>        Nucleon_Pz
```
