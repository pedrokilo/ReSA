/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGoToPointAiming
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexGoToPointAiming.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleUseGun.h"
#include "TaskSimpleGunControl.h"
#include "WeaponInfo.h"

void CTaskComplexGoToPointAiming::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToPointAiming, 0x86fe00, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x668790);
    RH_ScopedInstall(Destructor, 0x668870);
    RH_ScopedInstall(CreateSubTask, 0x6688D0);

    RH_ScopedVMTInstall(Clone, 0x66CD80);
    RH_ScopedVMTInstall(GetTaskType, 0x668860);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66DD70);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x66DDB0);
    RH_ScopedVMTInstall(ControlSubTask, 0x6689E0);
}

// 0x668790
CTaskComplexGoToPointAiming::CTaskComplexGoToPointAiming(
    eMoveState moveState,
    const CVector& movePos,
    CEntity* aimAtEntity,
    CVector aimPos,
    const float targetRadius,
    const float slowDownDist
) :
    m_aimPos{aimPos},
    m_aimAtEntity{aimAtEntity},
    m_moveState{moveState}
{
    CEntity::SafeRegisterRef(m_aimAtEntity);
    // The original updates the destination in-place, preserving the flag when
    // the pool slot already contains exactly the same destination and distances.
    if (m_movePos != movePos || m_moveTargetRadius != targetRadius || m_slowDownDistance != slowDownDist) {
        m_movePos = movePos;
        m_moveTargetRadius = targetRadius;
        m_slowDownDistance = slowDownDist;
        m_newTargetSet = true;
    }
}

CTaskComplexGoToPointAiming::CTaskComplexGoToPointAiming(const CTaskComplexGoToPointAiming& o) :
    CTaskComplexGoToPointAiming{
        o.m_moveState,
        o.m_movePos,
        o.m_aimAtEntity,
        o.m_aimPos,
        o.m_moveTargetRadius,
        o.m_slowDownDistance
    }
{
}

// 0x668870
CTaskComplexGoToPointAiming::~CTaskComplexGoToPointAiming() {
    CEntity::SafeCleanUpRef(m_aimAtEntity);
}

// 0x6688D0
CTask* CTaskComplexGoToPointAiming::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        return new CTaskComplexGoToPointAndStandStill(m_moveState, m_movePos, m_moveTargetRadius, m_slowDownDistance, false, false);
    case TASK_SIMPLE_GUN_CTRL: {
        // Como el original: la variante "shooting" (0x385) dispara ráfagas; la de apuntar no dispara
        // (el EXE pasa un puntero nulo como destino de movimiento: el constructor lo deja a cero)
        const auto cmd = GetTaskType() == TASK_COMPLEX_GO_TO_POINT_SHOOTING ? eGunCommand::FIREBURST : eGunCommand::NONE;
        return new CTaskSimpleGunControl(m_aimAtEntity, m_aimPos, CVector{}, cmd, 1, 600000);
    }
    default: // incluida TASK_FINISHED
        return nullptr;
    }
}

// 0x66DD70
CTask* CTaskComplexGoToPointAiming::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
    case TASK_SIMPLE_GUN_CTRL:
        return CreateSubTask(TASK_FINISHED);
    default:
        return nullptr; // Como el original: cualquier otro tipo devuelve nulo
    }
}

// 0x66DDB0
CTask* CTaskComplexGoToPointAiming::CreateFirstSubTask(CPed* ped) {
    m_newTargetSet = false;

    // Como el original: tipo del arma activa (índice con signo), GetWeaponSkill() y GetWeaponInfo(tipo, habilidad)
    const auto weaponType = ped->m_aWeapons.data()[static_cast<int8>(ped->m_nActiveWeaponSlot)].m_Type;
    const auto* const info = CWeaponInfo::GetWeaponInfo(weaponType, ped->GetWeaponSkill());

    if (info->flags.bAimWithArm) {
        auto& taskMgr = ped->GetTaskManager();
        if (!taskMgr.GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
            CTask* gunTask;
            if (GetTaskType() == TASK_COMPLEX_GO_TO_POINT_SHOOTING) {
                gunTask = new CTaskSimpleUseGun(m_aimAtEntity, m_aimPos, eGunCommand::FIREBURST, 3, false);
            } else {
                gunTask = new CTaskSimpleUseGun(m_aimAtEntity, m_aimPos, eGunCommand::AIM, 1, false);
            }
            taskMgr.SetTaskSecondary(gunTask, TASK_SECONDARY_ATTACK);
        } else {
            taskMgr.GetTaskSecondary(TASK_SECONDARY_ATTACK)->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
        }
        return new CTaskComplexGoToPointAndStandStill(m_moveState, m_movePos, m_moveTargetRadius, m_slowDownDistance, false, false);
    }

    if (info->flags.bCanAim) {
        const auto fire = info->m_nWeaponFire;
        if (fire == WEAPON_FIRE_INSTANT_HIT || fire == WEAPON_FIRE_AREA_EFFECT) {
            return CreateSubTask(TASK_SIMPLE_GUN_CTRL);
        }
    }
    return new CTaskComplexGoToPointAndStandStill(m_moveState, m_movePos, m_moveTargetRadius, m_slowDownDistance, false, false);
}

