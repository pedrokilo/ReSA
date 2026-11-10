/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventVehicleDamageCollision
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

#include "EventVehicleDamageCollision.h"

namespace CEventVehicleDamageBody {
void __fastcall Destructor(CEventVehicleDamage* self, void* edx); // EventVehicleDamage.cpp: 0x4B1990
}

namespace {
// 0x6A0650 (lote 143A, banco Fn143A; thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventVehicleDamageCollision del EXE (`mov [ecx],0x8710D0; jmp 0x4B1990`). Deja la vtable de
// CEventVehicleDamageCollision del EXE (0x8710D0) y sigue con el cuerpo de ~CEventVehicleDamage (0x4B1990, con hook). Funcion propia sin expansion en linea para que el banco sustituya el cuerpo base.
__declspec(noinline) void __fastcall CollisionDestructorBody(CEventVehicleDamageCollision* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x8710D0;
    CEventVehicleDamageBody::Destructor(self, nullptr);
}

// 0x6A06D0 (lote 143A, banco Fn143A): destructor con borrado de CEventVehicleDamageCollision del EXE (ranura 0 de su vtable 0x8710D0; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x6A0650; test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CEvent::operator delete (0x4B5630, sin hook: pool de eventos) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede
// nombrar la ranura del destructor).
CEventVehicleDamageCollision* __fastcall CollisionScalarDeletingDestructor(CEventVehicleDamageCollision* self, void* /*edx*/, uint8 flags) {
    CollisionDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventVehicleDamageCollision::InjectHooks()
{
    RH_ScopedVirtualClass(CEventVehicleDamageCollision, 0x8710D0, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(CollisionDestructorBody, "Destructor", 0x6A0650);                         // lote 143A (banco Fn143A)
    RH_ScopedNamedGlobalInstall(CollisionScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6A06D0); // lote 143A (banco Fn143A)
    RH_ScopedVMTInstall(GetEventType, 0x6A0660);                                                 // lote 143A: retorno de constante (73)

    RH_ScopedInstall(Constructor, 0x6A0620);
    RH_ScopedVMTInstall(CloneEditable, 0x6A0670);
}

// 0x6A0620
CEventVehicleDamageCollision::CEventVehicleDamageCollision(CVehicle* vehicle, CEntity* attacker, eWeaponType weaponType) :
    CEventVehicleDamage(vehicle, attacker, weaponType)
{
    // empty
}

CEventVehicleDamageCollision* CEventVehicleDamageCollision::Constructor(CVehicle* vehicle, CEntity* attacker, eWeaponType weaponType)
{
    this->CEventVehicleDamageCollision::CEventVehicleDamageCollision(vehicle, attacker, weaponType);
    return this;
}

// 0x6A0670
CEventEditableResponse* CEventVehicleDamageCollision::CloneEditable() const noexcept
{
    return new CEventVehicleDamageCollision(m_vehicle, m_attacker, m_weaponType);
}