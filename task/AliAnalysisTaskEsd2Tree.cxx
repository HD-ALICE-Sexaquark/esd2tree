#include <sstream>
#include <string>

#include "AliAnalysisTaskEsd2Tree.h"
#include "AliAnalysisTaskEsd2Vector_Const.h"

ClassImp(AliAnalysisTaskEsd2Tree);

// Empty I/O constructor. Non-persistent members are initialized to their default values from here.
AliAnalysisTaskEsd2Tree::AliAnalysisTaskEsd2Tree()
    : AliAnalysisTaskSE(""),
      /*  */
      fIsMC(false),
      fIsSignalMC(false),
      /*  */
      fMC(nullptr),
      fMC_PrimaryVertex(nullptr),
      fESD(nullptr),
      fPrimaryVertex(nullptr),
      fPIDResponse(nullptr),
      fEventCuts(),
      /*  */
      fRunNumber(0),
      fDirNumber(0),
      fDirNumberB(0),
      fEventNumberInFile(0),
      fCentrality(0.),
      fMagneticField(0.),
      /*  */
      fAliEnPath(""),
      fSignalLog_NewBasename(""),
      /*  */
      fVec_McEntry(),
      fEvVec_ReactionID(Const::NEventsPerSignalMCLog, std::vector<unsigned int>(Const::NSignalReactionsPerEvent)),
      fEvVec_Sexaquark_Px(Const::NEventsPerSignalMCLog, std::vector<float>(Const::NSignalReactionsPerEvent)),
      fEvVec_Sexaquark_Py(Const::NEventsPerSignalMCLog, std::vector<float>(Const::NSignalReactionsPerEvent)),
      fEvVec_Sexaquark_Pz(Const::NEventsPerSignalMCLog, std::vector<float>(Const::NSignalReactionsPerEvent)),
      fEvVec_Nucleon_Px(Const::NEventsPerSignalMCLog, std::vector<float>(Const::NSignalReactionsPerEvent)),
      fEvVec_Nucleon_Py(Const::NEventsPerSignalMCLog, std::vector<float>(Const::NSignalReactionsPerEvent)),
      fEvVec_Nucleon_Pz(Const::NEventsPerSignalMCLog, std::vector<float>(Const::NSignalReactionsPerEvent)),
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
      tEvent_IsGenPileup(false),
      tEvent_IsSBCPileup(false),
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
      tEvent_IsMB(false),
      tEvent_IsHighMultV0(false),
      tEvent_IsHighMultSPD(false),
      tEvent_IsCentral(false),
      tEvent_IsSemiCentral(false),
      /*  */
      tInjected_ReactionID(),
      tInjected_Px(),
      tInjected_Py(),
      tInjected_Pz(),
      tInjected_Nucleon_Px(),
      tInjected_Nucleon_Py(),
      tInjected_Nucleon_Pz(),
      /*  */
      tMC_PdgCode(),
      tMC_Mother_McEntry(),
      tMC_Px(),
      tMC_Py(),
      tMC_Pz(),
      tMC_Xv(),
      tMC_Yv(),
      tMC_Zv(),
      tMC_Status(),
      tMC_IsOOBPileup(),
      tMC_Generator(),
      tMC_IsPrimary(),
      tMC_IsSecFromMat(),
      tMC_IsSecFromWeak(),
      /*  */
      tTrack_Px(),
      tTrack_Py(),
      tTrack_Pz(),
      tTrack_X(),
      tTrack_Y(),
      tTrack_Z(),
      tTrack_Charge(),
#if INCLUDE_MUCH_INFO
      tTrack_Alpha(),
      tTrack_Snp(),
      tTrack_Tgl(),
      tTrack_Signed1Pt(),
      tTrack_SigmaY2(),
      tTrack_SigmaZY(),
      tTrack_SigmaZ2(),
      tTrack_SigmaSnpY(),
      tTrack_SigmaSnpZ(),
      tTrack_SigmaSnp2(),
      tTrack_SigmaTglY(),
      tTrack_SigmaTglZ(),
      tTrack_SigmaTglSnp(),
      tTrack_SigmaTgl2(),
      tTrack_Sigma1PtY(),
      tTrack_Sigma1PtZ(),
      tTrack_Sigma1PtSnp(),
      tTrack_Sigma1PtTgl(),
      tTrack_Sigma1Pt2(),
      tTrack_NSigmaPion(),
      tTrack_NSigmaKaon(),
      tTrack_NSigmaProton(),
      tTrack_DCAxy(),
      tTrack_DCAz(),
      tTrack_NTPCClusters(),
      tTrack_NCrossedRows(),
      tTrack_NFindableClusters(),
      tTrack_NSharedClusters(),
      tTrack_Chi2overNcls(),
#endif
      tTrack_IsKinkDaughter(),
      // tTrack_TPCFitMap(),
      // tTrack_TPCClusterMap(),
      // tTrack_TPCSharedMap(),
      tTrack_McEntry() {
}

