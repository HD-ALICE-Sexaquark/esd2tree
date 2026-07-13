#include <AliAnalysisManager.h>

#include "AliTaskEsd2Tree.h"

AliTaskEsd2Tree *AddTaskEsd2Tree(bool is_mc, bool is_sexa_mc, bool is_hdib_mc) {

    AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
    if (mgr == nullptr) return nullptr;

    auto *task = new AliTaskEsd2Tree("AliTaskEsd2Tree");

    // NOTE: `SelectCollisionCandidates` omitted because it's done within the task's `PassesEventSelection`
    task->Initialize(is_mc, is_sexa_mc, is_hdib_mc);

    mgr->AddTask(task);
    mgr->ConnectInput(task, 0, mgr->GetCommonInputContainer());
    mgr->ConnectOutput(task, 1, mgr->CreateContainer("QA", TList::Class(), mgr->kOutputContainer, mgr->GetCommonFileName()));
    mgr->ConnectOutput(task, 2, mgr->CreateContainer("Events", TTree::Class(), mgr->kOutputContainer, mgr->GetCommonFileName()));

    return task;
}
