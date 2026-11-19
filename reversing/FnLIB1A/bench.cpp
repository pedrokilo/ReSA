// Banco LIB1A: pares EXE original vs zlib 1.1.3 compilada desde la fuente oficial (libs/zlib-1.1.3), para las 14 funciones de inflate que el EXE lleva en 0x7E9430..0x7EB260.
//
// El EXE original (solo lectura) se mapea en 0x400000 dentro de este proceso de 32 bits (sin ejecutar el juego): son funciones puras de enteros (sin x87, sin CRT, sin estado global:
// solo leen tablas de .rdata/.data y llaman a zalloc/zfree/checkfn por punteros), asi que se pueden llamar directamente por su direccion. Cada caso se ejecuta DOS veces con exactamente las mismas
// entradas y la misma arena de memoria (misma direccion): una con las funciones del EXE y otra con las de la biblioteca; se comparan, tras cada llamada: valor devuelto, campos de z_stream
// (punteros normalizados), memoria de la arena (estado, ventana, tablas de Huffman, bloques liberados), salida y mensaje de error (por contenido).
// Oraculos independientes (gen_cases.py, modulo zlib de Python): salida de flujos validos (SHA-256 y adler32), clasificacion de las longitudes de codigo de Huffman (Kraft) y el modelo de inflate_flush.
//
// Uso:  bench.exe <carpeta de casos> <ruta del EXE> [--filtro nombre] [--max N]   (devuelve 0 si todo coincide)
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
#include <functional>

extern "C" {
#include "zutil.h"
#include "infblock.h"
#include "inftrees.h"
#include "infcodes.h"
#include "infutil.h"
#include "inffast.h"
int huft_build(uIntf*, uInt, uInt, const uIntf*, const uIntf*, inflate_huft**, uIntf*, inflate_huft*, uInt*, uIntf*); // inftrees.c compilado con -Dlocal=
}

static_assert(sizeof(z_stream) == 0x38 && sizeof(inflate_blocks_statef) == 0x40 && sizeof(inflate_huft) == 8, "disposicion de estructuras");

// ---------------------------------------------------------------- SHA-256
struct Sha256 {
    uint32_t h[8]; uint8_t buf[64]; uint64_t len; size_t fill;
    static uint32_t rr(uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }
    void init() { static const uint32_t i0[8] = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}; memcpy(h, i0, 32); len = 0; fill = 0; }
    void block(const uint8_t* p) {
        static const uint32_t k[64] = {0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
        uint32_t w[64];
        for (int i = 0; i < 16; i++) w[i] = (uint32_t)p[4*i] << 24 | (uint32_t)p[4*i+1] << 16 | (uint32_t)p[4*i+2] << 8 | p[4*i+3];
        for (int i = 16; i < 64; i++) { uint32_t s0 = rr(w[i-15],7) ^ rr(w[i-15],18) ^ (w[i-15] >> 3), s1 = rr(w[i-2],17) ^ rr(w[i-2],19) ^ (w[i-2] >> 10); w[i] = w[i-16] + s0 + w[i-7] + s1; }
        uint32_t a=h[0],b=h[1],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],hh=h[7];
        for (int i = 0; i < 64; i++) {
            uint32_t S1 = rr(e,6) ^ rr(e,11) ^ rr(e,25), ch = (e & f) ^ (~e & g), t1 = hh + S1 + ch + k[i] + w[i];
            uint32_t S0 = rr(a,2) ^ rr(a,13) ^ rr(a,22), mj = (a & b) ^ (a & c) ^ (b & c), t2 = S0 + mj;
            hh = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
        }
        h[0]+=a; h[1]+=b; h[2]+=c; h[3]+=d; h[4]+=e; h[5]+=f; h[6]+=g; h[7]+=hh;
    }
    void update(const uint8_t* p, size_t n) { len += n; while (n) { size_t t = std::min(n, 64 - fill); memcpy(buf + fill, p, t); fill += t; p += t; n -= t; if (fill == 64) { block(buf); fill = 0; } } }
    void final(uint8_t out[32]) {
        uint64_t bits = len * 8; uint8_t pad = 0x80; update(&pad, 1); uint8_t z = 0; while (fill != 56) update(&z, 1);
        uint8_t l[8]; for (int i = 0; i < 8; i++) l[i] = (uint8_t)(bits >> (56 - 8 * i)); update(l, 8);
        for (int i = 0; i < 8; i++) { out[4*i] = h[i] >> 24; out[4*i+1] = h[i] >> 16; out[4*i+2] = h[i] >> 8; out[4*i+3] = (uint8_t)h[i]; }
    }
};
static void sha256(const uint8_t* p, size_t n, uint8_t out[32]) { Sha256 s; s.init(); s.update(p, n); s.final(out); }

// ---------------------------------------------------------------- mapa del EXE
static const char* EXE_PATH = nullptr;
static bool map_exe(const char* path) {
    FILE* f = fopen(path, "rb"); if (!f) { printf("no se puede abrir el EXE\n"); return false; }
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    std::vector<uint8_t> d(n); fread(d.data(), 1, n, f); fclose(f);
    auto nt = (IMAGE_NT_HEADERS32*)(d.data() + ((IMAGE_DOS_HEADER*)d.data())->e_lfanew);
    if (nt->OptionalHeader.ImageBase != 0x400000) return false;
    uint8_t digest[32]; sha256(d.data(), d.size(), digest);
    printf("EXE sha256 "); for (int i = 0; i < 32; i++) printf("%02x", digest[i]); printf("\n");
    // el espacio 0x401000..0xCB1000 lo reserva este ejecutable (reserva.c, seccion .aaa en 0x401000, enlazado en 0x400000); se abre y se rellena con las secciones del EXE original
    DWORD oldp; struct Win { uintptr_t lo, size; } wins[1] = {{0x401000, 0x8B0000}};
    if (!VirtualProtect((void*)0x401000, 0x8B0000, PAGE_EXECUTE_READWRITE, &oldp)) { printf("no se pudo abrir la reserva (error %lu)\n", GetLastError()); return false; }
    if ((uintptr_t)&map_exe < 0xCB1000 && (uintptr_t)&map_exe >= 0x400000) { printf("el codigo del banco esta dentro de la reserva\n"); return false; }
    void* base = (void*)0x400000;
    auto sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; i++) {
        uintptr_t sva = 0x400000 + sec[i].VirtualAddress; size_t vsz = sec[i].Misc.VirtualSize, rsz = std::min<size_t>(sec[i].SizeOfRawData, vsz);
        for (auto& w : wins) { // interseccion de la seccion con la ventana
            uintptr_t lo = std::max(sva, w.lo), hi = std::min(sva + vsz, w.lo + w.size); if (lo >= hi) continue;
            for (uintptr_t p = lo; p < hi; p++) *(uint8_t*)p = (p - sva < rsz) ? d[sec[i].PointerToRawData + (p - sva)] : 0;
        }
    }
    (void)base;
    return true;
}

// ---------------------------------------------------------------- tablas de funciones
typedef void  (__cdecl* t_reset)(inflate_blocks_statef*, z_streamp, uLongf*);
typedef inflate_blocks_statef* (__cdecl* t_new)(z_streamp, check_func, uInt);
typedef int   (__cdecl* t_blocks)(inflate_blocks_statef*, z_streamp, int);
typedef int   (__cdecl* t_free)(inflate_blocks_statef*, z_streamp);
typedef void  (__cdecl* t_setdict)(inflate_blocks_statef*, const Bytef*, uInt);
typedef inflate_codes_statef* (__cdecl* t_cnew)(uInt, uInt, inflate_huft*, inflate_huft*, z_streamp);
typedef int   (__cdecl* t_codes)(inflate_blocks_statef*, z_streamp, int);
typedef void  (__cdecl* t_cfree)(inflate_codes_statef*, z_streamp);
typedef int   (__cdecl* t_bits)(uIntf*, uIntf*, inflate_huft**, inflate_huft*, z_streamp);
typedef int   (__cdecl* t_huft)(uIntf*, uInt, uInt, const uIntf*, const uIntf*, inflate_huft**, uIntf*, inflate_huft*, uInt*, uIntf*);
typedef int   (__cdecl* t_dyn)(uInt, uInt, uIntf*, uIntf*, uIntf*, inflate_huft**, inflate_huft**, inflate_huft*, z_streamp);
typedef int   (__cdecl* t_fixed)(uIntf*, uIntf*, inflate_huft**, inflate_huft**, z_streamp);
typedef int   (__cdecl* t_flush)(inflate_blocks_statef*, z_streamp, int);
typedef int   (__cdecl* t_fast)(uInt, uInt, inflate_huft*, inflate_huft*, inflate_blocks_statef*, z_streamp);

