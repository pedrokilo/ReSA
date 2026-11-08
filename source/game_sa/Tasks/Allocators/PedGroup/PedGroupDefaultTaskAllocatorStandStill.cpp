#include "StdInc.h"

#include "PedGroupDefaultTaskAllocatorStandStill.h"

// 0x5F64B0
// Constructor del EXE (thiscall, `mov eax, ecx ; mov [eax], vtable ; ret`, 9 bytes): deja la vtable de la clase del EXE (0x86C76C) y devuelve `this`. Lo llama el inicializador estatico del objeto global; no llama a nada ni lee memoria. (Lote 121A, banco Fn121A.)
CPedGroupDefaultTaskAllocatorStandStill* CPedGroupDefaultTaskAllocatorStandStill::Constructor() {
    *reinterpret_cast<uint32*>(this) = 0x86C76C;
    return this;
}
