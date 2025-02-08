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
      /*  */
      fSignalSimSet(""),
      fRunNumber(0),
      fDirNumber(0),
      fDirNumberB(0),
      fEventNumber(0),
      fCentrality(0.),
      fMagneticField(0.),
      /*  */
      fAliEnPath(""),
      fReactionChannel(""),
      fSignalLog_NewBasename(""),
      /*  */
      fOutputList(nullptr),
      fHist_Events_Bookkeeping(nullptr),
      fHist_Centrality(nullptr),
      fHist_CentralityINT7(nullptr),
      /*  */
      fOutputTree(nullptr),
      /*  */
      tEvent_PV_TrueXv(0.),
      tEvent_PV_TrueYv(0.),
      tEvent_PV_TrueZv(0.),
      tEvent_IsGenPileup(kFALSE),
      tEvent_IsSBCPileup(kFALSE),
      tEvent_PV_NContributors(0),
      tEvent_PV_Dispersion(0.),
      tEvent_PV_Xv(0.),
      tEvent_PV_Yv(0.),
      tEvent_PV_Zv(0.),
      tEvent_PV_CovMatrix(),
      tEvent_SPD_PV_Zv(0.),
      tEvent_SPD_PV_ZvErr(0.),
      tEvent_NTracks(0),
      tEvent_NTPCClusters(0),
      tEvent_IsMB(kFALSE),
      tEvent_IsHighMultV0(kFALSE),
      tEvent_IsHighMultSPD(kFALSE),
      tEvent_IsCentral(kFALSE),
      tEvent_IsSemiCentral(kFALSE),
      /*  */
      fMcEntry_(),
      fReactionID_(),
      fSexaquark_Px_(),
      fSexaquark_Py_(),
      fSexaquark_Pz_(),
      fNucleon_Px_(),
      fNucleon_Py_(),
      fNucleon_Pz_(),
      /*  */
      tInjected_ReactionID(0),
      tInjected_Px(0.),
      tInjected_Py(0.),
      tInjected_Pz(0.),
      tInjected_Nucleon_Px(0.),
      tInjected_Nucleon_Py(0.),
      tInjected_Nucleon_Pz(0.),
      /*  */
      tMC_PdgCode(0),
      tMC_Mother_McEntry(0),
      tMC_Px(0.),
      tMC_Py(0.),
      tMC_Pz(0.),
      tMC_Xv(0.),
      tMC_Yv(0.),
      tMC_Zv(0.),
      tMC_Status(0),
      tMC_IsOOBPileup(kFALSE),
      tMC_Generator(0),
      tMC_IsPrimary(kFALSE),
      tMC_IsSecFromMat(kFALSE),
      tMC_IsSecFromWeak(kFALSE),
      /*  */
      tTrack_Px(0.),
      tTrack_Py(0.),
      tTrack_Pz(0.),
      tTrack_X(0.),
      tTrack_Y(0.),
      tTrack_Z(0.),
      tTrack_Charge(0),
      tTrack_Alpha(0.),
      tTrack_Snp(0.),
      tTrack_Tgl(0.),
      tTrack_Signed1Pt(0.),
      tTrack_SigmaY2(0.),
      tTrack_SigmaZY(0.),
      tTrack_SigmaZ2(0.),
      tTrack_SigmaSnpY(0.),
      tTrack_SigmaSnpZ(0.),
      tTrack_SigmaSnp2(0.),
      tTrack_SigmaTglY(0.),
      tTrack_SigmaTglZ(0.),
      tTrack_SigmaTglSnp(0.),
      tTrack_SigmaTgl2(0.),
      tTrack_Sigma1PtY(0.),
      tTrack_Sigma1PtZ(0.),
      tTrack_Sigma1PtSnp(0.),
      tTrack_Sigma1PtTgl(0.),
      tTrack_Sigma1Pt2(0.),
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
      tTrack_McEntry(0),
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
      /*  */
      fSignalSimSet(""),
      fRunNumber(0),
      fDirNumber(0),
      fDirNumberB(0),
      fEventNumber(0),
      fCentrality(0.),
      fMagneticField(0.),
      /*  */
      fAliEnPath(""),
      fReactionChannel(""),
      fSignalLog_NewBasename(""),
      /*  */
      fOutputList(nullptr),
      fHist_Events_Bookkeeping(nullptr),
      fHist_Centrality(nullptr),
      fHist_CentralityINT7(nullptr),
      /*  */
      fOutputTree(nullptr),
      /*  */
      tEvent_PV_TrueXv(0.),
      tEvent_PV_TrueYv(0.),
      tEvent_PV_TrueZv(0.),
      tEvent_IsGenPileup(kFALSE),
      tEvent_IsSBCPileup(kFALSE),
      tEvent_PV_NContributors(0),
      tEvent_PV_Dispersion(0.),
      tEvent_PV_Xv(0.),
      tEvent_PV_Yv(0.),
      tEvent_PV_Zv(0.),
      tEvent_PV_CovMatrix(),
      tEvent_SPD_PV_Zv(0.),
      tEvent_SPD_PV_ZvErr(0.),
      tEvent_NTracks(0),
      tEvent_NTPCClusters(0),
      tEvent_IsMB(kFALSE),
      tEvent_IsHighMultV0(kFALSE),
      tEvent_IsHighMultSPD(kFALSE),
      tEvent_IsCentral(kFALSE),
      tEvent_IsSemiCentral(kFALSE),
      /*  */
      fMcEntry_(),
      fReactionID_(),
      fSexaquark_Px_(),
      fSexaquark_Py_(),
      fSexaquark_Pz_(),
      fNucleon_Px_(),
      fNucleon_Py_(),
      fNucleon_Pz_(),
      /*  */
      tInjected_ReactionID(0),
      tInjected_Px(0.),
      tInjected_Py(0.),
      tInjected_Pz(0.),
      tInjected_Nucleon_Px(0.),
      tInjected_Nucleon_Py(0.),
      tInjected_Nucleon_Pz(0.),
      /*  */
      tMC_PdgCode(0),
      tMC_Mother_McEntry(0),
      tMC_Px(0.),
      tMC_Py(0.),
      tMC_Pz(0.),
      tMC_Xv(0.),
      tMC_Yv(0.),
      tMC_Zv(0.),
      tMC_Status(0),
      tMC_IsOOBPileup(kFALSE),
      tMC_Generator(0),
      tMC_IsPrimary(kFALSE),
      tMC_IsSecFromMat(kFALSE),
      tMC_IsSecFromWeak(kFALSE),
      /*  */
      tTrack_Px(0.),
      tTrack_Py(0.),
      tTrack_Pz(0.),
      tTrack_X(0.),
      tTrack_Y(0.),
      tTrack_Z(0.),
      tTrack_Charge(0),
      tTrack_Alpha(0.),
      tTrack_Snp(0.),
      tTrack_Tgl(0.),
      tTrack_Signed1Pt(0.),
      tTrack_SigmaY2(0.),
      tTrack_SigmaZY(0.),
      tTrack_SigmaZ2(0.),
      tTrack_SigmaSnpY(0.),
      tTrack_SigmaSnpZ(0.),
      tTrack_SigmaSnp2(0.),
      tTrack_SigmaTglY(0.),
      tTrack_SigmaTglZ(0.),
      tTrack_SigmaTglSnp(0.),
      tTrack_SigmaTgl2(0.),
      tTrack_Sigma1PtY(0.),
      tTrack_Sigma1PtZ(0.),
      tTrack_Sigma1PtSnp(0.),
      tTrack_Sigma1PtTgl(0.),
      tTrack_Sigma1Pt2(0.),
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
      tTrack_McEntry(0),
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
    //
    DefineInput(0, TChain::Class());
    DefineOutput(1, TList::Class());  // fOutputList
    DefineOutput(2, TTree::Class());  // fOutputTree
}

