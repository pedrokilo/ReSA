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
 * Validación registrada: 5/5 entradas.
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

    // Como el original: recorre las 16 entradas del escáner y solo acepta vehículos robables;
    // la distancia es (ped - veh) con la suma (z*z + y*y) + x*x del exe.
    auto closestDistSq = FLT_MAX;
    CVehicle* closest{};
    for (auto* const entity : ped->GetIntelligence()->GetVehicleScanner().m_apEntities) {
        if (!entity) {
            continue;
        }
        auto* const vehicle = entity->AsVehicle();
        const CVector& vehPos = vehicle->GetPosition();
        const CVector& pedPos = ped->GetPosition();
        const float dx = pedPos.x - vehPos.x;
        const float dy = pedPos.y - vehPos.y;
        const float dz = pedPos.z - vehPos.z;
        const float distSq = dz * dz + dy * dy + dx * dx;
        if (distSq < closestDistSq && CCarEnterExit::IsVehicleStealable(vehicle, ped)) {
            closestDistSq = distSq;
            closest = vehicle;
        }
    }

    if (closest) {
        return new CTaskComplexEnterCarAsDriver{ closest };
    }
    return new CTaskSimpleNone{};
}
