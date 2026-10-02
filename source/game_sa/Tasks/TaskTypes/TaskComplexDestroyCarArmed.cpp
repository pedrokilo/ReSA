/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexDestroyCarArmed
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexDestroyCarArmed.h"
#include "TaskSimplePause.h"
#include "TaskSimpleThrowControl.h"
#include "TaskSimpleGunControl.h"
#include "SeekEntity/TaskComplexSeekEntityStandard.h"
#include <TaskComplexGoToPointAndStandStill.h>

void CTaskComplexDestroyCarArmed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDestroyCarArmed, 0x86d9c4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x621F50);
    RH_ScopedInstall(Destructor, 0x622010);

    RH_ScopedInstall(CalculateSearchPositionAndRanges, 0x628C80);
    RH_ScopedInstall(CreateSubTask, 0x628DA0);

    RH_ScopedVMTInstall(Clone, 0x623600);
    RH_ScopedVMTInstall(GetTaskType, 0x622000);
    RH_ScopedVMTInstall(MakeAbortable, 0x622070);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62DF20);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62E0A0);
    RH_ScopedVMTInstall(ControlSubTask, 0x628FA0);
}

// 0x621F50
CTaskComplexDestroyCarArmed::CTaskComplexDestroyCarArmed(CVehicle* vehicleToDestroy, uint32 a3, uint32 a4, uint32 a5) : 
    ctor58{ a3 },
    ctor5C{ a4 },
    ctor60{ a5 },
    m_VehToDestroy{vehicleToDestroy},
    m_bGotoPointAndStandStill{false}
{
    CEntity::SafeRegisterRef(m_VehToDestroy);
}

CTaskComplexDestroyCarArmed::CTaskComplexDestroyCarArmed(const CTaskComplexDestroyCarArmed& o) :
    CTaskComplexDestroyCarArmed{o.m_VehToDestroy, o.ctor58, o.ctor5C, o.ctor60}
{
}

// 0x622010
CTaskComplexDestroyCarArmed::~CTaskComplexDestroyCarArmed() {
    CEntity::SafeCleanUpRef(m_VehToDestroy);
}

// 0x628C80
void CTaskComplexDestroyCarArmed::CalculateSearchPositionAndRanges(CPed* ped) {
    m_PedPos = ped->GetPosition();
    m_VehiclePos = m_VehToDestroy->GetPosition();
    m_PedToVehDirUnnorm = m_VehiclePos - m_PedPos;
    const auto* delta = &m_PedToVehDirUnnorm;
    float distance;
    __asm {
        mov eax, delta
        fld dword ptr [eax]
        fmul st, st(0)
        fld dword ptr [eax + 4]
        fmul st, st(0)
        faddp st(1), st
        fld dword ptr [eax + 8]
        fmul st, st(0)
        faddp st(1), st
        fsqrt
        fstp distance
    }
    m_PedVehDist = distance;

    const auto& weapon = ped->GetActiveWeapon();
    const auto& winfo  = weapon.GetWeaponInfo(ped);
    const float range = winfo.m_fWeaponRange;
    const float half = .5f, inner = .9f, outer = 1.1f;
    float halfRange, innerRange, outerRange;
    __asm {
        fld range
        fmul half
        fst halfRange
        fmul inner
        fstp innerRange
        fld halfRange
        fmul outer
        fstp outerRange
    }
    m_fWeaponRangeClamped = std::max(.8f, innerRange);
    m_fWeaponRange = outerRange;
}

// 0x628DA0
CTask* CTaskComplexDestroyCarArmed::CreateSubTask(eTaskType taskType, CPed* ped) {
    const auto create = []<typename T, typename... Args>(Args&&... args) -> T* {
        auto* memory = CTask::operator new(sizeof(T));
        return memory ? ::new (memory) T(std::forward<Args>(args)...) : nullptr;
    };
    switch (taskType) {
    case TASK_SIMPLE_GUN_CTRL:
        if (ped->GetActiveWeapon().GetWeaponInfo(ped).flags.bThrow) {
            return create.operator()<CTaskSimpleThrowControl>(m_VehToDestroy, nullptr);
        }
        return create.operator()<CTaskSimpleGunControl>(m_VehToDestroy, CVector{}, CVector{}, eGunCommand::FIREBURST, int16{5}, 5000);
    case TASK_COMPLEX_SEEK_ENTITY:
        return create.operator()<CTaskComplexSeekEntityStandard>(m_VehToDestroy, 50000, 1000, 1.f, 2.f, 2.f, true, true);
    case TASK_SIMPLE_PAUSE:
        return create.operator()<CTaskSimplePause>(100);
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        return create.operator()<CTaskComplexGoToPointAndStandStill>(PEDMOVE_RUN, m_VehiclePos, .5f, 2.f, false, false);
    default:
        return nullptr;
    }
}

