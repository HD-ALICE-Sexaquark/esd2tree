#include "Rtypes.h"
#include "TChain.h"

#include "AliAnalysisAlien.h"
#include "AliAnalysisManager.h"
#include "AliESDInputHandler.h"
#include "AliMCEventHandler.h"

#include "AliAnalysisTaskPIDResponse.h"
#include "AliMultSelectionTask.h"
#include "AliPhysicsSelectionTask.h"

#include "AliAnalysisTaskEsd2Tree.h"

void runAnalysis(TString Mode,            // "local", "grid"
                 TString InputPath,       // what comes before the RN
                 TString ProductionName,  // for data: "LHC15o", "LHC18q", "LHC18r"
                                          // for signal MC: "LHC23l1a3", "LHC23l1b3"
                                          // for gen. purp. MC: "LHC20e3a", "LHC20j6a"
                 TString SimulationSet,   // (only valid for signal MC) format: "<A,D,E,H><1.73,1.8,1.87,1.94,2.01>" e.g. "A1.73"
                 Int_t RunNumber,         // single run number
                 /* only valid when Mode == "local" */
                 Int_t Local_NDirs = 1,           // for MC: number of subdirs per run
                 Int_t Local_LimitToNEvents = 0,  // 0 means all events
                 /* only valid when Mode == "grid" */
                 Bool_t Grid_TestMode = 0,             //
                 TString Grid_WorkingDir = "",         //
                 Int_t Grid_CustomSplitMaxNFiles = 0,  // 0 means default
                 TString Grid_CustomDataPattern = ""   // what comes after the RN, empty means default
) {

    /* Check for Input Errors */

    if (Mode != "local" && Mode != "grid") {
        std::cerr << "!! ERROR !! runAnalysis.C !! Invalid Mode " << Mode << std::endl;
        return;
    }

    if (Mode == "local" && (Grid_TestMode || Grid_WorkingDir.Length() || Grid_CustomSplitMaxNFiles || Grid_CustomDataPattern.Length())) {
        std::cerr << "!! WARNING !! runAnalysis.C !! Grid options are only valid when Mode == \"grid\", they will be ignored" << std::endl;
    }

    if (Mode == "grid" && (Local_LimitToNEvents || Local_NDirs > 1)) {
        std::cerr << "!! WARNING !! runAnalysis.C !! Local options are only valid when Mode == \"local\", they will be ignored" << std::endl;
    }

    std::cout << "!! INFO !! runAnalysis.C !! Passed initial input checks" << std::endl;

    /* Determine Further Options */

    Bool_t IsMC = ProductionName.Contains("LHC2");
    Bool_t IsSignalMC = ProductionName.Contains("23l1");

    Int_t SplitMaxNFiles = 60;      // default for data
    if (IsMC) SplitMaxNFiles = 10;  // default for MC
    if (Grid_CustomSplitMaxNFiles) SplitMaxNFiles = Grid_CustomSplitMaxNFiles;

    Int_t PassNumber = 3;  // default for 18qr and anchored sims
    if (ProductionName == "LHC15o" || ProductionName == "LHC20j6a" || ProductionName == "LHC23l1b3") PassNumber = 2;

    TString GridDataPattern = TString::Format("/pass%i/*/AliESDs.root", PassNumber);  // default for data
    if (IsMC) GridDataPattern = "/*/AliESDs.root";                                    // default for MC
    if (Grid_CustomDataPattern.Length()) GridDataPattern = Grid_CustomDataPattern;

    /* Start */

    gInterpreter->ProcessLine(".include $ROOTSYS/include");
    gInterpreter->ProcessLine(".include $ALICE_ROOT/include");
    gInterpreter->ProcessLine(".include $ALICE_PHYSICS/include");

    AliAnalysisManager *mgr = new AliAnalysisManager("Esd2Tree");

    std::cout << "!! INFO !! runAnalysis.C !! Created AliAnalysisManager" << std::endl;

    /* Grid Connection */

    AliAnalysisAlien *alienHandler = nullptr;

    if (Mode == "grid") {
        alienHandler = new AliAnalysisAlien();
        alienHandler->SetCheckCopy(kFALSE);
        alienHandler->AddIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");
        alienHandler->SetAdditionalLibs("AliAnalysisTaskEsd2Tree.cxx AliAnalysisTaskEsd2Tree.h");
        alienHandler->SetAnalysisSource("AliAnalysisTaskEsd2Tree.cxx");
        alienHandler->SetAliPhysicsVersion("vAN-20241126_O2-1");
        alienHandler->SetExecutableCommand("aliroot -l -q -b");
        alienHandler->SetGridDataDir(InputPath);
        if (!IsMC) alienHandler->SetRunPrefix("000");
        alienHandler->AddRunNumber(RunNumber);
        alienHandler->SetDataPattern(GridDataPattern);
        alienHandler->SetTTL(3600);
        alienHandler->SetOutputToRunNo(kTRUE);
        alienHandler->SetDefaultOutputs(kFALSE);
        alienHandler->SetOutputFiles("AnalysisResults.root");
        alienHandler->SetOutputArchive("");
        alienHandler->SetKeepLogs(kTRUE);
        alienHandler->SetMergeViaJDL(kFALSE);
        alienHandler->SetGridWorkingDir(Grid_WorkingDir);
        alienHandler->SetJDLName("TaskEsd2Tree.jdl");
        alienHandler->SetExecutable("TaskEsd2Tree.sh");

        mgr->SetGridHandler(alienHandler);

        std::cout << "!! INFO !! runAnalysis.C !! Passed grid connection" << std::endl;
    }

    /* Input Handlers */

    AliESDInputHandler *esdH = new AliESDInputHandler();
    esdH->SetNeedField();  // necessary to get GoldenChi2
    mgr->SetInputEventHandler(esdH);

    AliMCEventHandler *mcH = nullptr;
    if (IsMC) {
        mcH = new AliMCEventHandler();
        mcH->SetReadTR(kFALSE);
        mgr->SetMCtruthEventHandler(mcH);
    }

    std::cout << "!! INFO !! runAnalysis.C !! Passed creation of input handlers" << std::endl;

    /* Add Helper Tasks */

    Bool_t applyPileupCuts = kFALSE;  // kFALSE in Pb-Pb (as recommended in: https://twiki.cern.ch/twiki/bin/view/ALICE/AliDPGtoolsPhysSel)
    TString TaskPhysicsSelection_Options = TString::Format("(%i, %i)", (Int_t)IsMC, (Int_t)applyPileupCuts);
    AliPhysicsSelectionTask *TaskPhysicsSelection = reinterpret_cast<AliPhysicsSelectionTask *>(
        gInterpreter->ExecuteMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C" + TaskPhysicsSelection_Options));
    if (!TaskPhysicsSelection) return;

    TString TaskCentrality_Options = "";  // nothing
    AliMultSelectionTask *TaskCentrality = reinterpret_cast<AliMultSelectionTask *>(
        gInterpreter->ExecuteMacro("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C" + TaskCentrality_Options));
    if (!TaskCentrality) return;

    TString TaskPIDResponse_Options = TString::Format("(%i, 1, 1, \"%i\")", (Int_t)IsMC, PassNumber);
    AliAnalysisTaskPIDResponse *TaskPIDResponse = reinterpret_cast<AliAnalysisTaskPIDResponse *>(
        gInterpreter->ExecuteMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C" + TaskPIDResponse_Options));
    if (!TaskPIDResponse) return;

    std::cout << "!! INFO !! runAnalysis.C !! Passed addition of helper tasks" << std::endl;

    /* Add Main Task */

    gInterpreter->LoadMacro("AliAnalysisTaskEsd2Tree.cxx++g");

    TString TaskEsd2Tree_Options = TString::Format("(%i, %i)", (Int_t)IsMC, (Int_t)IsSignalMC);
    AliAnalysisTaskEsd2Tree *TaskEsd2Tree =
        reinterpret_cast<AliAnalysisTaskEsd2Tree *>(gInterpreter->ExecuteMacro("AddTaskEsd2Tree.C" + TaskEsd2Tree_Options));
    if (!TaskEsd2Tree) return;

    std::cout << "!! INFO !! runAnalysis.C !! Passed addition of main task" << std::endl;

    /* Init Analysis Manager */

    mgr->SetDebugLevel(3);
    if (!mgr->InitAnalysis()) return;

    std::cout << "!! INFO !! runAnalysis.C !! Passed InitAnalysis" << std::endl;

    /* Start Analysis */

    TChain *chain = nullptr;
    TString FilePath = "";

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
            for (Int_t DN = 1; DN <= Local_NDirs; DN++) {
                FilePath = TString::Format("%s/%i/%03i/AliESDs.root", InputPath.Data(), RunNumber, DN);
                chain->AddFile(FilePath);
            }
        } else {  // data
            /* PENDING until further notice... */
        }
        if (!Local_LimitToNEvents)
            mgr->StartAnalysis("local", chain);  // read all events
        else
            mgr->StartAnalysis("local", chain, (Long64_t)Local_LimitToNEvents);  // read first NEvents
    }

    std::cout << "!! INFO !! runAnalysis.C !! Passed StartAnalysis" << std::endl;
}
