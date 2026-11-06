#include "StdInc.h"
#include "TaskCategories.h"

void CTaskCategories::InjectHooks() {
    RH_ScopedClass(CTaskCategories);
    RH_ScopedCategory("Tasks");

    RH_ScopedInstall(ComputeTaskCategory, 0x698680);
    RH_ScopedInstall(IsFollowPedTask, 0x698610);
    RH_ScopedInstall(IsKillPedTask, 0x6985E0);
}

// 0x698680
// Como el original: consulta GetTaskType tres veces (ranura 4 de la vtabla, sin guardar el resultado entre consultas), con comparaciones con signo
// del rango TASK_SIMPLE_FIGHT_CTRL..TASK_SIMPLE_THROW_CTRL (0x3FB..0x3FD). Con tarea nula el EXE falla al leer la vtabla, igual que aqui.
int8 CTaskCategories::ComputeTaskCategory(const CTask* task) {
    int8 category = task != nullptr;
    const auto IsInRange = [task] {
        const int32 type = const_cast<CTask*>(task)->GetTaskType();
        return type >= int32(TASK_SIMPLE_FIGHT_CTRL) && type <= int32(TASK_SIMPLE_THROW_CTRL);
    };
    if (IsInRange()) {
        category = 4;
    }
    if (IsInRange()) {
        category = 2;
    }
    return IsInRange() ? 4 : category;
}

void CTaskCategories::IsFleePedTask(const CTask* task, bool& a2, bool& a3) {
    // unused
}

// 0x698610
void CTaskCategories::IsFollowPedTask(const CTask* task, bool& a2, bool& a3) {
    a2 = false;
    a3 = false;
}

// 0x6985E0
void CTaskCategories::IsKillPedTask(const CTask* task, bool& a2, bool& a3) {
    a2 = false;
    a3 = false;
    auto type = const_cast<CTask*>(task)->GetTaskType();
    if (type >= TASK_SIMPLE_FIGHT_CTRL && type <= TASK_SIMPLE_THROW_CTRL)
        a2 = true;
}
