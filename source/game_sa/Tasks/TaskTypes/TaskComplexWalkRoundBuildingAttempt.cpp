/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWalkRoundBuildingAttempt
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 12/12 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexWalkRoundBuildingAttempt.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskComplexFollowPointRoute.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleHitWall.h"
#include "TaskSimpleScratchHead.h"
#include "TaskSimpleLookAbout.h"
#include "PedGeometryAnalyser.h"

void CTaskComplexWalkRoundBuildingAttempt::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWalkRoundBuildingAttempt, 0x86f334, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x654740);
    RH_ScopedInstall(Destructor, 0x654950);

    RH_ScopedInstall(ComputeMoveDir, 0x654B80);
    RH_ScopedInstall(CreateSubTask, 0x654C80);
    RH_ScopedInstall(ComputeCrapRoute, 0x656F80);
    RH_ScopedInstall(ComputeRoute, 0x657BA0);

    RH_ScopedVMTInstall(Clone, 0x655B90);
    RH_ScopedVMTInstall(GetTaskType, 0x654830);
    RH_ScopedVMTInstall(MakeAbortable, 0x6549D0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x656E70);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x654A00);
    RH_ScopedVMTInstall(ControlSubTask, 0x658020);
}

namespace {
constexpr float WRBA_RAND_RECIPROCAL = 3.0518509e-05f; // 0x858C7C (1/32767)

// Como el original: CPointRoute::operator new (0x41B7C0) y solo se pone a cero el contador
CPointRoute* NewEmptyPointRoute() {
    auto* route = static_cast<CPointRoute*>(CPointRoute::operator new(sizeof(CPointRoute)));
    if (route) {
        route->m_NumEntries = 0;
    }
    return route;
}

// Como el original: añade un punto si el contador (con signo) es < 8
void AddPointUnlessFull(CPointRoute& route, const CVector& pt) {
    if ((int32)route.m_NumEntries < 8) {
        route.m_Entries.data()[route.m_NumEntries] = pt;
        route.m_NumEntries++;
    }
}

// Como el original (0x63A040): copia el contador y solo los puntos usados
void CopyPointRoute(CPointRoute& dst, const CPointRoute& src) {
    dst.m_NumEntries = src.m_NumEntries;
    for (int32 i = 0; i < (int32)dst.m_NumEntries; i++) {
        dst.m_Entries.data()[i] = src.m_Entries.data()[i];
    }
}

// Como el original (0x59C670): matriz * punto con el orden de sumas del EXE
CVector MulMatrixPoint(const CMatrix& m, const CVector& v) {
    CVector out;
    out.x = ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x;
    out.y = ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y;
    out.z = ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z;
    return out;
}
} // namespace

// 0x654740
CTaskComplexWalkRoundBuildingAttempt::CTaskComplexWalkRoundBuildingAttempt(eMoveState moveState, CVector const& targetPos, CVector const& pos, CVector const& normal, bool flag_0x1) :
    m_targetPos{targetPos},
    m_pos{pos},
    m_normal{normal},
    m_targetEntity{nullptr},
    m_moveState{(int8)moveState},
    m_nAttempts{0}
{
    // Como el original: solo se escriben los bits 0-5 (los bits 6-7 del byte 0x49 se conservan) y m_offset no se toca
    m_bIsHeadOnCollision    = flag_0x1;
    m_routeHasPoints        = false;
    m_isWalkingAroundEntity = false;
    m_crapRouteHasPoints    = false;
    m_justAbort             = false;
    m_isFollowingRouteNow   = false;

    m_normal.z = 0.f;
    m_normal.Normalise();

    m_route     = NewEmptyPointRoute();
    m_crapRoute = NewEmptyPointRoute();
}

// 0x654840
CTaskComplexWalkRoundBuildingAttempt::CTaskComplexWalkRoundBuildingAttempt(eMoveState moveState, CEntity* target, CVector const& offset, CVector const& pos, CVector const& normal, bool bIsHeadOnCollision) :
    m_pos{pos},
    m_normal{normal},
    m_targetEntity{target},
    m_offset{offset},
    m_moveState{(int8)moveState},
    m_nAttempts{0}
{
    // Como el original: m_targetPos no se toca
    m_bIsHeadOnCollision    = bIsHeadOnCollision;
    m_routeHasPoints        = false;
    m_isWalkingAroundEntity = true;
    m_crapRouteHasPoints    = false;
    m_justAbort             = false;
    m_isFollowingRouteNow   = false;

    m_normal.z = 0.f;
    m_normal.Normalise();

    m_route     = NewEmptyPointRoute();
    m_crapRoute = NewEmptyPointRoute();

    CEntity::RegisterReference(m_targetEntity);
}

