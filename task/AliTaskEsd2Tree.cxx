#include <fstream>
#include <memory>
#include <sstream>
#include <string>

#include <TChain.h>
#include <TFile.h>
#include <TGrid.h>
#include <TList.h>
#include <TObjString.h>
#include <TSystem.h>

#include <Math/Point3D.h>
#include <Math/Vector4D.h>

#include <AliAnalysisManager.h>
#include <AliAnalysisTaskSE.h>
#include <AliESDEvent.h>
#include <AliESDInputHandler.h>
#include <AliESDtrack.h>
#include <AliLog.h>
#include <AliMCEvent.h>
#include <AliMCParticle.h>
#include <AliPIDResponse.h>
#include <AliVEvent.h>

#include <AliAnalysisUtils.h>
#include <AliMultSelection.h>

#include "Constants.hpp"
#include "Cuts_E2T.hpp"
#include "Math.hpp"

#include "AliTaskEsd2Tree.h"

ClassImp(AliTaskEsd2Tree);

// Empty I/O constructor. Non-persistent members are initialized to their default values from here.
AliTaskEsd2Tree::AliTaskEsd2Tree()
    : AliAnalysisTaskSE{},
      //
      fIsMC{false},
      fIsMC_DedicatedSexaquark{false},
      fIsMC_DedicatedHdibaryon{false},
      //
      fMC{nullptr},
      fMC_PrimaryVertex{nullptr},
      fESD{nullptr},
      fPrimaryVertex{nullptr},
      fPIDResponse{nullptr},
      //
      fAliEnPath{""},
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
      fHist_Centrality{nullptr},
      fHist_CentralityINT7{nullptr},
      fHist_Tracks_Bookkeeping{nullptr},
      fHist_PreFoundLambdas_Bookkeeping{nullptr},
      //
      fOutputTree{nullptr},
      fOutput{},
      fWriter{nullptr} {}

// Constructor, called locally.
AliTaskEsd2Tree::AliTaskEsd2Tree(const char *name)
    : AliAnalysisTaskSE{name},
      //
      fIsMC{false},
      fIsMC_DedicatedSexaquark{false},
      fIsMC_DedicatedHdibaryon{false},
      //
      fMC{nullptr},
      fMC_PrimaryVertex{nullptr},
      fESD{nullptr},
      fPrimaryVertex{nullptr},
      fPIDResponse{nullptr},
      //
      fAliEnPath{""},
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
      fHist_Centrality{nullptr},
      fHist_CentralityINT7{nullptr},
      fHist_Tracks_Bookkeeping{nullptr},
      fHist_PreFoundLambdas_Bookkeeping{nullptr},
      //
      fOutputTree{nullptr},
      fOutput{},
      fWriter{nullptr} {
    DefineInput(0, TChain::Class());
    DefineOutput(1, TList::Class());  // fOutputList
    DefineOutput(2, TTree::Class());  // fOutputTree
}

// Destructor.
AliTaskEsd2Tree::~AliTaskEsd2Tree() {
    // NOTE: if `TList::SetOwner(true)` was called, the TList destructor should delete all objects added to it.
    delete fOutputList;
}

// Initialize analysis task. Needs to be called within an `AddTaskEsd2Tree.C` macro.
void AliTaskEsd2Tree::Initialize(bool is_mc, bool is_sexa_mc, bool is_hdib_mc) {
    fIsMC = is_mc;
    fIsMC_DedicatedSexaquark = is_sexa_mc;
    fIsMC_DedicatedHdibaryon = is_hdib_mc;
    // print settings //
    AliInfo("Initializing...");
    AliInfo("Settings:");
    AliInfo("========");
    AliInfoF(">> IsMC                    = %i", fIsMC);
    AliInfoF(">> IsMC_DedicatedSexaquark = %i", fIsMC_DedicatedSexaquark);
    AliInfoF(">> IsMC_DedicatedHdibaryon = %i", fIsMC_DedicatedHdibaryon);
}

// # Executed at Runtime # //

