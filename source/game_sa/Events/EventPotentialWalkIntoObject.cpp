/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPotentialWalkIntoObject
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
#include "EventPotentialWalkIntoObject.h"


namespace {
// 0x4AE670 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventPotentialWalkIntoObject que usan los destructores del EXE. Deja la vtable de CEventPotentialWalkIntoObject del EXE (0x85AF18), libera la referencia de m_object (+0x14) con CEntity::CleanUpOldReference (0x571A00) si no son nulas
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall PotentialWalkIntoObjectDestructorBody(CEventPotentialWalkIntoObject* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AF18;
    CEntity::SafeCleanUpRef(self->m_object);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
// 0x4B5F40 (lote 204A, sin validar; el listado decia CEventDamage; ranura 0 de la vtable de CEventPotentialWalkIntoObject; thiscall, un argumento de pila, `ret 4`, 76 bytes): destructor con borrado del EXE. Como el original: llama al cuerpo del destructor
// (0x4AE670, con hook propio) y, con el bit 0 del BYTE de banderas, libera la ranura en el pool de eventos (0xB744AC) EN LINEA, igual que CEvent::operator delete (0x4B5630, con hook: indice = (this - almacenamiento) / 0x44 con signo,
// estados[indice] |= 0x80, cursor = min con signo); devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
__declspec(noinline) CEventPotentialWalkIntoObject* __fastcall PotentialWalkIntoObjectScalarDeletingDestructor(CEventPotentialWalkIntoObject* self, void* /*edx*/, uint8 flags) {
    PotentialWalkIntoObjectDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventPotentialWalkIntoObject::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPotentialWalkIntoObject, 0x85AF18, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(PotentialWalkIntoObjectDestructorBody, "Destructor", 0x4AE670);
    RH_ScopedNamedGlobalInstall(PotentialWalkIntoObjectScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B5F40); // lote 204A (sin validar)

    RH_ScopedInstall(Constructor, 0x4AE5D0);
    RH_ScopedVMTInstall(GetEventType, 0x4AE640); // lote 152A (banco Fn152A)
    RH_ScopedVMTInstall(GetLifeTime, 0x4AE650); // lote 152A (banco Fn152A)
    RH_ScopedVMTInstall(GetEventPriority, 0x4AE660); // lote 152A (banco Fn152A)
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4AE6D0); // lote 152A (banco Fn152A)
    RH_ScopedVMTInstall(AffectsPed, 0x4B4950);
    RH_ScopedVMTInstall(CloneEditable, 0x4B7020); // lote 202A (sin validar): ranura 16
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

// 0x4B7020 (lote 202A, sin validar; el listado decia CEventVehicleCollision; ranura 16 (CloneEditable) de la vtable 0x85AF18; thiscall, sin argumentos, `ret`, 101 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite)).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x1C bytes; sin memoria devuelve nulo sin leer campos; si no, el constructor 0x4AE5D0 (Constructor, con hook) con (objeto +0x14, estado de movimiento +0x18) y devuelve el evento nuevo.
CEventPotentialWalkIntoObject* CEventPotentialWalkIntoObject::CloneEditable() const noexcept {
    auto* const object = static_cast<CEventPotentialWalkIntoObject*>(CEvent::operator new(sizeof(CEventPotentialWalkIntoObject)));
    if (!object) {
        return nullptr;
    }
    return object->Constructor(m_object, m_moveState);
}

// 0x4AE640 (lote 152A, banco Fn152A): `mov eax,2Ch; ret`
eEventType CEventPotentialWalkIntoObject::GetEventType() const { return EVENT_POTENTIAL_WALK_INTO_OBJECT; }

// 0x4AE6D0 (lote 152A, banco Fn152A): `mov al,1; ret 4` (solo AL; el evento recibido no se lee)
bool CEventPotentialWalkIntoObject::TakesPriorityOver(const CEvent& refEvent) { return true; }

// 0x4AE660 (lote 152A, banco Fn152A): `mov eax,1Bh; ret`
int32 CEventPotentialWalkIntoObject::GetEventPriority() const { return 27; }

// 0x4AE650 (lote 152A, banco Fn152A): `xor eax,eax; ret`
int32 CEventPotentialWalkIntoObject::GetLifeTime() { return 0; }

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