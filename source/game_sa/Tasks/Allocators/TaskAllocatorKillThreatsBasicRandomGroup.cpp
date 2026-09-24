/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskAllocatorKillThreatsBasicRandomGroup
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskAllocatorKillThreatsBasicRandomGroup.h"
#include "TaskSimpleNone.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexKillPedGroupOnFoot.h"
#include "TaskComplexSeekCoverUntilTargetDead.h"
#include "TaskSimpleLookAbout.h"

#include "InterestingEvents.h"

// 0x69D460
void CTaskAllocatorKillThreatsBasicRandomGroup::AllocateTasks(CPedGroupIntelligence* intel) {
    intel->FlushTasks(intel->GetPedTaskPairs(), nullptr);
    intel->FlushTasks(intel->GetSecondaryPedTaskPairs(), nullptr);

    if (!m_Threat) {
        return;
    }

    auto* const group = &intel->GetPedGroup();
    if (auto* const threatsGroup = m_Threat->GetGroup()) {
        if (threatsGroup == group) {
            NOTSA_LOG_DEBUG("ComputeKillThreatsBasicResponse() - threat ped already in group"); // vanilla
        } else {
            CPed* closest[TOTAL_PED_GROUP_MEMBERS]{};
            ComputeClosestPeds(*group, *threatsGroup, closest);
            for (int32 i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) { // 0x69D539
                auto* mem = group->GetMembership().GetMember(i);
                if (!mem || mem->IsPlayer()) {
                    continue;
                }
                if (!mem->GetActiveWeapon().IsTypeMelee() || m_Threat->GetActiveWeapon().IsTypeMelee()) {
                    intel->SetEventResponseTask(mem, CTaskComplexKillPedGroupOnFoot{ threatsGroup->GetId(), closest[i] });
                } else {
                    intel->SetEventResponseTask(mem, CTaskComplexSeekCoverUntilTargetDead{ threatsGroup->GetId() });
                }
            }
            g_InterestingEvents.Add(CInterestingEvents::GANG_FIGHT, group->GetMembership().GetLeader());
        }
    } else {
        for (auto* const mem : group->GetMembership().GetMembers()) {
            if (mem->IsPlayer()) {
                continue;
            }
            if (!mem->GetActiveWeapon().IsTypeMelee() || m_Threat->GetActiveWeapon().IsTypeMelee()) { // 0x69D6ED
                intel->SetEventResponseTask( // 0x69D864
                    mem,
                    CTaskComplexSequence{
                        new CTaskComplexKillPedOnFoot{ m_Threat },
                        new CTaskSimpleLookAbout{ CGeneral::GetRandomNumberInRange(1000u, 2000u) },
                    }
                );
            } else {
                intel->SetEventResponseTask(mem, CTaskComplexSeekCoverUntilTargetDead{ m_Threat }); // 0x69D71F
            }
        }
        g_InterestingEvents.Add(CInterestingEvents::GANG_ATTACKING_PED, group->GetMembership().GetLeader());
    }
}

void CTaskAllocatorKillThreatsBasicRandomGroup::InjectHooks() {
    RH_ScopedVirtualClass(CTaskAllocatorKillThreatsBasicRandomGroup, 0x86C80C, 6);
    RH_ScopedCategory("Tasks/Allocators");

    RH_ScopedInstall(GetType, 0x5F68F0);
    RH_ScopedInstall(AllocateTasks, 0x69D460);
}