// Create output objects, called once at RUNTIME ~ execution on Grid.
void AliTaskEsd2Tree::UserCreateOutputObjects() {

    auto *man = AliAnalysisManager::GetAnalysisManager();
    if (man == nullptr) AliFatal("AliAnalysisManager couldn't be found.");

    auto *inputHandler = dynamic_cast<AliESDInputHandler *>(man->GetInputEventHandler());
    if (inputHandler == nullptr) AliFatal("AliESDInputHandler couldn't be found.");

    fPIDResponse = inputHandler->GetPIDResponse();

    // Prepare output TTree //

    OpenFile(2)->SetCompressionLevel(9);
    fWriter = std::make_unique<Framework::TeeTree::Writer>(fOutput.CreateModel_TeeTree(fIsMC, fIsMC_DedicatedSexaquark), E2T::Name_OutputTree);
    fOutputTree = fWriter->GetTree();

    // Prepare output list and histograms //

    fOutputList = new TList();
    fOutputList->SetOwner(true);

    fHist_Centrality = new TH1F("Centrality", ";Centrality;Counts", 42, 0., 105.);
    fOutputList->Add(fHist_Centrality);

    fHist_CentralityINT7 = new TH1F("CentralityINT7", ";CentralityINT7;Counts", 42, 0., 105.);
    fOutputList->Add(fHist_CentralityINT7);

    TAxis *x_axis = nullptr;

    // -- tracks bookkeeping
    fHist_Tracks_Bookkeeping = new TH1F("Tracks_Bookkeeping", ";;Counts", ETrack::kNTrackCuts, 0., static_cast<double>(ETrack::kNTrackCuts));
    // x_axis = fHist_Tracks_Bookkeeping->GetXaxis();  // PENDING for when cuts defined
    fOutputList->Add(fHist_Tracks_Bookkeeping);

    // -- pre-found (anti)lambdas
    fHist_PreFoundLambdas_Bookkeeping = new TH1F("PreFoundLambdas_Bookkeeping", ";;Counts", EPreFoundLambda::kNPreFoundLambdaCuts, 0.,
                                                 static_cast<double>(EPreFoundLambda::kNPreFoundLambdaCuts));
    x_axis = fHist_PreFoundLambdas_Bookkeeping->GetXaxis();
    x_axis->SetBinLabel(EPreFoundLambda::kAllPreFoundV0s + 1, "AllPreFoundV0s");
    x_axis->SetBinLabel(EPreFoundLambda::kOnTheFlyV0s + 1, "OnTheFlyV0s");
    x_axis->SetBinLabel(EPreFoundLambda::kPassesDcaV0Daughters + 1, "PassesDcaV0Daughters");
    x_axis->SetBinLabel(EPreFoundLambda::kPassesMinPtLambda + 1, "PassesMinPtLambda");
    x_axis->SetBinLabel(EPreFoundLambda::kPassesMaxPtLambda + 1, "PassesMaxPtLambda");
    x_axis->SetBinLabel(EPreFoundLambda::kPassesMinDecayRadius2D + 1, "PassesMinDecayRadius2D");
    x_axis->SetBinLabel(EPreFoundLambda::kPassesDCAwrtPV + 1, "PassesDCAwrtPV");
    x_axis->SetBinLabel(EPreFoundLambda::kNegDaughterHasValidPid + 1, "NegDaughterHasValidPid");
    x_axis->SetBinLabel(EPreFoundLambda::kPosDaughterHasValidPid + 1, "PosDaughterHasValidPid");
    x_axis->SetBinLabel(EPreFoundLambda::kPassesPid + 1, "PassesPid");
    x_axis->SetBinLabel(EPreFoundLambda::kPassesMinPtDaughters + 1, "PassesMinPtDaughters");
    x_axis->SetBinLabel(EPreFoundLambda::kPassesMaxPtDaughters + 1, "PassesMaxPtDaughters");
    x_axis->SetBinLabel(EPreFoundLambda::kPassesAbsMaxEtaDaughters + 1, "PassesAbsMaxEtaDaughters");
    x_axis->SetBinLabel(EPreFoundLambda::kPassesInvariantMass + 1, "PassesInvariantMass");
    x_axis->SetBinLabel(EPreFoundLambda::kPassesArmenterosPodolanski + 1, "PassesArmenterosPodolanski");
    fOutputList->Add(fHist_PreFoundLambdas_Bookkeeping);

    fEventCuts.AddQAplotsToList(fOutputList);

    // Post data //

    PostData(1, fOutputList);
    PostData(2, fOutputTree);
}

// User implementation of `Notify()`. Needed for reading the AliEn path.
// This function is loaded during `AliAnalysisManager::Notify()`.
// It's called after `UserCreateOutputObjects()`, for each new file, and before the first `UserExec()`.
bool AliTaskEsd2Tree::UserNotify() {

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
    int tokens_n_entries = tokens->GetEntries();
    if (tokens_n_entries < 4) {
        delete tokens;
        AliFatal("Local paths must mimic the AliEn layout.");
    }
    if (fIsMC) {
        // NOTES:
        // - path of gen. purp. MC ends with format `.../297595/001/AliESDs.root`
        // - path of signal MC ends with format `.../A1.73/297595/001/AliESDs.root`
        fOutput.Event.RunNumber = (dynamic_cast<TObjString *>(tokens->At(tokens_n_entries - 3)))->GetString().Atoi();
        fOutput.Event.DirNumber = (dynamic_cast<TObjString *>(tokens->At(tokens_n_entries - 2)))->GetString().Atoi();
        fOutput.Event.DirNumberB = 0;                                              // non-existent in MC
        AliInfoF("Run Number = %04i", static_cast<int>(fOutput.Event.RunNumber));  // = 1
        AliInfoF("Dir Number = %04i", static_cast<int>(fOutput.Event.DirNumber));  // = 1
        if (fIsMC_DedicatedSexaquark) {
            TString SimSubSet = (dynamic_cast<TObjString *>(tokens->At(tokens_n_entries - 4)))->GetString();
            if (!ReadSignalLogs(SimSubSet)) return false;
        }
    } else {
        // NOTE: path of real data ends with format `.../LHC15o/000245232/pass2/15000245232039.914/AliESDs.root`
        fOutput.Event.RunNumber = (dynamic_cast<TObjString *>(tokens->At(tokens_n_entries - 4)))->GetString().Atoi();
        AliInfoF("Run Number = %i", fOutput.Event.RunNumber);
        auto aux_dir_nr = (dynamic_cast<TObjString *>(tokens->At(tokens_n_entries - 2)))->GetString();
        aux_dir_nr = TString(aux_dir_nr(2 + 3 + 6, 10));              // = "039.914"
        fOutput.Event.DirNumber = TString(aux_dir_nr(0, 3)).Atoi();   // = 39
        fOutput.Event.DirNumberB = TString(aux_dir_nr(4, 5)).Atoi();  // = 914
        AliInfoF("Dir Number = %i", fOutput.Event.DirNumber);
        AliInfoF("Dir Number B = %i", fOutput.Event.DirNumberB);
    }
    delete tokens;  // because ROOT

    // Adapt event cuts  //

    fEventCuts.OverrideAutomaticTriggerSelection(AliVEvent::kINT7 | AliVEvent::kCentral | AliVEvent::kSemiCentral);

    // For real data, depending on run number
    // Reference: https://twiki.cern.ch/twiki/bin/view/ALICE/AliDPGRunList18r
    if (!fIsMC && (fOutput.Event.RunNumber == 296749 || fOutput.Event.RunNumber == 296750 || fOutput.Event.RunNumber == 296849 ||
                   fOutput.Event.RunNumber == 296890 || fOutput.Event.RunNumber == 297029 || fOutput.Event.RunNumber == 297194 ||
                   fOutput.Event.RunNumber == 297219 || fOutput.Event.RunNumber == 297481)) {
        fEventCuts.UseTimeRangeCut();
    }

    return true;
}

