/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleAffectSecondaryBehaviour
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleAffectSecondaryBehaviour.h"

void CTaskSimpleAffectSecondaryBehaviour::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleAffectSecondaryBehaviour, 0x870b08, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x691270);

    RH_ScopedVMTInstall(Clone, 0x692910);
    RH_ScopedVMTInstall(GetTaskType, 0x6912A0);
    RH_ScopedVMTInstall(MakeAbortable, 0x6912B0);
    RH_ScopedVMTInstall(ProcessPed, 0x691320);
}

// 0x691270
CTaskSimpleAffectSecondaryBehaviour::CTaskSimpleAffectSecondaryBehaviour(bool add, eSecondaryTask secondaryTaskType, CTask* task) :
    m_bAdd{add},
    m_secTaskType{secondaryTaskType},
    m_task{task}
{
}

// NOTSA
CTaskSimpleAffectSecondaryBehaviour::CTaskSimpleAffectSecondaryBehaviour(const CTaskSimpleAffectSecondaryBehaviour& o) :
    CTaskSimpleAffectSecondaryBehaviour{o.m_bAdd, o.m_secTaskType, o.m_task ? o.m_task->Clone() : nullptr}
{
}

// 0x6912C0
CTaskSimpleAffectSecondaryBehaviour::~CTaskSimpleAffectSecondaryBehaviour() {
    delete m_task; // Como el original: la tarea guardada se borra con su destructor virtual
}

// 0x692910
CTask* CTaskSimpleAffectSecondaryBehaviour::Clone() const {
    // Como el original: la tarea guardada se clona antes de reservar la nueva tarea en el pool
    const auto task = m_task ? m_task->Clone() : nullptr;
    return new CTaskSimpleAffectSecondaryBehaviour(m_bAdd, m_secTaskType, task);
}

// 0x691320
bool CTaskSimpleAffectSecondaryBehaviour::ProcessPed(CPed* ped) {
    const auto currSecTask = ped->GetTaskManager().GetTaskSecondary(m_secTaskType);

    if (m_bAdd) {
        if (!currSecTask || currSecTask->MakeAbortable(ped)) {
            ped->GetTaskManager().SetTaskSecondary(m_task->Clone(), m_secTaskType);
            return true;
        }
        return false;
    }

    // Como el original: sin tarea secundaria termina; si la hay se pide abortar (LEISURE) y se sigue esperando
    if (currSecTask) {
        currSecTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
        return false;
    }
    return true;
}
