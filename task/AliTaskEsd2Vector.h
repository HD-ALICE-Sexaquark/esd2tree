#pragma once

#define E2V_DEBUG 0
#define E2V_MC_EXTRA 0
#define E2V_TPC_EXTRA 1

#include <array>
#include <cstddef>
#include <vector>

#include <TBits.h>

#include <AliAnalysisTaskSE.h>
#include <AliESDtrack.h>
#include <AliEventCuts.h>

#include "AliTaskEsd2Vector_Const.h"

class TClass;
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
    void Terminate(Option_t* option) {}

    // Tree //
    void CreateEventsBranches();
    void CreateMCBranches();
    void CreateTracksBranches();
    void CreateLambdasBranches();
    void CreateInjectedBranches();

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

    unsigned int fRunNumber;          //! run number
    unsigned int fDirNumber;          //! directory number
    unsigned int fDirNumberB;         //! component after-the-dot of data directories
    unsigned int fEventNumberInFile;  //! event number
    float fCentrality;                //! centrality percentile
    float fMagneticField;             //! magnetic field

    // Signal Logs //
    TString fAliEnPath;              //! loaded in `UserNotify()`
    TString fSignalLog_NewBasename;  //!

    // Utilities //
    std::array<std::array<int, Const::NReactionsPerEvent>, Const::NEventsInDedicatedMC> fEvVec_ReactionID;      //!
    std::array<std::array<float, Const::NReactionsPerEvent>, Const::NEventsInDedicatedMC> fEvVec_Sexaquark_Px;  //!
    std::array<std::array<float, Const::NReactionsPerEvent>, Const::NEventsInDedicatedMC> fEvVec_Sexaquark_Py;  //!
    std::array<std::array<float, Const::NReactionsPerEvent>, Const::NEventsInDedicatedMC> fEvVec_Sexaquark_Pz;  //!
    std::array<std::array<float, Const::NReactionsPerEvent>, Const::NEventsInDedicatedMC> fEvVec_Nucleon_Px;    //!
    std::array<std::array<float, Const::NReactionsPerEvent>, Const::NEventsInDedicatedMC> fEvVec_Nucleon_Py;    //!
    std::array<std::array<float, Const::NReactionsPerEvent>, Const::NEventsInDedicatedMC> fEvVec_Nucleon_Pz;    //!

    // Output //

    // -- QA Histograms
    TList* fOutputList;              //!
    TH1F* fHist_Events_Bookkeeping;  //!
    TH1F* fHist_Centrality;          //!
    TH1F* fHist_CentralityINT7;      //!
    TH1F* fHist_Tracks_Bookkeeping;  //!

    // -- Tree
    TTree* fOutputTree;  //!

    //   -- event properties
    int tEvent_PV_NContributors;               //!
    float tEvent_PV_Dispersion;                //!
    float tEvent_PV_X;                         //!
    float tEvent_PV_Y;                         //!
    float tEvent_PV_Z;                         //!
    std::array<float, 6> tEvent_PV_CovMatrix;  //!
    float tEvent_SPD_PV_Z;                     //!
    float tEvent_SPD_PV_ZErr;                  //!
    int tEvent_NTracks;                        //!
    int tEvent_NTPCClusters;                   //!
    bool tEvent_IsMB;                          //!
    bool tEvent_IsHighMultV0;                  //!
    bool tEvent_IsHighMultSPD;                 //!
    bool tEvent_IsCentral;                     //!
    bool tEvent_IsSemiCentral;                 //!

    //   -- MC event properties
    float tEvent_MC_PV_X;        //!
    float tEvent_MC_PV_Y;        //!
    float tEvent_MC_PV_Z;        //!
    bool tEvent_MC_IsGenPileup;  //!
    bool tEvent_MC_IsSBCPileup;  //!

    //   -- signal reaction properties
    std::vector<int> tInjected_ReactionID;    //!
    std::vector<float> tInjected_Px;          //!
    std::vector<float> tInjected_Py;          //!
    std::vector<float> tInjected_Pz;          //!
    std::vector<float> tInjected_Nucleon_Px;  //!
    std::vector<float> tInjected_Nucleon_Py;  //!
    std::vector<float> tInjected_Nucleon_Pz;  //!

    //   -- MC particles properties
    std::vector<int> tMC_PdgCode;               //!
    std::vector<char> tMC_Charge;               //!
    std::vector<int> tMC_Mother_McEntry;        //!
    std::vector<unsigned int> tMC_N_Daughters;  //!
    std::vector<int> tMC_FirstDau_McEntry;      //!
    std::vector<int> tMC_LastDau_McEntry;       //!
    std::vector<float> tMC_Origin_X;            //! origin x-vertex
    std::vector<float> tMC_Origin_Y;            //! origin y-vertex
    std::vector<float> tMC_Origin_Z;            //! origin z-vertex
    std::vector<float> tMC_Px;                  //!
    std::vector<float> tMC_Py;                  //!
    std::vector<float> tMC_Pz;                  //!
    std::vector<float> tMC_E;                   //!
    std::vector<unsigned int> tMC_Status;       //!
    std::vector<char> tMC_Generator;            //! 0: HIJING, 1: anti-neutron injector, 2: anti-sexaquark reaction
    std::vector<char> tMC_IsPhysPrimary;        //! is physical primary according to the ALICE definition
    std::vector<char> tMC_IsSecFromMat;         //!
    std::vector<char> tMC_IsSecFromWeak;        //!
#if E2V_MC_EXTRA
    std::vector<char> tMC_IsOOBPileup;  //!
