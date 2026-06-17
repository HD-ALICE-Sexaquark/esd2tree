#include <cstdint>
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
#include "Math.hpp"

#include "AliTaskEsd2Vector.h"

ClassImp(AliTaskEsd2Vector);

// Constructor, called locally.
AliTaskEsd2Vector::AliTaskEsd2Vector(const char *name)
    : AliAnalysisTaskSE{name},
      //
      fIsMC{false},
      fIsMC_DedicatedSexaquark{false},
      fIsFirstEvent{true},
      //
      fMC{nullptr},
      fMC_PrimaryVertex{nullptr},
      fESD{nullptr},
      fPrimaryVertex{nullptr},
      fPIDResponse{nullptr},
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
      fOutput{},
      fWriter{nullptr} {
    DefineInput(0, TChain::Class());
    DefineOutput(1, TList::Class());  // fOutputList
}

// Empty I/O constructor. Non-persistent members are initialized to their default values from here.
AliTaskEsd2Vector::AliTaskEsd2Vector() : AliTaskEsd2Vector{""} {}

// Destructor.
// NOTE: if `TList::SetOwner(true)` was called, the TList destructor should delete all objects added to it.
AliTaskEsd2Vector::~AliTaskEsd2Vector() { delete fOutputList; }

// Initialize analysis task. Needs to be called within an `AddTaskEsd2Vector.C` macro.
void AliTaskEsd2Vector::Initialize(bool is_mc, bool is_sexa_mc, bool is_hdib_mc) {
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

void AliTaskEsd2Vector::FinishTaskOutput() {
    fWriter->fRNT_Writer.reset();
    fOutput_AltFile->Write();
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

    fOutput_AltFile = std::make_unique<TFile>("EventsRNT.root", "RECREATE");
    if (!fOutput_AltFile || fOutput_AltFile->IsZombie()) AliFatal("Cannot open EventsRNT.root");

    // Prepare output RNTuple //

    ROOT::RNTupleWriteOptions rnt_opts;
    rnt_opts.SetCompression(ROOT::RCompressionSetting::EDefaults::kUseSmallest);

    fWriter =
        std::make_unique<Framework::Writer>(fOutput.CreateModel(fIsMC, fIsMC_DedicatedSexaquark), E2R::Name_OutputRNT, *fOutput_AltFile, rnt_opts);

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
        fOutput.Event.RunNumber = (dynamic_cast<TObjString *>(tokens->At(tokens->GetEntries() - 3)))->GetString().Atoi();
        fOutput.Event.DirNumber = (dynamic_cast<TObjString *>(tokens->At(tokens->GetEntries() - 2)))->GetString().Atoi();
        fOutput.Event.DirNumberB = 0;
        AliInfoF("Run Number = %04i", static_cast<int>(fOutput.Event.RunNumber));  // = 1
        AliInfoF("Dir Number = %04i", static_cast<int>(fOutput.Event.DirNumber));  // = 1
        if (fIsMC_DedicatedSexaquark) {
            BringSignalLogs();
            ReadSignalLogs();
        }
    } else {
        // NOTE: path of real data ends with format `.../LHC15o/000245232/pass2/15000245232039.914/AliESDs.root`
        fOutput.Event.RunNumber = (dynamic_cast<TObjString *>(tokens->At(tokens->GetEntries() - 4)))->GetString().Atoi();
        AliInfoF("Run Number = %i", fOutput.Event.RunNumber);
        auto aux_dir_nr = (dynamic_cast<TObjString *>(tokens->At(tokens->GetEntries() - 2)))->GetString();
        aux_dir_nr = TString(aux_dir_nr(2 + 3 + 6, 10));              // = "039.914"
        fOutput.Event.DirNumber = TString(aux_dir_nr(0, 3)).Atoi();   // = 39
        fOutput.Event.DirNumberB = TString(aux_dir_nr(4, 5)).Atoi();  // = 914
        AliInfoF("Dir Number = %i", fOutput.Event.DirNumber);
        AliInfoF("Dir Number B = %i", fOutput.Event.DirNumberB);
    }
    delete tokens;  // NOTE: because ROOT

    // Adapt event cuts  //

    // For real data, depending on run number
    // Reference: https://twiki.cern.ch/twiki/bin/viewauth/ALICE/AliDPGRunList18r1
    if (!fIsMC && (fOutput.Event.RunNumber == 296749 || fOutput.Event.RunNumber == 296750 || fOutput.Event.RunNumber == 296849 ||
                   fOutput.Event.RunNumber == 296890 || fOutput.Event.RunNumber == 297029 || fOutput.Event.RunNumber == 297194 ||
                   fOutput.Event.RunNumber == 297219 || fOutput.Event.RunNumber == 297481)) {
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
    if (fIsFirstEvent) fIsFirstEvent = false;

    // post data //

    PostData(1, fOutputList);
}

// # Events # //

bool AliTaskEsd2Vector::ProcessEvent() {

#if E2R_VERBOSE
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
    fOutput.Event.EventNumber = fESD->GetEventNumberInFile();
    fPrimaryVertex = fESD->GetPrimaryVertex();

    // Apply selection //

    if (!fIsMC_DedicatedHdibaryon && !PassesEventSelection()) return false;

    // Assign more branches (2) //

    auto *MultSelection = dynamic_cast<AliMultSelection *>(fESD->FindListObject("MultSelection"));
    if (MultSelection == nullptr) AliFatal("AliMultSelection couldn't be found.");
    fOutput.Event.Centrality = MultSelection->GetMultiplicityPercentile("V0M");
    fOutput.Event.MagneticField = static_cast<float>(fESD->GetMagneticField());

    // Fill QA hist //

    fHist_Centrality->Fill(fOutput.Event.Centrality);
    if ((fInputHandler->IsEventSelected() & AliVEvent::kINT7) != 0U) fHist_CentralityINT7->Fill(fOutput.Event.Centrality);

    // Assign rest of branches (3) //

    fOutput.Event.PV_X = static_cast<float>(fPrimaryVertex->GetX());
    fOutput.Event.PV_Y = static_cast<float>(fPrimaryVertex->GetY());
    fOutput.Event.PV_Z = static_cast<float>(fPrimaryVertex->GetZ());

    fOutput.Event.PV_NContributors = fPrimaryVertex->GetNContributors();
    fOutput.Event.PV_Dispersion = static_cast<float>(fPrimaryVertex->GetDispersion());

    double PV_CovMatrix[6];  // non-initialized on purpose
    fPrimaryVertex->GetCovarianceMatrix(PV_CovMatrix);
    fOutput.Event.PV_CovMatrix = {static_cast<float>(PV_CovMatrix[0]), static_cast<float>(PV_CovMatrix[1]), static_cast<float>(PV_CovMatrix[2]),
                                  static_cast<float>(PV_CovMatrix[3]), static_cast<float>(PV_CovMatrix[4]), static_cast<float>(PV_CovMatrix[5])};

    const auto *PrimaryVertex_SPD = fESD->GetPrimaryVertexSPD();
    fOutput.Event.SPD_PV_Z = static_cast<float>(PrimaryVertex_SPD->GetZ());

    double PV_SPD_CovMatrix[6];  // non-initialized on purpose
    PrimaryVertex_SPD->GetCovarianceMatrix(PV_SPD_CovMatrix);
    fOutput.Event.SPD_PV_ZErr = static_cast<float>(PV_SPD_CovMatrix[5]);

    fOutput.Event.NTracks = fESD->GetNumberOfTracks();
    fOutput.Event.NTPCClusters = fESD->GetNumberOfTPCClusters();
    fOutput.Event.IsMB = ((fInputHandler->IsEventSelected() & AliVEvent::kINT7) != 0U);
    fOutput.Event.IsCentral = ((fInputHandler->IsEventSelected() & AliVEvent::kCentral) != 0U);
    fOutput.Event.IsSemiCentral = ((fInputHandler->IsEventSelected() & AliVEvent::kSemiCentral) != 0U);

    // Assign MC branches (4) //

    if (fIsMC) {
        fMC_PrimaryVertex = fMC->GetPrimaryVertex();
        fOutput.MC_Event.PV_X = static_cast<float>(fMC_PrimaryVertex->GetX());
        fOutput.MC_Event.PV_Y = static_cast<float>(fMC_PrimaryVertex->GetY());
        fOutput.MC_Event.PV_Z = static_cast<float>(fMC_PrimaryVertex->GetZ());
        fOutput.MC_Event.IsGenPileup = !fIsMC_DedicatedHdibaryon ? AliAnalysisUtils::IsPileupInGeneratedEvent(fMC, "Hijing") : false;
        fOutput.MC_Event.IsSBCPileup = !fIsMC_DedicatedHdibaryon ? AliAnalysisUtils::IsSameBunchPileupInGeneratedEvent(fMC, "Hijing") : false;
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
    // if (!fIsMC && fESD->GetHeader()->GetEventType() != 7) return false;
    // fHist_Events_Bookkeeping->Fill(4.);
    // Trigger Selection //
    bool IsMB = (fInputHandler->IsEventSelected() & AliVEvent::kINT7) != 0U;
    bool IsCentral = (fInputHandler->IsEventSelected() & AliVEvent::kCentral) != 0U;
    bool IsSemiCentral = (fInputHandler->IsEventSelected() & AliVEvent::kSemiCentral) != 0U;
    if (!IsMB && !IsCentral && !IsSemiCentral) return false;
    fHist_Events_Bookkeeping->Fill(5.);
    // rec. PV z-vertex range //
    if (std::abs(fPrimaryVertex->GetZ()) > E2R::Cuts::AbsMax_PV_Z) return false;
    fHist_Events_Bookkeeping->Fill(6.);

    return true;
}

// # MC Generated # //

// Loop over MC particles in a single event.
void AliTaskEsd2Vector::ProcessMCParticles() {

    // vector preallocation //
    const int n_mc = fMC->GetNumberOfTracks();
    if (fIsFirstEvent) fOutput.McParticle.reserve(n_mc);

    // read mc particles //
    for (int mc_entry = 0; mc_entry < n_mc; ++mc_entry) {
        auto *mcPart = dynamic_cast<AliMCParticle *>(fMC->GetTrack(mc_entry));
        if (mcPart == nullptr) continue;

        // create new mc //
        POD::McParticle new_mc;
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
        new_mc.Generator = static_cast<std::uint8_t>(mcPart->GetGeneratorIndex());
        new_mc.IsPhysPrimary = mcPart->IsPhysicalPrimary();
        new_mc.IsSecFromMat = mcPart->IsSecondaryFromMaterial();
        new_mc.IsSecFromWeak = mcPart->IsSecondaryFromWeakDecay();
#if E2R_VERBOSE
        if (mcPart->GetGeneratorIndex() == 2) {
            AliInfoF("mc_entry=%i,mc_pdg=%i,mc_is_physprim=%i,mc_status=%i,mother_entry=%i",  //
                     mc_entry, mcPart->PdgCode(), mcPart->IsPhysicalPrimary(), mcPart->MCStatusCode(), mcPart->GetMother());
        }
#endif
#if E2R_MC_EXTRA
        new_mc.IsOOBPileup = AliAnalysisUtils::IsParticleFromOutOfBunchPileupCollision(mc_entry, fMC);
#endif

        // push //
        fOutput.McParticle.emplace_back(new_mc);
    }  // end of loop over MC particles
}

// # Reconstructed # //

// Loop over the reconstructed tracks in a single event.
void AliTaskEsd2Vector::ProcessTracks() {

    // vector preallocation //
    const int n_tracks = fESD->GetNumberOfTracks();
    if (fIsFirstEvent) {
        fOutput.Track.reserve(n_tracks);
        if (fIsMC) fOutput.Track_McEntry.reserve(n_tracks);
    }

    // read tracks //
    for (int esd_entry = 0; esd_entry < n_tracks; ++esd_entry) {
        // get track //
        auto *track = fESD->GetTrack(esd_entry);
        // track selection //
        if (!PassesTrackSelection(track)) continue;
        // get info //
        const auto *inner_param = track->GetInnerParam();  // NOTE: already protected in `PassesTrackSelection`
        if (inner_param == nullptr) continue;
        double position[3];  // non-initialized on purpose
        inner_param->GetXYZ(position);
        double momentum[3];  // non-initialized on purpose
        inner_param->GetPxPyPz(momentum);
        float dca[2], dca_cov[3];  // non-initialized on purpose
        track->GetImpactParameters(dca, dca_cov);
        double cov_xyz_pxpypz[Common::NCovMatrixComponents_State6];  // non-initialized on purpose
        track->GetCovarianceXYZPxPyPz(cov_xyz_pxpypz);

        // create new //
        POD::Track new_track;  // non-initialized on purpose
        new_track.EsdEntry = static_cast<unsigned int>(esd_entry);
        new_track.X = static_cast<float>(position[0]);
        new_track.Y = static_cast<float>(position[1]);
        new_track.Z = static_cast<float>(position[2]);
        new_track.Px = static_cast<float>(momentum[0]);
        new_track.Py = static_cast<float>(momentum[1]);
        new_track.Pz = static_cast<float>(momentum[2]);
        new_track.Charge = inner_param->Charge();
        // -- dca //
        new_track.PreDCAxy = dca[0];
        new_track.PreDCAz = dca[1];
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
#if !E2R_TPC_EXTRA
        if (fIsMC) {
            tpc_signal = track->GetTPCsignalTunedOnData();
        } else {
            tpc_signal = e_sigma_pion * n_sigmas_pion + bethe_pion;
        }
#else
        tpc_signal = track->GetTPCsignal();  // NOTE: uncorrected, store `Corrected` and `TunedOnData` in other branches
#endif
        new_track.TPC_Signal = static_cast<float>(tpc_signal);
        new_track.ITS_FirstLayer = GetFirstLayerFromITS(track);
        new_track.NSigmaPion = static_cast<float>(n_sigmas_pion);
        new_track.NSigmaKaon = static_cast<float>(n_sigmas_kaon);
        new_track.NSigmaProton = static_cast<float>(n_sigmas_proton);
        // -- cov. matrix //
        for (std::size_t idx_cov = 0; idx_cov < Common::NCovMatrixComponents_State6; ++idx_cov) {
            new_track.CovMatrix[idx_cov] = static_cast<float>(cov_xyz_pxpypz[idx_cov]);
        }
#if E2R_TPC_EXTRA
        // -- get tpc dca
        float tpc_dca_xy, tpc_dca_z;
        track->GetImpactParametersTPC(tpc_dca_xy, tpc_dca_z);
        // -- fill tpc branches (1)
        new_track.TPC_DCAxy = tpc_dca_xy;
        new_track.TPC_DCAz = tpc_dca_z;
        new_track.TPC_NCrossedRows = track->GetTPCCrossedRows();
        new_track.TPC_NClusters = static_cast<unsigned int>(track->GetTPCNcls());
        new_track.TPC_NClustersLC = static_cast<unsigned int>(track->GetTPCncls());
        new_track.TPC_NClustersFound = static_cast<unsigned int>(track->GetTPCNclsF());
        new_track.TPC_NClustersShared = static_cast<unsigned int>(track->GetTPCnclsS());
        new_track.TPC_Chi2 = static_cast<float>(track->GetTPCchi2());
        new_track.TPC_Chi2Constrained = static_cast<float>(track->GetConstrainedChi2TPC());
        new_track.TPC_Chi2TCVG = static_cast<float>(track->GetChi2TPCConstrainedVsGlobal(fPrimaryVertex));
        if (fIsMC) new_track.TPC_SignalTunedOnData = static_cast<float>(track->GetTPCsignalTunedOnData());
        new_track.TPC_SignalSigma = static_cast<float>(track->GetTPCsignalSigma());
        // -- get tpc pid info
        auto bethe_kaon = static_cast<double>(fPIDResponse->GetExpectedSignal(AliPIDResponse::kTPC, track, AliPID::kKaon));
        auto e_sigma_kaon = static_cast<double>(fPIDResponse->GetExpectedSigma(AliPIDResponse::kTPC, track, AliPID::kKaon));
        auto bethe_proton = static_cast<double>(fPIDResponse->GetExpectedSignal(AliPIDResponse::kTPC, track, AliPID::kProton));
        auto e_sigma_proton = static_cast<double>(fPIDResponse->GetExpectedSigma(AliPIDResponse::kTPC, track, AliPID::kProton));
        //    -- pion
        new_track.TPC_SignalCorrected = static_cast<float>(e_sigma_pion * n_sigmas_pion + bethe_pion);
        new_track.TPC_ESignalPion = static_cast<float>(bethe_pion);
        new_track.TPC_ESigmaPion = static_cast<float>(e_sigma_pion);
        //    -- kaon
        new_track.TPC_ESignalKaon = static_cast<float>(bethe_kaon);
        new_track.TPC_ESigmaKaon = static_cast<float>(e_sigma_kaon);
        //    -- proton
        new_track.TPC_ESignalProton = static_cast<float>(bethe_proton);
        new_track.TPC_ESigmaProton = static_cast<float>(e_sigma_proton);
        // -- fill tpc branches (2)
        new_track.TPC_SignalN = static_cast<unsigned int>(track->GetTPCsignalN());
        new_track.TPC_PointsFirst = static_cast<float>(track->GetTPCPoints(0));
        new_track.TPC_PointsIndexMax = static_cast<float>(track->GetTPCPoints(1));
        new_track.TPC_PointsLast = static_cast<float>(track->GetTPCPoints(2));
        new_track.TPC_PointsMaxDens = static_cast<float>(track->GetTPCPoints(3));
        new_track.TPC_FirstRow = GetFirstRowFromTPCClusterMap(track);
#endif

        // push reconstructed //
        fOutput.Track.emplace_back(new_track);
        // push mc info //
        if (fIsMC) fOutput.Track_McEntry.emplace_back(std::abs(track->GetLabel()));
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

// # Lambdas # //

void AliTaskEsd2Vector::ProcessPreFoundLambdas() {

    // vector preallocation //
    const int n_v0s = fESD->GetNumberOfV0s();
    if (fIsFirstEvent) {
        fOutput.PreFoundLambda.reserve(n_v0s);
        if (fIsMC) {
            fOutput.PreFoundLambda_Neg_McEntry.reserve(n_v0s);
            fOutput.PreFoundLambda_Pos_McEntry.reserve(n_v0s);
        }
    }

    // loop over pre-found v0s //
    for (int entry_v0 = 0; entry_v0 < n_v0s; ++entry_v0) {
        auto *v0 = fESD->GetV0(entry_v0);
        if (v0 == nullptr) continue;

        // apply cuts (1) -- choose only on-the-fly v0s //

        if (!v0->GetOnFlyStatus()) continue;

        // get daughters' info //

        // negative daughter
        // -- state + cov. matrix
        const auto *neg_param = v0->GetParamN();
        double neg_position[3];  // non-initialized on purpose
        neg_param->GetXYZ(neg_position);
        double neg_momentum[3];  // non-initialized on purpose
        neg_param->GetPxPyPz(neg_momentum);
        double neg_cov_xyz_pxpypz[Common::NCovMatrixComponents_State6];  // non-initialized on purpose
        neg_param->GetCovarianceXYZPxPyPz(neg_cov_xyz_pxpypz);
        // -- dca w.r.t. pv
        float neg_dca[2], neg_cov_dca[3];  // non-initialized on purpose
        neg_param->GetImpactParameters(neg_dca, neg_cov_dca);
        // -- pid
        auto *neg_track = fESD->GetTrack(v0->GetNindex());
        float neg_n_sigma_proton = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, neg_track, AliPID::kProton);
        float neg_n_sigma_kaon = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, neg_track, AliPID::kKaon);
        float neg_n_sigma_pion = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, neg_track, AliPID::kPion);
        // -- momentum @ pca w.r.t. v0
        double neg_px, neg_py, neg_pz;
        v0->GetNPxPyPz(neg_px, neg_py, neg_pz);

        // positive daughter
        // -- state + cov. matrix
        const auto *pos_param = v0->GetParamP();
        double pos_position[3];  // non-initialized on purpose
        pos_param->GetXYZ(pos_position);
        double pos_momentum[3];  // non-initialized on purpose
        pos_param->GetPxPyPz(pos_momentum);
        double pos_cov_xyz_pxpypz[Common::NCovMatrixComponents_State6];  // non-initialized on purpose
        pos_param->GetCovarianceXYZPxPyPz(pos_cov_xyz_pxpypz);
        // -- dca w.r.t. pv
        float pos_dca[2], pos_cov_dca[3];  // non-initialized on purpose
        pos_param->GetImpactParameters(pos_dca, pos_cov_dca);
        //-- pid
        auto *pos_track = fESD->GetTrack(v0->GetPindex());
        float pos_n_sigma_proton = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, pos_track, AliPID::kProton);
        float pos_n_sigma_kaon = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, pos_track, AliPID::kKaon);
        float pos_n_sigma_pion = fPIDResponse->NumberOfSigmas(AliPIDResponse::kTPC, pos_track, AliPID::kPion);
        //-- momentum @ pca w.r.t. v0
        double pos_px, pos_py, pos_pz;
        v0->GetPPxPyPz(pos_px, pos_py, pos_pz);

        // reconstruct v0 //

        TLorentzVector lv_antiproton(neg_px, neg_py, neg_pz, Common::Math::Hypot4(neg_px, neg_py, neg_pz, Common::PdgMass_Proton));
        TLorentzVector lv_piminus(neg_px, neg_py, neg_pz, Common::Math::Hypot4(neg_px, neg_py, neg_pz, Common::PdgMass_Pion));
        TLorentzVector lv_proton(pos_px, pos_py, pos_pz, Common::Math::Hypot4(pos_px, pos_py, pos_pz, Common::PdgMass_Proton));
        TLorentzVector lv_piplus(pos_px, pos_py, pos_pz, Common::Math::Hypot4(pos_px, pos_py, pos_pz, Common::PdgMass_Pion));
        TLorentzVector lv_antilambda = lv_antiproton + lv_piplus;
        TLorentzVector lv_lambda = lv_proton + lv_piminus;

        double delta_m_antilambda = std::abs(lv_antilambda.M() - Common::PdgMass_Lambda);
        double delta_m_lambda = std::abs(lv_lambda.M() - Common::PdgMass_Lambda);

        bool neg_could_be_proton = std::abs(neg_n_sigma_proton) < 3.;
        bool neg_could_be_pion = std::abs(neg_n_sigma_pion) < 3.;
        bool pos_could_be_proton = std::abs(pos_n_sigma_proton) < 3.;
        bool pos_could_be_pion = std::abs(pos_n_sigma_pion) < 3.;
        bool could_be_lambda = pos_could_be_proton && neg_could_be_pion;
        bool could_be_anti_lambda = neg_could_be_proton && pos_could_be_pion;
        if (!could_be_anti_lambda && !could_be_lambda) continue;

        // apply cuts (2) -- pre-selection //
        // -- as anti-lambda
        double antilambda_arm_alpha = Common::Math::ArmenterosAlpha(lv_antilambda.Px(), lv_antilambda.Py(), lv_antilambda.Pz(),  //
                                                                    lv_antiproton.Px(), lv_antiproton.Py(), lv_antiproton.Pz(),  //
                                                                    lv_piplus.Px(), lv_piplus.Py(), lv_piplus.Pz())
                                          .value_or(Common::DummyDouble);
        double antilambda_arm_qt = Common::Math::ArmenterosQt(lv_antilambda.Px(), lv_antilambda.Py(), lv_antilambda.Pz(),  //
                                                              lv_antiproton.Px(), lv_antiproton.Py(), lv_antiproton.Pz());
        bool antilambda_valid_arm = antilambda_arm_qt / std::abs(antilambda_arm_alpha) < 0.2;
        // -- as lambda
        double lambda_arm_alpha = Common::Math::ArmenterosAlpha(lv_lambda.Px(), lv_lambda.Py(), lv_lambda.Pz(),     //
                                                                lv_piminus.Px(), lv_piminus.Py(), lv_piminus.Pz(),  //
                                                                lv_proton.Px(), lv_proton.Py(), lv_proton.Pz())
                                      .value_or(Common::DummyDouble);
        double lambda_arm_qt = Common::Math::ArmenterosQt(lv_lambda.Px(), lv_lambda.Py(), lv_lambda.Pz(),  //
                                                          lv_piminus.Px(), lv_piminus.Py(), lv_piminus.Pz());
        bool lambda_valid_arm = lambda_arm_qt / std::abs(lambda_arm_alpha) < 0.2;

        if (!antilambda_valid_arm && !lambda_valid_arm) continue;

#if E2R_VERBOSE
        AliInfoF("id_v0=%i, id_neg=%i, id_pos=%i, mass_as_l=%f, mass_as_al=%f", entry_v0, v0->GetNindex(), v0->GetPindex(), lv_lambda.M(),
                 lv_antilambda.M());
#endif

        // create new //
        POD::PreFoundLambda new_lambda;  // non-initialized on purpose
        new_lambda.PreFoundEntry = static_cast<unsigned int>(entry_v0);
        new_lambda.Decay_X = static_cast<float>(v0->Xv());
        new_lambda.Decay_Y = static_cast<float>(v0->Yv());
        new_lambda.Decay_Z = static_cast<float>(v0->Zv());
        new_lambda.DcaV0Daughters = static_cast<float>(v0->GetDcaV0Daughters());
        // negative daughter
        new_lambda.Neg_EsdEntry = static_cast<unsigned int>(v0->GetNindex());
        new_lambda.Neg_State = {static_cast<float>(neg_position[0]), static_cast<float>(neg_position[1]), static_cast<float>(neg_position[2]),
                                static_cast<float>(neg_momentum[0]), static_cast<float>(neg_momentum[1]), static_cast<float>(neg_momentum[2])};
        for (std::size_t idx_cov = 0; idx_cov < Common::NCovMatrixComponents_State6; ++idx_cov) {
            new_lambda.Neg_CovMatrix[idx_cov] = static_cast<float>(neg_cov_xyz_pxpypz[idx_cov]);
        }
        new_lambda.Neg_PreDCAxy = neg_dca[0];
        new_lambda.Neg_PreDCAz = neg_dca[1];
        new_lambda.Neg_NSigmaProton = neg_n_sigma_proton;
        new_lambda.Neg_NSigmaKaon = neg_n_sigma_kaon;
        new_lambda.Neg_NSigmaPion = neg_n_sigma_pion;
        // -- related to (anti)lambda
        new_lambda.Neg_PCAwrtV0_Px = static_cast<float>(neg_px);
        new_lambda.Neg_PCAwrtV0_Py = static_cast<float>(neg_py);
        new_lambda.Neg_PCAwrtV0_Pz = static_cast<float>(neg_pz);
        // positive daughter
        new_lambda.Pos_EsdEntry = static_cast<unsigned int>(v0->GetPindex());
        new_lambda.Pos_State = {static_cast<float>(pos_position[0]), static_cast<float>(pos_position[1]), static_cast<float>(pos_position[2]),
                                static_cast<float>(pos_momentum[0]), static_cast<float>(pos_momentum[1]), static_cast<float>(pos_momentum[2])};
        for (std::size_t idx_cov = 0; idx_cov < Common::NCovMatrixComponents_State6; ++idx_cov) {
            new_lambda.Pos_CovMatrix[idx_cov] = static_cast<float>(pos_cov_xyz_pxpypz[idx_cov]);
        }
        new_lambda.Pos_PreDCAxy = pos_dca[0];
        new_lambda.Pos_PreDCAz = pos_dca[1];
        new_lambda.Pos_NSigmaProton = pos_n_sigma_proton;
        new_lambda.Pos_NSigmaKaon = pos_n_sigma_kaon;
        new_lambda.Pos_NSigmaPion = pos_n_sigma_pion;
        // -- related to (anti)lambda
        new_lambda.Pos_PCAwrtV0_Px = static_cast<float>(pos_px);
        new_lambda.Pos_PCAwrtV0_Py = static_cast<float>(pos_py);
        new_lambda.Pos_PCAwrtV0_Pz = static_cast<float>(pos_pz);

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
void AliTaskEsd2Vector::ProcessInjectedReactions() {
    // vector preallocation //
    fOutput.InjectedSexa.reserve(E2R::NSexaReactionsPerEvent);

    for (int r = 0; r < E2R::NSexaReactionsPerEvent; ++r) {
        // create new //
        POD::InjectedSexa new_injected{fEvVec_ReactionID[fOutput.Event.EventNumber][r],    //
                                       fEvVec_Sexaquark_Px[fOutput.Event.EventNumber][r],  //
                                       fEvVec_Sexaquark_Py[fOutput.Event.EventNumber][r],  //
                                       fEvVec_Sexaquark_Pz[fOutput.Event.EventNumber][r],  //
                                       fEvVec_Nucleon_Px[fOutput.Event.EventNumber][r],    //
                                       fEvVec_Nucleon_Py[fOutput.Event.EventNumber][r],    //
                                       fEvVec_Nucleon_Pz[fOutput.Event.EventNumber][r]};

        // push //
        fOutput.InjectedSexa.emplace_back(new_injected);
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
    auto AliEn_DirNumber = static_cast<int>(fOutput.Event.DirNumber);

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

#if E2R_VERBOSE
    for (int ev_print = 0; ev_print < E2R::NEventsInDedicatedMC; ++ev_print) {
        for (int r_print = 0; r_print < E2R::NSexaReactionsPerEvent; ++r_print) {
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
