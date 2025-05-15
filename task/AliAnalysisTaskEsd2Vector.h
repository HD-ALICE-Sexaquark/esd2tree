#ifndef E2V_TASK_H
#define E2V_TASK_H

#define WRITE_ESD_INDICES 0
#define INCLUDE_MUCH_INFO 0

#include <array>
#include <fstream>
#include <vector>

#include "TArray.h"
#include "TChain.h"
#include "TClass.h"
#include "TFile.h"
#include "TGrid.h"
#include "TH1.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TROOT.h"
#include "TString.h"
#include "TSystem.h"
#include "TTree.h"

#include "AliAnalysisManager.h"
#include "AliAnalysisTaskSE.h"
#include "AliExternalTrackParam.h"
#include "AliHelix.h"
#include "AliInputEventHandler.h"
#include "AliLog.h"
#include "AliVEvent.h"
#include "AliVVertex.h"

#include "AliESD.h"
#include "AliESDEvent.h"
#include "AliESDInputHandler.h"
#include "AliESDVertex.h"
#include "AliESDtrack.h"
#include "AliESDtrackCuts.h"

#include "AliMCEvent.h"
#include "AliMCEventHandler.h"
#include "AliMCParticle.h"

#include "AliAnalysisUtils.h"
#include "AliEventCuts.h"
#include "AliMultSelection.h"
#include "AliPIDResponse.h"

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
    std::vector<long> fVec_McEntry;                            //!
    std::vector<int> ToRealEventNumber;                        //!
    std::vector<std::vector<unsigned int>> fEvVec_ReactionID;  //!
    std::vector<std::vector<float>> fEvVec_Sexaquark_Px;       //!
    std::vector<std::vector<float>> fEvVec_Sexaquark_Py;       //!
    std::vector<std::vector<float>> fEvVec_Sexaquark_Pz;       //!
    std::vector<std::vector<float>> fEvVec_Nucleon_Px;         //!
    std::vector<std::vector<float>> fEvVec_Nucleon_Py;         //!
    std::vector<std::vector<float>> fEvVec_Nucleon_Pz;         //!

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
    float tEvent_PV_TrueXv;                    //!
    float tEvent_PV_TrueYv;                    //!
    float tEvent_PV_TrueZv;                    //!
    bool tEvent_IsGenPileup;                   //!
    bool tEvent_IsSBCPileup;                   //!
    int tEvent_PV_NContributors;               //!
    float tEvent_PV_Dispersion;                //!
    float tEvent_PV_Xv;                        //!
    float tEvent_PV_Yv;                        //!
    float tEvent_PV_Zv;                        //!
    std::array<float, 6> tEvent_PV_CovMatrix;  //!
    float tEvent_SPD_PV_Zv;                    //!
    float tEvent_SPD_PV_ZvErr;                 //!
    unsigned int tEvent_NTracks;               //!
    int tEvent_NTPCClusters;                   //!
    bool tEvent_IsMB;                          //!
    bool tEvent_IsHighMultV0;                  //!
    bool tEvent_IsHighMultSPD;                 //!
    bool tEvent_IsCentral;                     //!
    bool tEvent_IsSemiCentral;                 //!
    // -- Signal reaction properties //
    std::vector<unsigned int> tInjected_ReactionID;  //!
    std::vector<float> tInjected_Px;                 //!
    std::vector<float> tInjected_Py;                 //!
    std::vector<float> tInjected_Pz;                 //!
    std::vector<float> tInjected_Nucleon_Px;         //!
    std::vector<float> tInjected_Nucleon_Py;         //!
    std::vector<float> tInjected_Nucleon_Pz;         //!
    // -- MC particles properties //
    std::vector<int> tMC_PdgCode;          //!
    std::vector<long> tMC_Mother_McEntry;  //!
    std::vector<float> tMC_Px;             //!
    std::vector<float> tMC_Py;             //!
    std::vector<float> tMC_Pz;             //!
    std::vector<float> tMC_E;              //!
    std::vector<float> tMC_Xv;             //! origin x-vertex
    std::vector<float> tMC_Yv;             //! origin y-vertex
    std::vector<float> tMC_Zv;             //! origin z-vertex
    std::vector<unsigned int> tMC_Status;  //!
    std::vector<bool> tMC_IsOOBPileup;     //!
    std::vector<int> tMC_Generator;        //! 0: HIJING, 1: anti-neutron injector, 2: anti-sexaquark reaction
    std::vector<bool> tMC_IsPrimary;       //!
    std::vector<bool> tMC_IsSecFromMat;    //!
    std::vector<bool> tMC_IsSecFromWeak;   //!
    // -- Tracks properties //
