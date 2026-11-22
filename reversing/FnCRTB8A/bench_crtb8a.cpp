// Banco CRTB8A: pares EXE original vs C++ reconstruido para los restos del CRT del arranque propio (fase 8, rama vivo-crtb):
//  (1) mbcs: _setmbcp 0x833344 (con su nucleo 0x83319E, setSBCS 0x832F74, setSBUpLow 0x832F9D y 0x832F45 dentro), __initmbctable 0x833494, __updatetmbcinfo 0x83312F,
//      __mbschr 0x836697 y _strncnt 0x834B6E (usercall: se llama con un adaptador de 14 bytes);
//  (2) tabla C de __cinit: 0x826A72, __sse2_available_init 0x8289F5 (con 0x8289C0), 0x82AC54 y su filtro 0x82AC0E (con 0x832ECF), y __security_init_cookie 0x8339CA.
// Mismo metodo que FnCRT1A..5A y FnCRTF8A: el gta_sa.exe original (solo lectura) se mapea en 0x400000 dentro de este proceso de 32 bits y se llama por su direccion; el juego no se ejecuta.
// El estado del CRT vive en las direcciones del EXE y es el mismo para las dos versiones; cada caso se prepara igual y se ejecuta con el EXE y luego con el codigo propio (montones separados).
// Uso: bench_crtb8a.exe <ruta del EXE> [--casos N] [--semilla S] [--solo mbcs|tablac]   (devuelve 0 si todo coincide)
#define NOMINMAX
#include <windows.h>
#include <intrin.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

#ifndef CRT_BENCH
#define CRT_BENCH
#endif
#include "bench_apistubs.h"
#include "CrtString.cpp"
#include "CrtMem.cpp"
// Montón con fallo inyectable (los dos caminos, el del EXE por su IAT y el propio por esta macro)
static long g_failAt = -1;
static LPVOID WINAPI FailHeapAlloc(HANDLE h, DWORD f, SIZE_T n) { if (g_failAt >= 0 && g_failAt-- == 0) return nullptr; return HeapAlloc(h, f, n); }
#define HeapAlloc FailHeapAlloc
#include "CrtHeap.cpp"
#undef HeapAlloc
// Llamadas del sistema de la pagina de codigos con valores y fallos controlados (se ponen tambien en la IAT del EXE: 0x858058 GetCPInfo, 0x858034 GetACP, 0x858030 GetOEMCP)
static unsigned g_acp = 1252, g_oem = 437;
static int g_cpFail = -1;
static UINT WINAPI StubGetACP() { return g_acp; }
static UINT WINAPI StubGetOEMCP() { return g_oem; }
static BOOL WINAPI StubGetCPInfo(UINT cp, LPCPINFO info) { if (g_cpFail >= 0 && g_cpFail-- == 0) return FALSE; return GetCPInfo(cp, info); }
#define GetACP StubGetACP
#define GetOEMCP StubGetOEMCP
#define GetCPInfo StubGetCPInfo
#include "CrtLocale.h"
#include "CrtException.h"
#include "CrtLowIo.h"
// Piezas ajenas a este banco: el locale se queda con el cuerpo del EXE (ya comparado en FnCRT4A) y se sustituyen por vacio las que no se llaman
namespace ExeCrt {
LocInfo* __cdecl CurrentLocInfo() { return nullptr; }
int __cdecl IsctypeMt(LocInfo*, int, int) { return 0; }
int __cdecl Ioinit() { return 0; }
int __cdecl Initstdio() { return 0; }
void __cdecl RunX87StaticInitializers() {}
int __cdecl CrtLCMapStringA(unsigned int locale, unsigned int mapFlags, const char* src, int cchSrc, char* dest, int cchDest, int codePage, int bError) {
    return reinterpret_cast<int(__cdecl*)(unsigned int, unsigned int, const char*, int, char*, int, int, int)>(0x82BB1E)(locale, mapFlags, src, cchSrc, dest, cchDest, codePage, bError);
}
int __cdecl CrtGetStringTypeA(unsigned int infoType, const char* src, int cchSrc, unsigned short* charType, int codePage, int lcid, int bError) {
    return reinterpret_cast<int(__cdecl*)(unsigned int, const char*, int, unsigned short*, int, int, int)>(0x830FB8)(infoType, src, cchSrc, charType, codePage, lcid, bError);
}
}
void notsa::StdException::Destruct() { m_vtbl = (const void*)0x887E80; if (m_doFree) ExeCrt::Free((void*)m_what); }
static bool g_crtReady = false;
void operator delete(void* block) noexcept {
    if (!block) return;
    const HANDLE heap = *(HANDLE*)0xC9C2F4;
    if (g_crtReady && heap && HeapValidate(heap, 0, block)) ExeCrt::OperatorDelete(block); else free(block);
}
#include "CrtState.cpp"
#include "CrtOnexit.cpp"
#include "CrtMbcs.cpp"
#include "CrtStartup.h"
using ExeCrt::MbcInfo;

