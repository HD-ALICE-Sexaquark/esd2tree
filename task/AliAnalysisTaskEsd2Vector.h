#ifndef E2V_TASK_H
#define E2V_TASK_H

#define E2V_DEBUG 0

#include <array>
#include <fstream>
#include <vector>

#include <TArray.h>
#include <TChain.h>
#include <TClass.h>
#include <TFile.h>
#include <TGrid.h>
#include <TH1.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TROOT.h>
#include <TString.h>
#include <TSystem.h>
#include <TTree.h>

#include <AliAnalysisManager.h>
#include <AliAnalysisTaskSE.h>
#include <AliExternalTrackParam.h>
#include <AliHelix.h>
#include <AliInputEventHandler.h>
#include <AliLog.h>
#include <AliVEvent.h>
#include <AliVVertex.h>

#include <AliESD.h>
#include <AliESDEvent.h>
#include <AliESDInputHandler.h>
#include <AliESDVertex.h>
#include <AliESDtrack.h>

#include <AliMCEvent.h>
#include <AliMCEventHandler.h>
#include <AliMCParticle.h>

#include <AliAnalysisUtils.h>
#include <AliEventCuts.h>
#include <AliMultSelection.h>
#include <AliPIDResponse.h>

class AliPIDResponse;

class AliAnalysisTaskEsd2Vector : public AliAnalysisTaskSE {
   public:
    AliAnalysisTaskEsd2Vector();
    AliAnalysisTaskEsd2Vector(const char* name);
    ~AliAnalysisTaskEsd2Vector();

    AliAnalysisTaskEsd2Vector(const AliAnalysisTaskEsd2Vector&);
    AliAnalysisTaskEsd2Vector& operator=(const AliAnalysisTaskEsd2Vector&);

    // Settings ~ stored in Analysis Manager //
    void Initialize(bool is_mc, bool is_signal_mc = false);

    // Main ~ executed at runtime //
    void UserCreateOutputObjects();
    bool UserNotify();
    void UserExec(Option_t* option);
    void Terminate(Option_t* option) {}

    // Tree //
    void CreateEventsBranches();
    void CreateInjectedBranches();
    void CreateMCBranches();
    void CreateTracksBranches();

    // Events //
    bool ProcessEvent();
    bool PassesEventSelection();

    // MC Particles //
    void ProcessMCParticles();
    void ClearMCBranches();

    // Tracks //
    void ProcessTracks();
    bool PassesTrackSelection(const AliESDtrack* track, const AliExternalTrackParam* inner_param);
    bool PassesTrackSelection_V2(const AliESDtrack* track);
    void ClearTracksBranches();

    // Injected Reactions //
    void ProcessInjectedReactions();
    void BringSignalLogs();
    bool LoadSignalLogs();

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
    std::vector<int> fVec_McEntry;                        //!
    std::vector<std::vector<int>> fEvVec_ReactionID;      //!
    std::vector<std::vector<float>> fEvVec_Sexaquark_Px;  //!
    std::vector<std::vector<float>> fEvVec_Sexaquark_Py;  //!
    std::vector<std::vector<float>> fEvVec_Sexaquark_Pz;  //!
    std::vector<std::vector<float>> fEvVec_Nucleon_Px;    //!
    std::vector<std::vector<float>> fEvVec_Nucleon_Py;    //!
    std::vector<std::vector<float>> fEvVec_Nucleon_Pz;    //!

    // # Output //

    // QA Histograms //
    TList* fOutputList;              //!
    TH1F* fHist_Events_Bookkeeping;  //!
    TH1F* fHist_Centrality;          //!
    TH1F* fHist_CentralityINT7;      //!
    TH1F* fHist_Tracks_Bookkeeping;  //!

