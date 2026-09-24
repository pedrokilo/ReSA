/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPotentialWalkIntoObject
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventPotentialWalkIntoObject.h"


void CEventPotentialWalkIntoObject::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPotentialWalkIntoObject, 0x85AF18, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4AE5D0);
    RH_ScopedVMTInstall(AffectsPed, 0x4B4950);
}

// 0x4AE5D0
CEventPotentialWalkIntoObject::CEventPotentialWalkIntoObject(CObject* object, int32 moveState) {
    m_object = object;
    m_moveState = moveState;
    CEntity::SafeRegisterRef(m_object);
}

CEventPotentialWalkIntoObject::~CEventPotentialWalkIntoObject() {
    CEntity::SafeCleanUpRef(m_object);
}

// 0x4AE5D0
CEventPotentialWalkIntoObject* CEventPotentialWalkIntoObject::Constructor(CObject* object, int32 moveState) {
    this->CEventPotentialWalkIntoObject::CEventPotentialWalkIntoObject(object, moveState);
    return this;
}

// 0x4B4950
bool CEventPotentialWalkIntoObject::AffectsPed(CPed* ped) {
    if (ped->IsPlayer() || !ped->IsAlive() || !m_object)
        return false;

    if (m_moveState != PEDMOVE_STILL
        && !ped->m_pAttachedTo
        && m_object->m_pAttachedTo != ped
        && !m_object->physicalFlags.bDisableMoveForce
    ) {
        CColModel* colModel = m_object->GetModelInfo()->GetColModel();
        CVector length = colModel->GetBoundingBox().GetSize();
        if (length.x >= 0.01f && length.y >= 0.01f && length.z >= 0.01f) {
            CTask* activeTask = ped->GetTaskManager().GetActiveTask();
            if (!activeTask)
                return true;

            assert(activeTask->GetTaskType() != TASK_COMPLEX_AVOID_ENTITY); // unused task
            if (activeTask->GetTaskType() == TASK_COMPLEX_WALK_ROUND_OBJECT) {
                auto taskWalkRoundObject = reinterpret_cast<CTaskComplexWalkRoundObject*>(activeTask);
                if (taskWalkRoundObject->m_object == m_object)
                    return false;
            }
            return true;
        }
    }

    return false;
}