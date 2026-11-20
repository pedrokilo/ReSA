/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxInfo_c
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

#include "FxInfo.h"

namespace {
// 0x4A4940 (lote 230A, sin validar; thiscall, sin argumentos de pila, `ret`, 7 bytes: `mov dword ptr [ecx],85A7DCh; ret`): cuerpo del destructor de FxInfo_c del EXE (el listado lo atribuia a
// FxEmitterBP_c). Solo deja la vtable de la clase base del EXE (0x85A7DC; la misma que escriben los constructores de las derivadas, p. ej. 0x4A4B30); no toca ningun otro campo ni devuelve nada util.
__declspec(noinline) void __fastcall FxInfoDestructorBody(FxInfo_c* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85A7DC;
}
} // namespace

void FxInfo_c::InjectHooks() {
    RH_ScopedClass(FxInfo_c);
    RH_ScopedCategory("Fx");

    RH_ScopedNamedGlobalInstall(FxInfoDestructorBody, "DestructorBody", 0x4A4940); // lote 230A (sin validar)
}

FxInfo_c::~FxInfo_c() {
    // NOP
}