// 0x655B90 (Clone)
CTaskComplexWalkRoundBuildingAttempt::CTaskComplexWalkRoundBuildingAttempt(const CTaskComplexWalkRoundBuildingAttempt& o) :
    CTaskComplexWalkRoundBuildingAttempt{(eMoveState)o.m_moveState, o.m_targetPos, o.m_pos, o.m_normal, o.m_bIsHeadOnCollision}
{
}

// 0x654950
CTaskComplexWalkRoundBuildingAttempt::~CTaskComplexWalkRoundBuildingAttempt() {
    // Como el original: primero m_route y luego m_crapRoute
    if (m_route) {
        delete m_route;
    }
    if (m_crapRoute) {
        delete m_crapRoute;
    }
    CEntity::SafeCleanUpRef(m_targetEntity);
}

// 0x654B80
void CTaskComplexWalkRoundBuildingAttempt::ComputeMoveDir(CPed const& ped, CVector& outDir) {
    // Como el original: se proyecta (destino - posición del ped) en 2D sobre el plano de la normal;
    // si queda casi nulo se usa el vector derecho de la matriz del ped
    const auto& pedPos = ped.GetPosition();
    const float dx  = m_targetPos.x - pedPos.x;
    const float dy  = m_targetPos.y - pedPos.y;
    const float nx  = m_normal.x;
    const float ny  = m_normal.y;
    const float nz  = m_normal.z;
    const float dot = (dy * ny + dx * nx) + nz * 0.0f; // 0x858B50

    outDir.z = 0.f;
    const float x = dx - dot * nx;
    const float y = dy - dot * ny;
    outDir.x = x;
    outDir.y = y;
    if (y * y + x * x < 1.0e-8f) { // 0x86F360; NaN normaliza
        outDir = ped.m_matrix->GetRight();
    } else {
        outDir.Normalise();
    }
}

// 0x654C80
CTask* CTaskComplexWalkRoundBuildingAttempt::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_ACHIEVE_HEADING: {
        // Como el original: dirección normalizada (0x59C910), GetRadianAngleBetweenPoints(x, y, 0, 0) y LimitRadianAngle
        const auto& pedPos = ped->GetPosition();
        const auto& first  = m_route->m_Entries.data()[0];
        CVector dir{ first.x - pedPos.x, first.y - pedPos.y, first.z - pedPos.z };
        dir.Normalise();
        const float angle = CGeneral::GetRadianAngleBetweenPoints(dir.x, dir.y, 0.f, 0.f);
        return new CTaskSimpleAchieveHeading{
            CGeneral::LimitRadianAngle(angle),
            1.f,
            0.1f
        };
    }
    case TASK_COMPLEX_FOLLOW_POINT_ROUTE:
        return new CTaskComplexFollowPointRoute{
            (eMoveState)m_moveState,
            *m_route,
            CTaskComplexFollowPointRoute::Mode::ONE_WAY,
            0.5f,
            0.5f,
            false,
            false,
            true
        };
    case TASK_SIMPLE_GO_TO_POINT:
        return new CTaskSimpleGoToPoint{
            (eMoveState)m_moveState,
            m_targetPos,
            0.5f,
            false,
            false
        };
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill{1000, false, false, 8.f};
    case TASK_SIMPLE_HIT_WALL:
        return new CTaskSimpleHitWall{};
    case TASK_SIMPLE_SCRATCH_HEAD:
        return new CTaskSimpleScratchHead{};
    case TASK_SIMPLE_LOOK_ABOUT:
        return new CTaskSimpleLookAbout{1000};
    case TASK_FINISHED:
    default: // Como el original: cualquier otro tipo devuelve nullptr
        return nullptr;
    }
}

// 0x656F80
void CTaskComplexWalkRoundBuildingAttempt::ComputeCrapRoute(CPed const& ped) {
    // Como el original: siempre se calcula (dos rand y la línea de visión) aunque la ruta esté llena;
    // aleatorios en línea: normal * [1, 2) y derecha * [-1, 1)
    const float f1  = (float)CGeneral::GetRandomNumber() * WRBA_RAND_RECIPROCAL + 1.0f;
    const float nmx = f1 * m_normal.x;
    const float nmy = f1 * m_normal.y;
    const float nmz = f1 * m_normal.z;

    CVector zAxis{ 0.f, 0.f, 1.f };
    CVector right;
    CrossProduct(&right, &m_normal, &zAxis);

    float f2 = (float)CGeneral::GetRandomNumber() * WRBA_RAND_RECIPROCAL;
    f2 = f2 + f2;
    f2 = f2 - 1.0f;
    const float tx = nmx + right.x * f2;
    const float ty = nmy + right.y * f2;
    const float tz = nmz + right.z * f2;

    const auto&   pp = ped.GetPosition();
    const CVector pedPos{ pp.x, pp.y, pp.z - 0.5f }; // Ped's position is its center, so adjust it to be on the ground
    CVector target{ pedPos.x + tx, pedPos.y + ty, pedPos.z + tz };

    CColPoint cp;
    CEntity*  hitEntity{};
    if (CWorld::ProcessLineOfSight(pedPos, target, cp, hitEntity, true, false, false, false, false, false, false, false)) {
        CVector cpToPedDir{ cp.m_vecPoint.x - pedPos.x, cp.m_vecPoint.y - pedPos.y, cp.m_vecPoint.z - pedPos.z };
        if (!(cpToPedDir.NormaliseAndMag() >= 0.35f)) { // Como el original: < 0.35 o NaN sale sin marcar la ruta
            return;
        }
        target.x = cp.m_vecPoint.x - cpToPedDir.x * 0.35f;
        target.y = cp.m_vecPoint.y - cpToPedDir.y * 0.35f;
        target.z = cp.m_vecPoint.z - cpToPedDir.z * 0.35f;
    }
    AddPointUnlessFull(*m_crapRoute, target);

    m_crapRouteHasPoints = true;
}

