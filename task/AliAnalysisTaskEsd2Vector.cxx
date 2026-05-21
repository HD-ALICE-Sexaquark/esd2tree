#include "AliAnalysisTaskEsd2Vector.h"

#include <fstream>
#include <sstream>
#include <string>

#include <TChain.h>
#include <TFile.h>
#include <TGrid.h>
#include <TList.h>
#include <TObjString.h>
#include <TSystem.h>

#include <AliAnalysisManager.h>
#include <AliAnalysisTaskSE.h>
#include <AliAnalysisUtils.h>
#include <AliESDEvent.h>
#include <AliESDInputHandler.h>
#include <AliESDtrack.h>
#include <AliLog.h>
#include <AliMCEvent.h>
#include <AliMCParticle.h>
#include <AliMultSelection.h>
#include <AliPIDResponse.h>
#include <AliVEvent.h>

#include "AliAnalysisTaskEsd2Vector_Const.h"
#include "AliAnalysisTaskEsd2Vector_Cuts.h"

ClassImp(AliAnalysisTaskEsd2Vector);

// Constructor, called locally.
AliAnalysisTaskEsd2Vector::AliAnalysisTaskEsd2Vector(const char* name)
    : AliAnalysisTaskSE{name},
      //
      fIsMC{false},
      fIsSignalMC{false},
      //
      fMC{nullptr},
      fMC_PrimaryVertex{nullptr},
      fESD{nullptr},
      fPrimaryVertex{nullptr},
      fPIDResponse{nullptr},
      fEventCuts{},
      //
      fRunNumber{0},
      fDirNumber{0},
      fDirNumberB{0},
      fEventNumberInFile{0},
      fCentrality{0.},
      fMagneticField{0.},
      //
      fAliEnPath{""},
      fSignalLog_NewBasename{""},
      //
      fEvVec_ReactionID{},
      fEvVec_Sexaquark_Px{},
      fEvVec_Sexaquark_Py{},
      fEvVec_Sexaquark_Pz{},
      fEvVec_Nucleon_Px{},
      fEvVec_Nucleon_Py{},
      fEvVec_Nucleon_Pz{},
      //
      fOutputList{nullptr},
      fHist_Events_Bookkeeping{nullptr},
      fHist_Centrality{nullptr},
      fHist_CentralityINT7{nullptr},
      fHist_Tracks_Bookkeeping{nullptr},
      //
      fOutputTree{nullptr},
      //
      tEvent_PV_NContributors{0},
      tEvent_PV_Dispersion{0.},
      tEvent_PV_X{0.},
      tEvent_PV_Y{0.},
      tEvent_PV_Z{0.},
      tEvent_PV_CovMatrix{},
      tEvent_SPD_PV_Z{0.},
      tEvent_SPD_PV_ZErr{0.},
      tEvent_NTracks{0},
      tEvent_NTPCClusters{0},
      tEvent_IsMB{false},
      tEvent_IsHighMultV0{false},
      tEvent_IsHighMultSPD{false},
      tEvent_IsCentral{false},
      tEvent_IsSemiCentral{false},
      //
      tEvent_MC_PV_X{0.},
      tEvent_MC_PV_Y{0.},
      tEvent_MC_PV_Z{0.},
      tEvent_MC_IsGenPileup{false},
      tEvent_MC_IsSBCPileup{false},
      //
      tInjected_ReactionID{},
      tInjected_Px{},
      tInjected_Py{},
      tInjected_Pz{},
      tInjected_Nucleon_Px{},
      tInjected_Nucleon_Py{},
      tInjected_Nucleon_Pz{},
      //
      tMC_PdgCode{},
      tMC_Charge{},
      tMC_Mother_McEntry{},
      tMC_N_Daughters{},
      tMC_FirstDau_McEntry{},
      tMC_LastDau_McEntry{},
      tMC_Origin_X{},
      tMC_Origin_Y{},
      tMC_Origin_Z{},
      tMC_Px{},
      tMC_Py{},
      tMC_Pz{},
      tMC_E{},
      tMC_Status{},
      tMC_Generator{},
      tMC_IsPhysPrimary{},
      tMC_IsSecFromMat{},
      tMC_IsSecFromWeak{},
#if E2V_MC_EXTRA
      tMC_IsOOBPileup{},
#endif
      //
      tTrack_EsdEntry{},
      tTrack_X{},
      tTrack_Y{},
      tTrack_Z{},
      tTrack_Px{},
      tTrack_Py{},
      tTrack_Pz{},
      tTrack_Charge{},
      tTrack_PreDCAxy{},
      tTrack_PreDCAz{},
      tTrack_TPC_Signal{},
      tTrack_ITS_FirstLayer{},
      tTrack_NSigmaPion{},
      tTrack_NSigmaKaon{},
      tTrack_NSigmaProton{},
      tTrack_CovMatrix{},
#if E2V_TPC_EXTRA
      tTrack_TPC_DCAxy{},
      tTrack_TPC_DCAz{},
      tTrack_TPC_NCrossedRows{},
      tTrack_TPC_NClusters{},
      tTrack_TPC_NClustersLC{},
      tTrack_TPC_NClustersFound{},
      tTrack_TPC_NClustersShared{},
      tTrack_TPC_Chi2{},
      tTrack_TPC_Chi2Constrained{},
      tTrack_TPC_Chi2TCVG{},
      tTrack_TPC_SignalTunedOnData{},
      tTrack_TPC_SignalSigma{},
      tTrack_TPC_SignalCorrected{},
      tTrack_TPC_ESignalPion{},
      tTrack_TPC_ESigmaPion{},
      tTrack_TPC_ESignalKaon{},
      tTrack_TPC_ESigmaKaon{},
      tTrack_TPC_ESignalProton{},
      tTrack_TPC_ESigmaProton{},
      tTrack_TPC_SignalN{},
      tTrack_TPC_PointsFirst{},
      tTrack_TPC_PointsIndexMax{},
      tTrack_TPC_PointsLast{},
      tTrack_TPC_PointsMaxDens{},
      tTrack_TPC_FirstRow{},
#endif
      tTrack_McEntry{},
      //
      tLambda_EsdEntry{},
      tLambda_Decay_X{},
      tLambda_Decay_Y{},
      tLambda_Decay_Z{},
      tLambda_Px{},
      tLambda_Py{},
      tLambda_Pz{},
      tLambda_DcaV0Daughters{},
      //
      tLambda_Neg_EsdEntry{},
      tLambda_Neg_PCAwrtV0_Px{},
      tLambda_Neg_PCAwrtV0_Py{},
      tLambda_Neg_PCAwrtV0_Pz{},
      tLambda_Neg_PreDCAxy{},
      tLambda_Neg_PreDCAz{},
      tLambda_Neg_NSigmaProton{},
      tLambda_Neg_NSigmaKaon{},
      tLambda_Neg_NSigmaPion{},
      //
      tLambda_Pos_EsdEntry{},
      tLambda_Pos_PCAwrtV0_Px{},
      tLambda_Pos_PCAwrtV0_Py{},
      tLambda_Pos_PCAwrtV0_Pz{},
      tLambda_Pos_PreDCAxy{},
      tLambda_Pos_PreDCAz{},
      tLambda_Pos_NSigmaProton{},
      tLambda_Pos_NSigmaKaon{},
      tLambda_Pos_NSigmaPion{},
      //
      tLambda_Neg_McEntry{},
      tLambda_Pos_McEntry{} {
    DefineInput(0, TChain::Class());
    DefineOutput(1, TList::Class());  // fOutputList
    DefineOutput(2, TTree::Class());  // fOutputTree
}

// Empty I/O constructor. Non-persistent members are initialized to their default values from here.
AliAnalysisTaskEsd2Vector::AliAnalysisTaskEsd2Vector() : AliAnalysisTaskEsd2Vector{""} {}

// Destructor.
// NOTE: if `TList::SetOwner(true)` was called, the TList destructor should delete all objects added to it.
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

// # Executed at Runtime # //