// Llamadas del sistema interpuestas de CrtStartup.cpp (bench_apistubs.h)
unsigned g_ftLo, g_ftHi, g_pid, g_tid, g_tick, g_qpcLo, g_qpcHi;
void* g_prevFilter = nullptr;
void* g_setFilterArg = nullptr;
void  WINAPI StubGetSystemTimeAsFileTime(LPFILETIME f) { f->dwLowDateTime = g_ftLo; f->dwHighDateTime = g_ftHi; }
DWORD WINAPI StubGetCurrentProcessId() { return g_pid; }
DWORD WINAPI StubGetCurrentThreadId() { return g_tid; }
DWORD WINAPI StubGetTickCount() { return g_tick; }
BOOL  WINAPI StubQueryPerformanceCounter(LARGE_INTEGER* c) { c->LowPart = g_qpcLo; c->HighPart = (LONG)g_qpcHi; return TRUE; }
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI StubSetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER f) { g_setFilterArg = (void*)f; return (LPTOP_LEVEL_EXCEPTION_FILTER)g_prevFilter; }

int g_gpaMode = 0, g_present = 0;
static BOOL WINAPI StubPresent(DWORD) { return g_present; }
FARPROC WINAPI StubGetProcAddress(HMODULE m, LPCSTR name) {
    if (g_gpaMode == 1 && (uintptr_t)name > 0xFFFF && !strcmp(name, "IsProcessorFeaturePresent")) return (FARPROC)&StubPresent;
    if (g_gpaMode == 2 && (uintptr_t)name > 0xFFFF && !strcmp(name, "IsProcessorFeaturePresent")) return nullptr;
    return GetProcAddress(m, name);
}

static bool map_exe(const char* path) {
    FILE* f = fopen(path, "rb"); if (!f) { printf("no se puede abrir el EXE\n"); return false; }
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    std::vector<uint8_t> d(n); fread(d.data(), 1, n, f); fclose(f);
    auto nt = (IMAGE_NT_HEADERS32*)(d.data() + ((IMAGE_DOS_HEADER*)d.data())->e_lfanew);
    if (nt->OptionalHeader.ImageBase != 0x400000) return false;
    DWORD oldp;
    if (!VirtualProtect((void*)0x401000, 0x8B0000, PAGE_EXECUTE_READWRITE, &oldp)) { printf("no se pudo abrir la reserva (error %lu)\n", GetLastError()); return false; }
    auto sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; i++) {
        uintptr_t sva = 0x400000 + sec[i].VirtualAddress; size_t vsz = sec[i].Misc.VirtualSize, rsz = std::min<size_t>(sec[i].SizeOfRawData, vsz);
        uintptr_t lo = std::max<uintptr_t>(sva, 0x401000), hi = std::min<uintptr_t>(sva + vsz, 0x401000 + 0x8B0000); if (lo >= hi) continue;
        for (uintptr_t p = lo; p < hi; p++) *(uint8_t*)p = (p - sva < rsz) ? d[sec[i].PointerToRawData + (p - sva)] : 0;
    }
    auto dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    for (auto* d0 = (IMAGE_IMPORT_DESCRIPTOR*)(0x400000 + dir.VirtualAddress); d0->Name; d0++) {
        if (_stricmp((const char*)(0x400000 + d0->Name), "KERNEL32.dll") != 0) continue;
        HMODULE k = GetModuleHandleA("kernel32.dll");
        auto ilt = (uint32_t*)(0x400000 + (d0->OriginalFirstThunk ? d0->OriginalFirstThunk : d0->FirstThunk)); auto iat = (void**)(0x400000 + d0->FirstThunk);
        for (int i = 0; ilt[i]; i++) if (!(ilt[i] & 0x80000000)) { void* p = (void*)GetProcAddress(k, (const char*)(0x400000 + ilt[i] + 2)); if (p) iat[i] = p; }
    }
    return true;
}

