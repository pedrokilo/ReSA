#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <windows.h>
#include <intrin.h>
#include <float.h>
#include <emmintrin.h>
#include <cstdint>
#include <exception>
#include "CrtStartup.h"
#include "CrtMbcs.h"
#include "CrtOnexit.h"
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
// Igual que el EXE, llama antes a __initmbctable 0x833494 (_setmbcp(-3): tabla _mbctype segun la pagina de codigos ANSI, CrtMbcs.cpp) si todavia no se hizo, asi IsLeadByte ya ve los
// bytes iniciales de doble byte de la pagina del sistema.
int __cdecl Setargv() {
    InitMbcTable(); // 0x82DB30: __initmbctable si todavia no se hizo (_setmbcp(-3)); la bandera 0xC9D430 evita repetirla
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

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// __cinit 0x823B76 (sin gancho) y las entradas de su tabla C 0x8A5A14..0x8A5A30, en el orden del EXE. Sustituyen a la antigua `ExtraTablaC` del experimento de la fase 8.
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace {
inline int&      Sse2FlagA()        { return *reinterpret_cast<int*>(0xC9C400); } // elegida por pow/floor/ceil: != 0 = nucleo SSE2
inline int&      Sse2FlagB()        { return *reinterpret_cast<int*>(0xC9C404); }
inline uint32_t& PrevXcptFilter()   { return *reinterpret_cast<uint32_t*>(0xC9AC74); }
inline uint32_t& SecurityCookie()   { return *reinterpret_cast<uint32_t*>(0x8E31BC); }
constexpr uint32_t DEFAULT_SECURITY_COOKIE = 0xBB40E64E;

// 0x8289C0: prueba una instruccion SSE2 bajo SEH (el EXE ejecuta un movapd): 1 si no hay excepcion
int __cdecl Sse2ExecutionProbe() {
    __try {
        volatile __m128d probe = _mm_setzero_pd();
        (void)probe;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }
    return 1;
}

// 0x832ECF: 1 si el puntero a codigo es valido (IsBadCodePtr == 0)
int __cdecl IsValidCodePointer(uint32_t pointer) {
    return IsBadCodePtr(reinterpret_cast<FARPROC>(pointer)) ? 0 : 1;
}

} // namespace

// 0x826A72: ocho bytes a 0xFF en 0x8E37B4 (tabla de banderas del locale "C" del CRT)
int __cdecl TableC826A72() {
    for (int i = 0; i < 8; ++i) {
        *reinterpret_cast<volatile uint8_t*>(0x8E37B4 + i) = 0xFF;
    }
    return 0;
}

// 0x8289F5 (__sse2_available_init): CPUID funcion 1, EDX bit 26 (SSE2) y la prueba de ejecucion; sin CPUID (el bit ID de EFLAGS no se puede cambiar) EDX = 0. Deja 1 en 0xC9C400 y 0xC9C404 o 0.
int __cdecl TableCSseAvailableInit() {
    const unsigned int flags = __readeflags();
    __writeeflags(flags ^ 0x200000u);
    unsigned int edx = __readeflags() - flags;
    if (edx != 0) {
        __writeeflags(flags);
        int regs[4];
        __cpuid(regs, 1);
        edx = static_cast<unsigned int>(regs[3]);
    }
    Sse2FlagB() = 0;
    Sse2FlagA() = 0;
    if ((edx & (1u << 26)) && Sse2ExecutionProbe()) {
        Sse2FlagB() = 1;
        Sse2FlagA() = 1;
    }
    return 0;
}

// 0x82AC0E (filtro de excepciones no manejadas del CRT, stdcall con EXCEPTION_POINTERS*): una excepcion de C++ (0xE06D7363, 3 parametros, version 0x19930520) que nadie capturo llama a terminate
// (0x827A92 en el EXE). Aqui se llama al terminate del CRT de MSVC enlazado: el EXE propio usa el manejo de excepciones enlazado, no el del EXE (decision en ejecucion-crt-fase8b.md).
// Despues encadena con el filtro anterior si es un puntero de codigo valido; si no, EXCEPTION_CONTINUE_SEARCH (0).
long __stdcall CrtUnhandledExceptionFilter(void* exceptionPointers) {
    EXCEPTION_POINTERS* const info = static_cast<EXCEPTION_POINTERS*>(exceptionPointers);
    const EXCEPTION_RECORD* const record = info->ExceptionRecord;
    if (record->ExceptionCode == 0xE06D7363u && record->NumberParameters == 3 && record->ExceptionInformation[0] == 0x19930520u) {
        std::terminate();
    }
    if (PrevXcptFilter() && IsValidCodePointer(PrevXcptFilter())) {
        return reinterpret_cast<LPTOP_LEVEL_EXCEPTION_FILTER>(PrevXcptFilter())(info);
    }
    return 0;
}

// 0x82AC54: instala el filtro y guarda el anterior en 0xC9AC74
int __cdecl TableCUnhandledFilterInit() {
    PrevXcptFilter() = reinterpret_cast<uint32_t>(SetUnhandledExceptionFilter(reinterpret_cast<LPTOP_LEVEL_EXCEPTION_FILTER>(&CrtUnhandledExceptionFilter)));
    return 0;
}

namespace {
using InitFn = int(__cdecl*)();
// Orden de la tabla C del EXE: 0x8A5A14 (nulo), 0x826A72, __onexitinit 0x821CBE, __initstdio 0x823EE5, 0x8289F5, __initmbctable 0x833494, 0x82AC54
const InitFn kTableC[] = { nullptr, &TableC826A72, &Onexitinit, &Initstdio, &TableCSseAvailableInit, &InitMbcTable, &TableCUnhandledFilterInit };
} // namespace

// 0x82BF1E (terminacion de RTC registrada con atexit por __cinit): recorre el rango 0x88E248..0x88E248 de punteros a funcion, vacio en el EXE de este juego (no hay comprobaciones /RTC)
void __cdecl RtcTerminate() {
    for (auto* fn = reinterpret_cast<void(__cdecl**)()>(0x88E248); reinterpret_cast<uintptr_t>(fn) < 0x88E248; ++fn) {
        if (*fn) {
            (*fn)();
        }
    }
}

// 0x8339CA (__security_init_cookie, primer inicializador de C++ del EXE): mezcla hora, identificadores y contadores; si sale 0 deja 0xBB40E64E. Escribe en 0x8E31BC.
void __cdecl InitSecurityCookie() {
    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime);
    uint32_t value = fileTime.dwHighDateTime ^ fileTime.dwLowDateTime;
    value ^= GetCurrentProcessId();
    value ^= GetCurrentThreadId();
    value ^= GetTickCount();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    value ^= static_cast<uint32_t>(counter.HighPart) ^ counter.LowPart;
    SecurityCookie() = value ? value : DEFAULT_SECURITY_COOKIE;
}

