#include "AliAnalysisManager.h"
#include "AliAnalysisTaskEsd2Tree.h"

AliAnalysisTaskEsd2Tree *AddTaskEsd2Tree(Bool_t IsMC = kTRUE, Bool_t IsSignalMC = kTRUE) {

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

    mgr->ConnectOutput(task, 1, mgr->CreateContainer("QA", TList::Class(), AliAnalysisManager::kOutputContainer, filename.Data()));
    mgr->ConnectOutput(task, 2, mgr->CreateContainer("Events", TTree::Class(), AliAnalysisManager::kOutputContainer, filename.Data()));

    return task;
}
