#include <TChain.h>
#include <TGrid.h>
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
#include <AliESDtrackCuts.h>
#include <AliMultSelectionTask.h>
#include <AliPhysicsSelectionTask.h>
#include <AliTaskConfigOCDB.h>

// clang-format off
R__ADD_INCLUDE_PATH($ALICE_PHYSICS)
#include "PWGPP/TPC/macros/AddTaskConfigOCDB.C"
#include "AddTaskFilteredTreeLocal.C"
#include "AliAnalysisTaskTIdentityPID.h"
#include "AddTask_marsland_TIdentityPID.C"
// clang-format on

#include "AliTaskEsd2Tree.h"

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
             bool include_tidentity = false,    //
             const char *Grid_WorkingDir = "",  // customize output dirs, relative to GRID's home dir
             const char *Grid_CustomXML = ""    // if not empty, get input files from custom XML instead of automatically generating one
) {

    // # Derive Options # //

    bool IsGridMode = TString(Mode) == "grid";

    auto ProductionName_TStr = TString(ProductionName);
    bool IsMC = ProductionName_TStr.Contains("LHC2");  // fragile MC detection, but Run 2 finished in 2018
    bool IsSexaMC = ProductionName_TStr.Contains("23l1");
    bool IsHdibMC = ProductionName_TStr.Contains("26h");

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

    AliAnalysisManager *mgr = new AliAnalysisManager("Esd2Tree");

    std::cout << "INFO  !! RunTask.C !! Created AliAnalysisManager" << '\n';

    // # Grid Connection # //

    AliAnalysisAlien *alienHandler = nullptr;

    if (IsGridMode) {
        alienHandler = new AliAnalysisAlien();
        alienHandler->SetCheckCopy(false);
        alienHandler->SetExecutableCommand("aliroot -l -b -q -x");
        alienHandler->AddIncludePath("-I. -I${ROOTSYS}/include -I${ALICE_ROOT} -I${ALICE_ROOT}/include -I${ALICE_PHYSICS}/include");
        TString additional_libs =
            "AliTaskEsd2Tree.cxx AliTaskEsd2Tree.h AliTaskEsd2Tree_LinkDef.h Constants.hpp Framework_TeeTree.hpp Schema_Events.hpp "
            "POD_Event.hpp POD_InjectedSexa.hpp POD_McParticle.hpp POD_PreFoundLambda.hpp POD_Track.hpp Cuts_E2T.hpp Math.hpp";
        if (include_tidentity) {
            additional_libs +=
                " AddTask_marsland_TIdentityPID.C AliAnalysisTaskTIdentityPID.cxx AliAnalysisTaskTIdentityPID.h Config_marsland_TIdentityPID.C "
                "AddTaskFilteredTreeLocal.C";
        }
        alienHandler->SetAdditionalLibs(additional_libs);
        alienHandler->SetTTL(TimeToLive);
        alienHandler->SetDefaultOutputs(false);
        alienHandler->SetOutputFiles("AnalysisResults.root");
        alienHandler->SetOutputArchive("");
        alienHandler->SetKeepLogs(true);
        alienHandler->SetMergeViaJDL(false);
        alienHandler->SetAnalysisSource("AliTaskEsd2Tree.cxx");
        alienHandler->SetJDLName("TaskEsd2Tree.jdl");
        alienHandler->SetExecutable("TaskEsd2Tree.sh");
        alienHandler->SetAliPhysicsVersion("vAN-20260616_O2-1");  // see latest available in https://alimonitor.cern.ch/packages/

        TString job_tag;
        if (!AutomaticSearchESDs) {
            job_tag = gSystem->BaseName(Grid_CustomXML);  // xml files are already named <run>_<index>.xml
            job_tag.ReplaceAll(".xml", "");
        } else if (IsMC) {
            // InputPath ends with the sim set, e.g. /alice/sim/2023/LHC23l1a3/A1.73 -> "A1.73_296690"
            job_tag = Form("%s_%i", gSystem->BaseName(InputPath), RunNumber);
        } else {
            job_tag = Form("%i", RunNumber);
            if (IsMC) job_tag += Form("_%s", gSystem->BaseName(InputPath));  // InputPath ends with the sim set, e.g. A1.73
        }
        alienHandler->SetJobTag(job_tag);
        alienHandler->SetGridWorkingDir(Grid_WorkingDir);
        alienHandler->SetGridOutputDir(job_tag);  // no leading '/': relative to the working dir
        alienHandler->SetOutputToRunNo(0);

        if (AutomaticSearchESDs) {
            alienHandler->SetGridDataDir(Form("%s/%s%i", InputPath, IsMC ? "" : "000", RunNumber));
            alienHandler->SetDataPattern(GridDataPattern);
        } else {
            alienHandler->AddDataFile(Grid_CustomXML);
        }
        alienHandler->SetSplitMaxInputFileNumber(SplitMaxNFiles);
        alienHandler->SetRunMode("full");

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
    TaskCentrality->SetSelectedTriggerClass(AliVEvent::kMB | AliVEvent::kINT7 | AliVEvent::kCentral | AliVEvent::kSemiCentral);
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

    // # Add My Task # //

    gInterpreter->LoadMacro("AliTaskEsd2Tree.cxx++g");

    TString TaskEsd2Tree_Options = Form("(%i, %i, %i)", (int)IsMC, (int)IsSexaMC, (int)IsHdibMC);
    AliTaskEsd2Tree *TaskEsd2Tree = reinterpret_cast<AliTaskEsd2Tree *>(  //
        gInterpreter->ExecuteMacro("AddTaskEsd2Tree.C" + TaskEsd2Tree_Options));
    if (TaskEsd2Tree == nullptr) return;

    std::cout << "INFO  !! RunTask.C !! Passed addition of main task" << '\n';

    // # Add Ilya's Tasks # //

    if (include_tidentity && !IsMC && ProductionName_TStr == "LHC18r") {
        auto *TaskConfigOCDB = AddTaskConfigOCDB("raw://");
        if (TaskConfigOCDB == nullptr) return;

        AliAnalysisTask *TaskFilteredTree = AddTaskFilteredTreeLocal("", IsMC);
        if (TaskFilteredTree == nullptr) return;

        gInterpreter->LoadMacro("AliAnalysisTaskTIdentityPID.cxx++g");

        // Reference:
        // aliroot -b -q
        // 'runGrid.C(0,0,"full",1,"3","$RUN_ON_GRID_DIR/Ebye/lists/runs50longest-2018-LHC18q-pass3.list","PWGPP695_MC_remapping",0,4,0,2018,"18q",3,"vAN-20221119_O2-1")'
        AliAnalysisTask *TaskTIdentity = AddTask_marsland_TIdentityPID(kFALSE, "Config_marsland_TIdentityPID.C", 4, 2018, "18r", PassNumber);
        if (TaskTIdentity == nullptr) return;
    }

    // # Init Analysis Manager # //

    mgr->SetDebugLevel(0);
    if (!mgr->InitAnalysis()) return;

    std::cout << "INFO  !! RunTask.C !! Passed InitAnalysis" << '\n';

    // # Start Analysis # //

    TChain *chain = nullptr;
    TString FilePath = "";

    if (IsGridMode) {
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
            auto *list_of_files = top_dir.GetListOfFiles();
            if (list_of_files == nullptr) {
                std::cout << "ERROR !! RunTask.C !! Couldn't list " << top_path << '\n';
                return;
            }
            int dir_counter = 0;
            for (auto *file : *list_of_files) {
                if (Local_NDirs > 0 && dir_counter == Local_NDirs) break;
                // NOTE: `GetListOfFiles()` returns plain `TSystemFile`s for regular files, for which this cast yields a nullptr
                auto *one_dir = dynamic_cast<TSystemDirectory *>(file);
                if (one_dir == nullptr || !one_dir->IsDirectory()) continue;
                if (strcmp(one_dir->GetName(), ".") == 0 || strcmp(one_dir->GetName(), "..") == 0) continue;
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
