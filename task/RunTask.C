#include <TChain.h>
#include <TString.h>
#include <TSystem.h>
#include <TSystemDirectory.h>

#include <AliAnalysisAlien.h>
#include <AliAnalysisManager.h>
#include <AliDataFile.h>
#include <AliESDInputHandler.h>
#include <AliMCEventHandler.h>

#include <AliAnalysisTaskPIDResponse.h>
// #include <AliAnalysisTaskPIDqa.h> // COMMENTED OUT: only needed for debug purposes
#include <AliMultSelectionTask.h>
#include <AliPhysicsSelectionTask.h>

#include "AliTaskEsd2Vector.h"

void RunTask(const char *Mode,            // "local", "grid"
             const char *InputPath,       // what comes before the RN
             const char *ProductionName,  // for data: "LHC15o", "LHC18q", "LHC18r"
                                          // for sexa MC: "LHC23l1a3", "LHC23l1b3"
                                          // for gen. purp. MC: "LHC20e3a", "LHC20j6a"
                                          // for hdib MC: "LHC26h"
             int RunNumber,               // single run number
             /* only valid when "local" */
             int Local_NDirs = 1,                 // number of subdirs~files to run
             long long Local_LimitToNEvents = 0,  // 0 means all events
             /* only valid when "grid" */
             const char *Grid_WorkingDir = "",  // customize output dirs, relative to GRID's home dir
             const char *Grid_CustomXML = ""    // if not empty, get input files from custom XML instead of automatically generating one
) {

    // # Derive Options # //

    bool IsMC = TString(ProductionName).Contains("LHC2");  // fragile MC detection, but Run 2 finished in 2018
    bool IsSexaMC = TString(ProductionName).Contains("23l1");
    bool IsHdibMC = TString(ProductionName).Contains("26h");

    int SplitMaxNFiles = 60;        // default for data
    if (IsMC) SplitMaxNFiles = 12;  // default for MC

    int PassNumber = 3;  // default for 18qr and anchored sims
    if (RunNumber >= 244917 && RunNumber <= 246994) PassNumber = 2;

    TString GridDataPattern = Form("/pass%i/*/AliESDs.root", PassNumber);  // default for data
    if (IsMC) GridDataPattern = "/*/AliESDs.root";                         // default for MC

    int TimeToLive = 14400;       // seconds, = 4 hours
    if (IsMC) TimeToLive = 7200;  // seconds, = 2 hours

    bool AutomaticSearchESDs = TString(Grid_CustomXML) == "";

    // # Start # //

    gInterpreter->ProcessLine(".include ${ROOTSYS}/include");
    gInterpreter->ProcessLine(".include ${ALICE_ROOT}/include");
    gInterpreter->ProcessLine(".include ${ALICE_PHYSICS}/include");

    AliAnalysisManager *mgr = new AliAnalysisManager("Esd2Vector");

    std::cout << "INFO  !! RunTask.C !! Created AliAnalysisManager" << '\n';

    // # Grid Connection # //

    AliAnalysisAlien *alienHandler = nullptr;

    if (TString(Mode) == "grid") {
        alienHandler = new AliAnalysisAlien();
        alienHandler->SetCheckCopy(false);
        alienHandler->SetExecutableCommand("aliroot -l -b -q -x");
        alienHandler->AddIncludePath("-I. -I${ROOTSYS}/include -I${ALICE_ROOT} -I${ALICE_ROOT}/include -I${ALICE_PHYSICS}/include");
        alienHandler->SetAdditionalLibs(
            "AliTaskEsd2Vector.cxx AliTaskEsd2Vector.h AliTaskEsd2Vector_LinkDef.h Constants.hpp Framework_TeeTree.hpp Schema_Events.hpp "
            "POD_Event.hpp POD_InjectedSexa.hpp POD_McParticle.hpp POD_PreFoundLambda.hpp POD_Track.hpp E2R_Cuts.hpp");
        alienHandler->SetTTL(TimeToLive);
        alienHandler->SetDefaultOutputs(false);
        alienHandler->SetOutputFiles("AnalysisResults.root");
        alienHandler->SetOutputArchive("");
        alienHandler->SetKeepLogs(true);
        alienHandler->SetMergeViaJDL(false);
        alienHandler->SetGridWorkingDir(Grid_WorkingDir);
        alienHandler->SetAnalysisSource("AliTaskEsd2Vector.cxx");
        alienHandler->SetJDLName("TaskEsd2Vector.jdl");
        alienHandler->SetExecutable("TaskEsd2Vector.sh");
        alienHandler->SetAliPhysicsVersion("vAN-20260616_O2-1");  // see latest available in https://alimonitor.cern.ch/packages/

        alienHandler->SetOutputToRunNo(1);  // output-subdir will inherit xml's filename
        if (AutomaticSearchESDs) {
            if (!IsMC) alienHandler->SetRunPrefix("000");
            alienHandler->AddRunNumber(RunNumber);
            alienHandler->SetGridDataDir(InputPath);
            alienHandler->SetDataPattern(GridDataPattern);
        } else {
            alienHandler->AddDataFile(Grid_CustomXML);
        }
        alienHandler->SetSplitMaxInputFileNumber(SplitMaxNFiles);
        alienHandler->SetRunMode("full");  // PENDING

        mgr->SetGridHandler(alienHandler);

        std::cout << "INFO  !! RunTask.C !! Passed grid connection" << '\n';
    }

    // # Input Handlers # //

    AliESDInputHandler *esdH = new AliESDInputHandler();
    esdH->SetNeedField();  // necessary to get GetChi2TPCConstrainedVsGlobal
    mgr->SetInputEventHandler(esdH);

    AliMCEventHandler *mcH = nullptr;
    if (IsMC) {
        mcH = new AliMCEventHandler();
        mcH->SetReadTR(false);
        mgr->SetMCtruthEventHandler(mcH);
    }

    std::cout << "INFO  !! RunTask.C !! Passed creation of input handlers" << '\n';

    // # Add Helper Tasks # //

    // Reference:
    // https://twiki.cern.ch/twiki/bin/view/ALICE/AliDPGtoolsPhysSel
    bool applyPileupCuts = false;
    TString TaskPhysicsSelection_Options = Form("(%i, %i)", (int)IsMC, (int)applyPileupCuts);
    auto *TaskPhysicsSelection = reinterpret_cast<AliPhysicsSelectionTask *>(
        gInterpreter->ExecuteMacro("${ALICE_PHYSICS}/OADB/macros/AddTaskPhysicsSelection.C" + TaskPhysicsSelection_Options));
    if (TaskPhysicsSelection == nullptr) return;

    TString TaskCentrality_Options = "";  // nothing
    auto *TaskCentrality = reinterpret_cast<AliMultSelectionTask *>(
        gInterpreter->ExecuteMacro("${ALICE_PHYSICS}/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C" + TaskCentrality_Options));
    if (TaskCentrality == nullptr) return;
    // Reference:
    // Janik's latest train config: http://alitrain.cern.ch/train-workdir/PWGLF/LF_PbPb/1771_20220407-1015/config/MLTrainDefinition.cfg
    TaskCentrality->SetAddInfo(true);
    TaskCentrality->SetSelectedTriggerClass(AliVEvent::kINT7 | AliVEvent::kCentral | AliVEvent::kSemiCentral);
    if (IsMC) {
        TString AnchoredProdName = "LHC15o";
        if (RunNumber >= 295581 && RunNumber <= 296689) AnchoredProdName = "LHC18q";
        if (RunNumber >= 296690 && RunNumber <= 300000) AnchoredProdName = "LHC18r";
        TaskCentrality->SetAlternateOADBforEstimators(AnchoredProdName + "-DefaultMC-HIJING_V0fix");
    }

    // References:
    // https://twiki.cern.ch/twiki/bin/viewauth/ALICE/PIDInAnalysis
    // https://twiki.cern.ch/twiki/bin/viewauth/ALICE/TPCSplines
    // https://alisw.github.io/git-advanced/#how-to-use-large-data-files-for-analysis
    Bool_t pid_is_mc = IsMC;
    Bool_t pid_auto_mc_esd = kTRUE;
    Bool_t pid_tune_on_data = IsMC;
    TString pid_reco_pass = "";
    Bool_t pid_cache_pid = kFALSE;
    TString pid_path_oadb =
        AliDataFile::GetFileNameOADB("COMMON/PID/data/TPCPIDResponseOADB_pileupCorr.root");  // NOTE: large storage file not in common repos
    TString pid_path_eta_maps = "$ALICE_PHYSICS/OADB/COMMON/PID/data/TPCetaMaps_pileupCorr.root";
    TString pid_det_response = Form("TPC-OADB:%s;TPC-Maps:%s", pid_path_oadb.Data(), pid_path_eta_maps.Data());
    TString TaskPIDResponse_Options = Form("(%i, %i, %i, \"%s\", %i, \"%s\")", (int)pid_is_mc, (int)pid_auto_mc_esd, (int)pid_tune_on_data,
                                           pid_reco_pass.Data(), pid_cache_pid, pid_det_response.Data());
    auto *TaskPIDResponse = reinterpret_cast<AliAnalysisTaskPIDResponse *>(
        gInterpreter->ExecuteMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C" + TaskPIDResponse_Options));
    if (TaskPIDResponse == nullptr) return;

    /*
    // COMMENTED OUT: only needed for debug purposes
    // Reference:
    // https://twiki.cern.ch/twiki/bin/view/ALICE/PIDInAnalysis#Adding_the_PIDqa_task
    auto *TaskPIDqa = reinterpret_cast<AliAnalysisTaskPIDqa *>(  //
        gInterpreter->ExecuteMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDqa.C"));
    if (TaskPIDqa == nullptr) return;
    */

    std::cout << "INFO  !! RunTask.C !! Passed addition of helper tasks" << '\n';

    // # Add Main Task # //

    gInterpreter->LoadMacro("AliTaskEsd2Vector.cxx++g");

    TString TaskEsd2Vector_Options = Form("(%i, %i, %i)", (int)IsMC, (int)IsSexaMC, (int)IsHdibMC);
    AliTaskEsd2Vector *TaskEsd2Vector = reinterpret_cast<AliTaskEsd2Vector *>(  //
        gInterpreter->ExecuteMacro("AddTaskEsd2Vector.C" + TaskEsd2Vector_Options));
    if (TaskEsd2Vector == nullptr) return;

    std::cout << "INFO  !! RunTask.C !! Passed addition of main task" << '\n';

    // # Init Analysis Manager # //

    mgr->SetDebugLevel(0);
    if (!mgr->InitAnalysis()) return;

    std::cout << "INFO  !! RunTask.C !! Passed InitAnalysis" << '\n';

    // # Start Analysis # //

    TChain *chain = nullptr;
    TString FilePath = "";

    if (TString(Mode) == "grid") {
        // grid mode //
        // alienHandler->SetNtestFiles(Local_NDirs);  // PENDING
        // alienHandler->SetRunMode("test");          // PENDING
        mgr->StartAnalysis("grid");
    } else {
        // local mode //
        chain = new TChain("esdTree");
        if (IsMC) {
            for (int DN = 1; DN <= Local_NDirs; ++DN) {
                FilePath = Form("%s/%i/%03i/AliESDs.root", InputPath, RunNumber, DN);
                std::cout << "INFO  !! RunTask.C !! Adding file " << FilePath << '\n';
                chain->AddFile(FilePath);
            }
        } else {  // data
            TString top_path = Form("%s/000%i/pass%i", InputPath, RunNumber, PassNumber);
            TSystemDirectory top_dir(top_path, top_path);
            int dir_counter = 0;
            for (auto *file : *top_dir.GetListOfFiles()) {
                if (Local_NDirs > 0 && dir_counter == Local_NDirs) break;
                auto *one_dir = dynamic_cast<TSystemDirectory *>(file);
                if (strcmp(one_dir->GetName(), ".") == 0 || strcmp(one_dir->GetName(), "..") == 0) continue;
                if (!one_dir->IsDirectory()) continue;
                FilePath = Form("%s/%s/AliESDs.root", top_path.Data(), one_dir->GetName());
                if (gSystem->AccessPathName(FilePath)) continue;
                std::cout << "INFO  !! RunTask.C !! Adding file " << FilePath << '\n';
                chain->AddFile(FilePath);
                dir_counter++;
            }
        }
        if (Local_LimitToNEvents == 0)
            mgr->StartAnalysis("local", chain);  // read all events
        else
            mgr->StartAnalysis("local", chain, Local_LimitToNEvents);  // read first NEvents
    }

    std::cout << "INFO  !! RunTask.C !! Passed StartAnalysis" << '\n';
}
