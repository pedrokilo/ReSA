/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskAllocatorKillOnFoot
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskAllocatorKillOnFoot.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleLookAbout.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexKillPedGroupOnFoot.h"

// 0x69BB80
CTaskAllocatorKillOnFoot::CTaskAllocatorKillOnFoot(CPed* ped, int32 groupId) :
    m_Target{ped},
    m_GroupTargetID{groupId}
{ }

// 0x69BCA0
void CTaskAllocatorKillOnFoot::AllocateTasks(CPedGroupIntelligence* intel) {
    auto* const group = &intel->GetPedGroup();

    CPedList members{};
    members.BuildListFromGroup_NoLeader(group->GetMembership());

    if (auto* const leader = group->GetMembership().GetLeader()) { // 0x69BD07
        if (leader->IsPlayer()) {
            members.RemovePedsThatDontListenToPlayer();
        }
    }

    const auto ProcessList = [&](CPedList& list, bool withGuns) {
        for (auto i = 0u; i < list.m_count; i++) {
            auto* const mem = list.m_peds[i];
            if (auto* const task = intel->GetTask(mem, intel->GetPedTaskPairs())) { // 0x69BE1B and 0x69C097
                // Como el original: la tabla de saltos tambien excluye TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH (0x404)
                switch (task->GetTaskType()) {
                case TASK_SIMPLE_GUN_CTRL:
                case TASK_SIMPLE_GANG_DRIVEBY:
                case TASK_COMPLEX_KILL_PED_ON_FOOT_TIMED:
                case TASK_COMPLEX_KILL_PED_ON_FOOT_STAND_STILL:
                case TASK_KILL_PED_ON_FOOT_WHILE_DUCKING:
                case TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH:
                case TASK_COMPLEX_KILL_PED_ON_FOOT_KINDA_STAND_STILL:
                case TASK_COMPLEX_ARREST_PED:
                case TASK_GROUP_KILL_THREATS_BASIC:
                    continue;
                }
            }

            CTaskComplexSequence tseq{};

            if (mem->IsInVehicle()) { // 0x69BEC4 and 0x69C109
                tseq.AddTask(new CTaskComplexLeaveCar{ mem->m_pVehicle, 0, 0, true, false });
            }

            if (m_GroupTargetID == -1) { // 0x69BEC9 and 0x69C185
                tseq.AddTask(new CTaskComplexKillPedOnFoot{ m_Target, -1, 0, 0, 0, withGuns ? 2u : 1u });
                if (withGuns) {
                    // Como el original: 1000 - (int)(rand() * (1/32768) * -1000), es decir [1000, 2000)
                    tseq.AddTask(new CTaskSimpleLookAbout{ (uint32)CGeneral::GetRandomIntInRange(1000, 2000) });
                }
            } else { // 0x69BED5 and 0x69C17C
                tseq.AddTask(new CTaskComplexKillPedGroupOnFoot{ m_GroupTargetID, m_Target });
            }

            if (withGuns) {
                // Como el original: SetEventResponseTask (0x5F8510) con una CTaskSimpleNone sin usar en la pila
                CTaskSimpleNone none{};
                intel->SetEventResponseTask(mem, true, tseq, false, none, TASK_SECONDARY_INVALID); // 0x69C01E
            } else {
                // Como el original: SetTask (0x5F7540) directo con los pares principales, slot -1 y sin forzar
                intel->SetTask(mem, tseq, intel->GetPedTaskPairs(), TASK_SECONDARY_INVALID, false); // 0x69C1DA
            }
        }
    };

    // 0x69BD31
    // Como el original: el global 0xC1A554 fuerza la rama de armas de fuego (sin comprobar el objetivo)
    if (StaticRef<bool>(0xC1A554) || (m_Target && !m_Target->GetActiveWeapon().IsTypeMelee())) {
        CPedList membersWithGuns{};
        membersWithGuns.ExtractPedsWithGuns(members);

        CFormation::FindCoverPoints(m_Target->GetPosition(), 30.f);
        CFormation::DistributeDestinations_CoverPoints(membersWithGuns, m_Target->GetPosition());

        ProcessList(membersWithGuns, true);
    }

    // 0x69C06A
    ProcessList(members, false);
}

void CTaskAllocatorKillOnFoot::InjectHooks() {
    RH_ScopedVirtualClass(CTaskAllocatorKillOnFoot, 0x870e48, 6);
    RH_ScopedCategory("Tasks/Allocators");

    RH_ScopedInstall(Constructor, 0x69BB80);
    RH_ScopedInstall(Destructor, 0x69BBF0);

    RH_ScopedVMTInstall(GetType, 0x69BBE0);
    RH_ScopedVMTInstall(AllocateTasks, 0x69BCA0);
}
