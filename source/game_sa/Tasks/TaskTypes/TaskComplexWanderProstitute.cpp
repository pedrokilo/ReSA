/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWanderProstitute
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexWanderProstitute.h"
#include "TaskComplexProstituteSolicit.h"
#include "Events/EventAcquaintancePedRespect.h"
#include "PedIntelligence.h"
#include "PlayerPedData.h"

void CTaskComplexWanderProstitute::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderProstitute, 0x870148, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x672690);

    RH_ScopedVMTInstall(Clone, 0x673C80);
    RH_ScopedVMTInstall(GetWanderType, 0x6726C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x674920);
    RH_ScopedVMTInstall(ScanForStuff, 0x672700);
}

// 0x672690
CTaskComplexWanderProstitute::CTaskComplexWanderProstitute(eMoveState MoveState, uint8 Dir, bool bWanderSensibly) :
    CTaskComplexWanderStandard(MoveState, Dir, bWanderSensibly),
    m_nStartTimeInMs{ 0 }
{
}

// 0x672700
void CTaskComplexWanderProstitute::ScanForStuff(CPed* ped) {
    CTaskComplexWanderStandard::ScanForStuff(ped);

    if (CTimer::GetTimeInMS() <= m_nStartTimeInMs) {
        return;
    }
    m_nStartTimeInMs = CTimer::GetTimeInMS() + 2000;

    // Como el original: si hay algun policia entre los 16 peatones cercanos no hace nada
    auto* const nearby = ped->GetIntelligence()->GetPedEntities();
    for (size_t i = 0; i < 16; i++) {
        if (const auto* const other = static_cast<CPed*>(nearby[i]); other && other->m_nPedType == PED_TYPE_COP) {
            return;
        }
    }

    for (size_t i = 0; i < 16; i++) {
        auto* const other = static_cast<CPed*>(nearby[i]);
        if (!other || !other->GetPlayerData()) {
            continue;
        }
        auto* const veh = other->m_pVehicle;
        // Como el original: 0x672798 compara el campo +0xA8 del dato de jugador (m_pLastProstituteShagged)
        if (!veh || other->GetPlayerData()->m_pLastProstituteShagged == ped) {
            continue;
        }
        // Como el original: rand() & 0xFFFF * 2^-15 * 50000 (x87 a 24 bits) -> _ftol2; el valor del vehiculo se compara sin signo
        const auto price = static_cast<int32>(static_cast<float>(CGeneral::GetRandomNumber()) * 3.0517578125e-05f * 50000.0f);
        if (veh->m_pHandlingData->m_nMonetaryValue <= static_cast<uint32>(price)) {
            continue;
        }
        // Como el original: (z*50)^2 se suma como ((x*50)^2 + (y*50)^2) + (z*50)^2 y debe ser estrictamente menor que 4
        const float sz = veh->m_vecMoveSpeed.z * 50.0f;
        const float sy = veh->m_vecMoveSpeed.y * 50.0f;
        const float sx = veh->m_vecMoveSpeed.x * 50.0f;
        const float speedSq = sx * sx + sy * sy + sz * sz;
        if (!(speedSq < 4.0f)) {
            continue;
        }
        if (!CTaskComplexProstituteSolicit::IsTaskValid(ped, other)) {
            continue;
        }
        CEventAcquaintancePedRespect event{ other };
        event.m_TaskId = TASK_COMPLEX_PROSTITUTE_SOLICIT;
        ped->GetIntelligence()->GetEventGroup().Add(&event, false);
    }
}