struct Fns {
    const char* name;
    t_reset reset; t_new nw; t_blocks blocks; t_free fr; t_setdict setdict; t_cnew cnew; t_codes codes; t_cfree cfree;
    t_bits bits; t_huft huft; t_dyn dyn; t_fixed fixed; t_flush flush; t_fast fast;
};
static const char* FN_NAMES[14] = {"inflate_blocks_reset", "inflate_blocks_new", "inflate_blocks", "inflate_blocks_free", "inflate_set_dictionary", "inflate_codes_new", "inflate_codes",
                                   "inflate_codes_free", "inflate_trees_bits", "huft_build", "inflate_trees_dynamic", "inflate_trees_fixed", "inflate_flush", "inflate_fast"};
static const uint32_t FN_ADDR[14] = {0x7E9430, 0x7E94B0, 0x7E9570, 0x7EA1C0, 0x7EA250, 0x7EA2A0, 0x7EA2E0, 0x7EA9E0, 0x7EAA00, 0x7EAAB0, 0x7EAF60, 0x7EB0F0, 0x7EB120, 0x7EB260};
static Fns EXE_F, LIB_F;
static void init_fns() {
    EXE_F = Fns{"EXE", (t_reset)0x7E9430, (t_new)0x7E94B0, (t_blocks)0x7E9570, (t_free)0x7EA1C0, (t_setdict)0x7EA250, (t_cnew)0x7EA2A0, (t_codes)0x7EA2E0, (t_cfree)0x7EA9E0,
                (t_bits)0x7EAA00, (t_huft)0x7EAAB0, (t_dyn)0x7EAF60, (t_fixed)0x7EB0F0, (t_flush)0x7EB120, (t_fast)0x7EB260};
    LIB_F = Fns{"LIB", (t_reset)inflate_blocks_reset, (t_new)inflate_blocks_new, (t_blocks)inflate_blocks, (t_free)inflate_blocks_free, (t_setdict)inflate_set_dictionary,
                (t_cnew)inflate_codes_new, (t_codes)inflate_codes, (t_cfree)inflate_codes_free, (t_bits)inflate_trees_bits, (t_huft)huft_build, (t_dyn)inflate_trees_dynamic,
                (t_fixed)inflate_trees_fixed, (t_flush)inflate_flush, (t_fast)inflate_fast};
}
// Mutacion de la prueba negativa: el numero lo lee la biblioteca mutada (lib1a_mut_get); si no hay mutacion, 0.
static int g_mut = 0;

// ---------------------------------------------------------------- entorno determinista (arena, entrada, salida)
static int g_trace, g_dbg_codes = -1; static uint32_t g_last_bb; static uint64_t g_oob_cases; static int g_dump_idx = -1; static const char* g_dump_path = nullptr;
static const size_t ARENA = 8u << 20, GAP = 36864;
static uint8_t* g_base; // arena con relleno 0xCD
struct Alloc { size_t off, size; bool freed; };
static std::vector<Alloc> g_allocs;
static size_t g_brk, g_alloc_n, g_fail_at;
static uint64_t g_log;
static std::vector<std::pair<size_t, size_t>> g_dead_regions; // reservas cuyo contenido de tablas no esta definido (lectura fuera de cpdext[], ver README)
static std::vector<std::pair<size_t, size_t>> g_huft_regions; // (desplazamiento en la arena, entradas) con relleno de las entradas (bytes 2..3) sin definir
static uint8_t* g_in; static uint8_t* g_out;
static const size_t INCAP = 1 << 18, OUTCAP = 1 << 18;
static uint32_t g_fix_tl_exe = 0x8E0FC8, g_fix_td_exe = 0x8E1FC8, g_fix_tl_lib, g_fix_td_lib;

static inline uint64_t mix(uint64_t h, uint64_t v) { h ^= v + 0x9E3779B97F4A7C15ull + (h << 6) + (h >> 2); h *= 0xFF51AFD7ED558CCDull; return h ^ (h >> 29); }
static uint32_t norm(uint32_t v) {
    uint32_t a = (uint32_t)(uintptr_t)g_base;
    if (v >= a && v < a + ARENA) return 0xA0000000u + (v - a);
    if (v >= (uint32_t)(uintptr_t)g_in && v < (uint32_t)(uintptr_t)g_in + INCAP) return 0xB0000000u + (v - (uint32_t)(uintptr_t)g_in);
    if (v >= (uint32_t)(uintptr_t)g_out && v < (uint32_t)(uintptr_t)g_out + OUTCAP) return 0xC0000000u + (v - (uint32_t)(uintptr_t)g_out);
    if (v >= g_fix_tl_exe && v < g_fix_tl_exe + 0x1000) return 0xF1000000u + (v - g_fix_tl_exe);
    if (v >= g_fix_tl_lib && v < g_fix_tl_lib + 0x1000) return 0xF1000000u + (v - g_fix_tl_lib);
    if (v >= g_fix_td_exe && v < g_fix_td_exe + 0x100) return 0xF2000000u + (v - g_fix_td_exe);
    if (v >= g_fix_td_lib && v < g_fix_td_lib + 0x100) return 0xF2000000u + (v - g_fix_td_lib);
    return v;
}
static uint64_t hash_words(const void* p, size_t bytes, uint64_t h = 0x1234) {
    const uint32_t* w = (const uint32_t*)p; size_t n = bytes / 4;
    for (size_t i = 0; i < n; i++) h = mix(h, norm(w[i]));
    const uint8_t* b = (const uint8_t*)p + n * 4; for (size_t i = 0; i < (bytes & 3); i++) h = mix(h, b[i]);
    return h;
}
static uint64_t g_pad_diff; // informativo: pasos cuya unica diferencia es el relleno (bytes 2..3) de las entradas de las tablas de Huffman, sin definir en el original
static uint64_t hash_arena(bool masked = true) {
    uint64_t h = 0x77; std::vector<uint8_t> tmp;
    for (auto& a : g_allocs) {
        const uint8_t* p = g_base + a.off; size_t n = a.size;
        bool dead = false; for (auto& r : g_dead_regions) if (r.first == a.off) dead = true;
        if (dead) { h = mix(h, a.off); h = mix(h, a.size); h = mix(h, a.freed); continue; }
        bool has_region = false; if (masked) for (auto& r : g_huft_regions) if (r.first >= a.off && r.first < a.off + a.size) has_region = true;
        if (!has_region) { h = mix(h, a.off); h = mix(h, a.size); h = mix(h, a.freed); h = hash_words(p, n, h); continue; }
        tmp.assign(p, p + n);
        if (masked) for (auto& r : g_huft_regions) {
            if (r.first >= a.off && r.first < a.off + a.size) for (size_t e = 0; e < r.second && r.first + e * 8 + 8 <= a.off + a.size; e++) { tmp[r.first - a.off + e * 8 + 2] = 0; tmp[r.first - a.off + e * 8 + 3] = 0; }
        }
        h = mix(h, a.off); h = mix(h, a.size); h = mix(h, a.freed); h = hash_words(tmp.data(), n, h);
    }
    return h;
}
static uint64_t hash_huft(const inflate_huft* t, size_t n) { // tabla de Huffman con el relleno de la union `word` enmascarado
    uint64_t h = 0x55; if (!t) return mix(h, 1);
    for (size_t i = 0; i < n; i++) { const uint8_t* b = (const uint8_t*)&t[i]; h = mix(h, b[0]); h = mix(h, b[1]); h = mix(h, norm(*(const uint32_t*)(b + 4))); }
    return h;
}
static voidpf __cdecl zalloc_cb(voidpf, uInt items, uInt size) {
    g_alloc_n++; g_log = mix(g_log, (uint64_t)items << 32 | size);
    if (g_fail_at && g_alloc_n == g_fail_at) { g_log = mix(g_log, 0xFA11); return nullptr; }
    size_t bytes = (size_t)items * size;
    for (auto& a : g_allocs) if (a.freed && a.size == bytes) { a.freed = false; memset(g_base + a.off, 0xCD, a.size); g_log = mix(g_log, 0x4E05E | a.off); return g_base + a.off; }
    size_t off = (g_brk + GAP + 15) & ~(size_t)15;
    if (off + bytes + GAP > ARENA) { g_log = mix(g_log, 0xFA12); return nullptr; }
    g_brk = off + bytes; g_allocs.push_back({off, bytes, false}); return g_base + off;
}
static void __cdecl zfree_cb(voidpf, voidpf p) {
    uint32_t off = (uint32_t)((uint8_t*)p - g_base); g_log = mix(g_log, 0xF00000000ull | (p ? off : 0xFFFFFFFF));
    for (auto& a : g_allocs) if (a.off == off && !a.freed) { a.freed = true; memset(g_base + a.off, 0xDD, a.size); return; }
    g_log = mix(g_log, 0xBAD);
}
static uLong __cdecl check_cb(uLong adler, const Bytef* buf, uInt len) { return adler32(adler, buf, len); }

