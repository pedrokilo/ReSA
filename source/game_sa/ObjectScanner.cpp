/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CObjectScanner
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

#include "ObjectScanner.h"

void CObjectScanner::InjectHooks() {
    RH_ScopedClass(CObjectScanner);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ScanForObjectsInRange, 0x5FFF30); // lote 198A (sin validar)
}

// 0x5FFF30 (lote 198A, sin validar; thiscall, un argumento de pila (el peaton), `ret 4`, 55 bytes): con vehiculo (+0x58C no nulo) Y en vehiculo (bit 8 de +0x46C) no hace nada; si no, si el peaton esta vivo (CPed::IsAlive 0x5E0170, que se evalua
// DESPUES de las dos comprobaciones anteriores), CEntityScanner::ScanForEntitiesInRange (0x5FFA20, llamada directa) con la lista 2 (objetos).
void CObjectScanner::ScanForObjectsInRange(const CPed& ped) {
    if (ped.m_pVehicle && ped.bInVehicle)
        return;

    if (ped.IsAlive())
        CEntityScanner::ScanForEntitiesInRange(REPEATSECTOR_OBJECTS, ped);
}

CObject* CObjectScanner::GetClosestObjectInRange() const {
    return static_cast<CObject*>(m_pClosestEntityInRange);
}
