#include "AliTaskEsd2Vector.h"

#include <fstream>
#include <sstream>
#include <string>

#include <TChain.h>
#include <TFile.h>
#include <TGrid.h>
#include <TList.h>
#include <TObjString.h>
#include <TSystem.h>

#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleWriteOptions.hxx>
#include <ROOT/RNTupleWriter.hxx>

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

#include "Constants.hpp"
#include "E2R_Cuts.hpp"

ClassImp(AliTaskEsd2Vector);

// Constructor, called locally.
AliTaskEsd2Vector::AliTaskEsd2Vector(const char *name)
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
      fOutput_AltFile{nullptr},
      fWriter{},
      fEvent{},
      fInjectedSexa{},
      fMcParticle{},
      fTrack{},
      fLambda{} {
    DefineInput(0, TChain::Class());
    DefineOutput(1, TList::Class());  // fOutputList
}

// Empty I/O constructor. Non-persistent members are initialized to their default values from here.
AliTaskEsd2Vector::AliTaskEsd2Vector() : AliTaskEsd2Vector{""} {}

// Destructor.
// NOTE: if `TList::SetOwner(true)` was called, the TList destructor should delete all objects added to it.
AliTaskEsd2Vector::~AliTaskEsd2Vector() {
    delete fOutputList;
    fWriter.reset();
    if (fOutput_AltFile != nullptr) {
        fOutput_AltFile->Close();
        delete fOutput_AltFile;
    }
}

// Initialize analysis task. Needs to be called within an `AddTaskEsd2Vector.C` macro.
void AliTaskEsd2Vector::Initialize(bool is_mc, bool is_signal_mc) {
    fIsMC = is_mc;
    fIsSignalMC = is_signal_mc;
    // print settings //
    AliInfo("Initializing...");
    AliInfo("Settings:");
    AliInfo("========");
    AliInfoF(">> IsMC       = %i", fIsMC);
    AliInfoF(">> IsSignalMC = %i", fIsSignalMC);
}

void AliTaskEsd2Vector::FinishTaskOutput() {
    fWriter.reset();
    if (fOutput_AltFile) {
        fOutput_AltFile->Write();
        fOutput_AltFile->Close();
        delete fOutput_AltFile;
        fOutput_AltFile = nullptr;
    }
}

// # Executed at Runtime # //

// Create output objects, called once at RUNTIME ~ execution on Grid.
void AliTaskEsd2Vector::UserCreateOutputObjects() {

    auto *man = AliAnalysisManager::GetAnalysisManager();
    if (man == nullptr) AliFatal("AliAnalysisManager couldn't be found.");

    auto *inputHandler = dynamic_cast<AliESDInputHandler *>(man->GetInputEventHandler());
    if (inputHandler == nullptr) AliFatal("AliESDInputHandler couldn't be found.");

    fPIDResponse = inputHandler->GetPIDResponse();

    // Prepare output file //

    fOutput_AltFile = TFile::Open("EventsRNT.root", "RECREATE");
    if (!fOutput_AltFile || fOutput_AltFile->IsZombie()) AliFatal("Cannot open EventsRNT.root");

    // Prepare output RNTuple //

    auto rnt_model = ROOT::RNTupleModel::Create();

    CreateEventsBranches(rnt_model.get());
    if (fIsMC) {
        CreateMCBranches(rnt_model.get());
        if (fIsSignalMC) CreateInjectedBranches(rnt_model.get());
    }
    CreateTracksBranches(rnt_model.get());
    CreateLambdasBranches(rnt_model.get());

    ROOT::RNTupleWriteOptions rnt_opts;
    rnt_opts.SetCompression(ROOT::RCompressionSetting::EDefaults::kUseSmallest);

    fWriter = ROOT::RNTupleWriter::Append(std::move(rnt_model), "Events", *fOutput_AltFile, rnt_opts);

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

    // Post data //

    PostData(1, fOutputList);
}

// User implementation of `Notify()`. Needed for reading the AliEn path.
// This function is loaded during `AliAnalysisManager::Notify()`.
// It's called after `UserCreateOutputObjects()`, for each new file, and before the first `UserExec()`.
bool AliTaskEsd2Vector::UserNotify() {

    auto *man = AliAnalysisManager::GetAnalysisManager();
    if (man == nullptr) AliFatal("Analysis Manager not found");
    auto *man_tree = man->GetTree();
    if (man_tree == nullptr) AliFatal("Analysis Manager Tree not found");
    auto *man_file = man_tree->GetCurrentFile();
    if (man_file == nullptr) AliFatal("Analysis Manager File not found");

    // get AliEn path and tokenize it //

    fAliEnPath = man_file->GetName();
    if (fAliEnPath == "") AliWarning("fAliEnPath couldn't be found.");
    AliInfoF("fAliEnPath = %s", fAliEnPath.Data());

    TObjArray *tokens = fAliEnPath.Tokenize("/");
    if (fIsMC) {
        // NOTES: path of MC (signal/gen.purp.) ends with format `.../297595/001/AliESDs.root`
        *fEvent.RunNumber = (dynamic_cast<TObjString *>(tokens->At(tokens->GetEntries() - 3)))->GetString().Atoi();
        *fEvent.DirNumber = (dynamic_cast<TObjString *>(tokens->At(tokens->GetEntries() - 2)))->GetString().Atoi();
        AliInfoF("Run Number = %04i", static_cast<int>(*fEvent.RunNumber));  // = 1
        AliInfoF("Dir Number = %04i", static_cast<int>(*fEvent.DirNumber));  // = 1
        if (fIsSignalMC) {
            BringSignalLogs();
            ReadSignalLogs();
        }
    } else {
        // NOTE: path of real data ends with format `.../LHC15o/000245232/pass2/15000245232039.914/AliESDs.root`
        *fEvent.RunNumber = (dynamic_cast<TObjString *>(tokens->At(tokens->GetEntries() - 4)))->GetString().Atoi();
        AliInfoF("Run Number = %i", *fEvent.RunNumber);
        auto aux_dir_nr = (dynamic_cast<TObjString *>(tokens->At(tokens->GetEntries() - 2)))->GetString();
        aux_dir_nr = TString(aux_dir_nr(2 + 3 + 6, 10));        // = "039.914"
        *fEvent.DirNumber = TString(aux_dir_nr(0, 3)).Atoi();   // = 39
        *fEvent.DirNumberB = TString(aux_dir_nr(4, 5)).Atoi();  // = 914
        AliInfoF("Dir Number = %i", *fEvent.DirNumber);
        AliInfoF("Dir Number B = %i", *fEvent.DirNumberB);
    }
    delete tokens;  // NOTE: because ROOT

    // Adapt event cuts -- depending on run number //

    // Reference: https://twiki.cern.ch/twiki/bin/viewauth/ALICE/AliDPGRunList18r1
    if (!fIsMC && (*fEvent.RunNumber == 296749 || *fEvent.RunNumber == 296750 || *fEvent.RunNumber == 296849 || *fEvent.RunNumber == 296890 ||
                   *fEvent.RunNumber == 297029 || *fEvent.RunNumber == 297194 || *fEvent.RunNumber == 297219 || *fEvent.RunNumber == 297481)) {
        fEventCuts.UseTimeRangeCut();
        fEventCuts.OverrideAutomaticTriggerSelection(AliVEvent::kINT7);
    }

    return true;
}

