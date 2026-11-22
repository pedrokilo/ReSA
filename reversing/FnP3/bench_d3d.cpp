// Banco P3-S2: creacion de rasters D3D9. EXE original (0x4CCE60 con sus ayudantes usercall 0x4CB7C0, 0x4CB9C0, 0x4CC5C0, 0x4CCD70 y 0x4CB730) contra RasterCreate de
// source/game_sa/RenderWare/D3D9RasterDriver.cpp (compilado aqui tal cual, con cabeceras minimas de shim/), fuera del juego.
// El gta_sa.exe original (solo lectura) se mapea en 0x400000 en este proceso de 32 bits. Se sustituyen por falsos del banco: IDirect3D9 (CheckDeviceFormat, CheckDepthStencilMatch),
// IDirect3DDevice9 (EvictManagedResources, CreateTexture, CreateCubeTexture, CreateDepthStencilSurface), GetClientRect (IAT 0x858304), las funciones de memoria del motor (RwEngineInstance +0x134..+0x148)
// y RwFreeListCreate (0x801980, parcheado en la copia del EXE). El resto del EXE que intervienen (0x7FED70, 0x730510, 0x4CBDE0..0x4CC020, 0x4CB530) corre tal cual en los dos lados.
// Se comparan, tras cada caso con el mismo estado inicial (semilla): valor devuelto, bytes del raster y de su extension, la traza completa de llamadas a los falsos (orden y argumentos),
// la lista de rasters, la pila de indices de paleta y todos los globales del controlador. Los falsos devuelven resultados por hash de sus argumentos (exito, no disponible, sin memoria de video).
// Uso: bench_d3d.exe <ruta del EXE> [--casos N] [--semilla S]   (devuelve 0 si todo coincide)
#include "StdInc.h"
#include "D3DResourceSystem.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <map>

static std::vector<uint32_t> g_log;
static void L(uint32_t a) { g_log.push_back(a); }
static void L(uint32_t a, uint32_t b) { L(a); L(b); }

// ---------------------------------------------------------------- memoria falsa del motor
static uint8_t g_arena[1 << 20]; static size_t g_arenaUsed;
static void* arena_alloc(size_t n, uint8_t fill = 0xCD) { n = (n + 15) & ~15u; void* p = g_arena + g_arenaUsed; g_arenaUsed += n; memset(p, fill, n); return p; }
struct FakeFreeList { uint32_t entrySize; uint32_t magic; };

RwFreeList* RwFreeListCreate(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwUInt32 hint) {
    L(0xF1, entrySize); L(entriesPerBlock); L(alignment); L(hint);
    auto* l = (FakeFreeList*)arena_alloc(sizeof(FakeFreeList)); l->entrySize = entrySize; l->magic = 0xFEE1F00D;
    return (RwFreeList*)l;
}
void RwFreeListDestroy(RwFreeList*) { L(0xF2); }
static void* __cdecl EngMalloc(uint32_t size, uint32_t hint) { L(0xE1, size); L(hint); void* p = arena_alloc(size); L((uint32_t)((uint8_t*)p - g_arena)); return p; }
static void __cdecl EngFree(void* p) { L(0xE2, (uint32_t)((uint8_t*)p - g_arena)); }
static void* __cdecl EngRealloc(void* p, uint32_t size, uint32_t hint) {
    L(0xE3, (uint32_t)((uint8_t*)p - g_arena)); L(size); L(hint);
    void* n = arena_alloc(size); memcpy(n, p, std::min<size_t>(size, 0x400)); L((uint32_t)((uint8_t*)n - g_arena)); return n;
}
static void* __cdecl EngFreeListAlloc(FakeFreeList* l, uint32_t hint) {
    L(0xE4, l->entrySize); L(hint); void* p = arena_alloc(l->entrySize); L((uint32_t)((uint8_t*)p - g_arena)); return p;
}
static void __cdecl EngFreeListFree(FakeFreeList* l, void* p) { L(0xE5, l->entrySize); L((uint32_t)((uint8_t*)p - g_arena)); }

// ---------------------------------------------------------------- falsos COM
static uint32_t g_hseed; static uint32_t g_texId;
static uint32_t H(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f) {
    uint64_t x = g_hseed * 0x9E3779B97F4A7C15ull; for (uint32_t v : {a, b, c, d, e, f}) { x ^= v + 0x9E3779B9u + (x << 6) + (x >> 2); x *= 0xFF51AFD7ED558CCDull; } return (uint32_t)(x >> 32);
}
static const int32_t E_NOTAVAIL = (int32_t)0x8876086A, E_OOM = (int32_t)0x8876017C, E_FAIL_ = (int32_t)0x80004005;

