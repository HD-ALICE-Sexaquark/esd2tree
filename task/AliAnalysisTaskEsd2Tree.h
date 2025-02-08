#ifndef ALIANALYSISTASKESD2TREE_H
#define ALIANALYSISTASKESD2TREE_H

#include <fstream>
#include <unordered_map>
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

class AliAnalysisTaskEsd2Tree : public AliAnalysisTaskSE {
   public:
    AliAnalysisTaskEsd2Tree();
    AliAnalysisTaskEsd2Tree(const char* name);
    ~AliAnalysisTaskEsd2Tree();

    AliAnalysisTaskEsd2Tree(const AliAnalysisTaskEsd2Tree&);
    AliAnalysisTaskEsd2Tree& operator=(const AliAnalysisTaskEsd2Tree&);

    /* Settings ~ stored in Analysis Manager */
    void IsMC(Bool_t is_mc, Bool_t is_signal_mc = kFALSE) {
        fIsMC = is_mc;
        fIsSignalMC = is_signal_mc;
    };
    void Initialize();
    void DefineTracksCuts(TString cuts_option);

    /* Main ~ executed at runtime */
    void UserCreateOutputObjects();
    Bool_t UserNotify();
    void UserExec(Option_t* option);
    void Terminate(Option_t* option) { return; }

    /* Tree */
    void AssociateEventsBranches();
    void AssociateInjectedBranches();
    void AssociateMCBranches();
    void AssociateTracksBranches();

    /* Events */
    void ProcessEvent();
    Bool_t PassesEventSelection();

    /* MC Particles */
    void ProcessMCParticles();
    void ClearMCBranches();

    /* Tracks */
    void ProcessTracks();
    Bool_t PassesTrackSelection(AliESDtrack* track);
    void ClearTracksBranches();

    /* Injected Reactions */
    void ProcessInjectedReactions();
    void BringSignalLogs();
    Bool_t LoadSignalLogs();
    void ClearSignalLogs();
    void ClearInjectedBranches();

   private:
    /* Settings ~ stored in Analysis Manager ~ all persistent */
    Bool_t fIsMC;        // kTRUE if MC simulation, kFALSE if data
    Bool_t fIsSignalMC;  // kTRUE to read and load signal logs

    /* AliRoot Objects */
    AliMCEvent* fMC;                //! MC event
    AliVVertex* fMC_PrimaryVertex;  //! MC gen. (or true) primary vertex
    AliESDEvent* fESD;              //! reconstructed event
    AliESDVertex* fPrimaryVertex;   //! primary vertex
    AliPIDResponse* fPIDResponse;   //! pid response object
    AliEventCuts fEventCuts;        //! event cuts

    TString fSignalSimSet;   //! signal simulations set (e.g. "A1.8", "H2.01")
    UInt_t fRunNumber;       //! run number
    UInt_t fDirNumber;       //! directory number
    UInt_t fDirNumberB;      //! component after-the-dot of data directories
    UInt_t fEventNumber;     //! event number
    Float_t fCentrality;     //! centrality percentile
    Float_t fMagneticField;  //! magnetic field

    /* Signal Logs */
    TString fAliEnPath;              //! loaded in `UserNotify()`
    TString fReactionChannel;        //! derived from `fAliEnPath` in `UserNotify()`
    TString fSignalLog_NewBasename;  //!

    /* Utilities */
    std::unordered_map<Int_t, Long_t> fMcEntry_;                      //! key: mc_idx
    std::unordered_map<UInt_t, std::vector<UInt_t>> fReactionID_;     //! key: event_n
    std::unordered_map<UInt_t, std::vector<Float_t>> fSexaquark_Px_;  //! key: event_n
    std::unordered_map<UInt_t, std::vector<Float_t>> fSexaquark_Py_;  //! key: event_n
    std::unordered_map<UInt_t, std::vector<Float_t>> fSexaquark_Pz_;  //! key: event_n
    std::unordered_map<UInt_t, std::vector<Float_t>> fNucleon_Px_;    //! key: event_n
    std::unordered_map<UInt_t, std::vector<Float_t>> fNucleon_Py_;    //! key: event_n
    std::unordered_map<UInt_t, std::vector<Float_t>> fNucleon_Pz_;    //! key: event_n

    /* Output */

    /** QA Histograms **/
    TList* fOutputList;              //!
    TH1F* fHist_Events_Bookkeeping;  //!
    TH1F* fHist_Centrality;          //!
    TH1F* fHist_CentralityINT7;      //!

