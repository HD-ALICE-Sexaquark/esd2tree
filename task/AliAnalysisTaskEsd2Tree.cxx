#include "AliAnalysisTaskEsd2Tree.h"

ClassImp(AliAnalysisTaskEsd2Tree);

/*
 * Empty I/O constructor. Non-persistent members are initialized to their default values from here.
 */
AliAnalysisTaskEsd2Tree::AliAnalysisTaskEsd2Tree()
    : AliAnalysisTaskSE(),
      /*  */
      fIsMC(0),
      fIsSignalMC(0),
      /*  */
      fMC(0),
      fMC_PrimaryVertex(0),
      v3MC_PrimaryVertex(),
      fESD(0),
      fPrimaryVertex(0),
      v3PrimaryVertex(),
      fPIDResponse(0),
      fEventCuts(),
      fMagneticField(0.),
      /*  */
      fRunNumber(0),
      fDirNumber(0.),
      fEventNumber(0),
      fCentrality(0.),
      /*  */
      fPDG(),
      /*  */
      fAliEnPath(),
      fIsFirstEvent(0),
      fReactionChannel(),
      /*  */
      kMass_Neutron(0.),
      kMass_Proton(0.),
      kMass_Kaon(0.),
      kMass_Pion(0.),
      /*  */
      fTree_Events(0),
      fTree_Injected(0),
      fTree_MC(0),
      fTree_Tracks(0),
      /*  */
      tEvents_PV_TrueXv(0),
      tEvents_PV_TrueYv(0),
      tEvents_PV_TrueZv(0),
      tEvents_IsGenPileup(0),
      tEvents_IsSBCPileup(0),
      tEvents_PV_RecXv(0),
      tEvents_PV_RecYv(0),
      tEvents_PV_RecZv(0),
      tEvents_PV_NContributors(0),
      tEvents_PV_ZvErr_FromSPD(0),
      tEvents_PV_ZvErr_FromTracks(0),
      tEvents_PV_Zv_FromSPD(0),
      tEvents_PV_Zv_FromTracks(0),
      tEvents_PV_Dispersion(0),
      tEvents_NTracks(0),
      tEvents_NTPCClusters(0),
      tEvents_IsMB(0),
      tEvents_IsHighMultV0(0),
      tEvents_IsHighMultSPD(0),
      tEvents_IsCentral(0),
      tEvents_IsSemiCentral(0),
      /*  */
      tInjected_RunNumber(0),
      tInjected_DirNumber(0),
      tInjected_EventNumber(0),
      tInjected_ReactionID(0),
      tInjected_Px(0),
      tInjected_Py(0),
      tInjected_Pz(0),
      tInjected_Mass(0),
      tInjected_Nucleon_PdgCode(0),
      tInjected_Nucleon_Px(0),
      tInjected_Nucleon_Py(0),
      tInjected_Nucleon_Pz(0),
      tInjected_ReactionChannel(0),
      /*  */
      tMC_Idx(0),
      tMC_PdgCode(0),
      tMC_Idx_Mother(0),
      tMC_NDaughters(0),
      tMC_Idx_FirstDau(0),
      tMC_Idx_LastDau(0),
      tMC_ReactionID(0),
      tMC_Px(0),
      tMC_Py(0),
      tMC_Pz(0),
      tMC_Xv(0),
      tMC_Yv(0),
      tMC_Zv(0),
      tMC_Status(0),
      tMC_IsOOBPileup(0),
      tMC_IsSecondary(0),
      tMC_IsSignal(0),
      /*  */
      tTrack_Idx(0),
      tTrack_Px(0),
      tTrack_Py(0),
      tTrack_Pz(0),
      tTrack_Charge(0),
      tTrack_NSigmaPion(0),
      tTrack_NSigmaKaon(0),
      tTrack_NSigmaProton(0),
      tTrack_TPCFitMap(0),
      tTrack_TPCClusterMap(0),
      tTrack_TPCSharedMap(0),
      tTrack_IsKinkDaughter(0),
      tTrack_ITSin(0),
      tTrack_ITSout(0),
      tTrack_ITSrefit(0),
      tTrack_ITSpid(0),
      tTrack_TPCin(0),
      tTrack_TPCout(0),
      tTrack_TPCrefit(0),
      tTrack_TPCpid(0),
      tTrack_Idx_True(0),
      tTrack_True_PdgCode(0),
      tTrack_IsSecondary(0),
      tTrack_IsSignal(0),
      tTrack_ReactionID(0),

      /*  */
      getPdgCode_fromMcIdx(),
      isMcIdxSignal(),
      isMcIdxSecondary(),
      getReactionID_fromMcIdx(),
      getMcIndices_fromReactionID(),
      doesMcIdxHaveMother(),
      getMotherMcIdx_fromMcIdx(),
      getAncestorMcIdx_fromMcIdx(),
      getNegDauMcIdx_fromMcIdx(),
      getPosDauMcIdx_fromMcIdx(),
      /*  */
      getMcIdx_fromEsdIdx(),
      /*  */
      kMax_NSigma_Pion(0.),
      kMax_NSigma_Kaon(0.),
      kMax_NSigma_Proton(0.),
      kMax_Track_Eta(0.),
      kMin_Track_NTPCClusters(0.),
      kMax_Track_Chi2PerNTPCClusters(0.),
      kTurnedOn_Track_StatusCuts(0),
      kTurnedOn_Track_RejectKinks(0),
      kMin_Track_DCA_wrtPV(0.),
      kMin_Track_DCAxy_wrtPV(0.),
      kMin_Track_DCAz_wrtPV(0.),
      kMin_Track_Pt() {}

/*
 * Constructor, called locally.
 */
