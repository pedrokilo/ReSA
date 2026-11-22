#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <windows.h>
#include "CrtState.h"
#include "CrtHeap.h"
#include "CrtLocale.h"
#include "CrtStartup.h"
#include <cstring>
#include <string>

// Reconstruccion de las funciones con estado del CRT del EXE (lote CRT2A). Ver CrtState.h y reversing/FnCRT2A/README.md.

namespace ExeCrt {
namespace {
// Funciones del EXE de las que dependen (no reconstruidas en este lote: se llaman por su direccion)
inline int   CallMtInitLockNum(int n) { return reinterpret_cast<int(__cdecl*)(int)>(0x82AD3F)(n); }      // _mtinitlocknum
inline void* CallCalloc(unsigned int n, unsigned int s) { return reinterpret_cast<void*(__cdecl*)(unsigned int, unsigned int)>(0x824416)(n, s); }

inline DWORD&         TlsIndex()   { return *reinterpret_cast<DWORD*>(0x8E3110); }
inline CRITICAL_SECTION*& LockSlot(int n) { return *reinterpret_cast<CRITICAL_SECTION**>(0x8E31C0 + n * 8); }
constexpr unsigned int FIRST_STD_FILE = 0x8E2C30;
constexpr unsigned int LAST_STD_FILE  = 0x8E2E90;
inline int&   ErrorMode()  { return *reinterpret_cast<int*>(0xC9AC58); }   // __error_mode (0 predeterminado, 1 stderr, 2 MessageBox)
inline int&   AppType()    { return *reinterpret_cast<int*>(0x8E2EB4); }   // __app_type (1 consola, 2 grafica)
inline void (__cdecl*& BannerHook())() { return *reinterpret_cast<void(__cdecl**)()>(0xC9ADD0); } // _pFFMSGBANNER del EXE (nula si no hay)
struct RtErrEntry { int number; const char* text; };                        // tabla rterrs (0x8E3480, 18 entradas de 8 bytes)
constexpr int RT_ERR_COUNT = 18;
} // namespace

// 0x82D520 _NMSG_WRITE(numero): busca el mensaje en la tabla rterrs; si el modo de error es stderr (o es el predeterminado en una aplicacion de consola) lo escribe con WriteFile en
// GetStdHandle(STD_ERROR_HANDLE); si no (y el numero no es 0xFC, la cabecera de "run-time error") abre un MessageBox "Runtime Error! Program: <ruta recortada a 59 caracteres> <mensaje>".
// El EXE pasa por __crtMessageBoxA (0x835177: carga user32 por nombre y elige ventana de escritorio/servicio); aqui es MessageBoxA con las mismas banderas (0x12010).
void __cdecl NmsgWrite(int number) {
    const RtErrEntry* const table = reinterpret_cast<const RtErrEntry*>(0x8E3480);
    int index = 0;
    while (index < RT_ERR_COUNT && table[index].number != number) {
        ++index;
    }
    if (index >= RT_ERR_COUNT) {
        return;
    }
    const char* const message = table[index].text;
    const int mode = ErrorMode();
    if (mode == 1 || (mode == 0 && AppType() == 1)) {
        DWORD written = 0;
        WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, static_cast<DWORD>(std::strlen(message)), &written, nullptr);
        return;
    }
    if (number == 0xFC) {
        return;
    }
    char program[0x104 + 1];
    program[0] = '\0';
    if (GetModuleFileNameA(nullptr, program, 0x104) == 0) {
        std::strcpy(program, "<program name unknown>");
    }
    char* shown = program;
    if (std::strlen(program) + 1 > 0x3C) {
        shown = program + std::strlen(program) - 0x3B;
        std::strncpy(shown, "...", 3);
    }
    std::string text = "Runtime Error!\n\nProgram: ";
    text += shown;
    text += "\n\n";
    text += message;
    MessageBoxA(nullptr, text.c_str(), "Microsoft Visual C++ Runtime Library", 0x12010); // MB_ICONHAND | MB_SETFOREGROUND | MB_TASKMODAL
}

// 0x82D6C1 _FF_MSGBANNER: si los errores van a stderr (o es el modo predeterminado en consola) escribe la cabecera 0xFC, llama al gancho de usuario (0xC9ADD0) si existe y escribe 0xFF
void __cdecl FfMsgBanner() {
    const int mode = ErrorMode();
    if (mode == 1 || (mode == 0 && AppType() == 1)) {
        NmsgWrite(0xFC);
        if (const auto hook = BannerHook()) {
            hook();
        }
        NmsgWrite(0xFF);
    }
}

// 0x8244D1 _amsg_exit(numero): cabecera si el modo es stderr, mensaje y _exit(255) (el EXE llama por el puntero 0x8E2EB0, que vale 0x823CBF = _doexit(255, 1, 0)). No vuelve.
void __cdecl AmsgExit(int number) {
    if (ErrorMode() == 1) {
        FfMsgBanner();
    }
    NmsgWrite(number);
    DoExit(0xFF, 1, 0);
    ExitProcess(0xFF); // inalcanzable: DoExit no vuelve
}

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
            AmsgExit(0x10); // no vuelve
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

// 0x8228B5: ctype del locale actual (maximo de bytes por caracter 1: tabla de palabras; mayor que 1: _isctype). _PRINT = 0x157
int __cdecl Isprint(int c) {
    LocInfo* const li = CurrentLocInfo();
    if (li->mbCurMax > 1) {
        return IsctypeMt(li, c, 0x157);
    }
    unsigned short w;
    std::memcpy(&w, li->pctype + c * 2, 2);
    return w & 0x157;
}

// 0x82ADBE
void __cdecl Lock(int lockNumber) {
    if (LockSlot(lockNumber) == nullptr && CallMtInitLockNum(lockNumber) == 0) {
        AmsgExit(0x11);
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

    RH_ScopedGlobalInstall(ExeCrt::AmsgExit, 0x8244D1);
    RH_ScopedGlobalInstall(ExeCrt::FfMsgBanner, 0x82D6C1);
    RH_ScopedGlobalInstall(ExeCrt::NmsgWrite, 0x82D520);
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