    /** Trees **/
    TTree* fOutputTree;  //!
    /* -- Event properties */
    Float_t tEvent_PV_TrueXv;        //!
    Float_t tEvent_PV_TrueYv;        //!
    Float_t tEvent_PV_TrueZv;        //!
    Bool_t tEvent_IsGenPileup;       //!
    Bool_t tEvent_IsSBCPileup;       //!
    Int_t tEvent_PV_NContributors;   //!
    Float_t tEvent_PV_Dispersion;    //!
    Float_t tEvent_PV_Xv;            //!
    Float_t tEvent_PV_Yv;            //!
    Float_t tEvent_PV_Zv;            //!
    Float_t tEvent_PV_CovMatrix[6];  //!
    Float_t tEvent_SPD_PV_Zv;        //!
    Float_t tEvent_SPD_PV_ZvErr;     //!
    UInt_t tEvent_NTracks;           //!
    Int_t tEvent_NTPCClusters;       //!
    Bool_t tEvent_IsMB;              //!
    Bool_t tEvent_IsHighMultV0;      //!
    Bool_t tEvent_IsHighMultSPD;     //!
    Bool_t tEvent_IsCentral;         //!
    Bool_t tEvent_IsSemiCentral;     //!
    /* -- Signal reaction properties */
    std::vector<UInt_t> tInjected_ReactionID;   //!
    std::vector<Float_t> tInjected_Px;          //!
    std::vector<Float_t> tInjected_Py;          //!
    std::vector<Float_t> tInjected_Pz;          //!
    std::vector<Float_t> tInjected_Nucleon_Px;  //!
    std::vector<Float_t> tInjected_Nucleon_Py;  //!
    std::vector<Float_t> tInjected_Nucleon_Pz;  //!
    /* -- MC particles properties */
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
    /* -- Tracks properties */
    std::vector<Float_t> tTrack_Px;                //! inner parametrization
    std::vector<Float_t> tTrack_Py;                //! inner parametrization
    std::vector<Float_t> tTrack_Pz;                //! inner parametrization
    std::vector<Float_t> tTrack_X;                 //!
    std::vector<Float_t> tTrack_Y;                 //!
    std::vector<Float_t> tTrack_Z;                 //!
    std::vector<Int_t> tTrack_Charge;              //!
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
    std::vector<Float_t> tTrack_NSigmaPion;        //!
    std::vector<Float_t> tTrack_NSigmaKaon;        //!
    std::vector<Float_t> tTrack_NSigmaProton;      //!
    std::vector<Float_t> tTrack_DCAxy;             //! pre-calculated DCA wrt PV
    std::vector<Float_t> tTrack_DCAz;              //! pre-calculated DCA wrt PV
    std::vector<UInt_t> tTrack_NTPCClusters;       //!
    std::vector<Float_t> tTrack_NCrossedRows;      //!
    std::vector<UInt_t> tTrack_NFindableClusters;  //!
    std::vector<UInt_t> tTrack_NSharedClusters;    //!
    std::vector<Float_t> tTrack_Chi2overNcls;      //!
    std::vector<Bool_t> tTrack_IsKinkDaughter;     //!
    // TBits tTrack_TPCFitMap;             //!
    // TBits tTrack_TPCClusterMap;         //!
    // TBits tTrack_TPCSharedMap;          //!
    std::vector<ULong_t> tTrack_McEntry;  //!

    /*** Cuts ~ persistent, because they are set on `Initialize()` ***/
    Float_t kMax_NSigma_Pion;                //
    Float_t kMax_NSigma_Kaon;                //
    Float_t kMax_NSigma_Proton;              //
    Float_t kMax_Track_Eta;                  //
    Float_t kMin_Track_NTPCClusters;         //
    Float_t kMax_Track_Chi2PerNTPCClusters;  //
    Bool_t kTurnedOn_Track_StatusCuts;       //
    Bool_t kTurnedOn_Track_RejectKinks;      //
    Float_t kMin_Track_DCA_wrtPV;            //
    Float_t kMin_Track_DCAxy_wrtPV;          //
    Float_t kMin_Track_DCAz_wrtPV;           //

    ClassDef(AliAnalysisTaskEsd2Tree, 13);  // = number of persistent members
};

#endif  // ALIANALYSISTASKESD2TREE_H
