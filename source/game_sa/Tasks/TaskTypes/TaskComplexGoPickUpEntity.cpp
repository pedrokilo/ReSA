/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGoPickUpEntity
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexGoPickUpEntity.h"

#include "AnimManager.h"

static_assert(TASK_COMPLEX_GO_PICKUP_ENTITY == 0x136); // GetTaskType del EXE (0x691A40)

void CTaskComplexGoPickUpEntity::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoPickUpEntity, 0x870B98, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6919C0);
    RH_ScopedInstall(Destructor, 0x691A50);
}

// 0x6919C0
CTaskComplexGoPickUpEntity::CTaskComplexGoPickUpEntity(CEntity* entity, int32 animGroupId) : CTaskComplex() {
    m_pEntity = entity;
    m_vecPosition = CVector{ -999.0f, 0.0f, 0.0f };
    m_vecPickupPosition = CVector{ -999.0f, 0.0f, 0.0f };
    m_nAnimGroupId = animGroupId;
    if (m_pEntity) {
        m_pEntity->RegisterReference(&m_pEntity);
    }
    m_nTimePassedSinceLastSubTaskCreatedInMs = 0;
    m_bAnimBlockReferenced = false;
}

// 0x691A50
CTaskComplexGoPickUpEntity::~CTaskComplexGoPickUpEntity() {
    if (m_pEntity) {
        m_pEntity->CleanUpOldReference(&m_pEntity);
    }
    if (m_bAnimBlockReferenced) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(static_cast<AssocGroupId>(m_nAnimGroupId)));
        m_bAnimBlockReferenced = false;
    }
}

// 0x692C80: CTask::operator new (0x61A5A0) devuelve nulo si el pool esta agotado y el EXE lo comprueba antes de construir.
CTask* CTaskComplexGoPickUpEntity::Clone() const {
    void* const memory = CTask::operator new(sizeof(CTaskComplexGoPickUpEntity));
    return memory ? ::new (memory) CTaskComplexGoPickUpEntity(m_pEntity, m_nAnimGroupId) : nullptr;
}

// 0x691AE0
CTask* CTaskComplexGoPickUpEntity::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x691AE0, CTask*, CPed*>(this, ped);
}

// 0x693610
CTask* CTaskComplexGoPickUpEntity::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x693610, CTask*, CPed*>(this, ped);
}

// 0x691D50
CTask* CTaskComplexGoPickUpEntity::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x691D50, CTask*, CPed*>(this, ped);
}