// Constructor, called locally.
AliAnalysisTaskEsd2Tree::AliAnalysisTaskEsd2Tree(const char* name)
    : AliAnalysisTaskSE(name),
      /*  */
      fIsMC(false),
      fIsSignalMC(false),
      /*  */
      fMC(nullptr),
      fMC_PrimaryVertex(nullptr),
      fESD(nullptr),
      fPrimaryVertex(nullptr),
      fPIDResponse(nullptr),
      fEventCuts(),
      /*  */
      fRunNumber(0),
      fDirNumber(0),
      fDirNumberB(0),
      fEventNumberInFile(0),
      fCentrality(0.),
      fMagneticField(0.),
      /*  */
      fAliEnPath(""),
      fSignalLog_NewBasename(""),
      /*  */
      fVec_McEntry(),
      fEvVec_ReactionID(Const::NEventsPerSignalMCLog, std::vector<unsigned int>(Const::NSignalReactionsPerEvent)),
      fEvVec_Sexaquark_Px(Const::NEventsPerSignalMCLog, std::vector<float>(Const::NSignalReactionsPerEvent)),
      fEvVec_Sexaquark_Py(Const::NEventsPerSignalMCLog, std::vector<float>(Const::NSignalReactionsPerEvent)),
      fEvVec_Sexaquark_Pz(Const::NEventsPerSignalMCLog, std::vector<float>(Const::NSignalReactionsPerEvent)),
      fEvVec_Nucleon_Px(Const::NEventsPerSignalMCLog, std::vector<float>(Const::NSignalReactionsPerEvent)),
      fEvVec_Nucleon_Py(Const::NEventsPerSignalMCLog, std::vector<float>(Const::NSignalReactionsPerEvent)),
      fEvVec_Nucleon_Pz(Const::NEventsPerSignalMCLog, std::vector<float>(Const::NSignalReactionsPerEvent)),
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
      tEvent_IsGenPileup(false),
      tEvent_IsSBCPileup(false),
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
      tEvent_IsMB(false),
      tEvent_IsHighMultV0(false),
      tEvent_IsHighMultSPD(false),
      tEvent_IsCentral(false),
      tEvent_IsSemiCentral(false),
      /*  */
      tInjected_ReactionID(),
      tInjected_Px(),
      tInjected_Py(),
      tInjected_Pz(),
      tInjected_Nucleon_Px(),
      tInjected_Nucleon_Py(),
      tInjected_Nucleon_Pz(),
      /*  */
      tMC_PdgCode(),
      tMC_Mother_McEntry(),
      tMC_Px(),
      tMC_Py(),
      tMC_Pz(),
      tMC_Xv(),
      tMC_Yv(),
      tMC_Zv(),
      tMC_Status(),
      tMC_IsOOBPileup(),
      tMC_Generator(),
      tMC_IsPrimary(),
      tMC_IsSecFromMat(),
      tMC_IsSecFromWeak(),
      /*  */
      tTrack_Px(),
      tTrack_Py(),
      tTrack_Pz(),
      tTrack_X(),
      tTrack_Y(),
      tTrack_Z(),
      tTrack_Charge(),
#if INCLUDE_MUCH_INFO
      tTrack_Alpha(),
      tTrack_Snp(),
      tTrack_Tgl(),
      tTrack_Signed1Pt(),
      tTrack_SigmaY2(),
      tTrack_SigmaZY(),
      tTrack_SigmaZ2(),
      tTrack_SigmaSnpY(),
      tTrack_SigmaSnpZ(),
      tTrack_SigmaSnp2(),
      tTrack_SigmaTglY(),
      tTrack_SigmaTglZ(),
      tTrack_SigmaTglSnp(),
      tTrack_SigmaTgl2(),
      tTrack_Sigma1PtY(),
      tTrack_Sigma1PtZ(),
      tTrack_Sigma1PtSnp(),
      tTrack_Sigma1PtTgl(),
      tTrack_Sigma1Pt2(),
      tTrack_NSigmaPion(),
      tTrack_NSigmaKaon(),
      tTrack_NSigmaProton(),
      tTrack_DCAxy(),
      tTrack_DCAz(),
      tTrack_NTPCClusters(),
      tTrack_NCrossedRows(),
      tTrack_NFindableClusters(),
      tTrack_NSharedClusters(),
      tTrack_Chi2overNcls(),
#endif
      tTrack_IsKinkDaughter(),
      // tTrack_TPCFitMap(),
      // tTrack_TPCClusterMap(),
      // tTrack_TPCSharedMap(),
      tTrack_McEntry() {
    DefineInput(0, TChain::Class());
    DefineOutput(1, TList::Class());  // fOutputList
    DefineOutput(2, TTree::Class());  // fOutputTree
}