// Create output objects, called once at RUNTIME ~ execution on Grid.
void AliAnalysisTaskEsd2Vector::UserCreateOutputObjects() {

    auto* man = AliAnalysisManager::GetAnalysisManager();
    if (man == nullptr) AliFatal("AliAnalysisManager couldn't be found.");

    auto* inputHandler = dynamic_cast<AliESDInputHandler*>(man->GetInputEventHandler());
    if (inputHandler == nullptr) AliFatal("AliESDInputHandler couldn't be found.");

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
    CreateLambdasBranches();

    // Post data //

    PostData(1, fOutputList);
    PostData(2, fOutputTree);
}

// User implementation of `Notify()`. Needed for reading the AliEn path.
// This function is loaded during `AliAnalysisManager::Notify()`.
// It's called after `UserCreateOutputObjects()`, for each new file, and before the first `UserExec()`.
bool AliAnalysisTaskEsd2Vector::UserNotify() {

    auto* man = AliAnalysisManager::GetAnalysisManager();
    if (man == nullptr) AliFatal("Analysis Manager not found");
    auto* man_tree = man->GetTree();
    if (man_tree == nullptr) AliFatal("Analysis Manager Tree not found");
    auto* man_file = man_tree->GetCurrentFile();
    if (man_file == nullptr) AliFatal("Analysis Manager File not found");

    // get AliEn path and tokenize it //

    fAliEnPath = man_file->GetName();
    if (fAliEnPath == "") AliWarning("fAliEnPath couldn't be found.");
    AliInfoF("fAliEnPath = %s", fAliEnPath.Data());

    TObjArray* tokens = fAliEnPath.Tokenize("/");
    if (fIsMC) {
        // NOTES: path of MC (signal/gen.purp.) ends with format `.../297595/001/AliESDs.root`
        fRunNumber = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries() - 3)))->GetString().Atoi();
        fDirNumber = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries() - 2)))->GetString().Atoi();
        AliInfoF("Run Number = %04i", static_cast<int>(fRunNumber));  // = 1
        AliInfoF("Dir Number = %04i", static_cast<int>(fDirNumber));  // = 1
        if (fIsSignalMC) {
            BringSignalLogs();
            ReadSignalLogs();
        }
    } else {
        // NOTE: path of real data ends with format `.../LHC15o/000245232/pass2/15000245232039.914/AliESDs.root`
        fRunNumber = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries() - 4)))->GetString().Atoi();
        AliInfoF("Run Number = %i", fRunNumber);
        auto aux_dir_nr = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries() - 2)))->GetString();
        aux_dir_nr = TString(aux_dir_nr(2 + 3 + 6, 10));  // = "039.914"
        fDirNumber = TString(aux_dir_nr(0, 3)).Atoi();    // = 39
        fDirNumberB = TString(aux_dir_nr(4, 5)).Atoi();   // = 914
        AliInfoF("Dir Number = %i", fDirNumber);
        AliInfoF("Dir Number B = %i", fDirNumberB);
    }
    delete tokens;  // NOTE: because ROOT

    // Adapt event cuts -- depending on run number //

    // Reference: https://twiki.cern.ch/twiki/bin/viewauth/ALICE/AliDPGRunList18r1
    if (!fIsMC && (fRunNumber == 296749 || fRunNumber == 296750 || fRunNumber == 296849 || fRunNumber == 296890 || fRunNumber == 297029 ||
                   fRunNumber == 297194 || fRunNumber == 297219 || fRunNumber == 297481)) {
        fEventCuts.UseTimeRangeCut();
        fEventCuts.OverrideAutomaticTriggerSelection(AliVEvent::kINT7);
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

    // (anti)lambdas //

    ProcessLambdas();

    // end of event //

    fOutputTree->Fill();
    if (fIsMC) {
        if (fIsSignalMC) ClearBranches_Injected();
        ClearBranches_MC();
    }
    ClearBranches_Tracks();
    ClearBranches_Lambdas();

    // post data //

    PostData(1, fOutputList);
    PostData(2, fOutputTree);
}

// # Events # //