static void begin_side(size_t fail_at) {
    memset(g_base, 0xCD, ARENA); g_brk = 0; g_alloc_n = 0; g_fail_at = fail_at; g_log = 0; g_allocs.clear(); g_huft_regions.clear(); g_dead_regions.clear(); memset(g_out, 0xEE, OUTCAP); memset(g_in, 0xA7, INCAP);
}
static void zinit(z_stream& z) { memset(&z, 0, sizeof z); z.zalloc = zalloc_cb; z.zfree = zfree_cb; z.opaque = nullptr; z.msg = (char*)0x11111111; /* marcador: ninguna funcion lo ha tocado */ }

// ---------------------------------------------------------------- trazas
struct Step { int fn; uint32_t rc; uint64_t zh, ar, ar_raw, out, msg, ex, lg; };
struct Trace { std::vector<Step> v; std::vector<std::string> notes; };
static uint64_t hash_msg(const char* m) {
    if (!m) return 1; if ((uintptr_t)m == 0x11111111) return 2;
    __try { return hash_words(m, strlen(m)); } __except (EXCEPTION_EXECUTE_HANDLER) { return 3; }
}
static uint64_t hash_z(const z_stream& z) {
    uint64_t h = 0x99; h = mix(h, norm((uint32_t)(uintptr_t)z.next_in)); h = mix(h, z.avail_in); h = mix(h, z.total_in); h = mix(h, norm((uint32_t)(uintptr_t)z.next_out));
    h = mix(h, z.avail_out); h = mix(h, z.total_out); h = mix(h, norm((uint32_t)(uintptr_t)z.state)); h = mix(h, (uint32_t)(uintptr_t)z.zalloc); h = mix(h, (uint32_t)(uintptr_t)z.zfree);
    h = mix(h, (uint32_t)(uintptr_t)z.opaque); h = mix(h, (uint32_t)z.data_type); h = mix(h, z.adler); h = mix(h, z.reserved); return h;
}
static void push(Trace& t, int fn, uint32_t rc, z_stream& z, uint64_t ex = 0, bool out = true, size_t out_from = 0, size_t out_to = 0) {
    Step s; s.fn = fn; s.rc = rc; s.zh = hash_z(z); s.ar = hash_arena(); s.ar_raw = (t.v.size() % 8 == 0) ? hash_arena(false) : s.ar; s.msg = hash_msg(z.msg); s.ex = ex; s.lg = g_log;
    s.out = out ? hash_words(g_out + out_from, out_to - out_from) : 0; t.v.push_back(s);
}
static unsigned guarded(void (*fn)(void*), void* c) { __try { fn(c); return 0; } __except (EXCEPTION_EXECUTE_HANDLER) { return GetExceptionCode(); } }
template <class F> static void thunk(void* c) { (*(F*)c)(); }
template <class F> static unsigned guard(F&& f) { return guarded(&thunk<typename std::remove_reference<F>::type>, (void*)&f); }
#define CALLRC(rc, expr) do { int _r = 0; unsigned _e = guard([&]() { _r = (expr); }); (rc) = _e ? (0xE0000000u | (_e & 0xFFFFFF)) : (uint32_t)_r; } while (0)
#define CALLV(rc, expr) do { unsigned _e = guard([&]() { (expr); }); (rc) = _e ? (0xE0000000u | (_e & 0xFFFFFF)) : 0; } while (0)

// ---------------------------------------------------------------- contabilidad
struct Stat { uint64_t pairs = 0, diffs = 0, oracle_checks = 0, oracle_fail_exe = 0, oracle_fail_lib = 0, oracle_disagree = 0, faults = 0; std::string first; };
static Stat g_stat[14];
static uint64_t g_cases, g_case_diffs; static int g_verbose; static int g_dbg_stream = -1, g_cur_idx = -1, g_dbg_dyn = -1, g_snap_side = 0; static std::vector<uint8_t> g_snap[2]; static std::vector<std::pair<size_t, size_t>> g_snap_regions; static std::vector<Alloc> g_snap_allocs; static std::string g_snap_info;
static const char* FIELD[] = {"valor devuelto", "z_stream", "arena (estado/ventana/tablas/liberaciones)", "salida", "mensaje", "extra", "registro de zalloc/zfree"};
static int compare(const Trace& a, const Trace& b, const char* what, uint64_t seed) {
    int bad = 0;
    size_t n = std::min(a.v.size(), b.v.size());
    for (size_t i = 0; i < n; i++) {
        const Step &x = a.v[i], &y = b.v[i];
        g_stat[x.fn].pairs++;
        if (x.fn != y.fn) { bad = 1; g_stat[x.fn].diffs++; continue; }
        int f = -1;
        if (x.rc != y.rc) f = 0; else if (x.zh != y.zh) f = 1; else if (x.ar != y.ar) f = 2; else if (x.out != y.out) f = 3; else if (x.msg != y.msg) f = 4; else if (x.ex != y.ex) f = 5; else if (x.lg != y.lg) f = 6;
        if (f < 0 && x.ar_raw != y.ar_raw) g_pad_diff++;
        if (f >= 0) {
            bad = 1; g_stat[x.fn].diffs++;
            if (g_verbose) printf("  DIF %s %s semilla %llu paso %zu campo %s\n", FN_NAMES[x.fn], what, (unsigned long long)seed, i, FIELD[f]);
            if (g_stat[x.fn].first.empty()) { char b2[512]; snprintf(b2, sizeof b2, "%s semilla %llu paso %zu: difiere %s (EXE rc=%08x LIB rc=%08x)", what, (unsigned long long)seed, i, FIELD[f], x.rc, y.rc); g_stat[x.fn].first = b2; }
        }
    }
    if (a.v.size() != b.v.size()) { bad = 1; if (g_stat[0].first.empty()) { char b2[256]; snprintf(b2, sizeof b2, "%s semilla %llu: distinto numero de pasos %zu/%zu", what, (unsigned long long)seed, a.v.size(), b.v.size()); g_stat[0].first = b2; } g_stat[0].diffs++; }
    g_cases++; g_case_diffs += bad; return bad;
}
static void oracle(int fn, bool is_lib, bool pass, bool disagree_info = false) {
    Stat& s = g_stat[fn]; s.oracle_checks += is_lib ? 0 : 1; // se cuenta una vez por caso (lado EXE); los fallos se cuentan por lado
    if (!pass) { if (is_lib) s.oracle_fail_lib++; else s.oracle_fail_exe++; }
    (void)disagree_info;
}

// ---------------------------------------------------------------- lectura de ficheros de casos
static std::vector<uint8_t> slurp(const std::string& p) { FILE* f = fopen(p.c_str(), "rb"); std::vector<uint8_t> d; if (!f) return d; fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET); d.resize(n); fread(d.data(), 1, n, f); fclose(f); return d; }
struct Rd { const uint8_t* p; const uint8_t* e; uint32_t u32() { uint32_t v; memcpy(&v, p, 4); p += 4; return v; } const uint8_t* take(size_t n) { const uint8_t* r = p; p += n; return r; } bool ok() const { return p <= e; } };

