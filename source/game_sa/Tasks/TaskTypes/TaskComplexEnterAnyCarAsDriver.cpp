/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexEnterAnyCarAsDriver
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexEnterAnyCarAsDriver.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskSimpleNone.h"

void CTaskComplexEnterAnyCarAsDriver::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEnterAnyCarAsDriver, 0x86EA14, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(Clone, 0x63DE60);
    RH_ScopedVMTInstall(GetTaskType, 0x63D0F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x63D110);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x643510);
    RH_ScopedVMTInstall(ControlSubTask, 0x63D120);
}

// 0x643510
CTask* CTaskComplexEnterAnyCarAsDriver::CreateFirstSubTask(CPed* ped) {
    // Try entering the closest vehicle (If it's stealable)
    if (const auto closestVeh = ped->GetIntelligence()->GetVehicleScanner().GetClosestVehicleInRange()) {
        if (CCarEnterExit::IsVehicleStealable(closestVeh, ped)) {
            return new CTaskComplexEnterCarAsDriver{ closestVeh };
        }
    }

    // Otherwise just enter the closest vehicle (Not sure why they didn't use `GetClosestVehicleInRange`?)
    auto closestDistSq = FLT_MAX;
    CVehicle* closest{};
    for (auto& vehicle : ped->GetIntelligence()->GetVehicleScanner().GetEntities<CVehicle>()) {
        const auto distSq = (vehicle.GetPosition() - ped->GetPosition()).SquaredMagnitude();
        if (distSq < closestDistSq) {
            closestDistSq = distSq;
            closest = vehicle.AsVehicle();
        }
    }

    if (closest) {
        return new CTaskComplexEnterCarAsDriver{ closest };
    }
    return new CTaskSimpleNone{};
}