// Main function, called per each event at RUNTIME ~ execution on Grid.
void AliTaskEsd2Vector::UserExec(Option_t *option) {

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

    fWriter->Fill();
    if (fIsMC) {
        if (fIsSignalMC) ClearBranches_Injected();
        ClearBranches_MC();
    }
    ClearBranches_Tracks();
    ClearBranches_Lambdas();

    // post data //

    PostData(1, fOutputList);
}

// # Events # //

bool AliTaskEsd2Vector::ProcessEvent() {

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

    fESD = dynamic_cast<AliESDEvent *>(InputEvent());
    if (fESD == nullptr) AliFatal("AliESDEvent couldn't be found.");

    // Assign metadata branches (1) //

    // NOTE: `RunNumber`, `DirNumber` and `DirNumberB` are set in `UserNotify()`
    *fEvent.EventNumber = fESD->GetEventNumberInFile();
    fPrimaryVertex = fESD->GetPrimaryVertex();

    // Apply selection //

    if (!PassesEventSelection()) return false;

    // Assign more branches (2) //

    auto *MultSelection = dynamic_cast<AliMultSelection *>(fESD->FindListObject("MultSelection"));
    if (MultSelection == nullptr) AliFatal("AliMultSelection couldn't be found.");
    *fEvent.Centrality = MultSelection->GetMultiplicityPercentile("V0M");
    *fEvent.MagneticField = static_cast<float>(fESD->GetMagneticField());

    // Fill QA hist //

    fHist_Centrality->Fill(*fEvent.Centrality);
    if ((fInputHandler->IsEventSelected() & AliVEvent::kINT7) != 0U) fHist_CentralityINT7->Fill(*fEvent.Centrality);

    // Assign rest of branches (3) //

    *fEvent.PV_X = static_cast<float>(fPrimaryVertex->GetX());
    *fEvent.PV_Y = static_cast<float>(fPrimaryVertex->GetY());
    *fEvent.PV_Z = static_cast<float>(fPrimaryVertex->GetZ());

    *fEvent.PV_NContributors = fPrimaryVertex->GetNContributors();
    *fEvent.PV_Dispersion = static_cast<float>(fPrimaryVertex->GetDispersion());

    double PV_CovMatrix[6]{};
    fPrimaryVertex->GetCovarianceMatrix(PV_CovMatrix);
    for (int i = 0; i < 6; ++i) (*fEvent.PV_CovMatrix)[i] = static_cast<float>(PV_CovMatrix[i]);

    const auto *PrimaryVertex_SPD = fESD->GetPrimaryVertexSPD();
    *fEvent.SPD_PV_Z = static_cast<float>(PrimaryVertex_SPD->GetZ());

    double PV_SPD_CovMatrix[6]{};
    PrimaryVertex_SPD->GetCovarianceMatrix(PV_SPD_CovMatrix);
    *fEvent.SPD_PV_ZErr = static_cast<float>(PV_SPD_CovMatrix[5]);

    *fEvent.NTracks = fESD->GetNumberOfTracks();
    *fEvent.NTPCClusters = fESD->GetNumberOfTPCClusters();
    *fEvent.IsMB = ((fInputHandler->IsEventSelected() & AliVEvent::kINT7) != 0U);
    *fEvent.IsHighMultV0 = ((fInputHandler->IsEventSelected() & AliVEvent::kHighMultV0) != 0U);
    *fEvent.IsHighMultSPD = ((fInputHandler->IsEventSelected() & AliVEvent::kHighMultSPD) != 0U);
    *fEvent.IsCentral = ((fInputHandler->IsEventSelected() & AliVEvent::kCentral) != 0U);
    *fEvent.IsSemiCentral = ((fInputHandler->IsEventSelected() & AliVEvent::kSemiCentral) != 0U);

    // Assign MC branches (4) //

    if (fIsMC) {
        fMC_PrimaryVertex = fMC->GetPrimaryVertex();
        *fEvent.MC_PV_X = static_cast<float>(fMC_PrimaryVertex->GetX());
        *fEvent.MC_PV_Y = static_cast<float>(fMC_PrimaryVertex->GetY());
        *fEvent.MC_PV_Z = static_cast<float>(fMC_PrimaryVertex->GetZ());
        *fEvent.MC_IsGenPileup = AliAnalysisUtils::IsPileupInGeneratedEvent(fMC, "Hijing");
        *fEvent.MC_IsSBCPileup = AliAnalysisUtils::IsSameBunchPileupInGeneratedEvent(fMC, "Hijing");
    }

    return true;
}

// Apply event selection.
bool AliTaskEsd2Vector::PassesEventSelection() {

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
    if (std::abs(fPrimaryVertex->GetZ()) > E2R::Cuts::AbsMax_PV_Z) return false;
    fHist_Events_Bookkeeping->Fill(6.);

    return true;
}

// # Trees # //

// Add branches to `fOutputTree`.
void AliTaskEsd2Vector::CreateEventsBranches(ROOT::RNTupleModel *model) {
    fEvent.RunNumber = model->MakeField<unsigned int>("RunNumber");
    fEvent.DirNumber = model->MakeField<unsigned int>("DirNumber");
    if (!fIsMC) fEvent.DirNumberB = model->MakeField<unsigned int>("DirNumberB");
    fEvent.EventNumber = model->MakeField<unsigned int>("EventNumber");
    fEvent.Centrality = model->MakeField<float>("Centrality");
    fEvent.MagneticField = model->MakeField<float>("MagneticField");
    if (fIsMC) {
        fEvent.MC_PV_X = model->MakeField<float>("MC_PV_X");
        fEvent.MC_PV_Y = model->MakeField<float>("MC_PV_Y");
        fEvent.MC_PV_Z = model->MakeField<float>("MC_PV_Z");
        fEvent.MC_IsGenPileup = model->MakeField<bool>("MC_IsGenPileup");
        fEvent.MC_IsSBCPileup = model->MakeField<bool>("MC_IsSBCPileup");
    }
    fEvent.PV_NContributors = model->MakeField<int>("PV_NContributors");
    fEvent.PV_Dispersion = model->MakeField<float>("PV_Dispersion");
    fEvent.PV_X = model->MakeField<float>("PV_X");
    fEvent.PV_Y = model->MakeField<float>("PV_Y");
    fEvent.PV_Z = model->MakeField<float>("PV_Z");
    fEvent.PV_CovMatrix = model->MakeField<std::array<float, 6>>("PV_CovMatrix");
    fEvent.SPD_PV_Z = model->MakeField<float>("SPD_PV_Z");
    fEvent.SPD_PV_ZErr = model->MakeField<float>("SPD_PV_ZErr");
    fEvent.NTracks = model->MakeField<int>("NTracks");
    fEvent.NTPCClusters = model->MakeField<int>("NTPCClusters");
    fEvent.IsMB = model->MakeField<bool>("IsMB");
    fEvent.IsHighMultV0 = model->MakeField<bool>("IsHighMultV0");
    fEvent.IsHighMultSPD = model->MakeField<bool>("IsHighMultSPD");
    fEvent.IsCentral = model->MakeField<bool>("IsCentral");
    fEvent.IsSemiCentral = model->MakeField<bool>("IsSemiCentral");
}