// ---------------------------------------------------------------- escenario 1: flujos completos (inflate_blocks con entradas y salidas en trozos)
struct StreamCase { uint32_t seed, wsize, kind, status, judge, plen; uint8_t sha[32]; uint32_t adler, slen; const uint8_t* data; };
struct StreamResult { uint32_t final_rc; uint32_t total_out; uint32_t adler; uint8_t sha[32]; bool have_sha; int calls; bool use_check; size_t fail_at; };
static StreamResult run_stream(const Fns& F, const StreamCase& c, Trace& tr) {
    std::mt19937_64 rng(c.seed * 7919ull + 13); StreamResult res{};
    size_t fail_at = (rng() % 10 == 0) ? 1 + rng() % 3 : 0;
    begin_side(fail_at); const uint32_t slen = c.slen + 4; memcpy(g_in, c.data, c.slen); memset(g_in + c.slen, 0, 4); /* zlib 1.1.3 exige 'un byte ficticio' tras un flujo deflate sin cabecera */
    z_stream z; zinit(z); bool use_check = (rng() % 8) != 0; res.use_check = use_check; res.fail_at = fail_at;
    uint32_t rc; inflate_blocks_statef* s = nullptr;
    CALLV(rc, s = F.nw(&z, use_check ? (check_func)check_cb : nullptr, c.wsize)); push(tr, 1, rc ? rc : (s ? 0 : 1), z, norm((uint32_t)(uintptr_t)s), false);
    if (!s) { res.final_rc = 0xFFFF; return res; }
    g_huft_regions.push_back({(size_t)((uint8_t*)s->hufts - g_base), MANY});
    static const uint32_t in_slices[] = {1, 2, 3, 7, 16, 100, 1000, 0xFFFFFF}, out_slices[] = {1, 2, 5, 258, 259, 1000, 4096, 100000, 100000};
    uint32_t in_sl = in_slices[rng() % 8], out_sl = out_slices[rng() % 9];
    if (slen > 300) { in_sl = std::max<uint32_t>(in_sl, slen / 40 + 1); out_sl = std::max<uint32_t>(out_sl, 2048); } 
    size_t in_pos = 0, out_hashed = 0; int idle = 0; int r_arg = Z_BUF_ERROR;  // acota las llamadas por flujo (coste del banco); los flujos cortos conservan trozos de 1 byte
    z.next_in = g_in; z.avail_in = 0;
    for (int it = 0; it < 400000; it++) {
        if (z.avail_in == 0 && in_pos < slen) { uint32_t n = std::min<uint32_t>(in_sl, slen - in_pos); z.next_in = g_in + in_pos; z.avail_in = n; in_pos += n; }
        if (z.avail_out == 0) { size_t off = z.total_out; if (off >= OUTCAP - 8) break; z.next_out = g_out + off; z.avail_out = (uInt)std::min<size_t>(out_sl, OUTCAP - 8 - off); }
        uint32_t ti = z.total_in, to = z.total_out; r_arg = (rng() % 16 == 0) ? Z_OK : Z_BUF_ERROR;
        CALLRC(rc, F.blocks(s, &z, r_arg)); res.calls++;
        push(tr, 2, rc, z, 0, true, out_hashed, z.total_out); out_hashed = z.total_out;
        if (g_dbg_stream == g_cur_idx) printf("    llamada %d: r_arg=%d rc=%d avail_in=%u avail_out=%u total_in=%u total_out=%u mode=%d in_pos=%zu/%u\n", res.calls, r_arg, (int)rc, z.avail_in, z.avail_out, z.total_in, z.total_out, (int)s->mode, in_pos, slen);
        res.final_rc = rc;
        if (rc == (uint32_t)Z_STREAM_END || (rc & 0x80000000u) || (int)rc < 0 && (int)rc != Z_BUF_ERROR) break;
        if (z.total_in == ti && z.total_out == to) { if (z.avail_in == 0 && in_pos >= slen) { if (++idle > 1) break; } else if (++idle > 8) break; } else idle = 0;
    }
    res.total_out = z.total_out; res.adler = z.adler;
    if (g_dump_idx == g_cur_idx && g_dump_path && F.name[0] == 'E') { FILE* df = fopen(g_dump_path, "wb"); if (df) { fwrite(g_out, 1, std::min<size_t>(z.total_out, OUTCAP), df); fclose(df); } }
    if (c.judge) { sha256(g_out, std::min<size_t>(z.total_out, OUTCAP), res.sha); res.have_sha = true; }
    uLong chk = 0; CALLV(rc, F.reset(s, &z, &chk)); push(tr, 0, rc, z, chk, false);
    if (s->hufts) g_huft_regions.push_back({(size_t)((uint8_t*)s->hufts - g_base), MANY});
    CALLRC(rc, F.fr(s, &z)); push(tr, 3, rc, z, 0, false);
    return res;
}

// ---------------------------------------------------------------- escenarios de unidades
static const uInt CPLENS[31] = {3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258,0,0};
static const uInt CPLEXT[31] = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0,112,112};
static const uInt CPDIST[30] = {1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577};
static const uInt CPDEXT[30] = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};
struct LenCase { uint32_t seed, typ, nl, nd, m, kl, kd; const uint8_t* lens; };

static void* abuf(size_t bytes) { size_t sf = g_fail_at, sn = g_alloc_n; g_fail_at = 0; void* p = zalloc_cb(nullptr, 1, (uInt)bytes); g_fail_at = sf; g_alloc_n = sn; return p; } // reserva de preparacion del banco: no cuenta para la inyeccion de fallos
static uint64_t hash_u32s(const uInt* p, size_t n) { return hash_words(p, n * 4, 0x31); }

static bool tree_ok_expected(const uint8_t* lens, size_t n, uint32_t k) { // la tabla se construye (Z_OK) segun la desigualdad de Kraft y la regla de zlib para el codigo de un solo bit
    int mx = 0, nz = 0; for (size_t i = 0; i < n; i++) { if (lens[i]) { nz++; mx = std::max<int>(mx, lens[i]); } }
    if (nz == 0) return true; if (k == 2) return false; if (k == 1) return mx == 1; return true;
}

static void run_huft(const Fns& F, const LenCase& c, Trace& tr, int* exp_rc_out, bool bits_mode = false) {
    begin_side(0); z_stream z; zinit(z);
    uInt n = c.typ == 1 ? c.nl : c.nl; uInt s_ = 0; const uInt *d = nullptr, *e = nullptr;
    if (c.typ == 0) { s_ = 19; } else if (c.typ == 1 || c.typ == 2) { s_ = 257; d = CPLENS; e = CPLEXT; } else { s_ = 0; d = CPDIST; e = CPDEXT; }
    uInt* b = (uInt*)abuf(std::max<uInt>(n, 1) * 4); for (uInt i = 0; i < n; i++) b[i] = c.lens[i];
    inflate_huft* hp = (inflate_huft*)abuf(MANY * 8 + 64); g_huft_regions.push_back({(size_t)((uint8_t*)hp - g_base), MANY});
    uInt* v = (uInt*)abuf(288 * 4 + 16); uInt hn = (c.seed % 7 == 0) ? (c.seed >> 8) % 60 : 0; uInt m = c.m; inflate_huft* t = (inflate_huft*)0x5A5A5A5A; uint32_t rc;
    CALLRC(rc, F.huft(b, n, s_, d, e, &t, &m, hp, &hn, v));
    uint64_t ex = mix(mix(mix(mix(m, hn), norm((uint32_t)(uintptr_t)t)), hash_u32s(b, n)), hash_u32s(v, 288)); ex = mix(ex, hash_huft(hp, MANY));
    push(tr, 9, rc, z, ex, false);
    if (exp_rc_out) *exp_rc_out = (int)rc;
}
static void run_bits(const Fns& F, const LenCase& c, Trace& tr, int* rc_out) {
    begin_side(0); z_stream z; zinit(z);
    uInt* cc = (uInt*)abuf(19 * 4 + 8); for (int i = 0; i < 19; i++) cc[i] = c.lens[i];
    inflate_huft* hp = (inflate_huft*)abuf(MANY * 8 + 64); g_huft_regions.push_back({(size_t)((uint8_t*)hp - g_base), MANY});
    g_fail_at = (c.seed % 11 == 0) ? 1 + c.seed % 2 : 0; g_alloc_n = 0; // la inyeccion de fallos solo afecta a las reservas de la funcion probada
    uInt bb = c.m; inflate_huft* tb = (inflate_huft*)0x5A5A5A5A; uint32_t rc;
    CALLRC(rc, F.bits(cc, &bb, &tb, hp, &z));
    g_last_bb = bb;
    uint64_t ex = mix(mix(bb, norm((uint32_t)(uintptr_t)tb)), hash_u32s(cc, 19)); ex = mix(ex, hash_huft(hp, MANY));
    push(tr, 8, rc, z, ex, false); if (rc_out) *rc_out = (int)rc;
}
static int g_dirty = 0;
#pragma optimize("", off)
static void dirty_stack(uint32_t v) { volatile uint32_t buf[2048]; for (int i = 0; i < 2048; i++) buf[i] = v * 2654435761u + i; }
#pragma optimize("", on)
static void run_dyn(const Fns& F, const LenCase& c, Trace& tr, int* rc_out, uint32_t* bl_out, uint32_t* bd_out) {
    begin_side(0); z_stream z; zinit(z);
    uInt cn = c.nl + c.nd; uInt* cc = (uInt*)abuf(std::max<uInt>(cn, 1) * 4 + 8); for (uInt i = 0; i < cn; i++) cc[i] = c.lens[i];
    inflate_huft* hp = (inflate_huft*)abuf(MANY * 8 + 64); g_huft_regions.push_back({(size_t)((uint8_t*)hp - g_base), MANY});
    g_fail_at = (c.seed % 13 == 0) ? 1 + c.seed % 2 : 0; g_alloc_n = 0;
    bool oob = false; for (uInt q = 30; q < c.nd; q++) if (c.lens[c.nl + q]) oob = true; // simbolos de distancia 30/31: huft_build lee cpdext[30..31], fuera de la tabla de 30 entradas
    if (oob) { g_dead_regions.push_back({(size_t)((uint8_t*)hp - g_base), MANY * 8 + 64}); g_oob_cases++; }
    uInt bl = c.m, bd = 6; inflate_huft *tl = (inflate_huft*)0x5A5A5A5A, *td = (inflate_huft*)0x5B5B5B5B; uint32_t rc;
    if (g_dirty) dirty_stack(g_dirty);
    CALLRC(rc, F.dyn(c.nl, c.nd, cc, &bl, &bd, &tl, &td, hp, &z));
    uint64_t ex = mix(mix(mix(mix(bl, bd), norm((uint32_t)(uintptr_t)tl)), norm((uint32_t)(uintptr_t)td)), hash_u32s(cc, cn)); if (!oob) ex = mix(ex, hash_huft(hp, MANY));
    push(tr, 10, rc, z, ex, false); if (rc_out) *rc_out = (int)rc; if (bl_out) *bl_out = bl; if (bd_out) *bd_out = bd;
    if (g_dbg_dyn >= 0) { g_snap[g_snap_side].assign(g_base, g_base + g_brk); g_snap_regions = g_huft_regions; g_snap_allocs = g_allocs; g_snap_info = (std::string("rc=") + std::to_string((int)rc) + " bl=" + std::to_string(bl) + " bd=" + std::to_string(bd) + " msg=" + ((z.msg && (uintptr_t)z.msg != 0x11111111) ? z.msg : "-")); printf("    [%d] rc=%d bl=%u bd=%u tl=%08x td=%08x msg=%s\n", g_snap_side, (int)rc, bl, bd, norm((uint32_t)(uintptr_t)tl), norm((uint32_t)(uintptr_t)td), (z.msg && (uintptr_t)z.msg != 0x11111111) ? z.msg : "-"); }
}
static void run_fixed(const Fns& F, uint32_t seed, Trace& tr) {
    begin_side(0); z_stream z; zinit(z); uInt bl = seed, bd = ~seed; inflate_huft *tl = (inflate_huft*)0x5A5A5A5A, *td = (inflate_huft*)0x5B5B5B5B; uint32_t rc;
    CALLRC(rc, F.fixed(&bl, &bd, &tl, &td, (seed & 1) ? &z : nullptr));
    uint64_t ex = mix(mix(bl, bd), mix(norm((uint32_t)(uintptr_t)tl), norm((uint32_t)(uintptr_t)td)));
    ex = mix(ex, hash_huft(tl, 512)); ex = mix(ex, hash_huft(td, 32)); push(tr, 11, rc, z, ex, false);
}

