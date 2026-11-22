// Banco CAM: las envolturas del plugin 0x509 de RpWorld del EXE original contra su reconstruccion C++ de source/game_sa/RenderWare/rw/rpworld.cpp, fuera del juego. Metodo FnP3/FnD3DStd: el gta_sa.exe original
// se mapea en 0x400000 (reserva_imagen.asm) y se llama por su direccion; el texto C++ (CameraWorldPluginBeginUpdate/EndUpdate/Sync, LightWorldPluginSync, AtomicWorldPluginSync y los bucles de RpTie que
// comparten) se extrae tal cual con extract_cam.py a cam_func.inc y se compila aqui con tipos minimos de la misma disposicion. Las llamadas externas (RwFrameGetLTM 0x7F0990, _rwerror 0x8088D0, RwErrorSet 0x808820,
// RpAtomicGetWorldBoundingSphere 0x749330) se desvian a falsos con traza en el EXE; las funciones del motor (rwmalloc +0x134, rwrealloc +0x13C, memoryAlloc +0x144, memoryFree +0x148, hint de orden +0xA0) y los
// callbacks originales guardados en los datos de los plugins son los mismos falsos para los dos lados. Estado por semilla: BSP aleatorio (planos en los tres ejes, hojas con caja, valores en una rejilla con
// empates, -0 y NaN), camaras con 6 planos y caja del frustum, luces y atomos con RpTie previos, reservas de sectores de distinta capacidad y fallos de memoria. Se compara el valor devuelto, la arena completa,
// el estado del motor (curWorld, renderFrame) y la traza completa. x87 a 24 bits (los dos lados se compilan con /arch:IA32).
// Uso: bench_cam.exe <ruta del EXE> [--casos N] [--semilla S]   (devuelve 0 si todo coincide)
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <cstring>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <float.h>
#include <vector>
#include <random>
#include <algorithm>
#include <map>

typedef int32_t RwInt32; typedef uint32_t RwUInt32; typedef uint8_t RwUInt8; typedef uint16_t RwUInt16; typedef int32_t RwBool; typedef float RwReal;
template<typename T> static T& G(uintptr_t a) { return *(T*)a; }
#define UNUSED(x) (void)(x)

// ---------------------------------------------------------------- tipos minimos (misma disposicion que rwplcore.h / rwcore.h / rpworld.h)
struct RwV3d { RwReal x, y, z; };
struct RwBBox { RwV3d sup, inf; };
struct RwSphere { RwV3d center; RwReal radius; };
struct RwLLLink { RwLLLink* next; RwLLLink* prev; };
struct RwLinkList { RwLLLink link; };
#define rwLinkListAddLLLink(list, linkvar) \
    ((linkvar)->next = (list)->link.next, (linkvar)->prev = (&(list)->link), ((list)->link.next)->prev = (linkvar), (list)->link.next = (linkvar))