/*
 * Destructor.
 * Note: if `TList::SetOwner(kTRUE)` was called, the TList destructor should delete all objects added to it.
 */
AliAnalysisTaskEsd2Tree::~AliAnalysisTaskEsd2Tree() {
    if (fOutputList) {
        fOutputList->Delete();
        delete fOutputList;
    }
    if (fOutputTree) delete fOutputTree;
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
    kTurnedOn_Track_StatusCuts = kTRUE;
    kTurnedOn_Track_RejectKinks = kFALSE;
    kMin_Track_DCAxy_wrtPV = 2.;
}

/*                         */
/**  Executed at runtime  **/
/*** =================== ***/

/*
 * Create output objects, called once at RUNTIME ~ execution on Grid.
 */
void AliAnalysisTaskEsd2Tree::UserCreateOutputObjects() {
    //
    AliAnalysisManager* man = AliAnalysisManager::GetAnalysisManager();
    if (!man) AliFatal("ERROR: AliAnalysisManager couldn't be found.");

    AliESDInputHandler* inputHandler = (AliESDInputHandler*)(man->GetInputEventHandler());
    if (!inputHandler) AliFatal("ERROR: AliESDInputHandler couldn't be found.");

    fPIDResponse = inputHandler->GetPIDResponse();

    /* Prepare output list */

    fOutputList = new TList();
    fOutputList->SetOwner(kTRUE);

    fHist_Events_Bookkeeping = new TH1F("Events_Bookkeeping", ";;Counts", 10, 0., 10.);
    fOutputList->Add(fHist_Events_Bookkeeping);

    fHist_Centrality = new TH1F("Centrality", ";Centrality;Counts", 11, 0., 110.);
    fOutputList->Add(fHist_Centrality);

    fHist_CentralityINT7 = new TH1F("CentralityINT7", ";CentralityINT7;Counts", 11, 0., 110.);
    fOutputList->Add(fHist_CentralityINT7);

    /* Prepare output tree */

    fOutputTree = new TTree("Events", "Events");

    AssociateEventsBranches();
    if (fIsMC) {
        if (fIsSignalMC) AssociateInjectedBranches();
        AssociateMCBranches();
    }
    AssociateTracksBranches();

    /* Post data */

    PostData(1, fOutputList);
    PostData(2, fOutputTree);
}

/*
 * User implementation of `Notify()`. Needed for reading the AliEn path.
 * This function is loaded during `AliAnalysisManager::Notify()`.
 * It's called after `UserCreateOutputObjects()`, for each new file, and before the first `UserExec()`.
 */