struct FlushCase { uint32_t w, read, write, avail, r_in, use_check, check0, r_exp, rd_exp, wr_exp, av_exp, chk_exp, outlen; const uint8_t* win; const uint8_t* out; };
static void run_flush(const Fns& F, const FlushCase& c, Trace& tr, uint32_t* rc_out) {
    begin_side(0); z_stream z; zinit(z);
    inflate_blocks_statef* s = (inflate_blocks_statef*)abuf(sizeof(inflate_blocks_statef)); uint8_t* win = (uint8_t*)abuf(c.w); memcpy(win, c.win, c.w);
    memset(s, 0, sizeof *s); s->window = win; s->end = win + c.w; s->read = win + c.read; s->write = win + c.write; s->checkfn = c.use_check ? (check_func)check_cb : nullptr; s->check = c.check0; s->mode = TYPE;
    z.next_out = g_out; z.avail_out = c.avail; z.adler = 0x600D; uint32_t rc;
    CALLRC(rc, F.flush(s, &z, (int)c.r_in)); push(tr, 12, rc, z, mix(s->check, 0), true, 0, std::min<size_t>(z.total_out, OUTCAP));
    if (rc_out) *rc_out = rc;
    // verificacion contra el modelo independiente de Python
    bool ok = rc == c.r_exp && (uint32_t)(s->read - win) == c.rd_exp && (uint32_t)(s->write - win) == c.wr_exp && z.avail_out == c.av_exp && z.total_out == c.outlen && !memcmp(g_out, c.out, c.outlen)
              && s->check == c.chk_exp && (!c.use_check || z.adler == c.chk_exp) && (c.use_check || z.adler == 0x600D);
    oracle(12, F.name[0] == 'L', ok);
}