// Main function, called per each event at RUNTIME ~ execution on Grid.
void AliTaskEsd2Tree::UserExec(Option_t *option) {

    // events //

    if (!ProcessEvent()) return;

    // mc particles //

    if (fIsMC) {
        if (fIsMC_DedicatedSexaquark) ProcessInjectedReactions();
        ProcessMCParticles();
    }

    // tracks //

    ProcessTracks();

    // (anti)lambdas //

    ProcessPreFoundLambdas();

    // end of event //

    fWriter->Fill();
    fOutput.Clear(fIsMC, fIsMC_DedicatedSexaquark);

    // post data //

    PostData(1, fOutputList);
    PostData(2, fOutputTree);
}

// # Events # //

bool AliTaskEsd2Tree::ProcessEvent() {

#if E2T_VERBOSE
    AliInfoF("fPIDResponse.UseTPCEtaCorrection()          = %i", fPIDResponse->UseTPCEtaCorrection());
    AliInfoF("fPIDResponse.UseTPCMultiplicityCorrection() = %i", fPIDResponse->UseTPCMultiplicityCorrection());
    AliInfoF("fPIDResponse.UseTPCPileupCorrection()       = %i", fPIDResponse->UseTPCPileupCorrection());
#endif

    // Load MC Event //

    if (fIsMC) {
        fMC = MCEvent();
        if (fMC == nullptr) {
            AliWarning("AliMCEvent couldn't be found.");
            return false;
        }
        fPIDResponse->SetCurrentMCEvent(fMC);  // enable PID tuned on data
    }

    // Load Reconstructed Event //

    fESD = dynamic_cast<AliESDEvent *>(InputEvent());
    if (fESD == nullptr) {
        AliWarning("AliESDEvent couldn't be found.");
        return false;
    }

    // Event selection //

    // NOTE: exclude h-dibaryon MC, because it doesn't have underlying HIJING event + many detectors are turned off
    if (!fIsMC_DedicatedHdibaryon && !fEventCuts.AcceptEvent(fESD)) return false;

    // Assign branches //

    // NOTE: `RunNumber`, `DirNumber` and `DirNumberB` are assigned in `UserNotify()`
    fOutput.Event.EventNumber = fESD->GetEventNumberInFile();
    // -- magnetic field
    fOutput.Event.MagneticField = static_cast<float>(fESD->GetMagneticField());
    // -- primary vertex
    fPrimaryVertex = fESD->GetPrimaryVertex();
    fOutput.Event.PV_X = static_cast<float>(fPrimaryVertex->GetX());
    fOutput.Event.PV_Y = static_cast<float>(fPrimaryVertex->GetY());
    fOutput.Event.PV_Z = static_cast<float>(fPrimaryVertex->GetZ());
    // -- primary vertex's cov. matrix
    double PV_CovMatrix[6]{};
    fPrimaryVertex->GetCovarianceMatrix(PV_CovMatrix);
    fOutput.Event.PV_CovMatrix = {static_cast<float>(PV_CovMatrix[0]), static_cast<float>(PV_CovMatrix[1]), static_cast<float>(PV_CovMatrix[2]),
                                  static_cast<float>(PV_CovMatrix[3]), static_cast<float>(PV_CovMatrix[4]), static_cast<float>(PV_CovMatrix[5])};
    // -- centrality
    auto *MultSelection = dynamic_cast<AliMultSelection *>(fESD->FindListObject("MultSelection"));
    if (MultSelection == nullptr) {
        AliWarning("AliMultSelection couldn't be found.");
        return false;
    }
    fOutput.Event.Centrality = MultSelection->GetMultiplicityPercentile("V0M");
    // -- trigger classes
    fOutput.Event.IsINT7 = (fInputHandler->IsEventSelected() & AliVEvent::kINT7) > 0;
    fOutput.Event.IsCentral = (fInputHandler->IsEventSelected() & AliVEvent::kCentral) > 0;
    fOutput.Event.IsSemiCentral = (fInputHandler->IsEventSelected() & AliVEvent::kSemiCentral) > 0;

    // Fill event histograms //

    fHist_Centrality->Fill(fOutput.Event.Centrality);
    if (fOutput.Event.IsINT7) fHist_CentralityINT7->Fill(fOutput.Event.Centrality);

    // Assign MC branches //

    if (fIsMC) {
        fMC_PrimaryVertex = fMC->GetPrimaryVertex();
        fOutput.MC_Event.PV_X = static_cast<float>(fMC_PrimaryVertex->GetX());
        fOutput.MC_Event.PV_Y = static_cast<float>(fMC_PrimaryVertex->GetY());
        fOutput.MC_Event.PV_Z = static_cast<float>(fMC_PrimaryVertex->GetZ());
        // NOTE: added protection against dedicated h-dibaryon MC, not because it fails, but because the error logs slow down the program
        fOutput.MC_Event.IsGenPileup = !fIsMC_DedicatedHdibaryon ? AliAnalysisUtils::IsPileupInGeneratedEvent(fMC, "Hijing") : false;
        fOutput.MC_Event.IsSBCPileup = !fIsMC_DedicatedHdibaryon ? AliAnalysisUtils::IsSameBunchPileupInGeneratedEvent(fMC, "Hijing") : false;
    }

    return true;
}

// # MC Generated # //

