#include <AliAnalysisManager.h>

#include "AliTaskEsd2Vector.h"

AliTaskEsd2Vector *AddTaskEsd2Vector(bool is_mc, bool is_signal_mc) {

    AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
    if (mgr == nullptr) return nullptr;

    auto *task = new AliTaskEsd2Vector("AliTaskEsd2Vector");
    if (task == nullptr) return nullptr;

    // NOTE: `SelectCollisionCandidates` omitted because it's done within the task's `PassesEventSelection`
    task->Initialize(is_mc, is_signal_mc);

    mgr->AddTask(task);
    mgr->ConnectInput(task, 0, mgr->GetCommonInputContainer());
    mgr->ConnectOutput(task, 1, mgr->CreateContainer("QA", TList::Class(), mgr->kOutputContainer, mgr->GetCommonFileName()));

    return task;
}
