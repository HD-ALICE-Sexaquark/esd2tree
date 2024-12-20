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
      fSignalSimSet(""),
      fRunNumber(0),
      fDirNumber(0),
      fDirNumberB(0),
      fEventNumber(0),
      fCentrality(0.),
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
      fOutputFile(nullptr),
      fOutputDir(nullptr),
      /*  */
      fTree_Events(nullptr),
      fTree_Injected(nullptr),
      fTree_MC(nullptr),
      fTree_Tracks(nullptr),
      /*  */
      tEvent_PV_TrueXv(0.),
      tEvent_PV_TrueYv(0.),
      tEvent_PV_TrueZv(0.),
      tEvent_IsGenPileup(kFALSE),
      tEvent_IsSBCPileup(kFALSE),
      tEvent_PV_RecXv(0.),
      tEvent_PV_RecYv(0.),
      tEvent_PV_RecZv(0.),
      tEvent_PV_NContributors(0),
      tEvent_PV_ZvErr_FromSPD(0.),
      tEvent_PV_ZvErr_FromTracks(0.),
      tEvent_PV_Zv_FromSPD(0.),
      tEvent_PV_Zv_FromTracks(0.),
      tEvent_PV_Dispersion(0.),
      tEvent_NTracks(0),
      tEvent_NTPCClusters(0),
      tEvent_IsMB(kFALSE),
      tEvent_IsHighMultV0(kFALSE),
      tEvent_IsHighMultSPD(kFALSE),
      tEvent_IsCentral(kFALSE),
      tEvent_IsSemiCentral(kFALSE),
      /*  */
      fMap_ReactionID(),
      fMap_Sexaquark_Px(),
      fMap_Sexaquark_Py(),
      fMap_Sexaquark_Pz(),
      fMap_Nucleon_Px(),
      fMap_Nucleon_Py(),
      fMap_Nucleon_Pz(),
      /*  */
      tInjected_ReactionID(0),
      tInjected_Px(0.),
      tInjected_Py(0.),
      tInjected_Pz(0.),
      tInjected_Nucleon_Px(0.),
      tInjected_Nucleon_Py(0.),
      tInjected_Nucleon_Pz(0.),
      /*  */
      tMC_Idx(0),
      tMC_PdgCode(0),
      tMC_Idx_Mother(0),
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
      tMC_IsPrimary(kFALSE),
      tMC_IsSecFromMat(kFALSE),
      tMC_IsSecFromWeak(kFALSE),
      tMC_ReactionID(0),
      /*  */
      tTrack_Idx(0),
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
      fSignalSimSet(""),
      fRunNumber(0),
      fDirNumber(0),
      fDirNumberB(0),
      fEventNumber(0),
      fCentrality(0.),
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
      fOutputFile(nullptr),
      fOutputDir(nullptr),
      /*  */
      fTree_Events(nullptr),
      fTree_Injected(nullptr),
      fTree_MC(nullptr),
      fTree_Tracks(nullptr),
      /*  */
      tEvent_PV_TrueXv(0.),
      tEvent_PV_TrueYv(0.),
      tEvent_PV_TrueZv(0.),
      tEvent_IsGenPileup(kFALSE),
      tEvent_IsSBCPileup(kFALSE),
      tEvent_PV_RecXv(0.),
      tEvent_PV_RecYv(0.),
      tEvent_PV_RecZv(0.),
      tEvent_PV_NContributors(0),
      tEvent_PV_ZvErr_FromSPD(0.),
      tEvent_PV_ZvErr_FromTracks(0.),
      tEvent_PV_Zv_FromSPD(0.),
      tEvent_PV_Zv_FromTracks(0.),
      tEvent_PV_Dispersion(0.),
      tEvent_NTracks(0),
      tEvent_NTPCClusters(0),
      tEvent_IsMB(kFALSE),
      tEvent_IsHighMultV0(kFALSE),
      tEvent_IsHighMultSPD(kFALSE),
      tEvent_IsCentral(kFALSE),
      tEvent_IsSemiCentral(kFALSE),
      /*  */
      fMap_ReactionID(),
      fMap_Sexaquark_Px(),
      fMap_Sexaquark_Py(),
      fMap_Sexaquark_Pz(),
      fMap_Nucleon_Px(),
      fMap_Nucleon_Py(),
      fMap_Nucleon_Pz(),
      /*  */
      tInjected_ReactionID(0),
      tInjected_Px(0.),
      tInjected_Py(0.),
      tInjected_Pz(0.),
      tInjected_Nucleon_Px(0.),
      tInjected_Nucleon_Py(0.),
      tInjected_Nucleon_Pz(0.),
      /*  */
      tMC_Idx(0),
      tMC_PdgCode(0),
      tMC_Idx_Mother(0),
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
      tMC_IsPrimary(kFALSE),
      tMC_IsSecFromMat(kFALSE),
      tMC_IsSecFromWeak(kFALSE),
      tMC_ReactionID(0),
      /*  */
      tTrack_Idx(0),
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
    DefineOutput(1, TList::Class());  // fOutputList
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

    AliAnalysisManager* man = AliAnalysisManager::GetAnalysisManager();
    if (!man) AliFatal("ERROR: AliAnalysisManager couldn't be found.");

    AliESDInputHandler* inputHandler = (AliESDInputHandler*)(man->GetInputEventHandler());
    if (!inputHandler) AliFatal("ERROR: AliESDInputHandler couldn't be found.");

    fPIDResponse = inputHandler->GetPIDResponse();

    /* Prepare output file */

    fOutputFile = TFile::Open("SimpleTrees.root", "RECREATE", "Reduced ESDs", 505);

    // events tree ~ higher level
    fTree_Events = new TTree("Events", "Events");
    AssociateEventsBranches();

    /* Prepare output list */

    fOutputList = new TList();
    fOutputList->SetOwner(kTRUE);

    fHist_Events_Bookkeeping = new TH1F("Events_Bookkeeping", ";;Counts", 10, 0., 10.);
    fOutputList->Add(fHist_Events_Bookkeeping);

    fHist_Centrality = new TH1F("Centrality", ";Centrality;Counts", 11, 0., 110.);
    fOutputList->Add(fHist_Centrality);

    fHist_CentralityINT7 = new TH1F("CentralityINT7", ";CentralityINT7;Counts", 11, 0., 110.);
    fOutputList->Add(fHist_CentralityINT7);

    PostData(1, fOutputList);
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
        aux_dir_nr = TString(aux_dir_nr(2 + 3 + 6, 8));  // = "039.914"
        AliInfoF("Dir Number : %s", aux_dir_nr.Data());
        fDirNumber = TString(aux_dir_nr(0, 3)).Atoi();   // = 39
        fDirNumberB = TString(aux_dir_nr(4, 4)).Atoi();  // = 914
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

    /* Initialize event's unique identifier */

    TString EventUniqueID;
    if (fIsMC) {
        if (fIsSignalMC)
            EventUniqueID = TString::Format("%s_%6u_%04u_%03u", fSignalSimSet.ReplaceAll(".", "").Data(), fRunNumber, fDirNumber, fEventNumber);
        else
            EventUniqueID = TString::Format("BKG_%6u_%04u_%03u", fRunNumber, fDirNumber, fEventNumber);
    } else {
        EventUniqueID = TString::Format("DATA_%6u_%03u_%u_%03u", fRunNumber, fDirNumber, fDirNumberB, fEventNumber);
    }

    AliInfoF("Initializing event %s", EventUniqueID.Data());

    // inside TDirectory
    fOutputDir = fOutputFile->mkdir(EventUniqueID);

    if (fIsMC) {
        if (fIsSignalMC) {
            fTree_Injected = new TTree("Injected", "Injected");
            AssociateInjectedBranches();
        }
        fTree_MC = new TTree("MC", "MC");
        AssociateMCBranches();
    }

    fTree_Tracks = new TTree("Tracks", "Tracks");
    AssociateTracksBranches();

    /* Centrality */

    AliMultSelection* MultSelection = dynamic_cast<AliMultSelection*>(fESD->FindListObject("MultSelection"));
    if (!MultSelection) AliFatal("AliMultSelection couldn't be found.");
    fCentrality = MultSelection->GetMultiplicityPercentile("V0M");

    fHist_Centrality->Fill(fCentrality);
    if ((fInputHandler->IsEventSelected() & AliVEvent::kINT7)) fHist_CentralityINT7->Fill(fCentrality);

    /* Fill trees */

    FillEvent();
    if (fIsMC) {
        if (fIsSignalMC) FillInjected();
        FillMC();
    }
    FillTracks();

    /* Write trees */

    fOutputDir->cd();  // inside TDirectory
    if (fIsMC) {
        if (fIsSignalMC) WriteTree(fTree_Injected);
        WriteTree(fTree_MC);
    }
    WriteTree(fTree_Tracks);

    /* Post data */

    PostData(1, fOutputList);
}

