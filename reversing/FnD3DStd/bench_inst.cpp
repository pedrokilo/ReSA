// Banco D3DSTD-INST: el instanciado generico de los nodos AllInOne de D3D9 (0x756960, 1.080 bytes) del EXE original contra D3D9InstanceObject de source/game_sa/RenderWare/rw/rpworld_p6b.cpp (el texto de la funcion se
// extrae tal cual con extract_inst.py a inst_func.inc y se compila aqui, con tipos minimos de la misma disposicion), fuera del juego. Metodo FnP3: el gta_sa.exe original se mapea en 0x400000; se desvian a falsos con traza
// (tanto en el EXE como para el C++) RwResourcesAllocateResEntry (0x807ED0), RwResourcesFreeResEntry (0x807DE0), RwD3D9IndexBufferCreate (0x4C9970), las funciones de memoria del motor (+0x134/+0x138), el callback de instanciado y los
// metodos Lock y Unlock del bufer de indices (vtable falsa); 0x756710/0x756770/0x756830 (puras) corren del EXE en los dos lados. Se compara el valor devuelto, la arena (entrada de recursos, cabecera, datos de instancia, mallas),
// el bufer de indices y la traza completa.
// Uso: bench_inst.exe <ruta del EXE> [--casos N] [--semilla S]   (devuelve 0 si todo coincide)
#include "StdInc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <random>
#include <algorithm>
#include <map>

// ---------------------------------------------------------------- tipos minimos (misma disposicion que rpworld.h / rwplcore.h)
struct RwLLLink { RwLLLink* next; RwLLLink* prev; };
struct RwResEntry;
typedef void (*RwResEntryDestroyNotify)(RwResEntry*);
struct RwResEntry { RwLLLink link; RwInt32 size; void* owner; RwResEntry** ownerRef; RwResEntryDestroyNotify destroyNotify; };
struct RpMaterial;
struct RpMesh { RwUInt16* indices; RwUInt32 numIndices; RpMaterial* material; };
struct RpMeshHeader { RwUInt32 flags; RwUInt16 numMeshes; RwUInt16 serialNum; RwUInt32 totalIndicesInMesh; RwUInt32 firstMeshOffset; };
struct RxD3D9VertexStream { void* vertexBuffer; RwUInt32 offset; RwUInt32 stride; RwUInt16 geometryFlags; RwUInt8 managed; RwUInt8 dynamicLock; };
struct RxD3D9ResEntryHeader {
    RwUInt32 serialNumber; RwUInt32 numMeshes; IDirect3DIndexBuffer9* indexBuffer; D3DPRIMITIVETYPE primType; RxD3D9VertexStream vertexStream[2];
    RwBool useOffsets; IDirect3DVertexDeclaration9* vertexDeclaration; RwUInt32 totalNumIndex; RwUInt32 totalNumVertex;
};
struct RxD3D9InstanceData {
    RwUInt32 numIndex; RwUInt32 minVert; RpMaterial* material; RwBool vertexAlpha; IDirect3DVertexShader9* vertexShader; RwUInt32 baseIndex; RwUInt32 numVertices; RwUInt32 startIndex; RwUInt32 numPrimitives;
};
static_assert(sizeof(RxD3D9InstanceData) == 0x24 && sizeof(RxD3D9ResEntryHeader) == 0x40 && sizeof(RwResEntry) == 0x18 && sizeof(RpMesh) == 0xC && sizeof(RpMeshHeader) == 0x10);
typedef RwBool (*RxD3D9AllInOneInstanceCallBack)(void* object, RxD3D9ResEntryHeader* resEntryHeader, RwBool reinstance);
struct RpGeometry { uint8_t pad[0x10]; RwInt32 numTriangles; };
struct RpAtomic { uint8_t pad[0x18]; RpGeometry* geometry; };
struct RpWorldSector { uint8_t pad[0x84]; RwUInt16 numTriangles; };
static_assert(offsetof(RpGeometry, numTriangles) == 0x10 && offsetof(RpAtomic, geometry) == 0x18 && offsetof(RpWorldSector, numTriangles) == 0x84);

