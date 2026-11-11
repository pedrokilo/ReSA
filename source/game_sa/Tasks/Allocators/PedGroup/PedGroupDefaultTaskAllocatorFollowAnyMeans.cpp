#include "StdInc.h"

#include "PedGroupDefaultTaskAllocatorFollowAnyMeans.h"

// 0x5F6450 (lote 157A, banco Fn157A): constructor de CPedGroupDefaultTaskAllocatorFollowAnyMeans del EXE (thiscall, sin argumentos de pila, `ret`, 9 bytes: `mov eax,ecx; mov [eax],86C75Ch; ret`).
// La clase solo tiene vtable (sizeof = 4): deja en [this] la vtable de la clase del EXE (0x86C75C; la de la DLL no vale para los llamadores del EXE) y devuelve `this`.
CPedGroupDefaultTaskAllocatorFollowAnyMeans* __fastcall PedGroupDefaultTaskAllocatorFollowAnyMeansConstructorExe(CPedGroupDefaultTaskAllocatorFollowAnyMeans* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86C75C;
    return self;
}