// Loop over MC particles in a single event.
void AliTaskEsd2Tree::ProcessMCParticles() {

    // vector preallocation //
    const int n_mc = fMC->GetNumberOfTracks();
    fOutput.McParticle.reserve(n_mc);

    // read mc particles //
    for (int mc_entry = 0; mc_entry < n_mc; ++mc_entry) {
        auto *mcPart = dynamic_cast<AliMCParticle *>(fMC->GetTrack(mc_entry));
        if (mcPart == nullptr) continue;

        // create new mc //
        POD::McParticle new_mc;  // initialized by default
        new_mc.PdgCode = mcPart->PdgCode();
        new_mc.Charge = static_cast<int>(mcPart->Charge() / 3);  // because `TParticle` returns charge in |e|/3
        new_mc.Mother_McEntry = mcPart->GetMother();
        new_mc.N_Daughters = static_cast<unsigned int>(mcPart->GetNDaughters());
        new_mc.FirstDau_McEntry = mcPart->GetDaughterFirst();
        new_mc.LastDau_McEntry = mcPart->GetDaughterLast();
        new_mc.Origin_X = static_cast<float>(mcPart->Xv());
        new_mc.Origin_Y = static_cast<float>(mcPart->Yv());
        new_mc.Origin_Z = static_cast<float>(mcPart->Zv());
        new_mc.Px = static_cast<float>(mcPart->Px());
        new_mc.Py = static_cast<float>(mcPart->Py());
        new_mc.Pz = static_cast<float>(mcPart->Pz());
        new_mc.Energy = static_cast<float>(mcPart->E());
        new_mc.StatusCode = mcPart->MCStatusCode();  // in `AliMCParticle::MCStatusCode()` returns `unsigned int`
        new_mc.Generator = GetCustomGeneratorIndex(mcPart);
        new_mc.IsPhysPrimary = mcPart->IsPhysicalPrimary();
        new_mc.IsSecFromMat = mcPart->IsSecondaryFromMaterial();
        new_mc.IsSecFromWeak = mcPart->IsSecondaryFromWeakDecay();
#if E2T_VERBOSE
        if (mcPart->GetGeneratorIndex() == 2) {
            AliInfoF("mc_entry=%i,mc_pdg=%i,mc_is_physprim=%i,mc_status=%i,mother_entry=%i",  //
                     mc_entry, mcPart->PdgCode(), mcPart->IsPhysicalPrimary(), mcPart->MCStatusCode(), mcPart->GetMother());
        }
#endif
#if E2T_MC_EXTRA
        new_mc.IsOOBPileup = AliAnalysisUtils::IsParticleFromOutOfBunchPileupCollision(mc_entry, fMC);
#endif

        // push //
        fOutput.McParticle.emplace_back(new_mc);
    }  // end of loop over MC particles
}

// # Reconstructed # //

