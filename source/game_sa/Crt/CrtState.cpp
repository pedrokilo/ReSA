#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <windows.h>
#include "CrtState.h"
#include "CrtHeap.h"

// Reconstruccion de las funciones con estado del CRT del EXE (lote CRT2A). Ver CrtState.h y reversing/FnCRT2A/README.md.

namespace ExeCrt {
namespace {
// Funciones del EXE de las que dependen (no reconstruidas en este lote: se llaman por su direccion)
inline void  CallAmsgExit(int code) { reinterpret_cast<void(__cdecl*)(int)>(0x8244D1)(code); }          // _amsg_exit
inline int   CallMtInitLockNum(int n) { return reinterpret_cast<int(__cdecl*)(int)>(0x82AD3F)(n); }      // _mtinitlocknum
inline void* CallCalloc(unsigned int n, unsigned int s) { return reinterpret_cast<void*(__cdecl*)(unsigned int, unsigned int)>(0x824416)(n, s); }
inline void* CallUpdateTLocInfo() { return reinterpret_cast<void*(__cdecl*)()>(0x825456)(); }           // __updatetlocinfo
inline int   CallIsctype(void* locinfo, int c, int mask) { return reinterpret_cast<int(__cdecl*)(void*, int, int)>(0x82AAE2)(locinfo, c, mask); }

inline DWORD&         TlsIndex()   { return *reinterpret_cast<DWORD*>(0x8E3110); }
inline void*&         GlobalLocInfo() { return *reinterpret_cast<void**>(0x8E2F14); }
inline CRITICAL_SECTION*& LockSlot(int n) { return *reinterpret_cast<CRITICAL_SECTION**>(0x8E31C0 + n * 8); }
constexpr unsigned int FIRST_STD_FILE = 0x8E2C30;
constexpr unsigned int LAST_STD_FILE  = 0x8E2E90;
} // namespace

// 0x827B3D: bloque del hilo actual; lo crea (calloc 0x88, TlsSetValue) si no existe. Conserva el ultimo error del hilo (GetLastError/SetLastError).
ThreadData* __cdecl Getptd() {
    const DWORD lastError = GetLastError();
    ThreadData* ptd = static_cast<ThreadData*>(TlsGetValue(TlsIndex()));
    if (!ptd) {
        ptd = static_cast<ThreadData*>(CallCalloc(1, sizeof(ThreadData)));
        if (ptd && TlsSetValue(TlsIndex(), ptd)) {
            ptd->xcptActTab = reinterpret_cast<void*>(0x8E3510);
            ptd->holdrand = 1;
            const DWORD tid = GetCurrentThreadId();
            ptd->threadHandle = 0xFFFFFFFFu;
            ptd->threadId = tid;
        } else {
            CallAmsgExit(0x10); // no vuelve
        }
    }
    SetLastError(lastError);
    return ptd;
}

// 0x821B11
void __cdecl Srand(unsigned int seed) {
    Getptd()->holdrand = seed;
}

// 0x821B1E: congruencial lineal de MSVC, semilla por hilo en el bloque del CRT del EXE
int __cdecl Rand() {
    ThreadData* const ptd = Getptd();
    const unsigned int next = ptd->holdrand * 0x343FDu + 0x269EC3u;
    ptd->holdrand = next;
    return static_cast<int>((next >> 16) & 0x7FFF);
}

// 0x82244B: tabla de 256 bits (32 bytes) con los delimitadores y el cero; el puntero de continuacion vive en el bloque por hilo
char* __cdecl Strtok(char* str, const char* delims) {
    ThreadData* const ptd = Getptd();
    unsigned char table[32] = {};
    const unsigned char* d = reinterpret_cast<const unsigned char*>(delims);
    unsigned char c;
    do {
        c = *d++;
        table[c >> 3] |= static_cast<unsigned char>(1u << (c & 7));
    } while (c != 0);

    unsigned char* p = reinterpret_cast<unsigned char*>(str ? str : ptd->token);
    const auto isDelim = [&](unsigned char ch) { return (table[ch >> 3] & (1u << (ch & 7))) != 0; };

    // saltar delimitadores iniciales (se detiene en el cero, que tambien es delimitador)
    while (isDelim(*p) && *p != 0) {
        ++p;
    }
    unsigned char* const start = p;
    // buscar el final del token
    while (*p != 0) {
        if (isDelim(*p)) {
            *p++ = 0;
            break;
        }
        ++p;
    }
    ptd->token = reinterpret_cast<char*>(p);
    return start == p ? nullptr : reinterpret_cast<char*>(start);
}

// 0x8228B5: ctype del locale actual (maximo de bytes por caracter 1: tabla de palabras; mayor que 1: _isctype del EXE). _PRINT = 0x157
int __cdecl Isprint(int c) {
    ThreadData* const ptd = Getptd();
    void* locinfo = ptd->locInfo;
    if (locinfo != GlobalLocInfo()) {
        locinfo = CallUpdateTLocInfo();
    }
    const auto bytes = reinterpret_cast<unsigned char*>(locinfo);
    if (*reinterpret_cast<int*>(bytes + 0x28) > 1) {
        return CallIsctype(locinfo, c, 0x157);
    }
    const unsigned short* const table = *reinterpret_cast<unsigned short**>(bytes + 0x48);
    return table[c] & 0x157;
}

// 0x82ADBE
void __cdecl Lock(int lockNumber) {
    if (LockSlot(lockNumber) == nullptr && CallMtInitLockNum(lockNumber) == 0) {
        CallAmsgExit(0x11);
    }
    EnterCriticalSection(LockSlot(lockNumber));
}

// 0x82AD12
void __cdecl Unlock(int lockNumber) {
    LeaveCriticalSection(LockSlot(lockNumber));
}

// 0x823FA2: las FILE estandar usan los bloqueos 0x10.. (uno por posicion de 0x20 bytes); las demas, la seccion critica propia en +0x20
void __cdecl LockFile(File* file) {
    const unsigned int a = reinterpret_cast<unsigned int>(file);
    if (a < FIRST_STD_FILE || a > LAST_STD_FILE) {
        EnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<char*>(file) + 0x20));
        return;
    }
    Lock(static_cast<int>(static_cast<int>(a - FIRST_STD_FILE) >> 5) + 0x10);
}

