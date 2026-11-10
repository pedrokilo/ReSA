/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventScanner
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CNearbyFireScanner
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CObjectPotentialCollisionScanner
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CPedAcquaintanceScanner
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CSexyPedScanner
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
#include "FireManager.h"
#include "Weapon.h"
#include "InterestingEvents.h"
#include "PedStats.h"
#include "World.h"
#include "Events/EventFireNearby.h"
#include "Events/EventPotentialWalkIntoFire.h"
#include "Events/EventSexyPed.h"
#include "Events/EventPotentialWalkIntoObject.h"
#include "EntityScanner.h"
#include "ObjectScanner.h"
#include "Entity/Object/Object.h"
#include "Models/ModelInfo.h"
#include "DecisionMakers/DecisionMakerTypes.h"
#include "GameLogic.h"
#include "PedGroup.h"
#include "PedGroups.h"
#include "PedIntelligence.h"
#include "PedType.h"
#include "X87Math.h"

void CObjectPotentialCollisionScanner::InjectHooks() {
    RH_ScopedClass(CObjectPotentialCollisionScanner);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ScanForObjectPotentialCollisionEvents, 0x606890);
}

void CPedAcquaintanceScanner::InjectHooks() {
    RH_ScopedClass(CPedAcquaintanceScanner);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ScanForPedAcquaintances, 0x607D80);
    RH_ScopedInstall(ScanEntities, 0x607A90); // caso 607A90 (banco Fn607A)
    RH_ScopedInstall(CanJoinRiotAgainst, 0x603AF0); // caso COLAB-607A (banco Fn607B)
    RH_ScopedInstall(FindAcquaintance, 0x607560); // caso COLAB-607A (banco Fn607B)
}

void CSexyPedScanner::InjectHooks() {
    RH_ScopedClass(CSexyPedScanner);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ScanForSexyPeds, 0x603BF0);
}

void CNearbyFireScanner::InjectHooks() {
    RH_ScopedClass(CNearbyFireScanner);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ScanForNearbyFires, 0x603E70);
}

