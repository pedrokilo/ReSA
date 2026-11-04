/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CRGBA
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

#include "RGBA.h"

void CRGBA::InjectHooks() {
    RH_ScopedClass(CRGBA);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(Constructor, 0x7170C0);
}

// 0x7170C0 (thiscall, `ret 10h`): CRGBA::CRGBA(r, g, b, a); escribe los cuatro bytes y devuelve this en EAX (ECX y EDX son de trabajo en el EXE).
CRGBA* CRGBA::Constructor(uint8 r, uint8 g, uint8 b, uint8 a) {
    this->CRGBA::CRGBA(r, g, b, a);
    return this;
}
