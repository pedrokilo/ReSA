// Banco CRT1A: pares EXE original vs C++ reconstruido para qsort (0x8247E0) y las funciones de cadenas del CRT (strlen, strcmp, strncmp, strcpy, strcat, strncpy, strncat, strstr).
// Mismo metodo que FnLIB1A: el gta_sa.exe original (solo lectura) se mapea en 0x400000 dentro de este proceso de 32 bits (reserva_imagen.asm) y sus funciones se llaman por su direccion;
// el juego no se ejecuta. Cada caso se ejecuta DOS veces con las mismas entradas y en la misma direccion de memoria (una vez el EXE, otra el C++) y se compara: valor devuelto y toda la arena
// (qsort ademas: la secuencia completa de llamadas al comparador (par de posiciones) y el contenido final del vector).
// Uso: bench_crt.exe <ruta del EXE> [--casos N] [--semilla S] [--solo qsort|cadenas]   (devuelve 0 si todo coincide)
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

#define CRT_BENCH
#include "CrtQsort.cpp"
#include "CrtString.cpp"
#include "CrtHeap.cpp"

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

static uint64_t g_fnv(const void* p, size_t n, uint64_t h = 1469598103934665603ull) { auto b = (const uint8_t*)p; for (size_t i = 0; i < n; i++) { h ^= b[i]; h *= 1099511628211ull; } return h; }

// ------------------------------------------------------------------ qsort
typedef void(__cdecl* QsortFn)(void*, unsigned, unsigned, int(__cdecl*)(const void*, const void*));
static std::vector<uint32_t> g_trace; static const uint8_t* g_tbase; static int g_kind, g_w; static const uint64_t TRACE_CAP = 1u << 22;
static uint64_t g_ncalls; static bool g_reales; // --reales: solo los tres comparadores reales del EXE

static int __cdecl cmp_dispatch(const void* a, const void* b) {
    if (g_trace.size() < TRACE_CAP) { g_trace.push_back((uint32_t)((const uint8_t*)a - g_tbase)); g_trace.push_back((uint32_t)((const uint8_t*)b - g_tbase)); }
    g_ncalls++;
    auto A = (const uint8_t*)a, B = (const uint8_t*)b;
    auto le = [&](const uint8_t* p) { uint32_t v = 0; for (int i = std::min(g_w, 4) - 1; i >= 0; i--) v = v << 8 | p[i]; return v; };
    switch (g_kind) {
    case 0: { uint32_t x = le(A), y = le(B); return x < y ? -1 : x > y ? 1 : 0; }
    case 1: { uint32_t x = le(A), y = le(B); return x < y ? 1 : x > y ? -1 : 0; }
    case 2: return (int)A[0] - (int)B[0];
    case 3: return (int)(A[0] & 7) - (int)(B[0] & 7);
    case 4: { uint64_t h = g_fnv(A, std::min(g_w, 8)); h = g_fnv(B, std::min(g_w, 8), h); return (int)(h % 3) - 1; }
    case 5: return 0;
    case 6: { uint32_t x = le(A), y = le(B); return (int)(x - y); }                       // diferencia con desborde: solo importa el signo
    case 7: return ((int(__cdecl*)(const void*, const void*))0x7C8720)(a, b);               // EXE: diferencia de bytes (ancho 1)
    case 8: return ((int(__cdecl*)(const void*, const void*))0x7567A0)(a, b);               // EXE: triangulos de 3 indices u16 (ancho 6)
    case 9: return ((int(__cdecl*)(const void*, const void*))0x758240)(a, b);               // EXE: dos u16 (ancho 8)
    }
    return 0;
}

static std::mt19937_64 rng;
static uint32_t rnd(uint32_t n) { return n ? (uint32_t)(rng() % n) : 0; }

