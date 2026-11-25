// Banco P3 (codigo vivo, lotes S3 y S4): pares EXE original vs C++ propio, fuera del juego.
//   S3: callbacks x87 de _rwcseg: 0x857000, 0x85707F, 0x8570EF, 0x857160 (vectores), 0x8572A0 (matrices), 0x857404 y 0x85742B (palabra de control x87).
//   S4: 0x82872C (manejador de errores matematicos con matherr/errno), 0x833605 (matherr por defecto) y 0x821B40 (_ftol2, argumento en ST0).
// Mismo metodo que FnCRT1A..4A: el gta_sa.exe original (solo lectura) se mapea en 0x400000 dentro de este proceso de 32 bits y se llama por su direccion;
// el juego no se ejecuta. La x87 se pone a 24 bits (la precision que fija el propio 0x857404 en el juego) salvo en la prueba de la palabra de control.
// 0x82872C llama a _errno (0x82B181) y a la comprobacion de cookie (0x82AAD4): se parchean las dos llamadas en la copia mapeada para que usen utilidades del banco.
// 0x821B40 recibe el argumento en ST0: se llama por una plantilla de 10 bytes en memoria ejecutable (fld qword [esp+4]; call; ret), sin __asm.
// Uso: bench_p3.exe <ruta del EXE> [--casos N] [--semilla S] [--solo vec|mat|cw|err|ftol|x87]   (devuelve 0 si todo coincide)
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <xmmintrin.h>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

#define CRT_BENCH
#define RWMATHX87_BENCH
// ErrnoPtr del banco (la real vive en CrtLowIo.cpp, que arrastra el resto del CRT)
static int g_errnoOurs = 0;
#include "CrtState.h"
#include "CrtLowIo.h"
namespace ExeCrt { int* __cdecl ErrnoPtr() { return &g_errnoOurs; } }
#include "CrtMath.cpp"
#include "RwMathX87.cpp"

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
static uint32_t fbits(float x) { uint32_t b; memcpy(&b, &x, 4); return b; }
static float bitsf(uint32_t b) { float x; memcpy(&x, &b, 4); return x; }

// float "realista": mantisa aleatoria y exponente en [-30, 30] (sin desbordes ni subnormales en productos y sumas)
static float rfloat() {
    const uint32_t k = rnd(12);
    if (k == 0) return 0.0f;
    if (k == 1) return -0.0f;
    if (k == 2) return 1.0f;
    if (k == 3) return -1.0f;
    const int e = (int)rnd(61) - 30;
    const uint32_t bits = ((uint32_t)(127 + e) << 23) | ((uint32_t)rng() & 0x7FFFFF) | ((uint32_t)(rng() & 1) << 31);
    return bitsf(bits);
}

static void set_pc24() { unsigned c; _controlfp_s(&c, _PC_24, _MCW_PC); }

// ------------------------------------------------------------------ vectores y matrices
typedef float* (__cdecl* VecFn)(float*, const float*, const float*);
typedef float* (__cdecl* VecsFn)(float*, const float*, int, const float*);
typedef void(__cdecl* MatFn)(float*, const float*, const float*);

