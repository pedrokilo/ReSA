/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventScanner
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CVehiclePotentialCollisionScanner
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

#include "EventScanner.h"

#include "Events/EventPotentialWalkIntoVehicle.h"
#include "Tasks/TaskTypes/TaskSimpleGoTo.h"
#include "PedGeometryAnalyser.h"
#include "Events/EventInAir.h"
#include "Events/EventOnFire.h"
#include "Events/EventAreaCodes.h"
#include "Events/EventEscalator.h"
#include "Events/EventPedEnteredMyVehicle.h"
#include "Events/EventDamage.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFoot.h"
#include "Fire.h"
#include "Weapon.h"

void CEventScanner::InjectHooks() {
    RH_ScopedClass(CEventScanner);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ScanForEvents, 0x607E30);
}

// 0x605300
CEventScanner::CEventScanner() {
    // Como el original: los temporizadores quedan a cero (inicializadores de miembro); el escáner de atractores
    // empieza con field_0 = 1, efecto y field_14 nulos y se limpia; el de conocidos permite peatones de script
    m_attractorScanner.field_0       = 1;
    m_attractorScanner.m_pEffectInUse = nullptr;
    m_attractorScanner.field_14      = 0;
    m_attractorScanner.Clear();
    m_pedAcquaintanceScanner.m_bScanAllowedScriptPed    = true;
    m_pedAcquaintanceScanner.m_bScanAllowedInVehicle    = false;
    m_pedAcquaintanceScanner.m_bScanAllowedScriptedTask = false;
    // Como el original: tiempo - ftol((rand() & 0xFFFF) * (1/32768) * -3000), con productos redondeados a float
    const uint32 time = CTimer::GetTimeInMS();
    m_nNextScanTime = time - (uint32)(int32)((float)(CGeneral::GetRandomNumber() & 0xFFFF) * 3.0517578125e-05f * -3000.0f);
}

void CEventScanner::Clear() {
    m_attractorScanner.Clear();
}

