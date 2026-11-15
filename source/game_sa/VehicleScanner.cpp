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
    RH_ScopedInstall(ScanForVehiclesInRange, 0x5FFE60); // lote 198A (sin validar)
}

// 0x5FFE60 (lote 198A, sin validar; thiscall, un argumento de pila (el peaton), `ret 4`, 113 bytes): sin vida (CPed::IsAlive 0x5E0170) no hace nada. Con vehiculo (+0x58C) Y en vehiculo (bit 8 de +0x46C): si el peaton es de mision (byte +0x484 == 2)
// primero CEntityScanner::ScanForEntitiesInRange (0x5FFA20, llamada directa, lista 0 = vehiculos); despues SIEMPRE cambia el vehiculo mas cercano (+0x4C): CleanUpOldReference del anterior si no es nulo, +0x4C = vehiculo del peaton y RegisterReference
// de la direccion de +0x4C. Si no (sin vehiculo o a pie) solo el escaneo de la lista 0 (que incluye la limpieza del escaner). No devuelve nada.
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
