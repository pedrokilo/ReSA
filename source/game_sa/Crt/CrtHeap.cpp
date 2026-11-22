#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include "CrtHeap.h"
#include "CrtState.h"

// Reconstruccion de malloc/free/realloc/calloc/operator new del CRT del EXE para __active_heap == 1 (lote CRT1A). Ver CrtHeap.h y reversing/FnCRT1A/DISENO.md.
// Las ramas del EXE para el montón de bloques pequeños (__active_heap == 3) no se reproducen: no se alcanzan en Windows 2000 o posterior (__heap_select, 0x82C457).

namespace ExeCrt {
namespace {
inline HANDLE&       CrtHeap()          { return *reinterpret_cast<HANDLE*>(0xC9C2F4); }
inline int&          ActiveHeap()       { return *reinterpret_cast<int*>(0xC9C2F8); }
inline int&          NewMode()          { return *reinterpret_cast<int*>(0xC9ADCC); }
using NewHandler = NewHandlerFn;
inline NewHandler&   NewHandlerPtr()    { return *reinterpret_cast<NewHandler*>(0xC9AC68); }
constexpr unsigned int MAX_REQUEST = 0xFFFFFFE0u; // el EXE rechaza (size > -0x20) sin llamar al sistema

} // namespace

// 0x826A57 _callnewh: 1 si hay manejador y pide reintentar, 0 si no
int __cdecl CallNewHandler(unsigned int size) {
    if (const NewHandler handler = NewHandlerPtr()) {
        return handler(size) ? 1 : 0;
    }
    return 0;
}

// 0x826A2D _set_new_handler: __lock(4); anterior = [0xC9AC68]; [0xC9AC68] = handler; __unlock(4); devuelve el anterior
NewHandlerFn __cdecl SetNewHandler(NewHandlerFn handler) {
    Lock(4);
    const NewHandler previous = NewHandlerPtr();
    NewHandlerPtr()           = handler;
    Unlock(4);
    return previous;
}

// 0x826A51 _query_new_handler
NewHandlerFn __cdecl QueryNewHandler() {
    return NewHandlerPtr();
}

bool CrtHeapIsSystemHeap() {
    return ActiveHeap() == 1;
}

// 0x82422B
void* __cdecl NhMalloc(unsigned int size, int callNewHandler) {
    while (size <= MAX_REQUEST) {
        // 0x8241B0 _heap_alloc: un tamaño 0 se convierte en 1
        if (void* const block = HeapAlloc(CrtHeap(), 0, size ? size : 1u)) {
            return block;
        }
        if (callNewHandler == 0 || CallNewHandler(size) == 0) {
            return nullptr;
        }
    }
    return nullptr;
}

// 0x824257
void* __cdecl Malloc(unsigned int size) {
    return NhMalloc(size, NewMode());
}

// 0x82413F
void __cdecl Free(void* block) {
    if (block) {
        HeapFree(CrtHeap(), 0, block);
    }
}

// 0x824269
void* __cdecl Realloc(void* block, unsigned int size) {
    if (!block) {
        return Malloc(size);
    }
    if (size == 0) {
        Free(block);
        return nullptr;
    }
    for (;;) {
        void* result = nullptr;
        if (size <= MAX_REQUEST) {
            result = HeapReAlloc(CrtHeap(), 0, block, size);
        }
        if (result) {
            return result;
        }
        if (NewMode() == 0 || CallNewHandler(size) == 0) {
            return nullptr;
        }
    }
}

// 0x824416
void* __cdecl Calloc(unsigned int count, unsigned int size) {
    unsigned int total = count * size; // imul de 32 bits: el desbordamiento no se detecta, como en el EXE
    if (total == 0) {
        total = 1;
    }
    for (;;) {
        void* result = nullptr;
        if (total <= MAX_REQUEST) {
            result = HeapAlloc(CrtHeap(), HEAP_ZERO_MEMORY, total);
        }
        if (result) {
            return result;
        }
        if (NewMode() == 0 || CallNewHandler(total) == 0) {
            return nullptr;
        }
    }
}

// 0x828C4A (el EXE lo protege con un try/finally que solo toma el bloqueo 4 en el camino del monton de bloques pequenos, __active_heap == 3: no se reproduce)
unsigned int __cdecl Msize(void* block) {
    return static_cast<unsigned int>(HeapSize(CrtHeap(), 0, block));
}

// 0x82119A: malloc(size); si falla, el manejador de new decide reintentar; sin manejador llama a 0x8212ED (lanza std::bad_alloc), como el EXE
void* __cdecl OperatorNew(unsigned int size) {
    for (;;) {
        if (void* const block = Malloc(size)) {
            return block;
        }
        if (CallNewHandler(size) == 0) {
            reinterpret_cast<void(__cdecl*)()>(0x8212ED)();
        }
    }
}

// 0x821195 (`jmp 0x82119A`)
void* __cdecl OperatorNewArray(unsigned int size) {
    return OperatorNew(size);
}

// 0x8214BD (`jmp 0x82413F`)
void __cdecl OperatorDelete(void* block) {
    Free(block);
}

// 0x8213AE (`jmp 0x8214BD`)
void __cdecl OperatorDeleteArray(void* block) {
    OperatorDelete(block);
}
} // namespace ExeCrt

#ifndef CRT_BENCH
// Los ganchos del montón NO se instalan por defecto: el banco los valida fuera del juego (reversing/FnCRT1A) pero falta la prueba dentro del juego
// (EJECUTAR.md). Para activarlos, compilar con GTASA_CRT_HEAP_HOOKS=1. Solo se instalan si el CRT del EXE usa el montón del sistema (__active_heap == 1).
void CrtHeapInjectHooks() {
#ifdef GTASA_CRT_HEAP_HOOKS
    if (!ExeCrt::CrtHeapIsSystemHeap()) {
        return;
    }
    RH_ScopedNamespaceName("ExeCrtHeap");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::NhMalloc, 0x82422B);
    // malloc 0x824257 ya lo engancha VehicleRecording.cpp (CRT/CrtMalloc -> _nh_malloc 0x82422B, que si engancha NhMalloc de arriba): un segundo gancho en la misma direccion es un error.
    RH_ScopedGlobalInstall(ExeCrt::Free, 0x82413F);
    RH_ScopedGlobalInstall(ExeCrt::Realloc, 0x824269);
    RH_ScopedGlobalInstall(ExeCrt::Calloc, 0x824416);
    RH_ScopedGlobalInstall(ExeCrt::Msize, 0x828C4A); // lote P4 E2-monton
    RH_ScopedGlobalInstall(ExeCrt::OperatorNew, 0x82119A);
    // Lote NEWDEL: new[]/delete/delete[] (tres `jmp` de 5 bytes del EXE) y el manejador de new.
    RH_ScopedGlobalInstall(ExeCrt::OperatorNewArray, 0x821195);
    RH_ScopedGlobalInstall(ExeCrt::OperatorDelete, 0x8214BD);
    RH_ScopedGlobalInstall(ExeCrt::OperatorDeleteArray, 0x8213AE);
    RH_ScopedGlobalInstall(ExeCrt::CallNewHandler, 0x826A57);
    RH_ScopedGlobalInstall(ExeCrt::SetNewHandler, 0x826A2D);
    RH_ScopedGlobalInstall(ExeCrt::QueryNewHandler, 0x826A51);
#endif
}
#endif