// 0x657BA0
void CTaskComplexWalkRoundBuildingAttempt::ComputeRoute(CPed const& ped) {
    const int32 expectedBlocked = m_bIsHeadOnCollision ? 2 : 1;
    m_route->m_NumEntries = 0; // Como el original: la ruta se vacía siempre
    int32 numBlocked = 0;      // Como el original: solo cuenta los intentos con la vista ped -> punto bloqueada

    // If there were too many attempts just use crap route
    if (++m_nAttempts >= 20) {
        if (!m_crapRouteHasPoints) {
            ComputeCrapRoute(ped);
        }
        CopyPointRoute(*m_route, *m_crapRoute);
        m_routeHasPoints = true;
        return;
    }

    const CVector normal2x{ m_normal.x + m_normal.x, m_normal.y + m_normal.y, m_normal.z + m_normal.z };

    int32 num = 1;
    do {
        CVector md;
        ComputeMoveDir(ped, md);

        // Como el original: ((rand * 1/32767) * 0.4 - 0.2) + intentos
        float spread = (float)CGeneral::GetRandomNumber() * WRBA_RAND_RECIPROCAL;
        spread = spread * 0.4f;
        spread = spread - 0.2f;
        spread = spread + (float)m_nAttempts;

        const float numf = (float)num;
        CVector moveDir{ md.x * spread * numf, md.y * spread * numf, md.z * spread * numf };
        moveDir.x = moveDir.x + m_normal.x * 0.35f;
        moveDir.y = moveDir.y + m_normal.y * 0.35f;
        moveDir.z = moveDir.z + m_normal.z * 0.35f;

        CVector dir = moveDir;
        dir.Normalise();
        dir.x = dir.x * 0.7f;
        dir.y = dir.y * 0.7f;
        dir.z = dir.z * 0.7f;

        num -= m_bIsHeadOnCollision ? 2 : 3;

        // Como el original: el punto parte de m_pos (no de la posición del ped)
        const CVector target{ moveDir.x + m_pos.x, moveDir.y + m_pos.y, (moveDir.z + m_pos.z) - 0.5f };
        const CVector origin{ target.x - dir.x, target.y - dir.y, target.z - dir.z };

        const auto&   pp = ped.GetPosition();
        const CVector pedPos{ pp.x, pp.y, pp.z - 0.5f };

        if (!CWorld::GetIsLineOfSightClear(pedPos, target, true, false, false, false, false, false, false)) {
            numBlocked++;
            continue;
        }

        if (CWorld::GetIsLineOfSightClear(target, m_targetPos, true, false, false, false, false, false, false)) {
            AddPointUnlessFull(*m_route, target);
            m_routeHasPoints = true;
            return;
        }

        const CVector targetOffseted{ target.x - normal2x.x, target.y - normal2x.y, target.z - normal2x.z };
        const CVector originOffseted{ origin.x - normal2x.x, origin.y - normal2x.y, origin.z - normal2x.z };

        if (!CWorld::GetIsLineOfSightClear(target, targetOffseted, true, false, false, false, false, false, false)) {
            continue;
        }

        if (!CWorld::GetIsLineOfSightClear(origin, originOffseted, true, false, false, false, false, false, false)) {
            continue;
        }

        if (!m_crapRouteHasPoints) {
            m_crapRoute->m_NumEntries = 0; // Como el original: se vacía antes de añadir
            AddPointUnlessFull(*m_crapRoute, target);
            AddPointUnlessFull(*m_crapRoute, targetOffseted);
            m_crapRouteHasPoints = true;
        }

        if (CWorld::GetIsLineOfSightClear(targetOffseted, m_targetPos, true, false, false, false, false, false, false)) {
            AddPointUnlessFull(*m_route, target);
            AddPointUnlessFull(*m_route, targetOffseted);
            m_routeHasPoints = true;
        }
    } while (num >= -1);

    if (m_nAttempts == 1 && numBlocked == expectedBlocked) { // Current is the first attempt
        m_nAttempts = 20;
    }
}