static HRESULT __stdcall D3D_CheckDeviceFormat(void*, UINT adapter, UINT devType, UINT adapterFmt, DWORD usage, UINT rtype, UINT fmt) {
    L(0xD10, adapter); L(devType); L(adapterFmt); L(usage); L(rtype); L(fmt);
    const uint32_t h = H(0xA, usage, rtype, fmt, adapterFmt, 0) % 8;
    return h < 5 ? 0 : (h == 5 ? 1 : E_NOTAVAIL); // incluye un resultado positivo distinto de S_OK (para los que comparan con S_OK)
}
static HRESULT __stdcall D3D_CheckDepthStencilMatch(void*, UINT adapter, UINT devType, UINT adapterFmt, UINT rtFmt, UINT dsFmt) {
    L(0xD12, adapter); L(devType); L(adapterFmt); L(rtFmt); L(dsFmt);
    return H(0xC, rtFmt, dsFmt, adapterFmt, 0, 0) % 5 ? 0 : E_NOTAVAIL;
}
static HRESULT __stdcall Dev_Evict(void*) { L(0xD05); return 0; }
static HRESULT make_result(uint32_t tag, uint32_t a, uint32_t b, uint32_t c, uint32_t d, void** pp) {
    const uint32_t h = H(tag, a, b, c, d, 0) % 10;
    if (h < 6) { *pp = (void*)(uintptr_t)(0x70000000 + 16 * (++g_texId)); L(1); return 0; }
    L(0); return h == 6 ? E_OOM : (h == 7 ? E_FAIL_ : E_NOTAVAIL);
}
static HRESULT __stdcall Dev_CreateTexture(void*, UINT w, UINT h, UINT levels, DWORD usage, UINT fmt, UINT pool, void** pp, void* shared) {
    L(0xD23, w); L(h); L(levels); L(usage); L(fmt); L(pool); L((uint32_t)(uintptr_t)shared); L((uint32_t)(uintptr_t)pp);
    return make_result(23, usage, fmt, pool, levels, pp);
}
static HRESULT __stdcall Dev_CreateCubeTexture(void*, UINT edge, UINT levels, DWORD usage, UINT fmt, UINT pool, void** pp, void* shared) {
    L(0xD25, edge); L(levels); L(usage); L(fmt); L(pool); L((uint32_t)(uintptr_t)shared); L((uint32_t)(uintptr_t)pp);
    return make_result(25, usage, fmt, pool, levels, pp);
}
static HRESULT __stdcall Dev_CreateDepthStencilSurface(void*, UINT w, UINT h, UINT fmt, UINT ms, UINT msq, BOOL discard, void** pp, void* shared) {
    L(0xD29, w); L(h); L(fmt); L(ms); L(msq); L(discard); L((uint32_t)(uintptr_t)shared); L((uint32_t)(uintptr_t)pp);
    return make_result(29, fmt, ms, msq, discard, pp);
}
static HRESULT __stdcall Unexpected() { L(0xBAD); return E_FAIL_; }
static void* g_d3dVtbl[64], *g_devVtbl[200]; static void* g_d3dObj, *g_devObj;

static RECT g_clientRect;
BOOL WINAPI BenchGetClientRect(HWND hwnd, LPRECT r) { L(0xC1, (uint32_t)(uintptr_t)hwnd); *r = g_clientRect; return TRUE; }
static BOOL WINAPI BenchGetClientRectIat(HWND hwnd, LPRECT r) { return BenchGetClientRect(hwnd, r); }

// 0x7FED70 y 0x730510 corren del EXE en los dos lados
void _rwD3D9CheckValidTextureSize(RwInt32* width, RwInt32* height, RwUInt32 format) { reinterpret_cast<void(__cdecl*)(RwInt32*, RwInt32*, RwUInt32)>(0x7FED70)(width, height, format); }
int32 D3DResourceSystem::CreateTexture(int32 width, int32 height, int32 levels, uint32 format, void** pp) { return reinterpret_cast<int32(__cdecl*)(int32, int32, int32, uint32, void**)>(0x730510)(width, height, levels, format, pp); }
void D3DResourceSystem::DestroyTexture(void*) {}

#include "D3D9RasterDriver.cpp"

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
template<typename T> static T& G(uintptr_t a) { return *(T*)a; }

constexpr uintptr_t kRasterBase = 0;
struct Snapshot { std::vector<uint32_t> v; };

// estado inicial deterministico a partir de la semilla del caso
static std::mt19937_64 g_caseRng;
static uint32_t cr(uint32_t n) { return n ? (uint32_t)(g_caseRng() % n) : 0; }