#if WRITE_ESD_INDICES
    std::vector<long> tTrack_EsdIdx;  //! DEBUG
#endif
    std::vector<float> tTrack_Px;               //! inner parametrization
    std::vector<float> tTrack_Py;               //! inner parametrization
    std::vector<float> tTrack_Pz;               //! inner parametrization
    std::vector<float> tTrack_X;                //!
    std::vector<float> tTrack_Y;                //!
    std::vector<float> tTrack_Z;                //!
    std::vector<int> tTrack_Charge;             //!
    std::vector<float> tTrack_NSigmaPion;       //!
    std::vector<float> tTrack_NSigmaKaon;       //!
    std::vector<float> tTrack_NSigmaProton;     //!
    std::vector<float> tTrack_DCAxy;            //! pre-calculated DCA wrt PV
    std::vector<float> tTrack_DCAz;             //! pre-calculated DCA wrt PV
    std::vector<float> tTrack_SignalTPC;        //!
    std::vector<bool> tTrack_IsKinkDaughter;    //!
    std::vector<unsigned long> tTrack_McEntry;  //!
    std::vector<float> tTrack_Alpha;            //!
    std::vector<float> tTrack_Snp;              //! local sine of the track momentum azimuthal angle
    std::vector<float> tTrack_Tgl;              //! tangent of the track momentum dip angle
    std::vector<float> tTrack_Signed1Pt;        //! 1/pt
    std::vector<float> tTrack_SigmaY2;          //! cov_matrix[0]
    std::vector<float> tTrack_SigmaZY;          //! cov_matrix[1]
    std::vector<float> tTrack_SigmaZ2;          //! cov_matrix[2]
    std::vector<float> tTrack_SigmaSnpY;        //! cov_matrix[3]
    std::vector<float> tTrack_SigmaSnpZ;        //! cov_matrix[4]
    std::vector<float> tTrack_SigmaSnp2;        //! cov_matrix[5]
    std::vector<float> tTrack_SigmaTglY;        //! cov_matrix[6]
    std::vector<float> tTrack_SigmaTglZ;        //! cov_matrix[7]
    std::vector<float> tTrack_SigmaTglSnp;      //! cov_matrix[8]
    std::vector<float> tTrack_SigmaTgl2;        //! cov_matrix[9]
    std::vector<float> tTrack_Sigma1PtY;        //! cov_matrix[10]
    std::vector<float> tTrack_Sigma1PtZ;        //! cov_matrix[11]
    std::vector<float> tTrack_Sigma1PtSnp;      //! cov_matrix[12]
    std::vector<float> tTrack_Sigma1PtTgl;      //! cov_matrix[13]
    std::vector<float> tTrack_Sigma1Pt2;        //! cov_matrix[14]
#if INCLUDE_MUCH_INFO
    std::vector<unsigned int> tTrack_NTPCClusters;       //!
    std::vector<float> tTrack_NCrossedRows;              //!
    std::vector<unsigned int> tTrack_NFindableClusters;  //!
    std::vector<unsigned int> tTrack_NSharedClusters;    //!
    std::vector<float> tTrack_Chi2overNcls;              //!
#endif
    // TBits tTrack_TPCFitMap;             //!
    // TBits tTrack_TPCClusterMap;         //!
    // TBits tTrack_TPCSharedMap;          //!

    ClassDef(AliAnalysisTaskEsd2Vector, 2);  // = number of persistent members
};

#endif  // E2V_TASK_H