typedef int(__cdecl* IFn)();
static uint8_t* g_ptd = nullptr;
static void init_crt_state() {
    *(void**)0x8581F8 = (void*)&FailHeapAlloc;                    // HeapAlloc del EXE con el mismo fallo inyectable
    *(void**)0x858058 = (void*)&StubGetCPInfo;                    // GetCPInfo, GetACP, GetOEMCP controlables
    *(void**)0x858034 = (void*)&StubGetACP;
    *(void**)0x858030 = (void*)&StubGetOEMCP;
    *(void**)0x858200 = (void*)&StubGetSystemTimeAsFileTime;      // __security_init_cookie
    *(void**)0x85818C = (void*)&StubGetCurrentProcessId;
    *(void**)0x8581E8 = (void*)&StubGetCurrentThreadId;
    *(void**)0x858064 = (void*)&StubGetTickCount;
    *(void**)0x8580C8 = (void*)&StubQueryPerformanceCounter;
    *(void**)0x8581D4 = (void*)&StubSetUnhandledExceptionFilter;  // 0x82AC54
    *(void**)0x85806C = (void*)&StubGetProcAddress;               // 0x826F04 (_fpmath)
    *(int*)0xC9C2F8 = 1; *(HANDLE*)0xC9C2F4 = GetProcessHeap();   // montón del CRT = montón del proceso (__active_heap 1)
    *(DWORD*)0x8E3110 = TlsAlloc();
    *(int*)0xC9AC08 = 2;                                          // _osplatform
    ((IFn)0x82AC74)();                                            // _mtinitlocks: cerrojos estaticos
    g_ptd = (uint8_t*)calloc(1, 0x88); *(void**)(g_ptd + 0x54) = (void*)0x8E3510; TlsSetValue(*(DWORD*)0x8E3110, g_ptd);
}

static std::mt19937_64 rng;
static uint32_t rnd(uint32_t n) { return n ? (uint32_t)(rng() % n) : 0; }
static uint64_t fails = 0, total = 0;
#define CHECK(cond, ...) do { total++; if (!(cond)) { if (fails++ < 12) { printf("DIFERENCIA: " __VA_ARGS__); printf("\n"); } } } while (0)

static size_t busy_blocks(HANDLE h) {
    size_t n = 0; PROCESS_HEAP_ENTRY e; e.lpData = nullptr;
    HeapLock(h); while (HeapWalk(h, &e)) if (e.wFlags & PROCESS_HEAP_ENTRY_BUSY) n++; HeapUnlock(h);
    return n;
}

// Adaptador para una funcion usercall: cadena en EAX y cuenta en la pila (_strncnt 0x834B6E): mov eax,[esp+4]; push [esp+8]; call rel32; add esp,4; ret
static int call_strncnt_exe(const char* s, int n) {
    static uint8_t* thunk = nullptr;
    if (!thunk) {
        thunk = (uint8_t*)VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        const uint8_t head[] = { 0x8B, 0x44, 0x24, 0x04, 0xFF, 0x74, 0x24, 0x08, 0xE8 };
        memcpy(thunk, head, sizeof head);
        *(int32_t*)(thunk + 9) = (int32_t)(0x834B6E - (uintptr_t)(thunk + 13));
        thunk[13] = 0x83; thunk[14] = 0xC4; thunk[15] = 0x04; thunk[16] = 0xC3;
    }
    return ((int(__cdecl*)(const char*, int))thunk)(s, n);
}

// ------------------------------------------------------------------ mbcs
static const int kCps[] = { -4, -3, -2, 0, 932, 936, 949, 950, 1252, 437, 874, 1361, 65001, 28591, 99999, 1, 12345, 932, 950 };
static const unsigned kRealCps[] = { 0, 932, 936, 949, 950, 1252, 437, 874, 1361, 65001, 1250, 1251 };
enum { MBC_BEGIN = 0xC9C080, MBC_END = 0xC9C2C0 };
struct Setup {
    uint64_t seed;
    int initKind;     // __ptmbcinfo inicial: 0 nulo, 1 bloque con cuenta 0, 2 con cuenta 1, 3 con cuenta 2
    int sysSet;       // _bSystemSet inicial
    int flag;         // __mbctype_initialized
    int ptdKind;      // __getptd()->mbcInfo: 0 nulo, 1 el global, 2 otro con cuenta 1, 3 otro con cuenta 2
    unsigned curCp;   // __mbcodepage inicial
    unsigned lcCp;    // __lc_codepage
    int cpFail;       // fallo de GetCPInfo en la llamada n-esima (-1 ninguno)
    unsigned acp, oem;
};
struct Snap {
    int ret = 0;
    std::vector<uint8_t> mem;      // 0xC9C080..0xC9C2C0 con __ptmbcinfo normalizado
    int ptKind = -1;               // 0 nulo, 1 el bloque inicial, 2 otro bloque nuevo
    std::vector<uint8_t> glob;     // contenido (0x21D bytes) del bloque global actual
    int sysSet = 0, flag = 0;
    int ptdKind = -1;              // 0 nulo, 1 global, 2 el otro bloque inicial, 3 otro
    std::vector<uint8_t> ptdBlk;   // contenido del bloque del hilo si es el "otro" inicial
    size_t busyDelta = 0; long busyBefore = 0;
    uintptr_t ptr = 0;             // resultado de punteros normalizado (Mbschr: desplazamiento)
};
static MbcInfo* g_initBlk[2]; static MbcInfo* g_otherBlk[2];
static HANDLE g_heap[2];

