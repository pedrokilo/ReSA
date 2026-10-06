/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWalkRoundCar
 * Reconstrucción: COMPLETA — 13/13 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 13/13 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexFollowPointRoute.h"
#include "TaskComplexWalkRoundCar.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleAchieveHeading.h"
#include "PedGeometryAnalyser.h"
#include "CarEnterExit.h"

void CTaskComplexWalkRoundCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWalkRoundCar, 0x86f308, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6541B0);
    RH_ScopedInstall(Destructor, 0x656B00);

    RH_ScopedInstall(SetNewVehicle, 0x654290);
    RH_ScopedInstall(CreateRouteTask, 0x6542E0);
    RH_ScopedInstall(ComputeRouteRoundSmallCar, 0x6544F0);
    RH_ScopedInstall(GoingForDoor, 0x654720);
    RH_ScopedInstall(ComputeRouteRoundBigCar, 0x656BB0);
    RH_ScopedInstall(ComputeRoute, 0x657B80);

    RH_ScopedVMTInstall(Clone, 0x655B00);
    RH_ScopedVMTInstall(GetTaskType, 0x654280);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x656B70);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x658200);
    RH_ScopedVMTInstall(ControlSubTask, 0x654370);
}

// 0x6541B0
CTaskComplexWalkRoundCar::CTaskComplexWalkRoundCar(eMoveState moveState, CVector const& targetPt, CVehicle* vehicle, bool isPedGoingForCarDoor, uint8 forceThisDirectionRoundCar) {
    // Como el original: solo se escriben estos campos (el segundo bit de las banderas a 0, el resto sin tocar)
    m_MoveState                     = static_cast<uint8>(moveState);
    m_bIsPedGoingForCarDoor         = isPedGoingForCarDoor;
    m_bFirstSubTaskNeedsToBeCreated = false;
    m_ForceThisDirectionRoundCar    = forceThisDirectionRoundCar;
    m_TargetPt                      = targetPt;
    m_Veh                           = vehicle;
    // m_Timer: el temporizador se deja a cero (0x24, 0x28, 0x2C y 0x2D) por sus inicializadores
    m_EnterCarStartTime             = -1;
    CEntity::SafeRegisterRef(m_Veh);
    // Como el original: la ruta sale del pool de rutas y solo se pone a cero su contador
    m_Route = static_cast<CPointRoute*>(CPointRoute::operator new(sizeof(CPointRoute)));
    if (m_Route) {
        m_Route->m_NumEntries = 0;
    }
}

CTaskComplexWalkRoundCar::CTaskComplexWalkRoundCar(const CTaskComplexWalkRoundCar& o) :
    CTaskComplexWalkRoundCar{
        (eMoveState)o.m_MoveState,
        o.m_TargetPt,
        o.m_Veh,
        o.m_bIsPedGoingForCarDoor,
        o.m_ForceThisDirectionRoundCar
    }
{
}

// 0x656B00
CTaskComplexWalkRoundCar::~CTaskComplexWalkRoundCar() {
    CEntity::SafeCleanUpRef(m_Veh);
    delete m_Route;
}


// 0x654290
void CTaskComplexWalkRoundCar::SetNewVehicle(CVehicle * vehicle, uint8 forceThisDirectionRoundCar) {
    // Como el original: sin comprobar si el vehículo es el mismo; se limpia la referencia anterior y se registra la nueva
    CEntity::SafeCleanUpRef(m_Veh);
    m_Veh = vehicle;
    m_Veh->RegisterReference(reinterpret_cast<CEntity**>(&m_Veh));
    m_ForceThisDirectionRoundCar    = forceThisDirectionRoundCar;
    m_bFirstSubTaskNeedsToBeCreated = true;
    m_Route->Clear();
}

// 0x6542E0
CTask* CTaskComplexWalkRoundCar::CreateRouteTask(CPed*) const {
    if (m_Route->IsEmpty()) {
        return nullptr;
    }
    return new CTaskComplexFollowPointRoute{ (eMoveState)m_MoveState, *m_Route };
}

