/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleIKManager
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleIKManager.h"

void CTaskSimpleIKManager::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleIKManager, 0x86E358, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6337F0);
    RH_ScopedInstall(Destructor, 0x633830);

    RH_ScopedInstall(AddIKChainTask, 0x633940);
    RH_ScopedInstall(GetTaskAtSlot, 0x6339B0);
    RH_ScopedVMTInstall(Clone, 0x639350);
    RH_ScopedVMTInstall(GetTaskType, 0x633820);
    RH_ScopedVMTInstall(MakeAbortable, 0x6338A0);
    RH_ScopedVMTInstall(ProcessPed, 0x6338E0);
}

// For 0x639350
CTaskSimpleIKManager::CTaskSimpleIKManager(const CTaskSimpleIKManager& o) :
    CTaskSimpleIKManager()
{
    // Como el original: solo se clonan los huecos ocupados (AddIKChainTask en linea con el mismo hueco)
    for (size_t slot = 0; slot < m_IKChainTasks.size(); slot++) {
        if (const auto t = o.m_IKChainTasks[slot]) {
            m_IKChainTasks[slot] = static_cast<CTaskSimpleIKChain*>(t->Clone());
        }
    }
}

// 0x633830
CTaskSimpleIKManager::~CTaskSimpleIKManager() {
    for (const auto task : m_IKChainTasks) {
        delete task;
    }
}

// 0x633940
void CTaskSimpleIKManager::AddIKChainTask(CTaskSimpleIKChain* task, eIKChainSlot slot) {
    if ((int32)slot < 0) { // Find free slot, and store `task` there. (Como el original: cualquier hueco negativo)
        const auto it = rng::find(m_IKChainTasks, nullptr);
        if (it != m_IKChainTasks.end()) {
            *it = task;
        }
    } else { // Store `task` in the given `slot` (Como el original: sobrescribe sin comprobar)
        m_IKChainTasks.data()[(size_t)slot] = task;
    }
}

// 0x6338A0
bool CTaskSimpleIKManager::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == eAbortPriority::ABORT_PRIORITY_IMMEDIATE) {
        for (auto& task : m_IKChainTasks) {
            delete std::exchange(task, nullptr);
        }
        return true;
    } else {
        m_IsAborting = true;
        return false;
    }
}

// 0x6338E0
bool CTaskSimpleIKManager::ProcessPed(CPed* ped) {
    if (m_IsAborting) {
        return true;
    }

    bool hasAllProcessedPed{true};
    for (auto& task : m_IKChainTasks) {
        if (task) {
            if (task->ProcessPed(ped)) {
                delete std::exchange(task, nullptr);
            } else {
                hasAllProcessedPed = false;
            }
        }
    }
    return hasAllProcessedPed;
}