static void fill_block(MbcInfo* b, uint64_t seed, int refCount) {
    std::mt19937_64 r(seed); uint8_t* p = (uint8_t*)b; for (int i = 0; i < 0x21D; i++) p[i] = (uint8_t)r();
    b->refCount = refCount; b->isMbCodePage = (int)(r() & 1);
}
static void apply_setup(int pass, const Setup& su) {
    *(HANDLE*)0xC9C2F4 = g_heap[pass];
    std::mt19937_64 r(su.seed);
    uint8_t* mem = (uint8_t*)MBC_BEGIN; for (int i = 0; i < MBC_END - MBC_BEGIN; i++) mem[i] = (uint8_t)r();
    // las banderas de _mbctype son pocas: se enmascara para que el caso "ya hay tabla" se parezca a una real
    for (int i = 0; i < 0x101; i++) *(uint8_t*)(0xC9C0A0 + i) &= 0x3F;
    *(unsigned*)0xC9C1A4 = su.curCp; *(int*)0xC9C088 = (int)(r() & 1); { static const unsigned lcids[] = { 0, 0x411, 0x804, 0x412, 0x404, 0x409, 0x407 }; *(unsigned*)0xC9C080 = lcids[r() % 7]; } // LCID validos: con uno invalido LCMapStringA no escribe y queda basura de pila distinta en cada version
    g_initBlk[pass] = nullptr; g_otherBlk[pass] = nullptr;
    if (su.initKind) { g_initBlk[pass] = (MbcInfo*)ExeCrt::Malloc(0x220); fill_block(g_initBlk[pass], su.seed ^ 0x11, su.initKind - 1); }
    *(MbcInfo**)0xC9C084 = g_initBlk[pass];
    if (su.ptdKind >= 2) { g_otherBlk[pass] = (MbcInfo*)ExeCrt::Malloc(0x220); fill_block(g_otherBlk[pass], su.seed ^ 0x22, su.ptdKind - 1); }
    *(MbcInfo**)(g_ptd + 0x60) = su.ptdKind == 0 ? nullptr : su.ptdKind == 1 ? g_initBlk[pass] : g_otherBlk[pass];
    *(int*)0xC9AF8C = su.sysSet; *(int*)0xC9D430 = su.flag; *(unsigned*)0xC9AF40 = su.lcCp;
    g_acp = su.acp; g_oem = su.oem; g_cpFail = su.cpFail;
}
static Snap take_snap(int pass, int ret) {
    Snap s; s.ret = ret;
    s.mem.assign((uint8_t*)MBC_BEGIN, (uint8_t*)MBC_END);
    MbcInfo* pt = *(MbcInfo**)0xC9C084;
    s.ptKind = pt == nullptr ? 0 : pt == g_initBlk[pass] ? 1 : 2;
    *(uint32_t*)&s.mem[4] = 0;   // __ptmbcinfo normalizado (la direccion cambia entre montones)
    if (pt) s.glob.assign((uint8_t*)pt, (uint8_t*)pt + 0x21D);
    s.sysSet = *(int*)0xC9AF8C; s.flag = *(int*)0xC9D430;
    MbcInfo* td = *(MbcInfo**)(g_ptd + 0x60);
    s.ptdKind = td == nullptr ? 0 : td == pt ? 1 : td == g_otherBlk[pass] ? 2 : 3;
    if (s.ptdKind == 2) s.ptdBlk.assign((uint8_t*)td, (uint8_t*)td + 0x21D);
    return s;
}
static bool same(const Snap& a, const Snap& b) {
    return a.ret == b.ret && a.mem == b.mem && a.ptKind == b.ptKind && a.glob == b.glob && a.sysSet == b.sysSet && a.flag == b.flag && a.ptdKind == b.ptdKind && a.ptdBlk == b.ptdBlk && a.busyDelta == b.busyDelta && a.ptr == b.ptr;
}
static Setup random_setup() {
    Setup su; su.seed = rng(); su.initKind = rnd(4); su.sysSet = rnd(2); su.flag = rnd(2); su.ptdKind = rnd(4);
    su.curCp = rnd(3) == 0 ? 0 : kRealCps[rnd(sizeof kRealCps / sizeof *kRealCps)];
    { static const unsigned lcs[] = { 0, 1252, 437, 874 }; su.lcCp = lcs[rnd(4)]; } // paginas de un byte: con cp 0 y una de doble byte la conversion falla y queda basura de pila distinta
    su.cpFail = rnd(5) == 0 ? (int)rnd(4) : -1;
    su.acp = kRealCps[rnd(sizeof kRealCps / sizeof *kRealCps)]; su.oem = kRealCps[rnd(sizeof kRealCps / sizeof *kRealCps)];
    if (rnd(8) == 0) su.acp = 99999;
    return su;
}
static const char* opnames[] = { "Setmbcp", "SetmbcpCore", "SetSbcs", "SetSbUpLow", "InitMbcTable", "UpdateTMbcInfo", "Mbschr", "Strncnt" };
static std::string random_text(bool lead) {
    std::string s; const int len = rnd(12);
    for (int i = 0; i < len; i++) {
        const uint32_t k = rnd(10);
        s += (char)(k < 5 ? 'a' + rnd(26) : k < 7 ? (lead ? 0x81 + rnd(0x7C) : 0x80 + rnd(0x80)) : k < 8 ? 0x40 + rnd(0x80) : 1 + rnd(255));
    }
    return s;
}
static void run_mbcs(uint64_t n) {
    ExeCrt::Lock(0xD); ExeCrt::Unlock(0xD); // el cerrojo 0xD se crea en el primer uso: que no cuente como bloque de un caso
    g_heap[0] = HeapCreate(0, 0, 0); g_heap[1] = HeapCreate(0, 0, 0);
    for (uint64_t c = 0; c < n; c++) {
        HeapDestroy(g_heap[0]); HeapDestroy(g_heap[1]); g_heap[0] = HeapCreate(0, 0, 0); g_heap[1] = HeapCreate(0, 0, 0); // un monton nuevo por caso: nada se arrastra de uno al siguiente
        const Setup su = random_setup();
        const int op = rnd(8); if (getenv("BENCH_TRACE")) printf("caso %llu op %d\n", (unsigned long long)c, op);
        const int cp = kCps[rnd(sizeof kCps / sizeof *kCps)]; if (getenv("BENCH_TRACE")) printf("   cp %d init %d ptd %d cur %u\n", cp, su.initKind, su.ptdKind, su.curCp);
        const std::string text = random_text(rnd(2)); const uint32_t ch = rnd(3) == 0 ? (rnd(0x100) << 8 | (0x40 + rnd(0xB0))) : (rnd(5) == 0 ? 0 : 'a' + rnd(26));
        const int count = (int)rnd(14) - 1;
        Snap sn[2];
        for (int pass = 0; pass < 2; pass++) {
            apply_setup(pass, su);
            const size_t before = busy_blocks(g_heap[pass]);
            int ret = 0; uintptr_t ptr = 0;
            // Mbschr y UpdateTMbcInfo necesitan un bloque global con contenido
            if ((op == 6 || op == 5) && *(MbcInfo**)0xC9C084 == nullptr) {
                MbcInfo* b = (MbcInfo*)ExeCrt::Malloc(0x220); fill_block(b, su.seed ^ 0x33, 0); *(MbcInfo**)0xC9C084 = b;
                g_initBlk[pass] = b; // cuenta como "bloque inicial" en la normalizacion
            }
            const size_t before2 = busy_blocks(g_heap[pass]);
            if (getenv("BENCH_TRACE")) printf("  pasada %d: ejecutando validar=%d\n", pass, (int)HeapValidate(g_heap[pass], 0, nullptr));
            switch (op) {
            case 0: ret = pass ? ExeCrt::Setmbcp(cp) : ((int(__cdecl*)(int))0x833344)(cp); break;
            case 1: ret = pass ? ExeCrt::SetmbcpCore(cp) : ((int(__cdecl*)(int))0x83319E)(cp); break;
            case 2: if (pass) ExeCrt::SetSbcs(); else ((void(__cdecl*)())0x832F74)(); break;
            case 3: if (pass) ExeCrt::SetSbUpLow(); else ((void(__cdecl*)())0x832F9D)(); break;
            case 4: ret = pass ? ExeCrt::InitMbcTable() : ((int(__cdecl*)())0x833494)(); break;
            case 5: { MbcInfo* r = pass ? ExeCrt::UpdateTMbcInfo() : ((MbcInfo*(__cdecl*)())0x83312F)(); ret = r == *(MbcInfo**)0xC9C084; break; }
            case 6: { std::string t = text; const char* r = pass ? ExeCrt::Mbschr(t.c_str(), ch) : ((const char*(__cdecl*)(const char*, unsigned))0x836697)(t.c_str(), ch); ptr = r ? (uintptr_t)(r - t.c_str()) + 1 : 0; break; }
            case 7: { std::string t = text; ret = pass ? ExeCrt::Strncnt(t.c_str(), count) : call_strncnt_exe(t.c_str(), count); break; }
            }
            if (getenv("BENCH_TRACE")) printf("  pasada %d: hecho validar=%d\n", pass, (int)HeapValidate(g_heap[pass], 0, nullptr));
            sn[pass] = take_snap(pass, ret);
            if (getenv("BENCH_TRACE")) printf("   snap ok\n");
            sn[pass].ptr = ptr;
            sn[pass].busyDelta = busy_blocks(g_heap[pass]) - before; (void)before2;
            // limpieza: lo que quedo vivo de este caso se libera para que los montones no crezcan
            if (getenv("BENCH_TRACE")) printf("   busy ok\n");
            MbcInfo* pt = *(MbcInfo**)0xC9C084; MbcInfo* td = *(MbcInfo**)(g_ptd + 0x60);
            CHECK(HeapValidate(g_heap[pass], 0, nullptr), "monton danado tras la operacion (pasada %d, op %s)", pass, opnames[op]); (void)pt; (void)td;
            *(MbcInfo**)0xC9C084 = nullptr; *(MbcInfo**)(g_ptd + 0x60) = nullptr;
            if (g_initBlk[pass] && g_initBlk[pass] != pt && su.initKind && false) ExeCrt::Free(g_initBlk[pass]);
        }
        if (!same(sn[0], sn[1]) && fails < 3 && sn[0].mem != sn[1].mem) { int k = 0; for (size_t i = 0; i < sn[0].mem.size() && k < 8; i++) if (sn[0].mem[i] != sn[1].mem[i]) { printf("  mem[+%#zx] (0x%zX) exe=%02x cpp=%02x\n", i, 0xC9C080 + i, sn[0].mem[i], sn[1].mem[i]); k++; } }
        CHECK(same(sn[0], sn[1]), "%s: caso %llu (cp %d, init %d, ptd %d, sys %d, flag %d, cur %u, lc %u, fallo %d, acp %u) ret %d/%d ptKind %d/%d ptdKind %d/%d blocks %zu/%zu mem %s glob %s", opnames[op], (unsigned long long)c, cp, su.initKind, su.ptdKind, su.sysSet, su.flag, su.curCp, su.lcCp, su.cpFail, su.acp,
              sn[0].ret, sn[1].ret, sn[0].ptKind, sn[1].ptKind, sn[0].ptdKind, sn[1].ptdKind, sn[0].busyDelta, sn[1].busyDelta, sn[0].mem == sn[1].mem ? "=" : "!=", sn[0].glob == sn[1].glob ? "=" : "!=");
    }
    *(HANDLE*)0xC9C2F4 = GetProcessHeap();
}

