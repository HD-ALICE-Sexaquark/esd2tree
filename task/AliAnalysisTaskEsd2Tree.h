#ifndef AliAnalysisTaskEsd2Tree_H
#define AliAnalysisTaskEsd2Tree_H

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
#include "TDatabasePDG.h"
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

#include "Math/Factory.h"
#include "Math/Functor.h"
#include "Math/Minimizer.h"
#include "Math/Point3D.h"   // because "TVector3.h" is deprecated
#include "Math/Vector3D.h"  // because "TVector3.h" is deprecated
#include "Math/Vector4D.h"  // because "TLorentzVector.h" is deprecated
#include "Math/VectorUtil.h"

using namespace ROOT;

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
    void GetAncestor(Int_t mcIdx, Int_t& ancestorIdx, Int_t generation = 0);
    void ProcessSignalReactions();

    /* Tracks */
    void ProcessTracks();
    Bool_t PassesTrackSelection(AliESDtrack* track, Bool_t isSecondary, Bool_t isSignal);
    Bool_t PassesSpeciesSelection(AliESDtrack* track, Int_t esdPdgCode, Bool_t isSecondary, Bool_t isSignal);
    void DefineTracksCuts(TString cuts_option);

    /* Utilities */
    void ClearContainers();

    /* External Files */
    Bool_t ReadSignalLogs();

   private:
    /* Settings ~ stored in Analysis Manager ~ all persistent */
    Bool_t fIsMC;        // kTRUE if MC simulation, kFALSE if data
    Bool_t fIsSignalMC;  // kTRUE to read and load signal logs

    /* AliRoot Objects */
    AliMCEvent* fMC;                    //! MC event
    AliVVertex* fMC_PrimaryVertex;      //! MC gen. (or true) primary vertex
    Math::XYZPoint v3MC_PrimaryVertex;  //!
    AliESDEvent* fESD;                  //! reconstructed event
    AliESDVertex* fPrimaryVertex;       //! primary vertex
    Math::XYZPoint v3PrimaryVertex;     //!
    AliPIDResponse* fPIDResponse;       //! pid response object
    AliEventCuts fEventCuts;            //! event cuts
    Double_t fMagneticField;            //! magnetic field
    Int_t fRunNumber;                   //! run number
    Float_t fDirNumber;                 //! directory number
    Int_t fEventNumber;                 //! event number
    Float_t fCentrality;                //! centrality percentile

    /* ROOT Objects */
    TDatabasePDG fPDG;  //!

    /* External Files */
    TString fAliEnPath;        //! loaded in `UserNotify()`
    Bool_t fIsFirstEvent;      //!
    TString fReactionChannel;  //! derived from `fAliEnPath` in `UserNotify()`

    /* Filled at `Initialize()` ~ persistent */
    Double_t kMass_Neutron;  //
    Double_t kMass_Proton;   //
    Double_t kMass_Kaon;     //
    Double_t kMass_Pion;     //

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
    Float_t tMC_Px;          //!
    Float_t tMC_Py;          //!
    Float_t tMC_Pz;          //!
    Float_t tMC_Xv;          //! origin x-vertex
    Float_t tMC_Yv;          //! origin y-vertex
    Float_t tMC_Zv;          //! origin z-vertex
    UInt_t tMC_Status;       //!
    Bool_t tMC_IsOOBPileup;  //!
    Bool_t tMC_IsSecondary;  //!
    Bool_t tMC_IsSignal;     //!
    Int_t tMC_ReactionID;    //!

    /** Tracks **/
    Int_t tTrack_Idx;              //!
    Float_t tTrack_Px;             //! inner parametrization
    Float_t tTrack_Py;             //! inner parametrization
    Float_t tTrack_Pz;             //! inner parametrization
    Short_t tTrack_Charge;         //!
    Float_t tTrack_NSigmaPion;     //!
    Float_t tTrack_NSigmaKaon;     //!
    Float_t tTrack_NSigmaProton;   //!
    TBits tTrack_TPCFitMap;        //!
    TBits tTrack_TPCClusterMap;    //!
    TBits tTrack_TPCSharedMap;     //!
    Bool_t tTrack_IsKinkDaughter;  //!
    Bool_t tTrack_ITSin;           //!
    Bool_t tTrack_ITSout;          //!
    Bool_t tTrack_ITSrefit;        //!
    Bool_t tTrack_ITSpid;          //!
    Bool_t tTrack_TPCin;           //!
    Bool_t tTrack_TPCout;          //!
    Bool_t tTrack_TPCrefit;        //!
    Bool_t tTrack_TPCpid;          //!
    Int_t tTrack_Idx_True;         //!
    Int_t tTrack_True_PdgCode;     //!
    Bool_t tTrack_IsSecondary;     //!
    Bool_t tTrack_IsSignal;        //!
    Int_t tTrack_ReactionID;       //!

    /*** Containers -- vectors and hash tables ***/
    /* Note: when adding a new one, don't forget to clear it on `ClearContainers()` */

    /* filled at `ProcessMCGen()` */
    std::unordered_map<Int_t, Int_t> getPdgCode_fromMcIdx;  //! key: `mcIdx`

    std::unordered_map<Int_t, Bool_t> isMcIdxSignal;     //! key: `mcIdx`
    std::unordered_map<Int_t, Bool_t> isMcIdxSecondary;  //! key: `mcIdx`

    std::unordered_map<Int_t, Int_t> getReactionID_fromMcIdx;                   //! key: `mcIdx`
    std::unordered_map<Int_t, std::vector<Int_t>> getMcIndices_fromReactionID;  //! key: `ReactionID`

    std::unordered_map<Int_t, Bool_t> doesMcIdxHaveMother;        //! key: `mcIdx`
    std::unordered_map<Int_t, Int_t> getMotherMcIdx_fromMcIdx;    //! key: `mcIdx`
    std::unordered_map<Int_t, Int_t> getAncestorMcIdx_fromMcIdx;  //! key: `mcIdx`
    std::unordered_map<Int_t, Int_t> getNegDauMcIdx_fromMcIdx;    //! key: `mcIdx`
    std::unordered_map<Int_t, Int_t> getPosDauMcIdx_fromMcIdx;    //! key: `mcIdx`

    /* filled at `ProcessTracks()` */
    std::unordered_map<Int_t, Int_t> getMcIdx_fromEsdIdx;  //! key: `esdIdx`

    /*** Cuts ~ persistent, because they are set on `Initialize()` ***/

    /** Tracks **/
    Float_t kMax_NSigma_Pion;                          //
    Float_t kMax_NSigma_Kaon;                          //
    Float_t kMax_NSigma_Proton;                        //
    Float_t kMax_Track_Eta;                            //
    Float_t kMin_Track_NTPCClusters;                   //
    Float_t kMax_Track_Chi2PerNTPCClusters;            //
    Bool_t kTurnedOn_Track_StatusCuts;                 //
    Bool_t kTurnedOn_Track_RejectKinks;                //
    Float_t kMin_Track_DCA_wrtPV;                      //
    Float_t kMin_Track_DCAxy_wrtPV;                    //
    Float_t kMin_Track_DCAz_wrtPV;                     //
    std::unordered_map<Int_t, Float_t> kMin_Track_Pt;  // key: `pdg code`

    AliAnalysisTaskEsd2Tree(const AliAnalysisTaskEsd2Tree&);             // not implemented
    AliAnalysisTaskEsd2Tree& operator=(const AliAnalysisTaskEsd2Tree&);  // not implemented

    /// \cond CLASSDEF
    ClassDef(AliAnalysisTaskEsd2Tree, 78);  // = number of persistent members
    /// \endcond
};

#endif
