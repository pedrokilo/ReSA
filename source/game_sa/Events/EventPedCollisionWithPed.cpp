/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPedCollisionWithPed
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

#include "EventPedCollisionWithPed.h"
#include "TaskComplexAvoidOtherPedWhileWandering.h"

namespace CEventBody {
// 0x4ACA70 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventPedCollisionWithPed que usan los destructores del EXE. Deja la vtable de CEventPedCollisionWithPed del EXE (0x85AC68), libera la referencia de m_victim (+0x14) con CEntity::CleanUpOldReference (0x571A00) si no son nulas
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall PedCollisionWithPedDestructorBody(CEventPedCollisionWithPed* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AC68;
    CEntity::SafeCleanUpRef(self->m_victim);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}

// 0x4B5980 (lote 190A, sin validar): destructor con borrado de CEventPedCollisionWithPed del EXE (ranura 0 de su vtable 0x85AC68; thiscall, un argumento de pila, `ret 4`, 76 bytes: `push esi; push edi; mov edi,ecx; call 0x4ACA70; test byte [esp+0Ch],1; je ..;`
// y, con el bit 0 del BYTE de banderas, el operator delete de CEvent EN LINEA (pool de eventos 0xB744AC, como 0x4B5630); devuelve `this`). Como el original: destruye con el cuerpo de la clase (0x4ACA70), libera `this`
// con CEvent::operator delete (0x4B5630, con hook; mismo efecto que la copia en linea) solo si el bit 0 esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventPedCollisionWithPed* __fastcall PedCollisionWithPedScalarDeletingDestructor(CEventPedCollisionWithPed* self, void* /*edx*/, uint8 flags) {
    CEventBody::PedCollisionWithPedDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace CEventBody

void CEventPedCollisionWithPed::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPedCollisionWithPed, 0x85AC68, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(CEventBody::PedCollisionWithPedDestructorBody, "Destructor", 0x4ACA70);
    RH_ScopedNamedGlobalInstall(CEventBody::PedCollisionWithPedScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B5980); // lote 190A (sin validar)

    RH_ScopedInstall(Constructor, 0x4AC990);
    RH_ScopedVMTInstall(GetEventType, 0x4ACA40);     // lote 137A (banco Fn137A)
    RH_ScopedVMTInstall(GetLifeTime, 0x4ACA50);      // lote 137A
    RH_ScopedVMTInstall(GetEventPriority, 0x4ACA60); // lote 137A
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4ACAD0);
    RH_ScopedVMTInstall(AffectsPed, 0x4ACB10);
}

// 0x4AC990
CEventPedCollisionWithPed::CEventPedCollisionWithPed(int16 pieceType, float damageIntensity, CPed* victim, const CVector& collisionImpactVelocity, const CVector& collisionPos, eMoveState moveState, eMoveState victimMoveState)
{
    m_pieceType               = pieceType;
    m_damageIntensity         = damageIntensity;
    m_victim                  = victim;
    m_collisionImpactVelocity = collisionImpactVelocity;
    m_collisionPos            = collisionPos;
    m_movestate               = moveState;
    m_victimMoveState         = victimMoveState;
    CEntity::SafeRegisterRef(m_victim);
}

CEventPedCollisionWithPed::~CEventPedCollisionWithPed()
{
    CEntity::SafeCleanUpRef(m_victim);
}

// 0x4AC990
CEventPedCollisionWithPed* CEventPedCollisionWithPed::Constructor(int16 pieceType, float damageIntensity, CPed* victim, const CVector& collisionImpactVelocity, const CVector& collisionPos, eMoveState moveState, eMoveState victimMoveState)
{
    this->CEventPedCollisionWithPed::CEventPedCollisionWithPed(pieceType, damageIntensity, victim, collisionImpactVelocity, collisionPos, moveState, victimMoveState);
    return this;
}

// 0x4ACAD0
bool CEventPedCollisionWithPed::TakesPriorityOver(const CEvent& refEvent)
{
    if (CEventHandler::IsTemporaryEvent(refEvent))
        return true;
    return CEvent::TakesPriorityOver(refEvent);
}

// 0x4ACB10
bool CEventPedCollisionWithPed::AffectsPed(CPed* ped) {
    if (!ped->IsAlive() || ped->m_pAttachedTo || ped->bInVehicle) {
        return false;
    }

    // Como el original: el contador estático de la víctima se mira antes que IsThreatenedBy.
    if (!m_victim || m_victim->bInVehicle || m_victim->GetIntelligence()->m_AnotherStaticCounter > 30 || ped->GetIntelligence()->IsThreatenedBy(*m_victim)) {
        return false;
    }

    // Como el original: con un estado de movimiento menor que PEDMOVE_WALK (comparación de 16 bits con signo)
    // la víctima tiene que estar en un grupo y ser su líder; con los demás estados no se mira el grupo.
    if ((int32)(int16)(eMoveState)m_movestate < (int32)PEDMOVE_WALK) { // con signo: eMoveState es uint32 y la comparación sería sin signo
        const auto victimsGroup = CPedGroups::GetPedsGroup(m_victim);
        if (!victimsGroup || !victimsGroup->GetMembership().IsLeader(m_victim)) {
            return false;
        }
    }

    if (DotProduct(m_collisionImpactVelocity, ped->GetForward()) > -0.5f) { // If collision's velocity is not in [-120, 120] - So basically, if coming from behind
        return false;
    }

    if (ped->GetTaskManager().IsFirstFoundTaskMatching<TASK_COMPLEX_PARTNER_DEAL, TASK_COMPLEX_PARTNER_GREET>(m_victim->GetTaskManager())) {
        return false;
    }
                
    if (const auto task = ped->GetTaskManager().GetActiveTaskAs<CTaskComplexAvoidOtherPedWhileWandering>()) {
        // Como el original: lee el puntero crudo de +0xC (GetPedToAvoid copiaría la referencia y la registraría).
        if (*reinterpret_cast<CPed* const*>(reinterpret_cast<const uint8*>(task) + 0xC) == m_victim) {
            return false;
        }
    }

    if (const auto task = ped->GetTaskManager().Find<CTaskComplexKillPedOnFoot>()) {
        if (task->m_target == m_victim) {
            if (ped->GetTaskManager().Find<TASK_SIMPLE_FIGHT_CTRL>()) {
                return false;
            }
        }
    }

    return true;
}
