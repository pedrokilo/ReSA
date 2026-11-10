/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEntitySeekPosCalculator
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

#include "EntitySeekPosCalculator.h"

namespace {
// Cuerpo no virtual de ~CEntitySeekPosCalculator del EXE (thiscall, sin argumentos de pila, `ret`, 7 bytes): solo deja la vtable de CEntitySeekPosCalculator del EXE (0x859DC4, una ranura: la pura). Lo llaman por salto directo los destructores de limpieza (SEH) de las tareas que llevan un calculador. (Lote 121A, banco Fn121A.)
void __fastcall EntitySeekPosCalculatorDestructorBody(CEntitySeekPosCalculator* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x859DC4;
}
} // namespace

void CEntitySeekPosCalculator::InjectHooks() {
    RH_ScopedClass(CEntitySeekPosCalculator);
    RH_ScopedCategory("Tasks/TaskTypes/SeekPosCalculators");

    RH_ScopedNamedGlobalInstall(EntitySeekPosCalculatorDestructorBody, "Destructor", 0x46A610);
    RH_ScopedNamedGlobalInstall(EntitySeekPosCalculatorDestructorBody, "DestructorCopy", 0x492F60); // lote 147A (banco Fn147A): segunda copia identica del cuerpo (`mov [ecx],0x859DC4; ret`, 7 bytes)
}
