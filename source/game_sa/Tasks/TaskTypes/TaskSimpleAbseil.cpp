/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleAbseil
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

#include "TaskSimpleAbseil.h"

namespace {
// Cadena "Abseil" del EXE (0x86F484): el constructor de CTaskSimpleRunAnim la ignora, pero el EXE empuja esta direccion
const char* const kExeAbseilName = reinterpret_cast<const char*>(0x86F484);
} // namespace

void CTaskSimpleAbseil::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleAbseil, 0x86F45C, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6586A0); // lote 180A (banco Fn180A)
    RH_ScopedVMTInstall(Clone, 0x6586D0);    // lote 180A (banco Fn180A)
}

// 0x6586A0 (lote 180A, banco Fn180A; el listado decia CTaskComplexWalkRoundFire; thiscall, sin argumentos de pila, `ret`, 42 bytes): constructor del EXE. Como el original: llama al constructor de CTaskSimpleRunAnim (0x61A900, con
// hook) con (ANIM_GROUP_DEFAULT = 0, ANIM_ID_ABSEIL = 0x98, 4.0f, TASK_SIMPLE_ABSEIL = 0x1A7, "Abseil" (0x86F484), false), deja la vtable de la clase del EXE (0x86F45C) y devuelve `this`.
CTaskSimpleAbseil* CTaskSimpleAbseil::Constructor() {
    CTaskSimpleRunAnim::Constructor2(ANIM_GROUP_DEFAULT, ANIM_ID_ABSEIL, 4.0f, TASK_SIMPLE_ABSEIL, kExeAbseilName, false);
    *reinterpret_cast<uint32*>(this) = 0x86F45C;
    return this;
}

// 0x6586D0 (lote 180A, banco Fn180A; ranura 1 de la vtable 0x86F45C; thiscall, sin argumentos de pila, `ret`, 102 bytes con marco SEH que solo libera la memoria si el constructor lanza y aqui se omite): Clone. Como el original:
// reserva 0x20 bytes con CTask::operator new (0x61A5A0, con hook); sin memoria devuelve nulo; si no, el mismo constructor en linea (0x61A900 con los mismos argumentos y la vtable del EXE 0x86F45C) y devuelve el objeto reservado.
CTask* CTaskSimpleAbseil::Clone() const {
    void* const memory = CTask::operator new(sizeof(CTaskSimpleAbseil));
    if (!memory) {
        return nullptr;
    }
    auto* const task = static_cast<CTaskSimpleAbseil*>(memory);
    task->CTaskSimpleRunAnim::Constructor2(ANIM_GROUP_DEFAULT, ANIM_ID_ABSEIL, 4.0f, TASK_SIMPLE_ABSEIL, kExeAbseilName, false);
    *reinterpret_cast<uint32*>(task) = 0x86F45C;
    return task;
}
