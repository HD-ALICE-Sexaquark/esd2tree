#include <sstream>
#include <string>

#include "AliAnalysisTaskEsd2Vector.h"
#include "AliAnalysisTaskEsd2Vector_Const.h"
#include "AliAnalysisTaskEsd2Vector_Cuts.h"

ClassImp(AliAnalysisTaskEsd2Vector);

// Empty I/O constructor. Non-persistent members are initialized to their default values from here.
AliAnalysisTaskEsd2Vector::AliAnalysisTaskEsd2Vector()
    : AliAnalysisTaskSE{""},
      /*  */
      fIsMC{false},
      fIsSignalMC{false},
      /*  */
      fMC{nullptr},
      fMC_PrimaryVertex{nullptr},
      fESD{nullptr},
      fPrimaryVertex{nullptr},
      fPIDResponse{nullptr},
      fEventCuts{},
      /*  */
      fRunNumber{0},
      fDirNumber{0},
      fDirNumberB{0},
      fEventNumberInFile{0},
      fCentrality{0.},
      fMagneticField{0.},
      /*  */
      fAliEnPath{""},
      fSignalLog_NewBasename{""},
      /*  */
      fVec_McEntry{},
      fEvVec_ReactionID{Const::NEventsPerSignalMCLog, std::vector<int>(Const::NReactionsPerEvent)},
      fEvVec_Sexaquark_Px{Const::NEventsPerSignalMCLog, std::vector<float>(Const::NReactionsPerEvent)},
      fEvVec_Sexaquark_Py{Const::NEventsPerSignalMCLog, std::vector<float>(Const::NReactionsPerEvent)},
      fEvVec_Sexaquark_Pz{Const::NEventsPerSignalMCLog, std::vector<float>(Const::NReactionsPerEvent)},
      fEvVec_Nucleon_Px{Const::NEventsPerSignalMCLog, std::vector<float>(Const::NReactionsPerEvent)},
      fEvVec_Nucleon_Py{Const::NEventsPerSignalMCLog, std::vector<float>(Const::NReactionsPerEvent)},
      fEvVec_Nucleon_Pz{Const::NEventsPerSignalMCLog, std::vector<float>(Const::NReactionsPerEvent)},
      /*  */
      fOutputList{nullptr},
      fHist_Events_Bookkeeping{nullptr},
      fHist_Centrality{nullptr},
      fHist_CentralityINT7{nullptr},
      fHist_Tracks_Bookkeeping{nullptr},
      /*  */
      fOutputTree{nullptr},
      /*  */
      tEvent_PV_NContributors{0},
      tEvent_PV_Dispersion{0.},
      tEvent_PV_Xv{0.},
      tEvent_PV_Yv{0.},
      tEvent_PV_Zv{0.},
      tEvent_PV_CovMatrix{},
      tEvent_SPD_PV_Zv{0.},
      tEvent_SPD_PV_ZvErr{0.},
      tEvent_NTracks{0},
      tEvent_NTPCClusters{0},
      tEvent_IsMB{false},
      tEvent_IsHighMultV0{false},
      tEvent_IsHighMultSPD{false},
      tEvent_IsCentral{false},
      tEvent_IsSemiCentral{false},
      /*  */
      tEvent_MC_PV_Xv{0.},
      tEvent_MC_PV_Yv{0.},
      tEvent_MC_PV_Zv{0.},
      tEvent_MC_IsGenPileup{false},
      tEvent_MC_IsSBCPileup{false},
      /*  */
      tInjected_ReactionID{},
      tInjected_Px{},
      tInjected_Py{},
      tInjected_Pz{},
      tInjected_Nucleon_Px{},
      tInjected_Nucleon_Py{},
      tInjected_Nucleon_Pz{},
      /*  */
      tMC_PdgCode{},
      tMC_Mother_McEntry{},
      tMC_X{},
      tMC_Y{},
      tMC_Z{},
      tMC_Px{},
      tMC_Py{},
      tMC_Pz{},
      tMC_E{},
      tMC_Status{},
      tMC_IsOOBPileup{},
      tMC_Generator{},
      tMC_IsPrimary{},
      tMC_IsSecFromMat{},
      tMC_IsSecFromWeak{},
      /*  */
      tTrack_X{},
      tTrack_Y{},
      tTrack_Z{},
      tTrack_Px{},
      tTrack_Py{},
      tTrack_Pz{},
      tTrack_Charge{},
      tTrack_NSigmaPion{},
      tTrack_NSigmaKaon{},
      tTrack_NSigmaProton{},
      tTrack_DCAxy{},
      tTrack_DCAz{},
      tTrack_SignalTPC{},
      tTrack_Alpha{},
      tTrack_Snp{},
      tTrack_Tgl{},
      tTrack_Signed1Pt{},
      tTrack_SigmaY2{},
      tTrack_SigmaZY{},
      tTrack_SigmaZ2{},
      tTrack_SigmaSnpY{},
      tTrack_SigmaSnpZ{},
      tTrack_SigmaSnp2{},
      tTrack_SigmaTglY{},
      tTrack_SigmaTglZ{},
      tTrack_SigmaTglSnp{},
      tTrack_SigmaTgl2{},
      tTrack_Sigma1PtY{},
      tTrack_Sigma1PtZ{},
      tTrack_Sigma1PtSnp{},
      tTrack_Sigma1PtTgl{},
      tTrack_Sigma1Pt2{},
#if INCLUDE_MUCH_INFO
      tTrack_IsKinkDaughter{},
      tTrack_NTPCClusters{},
      tTrack_NCrossedRows{},
      tTrack_NFindableClusters{},
      tTrack_NSharedClusters{},
      tTrack_Chi2overNcls{},
#endif
      // tTrack_TPCFitMap(),
      // tTrack_TPCClusterMap(),
      // tTrack_TPCSharedMap(),
      tTrack_McEntry{} {
}