static uint64_t run_vec(uint64_t n, uint64_t fails[4], uint64_t counts[4]) {
    set_pc24();
    VecsFn exeVecs[2] = {(VecsFn)0x857000, (VecsFn)0x85707F}, oursVecs[2] = {(VecsFn)RwMathX87::TransformPoints, (VecsFn)RwMathX87::TransformVectors};
    VecFn exeVec[2] = {(VecFn)0x8570EF, (VecFn)0x857160}, oursVec[2] = {(VecFn)RwMathX87::TransformPoint, (VecFn)RwMathX87::TransformVector};
    const char* names[4] = {"puntos", "vectores", "punto", "vector"};
    for (uint64_t c = 0; c < n; c++) {
        alignas(16) float m[16];
        for (int i = 0; i < 16; i++) m[i] = rfloat();
        const int count = 1 + (int)rnd(6);
        std::vector<float> in(3 * count + 8), a(3 * count + 8, bitsf(0xDEADBEEF)), b(3 * count + 8, bitsf(0xDEADBEEF));
        for (auto& v : in) v = rfloat();
        const bool inplace = rnd(4) == 0;
        for (int w = 0; w < 2; w++) {
            // varios puntos
            std::vector<float> ea = in, eb = in;
            if (!inplace) { std::fill(ea.begin(), ea.end(), bitsf(0xDEADBEEF)); std::fill(eb.begin(), eb.end(), bitsf(0xDEADBEEF)); }
            const float* srcA = inplace ? ea.data() : in.data();
            const float* srcB = inplace ? eb.data() : in.data();
            float* ra = exeVecs[w](ea.data(), srcA, count, m);
            float* rb = oursVecs[w](eb.data(), srcB, count, m);
            counts[w]++;
            bool same = ra == (inplace ? ea.data() : in.data()) ? true : false; // el EXE devuelve la entrada
            same = same && (rb == (inplace ? eb.data() : in.data()));
            same = same && memcmp(ea.data(), eb.data(), ea.size() * 4) == 0;
            if (!same) { if (fails[w]++ < 6) printf("DIFERENCIA %s (n=%d, enplace=%d) ret exe=%p ours=%p\n", names[w], count, inplace, (void*)ra, (void*)rb); }
            // un punto
            std::vector<float> sa = in, sb = in;
            if (!inplace) { std::fill(sa.begin(), sa.end(), bitsf(0xDEADBEEF)); std::fill(sb.begin(), sb.end(), bitsf(0xDEADBEEF)); }
            const float* inA = inplace ? sa.data() : in.data();
            const float* inB = inplace ? sb.data() : in.data();
            float* xa = exeVec[w](sa.data(), inA, m);
            float* xb = oursVec[w](sb.data(), inB, m);
            counts[2 + w]++;
            bool same2 = (xa == inA) && (xb == inB) && memcmp(sa.data(), sb.data(), sa.size() * 4) == 0;
            if (!same2) { if (fails[2 + w]++ < 6) printf("DIFERENCIA %s (enplace=%d)\n", names[2 + w], inplace); }
        }
    }
    return 0;
}

static void run_mat(uint64_t n, uint64_t& fails, uint64_t& counts) {
    set_pc24();
    for (uint64_t c = 0; c < n; c++) {
        alignas(16) float a[16], b[16];
        for (int i = 0; i < 16; i++) { a[i] = rfloat(); b[i] = rfloat(); }
        const int mode = (int)rnd(3); // 0 destino aparte, 1 destino == A
        alignas(16) float da[16], db[16];
        for (int i = 0; i < 16; i++) da[i] = db[i] = bitsf(0xDEADBEEF + i);
        float ea[16], eb[16]; memcpy(ea, a, 64); memcpy(eb, a, 64);
        if (mode == 1) {
            ((MatFn)0x8572A0)(ea, ea, b);
            RwMathX87::MatrixMultiply((RwMatrix*)eb, (RwMatrix*)eb, (const RwMatrix*)b);
            counts++;
            if (memcmp(ea, eb, 64) != 0) { if (fails++ < 6) printf("DIFERENCIA matriz (destino == A)\n"); }
        } else {
            ((MatFn)0x8572A0)(da, a, b);
            RwMathX87::MatrixMultiply((RwMatrix*)db, (const RwMatrix*)a, (const RwMatrix*)b);
            counts++;
            if (memcmp(da, db, 64) != 0) {
                if (fails++ < 6) { printf("DIFERENCIA matriz\n"); for (int i = 0; i < 16; i++) if (fbits(da[i]) != fbits(db[i])) printf("  [%d] exe=%08x ours=%08x\n", i, fbits(da[i]), fbits(db[i])); }
            }
        }
    }
}

// ------------------------------------------------------------------ palabra de control x87
static uint32_t crt_cw() { unsigned c = 0; _controlfp_s(&c, 0, 0); return c & (_MCW_EM | _MCW_RC | _MCW_PC | _MCW_IC); }
static void set_crt_cw(uint32_t v) { unsigned c; _clearfp(); _controlfp_s(&c, v, _MCW_EM | _MCW_RC | _MCW_PC | _MCW_IC); }
static void mask_all() { set_crt_cw(_PC_24 | _MCW_EM); }

