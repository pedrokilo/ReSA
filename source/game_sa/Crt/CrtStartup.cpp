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
#include "CrtString.h"
#include "CrtMem.h"

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

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Linea de comandos, entorno y cierre (sin gancho). Estado del EXE: _acmdln 0xC9C414, _aenvptr 0xC9AC50, __argc 0xC9AC1C, __argv 0xC9AC20, _environ 0xC9AC28, _pgmptr 0xC9AC38 (apunta al
// bufer de 0x104 bytes 0xC9ADD8; el byte 0xC9AEDC es su terminador de seguridad), _env_initialized 0xC9D424, tabla _mbctype 0xC9C0A0 (el byte [c + 1] con el bit 4 marca un byte inicial de
// doble byte), f_use de __crtGetEnvironmentStringsA 0xC9AEE0.
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace {
inline char*&   CmdLine()          { return *reinterpret_cast<char**>(0xC9C414); }
inline char*&   EnvStrings()       { return *reinterpret_cast<char**>(0xC9AC50); }
inline int&     ArgCount()         { return *reinterpret_cast<int*>(0xC9AC1C); }
inline char**&  ArgVector()        { return *reinterpret_cast<char***>(0xC9AC20); }
inline char**&  Environ()          { return *reinterpret_cast<char***>(0xC9AC28); }
inline char*&   ProgramPath()      { return *reinterpret_cast<char**>(0xC9AC38); }
inline char*    ProgramPathBuf()   { return reinterpret_cast<char*>(0xC9ADD8); }
inline uint8_t& ProgramPathEnd()   { return *reinterpret_cast<uint8_t*>(0xC9AEDC); }
inline int&     EnvInitialized()   { return *reinterpret_cast<int*>(0xC9D424); }
inline int&     EnvApiKind()       { return *reinterpret_cast<int*>(0xC9AEE0); }
inline bool     IsLeadByte(char c) { return (reinterpret_cast<const uint8_t*>(0xC9C0A1)[static_cast<uint8_t>(c)] & 4) != 0; }

// 0x82D9BA (parse_cmdline de stdargv.c): cuenta o copia los argumentos. Sin `argv` ni `args` solo cuenta (numChars bytes de texto, numArgs punteros con el nulo final); con ellos escribe los
// punteros en `argv` y el texto en `args`. El nombre del programa se lee aparte (las comillas solo agrupan, sin escapes). En el resto: 2N barras + comilla = N barras y abrir o cerrar
// comillas; 2N+1 barras + comilla = N barras y la comilla literal; dos comillas seguidas dentro de comillas dan una comilla.
void ParseCmdline(const char* p, char** argv, char* args, int* numArgs, int* numChars) {
    *numChars = 0;
    *numArgs  = 1;
    if (argv) {
        *argv++ = args;
    }

    bool inQuote = false;
    char c;
    do {
        if (*p == '"') {
            inQuote = !inQuote;
            c = *p++;
            continue;
        }
        ++*numChars;
        if (args) {
            *args++ = *p;
        }
        c = *p++;
        if (IsLeadByte(c)) {
            ++*numChars;
            if (args) {
                *args++ = *p;
            }
            ++p;
        }
    } while (c != '\0' && (inQuote || (c != ' ' && c != '\t')));

    if (c == '\0') {
        --p;
    } else if (args) {
        *(args - 1) = '\0';
    }

    inQuote = false;
    for (;;) {
        if (*p) {
            while (*p == ' ' || *p == '\t') {
                ++p;
            }
        }
        if (*p == '\0') {
            break;
        }
        if (argv) {
            *argv++ = args;
        }
        ++*numArgs;

        for (;;) {
            bool         copyChar = true;
            unsigned int numSlash = 0;
            while (*p == '\\') {
                ++p;
                ++numSlash;
            }
            if (*p == '"') {
                if (numSlash % 2 == 0) {
                    if (inQuote && p[1] == '"') {
                        ++p;
                    } else {
                        copyChar = false;
                    }
                    inQuote = !inQuote;
                }
                numSlash /= 2;
            }
            while (numSlash--) {
                if (args) {
                    *args++ = '\\';
                }
                ++*numChars;
            }
            if (*p == '\0' || (!inQuote && (*p == ' ' || *p == '\t'))) {
                break;
            }
            if (copyChar) {
                if (args) {
                    if (IsLeadByte(*p)) {
                        *args++ = *p++;
                        ++*numChars;
                    }
                    *args++ = *p;
                } else if (IsLeadByte(*p)) {
                    ++p;
                    ++*numChars;
                }
                ++*numChars;
            }
            ++p;
        }
        if (args) {
            *args++ = '\0';
        }
        ++*numChars;
    }
    if (argv) {
        *argv++ = nullptr;
    }
    ++*numArgs;
}

// Final (doble nulo) de un bloque de cadenas consecutivas
template <class Char>
const Char* BlockEnd(const Char* p) {
    if (*p) {
        do {
            ++p;
            while (*p) {
                ++p;
            }
            ++p;
        } while (*p);
    }
    return p;
}
} // namespace

// 0x82DBC8 (__crtGetEnvironmentStringsA): copia ANSI, en memoria del CRT (malloc), del bloque de entorno de doble nulo; null si falla. f_use (0xC9AEE0): 0 sin decidir, 1 API ancha, 2 API ANSI
// (cuando GetEnvironmentStringsW responde ERROR_CALL_NOT_IMPLEMENTED).
char* __cdecl CrtGetEnvironmentStrings() {
    wchar_t* wide = nullptr;
    if (EnvApiKind() == 0) {
        wide = GetEnvironmentStringsW();
        if (wide) {
            EnvApiKind() = 1;
        } else if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED) {
            EnvApiKind() = 2;
        }
    }
    if (EnvApiKind() == 1) {
        if (!wide) {
            wide = GetEnvironmentStringsW();
            if (!wide) {
                return nullptr;
            }
        }
        const int wideCount = static_cast<int>(BlockEnd(wide) - wide) + 1;
        char*     result    = nullptr;
        if (const int size = WideCharToMultiByte(CP_ACP, 0, wide, wideCount, nullptr, 0, nullptr, nullptr)) {
            result = static_cast<char*>(Malloc(static_cast<unsigned int>(size)));
            if (result && !WideCharToMultiByte(CP_ACP, 0, wide, wideCount, result, size, nullptr, nullptr)) {
                Free(result);
                result = nullptr;
            }
        }
        FreeEnvironmentStringsW(wide);
        return result;
    }
    if (EnvApiKind() != 2 && EnvApiKind() != 0) {
        return nullptr;
    }
    char* const ansi = GetEnvironmentStrings();
    if (!ansi) {
        return nullptr;
    }
    const unsigned int size   = static_cast<unsigned int>(BlockEnd<char>(ansi) - ansi) + 1;
    char* const        result = static_cast<char*>(Malloc(size));
    if (result) {
        Memcpy(result, ansi, size);
    }
    FreeEnvironmentStringsA(ansi);
    return result;
}

