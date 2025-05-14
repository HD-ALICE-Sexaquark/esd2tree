#ifndef E2V_TASK_H
#define E2V_TASK_H

#define WRITE_ESD_INDICES 0
#define INCLUDE_MUCH_INFO 0

#include <array>
#include <fstream>
#include <vector>

#include "TArray.h"
#include "TChain.h"
#include "TClass.h"
#include "TFile.h"
#include "TGrid.h"
#include "TH1.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TROOT.h"
#include "TString.h"
#include "TSystem.h"
#include "TTree.h"

#ifndef ALIANALYSISTASKSE_H
#include "AliAnalysisTaskSE.h"
#endif

#include "AliAnalysisManager.h"
#include "AliExternalTrackParam.h"
#include "AliHelix.h"
#include "AliInputEventHandler.h"
#include "AliLog.h"
#include "AliVEvent.h"
#include "AliVVertex.h"

#include "AliESD.h"
#include "AliESDEvent.h"
#include "AliESDInputHandler.h"
#include "AliESDVertex.h"
#include "AliESDtrack.h"
#include "AliESDtrackCuts.h"

#include "AliMCEvent.h"
#include "AliMCEventHandler.h"
#include "AliMCParticle.h"

#include "AliAnalysisUtils.h"
#include "AliEventCuts.h"
#include "AliMultSelection.h"
#include "AliPIDResponse.h"

class AliPIDResponse;

class AliAnalysisTaskEsd2Vector : public AliAnalysisTaskSE {
   public:
    AliAnalysisTaskEsd2Vector();
    AliAnalysisTaskEsd2Vector(const char* name);
    ~AliAnalysisTaskEsd2Vector();

    AliAnalysisTaskEsd2Vector(const AliAnalysisTaskEsd2Vector&);
    AliAnalysisTaskEsd2Vector& operator=(const AliAnalysisTaskEsd2Vector&);

    // Settings ~ stored in Analysis Manager //
    void Initialize(bool is_mc, bool is_signal_mc = false);

    // Main ~ executed at runtime //
    void UserCreateOutputObjects();
    bool UserNotify();
    void UserExec(Option_t* option);
    void Terminate(Option_t* option) {}

    // Tree //
    void CreateEventsBranches();
    void CreateInjectedBranches();
    void CreateMCBranches();
    void CreateTracksBranches();

    // Events //
    bool ProcessEvent();
    bool PassesEventSelection();

    // MC Particles //
    void ProcessMCParticles();
    void ClearMCBranches();

    // Tracks //
    void ProcessTracks();
    bool PassesTrackSelection(const AliESDtrack* track, const AliExternalTrackParam* inner_param);
    void ClearTracksBranches();

    // Injected Reactions //
    void ProcessInjectedReactions();
    void BringSignalLogs();
    bool LoadSignalLogs();

   private:
    // Settings ~ stored in Analysis Manager ~ all persistent //
    bool fIsMC;        // kTRUE if MC simulation, kFALSE if data
    bool fIsSignalMC;  // kTRUE to read and load signal logs

    // AliRoot Objects //
    AliMCEvent* fMC;                      //! MC event
    const AliVVertex* fMC_PrimaryVertex;  //! MC gen. (or true) primary vertex
    AliESDEvent* fESD;                    //! reconstructed event
    const AliESDVertex* fPrimaryVertex;   //! primary vertex
    AliPIDResponse* fPIDResponse;         //! pid response object
    AliEventCuts fEventCuts;              //! event cuts

    UInt_t fRunNumber;          //! run number
    UInt_t fDirNumber;          //! directory number
    UInt_t fDirNumberB;         //! component after-the-dot of data directories
    UInt_t fEventNumberInFile;  //! event number
    Float_t fCentrality;        //! centrality percentile
    Float_t fMagneticField;     //! magnetic field

    // Signal Logs //
    TString fAliEnPath;              //! loaded in `UserNotify()`
    TString fSignalLog_NewBasename;  //!

    // Utilities //
    std::vector<Long_t> fVec_McEntry;                          //!
    std::vector<int> ToRealEventNumber;                        //!
    std::vector<std::vector<unsigned int>> fEvVec_ReactionID;  //!
    std::vector<std::vector<float>> fEvVec_Sexaquark_Px;       //!
    std::vector<std::vector<float>> fEvVec_Sexaquark_Py;       //!
    std::vector<std::vector<float>> fEvVec_Sexaquark_Pz;       //!
    std::vector<std::vector<float>> fEvVec_Nucleon_Px;         //!
    std::vector<std::vector<float>> fEvVec_Nucleon_Py;         //!
    std::vector<std::vector<float>> fEvVec_Nucleon_Pz;         //!

    // Output //

    // QA Histograms //
    TList* fOutputList;              //!
    TH1F* fHist_Events_Bookkeeping;  //!
    TH1F* fHist_Centrality;          //!
    TH1F* fHist_CentralityINT7;      //!
    TH1F* fHist_Tracks_Bookkeeping;  //!