AliAnalysisTaskEsd2Tree::AliAnalysisTaskEsd2Tree(const char* name)
    : AliAnalysisTaskSE(name),
      /*  */
      fIsMC(0),
      fIsSignalMC(0),
      /*  */
      fMC(0),
      fMC_PrimaryVertex(0),
      v3MC_PrimaryVertex(),
      fESD(0),
      fPrimaryVertex(0),
      v3PrimaryVertex(),
      fPIDResponse(0),
      fEventCuts(),
      fMagneticField(0.),
      fRunNumber(0),
      fDirNumber(0.),
      fEventNumber(0),
      fCentrality(0.),
      /*  */
      fPDG(),
      /*  */
      fAliEnPath(),
      fIsFirstEvent(0),
      fReactionChannel(),
      /*  */
      kMass_Neutron(0.),
      kMass_Proton(0.),
      kMass_Kaon(0.),
      kMass_Pion(0.),
      /*  */
      fTree_Events(0),
      fTree_Injected(0),
      fTree_MC(0),
      fTree_Tracks(0),
      /*  */
      tEvents_PV_TrueXv(0),
      tEvents_PV_TrueYv(0),
      tEvents_PV_TrueZv(0),
      tEvents_IsGenPileup(0),
      tEvents_IsSBCPileup(0),
      tEvents_PV_RecXv(0),
      tEvents_PV_RecYv(0),
      tEvents_PV_RecZv(0),
      tEvents_PV_NContributors(0),
      tEvents_PV_ZvErr_FromSPD(0),
      tEvents_PV_ZvErr_FromTracks(0),
      tEvents_PV_Zv_FromSPD(0),
      tEvents_PV_Zv_FromTracks(0),
      tEvents_PV_Dispersion(0),
      tEvents_NTracks(0),
      tEvents_NTPCClusters(0),
      tEvents_IsMB(0),
      tEvents_IsHighMultV0(0),
      tEvents_IsHighMultSPD(0),
      tEvents_IsCentral(0),
      tEvents_IsSemiCentral(0),
      /*  */
      tInjected_RunNumber(0),
      tInjected_DirNumber(0),
      tInjected_EventNumber(0),
      tInjected_ReactionID(0),
      tInjected_Px(0),
      tInjected_Py(0),
      tInjected_Pz(0),
      tInjected_Mass(0),
      tInjected_Nucleon_PdgCode(0),
      tInjected_Nucleon_Px(0),
      tInjected_Nucleon_Py(0),
      tInjected_Nucleon_Pz(0),
      tInjected_ReactionChannel(0),
      /*  */
      tMC_Idx(0),
      tMC_PdgCode(0),
      tMC_Idx_Mother(0),
      tMC_NDaughters(0),
      tMC_Idx_FirstDau(0),
      tMC_Idx_LastDau(0),
      tMC_ReactionID(0),
      tMC_Px(0),
      tMC_Py(0),
      tMC_Pz(0),
      tMC_Xv(0),
      tMC_Yv(0),
      tMC_Zv(0),
      tMC_Status(0),
      tMC_IsOOBPileup(0),
      tMC_IsSecondary(0),
      tMC_IsSignal(0),
      /*  */
      tTrack_Idx(0),
      tTrack_Px(0),
      tTrack_Py(0),
      tTrack_Pz(0),
      tTrack_Charge(0),
      tTrack_NSigmaPion(0),
      tTrack_NSigmaKaon(0),
      tTrack_NSigmaProton(0),
      tTrack_TPCFitMap(0),
      tTrack_TPCClusterMap(0),
      tTrack_TPCSharedMap(0),
      tTrack_IsKinkDaughter(0),
      tTrack_ITSin(0),
      tTrack_ITSout(0),
      tTrack_ITSrefit(0),
      tTrack_ITSpid(0),
      tTrack_TPCin(0),
      tTrack_TPCout(0),
      tTrack_TPCrefit(0),
      tTrack_TPCpid(0),
      tTrack_Idx_True(0),
      tTrack_True_PdgCode(0),
      tTrack_IsSecondary(0),
      tTrack_IsSignal(0),
      tTrack_ReactionID(0),
      /*  */
      getPdgCode_fromMcIdx(),
      isMcIdxSignal(),
      isMcIdxSecondary(),
      getReactionID_fromMcIdx(),
      getMcIndices_fromReactionID(),
      doesMcIdxHaveMother(),
      getMotherMcIdx_fromMcIdx(),
      getAncestorMcIdx_fromMcIdx(),
      getNegDauMcIdx_fromMcIdx(),
      getPosDauMcIdx_fromMcIdx(),
      /*  */
      getMcIdx_fromEsdIdx(),
      /*  */
      kMax_NSigma_Pion(0.),
      kMax_NSigma_Kaon(0.),
      kMax_NSigma_Proton(0.),
      kMax_Track_Eta(0.),
      kMin_Track_NTPCClusters(0.),
      kMax_Track_Chi2PerNTPCClusters(0.),
      kTurnedOn_Track_StatusCuts(0),
      kTurnedOn_Track_RejectKinks(0),
      kMin_Track_DCA_wrtPV(0.),
      kMin_Track_DCAxy_wrtPV(0.),
      kMin_Track_DCAz_wrtPV(0.),
      kMin_Track_Pt() {
    DefineInput(0, TChain::Class());

    DefineOutput(1, TTree::Class());  // fTree_Events
    DefineOutput(2, TTree::Class());  // fTree_Injected
    DefineOutput(3, TTree::Class());  // fTree_MC
    DefineOutput(4, TTree::Class());  // fTree_Tracks
}

/*
 * Destructor.
 * Note: if `TList::SetOwner(kTRUE)` was called, the TList destructor should delete all objects added to it
 */
AliAnalysisTaskEsd2Tree::~AliAnalysisTaskEsd2Tree() {
    if (fTree_Events) delete fTree_Events;
    if (fTree_Injected) delete fTree_Injected;
    if (fTree_MC) delete fTree_MC;
    if (fTree_Tracks) delete fTree_Tracks;
}

/*
 * Create output objects, called once at RUNTIME ~ execution on Grid.
 */
void AliAnalysisTaskEsd2Tree::UserCreateOutputObjects() {

    AliInfo("It begins");

    /* Add mandatory routines */

    AliAnalysisManager* man = AliAnalysisManager::GetAnalysisManager();
    if (!man) AliFatal("ERROR: AliAnalysisManager couldn't be found.");

    AliESDInputHandler* inputHandler = (AliESDInputHandler*)(man->GetInputEventHandler());
    if (!inputHandler) AliFatal("ERROR: AliESDInputHandler couldn't be found.");

    fPIDResponse = inputHandler->GetPIDResponse();

    /* Prepare output  */

    /** Trees **/

    fTree_Events = new TTree("Events", "Events");
    fTree_Injected = new TTree("Injected", "Injected");
    fTree_MC = new TTree("MC", "MC");
    fTree_Tracks = new TTree("Tracks", "Tracks");

    PrepareEventsBranches();
    PrepareInjectedBranches();
    PrepareMCParticlesBranches();
    PrepareTracksBranches();

    ClearEventsBranches();
    ClearInjectedBranches();
    ClearMCParticlesBranches();
    ClearTracksBranches();

    PostData(1, fTree_Events);
    PostData(2, fTree_Injected);
    PostData(3, fTree_MC);
    PostData(4, fTree_Tracks);

    AliInfo("It ends");
}

/*
 * Add branches to `fTree_Events`.
 */
void AliAnalysisTaskEsd2Tree::PrepareEventsBranches() {
    fTree_Events->Branch("EventNumber", &fEventNumber);
    fTree_Events->Branch("DirNumber", &fDirNumber);
    fTree_Events->Branch("RunNumber", &fRunNumber);
    fTree_Events->Branch("Centrality", &fCentrality);

    fTree_Events->Branch("PV_TrueXv", &tEvents_PV_TrueXv);
    fTree_Events->Branch("PV_TrueYv", &tEvents_PV_TrueYv);
    fTree_Events->Branch("PV_TrueZv", &tEvents_PV_TrueZv);
    fTree_Events->Branch("IsGenPileup", &tEvents_IsGenPileup);
    fTree_Events->Branch("IsSBCPileup", &tEvents_IsSBCPileup);

    fTree_Events->Branch("PV_RecXv", &tEvents_PV_RecXv);
    fTree_Events->Branch("PV_RecYv", &tEvents_PV_RecYv);
    fTree_Events->Branch("PV_RecZv", &tEvents_PV_RecZv);
    fTree_Events->Branch("PV_NContributors", &tEvents_PV_NContributors);
    fTree_Events->Branch("PV_ZvErr_FromSPD", &tEvents_PV_ZvErr_FromSPD);
    fTree_Events->Branch("PV_ZvErr_FromTracks", &tEvents_PV_ZvErr_FromTracks);
    fTree_Events->Branch("PV_Zv_FromSPD", &tEvents_PV_Zv_FromSPD);
    fTree_Events->Branch("PV_Zv_FromTracks", &tEvents_PV_Zv_FromTracks);
    fTree_Events->Branch("PV_Dispersion", &tEvents_PV_Dispersion);
    fTree_Events->Branch("NTracks", &tEvents_NTracks);
    fTree_Events->Branch("NTPCClusters", &tEvents_NTPCClusters);
    fTree_Events->Branch("IsMB", &tEvents_IsMB);
    fTree_Events->Branch("IsHighMultV0", &tEvents_IsHighMultV0);
    fTree_Events->Branch("IsHighMultSPD", &tEvents_IsHighMultSPD);
    fTree_Events->Branch("IsCentral", &tEvents_IsCentral);
    fTree_Events->Branch("IsSemiCentral", &tEvents_IsSemiCentral);
}