// Constructor, called locally.
AliAnalysisTaskEsd2Vector::AliAnalysisTaskEsd2Vector(const char* name)
    : AliAnalysisTaskSE{name},
      /*  */
      fIsMC{false},
      fIsSignalMC{false},
      /*  */
      fMC{nullptr},
      fMC_PrimaryVertex{nullptr},
      fESD{nullptr},
      fPrimaryVertex{nullptr},
      fPIDResponse{nullptr},
      fEventCuts{},
      /*  */
      fRunNumber{0},
      fDirNumber{0},
      fDirNumberB{0},
      fEventNumberInFile{0},
      fCentrality{0.},
      fMagneticField{0.},
      /*  */
      fAliEnPath{""},
      fSignalLog_NewBasename{""},
      /*  */
      fVec_McEntry{},
      fEvVec_ReactionID{Const::NEventsPerSignalMCLog, std::vector<int>(Const::NReactionsPerEvent)},
      fEvVec_Sexaquark_Px{Const::NEventsPerSignalMCLog, std::vector<float>(Const::NReactionsPerEvent)},
      fEvVec_Sexaquark_Py{Const::NEventsPerSignalMCLog, std::vector<float>(Const::NReactionsPerEvent)},
      fEvVec_Sexaquark_Pz{Const::NEventsPerSignalMCLog, std::vector<float>(Const::NReactionsPerEvent)},
      fEvVec_Nucleon_Px{Const::NEventsPerSignalMCLog, std::vector<float>(Const::NReactionsPerEvent)},
      fEvVec_Nucleon_Py{Const::NEventsPerSignalMCLog, std::vector<float>(Const::NReactionsPerEvent)},
      fEvVec_Nucleon_Pz{Const::NEventsPerSignalMCLog, std::vector<float>(Const::NReactionsPerEvent)},
      /*  */
      fOutputList{nullptr},
      fHist_Events_Bookkeeping{nullptr},
      fHist_Centrality{nullptr},
      fHist_CentralityINT7{nullptr},
      fHist_Tracks_Bookkeeping{nullptr},
      /*  */
      fOutputTree{nullptr},
      /*  */
      tEvent_PV_NContributors{0},
      tEvent_PV_Dispersion{0.},
      tEvent_PV_Xv{0.},
      tEvent_PV_Yv{0.},
      tEvent_PV_Zv{0.},
      tEvent_PV_CovMatrix{},
      tEvent_SPD_PV_Zv{0.},
      tEvent_SPD_PV_ZvErr{0.},
      tEvent_NTracks{0},
      tEvent_NTPCClusters{0},
      tEvent_IsMB{false},
      tEvent_IsHighMultV0{false},
      tEvent_IsHighMultSPD{false},
      tEvent_IsCentral{false},
      tEvent_IsSemiCentral{false},
      /*  */
      tEvent_MC_PV_Xv{0.},
      tEvent_MC_PV_Yv{0.},
      tEvent_MC_PV_Zv{0.},
      tEvent_MC_IsGenPileup{false},
      tEvent_MC_IsSBCPileup{false},
      /*  */
      tInjected_ReactionID{},
      tInjected_Px{},
      tInjected_Py{},
      tInjected_Pz{},
      tInjected_Nucleon_Px{},
      tInjected_Nucleon_Py{},
      tInjected_Nucleon_Pz{},
      /*  */
      tMC_PdgCode{},
      tMC_Mother_McEntry{},
      tMC_X{},
      tMC_Y{},
      tMC_Z{},
      tMC_Px{},
      tMC_Py{},
      tMC_Pz{},
      tMC_E{},
      tMC_Status{},
      tMC_IsOOBPileup{},
      tMC_Generator{},
      tMC_IsPrimary{},
      tMC_IsSecFromMat{},
      tMC_IsSecFromWeak{},
      /*  */
      tTrack_X{},
      tTrack_Y{},
      tTrack_Z{},
      tTrack_Px{},
      tTrack_Py{},
      tTrack_Pz{},
      tTrack_Charge{},
      tTrack_NSigmaPion{},
      tTrack_NSigmaKaon{},
      tTrack_NSigmaProton{},
      tTrack_DCAxy{},
      tTrack_DCAz{},
      tTrack_SignalTPC{},
      tTrack_Alpha{},
      tTrack_Snp{},
      tTrack_Tgl{},
      tTrack_Signed1Pt{},
      tTrack_SigmaY2{},
      tTrack_SigmaZY{},
      tTrack_SigmaZ2{},
      tTrack_SigmaSnpY{},
      tTrack_SigmaSnpZ{},
      tTrack_SigmaSnp2{},
      tTrack_SigmaTglY{},
      tTrack_SigmaTglZ{},
      tTrack_SigmaTglSnp{},
      tTrack_SigmaTgl2{},
      tTrack_Sigma1PtY{},
      tTrack_Sigma1PtZ{},
      tTrack_Sigma1PtSnp{},
      tTrack_Sigma1PtTgl{},
      tTrack_Sigma1Pt2{},
#if INCLUDE_MUCH_INFO
      tTrack_IsKinkDaughter{},
      tTrack_NTPCClusters{},
      tTrack_NCrossedRows{},
      tTrack_NFindableClusters{},
      tTrack_NSharedClusters{},
      tTrack_Chi2overNcls{},
#endif
      // tTrack_TPCFitMap{},
      // tTrack_TPCClusterMap{},
      // tTrack_TPCSharedMap{},
      tTrack_McEntry{} {
    DefineInput(0, TChain::Class());
    DefineOutput(1, TList::Class());  // fOutputList
    DefineOutput(2, TTree::Class());  // fOutputTree
}

// Destructor.
// Note: if `TList::SetOwner(true)` was called, the TList destructor should delete all objects added to it.
AliAnalysisTaskEsd2Vector::~AliAnalysisTaskEsd2Vector() {
    delete fOutputList;
    delete fOutputTree;
}

// Initialize analysis task. Needs to be called within an `AddTaskEsd2Vector.C` macro.
void AliAnalysisTaskEsd2Vector::Initialize(bool is_mc, bool is_signal_mc) {

    fIsMC = is_mc;
    fIsSignalMC = is_signal_mc;

    // print settings //
    AliInfo("Initializing...");
    AliInfo("Settings:");
    AliInfo("========");
    AliInfoF(">> IsMC       = %i", fIsMC);
    AliInfoF(">> IsSignalMC = %i", fIsSignalMC);
}

// # Executed at Runtime //

// Create output objects, called once at RUNTIME ~ execution on Grid.
void AliAnalysisTaskEsd2Vector::UserCreateOutputObjects() {

    auto* man = AliAnalysisManager::GetAnalysisManager();
    if (man == nullptr) AliFatal("ERROR: AliAnalysisManager couldn't be found.");

    auto* inputHandler = dynamic_cast<AliESDInputHandler*>(man->GetInputEventHandler());
    if (inputHandler == nullptr) AliFatal("ERROR: AliESDInputHandler couldn't be found.");

    fPIDResponse = inputHandler->GetPIDResponse();

    // Prepare output list and histograms //

    fOutputList = new TList();
    fOutputList->SetOwner(true);

    fHist_Events_Bookkeeping = new TH1F("Events_Bookkeeping", ";;Counts", 10, 0., 10.);
    fOutputList->Add(fHist_Events_Bookkeeping);

    fHist_Centrality = new TH1F("Centrality", ";Centrality;Counts", 11, 0., 110.);
    fOutputList->Add(fHist_Centrality);

    fHist_CentralityINT7 = new TH1F("CentralityINT7", ";CentralityINT7;Counts", 11, 0., 110.);
    fOutputList->Add(fHist_CentralityINT7);

    fHist_Tracks_Bookkeeping = new TH1F("Tracks_Bookkeeping", ";;Counts", 50, 0., 50.);
    fOutputList->Add(fHist_Tracks_Bookkeeping);

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
bool AliAnalysisTaskEsd2Vector::UserNotify() {

    auto* man{AliAnalysisManager::GetAnalysisManager()};
    if (man == nullptr) AliFatal("Analysis Manager not found");
    auto* man_tree{man->GetTree()};
    if (man_tree == nullptr) AliFatal("Analysis Manager Tree not found");
    auto* man_file{man_tree->GetCurrentFile()};
    if (man_file == nullptr) AliFatal("Analysis Manager File not found");

    // get AliEn path and tokenize it //

    fAliEnPath = man_file->GetName();
    if (fAliEnPath == "") AliWarning("fAliEnPath couldn't be found.");
    AliInfoF("fAliEnPath: %s", fAliEnPath.Data());

    TObjArray* tokens{fAliEnPath.Tokenize("/")};
    if (fIsMC) {
        // path of signal MC ends with format `.../LHC23l1a3/A1.73/297595/001/AliESDs.root` //
        // and path of general purpose MC ends with format `.../LHC20e3a/297595/001/AliESDs.root` //
        fDirNumber = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries() - 2)))->GetString().Atoi();
        AliInfoF("Dir Number : %04i", static_cast<int>(fDirNumber));  // = 1
        if (fIsSignalMC) {
            BringSignalLogs();
            LoadSignalLogs();
        }
    } else {
        // path of data ends with format `.../LHC15o/000245232/pass2/15000245232039.914/AliESDs.root` //
        auto aux_dir_nr{(dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries() - 2)))->GetString()};
        aux_dir_nr = TString(aux_dir_nr(2 + 3 + 6, 10));  // = "039.914"
        AliInfoF("Dir Number : %s", aux_dir_nr.Data());
        fDirNumber = TString(aux_dir_nr(0, 3)).Atoi();   // = 39
        fDirNumberB = TString(aux_dir_nr(4, 5)).Atoi();  // = 914
    }

    return true;
}