// Destructor.
// Note: if `TList::SetOwner(kTRUE)` was called, the TList destructor should delete all objects added to it.
AliAnalysisTaskEsd2Tree::~AliAnalysisTaskEsd2Tree() {
    delete fOutputList;
    delete fOutputTree;
}

// Initialize analysis task. Needs to be called within an `AddTaskEsd2Tree.C` macro.
void AliAnalysisTaskEsd2Tree::Initialize() {
    // Print settings //
    AliInfo("Initializing...");
    AliInfo("Settings:");
    AliInfo("========");
    AliInfoF(">> IsMC       = %i", (Int_t)fIsMC);
    AliInfoF(">> IsSignalMC = %i", (Int_t)fIsSignalMC);
}

// Executed at runtime //

// Create output objects, called once at RUNTIME ~ execution on Grid.
void AliAnalysisTaskEsd2Tree::UserCreateOutputObjects() {
    //
    AliAnalysisManager* man = AliAnalysisManager::GetAnalysisManager();
    if (man == nullptr) AliFatal("ERROR: AliAnalysisManager couldn't be found.");

    auto* inputHandler = dynamic_cast<AliESDInputHandler*>(man->GetInputEventHandler());
    if (inputHandler == nullptr) AliFatal("ERROR: AliESDInputHandler couldn't be found.");

    fPIDResponse = inputHandler->GetPIDResponse();

    // Prepare output list //

    fOutputList = new TList();
    fOutputList->SetOwner(true);

    fHist_Events_Bookkeeping = new TH1F("Events_Bookkeeping", ";;Counts", 10, 0., 10.);
    fOutputList->Add(fHist_Events_Bookkeeping);

    fHist_Centrality = new TH1F("Centrality", ";Centrality;Counts", 11, 0., 110.);
    fOutputList->Add(fHist_Centrality);

    fHist_CentralityINT7 = new TH1F("CentralityINT7", ";CentralityINT7;Counts", 11, 0., 110.);
    fOutputList->Add(fHist_CentralityINT7);

    // Prepare output tree //
    fOutputTree = new TTree("Events", "Events");
    CreateEventsBranches();
    if (fIsMC) {
        CreateMCBranches();
        if (fIsSignalMC) CreateInjectedBranches();
    }
    CreateTracksBranches();
    // Post data //
    PostData(1, fOutputList);
    PostData(2, fOutputTree);
}

// User implementation of `Notify()`. Needed for reading the AliEn path.
// This function is loaded during `AliAnalysisManager::Notify()`.
// It's called after `UserCreateOutputObjects()`, for each new file, and before the first `UserExec()`.
Bool_t AliAnalysisTaskEsd2Tree::UserNotify() {
    //
    AliAnalysisManager* man = AliAnalysisManager::GetAnalysisManager();
    if (man == nullptr) AliFatal("Analysis Manager not found");
    TTree* man_tree = man->GetTree();
    if (man_tree == nullptr) AliFatal("Analysis Manager Tree not found");
    TFile* man_file = man_tree->GetCurrentFile();
    if (man_file == nullptr) AliFatal("Analysis Manager File not found");

    // get AliEn path and tokenize it //

    fAliEnPath = man_file->GetName();
    if (fAliEnPath == "") AliWarning("fAliEnPath couldn't be found.");
    AliInfoF("fAliEnPath: %s", fAliEnPath.Data());

    TObjArray* tokens = fAliEnPath.Tokenize("/");
    if (fIsMC) {
        // path of signal MC ends with format `.../LHC23l1a3/A1.73/297595/001/AliESDs.root` //
        // and path of general purpose MC ends with format `.../LHC20e3a/297595/001/AliESDs.root` //
        fDirNumber = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries() - 2)))->GetString().Atof();
        AliInfoF("Dir Number : %04i", (Int_t)fDirNumber);
        if (fIsSignalMC) {
            BringSignalLogs();
            LoadSignalLogs();
        }
    } else {
        // path of data ends with format `.../LHC15o/000245232/pass2/15000245232039.914/AliESDs.root` //
        TString aux_dir_nr = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries() - 2)))->GetString();
        aux_dir_nr = TString(aux_dir_nr(2 + 3 + 6, 10));  // = "039.914"
        AliInfoF("Dir Number : %s", aux_dir_nr.Data());
        fDirNumber = TString(aux_dir_nr(0, 3)).Atoi();   // = 39
        fDirNumberB = TString(aux_dir_nr(4, 5)).Atoi();  // = 914
    }

    return true;
}