// 0x6544F0
float CTaskComplexWalkRoundCar::ComputeRouteRoundSmallCar(CPed* ped) {
    // Como el original: esquinas y planos del vehículo a la altura del peatón
    CVector corners[4];
    CPedGeometryAnalyser::ComputeEntityBoundingBoxCorners(ped->GetPosition().z, *m_Veh, corners);
    CVector planes[4];
    float   planesDot[4];
    CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes(ped->GetPosition().z, *m_Veh, &planes, planesDot);
    // Como el original: distancia con signo del peatón al plano del lado que toca (0 si no toca ninguno); es el valor de retorno (ST0)
    float pedDist = 0.0f;
    {
        const auto pedSide = CPedGeometryAnalyser::ComputeEntityHitSide(*ped, *m_Veh);
        if (pedSide != -1) {
            const auto& pp = ped->GetPosition();
            const auto& pl = planes[pedSide];
            pedDist = ((pl.z * pp.z + pl.y * pp.y) + pl.x * pp.x) + planesDot[pedSide];
        }
    }

    if (m_bIsPedGoingForCarDoor) {
        // Como el original: distancia con signo del destino a cada uno de los 4 planos, sumando en el orden de cada plano
        float dist[4];
        int32 k = 0;
        for (; k < 4; k++) {
            const auto& pl = planes[k];
            const auto& tp = m_TargetPt;
            float d;
            if (k == 0) {
                d = ((pl.x * tp.x + pl.z * tp.z) + tp.y * pl.y) + planesDot[0];
            } else {
                d = ((pl.y * tp.y + pl.x * tp.x) + pl.z * tp.z) + planesDot[k];
            }
            dist[k] = d;
            if (d >= 0.0f) { // el destino está fuera de este plano (con NaN se sigue con el siguiente)
                break;
            }
        }
        if (k == 4) {
            // Como el original: dentro de los 4 planos, se empuja el destino fuera por el lado en el que está
            const auto side = CPedGeometryAnalyser::ComputeEntityHitSide(m_TargetPt, *m_Veh);
            if (side != -1) {
                const float s = (-dist[side]) + 0.05f;
                const auto& pl = planes[side];
                const float ox = s * pl.x;
                const float oy = s * pl.y;
                const float oz = s * pl.z;
                m_TargetPt.x = ox + m_TargetPt.x;
                m_TargetPt.y = oy + m_TargetPt.y;
                m_TargetPt.z = oz + m_TargetPt.z;
            }
        }
    }

    // Como el original: dirección forzada con signo (4 bits) y el resultado se guarda en los bits 2-5 del byte 0xD
    const int32 forceDir = static_cast<int32>(static_cast<int8>(static_cast<uint8>(m_ForceThisDirectionRoundCar << 4))) >> 4;
    const auto  dirGoing = CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox(*ped, *m_Veh, m_TargetPt, *m_Route, forceDir);
    m_DirectionGoingRoundCar = dirGoing;
    return pedDist;
}