// Main function, called per each event at RUNTIME ~ execution on Grid.
void AliAnalysisTaskEsd2Vector::UserExec(Option_t* option) {
    // events //
    if (!ProcessEvent()) return;
    // mc particles //
    if (fIsMC) {
        if (fIsSignalMC) ProcessInjectedReactions();
        ProcessMCParticles();
    }
    // tracks //
    ProcessTracks();
    // end of event //
    fOutputTree->Fill();
    if (fIsMC) {
        ClearMCBranches();
        fVec_McEntry.clear();
    }
    ClearTracksBranches();
    // post data //
    PostData(1, fOutputList);
    PostData(2, fOutputTree);
}

// # Events //

bool AliAnalysisTaskEsd2Vector::ProcessEvent() {

    // Load MC Event //
    if (fIsMC) {
        fMC = MCEvent();
        if (fMC == nullptr) AliFatal("AliMCEvent couldn't be found.");
        fPIDResponse->SetCurrentMCEvent(fMC);
    }

    // Load Reconstructed Event, PV and Magnetic Field //
    fESD = dynamic_cast<AliESDEvent*>(InputEvent());
    if (fESD == nullptr) AliFatal("AliESDEvent couldn't be found.");

    // Assign metadata branches (1) //
    fRunNumber = fESD->GetRunNumber();
    // `fDirNumber` should be set in `UserNotify()`
    // `fDirNumberB` should be set in `UserNotify()`
    fEventNumberInFile = fESD->GetEventNumberInFile();
    fPrimaryVertex = fESD->GetPrimaryVertex();

    // Apply selection //
    if (!PassesEventSelection()) return false;

    // Assign more branches (2) //
    auto* MultSelection{dynamic_cast<AliMultSelection*>(fESD->FindListObject("MultSelection"))};
    if (MultSelection == nullptr) AliFatal("AliMultSelection couldn't be found.");
    fCentrality = MultSelection->GetMultiplicityPercentile("V0M");
    fMagneticField = static_cast<float>(fESD->GetMagneticField());

    // Fill QA hist //
    fHist_Centrality->Fill(fCentrality);
    if ((fInputHandler->IsEventSelected() & AliVEvent::kINT7) != 0U) fHist_CentralityINT7->Fill(fCentrality);

    // Assign rest of branches (3) //
    tEvent_PV_Xv = static_cast<float>(fPrimaryVertex->GetX());
    tEvent_PV_Yv = static_cast<float>(fPrimaryVertex->GetY());
    tEvent_PV_Zv = static_cast<float>(fPrimaryVertex->GetZ());

    tEvent_PV_NContributors = fPrimaryVertex->GetNContributors();
    tEvent_PV_Dispersion = static_cast<float>(fPrimaryVertex->GetDispersion());

    double PV_CovMatrix[6];
    fPrimaryVertex->GetCovarianceMatrix(PV_CovMatrix);
    for (int i{0}; i < 6; ++i) tEvent_PV_CovMatrix[i] = static_cast<float>(PV_CovMatrix[i]);

    const auto* PrimaryVertex_SPD = fESD->GetPrimaryVertexSPD();
    tEvent_SPD_PV_Zv = static_cast<float>(PrimaryVertex_SPD->GetZ());
    double PV_SPD_CovMatrix[6];
    PrimaryVertex_SPD->GetCovarianceMatrix(PV_SPD_CovMatrix);
    tEvent_SPD_PV_ZvErr = static_cast<float>(PV_SPD_CovMatrix[5]);

    tEvent_NTracks = static_cast<unsigned int>(fESD->GetNumberOfTracks());
    tEvent_NTPCClusters = fESD->GetNumberOfTPCClusters();
    tEvent_IsMB = ((fInputHandler->IsEventSelected() & AliVEvent::kINT7) != 0U);
    tEvent_IsHighMultV0 = ((fInputHandler->IsEventSelected() & AliVEvent::kHighMultV0) != 0U);
    tEvent_IsHighMultSPD = ((fInputHandler->IsEventSelected() & AliVEvent::AliVEvent::kHighMultSPD) != 0U);
    tEvent_IsCentral = ((fInputHandler->IsEventSelected() & AliVEvent::kCentral) != 0U);
    tEvent_IsSemiCentral = ((fInputHandler->IsEventSelected() & AliVEvent::kSemiCentral) != 0U);

    // Assign MC branches (4) //
    if (fIsMC) {
        fMC_PrimaryVertex = fMC->GetPrimaryVertex();
        tEvent_MC_PV_Xv = static_cast<float>(fMC_PrimaryVertex->GetX());
        tEvent_MC_PV_Yv = static_cast<float>(fMC_PrimaryVertex->GetY());
        tEvent_MC_PV_Zv = static_cast<float>(fMC_PrimaryVertex->GetZ());
        tEvent_MC_IsGenPileup = AliAnalysisUtils::IsPileupInGeneratedEvent(fMC, "Hijing");
        tEvent_MC_IsSBCPileup = AliAnalysisUtils::IsSameBunchPileupInGeneratedEvent(fMC, "Hijing");
    }

    return true;
}

// Apply event selection.
bool AliAnalysisTaskEsd2Vector::PassesEventSelection() {

    fHist_Events_Bookkeeping->Fill(0.);
    if (!fEventCuts.AcceptEvent(fESD)) return false;
    fHist_Events_Bookkeeping->Fill(1.);
    // reference: https://twiki.cern.ch/twiki/bin/viewauth/ALICE/AliDPGRunList18r1 //
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
    bool IsMB{(fInputHandler->IsEventSelected() & AliVEvent::kINT7) != 0U};
    bool IsHighMultV0{(fInputHandler->IsEventSelected() & AliVEvent::kHighMultV0) != 0U};
    bool IsHighMultSPD{(fInputHandler->IsEventSelected() & AliVEvent::kHighMultSPD) != 0U};
    bool IsCentral{(fInputHandler->IsEventSelected() & AliVEvent::kCentral) != 0U};
    bool IsSemiCentral{(fInputHandler->IsEventSelected() & AliVEvent::kSemiCentral) != 0U};
    if (!IsMB && !IsHighMultV0 && !IsHighMultSPD && !IsCentral && !IsSemiCentral) return false;
    fHist_Events_Bookkeeping->Fill(6.);
    // rec. PV z-vertex range //
    if (std::abs(fPrimaryVertex->GetZ()) > Cuts::Event::AbsMax_PV_Zv) return false;
    fHist_Events_Bookkeeping->Fill(7.);

    return true;
}