// Main function, called per each event at RUNTIME ~ execution on Grid.
void AliAnalysisTaskEsd2Tree::UserExec(Option_t*) {

    // Load MC Gen. Event and PV //

    if (fIsMC) {
        fMC = MCEvent();
        if (fMC == nullptr) AliFatal("AliMCEvent couldn't be found.");
        fMC_PrimaryVertex = fMC->GetPrimaryVertex();
        fPIDResponse->SetCurrentMCEvent(fMC);
    }

    // Load Reconstructed Event, PV and Magnetic Field //

    fESD = dynamic_cast<AliESDEvent*>(InputEvent());
    if (fESD == nullptr) AliFatal("AliESDEvent couldn't be found.");

    fPrimaryVertex = fESD->GetPrimaryVertex();
    fMagneticField = (Float_t)fESD->GetMagneticField();

    fRunNumber = fESD->GetRunNumber();
    fEventNumberInFile = fESD->GetEventNumberInFile();

    // Event selection //

    if (!PassesEventSelection()) return;

    // Centrality //

    auto* MultSelection = dynamic_cast<AliMultSelection*>(fESD->FindListObject("MultSelection"));
    if (MultSelection == nullptr) AliFatal("AliMultSelection couldn't be found.");
    fCentrality = MultSelection->GetMultiplicityPercentile("V0M");

    fHist_Centrality->Fill(fCentrality);
    if ((fInputHandler->IsEventSelected() & AliVEvent::kINT7) != 0U) fHist_CentralityINT7->Fill(fCentrality);

    // Main //

    ProcessEvent();

    if (fIsMC) {
        if (fIsSignalMC) ProcessInjectedReactions();
        ProcessMCParticles();
    }
    ProcessTracks();

    fOutputTree->Fill();

    // End of event //

    if (fIsMC) {
        ClearMCBranches();
        fVec_McEntry.clear();
    }
    ClearTracksBranches();

    PostData(1, fOutputList);
    PostData(2, fOutputTree);
}

// # Events //

// Apply event selection.
Bool_t AliAnalysisTaskEsd2Tree::PassesEventSelection() {

    fHist_Events_Bookkeeping->Fill(0.);

    // First Check //

    if (!fEventCuts.AcceptEvent(fESD)) return false;
    fHist_Events_Bookkeeping->Fill(1.);

    // Reference: https://twiki.cern.ch/twiki/bin/viewauth/ALICE/AliDPGRunList18r1 //

    if (!fIsMC && (fRunNumber == 296749 || fRunNumber == 296750 || fRunNumber == 296849 || fRunNumber == 296890 || fRunNumber == 297029 ||
                   fRunNumber == 297194 || fRunNumber == 297219 || fRunNumber == 297481)) {
        fEventCuts.UseTimeRangeCut();
        fEventCuts.OverrideAutomaticTriggerSelection(AliVEvent::kINT7);
        if (!fEventCuts.AcceptEvent(fESD)) return false;
    }
    fHist_Events_Bookkeeping->Fill(2.);

    // Pileup Events //

    if (!fEventCuts.PassedCut(AliEventCuts::kPileUp)) return false;
    fHist_Events_Bookkeeping->Fill(3.);

    // TPC Pileup Events //

    if (!fEventCuts.PassedCut(AliEventCuts::kTPCPileUp)) return false;
    fHist_Events_Bookkeeping->Fill(4.);

    // Important for data? //

    if (!fIsMC && fESD->GetHeader()->GetEventType() != 7) return false;
    fHist_Events_Bookkeeping->Fill(5.);

    // Trigger Selection //

    Bool_t IsMB = (fInputHandler->IsEventSelected() & AliVEvent::kINT7) != 0U;
    Bool_t IsHighMultV0 = (fInputHandler->IsEventSelected() & AliVEvent::kHighMultV0) != 0U;
    Bool_t IsHighMultSPD = (fInputHandler->IsEventSelected() & AliVEvent::kHighMultSPD) != 0U;
    Bool_t IsCentral = (fInputHandler->IsEventSelected() & AliVEvent::kCentral) != 0U;
    Bool_t IsSemiCentral = (fInputHandler->IsEventSelected() & AliVEvent::kSemiCentral) != 0U;

    if (!IsMB && !IsHighMultV0 && !IsHighMultSPD && !IsCentral && !IsSemiCentral) return false;
    fHist_Events_Bookkeeping->Fill(6.);

    // rec. PV z-vertex range //

    if (std::abs(fPrimaryVertex->GetZ()) > Cuts::Event::AbsMax_PV_Zv) return false;
    fHist_Events_Bookkeeping->Fill(7.);

    return true;
}