/*
 *
 */
void AliAnalysisTaskEsd2Tree::WriteTree(TTree* thisTree) {
    if (!thisTree) return;
    AliInfoF("Writing %s/%s",                                                        //
             TString(thisTree->GetName()) == "Events" ? "" : fOutputDir->GetName(),  //
             thisTree->GetName());
    thisTree->Write();
    delete thisTree;
    thisTree = nullptr;
}

/*
 *
 */
void AliAnalysisTaskEsd2Tree::FinishTaskOutput() {
    fOutputFile->cd();  // above `fOutputDir`
    WriteTree(fTree_Events);

    AliInfoF("Writing file %s", fOutputFile->GetName());
    fOutputFile->Write();
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
void AliAnalysisTaskEsd2Tree::FillEvent() {
    if (fIsMC) {
        tEvent_PV_TrueXv = (Float_t)fMC_PrimaryVertex->GetX();
        tEvent_PV_TrueYv = (Float_t)fMC_PrimaryVertex->GetY();
        tEvent_PV_TrueZv = (Float_t)fMC_PrimaryVertex->GetZ();
        tEvent_IsGenPileup = AliAnalysisUtils::IsPileupInGeneratedEvent(fMC, "Hijing");
        tEvent_IsSBCPileup = AliAnalysisUtils::IsSameBunchPileupInGeneratedEvent(fMC, "Hijing");
    }
    tEvent_PV_RecXv = (Float_t)fPrimaryVertex->GetX();
    tEvent_PV_RecYv = (Float_t)fPrimaryVertex->GetY();
    tEvent_PV_RecZv = (Float_t)fPrimaryVertex->GetZ();
    tEvent_PV_NContributors = fPrimaryVertex->GetNContributors();

    AliESDVertex* PrimaryVertex_SPD = const_cast<AliESDVertex*>(fESD->GetPrimaryVertexSPD());
    Double_t PV_SPD_CovMatrix[6];
    PrimaryVertex_SPD->GetCovarianceMatrix(PV_SPD_CovMatrix);
    tEvent_PV_ZvErr_FromSPD = (Float_t)PV_SPD_CovMatrix[5];

    Double_t PV_CovMatrix[6];
    fPrimaryVertex->GetCovarianceMatrix(PV_CovMatrix);
    tEvent_PV_ZvErr_FromTracks = (Float_t)PV_CovMatrix[5];

    tEvent_PV_Zv_FromSPD = (Float_t)PrimaryVertex_SPD->GetZ();
    tEvent_PV_Zv_FromTracks = (Float_t)fPrimaryVertex->GetZ();
    tEvent_PV_Dispersion = (Float_t)fPrimaryVertex->GetDispersion();
    tEvent_NTracks = (UInt_t)fESD->GetNumberOfTracks();
    tEvent_NTPCClusters = fESD->GetNumberOfTPCClusters();
    tEvent_IsMB = fInputHandler->IsEventSelected() & AliVEvent::kINT7;
    tEvent_IsHighMultV0 = fInputHandler->IsEventSelected() & AliVEvent::kHighMultV0;
    tEvent_IsHighMultSPD = fInputHandler->IsEventSelected() & AliVEvent::AliVEvent::kHighMultSPD;
    tEvent_IsCentral = fInputHandler->IsEventSelected() & AliVEvent::kCentral;
    tEvent_IsSemiCentral = fInputHandler->IsEventSelected() & AliVEvent::kSemiCentral;

    fTree_Events->Fill();
}

/*           */
/**  Trees  **/
/*** ===== ***/

/*
 * Add branches to `fTree_Events`.
 */
void AliAnalysisTaskEsd2Tree::AssociateEventsBranches() {
    fTree_Events->Branch("RunNumber", &fRunNumber, "RunNumber/i");
    fTree_Events->Branch("DirNumber", &fDirNumber, "DirNumber/i");
    if (!fIsMC) fTree_Events->Branch("DirNumberB", &fDirNumberB, "DirNumberB/i");
    fTree_Events->Branch("EventNumber", &fEventNumber, "EventNumber/i");
    fTree_Events->Branch("Centrality", &fCentrality, "Centrality/F");
    fTree_Events->Branch("PV_TrueXv", &tEvent_PV_TrueXv, "PV_TrueXv/F");
    fTree_Events->Branch("PV_TrueYv", &tEvent_PV_TrueYv, "PV_TrueYv/F");
    fTree_Events->Branch("PV_TrueZv", &tEvent_PV_TrueZv, "PV_TrueZv/F");
    fTree_Events->Branch("IsGenPileup", &tEvent_IsGenPileup, "IsGenPileup/O");
    fTree_Events->Branch("IsSBCPileup", &tEvent_IsSBCPileup, "IsSBCPileup/O");
    fTree_Events->Branch("PV_RecXv", &tEvent_PV_RecXv, "PV_RecXv/F");
    fTree_Events->Branch("PV_RecYv", &tEvent_PV_RecYv, "PV_RecYv/F");
    fTree_Events->Branch("PV_RecZv", &tEvent_PV_RecZv, "PV_RecZv/F");
    fTree_Events->Branch("PV_NContributors", &tEvent_PV_NContributors, "PV_NContributors/I");
    fTree_Events->Branch("PV_ZvErr_FromSPD", &tEvent_PV_ZvErr_FromSPD, "PV_ZvErr_FromSPD/F");
    fTree_Events->Branch("PV_ZvErr_FromTracks", &tEvent_PV_ZvErr_FromTracks, "PV_ZvErr_FromTracks/F");
    fTree_Events->Branch("PV_Zv_FromSPD", &tEvent_PV_Zv_FromSPD, "PV_Zv_FromSPD/F");
    fTree_Events->Branch("PV_Zv_FromTracks", &tEvent_PV_Zv_FromTracks, "PV_Zv_FromTracks/F");
    fTree_Events->Branch("PV_Dispersion", &tEvent_PV_Dispersion, "PV_Dispersion/F");
    fTree_Events->Branch("NTracks", &tEvent_NTracks, "NTracks/i");
    fTree_Events->Branch("NTPCClusters", &tEvent_NTPCClusters, "NTPCClusters/I");
    fTree_Events->Branch("IsMB", &tEvent_IsMB, "IsMB/O");
    fTree_Events->Branch("IsHighMultV0", &tEvent_IsHighMultV0, "IsHighMultV0/O");
    fTree_Events->Branch("IsHighMultSPD", &tEvent_IsHighMultSPD, "IsHighMultSPD/O");
    fTree_Events->Branch("IsCentral", &tEvent_IsCentral, "IsCentral/O");
    fTree_Events->Branch("IsSemiCentral", &tEvent_IsSemiCentral, "IsSemiCentral/O");
}

/*
 * Add branches to `fTree_Injected`.
 */
void AliAnalysisTaskEsd2Tree::AssociateInjectedBranches() {
    fTree_Injected->Branch("ReactionID", &tInjected_ReactionID, "ReactionID/i");
    fTree_Injected->Branch("Px", &tInjected_Px, "Px/F");
    fTree_Injected->Branch("Py", &tInjected_Py, "Py/F");
    fTree_Injected->Branch("Pz", &tInjected_Pz, "Pz/F");
    fTree_Injected->Branch("Nucleon_Px", &tInjected_Nucleon_Px, "Nucleon_Px/F");
    fTree_Injected->Branch("Nucleon_Py", &tInjected_Nucleon_Py, "Nucleon_Py/F");
    fTree_Injected->Branch("Nucleon_Pz", &tInjected_Nucleon_Pz, "Nucleon_Pz/F");
}

/*
 * Add branches to `fTree_MC`.
 */
void AliAnalysisTaskEsd2Tree::AssociateMCBranches() {
    fTree_MC->Branch("Idx", &tMC_Idx, "Idx/i");
    fTree_MC->Branch("PdgCode", &tMC_PdgCode, "PdgCode/I");
    fTree_MC->Branch("Idx_Mother", &tMC_Idx_Mother, "Idx_Mother/I");
    fTree_MC->Branch("Idx_Ancestor", &tMC_Idx_Ancestor, "Idx_Ancestor/I");
    fTree_MC->Branch("Px", &tMC_Px, "Px/F");
    fTree_MC->Branch("Py", &tMC_Py, "Py/F");
    fTree_MC->Branch("Pz", &tMC_Pz, "Pz/F");
    fTree_MC->Branch("Xv", &tMC_Xv, "Xv/F");
    fTree_MC->Branch("Yv", &tMC_Yv, "Yv/F");
    fTree_MC->Branch("Zv", &tMC_Zv, "Zv/F");
    fTree_MC->Branch("Status", &tMC_Status, "Status/i");
    fTree_MC->Branch("IsOOBPileup", &tMC_IsOOBPileup, "IsOOBPileup/O");
    fTree_MC->Branch("Generator", &tMC_Generator, "Generator/S");
    fTree_MC->Branch("IsPrimary", &tMC_IsPrimary, "IsPrimary/O");
    fTree_MC->Branch("IsSecFromMat", &tMC_IsSecFromMat, "IsSecFromMat/O");
    fTree_MC->Branch("IsSecFromWeak", &tMC_IsSecFromWeak, "IsSecFromWeak/O");
    fTree_MC->Branch("ReactionID", &tMC_ReactionID, "ReactionID/I");
}

/*
 * Add branches to `fTree_Tracks`.
 */
void AliAnalysisTaskEsd2Tree::AssociateTracksBranches() {
    fTree_Tracks->Branch("Idx", &tTrack_Idx, "Idx/i");
    fTree_Tracks->Branch("Px", &tTrack_Px, "Px/F");
    fTree_Tracks->Branch("Py", &tTrack_Py, "Py/F");
    fTree_Tracks->Branch("Pz", &tTrack_Pz, "Pz/F");
    fTree_Tracks->Branch("X", &tTrack_X, "X/F");
    fTree_Tracks->Branch("Y", &tTrack_Y, "Y/F");
    fTree_Tracks->Branch("Z", &tTrack_Z, "Z/F");
    fTree_Tracks->Branch("Charge", &tTrack_Charge, "Charge/I");
    fTree_Tracks->Branch("Alpha", &tTrack_Alpha, "Alpha/F");
    fTree_Tracks->Branch("Snp", &tTrack_Snp, "Snp/F");
    fTree_Tracks->Branch("Tgl", &tTrack_Tgl, "Tgl/F");
    fTree_Tracks->Branch("Signed1Pt", &tTrack_Signed1Pt, "Signed1Pt/F");
    fTree_Tracks->Branch("CovMatrix", &tTrack_CovMatrix, "CovMatrix[15]/F");
    fTree_Tracks->Branch("NSigmaPion", &tTrack_NSigmaPion, "NSigmaPion/F");
    fTree_Tracks->Branch("NSigmaKaon", &tTrack_NSigmaKaon, "NSigmaKaon/F");
    fTree_Tracks->Branch("NSigmaProton", &tTrack_NSigmaProton, "NSigmaProton/F");
    fTree_Tracks->Branch("DCAxy", &tTrack_DCAxy, "DCAxy/F");
    fTree_Tracks->Branch("DCAz", &tTrack_DCAz, "DCAz/F");
    fTree_Tracks->Branch("NTPCClusters", &tTrack_NTPCClusters, "NTPCClusters/I");
    fTree_Tracks->Branch("NCrossedRows", &tTrack_NCrossedRows, "NCrossedRows/I");
    fTree_Tracks->Branch("NFindableClusters", &tTrack_NFindableClusters, "NFindableClusters/I");
    fTree_Tracks->Branch("NSharedClusters", &tTrack_NSharedClusters, "NSharedClusters/I");
    fTree_Tracks->Branch("Chi2overNcls", &tTrack_Chi2overNcls, "Chi2overNcls/F");
    fTree_Tracks->Branch("IsKinkDaughter", &tTrack_IsKinkDaughter, "IsKinkDaughter/O");
    // fTree_Tracks->Branch("TPCFitMap", &tTrack_TPCFitMap);
    // fTree_Tracks->Branch("TPCClusterMap", &tTrack_TPCClusterMap);
    // fTree_Tracks->Branch("TPCSharedMap", &tTrack_TPCSharedMap);
    fTree_Tracks->Branch("Idx_True", &tTrack_Idx_True, "Idx_True/I");
}

/*                  */
/**  MC Generated  **/
/*** ============ ***/

/*
 * Loop over MC particles in a single event. Store the indices of the signal particles.
 */
void AliAnalysisTaskEsd2Tree::FillMC() {

    AliMCParticle* mcPart;

    for (Int_t mcIdx = 0; mcIdx < fMC->GetNumberOfTracks(); mcIdx++) {

        mcPart = (AliMCParticle*)fMC->GetTrack(mcIdx);
        if (!mcPart) continue;
        if (mcPart->P() < 0.01) continue;

        Int_t ancestor_idx = GetAncestor(mcIdx);

        /* Assign branches and fill tree */

        tMC_Idx = mcIdx;
        tMC_PdgCode = mcPart->PdgCode();
        tMC_Idx_Mother = mcPart->GetMother();
        tMC_Idx_Ancestor = ancestor_idx;
        tMC_Px = (Float_t)mcPart->Px();
        tMC_Py = (Float_t)mcPart->Py();
        tMC_Pz = (Float_t)mcPart->Pz();
        tMC_Xv = (Float_t)mcPart->Xv();
        tMC_Yv = (Float_t)mcPart->Yv();
        tMC_Zv = (Float_t)mcPart->Zv();
        tMC_Status = mcPart->MCStatusCode();
        tMC_IsOOBPileup = AliAnalysisUtils::IsParticleFromOutOfBunchPileupCollision(mcIdx, fMC);
        tMC_Generator = mcPart->GetGeneratorIndex();
        tMC_IsPrimary = mcPart->IsPhysicalPrimary();
        tMC_IsSecFromMat = mcPart->IsSecondaryFromMaterial();
        tMC_IsSecFromWeak = mcPart->IsSecondaryFromWeakDecay();
        tMC_ReactionID = tMC_Generator == 2 ? GetReactionID(mcIdx, ancestor_idx) : -1;

        fTree_MC->Fill();
    }  // end of loop over MC particles
}

/*
 * Rercursive function to get the first particle in the decay chain of a particle and the reaction ID of it.
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
 * Get the Reaction ID of a particle.
 */
Int_t AliAnalysisTaskEsd2Tree::GetReactionID(Int_t mcIdx, Int_t ancestorIdx) {
    AliMCParticle* thisMcParticle;
    if (ancestorIdx < 0)
        thisMcParticle = (AliMCParticle*)fMC->GetTrack(mcIdx);
    else
        thisMcParticle = (AliMCParticle*)fMC->GetTrack(ancestorIdx);
    return thisMcParticle->MCStatusCode();
}

/*                   */
/**  Reconstructed  **/
/*** ============= ***/

/*
 * Loop over the reconstructed tracks in a single event.
 */
void AliAnalysisTaskEsd2Tree::FillTracks() {

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
        if (trackInnerParam->Pt() < 1E-2 || trackInnerParam->Pt() > 1E2) continue;
        if (!PassesTrackSelection(track)) continue;

        /* Assign branches and fill tree */

        tTrack_Idx = esdIdx;
        trackInnerParam->GetPxPyPz(momentum);
        tTrack_Px = (Float_t)momentum[0];
        tTrack_Py = (Float_t)momentum[1];
        tTrack_Pz = (Float_t)momentum[2];
        trackInnerParam->GetXYZ(position);
        tTrack_X = (Float_t)position[0];
        tTrack_Y = (Float_t)position[1];
        tTrack_Z = (Float_t)position[2];
        tTrack_Charge = trackInnerParam->Charge();
        tTrack_Alpha = (Float_t)trackInnerParam->GetAlpha();
        tTrack_Snp = (Float_t)trackInnerParam->GetSnp();
        tTrack_Tgl = (Float_t)trackInnerParam->GetTgl();
        tTrack_Signed1Pt = (Float_t)trackInnerParam->GetSigned1Pt();
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
        tTrack_Idx_True = fIsMC ? TMath::Abs(track->GetLabel()) : -1;

        fTree_Tracks->Fill();
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

/*                 */
/**  Signal Logs  **/
/*** =========== ***/

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

        fMap_ReactionID[CurrentEventNumber].push_back(dynamic_cast<TObjString*>(csv_arr->At(0))->String().Atoi());
        fMap_Sexaquark_Px[CurrentEventNumber].push_back(dynamic_cast<TObjString*>(csv_arr->At(1))->String().Atof());
        fMap_Sexaquark_Py[CurrentEventNumber].push_back(dynamic_cast<TObjString*>(csv_arr->At(2))->String().Atof());
        fMap_Sexaquark_Pz[CurrentEventNumber].push_back(dynamic_cast<TObjString*>(csv_arr->At(3))->String().Atof());
        fMap_Nucleon_Px[CurrentEventNumber].push_back(dynamic_cast<TObjString*>(csv_arr->At(4))->String().Atof());
        fMap_Nucleon_Py[CurrentEventNumber].push_back(dynamic_cast<TObjString*>(csv_arr->At(5))->String().Atof());
        fMap_Nucleon_Pz[CurrentEventNumber].push_back(dynamic_cast<TObjString*>(csv_arr->At(6))->String().Atof());
    }  // end of loop over lines

    AliInfoF("Closing file %s ...", new_path.Data());
    SignalLog.close();

    return kTRUE;
}