bool AliAnalysisTaskEsd2Vector::ProcessEvent() {

#if E2V_DEBUG
    AliInfoF("fPIDResponse.UseTPCEtaCorrection()          = %i", fPIDResponse->UseTPCEtaCorrection());
    AliInfoF("fPIDResponse.UseTPCMultiplicityCorrection() = %i", fPIDResponse->UseTPCMultiplicityCorrection());
    AliInfoF("fPIDResponse.UseTPCPileupCorrection()       = %i", fPIDResponse->UseTPCPileupCorrection());
#endif

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

    // NOTE: `fRunNumber`, `fDirNumber` and `fDirNumberB` are set in `UserNotify()`
    fEventNumberInFile = fESD->GetEventNumberInFile();
    fPrimaryVertex = fESD->GetPrimaryVertex();

    // Apply selection //

    if (!PassesEventSelection()) return false;

    // Assign more branches (2) //

    auto* MultSelection = dynamic_cast<AliMultSelection*>(fESD->FindListObject("MultSelection"));
    if (MultSelection == nullptr) AliFatal("AliMultSelection couldn't be found.");
    fCentrality = MultSelection->GetMultiplicityPercentile("V0M");
    fMagneticField = static_cast<float>(fESD->GetMagneticField());

    // Fill QA hist //

    fHist_Centrality->Fill(fCentrality);
    if ((fInputHandler->IsEventSelected() & AliVEvent::kINT7) != 0U) fHist_CentralityINT7->Fill(fCentrality);

    // Assign rest of branches (3) //

    tEvent_PV_X = static_cast<float>(fPrimaryVertex->GetX());
    tEvent_PV_Y = static_cast<float>(fPrimaryVertex->GetY());
    tEvent_PV_Z = static_cast<float>(fPrimaryVertex->GetZ());

    tEvent_PV_NContributors = fPrimaryVertex->GetNContributors();
    tEvent_PV_Dispersion = static_cast<float>(fPrimaryVertex->GetDispersion());

    double PV_CovMatrix[6]{};
    fPrimaryVertex->GetCovarianceMatrix(PV_CovMatrix);
    for (int i = 0; i < 6; ++i) tEvent_PV_CovMatrix[i] = static_cast<float>(PV_CovMatrix[i]);

    const auto* PrimaryVertex_SPD = fESD->GetPrimaryVertexSPD();
    tEvent_SPD_PV_Z = static_cast<float>(PrimaryVertex_SPD->GetZ());

    double PV_SPD_CovMatrix[6]{};
    PrimaryVertex_SPD->GetCovarianceMatrix(PV_SPD_CovMatrix);
    tEvent_SPD_PV_ZErr = static_cast<float>(PV_SPD_CovMatrix[5]);

    tEvent_NTracks = fESD->GetNumberOfTracks();
    tEvent_NTPCClusters = fESD->GetNumberOfTPCClusters();
    tEvent_IsMB = ((fInputHandler->IsEventSelected() & AliVEvent::kINT7) != 0U);
    tEvent_IsHighMultV0 = ((fInputHandler->IsEventSelected() & AliVEvent::kHighMultV0) != 0U);
    tEvent_IsHighMultSPD = ((fInputHandler->IsEventSelected() & AliVEvent::kHighMultSPD) != 0U);
    tEvent_IsCentral = ((fInputHandler->IsEventSelected() & AliVEvent::kCentral) != 0U);
    tEvent_IsSemiCentral = ((fInputHandler->IsEventSelected() & AliVEvent::kSemiCentral) != 0U);

    // Assign MC branches (4) //

    if (fIsMC) {
        fMC_PrimaryVertex = fMC->GetPrimaryVertex();
        tEvent_MC_PV_X = static_cast<float>(fMC_PrimaryVertex->GetX());
        tEvent_MC_PV_Y = static_cast<float>(fMC_PrimaryVertex->GetY());
        tEvent_MC_PV_Z = static_cast<float>(fMC_PrimaryVertex->GetZ());
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
    // Pileup Events //
    if (!fEventCuts.PassedCut(AliEventCuts::kPileUp)) return false;
    fHist_Events_Bookkeeping->Fill(2.);
    // TPC Pileup Events //
    if (!fEventCuts.PassedCut(AliEventCuts::kTPCPileUp)) return false;
    fHist_Events_Bookkeeping->Fill(3.);
    // Important for data? //
    if (!fIsMC && fESD->GetHeader()->GetEventType() != 7) return false;
    fHist_Events_Bookkeeping->Fill(4.);
    // Trigger Selection //
    bool IsMB = (fInputHandler->IsEventSelected() & AliVEvent::kINT7) != 0U;
    bool IsHighMultV0 = (fInputHandler->IsEventSelected() & AliVEvent::kHighMultV0) != 0U;
    bool IsHighMultSPD = (fInputHandler->IsEventSelected() & AliVEvent::kHighMultSPD) != 0U;
    bool IsCentral = (fInputHandler->IsEventSelected() & AliVEvent::kCentral) != 0U;
    bool IsSemiCentral = (fInputHandler->IsEventSelected() & AliVEvent::kSemiCentral) != 0U;
    if (!IsMB && !IsHighMultV0 && !IsHighMultSPD && !IsCentral && !IsSemiCentral) return false;
    fHist_Events_Bookkeeping->Fill(5.);
    // rec. PV z-vertex range //
    if (std::abs(fPrimaryVertex->GetZ()) > Cuts::Event::AbsMax_PV_Z) return false;
    fHist_Events_Bookkeeping->Fill(6.);

    return true;
}

// # Trees # //

// Add branches to `fOutputTree`.
void AliAnalysisTaskEsd2Vector::CreateEventsBranches() {
    fOutputTree->Branch("RunNumber", &fRunNumber);
    fOutputTree->Branch("DirNumber", &fDirNumber);
    if (!fIsMC) fOutputTree->Branch("DirNumberB", &fDirNumberB);
    fOutputTree->Branch("EventNumber", &fEventNumberInFile);
    fOutputTree->Branch("Centrality", &fCentrality);
    fOutputTree->Branch("MagneticField", &fMagneticField);
    if (fIsMC) {
        fOutputTree->Branch("MC_PV_X", &tEvent_MC_PV_X);
        fOutputTree->Branch("MC_PV_Y", &tEvent_MC_PV_Y);
        fOutputTree->Branch("MC_PV_Z", &tEvent_MC_PV_Z);
        fOutputTree->Branch("MC_IsGenPileup", &tEvent_MC_IsGenPileup);
        fOutputTree->Branch("MC_IsSBCPileup", &tEvent_MC_IsSBCPileup);
    }
    fOutputTree->Branch("PV_NContributors", &tEvent_PV_NContributors);
    fOutputTree->Branch("PV_Dispersion", &tEvent_PV_Dispersion);
    fOutputTree->Branch("PV_X", &tEvent_PV_X);
    fOutputTree->Branch("PV_Y", &tEvent_PV_Y);
    fOutputTree->Branch("PV_Z", &tEvent_PV_Z);
    fOutputTree->Branch("PV_CovMatrix", &tEvent_PV_CovMatrix, "PV_CovMatrix[6]/F");
    fOutputTree->Branch("SPD_PV_Z", &tEvent_SPD_PV_Z);
    fOutputTree->Branch("SPD_PV_ZErr", &tEvent_SPD_PV_ZErr);
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
    fOutputTree->Branch("Sexa_Px", &tInjected_Px);
    fOutputTree->Branch("Sexa_Py", &tInjected_Py);
    fOutputTree->Branch("Sexa_Pz", &tInjected_Pz);
    fOutputTree->Branch("Nucleon_Px", &tInjected_Nucleon_Px);
    fOutputTree->Branch("Nucleon_Py", &tInjected_Nucleon_Py);
    fOutputTree->Branch("Nucleon_Pz", &tInjected_Nucleon_Pz);
}

// Add branches to the MC tree.
void AliAnalysisTaskEsd2Vector::CreateMCBranches() {
    fOutputTree->Branch("MC_PdgCode", &tMC_PdgCode);
    fOutputTree->Branch("MC_Charge", &tMC_Charge);
    fOutputTree->Branch("MC_Mother_McEntry", &tMC_Mother_McEntry);
    fOutputTree->Branch("MC_N_Daughters", &tMC_N_Daughters);
    fOutputTree->Branch("MC_FirstDau_McEntry", &tMC_FirstDau_McEntry);
    fOutputTree->Branch("MC_LastDau_McEntry", &tMC_LastDau_McEntry);
    fOutputTree->Branch("MC_Origin_X", &tMC_Origin_X);
    fOutputTree->Branch("MC_Origin_Y", &tMC_Origin_Y);
    fOutputTree->Branch("MC_Origin_Z", &tMC_Origin_Z);
    fOutputTree->Branch("MC_Px", &tMC_Px);
    fOutputTree->Branch("MC_Py", &tMC_Py);
    fOutputTree->Branch("MC_Pz", &tMC_Pz);
    fOutputTree->Branch("MC_E", &tMC_E);
    fOutputTree->Branch("MC_Status", &tMC_Status);
    fOutputTree->Branch("MC_Generator", &tMC_Generator);
    fOutputTree->Branch("MC_IsPhysPrimary", &tMC_IsPhysPrimary);
    fOutputTree->Branch("MC_IsSecFromMat", &tMC_IsSecFromMat);
    fOutputTree->Branch("MC_IsSecFromWeak", &tMC_IsSecFromWeak);
#if E2V_MC_EXTRA
    fOutputTree->Branch("MC_IsOOBPileup", &tMC_IsOOBPileup);
#endif
}

// Add branches to the tracks tree.
void AliAnalysisTaskEsd2Vector::CreateTracksBranches() {
    fOutputTree->Branch("Track_EsdEntry", &tTrack_EsdEntry);
    fOutputTree->Branch("Track_X", &tTrack_X);
    fOutputTree->Branch("Track_Y", &tTrack_Y);
    fOutputTree->Branch("Track_Z", &tTrack_Z);
    fOutputTree->Branch("Track_Px", &tTrack_Px);
    fOutputTree->Branch("Track_Py", &tTrack_Py);
    fOutputTree->Branch("Track_Pz", &tTrack_Pz);
    fOutputTree->Branch("Track_Charge", &tTrack_Charge);
    fOutputTree->Branch("Track_PreDCAxy", &tTrack_PreDCAxy);
    fOutputTree->Branch("Track_PreDCAz", &tTrack_PreDCAz);
    fOutputTree->Branch("Track_TPC_Signal", &tTrack_TPC_Signal);
    fOutputTree->Branch("Track_ITS_FirstLayer", &tTrack_ITS_FirstLayer);
    fOutputTree->Branch("Track_NSigmaPion", &tTrack_NSigmaPion);
    fOutputTree->Branch("Track_NSigmaKaon", &tTrack_NSigmaKaon);
    fOutputTree->Branch("Track_NSigmaProton", &tTrack_NSigmaProton);
    fOutputTree->Branch("Track_CovMatrix", &tTrack_CovMatrix);
#if E2V_TPC_EXTRA
    fOutputTree->Branch("Track_TPC_DCAxy", &tTrack_TPC_DCAxy);
    fOutputTree->Branch("Track_TPC_DCAz", &tTrack_TPC_DCAz);
    fOutputTree->Branch("Track_TPC_NCrossedRows", &tTrack_TPC_NCrossedRows);
    fOutputTree->Branch("Track_TPC_NClusters", &tTrack_TPC_NClusters);
    fOutputTree->Branch("Track_TPC_NClustersLC", &tTrack_TPC_NClustersLC);
    fOutputTree->Branch("Track_TPC_NClustersFound", &tTrack_TPC_NClustersFound);
    fOutputTree->Branch("Track_TPC_NClustersShared", &tTrack_TPC_NClustersShared);
    fOutputTree->Branch("Track_TPC_Chi2", &tTrack_TPC_Chi2);
    fOutputTree->Branch("Track_TPC_Chi2Constrained", &tTrack_TPC_Chi2Constrained);
    fOutputTree->Branch("Track_TPC_Chi2TCVG", &tTrack_TPC_Chi2TCVG);
    if (fIsMC) fOutputTree->Branch("Track_TPC_SignalTunedOnData", &tTrack_TPC_SignalTunedOnData);
    fOutputTree->Branch("Track_TPC_SignalSigma", &tTrack_TPC_SignalSigma);
    fOutputTree->Branch("Track_TPC_SignalCorrected", &tTrack_TPC_SignalCorrected);
    fOutputTree->Branch("Track_TPC_ESignalPion", &tTrack_TPC_ESignalPion);
    fOutputTree->Branch("Track_TPC_ESigmaPion", &tTrack_TPC_ESigmaPion);
    fOutputTree->Branch("Track_TPC_ESignalKaon", &tTrack_TPC_ESignalKaon);
    fOutputTree->Branch("Track_TPC_ESigmaKaon", &tTrack_TPC_ESigmaKaon);
    fOutputTree->Branch("Track_TPC_ESignalProton", &tTrack_TPC_ESignalProton);
    fOutputTree->Branch("Track_TPC_ESigmaProton", &tTrack_TPC_ESigmaProton);
    fOutputTree->Branch("Track_TPC_SignalN", &tTrack_TPC_SignalN);
    fOutputTree->Branch("Track_TPC_PointsFirst", &tTrack_TPC_PointsFirst);
    fOutputTree->Branch("Track_TPC_PointsIndexMax", &tTrack_TPC_PointsIndexMax);
    fOutputTree->Branch("Track_TPC_PointsLast", &tTrack_TPC_PointsLast);
    fOutputTree->Branch("Track_TPC_PointsMaxDens", &tTrack_TPC_PointsMaxDens);
    fOutputTree->Branch("Track_TPC_FirstRow", &tTrack_TPC_FirstRow);
#endif
    if (fIsMC) fOutputTree->Branch("Track_McEntry", &tTrack_McEntry);
}

void AliAnalysisTaskEsd2Vector::CreateLambdasBranches() {
    fOutputTree->Branch("Lambda_EsdEntry", &tLambda_EsdEntry);
    fOutputTree->Branch("Lambda_Decay_X", &tLambda_Decay_X);
    fOutputTree->Branch("Lambda_Decay_Y", &tLambda_Decay_Y);
    fOutputTree->Branch("Lambda_Decay_Z", &tLambda_Decay_Z);
    fOutputTree->Branch("Lambda_Px", &tLambda_Px);
    fOutputTree->Branch("Lambda_Py", &tLambda_Py);
    fOutputTree->Branch("Lambda_Pz", &tLambda_Pz);
    fOutputTree->Branch("Lambda_DcaV0Daughters", &tLambda_DcaV0Daughters);
    // -- negative daughter (anti-proton for anti-lambda, pi-minus for lambda)
    fOutputTree->Branch("Lambda_Neg_EsdEntry", &tLambda_Neg_EsdEntry);
    fOutputTree->Branch("Lambda_Neg_PCAwrtV0_Px", &tLambda_Neg_PCAwrtV0_Px);
    fOutputTree->Branch("Lambda_Neg_PCAwrtV0_Py", &tLambda_Neg_PCAwrtV0_Py);
    fOutputTree->Branch("Lambda_Neg_PCAwrtV0_Pz", &tLambda_Neg_PCAwrtV0_Pz);
    fOutputTree->Branch("Lambda_Neg_PreDCAxy", &tLambda_Neg_PreDCAxy);
    fOutputTree->Branch("Lambda_Neg_PreDCAz", &tLambda_Neg_PreDCAz);
    fOutputTree->Branch("Lambda_Neg_NSigmaProton", &tLambda_Neg_NSigmaProton);
    fOutputTree->Branch("Lambda_Neg_NSigmaKaon", &tLambda_Neg_NSigmaKaon);
    fOutputTree->Branch("Lambda_Neg_NSigmaPion", &tLambda_Neg_NSigmaPion);
    // -- positive daughter (proton for lambda, pi-plus for anti-lambda)
    fOutputTree->Branch("Lambda_Pos_EsdEntry", &tLambda_Pos_EsdEntry);
    fOutputTree->Branch("Lambda_Pos_PCAwrtV0_Px", &tLambda_Pos_PCAwrtV0_Px);
    fOutputTree->Branch("Lambda_Pos_PCAwrtV0_Py", &tLambda_Pos_PCAwrtV0_Py);
    fOutputTree->Branch("Lambda_Pos_PCAwrtV0_Pz", &tLambda_Pos_PCAwrtV0_Pz);
    fOutputTree->Branch("Lambda_Pos_PreDCAxy", &tLambda_Pos_PreDCAxy);
    fOutputTree->Branch("Lambda_Pos_PreDCAz", &tLambda_Pos_PreDCAz);
    fOutputTree->Branch("Lambda_Pos_NSigmaProton", &tLambda_Pos_NSigmaProton);
    fOutputTree->Branch("Lambda_Pos_NSigmaKaon", &tLambda_Pos_NSigmaKaon);
    fOutputTree->Branch("Lambda_Pos_NSigmaPion", &tLambda_Pos_NSigmaPion);
    if (fIsMC) {
        fOutputTree->Branch("Lambda_Neg_McEntry", &tLambda_Neg_McEntry);
        fOutputTree->Branch("Lambda_Pos_McEntry", &tLambda_Pos_McEntry);
    }
}

// # MC Generated # //

// Loop over MC particles in a single event.
void AliAnalysisTaskEsd2Vector::ProcessMCParticles() {
    const int n_mc = fMC->GetNumberOfTracks();
    ReserveBranches_MC(static_cast<std::size_t>(n_mc));
    // read mc particles //
    for (int mc_entry = 0; mc_entry < n_mc; ++mc_entry) {
        auto* mcPart = dynamic_cast<AliMCParticle*>(fMC->GetTrack(mc_entry));
        if (mcPart == nullptr) continue;
        // -- fill branches
        tMC_PdgCode.emplace_back(mcPart->PdgCode());
        tMC_Charge.emplace_back(static_cast<int>(mcPart->Charge() / 3));
        tMC_Mother_McEntry.emplace_back(mcPart->GetMother());
        tMC_N_Daughters.emplace_back(static_cast<unsigned int>(mcPart->GetNDaughters()));
        tMC_FirstDau_McEntry.emplace_back(mcPart->GetDaughterFirst());
        tMC_LastDau_McEntry.emplace_back(mcPart->GetDaughterLast());
        tMC_Origin_X.emplace_back(static_cast<float>(mcPart->Xv()));
        tMC_Origin_Y.emplace_back(static_cast<float>(mcPart->Yv()));
        tMC_Origin_Z.emplace_back(static_cast<float>(mcPart->Zv()));
        tMC_Px.emplace_back(static_cast<float>(mcPart->Px()));
        tMC_Py.emplace_back(static_cast<float>(mcPart->Py()));
        tMC_Pz.emplace_back(static_cast<float>(mcPart->Pz()));
        tMC_E.emplace_back(static_cast<float>(mcPart->E()));
        tMC_Status.emplace_back(mcPart->MCStatusCode());
        tMC_Generator.emplace_back(static_cast<char>(mcPart->GetGeneratorIndex()));
        tMC_IsPhysPrimary.emplace_back(static_cast<char>(mcPart->IsPhysicalPrimary()));
        tMC_IsSecFromMat.emplace_back(static_cast<char>(mcPart->IsSecondaryFromMaterial()));
        tMC_IsSecFromWeak.emplace_back(static_cast<char>(mcPart->IsSecondaryFromWeakDecay()));
#if E2V_DEBUG
        if (mcPart->GetGeneratorIndex() == 2) {
            AliInfoF("mc_entry=%i,mc_pdg=%i,mc_is_physprim=%i,mc_status=%i,mother_entry=%i",  //
                     mc_entry, mcPart->PdgCode(), mcPart->IsPhysicalPrimary(), mcPart->MCStatusCode(), mcPart->GetMother());
        }
#endif
#if E2V_MC_EXTRA
        tMC_IsOOBPileup.emplace_back(static_cast<char>(AliAnalysisUtils::IsParticleFromOutOfBunchPileupCollision(mc_entry, fMC)));
#endif
    }  // end of loop over MC particles
}

// Reserve MC particle's vectors' memory allocation.
void AliAnalysisTaskEsd2Vector::ReserveBranches_MC(std::size_t size) {
    tMC_PdgCode.reserve(size);
    tMC_Charge.reserve(size);
    tMC_Mother_McEntry.reserve(size);
    tMC_N_Daughters.reserve(size);
    tMC_FirstDau_McEntry.reserve(size);
    tMC_LastDau_McEntry.reserve(size);
    tMC_Origin_X.reserve(size);
    tMC_Origin_Y.reserve(size);
    tMC_Origin_Z.reserve(size);
    tMC_Px.reserve(size);
    tMC_Py.reserve(size);
    tMC_Pz.reserve(size);
    tMC_E.reserve(size);
    tMC_Status.reserve(size);
    tMC_Generator.reserve(size);
    tMC_IsPhysPrimary.reserve(size);
    tMC_IsSecFromMat.reserve(size);
    tMC_IsSecFromWeak.reserve(size);
#if E2V_MC_EXTRA
    tMC_IsOOBPileup.reserve(size);
#endif
}

// Clear MC branches.
void AliAnalysisTaskEsd2Vector::ClearBranches_MC() {
    tMC_PdgCode.clear();
    tMC_Charge.clear();
    tMC_Mother_McEntry.clear();
    tMC_N_Daughters.clear();
    tMC_FirstDau_McEntry.clear();
    tMC_LastDau_McEntry.clear();
    tMC_Origin_X.clear();
    tMC_Origin_Y.clear();
    tMC_Origin_Z.clear();
    tMC_Px.clear();
    tMC_Py.clear();
    tMC_Pz.clear();
    tMC_E.clear();
    tMC_Status.clear();
    tMC_Generator.clear();
    tMC_IsPhysPrimary.clear();
    tMC_IsSecFromMat.clear();
    tMC_IsSecFromWeak.clear();
#if E2V_MC_EXTRA
    tMC_IsOOBPileup.clear();
#endif
}

// # Reconstructed # //

// Loop over the reconstructed tracks in a single event.
void AliAnalysisTaskEsd2Vector::ProcessTracks() {
    const int n_tracks = fESD->GetNumberOfTracks();
    ReserveBranches_Tracks(static_cast<std::size_t>(n_tracks));
    for (int esd_entry = 0; esd_entry < n_tracks; ++esd_entry) {
        // get track //
        auto* track = fESD->GetTrack(esd_entry);
        // track selection //
        if (!PassesTrackSelection(track)) continue;
        // get info //
        const auto* inner_param = track->GetInnerParam();  // NOTE: already protected in `PassesTrackSelection`
        if (inner_param == nullptr) continue;
        double position[3]{};
        inner_param->GetXYZ(position);
        double momentum[3]{};
        inner_param->GetPxPyPz(momentum);
        float dca[2]{};
        float dca_cov[3]{};
        track->GetImpactParameters(dca, dca_cov);
        double cov_xyz_pxpypz[Const::NCovMatrixComponents]{};
        track->GetCovarianceXYZPxPyPz(cov_xyz_pxpypz);
        // fill branches //
        tTrack_EsdEntry.emplace_back(static_cast<unsigned int>(esd_entry));
        tTrack_X.emplace_back(static_cast<float>(position[0]));
        tTrack_Y.emplace_back(static_cast<float>(position[1]));
        tTrack_Z.emplace_back(static_cast<float>(position[2]));
        tTrack_Px.emplace_back(static_cast<float>(momentum[0]));
        tTrack_Py.emplace_back(static_cast<float>(momentum[1]));
        tTrack_Pz.emplace_back(static_cast<float>(momentum[2]));
        tTrack_Charge.emplace_back(inner_param->Charge());
        // -- dca //
        tTrack_PreDCAxy.emplace_back(dca[0]);
        tTrack_PreDCAz.emplace_back(dca[1]);
        // -- pid //
        auto bethe_pion = static_cast<double>(fPIDResponse->GetExpectedSignal(AliPIDResponse::kTPC, track, AliPID::kPion));
        auto e_sigma_pion = static_cast<double>(fPIDResponse->GetExpectedSigma(AliPIDResponse::kTPC, track, AliPID::kPion));
        auto n_sigmas_pion =
            static_cast<double>(fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, track, AliPID::kPion));  // NOTE: includes corrections
        auto n_sigmas_kaon =
            static_cast<double>(fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, track, AliPID::kKaon));  // NOTE: includes corrections
        auto n_sigmas_proton =
            static_cast<double>(fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, track, AliPID::kProton));  // NOTE: includes corrections
        double tpc_signal;