/*
 * Add branches to `fTree_MC`.
 */
void AliAnalysisTaskEsd2Tree::PrepareMCParticlesBranches() {
    fTree_MC->Branch("EventNumber", &fEventNumber);
    fTree_MC->Branch("DirNumber", &fDirNumber);
    fTree_MC->Branch("RunNumber", &fRunNumber);
    fTree_MC->Branch("Idx", &tMC_Idx);
    fTree_MC->Branch("PdgCode", &tMC_PdgCode);
    fTree_MC->Branch("Idx_Mother", &tMC_Idx_Mother);
    fTree_MC->Branch("NDaughters", &tMC_NDaughters);
    fTree_MC->Branch("Idx_FirstDau", &tMC_Idx_FirstDau);
    fTree_MC->Branch("Idx_LastDau", &tMC_Idx_LastDau);
    fTree_MC->Branch("ReactionID", &tMC_ReactionID);
    fTree_MC->Branch("Px", &tMC_Px);
    fTree_MC->Branch("Py", &tMC_Py);
    fTree_MC->Branch("Pz", &tMC_Pz);
    fTree_MC->Branch("Xv", &tMC_Xv);
    fTree_MC->Branch("Yv", &tMC_Yv);
    fTree_MC->Branch("Zv", &tMC_Zv);
    fTree_MC->Branch("Status", &tMC_Status);
    fTree_MC->Branch("IsOOBPileup", &tMC_IsOOBPileup);
    fTree_MC->Branch("IsSecondary", &tMC_IsSecondary);
    fTree_MC->Branch("IsSignal", &tMC_IsSignal);
}

/*
 * Add branches to `fTree_Tracks`.
 */
void AliAnalysisTaskEsd2Tree::PrepareTracksBranches() {
    fTree_Tracks->Branch("EventNumber", &fEventNumber);
    fTree_Tracks->Branch("DirNumber", &fDirNumber);
    fTree_Tracks->Branch("RunNumber", &fRunNumber);

    fTree_Tracks->Branch("Idx", &tTrack_Idx);
    fTree_Tracks->Branch("Px", &tTrack_Px);
    fTree_Tracks->Branch("Py", &tTrack_Py);
    fTree_Tracks->Branch("Pz", &tTrack_Pz);
    fTree_Tracks->Branch("Charge", &tTrack_Charge);
    fTree_Tracks->Branch("NSigmaPion", &tTrack_NSigmaPion);
    fTree_Tracks->Branch("NSigmaKaon", &tTrack_NSigmaKaon);
    fTree_Tracks->Branch("NSigmaProton", &tTrack_NSigmaProton);
    fTree_Tracks->Branch("TPCFitMap", &tTrack_TPCFitMap);
    fTree_Tracks->Branch("TPCClusterMap", &tTrack_TPCClusterMap);
    fTree_Tracks->Branch("TPCSharedMap", &tTrack_TPCSharedMap);
    fTree_Tracks->Branch("IsKinkDaughter", &tTrack_IsKinkDaughter);
    fTree_Tracks->Branch("ITSin", &tTrack_ITSin);
    fTree_Tracks->Branch("ITSout", &tTrack_ITSout);
    fTree_Tracks->Branch("ITSrefit", &tTrack_ITSrefit);
    fTree_Tracks->Branch("ITSpid", &tTrack_ITSpid);
    fTree_Tracks->Branch("TPCin", &tTrack_TPCin);
    fTree_Tracks->Branch("TPCout", &tTrack_TPCout);
    fTree_Tracks->Branch("TPCrefit", &tTrack_TPCrefit);
    fTree_Tracks->Branch("TPCpid", &tTrack_TPCpid);

    fTree_Tracks->Branch("Idx_True", &tTrack_Idx_True);
    fTree_Tracks->Branch("PdgCode_True", &tTrack_True_PdgCode);
    fTree_Tracks->Branch("IsSecondary", &tTrack_IsSecondary);
    fTree_Tracks->Branch("IsSignal", &tTrack_IsSignal);
    fTree_Tracks->Branch("ReactionID", &tTrack_ReactionID);
}

/*
 *
 */
void AliAnalysisTaskEsd2Tree::ClearEventsBranches() {
    // not clearing fRunNumber nor fDirNumber
    fEventNumber = 0;
    fCentrality = 0;

    tEvents_PV_TrueXv = -999.;
    tEvents_PV_TrueYv = -999.;
    tEvents_PV_TrueZv = -999.;
    tEvents_IsGenPileup = kFALSE;
    tEvents_IsSBCPileup = kFALSE;
    tEvents_PV_RecXv = 0.;
    tEvents_PV_RecYv = 0.;
    tEvents_PV_RecZv = 0.;
    tEvents_PV_NContributors = 0;
    tEvents_PV_ZvErr_FromSPD = 0.;
    tEvents_PV_ZvErr_FromTracks = 0.;
    tEvents_PV_Zv_FromSPD = 0.;
    tEvents_PV_Zv_FromTracks = 0.;
    tEvents_PV_Dispersion = 0;
    tEvents_NTracks = 0;
    tEvents_NTPCClusters = 0;
    tEvents_IsMB = kFALSE;
    tEvents_IsHighMultV0 = kFALSE;
    tEvents_IsHighMultSPD = kFALSE;
    tEvents_IsCentral = kFALSE;
    tEvents_IsSemiCentral = kFALSE;
}

/*
 *
 */
void AliAnalysisTaskEsd2Tree::ClearMCParticlesBranches() {
    tMC_Idx = 0;
    tMC_PdgCode = 0;
    tMC_Idx_Mother = 0;
    tMC_NDaughters = 0;
    tMC_Idx_FirstDau = 0;
    tMC_Idx_LastDau = 0;
    tMC_ReactionID = 0;
    tMC_Px = 0.;
    tMC_Py = 0.;
    tMC_Pz = 0.;
    tMC_Xv = 0.;
    tMC_Yv = 0.;
    tMC_Zv = 0.;
    tMC_Status = 0;
    tMC_IsOOBPileup = kFALSE;
    tMC_IsSecondary = kFALSE;
    tMC_IsSignal = kFALSE;
}

/*
 *
 */
void AliAnalysisTaskEsd2Tree::ClearTracksBranches() {
    tTrack_Idx = 0;
    tTrack_Px = 0.;
    tTrack_Py = 0.;
    tTrack_Pz = 0.;
    tTrack_Charge = 0;
    tTrack_NSigmaPion = 0.;
    tTrack_NSigmaKaon = 0.;
    tTrack_NSigmaProton = 0.;
    tTrack_TPCFitMap.ResetAllBits();
    tTrack_TPCClusterMap.ResetAllBits();
    tTrack_TPCSharedMap.ResetAllBits();
    tTrack_IsKinkDaughter = kFALSE;
    tTrack_ITSin = kFALSE;
    tTrack_ITSout = kFALSE;
    tTrack_ITSrefit = kFALSE;
    tTrack_ITSpid = kFALSE;
    tTrack_TPCin = kFALSE;
    tTrack_TPCout = kFALSE;
    tTrack_TPCrefit = kFALSE;
    tTrack_TPCpid = kFALSE;

    tTrack_Idx_True = 0;
    tTrack_True_PdgCode = 0;
    tTrack_IsSecondary = kFALSE;
    tTrack_IsSignal = kFALSE;
    tTrack_ReactionID = 0;
}