static alignas(16) uint8_t g_raster[0x34 + 0x24 + 0x40];
struct Case { uint32_t flags; };
static Case setup(uint64_t seed) {
    g_caseRng.seed(seed); g_hseed = (uint32_t)seed * 2654435761u; g_texId = 0; g_log.clear(); g_arenaUsed = 0; memset(g_arena, 0, 0x1000);
    // globales del motor
    static uint8_t engine[0x200]; memset(engine, 0, sizeof engine);
    *(void**)(engine + 0x134) = (void*)&EngMalloc; *(void**)(engine + 0x138) = (void*)&EngFree; *(void**)(engine + 0x13C) = (void*)&EngRealloc;
    *(void**)(engine + 0x144) = (void*)&EngFreeListAlloc; *(void**)(engine + 0x148) = (void*)&EngFreeListFree;
    G<void*>(0xC97B24) = engine;
    G<int32_t>(0xB4E9E0) = 0x34;
    // dispositivo y objeto D3D
    for (auto& s : g_d3dVtbl) s = (void*)&Unexpected; for (auto& s : g_devVtbl) s = (void*)&Unexpected;
    g_d3dVtbl[10] = (void*)&D3D_CheckDeviceFormat; g_d3dVtbl[12] = (void*)&D3D_CheckDepthStencilMatch;
    g_devVtbl[5] = (void*)&Dev_Evict; g_devVtbl[23] = (void*)&Dev_CreateTexture; g_devVtbl[25] = (void*)&Dev_CreateCubeTexture; g_devVtbl[29] = (void*)&Dev_CreateDepthStencilSurface;
    g_d3dObj = g_d3dVtbl; g_devObj = g_devVtbl;
    G<void*>(0xC97C20) = &g_d3dObj; G<void*>(0xC97C28) = &g_devObj;
    G<uint32_t>(0xC97C24) = cr(3); G<uint32_t>(0x8E2428) = 1 + cr(2); G<void*>(0xC97C2C) = (void*)0x71234560; G<HWND>(0xC97C1C) = (HWND)(uintptr_t)(0x10000 + cr(100));
    // formatos y capacidades
    static const int32_t bbFormats[] = {0x15, 0x16, 0x17, 0x18, 0x19, 0x23, 0x1A, 0x1B, 0x14, 0x32, 0x0, 0x28};
    G<int32_t>(0xC9C048) = bbFormats[cr(12)];
    G<int32_t>(0xC9BEF4) = cr(3) == 0 ? 0x10 : (cr(5) == 0 ? 8 : 0x20);
    static const int32_t zdepth[] = {0x10, 0x20, 0x10, 0x20, 0x18, 0};
    G<int32_t>(0xC9BEFC) = zdepth[cr(6)];
    G<uint32_t>(0xC9C050) = cr(4); G<uint32_t>(0xC9C054) = cr(3); G<uint32_t>(0xC9BEF0) = 0x16 - cr(2) * 0x7; G<uint32_t>(0xC9C068) = 0x4D + cr(4);
    G<uint32_t>(0xC9BF0C) = cr(2) ? 0x40000000u : 0; G<uint32_t>(0xC9BF3C) = (uint32_t)(g_caseRng() & 0x1FF) | (cr(2) ? 0x80 : 0);
    G<int32_t>(0xC9BF58) = 64 << cr(6); G<int32_t>(0xC9BF5C) = 64 << cr(6);
    // tabla de formatos de pixel (alfa, profundidad, formato RW de 16 bits) para todos los D3DFORMAT posibles (0..0x80)
    for (int i = 0; i < 0x80; i++) { uint8_t* e = (uint8_t*)0xB4E7E0 + 4 * i; e[0] = (uint8_t)cr(2); e[1] = (uint8_t)(cr(3) * 8 + 8); *(uint16_t*)(e + 2) = (uint16_t)((cr(16) << 8) | (cr(2) ? 0 : 0x2000 * cr(2)) | cr(256)); }
    // RECT de la ventana
    g_clientRect = RECT{0, 0, (LONG)(16 << cr(6)), (LONG)(16 << cr(6))};
    // listas
    G<void*>(0xB4EA00) = RwFreeListCreate(8, 0x7F, 4, 0x30411); L(0xAA);
    G<void*>(0xB4E9F8) = cr(2) ? nullptr : RwFreeListCreate(0x404, 0x40, 4, 0x30411);
    const uint32_t cap = cr(4) == 0 ? 0 : 4 + cr(6); const uint32_t cnt = cap ? cr(cap + 1) : 0;
    G<uint32_t>(0xB4E9EC) = cap; G<uint32_t>(0xB4E9E8) = cnt; G<uint32_t>(0xB4E9F0) = cr(300);
    uint16_t* idx = nullptr; if (cap) { idx = (uint16_t*)arena_alloc(cap * 2 + 16); for (uint32_t i = 0; i < cap; i++) idx[i] = (uint16_t)(0x100 + i * 3); }
    G<uint16_t*>(0xB4E9F4) = idx;
    void* head = nullptr; if (cr(3) == 0) { auto* nd = (uint32_t*)arena_alloc(8); nd[0] = 0x1234; nd[1] = 0; head = nd; }
    G<void*>(0xB4E9FC) = head;
    L(0xAB);
    // raster y extension con basura
    for (auto& b : g_raster) b = (uint8_t)g_caseRng();
    RwRaster* r = (RwRaster*)g_raster;
    r->width = cr(5) == 0 ? 0 : (cr(2) ? (int32_t)g_clientRect.right : (int32_t)(1 << cr(10)));
    r->height = cr(5) == 0 ? 0 : (cr(2) ? (int32_t)g_clientRect.bottom : (int32_t)(1 << cr(10)));
    if (cr(6) == 0) r->width = 100 + cr(900), r->height = 100 + cr(900);
    if (cr(8) == 0) { r->width = g_clientRect.right; r->height = g_clientRect.bottom; }
    r->depth = cr(2) ? 0 : cr(40);
    // banderas de creacion: tipo, banderas altas y formato
    static const uint32_t types[] = {0, 0, 0, 1, 1, 2, 2, 4, 4, 5, 5, 3, 6, 7};
    uint32_t flags = types[cr(14)] | ((cr(4) == 0) ? (cr(2) ? 0x80 : 0x10) : 0);
    flags |= (cr(2) ? (uint32_t)(cr(16) << 8) : 0) | (cr(3) == 0 ? 0x1000u : 0) | (cr(3) == 0 ? 0x2000u : 0) | (cr(8) == 0 ? 0x4000u : 0) | (cr(3) == 0 ? 0x8000u : 0);
    if (cr(3) == 0) flags |= 0x9000;
    return Case{flags};
}

