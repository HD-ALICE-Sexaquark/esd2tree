#include "AliAnalysisTaskEsd2Tree.h"

ClassImp(AliAnalysisTaskEsd2Tree);

/*
 * Empty I/O constructor. Non-persistent members are initialized to their default values from here.
 */
AliAnalysisTaskEsd2Tree::AliAnalysisTaskEsd2Tree()
    : AliAnalysisTaskSE(""),
      /*  */
      fIsMC(kFALSE),
      fIsSignalMC(kFALSE),
      /*  */
      fMC(nullptr),
      fMC_PrimaryVertex(nullptr),
      fESD(nullptr),
      fPrimaryVertex(nullptr),
      fPIDResponse(nullptr),
      fEventCuts(),
      fMagneticField(0.),
      /*  */
      fRunNumber(0),
      fDirNumber(0.),
      fEventNumber(0),
      fCentrality(0.),
      /*  */
      fAliEnPath(""),
      fIsFirstEvent(kFALSE),
      fReactionChannel(""),
      /*  */
      fTree_Events(nullptr),
      fTree_Injected(nullptr),
      fTree_MC(nullptr),
      fTree_Tracks(nullptr),
      /*  */
      tEvents_PV_TrueXv(0.),
      tEvents_PV_TrueYv(0.),
      tEvents_PV_TrueZv(0.),
      tEvents_IsGenPileup(kFALSE),
      tEvents_IsSBCPileup(kFALSE),
      tEvents_PV_RecXv(0.),
      tEvents_PV_RecYv(0.),
      tEvents_PV_RecZv(0.),
      tEvents_PV_NContributors(0),
      tEvents_PV_ZvErr_FromSPD(0.),
      tEvents_PV_ZvErr_FromTracks(0.),
      tEvents_PV_Zv_FromSPD(0.),
      tEvents_PV_Zv_FromTracks(0.),
      tEvents_PV_Dispersion(0.),
      tEvents_NTracks(0),
      tEvents_NTPCClusters(0),
      tEvents_IsMB(kFALSE),
      tEvents_IsHighMultV0(kFALSE),
      tEvents_IsHighMultSPD(kFALSE),
      tEvents_IsCentral(kFALSE),
      tEvents_IsSemiCentral(kFALSE),
      /*  */
      tInjected_RunNumber(0),
      tInjected_DirNumber(0),
      tInjected_EventNumber(0),
      tInjected_ReactionID(0),
      tInjected_Px(0.),
      tInjected_Py(0.),
      tInjected_Pz(0.),
      tInjected_Mass(0.),
      tInjected_Nucleon_PdgCode(0),
      tInjected_Nucleon_Px(0.),
      tInjected_Nucleon_Py(0.),
      tInjected_Nucleon_Pz(0.),
      tInjected_ReactionChannel(0),
      /*  */
      tMC_Idx(0),
      tMC_PdgCode(0),
      tMC_Idx_Mother(0),
      tMC_NDaughters(0),
      tMC_Idx_FirstDau(0),
      tMC_Idx_LastDau(0),
      tMC_Idx_Ancestor(0),
      tMC_Px(0.),
      tMC_Py(0.),
      tMC_Pz(0.),
      tMC_Xv(0.),
      tMC_Yv(0.),
      tMC_Zv(0.),
      tMC_Status(0),
      tMC_IsOOBPileup(kFALSE),
      tMC_Generator(0),
      tMC_IsSecondary(kFALSE),
      tMC_IsSignal(kFALSE),
      tMC_ReactionID(0),
      /*  */
      tTrack_Idx(0),
      tTrack_Px(0.),
      tTrack_Py(0.),
      tTrack_Pz(0.),
      tTrack_Xv(0.),
      tTrack_Yv(0.),
      tTrack_Zv(0.),
      tTrack_Charge(0),
      tTrack_Alpha(0.),
      tTrack_SinAzimAngle(0.),
      tTrack_TanDipAngle(0.),
      tTrack_OneOverPt(0.),
      tTrack_CovMatrix(),
      tTrack_NSigmaPion(0.),
      tTrack_NSigmaKaon(0.),
      tTrack_NSigmaProton(0.),
      tTrack_DCAxy(0.),
      tTrack_DCAz(0.),
      tTrack_NTPCClusters(0),
      tTrack_NCrossedRows(0),
      tTrack_NFindableClusters(0),
      tTrack_NSharedClusters(0),
      tTrack_Chi2overNcls(0.),
      tTrack_IsKinkDaughter(kFALSE),
      // tTrack_TPCFitMap(0),
      // tTrack_TPCClusterMap(0),
      // tTrack_TPCSharedMap(0),
      tTrack_Idx_True(0),
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
      kMin_Track_DCAz_wrtPV(0.) {}