#if !E2V_TPC_EXTRA
        if (fIsMC) {
            tpc_signal = track->GetTPCsignalTunedOnData();
        } else {
            tpc_signal = e_sigma_pion * n_sigmas_pion + bethe_pion;
        }
#else
        tpc_signal = track->GetTPCsignal();  // NOTE: uncorrected, store `Corrected` and `TunedOnData` in other branches
#endif
        tTrack_TPC_Signal.emplace_back(static_cast<float>(tpc_signal));
        tTrack_ITS_FirstLayer.emplace_back(GetFirstLayerFromITS(track));
        tTrack_NSigmaPion.emplace_back(static_cast<float>(n_sigmas_pion));
        tTrack_NSigmaKaon.emplace_back(static_cast<float>(n_sigmas_kaon));
        tTrack_NSigmaProton.emplace_back(static_cast<float>(n_sigmas_proton));
        // -- cov. matrix //
        for (double k : cov_xyz_pxpypz) tTrack_CovMatrix.emplace_back(static_cast<float>(k));
#if E2V_TPC_EXTRA
        // -- get tpc dca
        float tpc_dca_xy{};
        float tpc_dca_z{};
        track->GetImpactParametersTPC(tpc_dca_xy, tpc_dca_z);
        // -- fill tpc branches (1)
        tTrack_TPC_DCAxy.emplace_back(tpc_dca_xy);
        tTrack_TPC_DCAz.emplace_back(tpc_dca_z);
        tTrack_TPC_NCrossedRows.emplace_back(track->GetTPCCrossedRows());
        tTrack_TPC_NClusters.emplace_back(static_cast<unsigned int>(track->GetTPCNcls()));
        tTrack_TPC_NClustersLC.emplace_back(static_cast<unsigned int>(track->GetTPCncls()));
        tTrack_TPC_NClustersFound.emplace_back(static_cast<unsigned int>(track->GetTPCNclsF()));
        tTrack_TPC_NClustersShared.emplace_back(static_cast<unsigned int>(track->GetTPCnclsS()));
        tTrack_TPC_Chi2.emplace_back(static_cast<float>(track->GetTPCchi2()));
        tTrack_TPC_Chi2Constrained.emplace_back(static_cast<float>(track->GetConstrainedChi2TPC()));
        tTrack_TPC_Chi2TCVG.emplace_back(static_cast<float>(track->GetChi2TPCConstrainedVsGlobal(fPrimaryVertex)));
        if (fIsMC) tTrack_TPC_SignalTunedOnData.emplace_back(static_cast<float>(track->GetTPCsignalTunedOnData()));
        tTrack_TPC_SignalSigma.emplace_back(static_cast<float>(track->GetTPCsignalSigma()));
        // -- get tpc pid info
        auto bethe_kaon = static_cast<double>(fPIDResponse->GetExpectedSignal(AliPIDResponse::kTPC, track, AliPID::kKaon));
        auto e_sigma_kaon = static_cast<double>(fPIDResponse->GetExpectedSigma(AliPIDResponse::kTPC, track, AliPID::kKaon));
        auto bethe_proton = static_cast<double>(fPIDResponse->GetExpectedSignal(AliPIDResponse::kTPC, track, AliPID::kProton));
        auto e_sigma_proton = static_cast<double>(fPIDResponse->GetExpectedSigma(AliPIDResponse::kTPC, track, AliPID::kProton));
        //    -- pion
        tTrack_TPC_SignalCorrected.emplace_back(static_cast<float>(e_sigma_pion * n_sigmas_pion + bethe_pion));
        tTrack_TPC_ESignalPion.emplace_back(static_cast<float>(bethe_pion));
        tTrack_TPC_ESigmaPion.emplace_back(static_cast<float>(e_sigma_pion));
        //    -- kaon
        tTrack_TPC_ESignalKaon.emplace_back(static_cast<float>(bethe_kaon));
        tTrack_TPC_ESigmaKaon.emplace_back(static_cast<float>(e_sigma_kaon));
        //    -- proton
        tTrack_TPC_ESignalProton.emplace_back(static_cast<float>(bethe_proton));
        tTrack_TPC_ESigmaProton.emplace_back(static_cast<float>(e_sigma_proton));
        // -- fill tpc branches (2)
        tTrack_TPC_SignalN.emplace_back(static_cast<unsigned int>(track->GetTPCsignalN()));
        tTrack_TPC_PointsFirst.emplace_back(static_cast<float>(track->GetTPCPoints(0)));
        tTrack_TPC_PointsIndexMax.emplace_back(static_cast<float>(track->GetTPCPoints(1)));
        tTrack_TPC_PointsLast.emplace_back(static_cast<float>(track->GetTPCPoints(2)));
        tTrack_TPC_PointsMaxDens.emplace_back(static_cast<float>(track->GetTPCPoints(3)));
        tTrack_TPC_FirstRow.emplace_back(GetFirstRowFromTPCClusterMap(track));