// Loop over the reconstructed tracks in a single event.
void AliTaskEsd2Tree::ProcessTracks() {

    // vector preallocation //
    const int n_tracks = fESD->GetNumberOfTracks();
    fOutput.Track.reserve(n_tracks);
    if (fIsMC) fOutput.Track_McEntry.reserve(n_tracks);

    // allocate c-arrays //
    double position[3]{};
    double momentum[3]{};
    double cov_xyz_pxpypz[Common::NCovMatrixComponents_State6]{};
    float pre_dca[2]{};
    float pre_dca_cov[3]{};

    // read tracks //
    for (int esd_entry = 0; esd_entry < n_tracks; ++esd_entry) {
        auto *esd_track = fESD->GetTrack(esd_entry);
        if (esd_track == nullptr) continue;

        fHist_Tracks_Bookkeeping->Fill(ETrack::kAllTracks);

        // track selection //

        // tpc status (1)
        auto status = esd_track->GetStatus();
        bool tpc_in = (status & AliESDtrack::kTPCin) > 0;
        bool tpc_refit = (status & AliESDtrack::kTPCrefit) > 0;
        if (!tpc_in || !tpc_refit) continue;  // apply cut
        fHist_Tracks_Bookkeeping->Fill(ETrack::kValidStatusTPC_1);

        // tpc status (2)
        const auto *inner_param = esd_track->GetInnerParam();
        if (inner_param == nullptr) continue;  // apply cut
        fHist_Tracks_Bookkeeping->Fill(ETrack::kValidStatusTPC_2);

        // its status -- equivalent to counting n hits on its
        bool its_in = (status & AliESDtrack::kITSin) > 0;
        if (its_in) continue;  // apply cut
        fHist_Tracks_Bookkeeping->Fill(ETrack::kNoHitInITS);

        // pre-calc. dca w.r.t pv // PENDING
        esd_track->GetImpactParameters(pre_dca, pre_dca_cov);
        // if (/* add amazing DCA cut here */) continue;  // apply cut
        // fHist_Tracks_Bookkeeping->Fill(ETrack::kPassesDcaCuts);

        // max |pz|
        if (std::abs(inner_param->Pz()) > E2T::Cuts::Track::AbsMax_Pz) continue;  // apply cut
        fHist_Tracks_Bookkeeping->Fill(ETrack::kPassesAbsMaxPz);

        // min pt
        if (inner_param->Pt() < E2T::Cuts::Track::Min_Pt) continue;  // apply cut
        fHist_Tracks_Bookkeeping->Fill(ETrack::kPassesMinPt);

        // max pt
        if (inner_param->Pt() > E2T::Cuts::Track::Max_Pt) continue;  // apply cut
        fHist_Tracks_Bookkeeping->Fill(ETrack::kPassesMaxPt);

        // max |eta|
        if (std::abs(inner_param->Eta()) > E2T::Cuts::Track::AbsMax_Eta) continue;  // apply cut
        fHist_Tracks_Bookkeeping->Fill(ETrack::kPassesAbsMaxEta);

        // pid (1)
        auto pid_status = fPIDResponse->CheckPIDStatus(AliPIDResponse::kTPC, esd_track);
        if (pid_status != AliPIDResponse::kDetPidOk) continue;  // apply cut
        fHist_Tracks_Bookkeeping->Fill(ETrack::kHasValidPid);

        // pid (2) -- these functions include corrections (when RD) + tuning on data (when MC)
        float n_sigmas_p = fPIDResponse->NumberOfSigmasTPC(esd_track, AliPID::kProton);
        float n_sigmas_k = fPIDResponse->NumberOfSigmasTPC(esd_track, AliPID::kKaon);
        float n_sigmas_pi = fPIDResponse->NumberOfSigmasTPC(esd_track, AliPID::kPion);
        if (std::abs(n_sigmas_p) > E2T::Cuts::Track::AbsMax_NSigmas_PID && std::abs(n_sigmas_k) > E2T::Cuts::Track::AbsMax_NSigmas_PID &&
            std::abs(n_sigmas_pi) > E2T::Cuts::Track::AbsMax_NSigmas_PID) {
            continue;  // apply cut
        }
        fHist_Tracks_Bookkeeping->Fill(ETrack::kPidWithinRange);

        // n clusters
        auto tpc_n_clusters = esd_track->GetTPCNcls();
        if (tpc_n_clusters < E2T::Cuts::Track::Min_TPC_NClusters) continue;  // apply cut
        fHist_Tracks_Bookkeeping->Fill(ETrack::kPassesMinNTPCClusters);

        // chi2 / nclusters
        auto tpc_chi2_ncls = esd_track->GetTPCchi2() / static_cast<double>(tpc_n_clusters);  // division protected by prev. cut
        if (tpc_chi2_ncls > E2T::Cuts::Track::Max_TPC_Chi2NClusters) continue;               // apply cut
        fHist_Tracks_Bookkeeping->Fill(ETrack::kPassesChi2NClusters);

        // kink rejection
        if (esd_track->GetKinkIndex(0) > 0) continue;  // apply cut
        fHist_Tracks_Bookkeeping->Fill(ETrack::kNotAKink);

        // create new //
        POD::Track new_track;  // non-initialized on purpose
        new_track.EsdEntry = static_cast<unsigned int>(esd_entry);
        inner_param->GetXYZ(position);
        new_track.X = static_cast<float>(position[0]);
        new_track.Y = static_cast<float>(position[1]);
        new_track.Z = static_cast<float>(position[2]);
        inner_param->GetPxPyPz(momentum);
        new_track.Px = static_cast<float>(momentum[0]);
        new_track.Py = static_cast<float>(momentum[1]);
        new_track.Pz = static_cast<float>(momentum[2]);
        new_track.Charge = esd_track->Charge();
        // -- dca
        new_track.PreDCAxy = std::abs(pre_dca[0]);
        new_track.PreDCAz = std::abs(pre_dca[1]);
        // -- pid; for RD, it includes corrections; for MC, this is the correct way
        new_track.TPC_Signal = fIsMC ? static_cast<float>(esd_track->GetTPCsignalTunedOnData()) : static_cast<float>(esd_track->GetTPCsignal());
        new_track.NSigmasPion = n_sigmas_pi;
        new_track.NSigmasKaon = n_sigmas_k;
        new_track.NSigmasProton = n_sigmas_p;
        // -- cov. matrix
        inner_param->GetCovarianceXYZPxPyPz(cov_xyz_pxpypz);
        for (std::size_t idx_cov = 0; idx_cov < Common::NCovMatrixComponents_State6; ++idx_cov) {
            new_track.CovMatrix[idx_cov] = static_cast<float>(cov_xyz_pxpypz[idx_cov]);
        }
        new_track.TPC_FirstRow = GetFirstRow(esd_track->GetTPCClusterMap());
#if E2T_TPC_EXTRA
        // -- extra tpc branches
        new_track.TPC_Chi2 = static_cast<float>(esd_track->GetTPCchi2());
        new_track.TPC_Chi2Constrained = static_cast<float>(esd_track->GetConstrainedChi2TPC());
        new_track.TPC_Chi2TCVG = static_cast<float>(esd_track->GetChi2TPCConstrainedVsGlobal(fPrimaryVertex));
        new_track.TPC_ESignalPion = fPIDResponse->GetExpectedSignal(AliPIDResponse::kTPC, esd_track, AliPID::kPion);
        new_track.TPC_ESignalKaon = fPIDResponse->GetExpectedSignal(AliPIDResponse::kTPC, esd_track, AliPID::kKaon);
        new_track.TPC_ESignalProton = fPIDResponse->GetExpectedSignal(AliPIDResponse::kTPC, esd_track, AliPID::kProton);
        new_track.TPC_ESigmaPion = fPIDResponse->GetExpectedSigma(AliPIDResponse::kTPC, esd_track, AliPID::kPion);
        new_track.TPC_ESigmaKaon = fPIDResponse->GetExpectedSigma(AliPIDResponse::kTPC, esd_track, AliPID::kKaon);
        new_track.TPC_ESigmaProton = fPIDResponse->GetExpectedSigma(AliPIDResponse::kTPC, esd_track, AliPID::kProton);
        new_track.TPC_NCrossedRows = esd_track->GetTPCCrossedRows();
        new_track.TPC_NClusters = esd_track->GetTPCNcls();
        new_track.TPC_NClustersLC = esd_track->GetTPCncls();  // different from `GetTPCNCls` (uppercase), see wiki
        new_track.TPC_NClustersFindable = esd_track->GetTPCNclsF();
        new_track.TPC_NClustersShared = esd_track->GetTPCnclsS();
#endif

        // push reconstructed //
        fOutput.Track.emplace_back(new_track);

        // push mc info //
        if (fIsMC) fOutput.Track_McEntry.emplace_back(std::abs(esd_track->GetLabel()));
    }  // end of loop over tracks
}

// # Lambdas # //

