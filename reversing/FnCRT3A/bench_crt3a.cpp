// Banco CRT3A: pares EXE original vs C++ reconstruido para pow (nucleo SSE2 0x829B99 y envoltura 0x8220F0), std::exception (0x822D97/0x822DE5/0x822E2F), los lanzadores
// _Xran/_Xlen (0x8211FD/0x821255), __onexit (nucleo 0x821C3E con EDI, y 0x821CE6) y un experimento con fldcw (0x85742B).
// Mismo metodo que FnCRT1A/FnCRT2A: el gta_sa.exe original (solo lectura) se mapea en 0x400000 dentro de este proceso de 32 bits y se llama por su direccion; el juego no se ejecuta.
// El manejador de errores de la CRT (0x82872C), al que van a parar los casos especiales de pow, se sustituye en la imagen mapeada por un registrador (el mismo para el EXE y para el C++):
// se compara el codigo de error y el resultado que se le pasa.
// Uso: bench_crt3a.exe <ruta del EXE> [--casos N] [--semilla S] [--solo pow|exc|xran|onexit|fldcw]   (devuelve 0 si todo coincide)
#define NOMINMAX
#include <windows.h>
#include <intrin.h>
#include <emmintrin.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <cmath>
#include <map>

#define CRT_BENCH
#include "CrtString.cpp"
#include "CrtHeap.cpp"
#include "CrtState.cpp"
#include "CrtMath.cpp"
#include "CrtFile.cpp"
#include "CrtPow.cpp"
#include "CrtException.cpp"
#include "CrtOnexit.cpp"

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
    return true;
}

static std::mt19937_64 rng;
static uint32_t rnd(uint32_t n) { return n ? (uint32_t)(rng() % n) : 0; }
static uint64_t dbits(double x) { uint64_t b; memcpy(&b, &x, 8); return b; }
static double bitsd(uint64_t b) { double x; memcpy(&x, &b, 8); return x; }

static void fill_iat() {
    *(void**)0x858094 = (void*)&GetLastError; *(void**)0x85809c = (void*)&SetLastError;
    *(void**)0x8581E0 = (void*)&TlsGetValue;  *(void**)0x8581E4 = (void*)&TlsSetValue; *(void**)0x8581E8 = (void*)&GetCurrentThreadId;
    *(void**)0x8581F4 = (void*)&HeapReAlloc;  *(void**)0x8581F8 = (void*)&HeapAlloc;   *(void**)0x8581FC = (void*)&HeapFree;
    *(void**)0x858214 = (void*)&RaiseException; *(void**)0x8581D8 = (void*)&HeapSize;
    *(int*)0xC9C2F8 = 1; *(HANDLE*)0xC9C2F4 = GetProcessHeap();
    *(DWORD*)0x8E3110 = TlsAlloc();
}

// ------------------------------------------------------------------ manejador de errores de la CRT (0x82872C) sustituido por un registrador
struct HandlerLog { int calls = 0; int code = 0; uint64_t resBefore = 0, xBits = 0, yBits = 0; };
static HandlerLog g_log;
static void __cdecl FakeHandler(double* x, double* y, double* res, int code) {
    g_log.calls++; g_log.code = code; g_log.resBefore = dbits(*res); g_log.xBits = dbits(*x); g_log.yBits = dbits(*y);
}
static void patch_handler() {
    uint8_t* p = (uint8_t*)0x82872C; p[0] = 0xE9; *(int32_t*)(p + 1) = (int32_t)((uintptr_t)&FakeHandler - 0x82872C - 5);
}

typedef double(__cdecl* PFn)(double, double);

// ------------------------------------------------------------------ pow
static const double kEdge[] = {0.0, -0.0, 1.0, -1.0, 2.0, -2.0, 0.5, -0.5, 3.0, 10.0, 0.1, 1e-300, 1e300, -1e300, 1e-310, -1e-310, 4.9406564584124654e-324, -4.9406564584124654e-324,
    2.2250738585072014e-308, 2.2250738585072009e-308, 1.7976931348623157e308, -1.7976931348623157e308, 0.98, 0.9, 1.0000000000000002, 0.9999999999999999, -0.9999999999999999,
    9007199254740992.0, 9007199254740993.0, 4503599627370496.0, 4503599627370497.5, 2147483648.0, 4294967296.0, 1e10, 1e15, 1e16, 1e22, 1024.0, 1023.0, 1075.0, 1074.0, -1074.0, -1075.0, 709.0, 710.0, -745.0, -746.0, 255.0, 0.05, 20.0};
