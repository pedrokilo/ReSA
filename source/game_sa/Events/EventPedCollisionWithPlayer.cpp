/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPedCollisionWithPlayer
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventPedCollisionWithPlayer.h"


namespace CEventBody {
void __fastcall PedCollisionWithPedDestructorBody(CEventPedCollisionWithPed* self, void* edx); // EventPedCollisionWithPed.cpp: 0x4ACA70
}

namespace {
// 0x5FED80 (lote 160A, banco Fn160A; thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],86C970h; jmp 4ACA70`): cuerpo no virtual de ~CEventPedCollisionWithPlayer del EXE. Deja la vtable de la clase
// del EXE (0x86C970) y salta al cuerpo de ~CEventPedCollisionWithPed (0x4ACA70, con hook). El destructor generado por el compilador dejaria la vtable de la DLL. Funcion propia sin expansion en linea.
__declspec(noinline) void __fastcall PedCollisionWithPlayerDestructorBody(CEventPedCollisionWithPlayer* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86C970;
    CEventBody::PedCollisionWithPedDestructorBody(self, nullptr);
}

// 0x5FEE20 (lote 160A, banco Fn160A): destructor con borrado de CEventPedCollisionWithPlayer del EXE (ranura 0 de su vtable 0x86C970; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x5FED80; test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x5FED80, con hook),
// libera `this` con CEvent::operator delete (0x4B5630, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CEventPedCollisionWithPlayer* __fastcall PedCollisionWithPlayerScalarDeletingDestructor(CEventPedCollisionWithPlayer* self, void* /*edx*/, uint8 flags) {
    PedCollisionWithPlayerDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventPedCollisionWithPlayer::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPedCollisionWithPlayer, 0x86C970, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(PedCollisionWithPlayerDestructorBody, "DestructorBody", 0x5FED80);                       // lote 160A (banco Fn160A)
    RH_ScopedNamedGlobalInstall(PedCollisionWithPlayerScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5FEE20);   // lote 160A

    RH_ScopedInstall(Constructor, 0x5FED40);
    RH_ScopedVMTInstall(GetEventType, 0x5FED90); // lote 160A: ranura 1 de la vtable 0x86C970
    RH_ScopedVMTInstall(Clone, 0x5FEDA0);        // lote 160A: ranura 4
}

// 0x5FED90 (lote 160A, banco Fn160A; thiscall, `ret`, 6 bytes: `mov eax,3`): EVENT_PED_COLLISION_WITH_PLAYER (3); no lee el objeto.
eEventType CEventPedCollisionWithPlayer::GetEventType() const {
    return EVENT_PED_COLLISION_WITH_PLAYER;
}

// 0x5FEDA0 (lote 160A, banco Fn160A; thiscall, `ret`, 120 bytes con marco SEH que solo desenrolla): reserva el evento con CEvent::operator new (0x4B5620, con hook) para 0x34 bytes; sin memoria devuelve nulo.
// Con memoria llama al constructor de CEventPedCollisionWithPed (0x4AC990, con hook; aqui Constructor, sin expansion en linea) con (tipo de pieza +0xC, intensidad +0x10, victima +0x14, direccion de la velocidad
// +0x18, direccion de la posicion +0x24, estado +0x30, estado de la victima +0x32), IGNORA su valor devuelto, deja la vtable de la clase del EXE (0x86C970) en el objeto y lo devuelve. El marco SEH se omite.
CEventPedCollisionWithPlayer* CEventPedCollisionWithPlayer::Clone() const noexcept {
    auto* const object = static_cast<CEventPedCollisionWithPlayer*>(CEvent::operator new(sizeof(CEventPedCollisionWithPlayer)));
    if (!object) {
        return nullptr;
    }
    object->CEventPedCollisionWithPed::Constructor(m_pieceType, m_damageIntensity, m_victim, m_collisionImpactVelocity, m_collisionPos, m_movestate, m_victimMoveState);
    *reinterpret_cast<uint32*>(object) = 0x86C970;
    return object;
}

// 0x5FED40
CEventPedCollisionWithPlayer::CEventPedCollisionWithPlayer(int16 pieceType, float damageIntensity, CPed* victim, const CVector& collisionImpactVelocity, const CVector& collisionPos, eMoveState moveState, eMoveState victimMoveState) :
    CEventPedCollisionWithPed(pieceType, damageIntensity, victim, collisionImpactVelocity, collisionPos, moveState, victimMoveState)
{
}

// 0x5FED40
CEventPedCollisionWithPlayer* CEventPedCollisionWithPlayer::Constructor(int16 pieceType, float damageIntensity, CPed* victim, const CVector& collisionImpactVelocity, const CVector& collisionPos, eMoveState moveState, eMoveState victimMoveState)
{
    this->CEventPedCollisionWithPlayer::CEventPedCollisionWithPlayer(pieceType, damageIntensity, victim, collisionImpactVelocity, collisionPos, moveState, victimMoveState);
    return this;
}