template<size_t Slot, typename R, typename... Args>
R ComCall(void* self, Args... args) {
    auto* const vtbl = *reinterpret_cast<void***>(self);
    return reinterpret_cast<R(__stdcall*)(void*, Args...)>(vtbl[Slot])(self, args...);
}
static uint8_t* EngineBase() { return StaticRef<uint8_t*>(0xC97B24); }
#define RwEngineInstance EngineBase()
#define RwMalloc(_s, _h) ((*reinterpret_cast<void*(__cdecl**)(RwUInt32, RwUInt32)>(EngineBase() + 0x134))(_s, _h))
#define RwFree(_p) ((*reinterpret_cast<void(__cdecl**)(void*)>(EngineBase() + 0x138))(_p))
#define RxD3D9ResEntryHeaderDestroyCpp (reinterpret_cast<RwResEntryDestroyNotify>(0x4C9990))

static std::vector<uint32_t> g_log;
static void L(uint32_t a) { g_log.push_back(a); if (getenv("BENCH_TRACE2")) printf("L %x\n", a); }
static void L(uint32_t a, uint32_t b) { L(a); L(b); }
static void L(uint32_t a, uint32_t b, uint32_t c) { L(a); L(b); L(c); }
static void L(uint32_t a, uint32_t b, uint32_t c, uint32_t d) { L(a); L(b); L(c); L(d); }
static uint8_t g_arena[1 << 18]; static size_t g_arenaUsed;
static void* arena_alloc(size_t n, uint8_t fill = 0xCD) { n = (n + 15) & ~15u; void* p = g_arena + g_arenaUsed; g_arenaUsed += n; if (g_arenaUsed > sizeof g_arena) { printf("arena agotada\n"); ExitProcess(4); } memset(p, fill, n); return p; }
static uint32_t id_of(const void* p) { return (uint32_t)(uintptr_t)p; }
static uint32_t g_hseed;
static uint32_t H(uint32_t a, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0) {
    uint64_t x = g_hseed * 0x9E3779B97F4A7C15ull; for (uint32_t v : {a, b, c, d}) { x ^= v + 0x9E3779B9u + (x << 6) + (x >> 2); x *= 0xFF51AFD7ED558CCDull; } return (uint32_t)(x >> 32);
}
static std::mt19937_64 g_caseRng;
static uint32_t cr(uint32_t n) { return n ? (uint32_t)(g_caseRng() % n) : 0; }
template<typename T> static T& G(uintptr_t a) { return *(T*)a; }

static uint8_t g_idx[0x2000];
static void* g_vtblIdx[40]; static void* g_idxObj;
static HRESULT __stdcall Unexpected() { L(0xBAD); return (HRESULT)0x80004005; }
static HRESULT __stdcall Idx_Lock(void* self, UINT off, UINT size, void** pp, DWORD flags) { L(0xE0B, id_of(self), off, size); L(flags); if (H(0xE0B, id_of(self)) % 6 == 0) { L(0); return (HRESULT)0x80004005; } *pp = g_idx; L(1); return 0; }
static HRESULT __stdcall Idx_Unlock(void* self) { L(0xE0C, id_of(self)); return 0; }
static void* __cdecl EngMalloc(uint32_t size, uint32_t hint) { L(0xE01, size, hint); void* p = arena_alloc(size); L(id_of(p)); return p; }
static void __cdecl EngFree(void* p) { L(0xE02, id_of(p)); }
RwResEntry* RwResourcesAllocateResEntry(void* owner, RwResEntry** ownerRef, RwInt32 size, RwResEntryDestroyNotify notify) {
    L(0xE03, id_of(owner), id_of(ownerRef), size); L(id_of((void*)notify));
    auto* e = (RwResEntry*)arena_alloc(size + sizeof(RwResEntry), 0xCD); e->link.next = nullptr; e->link.prev = nullptr; e->size = size; e->owner = owner; e->ownerRef = ownerRef; e->destroyNotify = notify; *ownerRef = e; L(id_of(e)); return e;
}
RwBool RwResourcesFreeResEntry(RwResEntry* e) { L(0xE04, id_of(e)); return 1; }
RwBool RwD3D9IndexBufferCreate(RwUInt32 n, void** out) { L(0xE05, n); if (H(0xE05, n) % 7 == 0) { L(0); return 0; } *out = &g_idxObj; L(1); return 1; }
static RwBool __cdecl FakeCallback(void* object, RxD3D9ResEntryHeader* header, RwBool reinstance) { L(0xE06, id_of(object), id_of(header), reinstance); L(header->serialNumber, header->numMeshes, header->totalNumIndex, header->primType); return H(0xE06, g_hseed) % 5 != 0; }
// puras: del EXE en los dos lados
void D3D9FindMinVertAndNumVerticesCpp(const RwUInt16* indices, RwUInt32 numIndices, RwUInt32* numVertices, RwUInt32* minVert) { reinterpret_cast<void(__cdecl*)(const RwUInt16*, RwUInt32, RwUInt32*, RwUInt32*)>(0x756710)(indices, numIndices, numVertices, minVert); }
void D3D9SortTrianglesCpp(RwUInt16* indices, RwUInt32 numIndices) { reinterpret_cast<void(__cdecl*)(RwUInt16*, RwUInt32)>(0x756770)(indices, numIndices); }
RwUInt32 D3D9TriStripToTriListCpp(RwUInt16* dst, const RwUInt16* src, RwUInt32 numIndices, RwUInt32 offset) { return reinterpret_cast<RwUInt32(__cdecl*)(RwUInt16*, const RwUInt16*, RwUInt32, RwUInt32)>(0x756830)(dst, src, numIndices, offset); }