/*
 * Main function, called per each event at RUNTIME ~ execution on Grid.
 */
void AliAnalysisTaskEsd2Tree::UserExec(Option_t*) {

    /* Handle external logs */

    if (fIsFirstEvent) {
        if (fAliEnPath == "") AliWarning("!! No luck finding fAliEnPath !!");
        AliInfoF("!! fAliEnPath: %s !!", fAliEnPath.Data());
        if (fIsSignalMC) ReadSignalLogs();
        fIsFirstEvent = kFALSE;
    }

    /* Load MC Gen. Event and PV */

    if (fIsMC) {
        fMC = MCEvent();
        if (!fMC) AliFatal("ERROR: AliMCEvent couldn't be found.");
        fMC_PrimaryVertex = const_cast<AliVVertex*>(fMC->GetPrimaryVertex());
        v3MC_PrimaryVertex.SetXYZ(fMC_PrimaryVertex->GetX(), fMC_PrimaryVertex->GetY(), fMC_PrimaryVertex->GetZ());

        ProcessMCGen();
        if (fIsSignalMC) ProcessSignalReactions();
    }

    /* Load Reconstructed Event, PV and Magnetic Field */

    fESD = dynamic_cast<AliESDEvent*>(InputEvent());
    if (!fESD) AliFatal("ERROR: AliESDEvent couldn't be found.");
    fPrimaryVertex = const_cast<AliESDVertex*>(fESD->GetPrimaryVertex());
    v3PrimaryVertex.SetXYZ(fPrimaryVertex->GetX(), fPrimaryVertex->GetY(), fPrimaryVertex->GetZ());

    fMagneticField = fESD->GetMagneticField();

    fRunNumber = fESD->GetRunNumber();
    fEventNumber = fESD->GetEventNumberInFile();

    /* Centrality */

    AliMultSelection* MultSelection = dynamic_cast<AliMultSelection*>(fESD->FindListObject("MultSelection"));
    if (!MultSelection) AliFatal("ERROR: AliMultSelection couldn't be found.");
    fCentrality = MultSelection->GetMultiplicityPercentile("V0M");
    AliInfoF("Centrality (from MultSelection) = %.2f", fCentrality);  // DEBUG

    /* Primary Vertex Reconstruction */

    Double_t PV_CovMatrix[6];
    fPrimaryVertex->GetCovarianceMatrix(PV_CovMatrix);

    AliESDVertex* PrimaryVertex_SPD = const_cast<AliESDVertex*>(fESD->GetPrimaryVertexSPD());
    Double_t PV_SPD_CovMatrix[6];
    PrimaryVertex_SPD->GetCovarianceMatrix(PV_SPD_CovMatrix);

    /* Event */

    if (!PassesEventSelection()) {
        EndOfEvent();
        return;
    }

    /* Assign branches and fill tree */

    if (fMC_PrimaryVertex) {
        tEvents_PV_TrueXv = (Float_t)fMC_PrimaryVertex->GetX();
        tEvents_PV_TrueYv = (Float_t)fMC_PrimaryVertex->GetY();
        tEvents_PV_TrueZv = (Float_t)fMC_PrimaryVertex->GetZ();
        tEvents_IsGenPileup = AliAnalysisUtils::IsPileupInGeneratedEvent(fMC, "Hijing");
        tEvents_IsSBCPileup = AliAnalysisUtils::IsSameBunchPileupInGeneratedEvent(fMC, "Hijing");
    }
    tEvents_PV_RecXv = (Float_t)fPrimaryVertex->GetX();
    tEvents_PV_RecYv = (Float_t)fPrimaryVertex->GetY();
    tEvents_PV_RecZv = (Float_t)fPrimaryVertex->GetZ();
    tEvents_PV_NContributors = fPrimaryVertex->GetNContributors();
    tEvents_PV_ZvErr_FromSPD = (Float_t)PV_SPD_CovMatrix[5];
    tEvents_PV_ZvErr_FromTracks = (Float_t)PV_CovMatrix[5];
    tEvents_PV_Zv_FromSPD = (Float_t)PrimaryVertex_SPD->GetZ();
    tEvents_PV_Zv_FromTracks = (Float_t)fPrimaryVertex->GetZ();
    tEvents_PV_Dispersion = (Float_t)fPrimaryVertex->GetDispersion();
    tEvents_NTracks = fESD->GetNumberOfTracks();
    tEvents_NTPCClusters = fESD->GetNumberOfTPCClusters();
    tEvents_IsMB = fInputHandler->IsEventSelected() & AliVEvent::kINT7;
    tEvents_IsHighMultV0 = fInputHandler->IsEventSelected() & AliVEvent::kHighMultV0;
    tEvents_IsHighMultSPD = fInputHandler->IsEventSelected() & AliVEvent::AliVEvent::kHighMultSPD;
    tEvents_IsCentral = fInputHandler->IsEventSelected() & AliVEvent::kCentral;
    tEvents_IsSemiCentral = fInputHandler->IsEventSelected() & AliVEvent::kSemiCentral;

    fTree_Events->Fill();

    /* Tracks */

    ProcessTracks();

    EndOfEvent();
}

/*
 * End of event.
 */
void AliAnalysisTaskEsd2Tree::EndOfEvent() {

    ClearEventsBranches();
    ClearContainers();

    PostData(1, fTree_Events);
    PostData(2, fTree_Injected);
    PostData(3, fTree_MC);
    PostData(4, fTree_Tracks);
}

/*
 * User implementation of `Notify()`. Needed for reading the AliEn path.
 * This function is loaded during `AliAnalysisManager::Notify()`.
 * It's called after `UserCreateOutputObjects()`, for each new file, and before the first `UserExec()`.
 */
Bool_t AliAnalysisTaskEsd2Tree::UserNotify() {
    AliAnalysisManager* man = AliAnalysisManager::GetAnalysisManager();
    if (!man) AliFatal("Analysis Manager not found");
    TTree* man_tree = man->GetTree();
    if (!man_tree) AliFatal("Analysis Manager Tree not found");
    TFile* man_file = man_tree->GetCurrentFile();
    if (!man_file) AliFatal("Analysis Manager File not found");

    /* get AliEn path and tokenize it */

    fAliEnPath = man_file->GetName();
    AliInfoF("AliEn Path : %s", fAliEnPath.Data());

    TObjArray* tokens = fAliEnPath.Tokenize("/");

    fReactionChannel = "";

    if (fIsMC) {
        /* path of signal MC ends with format `.../LHC23l1a3/A1.73/297595/001/AliESDs.root` */
        /* and path of general MC ends with format `.../LHC20e3a/297595/001/AliESDs.root` */
        fDirNumber = ((TObjString*)tokens->At(tokens->GetEntries() - 2))->GetString().Atof();
        AliInfoF("Dir Number : %03.0f", fDirNumber);
        if (fIsSignalMC) {
            TString SimSet = ((TObjString*)tokens->At(tokens->GetEntries() - 4))->GetString();
            if (SimSet[0] == 'A') fReactionChannel = "ALK0";
            if (SimSet[0] == 'D') fReactionChannel = "ALPK";
            if (SimSet[0] == 'E') fReactionChannel = "ALPKPP";
            if (SimSet[0] == 'H') fReactionChannel = "PKPKX";
            AliInfoF("Simulation Set : %s", SimSet.Data());
            AliInfoF("Reaction Channel : %s", fReactionChannel.Data());
        }
    } else {  // fIsMC == kFALSE (data)
        /* path of data ends with format `.../LHC15o/000245232/pass2/15000245232039.914/AliESDs.root` */
        TString aux_dir_nr = ((TObjString*)tokens->At(tokens->GetEntries() - 2))->GetString();
        aux_dir_nr = TString(aux_dir_nr(2 + 3 + 6, 8));
        AliInfoF("Dir Number : %s", aux_dir_nr.Data());
        fDirNumber = aux_dir_nr.Atof();
    }

    fIsFirstEvent = kTRUE;

    return kTRUE;
}

