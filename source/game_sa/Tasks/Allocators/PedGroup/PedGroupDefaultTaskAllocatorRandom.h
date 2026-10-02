/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedGroupDefaultTaskAllocatorRandom
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#pragma once

#include <Base.h>
#include <PluginBase.h>
#include <reversiblehooks/ReversibleHooks.h>

#include "./PedGroupDefaultTaskAllocator.h"
#include "TaskComplexGangFollower.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexFollowLeaderInFormation.h"

class NOTSA_EXPORT_VTABLE CPedGroupDefaultTaskAllocatorRandom final : public CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    // 0x5F6E90
    void AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override {
        auto& pairs = pedGroup->GetIntelligence().GetDefaultPedTaskPairs();
        for (int32 i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; ++i) {
            auto& pair = pairs[i];
            if (!pair.Ped || (ped && pair.Ped != ped)) {
                continue;
            }
            auto* task = new CTaskComplexGangFollower{
                pedGroup,
                pedGroup->GetMembership().GetLeader(),
                static_cast<uint8>(i),
                CVector{CTaskComplexFollowLeaderInFormation::ms_offsets.Offsets[i]},
                10.f
            };
            pair.Task = task;
            task->SetCanEnterLeadersCar(pedGroup->m_bMembersEnterLeadersVehicle);
        }
        auto& leaderPair = pairs[CPedGroupMembership::LEADER_MEM_ID];
        if (leaderPair.Ped && (!ped || leaderPair.Ped == ped)) {
            leaderPair.Task = new CTaskComplexGangLeader{pedGroup};
        }
    }
    ePedGroupDefaultTaskAllocatorType GetType() const override { return ePedGroupDefaultTaskAllocatorType::RANDOM; }; // 0x5F6530

public:
    static inline void InjectHooks() {
        RH_ScopedVirtualClass(CPedGroupDefaultTaskAllocatorRandom, 0x86C77C, 2);
        RH_ScopedCategory("Tasks/Allocators/PedGroup");

        RH_ScopedVMTInstall(AllocateDefaultTasks, 0x5F6E90);
        RH_ScopedVMTInstall(GetType, 0x5F6530);
    }
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocatorRandom, sizeof(void*)); /* vtable only */