// Add branches to `fTree_Injected`.
void AliTaskEsd2Vector::CreateInjectedBranches(ROOT::RNTupleModel *model) {
    fInjectedSexa.ReactionID = model->MakeField<std::vector<int>>("ReactionID");
    fInjectedSexa.Px = model->MakeField<std::vector<float>>("Sexa_Px");
    fInjectedSexa.Py = model->MakeField<std::vector<float>>("Sexa_Py");
    fInjectedSexa.Pz = model->MakeField<std::vector<float>>("Sexa_Pz");
    fInjectedSexa.Nucleon_Px = model->MakeField<std::vector<float>>("Nucleon_Px");
    fInjectedSexa.Nucleon_Py = model->MakeField<std::vector<float>>("Nucleon_Py");
    fInjectedSexa.Nucleon_Pz = model->MakeField<std::vector<float>>("Nucleon_Pz");
}

// Add branches to the MC tree.
void AliTaskEsd2Vector::CreateMCBranches(ROOT::RNTupleModel *model) {
    fMcParticle.PdgCode = model->MakeField<std::vector<int>>("MC_PdgCode");
    fMcParticle.Charge = model->MakeField<std::vector<char>>("MC_Charge");
    fMcParticle.Mother_McEntry = model->MakeField<std::vector<int>>("MC_Mother_McEntry");
    fMcParticle.N_Daughters = model->MakeField<std::vector<unsigned int>>("MC_N_Daughters");
    fMcParticle.FirstDau_McEntry = model->MakeField<std::vector<int>>("MC_FirstDau_McEntry");
    fMcParticle.LastDau_McEntry = model->MakeField<std::vector<int>>("MC_LastDau_McEntry");
    fMcParticle.Origin_X = model->MakeField<std::vector<float>>("MC_Origin_X");
    fMcParticle.Origin_Y = model->MakeField<std::vector<float>>("MC_Origin_Y");
    fMcParticle.Origin_Z = model->MakeField<std::vector<float>>("MC_Origin_Z");
    fMcParticle.Px = model->MakeField<std::vector<float>>("MC_Px");
    fMcParticle.Py = model->MakeField<std::vector<float>>("MC_Py");
    fMcParticle.Pz = model->MakeField<std::vector<float>>("MC_Pz");
    fMcParticle.E = model->MakeField<std::vector<float>>("MC_E");
    fMcParticle.Status = model->MakeField<std::vector<unsigned int>>("MC_Status");
    fMcParticle.Generator = model->MakeField<std::vector<char>>("MC_Generator");
    fMcParticle.IsPhysPrimary = model->MakeField<std::vector<char>>("MC_IsPhysPrimary");
    fMcParticle.IsSecFromMat = model->MakeField<std::vector<char>>("MC_IsSecFromMat");
    fMcParticle.IsSecFromWeak = model->MakeField<std::vector<char>>("MC_IsSecFromWeak");
#if E2V_MC_EXTRA
    fMcParticle.IsOOBPileup = model->MakeField<std::vector<char>>("MC_IsOOBPileup");
#endif
}

// Add branches to the tracks tree.
void AliTaskEsd2Vector::CreateTracksBranches(ROOT::RNTupleModel *model) {
    fTrack.EsdEntry = model->MakeField<std::vector<unsigned int>>("Track_EsdEntry");
    fTrack.X = model->MakeField<std::vector<float>>("Track_X");
    fTrack.Y = model->MakeField<std::vector<float>>("Track_Y");
    fTrack.Z = model->MakeField<std::vector<float>>("Track_Z");
    fTrack.Px = model->MakeField<std::vector<float>>("Track_Px");
    fTrack.Py = model->MakeField<std::vector<float>>("Track_Py");
    fTrack.Pz = model->MakeField<std::vector<float>>("Track_Pz");
    fTrack.Charge = model->MakeField<std::vector<char>>("Track_Charge");
    fTrack.PreDCAxy = model->MakeField<std::vector<float>>("Track_PreDCAxy");
    fTrack.PreDCAz = model->MakeField<std::vector<float>>("Track_PreDCAz");
    fTrack.TPC_Signal = model->MakeField<std::vector<float>>("Track_TPC_Signal");
    fTrack.ITS_FirstLayer = model->MakeField<std::vector<int>>("Track_ITS_FirstLayer");
    fTrack.NSigmaPion = model->MakeField<std::vector<float>>("Track_NSigmaPion");
    fTrack.NSigmaKaon = model->MakeField<std::vector<float>>("Track_NSigmaKaon");
    fTrack.NSigmaProton = model->MakeField<std::vector<float>>("Track_NSigmaProton");
    fTrack.CovMatrix = model->MakeField<std::vector<std::array<float, 21>>>("Track_CovMatrix");
#if E2V_TPC_EXTRA
    fTrack.TPC_DCAxy = model->MakeField<std::vector<float>>("Track_TPC_DCAxy");
    fTrack.TPC_DCAz = model->MakeField<std::vector<float>>("Track_TPC_DCAz");
    fTrack.TPC_NCrossedRows = model->MakeField<std::vector<float>>("Track_TPC_NCrossedRows");
    fTrack.TPC_NClusters = model->MakeField<std::vector<unsigned int>>("Track_TPC_NClusters");
    fTrack.TPC_NClustersLC = model->MakeField<std::vector<unsigned int>>("Track_TPC_NClustersLC");
    fTrack.TPC_NClustersFound = model->MakeField<std::vector<unsigned int>>("Track_TPC_NClustersFound");
    fTrack.TPC_NClustersShared = model->MakeField<std::vector<unsigned int>>("Track_TPC_NClustersShared");
    fTrack.TPC_Chi2 = model->MakeField<std::vector<float>>("Track_TPC_Chi2");
    fTrack.TPC_Chi2Constrained = model->MakeField<std::vector<float>>("Track_TPC_Chi2Constrained");
    fTrack.TPC_Chi2TCVG = model->MakeField<std::vector<float>>("Track_TPC_Chi2TCVG");
    if (fIsMC) fTrack.TPC_SignalTunedOnData = model->MakeField<std::vector<float>>("Track_TPC_SignalTunedOnData");
    fTrack.TPC_SignalSigma = model->MakeField<std::vector<float>>("Track_TPC_SignalSigma");
    fTrack.TPC_SignalCorrected = model->MakeField<std::vector<float>>("Track_TPC_SignalCorrected");
    fTrack.TPC_ESignalPion = model->MakeField<std::vector<float>>("Track_TPC_ESignalPion");
    fTrack.TPC_ESigmaPion = model->MakeField<std::vector<float>>("Track_TPC_ESigmaPion");
    fTrack.TPC_ESignalKaon = model->MakeField<std::vector<float>>("Track_TPC_ESignalKaon");
    fTrack.TPC_ESigmaKaon = model->MakeField<std::vector<float>>("Track_TPC_ESigmaKaon");
    fTrack.TPC_ESignalProton = model->MakeField<std::vector<float>>("Track_TPC_ESignalProton");
    fTrack.TPC_ESigmaProton = model->MakeField<std::vector<float>>("Track_TPC_ESigmaProton");
    fTrack.TPC_SignalN = model->MakeField<std::vector<unsigned int>>("Track_TPC_SignalN");
    fTrack.TPC_PointsFirst = model->MakeField<std::vector<float>>("Track_TPC_PointsFirst");
    fTrack.TPC_PointsIndexMax = model->MakeField<std::vector<float>>("Track_TPC_PointsIndexMax");
    fTrack.TPC_PointsLast = model->MakeField<std::vector<float>>("Track_TPC_PointsLast");
    fTrack.TPC_PointsMaxDens = model->MakeField<std::vector<float>>("Track_TPC_PointsMaxDens");
    fTrack.TPC_FirstRow = model->MakeField<std::vector<int>>("Track_TPC_FirstRow");
#endif
    if (fIsMC) fTrack.McEntry = model->MakeField<std::vector<int>>("Track_McEntry");
}