// 0x6549D0
bool CTaskComplexWalkRoundBuildingAttempt::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (m_pSubTask->MakeAbortable(ped, priority, event)) {
        ped->bIgnoreHeightCheckOnGotoPointTask = false;
        return true;
    }
    return false;
}

// 0x656E70
CTask* CTaskComplexWalkRoundBuildingAttempt::CreateNextSubTask(CPed* ped) {
    if (m_justAbort) {
        const auto task = CreateSubTask(TASK_FINISHED, nullptr);
        ped->bIgnoreHeightCheckOnGotoPointTask = false;
        return task;
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateSubTask(TASK_COMPLEX_FOLLOW_POINT_ROUTE, ped);
    case TASK_SIMPLE_GO_TO_POINT:
    case TASK_SIMPLE_SCRATCH_HEAD:
    case TASK_SIMPLE_LOOK_ABOUT:
    case TASK_SIMPLE_HIT_WALL:
        // Como el original: LOOK_ABOUT se crea con ped nulo
        return m_routeHasPoints ? CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING, ped) : CreateSubTask(TASK_SIMPLE_LOOK_ABOUT, nullptr);
    case TASK_SIMPLE_STAND_STILL:
        ped->bIgnoreHeightCheckOnGotoPointTask = false;
        return CreateSubTask(TASK_FINISHED, nullptr);
    case TASK_COMPLEX_FOLLOW_POINT_ROUTE:
        return CreateSubTask(TASK_FINISHED, nullptr); // Como el original: no limpia el flag del ped
    default:
        // Como el original: cualquier otra subtarea limpia el flag y devuelve nullptr
        ped->bIgnoreHeightCheckOnGotoPointTask = false;
        return nullptr;
    }
}

// 0x654A00
CTask* CTaskComplexWalkRoundBuildingAttempt::CreateFirstSubTask(CPed* ped) {
    if (m_isWalkingAroundEntity) {
        if (!m_targetEntity) {
            ped->bIgnoreHeightCheckOnGotoPointTask = false;
            return nullptr;
        }
        m_targetPos = MulMatrixPoint(m_targetEntity->GetMatrix(), m_offset);
    }

    ped->bIgnoreHeightCheckOnGotoPointTask = true;

    switch (m_moveState) {
    case PEDMOVE_STILL:
    case PEDMOVE_TURN_L:
    case PEDMOVE_TURN_R:
    case PEDMOVE_WALK:
        return new CTaskSimpleScratchHead{};
    case PEDMOVE_RUN:
    case PEDMOVE_SPRINT:
        return new CTaskSimpleHitWall{};
    default:
        ped->bIgnoreHeightCheckOnGotoPointTask = false;
        return nullptr;
    }
}

// 0x658020
CTask* CTaskComplexWalkRoundBuildingAttempt::ControlSubTask(CPed* ped) {
    if (m_justAbort) {
        return m_pSubTask;
    }

    if (m_isWalkingAroundEntity) {
        if (!m_targetEntity) {
            m_justAbort = true;
            return CreateSubTask(TASK_SIMPLE_STAND_STILL, nullptr);
        }
        // Como el original: (matriz * offset) - destino, magnitud (z*z + y*y) + x*x > 16 (NaN no aborta)
        const auto  p  = MulMatrixPoint(m_targetEntity->GetMatrix(), m_offset);
        const float dx = p.x - m_targetPos.x;
        const float dy = p.y - m_targetPos.y;
        const float dz = p.z - m_targetPos.z;
        if ((dz * dz + dy * dy) + dx * dx > 16.f) {
            m_justAbort = true;
            return CreateSubTask(TASK_SIMPLE_STAND_STILL, nullptr);
        }
    }

    if (m_routeHasPoints && !m_isFollowingRouteNow && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        m_isFollowingRouteNow = true;
        if (m_route->m_NumEntries != 0) {
            return CreateSubTask(TASK_COMPLEX_FOLLOW_POINT_ROUTE, nullptr);
        }
        m_justAbort = true;
        return CreateSubTask(TASK_SIMPLE_HIT_WALL, nullptr);
    }

    if (!m_routeHasPoints) {
        ComputeRoute(*ped);
    }

    const auto subTask = m_pSubTask;
    if (subTask->GetTaskType() == TASK_COMPLEX_FOLLOW_POINT_ROUTE
     && ped->GetIntelligence()->m_AnotherStaticCounter > 30 // 0x86C938
     && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)
    ) {
        m_justAbort = true;
        return CreateSubTask(TASK_SIMPLE_HIT_WALL, ped);
    }
    return subTask;
}