#endif
        if (fIsMC) tTrack_McEntry.emplace_back(std::abs(track->GetLabel()));
    }  // end of loop over tracks
}

// Check if track passes selection and fill bookkeeping histograms.
bool AliAnalysisTaskEsd2Vector::PassesTrackSelection(const AliESDtrack* track) {

    fHist_Tracks_Bookkeeping->Fill(0);

    // dca //

    // float dca[2]{};
    // float cov_dca[3]{};
    // track->GetImpactParameters(dca, cov_dca);
    // if (std::abs(dca[0]) / Cuts::Track::AbsMin_DCAxy + std::abs(dca[1]) / Cuts::Track::AbsMin_DCAz <= 1.) return false;
    fHist_Tracks_Bookkeeping->Fill(1);

    // status //

    auto status = track->GetStatus();
    bool tpc_in = (status & AliESDtrack::kTPCin) > 0;
    bool tpc_refit = (status & AliESDtrack::kTPCrefit) > 0;

    if (!tpc_in) return false;
    fHist_Tracks_Bookkeeping->Fill(2);

    if (!tpc_refit) return false;
    fHist_Tracks_Bookkeeping->Fill(3);

    // kinematics //

    const auto* inner_param = track->GetInnerParam();
    if (inner_param == nullptr) return false;

    // if (std::abs(inner_param->Pz()) > Cuts::Track::AbsMax_Pz) return false;
    fHist_Tracks_Bookkeeping->Fill(4);

    double p = inner_param->P();
    if (p < Cuts::Track::Min_P) return false;
    fHist_Tracks_Bookkeeping->Fill(5);

    if (p > Cuts::Track::Max_P) return false;
    fHist_Tracks_Bookkeeping->Fill(6);

    if (std::abs(inner_param->Eta()) > Cuts::Track::AbsMax_Eta) return false;
    fHist_Tracks_Bookkeeping->Fill(7);

    // if (inner_param->Pt() > Cuts::Track::Max_Pt) return false;
    fHist_Tracks_Bookkeeping->Fill(8);

    // pid //

    // PENDING: commented our temporarily
    // float n_sigma_proton = fPIDResponse->NumberOfSigmasTPC(track, AliPID::kProton);
    // float n_sigma_kaon = fPIDResponse->NumberOfSigmasTPC(track, AliPID::kKaon);
    // float n_sigma_pion = fPIDResponse->NumberOfSigmasTPC(track, AliPID::kPion);
    // if (std::abs(n_sigma_proton) > Cuts::Track::AbsMax_NSigma_PID && std::abs(n_sigma_kaon) > Cuts::Track::AbsMax_NSigma_PID &&
    // std::abs(n_sigma_pion) > Cuts::Track::AbsMax_NSigma_PID) {
    // return false;
    // }
    fHist_Tracks_Bookkeeping->Fill(9);

    // its info //

    // PENDING: commented our temporarily
    // if (track->GetITSNcls() > 0) return false;
    // fHist_Tracks_Bookkeeping->Fill(10);

    // tpc info //

    auto tpc_n_clusters = track->GetTPCNcls();
    if (tpc_n_clusters < Cuts::Track::Min_TPC_NClusters) return false;
    fHist_Tracks_Bookkeeping->Fill(11);

    auto tpc_chi2_ncls = track->GetTPCchi2() / static_cast<double>(tpc_n_clusters);
    if (tpc_chi2_ncls > Cuts::Track::Max_TPC_Chi2NClusters) return false;
    fHist_Tracks_Bookkeeping->Fill(12);

    // others //

    if (track->GetKinkIndex(0) > 0) return false;
    fHist_Tracks_Bookkeeping->Fill(13);

    return true;
}