    // Trees //
    TTree* fOutputTree;  //!
    // -- Event properties //
    int tEvent_PV_NContributors;               //!
    float tEvent_PV_Dispersion;                //!
    float tEvent_PV_Xv;                        //!
    float tEvent_PV_Yv;                        //!
    float tEvent_PV_Zv;                        //!
    std::array<float, 6> tEvent_PV_CovMatrix;  //!
    float tEvent_SPD_PV_Zv;                    //!
    float tEvent_SPD_PV_ZvErr;                 //!
    int tEvent_NTracks;                        //!
    int tEvent_NTPCClusters;                   //!
    bool tEvent_IsMB;                          //!
    bool tEvent_IsHighMultV0;                  //!
    bool tEvent_IsHighMultSPD;                 //!
    bool tEvent_IsCentral;                     //!
    bool tEvent_IsSemiCentral;                 //!
    // -- MC Event properties //
    float tEvent_MC_PV_Xv;       //!
    float tEvent_MC_PV_Yv;       //!
    float tEvent_MC_PV_Zv;       //!
    bool tEvent_MC_IsGenPileup;  //!
    bool tEvent_MC_IsSBCPileup;  //!
    // -- Signal reaction properties //
    std::vector<int> tInjected_ReactionID;    //!
    std::vector<float> tInjected_Px;          //!
    std::vector<float> tInjected_Py;          //!
    std::vector<float> tInjected_Pz;          //!
    std::vector<float> tInjected_Nucleon_Px;  //!
    std::vector<float> tInjected_Nucleon_Py;  //!
    std::vector<float> tInjected_Nucleon_Pz;  //!
    // -- MC particles properties //
    std::vector<int> tMC_PdgCode;         //!
    std::vector<int> tMC_Mother_McEntry;  //!
    std::vector<float> tMC_X;             //! origin x-vertex
    std::vector<float> tMC_Y;             //! origin y-vertex
    std::vector<float> tMC_Z;             //! origin z-vertex
    std::vector<float> tMC_Px;            //!
    std::vector<float> tMC_Py;            //!
    std::vector<float> tMC_Pz;            //!
    std::vector<float> tMC_E;             //!
    std::vector<int> tMC_Status;          //!
    std::vector<char> tMC_Generator;      //! 0: HIJING, 1: anti-neutron injector, 2: anti-sexaquark reaction
    std::vector<char> tMC_IsPrimary;      //!
    std::vector<char> tMC_IsSecFromMat;   //!
    std::vector<char> tMC_IsSecFromWeak;  //!
    std::vector<char> tMC_IsOOBPileup;    //!
#if E2V_DEBUG
    std::vector<int> tMC_iMC;         //!
    std::vector<int> tMC_Mother_iMC;  //!
#endif
    // -- Tracks properties //
    std::vector<float> tTrack_X;             //! inner parametrization
    std::vector<float> tTrack_Y;             //! inner parametrization
    std::vector<float> tTrack_Z;             //! inner parametrization
    std::vector<float> tTrack_Px;            //! inner parametrization
    std::vector<float> tTrack_Py;            //! inner parametrization
    std::vector<float> tTrack_Pz;            //! inner parametrization
    std::vector<int> tTrack_Charge;          //!
    std::vector<float> tTrack_NSigmaPion;    //!
    std::vector<float> tTrack_NSigmaKaon;    //!
    std::vector<float> tTrack_NSigmaProton;  //!
    std::vector<float> tTrack_DCAxy;         //! pre-calculated DCA wrt PV
    std::vector<float> tTrack_DCAz;          //! pre-calculated DCA wrt PV
    std::vector<float> tTrack_SigmaX2;       //!
    std::vector<float> tTrack_SigmaXY;       //!
    std::vector<float> tTrack_SigmaY2;       //!
    std::vector<float> tTrack_SigmaXZ;       //!
    std::vector<float> tTrack_SigmaYZ;       //!
    std::vector<float> tTrack_SigmaZ2;       //!
    std::vector<float> tTrack_SigmaXPx;      //!
    std::vector<float> tTrack_SigmaYPx;      //!
    std::vector<float> tTrack_SigmaZPx;      //!
    std::vector<float> tTrack_SigmaPx2;      //!
    std::vector<float> tTrack_SigmaXPy;      //!
    std::vector<float> tTrack_SigmaYPy;      //!
    std::vector<float> tTrack_SigmaZPy;      //!
    std::vector<float> tTrack_SigmaPxPy;     //!
    std::vector<float> tTrack_SigmaPy2;      //!
    std::vector<float> tTrack_SigmaXPz;      //!
    std::vector<float> tTrack_SigmaYPz;      //!
    std::vector<float> tTrack_SigmaZPz;      //!
    std::vector<float> tTrack_SigmaPxPz;     //!
    std::vector<float> tTrack_SigmaPyPz;     //!
    std::vector<float> tTrack_SigmaPz2;      //!
#if E2V_DEBUG
    // indices //
    std::vector<int> tTrack_iESD;  //!
    std::vector<int> tTrack_iMC;   //!
#endif
    // -- MC Link //
    std::vector<int> tTrack_McEntry;  //!

    ClassDef(AliAnalysisTaskEsd2Vector, 2);  // = number of persistent members
};

#endif  // E2V_TASK_H
