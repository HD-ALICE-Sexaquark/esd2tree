#ifndef ALIANALYSISTASKESD2TREE_H
#define ALIANALYSISTASKESD2TREE_H

#ifndef ALIANALYSISTASKSE_H
#include "AliAnalysisTaskSE.h"
#endif

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "TArray.h"
#include "TChain.h"
#include "TFile.h"
#include "TGrid.h"
#include "TH1.h"
#include "TList.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TROOT.h"
#include "TString.h"
#include "TSystem.h"
#include "TTree.h"

#include "AliAnalysisManager.h"
#include "AliExternalTrackParam.h"
#include "AliHelix.h"
#include "AliInputEventHandler.h"
#include "AliLog.h"
#include "AliPIDResponse.h"
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

class AliPIDResponse;

class AliAnalysisTaskEsd2Tree : public AliAnalysisTaskSE {
   public:
    AliAnalysisTaskEsd2Tree();
    AliAnalysisTaskEsd2Tree(const char* name);
    virtual ~AliAnalysisTaskEsd2Tree();
    virtual void Terminate(Option_t* option) { return; }

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
    void EndOfEvent();

    /* Trees */
    void PrepareEventsBranches();
    void PrepareInjectedBranches();
    void PrepareMCParticlesBranches();
    void PrepareTracksBranches();

    void ClearEventsBranches();
    void ClearInjectedBranches();
    void ClearMCParticlesBranches();
    void ClearTracksBranches();

    /* Events */
    Bool_t PassesEventSelection();

    /* MC Generated */
    void ProcessMCGen();
    Int_t GetAncestor(Int_t mcIdx, Int_t generation = 0);
    Short_t GetGenerator(Int_t mcIdx);
    Int_t GetReactionID(Int_t ancestorIdx);

    /* Tracks */
    void ProcessTracks();
    Bool_t PassesTrackSelection(AliESDtrack* track);

    /* External Files */
    Bool_t ReadSignalLogs();

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
    Double_t fMagneticField;        //! magnetic field

    Int_t fRunNumber;     //! run number
    Float_t fDirNumber;   //! directory number
    Int_t fEventNumber;   //! event number
    Float_t fCentrality;  //! centrality percentile

    /* External Files */
    TString fAliEnPath;        //! loaded in `UserNotify()`
    Bool_t fIsFirstEvent;      //!
    TString fReactionChannel;  //! derived from `fAliEnPath` in `UserNotify()`

    /*** Trees ***/

    TTree* fTree_Events;    //!
    TTree* fTree_Injected;  //!
    TTree* fTree_MC;        //!
    TTree* fTree_Tracks;    //!

    /*** Branches ***/

    /** Events **/
    Float_t tEvents_PV_TrueXv;            //!
    Float_t tEvents_PV_TrueYv;            //!
    Float_t tEvents_PV_TrueZv;            //!
    Bool_t tEvents_IsGenPileup;           //!
    Bool_t tEvents_IsSBCPileup;           //!
    Float_t tEvents_PV_RecXv;             //!
    Float_t tEvents_PV_RecYv;             //!
    Float_t tEvents_PV_RecZv;             //!
    Int_t tEvents_PV_NContributors;       //!
    Float_t tEvents_PV_ZvErr_FromSPD;     //!
    Float_t tEvents_PV_ZvErr_FromTracks;  //!
    Float_t tEvents_PV_Zv_FromSPD;        //!
    Float_t tEvents_PV_Zv_FromTracks;     //!
    Float_t tEvents_PV_Dispersion;        //!
    Int_t tEvents_NTracks;                //!
    Int_t tEvents_NTPCClusters;           //!
    Bool_t tEvents_IsMB;                  //!
    Bool_t tEvents_IsHighMultV0;          //!
    Bool_t tEvents_IsHighMultSPD;         //!
    Bool_t tEvents_IsCentral;             //!
    Bool_t tEvents_IsSemiCentral;         //!

    /** Injected Sexaquarks **/
    Int_t tInjected_RunNumber;         //!
    Int_t tInjected_DirNumber;         //!
    Int_t tInjected_EventNumber;       //!
    Int_t tInjected_ReactionID;        //!
    Float_t tInjected_Px;              //!
    Float_t tInjected_Py;              //!
    Float_t tInjected_Pz;              //!
    Float_t tInjected_Mass;            //!
    Int_t tInjected_Nucleon_PdgCode;   //!
    Float_t tInjected_Nucleon_Px;      //!
    Float_t tInjected_Nucleon_Py;      //!
    Float_t tInjected_Nucleon_Pz;      //!
    UInt_t tInjected_ReactionChannel;  //! char -> ASCII -> uint

    /** MC Particles **/
    Int_t tMC_Idx;           //!
    Int_t tMC_PdgCode;       //!
    Int_t tMC_Idx_Mother;    //!
    Int_t tMC_NDaughters;    //!
    Int_t tMC_Idx_FirstDau;  //!
    Int_t tMC_Idx_LastDau;   //!
    Int_t tMC_Idx_Ancestor;  //!
    Float_t tMC_Px;          //!
    Float_t tMC_Py;          //!
    Float_t tMC_Pz;          //!
    Float_t tMC_Xv;          //! origin x-vertex
    Float_t tMC_Yv;          //! origin y-vertex
    Float_t tMC_Zv;          //! origin z-vertex
    UInt_t tMC_Status;       //!
    Bool_t tMC_IsOOBPileup;  //!
    Short_t tMC_Generator;   //! 0: HIJING, 1: anti-neutron injector, 2: anti-sexaquark reaction
    Bool_t tMC_IsSecondary;  //!
    Bool_t tMC_IsSignal;     //!
    Int_t tMC_ReactionID;    //!

    /** Tracks **/
    Int_t tTrack_Idx;                   //!
    Float_t tTrack_Px;                  //! inner parametrization
    Float_t tTrack_Py;                  //! inner parametrization
    Float_t tTrack_Pz;                  //! inner parametrization
    Short_t tTrack_Charge;              //!
    Float_t tTrack_Alpha;               //!
    Float_t tTrack_SineAzimuthalAngle;  //!
    Float_t tTrack_TangentDipAngle;     //!
    Float_t tTrack_OneOverPt;           //!
    // Float_t tTrack_CovMatrix[15];       //! covariance matrix
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

    TList* fList_Dummy;  //!
    TH1F* fHist_Dummy;   //!

    AliAnalysisTaskEsd2Tree(const AliAnalysisTaskEsd2Tree&);             // not implemented
    AliAnalysisTaskEsd2Tree& operator=(const AliAnalysisTaskEsd2Tree&);  // not implemented

    /// \cond CLASSDEF
    ClassDef(AliAnalysisTaskEsd2Tree, 1);  // = number of persistent members
    /// \endcond
};

#endif
