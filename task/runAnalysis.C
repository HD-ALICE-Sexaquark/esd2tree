#include "Rtypes.h"
#include "TChain.h"
#include "TSystemDirectory.h"

#include "AliAnalysisAlien.h"
#include "AliAnalysisManager.h"
#include "AliESDInputHandler.h"
#include "AliMCEventHandler.h"

#include "AliAnalysisTaskPIDResponse.h"
#include "AliMultSelectionTask.h"
#include "AliPhysicsSelectionTask.h"

#include "AliAnalysisTaskEsd2Vector.h"

void runAnalysis(const TString &Mode,            // "local", "grid"
                 const TString &InputPath,       // what comes before the RN
                 const TString &ProductionName,  // for data: "LHC15o", "LHC18q", "LHC18r"
                                                 // for signal MC: "LHC23l1a3", "LHC23l1b3"
                                                 // for gen. purp. MC: "LHC20e3a", "LHC20j6a"
                 int RunNumber,                  // single run number
                 /* only valid when Mode == "local" */
                 int Local_NDirs = 1,                 // for MC: number of subdirs per run, data doesn't use it
                 long long Local_LimitToNEvents = 0,  // 0 means all events
                 /* only valid when Mode == "grid" */
                 bool Grid_TestMode = false,                 //
                 const TString &Grid_WorkingDir = "",        //
                 int Grid_CustomSplitMaxNFiles = 0,          // 0 means default
                 const TString &Grid_CustomDataPattern = ""  // what comes after the RN, empty means default
) {

    /* Check for Input Errors */

    if (Mode != "local" && Mode != "grid") {
        std::cerr << "!! ERROR !! runAnalysis.C !! Invalid Mode " << Mode << '\n';
        return;
    }

    if (Mode == "local" && (Grid_TestMode || Grid_WorkingDir.Length() > 0 || Grid_CustomSplitMaxNFiles > 0 || Grid_CustomDataPattern.Length() > 0)) {
        std::cerr << "!! WARNING !! runAnalysis.C !! Grid options are only valid when Mode == \"grid\", they will be ignored" << '\n';
    }

    if (Mode == "grid" && (Local_LimitToNEvents > 0 || Local_NDirs > 1)) {
        std::cerr << "!! WARNING !! runAnalysis.C !! Local options are only valid when Mode == \"local\", they will be ignored" << '\n';
    }

    std::cout << "!! INFO  !! runAnalysis.C !! Passed initial input checks" << '\n';

    /* Determine Further Options */

    bool IsMC{ProductionName.Contains("LHC2")};
    bool IsSignalMC{ProductionName.Contains("23l1")};

    int SplitMaxNFiles{60};         // default for data
    if (IsMC) SplitMaxNFiles = 10;  // default for MC
    if (Grid_CustomSplitMaxNFiles > 0) SplitMaxNFiles = Grid_CustomSplitMaxNFiles;

    int PassNumber{3};  // default for 18qr and anchored sims
    if (ProductionName == "LHC15o" || ProductionName == "LHC20j6a" || ProductionName == "LHC23l1b3") PassNumber = 2;

    TString GridDataPattern{TString::Format("/pass%i/*/AliESDs.root", PassNumber)};  // default for data
    if (IsMC) GridDataPattern = "/*/AliESDs.root";                                   // default for MC
    if (Grid_CustomDataPattern.Length() > 0) GridDataPattern = Grid_CustomDataPattern;

    /* Start */

    gInterpreter->ProcessLine(".include $ROOTSYS/include");
    gInterpreter->ProcessLine(".include $ALICE_ROOT/include");
    gInterpreter->ProcessLine(".include $ALICE_PHYSICS/include");

    AliAnalysisManager *mgr{new AliAnalysisManager("Esd2Vector")};

    std::cout << "!! INFO  !! runAnalysis.C !! Created AliAnalysisManager" << '\n';

    /* Grid Connection */

    AliAnalysisAlien *alienHandler{nullptr};

    if (Mode == "grid") {
        alienHandler = new AliAnalysisAlien();
        alienHandler->SetCheckCopy(false);
        alienHandler->AddIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");
        alienHandler->SetAdditionalLibs("AliAnalysisTaskEsd2Vector.cxx AliAnalysisTaskEsd2Vector.h AliAnalysisTaskEsd2Vector_Const.h");
        alienHandler->SetAnalysisSource("AliAnalysisTaskEsd2Vector.cxx");
        alienHandler->SetAliPhysicsVersion("vAN-20241126_O2-1");
        alienHandler->SetExecutableCommand("aliroot -l -q -b");
        alienHandler->SetGridDataDir(InputPath);
        if (!IsMC) alienHandler->SetRunPrefix("000");
        alienHandler->AddRunNumber(RunNumber);
        alienHandler->SetDataPattern(GridDataPattern);
        alienHandler->SetTTL(3600);
        alienHandler->SetOutputToRunNo(static_cast<int>(true));
        alienHandler->SetDefaultOutputs(false);
        alienHandler->SetOutputFiles("AnalysisResults.root");
        alienHandler->SetOutputArchive("");
        alienHandler->SetKeepLogs(true);
        alienHandler->SetMergeViaJDL(false);
        alienHandler->SetGridWorkingDir(Grid_WorkingDir);
        alienHandler->SetJDLName("TaskEsd2Vector.jdl");
        alienHandler->SetExecutable("TaskEsd2Vector.sh");

        mgr->SetGridHandler(alienHandler);

        std::cout << "!! INFO  !! runAnalysis.C !! Passed grid connection" << '\n';
    }

    /* Input Handlers */

    AliESDInputHandler *esdH{new AliESDInputHandler()};
    esdH->SetNeedField();  // necessary to get GoldenChi2
    mgr->SetInputEventHandler(esdH);

    AliMCEventHandler *mcH{nullptr};
    if (IsMC) {
        mcH = new AliMCEventHandler();
        mcH->SetReadTR(false);
        mgr->SetMCtruthEventHandler(mcH);
    }

    std::cout << "!! INFO  !! runAnalysis.C !! Passed creation of input handlers" << '\n';

    /* Add Helper Tasks */

    bool applyPileupCuts{false};  // false in Pb-Pb (as recommended in: https://twiki.cern.ch/twiki/bin/view/ALICE/AliDPGtoolsPhysSel)
    TString TaskPhysicsSelection_Options{TString::Format("(%i, %i)", (int)IsMC, (int)applyPileupCuts)};
    AliPhysicsSelectionTask *TaskPhysicsSelection{reinterpret_cast<AliPhysicsSelectionTask *>(
        gInterpreter->ExecuteMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C" + TaskPhysicsSelection_Options)  //
        )};
    if (TaskPhysicsSelection == nullptr) return;

    TString TaskCentrality_Options{""};  // nothing
    AliMultSelectionTask *TaskCentrality{reinterpret_cast<AliMultSelectionTask *>(
        gInterpreter->ExecuteMacro("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C" + TaskCentrality_Options)  //
        )};
    if (TaskCentrality == nullptr) return;

    TString TaskPIDResponse_Options{TString::Format("(%i, 1, 1, \"%i\")", (int)IsMC, PassNumber)};
    AliAnalysisTaskPIDResponse *TaskPIDResponse{reinterpret_cast<AliAnalysisTaskPIDResponse *>(
        gInterpreter->ExecuteMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C" + TaskPIDResponse_Options)  //
        )};
    if (TaskPIDResponse == nullptr) return;

    std::cout << "!! INFO  !! runAnalysis.C !! Passed addition of helper tasks" << '\n';

    /* Add Main Task */

    gInterpreter->LoadMacro("AliAnalysisTaskEsd2Vector.cxx++g");

    TString TaskEsd2Vector_Options{TString::Format("(%i, %i)", (int)IsMC, (int)IsSignalMC)};
    AliAnalysisTaskEsd2Vector *TaskEsd2Vector{reinterpret_cast<AliAnalysisTaskEsd2Vector *>(  //
        gInterpreter->ExecuteMacro("AddTaskEsd2Vector.C" + TaskEsd2Vector_Options)            //
        )};
    if (TaskEsd2Vector == nullptr) return;

    std::cout << "!! INFO  !! runAnalysis.C !! Passed addition of main task" << '\n';

    /* Init Analysis Manager */

    mgr->SetDebugLevel(3);
    if (!mgr->InitAnalysis()) return;

    std::cout << "!! INFO  !! runAnalysis.C !! Passed InitAnalysis" << '\n';

    /* Start Analysis */

    TChain *chain{nullptr};
    TString FilePath{""};

    if (Mode == "grid") {
        if (Grid_TestMode) {
            alienHandler->SetNtestFiles(5);  // hardcoded
            alienHandler->SetRunMode("test");
        } else {
            alienHandler->SetSplitMaxInputFileNumber(SplitMaxNFiles);
            alienHandler->SetRunMode("full");
        }
        mgr->StartAnalysis("grid");
    } else {  // local mode
        chain = new TChain("esdTree");
        if (IsMC) {
            for (int DN{1}; DN <= Local_NDirs; ++DN) {
                FilePath = TString::Format("%s/%i/%03i/AliESDs.root", InputPath.Data(), RunNumber, DN);
                std::cout << "!! INFO  !! runAnalysis.C !! Adding file " << FilePath << '\n';
                chain->AddFile(FilePath);
            }
        } else {  // data
            TString top_path{TString::Format("%s/000%i/pass%i", InputPath.Data(), RunNumber, PassNumber)};
            TSystemDirectory top_dir(top_path, top_path);
            for (auto *file : *top_dir.GetListOfFiles()) {
                auto *one_dir{dynamic_cast<TSystemDirectory *>(file)};
                if (strcmp(one_dir->GetName(), ".") == 0 || strcmp(one_dir->GetName(), "..") == 0) continue;
                if (!one_dir->IsDirectory()) continue;
                FilePath = TString::Format("%s/%s/AliESDs.root", top_path.Data(), one_dir->GetName());
                if (gSystem->AccessPathName(FilePath)) continue;
                std::cout << "!! INFO  !! runAnalysis.C !! Adding file " << FilePath << '\n';
                chain->AddFile(FilePath);
            }
        }
        if (Local_LimitToNEvents == 0)
            mgr->StartAnalysis("local", chain);  // read all events
        else
            mgr->StartAnalysis("local", chain, Local_LimitToNEvents);  // read first NEvents
    }

    std::cout << "!! INFO  !! runAnalysis.C !! Passed StartAnalysis" << '\n';
}
