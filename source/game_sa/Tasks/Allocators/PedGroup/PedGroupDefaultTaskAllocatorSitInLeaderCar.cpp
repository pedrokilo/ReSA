#include "StdInc.h"

#include "PedGroupDefaultTaskAllocatorSitInLeaderCar.h"

// 0x5F6540
// Constructor del EXE (thiscall, `mov eax, ecx ; mov [eax], vtable ; ret`, 9 bytes): deja la vtable de la clase del EXE (0x86C784) y devuelve `this`. Lo llama el inicializador estatico del objeto global; no llama a nada ni lee memoria. (Lote 121A, banco Fn121A.)
CPedGroupDefaultTaskAllocatorSitInLeaderCar* CPedGroupDefaultTaskAllocatorSitInLeaderCar::Constructor() {
    *reinterpret_cast<uint32*>(this) = 0x86C784;
    return this;
}