/*
 * Constructor, called locally.
 */
AliAnalysisTaskEsd2Tree::AliAnalysisTaskEsd2Tree(const char* name)
    : AliAnalysisTaskSE(name),
      /*  */
      fIsMC(kFALSE),
      fIsSignalMC(kFALSE),
      /*  */
      fMC(nullptr),
      fMC_PrimaryVertex(nullptr),
      fESD(nullptr),
      fPrimaryVertex(nullptr),
      fPIDResponse(nullptr),
      fEventCuts(),
      fMagneticField(0.),
      /*  */
      fRunNumber(0),
      fDirNumber(0.),
      fEventNumber(0),
      fCentrality(0.),
      /*  */
      fAliEnPath(""),
      fIsFirstEvent(kFALSE),
      fReactionChannel(""),
      /*  */
      fTree_Events(nullptr),
      fTree_Injected(nullptr),
      fTree_MC(nullptr),
      fTree_Tracks(nullptr),
      /*  */
      tEvents_PV_TrueXv(0.),
      tEvents_PV_TrueYv(0.),
      tEvents_PV_TrueZv(0.),
      tEvents_IsGenPileup(kFALSE),
      tEvents_IsSBCPileup(kFALSE),
      tEvents_PV_RecXv(0.),
      tEvents_PV_RecYv(0.),
      tEvents_PV_RecZv(0.),
      tEvents_PV_NContributors(0),
      tEvents_PV_ZvErr_FromSPD(0.),
      tEvents_PV_ZvErr_FromTracks(0.),
      tEvents_PV_Zv_FromSPD(0.),
      tEvents_PV_Zv_FromTracks(0.),
      tEvents_PV_Dispersion(0.),
      tEvents_NTracks(0),
      tEvents_NTPCClusters(0),
      tEvents_IsMB(kFALSE),
      tEvents_IsHighMultV0(kFALSE),
      tEvents_IsHighMultSPD(kFALSE),
      tEvents_IsCentral(kFALSE),
      tEvents_IsSemiCentral(kFALSE),
      /*  */
      tInjected_RunNumber(0),
      tInjected_DirNumber(0),
      tInjected_EventNumber(0),
      tInjected_ReactionID(0),
      tInjected_Px(0.),
      tInjected_Py(0.),
      tInjected_Pz(0.),
      tInjected_Mass(0.),
      tInjected_Nucleon_PdgCode(0),
      tInjected_Nucleon_Px(0.),
      tInjected_Nucleon_Py(0.),
      tInjected_Nucleon_Pz(0.),
      tInjected_ReactionChannel(0),
      /*  */
      tMC_Idx(0),
      tMC_PdgCode(0),
      tMC_Idx_Mother(0),
      tMC_NDaughters(0),
      tMC_Idx_FirstDau(0),
      tMC_Idx_LastDau(0),
      tMC_Idx_Ancestor(0),
      tMC_Px(0.),
      tMC_Py(0.),
      tMC_Pz(0.),
      tMC_Xv(0.),
      tMC_Yv(0.),
      tMC_Zv(0.),
      tMC_Status(0),
      tMC_IsOOBPileup(kFALSE),
      tMC_Generator(0),
      tMC_IsSecondary(kFALSE),
      tMC_IsSignal(kFALSE),
      tMC_ReactionID(0),
      /*  */
      tTrack_Idx(0),
      tTrack_Px(0.),
      tTrack_Py(0.),
      tTrack_Pz(0.),
      tTrack_Xv(0.),
      tTrack_Yv(0.),
      tTrack_Zv(0.),
      tTrack_Charge(0),
      tTrack_Alpha(0.),
      tTrack_SinAzimAngle(0.),
      tTrack_TanDipAngle(0.),
      tTrack_OneOverPt(0.),
      tTrack_CovMatrix(),
      tTrack_NSigmaPion(0.),
      tTrack_NSigmaKaon(0.),
      tTrack_NSigmaProton(0.),
      tTrack_DCAxy(0.),
      tTrack_DCAz(0.),
      tTrack_NTPCClusters(0),
      tTrack_NCrossedRows(0),
      tTrack_NFindableClusters(0),
      tTrack_NSharedClusters(0),
      tTrack_Chi2overNcls(0),
      tTrack_IsKinkDaughter(kFALSE),
      // tTrack_TPCFitMap(0),
      // tTrack_TPCClusterMap(0),
      // tTrack_TPCSharedMap(0),
      tTrack_Idx_True(0),
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
      kMin_Track_DCAz_wrtPV(0.) {
    DefineInput(0, TChain::Class());

    DefineOutput(1, TTree::Class());  // fTree_Events
    DefineOutput(2, TTree::Class());  // fTree_Injected
    DefineOutput(3, TTree::Class());  // fTree_MC
    DefineOutput(4, TTree::Class());  // fTree_Tracks
}

