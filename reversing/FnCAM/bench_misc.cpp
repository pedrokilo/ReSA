// Banco CAM-misc: cuatro rutinas puras de RpWorld del EXE original contra su reconstruccion C++ de rpworld.cpp, fuera del juego (mismo metodo que bench_cam: EXE mapeado en 0x400000, texto C++ extraido tal cual con
// extract_misc.py): 0x74CE60 (tamano del chunk de estructura de una geometria), 0x762440 (datos de un sector hoja en el flujo), 0x762040 (subarbol de sectores; llama a _rwPluginRegistryGetSize 0x808B00, desviada a un
// falso con traza) y 0x75C290 (siguiente arista de una tira). Se compara el valor devuelto y la traza completa para estados aleatorios.
// Uso: bench_misc.exe <ruta del EXE> [--casos N] [--semilla S]   (devuelve 0 si todo coincide)
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <random>
#include <algorithm>

typedef int32_t RwInt32; typedef uint32_t RwUInt32; typedef uint8_t RwUInt8; typedef uint16_t RwUInt16; typedef int32_t RwBool; typedef float RwReal; typedef uint16_t RxVertexIndex;
template<typename T> static T& G(uintptr_t a) { return *(T*)a; }

struct RwV3d { RwReal x, y, z; };
struct RwLLLink { RwLLLink* next; RwLLLink* prev; };
struct RwPluginRegistry { int dummy; };
struct RpMorphTarget { RwUInt8 pad[0x14]; RwV3d* verts; RwV3d* normals; };
static_assert(sizeof(RpMorphTarget) == 0x1C);
struct RpGeometry {
    RwUInt8 pad0[8]; RwUInt32 flags; RwUInt16 lockedSinceLastInst; RwUInt16 refCount; RwInt32 numTriangles; RwInt32 numVertices; RwInt32 numMorphTargets; RwInt32 numTexCoordSets; RwUInt8 pad1[0x5C - 0x20]; RpMorphTarget* morphTarget;
};
static_assert(offsetof(RpGeometry, numTriangles) == 0x10 && offsetof(RpGeometry, numTexCoordSets) == 0x1C && offsetof(RpGeometry, morphTarget) == 0x5C);
struct RpSector { RwInt32 type; };
struct RpPlaneSector { RwInt32 type; RwReal value; RpSector* leftSubTree; RpSector* rightSubTree; RwReal leftValue; RwReal rightValue; };
struct RpWorldSector { RwInt32 type; RwUInt8 pad[0x7C]; RwUInt16 matListWindowBase; RwUInt16 numVertices; RwUInt16 numTriangles; RwUInt16 pad2; };
struct RpWorld { RwUInt8 object[8]; RwUInt32 flags; RwInt32 renderOrder; RwUInt8 matList[12]; RpSector* rootSector; RwInt32 numTexCoordSets; RwUInt8 pad[0x40]; };
static_assert(offsetof(RpWorldSector, numVertices) == 0x82 && offsetof(RpWorldSector, numTriangles) == 0x84 && offsetof(RpWorld, numTexCoordSets) == 0x20);
struct RpTriStripPolygon;
struct RpTriStripEdge { RwLLLink inUsedLink; RwLLLink inFreeLink; RpTriStripPolygon* poly1; RpTriStripPolygon* poly2; RxVertexIndex vert1; RxVertexIndex vert2; RwBool strip; };
struct RpTriStripPolygon { RwLLLink inEndLink; RwLLLink inUsedLink; RwLLLink inFreeLink; RwUInt32 numEdges; RpTriStripEdge* edges[3]; RxVertexIndex vertIndex[3]; RwUInt32 testFrame; };
static_assert(offsetof(RpTriStripPolygon, edges) == 0x1C && offsetof(RpTriStripPolygon, testFrame) == 0x30 && offsetof(RpTriStripEdge, vert1) == 0x18);
constexpr uintptr_t kWorldSectorPluginRegistry = 0x8D736C;

static std::vector<uint32_t> g_log;
static void L(uint32_t a) { g_log.push_back(a); }
static void L(uint32_t a, uint32_t b) { L(a); L(b); }
static uint32_t id_of(const void* p) { return (uint32_t)(uintptr_t)p; }
static std::mt19937_64 g_caseRng;
static uint32_t cr(uint32_t n) { return n ? (uint32_t)(g_caseRng() % n) : 0; }
static uint8_t g_arena[1 << 20]; static size_t g_arenaUsed;
static void* arena_alloc(size_t n, uint8_t fill = 0) { n = (n + 15) & ~15u; void* p = g_arena + g_arenaUsed; g_arenaUsed += n; if (g_arenaUsed > sizeof g_arena) { printf("arena agotada\n"); ExitProcess(4); } memset(p, fill, n); return p; }
static RwInt32 __cdecl _rwPluginRegistryGetSize(const RwPluginRegistry* reg, const void* object) { L(0xE70, id_of(reg)); L(id_of(object)); return (RwInt32)(((uint32_t)(uintptr_t)object >> 4) % 61); }

