// Banco fuera del juego del lote S31A (cierre): compara el EXE original (mapeado en memoria, solo lectura) con las reconstrucciones de
// RpTriStripMeshTunnel (0x75B780), RpBuildMeshGenerateTriStrip (0x75C380), 0x75C660 (TriStripMeshCreate), 0x75C9D0 (TriStripEmit) y 0x75D240 (TriStripNeighbours).
// Cada escenario parte de la misma arena determinista (misma direccion, relleno 0xCD) y se ejecuta primero con el EXE y luego con el codigo propio; se compara el valor devuelto,
// la huella de toda la arena y el registro de reservas/liberaciones del asignador del motor.
// RpGeometrySortByMaterial (0x75DAE0) no entra en este banco (necesita el estado de RpGeometry del motor).
#include "common.h"
#include <cstddef>

typedef uint32_t RwUInt32; typedef int32_t RwInt32; typedef uint16_t RwUInt16; typedef int16_t RwInt16; typedef uint8_t RwUInt8; typedef int32_t RwBool; typedef uint16_t RxVertexIndex;
#define TRUE 1
#define FALSE 0
struct RwLLLink { RwLLLink* next; RwLLLink* prev; };
struct RwLinkList { RwLLLink link; };
struct RpMaterial; struct RpMeshHeader;
struct RpTriStripEdge; struct RpTriStripPolygon;
struct RpTriStripPolygon { RwLLLink inEndLink; RwLLLink inUsedLink; RwLLLink inFreeLink; RwUInt32 numEdges; RpTriStripEdge* edges[3]; RxVertexIndex vertIndex[3]; RwUInt32 testFrame; };
struct RpTriStripEdge { RwLLLink inUsedLink; RwLLLink inFreeLink; RpTriStripPolygon* poly1; RpTriStripPolygon* poly2; RxVertexIndex vert1; RxVertexIndex vert2; RwBool strip; };
struct RpTriStripMesh { RwUInt32 numPolygons; RwLinkList polygonEndList; RwLinkList polygonUsedList; RwLinkList polygonFreeList; RwLinkList edgeUsedList; RwLinkList edgeFreeList; };
typedef RpTriStripMesh* (*RpTriStripMeshStripCallBack)(RpTriStripMesh*, void*);
struct RpTriStripData { RwBool ignoreWinding; RpTriStripMeshStripCallBack stripCB; void* data; };
typedef RwUInt32 (*RpTriStripCostCallBack)(RpTriStripPolygon*, RwUInt32, void*);
struct RpTriStripTunnelData { float quality; RwUInt32 lengthLimit; RpTriStripCostCallBack costCB; void* data; };
struct RpBuildMeshTriangle { RwUInt16 vertIndex[3]; RpMaterial* material; RwUInt16 matIndex; RwUInt16 textureIndex; RwUInt16 rasterIndex; RwUInt16 pipelineIndex; };
struct RpBuildMesh { RwUInt32 triangleBufferSize; RwUInt32 numTriangles; RpBuildMeshTriangle* meshTriangles; };
struct RpMesh { RxVertexIndex* indices; RwUInt32 numIndices; RpMaterial* material; };

template<class T> static inline T& StaticRef(uintptr_t a) { return *reinterpret_cast<T*>(a); }

// asignador del motor (tabla en RwEngineInstance + 0x134 / 0x138 / 0x13C): arena determinista; las liberaciones no reutilizan
static std::map<void*, u32> g_sizes;
static void* __cdecl rw_malloc(u32 size, u32 hint) { void* p = ar_alloc_fill(size ? size : 1); g_sizes[p] = size; L(0xA0000000u ^ size); L(hint); return p; }
static void __cdecl rw_free(void* p) { L(0xF0000000u); L((u32)p); }
static void* __cdecl rw_realloc(void* p, u32 size, u32 hint) { void* q = ar_alloc_fill(size); if (p) memcpy(q, p, std::min(size, g_sizes[p])); g_sizes[q] = size; L(0xB0000000u ^ size); L(hint); return q; }
#define RwMalloc(s, h) rw_malloc((s), (h))
#define RwFree(p) rw_free((p))

