/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedGroupDefaultTaskAllocatorSitInLeaderCar
 * Reconstrucción: INCOMPLETA — 1/2 funciones reconstruidas.
 * Pendientes de reconstruir: 1.
 * Funciones bloqueadas: 1.
 * Evidencia aprobada vinculada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   AllocateDefaultTasks | 0x00000000 | V
 * BLOQUEADAS (nombre | dirección | tipo):
 *   AllocateDefaultTasks | 0x00000000 | V
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
        const auto SetPedDefaultTask = [&](CPed* p, int32 i, CTask* task) {
            auto* const tp = &pedGroup->GetIntelligence().GetDefaultPedTaskPairs()[i];
            if (tp->Ped && (!ped || tp->Ped == ped)) {
                VERIFY(std::exchange(tp->Task, task) == nullptr);
            } else {
                delete task;
            }
        };
        SetPedDefaultTask(leader, CPedGroupMembership::LEADER_MEM_ID, new CTaskComplexSequence{
            new CTaskComplexEnterCarAsDriver{veh}, // 0x5F703E
            new CTaskSimpleCarDrive{veh} // 0x5F7074
        });
        size_t seat{};
        for (auto&& [i, mem] : rngv::enumerate(pedGroup->GetMembership().GetFollowers())) {
            if (seat >= veh->m_nMaxPassengers) {
                break;
            }
            SetPedDefaultTask(leader, CPedGroupMembership::LEADER_MEM_ID, new CTaskComplexSequence{
                new CTaskComplexEnterCarAsPassenger{veh, CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(veh, seat++)}, // 0x5F714B
                new CTaskSimpleCarDrive{veh} // 0x5F7184
            });
        }
    };

public:
    static inline void InjectHooks() {
        RH_ScopedVirtualClass(CPedGroupDefaultTaskAllocatorSitInLeaderCar, 0x86C784, 2);
        RH_ScopedCategory("Tasks/Allocators/PedGroup");

        RH_ScopedVMTInstall(AllocateDefaultTasks, 0x5F6FC0, { .reversed = false });
        RH_ScopedVMTInstall(GetType, 0x5F6560);
    }
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocatorSitInLeaderCar, sizeof(void*)); /* vtable only */