void AliAnalysisTaskEsd2Tree::ProcessEvent() {

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
    for (size_t i = 0; i < 6; ++i) tEvent_PV_CovMatrix[i] = (Float_t)PV_CovMatrix[i];

    const AliESDVertex* PrimaryVertex_SPD = (AliESDVertex*)fESD->GetPrimaryVertexSPD();
    tEvent_SPD_PV_Zv = (Float_t)PrimaryVertex_SPD->GetZ();
    Double_t PV_SPD_CovMatrix[6];
    PrimaryVertex_SPD->GetCovarianceMatrix(PV_SPD_CovMatrix);
    tEvent_SPD_PV_ZvErr = (Float_t)PV_SPD_CovMatrix[5];

    tEvent_NTracks = (UInt_t)fESD->GetNumberOfTracks();
    tEvent_NTPCClusters = fESD->GetNumberOfTPCClusters();
    tEvent_IsMB = ((fInputHandler->IsEventSelected() & AliVEvent::kINT7) != 0U);
    tEvent_IsHighMultV0 = ((fInputHandler->IsEventSelected() & AliVEvent::kHighMultV0) != 0U);
    tEvent_IsHighMultSPD = ((fInputHandler->IsEventSelected() & AliVEvent::AliVEvent::kHighMultSPD) != 0U);
    tEvent_IsCentral = ((fInputHandler->IsEventSelected() & AliVEvent::kCentral) != 0U);
    tEvent_IsSemiCentral = ((fInputHandler->IsEventSelected() & AliVEvent::kSemiCentral) != 0U);
}

// # Trees //

// Add branches to `fOutputTree`.
void AliAnalysisTaskEsd2Tree::CreateEventsBranches() {
    fOutputTree->Branch("RunNumber", &fRunNumber, "RunNumber/i");
    fOutputTree->Branch("DirNumber", &fDirNumber, "DirNumber/i");
    if (!fIsMC) fOutputTree->Branch("DirNumberB", &fDirNumberB, "DirNumberB/i");
    fOutputTree->Branch("EventNumber", &fEventNumberInFile, "EventNumber/i");
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
    fOutputTree->Branch("SPD_PV_Zv", &tEvent_SPD_PV_Zv, "SPD_PV_Zv/F");
    fOutputTree->Branch("SPD_PV_ZvErr", &tEvent_SPD_PV_ZvErr, "SPD_PV_ZvErr/F");
    fOutputTree->Branch("NTracks", &tEvent_NTracks, "NTracks/i");
    fOutputTree->Branch("NTPCClusters", &tEvent_NTPCClusters, "NTPCClusters/I");
    fOutputTree->Branch("IsMB", &tEvent_IsMB, "IsMB/O");
    fOutputTree->Branch("IsHighMultV0", &tEvent_IsHighMultV0, "IsHighMultV0/O");
    fOutputTree->Branch("IsHighMultSPD", &tEvent_IsHighMultSPD, "IsHighMultSPD/O");
    fOutputTree->Branch("IsCentral", &tEvent_IsCentral, "IsCentral/O");
    fOutputTree->Branch("IsSemiCentral", &tEvent_IsSemiCentral, "IsSemiCentral/O");
}

// Add branches to `fTree_Injected`.
void AliAnalysisTaskEsd2Tree::CreateInjectedBranches() {
    fOutputTree->Branch("ReactionID", &tInjected_ReactionID);
    fOutputTree->Branch("Sexaquark_Px", &tInjected_Px);
    fOutputTree->Branch("Sexaquark_Py", &tInjected_Py);
    fOutputTree->Branch("Sexaquark_Pz", &tInjected_Pz);
    fOutputTree->Branch("Nucleon_Px", &tInjected_Nucleon_Px);
    fOutputTree->Branch("Nucleon_Py", &tInjected_Nucleon_Py);
    fOutputTree->Branch("Nucleon_Pz", &tInjected_Nucleon_Pz);
}