/*
 * Destructor.
 * Note: if `TList::SetOwner(kTRUE)` was called, the TList destructor should delete all objects added to it.
 */
AliAnalysisTaskEsd2Tree::~AliAnalysisTaskEsd2Tree() {
    if (fTree_Events) delete fTree_Events;
    if (fTree_Injected) delete fTree_Injected;
    if (fTree_MC) delete fTree_MC;
    if (fTree_Tracks) delete fTree_Tracks;
}

/*
 * Initialize analysis task. Needs to be called within an `AddTaskEsd2Tree.C` macro.
 */
void AliAnalysisTaskEsd2Tree::Initialize() {

    /* Define cuts */

    DefineTracksCuts("standard");

    /* Initialize PDG */

    AliInfo("Initializing...");
    AliInfo("Settings:");
    AliInfo("========");
    AliInfoF(">> IsMC       = %i", (Int_t)fIsMC);
    AliInfoF(">> IsSignalMC = %i", (Int_t)fIsSignalMC);
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

/*                         */
/**  Executed at runtime  **/
/*** =================== ***/

/*
 * Create output objects, called once at RUNTIME ~ execution on Grid.
 */
void AliAnalysisTaskEsd2Tree::UserCreateOutputObjects() {

    AliAnalysisManager* man = AliAnalysisManager::GetAnalysisManager();
    if (!man) AliFatal("ERROR: AliAnalysisManager couldn't be found.");

    AliESDInputHandler* inputHandler = (AliESDInputHandler*)(man->GetInputEventHandler());
    if (!inputHandler) AliFatal("ERROR: AliESDInputHandler couldn't be found.");

    fPIDResponse = inputHandler->GetPIDResponse();

    /* Prepare output trees */

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
 * Main function, called per each event at RUNTIME ~ execution on Grid.
 */
void AliAnalysisTaskEsd2Tree::UserExec(Option_t*) {

    /* Handle external logs */

    if (fIsFirstEvent) {
        if (fAliEnPath == "") AliWarning("No luck finding fAliEnPath");
        AliInfoF("fAliEnPath: %s", fAliEnPath.Data());
        if (fIsSignalMC) ReadSignalLogs();
        fIsFirstEvent = kFALSE;
    }

    /* Load MC Gen. Event and PV */

    if (fIsMC) {
        fMC = MCEvent();
        if (!fMC) AliFatal("ERROR: AliMCEvent couldn't be found.");
        fMC_PrimaryVertex = const_cast<AliVVertex*>(fMC->GetPrimaryVertex());
        ProcessMCGen();
    }

    /* Load Reconstructed Event, PV and Magnetic Field */

    fESD = dynamic_cast<AliESDEvent*>(InputEvent());
    if (!fESD) AliFatal("ERROR: AliESDEvent couldn't be found.");

    fPrimaryVertex = const_cast<AliESDVertex*>(fESD->GetPrimaryVertex());

    Double_t PV_CovMatrix[6];
    fPrimaryVertex->GetCovarianceMatrix(PV_CovMatrix);

    AliESDVertex* PrimaryVertex_SPD = const_cast<AliESDVertex*>(fESD->GetPrimaryVertexSPD());
    Double_t PV_SPD_CovMatrix[6];
    PrimaryVertex_SPD->GetCovarianceMatrix(PV_SPD_CovMatrix);

    fMagneticField = fESD->GetMagneticField();

    fRunNumber = fESD->GetRunNumber();
    fEventNumber = fESD->GetEventNumberInFile();

    /* Centrality */

    AliMultSelection* MultSelection = dynamic_cast<AliMultSelection*>(fESD->FindListObject("MultSelection"));
    if (!MultSelection) AliFatal("ERROR: AliMultSelection couldn't be found.");
    fCentrality = MultSelection->GetMultiplicityPercentile("V0M");
    AliInfoF("Centrality (from MultSelection) = %.2f", fCentrality);  // DEBUG

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
    ClearEventsBranches();

    /* Tracks */

    ProcessTracks();

    EndOfEvent();
}

/*
 *
 */
void AliAnalysisTaskEsd2Tree::EndOfEvent() {
    PostData(1, fTree_Events);
    PostData(2, fTree_Injected);
    PostData(3, fTree_MC);
    PostData(4, fTree_Tracks);
}

/*           */
/**  Trees  **/
/*** ===== ***/

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
 * Add branches to `fTree_Injected`.
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
    fTree_MC->Branch("Idx_Ancestor", &tMC_Idx_Ancestor);
    fTree_MC->Branch("Px", &tMC_Px);
    fTree_MC->Branch("Py", &tMC_Py);
    fTree_MC->Branch("Pz", &tMC_Pz);
    fTree_MC->Branch("Xv", &tMC_Xv);
    fTree_MC->Branch("Yv", &tMC_Yv);
    fTree_MC->Branch("Zv", &tMC_Zv);
    fTree_MC->Branch("Status", &tMC_Status);
    fTree_MC->Branch("IsOOBPileup", &tMC_IsOOBPileup);
    fTree_MC->Branch("Generator", &tMC_Generator);
    fTree_MC->Branch("IsSecondary", &tMC_IsSecondary);
    fTree_MC->Branch("IsSignal", &tMC_IsSignal);
    fTree_MC->Branch("ReactionID", &tMC_ReactionID);
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
    fTree_Tracks->Branch("Alpha", &tTrack_Alpha);
    fTree_Tracks->Branch("SinAzimAngle", &tTrack_SinAzimAngle);
    fTree_Tracks->Branch("TanDipAngle", &tTrack_TanDipAngle);
    fTree_Tracks->Branch("OneOverPt", &tTrack_OneOverPt);
    fTree_Tracks->Branch("CovMatrix", &tTrack_CovMatrix, "CovMatrix[15]/F");
    fTree_Tracks->Branch("NSigmaPion", &tTrack_NSigmaPion);
    fTree_Tracks->Branch("NSigmaKaon", &tTrack_NSigmaKaon);
    fTree_Tracks->Branch("NSigmaProton", &tTrack_NSigmaProton);
    fTree_Tracks->Branch("DCAxy", &tTrack_DCAxy);
    fTree_Tracks->Branch("DCAz", &tTrack_DCAz);
    fTree_Tracks->Branch("NTPCClusters", &tTrack_NTPCClusters);
    fTree_Tracks->Branch("NCrossedRows", &tTrack_NCrossedRows);
    fTree_Tracks->Branch("NFindableClusters", &tTrack_NFindableClusters);
    fTree_Tracks->Branch("NSharedClusters", &tTrack_NSharedClusters);
    fTree_Tracks->Branch("Chi2overNcls", &tTrack_Chi2overNcls);
    fTree_Tracks->Branch("IsKinkDaughter", &tTrack_IsKinkDaughter);
    // fTree_Tracks->Branch("TPCFitMap", &tTrack_TPCFitMap);
    // fTree_Tracks->Branch("TPCClusterMap", &tTrack_TPCClusterMap);
    // fTree_Tracks->Branch("TPCSharedMap", &tTrack_TPCSharedMap);
    fTree_Tracks->Branch("Idx_True", &tTrack_Idx_True);
}

/*
 * Clear branches content of `fTree_Events`
 */
void AliAnalysisTaskEsd2Tree::ClearEventsBranches() {
    // not clearing fRunNumber nor fDirNumber
    fEventNumber = 0;
    fCentrality = 100.5;

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
 * Clear branches content of `fTree_Injected`.
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

/*
 * Clear branches content of `fTree_MC`.
 */
void AliAnalysisTaskEsd2Tree::ClearMCParticlesBranches() {
    tMC_Idx = 0;
    tMC_PdgCode = 0;
    tMC_Idx_Mother = -1;
    tMC_NDaughters = 0;
    tMC_Idx_FirstDau = -1;
    tMC_Idx_LastDau = -1;
    tMC_Idx_Ancestor = -1;
    tMC_Px = 0.;
    tMC_Py = 0.;
    tMC_Pz = 0.;
    tMC_Xv = 0.;
    tMC_Yv = 0.;
    tMC_Zv = 0.;
    tMC_Status = 0;
    tMC_IsOOBPileup = kFALSE;
    tMC_Generator = 0;
    tMC_IsSecondary = kFALSE;
    tMC_IsSignal = kFALSE;
    tMC_ReactionID = -1;
}

/*
 * Clear branches content of `fTree_Tracks`.
 */
void AliAnalysisTaskEsd2Tree::ClearTracksBranches() {
    tTrack_Idx = 0;
    tTrack_Px = 0.;
    tTrack_Py = 0.;
    tTrack_Pz = 0.;
    tTrack_Xv = 0.;
    tTrack_Yv = 0.;
    tTrack_Zv = 0.;
    tTrack_Charge = 0;
    tTrack_Alpha = 0.;
    tTrack_SinAzimAngle = 0.;
    tTrack_TanDipAngle = 0.;
    tTrack_OneOverPt = 0.;
    for (Int_t i = 0; i < 15; i++) tTrack_CovMatrix[i] = 0.;
    tTrack_NSigmaPion = 0.;
    tTrack_NSigmaKaon = 0.;
    tTrack_NSigmaProton = 0.;
    tTrack_DCAxy = 0.;
    tTrack_DCAz = 0.;
    tTrack_NTPCClusters = 0;
    tTrack_NCrossedRows = 0;
    tTrack_NFindableClusters = 0;
    tTrack_NSharedClusters = 0;
    tTrack_Chi2overNcls = 0.;
    // tTrack_TPCFitMap.ResetAllBits();
    // tTrack_TPCClusterMap.ResetAllBits();
    // tTrack_TPCSharedMap.ResetAllBits();
    tTrack_IsKinkDaughter = kFALSE;
    tTrack_Idx_True = 0;
}

/*            */
/**  Events  **/
/*** ====== ***/

/*
 * Apply event selection.
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

/*                  */
/**  MC Generated  **/
/*** ============ ***/

/*
 * Loop over MC particles in a single event. Store the indices of the signal particles.
 */
void AliAnalysisTaskEsd2Tree::ProcessMCGen() {

    AliMCParticle* mcPart;

    Int_t pdg_code;
    Int_t ancestor_idx;
    Short_t generator;

    AliInfoF("Number of MC particles: %i", fMC->GetNumberOfTracks());

    for (Int_t mcIdx = 0; mcIdx < fMC->GetNumberOfTracks(); mcIdx++) {

        mcPart = (AliMCParticle*)fMC->GetTrack(mcIdx);
        if (!mcPart) continue;
        if (TMath::Abs(mcPart->Px()) > 1E6 || TMath::Abs(mcPart->Py()) > 1E6 || TMath::Abs(mcPart->Pz()) > 1E6) continue;
        if (TMath::Abs(mcPart->Px()) < 1E-6 || TMath::Abs(mcPart->Py()) < 1E-6 || TMath::Abs(mcPart->Pz()) < 1E-6) continue;

        pdg_code = mcPart->PdgCode();
        ancestor_idx = GetAncestor(mcIdx);
        generator = GetGenerator(mcIdx);

        /* Assign branches and fill tree */

        tMC_Idx = mcIdx;
        tMC_PdgCode = pdg_code;
        tMC_Idx_Mother = mcPart->GetMother();
        tMC_NDaughters = mcPart->GetNDaughters();
        tMC_Idx_FirstDau = mcPart->GetDaughterFirst();
        tMC_Idx_LastDau = mcPart->GetDaughterLast();
        tMC_Idx_Ancestor = ancestor_idx;
        tMC_Px = (Float_t)mcPart->Px();
        tMC_Py = (Float_t)mcPart->Py();
        tMC_Pz = (Float_t)mcPart->Pz();
        tMC_Xv = (Float_t)mcPart->Xv();
        tMC_Yv = (Float_t)mcPart->Yv();
        tMC_Zv = (Float_t)mcPart->Zv();
        tMC_Status = mcPart->MCStatusCode();
        tMC_IsOOBPileup = AliAnalysisUtils::IsParticleFromOutOfBunchPileupCollision(mcIdx, fMC);
        tMC_Generator = generator;
        tMC_IsSignal = generator == 2;
        tMC_IsSecondary = mcPart->IsSecondaryFromMaterial() || mcPart->IsSecondaryFromWeakDecay() || tMC_IsSignal;
        tMC_ReactionID = tMC_IsSignal ? GetReactionID(mcIdx) : -1;

        fTree_MC->Fill();
        ClearMCParticlesBranches();
    }  // end of loop over MC particles
}

/*
 * Rercursive function to get the first particle in the decay chain of a particle.
 */
Int_t AliAnalysisTaskEsd2Tree::GetAncestor(Int_t mcIdx, Int_t generation) {
    AliMCParticle* thisMcParticle = (AliMCParticle*)fMC->GetTrack(mcIdx);
    Int_t motherIdx = thisMcParticle->GetMother();
    if (motherIdx < 0) {
        if (!generation) return -1;
        return mcIdx;
    }
    return GetAncestor(motherIdx, generation + 1);
}

/*
 * Returns an index to the generator of the particle.
 * - 0 : HIJING
 * - 1 : Anti-Neutron Injector
 * - 2 : Anti-Sexaquark Injector
 */
Short_t AliAnalysisTaskEsd2Tree::GetGenerator(Int_t mcIdx) {
    TString generator_name;
    fMC->GetCocktailGenerator(mcIdx, generator_name);
    return 2 * generator_name.Contains("Anti-Sexaquark") + 1 * generator_name.Contains("Anti-Neutron");
}

/*
 * Recursive function to get the Reaction ID of a particle.
 */
Int_t AliAnalysisTaskEsd2Tree::GetReactionID(Int_t mcIdx) {
    AliMCParticle* mcPart = (AliMCParticle*)fMC->GetTrack(mcIdx);
    Int_t motherIdx = mcPart->GetMother();
    if (motherIdx < 0) return mcPart->MCStatusCode();
    return GetReactionID(motherIdx);
}

/*                   */
/**  Reconstructed  **/
/*** ============= ***/

/*
 * Loop over the reconstructed tracks in a single event.
 */
void AliAnalysisTaskEsd2Tree::ProcessTracks() {

    AliESDtrack* track;
    AliExternalTrackParam* trackInnerParam;

    Int_t mcIdx;
    Double_t momentum[3];
    Double_t position[3];

    for (Int_t esdIdx = 0; esdIdx < fESD->GetNumberOfTracks(); esdIdx++) {

        track = fESD->GetTrack(esdIdx);
        trackInnerParam = const_cast<AliExternalTrackParam*>(track->GetInnerParam());

        /* Track selection */

        if (!trackInnerParam) continue;
        if (trackInnerParam->Pt() < 1E-3 || trackInnerParam->Pt() > 1E3) continue;
        if (!PassesTrackSelection(track)) continue;

        /* Assign branches and fill tree */

        tTrack_Idx = esdIdx;
        trackInnerParam->GetPxPyPz(momentum);
        tTrack_Px = (Float_t)momentum[0];
        tTrack_Py = (Float_t)momentum[1];
        tTrack_Pz = (Float_t)momentum[2];
        trackInnerParam->GetXYZ(position);
        tTrack_Xv = (Float_t)position[0];
        tTrack_Yv = (Float_t)position[1];
        tTrack_Zv = (Float_t)position[2];
        tTrack_Charge = trackInnerParam->Charge();
        tTrack_Alpha = trackInnerParam->GetAlpha();
        tTrack_SinAzimAngle = trackInnerParam->GetParameter()[2];
        tTrack_TanDipAngle = trackInnerParam->GetParameter()[3];
        tTrack_OneOverPt = trackInnerParam->GetParameter()[4];
        const Double_t* covariance_matrix = trackInnerParam->GetCovariance();
        for (Int_t i = 0; i < 15; i++) tTrack_CovMatrix[i] = (Float_t)covariance_matrix[i];
        tTrack_NSigmaPion = fPIDResponse->NumberOfSigmasTPC(track, AliPID::kPion);
        tTrack_NSigmaKaon = fPIDResponse->NumberOfSigmasTPC(track, AliPID::kKaon);
        tTrack_NSigmaProton = fPIDResponse->NumberOfSigmasTPC(track, AliPID::kProton);
        track->GetImpactParameters(tTrack_DCAxy, tTrack_DCAz);
        tTrack_NTPCClusters = track->GetTPCNcls();  // Note: capital N
        tTrack_NCrossedRows = track->GetTPCCrossedRows();
        tTrack_NFindableClusters = track->GetTPCNclsF();
        tTrack_NSharedClusters = track->GetTPCnclsS();
        tTrack_Chi2overNcls = tTrack_NTPCClusters > 1E-4 ? static_cast<Float_t>(track->GetTPCchi2() / (Double_t)tTrack_NTPCClusters) : 999.;
        tTrack_IsKinkDaughter = track->GetKinkIndex(0) > 0;
        // tTrack_TPCFitMap = track->GetTPCFitMap();
        // tTrack_TPCClusterMap = track->GetTPCClusterMap();
        // tTrack_TPCSharedMap = track->GetTPCSharedMap();
        if (fIsMC) tTrack_Idx_True = TMath::Abs(track->GetLabel());

        fTree_Tracks->Fill();
        ClearTracksBranches();
    }  // end of loop over tracks
}

/*
 * Check if track passes selection and fill bookkeeping histograms.
 */
Bool_t AliAnalysisTaskEsd2Tree::PassesTrackSelection(AliESDtrack* track) {

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

/*                    */
/**  External Files  **/
/*** ============== ***/

/*
 * Open the respective `sim.log` that corresponds to the `RunNumber+DirNumber` that's being analyzed.
 * From it, read the injected anti-sexaquark and struck nucleon kinematics and store them into a tree.
 */
Bool_t AliAnalysisTaskEsd2Tree::ReadSignalLogs() {

    TGrid* alien = nullptr;
    if (!gGrid) {
        alien = TGrid::Connect("alien://");
        if (!alien) return kFALSE;
    }

    TString AliEn_Dir = fAliEnPath(0, fAliEnPath.Last('/'));

    TString orig_path = Form("%s/sim.log", AliEn_Dir.Data());
    AliInfoF("Copying file %s ...", orig_path.Data());

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
    AliInfoF("Reading file %s ...", new_path.Data());

    std::ifstream SimLog(new_path);
    if (!SimLog.is_open()) {
        AliInfo("Unable to open file");
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

    AliInfo("Closing file ...");
    SimLog.close();

    return kTRUE;
}