// qsort del EXE (0x8247E0) necesita el CRT del EXE (sin inicializar en el banco): se desvia a una ordenacion por insercion del banco en los dos lados.
static void __cdecl BenchQsort(void* base, size_t count, size_t size, int(__cdecl* cmp)(const void*, const void*)) {
    L(0xE09, (uint32_t)count, (uint32_t)size);
    uint8_t* b = (uint8_t*)base; uint8_t tmp[64];
    for (size_t i = 1; i < count; i++) { memcpy(tmp, b + i * size, size); size_t j = i; while (j > 0 && cmp(b + (j - 1) * size, tmp) > 0) { memcpy(b + j * size, b + (j - 1) * size, size); j--; } memcpy(b + j * size, tmp, size); }
}

#include "inst_func.inc"

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

struct Args { void* object; void* owner; uint8_t type; RwResEntry** ownerRef; RpMeshHeader* mesh; RxD3D9AllInOneInstanceCallBack cb; RwUInt32 native; };
static uint8_t g_engine[0x400];
static Args setup(uint64_t seed) {
    g_caseRng.seed(seed); g_hseed = (uint32_t)seed * 2654435761u; g_log.clear(); g_arenaUsed = 0x100; memset(g_arena, 0, 0x100); memset(g_idx, 0xEE, sizeof g_idx);
    // motor: funciones de memoria y tabla de tipos de primitiva del plugin de mallas (desplazamiento 0x40; bytes +8..)
    memset(g_engine, 0, sizeof g_engine);
    *(void**)(g_engine + 0x134) = (void*)&EngMalloc; *(void**)(g_engine + 0x138) = (void*)&EngFree;
    G<uint8_t*>(0xC97B24) = g_engine; G<uint32_t>(0xC9B8C0) = 0x40;
    for (int i = 8; i < 0xE0; i++) g_engine[0x40 + i] = (uint8_t)(cr(4) == 0 ? cr(7) : 0);
    g_engine[0x40 + 8] = 3; g_engine[0x40 + 9] = 4; g_engine[0x40 + 0xA] = 5; g_engine[0x40 + 0xC] = 1; g_engine[0x40 + 0x10] = 2; g_engine[0x40 + 0x18] = 6;
    for (auto& s : g_vtblIdx) s = (void*)&Unexpected; g_vtblIdx[11] = (void*)&Idx_Lock; g_vtblIdx[12] = (void*)&Idx_Unlock; g_idxObj = g_vtblIdx;
    Args a{};
    static const uint32_t kinds[] = {0, 1, 2, 4, 8, 0x10, 0, 1, 1, 0, 3, 0x20};
    uint32_t flags = kinds[cr(12)]; if (cr(5) == 0) flags |= 0x100;
    const uint32_t nm = cr(5) == 0 ? 0 : 1 + cr(4);
    auto* mh = (RpMeshHeader*)arena_alloc(0x10 + nm * 12); mh->flags = flags; mh->numMeshes = (uint16_t)nm; mh->serialNum = (uint16_t)g_caseRng(); mh->totalIndicesInMesh = 0; mh->firstMeshOffset = 0;
    RpMesh* meshes = (RpMesh*)(mh + 1); const bool trimCase = cr(3) == 0;
    for (uint32_t i = 0; i < nm; i++) {
        uint32_t cnt = cr(4) == 0 ? cr(3) : cr(32); if (!(flags & 0x100) && cnt < 3) cnt = 3 + cnt; /* 0x756830 con menos de 3 indices daria una vuelta gigante (igual que el EXE real, que nunca la recibe) */ auto* idx = (RwUInt16*)arena_alloc(cnt * 2 + 16); const uint32_t base = cr(3) == 0 ? 0 : cr(100);
        for (uint32_t k = 0; k < cnt; k++) idx[k] = (RwUInt16)(trimCase ? base + (k % 5) : base + cr(40));
        meshes[i].indices = idx; meshes[i].numIndices = cnt; meshes[i].material = (RpMaterial*)(uintptr_t)(0x5000 + cr(8) * 16);
    }
    a.mesh = mh; const uint32_t tsel = cr(8); a.type = tsel < 3 ? 1 : (tsel < 6 ? 0xFF : (uint8_t)cr(4));
    auto* geom = (RpGeometry*)arena_alloc(0x40); geom->numTriangles = (RwInt32)cr(14);
    auto* atom = (RpAtomic*)arena_alloc(0x80); atom->geometry = geom;
    auto* sect = (RpWorldSector*)arena_alloc(0x100); sect->numTriangles = (RwUInt16)cr(14);
    a.object = a.type == 1 ? (void*)atom : (a.type == 0xFF ? (void*)sect : (void*)atom);
    a.owner = arena_alloc(32); a.ownerRef = (RwResEntry**)arena_alloc(16); a.cb = cr(5) == 0 ? nullptr : FakeCallback; a.native = cr(2);
    return a;
}
static std::vector<uint32_t> observe(RwResEntry* ret, const Args& a) {
    std::vector<uint32_t> o; o.push_back(id_of(ret));
    const uint32_t* p = (const uint32_t*)g_arena; for (size_t i = 0; i < g_arenaUsed / 4; i++) o.push_back(p[i]);
    for (size_t i = 0; i < 0x400; i += 4) o.push_back(*(uint32_t*)(g_idx + i));
    o.push_back(id_of(*a.ownerRef));
    o.insert(o.end(), g_log.begin(), g_log.end());
    return o;
}
static LONG WINAPI Crash(EXCEPTION_POINTERS* e) {
    printf("EXCEPCION %08lx en %p (acceso %p)\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, e->ExceptionRecord->NumberParameters > 1 ? (void*)e->ExceptionRecord->ExceptionInformation[1] : nullptr);
    fflush(stdout); ExitProcess(3); return EXCEPTION_EXECUTE_HANDLER;
}
int main(int argc, char** argv) {
    AddVectoredExceptionHandler(1, [](EXCEPTION_POINTERS* e) -> LONG { printf("VEH %08lx en %p (acceso %p) esp=%08lx\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, e->ExceptionRecord->NumberParameters > 1 ? (void*)e->ExceptionRecord->ExceptionInformation[1] : nullptr, e->ContextRecord->Esp); fflush(stdout); return EXCEPTION_CONTINUE_SEARCH; });
    SetUnhandledExceptionFilter(Crash);
    if (argc < 2) { printf("uso: bench_inst.exe <gta_sa.exe> [--casos N] [--semilla S]\n"); return 2; }
    uint64_t n = 100000, seed = 1;
    for (int i = 2; i < argc; i++) { if (!strcmp(argv[i], "--casos") && i + 1 < argc) n = strtoull(argv[++i], 0, 10); else if (!strcmp(argv[i], "--semilla") && i + 1 < argc) seed = strtoull(argv[++i], 0, 10); }
    setvbuf(stdout, nullptr, _IONBF, 0);
    if (!map_exe(argv[1])) return 2;
    divert(0x807ED0, (void*)&RwResourcesAllocateResEntry); divert(0x807DE0, (void*)&RwResourcesFreeResEntry); divert(0x4C9970, (void*)&RwD3D9IndexBufferCreate); divert(0x8247E0, (void*)&BenchQsort);
    typedef RwResEntry*(__cdecl* Fn)(void*, void*, RwUInt8, RwResEntry**, RpMeshHeader*, RxD3D9AllInOneInstanceCallBack, RwUInt32);
    uint64_t fails = 0, ok = 0; std::map<uint32_t, uint64_t> cov, prim; uint64_t trimmed = 0;
    for (uint64_t it = 0; it < n; it++) {
        const uint64_t cs = seed * 1000003 + it;
        Args a = setup(cs);
        if (getenv("BENCH_TRACE")) printf("caso %llu exe nat=%u type=%u cb=%p\n", (unsigned long long)cs, a.native, a.type, (void*)a.cb); if (getenv("BENCH_TRACE")) { printf(" flags=%x nm=%u total=", a.mesh->flags, a.mesh->numMeshes); RpMesh* ms=(RpMesh*)(a.mesh+1); for (unsigned i=0;i<a.mesh->numMeshes;i++) printf("%u ", ms[i].numIndices); printf("\n"); }
        RwResEntry* ra = ((Fn)0x756960)(a.object, a.owner, a.type, a.ownerRef, a.mesh, a.cb, a.native);
        const std::vector<uint32_t> oa = observe(ra, a);
        Args a2 = setup(cs);
        if (getenv("BENCH_TRACE")) printf(" ours\n");
        RwResEntry* rb = D3D9InstanceObject(a2.object, a2.owner, a2.type, a2.ownerRef, a2.mesh, a2.cb, a2.native);
        const std::vector<uint32_t> ob = observe(rb, a2);
        if (ra) { ok++; const auto* hh = (const RxD3D9ResEntryHeader*)(ra + 1); prim[hh->primType]++; if (hh->primType == 4 && hh->totalNumIndex) { const RwUInt8 kb = g_engine[0x40 + 8 + (a.mesh->flags & 0xFF)]; if (kb == 4) trimmed++; } }
        for (uint32_t t : {0xE01u, 0xE02u, 0xE03u, 0xE04u, 0xE05u, 0xE06u, 0xE0Bu, 0xE0Cu}) if (std::find(g_log.begin(), g_log.end(), t) != g_log.end()) cov[t]++;
        if (oa != ob) {
            fails++;
            if (fails <= 3) {
                printf("DIFERENCIA caso=%llu: exe ret=%08x (%zu valores) ours ret=%08x (%zu valores)\n", (unsigned long long)cs, id_of(ra), oa.size(), id_of(rb), ob.size());
                for (size_t i = 0; i < std::min(oa.size(), ob.size()); i++) if (oa[i] != ob[i]) { printf("  primer valor distinto en [%zu]: exe=%08x ours=%08x\n", i, oa[i], ob[i]); break; }
            }
        }
    }
    printf("0x756960: %llu casos (devuelven != 0: %llu), %llu diferencias; cobertura de marcas:", (unsigned long long)n, (unsigned long long)ok, (unsigned long long)fails);
    for (auto& kv : cov) printf(" %x=%llu", kv.first, (unsigned long long)kv.second);
    printf("\ntipos de primitiva (D3DPT) del resultado:"); for (auto& kv : prim) printf(" %u=%llu", kv.first, (unsigned long long)kv.second); printf("; tiras recortadas a lista de triangulos: %llu", (unsigned long long)trimmed);
    printf("\nRESULTADO: %s (%llu diferencias)\n", fails ? "HAY DIFERENCIAS" : "SIN DIFERENCIAS", (unsigned long long)fails);
    return fails ? 1 : 0;
}
