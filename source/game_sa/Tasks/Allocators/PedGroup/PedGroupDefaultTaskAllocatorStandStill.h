/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedGroupDefaultTaskAllocatorStandStill
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/2 entradas.
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
#include "TaskComplexBeStill.h"

class NOTSA_EXPORT_VTABLE CPedGroupDefaultTaskAllocatorStandStill final : public CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    // 0x5F64D0
    ePedGroupDefaultTaskAllocatorType GetType() const override { return ePedGroupDefaultTaskAllocatorType::STAND_STILL; };

    // 0x5F6DA0
    void AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override {
        for (auto& tp : pedGroup->GetIntelligence().GetDefaultPedTaskPairs()) {
            if (!tp.Ped) {
                continue;
            }
            if (ped && tp.Ped != ped) {
                continue;
            }
            assert(!tp.Task);
            tp.Task = new CTaskComplexBeStill{}; // Skipping the unnecessary leader check because they both get the same task
            if (notsa::IsFixBugs()) {
                if (ped) {
                    break; // There are no duplicate entries in the array, so we can stop now
                }
            }
        }
    };

public:
    static inline void InjectHooks() {
        RH_ScopedVirtualClass(CPedGroupDefaultTaskAllocatorStandStill, 0x86C76C, 2);
        RH_ScopedCategory("Tasks/Allocators/PedGroup");

        RH_ScopedVMTInstall(AllocateDefaultTasks, 0x5F6DA0);
        RH_ScopedVMTInstall(GetType, 0x5F64D0);
    }
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocatorStandStill, sizeof(void*)); /* vtable only */
