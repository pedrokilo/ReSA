/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWalkRoundObject
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

#include "TaskComplexWalkRoundObject.h"
#include "PedGeometryAnalyser.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexFollowPointRoute.h"
#include "X87Math.h"

void CTaskComplexWalkRoundObject::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWalkRoundObject, 0x86F364, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x655020);
    RH_ScopedInstall(CreateRouteTask, 0x655140);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x657220);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x657380);
    RH_ScopedVMTInstall(ControlSubTask, 0x6575F0);
}

// 0x655020
CTaskComplexWalkRoundObject::CTaskComplexWalkRoundObject(int32 moveState, const CVector& targetPoint, CEntity* object) : CTaskComplex() {
    m_moveState   = moveState;
    m_targetPoint = targetPoint;
    m_object      = object;
    field_24      = 0;
    field_28      = 0;
    field_2C      = 0;
    field_2D      = 0;

    CEntity::SafeRegisterRef(m_object);

    // Como el original: CPointRoute::operator new (0x41B7C0) y solo se pone a cero el contador;
    // `new CPointRoute()` inicializaba por valor los 0x60 bytes de puntos
    auto* route = static_cast<CPointRoute*>(CPointRoute::operator new(sizeof(CPointRoute)));
    if (route) {
        route->m_NumEntries = 0;
    }
    m_pointRoute = route;
}

CTaskComplexWalkRoundObject::~CTaskComplexWalkRoundObject() {
    CEntity::SafeCleanUpRef(m_object);

    if (m_pointRoute) { // CPool::Delete ya no comprueba nullptr (como el EXE)
        delete m_pointRoute;
    }
    // todo: m_pointRoute = nullptr;
}

CTaskComplexWalkRoundObject* CTaskComplexWalkRoundObject::Constructor(int32 moveState, const CVector& targetPoint, CEntity* object) {
    this->CTaskComplexWalkRoundObject::CTaskComplexWalkRoundObject(moveState, targetPoint, object);
    return this;
}

// 0x6575F0
CTask* CTaskComplexWalkRoundObject::ControlSubTask(CPed* ped) {
    // Como el original: se aborta si no hay objeto, si venció el plazo de la ruta (comparación sin signo), si el contador del peatón
    // supera 30 (0x86C938, con signo), o si el objeto se ha movido o girado; con la subtarea en reposo no se evalúa nada más
    bool abort = false;
    if (!m_object) {
        abort = true;
    } else {
        if (m_pSubTask->GetTaskType() == TASK_COMPLEX_FOLLOW_POINT_ROUTE && field_2C) {
            if (field_2D) {
                field_24 = (int32)CTimer::GetTimeInMS();
                field_2D = 0;
            }
            if (!(CTimer::GetTimeInMS() < (uint32)field_24 + (uint32)field_28)) {
                abort = true;
            }
        }
        if (!abort) {
            if (ped->GetIntelligence()->m_AnotherStaticCounter > 30) {
                abort = true;
            } else if (m_pSubTask->GetTaskType() == TASK_SIMPLE_STAND_STILL) {
                return m_pSubTask;
            } else {
                if (!m_object->m_matrix) {
                    m_object->AllocateMatrix();                              // 0x54F560
                    m_object->m_placement.UpdateMatrix(m_object->m_matrix);  // 0x54F1B0
                }
                const CMatrix& mat = *m_object->m_matrix;
                const CVector& pos = m_object->GetPosition();
                if (WalkRoundMovedCpp(&m_objectStartPos.x, &pos.x)) {                                       // |inicio - posición|² > 0.0625
                    abort = true;
                } else if (WalkRoundDotBelowCpp(&mat.GetForward().x, &m_objectStartForward.x)) {            // frente actual . inicial < 0.9
                    abort = true;
                } else if (WalkRoundDotBelowCpp(&m_objectStartRight.x, &mat.GetRight().x)) {                // derecha inicial . actual < 0.9
                    abort = true;
                } else {
                    return m_pSubTask;
                }
            }
        }
    }

    // Como el original (0x657718): si la subtarea se puede abortar, se crea la de reposo (CreateSubTask 0x655290, TASK_SIMPLE_STAND_STILL)
    if (!m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        return m_pSubTask;
    }
    return new CTaskSimpleStandStill(500, false, false, 8.0f);
}