static const uint64_t kSpecial[] = {0x7FF0000000000000ull, 0xFFF0000000000000ull, 0x7FF8000000000000ull, 0xFFF8000000000000ull, 0x7FF0000000000001ull, 0xFFF0000000000001ull, 0x7FF4000000000000ull,
    0x7FFFFFFFFFFFFFFFull, 0x7FF8000000000001ull, 0x0000000000000001ull, 0x000FFFFFFFFFFFFFull, 0x0010000000000000ull, 0x8000000000000001ull, 0x800FFFFFFFFFFFFFull, 0x8010000000000000ull, 0x3FF0000000000001ull, 0xBFF0000000000001ull, 0x3FEFFFFFFFFFFFFFull, 0xBFEFFFFFFFFFFFFFull};
static double pick_special() {
    const uint32_t ne = (uint32_t)(sizeof kEdge / sizeof kEdge[0]);
    const uint32_t k = rnd(ne + (uint32_t)(sizeof kSpecial / sizeof kSpecial[0]));
    if (k < ne) return kEdge[k];
    return bitsd(kSpecial[k - ne]);
}
static double rand_exp_bits(int emin, int emax) { // exponente sesgado en [emin, emax], mantisa y signo aleatorios
    return bitsd(((uint64_t)(emin + rnd((uint32_t)(emax - emin + 1))) << 52) | (rng() & 0xFFFFFFFFFFFFFull) | ((rng() & 1) << 63));
}
static double rand_unit(double lo, double hi) { return lo + (double)(rng() >> 11) / 9007199254740992.0 * (hi - lo); }
static double near_int(double base) { // entero (o casi) como exponente
    double v = std::floor(base);
    switch (rnd(4)) { case 0: return v; case 1: return nextafter(v, 1e300); case 2: return nextafter(v, -1e300); default: return v + 0.5; }
}
static void gen_pow(double& x, double& y) {
    const uint32_t k = rnd(25);
    switch (k) {
    case 0: x = pick_special(); y = pick_special(); break;
    case 1: x = bitsd(rng()); y = bitsd(rng()); break;                                   // cualquier patron (NaN, inf, subnormales)
    case 2: x = pick_special(); y = rand_unit(-40, 40); break;
    case 3: x = rand_unit(-40, 40); y = pick_special(); break;
    case 4: x = rand_unit(0.001, 1000); y = rand_unit(-50, 50); break;                  // uso tipico
    case 5: x = std::fabs(rand_unit(0, 2)); y = rand_unit(-3, 3); break;
    case 6: x = rand_unit(0.9, 1.0); y = rand_unit(0, 5000); break;                     // decaimientos tipo pow(0.98, dt)
    case 7: x = 2.0; y = (double)(int)rnd(2200) - 1100.0 + (rnd(4) ? 0.0 : rand_unit(-1, 1)); break; // potencias de 2 (desbordamiento / subnormales)
    case 8: x = 10.0; y = rand_unit(-330, 310); break;
    case 9: x = rand_unit(-100, 100); y = (double)((int)rnd(41) - 20); break;           // base negativa, exponente entero
    case 10: x = -rand_unit(0.001, 100); y = near_int(rand_unit(-30, 30)); break;       // base negativa, exponente entero o cercano
    case 11: x = rand_exp_bits(1, 2046); y = rand_unit(-3, 3); break;                   // exponentes extremos de la base
    case 12: x = rand_exp_bits(1, 2046); y = rand_exp_bits(900, 1100); break;           // base y exponente con exponentes extremos
    case 13: x = std::fabs(rand_exp_bits(0, 5)); y = rand_unit(-2, 2); break;           // base subnormal o casi
    case 14: x = bitsd(0x3FF0000000000000ull + (rng() % 2000) - 1000); y = rand_unit(-1e12, 1e12); break;   // base muy cerca de 1
    case 15: x = -bitsd(0x3FF0000000000000ull + (rng() % 2000) - 1000); y = near_int(rand_unit(-1e9, 1e9)); break;
    case 16: x = rand_exp_bits(1000, 1046); y = rand_unit(-1100, 1100) / (double)(1 + rnd(40)); break;       // resultado cerca de overflow
    case 17: { const double e = rand_unit(-1080, 1030); y = (double)(int)rnd(60) + 1.0; x = std::exp2(e / y); if (rnd(2)) x = -x; break; } // resultado cerca de los limites
    case 18: x = pick_special(); y = near_int(rand_unit(-1e5, 1e5)); break;
    case 19: x = (double)(float)rand_unit(-10, 10); y = (double)(float)rand_unit(-10, 10); break;           // valores float como los del juego
    case 20: x = std::fabs(rand_exp_bits(1000, 1046)); y = (double)(int)rnd(2000) - 1000; break;
    case 21: x = rand_unit(0.0, 4.0); y = rand_unit(-1e-3, 1e-3); break;
    case 22: x = rand_exp_bits(0x3FE, 0x400); { const double sp[] = {bitsd(0x7FF0000000000000ull), bitsd(0xFFF0000000000000ull), bitsd(0x7FF8000000000000ull)}; y = sp[rnd(3)]; } break; // |x| cerca de 1 con y = ±inf / NaN
    case 23: { static const int ex[] = {0, 1, 2, 3, 0x7FC, 0x7FD, 0x7FE, 0x7FF}; const int e = ex[rnd(8)]; x = rand_exp_bits(e, e); y = rand_unit(-3, 3); } break; // exponentes en los limites del x
    default: { static const int ex[] = {0, 1, 2, 3, 0x7FC, 0x7FD, 0x7FE, 0x7FF}; const int e = ex[rnd(8)]; x = rand_exp_bits(e, e); y = rnd(2) ? pick_special() : rand_unit(-3, 3); } break;
    }
}

