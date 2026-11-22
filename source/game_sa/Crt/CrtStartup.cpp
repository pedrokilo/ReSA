#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <windows.h>
#include <cstdint>
#include "CrtStartup.h"
#include "CrtHeap.h"
#include "CrtState.h"
#include "CrtLowIo.h"
#include "CrtStdio.h"
#include "CrtStaticInit.h"

// Ver CrtStartup.h. Cada funcion lleva la direccion del EXE y sigue su desensamblado. Nada de esto se engancha (ver la cabecera).

namespace ExeCrt {
namespace {
// Estado del CRT del EXE (se lee y escribe en sus direcciones)
inline int&          OsPlatform()     { return *reinterpret_cast<int*>(0xC9AC08); }
inline int&          OsBuild()        { return *reinterpret_cast<int*>(0xC9AC0C); }
inline int&          OsWinVer()       { return *reinterpret_cast<int*>(0xC9AC10); }
inline int&          OsMajor()        { return *reinterpret_cast<int*>(0xC9AC14); }
inline int&          OsMinor()        { return *reinterpret_cast<int*>(0xC9AC18); }
inline HANDLE&       CrtHeapSlot()    { return *reinterpret_cast<HANDLE*>(0xC9C2F4); }
inline int&          ActiveHeapSlot() { return *reinterpret_cast<int*>(0xC9C2F8); }
inline unsigned int& TlsIndex()       { return *reinterpret_cast<unsigned int*>(0x8E3110); }
inline uint32_t&     OnexitBegin()    { return *reinterpret_cast<uint32_t*>(0xC9D42C); }
inline uint32_t&     OnexitEnd()      { return *reinterpret_cast<uint32_t*>(0xC9D428); }

// Tabla de bloqueos del CRT: 36 entradas {puntero a la seccion critica, bandera}; bandera == 1: seccion critica estatica (arreglo 0xC9AC78, 0x18 bytes cada una)
struct LockSlot {
    CRITICAL_SECTION* lock; // +0
    int               kind; // +4
};
inline LockSlot* LockTable() { return reinterpret_cast<LockSlot*>(0x8E31C0); }
constexpr int LOCK_COUNT = 0x24;
inline CRITICAL_SECTION* StaticLocks() { return reinterpret_cast<CRITICAL_SECTION*>(0xC9AC78); }
constexpr DWORD LOCK_SPIN_COUNT = 0xFA0;
} // namespace

// Prologo de `entry` 0x824570: OSVERSIONINFOA de 0x94 bytes
void __cdecl OsVersionInit() {
    OSVERSIONINFOA info{};
    info.dwOSVersionInfoSize = 0x94;
    GetVersionExA(&info);
    OsPlatform() = static_cast<int>(info.dwPlatformId);
    OsMajor()    = static_cast<int>(info.dwMajorVersion);
    OsMinor()    = static_cast<int>(info.dwMinorVersion);
    int build = static_cast<int>(info.dwBuildNumber & 0x7FFF);
    if (info.dwPlatformId != 2) {
        build |= 0x8000;
    }
    OsBuild()  = build;
    OsWinVer() = static_cast<int>((info.dwMajorVersion << 8) + info.dwMinorVersion);
}

// 0x82C457: plataforma NT (2) con version mayor >= 5 -> monton del sistema (1); si no, monton de bloques pequenos (3)
int __cdecl HeapSelect() {
    return (OsPlatform() == 2 && static_cast<unsigned int>(OsMajor()) >= 5) ? 1 : 3;
}

// 0x82C471: HeapCreate(mode == 0 ? HEAP_NO_SERIALIZE : 0, 0x1000, 0). Con __active_heap == 3 el EXE crea ademas el monton de bloques pequenos (0x82C556, no reconstruido):
// ese camino no se alcanza en Windows 2000 o posterior y aqui se trata como fallo.
int __cdecl HeapInit(int mode) {
    HANDLE heap = HeapCreate(mode == 0 ? HEAP_NO_SERIALIZE : 0, 0x1000, 0);
    CrtHeapSlot() = heap;
    if (!heap) {
        return 0;
    }
    ActiveHeapSlot() = HeapSelect();
    if (ActiveHeapSlot() == 3) {
        HeapDestroy(heap);
        return 0;
    }
    return 1;
}

// 0x82AC74: asigna a cada bloqueo estatico su seccion critica (con contador de giros 0xFA0); si una falla, deja el puntero a nulo y devuelve 0
int __cdecl Mtinitlocks() {
    CRITICAL_SECTION* next = StaticLocks();
    for (int i = 0; i < LOCK_COUNT; ++i) {
        LockSlot& slot = LockTable()[i];
        if (slot.kind != 1) {
            continue;
        }
        slot.lock = next;
        ++next;
        if (!InitializeCriticalSectionAndSpinCount(slot.lock, LOCK_SPIN_COUNT)) {
            slot.lock = nullptr;
            return 0;
        }
    }
    return 1;
}

// 0x82ACBD: primero destruye y libera los bloqueos dinamicos (bandera distinta de 1); despues destruye los estaticos
void __cdecl Mtdeletelocks() {
    for (int i = 0; i < LOCK_COUNT; ++i) {
        LockSlot& slot = LockTable()[i];
        if (slot.lock && slot.kind != 1) {
            DeleteCriticalSection(slot.lock);
            Free(slot.lock);
            slot.lock = nullptr;
        }
    }
    for (int i = 0; i < LOCK_COUNT; ++i) {
        LockSlot& slot = LockTable()[i];
        if (slot.lock && slot.kind == 1) {
            DeleteCriticalSection(slot.lock);
        }
    }
}

// 0x827B0C
void __cdecl Mtterm() {
    Mtdeletelocks();
    if (TlsIndex() != 0xFFFFFFFFu) {
        TlsFree(TlsIndex());
        TlsIndex() = 0xFFFFFFFFu;
    }
}

// 0x827D25: bloqueos, indice TLS y bloque de datos del hilo principal (0x88 bytes con calloc)
int __cdecl Mtinit() {
    if (!Mtinitlocks()) {
        Mtterm();
        return 0;
    }
    TlsIndex() = TlsAlloc();
    if (TlsIndex() == 0xFFFFFFFFu) {
        Mtterm();
        return 0;
    }
    auto* const data = static_cast<ThreadData*>(Calloc(1, sizeof(ThreadData)));
    if (!data || !TlsSetValue(TlsIndex(), data)) {
        Mtterm();
        return 0;
    }
    data->xcptActTab   = reinterpret_cast<void*>(0x8E3510);
    data->holdrand     = 1;
    data->threadHandle = 0xFFFFFFFFu;
    data->threadId     = GetCurrentThreadId();
    return 1;
}

// 0x821CBE: tabla de atexit/onexit de 0x80 bytes (32 funciones); el primer hueco queda a cero
int __cdecl Onexitinit() {
    void* const table = Malloc(0x80);
    OnexitBegin() = reinterpret_cast<uint32_t>(table);
    if (!table) {
        return 0x18;
    }
    *static_cast<uint32_t*>(table) = 0;
    OnexitEnd() = OnexitBegin();
    return 0;
}

int __cdecl StartupEarly() {
    OsVersionInit();
    if (!HeapInit(1)) {
        return 0x1C;
    }
    if (!Mtinit()) {
        return 0x10;
    }
    if (Ioinit() < 0) {
        return 0x1B;
    }
    return 0;
}

int __cdecl StandaloneStartup() {
    if (const int code = StartupEarly()) {
        return code;
    }
    if (const int code = Onexitinit()) {
        return code;
    }
    Initstdio();
    RunX87StaticInitializers();
    return 0;
}
} // namespace ExeCrt