namespace ExeCrt {
using QsortCompare = int(__cdecl*)(const void*, const void*);
static void __cdecl Qsort(void* base, unsigned int num, unsigned int width, QsortCompare compare) { ((void(__cdecl*)(void*, unsigned, unsigned, QsortCompare))0x8247E0)(base, num, width, compare); }
static double __cdecl Pow(double x, double y) { return ((double(__cdecl*)(double, double))0x8220F0)(x, y); }
}
static int BuildMeshTriangleCompare(const void* a, const void* b) { return ((int(__cdecl*)(const void*, const void*))0x759640)(a, b); }


// ---- geometria simulada (RpGeometry, 0x60 bytes) y adaptadores: el EXE llama a estas funciones por direccion (parcheadas hacia los sustitutos) y el codigo propio por los adaptadores T_*
struct RpGeometry; typedef void (__cdecl *RpGeometrySortByMaterialCallBack)(const RpGeometry*, RpGeometry*, RwUInt16*, RwInt32);
struct RpMaterialList { RpMaterial** materials; RwInt32 numMaterials; RwInt32 space; };
struct RpMorphTarget { void* parent; float sphere[4]; float* verts; float* normals; };
struct RpGeometry { u8 obj[8]; RwUInt32 flags; RwUInt16 locked; RwInt16 refCount; RwInt32 numTriangles, numVertices, numMorphTargets, numTexCoordSets; RpMaterialList matList; void* triangles; u32* preLitLum; float* texCoords[8]; void* mesh; void* repEntry; RpMorphTarget* morphTarget; };
static_assert(sizeof(RpGeometry) == 0x60 && sizeof(RpMorphTarget) == 0x1C);
struct RwPluginRegEntry { RwInt32 offset; RwInt32 size; u8 pad[0x28]; RwPluginRegEntry* next; RwPluginRegEntry* prev; void* parent; };
static int g_geoFail; // bit0: Create devuelve nulo, bit1: AddMorphTarget falla, bit2: MaterialListCopy falla, bit3: Unlock falla
static RpGeometry* __cdecl st_Create(RwInt32 nv, RwInt32 nt, RwUInt32 fmt) {
    if (g_geoFail & 1) return nullptr;
    RpGeometry* g = (RpGeometry*)rw_malloc(0x60 + 0x20, 0x30001); memset(g, 0, 0x80);
    g->flags = fmt & 0xFFFF; g->numTexCoordSets = (fmt >> 16) & 0xFF; g->numTriangles = nt; g->numVertices = nv; g->numMorphTargets = 1; g->refCount = 1;
    g->triangles = rw_malloc(nt * 8, 0x30002);
    if (g->flags & 8) g->preLitLum = (u32*)rw_malloc(nv * 4, 0x30003);
    for (RwInt32 k = 0; k < g->numTexCoordSets; k++) g->texCoords[k] = (float*)rw_malloc(nv * 8, 0x30004);
    g->morphTarget = (RpMorphTarget*)rw_malloc(sizeof(RpMorphTarget), 0x30005); memset(g->morphTarget, 0, sizeof(RpMorphTarget));
    g->morphTarget[0].parent = g; g->morphTarget[0].verts = (float*)rw_malloc(nv * 12, 0x30006);
    if (g->flags & 0x10) g->morphTarget[0].normals = (float*)rw_malloc(nv * 12, 0x30007);
    return g;
}
static RpGeometry* __cdecl st_Lock(RpGeometry* g, RwInt32 mode) { (void)mode; return g; }
static RpGeometry* __cdecl st_Unlock(RpGeometry* g) { return (g_geoFail & 8) ? nullptr : g; }
static RwInt32 __cdecl st_AddMorph(RpGeometry* g) {
    if (g_geoFail & 2) return 0;
    RwInt32 n = g->numMorphTargets; g->morphTarget = (RpMorphTarget*)rw_realloc(g->morphTarget, (n + 1) * sizeof(RpMorphTarget), 0x30008);
    RpMorphTarget* m = g->morphTarget + n; memset(m, 0, sizeof(*m)); m->parent = g; m->verts = (float*)rw_malloc(g->numVertices * 12, 0x30006);
    if (g->flags & 0x10) m->normals = (float*)rw_malloc(g->numVertices * 12, 0x30007);
    g->numMorphTargets = n + 1; return 1;
}
static RwInt32 __cdecl st_Destroy(RpGeometry* g) { L(0xDE57u); L(g->numVertices); rw_free(g->morphTarget); rw_free(g); return 1; }
static RpMorphTarget* __cdecl st_Sphere(RpMorphTarget* mt, float* sphere) {
    const RpGeometry* g = (const RpGeometry*)mt->parent; float* v = mt->verts; float sx = 0, sy = 0, sz = 0;
    for (RwInt32 i = 0; i < g->numVertices; i++) { sx += v[i*3]; sy += v[i*3+1]; sz += v[i*3+2]; }
    sphere[0] = sx; sphere[1] = sy; sphere[2] = sz; sphere[3] = (float)g->numVertices; return mt;
}
static RpMaterialList* __cdecl st_MatCopy(RpMaterialList* dst, const RpMaterialList* src) {
    if (g_geoFail & 4) return nullptr;
    dst->numMaterials = src->numMaterials; dst->space = src->space; dst->materials = (RpMaterial**)rw_malloc(src->numMaterials * 4, 0x30009);
    memcpy(dst->materials, src->materials, src->numMaterials * 4); return dst;
}
static RwPluginRegEntry* _rpGeometryPluginFirstEntry() { return StaticRef<RwPluginRegEntry*>(0x8D629C); }
static uint32_t T_GeometryPluginFirst() { return (uint32_t)(uintptr_t)_rpGeometryPluginFirstEntry(); }
static uint32_t T_MorphTargetCalcBoundingSphere(uint32_t a, uint32_t b) { return (uint32_t)(uintptr_t)st_Sphere((RpMorphTarget*)(uintptr_t)a, (float*)(uintptr_t)b); }
static uint32_t T_GeometryAddMorphTarget(uint32_t a) { return (uint32_t)st_AddMorph((RpGeometry*)(uintptr_t)a); }
static uint32_t T_GeometryLock(uint32_t a, uint32_t m) { return (uint32_t)(uintptr_t)st_Lock((RpGeometry*)(uintptr_t)a, (RwInt32)m); }
static uint32_t T_GeometryUnlock(uint32_t a) { return (uint32_t)(uintptr_t)st_Unlock((RpGeometry*)(uintptr_t)a); }
static uint32_t T_GeometryCreate(uint32_t a, uint32_t b, uint32_t c) { return (uint32_t)(uintptr_t)st_Create((RwInt32)a, (RwInt32)b, c); }
static uint32_t T_GeometryDestroy(uint32_t a) { return (uint32_t)st_Destroy((RpGeometry*)(uintptr_t)a); }
static uint32_t T_MaterialListCopy(uint32_t a, uint32_t b) { return (uint32_t)(uintptr_t)st_MatCopy((RpMaterialList*)(uintptr_t)a, (const RpMaterialList*)(uintptr_t)b); }