struct Outcome { uint64_t result; HandlerLog log; uint32_t mxcsr; };
static Outcome call_pow(PFn fn, double x, double y) {
    g_log = HandlerLog(); _mm_setcsr(0x1F80);
    const double r = fn(x, y);
    Outcome o; o.result = dbits(r); o.log = g_log; o.mxcsr = _mm_getcsr() & 0x3F; return o;
}
static bool same(const Outcome& a, const Outcome& b) {
    return a.result == b.result && a.log.calls == b.log.calls && a.log.code == b.log.code && a.log.resBefore == b.log.resBefore && a.log.xBits == b.log.xBits && a.log.yBits == b.log.yBits;
}

struct PowStats { std::map<int, uint64_t> codes; uint64_t fails[3] = {}, handled = 0, flagDiff = 0, nanR = 0, infR = 0, zeroR = 0, subR = 0; };
static void run_pow(uint64_t n, PowStats& st) {
    *(int*)0xC9C400 = 1; // rama SSE2
    PFn exeCore = (PFn)0x829B99, exeWrap = (PFn)0x8220F0;
    for (uint64_t c = 0; c < n; c++) {
        double x, y; gen_pow(x, y);
        const Outcome e = call_pow(exeCore, x, y), o = call_pow(ExeCrt::PowSse2, x, y), ew = call_pow(exeWrap, x, y), ow = call_pow(ExeCrt::Pow, x, y);
        const double er = bitsd(e.result);
        if (e.log.calls) { st.handled++; st.codes[e.log.code]++; }
        if (er != er) st.nanR++; else if (std::isinf(er)) st.infR++; else if (er == 0) st.zeroR++; else if (std::fabs(er) < 2.2250738585072014e-308) st.subR++;
        if (!same(e, o)) { if (st.fails[0]++ < 12) printf("DIFERENCIA nucleo x=%016llx y=%016llx exe=%016llx/%d/%d propio=%016llx/%d/%d\n", (unsigned long long)dbits(x), (unsigned long long)dbits(y),
                    (unsigned long long)e.result, e.log.calls, e.log.code, (unsigned long long)o.result, o.log.calls, o.log.code); }
        if (!same(ew, ow)) { if (st.fails[1]++ < 12) printf("DIFERENCIA envoltura x=%016llx y=%016llx\n", (unsigned long long)dbits(x), (unsigned long long)dbits(y)); }
        if (!same(e, ew)) st.fails[2]++; // coherencia del propio EXE (envoltura -> nucleo)
        if (e.mxcsr != o.mxcsr) st.flagDiff++;
    }
}

