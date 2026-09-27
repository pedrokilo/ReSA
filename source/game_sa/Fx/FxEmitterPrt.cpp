/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxEmitterPrt_c
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

#include "FxEmitterPrt.h"
#include "FxManager.h"

// 0x4A18B0
// Los arrays de partículas se reservan linealmente dentro del pool de memoria FX.
void* FxEmitterPrt_c::operator new[](size_t size) {
    return g_fxMan.GetMemPool().GetMem(size);
}

/**
 * Registra la ruta de asignación de arrays de partículas de emisor.
 */
void FxEmitterPrt_c::InjectHooks() {
    RH_ScopedClass(FxEmitterPrt_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(operator new[], 0x4A18B0);
}