// ---- bloque propio (extraido de rpworld_s30a31a.cpp)
#include "bloque_s31a.inc"

// ---- banco
static void __cdecl bench_sort_cb(const RpGeometry* o, RpGeometry* n, RwUInt16* remap, RwInt32 nverts) { L(0xCB000000u); L((u32)nverts); L(fnv(remap, nverts * 2)); L(n->numVertices); L(o->numVertices); }
static u8 g_engine[0x400];
static RpMaterial* g_mats[4];
static u8* g_snap; static size_t g_snap_brk; static std::vector<u32> g_log_snap;

static void arena_save() { g_snap_brk = g_brk; memcpy(g_snap, g_ar, g_brk); g_log_snap = g_log; }
static void arena_restore() { memcpy(g_ar, g_snap, g_snap_brk); g_brk = g_snap_brk; g_log = g_log_snap; }

// callback de coste de tiras: marca testFrame en todos los poligonos de la tira (siguiendo las aristas de tira desde el extremo) y devuelve un coste que baja al alargar las tiras
static RwUInt32 __cdecl bench_cost(RpTriStripPolygon* start, RwUInt32 testFrame, void* data) {
    RwUInt32 n = 0; RpTriStripPolygon* prev = nullptr; RpTriStripPolygon* cur = start;
    while (cur) {
        cur->testFrame = testFrame; ++n;
        RpTriStripPolygon* next = nullptr;
        for (RwUInt32 i = 0; i < cur->numEdges; ++i) {
            RpTriStripEdge* e = cur->edges[i];
            if (!e->strip) continue;
            RpTriStripPolygon* o = e->poly1 == cur ? e->poly2 : e->poly1;
            if (o && o != prev) { next = o; break; }
        }
        prev = cur; cur = next;
        if (n > 100000) break;
    }
    return 10 + 100 / (n + (u32)(uintptr_t)data % 3);
}

