/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexEnterCarAsDriverTimed
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexEnterCarAsDriverTimed.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskSimpleCarSetPedInAsDriver.h"

void CTaskComplexEnterCarAsDriverTimed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEnterCarAsDriverTimed, 0x86e720, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63AD70);
    RH_ScopedInstall(Destructor, 0x63AE00);

    RH_ScopedVMTInstall(Clone, 0x63D780);
    RH_ScopedVMTInstall(GetTaskType, 0x63ADF0);
    RH_ScopedVMTInstall(StopTimer, 0x63AFF0);
    RH_ScopedVMTInstall(MakeAbortable, 0x63AE60);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x63AED0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6405B0);
    RH_ScopedVMTInstall(ControlSubTask, 0x63AEE0);
}

// 0x63AD70
CTaskComplexEnterCarAsDriverTimed::CTaskComplexEnterCarAsDriverTimed(CVehicle* veh, int32 time) :
    m_veh{veh},
    m_enterMaxInterval{time}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskComplexEnterCarAsDriverTimed::CTaskComplexEnterCarAsDriverTimed(const CTaskComplexEnterCarAsDriverTimed& o) :
    CTaskComplexEnterCarAsDriverTimed{ o.m_veh, o.m_enterMaxInterval }
{
    m_moveState = o.m_moveState;
}

// 0x63AE00
CTaskComplexEnterCarAsDriverTimed::~CTaskComplexEnterCarAsDriverTimed() {
    CEntity::SafeCleanUpRef(m_veh);
}

// 0x63AFF0
void CTaskComplexEnterCarAsDriverTimed::StopTimer(CEvent const* event) {
    if (!CEventHandler::IsTemporaryEvent(*event)) {
        m_enterBeganTimer.Pause();
    }
}

// 0x63AE60
bool CTaskComplexEnterCarAsDriverTimed::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (!m_pSubTask->MakeAbortable(ped, priority, event)) {
        return false;
    }
    // Como el original: primero la prioridad y después IsTemporaryEvent
    switch (priority) {
    case ABORT_PRIORITY_URGENT:
    case ABORT_PRIORITY_IMMEDIATE:
        if (!event || !CEventHandler::IsTemporaryEvent(*event)) {
            m_enterBeganTimer.Pause();
        }
        break;
    default:
        break;
    }
    return true;
}

// 0x6405B0
CTask* CTaskComplexEnterCarAsDriverTimed::CreateFirstSubTask(CPed* ped) {
    m_enterBeganTimer.Start(m_enterMaxInterval);
    return new CTaskComplexEnterCarAsDriver{ m_veh, m_moveState };
}

// 0x63AEE0
CTask* CTaskComplexEnterCarAsDriverTimed::ControlSubTask(CPed* ped) {
    if (!m_veh) {
        return nullptr;
    }

    if (!m_enterBeganTimer.IsOutOfTime()) {
        return m_pSubTask;
    }

    // Check if ped has got into the vehicle yet
    if (!m_pSubTask->MakeAbortable(ped) || ped->bInVehicle || m_veh->m_pDriver) {
        return m_pSubTask;
    }

    // Noep, still not in th evehicle, warp them into it
    return new CTaskSimpleCarSetPedInAsDriver{ m_veh, true };
}