void AliTaskEsd2Vector::CreateLambdasBranches(ROOT::RNTupleModel *model) {
    fLambda.EsdEntry = model->MakeField<std::vector<unsigned int>>("Lambda_EsdEntry");
    fLambda.Decay_X = model->MakeField<std::vector<float>>("Lambda_Decay_X");
    fLambda.Decay_Y = model->MakeField<std::vector<float>>("Lambda_Decay_Y");
    fLambda.Decay_Z = model->MakeField<std::vector<float>>("Lambda_Decay_Z");
    fLambda.Px = model->MakeField<std::vector<float>>("Lambda_Px");
    fLambda.Py = model->MakeField<std::vector<float>>("Lambda_Py");
    fLambda.Pz = model->MakeField<std::vector<float>>("Lambda_Pz");
    fLambda.DcaV0Daughters = model->MakeField<std::vector<float>>("Lambda_DcaV0Daughters");
    // -- negative daughter (anti-proton for anti-lambda, pi-minus for lambda)
    fLambda.Neg_EsdEntry = model->MakeField<std::vector<unsigned int>>("Lambda_Neg_EsdEntry");
    fLambda.Neg_PCAwrtV0_Px = model->MakeField<std::vector<float>>("Lambda_Neg_PCAwrtV0_Px");
    fLambda.Neg_PCAwrtV0_Py = model->MakeField<std::vector<float>>("Lambda_Neg_PCAwrtV0_Py");
    fLambda.Neg_PCAwrtV0_Pz = model->MakeField<std::vector<float>>("Lambda_Neg_PCAwrtV0_Pz");
    fLambda.Neg_PreDCAxy = model->MakeField<std::vector<float>>("Lambda_Neg_PreDCAxy");
    fLambda.Neg_PreDCAz = model->MakeField<std::vector<float>>("Lambda_Neg_PreDCAz");
    fLambda.Neg_NSigmaProton = model->MakeField<std::vector<float>>("Lambda_Neg_NSigmaProton");
    fLambda.Neg_NSigmaKaon = model->MakeField<std::vector<float>>("Lambda_Neg_NSigmaKaon");
    fLambda.Neg_NSigmaPion = model->MakeField<std::vector<float>>("Lambda_Neg_NSigmaPion");
    // -- positive daughter (proton for lambda, pi-plus for anti-lambda)
    fLambda.Pos_EsdEntry = model->MakeField<std::vector<unsigned int>>("Lambda_Pos_EsdEntry");
    fLambda.Pos_PCAwrtV0_Px = model->MakeField<std::vector<float>>("Lambda_Pos_PCAwrtV0_Px");
    fLambda.Pos_PCAwrtV0_Py = model->MakeField<std::vector<float>>("Lambda_Pos_PCAwrtV0_Py");
    fLambda.Pos_PCAwrtV0_Pz = model->MakeField<std::vector<float>>("Lambda_Pos_PCAwrtV0_Pz");
    fLambda.Pos_PreDCAxy = model->MakeField<std::vector<float>>("Lambda_Pos_PreDCAxy");
    fLambda.Pos_PreDCAz = model->MakeField<std::vector<float>>("Lambda_Pos_PreDCAz");
    fLambda.Pos_NSigmaProton = model->MakeField<std::vector<float>>("Lambda_Pos_NSigmaProton");
    fLambda.Pos_NSigmaKaon = model->MakeField<std::vector<float>>("Lambda_Pos_NSigmaKaon");
    fLambda.Pos_NSigmaPion = model->MakeField<std::vector<float>>("Lambda_Pos_NSigmaPion");
    if (fIsMC) {
        fLambda.Neg_McEntry = model->MakeField<std::vector<int>>("Lambda_Neg_McEntry");
        fLambda.Pos_McEntry = model->MakeField<std::vector<int>>("Lambda_Pos_McEntry");
    }
}

// # MC Generated # //

// Loop over MC particles in a single event.
void AliTaskEsd2Vector::ProcessMCParticles() {
    const int n_mc = fMC->GetNumberOfTracks();
    ReserveBranches_MC(static_cast<std::size_t>(n_mc));
    // read mc particles //
    for (int mc_entry = 0; mc_entry < n_mc; ++mc_entry) {
        auto *mcPart = dynamic_cast<AliMCParticle *>(fMC->GetTrack(mc_entry));
        if (mcPart == nullptr) continue;
        // -- fill branches
        fMcParticle.PdgCode->emplace_back(mcPart->PdgCode());
        fMcParticle.Charge->emplace_back(static_cast<int>(mcPart->Charge() / 3));
        fMcParticle.Mother_McEntry->emplace_back(mcPart->GetMother());
        fMcParticle.N_Daughters->emplace_back(static_cast<unsigned int>(mcPart->GetNDaughters()));
        fMcParticle.FirstDau_McEntry->emplace_back(mcPart->GetDaughterFirst());
        fMcParticle.LastDau_McEntry->emplace_back(mcPart->GetDaughterLast());
        fMcParticle.Origin_X->emplace_back(static_cast<float>(mcPart->Xv()));
        fMcParticle.Origin_Y->emplace_back(static_cast<float>(mcPart->Yv()));
        fMcParticle.Origin_Z->emplace_back(static_cast<float>(mcPart->Zv()));
        fMcParticle.Px->emplace_back(static_cast<float>(mcPart->Px()));
        fMcParticle.Py->emplace_back(static_cast<float>(mcPart->Py()));
        fMcParticle.Pz->emplace_back(static_cast<float>(mcPart->Pz()));
        fMcParticle.E->emplace_back(static_cast<float>(mcPart->E()));
        fMcParticle.Status->emplace_back(mcPart->MCStatusCode());
        fMcParticle.Generator->emplace_back(static_cast<char>(mcPart->GetGeneratorIndex()));
        fMcParticle.IsPhysPrimary->emplace_back(static_cast<char>(mcPart->IsPhysicalPrimary()));
        fMcParticle.IsSecFromMat->emplace_back(static_cast<char>(mcPart->IsSecondaryFromMaterial()));
        fMcParticle.IsSecFromWeak->emplace_back(static_cast<char>(mcPart->IsSecondaryFromWeakDecay()));
#if E2V_DEBUG
        if (mcPart->GetGeneratorIndex() == 2) {
            AliInfoF("mc_entry=%i,mc_pdg=%i,mc_is_physprim=%i,mc_status=%i,mother_entry=%i",  //
                     mc_entry, mcPart->PdgCode(), mcPart->IsPhysicalPrimary(), mcPart->MCStatusCode(), mcPart->GetMother());
        }
#endif
#if E2V_MC_EXTRA
        fMcParticle.IsOOBPileup->emplace_back(static_cast<char>(AliAnalysisUtils::IsParticleFromOutOfBunchPileupCollision(mc_entry, fMC)));
#endif
    }  // end of loop over MC particles
}