// Add branches to `fTree_MC`.
void AliAnalysisTaskEsd2Tree::CreateMCBranches() {
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

// Add branches to `fTree_Tracks`.
void AliAnalysisTaskEsd2Tree::CreateTracksBranches() {
#if WRITE_ESD_INDICES
    fOutputTree->Branch("Track_EsdIdx", &tTrack_EsdIdx);
#endif
    fOutputTree->Branch("Track_Px", &tTrack_Px);
    fOutputTree->Branch("Track_Py", &tTrack_Py);
    fOutputTree->Branch("Track_Pz", &tTrack_Pz);
    fOutputTree->Branch("Track_X", &tTrack_X);
    fOutputTree->Branch("Track_Y", &tTrack_Y);
    fOutputTree->Branch("Track_Z", &tTrack_Z);
    fOutputTree->Branch("Track_Charge", &tTrack_Charge);
#if INCLUDE_MUCH_INFO
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
#endif
    fOutputTree->Branch("Track_IsKinkDaughter", &tTrack_IsKinkDaughter);
    // fOutputTree->Branch("TPCFitMap", &tTrack_TPCFitMap);
    // fOutputTree->Branch("TPCClusterMap", &tTrack_TPCClusterMap);
    // fOutputTree->Branch("TPCSharedMap", &tTrack_TPCSharedMap);
    if (fIsMC) fOutputTree->Branch("Track_McEntry", &tTrack_McEntry);
}

// # MC Generated //

// Loop over MC particles in a single event.
void AliAnalysisTaskEsd2Tree::ProcessMCParticles() {
    AliMCParticle* mcPart{nullptr};
    // prepare vectors
    const int n_mc{fMC->GetNumberOfTracks()};
    fVec_McEntry.resize(n_mc, -1);
    tMC_PdgCode.reserve(n_mc);
    tMC_Mother_McEntry.reserve(n_mc);
    tMC_Px.reserve(n_mc);
    tMC_Py.reserve(n_mc);
    tMC_Pz.reserve(n_mc);
    tMC_Xv.reserve(n_mc);
    tMC_Yv.reserve(n_mc);
    tMC_Zv.reserve(n_mc);
    tMC_Status.reserve(n_mc);
    tMC_IsOOBPileup.reserve(n_mc);
    tMC_Generator.reserve(n_mc);
    tMC_IsPrimary.reserve(n_mc);
    tMC_IsSecFromMat.reserve(n_mc);
    tMC_IsSecFromWeak.reserve(n_mc);
    // read mc particles
    Long_t mc_entry = 0;
    for (auto mc_idx{0}; mc_idx < n_mc; ++mc_idx) {
        mcPart = dynamic_cast<AliMCParticle*>(fMC->GetTrack(mc_idx));
        if (mcPart == nullptr) continue;
        // Remove trash //
        if (mcPart->P() < Cuts::MC::Min_Momentum) continue;
        // Add to vectors //
        fVec_McEntry[mc_idx] = mc_entry;
        tMC_PdgCode.emplace_back(mcPart->PdgCode());
        tMC_Mother_McEntry.emplace_back(mcPart->GetMother() > 0 ? fVec_McEntry[mcPart->GetMother()] : -2);
        tMC_Px.emplace_back(static_cast<float>(mcPart->Px()));
        tMC_Py.emplace_back(static_cast<float>(mcPart->Py()));
        tMC_Pz.emplace_back(static_cast<float>(mcPart->Pz()));
        tMC_Xv.emplace_back(static_cast<float>(mcPart->Xv()));
        tMC_Yv.emplace_back(static_cast<float>(mcPart->Yv()));
        tMC_Zv.emplace_back(static_cast<float>(mcPart->Zv()));
        tMC_Status.emplace_back(mcPart->MCStatusCode());
        tMC_IsOOBPileup.emplace_back(AliAnalysisUtils::IsParticleFromOutOfBunchPileupCollision(mc_idx, fMC));
        tMC_Generator.emplace_back(mcPart->GetGeneratorIndex());
        tMC_IsPrimary.emplace_back(mcPart->IsPhysicalPrimary());
        tMC_IsSecFromMat.emplace_back(mcPart->IsSecondaryFromMaterial());
        tMC_IsSecFromWeak.emplace_back(mcPart->IsSecondaryFromWeakDecay());
        ++mc_entry;
    }  // end of loop over MC particles
}

// Clear MC branches.
void AliAnalysisTaskEsd2Tree::ClearMCBranches() {
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

// # Reconstructed //

// Loop over the reconstructed tracks in a single event.
void AliAnalysisTaskEsd2Tree::ProcessTracks() {

    AliESDtrack* track{nullptr};
    const AliExternalTrackParam* trackInnerParam{nullptr};

    for (auto esd_idx{0}; esd_idx < fESD->GetNumberOfTracks(); ++esd_idx) {
        track = fESD->GetTrack(esd_idx);
        trackInnerParam = track->GetInnerParam();
        // Track selection //
        if (trackInnerParam == nullptr) continue;
        if (trackInnerParam->Pt() < Cuts::Track::Min_Pt || trackInnerParam->Pt() > Cuts::Track::Max_Pt) continue;
        if (!PassesTrackSelection(track)) continue;
// Assign branches //
#if WRITE_ESD_INDICES
        tTrack_EsdIdx.push_back((Long_t)esd_idx);
#endif
        double momentum[3];
        trackInnerParam->GetPxPyPz(momentum);
        tTrack_Px.push_back(static_cast<float>(momentum[0]));
        tTrack_Py.push_back(static_cast<float>(momentum[1]));
        tTrack_Pz.push_back(static_cast<float>(momentum[2]));
        double position[3];
        trackInnerParam->GetXYZ(position);
        tTrack_X.push_back(static_cast<float>(position[0]));
        tTrack_Y.push_back(static_cast<float>(position[1]));
        tTrack_Z.push_back(static_cast<float>(position[2]));
        tTrack_Charge.push_back(trackInnerParam->Charge());
#if INCLUDE_MUCH_INFO
        tTrack_Alpha.push_back((Float_t)trackInnerParam->GetAlpha());
        tTrack_Snp.push_back((Float_t)trackInnerParam->GetSnp());
        tTrack_Tgl.push_back((Float_t)trackInnerParam->GetTgl());
        tTrack_Signed1Pt.push_back((Float_t)trackInnerParam->GetSigned1Pt());
        const Double_t* covariance_matrix = trackInnerParam->GetCovariance();
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
        Float_t AuxDCAxy, AuxDCAz;
        track->GetImpactParameters(AuxDCAxy, AuxDCAz);
        tTrack_DCAxy.push_back(AuxDCAxy);
        tTrack_DCAz.push_back(AuxDCAz);
        tTrack_NTPCClusters.push_back(track->GetTPCNcls());  // Note: capital N
        tTrack_NCrossedRows.push_back(track->GetTPCCrossedRows());
        tTrack_NFindableClusters.push_back(track->GetTPCNclsF());
        tTrack_NSharedClusters.push_back(track->GetTPCnclsS());
        tTrack_Chi2overNcls.push_back(static_cast<Float_t>(track->GetTPCNcls() > 0 ? track->GetTPCchi2() / (Double_t)track->GetTPCNcls() : 999.));
#endif
        tTrack_IsKinkDaughter.push_back(track->GetKinkIndex(0) > 0);
        // tTrack_TPCFitMap = track->GetTPCFitMap();
        // tTrack_TPCClusterMap = track->GetTPCClusterMap();
        // tTrack_TPCSharedMap = track->GetTPCSharedMap();
        if (fIsMC) tTrack_McEntry.push_back(fVec_McEntry[std::abs(track->GetLabel())]);
    }  // end of loop over tracks
}

// Check if track passes selection and fill bookkeeping histograms.
Bool_t AliAnalysisTaskEsd2Tree::PassesTrackSelection(AliESDtrack* track) {

    const AliExternalTrackParam* trackInnerParam = track->GetInnerParam();

    // Fulfill at least one of the PID cuts //

    Float_t n_sigma_proton = fPIDResponse->NumberOfSigmasTPC(track, AliPID::kProton);
    Float_t n_sigma_kaon = fPIDResponse->NumberOfSigmasTPC(track, AliPID::kKaon);
    Float_t n_sigma_pion = fPIDResponse->NumberOfSigmasTPC(track, AliPID::kPion);
    if (std::abs(n_sigma_proton) > Cuts::Track::AbsMax_NSigma_Proton && std::abs(n_sigma_kaon) > Cuts::Track::AbsMax_NSigma_Kaon &&
        std::abs(n_sigma_pion) > Cuts::Track::AbsMax_NSigma_Pion) {
        return false;
    }

    if (std::abs(trackInnerParam->Eta()) > Cuts::Track::AbsMax_Eta) return false;

    Double_t NTPCClusters = track->GetTPCNcls();
    if (NTPCClusters < Cuts::Track::Min_NTPCClusters) return false;

    Double_t Chi2PerNTPCClusters = NTPCClusters > 1E-4 ? track->GetTPCchi2() / (Double_t)track->GetTPCNcls() : 999.;
    if (Chi2PerNTPCClusters > Cuts::Track::Max_Chi2PerNTPCClusters) return false;

    // >> TPC and ITS status
    Bool_t tpc_status = ((track->GetStatus() & AliESDtrack::kTPCout) != 0U) && ((track->GetStatus() & AliESDtrack::kTPCrefit) != 0U);
    if (Cuts::Track::TurnedOn_StatusCuts && !tpc_status) return false;

    Bool_t its_status = ((track->GetStatus() & AliESDtrack::kITSin) == 0U) && ((track->GetStatus() & AliESDtrack::kITSout) == 0U) &&
                        ((track->GetStatus() & AliESDtrack::kITSrefit) == 0U);
    if (Cuts::Track::TurnedOn_StatusCuts && !its_status) return false;

    if (Cuts::Track::TurnedOn_RejectKinks && track->GetKinkIndex(0) > 0) return false;

    Float_t DCAxy_wrtPV, DCAz_wrtPV;
    track->GetImpactParameters(DCAxy_wrtPV, DCAz_wrtPV);
    Float_t DCA_wrtPV = std::sqrt(DCAxy_wrtPV * DCAxy_wrtPV + DCAz_wrtPV * DCAz_wrtPV);
    if (std::abs(DCAxy_wrtPV) < Cuts::Track::Min_DCAxy_wrtPV) return false;

    return true;
}

// Clear the branches of the reconstructed tracks.
void AliAnalysisTaskEsd2Tree::ClearTracksBranches() {
#if WRITE_ESD_INDICES
    tTrack_EsdIdx.clear();
#endif
    tTrack_Px.clear();
    tTrack_Py.clear();
    tTrack_Pz.clear();
    tTrack_X.clear();
    tTrack_Y.clear();
    tTrack_Z.clear();
    tTrack_Charge.clear();
#if INCLUDE_MUCH_INFO
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
#endif
    tTrack_IsKinkDaughter.clear();
    // tTrack_TPCFitMap.clear();
    // tTrack_TPCClusterMap.clear();
    // tTrack_TPCSharedMap.clear();
    if (fIsMC) tTrack_McEntry.clear();
}

// # Injected Reactions //

// Assign the in-memory values to the tree branches.
void AliAnalysisTaskEsd2Tree::ProcessInjectedReactions() {
    tInjected_ReactionID = fEvVec_ReactionID[fEventNumberInFile];
    tInjected_Px = fEvVec_Sexaquark_Px[fEventNumberInFile];
    tInjected_Py = fEvVec_Sexaquark_Py[fEventNumberInFile];
    tInjected_Pz = fEvVec_Sexaquark_Pz[fEventNumberInFile];
    tInjected_Nucleon_Px = fEvVec_Nucleon_Px[fEventNumberInFile];
    tInjected_Nucleon_Py = fEvVec_Nucleon_Py[fEventNumberInFile];
    tInjected_Nucleon_Pz = fEvVec_Nucleon_Pz[fEventNumberInFile];
}

// Open the respective `sim.log` that corresponds to the `RunNumber+DirNumber` that's being analyzed.
// From it, read the injected anti-sexaquark and struck nucleon kinematics and store them into a tree.
void AliAnalysisTaskEsd2Tree::BringSignalLogs() {

    TGrid* alien = nullptr;
    if (gGrid == nullptr) {
        alien = TGrid::Connect("alien://");
        if (alien == nullptr) return;
    }

    TString AliEn_Dir = fAliEnPath(0, fAliEnPath.Last('/'));

    TString orig_path = Form("%s/sim.log", AliEn_Dir.Data());
    AliInfoF("Copying file %s ...", orig_path.Data());

    // assuming path ends with format `.../LHC23l1a3/A1.73/297595/001/sim.log` //
    auto AliEn_DirNumber = static_cast<int>(fDirNumber);
    TObjArray* tokens = fAliEnPath.Tokenize("/");
    Int_t AliEn_RunNumber = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries() - 3)))->GetString().Atoi();
    TString AliEn_SimSubSet = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries() - 4)))->GetString();

    fSignalLog_NewBasename = Form("sim_%s_%i_%03i.log", AliEn_SimSubSet.Data(), AliEn_RunNumber, AliEn_DirNumber);

    if (AliEn_Dir.BeginsWith("alien://")) {
        gSystem->Exec(Form("alien.py cp %s file://./%s", orig_path.Data(), fSignalLog_NewBasename.Data()));
    } else {
        gSystem->Exec(Form("cp %s ./%s", orig_path.Data(), fSignalLog_NewBasename.Data()));
    }

    TString new_path = Form("%s/%s", gSystem->pwd(), fSignalLog_NewBasename.Data());
    AliInfoF("Signal log file ready at %s ...", new_path.Data());
}