static std::vector<uint32_t> observe(int32_t ret) {
    std::vector<uint32_t> o; o.push_back((uint32_t)ret);
    for (size_t i = 0; i < 0x34 + 0x24; i += 4) o.push_back(*(uint32_t*)(g_raster + i));
    for (uintptr_t a : {0xB4E9E8u, 0xB4E9ECu, 0xB4E9F0u, 0xB4E9F4u, 0xB4E9F8u, 0xB4E9FCu, 0xB4EA00u}) o.push_back(G<uint32_t>(a));
    uint16_t* idx = G<uint16_t*>(0xB4E9F4); if (idx) for (int i = 0; i < 12; i++) o.push_back(idx[i]);
    for (uint32_t* n = (uint32_t*)G<void*>(0xB4E9FC); n && o.size() < 400; n = (uint32_t*)n[1]) { o.push_back(n[0]); o.push_back((uint32_t)(uintptr_t)n[1]); }
    // contenido de un bloque de paleta si lo hay
    RwRaster* r = (RwRaster*)g_raster; uint8_t* ext = g_raster + 0x34; uint8_t* pal = *(uint8_t**)(ext + 4);
    if (pal >= g_arena && pal < g_arena + sizeof g_arena) o.push_back(*(uint32_t*)(pal + 0x400));
    (void)r;
    o.insert(o.end(), g_log.begin(), g_log.end());
    return o;
}

#include "bench_d3d_helpers.inc"

