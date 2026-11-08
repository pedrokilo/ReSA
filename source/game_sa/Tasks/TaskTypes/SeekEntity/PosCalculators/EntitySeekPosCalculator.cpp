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
}