// Load injected anti-sexaquark and struck nucleon info.
// From the `sim.log` file that corresponds to an entire dir number into memory.
Bool_t AliAnalysisTaskEsd2Tree::LoadSignalLogs() {

    TString new_path = Form("%s/%s", gSystem->pwd(), fSignalLog_NewBasename.Data());
    AliInfoF("Opening file %s ...", new_path.Data());

    std::ifstream SimLogFile(new_path);
    if (!SimLogFile.is_open()) {
        AliWarningF("Unable to open file %s", new_path.Data());
        return false;
    }

    int ev{-1};
    int re{0};

    std::string line;
    while (std::getline(SimLogFile, line)) {
        // a new event has appeared //
        if (line.rfind(Const::SimLog_EventHeader) == 0) {
            ++ev;
            re = 0;
            continue;
        }
        if (ev >= 0) {
            if (line.rfind(Const::SimLog_ReactionMarker, 0) == 0) {
                std::string data_part{line.substr(Const::SimLog_ReactionMarker.length() + 1)};
                std::stringstream ss{data_part};
                std::string temp_parse_buffer;
                // clang-format off
                std::getline(ss, temp_parse_buffer, ','); fEvVec_ReactionID[ev][re] = std::stoul(temp_parse_buffer);
                std::getline(ss, temp_parse_buffer, ','); fEvVec_Sexaquark_Px[ev][re] = std::stof(temp_parse_buffer);
                std::getline(ss, temp_parse_buffer, ','); fEvVec_Sexaquark_Py[ev][re] = std::stof(temp_parse_buffer);
                std::getline(ss, temp_parse_buffer, ','); fEvVec_Sexaquark_Pz[ev][re] = std::stof(temp_parse_buffer);
                std::getline(ss, temp_parse_buffer, ','); fEvVec_Nucleon_Px[ev][re] = std::stof(temp_parse_buffer);
                std::getline(ss, temp_parse_buffer, ','); fEvVec_Nucleon_Py[ev][re] = std::stof(temp_parse_buffer);
                std::getline(ss, temp_parse_buffer); fEvVec_Nucleon_Pz[ev][re] = std::stof(temp_parse_buffer);
                // clang-format on
                ++re;
            }
        }
    }  // finish reading lines

    AliInfoF("Closing file %s ...", new_path.Data());
    SimLogFile.close();

    return true;
}