Bool_t AliAnalysisTaskEsd2Tree::UserNotify() {
    //
    AliAnalysisManager* man = AliAnalysisManager::GetAnalysisManager();
    if (!man) AliFatal("Analysis Manager not found");
    TTree* man_tree = man->GetTree();
    if (!man_tree) AliFatal("Analysis Manager Tree not found");
    TFile* man_file = man_tree->GetCurrentFile();
    if (!man_file) AliFatal("Analysis Manager File not found");

    /* get AliEn path and tokenize it */

    fAliEnPath = man_file->GetName();
    if (fAliEnPath == "") AliWarning("fAliEnPath couldn't be found.");
    AliInfoF("fAliEnPath: %s", fAliEnPath.Data());

    TObjArray* tokens = fAliEnPath.Tokenize("/");
    if (fIsMC) {
        /* path of signal MC ends with format `.../LHC23l1a3/A1.73/297595/001/AliESDs.root` */
        /* and path of general purpose MC ends with format `.../LHC20e3a/297595/001/AliESDs.root` */
        fDirNumber = ((TObjString*)tokens->At(tokens->GetEntries() - 2))->GetString().Atof();
        AliInfoF("Dir Number : %04i", (Int_t)fDirNumber);
        if (fIsSignalMC) {
            fSignalSimSet = ((TObjString*)tokens->At(tokens->GetEntries() - 4))->GetString();
            if (fSignalSimSet[0] == 'A') fReactionChannel = "ALK0";
            if (fSignalSimSet[0] == 'D') fReactionChannel = "ALPK";
            if (fSignalSimSet[0] == 'E') fReactionChannel = "ALPKPP";
            if (fSignalSimSet[0] == 'H') fReactionChannel = "PKPKX";
            AliInfoF("Signal Simulation Set : %s", fSignalSimSet.Data());
            AliInfoF("Reaction Channel      : %s", fReactionChannel.Data());

            ClearSignalLogs();
            BringSignalLogs();
            LoadSignalLogs();
        }
    } else {
        /* path of data ends with format `.../LHC15o/000245232/pass2/15000245232039.914/AliESDs.root` */
        TString aux_dir_nr = ((TObjString*)tokens->At(tokens->GetEntries() - 2))->GetString();
        aux_dir_nr = TString(aux_dir_nr(2 + 3 + 6, 10));  // = "039.914"
        AliInfoF("Dir Number : %s", aux_dir_nr.Data());
        fDirNumber = TString(aux_dir_nr(0, 3)).Atoi();   // = 39
        fDirNumberB = TString(aux_dir_nr(4, 5)).Atoi();  // = 914
    }

    return kTRUE;
}

/*
 * Main function, called per each event at RUNTIME ~ execution on Grid.
 */
void AliAnalysisTaskEsd2Tree::UserExec(Option_t*) {

    /* Load MC Gen. Event and PV */

    if (fIsMC) {
        fMC = MCEvent();
        if (!fMC) AliFatal("AliMCEvent couldn't be found.");
        fMC_PrimaryVertex = const_cast<AliVVertex*>(fMC->GetPrimaryVertex());
        fPIDResponse->SetCurrentMCEvent(fMC);
    }

    /* Load Reconstructed Event, PV and Magnetic Field */

    fESD = dynamic_cast<AliESDEvent*>(InputEvent());
    if (!fESD) AliFatal("AliESDEvent couldn't be found.");

    fPrimaryVertex = const_cast<AliESDVertex*>(fESD->GetPrimaryVertex());
    fMagneticField = fESD->GetMagneticField();

    fRunNumber = fESD->GetRunNumber();
    fEventNumber = fESD->GetEventNumberInFile();

    /* Event selection */

    if (!PassesEventSelection()) return;

    /* Centrality */

    AliMultSelection* MultSelection = dynamic_cast<AliMultSelection*>(fESD->FindListObject("MultSelection"));
    if (!MultSelection) AliFatal("AliMultSelection couldn't be found.");
    fCentrality = MultSelection->GetMultiplicityPercentile("V0M");

    fHist_Centrality->Fill(fCentrality);
    if ((fInputHandler->IsEventSelected() & AliVEvent::kINT7)) fHist_CentralityINT7->Fill(fCentrality);

    /* Main */

    if (fIsMC) {
        if (fIsSignalMC) ProcessInjectedReactions();
        ProcessMCParticles();
    }
    ProcessTracks();

    fOutputTree->Fill();

    /* End of event */

    if (fIsMC) {
        if (fIsSignalMC) ClearInjectedBranches();
        ClearMCBranches();
        fMcEntry_.clear();
    }
    ClearTracksBranches();

    PostData(1, fOutputList);
    PostData(2, fOutputTree);
}

/*            */
/**  Events  **/
/*** ====== ***/

/*
 * Apply event selection.
 */
Bool_t AliAnalysisTaskEsd2Tree::PassesEventSelection() {

    fHist_Events_Bookkeeping->Fill(0.);

    /* First Check */

    if (!fEventCuts.AcceptEvent(fESD)) return kFALSE;
    fHist_Events_Bookkeeping->Fill(1.);

    /* Reference: https://twiki.cern.ch/twiki/bin/viewauth/ALICE/AliDPGRunList18r1 */

    if (!fIsMC && (fRunNumber == 296749 || fRunNumber == 296750 || fRunNumber == 296849 || fRunNumber == 296890 || fRunNumber == 297029 ||
                   fRunNumber == 297194 || fRunNumber == 297219 || fRunNumber == 297481)) {
        fEventCuts.UseTimeRangeCut();
        fEventCuts.OverrideAutomaticTriggerSelection(AliVEvent::kINT7);
        if (!fEventCuts.AcceptEvent(fESD)) return kFALSE;
    }
    fHist_Events_Bookkeeping->Fill(2.);

    /* Pileup Events */

    if (!fEventCuts.PassedCut(AliEventCuts::kPileUp)) return kFALSE;
    fHist_Events_Bookkeeping->Fill(3.);

    /* TPC Pileup Events */

    if (!fEventCuts.PassedCut(AliEventCuts::kTPCPileUp)) return kFALSE;
    fHist_Events_Bookkeeping->Fill(4.);

    /* Important for data? */

    if (!fIsMC && fESD->GetHeader()->GetEventType() != 7) return kFALSE;
    fHist_Events_Bookkeeping->Fill(5.);

    /* Trigger Selection */

    Bool_t IsMB = (fInputHandler->IsEventSelected() & AliVEvent::kINT7);
    Bool_t IsHighMultV0 = (fInputHandler->IsEventSelected() & AliVEvent::kHighMultV0);
    Bool_t IsHighMultSPD = (fInputHandler->IsEventSelected() & AliVEvent::kHighMultSPD);
    Bool_t IsCentral = (fInputHandler->IsEventSelected() & AliVEvent::kCentral);
    Bool_t IsSemiCentral = (fInputHandler->IsEventSelected() & AliVEvent::kSemiCentral);

    if (!IsMB && !IsHighMultV0 && !IsHighMultSPD && !IsCentral && !IsSemiCentral) return kFALSE;
    fHist_Events_Bookkeeping->Fill(6.);

    /* rec. PV z-vertex range */

    if (TMath::Abs(fPrimaryVertex->GetZ()) > 12.) return kFALSE;
    fHist_Events_Bookkeeping->Fill(7.);

    return kTRUE;
}

