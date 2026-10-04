/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedGroupDefaultTaskAllocatorFollowAnyMeans
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

#include "TaskComplexWanderGang.h"
#include "TaskComplexFollowLeaderAnyMeans.h"
#include "TaskComplexFollowLeaderInFormation.h"

class NOTSA_EXPORT_VTABLE CPedGroupDefaultTaskAllocatorFollowAnyMeans final : public CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    // 0x5F6470
    ePedGroupDefaultTaskAllocatorType GetType() const override { return ePedGroupDefaultTaskAllocatorType::FOLLOW_ANY_MEANS; };

    // 0x5F6B40
    void AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override {
        auto& pairs = pedGroup->GetIntelligence().GetDefaultPedTaskPairs();

        // Como el original: los seguidores (pares 0..6) siguen al líder en formación.
        for (int32 i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; ++i) {
            auto& tp = pairs[i];
            if (!tp.Ped || (ped && tp.Ped != ped)) {
                continue;
            }
            tp.Task = new CTaskComplexFollowLeaderAnyMeans{
                pedGroup,
                pedGroup->GetMembership().GetLeader(),
                CVector{CTaskComplexFollowLeaderInFormation::ms_offsets.Offsets[i]} // Como el original salvo z: el EXE pasa un CVector cuya z queda sin escribir en la pila
            };
        }

        // Como el original: el par del líder (7) deambula en grupo.
        auto& leaderPair = pairs[CPedGroupMembership::LEADER_MEM_ID];
        if (leaderPair.Ped && (!ped || leaderPair.Ped == ped)) {
            leaderPair.Task = new CTaskComplexWanderGang{
                PEDMOVE_WALK,
                (uint8)CGeneral::GetRandomIntInRange(0, 8), // Como el original: trunc(rand * (1/32768) * 8)
                5'000,
                true,
                0.5f
            };
        }
    };

public:
    static inline void InjectHooks() {
        RH_ScopedVirtualClass(CPedGroupDefaultTaskAllocatorFollowAnyMeans, 0x86C75C, 2);
        RH_ScopedCategory("Tasks/Allocators/PedGroup");

        RH_ScopedVMTInstall(AllocateDefaultTasks, 0x5F6B40);
        RH_ScopedVMTInstall(GetType, 0x5F6470);
    }
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocatorFollowAnyMeans, sizeof(void*)); /* vtable only */