// 0x622070
bool CTaskComplexDestroyCarArmed::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    switch (priority) {
    case ABORT_PRIORITY_URGENT:
    case ABORT_PRIORITY_IMMEDIATE:
        return m_pSubTask->MakeAbortable(ped, priority, event);
    default:
        return false;
    }
}

// 0x62DF20
CTask* CTaskComplexDestroyCarArmed::CreateNextSubTask(CPed* ped) {
    if (!m_VehToDestroy) {
        return nullptr;
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_GUN_CTRL:
    case TASK_SIMPLE_THROW_CTRL: {
        if (!ped->bStayInSamePlace) {
            CalculateSearchPositionAndRanges(ped);
            return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
        }
        return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
    }
    case TASK_COMPLEX_SEEK_ENTITY:
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
        return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
    }
    case TASK_SIMPLE_PAUSE: {
        if (!ped->bStayInSamePlace) {
            CalculateSearchPositionAndRanges(ped);
            return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
        }

        const auto& winfo = ped->GetActiveWeapon().GetWeaponInfo(ped);
        if (DistanceBetweenPointsSquared(m_VehToDestroy->GetPosition(), ped->GetPosition()) >= sq(winfo.m_fTargetRange)) {
            return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
        }
        return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
    }
    default:
        return nullptr;
    }
}

// 0x62E0A0
CTask* CTaskComplexDestroyCarArmed::CreateFirstSubTask(CPed* ped) {
    if (!m_VehToDestroy) {
        return nullptr;
    }

    m_bNeedsToCreatefirstSubTask = false;

    // Dead code:
    // const auto& winfo = ped->GetActiveWeapon().GetWeaponInfo(ped);
    CalculateSearchPositionAndRanges(ped);

    if (m_PedVehDist <= m_fWeaponRangeClamped) {
        ped->m_fAimingRotation = CGeneral::GetRadianAngleBetweenPoints(m_VehiclePos, m_PedPos);
        return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
    } else if (ped->bStayInSamePlace) {
        return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
    } else if (m_bGotoPointAndStandStill) {
        return CreateSubTask(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL, ped);
    } else {
        return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
    }
}

// 0x628FA0
CTask* CTaskComplexDestroyCarArmed::ControlSubTask(CPed* ped) {
    if (!m_VehToDestroy) {
        return nullptr;
    }

    if (m_bNeedsToCreatefirstSubTask) {
        return CreateFirstSubTask(ped);
    }

    if (m_VehToDestroy->m_fHealth <= 0.f) {
        return nullptr;
    }

    // TODO: Refactor (Most of the code is common, just find a way to get rid of the jump)
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_SEEK_ENTITY: {
        CalculateSearchPositionAndRanges(ped);
        if (m_PedVehDist >= m_fWeaponRange && !ped->bKindaStayInSamePlace) {
            return m_pSubTask;
        }
        ped->SetMoveState(PEDMOVE_STILL);
        return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
    }
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL: {
        CalculateSearchPositionAndRanges(ped);
        if (m_PedVehDist < dword4C) {
            ped->SetMoveState(PEDMOVE_STILL);
            return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
        }
        if (m_PedVehDist >= m_fWeaponRange && !ped->bKindaStayInSamePlace) {
            return m_pSubTask;
        }
        ped->SetMoveState(PEDMOVE_STILL);
        return CreateSubTask(TASK_SIMPLE_GUN_CTRL, ped);
    }
    default: {
        return m_pSubTask;
    }
    }
}
