#ifndef ALIANALYSISTASKESD2TREE_H
#define ALIANALYSISTASKESD2TREE_H

#include <fstream>
#include <map>

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
    virtual ~AliAnalysisTaskEsd2Tree();

    AliAnalysisTaskEsd2Tree(const AliAnalysisTaskEsd2Tree&);
    AliAnalysisTaskEsd2Tree& operator=(const AliAnalysisTaskEsd2Tree&);

    /* Settings ~ stored in Analysis Manager */
    void IsMC(Bool_t IsMC, Bool_t IsSignalMC = kFALSE) {
        fIsMC = IsMC;
        fIsSignalMC = IsSignalMC;
    };
    void Initialize();
    void DefineTracksCuts(TString cuts_option);

    /* Main ~ executed at runtime */
    virtual void UserCreateOutputObjects();
    virtual Bool_t UserNotify();
    virtual void UserExec(Option_t* option);
    virtual void FinishTaskOutput();
    virtual void Terminate(Option_t* option) { return; }

    /* Trees */
    void AssociateEventsBranches();
    void AssociateInjectedBranches();
    void AssociateMCBranches();
    void AssociateTracksBranches();
    void WriteTree(TTree* thisTree);

    /* Events */
    Bool_t PassesEventSelection();
    void FillEvent();

    /* Signal Logs */
    void BringSignalLogs();
    Bool_t LoadSignalLogs();
    void ClearSignalLogs();
    void FillInjected();

    /* MC Particles */
    void FillMC();
    Int_t GetAncestor(Int_t mcIdx, Int_t generation = 0);
    Int_t GetReactionID(Int_t mcIdx, Int_t ancestorIdx);

    /* Tracks */
    void FillTracks();
    Bool_t PassesTrackSelection(AliESDtrack* track);

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

    /* Output */

    /** QA Histograms **/

    TList* fOutputList;              //!
    TH1F* fHist_Events_Bookkeeping;  //!
    TH1F* fHist_Centrality;          //!
    TH1F* fHist_CentralityINT7;      //!

    /** File and subdir **/

    TFile* fOutputFile;      //! pointer to the output file
    TDirectory* fOutputDir;  //! pointer to the output subdirectory

    /** Trees **/

    TTree* fTree_Events;    //!
    TTree* fTree_Injected;  //!
    TTree* fTree_MC;        //!
    TTree* fTree_Tracks;    //!

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

    std::map<Int_t, std::vector<UInt_t>> fMap_ReactionID;     //!
    std::map<Int_t, std::vector<Float_t>> fMap_Sexaquark_Px;  //!
    std::map<Int_t, std::vector<Float_t>> fMap_Sexaquark_Py;  //!
    std::map<Int_t, std::vector<Float_t>> fMap_Sexaquark_Pz;  //!
    std::map<Int_t, std::vector<Float_t>> fMap_Nucleon_Px;    //!
    std::map<Int_t, std::vector<Float_t>> fMap_Nucleon_Py;    //!
    std::map<Int_t, std::vector<Float_t>> fMap_Nucleon_Pz;    //!

    UInt_t tInjected_ReactionID;   //!
    Float_t tInjected_Px;          //!
    Float_t tInjected_Py;          //!
    Float_t tInjected_Pz;          //!
    Float_t tInjected_Nucleon_Px;  //!
    Float_t tInjected_Nucleon_Py;  //!
    Float_t tInjected_Nucleon_Pz;  //!

    UInt_t tMC_Idx;            //!
    Int_t tMC_PdgCode;         //!
    Int_t tMC_Idx_Mother;      //!
    Int_t tMC_Idx_Ancestor;    //!
    Float_t tMC_Px;            //!
    Float_t tMC_Py;            //!
    Float_t tMC_Pz;            //!
    Float_t tMC_Xv;            //! origin x-vertex
    Float_t tMC_Yv;            //! origin y-vertex
    Float_t tMC_Zv;            //! origin z-vertex
    UInt_t tMC_Status;         //!
    Bool_t tMC_IsOOBPileup;    //!
    Short_t tMC_Generator;     //! 0: HIJING, 1: anti-neutron injector, 2: anti-sexaquark reaction
    Bool_t tMC_IsPrimary;      //!
    Bool_t tMC_IsSecFromMat;   //!
    Bool_t tMC_IsSecFromWeak;  //!
    Int_t tMC_ReactionID;      //!

    UInt_t tTrack_Idx;                  //!
    Float_t tTrack_Px;                  //! inner parametrization
    Float_t tTrack_Py;                  //! inner parametrization
    Float_t tTrack_Pz;                  //! inner parametrization
    Float_t tTrack_X;                   //!
    Float_t tTrack_Y;                   //!
    Float_t tTrack_Z;                   //!
    Short_t tTrack_Charge;              //!
    Float_t tTrack_Alpha;               //!
    Float_t tTrack_Snp;                 //! local sine of the track momentum azimuthal angle
    Float_t tTrack_Tgl;                 //! tangent of the track momentum dip angle
    Float_t tTrack_Signed1Pt;           //! 1/pt
    Float_t tTrack_CovMatrix[15];       //! covariance matrix
    Float_t tTrack_NSigmaPion;          //!
    Float_t tTrack_NSigmaKaon;          //!
    Float_t tTrack_NSigmaProton;        //!
    Float_t tTrack_DCAxy;               //! pre-calculated DCA wrt PV
    Float_t tTrack_DCAz;                //! pre-calculated DCA wrt PV
    UShort_t tTrack_NTPCClusters;       //!
    Float_t tTrack_NCrossedRows;        //!
    UShort_t tTrack_NFindableClusters;  //!
    UShort_t tTrack_NSharedClusters;    //!
    Float_t tTrack_Chi2overNcls;        //!
    Bool_t tTrack_IsKinkDaughter;       //!
    // TBits tTrack_TPCFitMap;             //!
    // TBits tTrack_TPCClusterMap;         //!
    // TBits tTrack_TPCSharedMap;          //!
    Int_t tTrack_Idx_True;  //!

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