// 0x82DB26 (__setargv): nombre del ejecutable en _pgmptr y linea de comandos (o ese nombre si esta vacia) partida en __argv/__argc. 0 = bien, -1 = sin memoria.
// El EXE llama antes a __initmbctable 0x833494 (_setmbcp(-3): tabla _mbctype segun la pagina de codigos ANSI) si todavia no se hizo; ese bloque del locale multibyte no esta reconstruido
// (lo lleva la rama del locale): IsLeadByte lee la tabla como este, sin bytes iniciales mientras nadie la inicialice.
int __cdecl Setargv() {
    ProgramPathEnd() = 0;
    GetModuleFileNameA(nullptr, ProgramPathBuf(), 0x104);
    ProgramPath() = ProgramPathBuf();

    const char* cmd = CmdLine();
    if (!cmd || !*cmd) {
        cmd = ProgramPath();
    }
    int numArgs  = 0;
    int numChars = 0;
    ParseCmdline(cmd, nullptr, nullptr, &numArgs, &numChars);

    const unsigned int bytes = static_cast<unsigned int>(numArgs) * 4 + static_cast<unsigned int>(numChars);
    auto* const        block = static_cast<char**>(Malloc(bytes));
    if (!block) {
        return -1;
    }
    ParseCmdline(cmd, block, reinterpret_cast<char*>(block) + static_cast<size_t>(numArgs) * 4, &numArgs, &numChars);
    ArgCount()  = numArgs - 1;
    ArgVector() = block;
    return 0;
}