    // Trees //
    TTree* fOutputTree;  //!
    // -- Event properties //
    Float_t tEvent_PV_TrueXv;                    //!
    Float_t tEvent_PV_TrueYv;                    //!
    Float_t tEvent_PV_TrueZv;                    //!
    Bool_t tEvent_IsGenPileup;                   //!
    Bool_t tEvent_IsSBCPileup;                   //!
    Int_t tEvent_PV_NContributors;               //!
    Float_t tEvent_PV_Dispersion;                //!
    Float_t tEvent_PV_Xv;                        //!
    Float_t tEvent_PV_Yv;                        //!
    Float_t tEvent_PV_Zv;                        //!
    std::array<Float_t, 6> tEvent_PV_CovMatrix;  //!
    Float_t tEvent_SPD_PV_Zv;                    //!
    Float_t tEvent_SPD_PV_ZvErr;                 //!
    UInt_t tEvent_NTracks;                       //!
    Int_t tEvent_NTPCClusters;                   //!
    Bool_t tEvent_IsMB;                          //!
    Bool_t tEvent_IsHighMultV0;                  //!
    Bool_t tEvent_IsHighMultSPD;                 //!
    Bool_t tEvent_IsCentral;                     //!
    Bool_t tEvent_IsSemiCentral;                 //!
    // -- Signal reaction properties //
    std::vector<unsigned int> tInjected_ReactionID;  //!
    std::vector<float> tInjected_Px;                 //!
    std::vector<float> tInjected_Py;                 //!
    std::vector<float> tInjected_Pz;                 //!
    std::vector<float> tInjected_Nucleon_Px;         //!
    std::vector<float> tInjected_Nucleon_Py;         //!
    std::vector<float> tInjected_Nucleon_Pz;         //!
    // -- MC particles properties //
    std::vector<Int_t> tMC_PdgCode;          //!
    std::vector<Long_t> tMC_Mother_McEntry;  //!
    std::vector<Float_t> tMC_Px;             //!
    std::vector<Float_t> tMC_Py;             //!
    std::vector<Float_t> tMC_Pz;             //!
    std::vector<Float_t> tMC_Xv;             //! origin x-vertex
    std::vector<Float_t> tMC_Yv;             //! origin y-vertex
    std::vector<Float_t> tMC_Zv;             //! origin z-vertex
    std::vector<UInt_t> tMC_Status;          //!
    std::vector<Bool_t> tMC_IsOOBPileup;     //!
    std::vector<Int_t> tMC_Generator;        //! 0: HIJING, 1: anti-neutron injector, 2: anti-sexaquark reaction
    std::vector<Bool_t> tMC_IsPrimary;       //!
    std::vector<Bool_t> tMC_IsSecFromMat;    //!
    std::vector<Bool_t> tMC_IsSecFromWeak;   //!
    // -- Tracks properties //
#if WRITE_ESD_INDICES
    std::vector<Long_t> tTrack_EsdIdx;  //! DEBUG
#endif
    std::vector<Float_t> tTrack_Px;             //! inner parametrization
    std::vector<Float_t> tTrack_Py;             //! inner parametrization
    std::vector<Float_t> tTrack_Pz;             //! inner parametrization
    std::vector<Float_t> tTrack_X;              //!
    std::vector<Float_t> tTrack_Y;              //!
    std::vector<Float_t> tTrack_Z;              //!
    std::vector<Int_t> tTrack_Charge;           //!
    std::vector<Float_t> tTrack_NSigmaPion;     //!
    std::vector<Float_t> tTrack_NSigmaKaon;     //!
    std::vector<Float_t> tTrack_NSigmaProton;   //!
    std::vector<Float_t> tTrack_DCAxy;          //! pre-calculated DCA wrt PV
    std::vector<Float_t> tTrack_DCAz;           //! pre-calculated DCA wrt PV
    std::vector<Bool_t> tTrack_IsKinkDaughter;  //!
    std::vector<ULong_t> tTrack_McEntry;        //!
#if INCLUDE_MUCH_INFO
    std::vector<Float_t> tTrack_Alpha;             //!
    std::vector<Float_t> tTrack_Snp;               //! local sine of the track momentum azimuthal angle
    std::vector<Float_t> tTrack_Tgl;               //! tangent of the track momentum dip angle
    std::vector<Float_t> tTrack_Signed1Pt;         //! 1/pt
    std::vector<Float_t> tTrack_SigmaY2;           //! cov_matrix[0]
    std::vector<Float_t> tTrack_SigmaZY;           //! cov_matrix[1]
    std::vector<Float_t> tTrack_SigmaZ2;           //! cov_matrix[2]
    std::vector<Float_t> tTrack_SigmaSnpY;         //! cov_matrix[3]
    std::vector<Float_t> tTrack_SigmaSnpZ;         //! cov_matrix[4]
    std::vector<Float_t> tTrack_SigmaSnp2;         //! cov_matrix[5]
    std::vector<Float_t> tTrack_SigmaTglY;         //! cov_matrix[6]
    std::vector<Float_t> tTrack_SigmaTglZ;         //! cov_matrix[7]
    std::vector<Float_t> tTrack_SigmaTglSnp;       //! cov_matrix[8]
    std::vector<Float_t> tTrack_SigmaTgl2;         //! cov_matrix[9]
    std::vector<Float_t> tTrack_Sigma1PtY;         //! cov_matrix[10]
    std::vector<Float_t> tTrack_Sigma1PtZ;         //! cov_matrix[11]
    std::vector<Float_t> tTrack_Sigma1PtSnp;       //! cov_matrix[12]
    std::vector<Float_t> tTrack_Sigma1PtTgl;       //! cov_matrix[13]
    std::vector<Float_t> tTrack_Sigma1Pt2;         //! cov_matrix[14]
    std::vector<UInt_t> tTrack_NTPCClusters;       //!
    std::vector<Float_t> tTrack_NCrossedRows;      //!
    std::vector<UInt_t> tTrack_NFindableClusters;  //!
    std::vector<UInt_t> tTrack_NSharedClusters;    //!
    std::vector<Float_t> tTrack_Chi2overNcls;      //!
#endif
    // TBits tTrack_TPCFitMap;             //!
    // TBits tTrack_TPCClusterMap;         //!
    // TBits tTrack_TPCSharedMap;          //!

    ClassDef(AliAnalysisTaskEsd2Vector, 13);  // = number of persistent members
};

#endif  // E2V_TASK_H