void AliTaskEsd2Tree::ProcessPreFoundLambdas() {

    // vector preallocation //
    const int n_v0s = fESD->GetNumberOfV0s();
    fOutput.PreFoundLambda.reserve(n_v0s);
    if (fIsMC) {
        fOutput.PreFoundLambda_Neg_McEntry.reserve(n_v0s);
        fOutput.PreFoundLambda_Pos_McEntry.reserve(n_v0s);
    }

    // allocate arrays //
    // -- negative daughter
    double neg_position[3]{};
    double neg_momentum[3]{};
    double neg_cov_xyz_pxpypz[Common::NCovMatrixComponents_State6]{};
    float neg_dca[2]{};
    float neg_cov_dca[3]{};
    double neg_px, neg_py, neg_pz;
    // -- positive daughter
    double pos_position[3]{};
    double pos_momentum[3]{};
    double pos_cov_xyz_pxpypz[Common::NCovMatrixComponents_State6]{};
    float pos_dca[2]{};
    float pos_cov_dca[3]{};
    double pos_px, pos_py, pos_pz;

    // auxiliary vars //
    const ROOT::Math::XYZPoint pv{fPrimaryVertex->GetX(), fPrimaryVertex->GetY(), fPrimaryVertex->GetZ()};
    ROOT::Math::PxPyPzMVector lv_antiproton;
    ROOT::Math::PxPyPzMVector lv_proton;
    ROOT::Math::PxPyPzMVector lv_piminus;
    ROOT::Math::PxPyPzMVector lv_piplus;
    ROOT::Math::XYZVector mom_v0;
    ROOT::Math::XYZPoint dv_v0;

    // loop over pre-found v0s //
    for (int entry_v0 = 0; entry_v0 < n_v0s; ++entry_v0) {
        auto *v0 = fESD->GetV0(entry_v0);
        if (v0 == nullptr) continue;

        fHist_PreFoundLambdas_Bookkeeping->Fill(EPreFoundLambda::kAllPreFoundV0s);

        // choose only on-the-fly v0s
        if (!v0->GetOnFlyStatus()) continue;  // apply cut
        fHist_PreFoundLambdas_Bookkeeping->Fill(EPreFoundLambda::kOnTheFlyV0s);

        // dca between daughters
        if (v0->GetDcaV0Daughters() > E2T::Cuts::PreFoundLambda::Max_DcaV0Daughters) continue;  // apply cut
        fHist_PreFoundLambdas_Bookkeeping->Fill(EPreFoundLambda::kPassesDcaV0Daughters);

        // transverse momentum
        if (v0->Pt() < E2T::Cuts::PreFoundLambda::Min_Pt) continue;  // apply cut
        fHist_PreFoundLambdas_Bookkeeping->Fill(EPreFoundLambda::kPassesMinPtLambda);
        if (v0->Pt() > E2T::Cuts::PreFoundLambda::Max_Pt) continue;  // apply cut
        fHist_PreFoundLambdas_Bookkeeping->Fill(EPreFoundLambda::kPassesMaxPtLambda);

        // decay radius 2d
        dv_v0.SetCoordinates(v0->Xv(), v0->Yv(), v0->Zv());
        if (dv_v0.Rho() < E2T::Cuts::PreFoundLambda::Min_Decay_Radius2D) continue;  // apply cut
        fHist_PreFoundLambdas_Bookkeeping->Fill(EPreFoundLambda::kPassesMinDecayRadius2D);

        // dca wrt pv
        mom_v0.SetCoordinates(v0->Px(), v0->Py(), v0->Pz());
        if (Common::Math::FastDCA_LineVertex(mom_v0, dv_v0, pv) > E2T::Cuts::PreFoundLambda::Max_DCA_wrt_PV) continue;  // apply cut
        fHist_PreFoundLambdas_Bookkeeping->Fill(EPreFoundLambda::kPassesDCAwrtPV);

        // negative daughter's pid
        auto *neg_track = fESD->GetTrack(v0->GetNindex());

        auto neg_pid_status = fPIDResponse->CheckPIDStatus(AliPIDResponse::kTPC, neg_track);
        if (neg_pid_status != AliPIDResponse::kDetPidOk) continue;  // apply cut
        fHist_PreFoundLambdas_Bookkeeping->Fill(EPreFoundLambda::kNegDaughterHasValidPid);

        float neg_n_sigmas_proton = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, neg_track, AliPID::kProton);
        float neg_n_sigmas_pion = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, neg_track, AliPID::kPion);

        // positive daughter's pid
        auto *pos_track = fESD->GetTrack(v0->GetPindex());

        auto pos_pid_status = fPIDResponse->CheckPIDStatus(AliPIDResponse::kTPC, pos_track);
        if (pos_pid_status != AliPIDResponse::kDetPidOk) continue;  // apply cut
        fHist_PreFoundLambdas_Bookkeeping->Fill(EPreFoundLambda::kPosDaughterHasValidPid);

        float pos_n_sigmas_proton = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, pos_track, AliPID::kProton);
        float pos_n_sigmas_pion = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, pos_track, AliPID::kPion);

        // both daughters' pid
        bool neg_could_be_proton = std::abs(neg_n_sigmas_proton) < E2T::Cuts::PreFoundLambda::AbsMax_NSigmas_PID;
        bool neg_could_be_pion = std::abs(neg_n_sigmas_pion) < E2T::Cuts::PreFoundLambda::AbsMax_NSigmas_PID;
        bool pos_could_be_proton = std::abs(pos_n_sigmas_proton) < E2T::Cuts::PreFoundLambda::AbsMax_NSigmas_PID;
        bool pos_could_be_pion = std::abs(pos_n_sigmas_pion) < E2T::Cuts::PreFoundLambda::AbsMax_NSigmas_PID;
        bool could_be_lambda = pos_could_be_proton && neg_could_be_pion;
        bool could_be_anti_lambda = neg_could_be_proton && pos_could_be_pion;
        if (!could_be_anti_lambda && !could_be_lambda) continue;  // apply cut
        fHist_PreFoundLambdas_Bookkeeping->Fill(EPreFoundLambda::kPassesPid);

        // daughter's min pt
        if (pos_track->Pt() < E2T::Cuts::PreFoundLambda::Min_Pt_Daughters) continue;  // apply cut
        if (neg_track->Pt() < E2T::Cuts::PreFoundLambda::Min_Pt_Daughters) continue;  // apply cut
        fHist_PreFoundLambdas_Bookkeeping->Fill(EPreFoundLambda::kPassesMinPtDaughters);

        // daughter's max pt
        if (pos_track->Pt() > E2T::Cuts::PreFoundLambda::Max_Pt_Daughters) continue;  // apply cut
        if (neg_track->Pt() > E2T::Cuts::PreFoundLambda::Max_Pt_Daughters) continue;  // apply cut
        fHist_PreFoundLambdas_Bookkeeping->Fill(EPreFoundLambda::kPassesMaxPtDaughters);

        // daughter's pseudorapidity
        if (std::abs(pos_track->Eta()) > E2T::Cuts::PreFoundLambda::AbsMax_Eta_Daughters) continue;  // apply cut
        if (std::abs(neg_track->Eta()) > E2T::Cuts::PreFoundLambda::AbsMax_Eta_Daughters) continue;  // apply cut
        fHist_PreFoundLambdas_Bookkeeping->Fill(EPreFoundLambda::kPassesAbsMaxEtaDaughters);

        // invariant mass
        // -- get daughter's momenta @ pca w.r.t. v0
        v0->GetNPxPyPz(neg_px, neg_py, neg_pz);
        v0->GetPPxPyPz(pos_px, pos_py, pos_pz);
        // -- anti-lambda hypothesis
        lv_antiproton.SetCoordinates(neg_px, neg_py, neg_pz, Common::PdgMass_Proton);
        lv_piplus.SetCoordinates(pos_px, pos_py, pos_pz, Common::PdgMass_Pion);
        double m_antilambda = (lv_antiproton + lv_piplus).M();
        double delta_m_antilambda = std::abs(m_antilambda - Common::PdgMass_Lambda);
        // -- lambda hypothesis
        lv_proton.SetCoordinates(pos_px, pos_py, pos_pz, Common::PdgMass_Proton);
        lv_piminus.SetCoordinates(neg_px, neg_py, neg_pz, Common::PdgMass_Pion);
        double m_lambda = (lv_proton + lv_piminus).M();
        double delta_m_lambda = std::abs(m_lambda - Common::PdgMass_Lambda);
        if (delta_m_antilambda > E2T::Cuts::PreFoundLambda::AbsMax_DeltaInvariantMass &&
            delta_m_lambda > E2T::Cuts::PreFoundLambda::AbsMax_DeltaInvariantMass) {
            continue;  // apply cut
        }
        fHist_PreFoundLambdas_Bookkeeping->Fill(EPreFoundLambda::kPassesInvariantMass);

        // armenteros-podolanski variables
        double arm_alpha = v0->AlphaV0();
        double arm_qt = v0->PtArmV0();
        if (arm_qt / std::abs(arm_alpha) > E2T::Cuts::PreFoundLambda::AbsSlope_ArmQtOverArmAlpha) continue;  // apply cuts
        fHist_PreFoundLambdas_Bookkeeping->Fill(EPreFoundLambda::kPassesArmenterosPodolanski);

