/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventVehicleDamageWeapon
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

#include "EventVehicleDamageWeapon.h"

namespace CEventVehicleDamageBody {
void __fastcall Destructor(CEventVehicleDamage* self, void* edx); // EventVehicleDamage.cpp: 0x4B1990
}

namespace {
// 0x61C310 (lote 143A, banco Fn143A; thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventVehicleDamageWeapon del EXE (`mov [ecx],0x86D640; jmp 0x4B1990`). Deja la vtable de
// CEventVehicleDamageWeapon del EXE (0x86D640) y sigue con el cuerpo de ~CEventVehicleDamage (0x4B1990, con hook). Funcion propia sin expansion en linea para que el banco sustituya el cuerpo base.
__declspec(noinline) void __fastcall WeaponDestructorBody(CEventVehicleDamageWeapon* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86D640;
    CEventVehicleDamageBody::Destructor(self, nullptr);
}

// 0x61C390 (lote 143A, banco Fn143A): destructor con borrado de CEventVehicleDamageWeapon del EXE (ranura 0 de su vtable 0x86D640; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x61C310; test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CEvent::operator delete (0x4B5630, sin hook: pool de eventos) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede
// nombrar la ranura del destructor).
CEventVehicleDamageWeapon* __fastcall WeaponScalarDeletingDestructor(CEventVehicleDamageWeapon* self, void* /*edx*/, uint8 flags) {
    WeaponDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventVehicleDamageWeapon::InjectHooks()
{
    RH_ScopedVirtualClass(CEventVehicleDamageWeapon, 0x86D640, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(WeaponDestructorBody, "Destructor", 0x61C310);                         // lote 143A (banco Fn143A)
    RH_ScopedNamedGlobalInstall(WeaponScalarDeletingDestructor, "ScalarDeletingDestructor", 0x61C390); // lote 143A (banco Fn143A)
    RH_ScopedVMTInstall(GetEventType, 0x61C320);                                                 // lote 143A: retorno de constante (41)

    RH_ScopedInstall(Constructor, 0x61C2E0);
    RH_ScopedVMTInstall(CloneEditable, 0x61C330);
}

// 0x61C2E0
CEventVehicleDamageWeapon::CEventVehicleDamageWeapon(CVehicle* vehicle, CEntity* attacker, eWeaponType weaponType) :
    CEventVehicleDamage(vehicle, attacker, weaponType)
{
    // nothing here
}

CEventVehicleDamageWeapon* CEventVehicleDamageWeapon::Constructor(CVehicle* vehicle, CEntity* attacker, eWeaponType weaponType)
{
    this->CEventVehicleDamageWeapon::CEventVehicleDamageWeapon(vehicle, attacker, weaponType);
    return this;
}

// 0x61C330
CEventEditableResponse* CEventVehicleDamageWeapon::CloneEditable() const noexcept
{
    return new CEventVehicleDamageWeapon(m_vehicle, m_attacker, m_weaponType);
}