#include "misc_func.inc"

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
static void divert(uintptr_t exeAddr, void* target) { *(uint8_t*)exeAddr = 0xE9; *(int32_t*)(exeAddr + 1) = (int32_t)((uintptr_t)target - (exeAddr + 5)); }

static std::vector<RpWorldSector*> g_leaves;
static RpSector* build(int depth) {
    if (depth == 0 || cr(3) == 0) { auto* s = (RpWorldSector*)arena_alloc(sizeof(RpWorldSector)); s->type = -1; s->numVertices = (RwUInt16)(cr(5) == 0 ? cr(65536) : cr(40)); s->numTriangles = (RwUInt16)(cr(5) == 0 ? cr(65536) : cr(40)); return (RpSector*)s; }
    auto* p = (RpPlaneSector*)arena_alloc(sizeof(RpPlaneSector)); p->type = (RwInt32)(cr(3) * 4); p->leftSubTree = build(depth - 1); p->rightSubTree = build(depth - 1); return (RpSector*)p;
}
enum Kind { K_STRUCT, K_DATA, K_PLANE, K_STRIP, K_COUNT };
struct Args { Kind kind; void* p0; void* p1; uint32_t u0, u1, u2; };
static Args setup(uint64_t seed, Kind kind) {
    g_caseRng.seed(seed); g_log.clear(); g_arenaUsed = 0x100; memset(g_arena, 0, 0x100);
    Args a{ kind, nullptr, nullptr, 0, 0, 0 };
    if (kind == K_STRUCT) {
        auto* g = (RpGeometry*)arena_alloc(sizeof(RpGeometry) + 0x20, 0xCD);
        static const uint32_t fl[] = { 0, 8, 0x10, 0x18, 0x1000000, 0x1000008, 0x2000000, 0x1B, 0x8000, 0x4 };
        g->flags = fl[cr(10)] | (cr(3) == 0 ? cr(0x10000) : 0); g->numVertices = (RwInt32)(cr(4) == 0 ? 0 : cr(300)); g->numTriangles = (RwInt32)cr(500); g->numTexCoordSets = (RwInt32)cr(5);
        g->numMorphTargets = (RwInt32)(cr(5) == 0 ? 0 : cr(5)); g->morphTarget = (RpMorphTarget*)arena_alloc(sizeof(RpMorphTarget) * (g->numMorphTargets + 1));
        for (int i = 0; i < g->numMorphTargets; i++) { g->morphTarget[i].verts = cr(3) ? (RwV3d*)(uintptr_t)(0x1000 + cr(100)) : nullptr; g->morphTarget[i].normals = cr(2) ? (RwV3d*)(uintptr_t)(0x2000 + cr(100)) : nullptr; }
        a.p0 = g;
    } else if (kind == K_DATA || kind == K_PLANE) {
        auto* w = (RpWorld*)arena_alloc(sizeof(RpWorld), 0xCD);
        static const uint32_t fl[] = { 0, 0x1000000, 0x10, 0x8, 0x18, 0x1000018, 0x4 };
        w->flags = fl[cr(7)] | (cr(3) == 0 ? cr(0x100) : 0); w->numTexCoordSets = (RwInt32)cr(6);
        a.p1 = w; a.u0 = (cr(2) ? w->flags : cr(0x40000000)) | (cr(5) == 0 ? 0x1000000u : 0u);
        if (kind == K_DATA) { auto* s = (RpWorldSector*)arena_alloc(sizeof(RpWorldSector)); s->type = -1; s->numVertices = (RwUInt16)(cr(5) == 0 ? cr(65536) : cr(100)); s->numTriangles = (RwUInt16)(cr(5) == 0 ? cr(65536) : cr(100)); a.p0 = s; }
        else { auto* p = (RpPlaneSector*)arena_alloc(sizeof(RpPlaneSector)); p->type = (RwInt32)(cr(3) * 4); p->leftSubTree = build(1 + cr(4)); p->rightSubTree = build(1 + cr(4)); a.p0 = p; }
    } else {
        const uint32_t np = 1 + cr(6); std::vector<RpTriStripPolygon*> polys; std::vector<RpTriStripEdge*> edges;
        for (uint32_t i = 0; i < np; i++) { auto* p = (RpTriStripPolygon*)arena_alloc(sizeof(RpTriStripPolygon), 0); p->testFrame = cr(4); p->inFreeLink.next = cr(3) ? (RwLLLink*)(uintptr_t)(0x100 + cr(8)) : nullptr; polys.push_back(p); }
        for (uint32_t i = 0; i < np * 3 + 2; i++) {
            auto* e = (RpTriStripEdge*)arena_alloc(sizeof(RpTriStripEdge), 0); e->poly1 = cr(6) == 0 ? nullptr : polys[cr(np)]; e->poly2 = cr(4) == 0 ? nullptr : polys[cr(np)]; e->vert1 = (RxVertexIndex)cr(5); e->vert2 = (RxVertexIndex)cr(5); edges.push_back(e);
        }
        for (auto* p : polys) { p->numEdges = 3; for (int k = 0; k < 3; k++) p->edges[k] = edges[cr((uint32_t)edges.size())]; if (cr(3) == 0) p->edges[cr(3)]->poly1 = p; if (cr(3) == 0) p->edges[cr(3)]->poly2 = p; }
        a.p0 = cr(4) == 0 ? nullptr : edges[cr((uint32_t)edges.size())]; a.p1 = polys[cr(np)]; a.u0 = cr(5) | (cr(4) == 0 ? 0xABCD0000u : 0u); a.u1 = cr(4);
    }
    return a;
}
static uint32_t call_exe(const Args& a) {
    switch (a.kind) {
    case K_STRUCT: return ((uint32_t(__cdecl*)(void*))0x74CE60)(a.p0);
    case K_DATA: return ((uint32_t(__cdecl*)(void*, void*, uint32_t))0x762440)(a.p0, a.p1, a.u0);
    case K_PLANE: return ((uint32_t(__cdecl*)(void*, void*, uint32_t))0x762040)(a.p0, a.p1, a.u0);
    default: return ((uint32_t(__cdecl*)(void*, void*, uint32_t, uint32_t))0x75C290)(a.p0, a.p1, a.u0, a.u1);
    }
}
static uint32_t call_cpp(const Args& a) {
    switch (a.kind) {
    case K_STRUCT: return (uint32_t)RpGeometryStreamStructSize((const RpGeometry*)a.p0);
    case K_DATA: return RpWorldSectorStreamDataSize(a.p0, (RpWorld*)a.p1, a.u0);
    case K_PLANE: return RpWorldPlaneStreamSize(a.p0, (RpWorld*)a.p1, a.u0);
    default: return id_of(StripNextEdge((RpTriStripEdge*)a.p0, (RpTriStripPolygon*)a.p1, a.u0, a.u1));
    }
}
static LONG WINAPI Crash(EXCEPTION_POINTERS* e) {
    printf("EXCEPCION %08lx en %p (acceso %p)\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, e->ExceptionRecord->NumberParameters > 1 ? (void*)e->ExceptionRecord->ExceptionInformation[1] : nullptr);
    fflush(stdout); ExitProcess(3); return EXCEPTION_EXECUTE_HANDLER;
}
int main(int argc, char** argv) {
    SetUnhandledExceptionFilter(Crash);
    if (argc < 2) { printf("uso: bench_misc.exe <gta_sa.exe> [--casos N] [--semilla S]\n"); return 2; }
    uint64_t n = 100000, seed = 1;
    for (int i = 2; i < argc; i++) { if (!strcmp(argv[i], "--casos") && i + 1 < argc) n = strtoull(argv[++i], 0, 10); else if (!strcmp(argv[i], "--semilla") && i + 1 < argc) seed = strtoull(argv[++i], 0, 10); }
    setvbuf(stdout, nullptr, _IONBF, 0);
    if (!map_exe(argv[1])) return 2;
    divert(0x808B00, (void*)&_rwPluginRegistryGetSize);
    static const char* names[] = { "0x74CE60 estructura de geometria", "0x762440 datos de sector hoja", "0x762040 subarbol de sectores", "0x75C290 siguiente arista de tira" };
    uint64_t fails[K_COUNT] = {}, runs[K_COUNT] = {}, nonzero[K_COUNT] = {};
    for (uint64_t it = 0; it < n; it++) {
        const Kind kind = (Kind)(it % K_COUNT); const uint64_t cs = seed * 1000003 + it;
        Args a = setup(cs, kind); const uint32_t ra = call_exe(a); std::vector<uint32_t> oa = g_log; oa.push_back(ra);
        Args b = setup(cs, kind); const uint32_t rb = call_cpp(b); std::vector<uint32_t> ob = g_log; ob.push_back(rb);
        runs[kind]++; if (ra) nonzero[kind]++;
        if (oa != ob) { fails[kind]++; if (fails[kind] <= 2) printf("DIFERENCIA %s caso=%llu: exe=%08x ours=%08x (trazas %zu / %zu)\n", names[kind], (unsigned long long)cs, ra, rb, oa.size(), ob.size()); }
    }
    uint64_t total = 0;
    for (int k = 0; k < K_COUNT; k++) { printf("%-34s %llu casos (resultado != 0: %llu), %llu diferencias\n", names[k], (unsigned long long)runs[k], (unsigned long long)nonzero[k], (unsigned long long)fails[k]); total += fails[k]; }
    printf("RESULTADO: %s (%llu diferencias)\n", total ? "HAY DIFERENCIAS" : "SIN DIFERENCIAS", (unsigned long long)total);
    return total ? 1 : 0;
}