// Reserve tracks-related vectors' memory allocation.
void AliAnalysisTaskEsd2Vector::ReserveBranches_Tracks(std::size_t size) {
    tTrack_EsdEntry.reserve(size);
    tTrack_X.reserve(size);
    tTrack_Y.reserve(size);
    tTrack_Z.reserve(size);
    tTrack_Px.reserve(size);
    tTrack_Py.reserve(size);
    tTrack_Pz.reserve(size);
    tTrack_Charge.reserve(size);
    tTrack_PreDCAxy.reserve(size);
    tTrack_PreDCAz.reserve(size);
    tTrack_TPC_Signal.reserve(size);
    tTrack_NSigmaPion.reserve(size);
    tTrack_NSigmaKaon.reserve(size);
    tTrack_NSigmaProton.reserve(size);
    tTrack_CovMatrix.reserve(Const::NCovMatrixComponents * size);
#if E2V_TPC_EXTRA
    tTrack_TPC_DCAxy.reserve(size);
    tTrack_TPC_DCAz.reserve(size);
    tTrack_TPC_NCrossedRows.reserve(size);
    tTrack_TPC_NClusters.reserve(size);
    tTrack_TPC_NClustersLC.reserve(size);
    tTrack_TPC_NClustersFound.reserve(size);
    tTrack_TPC_NClustersShared.reserve(size);
    tTrack_TPC_Chi2.reserve(size);
    tTrack_TPC_Chi2Constrained.reserve(size);
    tTrack_TPC_Chi2TCVG.reserve(size);
    if (fIsMC) tTrack_TPC_SignalTunedOnData.reserve(size);
    tTrack_TPC_SignalSigma.reserve(size);
    tTrack_TPC_SignalCorrected.reserve(size);
    tTrack_TPC_ESignalPion.reserve(size);
    tTrack_TPC_ESigmaPion.reserve(size);
    tTrack_TPC_ESignalKaon.reserve(size);
    tTrack_TPC_ESigmaKaon.reserve(size);
    tTrack_TPC_ESignalProton.reserve(size);
    tTrack_TPC_ESigmaProton.reserve(size);
    tTrack_TPC_SignalN.reserve(size);
    tTrack_TPC_PointsFirst.reserve(size);
    tTrack_TPC_PointsIndexMax.reserve(size);
    tTrack_TPC_PointsLast.reserve(size);
    tTrack_TPC_PointsMaxDens.reserve(size);
    tTrack_TPC_FirstRow.reserve(size);
#endif
    if (fIsMC) tTrack_McEntry.reserve(size);
}

// Clear the branches of the reconstructed tracks.
void AliAnalysisTaskEsd2Vector::ClearBranches_Tracks() {
    tTrack_EsdEntry.clear();
    tTrack_X.clear();
    tTrack_Y.clear();
    tTrack_Z.clear();
    tTrack_Px.clear();
    tTrack_Py.clear();
    tTrack_Pz.clear();
    tTrack_Charge.clear();
    tTrack_PreDCAxy.clear();
    tTrack_PreDCAz.clear();
    tTrack_TPC_Signal.clear();
    tTrack_NSigmaPion.clear();
    tTrack_NSigmaKaon.clear();
    tTrack_NSigmaProton.clear();
    tTrack_CovMatrix.clear();
#if E2V_TPC_EXTRA
    tTrack_TPC_DCAxy.clear();
    tTrack_TPC_DCAz.clear();
    tTrack_TPC_NCrossedRows.clear();
    tTrack_TPC_NClusters.clear();
    tTrack_TPC_NClustersLC.clear();
    tTrack_TPC_NClustersFound.clear();
    tTrack_TPC_NClustersShared.clear();
    tTrack_TPC_Chi2.clear();
    tTrack_TPC_Chi2Constrained.clear();
    tTrack_TPC_Chi2TCVG.clear();
    if (fIsMC) tTrack_TPC_SignalTunedOnData.clear();
    tTrack_TPC_SignalSigma.clear();
    tTrack_TPC_SignalCorrected.clear();
    tTrack_TPC_ESignalPion.clear();
    tTrack_TPC_ESigmaPion.clear();
    tTrack_TPC_ESignalKaon.clear();
    tTrack_TPC_ESigmaKaon.clear();
    tTrack_TPC_ESignalProton.clear();
    tTrack_TPC_ESigmaProton.clear();
    tTrack_TPC_SignalN.clear();
    tTrack_TPC_PointsFirst.clear();
    tTrack_TPC_PointsIndexMax.clear();
    tTrack_TPC_PointsLast.clear();
    tTrack_TPC_PointsMaxDens.clear();
    tTrack_TPC_FirstRow.clear();
#endif
    if (fIsMC) tTrack_McEntry.clear();
}

