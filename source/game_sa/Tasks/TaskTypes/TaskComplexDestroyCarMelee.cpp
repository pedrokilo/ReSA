/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexDestroyCarMelee
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "X87Math.h"

#include "TaskComplexDestroyCarMelee.h"
#include "SeekEntity/TaskComplexSeekEntity.h"
#include "TaskSimplePause.h"
#include "TaskSimpleFightingControl.h"

void CTaskComplexDestroyCarMelee::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDestroyCarMelee, 0x86d994, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x621D10);
    RH_ScopedInstall(Destructor, 0x621DA0);

    RH_ScopedInstall(CreateSubTask, 0x628A70);
    RH_ScopedInstall(CalculateSearchPositionAndRanges, 0x6289F0);

    RH_ScopedVMTInstall(Clone, 0x6235A0);
    RH_ScopedVMTInstall(GetTaskType, 0x621D90);
    RH_ScopedVMTInstall(MakeAbortable, 0x621E00);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62DC20);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62DB20);
    RH_ScopedVMTInstall(ControlSubTask, 0x62DDB0);
}

// 0x621D10
CTaskComplexDestroyCarMelee::CTaskComplexDestroyCarMelee(CVehicle* vehToDestroy) :
    m_VehToDestroy{ vehToDestroy }
{
    CEntity::SafeRegisterRef(m_VehToDestroy);
}

// 0x621DA0
CTaskComplexDestroyCarMelee::~CTaskComplexDestroyCarMelee() {
    CEntity::SafeCleanUpRef(m_VehToDestroy);
}

// 0x621E00
bool CTaskComplexDestroyCarMelee::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE:
        return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);

    case ABORT_PRIORITY_URGENT: {
        if (event) {
            // Como el original: GetEventType se consulta para la colision y otra vez para el aviso de choque
            if (event->GetEventType() == EVENT_VEHICLE_COLLISION && m_VehToDestroy && static_cast<const CEventVehicleCollision*>(event)->m_vehicle == m_VehToDestroy) {
                // Como el original: velocidad al cuadrado (0x406DA0) frente a 0.0225 (0x86D9C0)
                CVector moveDir = m_VehToDestroy->m_vecMoveSpeed;
                const float speedSq = moveDir.SquaredMagnitude();
                if (speedSq == 0.f) {
                    return false;
                }
                if (speedSq < std::bit_cast<float>(0x3CB851ECu)) {
                    moveDir.Normalise();
                    const CVector delta = ped->GetPosition() - m_VehToDestroy->GetPosition();
                    if (delta.z * moveDir.z + delta.y * moveDir.y + delta.x * moveDir.x > 0.f) { // Como el original: orden z, y, x
                        return false;
                    }
                }
            }
            if (event->GetEventType() == EVENT_POTENTIAL_WALK_INTO_VEHICLE && m_VehToDestroy && static_cast<const CEventPotentialWalkIntoVehicle*>(event)->m_vehicle == m_VehToDestroy) {
                return false;
            }
        }
        return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, event); // Como el original: se pasa el evento
    }
    default: { // Como el original: cualquier otra prioridad
        m_AbortAtLeisure = true;
        return false;
    }
    }
}

// 0x62DC20
CTask* CTaskComplexDestroyCarMelee::CreateNextSubTask(CPed* ped) {
    if (m_AbortAtLeisure) {
        return nullptr;
    }
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_PAUSE:
    case TASK_SIMPLE_FIGHT_CTRL: {
        if (!ped->bStayInSamePlace) {
            CalculateSearchPositionAndRanges(ped);
            return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
        }
        return CreateSubTask(
            m_PauseTimer != -1 && CTimer::GetTimeInMS() - (uint32)m_PauseTimer > 3000
                ? TASK_FINISHED
                : TASK_SIMPLE_PAUSE,
            ped
        );
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: { // 0x62DC70
        CalculateSearchPositionAndRanges(ped);
        // Como el original: distancia al cuadrado (0x406DA0) frente al rango al cuadrado
        const float range = m_MaxAtkRange;
        if (range * range > (m_VehToDestroy->GetPosition() - ped->GetPosition()).SquaredMagnitude()) {
            return CreateSubTask(TASK_SIMPLE_FIGHT_CTRL, ped);
        }
        return CreateSubTask(ped->bStayInSamePlace ? TASK_SIMPLE_PAUSE : TASK_COMPLEX_SEEK_ENTITY, ped);
    }
    case TASK_COMPLEX_SEEK_ENTITY: { // 0x62DD11
        return CreateSubTask(TASK_SIMPLE_FIGHT_CTRL, ped);
    }
    default: // Como el original: TASK_FINISHED y tipos desconocidos devuelven nullptr sin llamar a CreateSubTask
        return nullptr;
    }
}