/*
 *
 */
void AliAnalysisTaskEsd2Tree::ProcessEvent() {
    //
    if (fIsMC) {
        tEvent_PV_TrueXv = (Float_t)fMC_PrimaryVertex->GetX();
        tEvent_PV_TrueYv = (Float_t)fMC_PrimaryVertex->GetY();
        tEvent_PV_TrueZv = (Float_t)fMC_PrimaryVertex->GetZ();
        tEvent_IsGenPileup = AliAnalysisUtils::IsPileupInGeneratedEvent(fMC, "Hijing");
        tEvent_IsSBCPileup = AliAnalysisUtils::IsSameBunchPileupInGeneratedEvent(fMC, "Hijing");
    }

    tEvent_PV_NContributors = fPrimaryVertex->GetNContributors();
    tEvent_PV_Dispersion = (Float_t)fPrimaryVertex->GetDispersion();
    tEvent_PV_Xv = (Float_t)fPrimaryVertex->GetX();
    tEvent_PV_Yv = (Float_t)fPrimaryVertex->GetY();
    tEvent_PV_Zv = (Float_t)fPrimaryVertex->GetZ();
    Double_t PV_CovMatrix[6];
    fPrimaryVertex->GetCovarianceMatrix(PV_CovMatrix);
    for (Int_t i = 0; i < 6; i++) tEvent_PV_CovMatrix[i] = (Float_t)PV_CovMatrix[i];

    const AliESDVertex* PrimaryVertex_SPD = (AliESDVertex*)fESD->GetPrimaryVertexSPD();
    tEvent_SPD_PV_Zv = (Float_t)PrimaryVertex_SPD->GetZ();
    Double_t PV_SPD_CovMatrix[6];
    PrimaryVertex_SPD->GetCovarianceMatrix(PV_SPD_CovMatrix);
    tEvent_SPD_PV_ZvErr = (Float_t)PV_SPD_CovMatrix[5];

    tEvent_NTracks = (UInt_t)fESD->GetNumberOfTracks();
    tEvent_NTPCClusters = fESD->GetNumberOfTPCClusters();
    tEvent_IsMB = fInputHandler->IsEventSelected() & AliVEvent::kINT7;
    tEvent_IsHighMultV0 = fInputHandler->IsEventSelected() & AliVEvent::kHighMultV0;
    tEvent_IsHighMultSPD = fInputHandler->IsEventSelected() & AliVEvent::AliVEvent::kHighMultSPD;
    tEvent_IsCentral = fInputHandler->IsEventSelected() & AliVEvent::kCentral;
    tEvent_IsSemiCentral = fInputHandler->IsEventSelected() & AliVEvent::kSemiCentral;
}

/*           */
/**  Trees  **/
/*** ===== ***/

/*
 * Add branches to `fOutputTree`.
 */
void AliAnalysisTaskEsd2Tree::AssociateEventsBranches() {
    //
    fOutputTree->Branch("RunNumber", &fRunNumber, "RunNumber/i");
    fOutputTree->Branch("DirNumber", &fDirNumber, "DirNumber/i");
    if (!fIsMC) fOutputTree->Branch("DirNumberB", &fDirNumberB, "DirNumberB/i");
    fOutputTree->Branch("EventNumber", &fEventNumber, "EventNumber/i");
    fOutputTree->Branch("Centrality", &fCentrality, "Centrality/F");
    fOutputTree->Branch("MagneticField", &fMagneticField, "MagneticField/F");
    if (fIsMC) {
        fOutputTree->Branch("MC_PV_Xv", &tEvent_PV_TrueXv, "PV_TrueXv/F");
        fOutputTree->Branch("MC_PV_Yv", &tEvent_PV_TrueYv, "PV_TrueYv/F");
        fOutputTree->Branch("MC_PV_Zv", &tEvent_PV_TrueZv, "PV_TrueZv/F");
        fOutputTree->Branch("MC_IsGenPileup", &tEvent_IsGenPileup, "IsGenPileup/O");
        fOutputTree->Branch("MC_IsSBCPileup", &tEvent_IsSBCPileup, "IsSBCPileup/O");
    }
    fOutputTree->Branch("PV_NContributors", &tEvent_PV_NContributors, "PV_NContributors/I");
    fOutputTree->Branch("PV_Dispersion", &tEvent_PV_Dispersion, "PV_Dispersion/F");
    fOutputTree->Branch("PV_Xv", &tEvent_PV_Xv, "PV_Xv/F");
    fOutputTree->Branch("PV_Yv", &tEvent_PV_Yv, "PV_Yv/F");
    fOutputTree->Branch("PV_Zv", &tEvent_PV_Zv, "PV_Zv/F");
    fOutputTree->Branch("PV_CovMatrix", &tEvent_PV_CovMatrix, "PV_CovMatrix[6]/F");
    fOutputTree->Branch("SPD_PV_Zv", &tEvent_SPD_PV_Zv, "PV_Zv/F");
    fOutputTree->Branch("SPD_PV_ZvErr", &tEvent_SPD_PV_ZvErr, "PV_ZvErr/F");
    fOutputTree->Branch("NTracks", &tEvent_NTracks, "NTracks/i");
    fOutputTree->Branch("NTPCClusters", &tEvent_NTPCClusters, "NTPCClusters/I");
    fOutputTree->Branch("IsMB", &tEvent_IsMB, "IsMB/O");
    fOutputTree->Branch("IsHighMultV0", &tEvent_IsHighMultV0, "IsHighMultV0/O");
    fOutputTree->Branch("IsHighMultSPD", &tEvent_IsHighMultSPD, "IsHighMultSPD/O");
    fOutputTree->Branch("IsCentral", &tEvent_IsCentral, "IsCentral/O");
    fOutputTree->Branch("IsSemiCentral", &tEvent_IsSemiCentral, "IsSemiCentral/O");
}