static void run_cw(uint64_t& fails, uint64_t& counts) {
    const uint32_t ems[] = {0, _EM_INVALID, _EM_INEXACT, _EM_INVALID | _EM_DENORMAL | _EM_ZERODIVIDE | _EM_OVERFLOW | _EM_UNDERFLOW | _EM_INEXACT, _EM_OVERFLOW | _EM_INEXACT};
    const uint32_t rcs[] = {_RC_NEAR, _RC_DOWN, _RC_UP, _RC_CHOP};
    const uint32_t pcs[] = {_PC_24, _PC_53, _PC_64};
    volatile uint32_t* slotNew = (uint32_t*)0xCAF000; volatile uint32_t* slotOld = (uint32_t*)0xCAF004;
    const unsigned csr = _mm_getcsr();
    for (uint32_t em : ems) for (uint32_t rc : rcs) for (uint32_t pc : pcs) for (uint32_t pre : {0u, 0x12340000u}) {
        const uint32_t start = em | rc | pc;
        // EXE
        *slotNew = pre | 0x1111; *slotOld = 0xAAAAAAAA; set_crt_cw(start);
        ((void(__cdecl*)())0x857404)();
        const uint32_t exeCw = crt_cw(), exeNew = *slotNew, exeOld = *slotOld;
        // ahora la restauracion del EXE desde un valor conocido
        set_crt_cw(_PC_53 | _RC_UP | _MCW_EM); *slotOld = 0x027F; ((void(__cdecl*)())0x85742B)(); const uint32_t exeRest = crt_cw();
        // propio
        *slotNew = pre | 0x1111; *slotOld = 0xAAAAAAAA; set_crt_cw(start);
        RwMathX87::SetFpuControlWord();
        const uint32_t ourCw = crt_cw(), ourNew = *slotNew, ourOld = *slotOld;
        set_crt_cw(_PC_53 | _RC_UP | _MCW_EM); *slotOld = 0x027F; RwMathX87::RestoreFpuControlWord(); const uint32_t ourRest = crt_cw();
        counts++;
        if (exeCw != ourCw || exeNew != ourNew || exeOld != ourOld || exeRest != ourRest) {
            if (fails++ < 8) printf("DIFERENCIA palabra de control inicio=%05x: exe cw=%05x new=%08x old=%08x rest=%05x | ours cw=%05x new=%08x old=%08x rest=%05x\n", start, exeCw, exeNew, exeOld, exeRest, ourCw, ourNew, ourOld, ourRest);
        }
        mask_all(); _mm_setcsr(csr);
    }
    // restauraciones con palabras guardadas arbitrarias (incluye bits reservados y precision 01)
    for (uint32_t raw = 0; raw < 0x2000; raw += 1) {
        const uint32_t w = (raw & 0x1F3F) | 0x40;
        mask_all(); *slotOld = w; ((void(__cdecl*)())0x85742B)(); const uint32_t a = crt_cw();
        mask_all(); *slotOld = w; RwMathX87::RestoreFpuControlWord(); const uint32_t b = crt_cw();
        counts++;
        if (a != b) { if (fails++ < 8) printf("DIFERENCIA restauracion con %04x: exe=%05x ours=%05x\n", w, a, b); }
        mask_all(); _mm_setcsr(csr);
    }
    set_pc24();
}

// ------------------------------------------------------------------ _ftol2 (ST0 -> EDX:EAX)
static void* make_ftol_thunk() {
    uint8_t* mem = (uint8_t*)VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    uint8_t code[10] = {0xDD, 0x44, 0x24, 0x04, 0xE8, 0, 0, 0, 0, 0xC3}; // fld qword [esp+4]; call 0x821B40; ret
    const int32_t rel = (int32_t)(0x821B40 - ((uintptr_t)mem + 9));
    memcpy(code + 5, &rel, 4);
    memcpy(mem, code, 10);
    return mem;
}