// # Trees //

// Add branches to `fOutputTree`.
void AliAnalysisTaskEsd2Vector::CreateEventsBranches() {
    fOutputTree->Branch("RunNumber", &fRunNumber);
    fOutputTree->Branch("DirNumber", &fDirNumber);
    if (!fIsMC) fOutputTree->Branch("DirNumberB", &fDirNumberB);
    fOutputTree->Branch("EventNumber", &fEventNumberInFile);
    fOutputTree->Branch("Centrality", &fCentrality);
    fOutputTree->Branch("MagneticField", &fMagneticField);
    if (fIsMC) {
        fOutputTree->Branch("MC_PV_Xv", &tEvent_MC_PV_Xv);
        fOutputTree->Branch("MC_PV_Yv", &tEvent_MC_PV_Yv);
        fOutputTree->Branch("MC_PV_Zv", &tEvent_MC_PV_Zv);
        fOutputTree->Branch("MC_IsGenPileup", &tEvent_MC_IsGenPileup);
        fOutputTree->Branch("MC_IsSBCPileup", &tEvent_MC_IsSBCPileup);
    }
    fOutputTree->Branch("PV_NContributors", &tEvent_PV_NContributors);
    fOutputTree->Branch("PV_Dispersion", &tEvent_PV_Dispersion);
    fOutputTree->Branch("PV_Xv", &tEvent_PV_Xv);
    fOutputTree->Branch("PV_Yv", &tEvent_PV_Yv);
    fOutputTree->Branch("PV_Zv", &tEvent_PV_Zv);
    fOutputTree->Branch("PV_CovMatrix", &tEvent_PV_CovMatrix, "PV_CovMatrix[6]/F");
    fOutputTree->Branch("SPD_PV_Zv", &tEvent_SPD_PV_Zv);
    fOutputTree->Branch("SPD_PV_ZvErr", &tEvent_SPD_PV_ZvErr);
    fOutputTree->Branch("NTracks", &tEvent_NTracks);
    fOutputTree->Branch("NTPCClusters", &tEvent_NTPCClusters);
    fOutputTree->Branch("IsMB", &tEvent_IsMB);
    fOutputTree->Branch("IsHighMultV0", &tEvent_IsHighMultV0);
    fOutputTree->Branch("IsHighMultSPD", &tEvent_IsHighMultSPD);
    fOutputTree->Branch("IsCentral", &tEvent_IsCentral);
    fOutputTree->Branch("IsSemiCentral", &tEvent_IsSemiCentral);
}

// Add branches to `fTree_Injected`.
void AliAnalysisTaskEsd2Vector::CreateInjectedBranches() {
    fOutputTree->Branch("ReactionID", &tInjected_ReactionID);
    fOutputTree->Branch("Sexaquark_Px", &tInjected_Px);
    fOutputTree->Branch("Sexaquark_Py", &tInjected_Py);
    fOutputTree->Branch("Sexaquark_Pz", &tInjected_Pz);
    fOutputTree->Branch("Nucleon_Px", &tInjected_Nucleon_Px);
    fOutputTree->Branch("Nucleon_Py", &tInjected_Nucleon_Py);
    fOutputTree->Branch("Nucleon_Pz", &tInjected_Nucleon_Pz);
}

// Add branches to `fTree_MC`.
void AliAnalysisTaskEsd2Vector::CreateMCBranches() {
    fOutputTree->Branch("MC_PdgCode", &tMC_PdgCode);
    fOutputTree->Branch("MC_Mother_McEntry", &tMC_Mother_McEntry);
    fOutputTree->Branch("MC_X", &tMC_X);
    fOutputTree->Branch("MC_Y", &tMC_Y);
    fOutputTree->Branch("MC_Z", &tMC_Z);
    fOutputTree->Branch("MC_Px", &tMC_Px);
    fOutputTree->Branch("MC_Py", &tMC_Py);
    fOutputTree->Branch("MC_Pz", &tMC_Pz);
    fOutputTree->Branch("MC_E", &tMC_E);
    fOutputTree->Branch("MC_Status", &tMC_Status);
    fOutputTree->Branch("MC_IsOOBPileup", &tMC_IsOOBPileup);
    fOutputTree->Branch("MC_Generator", &tMC_Generator);
    fOutputTree->Branch("MC_IsPrimary", &tMC_IsPrimary);
    fOutputTree->Branch("MC_IsSecFromMat", &tMC_IsSecFromMat);
    fOutputTree->Branch("MC_IsSecFromWeak", &tMC_IsSecFromWeak);
}

// Add branches to `fTree_Tracks`.
void AliAnalysisTaskEsd2Vector::CreateTracksBranches() {
#if WRITE_ESD_INDICES
    fOutputTree->Branch("Track_EsdIdx", &tTrack_EsdIdx);
#endif
    fOutputTree->Branch("Track_X", &tTrack_X);
    fOutputTree->Branch("Track_Y", &tTrack_Y);
    fOutputTree->Branch("Track_Z", &tTrack_Z);
    fOutputTree->Branch("Track_Px", &tTrack_Px);
    fOutputTree->Branch("Track_Py", &tTrack_Py);
    fOutputTree->Branch("Track_Pz", &tTrack_Pz);
    fOutputTree->Branch("Track_Charge", &tTrack_Charge);
    fOutputTree->Branch("Track_NSigmaPion", &tTrack_NSigmaPion);
    fOutputTree->Branch("Track_NSigmaKaon", &tTrack_NSigmaKaon);
    fOutputTree->Branch("Track_NSigmaProton", &tTrack_NSigmaProton);
    fOutputTree->Branch("Track_DCAxy", &tTrack_DCAxy);
    fOutputTree->Branch("Track_DCAz", &tTrack_DCAz);
    fOutputTree->Branch("Track_SignalTPC", &tTrack_SignalTPC);
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
    if (fIsMC) fOutputTree->Branch("Track_McEntry", &tTrack_McEntry);
#if INCLUDE_MUCH_INFO
    fOutputTree->Branch("Track_IsKinkDaughter", &tTrack_IsKinkDaughter);
    fOutputTree->Branch("Track_NTPCClusters", &tTrack_NTPCClusters);
    fOutputTree->Branch("Track_NCrossedRows", &tTrack_NCrossedRows);
    fOutputTree->Branch("Track_NFindableClusters", &tTrack_NFindableClusters);
    fOutputTree->Branch("Track_NSharedClusters", &tTrack_NSharedClusters);
    fOutputTree->Branch("Track_Chi2overNcls", &tTrack_Chi2overNcls);
#endif
    // fOutputTree->Branch("TPCFitMap", &tTrack_TPCFitMap);
    // fOutputTree->Branch("TPCClusterMap", &tTrack_TPCClusterMap);
    // fOutputTree->Branch("TPCSharedMap", &tTrack_TPCSharedMap);
}

// # MC Generated //