static LONG WINAPI Crash(EXCEPTION_POINTERS* e) {
    printf("EXCEPCION %08lx en %p (acceso %p) eax=%08lx ecx=%08lx edx=%08lx esi=%08lx edi=%08lx\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress,
           e->ExceptionRecord->NumberParameters > 1 ? (void*)e->ExceptionRecord->ExceptionInformation[1] : nullptr, e->ContextRecord->Eax, e->ContextRecord->Ecx, e->ContextRecord->Edx, e->ContextRecord->Esi, e->ContextRecord->Edi);
    fflush(stdout); ExitProcess(3); return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char** argv) {
    SetUnhandledExceptionFilter(Crash);
    if (argc < 2) { printf("uso: bench_d3d.exe <gta_sa.exe> [--casos N] [--semilla S]\n"); return 2; }
    uint64_t n = 300000, seed = 1; for (int i = 2; i < argc; i++) { if (!strcmp(argv[i], "--casos") && i + 1 < argc) n = strtoull(argv[++i], 0, 10); else if (!strcmp(argv[i], "--semilla") && i + 1 < argc) seed = strtoull(argv[++i], 0, 10); }
    setvbuf(stdout, nullptr, _IONBF, 0);
    if (!map_exe(argv[1])) return 2;
    if (getenv("BENCH_TRACE")) printf("mapeado\n");
    // el EXE llama a GetClientRect por la IAT y a RwFreeListCreate por direccion directa (call 0x801980 en 0x4CB7C0): se desvian al banco
    *(void**)0x858304 = (void*)&BenchGetClientRectIat;
    {
        const uint32_t target = (uint32_t)(uintptr_t)&RwFreeListCreate; int patched = 0;
        for (uintptr_t p = 0x4CB7C0; p < 0x4CB7C0 + 512; p++) if (*(uint8_t*)p == 0xE8 && (uintptr_t)(p + 5 + *(int32_t*)(p + 1)) == 0x801980) { *(int32_t*)(p + 1) = (int32_t)(target - (p + 5)); patched++; }
        if (patched != 1) printf("aviso: RwFreeListCreate parcheadas = %d\n", patched);
    }
    std::map<uint32_t, uint64_t> coverage;
    uint64_t fails = 0; std::map<uint32_t, uint64_t> byType, failByType; uint64_t okRet = 0;
    typedef int32_t(__cdecl* CreateFn)(void*, RwRaster*, uint32_t);
    for (uint64_t it = 0; it < n; it++) {
        const uint64_t cs = seed * 1000003 + it;
        if (getenv("BENCH_TRACE")) printf("caso %llu\n", (unsigned long long)it);
        Case c = setup(cs); const int type = c.flags & 7;
        if (getenv("BENCH_TRACE")) printf(" setup ok flags=%05x w=%d h=%d\n", c.flags, ((RwRaster*)g_raster)->width, ((RwRaster*)g_raster)->height);
        std::vector<uint8_t> raster0(g_raster, g_raster + sizeof g_raster);
        const int32_t a = ((CreateFn)0x4CCE60)(nullptr, (RwRaster*)g_raster, c.flags);
        if (getenv("BENCH_TRACE")) printf(" exe ret=%d\n", a);
        const std::vector<uint32_t> oa = observe(a);
        Case c2 = setup(cs); (void)c2;
        const int32_t b = RwD3D9RasterDriver::RasterCreateCpp(nullptr, (RwRaster*)g_raster, c.flags);
        const std::vector<uint32_t> ob = observe(b);
        byType[type]++; if (a) okRet++;
        for (uint32_t tag : {0xD23u, 0xD25u, 0xD29u, 0xD05u, 0xC1u, 0xE1u, 0xE2u, 0xE3u, 0xE4u, 0xE5u, 0xD10u, 0xD12u, 0xF1u}) if (std::find(g_log.begin(), g_log.end(), tag) != g_log.end()) coverage[tag]++;
        if (oa != ob) {
            failByType[type]++;
            if (fails++ < 8) {
                printf("DIFERENCIA caso=%llu flags=%05x: exe ret=%d (%zu valores) ours ret=%d (%zu valores)\n", (unsigned long long)cs, c.flags, a, oa.size(), b, ob.size());
                for (size_t i = 0; i < std::min(oa.size(), ob.size()); i++) if (oa[i] != ob[i]) { printf("  primer valor distinto en [%zu]: exe=%08x ours=%08x\n", i, oa[i], ob[i]); break; }
            }
        }
    }
    printf("rasters: %llu casos (devuelven 1: %llu); por tipo:", (unsigned long long)n, (unsigned long long)okRet);
    for (auto& kv : byType) printf(" %u=%llu", kv.first, (unsigned long long)kv.second);
    printf("\ncasos con llamadas (marca 0xD23 CreateTexture, 0xD25 cubo, 0xD29 superficie de profundidad, 0xD05 Evict, 0xC1 GetClientRect, 0xE4 FreeListAlloc, 0xE1 malloc, 0xE3 realloc, 0xD10 CheckDeviceFormat, 0xF1 RwFreeListCreate):");
    for (auto& kv : coverage) printf(" %x=%llu", kv.first, (unsigned long long)kv.second);
    printf("\n");
    fails += (uint64_t)run_helpers(n, seed);
    printf("RESULTADO: %s (%llu diferencias)\n", fails ? "HAY DIFERENCIAS" : "SIN DIFERENCIAS", (unsigned long long)fails);
    return fails ? 1 : 0;
}