// 0x826B26: rutina de la tabla de despacho de intrinsecas (entrada 4, cdecl, un puntero a double): 1 si *valor >= 0.0 (fcomp con la constante 0.0 de 0x859EF8; con NaN la comparacion
// queda "no ordenada" y devuelve 0). Solo la llaman las rutinas matematicas del EXE por la tabla; el C++ propio pone su direccion en lugar de la del EXE (lote LNK5A).
int __cdecl FpIntrinsicNonNegative(const double* value) {
    return *value >= 0.0 ? 1 : 0;
}

// 0x82134F: seis punteros de la tabla de despacho de funciones intrinsecas (0x8E30F0..0x8E3104) a rutinas del EXE; solo las leen las rutinas matematicas del EXE (nada del C++ propio)
void __cdecl FpInitDispatch() {
    auto* const table = reinterpret_cast<volatile uint32_t*>(0x8E30F0);
    table[0] = 0x826E61;
    table[1] = 0x826ADB;
    table[2] = 0x826B40;
    table[3] = 0x826A9F;
    table[4] = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&FpIntrinsicNonNegative));
    table[5] = 0x826E61;
}

// 0x826EC4: comprobacion de la division del Pentium (FDIV) con las constantes del EXE (0x8882B0, 0x8882A8; limite 0x85A310). 1 si el residuo supera el limite (division defectuosa).
int __cdecl FdivBugCheck() {
    volatile double divisor  = *reinterpret_cast<const double*>(0x8882B0);
    volatile double dividend = *reinterpret_cast<const double*>(0x8882A8);
    volatile double quotient = dividend / divisor;
    volatile double product  = quotient * divisor;
    volatile double residue  = dividend - product;
    return residue > *reinterpret_cast<const double*>(0x85A310) ? 1 : 0;
}