// Loop over MC particles in a single event.
void AliAnalysisTaskEsd2Vector::ProcessMCParticles() {
    // prepare vectors //
    const int n_mc{fMC->GetNumberOfTracks()};
    fVec_McEntry.resize(n_mc, -1);
    tMC_PdgCode.reserve(n_mc);
    tMC_Mother_McEntry.reserve(n_mc);
    tMC_X.reserve(n_mc);
    tMC_Y.reserve(n_mc);
    tMC_Z.reserve(n_mc);
    tMC_Px.reserve(n_mc);
    tMC_Py.reserve(n_mc);
    tMC_Pz.reserve(n_mc);
    tMC_E.reserve(n_mc);
    tMC_Status.reserve(n_mc);
    tMC_IsOOBPileup.reserve(n_mc);
    tMC_Generator.reserve(n_mc);
    tMC_IsPrimary.reserve(n_mc);
    tMC_IsSecFromMat.reserve(n_mc);
    tMC_IsSecFromWeak.reserve(n_mc);
    // read mc particles //
    int mc_entry{0};
    for (int mc_idx{0}; mc_idx < n_mc; ++mc_idx) {
        auto* mcPart = dynamic_cast<AliMCParticle*>(fMC->GetTrack(mc_idx));
        if (mcPart == nullptr) continue;
        // remove trash //
        if (mcPart->P() < Cuts::MC::Min_Momentum) continue;
        // add to vectors //
        fVec_McEntry[mc_idx] = mc_entry;
        tMC_PdgCode.emplace_back(mcPart->PdgCode());
        tMC_Mother_McEntry.emplace_back(mcPart->GetMother() >= 0 ? fVec_McEntry[mcPart->GetMother()] : Const::DummyInt);
        tMC_X.emplace_back(static_cast<float>(mcPart->Xv()));
        tMC_Y.emplace_back(static_cast<float>(mcPart->Yv()));
        tMC_Z.emplace_back(static_cast<float>(mcPart->Zv()));
        tMC_Px.emplace_back(static_cast<float>(mcPart->Px()));
        tMC_Py.emplace_back(static_cast<float>(mcPart->Py()));
        tMC_Pz.emplace_back(static_cast<float>(mcPart->Pz()));
        tMC_E.emplace_back(static_cast<float>(mcPart->E()));
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
void AliAnalysisTaskEsd2Vector::ClearMCBranches() {
    tMC_PdgCode.clear();
    tMC_Mother_McEntry.clear();
    tMC_X.clear();
    tMC_Y.clear();
    tMC_Z.clear();
    tMC_Px.clear();
    tMC_Py.clear();
    tMC_Pz.clear();
    tMC_E.clear();
    tMC_Status.clear();
    tMC_IsOOBPileup.clear();
    tMC_Generator.clear();
    tMC_IsPrimary.clear();
    tMC_IsSecFromMat.clear();
    tMC_IsSecFromWeak.clear();
}

// # Reconstructed //

// Loop over the reconstructed tracks in a single event.
void AliAnalysisTaskEsd2Vector::ProcessTracks() {
    for (int esd_idx{0}; esd_idx < fESD->GetNumberOfTracks(); ++esd_idx) {
        auto* track = fESD->GetTrack(esd_idx);
        const auto* inner_param{track->GetInnerParam()};
        // track selection //
        // if (inner_param == nullptr) continue;
        // if (!PassesTrackSelection(track, inner_param)) continue;
        if (!PassesTrackSelection_V2(track)) continue;
// assign branches //
#if WRITE_ESD_INDICES
        tTrack_EsdIdx.push_back((Long_t)esd_idx);
#endif
        double position[3];
        inner_param->GetXYZ(position);
        tTrack_X.push_back(static_cast<float>(position[0]));
        tTrack_Y.push_back(static_cast<float>(position[1]));
        tTrack_Z.push_back(static_cast<float>(position[2]));
        double momentum[3];
        inner_param->GetPxPyPz(momentum);
        tTrack_Px.push_back(static_cast<float>(momentum[0]));
        tTrack_Py.push_back(static_cast<float>(momentum[1]));
        tTrack_Pz.push_back(static_cast<float>(momentum[2]));
        tTrack_Charge.push_back(inner_param->Charge());
        tTrack_NSigmaPion.push_back(fPIDResponse->NumberOfSigmasTPC(track, AliPID::kPion));
        tTrack_NSigmaKaon.push_back(fPIDResponse->NumberOfSigmasTPC(track, AliPID::kKaon));
        tTrack_NSigmaProton.push_back(fPIDResponse->NumberOfSigmasTPC(track, AliPID::kProton));
        float AuxDCAxy, AuxDCAz;
        track->GetImpactParameters(AuxDCAxy, AuxDCAz);
        tTrack_DCAxy.push_back(AuxDCAxy);
        tTrack_DCAz.push_back(AuxDCAz);
        tTrack_SignalTPC.push_back(static_cast<float>(track->GetTPCsignal()));
        tTrack_Alpha.push_back(static_cast<float>(inner_param->GetAlpha()));
        tTrack_Snp.push_back(static_cast<float>(inner_param->GetSnp()));
        tTrack_Tgl.push_back(static_cast<float>(inner_param->GetTgl()));
        tTrack_Signed1Pt.push_back(static_cast<float>(inner_param->GetSigned1Pt()));
        const auto* covariance_matrix = inner_param->GetCovariance();
        tTrack_SigmaY2.push_back(static_cast<float>(covariance_matrix[0]));
        tTrack_SigmaZY.push_back(static_cast<float>(covariance_matrix[1]));
        tTrack_SigmaZ2.push_back(static_cast<float>(covariance_matrix[2]));
        tTrack_SigmaSnpY.push_back(static_cast<float>(covariance_matrix[3]));
        tTrack_SigmaSnpZ.push_back(static_cast<float>(covariance_matrix[4]));
        tTrack_SigmaSnp2.push_back(static_cast<float>(covariance_matrix[5]));
        tTrack_SigmaTglY.push_back(static_cast<float>(covariance_matrix[6]));
        tTrack_SigmaTglZ.push_back(static_cast<float>(covariance_matrix[7]));
        tTrack_SigmaTglSnp.push_back(static_cast<float>(covariance_matrix[8]));
        tTrack_SigmaTgl2.push_back(static_cast<float>(covariance_matrix[9]));
        tTrack_Sigma1PtY.push_back(static_cast<float>(covariance_matrix[10]));
        tTrack_Sigma1PtZ.push_back(static_cast<float>(covariance_matrix[11]));
        tTrack_Sigma1PtSnp.push_back(static_cast<float>(covariance_matrix[12]));
        tTrack_Sigma1PtTgl.push_back(static_cast<float>(covariance_matrix[13]));
        tTrack_Sigma1Pt2.push_back(static_cast<float>(covariance_matrix[14]));
        if (fIsMC) tTrack_McEntry.push_back(fVec_McEntry[std::abs(track->GetLabel())]);
#if INCLUDE_MUCH_INFO
        tTrack_IsKinkDaughter.push_back(track->GetKinkIndex(0) > 0);
        tTrack_NTPCClusters.push_back(track->GetTPCNcls());  // Note: capital N
        tTrack_NCrossedRows.push_back(track->GetTPCCrossedRows());
        tTrack_NFindableClusters.push_back(track->GetTPCNclsF());
        tTrack_NSharedClusters.push_back(track->GetTPCnclsS());
        tTrack_Chi2overNcls.push_back(static_cast<float>(track->GetTPCNcls() > 0 ? track->GetTPCchi2() / (Double_t)track->GetTPCNcls() : 999.));
#endif
        // tTrack_TPCFitMap = track->GetTPCFitMap();
        // tTrack_TPCClusterMap = track->GetTPCClusterMap();
        // tTrack_TPCSharedMap = track->GetTPCSharedMap();
    }  // end of loop over tracks
}

// Check if track passes selection and fill bookkeeping histograms.
bool AliAnalysisTaskEsd2Vector::PassesTrackSelection(const AliESDtrack* track, const AliExternalTrackParam* inner_param) {

    bool its_status{((track->GetStatus() & AliESDtrack::kITSin) == 0U) && ((track->GetStatus() & AliESDtrack::kITSout) == 0U) &&
                    ((track->GetStatus() & AliESDtrack::kITSrefit) == 0U)};
    if (Cuts::Track::TurnedOn_StatusCuts && !its_status) return false;
    fHist_Tracks_Bookkeeping->Fill(0);

    if (std::abs(inner_param->Eta()) > Cuts::Track::AbsMax_Eta) return false;
    fHist_Tracks_Bookkeeping->Fill(1);

    float n_sigma_proton{fPIDResponse->NumberOfSigmasTPC(track, AliPID::kProton)};
    float n_sigma_kaon{fPIDResponse->NumberOfSigmasTPC(track, AliPID::kKaon)};
    float n_sigma_pion{fPIDResponse->NumberOfSigmasTPC(track, AliPID::kPion)};
    if (std::abs(n_sigma_proton) > Cuts::Track::AbsMax_NSigma_Proton && std::abs(n_sigma_kaon) > Cuts::Track::AbsMax_NSigma_Kaon &&
        std::abs(n_sigma_pion) > Cuts::Track::AbsMax_NSigma_Pion) {
        return false;
    }
    fHist_Tracks_Bookkeeping->Fill(2);

    float DCAxy_wrtPV, DCAz_wrtPV;
    track->GetImpactParameters(DCAxy_wrtPV, DCAz_wrtPV);
    if (std::abs(DCAxy_wrtPV) < Cuts::Track::Min_DCAxy_wrtPV) return false;
    fHist_Tracks_Bookkeeping->Fill(3);

    if (inner_param->Pt() < Cuts::Track::Min_Pt || inner_param->Pt() > Cuts::Track::Max_Pt) return false;
    fHist_Tracks_Bookkeeping->Fill(4);

    double NTPCClusters{static_cast<double>(track->GetTPCNcls())};
    if (NTPCClusters < Cuts::Track::Min_NClusterTPC) return false;
    fHist_Tracks_Bookkeeping->Fill(5);

    double Chi2PerNTPCClusters{NTPCClusters > 0. ? track->GetTPCchi2() / NTPCClusters : Const::DummyFloat};
    if (Chi2PerNTPCClusters > Cuts::Track::Max_Chi2PerClusterTPC) return false;
    fHist_Tracks_Bookkeeping->Fill(6);

    bool tpc_status{((track->GetStatus() & AliESDtrack::kTPCout) != 0U) && ((track->GetStatus() & AliESDtrack::kTPCrefit) != 0U)};
    if (Cuts::Track::TurnedOn_StatusCuts && !tpc_status) return false;
    fHist_Tracks_Bookkeeping->Fill(7);

    if (Cuts::Track::RejectKinks && track->GetKinkIndex(0) > 0) return false;
    fHist_Tracks_Bookkeeping->Fill(8);

    return true;
}

// Figure out if the track survives all 45 cuts defined by
// different quality parameters, kinematics and geometry.
// Based on `AliESDtrackCuts::AcceptTrack`
bool AliAnalysisTaskEsd2Vector::PassesTrackSelection_V2(const AliESDtrack* track) {

    // get status //
    unsigned long long status{track->GetStatus()};

    // 0 : require TPC refit
    fHist_Tracks_Bookkeeping->Fill(0);
    if (Cuts::Track::RequireTPCRefit && (status & AliESDtrack::kTPCrefit) == 0) return false;

    // 1 : require TPC standalone
    fHist_Tracks_Bookkeeping->Fill(1);
    if (Cuts::Track::RequireTPCStandalone && (status & AliESDtrack::kTPCin) == 0) return false;

    // 2 : require ITS refit
    // -- removed

    // 3 : n TPC clusters
    fHist_Tracks_Bookkeeping->Fill(3);
    unsigned short nClustersTPC{track->GetTPCNcls()};
    if (Cuts::Track::RequireTPCStandalone) {
        nClustersTPC = track->GetTPCNclsIter1();
    }
    if (nClustersTPC < Cuts::Track::Min_NClusterTPC) return false;

    // 4 : n ITS clusters
    // -- removed

    // 5 : chi2 per TPC cluster
    fHist_Tracks_Bookkeeping->Fill(5);
    double chi2PerClusterTPC{-1.};
    if (nClustersTPC > 0) {
        if (Cuts::Track::RequireTPCStandalone) {
            chi2PerClusterTPC = static_cast<double>(track->GetTPCchi2Iter1()) / static_cast<double>(nClustersTPC);
        } else {
            chi2PerClusterTPC = static_cast<double>(track->GetTPCchi2()) / static_cast<double>(nClustersTPC);
        }
    }
    if (chi2PerClusterTPC > Cuts::Track::Max_Chi2PerClusterTPC) return false;

    // 6 : chi2 per ITS cluster
    // -- removed

    // get extermal param. covariance matrix //
    double extCov[15]{};
    track->GetExternalCovariance(extCov);

    // 7 : max res. y^2
    fHist_Tracks_Bookkeeping->Fill(7);
    if (extCov[0] > Cuts::Track::Max_C11) return false;

    // 8 : max res. z^2
    fHist_Tracks_Bookkeeping->Fill(8);
    if (extCov[2] > Cuts::Track::Max_C22) return false;

    // 9 : max res. sin(phi)^2
    fHist_Tracks_Bookkeeping->Fill(9);
    if (extCov[5] > Cuts::Track::Max_C33) return false;

    // 10 : max res. tan(theta_dip)^2
    fHist_Tracks_Bookkeeping->Fill(10);
    if (extCov[9] > Cuts::Track::Max_C44) return false;

    // 11 : max res. 1/pt^2
    fHist_Tracks_Bookkeeping->Fill(11);
    if (extCov[14] > Cuts::Track::Max_C55) return false;

    // 12 : (below)
    // 13 : (below)

    // 14 : reject kinks
    fHist_Tracks_Bookkeeping->Fill(14);
    if (Cuts::Track::RejectKinks && track->GetKinkIndex(0) > 0) return false;

    // get kinematics //
    double p[3]{};
    track->GetPxPyPz(p);
    double momentum{std::sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2])};
    double pt{std::sqrt(p[0] * p[0] + p[1] * p[1])};

    // 15 : total momentum
    fHist_Tracks_Bookkeeping->Fill(15);
    if ((momentum < Cuts::Track::Min_P) || (momentum > Cuts::Track::Max_P)) return false;

    // 16 : transverse momentum
    fHist_Tracks_Bookkeeping->Fill(16);
    if ((pt < Cuts::Track::Min_Pt) || (pt > Cuts::Track::Max_Pt)) return false;

    // 17 : x-component of momentum
    fHist_Tracks_Bookkeeping->Fill(17);
    if ((p[0] < Cuts::Track::Min_Px) || (p[0] > Cuts::Track::Max_Px)) return false;

    // 18 : y-component of momentum
    fHist_Tracks_Bookkeeping->Fill(18);
    if ((p[1] < Cuts::Track::Min_Py) || (p[1] > Cuts::Track::Max_Py)) return false;

    // 19 : z-component of momentum
    fHist_Tracks_Bookkeeping->Fill(19);
    if ((p[2] < Cuts::Track::Min_Pz) || (p[2] > Cuts::Track::Max_Pz)) return false;

    // 20 : pseudorapidity
    // -- modified from min. and max. to abs. max.
    fHist_Tracks_Bookkeeping->Fill(20);
    double eta{-100.};
    if (momentum != std::abs(p[2])) eta = std::log((momentum + p[2]) / (momentum - p[2])) / 2.;
    if (std::abs(eta) > Cuts::Track::AbsMax_Eta) return false;

    // 21 : rapidity
    // -- removed as it requires mass hypothesis

    // get DCA //
    float b[2]{};
    float bCov[3]{};
    track->GetImpactParameters(b, bCov);
    float DCAxy{b[0]};
    float DCAz{b[1]};

    // 22 : DCA cut as ellipse (a)
    fHist_Tracks_Bookkeeping->Fill(22);
    if (Cuts::Track::DoDCACut_AsEllipse && DCAxy * DCAxy / Cuts::Track::Max_DCAxy_wrtPV / Cuts::Track::Max_DCAxy_wrtPV +
                                                   DCAz * DCAz / Cuts::Track::Max_DCAz_wrtPV / Cuts::Track::Max_DCAz_wrtPV >
                                               1.) {
        return false;
    }

    // 23 : DCA cut as rectangle (a)
    fHist_Tracks_Bookkeeping->Fill(23);
    if (Cuts::Track::DoDCACut_AsRectangle && std::abs(DCAxy) > Cuts::Track::Max_DCAxy_wrtPV) return false;

    // 24 : DCA cut as rectangle (b)
    fHist_Tracks_Bookkeeping->Fill(24);
    if (Cuts::Track::DoDCACut_AsRectangle && std::abs(DCAz) > Cuts::Track::Max_DCAz_wrtPV) return false;

    // 25 : DCA cut as ellipse (b)
    fHist_Tracks_Bookkeeping->Fill(25);
    if (Cuts::Track::DoDCACut_AsEllipse && Cuts::Track::Min_DCAxy_wrtPV > 0. && Cuts::Track::Min_DCAz_wrtPV > 0. &&
        DCAxy * DCAxy / Cuts::Track::Min_DCAxy_wrtPV / Cuts::Track::Min_DCAxy_wrtPV +
                DCAz * DCAz / Cuts::Track::Min_DCAz_wrtPV / Cuts::Track::Min_DCAz_wrtPV <
            1.) {
        return false;
    }

    // 26 : DCA cut as rectangle (c)
    fHist_Tracks_Bookkeeping->Fill(26);
    if (Cuts::Track::DoDCACut_AsRectangle && std::abs(DCAxy) < Cuts::Track::Min_DCAxy_wrtPV) return false;

    // 27 : DCA cut as rectangle (d)
    fHist_Tracks_Bookkeeping->Fill(27);
    if (Cuts::Track::DoDCACut_AsRectangle && std::abs(DCAz) < Cuts::Track::Min_DCAz_wrtPV) return false;

    // 28 : SPD cluster requirement
    // -- removed and replaced by
    // 28 : DCA cut as circle (a)
    fHist_Tracks_Bookkeeping->Fill(28);
    if (Cuts::Track::DoDCACut_AsCircle && std::sqrt(DCAxy * DCAxy + DCAz * DCAz) < Cuts::Track::Min_DCA_wrtPV) return false;

    // 29 : SDD cluster requirement
    // -- removed and replaced by
    // 29 : DCA cut as circle (b)
    fHist_Tracks_Bookkeeping->Fill(29);
    if (Cuts::Track::DoDCACut_AsCircle && std::sqrt(DCAxy * DCAxy + DCAz * DCAz) > Cuts::Track::Max_DCA_wrtPV) return false;

    // 30 : SSD cluster requirement
    // -- removed

    // 31 : ITS standalone
    // -- removed

    // 32 : pt-dependent pt resolution cut
    // -- temporarily removed

    // 33 : reject shared TPC clusters
    fHist_Tracks_Bookkeeping->Fill(33);
    int nClustersTPCShared{track->GetTPCnclsS()};
    if (Cuts::Track::RejectSharedTPCClusters && nClustersTPCShared != 0) return false;

    // 34 : fraction shared TPC clusters
    fHist_Tracks_Bookkeeping->Fill(34);
    double fracClustersTPCShared{-1.};
    if (nClustersTPC > 0) {
        fracClustersTPCShared = static_cast<double>(nClustersTPCShared) / static_cast<double>(nClustersTPC);
    }
    if (fracClustersTPCShared > Cuts::Track::Max_FractionSharedTPCClusters) return false;

    // 35 : require ITS PID
    // -- removed

    // 36 : n crossed rows TPC
    fHist_Tracks_Bookkeeping->Fill(36);
    float nCrossedRowsTPC{track->GetTPCCrossedRows()};
    if (nCrossedRowsTPC < Cuts::Track::Min_NCrossedRowsTPC) return false;

    // 37 : ratio crossed rows over findable clusters TPC
    fHist_Tracks_Bookkeeping->Fill(37);
    double nFindableClustersTPC{static_cast<double>(track->GetTPCNclsF())};
    double ratioCrossedRowsOverFindableClustersTPC{1.};
    if (nFindableClustersTPC > 0) {
        ratioCrossedRowsOverFindableClustersTPC = static_cast<double>(nCrossedRowsTPC) / static_cast<double>(nFindableClustersTPC);
    }
    if (ratioCrossedRowsOverFindableClustersTPC < Cuts::Track::Min_RatioCrossedRowsOverFindableClustersTPC) return false;

    // 38 : max missing ITS points
    // -- removed

    // 39 : (below)

    // 40 : require TOF out
    // -- removed

    // 41 : TOF signal dz
    // -- removed

    // CPU intensive cuts //

    if (Cuts::Track::RequireNSigmaToVertex) {
        // 12 : n sigma to vertex
        fHist_Tracks_Bookkeeping->Fill(12);
        double nSigmaToVertex{Cuts::GetSigmaToVertex(track)};
        if (nSigmaToVertex > Cuts::Track::Max_NSigmaToVertex) return false;

        // 13 : if n sigma to vertex couldn't be calculated
        fHist_Tracks_Bookkeeping->Fill(13);
        if (nSigmaToVertex < 0.) return false;
    }

    // 39 : max chi2 TPC constrained vs global track
    fHist_Tracks_Bookkeeping->Fill(39);
    if (Cuts::Track::Max_Chi2TPCConstrainedVsGlobal < 1E9) {
        // get vertex
        const AliESDVertex* vertex{nullptr};
        if (Cuts::Track::VertexType_Chi2TPCConstrainedVsGlobal & Cuts::EVertexType::VertexTracks) {
            vertex = fESD->GetPrimaryVertexTracks();
        }
        if ((!vertex || !vertex->GetStatus()) && Cuts::Track::VertexType_Chi2TPCConstrainedVsGlobal & Cuts::EVertexType::VertexSPD) {
            vertex = fESD->GetPrimaryVertexSPD();
        }
        if ((!vertex || !vertex->GetStatus()) && Cuts::Track::VertexType_Chi2TPCConstrainedVsGlobal & Cuts::EVertexType::VertexTPC) {
            vertex = fESD->GetPrimaryVertexTPC();
        }

        double chi2TPCConstrainedVsGlobal{-2.};
        if (vertex->GetStatus()) chi2TPCConstrainedVsGlobal = track->GetChi2TPCConstrainedVsGlobal(vertex);

        if (chi2TPCConstrainedVsGlobal < 0. || chi2TPCConstrainedVsGlobal > Cuts::Track::Max_Chi2TPCConstrainedVsGlobal) return false;
    }

    if (Cuts::Track::Min_LengthActiveVolumeTPC > 1. || Cuts::Track::GeoNcrNclLength > 0.) {
        double lengthInActiveZoneTPC{-1.};

        // 42 : min length in active volume
        fHist_Tracks_Bookkeeping->Fill(42);
        const double kMaxZ = 220;
        if (track->GetInnerParam()) {
            lengthInActiveZoneTPC = track->GetLengthInActiveZone(1, Cuts::Track::DeadZoneWidth, kMaxZ, fESD->GetMagneticField());
        }
        if (lengthInActiveZoneTPC < Cuts::Track::Min_LengthActiveVolumeTPC) return false;

        // 43 : n-geometrical+n-crossed-row and n-clusters cut
        if (Cuts::Track::GeoNcrNclLength > 0) {
            fHist_Tracks_Bookkeeping->Fill(43);
            double cutGeoNcrNclLength = Cuts::Track::GeoNcrNclLength - std::pow(std::abs(track->GetSigned1Pt()), Cuts::Track::GeoNcrNclGeom1Pt);
            if (lengthInActiveZoneTPC < cutGeoNcrNclLength) return false;
            if (nCrossedRowsTPC < Cuts::Track::GeoNcrNclFractionNcr * cutGeoNcrNclLength) return false;
            if (track->GetTPCncls() < Cuts::Track::GeoNcrNclFractionNcl * cutGeoNcrNclLength) return false;
        }
    }

    // 44 : is track in distorted TPC region
    if (Cuts::Track::DistortedRegionTPC) {
        fHist_Tracks_Bookkeeping->Fill(44);
        if (Cuts::IsTrackInDistortedTpcRegion(track)) return false;
    }

    return true;
}