/*
 * Initialize analysis task. Needs to be called within an `AddTask.C` macro.
 */
void AliAnalysisTaskEsd2Tree::Initialize() {

    /* Define cuts */

    DefineTracksCuts("standard");

    /*  */

    kMass_Neutron = fPDG.GetParticle(2112)->Mass();
    kMass_Proton = fPDG.GetParticle(2212)->Mass();
    kMass_Pion = fPDG.GetParticle(211)->Mass();
    kMass_Kaon = fPDG.GetParticle(321)->Mass();

    /* Initialize PDG */

    AliInfo("Initializing...");
    AliInfo("Settings:");
    AliInfo("========");
    AliInfoF(">> IsMC             = %i", (Int_t)fIsMC);
    AliInfoF(">> Read Signal Logs = %i", (Int_t)fIsSignalMC);
}

/*
 * Define track selection cuts.
 */
void AliAnalysisTaskEsd2Tree::DefineTracksCuts(TString cuts_option) {

    kMax_NSigma_Pion = 3.;
    kMax_NSigma_Kaon = 3.;
    kMax_NSigma_Proton = 3.;

    kMax_Track_Eta = 1.;
    kMin_Track_NTPCClusters = 50;
    kMax_Track_Chi2PerNTPCClusters = 2.;
    kTurnedOn_Track_StatusCuts = kFALSE;
    kTurnedOn_Track_RejectKinks = kTRUE;
    kMin_Track_DCAxy_wrtPV = 2.;  // PENDING: could be improved for pions with DCAxy > 4 ??
}

/*                  */
/**  MC Generated  **/
/*** ============ ***/

/*
 * Loop over MC particles in a single event. Store the indices of the signal particles.
 */
void AliAnalysisTaskEsd2Tree::ProcessMCGen() {

    AliMCParticle* mcPart;

    /* Auxiliary variables */

    TString generator_name;
    Int_t pdg_mc;

    Int_t mother_idx;
    Int_t n_daughters;
    Int_t idx_first_dau;
    Int_t idx_last_dau;
    Int_t ancestor_idx;

    Bool_t is_signal;
    Bool_t is_secondary;
    Int_t reaction_id;

    /* V0 properties */

    AliMCParticle* mcDaughter;
    Int_t pdg_dau;

    Int_t mcIdxNegDaughter;
    AliMCParticle* mcNegDau;

    Int_t mcIdxPosDaughter;
    AliMCParticle* mcPosDau;

    Math::XYZVector NegDaughter;
    Math::XYZVector PosDaughter;

    Math::XYZPoint OriginVertex;
    Math::XYZPoint DecayVertex;

    Double_t dca_wrt_pv, cpa_wrt_pv;
    Double_t armenteros_alpha, armenteros_qt;

    /* Loop over MC gen. particles in this event */

    for (Int_t mcIdx = 0; mcIdx < fMC->GetNumberOfTracks(); mcIdx++) {

        /* Protection: exclude particles from the anti-neutron injector */
        /* Note: `GetCocktailGenerator` not only considers primaries, but subsequent daughters/secondaries all the way down, as well */

        fMC->GetCocktailGenerator(mcIdx, generator_name);
        if (generator_name.Contains("Injector (Anti-Neutron)")) continue;

        /* Get MC particle info */

        mcPart = (AliMCParticle*)fMC->GetTrack(mcIdx);

        pdg_mc = mcPart->PdgCode();

        mother_idx = mcPart->GetMother();
        GetAncestor(mcIdx, ancestor_idx);
        n_daughters = mcPart->GetNDaughters();
        idx_first_dau = mcPart->GetDaughterFirst();
        idx_last_dau = mcPart->GetDaughterLast();

        /* Check if particle is signal */
        /* - It should be produced by the "Anti-Sexaquark Interaction (Channel %c)" generator */
        /* - Then, the unique reaction ID is provided by MCStatusCode of the injected signal particles */

        is_signal = generator_name.Contains("Anti-Sexaquark Interaction");
        reaction_id = is_signal ? (mother_idx < 0 ? mcPart->MCStatusCode() : getReactionID_fromMcIdx[mother_idx]) : -1;

        /* Check if particle is secondary */
        /* - It includes signal particles too, because despite they were injected as primaries, they are really not */

        is_secondary = mcPart->IsSecondaryFromMaterial() || mcPart->IsSecondaryFromWeakDecay() || is_signal;

        /* Store info into containers */

        getPdgCode_fromMcIdx[mcIdx] = pdg_mc;
        getMotherMcIdx_fromMcIdx[mcIdx] = mother_idx;
        getAncestorMcIdx_fromMcIdx[mcIdx] = ancestor_idx;
        isMcIdxSignal[mcIdx] = is_signal;
        getReactionID_fromMcIdx[mcIdx] = reaction_id;
        getMcIndices_fromReactionID[reaction_id].push_back(mcIdx);
        isMcIdxSecondary[mcIdx] = is_secondary;
        doesMcIdxHaveMother[mcIdx] = mother_idx >= 0;

        /* Assign branches */

        tMC_Idx = mcIdx;
        tMC_PdgCode = pdg_mc;
        tMC_Idx_Mother = mother_idx;
        tMC_NDaughters = n_daughters;
        tMC_Idx_FirstDau = idx_first_dau;
        tMC_Idx_LastDau = idx_last_dau;
        tMC_Px = (Float_t)mcPart->Px();
        tMC_Py = (Float_t)mcPart->Py();
        tMC_Pz = (Float_t)mcPart->Pz();
        tMC_Xv = (Float_t)mcPart->Xv();
        tMC_Yv = (Float_t)mcPart->Yv();
        tMC_Zv = (Float_t)mcPart->Zv();
        tMC_Status = mcPart->MCStatusCode();
        tMC_IsSecondary = is_secondary;
        tMC_IsSignal = is_signal;
        tMC_ReactionID = reaction_id;

        /* Charged particles: (anti-)protons, kaons, pions */

        if (TMath::Abs(pdg_mc) == 2212 || TMath::Abs(pdg_mc) == 321 || TMath::Abs(pdg_mc) == 211) {

            /** Fill MC tree **/

            fTree_MC->Fill();
            ClearMCParticlesBranches();
        }  // end of (anti-)protons, kaons, pions condition

        /* Neutral particles: (anti-)lambdas and K0S */

        if (TMath::Abs(pdg_mc) == 3122 || pdg_mc == 310) {

            /** Fill MC tree **/

            fTree_MC->Fill();
            ClearMCParticlesBranches();

            /** Check if V0 can be reconstructed **/
            /** i.e.: it decays (1) into their most probable decay charged channel (2) **/

            n_daughters = mcPart->GetNDaughters();
            if (!n_daughters) continue;

            mcIdxNegDaughter = 0;
            mcIdxPosDaughter = 0;

            // loop over daughters and fill mcIdxNegDaughter and mcIdxPosDaughter in case of relevant decays
            for (Int_t mcIdxDaughter = mcPart->GetDaughterFirst(); mcIdxDaughter <= mcPart->GetDaughterLast(); mcIdxDaughter++) {
                mcDaughter = (AliMCParticle*)fMC->GetTrack(mcIdxDaughter);
                pdg_dau = mcDaughter->PdgCode();
                if (mcDaughter->GetMother() != mcIdx) continue;
                if (pdg_mc == 3122) {
                    if (pdg_dau == -211) mcIdxNegDaughter = mcIdxDaughter;
                    if (pdg_dau == 2212) mcIdxPosDaughter = mcIdxDaughter;
                }
                if (pdg_mc == -3122) {
                    if (pdg_dau == -2212) mcIdxNegDaughter = mcIdxDaughter;
                    if (pdg_dau == 211) mcIdxPosDaughter = mcIdxDaughter;
                }
                if (pdg_mc == 310) {
                    if (pdg_dau == -211) mcIdxNegDaughter = mcIdxDaughter;
                    if (pdg_dau == 211) mcIdxPosDaughter = mcIdxDaughter;
                }
            }

            if (!mcIdxNegDaughter || !mcIdxPosDaughter) continue;

            /* Store info into containers */

            getNegDauMcIdx_fromMcIdx[mcIdx] = mcIdxNegDaughter;
            getPosDauMcIdx_fromMcIdx[mcIdx] = mcIdxPosDaughter;
        }  // end of (anti-)lambda && K0S condition
    }      // end of loop over MC particles
}

