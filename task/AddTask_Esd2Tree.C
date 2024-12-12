#include "AliAnalysisManager.h"
#include "AliAnalysisTaskEsd2Tree.h"

AliAnalysisTaskEsd2Tree *AddTask_Esd2Tree(Bool_t IsMC = kTRUE, Bool_t IsSignalMC = kTRUE) {

    AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
    if (!mgr) return nullptr;

    AliAnalysisTaskEsd2Tree *task = new AliAnalysisTaskEsd2Tree("AliAnalysisTaskEsd2Tree");
    if (!task) return nullptr;

    task->SelectCollisionCandidates(AliVEvent::kINT7);  // from `AliPhysicsSelectionTask`

    task->IsMC(IsMC, IsSignalMC);
    task->Initialize();

    mgr->AddTask(task);

    mgr->ConnectInput(task, 0, mgr->GetCommonInputContainer());

    TString filename = AliAnalysisManager::GetCommonFileName();
    AliAnalysisManager::EAliAnalysisContType output_container = AliAnalysisManager::kOutputContainer;

    mgr->ConnectOutput(task, 1, mgr->CreateContainer("Events", TTree::Class(), output_container, filename.Data()));
    mgr->ConnectOutput(task, 2, mgr->CreateContainer("Injected", TTree::Class(), output_container, filename.Data()));
    mgr->ConnectOutput(task, 3, mgr->CreateContainer("MC", TTree::Class(), output_container, filename.Data()));
    mgr->ConnectOutput(task, 4, mgr->CreateContainer("Tracks", TTree::Class(), output_container, filename.Data()));

    return task;
}
