#include "StdInc.h"

#include "TaskSimpleAbseil.h"

void CTaskSimpleAbseil::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleAbseil, 0x86F45C, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    // 0x6586A0 (lote 180A, sin validar; el listado decia CTaskComplexWalkRoundFire; thiscall, sin argumentos de pila, `ret`, 42 bytes): constructor del EXE. Llama al constructor de CTaskSimpleRunAnim (0x61A900, con hook) con
    // (ANIM_GROUP_DEFAULT = 0, ANIM_ID_ABSEIL = 0x98, 4.0f, TASK_SIMPLE_ABSEIL = 0x1A7, "Abseil" (0x86F484, cadena que el constructor ignora), false), deja la vtable de la clase del EXE (0x86F45C) y devuelve `this`.
    RH_ScopedInstall(Constructor, 0x6586A0);
    // 0x6586D0 (lote 180A, sin validar; ranura 1 de la vtable 0x86F45C; thiscall, `ret`, 102 bytes con marco SEH que solo desenrolla y aqui se omite): Clone. Reserva 0x20 bytes en el pool de tareas (0x61A5A0); si no es nula,
    // construye lo mismo que el constructor anterior (en linea); nula -> nulo.
    RH_ScopedVMTInstall(Clone, 0x6586D0);
}