/*
 * Rercursive function to get the first particle in the decay chain of a particle
 */
void AliAnalysisTaskEsd2Tree::GetAncestor(Int_t mcIdx, Int_t& ancestorIdx, Int_t generation) {

    AliMCParticle* thisMcParticle = (AliMCParticle*)fMC->GetTrack(mcIdx);
    Int_t motherIdx = thisMcParticle->GetMother();

    if (motherIdx < 0) {
        ancestorIdx = generation ? mcIdx : -1;
        return;
    }

    GetAncestor(motherIdx, ancestorIdx, generation + 1);
}

/*                   */
/**  Reconstructed  **/
/*** ============= ***/

/*
 * Apply event selection
 */
Bool_t AliAnalysisTaskEsd2Tree::PassesEventSelection() {

    /* Reference: https://twiki.cern.ch/twiki/bin/viewauth/ALICE/AliDPGRunList18r1 */
    if (!fIsMC && (fRunNumber == 296749 || fRunNumber == 296750 || fRunNumber == 296849 || fRunNumber == 296890 || fRunNumber == 297029 ||
                   fRunNumber == 297194 || fRunNumber == 297219 || fRunNumber == 297481)) {
        fEventCuts.UseTimeRangeCut();
        fEventCuts.OverrideAutomaticTriggerSelection(AliVEvent::kAny);  // PENDING: remove kAny?
        if (!fEventCuts.AcceptEvent(fESD)) return kFALSE;
    }

    /* Trigger Selection */

    Bool_t IsMB = kFALSE;
    Bool_t IsHighMultV0 = kFALSE;
    Bool_t IsHighMultSPD = kFALSE;
    Bool_t IsCentral = kFALSE;
    Bool_t IsSemiCentral = kFALSE;

    if (fInputHandler->IsEventSelected() & AliVEvent::kINT7) IsMB = kTRUE;
    if (fInputHandler->IsEventSelected() & AliVEvent::kHighMultV0) IsHighMultV0 = kTRUE;
    if (fInputHandler->IsEventSelected() & AliVEvent::kHighMultSPD) IsHighMultSPD = kTRUE;
    if (fInputHandler->IsEventSelected() & AliVEvent::kCentral) IsCentral = kTRUE;
    if (fInputHandler->IsEventSelected() & AliVEvent::kSemiCentral) IsSemiCentral = kTRUE;

    if (!IsMB && !IsHighMultV0 && !IsHighMultSPD && !IsCentral && !IsSemiCentral) {
        AliInfoF("Event Rejected -- %u %u %u %u", fInputHandler->IsEventSelected(), AliVEvent::kINT7, AliVEvent::kCentral,
                 AliVEvent::kSemiCentral);  // DEBUG
        return kFALSE;
    }
    AliInfoF("Event Selected -- %u %u %u %u", fInputHandler->IsEventSelected(), AliVEvent::kINT7, AliVEvent::kCentral,
             AliVEvent::kSemiCentral);  // DEBUG

    /* rec. PV z-vertex range */

    if (TMath::Abs(fPrimaryVertex->GetZ()) > 12.) return kFALSE;  // PENDING: hardcoded? not a good idea
    AliInfo("Event Passed z-vertex range on PV");                 // DEBUG

    return kTRUE;
}

/*
 * Loop over the reconstructed tracks in a single event.
 */