/*
 * Add branches to `fTree_Injected`.
 */
void AliAnalysisTaskEsd2Tree::AssociateInjectedBranches() {
    //
    fOutputTree->Branch("ReactionID", &tInjected_ReactionID);
    fOutputTree->Branch("Sexaquark_Px", &tInjected_Px);
    fOutputTree->Branch("Sexaquark_Py", &tInjected_Py);
    fOutputTree->Branch("Sexaquark_Pz", &tInjected_Pz);
    fOutputTree->Branch("Nucleon_Px", &tInjected_Nucleon_Px);
    fOutputTree->Branch("Nucleon_Py", &tInjected_Nucleon_Py);
    fOutputTree->Branch("Nucleon_Pz", &tInjected_Nucleon_Pz);
}

/*
 * Add branches to `fTree_MC`.
 */
void AliAnalysisTaskEsd2Tree::AssociateMCBranches() {
    //
    fOutputTree->Branch("MC_PdgCode", &tMC_PdgCode);
    fOutputTree->Branch("MC_Mother_McEntry", &tMC_Mother_McEntry);
    fOutputTree->Branch("MC_Px", &tMC_Px);
    fOutputTree->Branch("MC_Py", &tMC_Py);
    fOutputTree->Branch("MC_Pz", &tMC_Pz);
    fOutputTree->Branch("MC_Xv", &tMC_Xv);
    fOutputTree->Branch("MC_Yv", &tMC_Yv);
    fOutputTree->Branch("MC_Zv", &tMC_Zv);
    fOutputTree->Branch("MC_Status", &tMC_Status);
    fOutputTree->Branch("MC_IsOOBPileup", &tMC_IsOOBPileup);
    fOutputTree->Branch("MC_Generator", &tMC_Generator);
    fOutputTree->Branch("MC_IsPrimary", &tMC_IsPrimary);
    fOutputTree->Branch("MC_IsSecFromMat", &tMC_IsSecFromMat);
    fOutputTree->Branch("MC_IsSecFromWeak", &tMC_IsSecFromWeak);
}

/*
 * Add branches to `fTree_Tracks`.
 */
void AliAnalysisTaskEsd2Tree::AssociateTracksBranches() {
    //
    fOutputTree->Branch("Track_Px", &tTrack_Px);
    fOutputTree->Branch("Track_Py", &tTrack_Py);
    fOutputTree->Branch("Track_Pz", &tTrack_Pz);
    fOutputTree->Branch("Track_X", &tTrack_X);
    fOutputTree->Branch("Track_Y", &tTrack_Y);
    fOutputTree->Branch("Track_Z", &tTrack_Z);
    fOutputTree->Branch("Track_Charge", &tTrack_Charge);
    fOutputTree->Branch("Track_Alpha", &tTrack_Alpha);
    fOutputTree->Branch("Track_Snp", &tTrack_Snp);
    fOutputTree->Branch("Track_Tgl", &tTrack_Tgl);
    fOutputTree->Branch("Track_Signed1Pt", &tTrack_Signed1Pt);
    fOutputTree->Branch("Track_SigmaY2", &tTrack_SigmaY2);
    fOutputTree->Branch("Track_SigmaZY", &tTrack_SigmaZY);
    fOutputTree->Branch("Track_SigmaZ2", &tTrack_SigmaZ2);
    fOutputTree->Branch("Track_SigmaSnpY", &tTrack_SigmaSnpY);
    fOutputTree->Branch("Track_SigmaSnpZ", &tTrack_SigmaSnpZ);
    fOutputTree->Branch("Track_SigmaSnp2", &tTrack_SigmaSnp2);
    fOutputTree->Branch("Track_SigmaTglY", &tTrack_SigmaTglY);
    fOutputTree->Branch("Track_SigmaTglZ", &tTrack_SigmaTglZ);
    fOutputTree->Branch("Track_SigmaTglSnp", &tTrack_SigmaTglSnp);
    fOutputTree->Branch("Track_SigmaTgl2", &tTrack_SigmaTgl2);
    fOutputTree->Branch("Track_Sigma1PtY", &tTrack_Sigma1PtY);
    fOutputTree->Branch("Track_Sigma1PtZ", &tTrack_Sigma1PtZ);
    fOutputTree->Branch("Track_Sigma1PtSnp", &tTrack_Sigma1PtSnp);
    fOutputTree->Branch("Track_Sigma1PtTgl", &tTrack_Sigma1PtTgl);
    fOutputTree->Branch("Track_Sigma1Pt2", &tTrack_Sigma1Pt2);
    fOutputTree->Branch("Track_NSigmaPion", &tTrack_NSigmaPion);
    fOutputTree->Branch("Track_NSigmaKaon", &tTrack_NSigmaKaon);
    fOutputTree->Branch("Track_NSigmaProton", &tTrack_NSigmaProton);
    fOutputTree->Branch("Track_DCAxy", &tTrack_DCAxy);
    fOutputTree->Branch("Track_DCAz", &tTrack_DCAz);
    fOutputTree->Branch("Track_NTPCClusters", &tTrack_NTPCClusters);
    fOutputTree->Branch("Track_NCrossedRows", &tTrack_NCrossedRows);
    fOutputTree->Branch("Track_NFindableClusters", &tTrack_NFindableClusters);
    fOutputTree->Branch("Track_NSharedClusters", &tTrack_NSharedClusters);
    fOutputTree->Branch("Track_Chi2overNcls", &tTrack_Chi2overNcls);
    fOutputTree->Branch("Track_IsKinkDaughter", &tTrack_IsKinkDaughter);
    // fOutputTree->Branch("TPCFitMap", &tTrack_TPCFitMap);
    // fOutputTree->Branch("TPCClusterMap", &tTrack_TPCClusterMap);
    // fOutputTree->Branch("TPCSharedMap", &tTrack_TPCSharedMap);
    if (fIsMC) fOutputTree->Branch("Track_McEntry", &tTrack_McEntry);
}

