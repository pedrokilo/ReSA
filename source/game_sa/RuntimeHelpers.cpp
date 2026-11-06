#include "StdInc.h"

#include "RuntimeHelpers.h"

// 0x403D60 (stdcall, `ret 10h`): dec count; si es negativo (con signo) sale; si no, repite `count` veces (ECX = puntero, que avanza `size` bytes con aritmetica de 32 bits). EBX, EBP, ESI y EDI se conservan.
void __stdcall VectorConstructorIteratorExe(void* base, uint32 size, int32 count, void(__thiscall* ctor)(void*))
{
    if (int32(uint32(count) - 1u) < 0) {
        return;
    }
    uint32 address = uint32(reinterpret_cast<uintptr_t>(base));
    for (uint32 left = uint32(count); left != 0; --left) {
        ctor(reinterpret_cast<void*>(uintptr_t(address)));
        address += size;
    }
}

void RuntimeHelpersInjectHooksLote105A()
{
    RH_ScopedNamespaceName("VectorConstructorIterator");
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(VectorConstructorIteratorExe, 0x403D60);
}