// # Lambdas # //

void AliAnalysisTaskEsd2Vector::ProcessLambdas() {
    const int n_v0s = fESD->GetNumberOfV0s();
    for (int v0_entry = 0; v0_entry < n_v0s; ++v0_entry) {
        auto* v0 = fESD->GetV0(v0_entry);
        if (v0 == nullptr) continue;

        // get pid //
        // -- negative daughter
        auto* neg_track = fESD->GetTrack(v0->GetNindex());
        float neg_n_sigma_proton = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, neg_track, AliPID::kProton);
        float neg_n_sigma_kaon = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, neg_track, AliPID::kKaon);
        float neg_n_sigma_pion = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, neg_track, AliPID::kPion);
        // -- positive daughter
        auto* pos_track = fESD->GetTrack(v0->GetPindex());
        float pos_n_sigma_proton = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, pos_track, AliPID::kProton);
        float pos_n_sigma_kaon = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, pos_track, AliPID::kKaon);
        float pos_n_sigma_pion = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, pos_track, AliPID::kPion);

        // (1) cut: pid
        bool neg_could_be_proton = std::abs(neg_n_sigma_proton) < 3.;
        bool neg_could_be_pion = std::abs(neg_n_sigma_pion) < 3.;
        bool pos_could_be_proton = std::abs(pos_n_sigma_proton) < 3.;
        bool pos_could_be_pion = std::abs(pos_n_sigma_pion) < 3.;
        bool could_be_lambda = pos_could_be_proton && neg_could_be_pion;
        bool could_be_anti_lambda = neg_could_be_proton && pos_could_be_pion;
        if (!could_be_anti_lambda && !could_be_lambda) continue;

        // get kinematics //
        // -- negative daughter
        double neg_px, neg_py, neg_pz;
        v0->GetNPxPyPz(neg_px, neg_py, neg_pz);
        TLorentzVector lv_antiproton(neg_px, neg_py, neg_pz,
                                     std::sqrt(neg_px * neg_px + neg_py * neg_py + neg_pz * neg_pz + Const::PdgMass_Proton * Const::PdgMass_Proton));
        TLorentzVector lv_piminus(neg_px, neg_py, neg_pz,
                                  std::sqrt(neg_px * neg_px + neg_py * neg_py + neg_pz * neg_pz + Const::PdgMass_Pion * Const::PdgMass_Pion));
        // -- positive daughter
        double pos_px, pos_py, pos_pz;
        v0->GetPPxPyPz(pos_px, pos_py, pos_pz);
        TLorentzVector lv_proton(pos_px, pos_py, pos_pz,
                                 std::sqrt(pos_px * pos_px + pos_py * pos_py + pos_pz * pos_pz + Const::PdgMass_Proton * Const::PdgMass_Proton));
        TLorentzVector lv_piplus(pos_px, pos_py, pos_pz,
                                 std::sqrt(pos_px * pos_px + pos_py * pos_py + pos_pz * pos_pz + Const::PdgMass_Pion * Const::PdgMass_Pion));
        // -- lambda
        TLorentzVector lv_antilambda = lv_antiproton + lv_piplus;
        TLorentzVector lv_lambda = lv_proton + lv_piminus;

        double delta_m_antilambda = std::abs(lv_antilambda.M() - Const::PdgMass_Lambda);
        double delta_m_lambda = std::abs(lv_lambda.M() - Const::PdgMass_Lambda);

        // (2) cut: mass
        bool valid_delta_m_antilambda = could_be_anti_lambda && delta_m_antilambda < 0.01;
        bool valid_delta_m_lambda = could_be_lambda && delta_m_lambda < 0.01;
        if (!valid_delta_m_antilambda && !valid_delta_m_lambda) continue;

        // DEBUG
        AliInfoF("v0_entry=%i, pos_id=%i, neg_id=%i, mass=%f", v0_entry, v0->GetPindex(), v0->GetNindex(),
                 delta_m_antilambda < delta_m_lambda ? lv_antilambda.M() : lv_lambda.M());

        // geometry //
        // -- negative daughter
        float neg_dca[2]{};
        float neg_cov_dca[3]{};
        neg_track->GetImpactParameters(neg_dca, neg_cov_dca);
        // -- negative daughter
        float pos_dca[2]{};
        float pos_cov_dca[3]{};
        pos_track->GetImpactParameters(pos_dca, pos_cov_dca);

        // fill branches //
        // -- (anti)lambda
        tLambda_EsdEntry.emplace_back(static_cast<unsigned int>(v0_entry));
        tLambda_Decay_X.emplace_back(static_cast<float>(v0->Xv()));
        tLambda_Decay_Y.emplace_back(static_cast<float>(v0->Yv()));
        tLambda_Decay_Z.emplace_back(static_cast<float>(v0->Zv()));
        tLambda_Px.emplace_back(static_cast<float>(v0->Px()));
        tLambda_Py.emplace_back(static_cast<float>(v0->Py()));
        tLambda_Pz.emplace_back(static_cast<float>(v0->Pz()));
        tLambda_DcaV0Daughters.emplace_back(static_cast<float>(v0->GetDcaV0Daughters()));
        // -- negative daughter
        tLambda_Neg_EsdEntry.emplace_back(static_cast<unsigned int>(v0->GetNindex()));
        tLambda_Neg_PCAwrtV0_Px.emplace_back(static_cast<float>(neg_px));
        tLambda_Neg_PCAwrtV0_Py.emplace_back(static_cast<float>(neg_py));
        tLambda_Neg_PCAwrtV0_Pz.emplace_back(static_cast<float>(neg_pz));
        tLambda_Neg_PreDCAxy.emplace_back(neg_dca[0]);
        tLambda_Neg_PreDCAz.emplace_back(neg_dca[1]);
        tLambda_Neg_NSigmaProton.emplace_back(neg_n_sigma_proton);
        tLambda_Neg_NSigmaKaon.emplace_back(neg_n_sigma_kaon);
        tLambda_Neg_NSigmaPion.emplace_back(neg_n_sigma_pion);
        // -- positive daughter
        tLambda_Pos_EsdEntry.emplace_back(static_cast<unsigned int>(v0->GetPindex()));
        tLambda_Pos_PCAwrtV0_Px.emplace_back(static_cast<float>(pos_px));
        tLambda_Pos_PCAwrtV0_Py.emplace_back(static_cast<float>(pos_py));
        tLambda_Pos_PCAwrtV0_Pz.emplace_back(static_cast<float>(pos_pz));
        tLambda_Pos_PreDCAxy.emplace_back(pos_dca[0]);
        tLambda_Pos_PreDCAz.emplace_back(pos_dca[1]);
        tLambda_Pos_NSigmaProton.emplace_back(pos_n_sigma_proton);
        tLambda_Pos_NSigmaKaon.emplace_back(pos_n_sigma_kaon);
        tLambda_Pos_NSigmaPion.emplace_back(pos_n_sigma_pion);
        // -- mc link
        if (fIsMC) {
            tLambda_Neg_McEntry.emplace_back(std::abs(neg_track->GetLabel()));
            tLambda_Pos_McEntry.emplace_back(std::abs(pos_track->GetLabel()));
        }
    }
}