// 0x826F04: IsProcessorFeaturePresent(PF_FLOATING_POINT_PRECISION_ERRATA) buscada en KERNEL32 por nombre (el EXE no la importa para funcionar en sistemas viejos); sin ella, la prueba FDIV
int __cdecl DetectFdivBug() {
    if (const HMODULE kernel = GetModuleHandleA("KERNEL32")) {
        if (const auto present = reinterpret_cast<BOOL(WINAPI*)(DWORD)>(GetProcAddress(kernel, "IsProcessorFeaturePresent"))) {
            return present(0);
        }
    }
    return FdivBugCheck();
}

// 0x821397 (__fpmath, apuntada por _FPinit 0x8E2B74 y llamada por __cinit antes de la tabla C): tabla de despacho, _adjust_fdiv (0xC9AC00), precision de 53 bits (_controlfp(_PC_53, _MCW_PC),
// que en el EXE pasa por 0x825136 con la mascara sin el bit 0x80000) y limpieza de las excepciones pendientes (fnclex; aqui _clearfp, que ademas limpia las banderas de MXCSR)
void __cdecl FpMath() {
    FpInitDispatch();
    *reinterpret_cast<volatile int*>(0xC9AC00) = DetectFdivBug();
    _controlfp(0x10000, 0x30000);
    _clearfp();
}

// 0x823B76 (__cinit): _FPinit (puntero 0x8E2B74 = __fpmath 0x821397), tabla C (se detiene en la primera que devuelve distinto de cero y devuelve ese valor), atexit(0x82BF1E) e inicializadores de C++.
// De los 1.668 inicializadores de C++ se ejecutan aqui los del CRT (cookie de seguridad) y las 1.489 constantes x87; el resto (140 constructores con atexit, 34 asignaciones de datos del juego, 1 llamada)
// escribe en objetos de DatosExe (ver docs/CodigoVivoExe/fase8-inicializadores.csv).
int __cdecl Cinit() {
    if (const uint32_t preHook = *reinterpret_cast<uint32_t*>(0x8E2B74)) { // _FPinit
        (void)preHook;
        FpMath(); // el unico valor que lleva este EXE en _FPinit es __fpmath (0x821397), reconstruida: se llama directo, sin comparar con la direccion del EXE (lote LNK5A)
    }
    for (const InitFn fn : kTableC) {
        if (!fn) {
            continue;
        }
        if (const int code = fn()) {
            return code;
        }
    }
    Onexit(reinterpret_cast<void*>(&RtcTerminate));
    InitSecurityCookie();
    RunX87StaticInitializers();
    return 0;
}

namespace { bool g_standaloneStartupRan = false; }
bool __cdecl StandaloneStartupRan() { return g_standaloneStartupRan; }

int __cdecl StandaloneStartup() {
    g_standaloneStartupRan = true;
    if (const int code = StartupEarly()) {
        return code;
    }
    if (const int code = StartupArguments()) {
        return code;
    }
    return Cinit();
}
} // namespace ExeCrt
