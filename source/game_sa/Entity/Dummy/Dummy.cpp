/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CDummy
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Dummy.h"

/**
 * Registra la validación de punteros para entidades CDummy.
 */
void CDummy::InjectHooks() {
    RH_ScopedVirtualClass(CDummy, 0x8638C0, 22);
    RH_ScopedCategory("Entity/Dummy");

    RH_ScopedGlobalInstall(IsDummyPointerValid, 0x532730);
}

// 0x5326E0
// Obtiene almacenamiento del pool específico de dummies.
void* CDummy::operator new(size_t size) {
    return GetDummyPool()->New();
}

// 0x5326F0
void CDummy::operator delete(void* obj) {
    GetDummyPool()->Delete(static_cast<CDummy*>(obj));
}

// 0x532540
CDummy::CDummy() : CEntity() {
    SetTypeDummy();
}

// Comprueba que el puntero corresponda a un slot actualmente válido del pool.
bool IsDummyPointerValid(CDummy* dummy) {
    return GetDummyPool()->IsObjectValid(dummy);
}
