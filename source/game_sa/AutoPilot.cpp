/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAutoPilot
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

#include "AutoPilot.h"

void CAutoPilot::InjectHooks() {
    RH_ScopedClass(CAutoPilot);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(RemoveOnePathNode, 0x41B950);
}

// 0x6D5E20
CAutoPilot::CAutoPilot() : m_aPathFindNodesInfo() {
    _smthNext = 1;
    _smthCurr = 1;

    m_nCarCtrlFlags = 0;

    field_C = 0;
    m_nSpeedScaleFactor = 1000;
    m_nNextLane = 0;
    m_nCurrentLane = 0;
    m_nCarDrivingStyle = DRIVING_STYLE_STOP_FOR_CARS;
    m_nCarMission = eCarMission::MISSION_NONE;
    m_nTempAction = TEMPACT_NONE;
    SetCruiseSpeed(10);
    m_speed = 10.0F;
    m_nPathFindNodesCount = 0;
    m_TargetEntity = nullptr;

    m_nTimeToStartMission = CTimer::GetTimeInMS();
    m_nTimeSwitchedToRealPhysics = CTimer::GetTimeInMS();
    m_LastUpdateTimeMs = 0;

    m_nStraightLineDistance = 20;
    m_ucTempActionMode = 0;
    m_ucCarMissionModeCounter = 0;
    field_41 = 0;
    m_SpeedMult = 1.0f;
    m_ucHeliSpeedMult = 0;
    movementFlags.bIsStopped = false;
    movementFlags.bIsParked = false;
    field_4A = 0;
    m_ucCarFollowDist = 10;
    m_ucHeliTargetDist2 = 10;
    field_50 = CGeneral::GetRandomNumber() % 8 + 2;
    m_vehicleRecordingId = -1;
    m_bPlaneDogfightSomething = false;
    m_ObstructingEntity = nullptr;
    m_fMaxTrafficSpeed = 0.0F;
}

void CAutoPilot::ModifySpeed(float target) {
    plugin::CallMethod<0x41B980, CAutoPilot*, float>(this, target);
}

// 0x41B950: quita el primer nodo de la ruta (m_aPathFindNodesInfo, +0x68): decrementa el contador de 16 bits (+0x88) y, si queda positivo (con signo), desplaza
// cada nodo una posicion hacia el principio. Como el original, el indice es un int16 y el contador se vuelve a leer en cada vuelta: con un contador mayor que 8 la
// copia llega a escribir sobre el propio contador (+0x88), asi que la lectura es volatile y los accesos son por direccion (sin comprobar limites de std::array).
// Ninguno de sus 9 llamadores del EXE lee EAX/ECX/EDX a la salida.
void CAutoPilot::RemoveOnePathNode() {
    auto* const base = reinterpret_cast<uint8*>(this);
    volatile int16* const count = reinterpret_cast<volatile int16*>(base + 0x88);

    --m_nPathFindNodesCount;
    int16 i = 0;
    if (*count > 0) {
        do {
            const int32 slot = i;
            const uint32 next = *reinterpret_cast<const uint32*>(base + 0x6C + 4 * slot);
            ++i;
            *reinterpret_cast<uint32*>(base + 0x68 + 4 * slot) = next;
        } while (i < *count);
    }
}

void CAutoPilot::SetCarMission(eCarMission carMission, uint32 timeOffsetMs) {
    m_nCarMission = carMission;
    m_nTimeToStartMission = CTimer::GetTimeInMS() + timeOffsetMs;
}

// notsa
void CAutoPilot::SetTempAction(eAutoPilotTempAction action, uint32 durMs) {
    m_nTempAction = action;
    m_nTempActionTime = CTimer::GetTimeInMS() + durMs;
}
