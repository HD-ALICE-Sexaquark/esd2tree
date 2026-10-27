#pragma once

#include <cmath>
#include <cstddef>
#include <memory>
#include <vector>

#include <AliAnalysisTaskSE.h>
#include <AliESDtrack.h>
#include <AliEventCuts.h>
#include <AliMCParticle.h>

#include "Constants.hpp"
#include "Framework_TeeTree.hpp"
#include "Schema_Events.hpp"

#include "AliTaskEsd2Tree_LinkDef.h"

class TClass;
class TList;
class TFile;
class TH1F;
class TObjArray;
class TObjString;
class TString;
class TTree;

class AliESDEvent;
class AliESDVertex;
class AliMCEvent;
class AliVVertex;
class AliPIDResponse;

class AliTaskEsd2Tree : public AliAnalysisTaskSE {

    // NOTE: no tracks are stored; kept only to book `Tracks_Bookkeeping` (left empty) with the same bins, which T2DS requires
    enum ETrack {
        kAllTracks,
        kValidStatusTPC_1,
        kValidStatusTPC_2,
        kNoHitInITS,
        kPassesMinPreDCAxy,
        kPassesMaxPreDCAxy,
        kPassesMinPreDCAz,
        kPassesMaxPreDCAz,
        kPassesAbsMaxPz,
        kPassesMinPt,
        kPassesMaxPt,
        kPassesAbsMaxEta,
        kPassesMinNTPCClusters,
        kPassesChi2NClusters,
        kNotAKink,
        kHasValidPid,
        kPidWithinRange,
        kNTrackCuts,
    };

    enum EPreFoundLambda {
        kAllPreFoundV0s,
        kOnTheFlyV0s,
        kPassesDcaV0Daughters,
        kPassesMinDecayRadius2D,
        kPassesDCAwrtPV,
        kPassesCPAwrtPV,
        kDaughtersHaveTPCRefit,
        kPassesMinTPCNCrossedRowsDaughters,
        kPassesMinTPCNCrossedRowsNClsFDaughters,
        kPassesMaxTPCChi2NClsDaughters,
        kPassesPid,
        kPassesAbsMaxEtaDaughters,
        kPassesInvariantMass,
        kPassesArmenterosPodolanski,
        kNPreFoundLambdaCuts,
    };

   public:
    AliTaskEsd2Tree();
    AliTaskEsd2Tree(const char* name);
    ~AliTaskEsd2Tree();

    // Settings ~ stored in Analysis Manager //
    void Initialize(bool is_mc, bool is_hdib_mc = false);

    // Main ~ executed at runtime //
    void UserCreateOutputObjects();
    bool UserNotify();
    void UserExec(Option_t* option);
    void Terminate(Option_t* option) {}

    // Events //
    bool ProcessEvent();

    // MC Particles //
    void ProcessMCParticles();

    // (Anti)Lambdas //
    void ProcessPreFoundLambdas();

    // Utilities //
    Common::ECustomGeneratorIdx GetCustomGeneratorIndex(const AliMCParticle* mc) const {
        short orig_gen_idx = mc->GetGeneratorIndex();
        if (fIsMC_DedicatedHdibaryon) {
            if (orig_gen_idx < 6)
                return Common::ECustomGeneratorIdx::kInjectedAuxCharged;
            else
                return Common::ECustomGeneratorIdx::kInjectedHdibaryon;
        }
        return Common::kHijing;  // valid for general-purpose MC
    }
    // Distance in (pt, pz) between a momentum and a track parametrization; both are constant along a helix.
    static double GetMomentumMismatch(const double* momentum, const AliExternalTrackParam& param) {
        return std::hypot(std::hypot(momentum[0], momentum[1]) - param.Pt(), momentum[2] - param.Pz());
    }
    // Track length in the TPC active zone, from the inner param, as in PWGLF: 2 cm dead zone at the sector edges, 220 cm drift length.
    static float GetTPCLength(const AliESDtrack& track, double magnetic_field) {
        // NOTE: AliRoot doesn't protect against a missing inner param
        if (track.GetInnerParam() == nullptr) return Common::DummyFloat;
        return static_cast<float>(track.GetLengthInActiveZone(1, 2., 220., magnetic_field));
    }

   private:
    // Settings ~ stored in Analysis Manager ~ all persistent //
    bool fIsMC;                     // kTRUE if MC simulation, kFALSE if data
    bool fIsMC_DedicatedHdibaryon;  //

    // AliRoot Objects //
    AliMCEvent* fMC;                      //! MC event
    const AliVVertex* fMC_PrimaryVertex;  //! MC gen. (or true) primary vertex
    AliESDEvent* fESD;                    //! reconstructed event
    const AliESDVertex* fPrimaryVertex;   //! primary vertex
    AliPIDResponse* fPIDResponse;         //! pid response object
    AliEventCuts fEventCuts;              //! event cuts

    // Output //

    // -- QA Histograms
    TList* fOutputList;                       //!
    TH1F* fHist_Centrality;                   //!
    TH1F* fHist_CentralityINT7;               //!
    TH1F* fHist_Tracks_Bookkeeping;           //!
    TH1F* fHist_PreFoundLambdas_Bookkeeping;  //!

    // -- Tree
    TTree* fOutputTree;                                   //!
    Schema::Events fOutput;                               //!
    std::unique_ptr<Framework::TeeTree::Writer> fWriter;  //!

    ClassDef(AliTaskEsd2Tree, 3);
};
