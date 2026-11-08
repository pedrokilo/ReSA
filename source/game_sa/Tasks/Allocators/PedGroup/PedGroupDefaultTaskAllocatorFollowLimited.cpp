#include "StdInc.h"

#include "PedGroupDefaultTaskAllocatorFollowLimited.h"

// 0x5F6480
// Constructor del EXE (thiscall, `mov eax, ecx ; mov [eax], vtable ; ret`, 9 bytes): deja la vtable de la clase del EXE (0x86C764) y devuelve `this`. Lo llama el inicializador estatico del objeto global; no llama a nada ni lee memoria. (Lote 121A, banco Fn121A.)
CPedGroupDefaultTaskAllocatorFollowLimited* CPedGroupDefaultTaskAllocatorFollowLimited::Constructor() {
    *reinterpret_cast<uint32*>(this) = 0x86C764;
    return this;
}