// 0x823FF4
void __cdecl UnlockFile(File* file) {
    const unsigned int a = reinterpret_cast<unsigned int>(file);
    if (a < FIRST_STD_FILE || a > LAST_STD_FILE) {
        LeaveCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<char*>(file) + 0x20));
        return;
    }
    Unlock(static_cast<int>(static_cast<int>(a - FIRST_STD_FILE) >> 5) + 0x10);
}

// 0x8262A2: indicador de error de la FILE (_IOERR = 0x10)
int __cdecl Ferror(File* file) {
    return file->flag & 0x10;
}
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtStateInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::Getptd, 0x827B3D);
    RH_ScopedGlobalInstall(ExeCrt::Srand, 0x821B11);
    RH_ScopedGlobalInstall(ExeCrt::Rand, 0x821B1E);
    RH_ScopedGlobalInstall(ExeCrt::Strtok, 0x82244B);
    RH_ScopedGlobalInstall(ExeCrt::Isprint, 0x8228B5);
    RH_ScopedGlobalInstall(ExeCrt::Lock, 0x82ADBE);
    RH_ScopedGlobalInstall(ExeCrt::Unlock, 0x82AD12);
    RH_ScopedGlobalInstall(ExeCrt::LockFile, 0x823FA2);
    RH_ScopedGlobalInstall(ExeCrt::UnlockFile, 0x823FF4);
    RH_ScopedGlobalInstall(ExeCrt::Ferror, 0x8262A2);
}
#endif