// ------------------------------------------------------------------ tabla C y cookie
static LONG g_prevCalled; static LONG g_prevRet;
static LONG WINAPI PrevFilter(EXCEPTION_POINTERS*) { g_prevCalled++; return g_prevRet; }
static void run_tablac(uint64_t n) {
    for (uint64_t c = 0; c < n; c++) {
        // 0x826A72: ocho bytes a 0xFF; el resto del entorno queda igual
        uint8_t before[16], after[2][16]; int ret[2];
        for (int i = 0; i < 16; i++) before[i] = (uint8_t)rnd(256);
        for (int pass = 0; pass < 2; pass++) { memcpy((void*)0x8E37B0, before, 16); ret[pass] = pass ? ExeCrt::TableC826A72() : ((IFn)0x826A72)(); memcpy(after[pass], (void*)0x8E37B0, 16); }
        CHECK(ret[0] == ret[1] && !memcmp(after[0], after[1], 16), "0x826A72: bytes distintos");
        // 0x8289F5: deteccion de SSE2
        int sse[2][2];
        for (int pass = 0; pass < 2; pass++) { *(int*)0xC9C400 = (int)rnd(3); *(int*)0xC9C404 = (int)rnd(3); ret[pass] = pass ? ExeCrt::TableCSseAvailableInit() : ((IFn)0x8289F5)(); sse[pass][0] = *(int*)0xC9C400; sse[pass][1] = *(int*)0xC9C404; }
        CHECK(ret[0] == ret[1] && sse[0][0] == sse[1][0] && sse[0][1] == sse[1][1], "0x8289F5: SSE2 %d,%d / %d,%d", sse[0][0], sse[0][1], sse[1][0], sse[1][1]);
        CHECK(sse[0][0] == 1 && sse[0][1] == 1, "este equipo deberia tener SSE2 (EXE %d,%d)", sse[0][0], sse[0][1]);
        // 0x82AC54: instala el filtro y guarda el anterior
        void* prevs[] = { nullptr, (void*)&PrevFilter, (void*)(uintptr_t)(0x1000 + rnd(0x100000)) };
        g_prevFilter = prevs[rnd(3)];
        uint32_t saved[2], arg[2];
        for (int pass = 0; pass < 2; pass++) { *(uint32_t*)0xC9AC74 = rnd(1000); g_setFilterArg = nullptr; ret[pass] = pass ? ExeCrt::TableCUnhandledFilterInit() : ((IFn)0x82AC54)(); saved[pass] = *(uint32_t*)0xC9AC74; arg[pass] = (uint32_t)(uintptr_t)g_setFilterArg; }
        CHECK(ret[0] == ret[1] && saved[0] == saved[1] && arg[0] == 0x82AC0E && arg[1] == (uint32_t)(uintptr_t)&ExeCrt::CrtUnhandledExceptionFilter, "0x82AC54: ret %d/%d anterior %x/%x filtro %x/%x", ret[0], ret[1], saved[0], saved[1], arg[0], arg[1]);
        // 0x82AC0E: el filtro con distintos filtros anteriores y registros de excepcion (sin la rama de terminate de C++: no se puede comparar)
        EXCEPTION_RECORD rec = {}; EXCEPTION_POINTERS ep = { &rec, nullptr };
        const uint32_t codes[] = { 0xC0000005u, 0xE06D7363u, 0x80000003u, 0xC000001Du, 0xE0000001u };
        rec.ExceptionCode = codes[rnd(5)]; rec.NumberParameters = rnd(4);
        if (rec.ExceptionCode == 0xE06D7363u) { if (rec.NumberParameters == 3) { rec.NumberParameters = 2; } rec.ExceptionInformation[0] = 0x19930520u; }
        void* fprev[] = { nullptr, (void*)&PrevFilter, (void*)(uintptr_t)(0x10 + rnd(0x40)) };
        void* fp = fprev[rnd(3)]; g_prevRet = (LONG)rnd(3) - 1;
        LONG rv[2], called[2];
        for (int pass = 0; pass < 2; pass++) {
            *(uint32_t*)0xC9AC74 = (uint32_t)(uintptr_t)fp; g_prevCalled = 0;
            rv[pass] = pass ? ExeCrt::CrtUnhandledExceptionFilter(&ep) : ((LONG(__stdcall*)(void*))0x82AC0E)(&ep);
            called[pass] = g_prevCalled;
        }
        CHECK(rv[0] == rv[1] && called[0] == called[1], "0x82AC0E: codigo %08x np %u anterior %p: %ld/%ld llamadas %ld/%ld", rec.ExceptionCode, rec.NumberParameters, fp, rv[0], rv[1], called[0], called[1]);
        // 0x821397 (__fpmath): tabla de despacho, _adjust_fdiv, precision de 53 bits y fnclex; con IsProcessorFeaturePresent real, propia (bug presente o no) o ausente (prueba FDIV)
        {
            struct Fp { uint32_t table[6]; int fdiv; unsigned cw; } fp[2];
            const unsigned startCw = (rnd(3) << 16) | (rnd(4) << 8);   // PC y RC aleatorios, excepciones enmascaradas
            g_gpaMode = (int)rnd(3); g_present = (int)rnd(2);
            for (int pass = 0; pass < 2; pass++) {
                memset((void*)0x8E30F0, 0, 24); *(int*)0xC9AC00 = 7 + pass;
                _controlfp(startCw, 0x30000 | 0x300);
                if (pass) ExeCrt::FpMath(); else ((void(__cdecl*)())0x821397)();
                memcpy(fp[pass].table, (void*)0x8E30F0, 24); fp[pass].fdiv = *(int*)0xC9AC00; fp[pass].cw = _controlfp(0, 0) & (0x30000 | 0x300);
            }
            g_gpaMode = 0; _controlfp(0x10000, 0x30000);
            CHECK(!memcmp(&fp[0], &fp[1], sizeof(Fp)), "0x821397: despacho %x/%x fdiv %d/%d control %x/%x (modo %d)", fp[0].table[0], fp[1].table[0], fp[0].fdiv, fp[1].fdiv, fp[0].cw, fp[1].cw, g_gpaMode);
            CHECK((fp[1].cw & 0x30000) == 0x10000, "0x821397: precision no es 53 bits (%x)", fp[1].cw);
        }
        // 0x8339CA: cookie con las llamadas del sistema fijadas (una de cada cuatro, todas a cero: el resultado 0 sale como 0xBB40E64E)
        const bool zeros = rnd(4) == 0;
        g_ftLo = zeros ? 0 : (unsigned)rng(); g_ftHi = zeros ? 0 : (unsigned)rng(); g_pid = zeros ? 0 : (unsigned)rng(); g_tid = zeros ? 0 : (unsigned)rng(); g_tick = zeros ? 0 : (unsigned)rng();
        g_qpcLo = zeros ? 0 : (unsigned)rng(); g_qpcHi = zeros ? 0 : (unsigned)rng();
        if (rnd(6) == 0) { g_ftLo = 0x1234; g_ftHi = 0x1234; g_pid = 0; g_tid = 0; g_tick = 0; g_qpcLo = 0x77; g_qpcHi = 0x77; } // la mezcla da 0 sin ser todo cero
        uint32_t cookie[2];
        for (int pass = 0; pass < 2; pass++) { *(uint32_t*)0x8E31BC = (uint32_t)rng(); if (pass) ExeCrt::InitSecurityCookie(); else ((void(__cdecl*)())0x8339CA)(); cookie[pass] = *(uint32_t*)0x8E31BC; }
        CHECK(cookie[0] == cookie[1], "cookie de seguridad %08x / %08x", cookie[0], cookie[1]);
    }
}


