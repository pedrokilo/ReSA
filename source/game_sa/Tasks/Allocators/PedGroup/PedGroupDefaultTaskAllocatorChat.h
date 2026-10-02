/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedGroupDefaultTaskAllocatorChat
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
#include "TaskComplexSequence.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimpleRunTimedAnim.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleChat.h"

class NOTSA_EXPORT_VTABLE CPedGroupDefaultTaskAllocatorChat final : public CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    // 0x5F6500
    ePedGroupDefaultTaskAllocatorType GetType() const override { return ePedGroupDefaultTaskAllocatorType::CHAT; };

    // 0x5F8180
    void AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override {
        auto& membership = pedGroup->GetMembership();

        // Como el original: índices de los miembros presentes (incluido el líder, índice 7), en orden.
        int32 memberIds[TOTAL_PED_GROUP_MEMBERS];
        int32 numMembers = 0;
        for (int32 i = 0; i < TOTAL_PED_GROUP_MEMBERS; ++i) {
            if (membership.GetMember(i)) {
                memberIds[numMembers++] = i;
            }
        }

        // Como el original: una secuencia en la pila por hueco del grupo (se indexa por id de miembro).
        CTaskComplexSequence seqs[TOTAL_PED_GROUP_MEMBERS];
        for (int32 j = 0; j < numMembers; ++j) {
            const auto id   = memberIds[j];
            auto* const mem = membership.GetMember(id);
            seqs[id].AddTask(new CTaskComplexGoToPointAndStandStill{ PEDMOVE_WALK, mem->GetPosition(), 0.5f, 2.f, false, false }); // 0x5F82B8
            seqs[id].AddTask(new CTaskSimpleAchieveHeading{ mem->m_fCurrentRotation, 0.5f, 0.2f });                             // 0x5F8311
            seqs[id].SetRepeatMode(true);                                                                                      // 0x5F8338
        }

        // Como el original: tantas rondas como miembros; en cada una un miembro al azar charla y el resto espera.
        // Las tareas que no caben en la secuencia (8 como máximo) las borra AddTask.
        if (numMembers > 0) {
            const float fNumMembers = (float)numMembers;
            for (int32 round = numMembers; round; --round) {
                const auto chatter = (int32)(fNumMembers * ((float)CGeneral::GetRandomNumber() * (1.f / 32768.f)));
                const auto time    = 3000 - (int32)((float)CGeneral::GetRandomNumber() * (1.f / 32768.f) * -3000.f);
                for (int32 j = 0; j < numMembers; ++j) {
                    if (chatter == j) {
                        seqs[memberIds[j]].AddTask(new CTaskSimpleChat{ (uint32)time });
                    } else {
                        seqs[memberIds[j]].AddTask(new CTaskSimpleStandStill{ time, false, false, 8.f });
                    }
                }
            }
        }

        // Como el original: cada par recibe un clon de la secuencia de su hueco (si no se pidió un peatón concreto o es él).
        auto& pairs = pedGroup->GetIntelligence().GetDefaultPedTaskPairs();
        for (int32 j = 0; j < numMembers; ++j) {
            const auto id = memberIds[j];
            if (!ped || pairs[id].Ped == ped) {
                pairs[id].Task = seqs[id].Clone();
            }
        }
    };

public:
    static inline void InjectHooks() {
        RH_ScopedVirtualClass(CPedGroupDefaultTaskAllocatorChat, 0x86C774, 2);
        RH_ScopedCategory("Tasks/Allocators/PedGroup");

        RH_ScopedVMTInstall(AllocateDefaultTasks, 0x5F8180);
        RH_ScopedVMTInstall(GetType, 0x5F6500);
    }
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocatorChat, sizeof(void*)); /* vtable only */
