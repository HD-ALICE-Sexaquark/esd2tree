#include <fstream>

#include "Rtypes.h"
#include "TChain.h"
#include "TGrid.h"
#include "TSystem.h"

#include "AliAnalysisAlien.h"
#include "AliAnalysisManager.h"
#include "AliESDInputHandler.h"
#include "AliMCEventHandler.h"

#include "AliAnalysisTaskPIDResponse.h"
#include "AliMultSelectionTask.h"
#include "AliPhysicsSelectionTask.h"

#include "AliAnalysisTaskEsd2Tree.h"

void runAnalysis(TString Mode,            // "local", "grid", "hybrid"
                 TString InputPath,       // (only valid when Mode == "local" or Mode == "hybrid") dir that contains ProductionName dirs
                 Int_t LocalNDirs,        // (only valid when Mode == "local" or Mode == "hybrid") number of subdirs per run
                 Bool_t GridTestMode,     // (only valid when Mode == "grid")
                 Bool_t IsMC,             // kTRUE for MC, kFALSE for data
                 TString ProductionName,  // for data: "LHC15o", "LHC18q", "LHC18r"
                                          // for signal MC: "LHC23l1a3", "LHC23l1b3"
                                          // for gen. purp. MC: "LHC20e3a", "LHC20j6a"
                 TString RunNumbersList,  // path to file with run numbers
                 TString SimulationSet,   // format: "<A,D,E,H><1.73,1.8,1.87,1.94,2.01>" e.g. "A1.73"
                 Int_t ChooseNEvents = 0  // (only valid when Mode == "local" or Mode == "hybrid") 0 means all events
) {

    /* Check for Input Errors */

    if (Mode != "local" && Mode != "grid" && Mode != "hybrid") {
        std::cerr << "!! ERROR !! runAnalysis.C !! Invalid Mode " << Mode << std::endl;
        return;
    }

    if (Mode == "local" || Mode == "hybrid") {
        if (InputPath == "") {
            std::cerr << "!! ERROR !! runAnalysis.C !! InputPath cannot be empty when Mode == \"local\" or \"hybrid\"" << std::endl;
            return;
        }
        if (GridTestMode) {
            std::cerr << "!! ERROR !! runAnalysis.C !! GridTestMode is only valid when Mode == \"grid\"" << std::endl;
            return;
        }
    }

    if (Mode == "grid") {
        if (InputPath != "")
            std::cerr << "!! WARNING !! runAnalysis.C !! InputPath is only valid when Mode == \"local\" or \"hybrid\", will be ignored" << std::endl;
        if (ChooseNEvents)
            std::cerr << "!! WARNING !! runAnalysis.C !! ChooseNEvents is only valid when Mode == \"local\" or \"hybrid\", will be ignored"
                      << std::endl;
    }

    if (IsMC && ProductionName != "LHC23l1a3" && ProductionName != "LHC23l1b3" && ProductionName != "LHC20e3a" && ProductionName != "LHC20j6a") {
        std::cerr << "!! ERROR !! runAnalysis.C !! Make sure to put a valid ProductionName for simulations" << std::endl;
        return;
    }

    if (!IsMC && ProductionName != "LHC15o" && ProductionName != "LHC18q" && ProductionName != "LHC18r") {
        std::cerr << "!! ERROR !! runAnalysis.C !! Make sure to put a valid ProductionName for data" << std::endl;
        return;
    }

    std::cout << "!! INFO !! runAnalysis.C !! Passed initial input checks" << std::endl;

    /* Determine Further Options */

    Bool_t IsSignalMC = ProductionName.Contains("23l1");

    Int_t PassNumber = 3;  // default for 18qr and anchored sims
    if (ProductionName == "LHC15o" || ProductionName == "LHC20j6a" || ProductionName == "LHC23l1b3") PassNumber = 2;

    TString ProductionYear = "20" + ProductionName(3, 2);

    TString DataPath = TString::Format("%s/%s", InputPath.Data(), ProductionName.Data());
    if (IsSignalMC) DataPath += TString::Format("/%s", SimulationSet.Data());

    TString GridDataDir = TString::Format("/alice/data/%s/%s", ProductionYear.Data(), ProductionName.Data());
    if (IsMC) GridDataDir = TString::Format("/alice/sim/%s/%s", ProductionYear.Data(), ProductionName.Data());
    if (IsSignalMC) GridDataDir += TString::Format("/%s", SimulationSet.Data());

    TString GridDataPattern = TString::Format("/pass%i/*/AliESDs.root", PassNumber);
    if (IsMC) GridDataPattern = "/*/AliESDs.root";

    TString GridWorkingDir = TString::Format("work/Esd2Tree_Data_%s", ProductionName.Data());
    if (IsMC) GridWorkingDir = TString::Format("work/Esd2Tree_MC_%s", ProductionName.Data());
    if (IsSignalMC) GridWorkingDir += TString::Format("_%s", SimulationSet.Data());

    TString GridOutputDir = "output";

    std::vector<Int_t> RunNumbersFromList;
    std::ifstream RunNumbersFile(RunNumbersList);
    if (!RunNumbersFile.is_open()) {
        std::cerr << "!! ERROR !! runAnalysis.C !! Unable to open file " << RunNumbersList << std::endl;
        return;
    }
    Int_t SingleRN;
    while (RunNumbersFile >> SingleRN) RunNumbersFromList.push_back(SingleRN);
    RunNumbersFile.close();

    std::cout << "!! INFO !! runAnalysis.C !! Passed further options" << std::endl;

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
        alienHandler->SetGridDataDir(GridDataDir);
        if (!IsMC) alienHandler->SetRunPrefix("000");
        for (Int_t &RN : RunNumbersFromList) alienHandler->AddRunNumber(RN);
        alienHandler->SetDataPattern(GridDataPattern);
        alienHandler->SetTTL(3600);
        alienHandler->SetOutputToRunNo(kTRUE);
        alienHandler->SetDefaultOutputs(kFALSE);
        alienHandler->SetOutputFiles("AnalysisResults.root SimpleTrees.root");
        alienHandler->SetKeepLogs(kTRUE);
        alienHandler->SetMergeViaJDL(kFALSE);
        // alienHandler->SetMaxMergeStages(1);
        alienHandler->SetGridWorkingDir(GridWorkingDir);
        alienHandler->SetGridOutputDir(GridOutputDir);
        alienHandler->SetJDLName("TaskEsd2Tree.jdl");
        alienHandler->SetExecutable("TaskEsd2Tree.sh");

        mgr->SetGridHandler(alienHandler);

        std::cout << "!! INFO !! runAnalysis.C !! Passed grid connection" << std::endl;
    }

    TGrid *grid_connection = nullptr;

    if (Mode == "hybrid") {
        if (!gGrid) {
            grid_connection = TGrid::Connect("alien://");
            if (!grid_connection) return;
        }
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
    TString Prefix = "";
    if (Mode == "hybrid") Prefix = "alien://";
    TString FilePath = "";
    TString DirString = "";

    if (Mode == "grid") {
        if (GridTestMode) {
            alienHandler->SetNtestFiles(5);  // hardcoded
            alienHandler->SetRunMode("test");
        } else {
            alienHandler->SetSplitMaxInputFileNumber(35);  // hardcoded
            alienHandler->SetRunMode("full");
        }
        mgr->StartAnalysis("grid");
    } else {  // "local" or "hybrid" mode
        chain = new TChain("esdTree");
        for (Int_t &RN : RunNumbersFromList) {
            if (IsMC) {
                for (Int_t DN = 1; DN <= LocalNDirs; DN++) {
                    FilePath = TString::Format("%s%s/%i/%03i/AliESDs.root", Prefix.Data(), DataPath.Data(), RN, DN);
                    chain->AddFile(FilePath);
                }
            } else {  // !IsMC (data)
                for (Int_t DN = 1; DN <= 99; DN++) {
                    for (Int_t sDN = 100; sDN <= 5000; sDN++) {
                        DirString = TString::Format("%s000%i%03i.%i", TString(ProductionName(3, 2)).Data(), RN, DN, sDN);
                        FilePath =
                            TString::Format("%s%s/000%i/pass%i/%s/AliESDs.root", Prefix.Data(), DataPath.Data(), RN, PassNumber, DirString.Data());
                        if (gSystem->AccessPathName(FilePath)) continue;
                        chain->AddFile(FilePath);
                    }
                }
            }
        }
        if (!ChooseNEvents)
            mgr->StartAnalysis("local", chain);  // read all events
        else
            mgr->StartAnalysis("local", chain, (Long64_t)ChooseNEvents);  // read first NEvents
    }

    std::cout << "!! INFO !! runAnalysis.C !! Passed StartAnalysis" << std::endl;
}
