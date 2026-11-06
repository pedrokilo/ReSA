/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CVehicleScanner
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "VehicleScanner.h"

// Lote 109A (banco Fn109A): GetClosestVehicleInRange (0x5FFEE0; thiscall, devuelve el dword +0x4C del escaner en EAX).
void CVehicleScannerInjectHooksLote109A() {
    RH_ScopedClass(CVehicleScanner);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GetClosestVehicleInRange, 0x5FFEE0);
}

// 0x5FFE60
void CVehicleScanner::ScanForVehiclesInRange(const CPed& ped) {
    if (!ped.IsAlive())
        return;

    if (ped.m_pVehicle && ped.bInVehicle) {
        if (ped.IsCreatedByMission())
            CEntityScanner::ScanForEntitiesInRange(REPEATSECTOR_VEHICLES, ped);

        CEntity::SafeCleanUpRef(m_pClosestEntityInRange);
        m_pClosestEntityInRange = ped.m_pVehicle;
        CEntity::SafeRegisterRef(m_pClosestEntityInRange);
    } else {
        CEntityScanner::ScanForEntitiesInRange(REPEATSECTOR_VEHICLES, ped);
    }
}

CVehicle* CVehicleScanner::GetClosestVehicleInRange() const {
    return static_cast<CVehicle*>(m_pClosestEntityInRange);
}