void CEventScanner::InjectHooks() {
    RH_ScopedClass(CEventScanner);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ScanForEvents, 0x607E30);
    RH_ScopedInstall(ScanForEventsNow, 0x6053D0);
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
    m_objectPotentialCollisionScanner.ScanForObjectPotentialCollisionEvents(ped);
    m_pedAcquaintanceScanner.ScanForPedAcquaintances(ped, intel->GetPedEntities(), 16);
    m_attractorScanner.ScanForAttractors(ped);
    m_nearbyFireScanner.ScanForNearbyFires(ped);
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

    m_sexyPedScanner.ScanForSexyPeds(ped, intel->GetPedEntities(), 16);

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
// Como el original: `bDontScan` se prueba como dword (`test esi,esi`), no como bool (el llamador empuja un dword)
void CEventScanner::ScanForEventsNow(const CPed& ped, uint32 bDontScan) {
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

// 0x606890
void CObjectPotentialCollisionScanner::ScanForObjectPotentialCollisionEvents(CPed& ped) {
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

    // Como el original: estado de movimiento de la tarea de ir a un punto (1 = PEDMOVE_STILL si la tarea mas simple no lo es). El jugador y un peaton quieto no escanean
    CPedIntelligence* intel = ped.GetIntelligence();
    CTask* simplest = intel->GetTaskManager().GetSimplestActiveTask();
    eMoveState moveState = PEDMOVE_STILL;
    if (simplest && CTask::IsGoToTask(simplest)) {
        moveState = static_cast<CTaskSimpleGoTo*>(simplest)->m_moveState;
    }
    if (ped.IsPlayer() || moveState == PEDMOVE_STILL) {
        return;
    }
    CTask* const goTo = intel->GetTaskManager().GetSimplestActiveTask();
    if (!goTo || !CTask::IsGoToTask(goTo)) {
        return;
    }
    const CVector& targetPoint = static_cast<CTaskSimpleGoTo*>(goTo)->m_vecTargetPoint;

    // Escaner de objetos temporal (0x606010 / 0x5FFF30 / 0x5FF9D0): el destructor lo limpia al salir
    CObjectScanner scanner;
    scanner.ScanForObjectsInRange(ped);
    CObject* const object = scanner.GetClosestObjectInRange();
    if (!object) {
        return;
    }
    // Como el original: se descartan los recogibles (bit 0) y los rotos (bit 10) de las banderas +0x140, y los que no usan colision (bit 0 de +0x1C)
    if (object->objectFlags.bIsBroken || object->objectFlags.bIsPickup || !object->GetUsesCollision()) {
        return;
    }

    // Como el original (x87 a 24 bits: cada operacion se redondea a float): diferencia peaton - objeto (0x40FE00) y su cuadrado (0x406DA0): (x*x + y*y) + z*z; el limite es 7.5 (0x86C8FC) al cuadrado
    {
        const CVector& objPos = object->GetPosition();
        const CVector& pedPos = ped.GetPosition();
        const float    dx     = pedPos.x - objPos.x;
        const float    dy     = pedPos.y - objPos.y;
        const float    dz     = pedPos.z - objPos.z;
        const float    xx     = dx * dx;
        const float    yy     = dy * dy;
        const float    zz     = dz * dz;
        const float    sq     = (xx + yy) + zz;
        if (!(7.5f * 7.5f > sq)) {
            return;
        }
    }

    // Como el original: la altura del centro del objeto (0x534250) y el radio de la esfera delimitadora de la colision del modelo (+0x24; indice de modelo con signo, sin comprobar rango)
    const float centreZ = object->GetBoundCentre().z;
    const CBaseModelInfo* const modelInfo = CModelInfo::GetModelInfo(static_cast<int16>(object->m_nModelIndex));
    const float radius  = modelInfo->GetColModel()->m_boundSphere.m_fRadius;
    const float pedZ    = ped.GetPosition().z;
    // El peaton (z - 1.0 .. z + 1.0, 0x858624) debe solapar la altura del objeto (centro -+ radio): se descarta si z - 1.0 > radio + centro (sin guardar a float) o
    // si (z + 1.0) < (centro - radio) (guardados como float); NaN no descarta
    const float below  = pedZ - 1.0f;
    const float above  = pedZ + 1.0f;
    const float bottom = centreZ - radius;
    const float top    = radius + centreZ;
    if (below > top) {
        return;
    }
    if (above < bottom) {
        return;
    }

    // Como el original: la linea de vision del peaton al punto de destino; si esta libre, o la distancia no pasa de 0.5 (0x86C900), no hay evento
    float distance = 0.0f;
    if (CPedGeometryAnalyser::GetIsLineOfSightClear(ped, targetPoint, *object, distance)) {
        return;
    }
    if (!(distance > 0.5f)) {
        return;
    }
    // Como el original: segunda comprobacion desde 0.75 (constante inmediata 0x3F400000) por debajo de la posicion del peaton (se vuelve a leer; la resta con 0 es la de 0x40FE60)
    const CVector& pos = ped.GetPosition();
    const CVector  low{ pos.x - 0.0f, pos.y - 0.0f, pos.z - 0.75f };
    if (CPedGeometryAnalyser::GetIsLineOfSightClear(low, targetPoint, *object)) {
        return;
    }
    CEventPotentialWalkIntoObject event(object, moveState);
    ped.GetIntelligence()->GetEventGroup().Add(&event, false);
}

// 0x607D80
void CPedAcquaintanceScanner::ScanForPedAcquaintances(CPed& ped, CEntity** entities, int32 count) {
    // Como el original: CTaskTimer en linea con el intervalo de la tabla del EXE (0x8D2358, 500). Si no estaba arrancado se arranca (y se sigue);
    // si estaba detenido se reinicia el inicio; si ya vencio (unsigned: hora >= inicio + intervalo) se rearranca y se escanea. Los booleanos se leen como bytes (!= 0).
    const auto& interval = StaticRef<int32>(0x8D2358);
    uint32 now = CTimer::GetTimeInMS();
    if (!m_timer.m_bStarted) {
        m_timer.m_nInterval  = interval;
        m_timer.m_nStartTime = now;
        m_timer.m_bStarted   = true;
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
    m_timer.m_nInterval  = interval;
    m_timer.m_bStarted   = true;

    // 0x603A30: decide si este escaner puede actuar sobre el peaton (aun por direccion); 0x607A90 (ScanEntities) hace el escaneo.
    // El EXE le pasa dos punteros a variables locales que valen 0 y -1 (el escaneo las rellena; esta funcion no las lee).
    if (!plugin::CallMethodAndReturn<bool, 0x603A30>(this, &ped)) {
        return;
    }
    int32 outA = 0;
    int32 outB = -1;
    ScanEntities(ped, -1, entities, count, &outA, &outB);
}

// 0x607A90: escaneo de entidades (caso 607A90, banco Fn607A). Thiscall con seis argumentos de pila (`ret 18h`); el unico llamador del EXE es ScanForPedAcquaintances. Devuelve el numero de candidatos que
// pasan el filtro (0 si ninguno). Como el original:
//  - *outA = 0 al empezar; para cada entidad no nula y viva (0x5E0170) se calcula delta = posicion(candidato) - posicion(ped) (posicion de la matriz +0x30 o, sin matriz, la de +4) en la x87 (AcquaintanceDeltaCpp,
//    guardada como float), se normaliza (0x59C910) y se compara (z*fz + y*fy) + x*fx con el umbral de 0xC0B034, con el vector adelante de la matriz del ped (+0x10..+0x18) leido DESPUES de normalizar
//    (AcquaintanceInViewCpp, tambien x87). Si NO es mayor, solo sigue si el ped o el candidato tienen el bit 0x100 de +0x46C (bInVehicle).
//  - Si la prioridad es -1 y el tipo del candidato es 6 (policia), pasa directamente. Si no, para los indices 4..0 (todos con prioridad -1, solo el indicado en otro caso) pasa si
//    (acquaintances del ped[indice] & GetPedFlag(tipo)) != 0 (0x608970, 0x608830). Si ninguno acierta, solo pasa con disturbios activos (0x441C10) y CanJoinRiotAgainst (0x603AF0).
//  - Un candidato de tipo 6 entra en la tabla local (16 entradas, como la del EXE: mas desbordan los registros guardados) sin mas condiciones; el resto, con disturbios y 0x603AF0 o si el decision maker del ped
//    tiene respuesta a alguno de los eventos 36, 37, 39 y 40 (0x4684F0 + 0x6042B0, count = 5: el quinto elemento del EXE es un resto de pila que no se lee; aqui se repite el cuarto).
//  - Por cada candidato de la tabla, mientras el acumulador (-1 al empezar) no valga 4: CanPedTargetPed (0x5F1C40, con la bandera de que ni el ped ni el candidato estan en vehiculo) y, si acierta, el
//    acumulador pasa a ser el valor de 0x607560 (FindAcquaintance, que escribe *outA y *outB y fija el temporizador del escaner).
int32 CPedAcquaintanceScanner::ScanEntities(CPed& ped, int32 priority, CEntity** entities, int32 count, int32* outA, int32* outB) {
    *outA = 0;
    CPed* candidates[16];
    int32 numCandidates = 0;
    for (int32 i = 0; i < count; ++i) {
        CPed* const other = static_cast<CPed*>(entities[i]);
        if (!other || !other->IsAlive()) {
            continue;
        }

        // Posiciones: la del ped (matriz o +4) y la del candidato; delta guardada como float y normalizada
        CVector delta;
        AcquaintanceDeltaCpp(&other->GetPosition().x, &ped.GetPosition().x, &delta.x);
        delta.Normalise();

        // Producto escalar con el vector adelante del ped (se lee despues de normalizar) contra el umbral (x87); si no pasa, solo siguen los que van en vehiculo
        if (!AcquaintanceInViewCpp(&delta.x, &ped.GetForward().x, ms_fThresholdDotProduct)) {
            if (!ped.bInVehicle && !other->bInVehicle) {
                continue;
            }
        }

        // Relacion: policia con prioridad -1, o algun indice 4..0 cuyas acquaintances incluyan el tipo del candidato
        bool related = priority == -1 && other->m_nPedType == PED_TYPE_COP;
        if (!related) {
            for (int32 id = 4; id >= 0 && !related; --id) {
                if (priority == -1 || priority == id) {
                    const uint32 acquaintances = ped.m_acquaintance.GetAcquaintances(id);
                    const uint32 pedFlag       = CPedType::GetPedFlag(other->m_nPedType);
                    related = (acquaintances & pedFlag) != 0;
                }
            }
            if (!related) {
                if (!CGameLogic::LaRiotsActiveHere() || !CanJoinRiotAgainst(&ped, other)) {
                    continue;
                }
            }
        }

        // Tabla de candidatos: los policias, o disturbios + 0x603AF0, o una respuesta del decision maker a los eventos 36, 37, 39 y 40
        if (other->m_nPedType != PED_TYPE_COP) {
            bool accepted = CGameLogic::LaRiotsActiveHere() && CanJoinRiotAgainst(&ped, other);
            if (!accepted) {
                const int32 events[5]{ 0x24, 0x25, 0x27, 0x28, 0x28 };
                accepted = CDecisionMakerTypes::GetInstance()->HasResponseToAnyEvent(&ped, events, 5);
            }
            if (!accepted) {
                continue;
            }
        }
        candidates[numCandidates++] = other;
    }

    int32 accumulator = -1;
    for (int32 i = 0; i < numCandidates; ++i) {
        if (accumulator == 4) {
            continue;
        }
        CPed* const other = candidates[i];
        const bool canTarget = !ped.bInVehicle && !other->bInVehicle;
        if (CPedGeometryAnalyser::CanPedTargetPed(ped, *other, canTarget)) {
            accumulator = FindAcquaintance(ped, priority, accumulator, other, outA, outB);
        }
    }
    return numCandidates;
}

// 0x607560 (hook S, banco Fn607B; la llama ScanEntities). Thiscall con seis argumentos de pila (`ret 18h`): para los indices 4..0 (parando con -1 si llega al ya usado, `current`) comprueba si el ped tiene al
// candidato como acquaintance de ese indice (0x608970/0x608830, o disturbios y CanJoinRiotAgainst; con prioridad -1 el indice 2 y un policia pasan directo) y la visibilidad con luces (0x605550, float): sigue si
// es mayor que 0 o, solo con el indice 4, si es distinta de 0 (negativa o NaN). Entonces escribe *outA = candidato, *outB = indice; con una prioridad distinta de -1 devuelve la prioridad; con -1 pide el evento a
// 0x606BA0 (por direccion: aun sin reconstruir) y fija el temporizador del escaner (inicio = hora, intervalo = 200 si el indice es 4 y la visibilidad es negativa, 3000 si no, arrancado), devolviendo
// el indice si el evento se creo. Devuelve -1 si ningun indice sirve. Validada por el banco Fn607B.
int32 CPedAcquaintanceScanner::FindAcquaintance(CPed& ped, int32 priority, int32 current, CPed* candidate, int32* outA, int32* outB) {
    const float visibility = ped.GetIntelligence()->CanSeeEntityWithLights(candidate, 0);
    for (int32 id = 4; id >= 0; --id) {
        if (id == current) {
            return -1;
        }
        if (priority != -1 && priority != id) {
            continue;
        }
        if (!(priority == -1 && id == 2 && candidate->m_nPedType == PED_TYPE_COP)) {
            const uint32 acquaintances = ped.m_acquaintance.GetAcquaintances(id);
            const uint32 pedFlag       = CPedType::GetPedFlag(candidate->m_nPedType);
            const bool   related       = (acquaintances & pedFlag) != 0;
            if (!related && (!CGameLogic::LaRiotsActiveHere() || !CanJoinRiotAgainst(&ped, candidate))) {
                continue;
            }
        }
        if (!(visibility > 0.0f)) {
            if (id != 4 || visibility == 0.0f) {
                continue;
            }
        }
        *outA = reinterpret_cast<int32>(candidate);
        *outB = id;
        if (priority != -1) {
            return priority;
        }
        if (!*outA) {
            continue;
        }
        const bool created = plugin::CallMethodAndReturn<bool, 0x606BA0, CPedAcquaintanceScanner*, CPed*, int32, CEntity*>(this, &ped, id, reinterpret_cast<CEntity*>(*outA));
        const int32 interval = (*outB == 4 && visibility < 0.0f) ? StaticRef<int32>(0x8D2360) : StaticRef<int32>(0x8D235C);
        m_timer.m_nStartTime = CTimer::GetTimeInMS();
        m_timer.m_nInterval  = interval;
        m_timer.m_bStarted   = true;
        if (created) {
            return *outB;
        }
    }
    return -1;
}

// 0x603AF0 (hook S, cdecl, banco Fn607B; la llaman ScanEntities y FindAcquaintance). Dice si `ped` puede unirse a un disturbio contra `other`: falso si el ped es policia, medico o bombero (tipos 6, 18 y 19),
// es el jugador (0x5DF8F0) o fue creado por mision (byte +0x484 == 2); si `other` es el jugador, es lo contrario de que el ped lo respete (0x601C90); si no, si `other` tiene un grupo (0x5F7E80) cuyo lider (0x5F69A0)
// es el jugador, es lo contrario de que el ped respete al lider; en el resto de casos es falso solo si los dos son de la misma banda (tipos de banda, 0x5FE9C0) y verdadero en caso contrario.
bool CPedAcquaintanceScanner::CanJoinRiotAgainst(CPed* ped, CPed* other) {
    const ePedType type = ped->m_nPedType;
    if (type == PED_TYPE_COP || type == PED_TYPE_MEDIC || type == PED_TYPE_FIREMAN) {
        return false;
    }
    if (ped->IsPlayer() || ped->GetCreatedBy() == PED_MISSION) {
        return false;
    }
    if (other->IsPlayer()) {
        return !ped->GetIntelligence()->Respects(other);
    }
    if (CPedGroup* const group = CPedGroups::GetPedsGroup(other)) {
        auto& membership = group->GetMembership();
        if (membership.GetLeader() && membership.GetLeader()->IsPlayer()) {
            return !ped->GetIntelligence()->Respects(membership.GetLeader());
        }
    }
    if (IsPedTypeGang(type) && IsPedTypeGang(other->m_nPedType) && type == other->m_nPedType) {
        return false;
    }
    return true;
}

// 0x603E70
void CNearbyFireScanner::ScanForNearbyFires(CPed& ped) {
    // Como el original: CTaskTimer en linea. Si no estaba arrancado se arranca con intervalo 0 (y se sigue); si estaba detenido se reinicia el inicio;
    // si ya vencio (unsigned: hora >= inicio + intervalo) se rearranca con 100 ms y se escanea. Los booleanos se leen como bytes (!= 0).
    uint32 now = CTimer::GetTimeInMS();
    if (!m_timer.m_bStarted) {
        m_timer.m_nStartTime = now;
        m_timer.m_bStarted   = true;
        m_timer.m_nInterval  = 0;
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
    m_timer.m_nInterval  = 100;
    m_timer.m_bStarted   = true;

    // Como el original: estado de movimiento de la tarea de ir a un punto (1 = PEDMOVE_STILL si la tarea mas simple no lo es), tarea activa y fuego mas cercano
    CPedIntelligence* intel = ped.GetIntelligence();
    CTask* simplest = intel->GetTaskManager().GetSimplestActiveTask();
    eMoveState moveState = PEDMOVE_STILL;
    if (simplest && CTask::IsGoToTask(simplest)) {
        moveState = static_cast<CTaskSimpleGoTo*>(simplest)->m_moveState;
    }
    CTask* const active = intel->GetTaskManager().GetActiveTask();
    CFire* const fire = gFireManager.FindNearestFire(ped.GetPosition(), false, false);

    // Como el original (x87 a 24 bits: cada operacion se redondea a float): d = fuego - peaton (la posicion del peaton se vuelve a leer);
    // distancia al cuadrado = (dy*dy + dx*dx) + dz*dz, guardada como float
    float dist2 = 0.0f;
    float dz    = 0.0f;
    if (fire) {
        const CVector& firePos = fire->GetPosition();
        const CVector& pedPos  = ped.GetPosition();
        const float    dx      = firePos.x - pedPos.x;
        const float    dy      = firePos.y - pedPos.y;
        dz                     = firePos.z - pedPos.z;
        const float yy         = dy * dy;
        const float xx         = dx * dx;
        const float zz         = dz * dz;
        dist2                  = (yy + xx) + zz;
        // 0x86C918 = 20.0f: radio de aviso (comparacion ordenada: con NaN no entra); 0x858CA0 = 2.0f: desnivel maximo
        if (dist2 < 20.0f * 20.0f && fabs(dz) < 2.0f) {
            CEventFireNearby event(fire->GetPosition());
            ped.GetIntelligence()->GetEventGroup().Add(&event, false);
        }
    }

    // Como el original: si la tarea activa no es CTaskComplexWalkRoundFire (514 = 0x202) y la mas simple es de ir a un punto, el fuego a menos de 4 m (0x86C91C)
    // y con desnivel < 2.0f genera un CEventPotentialWalkIntoFire con el tamano del fuego (+0x1C) y el estado de movimiento de la primera consulta
    if (active && active->GetTaskType() != static_cast<eTaskType>(0x202)) {
        CTask* const goTo = ped.GetIntelligence()->GetTaskManager().GetSimplestActiveTask();
        if (goTo && CTask::IsGoToTask(goTo) && fire) {
            if (4.0f * 4.0f > dist2 && fabs(dz) < 2.0f) {
                CEventPotentialWalkIntoFire event(fire->GetPosition(), fire->GetStrength(), moveState);
                ped.GetIntelligence()->GetEventGroup().Add(&event, false);
            }
        }
    }
}

// 0x603BF0
void CSexyPedScanner::ScanForSexyPeds(CPed& ped, CEntity** entities, int32 count) {
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

    // Como el original: un peaton que no es de mision y esta en un vehiculo (bit 8 de 0x46C) no escanea
    if (ped.GetCreatedBy() != PED_MISSION && ped.bInVehicle) {
        return;
    }

    // Candidato mas cercano (distancia inicial 0x501502F9 = 1.0e10f) entre los peatones con tipo 5 (civil femenino) y mayor atractivo (byte con signo +0x27 de las estadisticas),
    // fuera de vehiculos, a menos de 100 m (0x859AA4 = 10000.0f al cuadrado), delante del peaton (producto con el eje +0x10 de su matriz > 0) y con linea de vision libre
    CPed*         best    = nullptr;
    float         minDist = std::bit_cast<float>(0x501502F9u);
    const CVector pedPos  = ped.GetPosition();
    for (int32 i = 0; i < count; i++) {
        CPed* const other = static_cast<CPed*>(entities[i]);
        if (!other || other->m_nPedType != PED_TYPE_CIVFEMALE) {
            continue;
        }
        if (static_cast<int8>(other->m_pStats->m_nSexiness) <= static_cast<int8>(ped.m_pStats->m_nSexiness)) {
            continue;
        }
        if (other->bInVehicle) {
            continue;
        }
        const CVector otherPos = other->GetPosition();
        const float   dx       = otherPos.x - pedPos.x;
        const float   dy       = otherPos.y - pedPos.y;
        const float   dz       = otherPos.z - pedPos.z;
        const float   zz       = dz * dz;
        const float   yy       = dy * dy;
        const float   dist2    = (zz + yy) + dx * dx;
        if (!(dist2 < 10000.0f)) {
            continue;
        }
        if (ped.GetPlayerData()) {
            g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_9, other);
        }
        if (!(dist2 < minDist)) {
            continue;
        }
        // Producto con el eje +0x10 de la matriz del peaton (sin comprobar nulo, como el EXE): (dz*f.z + dy*f.y) + dx*f.x
        const CVector& fwd = ped.m_matrix->GetForward();
        const float    p1  = dz * fwd.z;
        const float    p2  = dy * fwd.y;
        const float    dot = (p1 + p2) + dx * fwd.x;
        if (!(dot > 0.0f)) {
            continue;
        }
        if (CWorld::GetIsLineOfSightClear(pedPos, otherPos, true, false, false, true, false, false, false)) {
            best    = other;
            minDist = dist2;
        }
    }
    if (!best) {
        return;
    }
    CEventSexyPed event(best);
    ped.GetIntelligence()->GetEventGroup().Add(&event, false);
    m_timer.m_nStartTime = CTimer::GetTimeInMS();
    m_timer.m_nInterval  = 3000;
    m_timer.m_bStarted   = true;
}
