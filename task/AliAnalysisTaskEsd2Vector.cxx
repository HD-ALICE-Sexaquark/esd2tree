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
      tMC_Generator{},
      tMC_IsPrimary{},
      tMC_IsSecFromMat{},
      tMC_IsSecFromWeak{},
      tMC_IsOOBPileup{},
#if E2V_DEBUG
      tMC_iMC{},
      tMC_Mother_iMC{},
#endif
      /*  */
      tTrack_X{},
      tTrack_Y{},
      tTrack_Z{},
      tTrack_Px{},
      tTrack_Py{},
      tTrack_Pz{},
      tTrack_Charge{},
      tTrack_DCAxy{},
      tTrack_DCAz{},
      tTrack_TPCSignal{},
      tTrack_NSigmaPion{},
      tTrack_NSigmaKaon{},
      tTrack_NSigmaProton{},
      tTrack_SigmaX2{},
      tTrack_SigmaXY{},
      tTrack_SigmaY2{},
      tTrack_SigmaXZ{},
      tTrack_SigmaYZ{},
      tTrack_SigmaZ2{},
      tTrack_SigmaXPx{},
      tTrack_SigmaYPx{},
      tTrack_SigmaZPx{},
      tTrack_SigmaPx2{},
      tTrack_SigmaXPy{},
      tTrack_SigmaYPy{},
      tTrack_SigmaZPy{},
      tTrack_SigmaPxPy{},
      tTrack_SigmaPy2{},
      tTrack_SigmaXPz{},
      tTrack_SigmaYPz{},
      tTrack_SigmaZPz{},
      tTrack_SigmaPxPz{},
      tTrack_SigmaPyPz{},
      tTrack_SigmaPz2{},
#if E2V_DEBUG
      tTrack_iESD{},
      tTrack_iMC{},
#endif
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
      tMC_Generator{},
      tMC_IsPrimary{},
      tMC_IsSecFromMat{},
      tMC_IsSecFromWeak{},
      tMC_IsOOBPileup{},
#if E2V_DEBUG
      tMC_iMC{},
      tMC_Mother_iMC{},
#endif
      /*  */
      tTrack_X{},
      tTrack_Y{},
      tTrack_Z{},
      tTrack_Px{},
      tTrack_Py{},
      tTrack_Pz{},
      tTrack_Charge{},
      tTrack_DCAxy{},
      tTrack_DCAz{},
      tTrack_TPCSignal{},
      tTrack_NSigmaPion{},
      tTrack_NSigmaKaon{},
      tTrack_NSigmaProton{},
      tTrack_SigmaX2{},
      tTrack_SigmaXY{},
      tTrack_SigmaY2{},
      tTrack_SigmaXZ{},
      tTrack_SigmaYZ{},
      tTrack_SigmaZ2{},
      tTrack_SigmaXPx{},
      tTrack_SigmaYPx{},
      tTrack_SigmaZPx{},
      tTrack_SigmaPx2{},
      tTrack_SigmaXPy{},
      tTrack_SigmaYPy{},
      tTrack_SigmaZPy{},
      tTrack_SigmaPxPy{},
      tTrack_SigmaPy2{},
      tTrack_SigmaXPz{},
      tTrack_SigmaYPz{},
      tTrack_SigmaZPz{},
      tTrack_SigmaPxPz{},
      tTrack_SigmaPyPz{},
      tTrack_SigmaPz2{},
#if E2V_DEBUG
      tTrack_iESD{},
      tTrack_iMC{},
#endif
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
        fPIDResponse->SetCurrentMCEvent(fMC);  // enable PID tuned on data
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

    double PV_CovMatrix[6]{};
    fPrimaryVertex->GetCovarianceMatrix(PV_CovMatrix);
    for (int i{0}; i < 6; ++i) tEvent_PV_CovMatrix[i] = static_cast<float>(PV_CovMatrix[i]);

    const auto* PrimaryVertex_SPD = fESD->GetPrimaryVertexSPD();
    tEvent_SPD_PV_Zv = static_cast<float>(PrimaryVertex_SPD->GetZ());
    double PV_SPD_CovMatrix[6]{};
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

