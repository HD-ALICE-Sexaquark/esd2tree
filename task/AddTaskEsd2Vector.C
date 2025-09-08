#include <AliAnalysisManager.h>

#include "AliAnalysisTaskEsd2Vector.h"

AliAnalysisTaskEsd2Vector *AddTaskEsd2Vector(bool is_mc, bool is_signal_mc) {

    AliAnalysisManager *mgr{AliAnalysisManager::GetAnalysisManager()};
    if (mgr == nullptr) return nullptr;

    auto *task{new AliAnalysisTaskEsd2Vector("AliAnalysisTaskEsd2Vector")};
    if (task == nullptr) return nullptr;

    task->SelectCollisionCandidates(AliVEvent::kINT7);  // from `AliPhysicsSelectionTask`
    task->Initialize(is_mc, is_signal_mc);

    mgr->AddTask(task);

    mgr->ConnectInput(task, 0, mgr->GetCommonInputContainer());

    mgr->ConnectOutput(task, 1, mgr->CreateContainer("QA", TList::Class(), mgr->kOutputContainer, mgr->GetCommonFileName()));
    mgr->ConnectOutput(task, 2, mgr->CreateContainer("Events", TTree::Class(), mgr->kOutputContainer, mgr->GetCommonFileName()));

    return task;
}