struct Scenario {
    u32 nv, nt; std::vector<RpBuildMeshTriangle> tris; float quality; u32 limit; int ignoreWinding;
};
static Scenario make(Rng& r, bool distinct) {
    Scenario s; s.nv = 3 + r.below(r.chance(10) ? 200 : 14); s.nt = 1 + r.below(r.chance(10) ? 150 : 24);
    u32 nm = 1 + r.below(3);
    for (u32 i = 0; i < s.nt; i++) {
        RpBuildMeshTriangle t = {};
        for (;;) {
            for (int k = 0; k < 3; k++) t.vertIndex[k] = (RwUInt16)r.below(s.nv);
            if (!distinct || (t.vertIndex[0] != t.vertIndex[1] && t.vertIndex[1] != t.vertIndex[2] && t.vertIndex[2] != t.vertIndex[0])) break;
        }
        u32 m = r.below(nm);
        t.material = g_mats[m]; t.matIndex = (RwUInt16)m; t.textureIndex = (RwUInt16)m; t.rasterIndex = (RwUInt16)(m & 1); t.pipelineIndex = 0;
        s.tris.push_back(t);
    }
    static const float qs[] = {0.05f, 0.1f, 0.3f, 0.5f, 1.0f, 3.0f, 5.0f};
    s.quality = qs[r.below(7)]; s.limit = r.chance(3) ? 0 : 1 + r.below(r.chance(20) ? 40 : 8); s.ignoreWinding = (int)r.below(2);
    return s;
}

struct Out { u32 ret; u32 h; size_t brk; u32 logh; u32 extra; };
static Out snapshot(u32 ret, u32 extra = 0) { Out o; o.ret = ret; o.brk = g_brk; o.h = fnv(g_ar, g_brk); o.logh = fnv(g_log.data(), g_log.size() * 4); o.extra = extra; return o; }
static bool same(const Out& a, const Out& b) { return a.ret == b.ret && a.brk == b.brk && a.h == b.h && a.logh == b.logh && a.extra == b.extra; }

typedef RpTriStripMesh* (__cdecl* CreateFn)(RpBuildMeshTriangle**, RwUInt32);
typedef RpTriStripMesh* (__cdecl* TunnelFn)(RpTriStripMesh*, void*);
typedef void (__cdecl* EmitFn)(RpTriStripMesh*, RpMesh*, RpTriStripData*);
typedef RpMeshHeader* (__cdecl* GenFn)(RpBuildMesh*, void*);

