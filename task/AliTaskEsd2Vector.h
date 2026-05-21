#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <vector>

#include <AliAnalysisTaskSE.h>
#include <AliESDtrack.h>
#include <AliEventCuts.h>

#include "Constants.hpp"
#include "E2R_Event.hpp"
#include "E2R_InjectedSexa.hpp"
#include "E2R_Lambda.hpp"
#include "E2R_McParticle.hpp"
#include "E2R_Track.hpp"

class TClass;
class TBits;
class TList;
class TFile;
class TGrid;
class TH1F;
class TObjArray;
class TObjString;
class TString;
class TTree;

namespace ROOT {
class RNTupleModel;
class RNTupleWriter;
}  // namespace ROOT

class AliESDEvent;
class AliESDVertex;
class AliMCEvent;
class AliVVertex;
class AliPIDResponse;

class AliTaskEsd2Vector final : public AliAnalysisTaskSE {
   public:
    AliTaskEsd2Vector(const char* name);
    AliTaskEsd2Vector();
    ~AliTaskEsd2Vector();

    AliTaskEsd2Vector(const AliTaskEsd2Vector&) = delete;
    AliTaskEsd2Vector& operator=(const AliTaskEsd2Vector&) = delete;
    AliTaskEsd2Vector(AliTaskEsd2Vector&&) = delete;
    AliTaskEsd2Vector& operator=(AliTaskEsd2Vector&&) = delete;

    // Settings ~ stored in Analysis Manager //
    void Initialize(bool is_mc, bool is_signal_mc = false);

    // Main ~ executed at runtime //
    void UserCreateOutputObjects();
    bool UserNotify();
    void UserExec(Option_t* option);
    void FinishTaskOutput();  // close & write footer per worker
    void Terminate(Option_t* option) {}

    // Tree //
    void CreateEventsBranches(ROOT::RNTupleModel* model);
    void CreateMCBranches(ROOT::RNTupleModel* model);
    void CreateTracksBranches(ROOT::RNTupleModel* model);
    void CreateLambdasBranches(ROOT::RNTupleModel* model);
    void CreateInjectedBranches(ROOT::RNTupleModel* model);

    // Events //
    bool ProcessEvent();
    bool PassesEventSelection();

    // MC Particles //
    void ProcessMCParticles();
    void ReserveBranches_MC(std::size_t size);
    void ClearBranches_MC();

    // Tracks //
    void ProcessTracks();
    bool PassesTrackSelection(const AliESDtrack* track);
    void ReserveBranches_Tracks(std::size_t size);
    void ClearBranches_Tracks();

    // (Anti)Lambdas //
    void ProcessLambdas();
    void ReserveBranches_Lambdas(std::size_t size);
    void ClearBranches_Lambdas();

    // Injected Reactions //
    void ProcessInjectedReactions();
    void BringSignalLogs();
    bool ReadSignalLogs();
    void ReserveBranches_Injected();
    void ClearBranches_Injected();

    // Utilities //
    static int GetFirstRowFromTPCClusterMap(const AliESDtrack* track) {
        const TBits& clMap = track->GetTPCClusterMap();
        int firstRow = -1;  // default value
        for (int i = 0; i < 159; ++i) {
            if (clMap.TestBitNumber(i)) {
                firstRow = i;
                break;
            }
        }
        return firstRow;
    }

    static int GetFirstLayerFromITS(const AliESDtrack* track) {
        for (int iLayer = 0; iLayer < 6; ++iLayer) {
            if (track->HasPointOnITSLayer(iLayer)) return iLayer;
        }
        return -1;  // no hit in any layer
    }

   private:
    // Settings ~ stored in Analysis Manager ~ all persistent //
    bool fIsMC;        // kTRUE if MC simulation, kFALSE if data
    bool fIsSignalMC;  // kTRUE to read and load signal logs

    // AliRoot Objects //
    AliMCEvent* fMC;                      //! MC event
    const AliVVertex* fMC_PrimaryVertex;  //! MC gen. (or true) primary vertex
    AliESDEvent* fESD;                    //! reconstructed event
    const AliESDVertex* fPrimaryVertex;   //! primary vertex
    AliPIDResponse* fPIDResponse;         //! pid response object
    AliEventCuts fEventCuts;              //! event cuts

    // Signal Logs //
    TString fAliEnPath;                                                                                     //! loaded in `UserNotify()`
    TString fSignalLog_NewBasename;                                                                         //!
    std::array<std::array<int, E2R::NReactionsPerEvent>, E2R::NEventsInDedicatedMC> fEvVec_ReactionID;      //!
    std::array<std::array<float, E2R::NReactionsPerEvent>, E2R::NEventsInDedicatedMC> fEvVec_Sexaquark_Px;  //!
    std::array<std::array<float, E2R::NReactionsPerEvent>, E2R::NEventsInDedicatedMC> fEvVec_Sexaquark_Py;  //!
    std::array<std::array<float, E2R::NReactionsPerEvent>, E2R::NEventsInDedicatedMC> fEvVec_Sexaquark_Pz;  //!
    std::array<std::array<float, E2R::NReactionsPerEvent>, E2R::NEventsInDedicatedMC> fEvVec_Nucleon_Px;    //!
    std::array<std::array<float, E2R::NReactionsPerEvent>, E2R::NEventsInDedicatedMC> fEvVec_Nucleon_Py;    //!
    std::array<std::array<float, E2R::NReactionsPerEvent>, E2R::NEventsInDedicatedMC> fEvVec_Nucleon_Pz;    //!

    // Output //

    // -- QA Histograms
    TList* fOutputList;              //!
    TH1F* fHist_Events_Bookkeeping;  //!
    TH1F* fHist_Centrality;          //!
    TH1F* fHist_CentralityINT7;      //!
    TH1F* fHist_Tracks_Bookkeeping;  //!

    // -- RNTuple
    TFile* fOutput_AltFile;                        //! output file owned by the task
    std::unique_ptr<ROOT::RNTupleWriter> fWriter;  //!
    E2R::Event fEvent;                             //!
    E2R::InjectedSexa fInjectedSexa;               //!
    E2R::McParticle fMcParticle;                   //!
    E2R::Track fTrack;                             //!
    E2R::Lambda fLambda;                           //!

    ClassDef(AliTaskEsd2Vector, 2);  // = number of persistent members
};