/*                  */
/**  MC Generated  **/
/*** ============ ***/

/*
 * Loop over MC particles in a single event. Store the indices of the signal particles.
 */
void AliAnalysisTaskEsd2Tree::ProcessMCParticles() {
    AliMCParticle* mcPart;
    fMcEntry_[-1] = -1;
    Long_t mc_entry = 0;
    for (Int_t mc_idx = 0; mc_idx < fMC->GetNumberOfTracks(); mc_idx++) {
        mcPart = (AliMCParticle*)fMC->GetTrack(mc_idx);
        if (!mcPart) continue;
        /* Remove trash */
        if (mcPart->P() < 0.01) continue;
        fMcEntry_[mc_idx] = mc_entry;
        /* Assign branches and fill tree */
        tMC_PdgCode.push_back(mcPart->PdgCode());
        tMC_Mother_McEntry.push_back(fMcEntry_[mcPart->GetMother()]);
        tMC_Px.push_back((Float_t)mcPart->Px());
        tMC_Py.push_back((Float_t)mcPart->Py());
        tMC_Pz.push_back((Float_t)mcPart->Pz());
        tMC_Xv.push_back((Float_t)mcPart->Xv());
        tMC_Yv.push_back((Float_t)mcPart->Yv());
        tMC_Zv.push_back((Float_t)mcPart->Zv());
        tMC_Status.push_back(mcPart->MCStatusCode());
        tMC_IsOOBPileup.push_back(AliAnalysisUtils::IsParticleFromOutOfBunchPileupCollision(mc_idx, fMC));
        tMC_Generator.push_back(mcPart->GetGeneratorIndex());
        tMC_IsPrimary.push_back(mcPart->IsPhysicalPrimary());
        tMC_IsSecFromMat.push_back(mcPart->IsSecondaryFromMaterial());
        tMC_IsSecFromWeak.push_back(mcPart->IsSecondaryFromWeakDecay());
        AliInfoF("mc_idx=%i, mc_entry=%li, pdg=%i, mother_idx=%i, mother_entry=%li",  //
                 mc_idx, mc_entry, mcPart->PdgCode(), mcPart->GetMother(), fMcEntry_[mcPart->GetMother()]);
        mc_entry++;
    }  // end of loop over MC particles
}

/*
 * Clear MC branches.
 */
