#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <vector>

#include <AliAnalysisTaskSE.h>
#include <AliESDtrack.h>
#include <AliEventCuts.h>

#include "Constants.hpp"
#include "Framework_TeeTree.hpp"
#include "Schema_Events.hpp"

#include "AliTaskEsd2Tree_LinkDef.h"

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

class AliESDEvent;
class AliESDVertex;
class AliMCEvent;
class AliVVertex;
class AliPIDResponse;

class AliTaskEsd2Tree : public AliAnalysisTaskSE {

    enum ETrack {
        kAllTracks,
        kValidStatusTPC_1,
        kValidStatusTPC_2,
        kNoHitInITS,
        kPassesDcaCuts,
        kPassesAbsMaxPz,
        kPassesMinPt,
        kPassesMaxPt,
        kPassesAbsMaxEta,
        kHasValidPid,
        kPidWithinRange,
        kPassesMinNTPCClusters,
        kPassesChi2NClusters,
        kNotAKink,
        kNTrackCuts,
    };

    enum EPreFoundLambda {
        kAllPreFoundV0s,
        kOnTheFlyV0s,
        kNegDaughterHasValidPid,
        kPosDaughterHasValidPid,
        kPassesPid,
        kPassesInvariantMass,
        kPassesArmenterosPodolanski,
        kNPreFoundLambdaCuts,
    };

   public:
    AliTaskEsd2Tree(const char* name);
    AliTaskEsd2Tree();
    ~AliTaskEsd2Tree();

    // Settings ~ stored in Analysis Manager //
    void Initialize(bool is_mc, bool is_sexa_mc = false, bool is_hdib_mc = false);

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

    // Injected Reactions //
    void ProcessInjectedReactions();
    void BringSignalLogs();
    bool ReadSignalLogs();

    // Utilities //
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

   private:
    // Settings ~ stored in Analysis Manager ~ all persistent //
    bool fIsMC;                     // kTRUE if MC simulation, kFALSE if data
    bool fIsMC_DedicatedSexaquark;  // kTRUE to read and load signal logs
    bool fIsMC_DedicatedHdibaryon;  //

    // AliRoot Objects //
    AliMCEvent* fMC;                      //! MC event
    const AliVVertex* fMC_PrimaryVertex;  //! MC gen. (or true) primary vertex
    AliESDEvent* fESD;                    //! reconstructed event
    const AliESDVertex* fPrimaryVertex;   //! primary vertex
    AliPIDResponse* fPIDResponse;         //! pid response object
    AliEventCuts fEventCuts;              //! event cuts

    // Signal Logs //
    TString fAliEnPath;                                                                                         //! loaded in `UserNotify()`
    TString fSignalLog_NewBasename;                                                                             //!
    std::array<std::array<int, E2T::NSexaReactionsPerEvent>, E2T::NEventsInDedicatedMC> fEvVec_ReactionID;      //!
    std::array<std::array<float, E2T::NSexaReactionsPerEvent>, E2T::NEventsInDedicatedMC> fEvVec_Sexaquark_Px;  //!
    std::array<std::array<float, E2T::NSexaReactionsPerEvent>, E2T::NEventsInDedicatedMC> fEvVec_Sexaquark_Py;  //!
    std::array<std::array<float, E2T::NSexaReactionsPerEvent>, E2T::NEventsInDedicatedMC> fEvVec_Sexaquark_Pz;  //!
    std::array<std::array<float, E2T::NSexaReactionsPerEvent>, E2T::NEventsInDedicatedMC> fEvVec_Nucleon_Px;    //!
    std::array<std::array<float, E2T::NSexaReactionsPerEvent>, E2T::NEventsInDedicatedMC> fEvVec_Nucleon_Py;    //!
    std::array<std::array<float, E2T::NSexaReactionsPerEvent>, E2T::NEventsInDedicatedMC> fEvVec_Nucleon_Pz;    //!

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

    ClassDef(AliTaskEsd2Tree, 2);
};
