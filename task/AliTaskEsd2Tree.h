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
class TBits;
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

    // NOTE: frozen, as the antisexaquark real-data trees (`f1ba831`); when tracks aren't processed, `Tracks_Bookkeeping` is still booked
    //       with these bins, and left empty, because T2DS requires it
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
    void Initialize(bool is_mc, bool is_sexa_mc = false, bool is_hdib_mc = false, bool is_embedded = false, bool process_tracks = false);

    // Main ~ executed at runtime //
    void UserCreateOutputObjects();
    bool UserNotify();
    void UserExec(Option_t* option);
    void Terminate(Option_t* option) {}

    // Events //
    bool ProcessEvent();

    // MC Particles //
    void ProcessMCParticles();

    // Tracks //
    void ProcessTracks();

    // (Anti)Lambdas //
    void ProcessPreFoundLambdas();

    // Utilities //
    Common::ECustomGeneratorIdx GetCustomGeneratorIndex(const AliMCParticle* mc) const {
        if (!fIsMC_DedicatedSexaquark && !fIsMC_DedicatedHdibaryon) return Common::kHijing;  // valid for general-purpose MC
        // both dedicated MC productions share the generator layout, and differ only in what they inject
        Common::ECustomGeneratorIdx injected =
            fIsMC_DedicatedSexaquark ? Common::ECustomGeneratorIdx::kInjectedAntiSexaquarkReaction : Common::ECustomGeneratorIdx::kInjectedHdibaryon;
        short orig_gen_idx = mc->GetGeneratorIndex();
        // `embedded`: HIJING (0) + injections (1)
        if (fIsMC_Embedded) {
            if (orig_gen_idx == 0)
                return Common::ECustomGeneratorIdx::kHijing;
            else
                return injected;
        }
        // `signal`: boxes of auxiliary charged primaries (0-5) + injections (6)
        if (orig_gen_idx < 6)
            return Common::ECustomGeneratorIdx::kInjectedAuxCharged;
        else
            return injected;
    }
    static int GetFirstRow(const TBits& map) {
        int firstRow = -1;  // default value
        for (int i = 0; i < 159; ++i) {
            if (map.TestBitNumber(i)) {
                firstRow = i;
                break;
            }
        }
        return firstRow;
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
    bool fIsMC_DedicatedSexaquark;  // kTRUE if antisexaquark MC (LHC23l1ab4), either variant
    bool fIsMC_DedicatedHdibaryon;  // kTRUE if h-dibaryon MC (LHC26h2), either variant
    bool fIsMC_Embedded;            // kTRUE if a dedicated MC's `embedded` variant (HIJING + injections), kFALSE if `signal` (boxes + injections)
    bool fProcessTracks;            // kTRUE to select and store tracks

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

    ClassDef(AliTaskEsd2Tree, 5);
};