// 0x656BB0
float CTaskComplexWalkRoundCar::ComputeRouteRoundBigCar(CPed* ped) {
    auto& pedHitDist = StaticRef<float>(0x8D22B0);
    const auto pedSide    = CPedGeometryAnalyser::ComputeEntityHitSide(*ped, *m_Veh);
    const auto targetSide = CPedGeometryAnalyser::ComputeEntityHitSide(m_TargetPt, *m_Veh);

    CVector pedSurf, targetSurf;
    if (pedSide == targetSide) {
        // Como el original: mismo lado, la ruta es el punto de superficie del peatón y el del destino
        const float saved = pedHitDist;
        pedHitDist = 0.7f; // 0x3F333333
        CPedGeometryAnalyser::ComputeClosestSurfacePoint(*ped, *m_Veh, pedSurf);
        CPedGeometryAnalyser::ComputeClosestSurfacePoint(m_TargetPt, *m_Veh, targetSurf);
        pedHitDist = saved;
        m_Route->AddUnlessFull(pedSurf);
        m_Route->AddUnlessFull(targetSurf);
        return 0.0f;
    }

    // Como el original: lados distintos, se calcula la ruta de un coche pequeño entre ambos puntos de superficie
    float saved = pedHitDist;
    pedHitDist = 0.7f;
    CPedGeometryAnalyser::ComputeClosestSurfacePoint(*ped, *m_Veh, pedSurf);
    CPedGeometryAnalyser::ComputeClosestSurfacePoint(m_TargetPt, *m_Veh, targetSurf);
    pedHitDist = saved;

    CVector& pedPos = ped->GetPosition();
    const CVector pedPosSaved = pedPos;
    pedPos = pedSurf;
    const CVector targetSaved = m_TargetPt;
    m_TargetPt = targetSurf;
    (void)ComputeRouteRoundSmallCar(ped);
    m_TargetPt = targetSaved;
    pedPos = pedPosSaved;

    // Como el original: la ruta se reconstruye con el punto de superficie del peatón, la ruta pequeña y el del destino
    const CPointRoute smallRoute = *m_Route;
    m_Route->m_NumEntries = 0;
    m_Route->AddUnlessFull(pedSurf);
    for (uint32 i = 0; i < smallRoute.m_NumEntries; i++) {
        m_Route->AddUnlessFull(smallRoute.m_Entries.data()[i]);
    }
    saved = pedHitDist;
    pedHitDist = 0.7f;
    CPedGeometryAnalyser::ComputeClosestSurfacePoint(m_TargetPt, *m_Veh, pedSurf);
    pedHitDist = saved;
    m_Route->AddUnlessFull(pedSurf);
    return 0.0f;
}

// 0x657B80
float CTaskComplexWalkRoundCar::ComputeRoute(CPed* ped) {
    return m_Veh->vehicleFlags.bIsBig
        ? ComputeRouteRoundBigCar(ped)
        : ComputeRouteRoundSmallCar(ped);
}

// 0x656B70
CTask* CTaskComplexWalkRoundCar::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateRouteTask(ped);
    }
    return nullptr;
}

// 0x658200
CTask* CTaskComplexWalkRoundCar::CreateFirstSubTask(CPed* ped) {
    m_Route->m_NumEntries = 0;
    const float routeResult = m_Veh->vehicleFlags.bIsBig ? ComputeRouteRoundBigCar(ped) : ComputeRouteRoundSmallCar(ped);
    if (m_Route->m_NumEntries == 0) {
        return nullptr;
    }

    // Como el original: 15000 ms (20000 si camina), multiplicado por 4 si el vehículo remolca o es remolcado
    int32 timeMs = m_MoveState == PEDMOVE_WALK ? 20000 : 15000;
    if (m_Veh->m_pVehicleBeingTowed || m_Veh->m_pTowingVehicle) { // +0x4C8 y +0x4C4
        timeMs = static_cast<int32>(static_cast<float>(timeMs) * 4.0f);
    }
    m_Timer.m_nStartTime = CTimer::m_snTimeInMilliseconds;
    m_Timer.m_nInterval  = timeMs;
    m_Timer.m_bStarted   = true;

    // Como el original: posición y ejes (delantero y derecho) del vehículo; se lee su matriz sin guarda de nulo
    const CMatrix& vehMat = *m_Veh->m_matrix;
    m_VehPos      = m_Veh->GetPosition();
    m_VehMatFwd   = vehMat.GetForward();
    m_VehMatRight = vehMat.GetRight();

    // Como el original: dirección normalizada al primer punto de la ruta y su producto escalar con el eje delantero del peatón
    const CVector& pedPos = ped->GetPosition();
    const CVector& first  = m_Route->m_Entries.data()[0];
    CVector toFirst{ first.x - pedPos.x, first.y - pedPos.y, first.z - pedPos.z };
    const float distToFirst = toFirst.NormaliseAndMag();
    const CMatrix& pedMat = *ped->m_matrix;
    const float fwdDot = (toFirst.z * pedMat.GetForward().z + toFirst.y * pedMat.GetForward().y) + toFirst.x * pedMat.GetForward().x;

    float threshold;
    if (m_MoveState == PEDMOVE_WALK) {
        threshold = 2.0f;
    } else if (m_MoveState == PEDMOVE_RUN) {
        threshold = 4.0f;
    } else {
        threshold = 6.0f;
    }

    // Como el original: el jugador recuerda cuándo empezó a entrar al coche (campo 0x4C de la tarea de entrada)
    if (ped->IsPlayer()) {
        if (CTask* const enterTask = ped->GetTaskManager().FindTaskByType(TASK_PRIMARY_PRIMARY, TASK_COMPLEX_ENTER_CAR_AS_DRIVER)) {
            m_EnterCarStartTime = *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(enterTask) + 0x4C);
        }
    }

    // Como el original: coches pequeños lejos del destino siguen la ruta; si no, se queda quieto y mira al primer punto
    if (!m_Veh->vehicleFlags.bIsBig && distToFirst > threshold && routeResult > threshold && fwdDot > 0.0f) {
        return CreateRouteTask(ped);
    }

    CTaskSimpleStandStill standStill{ 0, false, false, 8.0f };
    standStill.CTaskSimpleStandStill::ProcessPed(ped);
    const CVector& pedPos2 = ped->GetPosition();
    const CVector& first2  = m_Route->m_Entries.data()[0];
    const float angle = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(first2.x - pedPos2.x, first2.y - pedPos2.y, 0.0f, 0.0f));
    return new CTaskSimpleAchieveHeading(angle, 1.0f, 0.1f);
}