// ------------------------------------------------------------------ std::exception
typedef notsa::StdException*(__fastcall* Ctor0)(void*, void*);
typedef notsa::StdException*(__fastcall* Ctor1)(void*, void*, const void*);
static size_t busy_blocks(HANDLE h) {
    size_t n = 0; PROCESS_HEAP_ENTRY e; e.lpData = nullptr;
    HeapLock(h);
    while (HeapWalk(h, &e)) if (e.wFlags & PROCESS_HEAP_ENTRY_BUSY) n++;
    HeapUnlock(h);
    return n;
}
static std::string busy_sizes(HANDLE h) {
    std::string r; PROCESS_HEAP_ENTRY e; e.lpData = nullptr;
    HeapLock(h);
    while (HeapWalk(h, &e)) if (e.wFlags & PROCESS_HEAP_ENTRY_BUSY) r += std::to_string(e.cbData) + " ";
    HeapUnlock(h);
    return r;
}
static void run_exc(uint64_t n, uint64_t& fails, uint64_t& total) {
    HANDLE hExe = HeapCreate(0, 0, 0), hOurs = HeapCreate(0, 0, 0);
    for (uint64_t c = 0; c < n; c++) {
        uint8_t a[12], b[12]; memset(a, 0xAA, 12); memset(b, 0xAA, 12);
        ((Ctor0)0x822D97)(a, nullptr); ((notsa::StdException*)b)->Construct(); total++;
        if (memcmp(a, b, 12)) { if (fails++ < 5) printf("DIFERENCIA Construct\n"); }
        const int len = rnd(5) == 0 ? rnd(300) : rnd(40);
        std::string text; for (int i = 0; i < len; i++) text += (char)(1 + rnd(255));
        const bool own = rnd(2) != 0;
        notsa::StdException src; src.m_vtbl = (void*)(uintptr_t)rng(); src.m_what = text.c_str(); src.m_doFree = own ? (int32_t)(1 + rnd(3)) : 0;
        uint8_t ca[12], cb[12]; memset(ca, 0xBB, 12); memset(cb, 0xBB, 12);
        *(HANDLE*)0xC9C2F4 = hExe; ((Ctor1)0x822DE5)(ca, nullptr, &src);
        *(HANDLE*)0xC9C2F4 = hOurs; ((notsa::StdException*)cb)->CopyConstruct(src); total++;
        const auto* ea = (notsa::StdException*)ca; const auto* eb = (notsa::StdException*)cb;
        bool ok = ea->m_vtbl == eb->m_vtbl && ea->m_doFree == eb->m_doFree && (ea->m_what == nullptr) == (eb->m_what == nullptr);
        if (own) ok = ok && ea->m_what != text.c_str() && eb->m_what != text.c_str() && !strcmp(ea->m_what, text.c_str()) && !strcmp(eb->m_what, text.c_str()) && HeapSize(hExe, 0, ea->m_what) == HeapSize(hOurs, 0, eb->m_what);
        else ok = ok && ea->m_what == text.c_str() && eb->m_what == text.c_str();
        if (!ok) { if (fails++ < 5) printf("DIFERENCIA CopyConstruct (propia=%d)\n", (int)own); }
        const size_t beforeE = busy_blocks(hExe), beforeO = busy_blocks(hOurs);
        uint8_t da[12], db[12]; memcpy(da, ca, 12); memcpy(db, cb, 12);
        *(HANDLE*)0xC9C2F4 = hExe; ((void(__fastcall*)(void*, void*))0x822E2F)(da, nullptr);
        *(HANDLE*)0xC9C2F4 = hOurs; ((notsa::StdException*)db)->Destruct(); total++;
        const size_t afterE = busy_blocks(hExe), afterO = busy_blocks(hOurs);
        if (memcmp(da, db, own ? 4 : 12) || memcmp(da + 8, db + 8, 4) || beforeE - afterE != beforeO - afterO || (own && beforeE - afterE != (ea->m_what ? 1u : 0u))) { if (fails++ < 5) printf("DIFERENCIA Destruct\n"); }
        if (!own && beforeE != afterE) { if (fails++ < 5) printf("DIFERENCIA Destruct libero sin bandera\n"); }
    }
    *(HANDLE*)0xC9C2F4 = GetProcessHeap();
}