// Clear the branches of the reconstructed tracks.
void AliAnalysisTaskEsd2Vector::ClearTracksBranches() {
#if WRITE_ESD_INDICES
    tTrack_EsdIdx.clear();
#endif
    tTrack_X.clear();
    tTrack_Y.clear();
    tTrack_Z.clear();
    tTrack_Px.clear();
    tTrack_Py.clear();
    tTrack_Pz.clear();
    tTrack_Charge.clear();
    tTrack_NSigmaPion.clear();
    tTrack_NSigmaKaon.clear();
    tTrack_NSigmaProton.clear();
    tTrack_DCAxy.clear();
    tTrack_DCAz.clear();
    tTrack_SignalTPC.clear();
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
    if (fIsMC) tTrack_McEntry.clear();
#if INCLUDE_MUCH_INFO
    tTrack_IsKinkDaughter.clear();
    tTrack_NTPCClusters.clear();
    tTrack_NCrossedRows.clear();
    tTrack_NFindableClusters.clear();
    tTrack_NSharedClusters.clear();
    tTrack_Chi2overNcls.clear();
#endif
    // tTrack_TPCFitMap.clear();
    // tTrack_TPCClusterMap.clear();
    // tTrack_TPCSharedMap.clear();
}

// # Injected Reactions //