static Patch g_neg = {0x75BB80, nullptr, {0}, false};
int main(int argc, char** argv) {
    bool neg = argc > 4 && !strcmp(argv[4], "neg");
    const char* exe = argc > 1 ? argv[1] : "E:\\JUEGOS\\GTA San Andreas - CVehicle Test\\gta_sa.exe";
    int N = argc > 2 ? atoi(argv[2]) : 500; u32 seed = argc > 3 ? (u32)atoi(argv[3]) : 1;
    if (!map_exe(exe)) return 2;
    g_ar = (u8*)VirtualAlloc(nullptr, ARENA, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); g_snap = (u8*)VirtualAlloc(nullptr, ARENA, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    // motor falso
    *(void**)(g_engine + 0x134) = (void*)rw_malloc; *(void**)(g_engine + 0x138) = (void*)rw_free; *(void**)(g_engine + 0x13C) = (void*)rw_realloc;
    *(void**)0xC97B24 = g_engine;
    _controlfp(_PC_24, _MCW_PC);
    Rng rng(seed); R = &rng;
    int fails = 0, runs = 0; u64 tunnelsApplied = 0;
    for (int it = 0; it < N; it++) {
        g_brk = 0; g_log.clear();
        for (int m = 0; m < 4; m++) { u8* mat = (u8*)ar_alloc_fill(0x1C); memset(mat, 0, 0x1C); mat[4] = 0xFF; mat[5] = 0xFF; mat[6] = 0xFF; mat[7] = (m == 3) ? 0x80 : 0xFF; g_mats[m] = (RpMaterial*)mat; }
        Scenario sc = make(rng, true);
        RpBuildMeshTriangle* tris = (RpBuildMeshTriangle*)ar_alloc_fill(sc.nt * sizeof(RpBuildMeshTriangle)); memcpy(tris, sc.tris.data(), sc.nt * sizeof(RpBuildMeshTriangle));
        RpBuildMeshTriangle** ptrs = (RpBuildMeshTriangle**)ar_alloc_fill(sc.nt * 4);
        for (u32 i = 0; i < sc.nt; i++) ptrs[i] = tris + i;
        RpTriStripTunnelData* td = (RpTriStripTunnelData*)ar_alloc_fill(sizeof(RpTriStripTunnelData));
        td->quality = sc.quality; td->lengthLimit = sc.limit; td->costCB = bench_cost; td->data = (void*)(uintptr_t)(it);
        RpMesh* rec = (RpMesh*)ar_alloc_fill(sizeof(RpMesh)); RxVertexIndex* idx = (RxVertexIndex*)ar_alloc_fill(sc.nt * 12 + 64);
        RpTriStripData* sd = (RpTriStripData*)ar_alloc_fill(sizeof(RpTriStripData));
        arena_save();
        // --- 1. TriStripMeshCreate (0x75C660)
        { StaticRef<RwUInt32>(0x8D6390) = 1; Out a = snapshot((u32)((CreateFn)0x75C660)(ptrs, sc.nt)); arena_restore();
          StaticRef<RwUInt32>(0x8D6390) = 1; Out b = snapshot((u32)TriStripMeshCreate(ptrs, sc.nt)); runs++; if (!same(a, b)) { fails++; printf("it=%d CREATE difiere (ret %08x/%08x brk %zx/%zx h %08x/%08x log %08x/%08x)\n", it, a.ret, b.ret, a.brk, b.brk, a.h, b.h, a.logh, b.logh); } arena_restore(); }
        // --- 2. Tunnel (0x75B780) sobre la malla del EXE
        RpTriStripMesh* mesh = ((CreateFn)0x75C660)(ptrs, sc.nt); u32 meshBrk = (u32)g_brk; (void)meshBrk;
        arena_save();
        { if (neg) patch_on(g_neg); StaticRef<RwUInt32>(0x8D6390) = 1 + (it % 7); Out a = snapshot((u32)((TunnelFn)0x75B780)(mesh, td), StaticRef<RwUInt32>(0x8D6390)); arena_restore(); patch_off(g_neg);
          StaticRef<RwUInt32>(0x8D6390) = 1 + (it % 7); Out b = snapshot((u32)RpTriStripMeshTunnel(mesh, td), StaticRef<RwUInt32>(0x8D6390)); runs++;
          if (!same(a, b)) { fails++; printf("it=%d TUNNEL difiere (nt=%u limit=%u q=%g ret %08x/%08x h %08x/%08x log %08x/%08x tf %u/%u)\n", it, sc.nt, sc.limit, sc.quality, a.ret, b.ret, a.h, b.h, a.logh, b.logh, a.extra, b.extra); }
          // deja la malla con el resultado del EXE para la prueba siguiente
          arena_restore(); StaticRef<RwUInt32>(0x8D6390) = 1 + (it % 7); ((TunnelFn)0x75B780)(mesh, td); for (RwLLLink* l = mesh->polygonUsedList.link.next; l != &mesh->polygonUsedList.link; l = l->next) tunnelsApplied += ((RpTriStripPolygon*)((u8*)l - 8))->edges[0]->strip; }
        arena_save();
        // --- 3. TriStripEmit (0x75C9D0) y 4. vecinos (0x75D240), sobre la malla ya procesada
        for (int w = 0; w < 2; w++) {
            sd->ignoreWinding = w; sd->stripCB = nullptr; sd->data = nullptr;
            rec->indices = idx; rec->numIndices = 0; rec->material = g_mats[0]; memset(idx, 0xCD, sc.nt * 12 + 64);
            arena_save();
            ((EmitFn)0x75C9D0)(mesh, rec, sd); Out a = snapshot(rec->numIndices, fnv(idx, sc.nt * 12 + 64)); arena_restore();
            rec->indices = idx; rec->numIndices = 0; memset(idx, 0xCD, sc.nt * 12 + 64);
            TriStripEmit(mesh, rec, sd); Out b = snapshot(rec->numIndices, fnv(idx, sc.nt * 12 + 64)); runs++;
            if (!same(a, b)) { fails++; printf("it=%d EMIT(w=%d) difiere (n %u/%u h %08x/%08x idx %08x/%08x)\n", it, w, a.ret, b.ret, a.h, b.h, a.extra, b.extra); }
            arena_restore();
        }
        // --- 5. raiz completa (0x75C380) con el Tunnel del EXE contra la de Tunnel propio
        arena_restore();   // vuelve a antes de la malla tunelada: la raiz crea la suya
        g_brk = g_snap_brk;
        {
            Scenario s2 = sc; (void)s2;
            RpBuildMesh* bm = (RpBuildMesh*)ar_alloc_fill(sizeof(RpBuildMesh)); bm->triangleBufferSize = sc.nt; bm->numTriangles = sc.nt; bm->meshTriangles = tris;
            // inyecta triangulos degenerados en algunos casos
            if (rng.chance(30) && sc.nt > 2) { tris[1].vertIndex[1] = tris[1].vertIndex[0]; }
            RpTriStripData* sd1 = (RpTriStripData*)ar_alloc_fill(sizeof(RpTriStripData)); sd1->ignoreWinding = sc.ignoreWinding; sd1->stripCB = (RpTriStripMeshStripCallBack)0x75B780; sd1->data = td;
            RpTriStripData* sd2 = (RpTriStripData*)ar_alloc_fill(sizeof(RpTriStripData)); sd2->ignoreWinding = sc.ignoreWinding; sd2->stripCB = RpTriStripMeshTunnel; sd2->data = td;
            arena_save();
            StaticRef<RwUInt32>(0x8D6390) = 1; Out a = snapshot((u32)((GenFn)0x75C380)(bm, sd1), StaticRef<RwUInt32>(0x8D6390)); arena_restore();
            StaticRef<RwUInt32>(0x8D6390) = 1; Out b = snapshot((u32)RpBuildMeshGenerateTriStrip(bm, sd2), StaticRef<RwUInt32>(0x8D6390)); runs++;
            if (!same(a, b)) { fails++; printf("it=%d RAIZ difiere (nt=%u limit=%u w=%d ret %08x/%08x brk %zx/%zx h %08x/%08x log %08x/%08x)\n", it, sc.nt, sc.limit, sc.ignoreWinding, a.ret, b.ret, a.brk, b.brk, a.h, b.h, a.logh, b.logh); }
        }
    }
    // ---- RpGeometrySortByMaterial (0x75DAE0)
    {
        Patch pc[] = {{0x74CA90, (void*)st_Create, {0}, false}, {0x74C7D0, (void*)st_Lock, {0}, false}, {0x74C800, (void*)st_Unlock, {0}, false}, {0x74C4D0, (void*)st_AddMorph, {0}, false},
                      {0x74CCC0, (void*)st_Destroy, {0}, false}, {0x74C200, (void*)st_Sphere, {0}, false}, {0x74E1F0, (void*)st_MatCopy, {0}, false}};
        for (auto& p : pc) patch_on(p);
        int gfails = 0, nonnull = 0;
        for (int it = 0; it < N; it++) {
            g_brk = 0; g_log.clear(); g_geoFail = 0;
            int failMode = rng.chance(15) ? (int)(1u << rng.below(4)) : 0;
            // registro de plugins de geometria: 0-2 entradas con datos dentro de los 0x20 bytes tras la cabecera
            RwPluginRegEntry* e1 = (RwPluginRegEntry*)ar_alloc_fill(sizeof(RwPluginRegEntry)); RwPluginRegEntry* e2 = (RwPluginRegEntry*)ar_alloc_fill(sizeof(RwPluginRegEntry));
            memset(e1, 0, sizeof(*e1)); memset(e2, 0, sizeof(*e2)); e1->offset = 0x60; e1->size = 8 + 4 * rng.below(3); e2->offset = 0x70; e2->size = 4 * rng.below(5);
            u32 np = rng.below(3); e1->next = np > 1 ? e2 : nullptr; StaticRef<RwPluginRegEntry*>(0x8D629C) = np ? e1 : nullptr;
            for (int m = 0; m < 4; m++) { u8* mat = (u8*)ar_alloc_fill(0x1C); memset(mat, 0, 0x1C); g_mats[m] = (RpMaterial*)mat; }
            RwInt32 nv = 3 + rng.below(rng.chance(10) ? 120 : 30), nt = 1 + rng.below(rng.chance(10) ? 90 : 30), nm = 1 + rng.below(4), nmorph = 1 + rng.below(3);
            u32 tcs = rng.below(3); u32 fl = (tcs ? 4 : 0) | (rng.chance(50) ? 8 : 0) | (rng.chance(50) ? 0x10 : 0) | (rng.chance(10) ? 0x1000000 : 0);
            RpGeometry* g = st_Create(nv, nt, (fl & 0xFFFF) | (tcs << 16)); g->flags = fl;
            for (RwInt32 k = 1; k < nmorph; k++) st_AddMorph(g);
            RpMaterial** mats = (RpMaterial**)ar_alloc_fill(nm * 4); for (RwInt32 k = 0; k < nm; k++) mats[k] = g_mats[k];
            g->matList.materials = mats; g->matList.numMaterials = nm; g->matList.space = nm;
            u16* tr = (u16*)g->triangles;
            for (RwInt32 t = 0; t < nt; t++) { for (int k = 0; k < 3; k++) tr[t * 4 + k] = (u16)rng.below(nv); tr[t * 4 + 3] = (u16)rng.below(nm); }
            for (RwInt32 m = 0; m < nmorph; m++) { for (RwInt32 v = 0; v < nv * 3; v++) g->morphTarget[m].verts[v] = (float)(int)rng.below(1000) / 8; if (g->morphTarget[m].normals) for (RwInt32 v = 0; v < nv * 3; v++) g->morphTarget[m].normals[v] = (float)(int)rng.below(100) / 10; }
            if (g->preLitLum) for (RwInt32 v = 0; v < nv; v++) g->preLitLum[v] = rng.next();
            for (u32 k = 0; k < tcs; k++) for (RwInt32 v = 0; v < nv * 2; v++) g->texCoords[k][v] = (float)(int)rng.below(64) / 16;
            RpGeometrySortByMaterialCallBack callback = rng.chance(75) ? bench_sort_cb : nullptr;
            g_geoFail = failMode;
            arena_save();
            static Patch negc = {0x75E230, nullptr, {0}, false}; if (argc > 4 && !strcmp(argv[4], "negg")) patch_on(negc);
            Out a = snapshot((u32)((RpGeometry*(__cdecl*)(const RpGeometry*, RpGeometrySortByMaterialCallBack))0x75DAE0)(g, callback)); arena_restore(); patch_off(negc); if (a.ret) nonnull++;
            Out b = snapshot((u32)RpGeometrySortByMaterial(g, callback));
            if (!same(a, b)) { gfails++; printf("geom it=%d difiere (nv=%d nt=%d nm=%d morph=%d fl=%x tcs=%u fail=%d ret %08x/%08x brk %zx/%zx h %08x/%08x log %08x/%08x)\n", it, nv, nt, nm, nmorph, fl, tcs, failMode, a.ret, b.ret, a.brk, b.brk, a.h, b.h, a.logh, b.logh); }
        }
        printf("geometria: escenarios=%d (con resultado: %d) diferencias=%d\n", N, nonnull, gfails); fails += gfails;
    }
    printf("escenarios=%d comparaciones=%d diferencias=%d (aristas de tira finales sumadas: %llu)\n", N, runs, fails, (unsigned long long)tunnelsApplied);
    return fails ? 1 : 0;
}