#if E2T_VERBOSE
        AliInfoF("id_v0=%i, id_neg=%i, id_pos=%i, delta_mass_as_l=%f, delta_mass_as_al=%f",  //
                 entry_v0, v0->GetNindex(), v0->GetPindex(), delta_m_lambda, delta_m_antilambda);
#endif

        // passed pre-selection, get some more properties //

        // negative daughter
        // -- state + cov. matrix
        const auto *neg_param = v0->GetParamN();
        neg_param->GetXYZ(neg_position);
        neg_param->GetPxPyPz(neg_momentum);
        neg_param->GetCovarianceXYZPxPyPz(neg_cov_xyz_pxpypz);
        // -- pre-calc. dca w.r.t. pv
        neg_track->GetImpactParameters(neg_dca, neg_cov_dca);
        // -- remaining pid as kaon
        float neg_n_sigmas_kaon = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, neg_track, AliPID::kKaon);

        // positive daughter
        // -- state + cov. matrix
        const auto *pos_param = v0->GetParamP();
        pos_param->GetXYZ(pos_position);
        pos_param->GetPxPyPz(pos_momentum);
        pos_param->GetCovarianceXYZPxPyPz(pos_cov_xyz_pxpypz);
        // -- pre-calc. dca w.r.t. pv
        pos_track->GetImpactParameters(pos_dca, pos_cov_dca);
        // -- remaining pid as kaon
        float pos_n_sigmas_kaon = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, pos_track, AliPID::kKaon);

        // create new (anti)lambda candidate //
        POD::PreFoundLambda new_lambda;  // non-initialized on purpose
        new_lambda.PreFoundEntry = static_cast<unsigned int>(entry_v0);
        new_lambda.Decay_X = static_cast<float>(v0->Xv());
        new_lambda.Decay_Y = static_cast<float>(v0->Yv());
        new_lambda.Decay_Z = static_cast<float>(v0->Zv());
        new_lambda.DcaV0Daughters = static_cast<float>(v0->GetDcaV0Daughters());
        // negative daughter //
        new_lambda.Neg_EsdEntry = static_cast<unsigned int>(v0->GetNindex());
        new_lambda.Neg_State = {static_cast<float>(neg_position[0]), static_cast<float>(neg_position[1]), static_cast<float>(neg_position[2]),
                                static_cast<float>(neg_momentum[0]), static_cast<float>(neg_momentum[1]), static_cast<float>(neg_momentum[2])};
        for (std::size_t idx_cov = 0; idx_cov < Common::NCovMatrixComponents_State6; ++idx_cov) {
            new_lambda.Neg_CovMatrix[idx_cov] = static_cast<float>(neg_cov_xyz_pxpypz[idx_cov]);
        }
        new_lambda.Neg_PreDCAxy = std::abs(neg_dca[0]);
        new_lambda.Neg_PreDCAz = std::abs(neg_dca[1]);
        new_lambda.Neg_NSigmasProton = neg_n_sigmas_proton;
        new_lambda.Neg_NSigmasKaon = neg_n_sigmas_kaon;
        new_lambda.Neg_NSigmasPion = neg_n_sigmas_pion;
        // -- related to (anti)lambda
        new_lambda.Neg_PCAwrtV0_Px = static_cast<float>(neg_px);
        new_lambda.Neg_PCAwrtV0_Py = static_cast<float>(neg_py);
        new_lambda.Neg_PCAwrtV0_Pz = static_cast<float>(neg_pz);
#if E2T_TPC_EXTRA
        new_lambda.Neg_TPC_Chi2 = static_cast<float>(neg_track->GetTPCchi2());
        new_lambda.Neg_TPC_NCrossedRows = neg_track->GetTPCCrossedRows();
        new_lambda.Neg_TPC_NClusters = neg_track->GetTPCNcls();
        new_lambda.Neg_TPC_NClustersFindable = neg_track->GetTPCNclsF();
