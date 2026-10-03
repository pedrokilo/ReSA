/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimplePlayerOnFire
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimplePlayerOnFire.h"
#include "TaskComplexOnFire.h"

void CTaskSimplePlayerOnFire::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimplePlayerOnFire, 0x86e334, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

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