// 0x82D8F3 (__setenvp): copia cada variable de _aenvptr (salvo las que empiezan por '=') a un vector terminado en nulo, _environ, y libera el bloque. 0 = bien, -1 = sin memoria.
int __cdecl Setenvp() {
    if (!EnvStrings()) {
        return -1;
    }
    unsigned int count = 0;
    for (const char* p = EnvStrings(); *p; p += Strlen(p) + 1) {
        if (*p != '=') {
            ++count;
        }
    }
    char** const env = static_cast<char**>(Malloc((count + 1) * 4));
    Environ()        = env;
    if (!env) {
        return -1;
    }
    char** out = env;
    for (const char* p = EnvStrings(); *p;) {
        const unsigned int size = Strlen(p) + 1;
        if (*p != '=') {
            char* const copy = static_cast<char*>(Malloc(size));
            *out             = copy;
            if (!copy) {
                Free(Environ());
                Environ() = nullptr;
                return -1;
            }
            Strcpy(copy, p);
            ++out;
        }
        p += size;
    }
    Free(EnvStrings());
    EnvStrings() = nullptr;
    *out         = nullptr;
    EnvInitialized() = 1;
    return 0;
}

// Parte de `entry` entre __ioinit y __cinit: guarda GetCommandLineA y el entorno ANSI y construye __argv y _environ. Devuelve 0 o el codigo de _amsg_exit (8 argumentos, 9 entorno).
int __cdecl StartupArguments() {
    CmdLine()    = GetCommandLineA();
    EnvStrings() = CrtGetEnvironmentStrings();
    if (Setargv() < 0) {
        return 8;
    }
    if (Setenvp() < 0) {
        return 9;
    }
    return 0;
}

// 0x823AFE (__crtExitProcess): si mscoree.dll esta cargada llama a su CorExitProcess; despues ExitProcess. No vuelve.
void __cdecl CrtExitProcess(int code) {
    if (const HMODULE mscoree = GetModuleHandleA("mscoree.dll")) {
        if (const auto corExitProcess = reinterpret_cast<void(__stdcall*)(UINT)>(GetProcAddress(mscoree, "CorExitProcess"))) {
            corExitProcess(static_cast<UINT>(code));
        }
    }
    ExitProcess(static_cast<UINT>(code));
}

// 0x823BDB (_doexit). Bajo el bloqueo 8: si otro hilo ya esta saliendo (0xC9AC48 == 1) mata el proceso; anota que se esta saliendo (0xC9AC44 = 1) y `retcaller` (byte 0xC9AC40); si no es
// `quick` ejecuta la tabla de atexit/_onexit de atras adelante (0xC9D42C..0xC9D428; el extremo se relee tras cada llamada) y la tabla 0x8A5A34..0x8A5A3C; siempre ejecuta 0x8A5A40..0x8A5A48
// (terminadores del EXE, los aporta DatosExe). Con `retcaller` suelta el bloqueo y vuelve; si no, marca 0xC9AC48 = 1 y sale por CrtExitProcess.
void __cdecl DoExit(int code, int quick, int retcaller) {
    auto& exitInProgress = *reinterpret_cast<int*>(0xC9AC48);
    auto& exitFlag       = *reinterpret_cast<int*>(0xC9AC44);
    auto& callerReturned = *reinterpret_cast<uint8_t*>(0xC9AC40);
    const auto runTable  = [](uintptr_t begin, uintptr_t end) {
        for (auto* fn = reinterpret_cast<void(__cdecl**)()>(begin); reinterpret_cast<uintptr_t>(fn) < end; ++fn) {
            if (*fn) {
                (*fn)();
            }
        }
    };

    Lock(8);
    if (exitInProgress == 1) {
        TerminateProcess(GetCurrentProcess(), static_cast<UINT>(code));
    }
    exitFlag       = 1;
    callerReturned = static_cast<uint8_t>(retcaller);
    if (!quick) {
        if (OnexitBegin()) {
            uint32_t next = OnexitEnd() - 4;
            bool     more = next >= OnexitBegin();
            OnexitEnd()   = next;
            while (more) {
                if (const uint32_t fn = *reinterpret_cast<uint32_t*>(next)) {
                    reinterpret_cast<void(__cdecl*)()>(fn)();
                }
                next        = OnexitEnd() - 4;
                more        = next >= OnexitBegin();
                OnexitEnd() = next;
            }
        }
        runTable(0x8A5A34, 0x8A5A3C);
    }
    runTable(0x8A5A40, 0x8A5A48);
    if (retcaller) {
        Unlock(8);
    } else {
        exitInProgress = 1;
        CrtExitProcess(code);
    }
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
    if (const int code = StartupArguments()) {
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
