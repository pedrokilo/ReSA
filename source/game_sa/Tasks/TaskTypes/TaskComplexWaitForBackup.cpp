/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWaitForBackup
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
#include "TaskComplexWaitForBackup.h"
#include "TaskSimpleGunControl.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimplePause.h"
#include "TaskComplexLeaveCar.h"

void CTaskComplexWaitForBackup::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWaitForBackup, 0x870da4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x697290);
    RH_ScopedInstall(Destructor, 0x697330);

    RH_ScopedInstall(CreateSubTask, 0x6973B0);

    RH_ScopedVMTInstall(Clone, 0x697740);
    RH_ScopedVMTInstall(GetTaskType, 0x697320);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x697F10);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6980C0);
    RH_ScopedVMTInstall(ControlSubTask, 0x6973A0);
}

// 0x697290
CTaskComplexWaitForBackup::CTaskComplexWaitForBackup(CPed* aggressor, CPed* backup, float waitDist) :
    m_backup{backup},
    m_aggressor{aggressor},
    m_waitDistance{waitDist}
{
    // Como el original: registra primero al agresor (+0x10) y luego al refuerzo (+0x14)
    CEntity::SafeRegisterRef(m_aggressor);
    CEntity::SafeRegisterRef(m_backup);
}

CTaskComplexWaitForBackup::CTaskComplexWaitForBackup(const CTaskComplexWaitForBackup& o) :
    CTaskComplexWaitForBackup{o.m_aggressor, o.m_backup, o.m_waitDistance}
{
}

// 0x697330
CTaskComplexWaitForBackup::~CTaskComplexWaitForBackup() {
    // Como el original: limpia primero al agresor (+0x10) y luego al refuerzo (+0x14)
    CEntity::SafeCleanUpRef(m_aggressor);
    CEntity::SafeCleanUpRef(m_backup);
}

// 0x6973B0
CTask* CTaskComplexWaitForBackup::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_GUN_CTRL:
        return new CTaskSimpleGunControl{ m_aggressor, {}, {}, eGunCommand::AIM, 100, 2000 };
    case TASK_SIMPLE_ACHIEVE_HEADING: {
        // Como el original: GetRadianAngleBetweenPoints(dx, dy, 0, 0) y luego LimitRadianAngle
        const auto& aggressorPos = m_aggressor->GetPosition();
        const auto& pedPos       = ped->GetPosition();
        const float dx           = aggressorPos.x - pedPos.x;
        const float dy           = aggressorPos.y - pedPos.y;
        const float heading      = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(dx, dy, 0.0f, 0.0f));
        ped->m_fAimingRotation   = heading;
        return new CTaskSimpleAchieveHeading{ heading, 0.5f, 0.2f };
    }
    case TASK_SIMPLE_PAUSE:
        return new CTaskSimplePause{ ped->IsInVehicle() ? CGeneral::GetRandomIntInRange(1000, 2000) /* 0x407180: [1000, 2000) */ : 0 };
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar{ ped->m_pVehicle, TARGET_DOOR_FRONT_LEFT, 0, true, false };
    case TASK_FINISHED:
    default: // Como el original: cualquier otro tipo devuelve nullptr
        return nullptr;
    }
}

// 0x697F10
CTask* CTaskComplexWaitForBackup::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_GUN_CTRL: {
        if (m_backup && m_backup->IsAlive() && !m_timer.IsOutOfTime()) {
            // Como el original (0x69806A): sigue esperando mientras el refuerzo NO esté dentro
            // de la distancia (dist² >= espera², NaN incluido); si ya llegó, termina.
            if (!(CVector2D{ ped->GetPosition() - m_backup->GetPosition() }.SquaredMagnitude() < sq(m_waitDistance))) {
                return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING, ped);
            }
        }
        return CreateSubTask(TASK_FINISHED, ped);
    }
    case TASK_SIMPLE_ACHIEVE_HEADING: {
        const auto& winfo = ped->GetActiveWeapon().GetWeaponInfo();
        return CreateSubTask(
            winfo.m_nWeaponFire == eWeaponFire::WEAPON_FIRE_INSTANT_HIT
                ? TASK_SIMPLE_GUN_CTRL
                : TASK_FINISHED,
            ped
        );
    }
    case TASK_COMPLEX_LEAVE_CAR:
        return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING, ped);
    case TASK_SIMPLE_PAUSE:
        return CreateSubTask(
            ped->IsInVehicle()
                ? TASK_COMPLEX_LEAVE_CAR
                : TASK_SIMPLE_ACHIEVE_HEADING,
            ped
        );
    default: // Como el original: devuelve nullptr
        return nullptr;
    }
}

// 0x6980C0
CTask* CTaskComplexWaitForBackup::CreateFirstSubTask(CPed* ped) {
    m_timer.Start(20'000);
    return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
}