// 0x6689E0
CTask* CTaskComplexGoToPointAiming::ControlSubTask(CPed* ped) {
    // Como el original: tipo del arma activa y subtarea actual se leen antes de pedir la información del arma
    const auto weaponType = ped->m_aWeapons.data()[static_cast<int8>(ped->m_nActiveWeaponSlot)].m_Type;
    CTask* const result = m_pSubTask;
    const auto* const info = CWeaponInfo::GetWeaponInfo(weaponType, ped->GetWeaponSkill());

    if (m_newTargetSet) {
        return CreateFirstSubTask(ped);
    }

    auto& taskMgr = ped->GetTaskManager();
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
        static_cast<CTaskComplexGoToPointAndStandStill*>(m_pSubTask)->GoToPoint(m_movePos, 0.5f, 2.0f, false);
        if (!info->flags.bAimWithArm) {
            break;
        }
        if (!taskMgr.GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
            CTask* const gunTask = new CTaskSimpleUseGun(m_aimAtEntity, m_aimPos, eGunCommand::AIM, 1, false);
            taskMgr.SetTaskSecondary(gunTask, TASK_SECONDARY_ATTACK);
        } else if (ped->GetIntelligence()->GetTaskUseGun()) {
            const auto cmd = (GetTaskType() == TASK_COMPLEX_GO_TO_POINT_SHOOTING && (CGeneral::GetRandomNumber() & 0x3F) == 0)
                ? eGunCommand::FIRE
                : eGunCommand::AIM;
            static_cast<CTaskSimpleUseGun*>(taskMgr.GetTaskSecondary(TASK_SECONDARY_ATTACK))->ControlGun(ped, m_aimAtEntity, cmd);
        } else {
            taskMgr.GetTaskSecondary(TASK_SECONDARY_ATTACK)->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
        }
        break;
    }
    case TASK_SIMPLE_GUN_CTRL: {
        if (!ped->GetIntelligence()->GetTaskUseGun()) {
            break;
        }
        // Como el original: desplazamiento al destino proyectado sobre los ejes derecho y delantero del peatón
        // (sin guarda: el EXE lee la matriz del peatón directamente)
        const CVector& pedPos = ped->GetPosition();
        const CMatrix& mat = *ped->m_matrix;
        const float dx = m_movePos.x - pedPos.x;
        const float dy = m_movePos.y - pedPos.y;
        const float dz = m_movePos.z - pedPos.z;
        const float rightDot = (dz * mat.GetRight().z + dy * mat.GetRight().y) + dx * mat.GetRight().x;
        const float fwdDot = (dz * mat.GetForward().z + dy * mat.GetForward().y) + dx * mat.GetForward().x;
        const float v = fwdDot * -1.0f;
        const float lenSq = v * v + rightDot * rightDot;
        CVector2D move{};
        // Como el original: se normaliza solo si el cuadrado supera el del radio (0 con NaN)
        if (lenSq > m_moveTargetRadius * m_moveTargetRadius) {
            const float scale = 1.0f / std::sqrt(lenSq);
            move.x = rightDot * scale;
            move.y = scale * v;
        }
        static_cast<CTaskSimpleUseGun*>(taskMgr.GetTaskSecondary(TASK_SECONDARY_ATTACK))->ControlGunMove(move);
        break;
    }
    default:
        break;
    }
    return result;
}