/*
 * Clear the maps from memory.
 */
void AliAnalysisTaskEsd2Tree::ClearSignalLogs() {
    fMap_ReactionID.clear();
    fMap_Sexaquark_Px.clear();
    fMap_Sexaquark_Py.clear();
    fMap_Sexaquark_Pz.clear();
    fMap_Nucleon_Px.clear();
    fMap_Nucleon_Py.clear();
    fMap_Nucleon_Pz.clear();
}

/*
 * Assign the in-memory values to the tree branches.
 */
void AliAnalysisTaskEsd2Tree::FillInjected() {
    for (Int_t i = 0; i < (Int_t)fMap_ReactionID[fEventNumber].size(); i++) {
        tInjected_ReactionID = fMap_ReactionID[fEventNumber][i];
        tInjected_Px = fMap_Sexaquark_Px[fEventNumber][i];
        tInjected_Py = fMap_Sexaquark_Py[fEventNumber][i];
        tInjected_Pz = fMap_Sexaquark_Pz[fEventNumber][i];
        tInjected_Nucleon_Px = fMap_Nucleon_Px[fEventNumber][i];
        tInjected_Nucleon_Py = fMap_Nucleon_Py[fEventNumber][i];
        tInjected_Nucleon_Pz = fMap_Nucleon_Pz[fEventNumber][i];
        fTree_Injected->Fill();
    }
}