// ------------------------------------------------------------------ _Xran / _Xlen: se compara el registro de excepcion (codigo, parametros, objeto lanzado y su texto)
struct ThrowRec { size_t busyAtFilter = 0; std::string sizesAtFilter; DWORD code = 0, flags = 0, nparams = 0; uintptr_t obj = 0, tinfo = 0; uint8_t objBytes[0x28] = {}; std::string text; uint32_t vtbl = 0; bool caught = false; };
static LONG capture(EXCEPTION_POINTERS* ep, ThrowRec* r) {
    auto* er = ep->ExceptionRecord; r->code = er->ExceptionCode; r->flags = er->ExceptionFlags; r->nparams = er->NumberParameters; r->caught = true; r->busyAtFilter = busy_blocks(*(HANDLE*)0xC9C2F4); r->sizesAtFilter = busy_sizes(*(HANDLE*)0xC9C2F4); if (getenv("DBG")) printf("    [filtro] bloques: %s\n", r->sizesAtFilter.c_str());
    if (er->NumberParameters >= 3) {
        r->obj = er->ExceptionInformation[1]; r->tinfo = er->ExceptionInformation[2];
        memcpy(r->objBytes, (void*)r->obj, 0x28); r->vtbl = *(uint32_t*)r->obj;
        const uint8_t* s = (const uint8_t*)r->obj + 0xC; const uint32_t cap = *(uint32_t*)(s + 0x18), len = *(uint32_t*)(s + 0x14);
        const char* data = cap >= 16 ? *(const char**)(s + 4) : (const char*)(s + 4);
        r->text.assign(data, len);
    }
    return EXCEPTION_EXECUTE_HANDLER;
}
static void throw_via(void(__cdecl* fn)(), ThrowRec* r) {
    __try { fn(); } __except (capture(GetExceptionInformation(), r)) {}
}
static void run_xran(uint64_t& fails, uint64_t& total) {
    HANDLE hExe = HeapCreate(0, 0, 0), hOurs = HeapCreate(0, 0, 0);
    struct Item { uint32_t addr; void(__cdecl* ours)(); const char* name; };
    Item t[2] = {{0x8211FD, (void(__cdecl*)())ExeCrt::StringXran, "_Xran"}, {0x821255, (void(__cdecl*)())ExeCrt::StringXlen, "_Xlen"}};
    for (auto& k : t) {
        ThrowRec a, b;
        *(HANDLE*)0xC9C2F4 = hExe;  throw_via((void(__cdecl*)())k.addr, &a); const size_t busyE = busy_blocks(hExe);
        *(HANDLE*)0xC9C2F4 = hOurs; throw_via(k.ours, &b); const size_t busyO = busy_blocks(hOurs);
        total++;
        if (getenv("DBG")) printf("    [tras desenrollar] EXE: %s | propio: %s\n", busy_sizes(hExe).c_str(), busy_sizes(hOurs).c_str());
        // el temporal de _Xran (23 caracteres) esta en el montón y la tabla de desenrollado del EXE lo destruye; el de _Xlen (15) esta en el buffer interno: no libera nada
        const size_t expectedFreed = k.addr == 0x8211FD ? 1 : 0;
        const bool ok = a.caught && b.caught && a.code == b.code && a.flags == b.flags && a.nparams == b.nparams && a.tinfo == b.tinfo && a.vtbl == b.vtbl && a.text == b.text
            && !memcmp(a.objBytes, b.objBytes, 0xC) && a.busyAtFilter == b.busyAtFilter && b.busyAtFilter - busyO == expectedFreed;
        printf("  %s: codigo EXE %08lX/propio %08lX, ThrowInfo %08X/%08X, vtable %08X/%08X, texto '%s'/'%s', bloques vivos al llegar al filtro %zu/%zu, tras desenrollar %zu/%zu\n", k.name, a.code, b.code,
               (unsigned)a.tinfo, (unsigned)b.tinfo, a.vtbl, b.vtbl, a.text.c_str(), b.text.c_str(), a.busyAtFilter, b.busyAtFilter, busyE, busyO);
        if (!ok) { fails++; printf("DIFERENCIA %s\n", k.name); }
    }
    *(HANDLE*)0xC9C2F4 = GetProcessHeap();
}

