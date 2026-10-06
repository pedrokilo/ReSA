/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWanderCriminal
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

#include "TaskComplexWanderCriminal.h"
#include "CarEnterExit.h"
#include "Events/EventVehicleToSteal.h"
#include "PedIntelligence.h"

void CTaskComplexWanderCriminal::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderCriminal, 0x85A23C, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x48E610);
    RH_ScopedInstall(Destructor, 0x48E720);
    RH_ScopedInstall(LookForCarsToSteal, 0x66B4F0);

    RH_ScopedVMTInstall(Clone, 0x48E650);
    RH_ScopedVMTInstall(GetWanderType, 0x48E6F0);
    RH_ScopedVMTInstall(ScanForStuff, 0x670350);
}

// 0x48E610
CTaskComplexWanderCriminal::CTaskComplexWanderCriminal(eMoveState MoveState, uint8 Dir, bool bWanderSensibly) : CTaskComplexWander(MoveState, Dir, bWanderSensibly) {
    // Como el original: el constructor pone a cero el temporizador (0x28..0x31) y m_nMinNextScanTime (0x34)
    m_nMinNextScanTime = 0;
}

// 0x670350
void CTaskComplexWanderCriminal::ScanForStuff(CPed* ped) {
    if (!m_TaskTimer.m_bStarted) {
        m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_TaskTimer.m_nInterval  = 50;
        m_TaskTimer.m_bStarted   = true;
        // Como el original: el reloj se lee antes de llamar a rand(); rand() & 0xFFFF * 2^-15 * -30000 (x87 a 24 bits) -> _ftol2
        const auto now = CTimer::GetTimeInMS();
        const auto offset = static_cast<float>(CGeneral::GetRandomNumber()) * 3.0517578125e-05f * -30000.0f;
        m_nMinNextScanTime = now - static_cast<uint32>(static_cast<int32>(offset));
    }

    if (m_TaskTimer.m_bStopped) {
        m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_TaskTimer.m_bStopped   = false;
    }

    if (CTimer::GetTimeInMS() >= m_TaskTimer.m_nStartTime + static_cast<uint32>(m_TaskTimer.m_nInterval)) {
        m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_TaskTimer.m_nInterval  = 50;
        m_TaskTimer.m_bStarted   = true;
        if (CTimer::GetTimeInMS() >= m_nMinNextScanTime) {
            LookForCarsToSteal(ped);
        }
    }
}

// 0x66B4F0
void CTaskComplexWanderCriminal::LookForCarsToSteal(CPed* ped) {
    // Como el original: recorre los 16 vehiculos del escaner y se queda con el robable mas cercano (distancia al cuadrado
    // sumada como (dz*dz + dy*dy) + dx*dx, solo gana si es estrictamente menor)
    CVehicle* closest{};
    float     closestDistSq = std::bit_cast<float>(0x7F7FFFFFu);
    for (size_t i = 0; i < 16; i++) {
        auto* const veh = static_cast<CVehicle*>(ped->GetIntelligence()->GetVehicleEntities()[i]);
        if (!veh) {
            continue;
        }
        if (!CCarEnterExit::IsVehicleStealable(veh, ped)) {
            continue;
        }
        const auto& vehPos = veh->GetPosition();
        const auto& pedPos = ped->GetPosition();
        const float dx = pedPos.x - vehPos.x;
        const float dy = pedPos.y - vehPos.y;
        const float dz = pedPos.z - vehPos.z;
        const float distSq = dz * dz + dy * dy + dx * dx;
        if (distSq < closestDistSq) {
            closestDistSq = distSq;
            closest       = veh;
        }
    }
    if (!closest) {
        return;
    }

    m_nMinNextScanTime = CTimer::GetTimeInMS() + 30'000;
    // Como el original: solo con rand() * (1/32767) < 0.2
    if (static_cast<float>(CGeneral::GetRandomNumber()) * RAND_MAX_FLOAT_RECIPROCAL < 0.2f) {
        CEventVehicleToSteal event{ closest };
        ped->GetIntelligence()->GetEventGroup().Add(&event, false);
    }
}