#define rwLinkListRemoveLLLink(linkvar) (((linkvar)->prev)->next = (linkvar)->next, ((linkvar)->next)->prev = (linkvar)->prev)
struct RwObject { RwUInt8 type, subType, flags, privateFlags; void* parent; };
struct RwObjectHasFrame { RwObject object; RwLLLink lFrame; void* sync; };
struct RwMatrix { RwV3d right; RwUInt32 flags; RwV3d up; RwUInt32 pad1; RwV3d at; RwUInt32 pad2; RwV3d pos; RwUInt32 pad3; };
struct RwFrame { RwMatrix ltm; };
struct RwPlane { RwV3d normal; RwReal distance; };
struct RwFrustumPlane { RwPlane plane; RwUInt8 closestX, closestY, closestZ, pad; };
struct RwCamera {
    RwObjectHasFrame object; RwUInt8 pad0[0x94 - 0x14]; RwFrustumPlane frustumPlanes[6]; RwBBox frustumBoundBox; RwV3d frustumCorners[8]; RwUInt8 plugin[0x40];
};
static_assert(offsetof(RwCamera, frustumPlanes) == 0x94 && offsetof(RwCamera, frustumBoundBox) == 0x10C && sizeof(RwFrustumPlane) == 0x14);
struct RpSector { RwInt32 type; };
struct RpPlaneSector { RwInt32 type; RwReal value; RpSector* leftSubTree; RpSector* rightSubTree; RwReal leftValue; RwReal rightValue; };
struct RpWorldSector {
    RwInt32 type; RwUInt8 pad0[0x34]; RwLinkList collAtomicsInWorldSector; RwLinkList lightsInWorldSector; RwBBox boundingBox; RwBBox tightBoundingBox; RwUInt8 pad1[0x100 - 0x78];
};
static_assert(offsetof(RpWorldSector, collAtomicsInWorldSector) == 0x38 && offsetof(RpWorldSector, lightsInWorldSector) == 0x40 && offsetof(RpWorldSector, boundingBox) == 0x48);
struct RpWorld { RwObject object; RwUInt32 flags; RwInt32 renderOrder; RwUInt8 matList[12]; RpSector* rootSector; RwUInt8 pad[0x40]; };
static_assert(offsetof(RpWorld, renderOrder) == 0xC && offsetof(RpWorld, rootSector) == 0x1C);
struct RpLight { RwObjectHasFrame object; RwReal radius; RwReal color[4]; RwReal minusCosAngle; RwLinkList WorldSectorsInLight; RwLLLink inWorld; RwUInt16 lightFrame; RwUInt16 pad; };
static_assert(offsetof(RpLight, radius) == 0x14 && offsetof(RpLight, WorldSectorsInLight) == 0x2C && offsetof(RpLight, inWorld) == 0x34);
struct RpAtomic {
    RwObjectHasFrame object; void* repEntry; void* geometry; RwSphere boundingSphere; RwSphere worldBoundingSphere; void* clump; RwLLLink inClumpLink; void* renderCallBack;
    RwUInt8 interpolator[0x14]; RwUInt16 renderFrame; RwUInt16 pad; RwLinkList llWorldSectorsInAtomic; void* pipeline;
};
static_assert(offsetof(RpAtomic, worldBoundingSphere) == 0x2C && offsetof(RpAtomic, renderFrame) == 0x60 && offsetof(RpAtomic, llWorldSectorsInAtomic) == 0x64);
struct RpTie { RwLLLink lAtomicInWorldSector; RpAtomic* apAtom; RwLLLink lWorldSectorInAtomic; RpWorldSector* worldSector; };
struct RpLightTie { RwLLLink lightInWorldSector; RpLight* light; RwLLLink WorldSectorInLight; RpWorldSector* sect; };
static_assert(sizeof(RpTie) == 0x18 && offsetof(RpTie, lWorldSectorInAtomic) == 0xC && offsetof(RpLightTie, WorldSectorInLight) == 0xC);
struct RwFreeList;
struct RwError { RwInt32 pluginID; RwInt32 errorCode; };
typedef RwBool(*RwStandardFunc)(void*, void*, RwInt32);
#define rwSTANDARDHINTRENDERF2B 22
struct RwMemoryFunctions { void* (__cdecl* rwmalloc)(size_t, RwUInt32); void(__cdecl* rwfree)(void*); void* (__cdecl* rwrealloc)(void*, size_t, RwUInt32); void* (__cdecl* rwcalloc)(size_t, size_t, RwUInt32); };
struct RwGlobals {
    void* curCamera; void* curWorld; RwUInt16 renderFrame; RwUInt16 lightFrame; RwUInt16 pad[2]; RwUInt8 device[0x38]; RwStandardFunc stdFunc[29]; RwUInt8 pad2[0x134 - 0x10 - 0x38 - 29 * 4];
    RwMemoryFunctions memoryFuncs; void* (__cdecl* memoryAlloc)(RwFreeList*, RwUInt32); RwFreeList* (__cdecl* memoryFree)(RwFreeList*, void*); RwUInt8 tail[0x100];
};
static_assert(offsetof(RwGlobals, stdFunc) + 22 * 4 == 0xA0 && offsetof(RwGlobals, memoryFuncs) == 0x134 && offsetof(RwGlobals, memoryAlloc) == 0x144 && offsetof(RwGlobals, memoryFree) == 0x148);
static RwUInt8* EngineBase() { return G<RwUInt8*>(0xC97B24); }
#define RwEngineInstance (reinterpret_cast<RwGlobals*>(EngineBase()))
#define RwMalloc(_s, _h) (RwEngineInstance->memoryFuncs.rwmalloc((_s), (_h)))
#define RwRealloc(_p, _s, _h) (RwEngineInstance->memoryFuncs.rwrealloc((_p), (_s), (_h)))
#define RwFreeListFree(_f, _p) (RwEngineInstance->memoryFree((_f), (_p)))
struct TieModuleGlobals { RwFreeList* tieFreeList; RwFreeList* lightTieFreeList; RwUInt32 tieFrame; };
static TieModuleGlobals& TieGlobals() { return *reinterpret_cast<TieModuleGlobals*>(EngineBase() + G<RwInt32>(0xC9259C)); }
static void* FreeListAlloc(RwFreeList* freeList, RwUInt32 hint) { return RwEngineInstance->memoryAlloc(freeList, hint); }
static auto& s_cameraWorldOffset = G<RwInt32>(0xC925A4);
static auto& s_atomicWorldOffset = G<RwInt32>(0xC925A8);
static auto& s_lightWorldOffset  = G<RwInt32>(0xC925B0);
constexpr RwInt32 kErrorNoMemory = static_cast<RwInt32>(0x80000013);

