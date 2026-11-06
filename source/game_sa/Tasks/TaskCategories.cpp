/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskCategories
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

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