void AliAnalysisTaskEsd2Tree::ClearMCBranches() {
    //
    tMC_PdgCode.clear();
    tMC_Mother_McEntry.clear();
    tMC_Px.clear();
    tMC_Py.clear();
    tMC_Pz.clear();
    tMC_Xv.clear();
    tMC_Yv.clear();
    tMC_Zv.clear();
    tMC_Status.clear();
    tMC_IsOOBPileup.clear();
    tMC_Generator.clear();
    tMC_IsPrimary.clear();
    tMC_IsSecFromMat.clear();
    tMC_IsSecFromWeak.clear();
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

    Double_t momentum[3];
    Double_t position[3];
    Float_t AuxDCAxy, AuxDCAz;

    for (Int_t esd_idx = 0; esd_idx < fESD->GetNumberOfTracks(); esd_idx++) {
        track = fESD->GetTrack(esd_idx);
        trackInnerParam = const_cast<AliExternalTrackParam*>(track->GetInnerParam());
        /* Track selection */
        if (!trackInnerParam) continue;
        if (trackInnerParam->Pt() < 1E-2 || trackInnerParam->Pt() > 1E2) continue;
        if (!PassesTrackSelection(track)) continue;
        /* Assign branches */
        /*  */ trackInnerParam->GetPxPyPz(momentum);
        tTrack_Px.push_back((Float_t)momentum[0]);
        tTrack_Py.push_back((Float_t)momentum[1]);
        tTrack_Pz.push_back((Float_t)momentum[2]);
        /*  */ trackInnerParam->GetXYZ(position);
        tTrack_X.push_back((Float_t)position[0]);
        tTrack_Y.push_back((Float_t)position[1]);
        tTrack_Z.push_back((Float_t)position[2]);
        tTrack_Charge.push_back(trackInnerParam->Charge());
        tTrack_Alpha.push_back((Float_t)trackInnerParam->GetAlpha());
        tTrack_Snp.push_back((Float_t)trackInnerParam->GetSnp());
        tTrack_Tgl.push_back((Float_t)trackInnerParam->GetTgl());
        tTrack_Signed1Pt.push_back((Float_t)trackInnerParam->GetSigned1Pt());
        /*  */ const Double_t* covariance_matrix = trackInnerParam->GetCovariance();
        tTrack_SigmaY2.push_back(covariance_matrix[0]);
        tTrack_SigmaZY.push_back(covariance_matrix[1]);
        tTrack_SigmaZ2.push_back(covariance_matrix[2]);
        tTrack_SigmaSnpY.push_back(covariance_matrix[3]);
        tTrack_SigmaSnpZ.push_back(covariance_matrix[4]);
        tTrack_SigmaSnp2.push_back(covariance_matrix[5]);
        tTrack_SigmaTglY.push_back(covariance_matrix[6]);
        tTrack_SigmaTglZ.push_back(covariance_matrix[7]);
        tTrack_SigmaTglSnp.push_back(covariance_matrix[8]);
        tTrack_SigmaTgl2.push_back(covariance_matrix[9]);
        tTrack_Sigma1PtY.push_back(covariance_matrix[10]);
        tTrack_Sigma1PtZ.push_back(covariance_matrix[11]);
        tTrack_Sigma1PtSnp.push_back(covariance_matrix[12]);
        tTrack_Sigma1PtTgl.push_back(covariance_matrix[13]);
        tTrack_Sigma1Pt2.push_back(covariance_matrix[14]);
        tTrack_NSigmaPion.push_back(fPIDResponse->NumberOfSigmasTPC(track, AliPID::kPion));
        tTrack_NSigmaKaon.push_back(fPIDResponse->NumberOfSigmasTPC(track, AliPID::kKaon));
        tTrack_NSigmaProton.push_back(fPIDResponse->NumberOfSigmasTPC(track, AliPID::kProton));
        /*  */ track->GetImpactParameters(AuxDCAxy, AuxDCAz);
        tTrack_DCAxy.push_back((Float_t)AuxDCAxy);
        tTrack_DCAz.push_back((Float_t)AuxDCAz);
        tTrack_NTPCClusters.push_back(track->GetTPCNcls());  // Note: capital N
        tTrack_NCrossedRows.push_back(track->GetTPCCrossedRows());
        tTrack_NFindableClusters.push_back(track->GetTPCNclsF());
        tTrack_NSharedClusters.push_back(track->GetTPCnclsS());
        tTrack_Chi2overNcls.push_back(track->GetTPCNcls() > 1E-4 ? Float_t(track->GetTPCchi2() / (Double_t)track->GetTPCNcls()) : 999.);
        tTrack_IsKinkDaughter.push_back(track->GetKinkIndex(0) > 0);
        // tTrack_TPCFitMap = track->GetTPCFitMap();
        // tTrack_TPCClusterMap = track->GetTPCClusterMap();
        // tTrack_TPCSharedMap = track->GetTPCSharedMap();
        if (fIsMC) tTrack_McEntry.push_back(fMcEntry_[TMath::Abs(track->GetLabel())]);
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
    Bool_t tpc_status = (track->GetStatus() & AliESDtrack::kTPCout) && (track->GetStatus() & AliESDtrack::kTPCrefit);
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
 * Clear the branches of the reconstructed tracks.
 */
void AliAnalysisTaskEsd2Tree::ClearTracksBranches() {
    //
    tTrack_Px.clear();
    tTrack_Py.clear();
    tTrack_Pz.clear();
    tTrack_X.clear();
    tTrack_Y.clear();
    tTrack_Z.clear();
    tTrack_Charge.clear();
    tTrack_Alpha.clear();
    tTrack_Snp.clear();
    tTrack_Tgl.clear();
    tTrack_Signed1Pt.clear();
    tTrack_SigmaY2.clear();
    tTrack_SigmaZY.clear();
    tTrack_SigmaZ2.clear();
    tTrack_SigmaSnpY.clear();
    tTrack_SigmaSnpZ.clear();
    tTrack_SigmaSnp2.clear();
    tTrack_SigmaTglY.clear();
    tTrack_SigmaTglZ.clear();
    tTrack_SigmaTglSnp.clear();
    tTrack_SigmaTgl2.clear();
    tTrack_Sigma1PtY.clear();
    tTrack_Sigma1PtZ.clear();
    tTrack_Sigma1PtSnp.clear();
    tTrack_Sigma1PtTgl.clear();
    tTrack_Sigma1Pt2.clear();
    tTrack_NSigmaPion.clear();
    tTrack_NSigmaKaon.clear();
    tTrack_NSigmaProton.clear();
    tTrack_DCAxy.clear();
    tTrack_DCAz.clear();
    tTrack_NTPCClusters.clear();
    tTrack_NCrossedRows.clear();
    tTrack_NFindableClusters.clear();
    tTrack_NSharedClusters.clear();
    tTrack_Chi2overNcls.clear();
    tTrack_IsKinkDaughter.clear();
    // tTrack_TPCFitMap.clear();
    // tTrack_TPCClusterMap.clear();
    // tTrack_TPCSharedMap.clear();
    if (fIsMC) tTrack_McEntry.clear();
}

/*                        */
/**  Injected Reactions  **/
/*** ================== ***/

/*
 * Assign the in-memory values to the tree branches.
 */
void AliAnalysisTaskEsd2Tree::ProcessInjectedReactions() {
    //
    for (size_t i = 0; i < fReactionID_[fEventNumber].size(); i++) {
        tInjected_ReactionID.push_back(fReactionID_[fEventNumber][i]);
        tInjected_Px.push_back(fSexaquark_Px_[fEventNumber][i]);
        tInjected_Py.push_back(fSexaquark_Py_[fEventNumber][i]);
        tInjected_Pz.push_back(fSexaquark_Pz_[fEventNumber][i]);
        tInjected_Nucleon_Px.push_back(fNucleon_Px_[fEventNumber][i]);
        tInjected_Nucleon_Py.push_back(fNucleon_Py_[fEventNumber][i]);
        tInjected_Nucleon_Pz.push_back(fNucleon_Pz_[fEventNumber][i]);
    }
}

/*
 * Open the respective `sim.log` that corresponds to the `RunNumber+DirNumber` that's being analyzed.
 * From it, read the injected anti-sexaquark and struck nucleon kinematics and store them into a tree.
 */
void AliAnalysisTaskEsd2Tree::BringSignalLogs() {

    TGrid* alien = nullptr;
    if (!gGrid) {
        alien = TGrid::Connect("alien://");
        if (!alien) return;
    }

    TString AliEn_Dir = fAliEnPath(0, fAliEnPath.Last('/'));

    TString orig_path = Form("%s/sim.log", AliEn_Dir.Data());
    AliInfoF("Copying file %s ...", orig_path.Data());

    /* assuming path ends with format `.../LHC23l1a3/A1.73/297595/001/sim.log` */
    Int_t AliEn_DirNumber = static_cast<Int_t>(fDirNumber);
    TObjArray* tokens = fAliEnPath.Tokenize("/");
    Int_t AliEn_RunNumber = ((TObjString*)tokens->At(tokens->GetEntries() - 3))->GetString().Atoi();
    TString AliEn_SimSubSet = ((TObjString*)tokens->At(tokens->GetEntries() - 4))->GetString();

    fSignalLog_NewBasename = Form("sim_%s_%i_%03i.log", AliEn_SimSubSet.Data(), AliEn_RunNumber, AliEn_DirNumber);

    if (AliEn_Dir.BeginsWith("alien://")) {
        gSystem->Exec(Form("alien.py cp %s file://./%s", orig_path.Data(), fSignalLog_NewBasename.Data()));
    } else {
        gSystem->Exec(Form("cp %s ./%s", orig_path.Data(), fSignalLog_NewBasename.Data()));
    }

    TString new_path = Form("%s/%s", gSystem->pwd(), fSignalLog_NewBasename.Data());
    AliInfoF("Signal log file ready at %s ...", new_path.Data());
}

/*
 * Load injected anti-sexaquark and struck nucleon info.
 * From the `sim.log` file that corresponds to an entire dir number into memory.
 */
Bool_t AliAnalysisTaskEsd2Tree::LoadSignalLogs() {
    //
    TString new_path = Form("%s/%s", gSystem->pwd(), fSignalLog_NewBasename.Data());

    AliInfoF("Opening file %s ...", new_path.Data());
    std::ifstream SignalLog(new_path);
    if (!SignalLog.is_open()) {
        AliWarningF("Unable to open file %s", new_path.Data());
        return kFALSE;
    }

    Int_t CurrentEventNumber = -1;

    // auxiliary variables
    std::string cstr_line;
    TString tstr_line, csv;
    TObjArray* csv_arr = nullptr;

    Char_t aux_char;

    /* Read lines */

    while (std::getline(SignalLog, cstr_line)) {

        tstr_line = cstr_line;

        // a new event has appeared
        if (tstr_line.Contains("I-AliGenCocktail::Generate: Generator 1: AliGenHijing")) CurrentEventNumber++;

        if (!tstr_line.Contains("I-AliGenSexaquarkReaction::GenerateN: 6")) continue;

        csv = static_cast<TString>(tstr_line(38, tstr_line.Length() - 1));
        csv_arr = csv.Tokenize(",");

        /* Load content to memory */

        fReactionID_[CurrentEventNumber].push_back(dynamic_cast<TObjString*>(csv_arr->At(0))->String().Atoi());
        fSexaquark_Px_[CurrentEventNumber].push_back(dynamic_cast<TObjString*>(csv_arr->At(1))->String().Atof());
        fSexaquark_Py_[CurrentEventNumber].push_back(dynamic_cast<TObjString*>(csv_arr->At(2))->String().Atof());
        fSexaquark_Pz_[CurrentEventNumber].push_back(dynamic_cast<TObjString*>(csv_arr->At(3))->String().Atof());
        fNucleon_Px_[CurrentEventNumber].push_back(dynamic_cast<TObjString*>(csv_arr->At(4))->String().Atof());
        fNucleon_Py_[CurrentEventNumber].push_back(dynamic_cast<TObjString*>(csv_arr->At(5))->String().Atof());
        fNucleon_Pz_[CurrentEventNumber].push_back(dynamic_cast<TObjString*>(csv_arr->At(6))->String().Atof());
    }  // end of loop over lines

    AliInfoF("Closing file %s ...", new_path.Data());
    SignalLog.close();

    return kTRUE;
}

/*
 * Clear the maps from memory.
 */
void AliAnalysisTaskEsd2Tree::ClearSignalLogs() {
    //
    fReactionID_.clear();
    fSexaquark_Px_.clear();
    fSexaquark_Py_.clear();
    fSexaquark_Pz_.clear();
    fNucleon_Px_.clear();
    fNucleon_Py_.clear();
    fNucleon_Pz_.clear();
}

/*
 * Clear the branches from the tree.
 */
void AliAnalysisTaskEsd2Tree::ClearInjectedBranches() {
    //
    tInjected_ReactionID.clear();
    tInjected_Px.clear();
    tInjected_Py.clear();
    tInjected_Pz.clear();
    tInjected_Nucleon_Px.clear();
    tInjected_Nucleon_Py.clear();
    tInjected_Nucleon_Pz.clear();
}
