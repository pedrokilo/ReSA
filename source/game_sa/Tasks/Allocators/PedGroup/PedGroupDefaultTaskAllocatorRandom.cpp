#include "StdInc.h"

#include "PedGroupDefaultTaskAllocatorRandom.h"

// 0x5F6510
// Constructor del EXE (thiscall, `mov eax, ecx ; mov [eax], vtable ; ret`, 9 bytes): deja la vtable de la clase del EXE (0x86C77C) y devuelve `this`. No llama a nada ni lee memoria. (Lote 149A, banco Fn149A.)
CPedGroupDefaultTaskAllocatorRandom* CPedGroupDefaultTaskAllocatorRandom::Constructor() {
    *reinterpret_cast<uint32*>(this) = 0x86C77C;
    return this;
}