void AliAnalysisTaskEsd2Tree::ProcessTracks() {

    AliESDtrack* track;
    AliExternalTrackParam* trackInnerParam;

    /* MC variables */

    Int_t mcIdx;
    Int_t motherMcIdx;
    Int_t mcPdgCode;
    Bool_t is_secondary;
    Bool_t is_signal;
    Int_t reaction_id;
    TString generator_name;

    /* Auxiliary variables */

    Float_t dca_wrt_pv;
    Float_t dca_xy_wrt_pv, dca_z_wrt_pv;

    Float_t n_sigma_pion;
    Float_t n_sigma_kaon;
    Float_t n_sigma_proton;
    Float_t n_sigma;

    /* QA variables */

    Float_t n_tpc_clusters;
    Float_t n_crossed_rows;
    Float_t chi2_over_nclusters;
    Float_t n_findable_clusters, n_shared_clusters;
    Float_t chi2_over_ncls;
    UInt_t nSelectedTracks = 0;

    /* Loop over tracks in a single event */

    for (Int_t esdIdx = 0; esdIdx < fESD->GetNumberOfTracks(); esdIdx++) {

        /* Get track */

        track = fESD->GetTrack(esdIdx);
        trackInnerParam = const_cast<AliExternalTrackParam*>(track->GetInnerParam());

        /* Get true MC info */

        if (fIsMC) {
            mcIdx = TMath::Abs(track->GetLabel());
            mcPdgCode = getPdgCode_fromMcIdx[mcIdx];
            motherMcIdx = getMotherMcIdx_fromMcIdx[mcIdx];
            is_secondary = isMcIdxSecondary[mcIdx];
            is_signal = isMcIdxSignal[mcIdx];
            reaction_id = getReactionID_fromMcIdx[mcIdx];

            /** Protection: exclude particles from the anti-neutron injector **/
            /** Note: `GetCocktailGenerator` considers subsequent daughters/secondaries all the way down, too **/

            fMC->GetCocktailGenerator(mcIdx, generator_name);
            if (generator_name.Contains("Injector (Anti-Neutron)")) continue;
        }

        /* Cuts (1) : Pre-Selection */

        if (!trackInnerParam) continue;

        if (trackInnerParam->Pt() < 1E-3 || trackInnerParam->Pt() > 1E3) continue;

        /* Cuts (2) : Track Selection */

        if (!PassesTrackSelection(track, is_secondary, is_signal)) continue;

        /* Store info into containers (1) */

        getMcIdx_fromEsdIdx[esdIdx] = mcIdx;

        /* Get track properties */

        n_sigma_proton = fPIDResponse->NumberOfSigmasTPC(track, AliPID::kProton);
        n_sigma_kaon = fPIDResponse->NumberOfSigmasTPC(track, AliPID::kKaon);
        n_sigma_pion = fPIDResponse->NumberOfSigmasTPC(track, AliPID::kPion);

        track->GetImpactParameters(dca_xy_wrt_pv, dca_z_wrt_pv);  // pre-calculated DCA w.r.t. PV
        dca_xy_wrt_pv = TMath::Abs(dca_xy_wrt_pv);
        dca_z_wrt_pv = TMath::Abs(dca_z_wrt_pv);
        dca_wrt_pv = TMath::Sqrt(dca_xy_wrt_pv * dca_xy_wrt_pv + dca_z_wrt_pv * dca_z_wrt_pv);
        n_tpc_clusters = track->GetTPCNcls();  // Note: capital N
        n_crossed_rows = track->GetTPCCrossedRows();
        n_findable_clusters = (Int_t)track->GetTPCNclsF();
        n_shared_clusters = (Int_t)track->GetTPCnclsS();
        chi2_over_ncls = n_tpc_clusters > 1E-4 ? track->GetTPCchi2() / (Double_t)n_tpc_clusters : 999.;

        /* Assign branches and fill tree */

        tTrack_Idx = esdIdx;
        tTrack_Px = trackInnerParam->Px();
        tTrack_Py = trackInnerParam->Py();
        tTrack_Pz = trackInnerParam->Pz();
        tTrack_Charge = trackInnerParam->Charge();
        tTrack_NSigmaPion = n_sigma_pion;
        tTrack_NSigmaKaon = n_sigma_kaon;
        tTrack_NSigmaProton = n_sigma_proton;
        tTrack_TPCFitMap = track->GetTPCFitMap();
        tTrack_TPCClusterMap = track->GetTPCClusterMap();
        tTrack_TPCSharedMap = track->GetTPCSharedMap();
        tTrack_IsKinkDaughter = track->GetKinkIndex(0) > 0;
        tTrack_ITSin = (track->GetStatus() & AliESDtrack::kITSin);
        tTrack_ITSout = (track->GetStatus() & AliESDtrack::kITSout);
        tTrack_ITSrefit = (track->GetStatus() & AliESDtrack::kITSrefit);
        tTrack_ITSpid = (track->GetStatus() & AliESDtrack::kITSpid);
        tTrack_TPCin = (track->GetStatus() & AliESDtrack::kTPCin);
        tTrack_TPCout = (track->GetStatus() & AliESDtrack::kTPCout);
        tTrack_TPCrefit = (track->GetStatus() & AliESDtrack::kTPCrefit);
        tTrack_TPCpid = (track->GetStatus() & AliESDtrack::kTPCpid);

        if (fIsMC) {
            tTrack_Idx_True = mcIdx;
            tTrack_True_PdgCode = mcPdgCode;
            tTrack_IsSecondary = is_secondary;
            tTrack_IsSignal = is_signal;
            tTrack_ReactionID = reaction_id;
        }

        fTree_Tracks->Fill();
        ClearTracksBranches();

        /* Loop over PID hypotheses */

        for (AliPID::EParticleType pidHypothesis : {AliPID::kProton, AliPID::kKaon, AliPID::kPion}) {

            /* Cuts (3) : PID */

            n_sigma = fPIDResponse->NumberOfSigmasTPC(track, pidHypothesis);

            if (pidHypothesis == AliPID::kProton && TMath::Abs(n_sigma) > kMax_NSigma_Proton) continue;
            if (pidHypothesis == AliPID::kKaon && TMath::Abs(n_sigma) > kMax_NSigma_Kaon) continue;
            if (pidHypothesis == AliPID::kPion && TMath::Abs(n_sigma) > kMax_NSigma_Pion) continue;

            /** Convert PID hypothesis into PDG Code **/

            Int_t esdPdgCode;
            if (pidHypothesis == AliPID::kProton) esdPdgCode = 2212;
            if (pidHypothesis == AliPID::kKaon) esdPdgCode = 321;
            if (pidHypothesis == AliPID::kPion) esdPdgCode = 211;
            esdPdgCode *= trackInnerParam->Charge() < 0 ? -1 : 1;

            /* Cuts (4) : Species-Dedicated Selection */

            if (!PassesSpeciesSelection(track, esdPdgCode, is_secondary, is_signal)) continue;
        }  // end of loop over PID hypotheses
    }      // end of loop over tracks
}

/*
 * Check if track passes selection and fill bookkeeping histograms.
 */
Bool_t AliAnalysisTaskEsd2Tree::PassesTrackSelection(AliESDtrack* track, Bool_t isSecondary, Bool_t isSignal) {

    AliExternalTrackParam* trackInnerParam = const_cast<AliExternalTrackParam*>(track->GetInnerParam());

    /* Fulfill at least one of the PID cuts */

    Float_t n_sigma_proton = fPIDResponse->NumberOfSigmasTPC(track, AliPID::kProton);
    Float_t n_sigma_kaon = fPIDResponse->NumberOfSigmasTPC(track, AliPID::kKaon);
    Float_t n_sigma_pion = fPIDResponse->NumberOfSigmasTPC(track, AliPID::kPion);
    if (TMath::Abs(n_sigma_proton) > kMax_NSigma_Proton && TMath::Abs(n_sigma_kaon) > kMax_NSigma_Kaon &&
        TMath::Abs(n_sigma_pion) > kMax_NSigma_Pion) {
        return kFALSE;
    }

    if (kMax_Track_Eta && TMath::Abs(trackInnerParam->Eta()) > kMax_Track_Eta) return kFALSE;

    Double_t NTPCClusters = track->GetTPCNcls();
    if (kMin_Track_NTPCClusters && NTPCClusters < kMin_Track_NTPCClusters) return kFALSE;

    Double_t Chi2PerNTPCClusters = NTPCClusters > 1E-4 ? track->GetTPCchi2() / (Double_t)track->GetTPCNcls() : 999;
    if (kMax_Track_Chi2PerNTPCClusters && Chi2PerNTPCClusters > kMax_Track_Chi2PerNTPCClusters) return kFALSE;

    // >> TPC and ITS status
    Bool_t tpc_status =
        (track->GetStatus() & AliESDtrack::kTPCin) && (track->GetStatus() & AliESDtrack::kTPCout) && (track->GetStatus() & AliESDtrack::kTPCrefit);
    if (kTurnedOn_Track_StatusCuts && !tpc_status) return kFALSE;

    Bool_t its_status =
        !(track->GetStatus() & AliESDtrack::kITSin) && !(track->GetStatus() & AliESDtrack::kITSout) && !(track->GetStatus() & AliESDtrack::kITSrefit);
    if (kTurnedOn_Track_StatusCuts && !its_status) return kFALSE;

    if (kTurnedOn_Track_RejectKinks && track->GetKinkIndex(0) > 0) return kFALSE;

    Float_t DCAxy_wrtPV, DCAz_wrtPV;
    track->GetImpactParameters(DCAxy_wrtPV, DCAz_wrtPV);
    Float_t DCA_wrtPV = TMath::Sqrt((Double_t)DCAxy_wrtPV * (Double_t)DCAxy_wrtPV + (Double_t)DCAz_wrtPV * (Double_t)DCAz_wrtPV);
    if (kMin_Track_DCA_wrtPV && DCA_wrtPV < kMin_Track_DCA_wrtPV) return kFALSE;

    if (kMin_Track_DCAxy_wrtPV && TMath::Abs(DCAxy_wrtPV) < kMin_Track_DCAxy_wrtPV) return kFALSE;

    if (kMin_Track_DCAz_wrtPV && TMath::Abs(DCAz_wrtPV) < kMin_Track_DCAz_wrtPV) return kFALSE;

    return kTRUE;
}