// Reserve MC particle's vectors' memory allocation.
void AliTaskEsd2Vector::ReserveBranches_MC(std::size_t size) {
    fMcParticle.PdgCode->reserve(size);
    fMcParticle.Charge->reserve(size);
    fMcParticle.Mother_McEntry->reserve(size);
    fMcParticle.N_Daughters->reserve(size);
    fMcParticle.FirstDau_McEntry->reserve(size);
    fMcParticle.LastDau_McEntry->reserve(size);
    fMcParticle.Origin_X->reserve(size);
    fMcParticle.Origin_Y->reserve(size);
    fMcParticle.Origin_Z->reserve(size);
    fMcParticle.Px->reserve(size);
    fMcParticle.Py->reserve(size);
    fMcParticle.Pz->reserve(size);
    fMcParticle.E->reserve(size);
    fMcParticle.Status->reserve(size);
    fMcParticle.Generator->reserve(size);
    fMcParticle.IsPhysPrimary->reserve(size);
    fMcParticle.IsSecFromMat->reserve(size);
    fMcParticle.IsSecFromWeak->reserve(size);
#if E2V_MC_EXTRA
    fMcParticle.IsOOBPileup->reserve(size);
#endif
}

// Clear MC branches.
void AliTaskEsd2Vector::ClearBranches_MC() {
    fMcParticle.PdgCode->clear();
    fMcParticle.Charge->clear();
    fMcParticle.Mother_McEntry->clear();
    fMcParticle.N_Daughters->clear();
    fMcParticle.FirstDau_McEntry->clear();
    fMcParticle.LastDau_McEntry->clear();
    fMcParticle.Origin_X->clear();
    fMcParticle.Origin_Y->clear();
    fMcParticle.Origin_Z->clear();
    fMcParticle.Px->clear();
    fMcParticle.Py->clear();
    fMcParticle.Pz->clear();
    fMcParticle.E->clear();
    fMcParticle.Status->clear();
    fMcParticle.Generator->clear();
    fMcParticle.IsPhysPrimary->clear();
    fMcParticle.IsSecFromMat->clear();
    fMcParticle.IsSecFromWeak->clear();
#if E2V_MC_EXTRA
    fMcParticle.IsOOBPileup->clear();
#endif
}

// # Reconstructed # //

// Loop over the reconstructed tracks in a single event.
void AliTaskEsd2Vector::ProcessTracks() {
    const int n_tracks = fESD->GetNumberOfTracks();
    ReserveBranches_Tracks(static_cast<std::size_t>(n_tracks));
    std::array<float, Common::NCovMatrixComponents> cov_arr{};  // auxiliary
    for (int esd_entry = 0; esd_entry < n_tracks; ++esd_entry) {
        // get track //
        auto *track = fESD->GetTrack(esd_entry);
        // track selection //
        if (!PassesTrackSelection(track)) continue;
        // get info //
        const auto *inner_param = track->GetInnerParam();  // NOTE: already protected in `PassesTrackSelection`
        if (inner_param == nullptr) continue;
        double position[3]{};
        inner_param->GetXYZ(position);
        double momentum[3]{};
        inner_param->GetPxPyPz(momentum);
        float dca[2]{};
        float dca_cov[3]{};
        track->GetImpactParameters(dca, dca_cov);
        double cov_xyz_pxpypz[Common::NCovMatrixComponents]{};
        track->GetCovarianceXYZPxPyPz(cov_xyz_pxpypz);
        // fill branches //
        fTrack.EsdEntry->emplace_back(static_cast<unsigned int>(esd_entry));
        fTrack.X->emplace_back(static_cast<float>(position[0]));
        fTrack.Y->emplace_back(static_cast<float>(position[1]));
        fTrack.Z->emplace_back(static_cast<float>(position[2]));
        fTrack.Px->emplace_back(static_cast<float>(momentum[0]));
        fTrack.Py->emplace_back(static_cast<float>(momentum[1]));
        fTrack.Pz->emplace_back(static_cast<float>(momentum[2]));
        fTrack.Charge->emplace_back(inner_param->Charge());
        // -- dca //
        fTrack.PreDCAxy->emplace_back(dca[0]);
        fTrack.PreDCAz->emplace_back(dca[1]);
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
        fTrack.TPC_Signal->emplace_back(static_cast<float>(tpc_signal));
        fTrack.ITS_FirstLayer->emplace_back(GetFirstLayerFromITS(track));
        fTrack.NSigmaPion->emplace_back(static_cast<float>(n_sigmas_pion));
        fTrack.NSigmaKaon->emplace_back(static_cast<float>(n_sigmas_kaon));
        fTrack.NSigmaProton->emplace_back(static_cast<float>(n_sigmas_proton));
        // -- cov. matrix //
        for (std::size_t idx_cov = 0; idx_cov < Common::NCovMatrixComponents; ++idx_cov) {
            cov_arr[idx_cov] = static_cast<float>(cov_xyz_pxpypz[idx_cov]);
        }
        fTrack.CovMatrix->emplace_back(cov_arr);
#if E2V_TPC_EXTRA
        // -- get tpc dca
        float tpc_dca_xy{};
        float tpc_dca_z{};
        track->GetImpactParametersTPC(tpc_dca_xy, tpc_dca_z);
        // -- fill tpc branches (1)
        fTrack.TPC_DCAxy->emplace_back(tpc_dca_xy);
        fTrack.TPC_DCAz->emplace_back(tpc_dca_z);
        fTrack.TPC_NCrossedRows->emplace_back(track->GetTPCCrossedRows());
        fTrack.TPC_NClusters->emplace_back(static_cast<unsigned int>(track->GetTPCNcls()));
        fTrack.TPC_NClustersLC->emplace_back(static_cast<unsigned int>(track->GetTPCncls()));
        fTrack.TPC_NClustersFound->emplace_back(static_cast<unsigned int>(track->GetTPCNclsF()));
        fTrack.TPC_NClustersShared->emplace_back(static_cast<unsigned int>(track->GetTPCnclsS()));
        fTrack.TPC_Chi2->emplace_back(static_cast<float>(track->GetTPCchi2()));
        fTrack.TPC_Chi2Constrained->emplace_back(static_cast<float>(track->GetConstrainedChi2TPC()));
        fTrack.TPC_Chi2TCVG->emplace_back(static_cast<float>(track->GetChi2TPCConstrainedVsGlobal(fPrimaryVertex)));
        if (fIsMC) fTrack.TPC_SignalTunedOnData->emplace_back(static_cast<float>(track->GetTPCsignalTunedOnData()));
        fTrack.TPC_SignalSigma->emplace_back(static_cast<float>(track->GetTPCsignalSigma()));
        // -- get tpc pid info
        auto bethe_kaon = static_cast<double>(fPIDResponse->GetExpectedSignal(AliPIDResponse::kTPC, track, AliPID::kKaon));
        auto e_sigma_kaon = static_cast<double>(fPIDResponse->GetExpectedSigma(AliPIDResponse::kTPC, track, AliPID::kKaon));
        auto bethe_proton = static_cast<double>(fPIDResponse->GetExpectedSignal(AliPIDResponse::kTPC, track, AliPID::kProton));
        auto e_sigma_proton = static_cast<double>(fPIDResponse->GetExpectedSigma(AliPIDResponse::kTPC, track, AliPID::kProton));
        //    -- pion
        fTrack.TPC_SignalCorrected->emplace_back(static_cast<float>(e_sigma_pion * n_sigmas_pion + bethe_pion));
        fTrack.TPC_ESignalPion->emplace_back(static_cast<float>(bethe_pion));
        fTrack.TPC_ESigmaPion->emplace_back(static_cast<float>(e_sigma_pion));
        //    -- kaon
        fTrack.TPC_ESignalKaon->emplace_back(static_cast<float>(bethe_kaon));
        fTrack.TPC_ESigmaKaon->emplace_back(static_cast<float>(e_sigma_kaon));
        //    -- proton
        fTrack.TPC_ESignalProton->emplace_back(static_cast<float>(bethe_proton));
        fTrack.TPC_ESigmaProton->emplace_back(static_cast<float>(e_sigma_proton));
        // -- fill tpc branches (2)
        fTrack.TPC_SignalN->emplace_back(static_cast<unsigned int>(track->GetTPCsignalN()));
        fTrack.TPC_PointsFirst->emplace_back(static_cast<float>(track->GetTPCPoints(0)));
        fTrack.TPC_PointsIndexMax->emplace_back(static_cast<float>(track->GetTPCPoints(1)));
        fTrack.TPC_PointsLast->emplace_back(static_cast<float>(track->GetTPCPoints(2)));
        fTrack.TPC_PointsMaxDens->emplace_back(static_cast<float>(track->GetTPCPoints(3)));
        fTrack.TPC_FirstRow->emplace_back(GetFirstRowFromTPCClusterMap(track));
#endif
        if (fIsMC) fTrack.McEntry->emplace_back(std::abs(track->GetLabel()));
    }  // end of loop over tracks
}