#endif

    //   -- tracks properties
    std::vector<unsigned int> tTrack_EsdEntry;  //!
    std::vector<float> tTrack_X;                //! inner parametrization
    std::vector<float> tTrack_Y;                //! inner parametrization
    std::vector<float> tTrack_Z;                //! inner parametrization
    std::vector<float> tTrack_Px;               //! inner parametrization
    std::vector<float> tTrack_Py;               //! inner parametrization
    std::vector<float> tTrack_Pz;               //! inner parametrization
    std::vector<char> tTrack_Charge;            //!
    std::vector<float> tTrack_PreDCAxy;         //! pre-calculated DCA wrt PV
    std::vector<float> tTrack_PreDCAz;          //! pre-calculated DCA wrt PV
    std::vector<float> tTrack_TPC_Signal;       //!
    std::vector<int> tTrack_ITS_FirstLayer;     //! -1 if no ITS hit
    std::vector<float> tTrack_NSigmaPion;       //!
    std::vector<float> tTrack_NSigmaKaon;       //!
    std::vector<float> tTrack_NSigmaProton;     //!
    std::vector<float> tTrack_CovMatrix;        // size = N_tracks * 21
#if E2V_TPC_EXTRA
    //   -- extra tpc info
    std::vector<float> tTrack_TPC_DCAxy;                   //!
    std::vector<float> tTrack_TPC_DCAz;                    //!
    std::vector<float> tTrack_TPC_NCrossedRows;            //!
    std::vector<unsigned int> tTrack_TPC_NClusters;        //!
    std::vector<unsigned int> tTrack_TPC_NClustersLC;      //! `LC` stands for the lowercase method `GetTPCncls`
    std::vector<unsigned int> tTrack_TPC_NClustersFound;   //!
    std::vector<unsigned int> tTrack_TPC_NClustersShared;  //!
    std::vector<float> tTrack_TPC_Chi2;                    //!
    std::vector<float> tTrack_TPC_Chi2Constrained;         //! TPC-only chi2 at the primary vertex
    std::vector<float> tTrack_TPC_Chi2TCVG;                //! chi2 TPC constrained vs global
    std::vector<float> tTrack_TPC_SignalTunedOnData;       //! NOTE: MC only
    std::vector<float> tTrack_TPC_SignalSigma;             //!
    std::vector<float> tTrack_TPC_SignalCorrected;         //!
    std::vector<float> tTrack_TPC_ESignalPion;             //! `E` stands for `Expected`
    std::vector<float> tTrack_TPC_ESigmaPion;              //!
    std::vector<float> tTrack_TPC_ESignalKaon;             //!
    std::vector<float> tTrack_TPC_ESigmaKaon;              //!
    std::vector<float> tTrack_TPC_ESignalProton;           //!
    std::vector<float> tTrack_TPC_ESigmaProton;            //!
    std::vector<unsigned int> tTrack_TPC_SignalN;          //!
    std::vector<float> tTrack_TPC_PointsFirst;             //!
    std::vector<float> tTrack_TPC_PointsIndexMax;          //!
    std::vector<float> tTrack_TPC_PointsLast;              //!
    std::vector<float> tTrack_TPC_PointsMaxDens;           //!
    std::vector<int> tTrack_TPC_FirstRow;                  //!
#endif
    //   -- MC Link
    std::vector<int> tTrack_McEntry;  //! only available for MC

    // -- lambda candidates
    std::vector<unsigned int> tLambda_EsdEntry;  //!
    std::vector<float> tLambda_Decay_X;          //!
    std::vector<float> tLambda_Decay_Y;          //!
    std::vector<float> tLambda_Decay_Z;          //!
    std::vector<float> tLambda_Px;               //!
    std::vector<float> tLambda_Py;               //!
    std::vector<float> tLambda_Pz;               //!
    std::vector<float> tLambda_DcaV0Daughters;   //!
    //   -- neg. daughter
    std::vector<unsigned int> tLambda_Neg_EsdEntry;  //!
    std::vector<float> tLambda_Neg_PCAwrtV0_Px;      //!
    std::vector<float> tLambda_Neg_PCAwrtV0_Py;      //!
    std::vector<float> tLambda_Neg_PCAwrtV0_Pz;      //!
    std::vector<float> tLambda_Neg_PreDCAxy;         //!
    std::vector<float> tLambda_Neg_PreDCAz;          //!
    std::vector<float> tLambda_Neg_NSigmaProton;     //!
    std::vector<float> tLambda_Neg_NSigmaKaon;       //!
    std::vector<float> tLambda_Neg_NSigmaPion;       //!
    //   -- pos. daughter
    std::vector<unsigned int> tLambda_Pos_EsdEntry;  //!
    std::vector<float> tLambda_Pos_PCAwrtV0_Px;      //!
    std::vector<float> tLambda_Pos_PCAwrtV0_Py;      //!
    std::vector<float> tLambda_Pos_PCAwrtV0_Pz;      //!
    std::vector<float> tLambda_Pos_PreDCAxy;         //!
    std::vector<float> tLambda_Pos_PreDCAz;          //!
    std::vector<float> tLambda_Pos_NSigmaProton;     //!
    std::vector<float> tLambda_Pos_NSigmaKaon;       //!
    std::vector<float> tLambda_Pos_NSigmaPion;       //!
    //   -- MC Link
    std::vector<int> tLambda_Neg_McEntry;  //!
    std::vector<int> tLambda_Pos_McEntry;  //!

    ClassDef(AliTaskEsd2Vector, 2);  // = number of persistent members
};