// escenario: estado de codigos fabricado (inflate_codes e inflate_fast) y ciclo de vida de bloques
static void prepare_codes_env(const Fns& F, uint64_t seedv, std::mt19937_64& rng, Trace& tr, z_stream& z, inflate_blocks_statef*& s, inflate_codes_statef*& cd, uInt& bl, uInt& bd, inflate_huft*& tl, inflate_huft*& td,
                              const std::vector<LenCase>* dyn) {
    zinit(z); uint32_t rc; s = nullptr; cd = nullptr;
    static const uint32_t ws[] = {512, 1024, 4096, 32768, 32768, 8192, 300, 2048};
    uint32_t w = ws[rng() % 8];
    CALLV(rc, s = F.nw(&z, (rng() % 3) ? (check_func)check_cb : nullptr, w)); push(tr, 1, rc ? rc : (s ? 0 : 1), z, norm((uint32_t)(uintptr_t)s), false);
    if (!s) return;
    g_huft_regions.push_back({(size_t)((uint8_t*)s->hufts - g_base), MANY});
    bool dynamic = dyn && !dyn->empty() && (rng() % 3 != 0);
    bl = 9; bd = 5; tl = td = nullptr;
    if (dynamic) {
        // tablas dinamicas construidas con la propia funcion del lado probado a partir de longitudes validas (completas)
        const LenCase* pick = nullptr;
        for (int k = 0; k < 40 && !pick; k++) { const LenCase& cand = (*dyn)[rng() % dyn->size()]; if (cand.typ == 1 && cand.kl == 0 && cand.kd == 0 && cand.nl >= 257) pick = &cand; }
        if (pick) {
            uInt cn = pick->nl + pick->nd; uInt* cc = (uInt*)abuf(cn * 4 + 8); for (uInt i = 0; i < cn; i++) cc[i] = pick->lens[i];
            for (uInt q = 30; q < pick->nd; q++) if (pick->lens[pick->nl + q]) { g_dead_regions.push_back({(size_t)((uint8_t*)s->hufts - g_base), MANY * 8}); break; }
            inflate_huft* hp = s->hufts; bl = 9; bd = 6; uint32_t r2; CALLRC(r2, F.dyn(pick->nl, pick->nd, cc, &bl, &bd, &tl, &td, hp, &z));
            push(tr, 10, r2, z, mix(mix(bl, bd), mix(norm((uint32_t)(uintptr_t)tl), norm((uint32_t)(uintptr_t)td))), false);
            if (r2 != 0) { dynamic = false; }
        } else dynamic = false;
    }
    if (!dynamic) { uint32_t r2; CALLRC(r2, F.fixed(&bl, &bd, &tl, &td, &z)); push(tr, 11, r2, z, mix(mix(bl, bd), mix(norm((uint32_t)(uintptr_t)tl), norm((uint32_t)(uintptr_t)td))), false); }
    CALLV(rc, cd = F.cnew(bl, bd, tl, td, &z)); push(tr, 5, rc ? rc : (cd ? 0 : 1), z, norm((uint32_t)(uintptr_t)cd), false);
    // entrada, bits pendientes y ventana
    uint32_t n = (rng() % 4 == 0) ? rng() % 12 : 10 + rng() % 400; for (uint32_t i = 0; i < n; i++) g_in[i] = (uint8_t)((rng() % 3) ? rng() : (rng() % 2 ? 0xFF : 0x00));
    z.next_in = g_in; z.avail_in = n; z.next_out = g_out; z.avail_out = (rng() % 4 == 0) ? rng() % 300 : 100 + rng() % 600;
    uint32_t k = rng() % 25; s->bitk = k; s->bitb = k ? (uint32_t)(rng() & ((1ull << k) - 1)) : 0;
    uint32_t wr = rng() % (w + 1); if (rng() % 3 == 0) wr = rng() % std::min<uint32_t>(w + 1, 400); uint32_t rd = (rng() % 2) ? wr : rng() % (w + 1);
    if (rng() % 5) { /* caso habitual: espacio de ventana libre grande (m >= 258) */ wr = rng() % (w > 600 ? w - 300 : 1); rd = (rng() % 2) ? wr : rng() % (wr + 1); }
    s->write = s->window + wr; s->read = s->window + rd;
    s->mode = CODES; if (cd) s->sub.decode.codes = cd;
    // llenar parte de la ventana con datos pseudoaleatorios (historial) para que las copias lean algo definido
    uint32_t fill = std::min<uint32_t>(w, wr + (rng() % 2 ? 1000 : 0)); for (uint32_t i = 0; i < fill; i++) s->window[i] = (uint8_t)rng();
    (void)seedv;
}
static void run_codes(const Fns& F, uint64_t seed, const std::vector<LenCase>* dyn, Trace& tr, bool fast_only) {
    std::mt19937_64 rng(seed * 2654435761ull + 99); size_t fail_at = (rng() % 12 == 0) ? 1 + rng() % 5 : 0; begin_side(fail_at);
    z_stream z; inflate_blocks_statef* s; inflate_codes_statef* cd; uInt bl, bd; inflate_huft *tl, *td; prepare_codes_env(F, seed, rng, tr, z, s, cd, bl, bd, tl, td, dyn);
    if (!s) return; uint32_t rc; size_t out_hashed = 0;
    if (!fast_only && (rng() % 4 == 0)) { inflate_codes_statef* cd2 = nullptr; CALLV(rc, cd2 = F.cnew(bl, bd, tl, td, &z)); push(tr, 5, rc ? rc : (cd2 ? 0 : 1), z, norm((uint32_t)(uintptr_t)cd2), false); if (cd2) { CALLV(rc, F.cfree(cd2, &z)); push(tr, 7, rc, z, 0, false); } }
    auto window_ex = [&]() { return mix(hash_words(s->window, 0), 0); };
    (void)window_ex;
    if (fast_only) {
        if (cd && g_dbg_stream == (int)(seed - 1000000)) { printf("  [%s] antes de fast: bl=%u bd=%u tl=%08x td=%08x avail_in=%u avail_out=%u bitk=%u bitb=%x read=%ld write=%ld end=%ld w mode=%d in:", F.name, bl, bd, (unsigned)(uintptr_t)tl, (unsigned)(uintptr_t)td, z.avail_in, z.avail_out, (unsigned)s->bitk, (unsigned)s->bitb, (long)(s->read - s->window), (long)(s->write - s->window), (long)(s->end - s->window), (int)s->mode); for (int q = 0; q < 24; q++) printf("%02x", g_in[q]); printf("\n"); }
        if (cd) { CALLRC(rc, F.fast(bl, bd, tl, td, s, &z)); push(tr, 13, rc, z, mix(0, 0), true, 0, std::min<size_t>(z.total_out + 0, 0) ); if (g_dbg_stream == (int)(seed - 1000000)) printf("  [%s] fast rc=%d avail_in=%u avail_out=%u bitk=%u bitb=%x read=%ld write=%ld msg=%s\n", F.name, (int)rc, z.avail_in, z.avail_out, (unsigned)s->bitk, (unsigned)s->bitb, (long)(s->read - s->window), (long)(s->write - s->window), (z.msg && (uintptr_t)z.msg != 0x11111111) ? z.msg : "-"); }
        // la salida de fast va a la ventana (arena): ya cubierta por el hash de la arena
        if (cd && (rng() % 2)) { uint32_t n = 10 + rng() % 300; for (uint32_t i = 0; i < n; i++) g_in[i] = (uint8_t)rng(); z.next_in = g_in; z.avail_in = n; CALLRC(rc, F.fast(bl, bd, tl, td, s, &z)); push(tr, 13, rc, z, 0, false); }
    } else if (cd) {
        for (int it = 0; it < 6; it++) {
            uint32_t to = z.total_out; int r_arg = (rng() % 4 == 0) ? Z_OK : Z_BUF_ERROR;
            CALLRC(rc, F.codes(s, &z, r_arg)); push(tr, 6, rc, z, mix(norm((uint32_t)(uintptr_t)s->sub.decode.codes), s->mode), true, out_hashed, z.total_out); out_hashed = z.total_out; (void)to;
            if (rc != (uint32_t)Z_OK) break;
            if (rng() % 2) { uint32_t n = rng() % 200; for (uint32_t i = 0; i < n; i++) g_in[i] = (uint8_t)rng(); z.next_in = g_in; z.avail_in = n; }
            if (z.avail_out == 0 || rng() % 2) { size_t off = z.total_out; z.next_out = g_out + off; z.avail_out = 50 + rng() % 500; }
        }
    }
    if (g_dbg_codes >= 0 && g_dbg_codes == g_cur_idx) { g_snap_side = F.name[0] == 'L'; g_snap[g_snap_side].assign(g_base, g_base + g_brk); g_snap_regions = g_huft_regions; g_snap_allocs = g_allocs; printf("  [%s] s: mode=%d read=%ld write=%ld bitk=%u bitb=%x avail_in=%u avail_out=%u total_out=%u\n", F.name, (int)s->mode, (long)(s->read - s->window), (long)(s->write - s->window), (unsigned)s->bitk, (unsigned)s->bitb, z.avail_in, z.avail_out, z.total_out); }
    if (cd && s->mode != DONE) { CALLV(rc, F.cfree(cd, &z)); push(tr, 7, rc, z, 0, false); s->sub.decode.codes = nullptr; }
    uLong chk = 0; CALLV(rc, F.reset(s, &z, (rng() % 2) ? &chk : nullptr)); push(tr, 0, rc, z, chk, false);
    CALLRC(rc, F.fr(s, &z)); push(tr, 3, rc, z, 0, false);
}
static void run_dict(const Fns& F, uint64_t seed, Trace& tr) {
    std::mt19937_64 rng(seed * 40503ull + 5); begin_side((rng() % 15 == 0) ? 1 + rng() % 3 : 0); z_stream z; zinit(z); uint32_t rc;
    static const uint32_t ws[] = {256, 512, 1000, 4096, 32768, 77}; uint32_t w = ws[rng() % 6]; inflate_blocks_statef* s = nullptr;
    CALLV(rc, s = F.nw(&z, (rng() % 3) ? (check_func)check_cb : nullptr, w)); push(tr, 1, rc ? rc : (s ? 0 : 1), z, norm((uint32_t)(uintptr_t)s), false); if (!s) return;
    uint32_t len = rng() % (w + 1); for (uint32_t i = 0; i < len; i++) g_in[i] = (uint8_t)rng();
    CALLV(rc, F.setdict(s, g_in, len)); push(tr, 4, rc, z, mix(s->read - s->window, s->write - s->window), false);
    // ciclo de vida con modos variados antes de reiniciar/liberar
    uint32_t modes[] = {TYPE, LENS, STORED, TABLE, BTREE, DTREE, CODES, DRY, DONE, BAD}; s->mode = (inflate_block_mode)modes[rng() % 10];
    if (s->mode == BTREE || s->mode == DTREE) s->sub.trees.blens = (uIntf*)abuf(20 * 4);
    if (s->mode == CODES) { inflate_huft *tl, *td; uInt bl, bd; F.fixed(&bl, &bd, &tl, &td, &z); s->sub.decode.codes = F.cnew(bl, bd, tl, td, &z); }
    uLong chk = 7; CALLV(rc, F.reset(s, &z, (rng() % 2) ? &chk : nullptr)); push(tr, 0, rc, z, chk, false);
    if (rng() % 2) { s->mode = (inflate_block_mode)modes[rng() % 10]; if (s->mode == BTREE || s->mode == DTREE) s->sub.trees.blens = (uIntf*)abuf(20 * 4); if (s->mode == CODES) s->sub.decode.codes = nullptr; }
    CALLRC(rc, F.fr(s, &z)); push(tr, 3, rc, z, 0, false);
}

// ---------------------------------------------------------------- programa principal
static int run_pair(const char* label, uint64_t seed, const std::function<void(const Fns&, Trace&)>& body, Trace* keep = nullptr) {
    if (g_trace) printf("%s %llu\n", label, (unsigned long long)seed);
    Trace a, b; body(EXE_F, a); body(LIB_F, b); int bad = compare(a, b, label, seed);
    if (bad && g_verbose) { printf("  [%s semilla %llu]\n", label, (unsigned long long)seed); }
    return bad;
}

