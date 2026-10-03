/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexSeekEntityAiming
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexSeekEntityAiming.h"

#include "SeekEntity/TaskComplexSeekEntity.h"
#include "TaskSimpleUseGun.h"

void CTaskComplexSeekEntityAiming::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSeekEntityAiming, 0x870c94, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x694B90);
    RH_ScopedInstall(Destructor, 0x694C10);

    RH_ScopedVMTInstall(Clone, 0x695820);
    RH_ScopedVMTInstall(GetTaskType, 0x694C00);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x695C20);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x695D30);
    RH_ScopedVMTInstall(ControlSubTask, 0x694C70);
}

// 0x694B90
CTaskComplexSeekEntityAiming::CTaskComplexSeekEntityAiming(CEntity* toSeek, float seekRadius, float aimRadius) :
    m_ToSeek{toSeek},
    m_SeekRadius{seekRadius},
    m_AimRadius{aimRadius}
{
    CEntity::SafeRegisterRef(m_ToSeek);
}

// 0x695820
// Como el original: la copia recibe el radio de búsqueda (+0x10) y el de apuntado (+0x14)
CTaskComplexSeekEntityAiming::CTaskComplexSeekEntityAiming(const CTaskComplexSeekEntityAiming& o) :
    CTaskComplexSeekEntityAiming{o.m_ToSeek, o.m_SeekRadius, o.m_AimRadius}
{
}

// 0x694C10
CTaskComplexSeekEntityAiming::~CTaskComplexSeekEntityAiming() {
    CEntity::SafeCleanUpRef(m_ToSeek);
}

// 0x695C20
CTask* CTaskComplexSeekEntityAiming::CreateNextSubTask(CPed* ped) {
    // Como el original: primero el tipo de la subtarea; tipos desconocidos devuelven nullptr
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL:
        if (!m_ToSeek) {
            return nullptr;
        }
        return new CTaskComplexSeekEntity<>{
            m_ToSeek,
            50'000,
            1'000,
            m_SeekRadius,
            2.f,
            2.f,
            true,
            true
        };
    case TASK_COMPLEX_SEEK_ENTITY:
        if (!m_ToSeek) {
            return nullptr;
        }
        return new CTaskSimpleStandStill{2'000};
    default:
        return nullptr;
    }
}

// 0x695D30
CTask* CTaskComplexSeekEntityAiming::CreateFirstSubTask(CPed* ped) {
    if (!m_ToSeek) {
        return nullptr;
    }
    return new CTaskComplexSeekEntity<>{
        m_ToSeek,
        50'000,
        1'000,
        m_SeekRadius,
        2.f,
        2.f,
        true,
        true
    };
}

// 0x694C70
CTask* CTaskComplexSeekEntityAiming::ControlSubTask(CPed* ped) {
    const auto pedToSoughtDist2DSq = (ped->GetPosition() - m_ToSeek->GetPosition()).SquaredMagnitude2D();
    // Como el original: la tarea secundaria de ataque se usa solo si es TASK_SIMPLE_USE_GUN; si hay otra no se crea ninguna
    auto& taskMgr = ped->GetTaskManager();
    CTask* const attackTask = taskMgr.GetTaskSecondary(TASK_SECONDARY_ATTACK);
    const auto UseGun = [&]() -> CTaskSimpleUseGun* {
        return attackTask && attackTask->GetTaskType() == TASK_SIMPLE_USE_GUN ? static_cast<CTaskSimpleUseGun*>(attackTask) : nullptr;
    };
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL: {
        if (const auto pedUseGun = UseGun()) {
            pedUseGun->ControlGun(ped, m_ToSeek, eGunCommand::AIM);
        }
        if (pedToSoughtDist2DSq >= sq(m_SeekRadius)) {
            m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
        }
        break;
    }
    case TASK_COMPLEX_SEEK_ENTITY: {
        // Como el original: con NaN también se termina el apuntado
        if (!(pedToSoughtDist2DSq < sq(m_AimRadius))) {
            if (const auto pedUseGun = UseGun()) {
                pedUseGun->ControlGun(ped, m_ToSeek, eGunCommand::END_LEISURE);
            }
        } else if (!attackTask) {
            taskMgr.SetTaskSecondary(
                new CTaskSimpleUseGun{
                    m_ToSeek,
                    {},
                    eGunCommand::AIM,
                    1,
                    false
                },
                TASK_SECONDARY_ATTACK
            );
        } else if (const auto pedUseGun = UseGun()) {
            pedUseGun->ControlGun(ped, m_ToSeek, eGunCommand::AIM);
        }
        break;
    }
    default:
        break; // Como el original: otros tipos no hacen nada
    }
    return m_pSubTask;
}