// ---------------------------------------------------------------- traza, arena y falsos
static std::vector<uint32_t> g_log;
static void L(uint32_t a) { g_log.push_back(a); if (getenv("BENCH_TRACE2")) printf("L %x\n", a); }
static void L(uint32_t a, uint32_t b) { L(a); L(b); }
static void L(uint32_t a, uint32_t b, uint32_t c) { L(a); L(b); L(c); }
static void L(uint32_t a, uint32_t b, uint32_t c, uint32_t d) { L(a); L(b); L(c); L(d); }
static uint8_t g_arena[1 << 20]; static size_t g_arenaUsed;
static void* arena_alloc(size_t n, uint8_t fill = 0xCD) { n = (n + 15) & ~15u; void* p = g_arena + g_arenaUsed; g_arenaUsed += n; if (g_arenaUsed > sizeof g_arena) { printf("arena agotada\n"); ExitProcess(4); } memset(p, fill, n); return p; }
static uint32_t id_of(const void* p) { return (uint32_t)(uintptr_t)p; }
static uint32_t g_hseed;
static uint32_t H(uint32_t a, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0) {
    uint64_t x = g_hseed * 0x9E3779B97F4A7C15ull; for (uint32_t v : {a, b, c, d}) { x ^= v + 0x9E3779B9u + (x << 6) + (x >> 2); x *= 0xFF51AFD7ED558CCDull; } return (uint32_t)(x >> 32);
}
static std::mt19937_64 g_caseRng;
static uint32_t cr(uint32_t n) { return n ? (uint32_t)(g_caseRng() % n) : 0; }

