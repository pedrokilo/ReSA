/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CKeyboardState
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

#include "KeyboardState.h"

void CKeyboardState::InjectHooks() {
    RH_ScopedClass(CKeyboardState);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Clear, 0x53F090);
}

// 0x53F090
void CKeyboardState::Clear() {
    const auto keepEnter = enter; // 0x53F090 clears every key except `enter` (offset 0x23C is skipped)
    *this = CKeyboardState{};
    enter = keepEnter;
}