// 0x654370
CTask* CTaskComplexWalkRoundCar::ControlSubTask(CPed* ped) {
    if (m_bFirstSubTaskNeedsToBeCreated) {
        m_bFirstSubTaskNeedsToBeCreated = false;
        // Como el original: llamada virtual (hueco 9); la clase es final y el compilador la resolvería directamente
        return static_cast<CTaskComplex*>(this)->CreateFirstSubTask(ped);
    }

    int32 newTaskType = TASK_NONE;
    bool  playerQuits{};
    if (ped->IsPlayer() && m_Veh && m_EnterCarStartTime != -1
        && CCarEnterExit::IsPlayerToQuitCarEnter(ped, m_Veh, m_EnterCarStartTime, m_pSubTask)
        && m_pSubTask->GetTaskType() == TASK_COMPLEX_FOLLOW_POINT_ROUTE
    ) {
        playerQuits = true;
        newTaskType = TASK_FINISHED;
    } else if (m_pSubTask->GetTaskType() == TASK_COMPLEX_FOLLOW_POINT_ROUTE && m_Timer.IsOutOfTime()) {
        newTaskType = TASK_FINISHED;
    }

    // Como el original: con vehículo que no se ha movido ni girado (0,0625 de distancia al cuadrado y 0,9 de producto escalar)
    // y sin motivo para terminar, la subtarea sigue; sin vehículo se intenta abortar siempre
    if (m_Veh) {
        const CVector& vehPos = m_Veh->GetPosition();
        const float dx = m_VehPos.x - vehPos.x;
        const float dy = m_VehPos.y - vehPos.y;
        const float dz = m_VehPos.z - vehPos.z;
        const float distSq = (dz * dz + dy * dy) + dx * dx;
        if (!(distSq > 0.0625f)) {
            const CMatrix& vehMat = *m_Veh->m_matrix;
            const float fwdChange = (vehMat.GetForward().z * m_VehMatFwd.z + vehMat.GetForward().y * m_VehMatFwd.y) + vehMat.GetForward().x * m_VehMatFwd.x;
            if (!(fwdChange < 0.9f)) {
                const float rightChange = (m_VehMatRight.z * vehMat.GetRight().z + m_VehMatRight.y * vehMat.GetRight().y) + vehMat.GetRight().x * m_VehMatRight.x;
                if (!(rightChange < 0.9f)) {
                    if (newTaskType == TASK_NONE) {
                        return m_pSubTask;
                    }
                }
            }
        }
    }

    if (m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        if (playerQuits) {
            // Como el original: la tarea primaria de entrar al coche como conductor (si la hay) también se aborta
            if (CTask* const primary = ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_PRIMARY)) {
                if (primary->GetTaskType() == TASK_COMPLEX_ENTER_CAR_AS_DRIVER) {
                    primary->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
                }
            }
        }
        return nullptr;
    }
    return m_pSubTask;
}