// ------------------------------------------------------------------ __onexit: nucleo del EXE (func en EDI) contra OnexitNoLock
static void* exe_core(void* f) {
    void* r;
    __asm {
        push edi
        mov edi, f
        mov eax, 0x821C3E
        call eax
        pop edi
        mov r, eax
    }
    return r;
}
static void run_onexit(uint64_t n, uint64_t& fails, uint64_t& total) {
    for (uint64_t c = 0; c < n; c++) {
        const bool limited = rnd(3) == 0; const SIZE_T cap = limited ? (SIZE_T)(0x1000 + rnd(0x5000)) : 0;
        HANDLE h[2] = {HeapCreate(0, limited ? cap : 0, cap), HeapCreate(0, limited ? cap : 0, cap)};
        if (!h[0] || !h[1]) { printf("HeapCreate fallo\n"); return; }
        std::vector<uint32_t> resList[2], endOff[2], sizes[2];
        const uint32_t count = limited ? rnd(1500) : (rnd(10) == 0 ? 600 + rnd(900) : rnd(300));
        for (int w = 0; w < 2; w++) {
            *(HANDLE*)0xC9C2F4 = h[w];
            void* tab = HeapAlloc(h[w], 0, 0x80); // como 0x821CBE (__onexitinit): malloc(0x80); [tabla] = 0; fin = inicio
            *(uint32_t*)0xC9D42C = (uint32_t)tab; *(uint32_t*)0xC9D428 = (uint32_t)tab; *(uint32_t*)tab = 0;
            for (uint32_t i = 0; i < count; i++) {
                void* f = (void*)(uintptr_t)(0x10000 + i * 4);
                void* r = w == 0 ? exe_core(f) : ExeCrt::OnexitNoLock(f);
                resList[w].push_back((uint32_t)(uintptr_t)r);
                endOff[w].push_back(*(uint32_t*)0xC9D428 - *(uint32_t*)0xC9D42C);
                sizes[w].push_back((uint32_t)HeapSize(h[w], 0, (void*)*(uint32_t*)0xC9D42C));
            }
            std::vector<uint32_t> content((uint32_t*)*(uint32_t*)0xC9D42C, (uint32_t*)*(uint32_t*)0xC9D428);
            resList[w].insert(resList[w].end(), content.begin(), content.end());
        }
        total++;
        if (resList[0] != resList[1] || endOff[0] != endOff[1] || sizes[0] != sizes[1]) { if (fails++ < 5) printf("DIFERENCIA onexit caso %llu (limitado=%d, n=%u)\n", (unsigned long long)c, (int)limited, count); }
        HeapDestroy(h[0]); HeapDestroy(h[1]);
    }
    *(HANDLE*)0xC9C2F4 = GetProcessHeap();
}

// ------------------------------------------------------------------ fldcw (0x85742B): fldcw [0xCAF004]; ret. Experimento: ¿_control87 da lo mismo?
static unsigned short get_cw() {
    unsigned short w;
    __asm { fnstcw w }
    return w;
}
static void set_cw(unsigned short w) {
    __asm { fldcw w }
}
static void run_fldcw() {
    const unsigned short saved = get_cw(); const unsigned csrSaved = _mm_getcsr();
    const unsigned short words[] = {0x27F, 0x7F, 0x37F, 0xF7F, 0x127F, 0x17F, 0x67F, 0xA7F, 0x1BF, 0x23F, 0x03F};
    int equalCw = 0, equalCsr = 0, total = 0;
    for (unsigned short w : words) {
        _mm_setcsr(0x1F80); set_cw(0x37F);
        *(unsigned short*)0xCAF004 = w; ((void(__cdecl*)())0x85742B)();
        const unsigned short cwExe = get_cw(); const unsigned csrExe = _mm_getcsr();
        _mm_setcsr(0x1F80); set_cw(0x37F);
        unsigned mask = _MCW_EM | _MCW_RC | _MCW_PC | _MCW_IC, nv = 0;
        if (w & 0x01) nv |= _EM_INVALID;
        if (w & 0x02) nv |= _EM_DENORMAL;
        if (w & 0x04) nv |= _EM_ZERODIVIDE;
        if (w & 0x08) nv |= _EM_OVERFLOW;
        if (w & 0x10) nv |= _EM_UNDERFLOW;
        if (w & 0x20) nv |= _EM_INEXACT;
        nv |= ((w >> 10) & 3u) << 8; if (((w >> 8) & 3u) == 0) nv |= _PC_24; else if (((w >> 8) & 3u) == 2) nv |= _PC_53; if (w & 0x1000) nv |= _IC_AFFINE; // PC: 24 -> 0x20000, 53 -> 0x10000, 64 -> 0
        _control87(nv, mask);
        const unsigned short cwAlt = get_cw(); const unsigned csrAlt = _mm_getcsr();
        total++; equalCw += cwExe == cwAlt; equalCsr += csrExe == csrAlt;
        printf("  fldcw 0x%04X: EXE cw=0x%04X mxcsr=0x%04X | _control87 cw=0x%04X mxcsr=0x%04X%s%s\n", w, cwExe, csrExe, cwAlt, csrAlt, cwExe != cwAlt ? "  [CW DISTINTA]" : "", csrExe != csrAlt ? "  [MXCSR DISTINTO]" : "");
    }
    set_cw(saved); _mm_setcsr(csrSaved);
    printf("fldcw: %d palabras, cw igual en %d, mxcsr igual en %d\n", total, equalCw, equalCsr);
}

