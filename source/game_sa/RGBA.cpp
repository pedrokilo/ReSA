/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CRGBA
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "RGBA.h"

namespace {
// 0x463370 (lote 129A, banco Fn129A): copia de un CRGBA del EXE (el listado de pendientes decia CRoadBlocks): thiscall, un argumento de pila, `ret 4`, 31 bytes. Copia los cuatro
// bytes de uno en uno y en orden (carga y escritura alternadas; el origen puede solaparse con el destino) y devuelve this en EAX. Lo llaman CFont::ParseToken (0x718F4E, 0x719057)
// y cuatro llamadores mas del EXE. Los accesos volatiles impiden que el compilador los funda en una sola copia de 32 bits.
CRGBA* __fastcall CRGBACopyExe(CRGBA* self, void* /*edx*/, const CRGBA* source) {
    volatile uint8* const to = reinterpret_cast<volatile uint8*>(self);
    const volatile uint8* const from = reinterpret_cast<const volatile uint8*>(source);
    to[0] = from[0];
    to[1] = from[1];
    to[2] = from[2];
    to[3] = from[3];
    return self;
}
} // namespace

void CRGBA::InjectHooks() {
    RH_ScopedClass(CRGBA);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(Constructor, 0x7170C0);
    RH_ScopedNamedGlobalInstall(CRGBACopyExe, "Copy", 0x463370);
}

// 0x7170C0 (thiscall, `ret 10h`): CRGBA::CRGBA(r, g, b, a); escribe los cuatro bytes y devuelve this en EAX (ECX y EDX son de trabajo en el EXE).
CRGBA* CRGBA::Constructor(uint8 r, uint8 g, uint8 b, uint8 a) {
    this->CRGBA::CRGBA(r, g, b, a);
    return this;
}