#endif
        // positive daughter //
        new_lambda.Pos_EsdEntry = static_cast<unsigned int>(v0->GetPindex());
        new_lambda.Pos_State = {static_cast<float>(pos_position[0]), static_cast<float>(pos_position[1]), static_cast<float>(pos_position[2]),
                                static_cast<float>(pos_momentum[0]), static_cast<float>(pos_momentum[1]), static_cast<float>(pos_momentum[2])};
        for (std::size_t idx_cov = 0; idx_cov < Common::NCovMatrixComponents_State6; ++idx_cov) {
            new_lambda.Pos_CovMatrix[idx_cov] = static_cast<float>(pos_cov_xyz_pxpypz[idx_cov]);
        }
        new_lambda.Pos_PreDCAxy = std::abs(pos_dca[0]);
        new_lambda.Pos_PreDCAz = std::abs(pos_dca[1]);
        new_lambda.Pos_NSigmasProton = pos_n_sigmas_proton;
        new_lambda.Pos_NSigmasKaon = pos_n_sigmas_kaon;
        new_lambda.Pos_NSigmasPion = pos_n_sigmas_pion;
        // -- related to (anti)lambda
        new_lambda.Pos_PCAwrtV0_Px = static_cast<float>(pos_px);
        new_lambda.Pos_PCAwrtV0_Py = static_cast<float>(pos_py);
        new_lambda.Pos_PCAwrtV0_Pz = static_cast<float>(pos_pz);
#if E2T_TPC_EXTRA
        new_lambda.Pos_TPC_Chi2 = static_cast<float>(pos_track->GetTPCchi2());
        new_lambda.Pos_TPC_NCrossedRows = pos_track->GetTPCCrossedRows();
        new_lambda.Pos_TPC_NClusters = pos_track->GetTPCNcls();
        new_lambda.Pos_TPC_NClustersFindable = pos_track->GetTPCNclsF();
#endif

        // push reconstructed //
        fOutput.PreFoundLambda.emplace_back(new_lambda);

        // push mc info //
        if (fIsMC) {
            fOutput.PreFoundLambda_Neg_McEntry.emplace_back(std::abs(neg_track->GetLabel()));
            fOutput.PreFoundLambda_Pos_McEntry.emplace_back(std::abs(pos_track->GetLabel()));
        }
    }
}

// # Injected Reactions # //

// Store the in-memory values into the tree branches.
void AliTaskEsd2Tree::ProcessInjectedReactions() {
    if (fOutput.Event.EventNumber >= E2T::NEventsInDedicatedMCFile) {
        AliWarning("Invalid EventNumber to read signal logs.");
        return;
    }
    fOutput.InjectedSexa.reserve(E2T::NSexaReactionsPerEvent);  // vector preallocation
    for (int r = 0; r < E2T::NSexaReactionsPerEvent; ++r) {
        fOutput.InjectedSexa.emplace_back(fEvVec_ReactionID[fOutput.Event.EventNumber][r],    //
                                          fEvVec_Sexaquark_Px[fOutput.Event.EventNumber][r],  //
                                          fEvVec_Sexaquark_Py[fOutput.Event.EventNumber][r],  //
                                          fEvVec_Sexaquark_Pz[fOutput.Event.EventNumber][r],  //
                                          fEvVec_Nucleon_Px[fOutput.Event.EventNumber][r],    //
                                          fEvVec_Nucleon_Py[fOutput.Event.EventNumber][r],    //
                                          fEvVec_Nucleon_Pz[fOutput.Event.EventNumber][r]);
    }
}

// Copy to working directory the respective `sim.log` that corresponds to the `RunNumber+DirNumber` that's being analyzed.
// Read the anti-sexaquark and struck nucleon kinematics for each injected reaction from the `sim.log` file that corresponds to an entire dir number
// into memory.
bool AliTaskEsd2Tree::ReadSignalLogs(const TString &sim_sub_set) {

    // (1) Clear signal cache //

    fEvVec_ReactionID = {};
    fEvVec_Sexaquark_Px = {};
    fEvVec_Sexaquark_Py = {};
    fEvVec_Sexaquark_Pz = {};
    fEvVec_Nucleon_Px = {};
    fEvVec_Nucleon_Py = {};
    fEvVec_Nucleon_Pz = {};

    // (2) Bring Signal Logs //

    TString AliEn_Dir = fAliEnPath(0, fAliEnPath.Last('/'));

    TString orig_path = Form("%s/sim.log", AliEn_Dir.Data());
    AliInfoF("Copying file %s ...", orig_path.Data());

    TString SignalLog_NewBasename =
        Form("sim_%s_%i_%03i.log", sim_sub_set.Data(), fOutput.Event.RunNumber, static_cast<int>(fOutput.Event.DirNumber));

    bool copy_succesful = false;
    if (AliEn_Dir.BeginsWith("alien://")) {
        if (TGrid::Connect("alien://") == nullptr) return false;
        copy_succesful = TFile::Cp(Form("%s", orig_path.Data()), Form("file://./%s", SignalLog_NewBasename.Data()));
    } else {
        copy_succesful = gSystem->CopyFile(Form("%s", orig_path.Data()), Form("./%s", SignalLog_NewBasename.Data()), true) == 0;
    }
    if (!copy_succesful) return false;

    // (3) Read signal logs //

    TString new_path = Form("%s/%s", gSystem->pwd(), SignalLog_NewBasename.Data());
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

        // -- a new event has appeared!
        if (line.rfind(E2T::SimLog_EventHeader, 0) == 0) {
            ++event_n;
            react_id = 0;
            continue;
        }

        // -- small protection
        if (event_n < 0) continue;
        if (line.rfind(E2T::SimLog_ReactionMarker, 0) != 0) continue;
        if (event_n >= static_cast<int>(E2T::NEventsInDedicatedMCFile) || react_id >= static_cast<int>(E2T::NSexaReactionsPerEvent)) {
            AliWarningF("sim.log exceeds expected events/reactions (event %i, reaction %i), skipping", event_n, react_id);
            continue;
        }

        std::string data_part = line.substr(E2T::SimLog_ReactionMarker.length() + 1);
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
    }  // finish reading lines

#if E2T_VERBOSE
    for (int ev_print = 0; ev_print < E2T::NEventsInDedicatedMC; ++ev_print) {
        for (int r_print = 0; r_print < E2T::NSexaReactionsPerEvent; ++r_print) {
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