static void run_ftol(uint64_t n, uint64_t& fails, uint64_t& counts) {
    set_pc24();
    typedef int64_t(__cdecl* Fn)(double);
    Fn exe = (Fn)make_ftol_thunk();
    std::vector<double> edge = {0.0, -0.0, 0.5, -0.5, 1.0, -1.0, 1.5, -1.5, 2.5, -2.5, 0.49999999999999994, -0.49999999999999994, 0.9999999999999999, -0.9999999999999999, 0.99999994, -0.99999994,
        2147483647.5, -2147483648.5, 4294967296.0, 4294967296.3, -4294967296.3, 4294967295.9, 8589934592.7, -8589934592.7, 1e18, -1e18, 4611686018427387904.0, 9223372036854775807.0, 9223372036854775808.0,
        -9223372036854775808.0, -9223372036854777856.0, 1e300, -1e300, 1e-300, -1e-300, bitsd(0x7FF0000000000000ull), bitsd(0xFFF0000000000000ull), bitsd(0x7FF8000000000000ull), bitsd(0xFFF8000000000000ull),
        4503599627370496.5, 4503599627370495.5, -4503599627370495.5, 16777216.5, 16777217.0, 100.4999, 100.5, 100.50001, -100.4999, -100.5, -100.50001, 4096.0 * 32767.5, 1.4e-45, -1.4e-45};
    for (uint64_t c = 0; c < n; c++) {
        double x;
        const uint32_t k = rnd(10);
        if (k == 0) x = edge[rnd((uint32_t)edge.size())];
        else if (k == 1) x = bitsd(rng());
        else if (k == 2) x = (double)(int32_t)rng() / 256.0;
        else if (k == 3) x = (double)(int32_t)rng() / 1000.0;
        else if (k == 4) x = (double)(float)((int32_t)rng() / 65536.0);
        else if (k == 5) x = (double)(int32_t)rng() + 0.5;
        else if (k == 6) x = ((double)(int64_t)rng()) * (1.0 / 4096.0) / 1048576.0;                // hasta ~2^41
        else if (k == 7) x = (double)(int64_t)(rng() & 0xFFFFFFFF00000000ull) + (double)(int32_t)(rng() % 3) - 1.0 + (rnd(2) ? 0.25 : -0.25); // palabra baja a 0 cerca de multiplos de 2^32
        else if (k == 8) x = bitsd(((uint64_t)(0x3FF - 5 + rnd(75)) << 52) | (rng() & 0xFFFFFFFFFFFFFull) | ((rng() & 1) << 63));
        else x = (double)(float)(rnd(2000000) / 3.0 - 333333.0);
        const int64_t a = exe(x), b = ExeCrt::FloatToInt64(x);
        counts++;
        if (a != b) { if (fails++ < 10) printf("DIFERENCIA ftol x=%016llx (%g) exe=%016llx ours=%016llx\n", (unsigned long long)dbits(x), x, (unsigned long long)a, (unsigned long long)b); }
    }
}

// ------------------------------------------------------------------ 0x82872C
static int g_errnoExe = 0;
struct Rec { int type; char name[16]; uint64_t a1, a2, rv; int calls; };
static Rec g_rec; static int g_matherrMode = 0;
static int __cdecl BenchMatherr(void* p) {
    struct E { int type; const char* name; double a1, a2, rv; };
    E* e = (E*)p;
    g_rec.type = e->type; strncpy(g_rec.name, e->name, 15); g_rec.name[15] = 0; g_rec.a1 = dbits(e->a1); g_rec.a2 = dbits(e->a2); g_rec.rv = dbits(e->rv); g_rec.calls++;
    if (g_matherrMode == 1) { e->rv = 123.5; return 1; }
    return 0;
}

static void patch_call(uintptr_t at, void* target) {
    // at apunta al opcode E8 de una `call rel32`
    const int32_t rel = (int32_t)((uintptr_t)target - (at + 5));
    memcpy((void*)(at + 1), &rel, 4);
}