void AliAnalysisTaskEsd2Vector::ReserveBranches_Lambdas(std::size_t size) {
    tLambda_EsdEntry.reserve(size);
    tLambda_Decay_X.reserve(size);
    tLambda_Decay_Y.reserve(size);
    tLambda_Decay_Z.reserve(size);
    tLambda_Px.reserve(size);
    tLambda_Py.reserve(size);
    tLambda_Pz.reserve(size);
    tLambda_DcaV0Daughters.reserve(size);
    // -- negative daughter
    tLambda_Neg_EsdEntry.reserve(size);
    tLambda_Neg_PCAwrtV0_Px.reserve(size);
    tLambda_Neg_PCAwrtV0_Py.reserve(size);
    tLambda_Neg_PCAwrtV0_Pz.reserve(size);
    tLambda_Neg_PreDCAxy.reserve(size);
    tLambda_Neg_PreDCAz.reserve(size);
    tLambda_Neg_NSigmaProton.reserve(size);
    tLambda_Neg_NSigmaKaon.reserve(size);
    tLambda_Neg_NSigmaPion.reserve(size);
    // -- positive daughter
    tLambda_Pos_EsdEntry.reserve(size);
    tLambda_Pos_PCAwrtV0_Px.reserve(size);
    tLambda_Pos_PCAwrtV0_Py.reserve(size);
    tLambda_Pos_PCAwrtV0_Pz.reserve(size);
    tLambda_Pos_PreDCAxy.reserve(size);
    tLambda_Pos_PreDCAz.reserve(size);
    tLambda_Pos_NSigmaProton.reserve(size);
    tLambda_Pos_NSigmaKaon.reserve(size);
    tLambda_Pos_NSigmaPion.reserve(size);
    // -- mc link
    if (fIsMC) {
        tLambda_Neg_McEntry.reserve(size);
        tLambda_Pos_McEntry.reserve(size);
    }
}

void AliAnalysisTaskEsd2Vector::ClearBranches_Lambdas() {
    tLambda_EsdEntry.clear();
    tLambda_Decay_X.clear();
    tLambda_Decay_Y.clear();
    tLambda_Decay_Z.clear();
    tLambda_Px.clear();
    tLambda_Py.clear();
    tLambda_Pz.clear();
    tLambda_DcaV0Daughters.clear();
    // -- negative daughter
    tLambda_Neg_EsdEntry.clear();
    tLambda_Neg_PCAwrtV0_Px.clear();
    tLambda_Neg_PCAwrtV0_Py.clear();
    tLambda_Neg_PCAwrtV0_Pz.clear();
    tLambda_Neg_PreDCAxy.clear();
    tLambda_Neg_PreDCAz.clear();
    tLambda_Neg_NSigmaProton.clear();
    tLambda_Neg_NSigmaKaon.clear();
    tLambda_Neg_NSigmaPion.clear();
    // -- positive daughter
    tLambda_Pos_EsdEntry.clear();
    tLambda_Pos_PCAwrtV0_Px.clear();
    tLambda_Pos_PCAwrtV0_Py.clear();
    tLambda_Pos_PCAwrtV0_Pz.clear();
    tLambda_Pos_PreDCAxy.clear();
    tLambda_Pos_PreDCAz.clear();
    tLambda_Pos_NSigmaProton.clear();
    tLambda_Pos_NSigmaKaon.clear();
    tLambda_Pos_NSigmaPion.clear();
    // -- mc link
    if (fIsMC) {
        tLambda_Neg_McEntry.clear();
        tLambda_Pos_McEntry.clear();
    }
}

// # Injected Reactions # //

// Store the in-memory values into the tree branches.
void AliAnalysisTaskEsd2Vector::ProcessInjectedReactions() {
    ReserveBranches_Injected();
    for (int r = 0; r < Const::NReactionsPerEvent; ++r) {
        tInjected_ReactionID.emplace_back(fEvVec_ReactionID[fEventNumberInFile][r]);
        tInjected_Px.emplace_back(fEvVec_Sexaquark_Px[fEventNumberInFile][r]);
        tInjected_Py.emplace_back(fEvVec_Sexaquark_Py[fEventNumberInFile][r]);
        tInjected_Pz.emplace_back(fEvVec_Sexaquark_Pz[fEventNumberInFile][r]);
        tInjected_Nucleon_Px.emplace_back(fEvVec_Nucleon_Px[fEventNumberInFile][r]);
        tInjected_Nucleon_Py.emplace_back(fEvVec_Nucleon_Py[fEventNumberInFile][r]);
        tInjected_Nucleon_Pz.emplace_back(fEvVec_Nucleon_Pz[fEventNumberInFile][r]);
    }
}

// Copy to working directory the respective `sim.log` that corresponds to the `RunNumber+DirNumber` that's being analyzed.
void AliAnalysisTaskEsd2Vector::BringSignalLogs() {

    TGrid* alien = nullptr;
    if (gGrid == nullptr) {
        alien = TGrid::Connect("alien://");
        if (alien == nullptr) return;
    }

    TString AliEn_Dir = fAliEnPath(0, fAliEnPath.Last('/'));

    TString orig_path = TString::Format("%s/sim.log", AliEn_Dir.Data());
    AliInfoF("Copying file %s ...", orig_path.Data());

    // assuming path ends with format `.../LHC23l1a3/A1.73/297595/001/sim.log` //
    auto AliEn_DirNumber = static_cast<int>(fDirNumber);

    TObjArray* tokens = fAliEnPath.Tokenize("/");
    auto AliEn_RunNumber = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries() - 3)))->GetString().Atoi();
    TString AliEn_SimSubSet = (dynamic_cast<TObjString*>(tokens->At(tokens->GetEntries() - 4)))->GetString();
    delete tokens;

    fSignalLog_NewBasename = TString::Format("sim_%s_%i_%03i.log", AliEn_SimSubSet.Data(), AliEn_RunNumber, AliEn_DirNumber);

    if (AliEn_Dir.BeginsWith("alien://")) {
        gSystem->Exec(TString::Format("alien.py cp %s file://./%s", orig_path.Data(), fSignalLog_NewBasename.Data()));
    } else {
        gSystem->Exec(TString::Format("cp %s ./%s", orig_path.Data(), fSignalLog_NewBasename.Data()));
    }

    TString new_path = TString::Format("%s/%s", gSystem->pwd(), fSignalLog_NewBasename.Data());
    AliInfoF("Signal log file ready at %s ...", new_path.Data());
}

// Read the anti-sexaquark and struck nucleon kinematics for each injected reaction
// from the `sim.log` file that corresponds to an entire dir number into memory.
bool AliAnalysisTaskEsd2Vector::ReadSignalLogs() {

    TString new_path = TString::Format("%s/%s", gSystem->pwd(), fSignalLog_NewBasename.Data());
    AliInfoF("Opening file %s ...", new_path.Data());

    std::ifstream SimLogFile(new_path);
    if (!SimLogFile.is_open()) {
        AliWarningF("Unable to open file %s", new_path.Data());
        return false;
    }

    int event_n = -1;
    int react_id = 0;

    std::string line;
    while (std::getline(SimLogFile, line)) {

        // a new event has appeared //

        if (line.rfind(Const::SimLog_EventHeader, 0) == 0) {
            ++event_n;
            // std::cout << "Reading Event " << event_n << '\n'; // DEBUG
            react_id = 0;
            continue;
        }

        if (event_n >= 0) {
            if (line.rfind(Const::SimLog_ReactionMarker, 0) == 0) {
                std::string data_part = line.substr(Const::SimLog_ReactionMarker.length() + 1);
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
    for (int ev_print = 0; ev_print < Const::NEventsInDedicatedMC; ++ev_print) {
        for (int r_print = 0; r_print < Const::NReactionsPerEvent; ++r_print) {
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

// Reserve injected reactions-related vectors' memory allocation.
void AliAnalysisTaskEsd2Vector::ReserveBranches_Injected() {
    tInjected_ReactionID.reserve(Const::NReactionsPerEvent);
    tInjected_Px.reserve(Const::NReactionsPerEvent);
    tInjected_Py.reserve(Const::NReactionsPerEvent);
    tInjected_Pz.reserve(Const::NReactionsPerEvent);
    tInjected_Nucleon_Px.reserve(Const::NReactionsPerEvent);
    tInjected_Nucleon_Py.reserve(Const::NReactionsPerEvent);
    tInjected_Nucleon_Pz.reserve(Const::NReactionsPerEvent);
}

// Clear the branches of the injected reactions.
void AliAnalysisTaskEsd2Vector::ClearBranches_Injected() {
    tInjected_ReactionID.clear();
    tInjected_Px.clear();
    tInjected_Py.clear();
    tInjected_Pz.clear();
    tInjected_Nucleon_Px.clear();
    tInjected_Nucleon_Py.clear();
    tInjected_Nucleon_Pz.clear();
}
