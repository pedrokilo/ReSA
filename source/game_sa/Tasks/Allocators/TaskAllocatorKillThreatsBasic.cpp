/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskAllocatorKillThreatsBasic
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskAllocatorKillThreatsBasic.h"
#include <InterestingEvents.h>
#include <TaskComplexKillPedGroupOnFoot.h>
#include <TaskComplexSequence.h>
#include <TaskSimpleLookAbout.h>

// 0x69C710
CTaskAllocatorKillThreatsBasic::CTaskAllocatorKillThreatsBasic(CPed* threat) :
    m_Threat{threat}
{
}

// 0x69D170
void CTaskAllocatorKillThreatsBasic::AllocateTasks(CPedGroupIntelligence* intel) {
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
            for (int32 i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
                auto* mem = group->GetMembership().GetMember(i);
                if (!mem || mem->IsPlayer()) {
                    continue;
                }
                intel->SetEventResponseTask(
                    mem,
                    CTaskComplexKillPedGroupOnFoot{ CPedGroups::GetGroupId(threatsGroup), closest[i] }
                );
            }
            g_InterestingEvents.Add(CInterestingEvents::GANG_FIGHT, group->GetMembership().GetLeader()); // 0x69D436
        }
    } else { // 0x69D2EB
        for (auto* const mem : group->GetMembership().GetMembers()) {
            if (mem->IsPlayer()) {
                continue;
            }
            intel->SetEventResponseTask(
                mem,
                CTaskComplexSequence{
                    new CTaskComplexKillPedOnFoot{ m_Threat },
                    new CTaskSimpleLookAbout{ CGeneral::GetRandomNumberInRange(1'000u, 2'000u) },
                }
            );
        }
        g_InterestingEvents.Add(CInterestingEvents::GANG_ATTACKING_PED, group->GetMembership().GetLeader()); // 0x69D436
    }
}

// 0x69C7E0
CTaskAllocator* CTaskAllocatorKillThreatsBasic::ProcessGroup(CPedGroupIntelligence* intel) {
    m_Timer.StartIfNotAlready(0);
    if (m_Timer.IsOutOfTime()) {
        m_Timer.Start(5'000);
        AllocateTasks(intel);
    }
    return this;
}

// Preserve x87 intermediate precision and the original (z*z + x*x) + y*y order.
static float ClosestThreatDistance(const CVector& a, const CVector& b) {
    float result;
    __asm {
        mov eax, a
        mov ecx, b
        fld dword ptr [eax]
        fsub dword ptr [ecx]
        fld dword ptr [eax + 4]
        fsub dword ptr [ecx + 4]
        fld dword ptr [eax + 8]
        fsub dword ptr [ecx + 8]
        fld st(0)
        fmul st(0), st(1)
        fld st(3)
        fmul st(0), st(4)
        faddp st(1), st(0)
        fld st(2)
        fmul st(0), st(3)
        faddp st(1), st(0)
        fstp result
        fstp st(0)
        fstp st(0)
        fstp st(0)
    }
    return result;
}

// 0x69C850
void CTaskAllocatorKillThreatsBasic::ComputeClosestPeds(CPedGroup& group1, CPedGroup& group2, CPed** peds) {
    constexpr auto count = TOTAL_PED_GROUP_MEMBERS;
    constexpr auto unused = std::numeric_limits<float>::max();
    float distances[count][count];
    std::fill_n(peds, count, nullptr);
    for (auto& row : distances) {
        std::fill_n(row, count, unused);
    }
    auto& members = group1.GetMembership();
    auto& threats = group2.GetMembership();
    for (int32 i = 0; i < count; ++i) {
        auto* member = members.GetMember(i);
        if (!member || !member->IsAlive() || member->IsPlayer()) {
            continue;
        }
        for (int32 j = 0; j < count; ++j) {
            auto* threat = threats.GetMember(j);
            if (threat && threat->IsAlive()) {
                distances[i][j] = ClosestThreatDistance(member->GetPosition(), threat->GetPosition());
            }
        }
    }
    for (int32 pair = 0; pair < count; ++pair) {
        float nearest = unused;
        int32 member = -1, threat = -1;
        for (int32 i = 0; i < count; ++i) {
            for (int32 j = 0; j < count; ++j) {
                if (distances[i][j] < nearest) {
                    nearest = distances[i][j];
                    member = i;
                    threat = j;
                }
            }
        }
        if (member >= 0 && threat >= 0) {
            for (int32 i = 0; i < count; ++i) {
                distances[member][i] = unused;
                distances[i][threat] = unused;
            }
            peds[member] = threats.GetMember(threat);
        }
    }
    auto* fallback = threats.GetLeader();
    if (!fallback || !fallback->IsAlive()) {
        fallback = nullptr;
        for (int32 i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; ++i) {
            auto* threat = threats.GetMember(i);
            if (threat && threat->IsAlive()) {
                fallback = threat;
                break;
            }
        }
    }
    if (fallback) {
        // The fallback also covers dead members and the player, as in the original.
        for (int32 i = 0; i < count; ++i) {
            if (members.GetMember(i) && !peds[i]) {
                peds[i] = fallback;
            }
        }
    }
}

void CTaskAllocatorKillThreatsBasic::InjectHooks() {
    RH_ScopedVirtualClass(CTaskAllocatorKillThreatsBasic, 0x870e90, 6);
    RH_ScopedCategory("Tasks/Allocators");

    RH_ScopedInstall(Constructor, 0x69C710);
    RH_ScopedInstall(Destructor, 0x69C780);

    RH_ScopedGlobalInstall(ComputeClosestPeds, 0x69C850);
    RH_ScopedVMTInstall(GetType, 0x69C770);
    RH_ScopedVMTInstall(AllocateTasks, 0x69D170);
    RH_ScopedVMTInstall(ProcessGroup, 0x69C7E0);
}