static void run_err(uint64_t n, uint64_t& fails, uint64_t& counts) {
    set_pc24();
    uint8_t* mem = (uint8_t*)VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    // errno: mov eax, &g_errnoExe; ret   |   cookie: ret
    mem[0] = 0xB8; uint32_t pe = (uint32_t)(uintptr_t)&g_errnoExe; memcpy(mem + 1, &pe, 4); mem[5] = 0xC3; mem[8] = 0xC3;
    // las llamadas a 0x82B181 y 0x82AAD4 dentro de 0x82872C
    int patched = 0;
    for (uintptr_t p = 0x82872C; p < 0x82872C + 660; p++) {
        if (*(uint8_t*)p != 0xE8) continue;
        const int32_t rel = *(int32_t*)(p + 1);
        const uintptr_t tgt = p + 5 + rel;
        if (tgt == 0x82B181) { patch_call(p, mem); patched++; }
        else if (tgt == 0x82AAD4) { patch_call(p, mem + 8); patched++; }
    }
    if (patched != 3) printf("aviso: llamadas parcheadas = %d (se esperaban 3)\n", patched);
    typedef void(__cdecl* Fn)(double*, double*, double*, int);
    Fn exe = (Fn)0x82872C, ours = ExeCrt::MathError;
    const uintptr_t savedPtr = *(uintptr_t*)0x8E3128;
    std::vector<int> codes;
    for (int c = 0; c < 0x400; c++) codes.push_back(c);
    codes.push_back(0x3EF); codes.push_back(0x3F0); codes.push_back(0x7FFFFFFF); codes.push_back(-1);
    for (uint64_t it = 0; it < n; it++) {
        const int code = codes[it % codes.size()];
        const uint32_t k = rnd(8);
        double x, y, r;
        auto pick = [&]() -> double {
            const uint32_t q = rnd(8);
            if (q == 0) return bitsd(0x7FF4000000000001ull);          // NaN senalizador
            if (q == 1) return bitsd(0x7FF8000000000000ull);
            if (q == 2) return bitsd(0xFFF0000000000000ull);
            if (q == 3) return 0.0;
            if (q == 4) return bitsd(rng());
            return (double)(int32_t)rng() / 997.0;
        };
        x = pick(); y = pick(); r = pick();
        const int variant = (int)rnd(3); // 0 punteros distintos, 1 los tres iguales (como RaiseDomain), 2 arg1 == resultado
        const int mode = (int)rnd(3);    // 0 matherr por defecto del EXE, 1 matherr del banco que trata el error, 2 matherr del banco que no lo trata
        (void)k;
        if (mode == 0) *(uintptr_t*)0x8E3128 = savedPtr; else { *(uintptr_t*)0x8E3128 = (uintptr_t)&BenchMatherr; g_matherrMode = mode == 1 ? 1 : 0; }
        double ax = x, ay = y, ar = r, bx = x, by = y, br = r;
        double *pax = &ax, *pay = &ay, *par = &ar, *pbx = &bx, *pby = &by, *pbr = &br;
        if (variant == 1) { pay = pax; par = pax; pby = pbx; pbr = pbx; }
        else if (variant == 2) { par = pax; pbr = pbx; }
        g_errnoExe = 0; g_errnoOurs = 0; memset(&g_rec, 0, sizeof g_rec);
        exe(pax, pay, par, code);
        const Rec re = g_rec; const int ee = g_errnoExe; const uint64_t exeX = dbits(ax), exeY = dbits(ay), exeR = dbits(ar);
        memset(&g_rec, 0, sizeof g_rec);
        ours(pbx, pby, pbr, code);
        const Rec ro = g_rec; const int eo = g_errnoOurs;
        counts++;
        const bool same = re.calls == ro.calls && re.type == ro.type && strcmp(re.name, ro.name) == 0 && re.a1 == ro.a1 && re.a2 == ro.a2 && re.rv == ro.rv &&
                          ee == eo && exeX == dbits(bx) && exeY == dbits(by) && exeR == dbits(br);
        if (!same) {
            if (fails++ < 10) printf("DIFERENCIA err codigo=%x variante=%d modo=%d: exe calls=%d tipo=%d '%s' errno=%d res=%016llx | ours calls=%d tipo=%d '%s' errno=%d res=%016llx\n", code, variant, mode,
                                     re.calls, re.type, re.name, ee, (unsigned long long)exeR, ro.calls, ro.type, ro.name, eo, (unsigned long long)dbits(br));
        }
    }
    *(uintptr_t*)0x8E3128 = savedPtr;
    // matherr por defecto: 0x833605 devuelve 0 igual que el propio
    typedef int(__cdecl* MFn)(void*);
    counts++;
    if (((MFn)0x833605)(nullptr) != ExeCrt::DefaultMatherr(nullptr)) { fails++; printf("DIFERENCIA matherr por defecto\n"); }
}