// 0x607E30
void CEventScanner::ScanForEvents(CPed& ped) {
    // Como el original: no escanea hasta que la hora (sin signo) supera m_nNextScanTime, y no lo actualiza
    if (CTimer::GetTimeInMS() <= m_nNextScanTime) {
        return;
    }
    CPedIntelligence* intel = ped.GetIntelligence();

    // Escaneres: los reconstruidos se llaman en C++; el resto por direccion del EXE
    m_vehiclePotentialCollisionScanner.ScanForVehiclePotentialCollisionEvents(ped, intel->GetVehicleEntities(), 16);
    // 0x606580: funcion sin `this` (el EXE le deja en ECX la direccion del argumento); recibe el peaton y el peaton mas cercano del escaner de peatones
    reinterpret_cast<void(__stdcall*)(CPed*, CEntity*)>(0x606580)(&ped, intel->m_pedScanner.GetClosestPedInRange());
    plugin::CallMethod<0x606890, CObjectPotentialCollisionScanner*, CPed*>(&m_objectPotentialCollisionScanner, &ped);                              // ScanForObjectPotentialCollisionEvents
    plugin::CallMethod<0x607D80, CPedAcquaintanceScanner*, CPed*, CEntity**, int32>(&m_pedAcquaintanceScanner, &ped, intel->GetPedEntities(), 16);  // ScanForPedAcquaintances
    plugin::CallMethod<0x6060A0, CAttractorScanner*, CPed*>(&m_attractorScanner, &ped);                                                              // ScanForAttractors
    plugin::CallMethod<0x603E70, CNearbyFireScanner*, CPed*>(&m_nearbyFireScanner, &ped);                                                            // ScanForNearbyFires
    intel->m_mentalState.Process(&ped);

    // En el aire: evento CEventInAir si no esta de pie y (la bandera dice que esta en el aire o la geometria lo dice); si esta de pie con la bandera puesta
    // y el evento actual no es CEventInAir se baja la bandera
    if (!ped.bIsStanding && (ped.bIsInTheAir || CPedGeometryAnalyser::IsInAir(ped))) {
        CEventInAir event;
        intel->GetEventGroup().Add(&event, false);
    } else if (ped.bIsInTheAir) {
        auto& history = intel->m_eventHandler.GetHistory();
        if (!history.GetCurrentEvent() || history.GetCurrentEvent()->GetEventType() != EVENT_IN_AIR) {
            ped.bIsInTheAir = false;
        }
    }

    // En llamas
    if (ped.m_pFire) {
        CEventOnFire event;
        bool addEvent = true;
        if (!ped.IsPlayer()) {
            if (CTask* task = intel->GetTaskManager().GetSimplestActiveTask()) {
                // Como el original: si la tarea no se puede abortar, el dano de fuego se genera directamente en lugar de anadir el evento
                if (!task->MakeAbortable(&ped, ABORT_PRIORITY_URGENT, &event)) {
                    addEvent = false;
                    CWeapon::GenerateDamageEvent(&ped, ped.m_pFire->GetEntityStartedFire(), static_cast<eWeaponType>(0x25), 5, static_cast<ePedPieceTypes>(3), 0);
                }
            }
        }
        if (addEvent) {
            intel->GetEventGroup().Add(&event, false);
        }
    }

    plugin::CallMethod<0x603BF0, CSexyPedScanner*, CPed*, CEntity**, int32>(&m_sexyPedScanner, &ped, intel->GetPedEntities(), 16);                 // ScanForSexyPeds

    // Objetivo de una tarea de matar (4, 3, 1, 2 en ese orden) en otra area: CEventAreaCodes
    {
        CPedIntelligence* intel2 = ped.GetIntelligence();
        CTaskManager& taskMgr = intel2->m_TaskMgr;
        CTask* task = taskMgr.FindTaskByType(static_cast<ePrimaryTasks>(4), static_cast<eTaskType>(1000));
        if (!task) { task = taskMgr.FindTaskByType(static_cast<ePrimaryTasks>(3), static_cast<eTaskType>(1000)); }
        if (!task) { task = taskMgr.FindTaskByType(static_cast<ePrimaryTasks>(1), static_cast<eTaskType>(1000)); }
        if (!task) { task = taskMgr.FindTaskByType(static_cast<ePrimaryTasks>(2), static_cast<eTaskType>(1000)); }
        if (task) {
            if (CPed* target = static_cast<CTaskComplexKillPedOnFoot*>(task)->m_target) {
                CEntity* targetContact = target->m_pContactEntity;
                if (targetContact && ped.m_pContactEntity) {
                    if (targetContact->GetAreaCode() != ped.m_pContactEntity->GetAreaCode()) {
                        CEventAreaCodes event(target);
                        ped.GetIntelligence()->GetEventGroup().Add(&event, false);
                    }
                }
            }
        }
    }

    // Sobre una escalera mecanica (el indice de modelo del contacto, con signo, igual a alguno de los dos modelos): CEventEscalator
    if (ped.m_pContactEntity) {
        const int32 contactModel = static_cast<int16>(ped.m_pContactEntity->m_nModelIndex);
        if (static_cast<int32>(StaticRef<uint16>(0x8CD674)) == contactModel || static_cast<int32>(StaticRef<uint16>(0x8CD678)) == contactModel) {
            CEventEscalator event;
            ped.GetIntelligence()->GetEventGroup().Add(&event, false);
        }
    }

    // En un barco (sin ser el jugador) cuyo vehiculo es el que pisa el jugador: CEventPedEnteredMyVehicle (con tarea 0x2C2)
    if (ped.bInVehicle && ped.m_pVehicle && ped.m_pVehicle->m_nVehicleType == VEHICLE_TYPE_BOAT && !ped.IsPlayer()) {
        CVehicle* vehicle = ped.m_pVehicle;
        if (FindPlayerPed(-1)->m_pContactEntity == vehicle) {
            CEventPedEnteredMyVehicle event(FindPlayerPed(-1), vehicle, static_cast<eTargetDoor>(10));
            event.m_TaskId = static_cast<eTaskType>(0x2C2);
            ped.GetIntelligence()->GetEventGroup().Add(&event, false);
        }
    }

    // Peaton vivo con la salud en 0 o menos: dano de ahogamiento cada 2 segundos si no tiene ya un evento de dano
    if (ped.m_fHealth <= 0.0f && ped.IsAlive() && CTimer::GetTimeInMS() > m_sDeadPedWalkingTimer) {
        CEventDamage event(nullptr, 0, static_cast<eWeaponType>(0x37), static_cast<ePedPieceTypes>(3), 0, false, false);
        if (!ped.GetIntelligence()->GetEventGroup().HasEventOfType(&event)) {
            CWeapon::GenerateDamageEvent(&ped, nullptr, static_cast<eWeaponType>(0x36), 10, static_cast<ePedPieceTypes>(3), 0);
            m_sDeadPedWalkingTimer = CTimer::GetTimeInMS() + 2000;
        }
    }
}

// 0x6053D0
void CEventScanner::ScanForEventsNow(const CPed& ped, bool bDontScan) {
    if (bDontScan)
        return;

    auto scanner = &m_vehiclePotentialCollisionScanner;
    if (scanner->m_timer.m_bStarted) { // todo: inlined?
        scanner->m_timer.m_nStartTime = CTimer::GetTimeInMS();
        scanner->m_timer.m_nInterval = -1;
        scanner->m_timer.m_bStarted = true;
    }
    scanner->ScanForVehiclePotentialCollisionEvents(ped, ped.GetIntelligence()->GetVehicleEntities(), 16);
}

void CVehiclePotentialCollisionScanner::InjectHooks() {
    RH_ScopedClass(CVehiclePotentialCollisionScanner);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ScanForVehiclePotentialCollisionEvents, 0x603720);
}