// Add branches to the MC tree.
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
    fOutputTree->Branch("MC_Generator", &tMC_Generator);
    fOutputTree->Branch("MC_IsPrimary", &tMC_IsPrimary);
    fOutputTree->Branch("MC_IsSecFromMat", &tMC_IsSecFromMat);
    fOutputTree->Branch("MC_IsSecFromWeak", &tMC_IsSecFromWeak);
    fOutputTree->Branch("MC_IsOOBPileup", &tMC_IsOOBPileup);
#if E2V_DEBUG
    fOutputTree->Branch("MC_iMC", &tMC_iMC);
    fOutputTree->Branch("MC_Mother_iMC", &tMC_Mother_iMC);
#endif
}

// Add branches to the tracks tree.
void AliAnalysisTaskEsd2Vector::CreateTracksBranches() {
    fOutputTree->Branch("Track_X", &tTrack_X);
    fOutputTree->Branch("Track_Y", &tTrack_Y);
    fOutputTree->Branch("Track_Z", &tTrack_Z);
    fOutputTree->Branch("Track_Px", &tTrack_Px);
    fOutputTree->Branch("Track_Py", &tTrack_Py);
    fOutputTree->Branch("Track_Pz", &tTrack_Pz);
    fOutputTree->Branch("Track_Charge", &tTrack_Charge);
    fOutputTree->Branch("Track_DCAxy", &tTrack_DCAxy);
    fOutputTree->Branch("Track_DCAz", &tTrack_DCAz);
    fOutputTree->Branch("Track_TPCSignal", &tTrack_TPCSignal);
    fOutputTree->Branch("Track_NSigmaPion", &tTrack_NSigmaPion);
    fOutputTree->Branch("Track_NSigmaKaon", &tTrack_NSigmaKaon);
    fOutputTree->Branch("Track_NSigmaProton", &tTrack_NSigmaProton);
    fOutputTree->Branch("Track_SigmaX2", &tTrack_SigmaX2);
    fOutputTree->Branch("Track_SigmaXY", &tTrack_SigmaXY);
    fOutputTree->Branch("Track_SigmaY2", &tTrack_SigmaY2);
    fOutputTree->Branch("Track_SigmaXZ", &tTrack_SigmaXZ);
    fOutputTree->Branch("Track_SigmaYZ", &tTrack_SigmaYZ);
    fOutputTree->Branch("Track_SigmaZ2", &tTrack_SigmaZ2);
    fOutputTree->Branch("Track_SigmaXPx", &tTrack_SigmaXPx);
    fOutputTree->Branch("Track_SigmaYPx", &tTrack_SigmaYPx);
    fOutputTree->Branch("Track_SigmaZPx", &tTrack_SigmaZPx);
    fOutputTree->Branch("Track_SigmaPx2", &tTrack_SigmaPx2);
    fOutputTree->Branch("Track_SigmaXPy", &tTrack_SigmaXPy);
    fOutputTree->Branch("Track_SigmaYPy", &tTrack_SigmaYPy);
    fOutputTree->Branch("Track_SigmaZPy", &tTrack_SigmaZPy);
    fOutputTree->Branch("Track_SigmaPxPy", &tTrack_SigmaPxPy);
    fOutputTree->Branch("Track_SigmaPy2", &tTrack_SigmaPy2);
    fOutputTree->Branch("Track_SigmaXPz", &tTrack_SigmaXPz);
    fOutputTree->Branch("Track_SigmaYPz", &tTrack_SigmaYPz);
    fOutputTree->Branch("Track_SigmaZPz", &tTrack_SigmaZPz);
    fOutputTree->Branch("Track_SigmaPxPz", &tTrack_SigmaPxPz);
    fOutputTree->Branch("Track_SigmaPyPz", &tTrack_SigmaPyPz);
    fOutputTree->Branch("Track_SigmaPz2", &tTrack_SigmaPz2);
#if E2V_DEBUG
    fOutputTree->Branch("Track_iESD", &tTrack_iESD);
    if (fIsMC) fOutputTree->Branch("Track_iMC", &tTrack_iMC);
#endif
    if (fIsMC) fOutputTree->Branch("Track_McEntry", &tTrack_McEntry);
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
    tMC_Generator.reserve(n_mc);
    tMC_IsPrimary.reserve(n_mc);
    tMC_IsSecFromMat.reserve(n_mc);
    tMC_IsSecFromWeak.reserve(n_mc);
    tMC_IsOOBPileup.reserve(n_mc);
#if E2V_DEBUG
    tMC_iMC.reserve(n_mc);
    tMC_Mother_iMC.reserve(n_mc);
    AliInfo("mc_idx mc_entry mc_mother_idx mc_mother_entry pdg_code mc_dau_first_idx mc_dau_last_idx");
#endif
    // read mc particles //
    int mc_entry{0};
    for (int mc_idx{0}; mc_idx < n_mc; ++mc_idx) {
        auto* mcPart = dynamic_cast<AliMCParticle*>(fMC->GetTrack(mc_idx));
        if (mcPart == nullptr) continue;
        // remove trash //
        if (mcPart->P() < Cuts::MC::Min_Momentum) continue;
#if E2V_DEBUG
        if (mcPart->GetGeneratorIndex() == 2) {
            AliInfoF("%6i %8i %13i %15i %8i %16i %15i",                                                                    //
                     mc_idx, mc_entry,                                                                                     //
                     mcPart->GetMother(), mcPart->GetMother() >= 0 ? fVec_McEntry[mcPart->GetMother()] : Const::DummyInt,  //
                     mcPart->PdgCode(),                                                                                    //
                     mcPart->GetDaughterFirst(), mcPart->GetDaughterLast());
        }
#endif
        // update translation map //
        fVec_McEntry[mc_idx] = mc_entry;
        // fill branches //
        tMC_PdgCode.emplace_back(mcPart->PdgCode());
        tMC_Mother_McEntry.emplace_back(mcPart->GetMother() >= 0 ? fVec_McEntry[mcPart->GetMother()] : Const::DummyInt);
        tMC_X.emplace_back(static_cast<float>(mcPart->Xv()));
        tMC_Y.emplace_back(static_cast<float>(mcPart->Yv()));
        tMC_Z.emplace_back(static_cast<float>(mcPart->Zv()));
        tMC_Px.emplace_back(static_cast<float>(mcPart->Px()));
        tMC_Py.emplace_back(static_cast<float>(mcPart->Py()));
        tMC_Pz.emplace_back(static_cast<float>(mcPart->Pz()));
        tMC_E.emplace_back(static_cast<float>(mcPart->E()));
        tMC_Status.emplace_back(static_cast<int>(mcPart->MCStatusCode()));
        tMC_Generator.emplace_back(static_cast<char>(mcPart->GetGeneratorIndex()));
        tMC_IsPrimary.emplace_back(static_cast<char>(mcPart->IsPhysicalPrimary()));
        tMC_IsSecFromMat.emplace_back(static_cast<char>(mcPart->IsSecondaryFromMaterial()));
        tMC_IsSecFromWeak.emplace_back(static_cast<char>(mcPart->IsSecondaryFromWeakDecay()));
        tMC_IsOOBPileup.emplace_back(static_cast<char>(AliAnalysisUtils::IsParticleFromOutOfBunchPileupCollision(mc_idx, fMC)));
#if E2V_DEBUG
        tMC_iMC.emplace_back(mc_idx);
        tMC_Mother_iMC.emplace_back(mcPart->GetMother());
#endif
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
    tMC_Generator.clear();
    tMC_IsPrimary.clear();
    tMC_IsSecFromMat.clear();
    tMC_IsSecFromWeak.clear();
    tMC_IsOOBPileup.clear();
#if E2V_DEBUG
    tMC_iMC.clear();
    tMC_Mother_iMC.clear();
#endif
}

// # Reconstructed //

// Loop over the reconstructed tracks in a single event.
void AliAnalysisTaskEsd2Vector::ProcessTracks() {
    for (int iESD{0}; iESD < fESD->GetNumberOfTracks(); ++iESD) {
        // get track //
        auto* track = fESD->GetTrack(iESD);
        // track selection //
        if (!PassesTrackSelection(track)) continue;
        // get info //
        const auto* inner_param{track->GetInnerParam()};
        double position[3]{};
        inner_param->GetXYZ(position);
        double momentum[3]{};
        inner_param->GetPxPyPz(momentum);
        float dca[2]{};
        float dca_cov[3]{};
        track->GetImpactParametersTPC(dca, dca_cov);
        double cov_xyz_pxpypz[21]{};
        track->GetCovarianceXYZPxPyPz(cov_xyz_pxpypz);
        // fill branches //
        tTrack_X.push_back(static_cast<float>(position[0]));
        tTrack_Y.push_back(static_cast<float>(position[1]));
        tTrack_Z.push_back(static_cast<float>(position[2]));
        tTrack_Px.push_back(static_cast<float>(momentum[0]));
        tTrack_Py.push_back(static_cast<float>(momentum[1]));
        tTrack_Pz.push_back(static_cast<float>(momentum[2]));
        tTrack_Charge.push_back(inner_param->Charge());
        // -- dca //
        tTrack_DCAxy.push_back(dca[0]);
        tTrack_DCAz.push_back(dca[1]);
        // -- pid //
        tTrack_TPCSignal.push_back(fIsMC ? track->GetTPCsignalTunedOnData() : track->GetTPCsignal());
        tTrack_NSigmaPion.push_back(fPIDResponse->NumberOfSigmasTPC(track, AliPID::kPion));
        tTrack_NSigmaKaon.push_back(fPIDResponse->NumberOfSigmasTPC(track, AliPID::kKaon));
        tTrack_NSigmaProton.push_back(fPIDResponse->NumberOfSigmasTPC(track, AliPID::kProton));
        // -- cov. matrix //
        tTrack_SigmaX2.push_back(static_cast<float>(cov_xyz_pxpypz[0]));
        tTrack_SigmaXY.push_back(static_cast<float>(cov_xyz_pxpypz[1]));
        tTrack_SigmaY2.push_back(static_cast<float>(cov_xyz_pxpypz[2]));
        tTrack_SigmaXZ.push_back(static_cast<float>(cov_xyz_pxpypz[3]));
        tTrack_SigmaYZ.push_back(static_cast<float>(cov_xyz_pxpypz[4]));
        tTrack_SigmaZ2.push_back(static_cast<float>(cov_xyz_pxpypz[5]));
        tTrack_SigmaXPx.push_back(static_cast<float>(cov_xyz_pxpypz[6]));
        tTrack_SigmaYPx.push_back(static_cast<float>(cov_xyz_pxpypz[7]));
        tTrack_SigmaZPx.push_back(static_cast<float>(cov_xyz_pxpypz[8]));
        tTrack_SigmaPx2.push_back(static_cast<float>(cov_xyz_pxpypz[9]));
        tTrack_SigmaXPy.push_back(static_cast<float>(cov_xyz_pxpypz[10]));
        tTrack_SigmaYPy.push_back(static_cast<float>(cov_xyz_pxpypz[11]));
        tTrack_SigmaZPy.push_back(static_cast<float>(cov_xyz_pxpypz[12]));
        tTrack_SigmaPxPy.push_back(static_cast<float>(cov_xyz_pxpypz[13]));
        tTrack_SigmaPy2.push_back(static_cast<float>(cov_xyz_pxpypz[14]));
        tTrack_SigmaXPz.push_back(static_cast<float>(cov_xyz_pxpypz[15]));
        tTrack_SigmaYPz.push_back(static_cast<float>(cov_xyz_pxpypz[16]));
        tTrack_SigmaZPz.push_back(static_cast<float>(cov_xyz_pxpypz[17]));
        tTrack_SigmaPxPz.push_back(static_cast<float>(cov_xyz_pxpypz[18]));
        tTrack_SigmaPyPz.push_back(static_cast<float>(cov_xyz_pxpypz[19]));
        tTrack_SigmaPz2.push_back(static_cast<float>(cov_xyz_pxpypz[20]));
#if E2V_DEBUG
        tTrack_iESD.push_back(iESD);
        if (fIsMC) tTrack_iMC.push_back(std::abs(track->GetLabel()));
#endif
        if (fIsMC) tTrack_McEntry.push_back(fVec_McEntry[std::abs(track->GetLabel())]);
    }  // end of loop over tracks
}

// Check if track passes selection and fill bookkeeping histograms.
bool AliAnalysisTaskEsd2Vector::PassesTrackSelection(const AliESDtrack* track) {

    fHist_Tracks_Bookkeeping->Fill(0);

    // -- dca

    float dca[2]{}, cov_dca[3]{};
    track->GetImpactParameters(dca, cov_dca);
    if (std::abs(dca[0]) / 15. + std::abs(dca[1]) / 20. <= 1.) return false;
    fHist_Tracks_Bookkeeping->Fill(1);

    // -- status

    unsigned long long status{track->GetStatus()};
    bool tpc_in{(status & AliESDtrack::kTPCin) > 0};
    bool tpc_refit{(status & AliESDtrack::kTPCrefit) > 0};

    if (!tpc_in) return false;
    fHist_Tracks_Bookkeeping->Fill(2);

    if (!tpc_refit) return false;
    fHist_Tracks_Bookkeeping->Fill(3);

    // -- kinematics //

    const auto* inner_param = track->GetInnerParam();

    if (std::abs(inner_param->Pz()) > Cuts::Track::AbsMax_Pz) return false;
    fHist_Tracks_Bookkeeping->Fill(4);

    if (inner_param->P() < Cuts::Track::Min_P || inner_param->P() > Cuts::Track::Max_P) return false;
    fHist_Tracks_Bookkeeping->Fill(5);

    if (std::abs(inner_param->Eta()) > Cuts::Track::AbsMax_Eta) return false;
    fHist_Tracks_Bookkeeping->Fill(6);

    if (inner_param->Pt() < Cuts::Track::Min_Pt || inner_param->Pt() > Cuts::Track::Max_Pt) return false;
    fHist_Tracks_Bookkeeping->Fill(7);

    // -- pid //

    if (track->GetTPCsignal() > 200.) return false;
    fHist_Tracks_Bookkeeping->Fill(8);

    float n_sigma_proton{fPIDResponse->NumberOfSigmasTPC(track, AliPID::kProton)};
    float n_sigma_kaon{fPIDResponse->NumberOfSigmasTPC(track, AliPID::kKaon)};
    float n_sigma_pion{fPIDResponse->NumberOfSigmasTPC(track, AliPID::kPion)};
    if (std::abs(n_sigma_proton) > Cuts::Track::AbsMax_NSigma_PID && std::abs(n_sigma_kaon) > Cuts::Track::AbsMax_NSigma_PID &&
        std::abs(n_sigma_pion) > Cuts::Track::AbsMax_NSigma_PID) {
        return false;
    }
    fHist_Tracks_Bookkeeping->Fill(9);

    // -- its info

    if (track->GetITSNcls() > 0) return false;
    fHist_Tracks_Bookkeeping->Fill(10);

    // -- tpc info

    if (track->GetTPCPoints(0) > 110.) return false;
    fHist_Tracks_Bookkeeping->Fill(11);

    double n_tpc_clusters{static_cast<double>(track->GetTPCNcls())};
    if (n_tpc_clusters < Cuts::Track::Min_NClusterTPC) return false;
    fHist_Tracks_Bookkeeping->Fill(12);

    // -- others

    if (track->GetKinkIndex(0) > 0) return false;
    fHist_Tracks_Bookkeeping->Fill(13);

    // -- qa

    if (std::abs(cov_dca[0]) >= Cuts::Track::AbsMax_Cov) return false;
    fHist_Tracks_Bookkeeping->Fill(14);

    if (std::abs(cov_dca[1]) >= Cuts::Track::AbsMax_Cov) return false;
    fHist_Tracks_Bookkeeping->Fill(15);

    if (std::abs(cov_dca[2]) >= Cuts::Track::AbsMax_Cov) return false;
    fHist_Tracks_Bookkeeping->Fill(16);

    if (std::abs(inner_param->GetSigmaY2()) >= Cuts::Track::AbsMax_Cov) return false;
    fHist_Tracks_Bookkeeping->Fill(17);

    if (std::abs(inner_param->GetSigmaZ2()) >= Cuts::Track::AbsMax_Cov) return false;
    fHist_Tracks_Bookkeeping->Fill(18);

    if (std::abs(inner_param->GetSigmaSnp2()) >= Cuts::Track::AbsMax_Cov) return false;
    fHist_Tracks_Bookkeeping->Fill(19);

    if (std::abs(inner_param->GetSigmaTgl2()) >= Cuts::Track::AbsMax_Cov) return false;
    fHist_Tracks_Bookkeeping->Fill(20);

    if (std::abs(inner_param->GetSigma1Pt2()) >= Cuts::Track::AbsMax_Cov) return false;
    fHist_Tracks_Bookkeeping->Fill(21);

    return true;
}

// Clear the branches of the reconstructed tracks.
void AliAnalysisTaskEsd2Vector::ClearTracksBranches() {
    tTrack_X.clear();
    tTrack_Y.clear();
    tTrack_Z.clear();
    tTrack_Px.clear();
    tTrack_Py.clear();
    tTrack_Pz.clear();
    tTrack_Charge.clear();
    tTrack_DCAxy.clear();
    tTrack_DCAz.clear();
    tTrack_TPCSignal.clear();
    tTrack_NSigmaPion.clear();
    tTrack_NSigmaKaon.clear();
    tTrack_NSigmaProton.clear();
    tTrack_SigmaX2.clear();
    tTrack_SigmaXY.clear();
    tTrack_SigmaY2.clear();
    tTrack_SigmaXZ.clear();
    tTrack_SigmaYZ.clear();
    tTrack_SigmaZ2.clear();
    tTrack_SigmaXPx.clear();
    tTrack_SigmaYPx.clear();
    tTrack_SigmaZPx.clear();
    tTrack_SigmaPx2.clear();
    tTrack_SigmaXPy.clear();
    tTrack_SigmaYPy.clear();
    tTrack_SigmaZPy.clear();
    tTrack_SigmaPxPy.clear();
    tTrack_SigmaPy2.clear();
    tTrack_SigmaXPz.clear();
    tTrack_SigmaYPz.clear();
    tTrack_SigmaZPz.clear();
    tTrack_SigmaPxPz.clear();
    tTrack_SigmaPyPz.clear();
    tTrack_SigmaPz2.clear();
#if E2V_DEBUG
    tTrack_iESD.clear();
    if (fIsMC) tTrack_iMC.clear();
#endif
    if (fIsMC) tTrack_McEntry.clear();
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

#if E2V_DEBUG
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
#endif

    AliInfoF("Closing file %s ...", new_path.Data());
    SimLogFile.close();

    return true;
}