// ------------------------------------------------------------------ ramas x87 de floor/ceil (0x828A60, 0x82BA31) frente a ExeCrt::Floor/Ceil sin SSE2 (0xC9C400 == 0)
// El EXE llama a _errno (0x82B181) y a la comprobacion de cookie (0x82AAD4) dentro de las ramas x87 y de su manejador de errores (0x82E53A..0x82E700): se parchean
// en la copia mapeada, como en run_err. Se comparan el resultado (bits) y errno, con [0x8E3AE0] = 0x9876 (arranque del EXE) y 0, y la x87 a 24 y 53 bits.
static void run_x87(uint64_t n, uint64_t& fails, uint64_t& counts) {
    uint8_t* mem = (uint8_t*)VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    mem[0] = 0xB8; uint32_t pe = (uint32_t)(uintptr_t)&g_errnoExe; memcpy(mem + 1, &pe, 4); mem[5] = 0xC3; mem[8] = 0xC3;
    const uintptr_t ranges[3][2] = {{0x828A60, 0x828C40}, {0x82BA31, 0x82BC10}, {0x82E53A, 0x82E700}};
    int patched = 0;
    for (auto& r : ranges) {
        for (uintptr_t p = r[0]; p < r[1]; p++) {
            if (*(uint8_t*)p != 0xE8) continue;
            const uintptr_t tgt = p + 5 + *(int32_t*)(p + 1);
            if (tgt == 0x82B181) { patch_call(p, mem); patched++; }
            else if (tgt == 0x82AAD4) { patch_call(p, mem + 8); patched++; }
        }
    }
    printf("x87: llamadas parcheadas = %d\n", patched);
    *(int*)0xC9C400 = 0; // sin SSE2: ExeCrt::Floor/Ceil entran por la rama sustituida
    typedef double(__cdecl* DFn)(double);
    const DFn exeFn[2] = {(DFn)0x828A60, (DFn)0x82BA31};
    const DFn oursFn[2] = {ExeCrt::Floor, ExeCrt::Ceil};
    const uintptr_t savedPtr = *(uintptr_t*)0x8E3128;
    const int savedFlag = *(int*)0x8E3AE0;
    auto pick = [&]() -> double {
        const uint32_t q = rnd(16);
        switch (q) {
        case 0: return 0.0; case 1: return -0.0;
        case 2: return bitsd(0x7FF0000000000000ull); case 3: return bitsd(0xFFF0000000000000ull);
        case 4: return bitsd(0x7FF8000000000000ull); case 5: return bitsd(0x7FF4000000000001ull);
        case 6: return bitsd(0xFFF8000000000123ull);
        case 7: return (double)(int32_t)rng();                                  // entero
        case 8: return (double)(int32_t)rng() + 0.5;                            // mitad
        case 9: return bitsd(rng() & 0x800FFFFFFFFFFFFFull);                    // subnormal
        case 10: return bitsd(((uint64_t)(rnd(0x7FF)) << 52) | (rng() & 0xFFFFFFFFFFFFFull) | ((rng() & 1) << 63)); // cualquier exponente
        case 11: return ldexp((double)(int64_t)rng(), -(int)rnd(80));           // 2^52 y alrededores
        default: return (double)(int32_t)rng() / 997.0;
        }
    };
    unsigned c; _controlfp_s(&c, _PC_53, _MCW_PC);
    for (uint64_t it = 0; it < n; it++) {
        const int w = (int)(it & 1);
        const double x = pick();
        const int mode = 0;
        *(int*)0x8E3AE0 = rnd(2) ? 0x9876 : 0; // 0x9876 es el valor de arranque del EXE; con 0 la rama x87 pasa por 0x82E5AF (matherr por defecto 0x833605, errno despues)
        if (rnd(2)) _controlfp_s(&c, _PC_24, _MCW_PC); else _controlfp_s(&c, _PC_53, _MCW_PC);
        g_errnoExe = 0; g_errnoOurs = 0; memset(&g_rec, 0, sizeof g_rec);
        const double re = exeFn[w](x);
        const Rec rx = g_rec; const int ee = g_errnoExe;
        memset(&g_rec, 0, sizeof g_rec);
        const double ro = oursFn[w](x);
        const Rec ry = g_rec; const int eo = g_errnoOurs;
        counts++;
        (void)rx; (void)ry;
        const bool same = dbits(re) == dbits(ro) && ee == eo; // el matherr por defecto no deja rastro: solo se observan el resultado (bits) y errno
        if (!same && fails++ < 12) {
            printf("DIFERENCIA %s x=%016llx modo=%d [3AE0]=%d: exe res=%016llx errno=%d calls=%d tipo=%d '%s' | ours res=%016llx errno=%d calls=%d tipo=%d '%s'\n", w ? "ceil" : "floor", (unsigned long long)dbits(x), mode,
                   *(int*)0x8E3AE0, (unsigned long long)dbits(re), ee, rx.calls, rx.type, rx.name, (unsigned long long)dbits(ro), eo, ry.calls, ry.type, ry.name);
        }
    }
    *(uintptr_t*)0x8E3128 = savedPtr; *(int*)0x8E3AE0 = savedFlag;
}