// Check if track passes selection and fill bookkeeping histograms.
bool AliTaskEsd2Vector::PassesTrackSelection(const AliESDtrack *track) {

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

    const auto *inner_param = track->GetInnerParam();
    if (inner_param == nullptr) return false;

    // if (std::abs(inner_param->Pz()) > Cuts::Track::AbsMax_Pz) return false;
    fHist_Tracks_Bookkeeping->Fill(4);

    double p = inner_param->P();
    if (p < E2R::Cuts::Min_P) return false;
    fHist_Tracks_Bookkeeping->Fill(5);

    if (p > E2R::Cuts::Max_P) return false;
    fHist_Tracks_Bookkeeping->Fill(6);

    if (std::abs(inner_param->Eta()) > E2R::Cuts::AbsMax_Eta) return false;
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
    if (tpc_n_clusters < E2R::Cuts::Min_TPC_NClusters) return false;
    fHist_Tracks_Bookkeeping->Fill(11);

    auto tpc_chi2_ncls = track->GetTPCchi2() / static_cast<double>(tpc_n_clusters);
    if (tpc_chi2_ncls > E2R::Cuts::Max_TPC_Chi2NClusters) return false;
    fHist_Tracks_Bookkeeping->Fill(12);

    // others //

    if (track->GetKinkIndex(0) > 0) return false;
    fHist_Tracks_Bookkeeping->Fill(13);

    return true;
}

// Reserve tracks-related vectors' memory allocation.
void AliTaskEsd2Vector::ReserveBranches_Tracks(std::size_t size) {
    fTrack.EsdEntry->reserve(size);
    fTrack.X->reserve(size);
    fTrack.Y->reserve(size);
    fTrack.Z->reserve(size);
    fTrack.Px->reserve(size);
    fTrack.Py->reserve(size);
    fTrack.Pz->reserve(size);
    fTrack.Charge->reserve(size);
    fTrack.PreDCAxy->reserve(size);
    fTrack.PreDCAz->reserve(size);
    fTrack.TPC_Signal->reserve(size);
    fTrack.NSigmaPion->reserve(size);
    fTrack.NSigmaKaon->reserve(size);
    fTrack.NSigmaProton->reserve(size);
    fTrack.CovMatrix->reserve(size);
#if E2V_TPC_EXTRA
    fTrack.TPC_DCAxy->reserve(size);
    fTrack.TPC_DCAz->reserve(size);
    fTrack.TPC_NCrossedRows->reserve(size);
    fTrack.TPC_NClusters->reserve(size);
    fTrack.TPC_NClustersLC->reserve(size);
    fTrack.TPC_NClustersFound->reserve(size);
    fTrack.TPC_NClustersShared->reserve(size);
    fTrack.TPC_Chi2->reserve(size);
    fTrack.TPC_Chi2Constrained->reserve(size);
    fTrack.TPC_Chi2TCVG->reserve(size);
    if (fIsMC) fTrack.TPC_SignalTunedOnData->reserve(size);
    fTrack.TPC_SignalSigma->reserve(size);
    fTrack.TPC_SignalCorrected->reserve(size);
    fTrack.TPC_ESignalPion->reserve(size);
    fTrack.TPC_ESigmaPion->reserve(size);
    fTrack.TPC_ESignalKaon->reserve(size);
    fTrack.TPC_ESigmaKaon->reserve(size);
    fTrack.TPC_ESignalProton->reserve(size);
    fTrack.TPC_ESigmaProton->reserve(size);
    fTrack.TPC_SignalN->reserve(size);
    fTrack.TPC_PointsFirst->reserve(size);
    fTrack.TPC_PointsIndexMax->reserve(size);
    fTrack.TPC_PointsLast->reserve(size);
    fTrack.TPC_PointsMaxDens->reserve(size);
    fTrack.TPC_FirstRow->reserve(size);
#endif
    if (fIsMC) fTrack.McEntry->reserve(size);
}