int main(int argc, char** argv) {
    if (argc < 2) { printf("uso: bench_crt3a <gta_sa.exe> [--casos N] [--semilla S] [--solo pow|exc|xran|onexit|fldcw]\n"); return 2; }
    uint64_t n = 200000, seed = 12345; std::string solo;
    for (int i = 2; i < argc; i++) { if (!strcmp(argv[i], "--casos") && i + 1 < argc) n = _strtoui64(argv[++i], 0, 10); else if (!strcmp(argv[i], "--semilla") && i + 1 < argc) seed = _strtoui64(argv[++i], 0, 10); else if (!strcmp(argv[i], "--solo") && i + 1 < argc) solo = argv[++i]; }
    if (!map_exe(argv[1])) return 2;
    fill_iat(); patch_handler();
    rng.seed(seed); int rc = 0;
    if (solo.empty() || solo == "pow") {
        PowStats st; run_pow(n, st);
        printf("pow: %llu casos; diferencias nucleo 0x829B99: %llu, envoltura 0x8220F0: %llu (coherencia del EXE: %llu); banderas MXCSR distintas: %llu\n", (unsigned long long)n, (unsigned long long)st.fails[0], (unsigned long long)st.fails[1], (unsigned long long)st.fails[2], (unsigned long long)st.flagDiff);
        printf("  resultados: %llu por el manejador de errores, NaN %llu, inf %llu, cero %llu, subnormal %llu; por codigo:", (unsigned long long)st.handled, (unsigned long long)st.nanR, (unsigned long long)st.infR, (unsigned long long)st.zeroR, (unsigned long long)st.subR);
        for (auto& kv : st.codes) printf(" %d:%llu", kv.first, (unsigned long long)kv.second);
        printf("\n");
        if (st.fails[0] || st.fails[1] || st.fails[2]) rc = 1;
    }
    if (solo.empty() || solo == "exc") { uint64_t f = 0, t = 0; run_exc(n, f, t); printf("std::exception: %llu comparaciones, %llu diferencias\n", (unsigned long long)t, (unsigned long long)f); if (f) rc = 1; }
    if (solo.empty() || solo == "xran") { uint64_t f = 0, t = 0; run_xran(f, t); printf("_Xran/_Xlen: %llu lanzadores, %llu diferencias\n", (unsigned long long)t, (unsigned long long)f); if (f) rc = 1; }
    if (solo.empty() || solo == "onexit") { uint64_t f = 0, t = 0; run_onexit(std::min<uint64_t>(n, 20000), f, t); printf("__onexit: %llu secuencias, %llu diferencias\n", (unsigned long long)t, (unsigned long long)f); if (f) rc = 1; }
    if (solo.empty() || solo == "fldcw") run_fldcw();
    printf(rc ? "RESULTADO: HAY DIFERENCIAS\n" : "RESULTADO: SIN DIFERENCIAS\n");
    return rc;
}