int main(int argc, char** argv) {
    if (argc < 2) { printf("uso: bench_p3.exe <gta_sa.exe> [--casos N] [--semilla S] [--solo vec|mat|cw|err|ftol|x87]\n"); return 2; }
    uint64_t n = 200000, seed = 12345; std::string only;
    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "--casos") && i + 1 < argc) n = strtoull(argv[++i], 0, 10);
        else if (!strcmp(argv[i], "--semilla") && i + 1 < argc) seed = strtoull(argv[++i], 0, 10);
        else if (!strcmp(argv[i], "--solo") && i + 1 < argc) only = argv[++i];
    }
    setvbuf(stdout, nullptr, _IONBF, 0);
    rng.seed(seed);
    if (!map_exe(argv[1])) return 2;
    uint64_t total = 0;
    if (only.empty() || only == "vec") {
        uint64_t f[4] = {}, c[4] = {}; run_vec(n, f, c);
        printf("vectores: puntos %llu (dif %llu), vectores %llu (dif %llu), punto %llu (dif %llu), vector %llu (dif %llu)\n", (unsigned long long)c[0], (unsigned long long)f[0], (unsigned long long)c[1], (unsigned long long)f[1],
               (unsigned long long)c[2], (unsigned long long)f[2], (unsigned long long)c[3], (unsigned long long)f[3]);
        total += f[0] + f[1] + f[2] + f[3];
    }
    if (only.empty() || only == "mat") { uint64_t f = 0, c = 0; run_mat(n, f, c); printf("matrices: %llu casos, %llu diferencias\n", (unsigned long long)c, (unsigned long long)f); total += f; }
    if (only.empty() || only == "cw") { uint64_t f = 0, c = 0; run_cw(f, c); printf("palabra de control x87: %llu casos, %llu diferencias\n", (unsigned long long)c, (unsigned long long)f); total += f; }
    if (only.empty() || only == "ftol") { uint64_t f = 0, c = 0; run_ftol(n, f, c); printf("_ftol2 (0x821B40): %llu casos, %llu diferencias\n", (unsigned long long)c, (unsigned long long)f); total += f; }
    if (only.empty() || only == "err") { uint64_t f = 0, c = 0; run_err(n, f, c); printf("0x82872C + matherr: %llu casos, %llu diferencias\n", (unsigned long long)c, (unsigned long long)f); total += f; }
    if (only == "x87") { uint64_t f = 0, c = 0; run_x87(n, f, c); printf("ramas x87 de floor/ceil: %llu casos, %llu diferencias\n", (unsigned long long)c, (unsigned long long)f); total += f; }
    printf(total ? "RESULTADO: HAY DIFERENCIAS (%llu)\n" : "RESULTADO: SIN DIFERENCIAS\n", (unsigned long long)total);
    return total ? 1 : 0;
}