static void print_snap_diff() {
    size_t n = std::min(g_snap[0].size(), g_snap[1].size()); printf("  arena %zu/%zu bytes, %zu reservas\n", g_snap[0].size(), g_snap[1].size(), g_snap_allocs.size());
    for (size_t q = 0; q < g_snap_allocs.size(); q++) printf("    reserva %zu: +%zu tam %zu%s\n", q, g_snap_allocs[q].off, g_snap_allocs[q].size, g_snap_allocs[q].freed ? " (liberada)" : "");
    int shown = 0;
    for (size_t o = 0; o < n && shown < 30; o++) if (g_snap[0][o] != g_snap[1][o]) {
        bool pad = false; for (auto& r : g_snap_regions) if (o >= r.first && o < r.first + r.second * 8 && ((o - r.first) % 8) >= 2 && ((o - r.first) % 8) <= 3) pad = true; if (pad) continue;
        size_t ai = 0; for (size_t q = 0; q < g_snap_allocs.size(); q++) if (o >= g_snap_allocs[q].off && o < g_snap_allocs[q].off + g_snap_allocs[q].size) ai = q;
        printf("    dif en %zu (reserva %zu +%zu): EXE %02x LIB %02x\n", o, ai, o - g_snap_allocs[ai].off, g_snap[0][o], g_snap[1][o]); shown++; }
}

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    SetUnhandledExceptionFilter([](EXCEPTION_POINTERS* e) -> LONG { printf("EXCEPCION %08lx en %p (acceso %p)\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, e->ExceptionRecord->NumberParameters > 1 ? (void*)e->ExceptionRecord->ExceptionInformation[1] : nullptr); return EXCEPTION_EXECUTE_HANDLER; });
    if (argc < 3) { printf("uso: bench <casos> <exe> [--filtro x] [--max N]\n"); return 2; }
    std::string dir = argv[1]; const char* filtro = nullptr; size_t maxn = 1u << 30;
    for (int i = 3; i < argc; i++) { if (!strcmp(argv[i], "--filtro") && i + 1 < argc) filtro = argv[++i]; else if (!strcmp(argv[i], "--max") && i + 1 < argc) maxn = atol(argv[++i]); else if (!strcmp(argv[i], "-v")) g_verbose = 1; else if (!strcmp(argv[i], "--trace")) g_trace = 1; else if (!strcmp(argv[i], "--dbgcodes") && i + 1 < argc) g_dbg_codes = atoi(argv[++i]); else if (!strcmp(argv[i], "--dbg") && i + 1 < argc) g_dbg_stream = atoi(argv[++i]); else if (!strcmp(argv[i], "--dbgdyn") && i + 1 < argc) g_dbg_dyn = atoi(argv[++i]); else if (!strcmp(argv[i], "--dirty") && i + 1 < argc) g_dirty = atoi(argv[++i]); else if (!strcmp(argv[i], "--dump") && i + 2 < argc) { g_dump_idx = atoi(argv[++i]); g_dump_path = argv[++i]; } }
    EXE_PATH = argv[2];
    char* mutenv = getenv("LIB1A_MUT"); g_mut = mutenv ? atoi(mutenv) : 0;
    if (!map_exe(EXE_PATH)) return 3;
    init_fns();
    g_base = (uint8_t*)VirtualAlloc(nullptr, ARENA, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE); g_in = (uint8_t*)VirtualAlloc(nullptr, INCAP, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE); g_out = (uint8_t*)VirtualAlloc(nullptr, OUTCAP, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    { z_stream z; zinit(z); uInt bl, bd; inflate_huft *tl, *td; inflate_trees_fixed(&bl, &bd, &tl, &td, &z); g_fix_tl_lib = (uint32_t)(uintptr_t)tl; g_fix_td_lib = (uint32_t)(uintptr_t)td; }
    printf("tablas fijas: EXE tl=%08x td=%08x  LIB tl=%08x td=%08x\n", g_fix_tl_exe, g_fix_td_exe, g_fix_tl_lib, g_fix_td_lib);
    bool run_all = !filtro; auto want = [&](const char* n) { return run_all || strstr(n, filtro); };
    uint32_t nstream = 0, nlens = 0, nflush = 0;
    // ---- tablas fijas (16384 pares)
    if (want("fixed")) { for (uint32_t i = 0; i < std::min<size_t>(maxn, 16384); i++) run_pair("fixed", i, [&](const Fns& F, Trace& t) { run_fixed(F, i, t); }); printf("fixed: hecho\n"); }
    // ---- longitudes de codigo
    std::vector<uint8_t> lensd = slurp(dir + "/lens.bin"); std::vector<LenCase> lens;
    if (lensd.size() > 8) { Rd r{lensd.data() + 4, lensd.data() + lensd.size()}; nlens = r.u32(); for (uint32_t i = 0; i < nlens; i++) { LenCase c; c.seed = r.u32(); c.typ = r.u32(); c.nl = r.u32(); c.nd = r.u32(); c.m = r.u32(); c.kl = r.u32(); c.kd = r.u32(); c.lens = r.take(320); lens.push_back(c); } }
    if (want("huft") || want("bits") || want("dyn")) {
        uint64_t nh = 0, nb = 0, nd_ = 0, dis_h = 0, dis_b = 0, dis_d = 0; uint64_t bits_ok_hist[16] = {0};
        for (size_t i = 0; i < lens.size(); i++) {
            const LenCase& c = lens[i];
            if (want("huft") && nh < maxn) {
                nh++; Trace a, b; int ra = 0, rb = 0; run_huft(EXE_F, c, a, &ra); run_huft(LIB_F, c, b, &rb); compare(a, b, "huft_build", i);
                // oraculo: el valor devuelto sigue la desigualdad de Kraft calculada en Python (kl) y la regla de zlib para un unico codigo de longitud 1
                { uint32_t n_ = c.nl; int mx = 0; bool nz = false; for (uint32_t q = 0; q < n_; q++) if (c.lens[q]) { nz = true; mx = std::max<int>(mx, c.lens[q]); }
                  auto okf = [&](int rc) { if (!nz) return rc == 0; if (c.kl == 2) return rc == -3; if (c.kl == 1) return mx == 1 ? rc == 0 : (rc == -5 || rc == -4); /* codigo completo: Z_OK, o Z_MEM_ERROR si pide mas tablas de las que caben en MANY=1440 entradas (solo con m > 9, mas de lo que usa zlib) */ return rc == 0 || (rc == -4 && c.m > 9); };
                  oracle(9, false, okf(ra)); oracle(9, true, okf(rb)); if (!okf(ra) && ra == rb) dis_h++;
                  if (g_verbose && !okf(ra)) { printf("  huft caso %zu: typ=%u nl=%u m=%u kl=%u maxlen=%d nz=%d rc=%d lens=", i, c.typ, c.nl, c.m, c.kl, mx, (int)nz, ra); for (uint32_t q = 0; q < std::min<uint32_t>(c.nl, 40); q++) printf("%d ", c.lens[q]); printf("\n"); } }
            }
            if (c.typ == 0 && want("bits") && nb < maxn) {
                nb++; Trace a, b; int ra = 0, rb = 0; run_bits(EXE_F, c, a, &ra); uint32_t bbe = g_last_bb; run_bits(LIB_F, c, b, &rb); if (ra == 0 && bbe < 16) bits_ok_hist[bbe]++; compare(a, b, "trees_bits", i);
                if (c.seed % 11 == 0) continue; /* fallo de reserva inyectado: el oraculo de Kraft no aplica */
                bool ok = tree_ok_expected(c.lens, 19, c.kl); bool nonempty = false; for (int q = 0; q < 19; q++) if (c.lens[q]) nonempty = true; int expect = (ok && nonempty) ? 0 : -3;
                oracle(8, false, ra == expect); oracle(8, true, rb == expect); if (ra != expect && ra == rb) dis_b++; if (g_verbose && ra != expect) { printf("  bits caso %zu: kl=%u m=%u rc=%d esperado=%d lens=", i, c.kl, c.m, ra, expect); for (int q = 0; q < 19; q++) printf("%d ", c.lens[q]); printf("\n"); }
            }
            if (c.typ == 1 && want("dyn") && nd_ < maxn) {
                nd_++; Trace a, b; int ra = 0, rb = 0; uint32_t bla = 0, bda = 0, blb = 0, bdb = 0; int sv = g_dbg_dyn; if (sv != (int)i) g_dbg_dyn = -1; g_snap_side = 0; run_dyn(EXE_F, c, a, &ra, &bla, &bda); g_snap_side = 1; run_dyn(LIB_F, c, b, &rb, &blb, &bdb); g_dbg_dyn = sv; compare(a, b, "trees_dynamic", i);
                if (sv == (int)i) { size_t n = std::min(g_snap[0].size(), g_snap[1].size()); printf("  nl=%u nd=%u m=%u kl=%u kd=%u arena %zu/%zu bytes\n", c.nl, c.nd, c.m, c.kl, c.kd, g_snap[0].size(), g_snap[1].size()); int shown = 0; for (size_t o = 0; o < n && shown < 40; o++) if (g_snap[0][o] != g_snap[1][o]) { bool pad = false; for (auto& r : g_snap_regions) if (o >= r.first && o < r.first + r.second * 8 && ((o - r.first) % 8) >= 2 && ((o - r.first) % 8) <= 3) pad = true; if (pad) continue; size_t ai = 0; for (size_t q = 0; q < g_snap_allocs.size(); q++) if (o >= g_snap_allocs[q].off && o < g_snap_allocs[q].off + g_snap_allocs[q].size) ai = q; printf("    dif en %zu (alloc %zu +%zu): EXE %02x LIB %02x\n", o, ai, o - g_snap_allocs[ai].off, g_snap[0][o], g_snap[1][o]); if (shown == 0) { size_t eb = g_snap_allocs[ai].off + ((o - g_snap_allocs[ai].off) / 8) * 8; for (int side = 0; side < 2; side++) { printf("      [%s] entradas -2..+2:", side ? "LIB" : "EXE"); for (int q = -2; q <= 2; q++) { printf(" |"); for (int bb = 0; bb < 8; bb++) printf("%02x", g_snap[side][eb + q * 8 + bb]); } printf("\n"); } }
       shown++; } }
                if (c.seed % 13 == 0) continue; /* fallo de reserva inyectado */
                bool lit_nonempty = false, dist_nonempty = false; for (uint32_t q = 0; q < c.nl; q++) if (c.lens[q]) lit_nonempty = true; for (uint32_t q = 0; q < c.nd; q++) if (c.lens[c.nl + q]) dist_nonempty = true;
                bool ok = tree_ok_expected(c.lens, c.nl, c.kl) && lit_nonempty && tree_ok_expected(c.lens + c.nl, c.nd, c.kd) && !(!dist_nonempty && c.nl > 257);
                bool lit_bad = !(tree_ok_expected(c.lens, c.nl, c.kl) && lit_nonempty), dist_empty_bad = !dist_nonempty && c.nl > 257;
                auto okd = [&](int rc) { if (ok) return rc == 0; if (lit_bad) return c.kl == 2 ? rc == -3 : (rc == -3 || rc == -4); if (dist_empty_bad && !(!tree_ok_expected(c.lens + c.nl, c.nd, c.kd))) return rc == -3; return c.kd == 2 ? rc == -3 : (rc == -3 || rc == -4); }; oracle(10, false, okd(ra)); oracle(10, true, okd(rb)); if (!okd(ra) && ra == rb) dis_d++; if (g_verbose && !okd(ra)) { printf("  dyn caso %zu: nl=%u nd=%u kl=%u kd=%u m=%u rc=%d bl=%u bd=%u ok=%d litne=%d distne=%d lens=", i, c.nl, c.nd, c.kl, c.kd, c.m, ra, bla, bda, (int)ok, (int)lit_nonempty, (int)dist_nonempty); for (uint32_t q = 0; q < 24; q++) printf("%d ", c.lens[q]); printf("\n"); }
            }
        }
        if (g_verbose) { printf("  bits con rc=0 por *bb:"); for (int q = 0; q < 16; q++) printf(" %d:%llu", q, (unsigned long long)bits_ok_hist[q]); printf("\n"); }
        printf("huft/bits/dyn: %llu/%llu/%llu casos; el oraculo de Kraft discrepa de AMBOS lados (iguales entre si) en %llu/%llu/%llu\n", (unsigned long long)nh, (unsigned long long)nb, (unsigned long long)nd_, (unsigned long long)dis_h, (unsigned long long)dis_b, (unsigned long long)dis_d);
    }
    // ---- flush
    if (want("flush")) {
        std::vector<uint8_t> fd = slurp(dir + "/flush.bin"); if (fd.size() > 8) {
            Rd r{fd.data() + 4, fd.data() + fd.size()}; nflush = r.u32();
            for (uint32_t i = 0; i < nflush && i < maxn; i++) {
                FlushCase c; c.w = r.u32(); c.read = r.u32(); c.write = r.u32(); c.avail = r.u32(); c.r_in = r.u32(); c.use_check = r.u32(); c.check0 = r.u32(); c.r_exp = r.u32(); c.rd_exp = r.u32(); c.wr_exp = r.u32();
                c.av_exp = r.u32(); c.chk_exp = r.u32(); c.outlen = r.u32(); c.win = r.take(c.w); c.out = r.take(c.outlen);
                Trace a, b; uint32_t ra, rb; run_flush(EXE_F, c, a, &ra); run_flush(LIB_F, c, b, &rb); compare(a, b, "flush", i);
            }
            printf("flush: hecho\n");
        }
    }
    // ---- codigos / fast / diccionario
    // tablas dinamicas validas para decodificar: sin longitudes en los simbolos de distancia 30 y 31 (el RFC 1951 los prohibe y zlib 1.1.3 lee fuera de cpdext[] al usarlos: comportamiento indefinido)
    std::vector<LenCase> dynv; for (auto& c : lens) { bool oob = false; for (uint32_t q = 30; q < c.nd; q++) if (c.lens[c.nl + q]) oob = true; if (c.typ == 1 && c.kl == 0 && c.kd == 0 && !oob) dynv.push_back(c); }
    if (want("codes")) { for (uint32_t i = 0; i < std::min<size_t>(maxn, 20000); i++) { g_cur_idx = (int)i; run_pair("codes", i, [&](const Fns& F, Trace& t) { run_codes(F, i, &dynv, t, false); }); if (g_dbg_codes == (int)i) print_snap_diff(); } printf("codes: hecho\n"); }
    if (want("fast")) { for (uint32_t i = 0; i < std::min<size_t>(maxn, 20000); i++) { g_cur_idx = (int)i; run_pair("fast", i, [&](const Fns& F, Trace& t) { run_codes(F, 1000000 + i, &dynv, t, true); }); if (g_dbg_codes == (int)i) print_snap_diff(); } printf("fast: hecho\n"); }
    if (want("dict")) { for (uint32_t i = 0; i < std::min<size_t>(maxn, 20000); i++) run_pair("dict", i, [&](const Fns& F, Trace& t) { run_dict(F, i, t); }); printf("dict: hecho\n"); }
    // ---- flujos completos
    if (want("stream")) {
        std::vector<uint8_t> sd = slurp(dir + "/streams.bin"); if (sd.size() > 8) {
            Rd r{sd.data() + 4, sd.data() + sd.size()}; nstream = r.u32(); uint64_t okc = 0, errc = 0, incc = 0, cls_dis = 0, judged = 0;
            for (uint32_t i = 0; i < nstream && i < maxn; i++) {
                StreamCase c; c.seed = r.u32(); c.wsize = r.u32(); c.kind = r.u32(); c.status = r.u32(); c.judge = r.u32(); c.plen = r.u32(); memcpy(c.sha, r.take(32), 32); c.adler = r.u32(); c.slen = r.u32(); c.data = r.take(c.slen);
                if (c.slen > INCAP - 16) continue;
                g_cur_idx = (int)i; Trace a, b; StreamResult ra = run_stream(EXE_F, c, a), rb = run_stream(LIB_F, c, b); compare(a, b, "stream", i);
                if (c.judge) { // flujo valido (segun Python) y ventana suficiente: salida y adler deben coincidir con Python
                    judged++; for (int side = 0; side < 2; side++) { const StreamResult& q = side ? rb : ra; if (q.fail_at) continue; bool ok = q.final_rc == (uint32_t)Z_STREAM_END && q.total_out == c.plen && q.have_sha && !memcmp(q.sha, c.sha, 32) && (!q.use_check || q.adler == c.adler) && (q.use_check || q.adler == 0); if (c.plen == 0 && q.final_rc == (uint32_t)Z_STREAM_END) ok = q.total_out == 0; oracle(2, side == 1, ok); if (!ok && g_verbose) printf("  oraculo flujo %u lado %d: rc=%d total_out=%u esperado=%u sha_ok=%d adler=%08x esperado=%08x wsize=%u slen=%u calls=%d\n", i, side, (int)q.final_rc, q.total_out, c.plen, q.have_sha && !memcmp(q.sha, c.sha, 32), q.adler, c.adler, c.wsize, c.slen, q.calls); }
                    if (ra.final_rc == (uint32_t)Z_STREAM_END) okc++;
                } else if (c.status == 1) { bool ea = (int)ra.final_rc == Z_DATA_ERROR, eb = (int)rb.final_rc == Z_DATA_ERROR; if (ea) errc++; if (ea != eb || (ra.final_rc == rb.final_rc && !ea)) cls_dis += (ea != eb); }
                else if (c.status == 2) incc++;
            }
            printf("stream: %u flujos; juzgados por Python %llu (finalizan bien: %llu); corruptos con error en Python y DATA_ERROR en el EXE: %llu; truncados: %llu\n", nstream, (unsigned long long)judged, (unsigned long long)okc, (unsigned long long)errc, (unsigned long long)incc);
        }
    }
    // ---- informe
    printf("\nfuncion | direccion | pares | diferencias | comprobaciones del oraculo (fallos EXE/LIB)\n");
    uint64_t total_diff = 0, min_pairs = ~0ull; bool oracle_bad = false;
    for (int i = 0; i < 14; i++) {
        Stat& s = g_stat[i]; total_diff += s.diffs; min_pairs = std::min(min_pairs, s.pairs); if (s.oracle_fail_exe || s.oracle_fail_lib) oracle_bad = true;
        printf("%-24s 0x%06X %8llu %6llu   oraculo %llu (fallos %llu/%llu)\n", FN_NAMES[i], FN_ADDR[i], (unsigned long long)s.pairs, (unsigned long long)s.diffs, (unsigned long long)s.oracle_checks, (unsigned long long)s.oracle_fail_exe, (unsigned long long)s.oracle_fail_lib);
        if (!s.first.empty()) printf("    primera diferencia: %s\n", s.first.c_str());
    }
    printf("casos de trees_dynamic con simbolos de distancia 30/31 (contenido de tablas sin definir, enmascarado): %llu\n", (unsigned long long)g_oob_cases);
    printf("pasos con diferencia solo en el relleno indefinido de las entradas de tablas (enmascarado): %llu\n", (unsigned long long)g_pad_diff);
    printf("casos %llu, con diferencias %llu; minimo de pares por funcion %llu; mutacion %d\n", (unsigned long long)g_cases, (unsigned long long)g_case_diffs, (unsigned long long)min_pairs, g_mut);
    printf("%s\n", (total_diff == 0 && !oracle_bad) ? "RESULTADO: SIN DIFERENCIAS" : "RESULTADO: HAY DIFERENCIAS");
    return (total_diff == 0 && !oracle_bad) ? 0 : 1;
}
