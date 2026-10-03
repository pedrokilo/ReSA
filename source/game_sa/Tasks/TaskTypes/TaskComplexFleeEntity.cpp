/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexFleeEntity
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
#include "TaskComplexFleeEntity.h"
#include "TaskComplexFleePoint.h"
#include "Ragdoll/IKChainManager.h"
#include "InterestingEvents.h"
#include "EventSeenPanickedPed.h"

void CTaskComplexFleeEntity::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFleeEntity, 0x86f708, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65B930);
    RH_ScopedInstall(Destructor, 0x65B9D0);

    RH_ScopedInstall(CreateSubTask, 0x65BA30);

    RH_ScopedVMTInstall(Clone, 0x65CE50);
    RH_ScopedVMTInstall(GetTaskType, 0x65B9C0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65BAC0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65BB00);
    RH_ScopedVMTInstall(ControlSubTask, 0x65BBB0);
}

// 0x65B930
CTaskComplexFleeEntity::CTaskComplexFleeEntity(
    CEntity* fleeEntity,
    bool scream,
    float safeDistance,
    int32 fleeTime,
    int32 posCheckPeriod,
    float posChangeTolerance
) :

    m_entity{ fleeEntity },
    m_pedScream{ scream },
    m_safeDistance{ safeDistance },
    m_time{ fleeTime },
    m_posCheckPeriod{ posCheckPeriod },
    m_posChangeTolerance{ posChangeTolerance }
{
    CEntity::SafeRegisterRef(m_entity);
}

CTaskComplexFleeEntity::CTaskComplexFleeEntity(const CTaskComplexFleeEntity& o) :
    CTaskComplexFleeEntity{ o.m_entity, o.m_pedScream, o.m_safeDistance, o.m_time, o.m_posCheckPeriod, o.m_posChangeTolerance }
{
}

// 0x65B9D0
CTaskComplexFleeEntity::~CTaskComplexFleeEntity() {
    CEntity::SafeCleanUpRef(m_entity);
}

// 0x65BA30
CTask* CTaskComplexFleeEntity::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_COMPLEX_FLEE_POINT:
        return new CTaskComplexFleePoint{
            m_pos,
            m_pedScream,
            m_safeDistance,
            m_time
        };
    default: // Como el original: TASK_FINISHED y cualquier otro tipo devuelven nullptr
        return nullptr;
    }
}

// 0x65BAC0
CTask* CTaskComplexFleeEntity::CreateNextSubTask(CPed* ped) {
    // Not even sure what's the point here, both returns null xD

    if (m_entity && !notsa::isa<CTaskComplexFleePoint>(m_pSubTask)) {
        return nullptr;
    }
    return CreateSubTask(TASK_FINISHED);
}

// 0x65BB00
CTask* CTaskComplexFleeEntity::CreateFirstSubTask(CPed* ped) {
    if (!m_entity) { // NOTSA: el original no lo comprueba (leería el tipo de una entidad nula)
        return nullptr;
    }

    g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_13, ped);

    if (!g_ikChainMan.IsLooking(ped)) {
        g_ikChainMan.LookAt(
            "TaskFleeEntity",
            ped,
            m_entity,
            3000,
            m_entity->GetIsTypePed() // Como el original: se mira el tipo de entidad (peatón), no el estado
            ? BONE_HEAD
            : BONE_UNKNOWN,
            nullptr,
            true,
            0.25f,
            500,
            3,
            false
        );
    }

    // Como el original: el temporizador se arranca sin comprobar el periodo y la posición se copia aquí
    m_posCheckTimer.m_nStartTime = CTimer::GetTimeInMS();
    m_posCheckTimer.m_nInterval = m_posCheckPeriod;
    m_posCheckTimer.m_bStarted = true;
    m_pos = m_entity->GetPosition();
    return CreateSubTask(TASK_COMPLEX_FLEE_POINT);
}

// 0x65BBB0
CTask* CTaskComplexFleeEntity::ControlSubTask(CPed* ped) {
    if (!m_entity) {
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
        return m_pSubTask;
    }
    if (m_pSubTask->GetTaskType() != TASK_COMPLEX_FLEE_POINT || !m_posCheckTimer.m_bStarted) {
        return m_pSubTask;
    }
    if (m_posCheckTimer.m_bStopped) {
        m_posCheckTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_posCheckTimer.m_bStopped = false;
    }
    const uint32 now = CTimer::GetTimeInMS();
    if (now < m_posCheckTimer.m_nStartTime + (uint32)m_posCheckTimer.m_nInterval) {
        return m_pSubTask;
    }

    // Como el original: (z*z + y*y) + x*x de m_pos - posición actual; solo se actualiza si supera estrictamente
    // la tolerancia (NaN no actualiza) y el temporizador solo se reinicia al actualizar
    const auto& currPos = m_entity->GetPosition();
    const float dx = m_pos.x - currPos.x;
    const float dy = m_pos.y - currPos.y;
    const float dz = m_pos.z - currPos.z;
    if (!(m_posChangeTolerance * m_posChangeTolerance < dz * dz + dy * dy + dx * dx)) {
        return m_pSubTask;
    }
    m_posCheckTimer.m_nStartTime = now;
    m_posCheckTimer.m_nInterval = m_posCheckPeriod;
    m_posCheckTimer.m_bStarted = true;
    m_pos = m_entity->GetPosition();
    static_cast<CTaskComplexFleePoint*>(m_pSubTask)->SetFleePosition(m_pos, m_safeDistance, m_pedScream);

    CEventSeenPanickedPed event{ ped };
    GetEventGlobalGroup()->Add(&event, false);

    return m_pSubTask;
}
