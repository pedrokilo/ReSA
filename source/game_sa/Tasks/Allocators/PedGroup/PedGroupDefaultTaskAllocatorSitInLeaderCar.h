/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedGroupDefaultTaskAllocatorSitInLeaderCar
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
#include "CarEnterExit.h"

#include "./PedGroupDefaultTaskAllocator.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskSimpleCarDrive.h"

class NOTSA_EXPORT_VTABLE CPedGroupDefaultTaskAllocatorSitInLeaderCar final : public CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    // 0x5F6560
    ePedGroupDefaultTaskAllocatorType GetType() const override { return ePedGroupDefaultTaskAllocatorType::SIT_IN_LEADER_CAR; };

    // 0x5F6FC0
    void AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override {
        const auto leader = pedGroup->GetMembership().GetLeader();
        if (!leader) {
            return;
        }
        const auto veh = leader->m_pVehicle;
        if (!veh) {
            return;
        }
        const auto SetPedDefaultTask = [&](int32 i, CTask* task) {
            auto* const tp = &pedGroup->GetIntelligence().GetDefaultPedTaskPairs()[i];
            if (tp->Ped && (!ped || tp->Ped == ped)) {
                tp->Task = task;
            } else {
                delete task;
            }
        };
        auto* leaderTask = new CTaskComplexSequence{};
        leaderTask->AddTask(new CTaskComplexEnterCarAsDriver{veh});
        leaderTask->AddTask(new CTaskSimpleCarDrive{veh});
        SetPedDefaultTask(CPedGroupMembership::LEADER_MEM_ID, leaderTask);
        int32 seat{};
        const auto maxPassengers = veh->m_nMaxPassengers;
        for (int32 i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; ++i) {
            if (!pedGroup->GetMembership().GetMember(i) || seat >= maxPassengers) {
                continue;
            }
            const auto door = CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(veh, seat);
            auto* followerTask = new CTaskComplexSequence{};
            followerTask->AddTask(new CTaskComplexEnterCarAsPassenger{veh, door});
            followerTask->AddTask(new CTaskSimpleCarDrive{veh});
            SetPedDefaultTask(i, followerTask);
            ++seat;
        }
    };

public:
    static inline void InjectHooks() {
        RH_ScopedVirtualClass(CPedGroupDefaultTaskAllocatorSitInLeaderCar, 0x86C784, 2);
        RH_ScopedCategory("Tasks/Allocators/PedGroup");

        RH_ScopedVMTInstall(AllocateDefaultTasks, 0x5F6FC0);
        RH_ScopedVMTInstall(GetType, 0x5F6560);
    }
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocatorSitInLeaderCar, sizeof(void*)); /* vtable only */