// 0x657220
CTask* CTaskComplexWalkRoundObject::CreateNextSubTask(CPed* ped) {
    if (!m_object) {
        return nullptr;
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: // 0x387
        return CreateRouteTask(ped);
    case TASK_SIMPLE_ACHIEVE_HEADING: // 0x386
        break;
    default: // incluye TASK_SIMPLE_STAND_STILL (0xCB) y TASK_COMPLEX_FOLLOW_POINT_ROUTE (0x389)
        return nullptr;
    }

    if (m_pointRoute->m_NumEntries != 0) {
        return CreateRouteTask(ped);
    }

    // Como el original: punto más cercano de la superficie del objeto al peatón; si cae del mismo lado que el destino se añaden a la ruta
    // ese punto y el más cercano al destino; si no, se camina hasta el punto
    CVector point;
    CPedGeometryAnalyser::ComputeClosestSurfacePoint(*ped, *m_object, point);
    const int32 sideOfPoint  = CPedGeometryAnalyser::ComputeEntityHitSide(point, *m_object);
    const int32 sideOfTarget = CPedGeometryAnalyser::ComputeEntityHitSide(m_targetPoint, *m_object);
    if (sideOfPoint == sideOfTarget) {
        m_pointRoute->AddUnlessFull(point);
        CPedGeometryAnalyser::ComputeClosestSurfacePoint(m_targetPoint, *m_object, point);
        m_pointRoute->AddUnlessFull(point);
        return CreateRouteTask(ped);
    }
    return new CTaskComplexGoToPointAndStandStill(PEDMOVE_WALK, point, 0.5f, 2.0f, false, false); // 0x86FC84 y 0x86FC88
}

// 0x657380
CTask* CTaskComplexWalkRoundObject::CreateFirstSubTask(CPed* ped) {
    if (!m_object) {
        return nullptr;
    }

    m_pointRoute->m_NumEntries = 0;
    // 0x6551D0 (sin símbolo en el catálogo): calcula la ruta alrededor del objeto y devuelve un float en ST0
    const float routeDist = plugin::CallMethodAndReturn<float, 0x6551D0, CTaskComplexWalkRoundObject*, CPed*>(this, ped);
    if (m_pointRoute->m_NumEntries == 0) {
        return nullptr;
    }

    field_24 = (int32)CTimer::GetTimeInMS();
    field_28 = m_moveState == PEDMOVE_WALK ? 8000 : 4000;
    field_2C = 1;

    // Como el original: posición, frente y derecha iniciales del objeto (se crea la matriz si falta)
    m_objectStartPos = m_object->GetPosition();
    if (!m_object->m_matrix) {
        m_object->AllocateMatrix();                              // 0x54F560
        m_object->m_placement.UpdateMatrix(m_object->m_matrix);  // 0x54F1B0
    }
    m_objectStartForward = m_object->m_matrix->GetForward();
    if (!m_object->m_matrix) {
        m_object->AllocateMatrix();
        m_object->m_placement.UpdateMatrix(m_object->m_matrix);
    }
    m_objectStartRight = m_object->m_matrix->GetRight();

    // Como el original: dirección del peatón al primer punto de la ruta (floats guardados), normalizada con su módulo (0x59C970), y su
    // producto escalar con el frente del peatón (sin comprobar su matriz) en el orden (z + y) + x, sin redondear
    const CVector& pedPos = ped->GetPosition();
    const CVector& p0     = m_pointRoute->m_Entries.data()[0];
    CVector        dir{ p0.x - pedPos.x, p0.y - pedPos.y, p0.z - pedPos.z };
    const float    mag    = dir.NormaliseAndMag();
    const CMatrix* pedMat = ped->m_matrix;
    const bool     facing = WalkRoundDotPositiveCpp(&dir.x, &pedMat->GetForward().x);
    const float    maxDist = m_moveState == PEDMOVE_WALK ? 2.0f : (m_moveState == PEDMOVE_RUN ? 4.0f : 6.0f); // 0x858CA0 / 0x858B90 / 0x858B44
    if (mag > maxDist && routeDist > maxDist && facing) {
        return CreateRouteTask(ped);
    }

    // Como el original: se gira hacia el primer punto (ángulo entre (x0 - px, y0 - py) y el origen, limitado)
    const float x = p0.x - pedPos.x;
    const float y = p0.y - pedPos.y;
    const float heading = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(x, y, 0.f, 0.f));
    return new CTaskSimpleAchieveHeading(heading, 1.0f, 0.2f);
}

// 0x655140 (ret 4: el EXE no usa el peaton). Sin ruta (contador a cero) no crea nada; si no, reserva con CTask::operator new (0x61A5A0) y, con el pool agotado (nulo),
// no construye y devuelve nulo (`test eax, eax; je`); el constructor 0x671510 recibe (movimiento, ruta, 0 = ONE_WAY, 0.5f [0x86FC98], 0.0f, true, false, false).
// La ruta se lee sin comprobar el nulo, como el EXE.
CTask* CTaskComplexWalkRoundObject::CreateRouteTask(CPed* ped) {
    if (m_pointRoute->m_NumEntries == 0) {
        return nullptr;
    }
    void* const memory = CTask::operator new(sizeof(CTaskComplexFollowPointRoute));
    if (!memory) {
        return nullptr;
    }
    return ::new (memory) CTaskComplexFollowPointRoute(static_cast<eMoveState>(m_moveState), *m_pointRoute, CTaskComplexFollowPointRoute::Mode::ONE_WAY, 0.5f, 0.0f, true, false, false);
}