// Assign the in-memory values to the tree branches.
void AliAnalysisTaskEsd2Vector::ProcessInjectedReactions() {
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
void AliAnalysisTaskEsd2Vector::BringSignalLogs() {

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
    auto AliEn_RunNumber = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries() - 3)))->GetString().Atoi();
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
bool AliAnalysisTaskEsd2Vector::LoadSignalLogs() {

    TString new_path = Form("%s/%s", gSystem->pwd(), fSignalLog_NewBasename.Data());
    AliInfoF("Opening file %s ...", new_path.Data());

    std::ifstream SimLogFile(new_path);
    if (!SimLogFile.is_open()) {
        AliWarningF("Unable to open file %s", new_path.Data());
        return false;
    }

    int event_n{-1};
    int react_id{0};

    std::string line;
    while (std::getline(SimLogFile, line)) {
        // a new event has appeared //
        if (line.rfind(Const::SimLog_EventHeader) == 0) {
            ++event_n;
            // std::cout << "Reading Event " << event_n << '\n'; // DEBUG
            react_id = 0;
            continue;
        }
        if (event_n >= 0) {
            if (line.rfind(Const::SimLog_ReactionMarker, 0) == 0) {
                std::string data_part{line.substr(Const::SimLog_ReactionMarker.length() + 1)};
                // std::cout << data_part << '\n'; // DEBUG
                std::stringstream ss{data_part};
                std::string temp_parse_buffer;
                // clang-format off
                std::getline(ss, temp_parse_buffer, ','); fEvVec_ReactionID[event_n][react_id] = std::stoi(temp_parse_buffer);
                std::getline(ss, temp_parse_buffer, ','); fEvVec_Sexaquark_Px[event_n][react_id] = std::stof(temp_parse_buffer);
                std::getline(ss, temp_parse_buffer, ','); fEvVec_Sexaquark_Py[event_n][react_id] = std::stof(temp_parse_buffer);
                std::getline(ss, temp_parse_buffer, ','); fEvVec_Sexaquark_Pz[event_n][react_id] = std::stof(temp_parse_buffer);
                std::getline(ss, temp_parse_buffer, ','); fEvVec_Nucleon_Px[event_n][react_id] = std::stof(temp_parse_buffer);
                std::getline(ss, temp_parse_buffer, ','); fEvVec_Nucleon_Py[event_n][react_id] = std::stof(temp_parse_buffer);
                std::getline(ss, temp_parse_buffer); fEvVec_Nucleon_Pz[event_n][react_id] = std::stof(temp_parse_buffer);
                // clang-format on
                ++react_id;
            }
        }
    }  // finish reading lines
    /*
    // DEBUG
    for (int ev_print{0}; ev_print < Const::NEventsPerSignalMCLog; ++ev_print) {
        for (int r_print{0}; r_print < Const::NReactionsPerEvent; ++r_print) {
            std::cout << "Event " << ev_print << ", Reaction " << r_print << ":" << '\n';
            std::cout << "  ReactionID: " << fEvVec_ReactionID[ev_print][r_print] << '\n';
            std::cout << "  Px: " << fEvVec_Sexaquark_Px[ev_print][r_print] << ", Py: " << fEvVec_Sexaquark_Py[ev_print][r_print]
                        << ", Pz: " << fEvVec_Sexaquark_Pz[ev_print][r_print] << '\n';
            std::cout << "  NPx: " << fEvVec_Nucleon_Px[ev_print][r_print] << ", NPy: " << fEvVec_Nucleon_Py[ev_print][r_print]
                        << ", NPz: " << fEvVec_Nucleon_Pz[ev_print][r_print] << '\n';
        }
    }
    */

    AliInfoF("Closing file %s ...", new_path.Data());
    SimLogFile.close();

    return true;
}