static void* __cdecl EngMalloc(size_t size, RwUInt32 hint) { L(0xE01, (uint32_t)size, hint); if (H(0xE01, (uint32_t)size) % 6 == 0) { L(0); return nullptr; } void* p = arena_alloc(size); L(id_of(p)); return p; }
static void __cdecl EngFree(void* p) { L(0xE02, id_of(p)); }
static void* __cdecl EngRealloc(void* old, size_t size, RwUInt32 hint) {
    L(0xE07, id_of(old), (uint32_t)size); L(hint);
    if (H(0xE07, (uint32_t)size) % 6 == 0) { L(0); return nullptr; }
    void* p = arena_alloc(size); if (size >= 0xC8) memcpy(p, old, size - 0xC8); L(id_of(p)); return p;
}
static void* __cdecl FreeAlloc(RwFreeList* list, RwUInt32 hint) { L(0xE30, id_of(list), hint); void* p = arena_alloc(0x18); L(id_of(p)); return p; }
static RwFreeList* __cdecl FreeFree(RwFreeList* list, void* p) { L(0xE31, id_of(list), id_of(p)); return list; }
static RwBool __cdecl HintFn(void* a, void* b, RwInt32 f) { L(0xE40, id_of(a), id_of(b), (uint32_t)f); return 1; }
static RwMatrix* __cdecl RwFrameGetLTM(RwFrame* f) { L(0xE20, id_of(f)); return &f->ltm; }
static const RwSphere* __cdecl RpAtomicGetWorldBoundingSphere(RpAtomic* a) { L(0xE21, id_of(a)); return &a->worldBoundingSphere; }
static int __cdecl FakeRwError(int code, int arg) { L(0xE10, (uint32_t)code, code == kErrorNoMemory ? (uint32_t)arg : 0u); return code; } // el EXE solo pasa un segundo argumento con 0x80000013
static int __cdecl FakeRwErrorSet(RwError* e) { L(0xE11, (uint32_t)e->pluginID, (uint32_t)e->errorCode); return 0; }
static void RaiseRwError(RwInt32 code) { RwError e; e.pluginID = 2; e.errorCode = FakeRwError(code, 0); FakeRwErrorSet(&e); }
static void RaiseRwError(RwInt32 code, RwUInt32 arg) { RwError e; e.pluginID = 2; e.errorCode = FakeRwError(code, (int)arg); FakeRwErrorSet(&e); }
static RwCamera* __cdecl OrigCamBegin(RwCamera* c) { L(0xE51, id_of(c)); return c; }
static RwCamera* __cdecl OrigCamEnd(RwCamera* c) { L(0xE52, id_of(c)); return H(0xE52, id_of(c)) % 3 == 0 ? nullptr : c; }
static RwCamera* __cdecl OrigCamSync(RwCamera* c) { L(0xE50, id_of(c)); return H(0xE50, id_of(c)) % 7 == 0 ? nullptr : c; }
static RpLight* __cdecl OrigLightSync(RpLight* l) { L(0xE53, id_of(l)); return H(0xE53, id_of(l)) % 6 == 0 ? nullptr : l; }
static RpAtomic* __cdecl OrigAtomicSync(RpAtomic* a) { L(0xE54, id_of(a)); return H(0xE54, id_of(a)) % 6 == 0 ? nullptr : a; }

#include "cam_func.inc"

// ---------------------------------------------------------------- mapa del EXE
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

