/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexInWater
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexInWater.h"
#include "TaskSimpleSwim.h"
#include "TaskSimpleClimb.h"
#include "TaskComplexDie.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleRunAnim.h"

void CTaskComplexInWater::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexInWater, 0x86e4ec, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6350D0);
    RH_ScopedInstall(Destructor, 0x635100);

    RH_ScopedVMTInstall(Clone, 0x6370B0);
    RH_ScopedVMTInstall(GetTaskType, 0x6350F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x635110);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x635200);
    RH_ScopedVMTInstall(ControlSubTask, 0x635420);
}

CTaskComplexInWater::CTaskComplexInWater(const CTaskComplexInWater& o) :
    CTaskComplexInWater{}
{
}

// 0x635110
CTask* CTaskComplexInWater::CreateNextSubTask(CPed* ped) {
    if (const auto swim = notsa::dyn_cast_if_present<CTaskSimpleSwim>(m_pSubTask); swim && swim->m_pEntity) {
        ped->bIsInTheAir = true;
        return new CTaskSimpleClimb{
            swim->m_pEntity,
            swim->m_pClimbPos,
            swim->m_fAngle,
            swim->m_nSurfaceType,
            (swim->m_pClimbPos.z - ped->GetPosition().z < CTaskSimpleClimb::ms_fMinForStretchGrab)
                ? eClimbHeights::CLIMB_PULLUP
                : eClimbHeights::CLIMB_GRAB,
            false
        };
    }
    return nullptr;
}

// 0x635200
CTask* CTaskComplexInWater::CreateFirstSubTask(CPed* ped) {
    if (ped->IsPlayer() || CPopulation::IsSunbather(ped)) {
        return new CTaskSimpleSwim{ nullptr, nullptr };
    }

    if (!ped->bDrownsInWater) {
        return new CTaskComplexDie{
            WEAPON_DROWNING,
            ANIM_GROUP_DEFAULT,
            ANIM_ID_DROWN,
        };
    }

    if (const auto pedsGrp = ped->GetGroup()) {
        if (const auto leader = pedsGrp->GetMembership().GetLeader()) {
            assert(!!leader->GetPlayerData() == leader->IsPlayer()); // Make sure this way is the same as the old
            if (leader->IsPlayer()) {
                return new CTaskSimpleSwim{ nullptr, leader };
            }
        }
    }

    if (ped->IsCreatedByMission()) {
        if (const auto goTo = ped->GetTaskManager().Find<CTaskSimpleGoToPoint>()) {
            return new CTaskSimpleSwim{ &goTo->m_vecTargetPoint, nullptr };
        }
    }

    return new CTaskSimpleRunAnim{ ANIM_GROUP_DEFAULT, ANIM_ID_SWIM_TREAD, 8.f };
}