static void run_qsort(uint64_t ncases, uint64_t& fails, uint64_t* bykind, uint64_t* bywidth_cnt, const int* widths, int nw) {
    QsortFn exe = (QsortFn)0x8247E0, ours = (QsortFn)ExeCrt::Qsort;
    const size_t ARENA = 1 << 20; uint8_t* arena = (uint8_t*)VirtualAlloc(nullptr, ARENA + 0x20000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    uint8_t* vec = arena + 0x18000;  // el vector que se ordena
    const size_t VECCAP = ARENA - 0x18000;
    std::vector<uint8_t> save, r_exe, r_our; std::vector<uint32_t> t_exe, t_our;
    for (uint64_t c = 0; c < ncases; c++) {
        int kind = (int)rnd(7), w = widths[rnd(nw)];
        int special = g_reales ? (int)rnd(3) : (int)rnd(40); // ~10 % usan comparadores reales del EXE con su ancho real
        if (special == 0) { kind = 7; w = 1; } else if (special == 1) { kind = 8; w = 6; } else if (special == 2) { kind = 9; w = 8; }
        unsigned num; uint32_t r = rnd(100);
        if (r < 45) num = rnd(20); else if (r < 70) num = 6 + rnd(8); else if (r < 90) num = rnd(300); else if (r < 98) num = rnd(3000); else num = 3000 + rnd(20000);
        if ((size_t)num * w > VECCAP - 64) num = (unsigned)((VECCAP - 64) / w);
        memset(arena, 0xCD, 0x18000); memset(vec, 0xCD, (size_t)num * w + 64);
        uint32_t pat = rnd(8), range = (rnd(3) == 0) ? 0xFFFFFFFFu : (1u + rnd(rnd(2) ? 4 : 40));
        for (unsigned i = 0; i < num; i++) {
            uint8_t* e = vec + (size_t)i * w;
            for (int b = 0; b < w; b++) e[b] = (uint8_t)rng();
            uint32_t key = (uint32_t)rng(); key = (range == 0xFFFFFFFFu) ? key : key % range;
            switch (pat) { case 1: key = i; break; case 2: key = num - i; break; case 3: key = i < num / 2 ? i : num - i; break; case 4: key = 7; break; default: break; }
            for (int b = 0; b < std::min(w, 4); b++) e[b] = (uint8_t)(key >> (8 * b));
            if (kind == 8) { for (int b = 0; b < 3; b++) { uint16_t v = (uint16_t)(range == 0xFFFFFFFFu ? (uint32_t)rng() : rnd(range)); memcpy(e + 2 * b, &v, 2); } }
            if (kind == 9) { uint16_t a = (uint16_t)(range == 0xFFFFFFFFu ? (uint32_t)rng() : rnd(range)); uint16_t b2 = (uint16_t)rnd(range == 0xFFFFFFFFu ? 5 : 3); memcpy(e, &a, 2); memcpy(e + 2, &b2, 2); }
        }
        save.assign(arena, arena + 0x18000 + (size_t)num * w + 64);
        g_kind = kind; g_w = w; g_tbase = vec;
        g_trace.clear(); g_ncalls = 0; exe(vec, num, w, cmp_dispatch);
        r_exe.assign(arena, arena + save.size()); t_exe = g_trace;
        memcpy(arena, save.data(), save.size());
        g_trace.clear(); ours(vec, num, w, cmp_dispatch);
        r_our.assign(arena, arena + save.size()); t_our = g_trace;
        bykind[kind]++; for (int i = 0; i < nw; i++) if (widths[i] == w) bywidth_cnt[i]++;
        if (r_exe != r_our || t_exe != t_our) {
            fails++;
            if (fails <= 5) printf("DIFERENCIA qsort: caso %llu kind=%d width=%d num=%u (llamadas al comparador EXE=%zu, propio=%zu, vector %s)\n", (unsigned long long)c, kind, w, num, t_exe.size() / 2, t_our.size() / 2, r_exe == r_our ? "igual" : "distinto");
            if (fails == 1 && num <= 12) { // detalle del primer fallo pequeño: pares comparados (indices de elemento)
                printf("  EXE   :"); for (size_t i = 0; i + 1 < t_exe.size(); i += 2) printf(" (%u,%u)", t_exe[i] / w, t_exe[i + 1] / w);
                printf("\n  propio:"); for (size_t i = 0; i + 1 < t_our.size(); i += 2) printf(" (%u,%u)", t_our[i] / w, t_our[i + 1] / w); printf("\n");
            }
        }
    }
    VirtualFree(arena, 0, MEM_RELEASE);
}

// ------------------------------------------------------------------ cadenas
static void run_strings(uint64_t ncases, uint64_t* fails, uint64_t* counts) {
    const size_t ARENA = 0x10000; uint8_t* arena = (uint8_t*)VirtualAlloc(nullptr, ARENA + 0x2000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    const size_t SRC = 0x1000, DST = 0x4000;
    std::vector<uint8_t> save, r1, r2;
    auto mkstr = [&](size_t at, size_t len, uint32_t alpha) { for (size_t i = 0; i < len; i++) arena[at + i] = (uint8_t)(alpha ? 'a' + rnd(alpha) : 1 + rnd(255)); arena[at + len] = 0; };
    for (uint64_t c = 0; c < ncases; c++) {
        int fn = (int)(c % 8); uint32_t alpha = rnd(3) == 0 ? 0 : (1 + rnd(3)); // alfabetos pequenos: muchas coincidencias parciales
        memset(arena, 0xCD, ARENA + 0x2000);
        size_t la = rnd(4) == 0 ? rnd(300) : rnd(24), lb = rnd(4) == 0 ? rnd(300) : rnd(24);
        size_t a = SRC + rnd(64), b = SRC + 0x800 + rnd(64);
        mkstr(a, la, alpha);
        mkstr(b, lb, alpha);
        if (rnd(3) == 0 && lb) { // b comparte prefijo con a, o es subcadena de a
            size_t k = rnd((uint32_t)std::min(la, lb) + 1); memcpy(arena + b, arena + a, k); if (rnd(2)) arena[b + k] = 0;
            if (fn == 7 && rnd(2) && la) { size_t s = rnd((uint32_t)la), l = rnd((uint32_t)(la - s) + 1); memcpy(arena + b, arena + a + s, l); arena[b + l] = 0; }
        }
        size_t d = DST + rnd(64); size_t dl = rnd(40);
        mkstr(d, dl, alpha); // contenido previo del destino (strcat / strncat)
        for (size_t i = 0; i < 400; i++) if (rnd(2)) arena[d + dl + 1 + i] = (uint8_t)rng(); // basura tras el cero
        uint32_t n = rnd(4) == 0 ? rnd(400) : rnd(40);
        save.assign(arena, arena + ARENA);
        const uintptr_t base = (uintptr_t)arena;
        auto norm = [&](uintptr_t r) -> uint32_t { return (r >= base && r < base + ARENA) ? 0x80000000u + (uint32_t)(r - base) : (uint32_t)r; };
        auto call = [&](bool exe) -> uintptr_t {
            char* A = (char*)arena + a; char* B = (char*)arena + b; char* D = (char*)arena + d;
            switch (fn) {
            case 0: return exe ? ((size_t(__cdecl*)(const char*))0x826330)(A) : ExeCrt::Strlen(A);
            case 1: return exe ? (uintptr_t)((int(__cdecl*)(const char*, const char*))0x8263C0)(A, B) : (uintptr_t)ExeCrt::Strcmp(A, B);
            case 2: return exe ? (uintptr_t)((int(__cdecl*)(const char*, const char*, unsigned))0x8214D0)(A, B, n) : (uintptr_t)ExeCrt::Strncmp(A, B, n);
            case 3: return exe ? (uintptr_t)((char*(__cdecl*)(char*, const char*))0x826590)(D, A) : (uintptr_t)ExeCrt::Strcpy(D, A);
            case 4: return exe ? (uintptr_t)((char*(__cdecl*)(char*, const char*))0x8265A0)(D, A) : (uintptr_t)ExeCrt::Strcat(D, A);
            case 5: return exe ? (uintptr_t)((char*(__cdecl*)(char*, const char*, unsigned))0x821F40)(D, A, n) : (uintptr_t)ExeCrt::Strncpy(D, A, n);
            case 6: return exe ? (uintptr_t)((char*(__cdecl*)(char*, const char*, unsigned))0x826450)(D, A, n) : (uintptr_t)ExeCrt::Strncat(D, A, n);
            default: return exe ? (uintptr_t)((char*(__cdecl*)(const char*, const char*))0x822650)(A, B) : (uintptr_t)ExeCrt::Strstr(A, B);
            }
        };
        uintptr_t re = call(true); r1.assign(arena, arena + ARENA);
        memcpy(arena, save.data(), ARENA);
        uintptr_t ro = call(false); r2.assign(arena, arena + ARENA);
        counts[fn]++;
        if (norm(re) != norm(ro) || r1 != r2) {
            fails[fn]++;
            if (fails[fn] <= 3) printf("DIFERENCIA cadenas fn=%d caso %llu: ret EXE=%08x propio=%08x, arena %s (la=%zu lb=%zu n=%u)\n", fn, (unsigned long long)c, (unsigned)norm(re), (unsigned)norm(ro), r1 == r2 ? "igual" : "distinta", la, lb, n);
        }
    }
    VirtualFree(arena, 0, MEM_RELEASE);
}

// ------------------------------------------------------------------ asignadores (malloc, calloc, realloc, free, operator new/delete) con __active_heap == 1
// Las funciones del EXE leen _crtheap (0xC9C2F4), __active_heap (0xC9C2F8), _newmode (0xC9ADCC) y el manejador de new (0xC9AC68) de su propia imagen y llaman a HeapAlloc/HeapReAlloc/HeapFree por la
// IAT (0x8581F8/0x8581F4/0x8581FC; aqui se rellenan con las funciones reales del sistema: el cargador del EXE no las resuelve). Cada implementacion trabaja en SU montón (dos montones identicos):
// se cambia el manejador global antes de cada llamada. Se compara: nulo/no nulo, HeapSize del bloque (prueba el tamaño pedido a HeapAlloc/HeapReAlloc), contenido (calloc a cero, realloc conserva),
// numero de llamadas al manejador de new, y la validez de los dos montones al final de cada secuencia.
static int g_handler_mode, g_handler_calls, g_handler_limit;
static int __cdecl new_handler(unsigned) { g_handler_calls++; return g_handler_mode == 2 && g_handler_calls <= g_handler_limit ? 1 : 0; }
typedef void*(__cdecl* PMalloc)(unsigned); typedef void(__cdecl* PFree)(void*); typedef void*(__cdecl* PRealloc)(void*, unsigned); typedef void*(__cdecl* PCalloc)(unsigned, unsigned);
struct Blk { uint8_t* p; size_t req; };
struct HeapImpl { PMalloc mal; PFree fr; PRealloc re; PCalloc ca; PMalloc nw; PFree del; HANDLE heap; std::vector<Blk> pool; };
static void run_heap(uint64_t nseq, uint64_t& fails, uint64_t* opcount) {
    *(void**)0x8581F4 = (void*)&HeapReAlloc; *(void**)0x8581F8 = (void*)&HeapAlloc; *(void**)0x8581FC = (void*)&HeapFree;
    *(int*)0xC9C2F8 = 1;
    HeapImpl H[2] = {{(PMalloc)0x824257, (PFree)0x82413F, (PRealloc)0x824269, (PCalloc)0x824416, (PMalloc)0x82119A, (PFree)0x8214BD, HeapCreate(0, 0x1000, 0), {}},
                     {(PMalloc)ExeCrt::Malloc, (PFree)ExeCrt::Free, (PRealloc)ExeCrt::Realloc, (PCalloc)ExeCrt::Calloc, (PMalloc)ExeCrt::OperatorNew, (PFree)ExeCrt::Free, HeapCreate(0, 0x1000, 0), {}}};
    auto report = [&](const char* what, uint64_t seq, int op) { fails++; if (fails <= 8) printf("DIFERENCIA montón: %s (secuencia %llu, operacion %d)\n", what, (unsigned long long)seq, op); };
    auto setheap = [&](int i) { *(HANDLE*)0xC9C2F4 = H[i].heap; };
    auto sizeof_blk = [&](int i, void* p) -> SIZE_T { setheap(i); return HeapSize(H[i].heap, 0, p); };
    auto fillb = [](void* p, size_t n, uint64_t seed) { for (size_t i = 0; i < n; i++) ((uint8_t*)p)[i] = (uint8_t)(i * 31 + seed); };
    for (uint64_t sq = 0; sq < nseq; sq++) {
        g_handler_mode = (int)rnd(3); g_handler_limit = (int)rnd(4);
        *(int*)0xC9ADCC = g_handler_mode ? 1 : 0; *(void**)0xC9AC68 = g_handler_mode ? (void*)new_handler : nullptr;
        int nops = 1 + (int)rnd(60);
        for (int k = 0; k < nops; k++) {
            int op = (int)rnd(7); uint32_t sz; uint32_t r = rnd(100);
            if (r < 55) sz = rnd(64); else if (r < 85) sz = rnd(8192); else if (r < 93) sz = rnd(1 << 20); else if (r < 96) sz = 0xFFFFFFE0u + rnd(32); else if (r < 98) sz = 0x7FFFFFF0u + rnd(32); else sz = 0x80000000u + rnd(0x7FFFFF00u);
            if (op == 5 && sz > (1u << 20)) sz = rnd(8192); // operator new solo en la ruta de exito (sin memoria y sin manejador lanza una excepcion)
            opcount[op]++;
            uint32_t idx = H[0].pool.empty() ? 0 : rnd((uint32_t)H[0].pool.size()); uint32_t cb = 1 + rnd(5); uint64_t seed = rng();
            uint8_t* res[2] = {nullptr, nullptr}; int calls[2] = {0, 0}; size_t oldreq = 0; bool hadblk = false;
            for (int i = 0; i < 2; i++) {
                setheap(i); g_handler_calls = 0; void* p = nullptr; HeapImpl& h = H[i];
                bool has = !h.pool.empty() && h.pool[idx].p;
                if (i == 0) { hadblk = has; oldreq = has ? h.pool[idx].req : 0; }
                switch (op) {
                case 0: p = h.mal(sz); break;
                case 1: p = h.ca(sz / cb, cb); break;
                case 2: p = h.re(has ? h.pool[idx].p : nullptr, sz); break;
                case 3: h.fr(has ? h.pool[idx].p : nullptr); break;
                case 4: h.del(has ? h.pool[idx].p : nullptr); break;
                case 5: p = h.nw(sz); break;
                case 6: p = h.mal(0); break;
                }
                res[i] = (uint8_t*)p; calls[i] = g_handler_calls;
            }
            if ((res[0] == nullptr) != (res[1] == nullptr) && op != 3 && op != 4) { report("uno devuelve nulo y el otro no", sq, op); }
            if (calls[0] != calls[1]) { report("llamadas distintas al manejador de new", sq, op); }
            size_t reqsz = op == 1 ? (size_t)(sz / cb) * cb : op == 6 ? 0 : sz;
            if (res[0] && res[1]) {
                SIZE_T s0 = sizeof_blk(0, res[0]), s1 = sizeof_blk(1, res[1]);
                if (s0 != s1) report("HeapSize distinto (tamaño pedido a HeapAlloc/HeapReAlloc)", sq, op);
                size_t n = std::min<size_t>(s0, 512);
                if (op == 1) for (size_t i = 0; i < n; i++) if (res[0][i] != 0 || res[1][i] != 0) { report("calloc no deja el bloque a cero", sq, op); break; }
                if (op == 2 && hadblk) { size_t keep = std::min<size_t>(std::min<size_t>(oldreq, sz), 512); if (keep && memcmp(res[0], res[1], keep) != 0) report("realloc no conserva el mismo contenido", sq, op); }
                for (int i = 0; i < 2; i++) fillb(res[i], std::min<size_t>(i ? s1 : s0, 4096), seed);
            }
            // actualizar los pools de forma identica
            for (int i = 0; i < 2; i++) {
                HeapImpl& h = H[i];
                if (op == 0 || op == 1 || op == 5 || op == 6) h.pool.push_back({res[i], reqsz});
                else if (op == 2) { if (h.pool.empty() || !h.pool[idx].p) h.pool.push_back({res[i], reqsz}); else if (res[i]) h.pool[idx] = {res[i], (size_t)sz}; else if (sz == 0) h.pool[idx] = {nullptr, 0}; }
                else if ((op == 3 || op == 4) && !h.pool.empty()) h.pool[idx] = {nullptr, 0};
            }
            if (H[0].pool.size() > 300) for (int i = 0; i < 2; i++) { setheap(i); for (auto& e : H[i].pool) if (e.p) H[i].fr(e.p); H[i].pool.clear(); }
        }
        for (int i = 0; i < 2; i++) { setheap(i); for (auto& e : H[i].pool) if (e.p) H[i].fr(e.p); H[i].pool.clear(); if (!HeapValidate(H[i].heap, 0, nullptr)) report("montón corrupto", sq, -1); }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) { printf("uso: bench_crt <gta_sa.exe> [--casos N] [--semilla S] [--solo qsort|cadenas]\n"); return 2; }
    uint64_t n = 60000, seed = 12345; std::string solo;
    for (int i = 2; i < argc; i++) { if (!strcmp(argv[i], "--casos") && i + 1 < argc) n = _strtoui64(argv[++i], 0, 10); else if (!strcmp(argv[i], "--semilla") && i + 1 < argc) seed = _strtoui64(argv[++i], 0, 10); else if (!strcmp(argv[i], "--reales")) g_reales = true; else if (!strcmp(argv[i], "--solo") && i + 1 < argc) solo = argv[++i]; }
    if (!map_exe(argv[1])) return 2;
    rng.seed(seed); int rc = 0;
    if (solo.empty() || solo == "qsort") {
        static const int widths[] = {1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 24, 33}; const int nw = 12;
        uint64_t fails = 0, bykind[16] = {}, bywidth[16] = {};
        run_qsort(n, fails, bykind, bywidth, widths, nw);
        printf("qsort: %llu casos, %llu diferencias\n  por comparador:", (unsigned long long)n, (unsigned long long)fails);
        const char* kn[] = {"u32 asc", "u32 desc", "byte0", "byte0&7", "inconsistente", "todo igual", "diferencia c/desborde", "EXE 7C8720", "EXE 7567A0", "EXE 758240"};
        for (int k = 0; k < 10; k++) printf(" %s=%llu", kn[k], (unsigned long long)bykind[k]);
        printf("\n  por ancho:"); for (int i = 0; i < nw; i++) printf(" %d=%llu", widths[i], (unsigned long long)bywidth[i]); printf("\n");
        if (fails) rc = 1;
    }
    if (solo.empty() || solo == "cadenas") {
        uint64_t fails[8] = {}, counts[8] = {}; const char* nm[] = {"strlen", "strcmp", "strncmp", "strcpy", "strcat", "strncpy", "strncat", "strstr"};
        run_strings(n * 2, fails, counts);
        for (int i = 0; i < 8; i++) { printf("%s: %llu pares, %llu diferencias\n", nm[i], (unsigned long long)counts[i], (unsigned long long)fails[i]); if (fails[i]) rc = 1; }
    }
    if (solo.empty() || solo == "monton") {
        uint64_t fails = 0, oc[8] = {};
        run_heap(n / 4, fails, oc);
        printf("montón (malloc/calloc/realloc/free/new/delete): %llu secuencias; operaciones malloc=%llu calloc=%llu realloc=%llu free=%llu delete=%llu new=%llu malloc(0)=%llu; %llu diferencias\n", (unsigned long long)(n / 4),
               (unsigned long long)oc[0], (unsigned long long)oc[1], (unsigned long long)oc[2], (unsigned long long)oc[3], (unsigned long long)oc[4], (unsigned long long)oc[5], (unsigned long long)oc[6], (unsigned long long)fails);
        if (fails) rc = 1;
    }
    printf(rc ? "RESULTADO: HAY DIFERENCIAS\n" : "RESULTADO: SIN DIFERENCIAS\n");
    return rc;
}