// 0x62DB20
CTask* CTaskComplexDestroyCarMelee::CreateFirstSubTask(CPed* ped) {
    m_HasNewTarget = false;
    // CWeaponInfo::GetWeaponInfo(ped->GetActiveWeapon().m_nType); // unused
    CalculateSearchPositionAndRanges(ped);
    const CVector delta = m_VehToDestroy->GetPosition() - ped->GetPosition();
    const float   range = m_MaxArriveRange;
    // Como el original: suma z, x, y; NaN cuenta como dentro del radio
    if (!(range * range < delta.z * delta.z + delta.x * delta.x + delta.y * delta.y)) {
        ped->m_fAimingRotation = CGeneral::GetRadianAngleBetweenPoints(m_VehPos, ped->GetPosition());
        return CreateSubTask(TASK_SIMPLE_FIGHT_CTRL, ped);
    }
    return CreateSubTask(ped->bStayInSamePlace ? TASK_SIMPLE_PAUSE : TASK_COMPLEX_SEEK_ENTITY, ped);
}

// 0x62DDB0
CTask* CTaskComplexDestroyCarMelee::ControlSubTask(CPed* ped) {
    CTask* const subTask = m_pSubTask;
    if (m_HasNewTarget) {
        return CreateFirstSubTask(ped);
    }

    if (m_VehToDestroy->m_fHealth <= 0.0f)
        return nullptr;

    switch (subTask->GetTaskType()) {
    case TASK_COMPLEX_SEEK_ENTITY:
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: { // 0x62DEAB
        CalculateSearchPositionAndRanges(ped);
        const CVector delta = m_VehToDestroy->GetPosition() - ped->GetPosition();
        const float   range = m_MaxAtkRange;
        // Como el original: suma z, x, y y comparacion estricta
        if (range * range > delta.z * delta.z + delta.x * delta.x + delta.y * delta.y) {
            return CreateSubTask(TASK_SIMPLE_FIGHT_CTRL, ped);
        }
        return subTask;
    }
    case TASK_SIMPLE_FIGHT_CTRL: { // 0x62DE1A
        CalculateSearchPositionAndRanges(ped);
        const CVector delta = m_VehToDestroy->GetPosition() - ped->GetPosition();
        const float   dist  = std::sqrt(delta.z * delta.z + delta.y * delta.y + delta.x * delta.x); // Como el original: orden z, y, x
        if (m_MaxAtkRange + 0.6f < dist) {
            return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
        }
        const auto tFightCtrl = static_cast<CTaskSimpleFightingControl*>(subTask);
        tFightCtrl->m_angleRad = m_MaxAtkAngleRad;
        tFightCtrl->m_maxAttackRange = m_MaxAtkRange;
        return subTask;
    }
    default: // Como el original: devuelve la subtarea actual
        return subTask;
    }
}

// 0x6289F0
void CTaskComplexDestroyCarMelee::CalculateSearchPositionAndRanges(CPed* ped) {
    m_VehPos = m_VehToDestroy->GetPosition();
    const CVector& pedPos = ped->GetPosition();
    const float deltaX = pedPos.x - m_VehPos.x;
    const float deltaY = pedPos.y - m_VehPos.y;
    m_MaxAtkRange = m_MaxArriveRange = m_VehToDestroy->GetModelInfo()->GetColModel()->GetBoundRadius() + 0.35f;
    // Como el original: atan2(dx, -dy) con fpatan (precision completa del FPU, redondeo al guardar)
    const float angle = TrigAtan2Cpp(deltaX, -deltaY); // X87Math.cpp
    m_MaxAtkAngleRad = angle;
}

// 0x628A70
CTask* CTaskComplexDestroyCarMelee::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_SEEK_ENTITY: { // 0x628B89
        m_PauseTimer = -1;
        return new CTaskComplexSeekEntity{
            m_VehToDestroy,
            50'000,
            1'000,
            1.f,
            2.f,
            2.f,
            true,
            true
        };
    }
    case TASK_SIMPLE_PAUSE: { // 0x628A9A
        CTaskSimpleStandStill{}.ProcessPed(ped);
        if (m_PauseTimer == -1) {
            m_PauseTimer = CTimer::GetTimeInMS();
        }
        return new CTaskSimplePause{ 100 };
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: { // 0x628AB4
        m_PauseTimer = -1;
        return new CTaskComplexGoToPointAndStandStill{
            PEDMOVE_RUN,
            m_VehPos,
            0.25f,
            0.5f
        };
    }
    case TASK_SIMPLE_FIGHT_CTRL: { // 0x628B1D
        m_PauseTimer = -1;
        return new CTaskSimpleFightingControl{
            m_VehToDestroy,
            m_MaxAtkAngleRad,
            m_MaxAtkRange
        };
    }
    case TASK_FINISHED: { // 0x628BF6
        m_PauseTimer = -1;
        return nullptr;
    }
    default: // Como el original: tipos desconocidos devuelven nullptr
        return nullptr;
    }
}