// ---------------------------------------------------------------- estado por semilla
static RwReal rf() {
    const uint32_t k = cr(60);
    if (k == 0) return -0.0f;
    if (k == 1) { uint32_t b = 0x7FC00000u; RwReal v; memcpy(&v, &b, 4); return v; }
    return ((int)cr(25) - 12) * 0.25f;
}
static RwReal rpos() { return (RwReal)(1 + cr(8)) * 0.25f; }
static std::vector<RpWorldSector*> g_leaves;
static RpSector* build(int depth) {
    if (depth == 0 || cr(4) == 0) {
        auto* s = (RpWorldSector*)arena_alloc(sizeof(RpWorldSector)); s->type = -1;
        s->collAtomicsInWorldSector.link.next = s->collAtomicsInWorldSector.link.prev = &s->collAtomicsInWorldSector.link;
        s->lightsInWorldSector.link.next = s->lightsInWorldSector.link.prev = &s->lightsInWorldSector.link;
        s->boundingBox.inf = { rf(), rf(), rf() }; s->boundingBox.sup = { rf(), rf(), rf() };
        g_leaves.push_back(s); return (RpSector*)s;
    }
    auto* p = (RpPlaneSector*)arena_alloc(sizeof(RpPlaneSector)); p->type = (RwInt32)(cr(3) * 4); p->value = rf(); p->leftValue = rf(); p->rightValue = rf();
    p->leftSubTree = build(depth - 1); p->rightSubTree = build(depth - 1); return (RpSector*)p;
}
static RpWorld* makeWorld() {
    g_leaves.clear(); auto* w = (RpWorld*)arena_alloc(sizeof(RpWorld)); w->renderOrder = (RwInt32)cr(3); w->rootSector = build(1 + cr(5)); return w;
}
static RwFrame* makeFrame() { auto* f = (RwFrame*)arena_alloc(sizeof(RwFrame)); f->ltm.pos = { rf(), rf(), rf() }; return f; }
static uint8_t g_engine[0x400];
enum Kind { K_BEGIN, K_END, K_CAMSYNC, K_LIGHT, K_ATOMIC, K_COUNT };
struct Args { Kind kind; void* object; };
static Args setup(uint64_t seed, Kind kind) {
    g_caseRng.seed(seed); g_hseed = (uint32_t)seed * 2654435761u; g_log.clear(); g_arenaUsed = 0x100; memset(g_arena, 0, 0x100);
    memset(g_engine, 0, sizeof g_engine);
    auto* eng = (RwGlobals*)g_engine;
    eng->memoryFuncs.rwmalloc = EngMalloc; eng->memoryFuncs.rwfree = EngFree; eng->memoryFuncs.rwrealloc = EngRealloc; eng->memoryAlloc = FreeAlloc; eng->memoryFree = FreeFree; eng->stdFunc[rwSTANDARDHINTRENDERF2B] = HintFn;
    eng->curWorld = (void*)(uintptr_t)(0x1000 + cr(16)); eng->renderFrame = (RwUInt16)cr(65536);
    G<uint8_t*>(0xC97B24) = g_engine; G<RwInt32>(0xC9259C) = 0x200; *(void**)(g_engine + 0x200) = (void*)(uintptr_t)0xA0A0; *(void**)(g_engine + 0x204) = (void*)(uintptr_t)0xB0B0;
    G<RwInt32>(0xC925A4) = 0x190; G<RwInt32>(0xC925A8) = 0x70; G<RwInt32>(0xC925B0) = 0x40;
    Args a{ kind, nullptr };
    RpWorld* world = cr(8) == 0 ? nullptr : makeWorld();
    if (kind == K_BEGIN || kind == K_END || kind == K_CAMSYNC) {
        auto* cam = (RwCamera*)arena_alloc(sizeof(RwCamera) + 0x40);
        cam->object.object.parent = cr(10) == 0 ? nullptr : makeFrame();
        const bool lenient = cr(3) != 0; // la mayoria de los casos dejan pasar casi todas las hojas (si no, casi ninguna llega al arreglo de sectores)
        for (auto& pl : cam->frustumPlanes) { pl.plane.normal = { rf(), rf(), rf() }; pl.plane.distance = (lenient && cr(8) != 0) ? 50.0f : rf(); pl.closestX = (RwUInt8)cr(2); pl.closestY = (RwUInt8)cr(2); pl.closestZ = (RwUInt8)cr(2); }
        cam->frustumBoundBox.inf = { rf(), rf(), rf() }; cam->frustumBoundBox.sup = { rf(), rf(), rf() };
        if (kind == K_CAMSYNC && !cam->object.object.parent) cam->object.object.parent = makeFrame(); // 0x7F0990 con marco nulo leeria de la direccion 0: el juego siempre tiene marco
        auto* d = (CameraWorldData*)((uint8_t*)cam + 0x190);
        d->world = world; d->beginUpdate = OrigCamBegin; d->endUpdate = OrigCamEnd; d->sync = OrigCamSync;
        const bool hasArray = cr(3) != 0; const uint32_t cap = !hasArray ? 0 : (cr(4) == 0 ? 50 : cr(4));
        d->capacity = (RwInt32)cap; d->count = (RwInt32)cr(60);
        d->sectors = hasArray ? (RpWorldSector**)arena_alloc(cap * 4 + 0xC8, 0xAB) : nullptr;
        a.object = cam;
    } else if (kind == K_LIGHT) {
        auto* light = (RpLight*)arena_alloc(0x60);
        static const RwUInt8 st[] = { 1, 2, 0x80, 0x81, 0x82, 0x80, 0x81 };
        light->object.object.subType = st[cr(7)]; light->object.object.parent = cr(8) == 0 ? nullptr : makeFrame(); light->radius = rpos();
        light->WorldSectorsInLight.link.next = light->WorldSectorsInLight.link.prev = &light->WorldSectorsInLight.link;
        auto* d = (RwUInt8*)light + 0x40; *(RpWorld**)d = world; *(void**)(d + 4) = (void*)OrigLightSync;
        if (world) for (uint32_t k = cr(4); k > 0 && !g_leaves.empty(); k--) {
            RpWorldSector* s = g_leaves[cr((uint32_t)g_leaves.size())]; auto* t = (RpLightTie*)arena_alloc(sizeof(RpLightTie));
            rwLinkListAddLLLink(&s->lightsInWorldSector, &t->lightInWorldSector); t->light = light; rwLinkListAddLLLink(&light->WorldSectorsInLight, &t->WorldSectorInLight); t->sect = s;
        }
        a.object = light;
    } else {
        auto* atom = (RpAtomic*)arena_alloc(0x80);
        atom->worldBoundingSphere.center = { rf(), rf(), rf() }; atom->worldBoundingSphere.radius = rpos();
        atom->llWorldSectorsInAtomic.link.next = atom->llWorldSectorsInAtomic.link.prev = &atom->llWorldSectorsInAtomic.link;
        auto* d = (RwUInt8*)atom + 0x70; *(RpWorld**)d = world; *(void**)(d + 4) = (void*)OrigAtomicSync;
        if (world) for (uint32_t k = cr(4); k > 0 && !g_leaves.empty(); k--) {
            RpWorldSector* s = g_leaves[cr((uint32_t)g_leaves.size())]; auto* t = (RpTie*)arena_alloc(sizeof(RpTie)); const uint32_t mode = cr(5); // 0: sin atomo, 1: sin sector (no se desatan)
            if (mode >= 2) { rwLinkListAddLLLink(&s->collAtomicsInWorldSector, &t->lAtomicInWorldSector); t->apAtom = atom; t->worldSector = s; }
            else { t->apAtom = mode == 0 ? nullptr : atom; t->worldSector = mode == 1 ? nullptr : s; }
            rwLinkListAddLLLink(&atom->llWorldSectorsInAtomic, &t->lWorldSectorInAtomic);
        }
        a.object = atom;
    }
    return a;
}
static void* call_exe(const Args& a) {
    switch (a.kind) {
    case K_BEGIN: return ((void* (__cdecl*)(void*))0x750430)(a.object);
    case K_END: return ((void* (__cdecl*)(void*))0x750460)(a.object);
    case K_CAMSYNC: return ((void* (__cdecl*)(void*))0x750480)(a.object);
    case K_LIGHT: return ((void* (__cdecl*)(void*))0x750AC0)(a.object);
    default: return ((void* (__cdecl*)(void*))0x7507E0)(a.object);
    }
}
static void* call_cpp(const Args& a) {
    switch (a.kind) {
    case K_BEGIN: return CameraWorldPluginBeginUpdate((RwCamera*)a.object);
    case K_END: return CameraWorldPluginEndUpdate((RwCamera*)a.object);
    case K_CAMSYNC: return CameraWorldPluginSync((RwCamera*)a.object);
    case K_LIGHT: return LightWorldPluginSync((RpLight*)a.object);
    default: return AtomicWorldPluginSync((RpAtomic*)a.object);
    }
}
static std::vector<uint32_t> observe(void* ret) {
    std::vector<uint32_t> o; o.push_back(id_of(ret)); const auto* e = (RwGlobals*)g_engine; o.push_back(id_of(e->curWorld)); o.push_back(e->renderFrame);
    const uint32_t* p = (const uint32_t*)g_arena; for (size_t i = 0; i < g_arenaUsed / 4; i++) o.push_back(p[i]);
    o.insert(o.end(), g_log.begin(), g_log.end());
    return o;
}
static LONG WINAPI Crash(EXCEPTION_POINTERS* e) {
    printf("EXCEPCION %08lx en %p (acceso %p)\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, e->ExceptionRecord->NumberParameters > 1 ? (void*)e->ExceptionRecord->ExceptionInformation[1] : nullptr);
    fflush(stdout); ExitProcess(3); return EXCEPTION_EXECUTE_HANDLER;
}
int main(int argc, char** argv) {
    SetUnhandledExceptionFilter(Crash);
    if (argc < 2) { printf("uso: bench_cam.exe <gta_sa.exe> [--casos N] [--semilla S] [--solo K]\n"); return 2; }
    uint64_t n = 100000, seed = 1; int only = -1;
    for (int i = 2; i < argc; i++) { if (!strcmp(argv[i], "--casos") && i + 1 < argc) n = strtoull(argv[++i], 0, 10); else if (!strcmp(argv[i], "--semilla") && i + 1 < argc) seed = strtoull(argv[++i], 0, 10); else if (!strcmp(argv[i], "--solo") && i + 1 < argc) only = atoi(argv[++i]); }
    setvbuf(stdout, nullptr, _IONBF, 0);
    if (!map_exe(argv[1])) return 2;
    unsigned int cw; _controlfp_s(&cw, _PC_24, _MCW_PC);
    divert(0x7F0990, (void*)&RwFrameGetLTM); divert(0x8088D0, (void*)&FakeRwError); divert(0x808820, (void*)&FakeRwErrorSet); divert(0x749330, (void*)&RpAtomicGetWorldBoundingSphere);
    static const char* names[] = { "0x750430 begin", "0x750460 end", "0x750480 sync de camara", "0x750AC0 sync de luz", "0x7507E0 sync de atomo" };
    uint64_t fails[K_COUNT] = {}, runs[K_COUNT] = {}, nonnull[K_COUNT] = {}; std::map<uint32_t, uint64_t> cov;
    for (uint64_t it = 0; it < n; it++) {
        const Kind kind = (Kind)(it % K_COUNT); if (only >= 0 && kind != (Kind)only) continue;
        const uint64_t cs = seed * 1000003 + it;
        Args a = setup(cs, kind); void* ra = call_exe(a); const std::vector<uint32_t> oa = observe(ra);
        for (uint32_t t : {0xE01u, 0xE07u, 0xE10u, 0xE30u, 0xE31u, 0xE40u, 0xE50u, 0xE53u, 0xE54u}) if (std::find(g_log.begin(), g_log.end(), t) != g_log.end()) cov[t]++;
        Args b = setup(cs, kind); void* rb = call_cpp(b); const std::vector<uint32_t> ob = observe(rb);
        runs[kind]++; if (ra) nonnull[kind]++;
        if (oa != ob) {
            fails[kind]++;
            if (fails[kind] <= 2) {
                printf("DIFERENCIA %s caso=%llu: exe ret=%08x (%zu valores) ours ret=%08x (%zu valores)\n", names[kind], (unsigned long long)cs, id_of(ra), oa.size(), id_of(rb), ob.size());
                for (size_t i = 0; i < std::min(oa.size(), ob.size()); i++) if (oa[i] != ob[i]) { printf("  primer valor distinto en [%zu]: exe=%08x ours=%08x\n", i, oa[i], ob[i]); break; }
            }
        }
    }
    uint64_t total = 0;
    for (int k = 0; k < K_COUNT; k++) { if (!runs[k]) continue; printf("%-26s %llu casos (devuelven != 0: %llu), %llu diferencias\n", names[k], (unsigned long long)runs[k], (unsigned long long)nonnull[k], (unsigned long long)fails[k]); total += fails[k]; }
    printf("cobertura de marcas:"); for (auto& kv : cov) printf(" %x=%llu", kv.first, (unsigned long long)kv.second);
    printf("\nRESULTADO: %s (%llu diferencias)\n", total ? "HAY DIFERENCIAS" : "SIN DIFERENCIAS", (unsigned long long)total);
    return total ? 1 : 0;
}
