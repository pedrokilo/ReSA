#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <windows.h>
#include <cstdint>
#include "CrtOnexit.h"
#include "CrtHeap.h"
#include "CrtState.h"

// Reconstruccion de __onexit del CRT del EXE (lote CRT3A). Ver CrtOnexit.h.

namespace ExeCrt {
namespace {
inline uint32_t& TableBegin() { return *reinterpret_cast<uint32_t*>(0xC9D42C); }
inline uint32_t& TableEnd() { return *reinterpret_cast<uint32_t*>(0xC9D428); }

struct ExitLock {
    ExitLock() { Lock(8); }
    ~ExitLock() { Unlock(8); }
};
} // namespace

// 0x821C3E: añade func al final de la tabla; si no cabe la agranda (ver CrtOnexit.h)
void* __cdecl OnexitNoLock(void* func) {
    unsigned int size = Msize(reinterpret_cast<void*>(TableBegin()));
    if (size < TableEnd() - TableBegin() + 4) { // sin signo, como el EXE (cmp esi, edx; jae)
        unsigned int grow = size < 0x800 ? size : 0x800;
        void* table = Realloc(reinterpret_cast<void*>(TableBegin()), grow + size);
        if (table == nullptr) {
            table = Realloc(reinterpret_cast<void*>(TableBegin()), size + 0x10);
            if (table == nullptr) {
                return nullptr;
            }
        }
        const int32_t usedBytes = static_cast<int32_t>(TableEnd() - TableBegin());
        TableBegin() = reinterpret_cast<uint32_t>(table);
        TableEnd()   = reinterpret_cast<uint32_t>(table) + static_cast<uint32_t>((usedBytes >> 2) * 4);
    }
    *reinterpret_cast<void**>(TableEnd()) = func;
    TableEnd() += 4;
    return func;
}

// 0x821CE6
void* __cdecl Onexit(void* func) {
    ExitLock lock;
    return OnexitNoLock(func);
}
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtOnexitInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::Onexit, 0x821CE6);
}
#endif