static void debug_sbuplow() {
    static const unsigned cps[] = { 0, 1252, 437, 932, 65001 };
    for (unsigned cp : cps) for (unsigned lc : { 0u, 1252u, 1361u }) {
        std::vector<uint8_t> out[2];
        for (int pass = 0; pass < 2; pass++) {
            memset((void*)0xC9C0A0, 0, 0x101); memset((void*)0xC9C1C0, 0, 0x100);
            *(unsigned*)0xC9C1A4 = cp; *(unsigned*)0xC9C080 = 0x409; *(unsigned*)0xC9AF40 = lc; g_cpFail = -1;
            if (pass) ExeCrt::SetSbUpLow(); else ((void(__cdecl*)())0x832F9D)();
            out[pass].assign((uint8_t*)0xC9C0A0, (uint8_t*)0xC9C0A0 + 0x101); out[pass].insert(out[pass].end(), (uint8_t*)0xC9C1C0, (uint8_t*)0xC9C1C0 + 0x100);
        }
        printf("debug SbUpLow cp %u lc %u: %s\n", cp, lc, out[0] == out[1] ? "igual" : "DISTINTO");
    }
}
static LONG WINAPI CrashFilter(EXCEPTION_POINTERS* e) {
    printf("EXCEPCION %08lx en %p (acceso %p)\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, e->ExceptionRecord->NumberParameters > 1 ? (void*)e->ExceptionRecord->ExceptionInformation[1] : nullptr);
    return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char** argv) {
    SetUnhandledExceptionFilter(CrashFilter);
    setvbuf(stdout, nullptr, _IONBF, 0);
    if (argc < 2) { printf("uso: bench_crtb8a.exe <gta_sa.exe> [--casos N] [--semilla S] [--solo mbcs|tablac]\n"); return 2; }
    uint64_t n = 3000, seed = 1; std::string solo;
    for (int i = 2; i < argc; i++) { if (!strcmp(argv[i], "--casos") && i + 1 < argc) n = _strtoui64(argv[++i], 0, 10); else if (!strcmp(argv[i], "--semilla") && i + 1 < argc) seed = _strtoui64(argv[++i], 0, 10); else if (!strcmp(argv[i], "--solo") && i + 1 < argc) solo = argv[++i]; }
    rng.seed(seed);
    if (!map_exe(argv[1])) return 2;
    init_crt_state();
    g_crtReady = true;
    int rc = 0;
    if (getenv("BENCH_DEBUG")) debug_sbuplow();
    auto section = [&](const char* name, const char* label, void (*fn)(uint64_t), uint64_t cases) {
        if (!solo.empty() && solo != name) return;
        const uint64_t f0 = fails, t0 = total; fn(cases);
        printf("%s: %llu comparaciones, %llu diferencias\n", label, (unsigned long long)(total - t0), (unsigned long long)(fails - f0));
    };
    section("mbcs", "_setmbcp y familia / __updatetmbcinfo / __mbschr / _strncnt", run_mbcs, n * 10);
    section("tablac", "tabla C de __cinit y cookie", run_tablac, n);
    if (fails) rc = 1;
    printf("RESULTADO: %s (%llu comparaciones, %llu diferencias)\n", rc ? "HAY DIFERENCIAS" : "TODO IGUAL", (unsigned long long)total, (unsigned long long)fails);
    return rc;
}
