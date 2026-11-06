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
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventPotentialWalkIntoObject.h"


namespace {
// 0x4AE670 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventPotentialWalkIntoObject que usan los destructores del EXE. Deja la vtable de CEventPotentialWalkIntoObject del EXE (0x85AF18), libera la referencia de m_object (+0x14) con CEntity::CleanUpOldReference (0x571A00) si no son nulas
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall PotentialWalkIntoObjectDestructorBody(CEventPotentialWalkIntoObject* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AF18;
    CEntity::SafeCleanUpRef(self->m_object);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventPotentialWalkIntoObject::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPotentialWalkIntoObject, 0x85AF18, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(PotentialWalkIntoObjectDestructorBody, "Destructor", 0x4AE670);

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
        // Como el original: solo se descarta si alguna medida es menor que 0.01 (NaN pasa)
        if (!(length.x < 0.01f) && !(length.y < 0.01f) && !(length.z < 0.01f)) {
            CTask* activeTask = ped->GetTaskManager().GetActiveTask();
            if (!activeTask)
                return true;

            // Como el original: CTaskComplexAvoidEntity guarda la entidad en +0xC
            if (activeTask->GetTaskType() == TASK_COMPLEX_AVOID_ENTITY && *reinterpret_cast<CEntity**>(reinterpret_cast<uint8*>(activeTask) + 0xC) == m_object)
                return false;
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