// Clear the branches of the reconstructed tracks.
void AliTaskEsd2Vector::ClearBranches_Tracks() {
    fTrack.EsdEntry->clear();
    fTrack.X->clear();
    fTrack.Y->clear();
    fTrack.Z->clear();
    fTrack.Px->clear();
    fTrack.Py->clear();
    fTrack.Pz->clear();
    fTrack.Charge->clear();
    fTrack.PreDCAxy->clear();
    fTrack.PreDCAz->clear();
    fTrack.TPC_Signal->clear();
    fTrack.NSigmaPion->clear();
    fTrack.NSigmaKaon->clear();
    fTrack.NSigmaProton->clear();
    fTrack.CovMatrix->clear();
#if E2V_TPC_EXTRA
    fTrack.TPC_DCAxy->clear();
    fTrack.TPC_DCAz->clear();
    fTrack.TPC_NCrossedRows->clear();
    fTrack.TPC_NClusters->clear();
    fTrack.TPC_NClustersLC->clear();
    fTrack.TPC_NClustersFound->clear();
    fTrack.TPC_NClustersShared->clear();
    fTrack.TPC_Chi2->clear();
    fTrack.TPC_Chi2Constrained->clear();
    fTrack.TPC_Chi2TCVG->clear();
    if (fIsMC) fTrack.TPC_SignalTunedOnData->clear();
    fTrack.TPC_SignalSigma->clear();
    fTrack.TPC_SignalCorrected->clear();
    fTrack.TPC_ESignalPion->clear();
    fTrack.TPC_ESigmaPion->clear();
    fTrack.TPC_ESignalKaon->clear();
    fTrack.TPC_ESigmaKaon->clear();
    fTrack.TPC_ESignalProton->clear();
    fTrack.TPC_ESigmaProton->clear();
    fTrack.TPC_SignalN->clear();
    fTrack.TPC_PointsFirst->clear();
    fTrack.TPC_PointsIndexMax->clear();
    fTrack.TPC_PointsLast->clear();
    fTrack.TPC_PointsMaxDens->clear();
    fTrack.TPC_FirstRow->clear();
#endif
    if (fIsMC) fTrack.McEntry->clear();
}

// # Lambdas # //

void AliTaskEsd2Vector::ProcessLambdas() {
    const int n_v0s = fESD->GetNumberOfV0s();
    for (int v0_entry = 0; v0_entry < n_v0s; ++v0_entry) {
        auto *v0 = fESD->GetV0(v0_entry);
        if (v0 == nullptr) continue;

        // get pid //
        // -- negative daughter
        auto *neg_track = fESD->GetTrack(v0->GetNindex());
        float neg_n_sigma_proton = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, neg_track, AliPID::kProton);
        float neg_n_sigma_kaon = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, neg_track, AliPID::kKaon);
        float neg_n_sigma_pion = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, neg_track, AliPID::kPion);
        // -- positive daughter
        auto *pos_track = fESD->GetTrack(v0->GetPindex());
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
        TLorentzVector lv_antiproton(
            neg_px, neg_py, neg_pz, std::sqrt(neg_px * neg_px + neg_py * neg_py + neg_pz * neg_pz + Common::PdgMass_Proton * Common::PdgMass_Proton));
        TLorentzVector lv_piminus(neg_px, neg_py, neg_pz,
                                  std::sqrt(neg_px * neg_px + neg_py * neg_py + neg_pz * neg_pz + Common::PdgMass_Pion * Common::PdgMass_Pion));
        // -- positive daughter
        double pos_px, pos_py, pos_pz;
        v0->GetPPxPyPz(pos_px, pos_py, pos_pz);
        TLorentzVector lv_proton(pos_px, pos_py, pos_pz,
                                 std::sqrt(pos_px * pos_px + pos_py * pos_py + pos_pz * pos_pz + Common::PdgMass_Proton * Common::PdgMass_Proton));
        TLorentzVector lv_piplus(pos_px, pos_py, pos_pz,
                                 std::sqrt(pos_px * pos_px + pos_py * pos_py + pos_pz * pos_pz + Common::PdgMass_Pion * Common::PdgMass_Pion));
        // -- lambda
        TLorentzVector lv_antilambda = lv_antiproton + lv_piplus;
        TLorentzVector lv_lambda = lv_proton + lv_piminus;

        double delta_m_antilambda = std::abs(lv_antilambda.M() - Common::PdgMass_Lambda);
        double delta_m_lambda = std::abs(lv_lambda.M() - Common::PdgMass_Lambda);

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
        fLambda.EsdEntry->emplace_back(static_cast<unsigned int>(v0_entry));
        fLambda.Decay_X->emplace_back(static_cast<float>(v0->Xv()));
        fLambda.Decay_Y->emplace_back(static_cast<float>(v0->Yv()));
        fLambda.Decay_Z->emplace_back(static_cast<float>(v0->Zv()));
        fLambda.Px->emplace_back(static_cast<float>(v0->Px()));
        fLambda.Py->emplace_back(static_cast<float>(v0->Py()));
        fLambda.Pz->emplace_back(static_cast<float>(v0->Pz()));
        fLambda.DcaV0Daughters->emplace_back(static_cast<float>(v0->GetDcaV0Daughters()));
        // -- negative daughter
        fLambda.Neg_EsdEntry->emplace_back(static_cast<unsigned int>(v0->GetNindex()));
        fLambda.Neg_PCAwrtV0_Px->emplace_back(static_cast<float>(neg_px));
        fLambda.Neg_PCAwrtV0_Py->emplace_back(static_cast<float>(neg_py));
        fLambda.Neg_PCAwrtV0_Pz->emplace_back(static_cast<float>(neg_pz));
        fLambda.Neg_PreDCAxy->emplace_back(neg_dca[0]);
        fLambda.Neg_PreDCAz->emplace_back(neg_dca[1]);
        fLambda.Neg_NSigmaProton->emplace_back(neg_n_sigma_proton);
        fLambda.Neg_NSigmaKaon->emplace_back(neg_n_sigma_kaon);
        fLambda.Neg_NSigmaPion->emplace_back(neg_n_sigma_pion);
        // -- positive daughter
        fLambda.Pos_EsdEntry->emplace_back(static_cast<unsigned int>(v0->GetPindex()));
        fLambda.Pos_PCAwrtV0_Px->emplace_back(static_cast<float>(pos_px));
        fLambda.Pos_PCAwrtV0_Py->emplace_back(static_cast<float>(pos_py));
        fLambda.Pos_PCAwrtV0_Pz->emplace_back(static_cast<float>(pos_pz));
        fLambda.Pos_PreDCAxy->emplace_back(pos_dca[0]);
        fLambda.Pos_PreDCAz->emplace_back(pos_dca[1]);
        fLambda.Pos_NSigmaProton->emplace_back(pos_n_sigma_proton);
        fLambda.Pos_NSigmaKaon->emplace_back(pos_n_sigma_kaon);
        fLambda.Pos_NSigmaPion->emplace_back(pos_n_sigma_pion);
        // -- mc link
        if (fIsMC) {
            fLambda.Neg_McEntry->emplace_back(std::abs(neg_track->GetLabel()));
            fLambda.Pos_McEntry->emplace_back(std::abs(pos_track->GetLabel()));
        }
    }
}