/*
 * Check if track passes cuts dedicated to their hypothesized species.
 */
Bool_t AliAnalysisTaskEsd2Tree::PassesSpeciesSelection(AliESDtrack* track, Int_t esdPdgCode, Bool_t isSecondary, Bool_t isSignal) {

    AliExternalTrackParam* trackInnerParam = const_cast<AliExternalTrackParam*>(track->GetInnerParam());

    if (kMin_Track_Pt.count(esdPdgCode) && trackInnerParam->Pt() < kMin_Track_Pt[esdPdgCode]) return kFALSE;

    return kTRUE;
}

/*
 * Clear all containers.
 */
void AliAnalysisTaskEsd2Tree::ClearContainers() {

    getPdgCode_fromMcIdx.clear();

    isMcIdxSignal.clear();
    isMcIdxSecondary.clear();

    getReactionID_fromMcIdx.clear();
    getMcIndices_fromReactionID.clear();

    doesMcIdxHaveMother.clear();
    getMotherMcIdx_fromMcIdx.clear();
    getAncestorMcIdx_fromMcIdx.clear();
    getNegDauMcIdx_fromMcIdx.clear();
    getPosDauMcIdx_fromMcIdx.clear();

    getMcIdx_fromEsdIdx.clear();
}

/*                    */
/**  External Files  **/
/*** ============== ***/

/*
 *
 */
void AliAnalysisTaskEsd2Tree::PrepareInjectedBranches() {
    fTree_Injected->Branch("RunNumber", &tInjected_RunNumber);
    fTree_Injected->Branch("DirNumber", &tInjected_DirNumber);
    fTree_Injected->Branch("EventNumber", &tInjected_EventNumber);
    fTree_Injected->Branch("ReactionID", &tInjected_ReactionID);
    fTree_Injected->Branch("Px", &tInjected_Px);
    fTree_Injected->Branch("Py", &tInjected_Py);
    fTree_Injected->Branch("Pz", &tInjected_Pz);
    fTree_Injected->Branch("Mass", &tInjected_Mass);
    fTree_Injected->Branch("Nucleon_PdgCode", &tInjected_Nucleon_PdgCode);
    fTree_Injected->Branch("Nucleon_Px", &tInjected_Nucleon_Px);
    fTree_Injected->Branch("Nucleon_Py", &tInjected_Nucleon_Py);
    fTree_Injected->Branch("Nucleon_Pz", &tInjected_Nucleon_Pz);
    fTree_Injected->Branch("ReactionChannel", &tInjected_ReactionChannel);
}

/*
 * Open the respective `sim.log` that corresponds to the `RunNumber+DirNumber` that's being analyzed.
 * From it, read the injected anti-sexaquark and struck nucleon kinematics and store them into a tree.
 * Note: it must be executed ONLY when analyzing SIGNAL SIMs, protection PENDING!
 */
Bool_t AliAnalysisTaskEsd2Tree::ReadSignalLogs() {

    TGrid* alien = nullptr;
    if (!gGrid) {
        alien = TGrid::Connect("alien://");
        if (!alien) return kFALSE;
    }

    TString AliEn_Dir = fAliEnPath(0, fAliEnPath.Last('/'));

    TString orig_path = Form("%s/sim.log", AliEn_Dir.Data());
    AliInfoF("!! Copying file %s ... !!", orig_path.Data());

    /* assuming path ends with format `.../LHC23l1a3/A1.73/297595/001/sim.log` */
    Int_t AliEn_DirNumber = static_cast<Int_t>(fDirNumber);
    TObjArray* tokens = fAliEnPath.Tokenize("/");
    Int_t AliEn_RunNumber = ((TObjString*)tokens->At(tokens->GetEntries() - 3))->GetString().Atoi();
    TString AliEn_SimSubSet = ((TObjString*)tokens->At(tokens->GetEntries() - 4))->GetString();

    TString Log_NewBasename = Form("sim_%s_%i_%03i.log", AliEn_SimSubSet.Data(), AliEn_RunNumber, AliEn_DirNumber);

    if (AliEn_Dir.BeginsWith("alien://")) {
        gSystem->Exec(Form("alien.py cp %s file://./%s", orig_path.Data(), Log_NewBasename.Data()));
    } else {
        gSystem->Exec(Form("cp %s ./%s", orig_path.Data(), Log_NewBasename.Data()));
    }

    TString new_path = Form("%s/%s", gSystem->pwd(), Log_NewBasename.Data());
    AliInfoF("!! Reading file %s ... !!", new_path.Data());

    std::ifstream SimLog(new_path);
    if (!SimLog.is_open()) {
        AliInfo("!! Unable to open file !!");
        return kFALSE;
    }

    Int_t CurrentEventNumber = -1;

    // auxiliary variables
    std::string cstr_line;
    TString tstr_line, csv;
    TObjArray* csv_arr = nullptr;

    Char_t aux_char;

    /* Read lines */

    while (std::getline(SimLog, cstr_line)) {

        tstr_line = cstr_line;

        // a new event has appeared
        if (tstr_line.Contains("I-AliGenCocktail::Generate: Generator 1: AliGenHijing")) CurrentEventNumber++;

        if (!tstr_line.Contains("I-AliGenSexaquarkReaction::GenerateN: 6")) continue;

        csv = static_cast<TString>(tstr_line(38, tstr_line.Length() - 1));
        csv_arr = csv.Tokenize(",");

        /* Assign branches and fill tree */

        tInjected_RunNumber = AliEn_RunNumber;
        tInjected_DirNumber = AliEn_DirNumber;  // PENDING: could be another var to simplify things
        tInjected_EventNumber = CurrentEventNumber;
        tInjected_ReactionID = dynamic_cast<TObjString*>(csv_arr->At(0))->String().Atoi();
        tInjected_Px = dynamic_cast<TObjString*>(csv_arr->At(1))->String().Atof();
        tInjected_Py = dynamic_cast<TObjString*>(csv_arr->At(2))->String().Atof();
        tInjected_Pz = dynamic_cast<TObjString*>(csv_arr->At(3))->String().Atof();
        tInjected_Mass = TString(AliEn_SimSubSet(1, 4)).Atof();
        tInjected_Nucleon_PdgCode = AliEn_SimSubSet[0] == 'A' ? 2112 : 2212;  // considering only ADEH
        tInjected_Nucleon_Px = dynamic_cast<TObjString*>(csv_arr->At(4))->String().Atof();
        tInjected_Nucleon_Py = dynamic_cast<TObjString*>(csv_arr->At(5))->String().Atof();
        tInjected_Nucleon_Pz = dynamic_cast<TObjString*>(csv_arr->At(6))->String().Atof();
        tInjected_ReactionChannel = (UInt_t)AliEn_SimSubSet[0];

        fTree_Injected->Fill();
        ClearInjectedBranches();
    }  // end of loop over lines

    AliInfo("!! Closing file ... !!");
    SimLog.close();

    return kTRUE;
}

/*
 *
 */
void AliAnalysisTaskEsd2Tree::ClearInjectedBranches() {
    tInjected_RunNumber = 0;
    tInjected_DirNumber = 0;
    tInjected_EventNumber = 0;
    tInjected_ReactionID = 0;
    tInjected_Px = 0.;
    tInjected_Py = 0.;
    tInjected_Pz = 0.;
    tInjected_Mass = 0.;
    tInjected_Nucleon_PdgCode = 0;
    tInjected_Nucleon_Px = 0.;
    tInjected_Nucleon_Py = 0.;
    tInjected_Nucleon_Pz = 0.;
    tInjected_ReactionChannel = 0;
}
