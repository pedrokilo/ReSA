/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimplePlayerOnFire
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimplePlayerOnFire.h"
#include "TaskComplexOnFire.h"

namespace {
// 0x6335B0 (lote 145A, banco Fn145A): cuerpo del destructor de CTaskSimplePlayerOnFire del EXE (con hook propio (Destructor); vtable 0x86E334). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama
// el destructor con borrado como el `call 0x6335B0` del EXE.
__declspec(noinline) void __fastcall CTaskSimplePlayerOnFireDestructorBody(CTaskSimplePlayerOnFire* self, void* /*edx*/) { self->CTaskSimplePlayerOnFire::~CTaskSimplePlayerOnFire(); }

// 0x639310 (lote 145A, banco Fn145A): destructor con borrado de CTaskSimplePlayerOnFire del EXE (ranura 0 de su vtable 0x86E334; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x6335B0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimplePlayerOnFire* __fastcall CTaskSimplePlayerOnFireScalarDeletingDestructor(CTaskSimplePlayerOnFire* self, void* /*edx*/, uint8 flags) {
    CTaskSimplePlayerOnFireDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote145ABody_CTaskSimplePlayerOnFire() { return reinterpret_cast<uintptr_t>(&CTaskSimplePlayerOnFireDestructorBody); } // para el banco Fn145A

void CTaskSimplePlayerOnFire::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimplePlayerOnFire, 0x86e334, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimplePlayerOnFireScalarDeletingDestructor, "ScalarDeletingDestructor", 0x639310); // lote 145A (banco Fn145A)

    RH_ScopedInstall(Constructor, 0x633560);
    RH_ScopedInstall(Destructor, 0x6335B0);

    RH_ScopedVMTInstall(Clone, 0x636E80);
    RH_ScopedVMTInstall(GetTaskType, 0x633590);
    RH_ScopedVMTInstall(MakeAbortable, 0x6335A0);
    RH_ScopedVMTInstall(ProcessPed, 0x6335C0);
}

// 0x6335C0
bool CTaskSimplePlayerOnFire::ProcessPed(CPed* ped) {
    m_timer.StartIfNotAlready(5000);

    if (!ped->m_pFire) {
        return true;
    }

    if (m_timer.IsOutOfTime() || ped->GetIntelligence()->GetTaskSwim()) {
        ped->m_pFire->Extinguish();
        return true;
    }

    // Como el original: lee CWorld::Players[0] directamente (FindPlayerPed/FindPlayerInfo en linea)
    if (CWorld::Players[0].m_pPed == ped && CWorld::Players[0].m_bFireProof) {
        return false;
    }

    CPedDamageResponse dmgResp;
    CTaskComplexOnFire::ComputeFireDamage(ped, dmgResp);
    if (!dmgResp.m_bHealthZero) {
        if (CLocalisation::PedsOnFire()) {
            ped->Say(CTX_GLOBAL_PAIN_ON_FIRE, 0, 0.1f); // Screaming
        }
        return false;
    }

    if (m_bAddedDamageEvent) {
        return true;
    }

    // Como el original: tambien grita la primera vez que el dano es mortal
    if (CLocalisation::PedsOnFire()) {
        ped->Say(CTX_GLOBAL_PAIN_ON_FIRE, 0, 0.1f);
    }

    CEventDamage dmgEvent{ nullptr, CTimer::GetTimeInMS(), WEAPON_FLAMETHROWER, PED_PIECE_TORSO, 0u, false, !!ped->bInVehicle };
    dmgEvent.m_damageResponse = dmgResp; // Como el original: copia la respuesta calculada al evento
    ped->GetEventGroup().Add(&dmgEvent);
    m_bAddedDamageEvent = true; // Como el original: solo se anade una vez

    return false;
}