void AliTaskEsd2Vector::ReserveBranches_Lambdas(std::size_t size) {
    fLambda.EsdEntry->reserve(size);
    fLambda.Decay_X->reserve(size);
    fLambda.Decay_Y->reserve(size);
    fLambda.Decay_Z->reserve(size);
    fLambda.Px->reserve(size);
    fLambda.Py->reserve(size);
    fLambda.Pz->reserve(size);
    fLambda.DcaV0Daughters->reserve(size);
    // -- negative daughter
    fLambda.Neg_EsdEntry->reserve(size);
    fLambda.Neg_PCAwrtV0_Px->reserve(size);
    fLambda.Neg_PCAwrtV0_Py->reserve(size);
    fLambda.Neg_PCAwrtV0_Pz->reserve(size);
    fLambda.Neg_PreDCAxy->reserve(size);
    fLambda.Neg_PreDCAz->reserve(size);
    fLambda.Neg_NSigmaProton->reserve(size);
    fLambda.Neg_NSigmaKaon->reserve(size);
    fLambda.Neg_NSigmaPion->reserve(size);
    // -- positive daughter
    fLambda.Pos_EsdEntry->reserve(size);
    fLambda.Pos_PCAwrtV0_Px->reserve(size);
    fLambda.Pos_PCAwrtV0_Py->reserve(size);
    fLambda.Pos_PCAwrtV0_Pz->reserve(size);
    fLambda.Pos_PreDCAxy->reserve(size);
    fLambda.Pos_PreDCAz->reserve(size);
    fLambda.Pos_NSigmaProton->reserve(size);
    fLambda.Pos_NSigmaKaon->reserve(size);
    fLambda.Pos_NSigmaPion->reserve(size);
    // -- mc link
    if (fIsMC) {
        fLambda.Neg_McEntry->reserve(size);
        fLambda.Pos_McEntry->reserve(size);
    }
}

void AliTaskEsd2Vector::ClearBranches_Lambdas() {
    fLambda.EsdEntry->clear();
    fLambda.Decay_X->clear();
    fLambda.Decay_Y->clear();
    fLambda.Decay_Z->clear();
    fLambda.Px->clear();
    fLambda.Py->clear();
    fLambda.Pz->clear();
    fLambda.DcaV0Daughters->clear();
    // -- negative daughter
    fLambda.Neg_EsdEntry->clear();
    fLambda.Neg_PCAwrtV0_Px->clear();
    fLambda.Neg_PCAwrtV0_Py->clear();
    fLambda.Neg_PCAwrtV0_Pz->clear();
    fLambda.Neg_PreDCAxy->clear();
    fLambda.Neg_PreDCAz->clear();
    fLambda.Neg_NSigmaProton->clear();
    fLambda.Neg_NSigmaKaon->clear();
    fLambda.Neg_NSigmaPion->clear();
    // -- positive daughter
    fLambda.Pos_EsdEntry->clear();
    fLambda.Pos_PCAwrtV0_Px->clear();
    fLambda.Pos_PCAwrtV0_Py->clear();
    fLambda.Pos_PCAwrtV0_Pz->clear();
    fLambda.Pos_PreDCAxy->clear();
    fLambda.Pos_PreDCAz->clear();
    fLambda.Pos_NSigmaProton->clear();
    fLambda.Pos_NSigmaKaon->clear();
    fLambda.Pos_NSigmaPion->clear();
    // -- mc link
    if (fIsMC) {
        fLambda.Neg_McEntry->clear();
        fLambda.Pos_McEntry->clear();
    }
}

// # Injected Reactions # //

// Store the in-memory values into the tree branches.
void AliTaskEsd2Vector::ProcessInjectedReactions() {
    ReserveBranches_Injected();
    for (int r = 0; r < E2R::NReactionsPerEvent; ++r) {
        fInjectedSexa.ReactionID->emplace_back(fEvVec_ReactionID[*fEvent.EventNumber][r]);
        fInjectedSexa.Px->emplace_back(fEvVec_Sexaquark_Px[*fEvent.EventNumber][r]);
        fInjectedSexa.Py->emplace_back(fEvVec_Sexaquark_Py[*fEvent.EventNumber][r]);
        fInjectedSexa.Pz->emplace_back(fEvVec_Sexaquark_Pz[*fEvent.EventNumber][r]);
        fInjectedSexa.Nucleon_Px->emplace_back(fEvVec_Nucleon_Px[*fEvent.EventNumber][r]);
        fInjectedSexa.Nucleon_Py->emplace_back(fEvVec_Nucleon_Py[*fEvent.EventNumber][r]);
        fInjectedSexa.Nucleon_Pz->emplace_back(fEvVec_Nucleon_Pz[*fEvent.EventNumber][r]);
    }
}

// Copy to working directory the respective `sim.log` that corresponds to the `RunNumber+DirNumber` that's being analyzed.
void AliTaskEsd2Vector::BringSignalLogs() {

    TGrid *alien = nullptr;
    if (gGrid == nullptr) {
        alien = TGrid::Connect("alien://");
        if (alien == nullptr) return;
    }

    TString AliEn_Dir = fAliEnPath(0, fAliEnPath.Last('/'));

    TString orig_path = TString::Format("%s/sim.log", AliEn_Dir.Data());
    AliInfoF("Copying file %s ...", orig_path.Data());

    // assuming path ends with format `.../LHC23l1a3/A1.73/297595/001/sim.log` //
    auto AliEn_DirNumber = static_cast<int>(*fEvent.DirNumber);

    TObjArray *tokens = fAliEnPath.Tokenize("/");
    auto AliEn_RunNumber = (dynamic_cast<TObjString *>(tokens->At(tokens->GetEntries() - 3)))->GetString().Atoi();
    TString AliEn_SimSubSet = (dynamic_cast<TObjString *>(tokens->At(tokens->GetEntries() - 4)))->GetString();
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
bool AliTaskEsd2Vector::ReadSignalLogs() {

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

        if (line.rfind(E2R::SimLog_EventHeader, 0) == 0) {
            ++event_n;
            // std::cout << "Reading Event " << event_n << '\n'; // DEBUG
            react_id = 0;
            continue;
        }

        if (event_n >= 0) {
            if (line.rfind(E2R::SimLog_ReactionMarker, 0) == 0) {
                std::string data_part = line.substr(E2R::SimLog_ReactionMarker.length() + 1);
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
void AliTaskEsd2Vector::ReserveBranches_Injected() {
    fInjectedSexa.ReactionID->reserve(E2R::NReactionsPerEvent);
    fInjectedSexa.Px->reserve(E2R::NReactionsPerEvent);
    fInjectedSexa.Py->reserve(E2R::NReactionsPerEvent);
    fInjectedSexa.Pz->reserve(E2R::NReactionsPerEvent);
    fInjectedSexa.Nucleon_Px->reserve(E2R::NReactionsPerEvent);
    fInjectedSexa.Nucleon_Py->reserve(E2R::NReactionsPerEvent);
    fInjectedSexa.Nucleon_Pz->reserve(E2R::NReactionsPerEvent);
}

// Clear the branches of the injected reactions.
void AliTaskEsd2Vector::ClearBranches_Injected() {
    fInjectedSexa.ReactionID->clear();
    fInjectedSexa.Px->clear();
    fInjectedSexa.Py->clear();
    fInjectedSexa.Pz->clear();
    fInjectedSexa.Nucleon_Px->clear();
    fInjectedSexa.Nucleon_Py->clear();
    fInjectedSexa.Nucleon_Pz->clear();
}