// 0x603720
// Como el original, solo se usa `ped`: `entities` y `count` se empujan en la pila pero la funcion no los lee (`ret 0Ch`)
void CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents(const CPed& ped, CEntity** entities, int32 count) {
    // Como el original: CTaskTimer en linea. Si no estaba arrancado se arranca con 500 ms (y se sigue); si estaba detenido se reinicia el inicio;
    // si ya vencio (unsigned: hora >= inicio + intervalo) se rearranca con 500 ms y se escanea. Los booleanos se leen como bytes (!= 0).
    uint32 now = CTimer::GetTimeInMS();
    if (!m_timer.m_bStarted) {
        m_timer.m_nStartTime = now;
        m_timer.m_bStarted   = true;
        m_timer.m_nInterval  = 500;
    }
    if (m_timer.m_bStopped) {
        m_timer.m_nStartTime = CTimer::GetTimeInMS();
        m_timer.m_bStopped   = false;
    }
    now = CTimer::GetTimeInMS();
    if (!(now >= m_timer.m_nStartTime + static_cast<uint32>(m_timer.m_nInterval))) {
        return;
    }
    m_timer.m_nStartTime = now;
    m_timer.m_nInterval  = 500;
    m_timer.m_bStarted   = true;

    CPedIntelligence* intel = ped.GetIntelligence();
    CTask* task = intel->GetTaskManager().GetSimplestActiveTask();
    if (!task || !CTask::IsGoToTask(task)) {
        return;
    }
    CVehicle* vehicle = intel->m_vehicleScanner.GetClosestVehicleInRange();
    if (!vehicle) {
        return;
    }

    // Como el original: diferencia peaton - vehiculo (posiciones de la matriz o del placement)
    const CVector& vehPos = vehicle->GetPosition();
    const CVector& pedPos = ped.GetPosition();
    const CVector  diff{ pedPos.x - vehPos.x, pedPos.y - vehPos.y, pedPos.z - vehPos.z };

    // Las dos esquinas de la caja delimitadora del vehiculo en el mundo (la matriz del vehiculo se usa sin comprobar nulo, como el EXE)
    CColModel* colModel = vehicle->GetColModel();
    const CMatrix& matrix = *vehicle->m_matrix;
    CVector corner1, corner2;
    corner1.FromMultiply(matrix, colModel->m_boundBox.m_vecMin);
    corner2.FromMultiply(matrix, colModel->m_boundBox.m_vecMax);

    // Como el original (x87 a 24 bits: cada operacion se redondea a float; las sumas se agrupan como en el EXE):
    // d2 = -(U.z*c2.z + U.y*c2.y + U.x*c2.x) con U el eje "arriba" (+0x20) y d1 = -(c1.y*h.y + c1.z*h.z + h.x*c1.x) con h = U * -1
    const CVector up = matrix.GetUp();
    const float t2a = up.z * corner2.z;
    const float t2b = up.y * corner2.y;
    const float t2c = t2a + t2b;
    const float t2d = up.x * corner2.x;
    const float d2  = -(t2c + t2d);
    const float hz  = up.z * -1.0f;
    const float hy  = up.y * -1.0f;
    const float hx  = up.x * -1.0f;
    const float t1a = corner1.y * hy;
    const float t1b = corner1.z * hz;
    const float t1c = t1a + t1b;
    const float t1d = hx * corner1.x;
    const float d1  = -(t1c + t1d);

    // Como el original: la posicion del peaton se vuelve a leer
    const CVector& p = ped.GetPosition();
    const float s1a = up.z * p.z;
    const float s1b = up.y * p.y;
    const float s1c = s1a + s1b;
    const float s1d = up.x * p.x;
    const float side1 = (s1c + s1d) + d2;
    bool inside = false;
    if (side1 < 0.5f) {   // fcomp + test ah,5 + jp: NaN y >= 0.5 no cumplen
        const float s2a = hy * p.y;
        const float s2b = hz * p.z;
        const float s2c = s2a + s2b;
        const float s2d = hx * p.x;
        const float side2 = (s2c + s2d) + d1;
        inside = side2 < 0.5f;
    }

    // Como el original: el limite es 5.0 (10.0 = 5.0 + 5.0 si el vehiculo es un tren)
    float limit = (vehicle->m_nVehicleSubType == VEHICLE_TYPE_TRAIN) ? 5.0f + 5.0f : 5.0f;
    if (!inside) {
        return;
    }
    // Como el original: la comprobacion de distancia (|diff|^2 < limite^2) se salta si el peaton acaba de dejar un coche
    if (!ped.bHasJustLeftCar) {
        const float sq = (diff.x * diff.x + diff.y * diff.y) + diff.z * diff.z;   // CVector::SquaredMagnitude (0x406DA0)
        const float limitSq = limit * limit;
        if (!(limitSq > sq)) {
            return;
        }
    }

    // Como el original: la misma variable que guardaba el limite recibe la distancia (puesta a 0 antes de la llamada)
    float distance = 0.0f;
    const CVector& targetPoint = static_cast<CTaskSimpleGoTo*>(task)->m_vecTargetPoint;
    if (CPedGeometryAnalyser::GetIsLineOfSightClear(ped, targetPoint, *vehicle, distance)) {
        return;
    }
    if (!(distance > 0.5f)) {
        return;
    }
    const eMoveState moveState = intel->GetMoveStateFromGoToTask();
    CEventPotentialWalkIntoVehicle event(vehicle, moveState);
    intel->GetEventGroup().Add(&event, false);
}
