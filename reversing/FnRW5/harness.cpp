// Arnes de verificacion por pares del lote RW5 (22/11/2026): el codigo ORIGINAL de gta-sa.exe se mapea en este mismo proceso x86 (direcciones originales,
// 0x400000..) y se ejecuta de forma nativa; las llamadas del original al motor (reserva de memoria, flujos, registros de plugins, errores) pasan por
// copias parcheadas (`jmp`) a las MISMAS funciones simuladas que usa el C++ nuevo (extraido de las fuentes del proyecto: core_*.inc). Cada prueba ejecuta
// original y C++ con entradas identicas y compara retorno, registro de llamadas (con los bytes escritos) y memoria.
// Uso: harness.exe <matrix|strip|sector|clump|tex> [pruebas] [semilla]       (variable GTASA_EXE = ruta de gta-sa.exe)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <float.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <cstdarg>
#include <string>
#include <vector>
#include <random>

typedef int32_t RwInt32; typedef uint32_t RwUInt32; typedef uint16_t RwUInt16; typedef int16_t RwInt16; typedef uint8_t RwUInt8; typedef uint8_t uint8; typedef uint32_t uint32;
typedef float RwReal; typedef int32_t RwBool; typedef char RwChar; typedef uint16_t RxVertexIndex;
#define TRUE 1
struct RwV3d { float x, y, z; };
struct RwBBox { RwV3d sup, inf; };
struct RwLLLink { RwLLLink* next; RwLLLink* prev; };
struct RwLinkList { RwLLLink link; };
struct RwMatrix { RwV3d right; RwUInt32 flags; RwV3d up; RwUInt32 pad1; RwV3d at; RwUInt32 pad2; RwV3d pos; RwUInt32 pad3; };
struct RwStream; struct RwFrame; struct RpMaterial; struct RpGeometry; struct RwFreeList; struct RwResEntry; struct RpMeshHeader; struct RpWorldSector;
#pragma section(".aaa", read)
#pragma comment(linker, "/SECTION:.aaa,RWE")
#pragma comment(linker, "/INCLUDE:_gtaimg")
extern "C" __declspec(allocate(".aaa")) unsigned char gtaimg[0xE00000] = { 1 };   // = 0x401000..0x1201000
struct RwGlobals {};
template<class T> T& StaticRef(uintptr_t a) { return *reinterpret_cast<T*>(a); }
static RwGlobals* const& RwEngineInstance = *reinterpret_cast<RwGlobals**>(0xC97B24);
static RwUInt32& s_meshSerialOffset = *reinterpret_cast<RwUInt32*>(0xC9B8C0);

struct RpTriangle { RwUInt16 vertIndex[3]; RwUInt16 matIndex; };
struct RpVertexNormal { RwUInt8 x, y, z, pad; };
struct RwTexCoords { float u, v; };
struct RwRGBA { RwUInt8 red, green, blue, alpha; };
struct RpSector { RwInt32 type; };
struct RpWorldSector {
    RwInt32 type; RpTriangle* triangles; RwV3d* vertices; RpVertexNormal* normals; RwTexCoords* texCoords[8]; RwRGBA* preLitLum; RwResEntry* repEntry;
    RwLinkList collAtomicsInWorldSector; RwLinkList lightsInWorldSector; RwBBox boundingBox; RwBBox tightBoundingBox; void* mesh; void* pipeline;
    RwUInt16 matListWindowBase; RwUInt16 numVertices; RwUInt16 numTriangles; RwUInt16 pad;
};
struct RpPlaneSector { RwInt32 type; float value; RpSector* leftSubTree; RpSector* rightSubTree; float leftValue; float rightValue; };
struct RpWorld { struct { RwUInt8 type, subType, flags, privateFlags; void* parent; } object; RwUInt32 flags; RwUInt8 pad1[0x14]; RwInt32 numTexCoordSets; };
static_assert(offsetof(RpWorld, numTexCoordSets) == 0x20);
struct RwPluginRegistry { RwInt32 sizeOfStruct; RwInt32 pad[7]; };
struct RwError { RwInt32 pluginID; RwInt32 errorCode; };
struct RwObject { RwUInt8 type, subType, flags, privateFlags; void* parent; };
struct RwObjectHasFrame { RwObject object; RwLLLink lFrame; void* sync; };
struct RpAtomic { RwObjectHasFrame object; void* repEntry; RpGeometry* geometry; RwUInt8 pad[0x40 - 0x1C]; RwLLLink inClumpLink; };
struct RpClump { RwObject object; RwLinkList atomicList; };
struct rwFrameList { RwFrame** frames; RwInt32 numFrames; };
struct RpGeometryList { RpGeometry** geometries; RwInt32 numGeoms; };
struct ClumpWriteContext { RwStream* stream; rwFrameList frameList; RpGeometryList geomList; RwInt32 ok; };
static_assert(sizeof(ClumpWriteContext) == 0x18);
struct RpBuildMeshTriangle { RwUInt16 vertIndex[3]; RpMaterial* material; RwUInt16 matIndex, textureIndex, rasterIndex, pipelineIndex; };
static_assert(sizeof(RpBuildMeshTriangle) == 0x14 && offsetof(RpBuildMeshTriangle, material) == 8);
struct RpBuildMesh { RwUInt32 triangleBufferSize; RwUInt32 numTriangles; RpBuildMeshTriangle* meshTriangles; };
struct RpMesh { RxVertexIndex* indices; RwUInt32 numIndices; RpMaterial* material; };
struct RpMeshHeader { RwUInt32 flags; RwUInt16 numMeshes; RwUInt16 serialNum; RwUInt32 totalIndicesInMesh; RwUInt32 firstMeshOffset; };
struct RpBuildStripEdge;
struct RpBuildStripNode { RwUInt32 index; RpBuildStripEdge* edges[3]; RpBuildStripNode* next; RpBuildStripNode* prev; RwUInt32 used; RwUInt32 trialUsed; RwUInt8 numOpenNeighbours; };
struct RpBuildStripEdge { RwUInt16 vertA; RwUInt16 vertB; RpBuildStripNode* tri1; RpBuildStripNode* tri2; RpBuildStripEdge* next; };
struct RpBuildStripList { RwUInt16* indices; RwUInt32 numIndices; RwUInt32 capacity; RpBuildStripList* next; };
struct RwTexture { void* raster; struct RwTexDictionary* dict; RwLLLink lInDictionary; char name[32]; char mask[32]; RwUInt32 filterAddressing; RwInt32 refCount; };
struct RwTexDictionary { RwObject object; RwLinkList texturesInDict; RwLLLink lInInstance; };

constexpr uintptr_t kSectorPluginRegistry = 0x8D736C;
constexpr uintptr_t kAtomicPluginRegistry = 0x8D624C;
constexpr RwUInt32 kStripListHint = 0x10502, kStripNodeHint = 0x30502, kStripEntriesHint = 0x1010502;

// ------------------------------------------------------------------------------------------------ memoria y registro
static uint8_t* const ARENA = reinterpret_cast<uint8_t*>(0x60000000);
constexpr size_t ARENA_SIZE = 32u << 20;
static uint8_t* const ENG = reinterpret_cast<uint8_t*>(0x50000000);
static size_t g_bump;
static std::string LOG;
static int g_failAt, g_failCount, g_callNo;
static const unsigned char* g_in; static size_t g_inLen, g_inPos;
static bool g_errArgMask;
static int g_garbFrame, g_garbGeom;
static int g_mask0, g_mask1; // bytes [mask0, mask1) del siguiente RwStreamWrite se registran como cero (basura de pila del original)

static void L(const char* f, ...) {
    char b[512]; va_list a; va_start(a, f); vsnprintf(b, sizeof b, f, a); va_end(a); LOG += b; LOG += '\n';
}
static bool failNow() { ++g_failCount; return g_failAt != 0 && g_failCount == g_failAt; }
static size_t g_prevBump;
static void* bump(size_t n) { g_bump = (g_bump + 15) & ~size_t(15); void* p = ARENA + g_bump + (16u << 20); g_bump += n; g_prevBump = g_bump; return p; } // zona alta de la arena para reservas

static void* __cdecl M_Malloc(RwUInt32 size, RwUInt32 hint) { void* p = bump(size ? size : 1); L("malloc %x h=%x -> %p", size, hint, p); return p; }
static void  __cdecl M_Free(void* p) { L("free %p", p); }
struct FL { RwUInt32 entry; };
static RwFreeList* __cdecl M_FLCreate(RwInt32 entry, RwInt32 per, RwInt32 align, RwInt32 hint) { FL* f = static_cast<FL*>(bump(16)); f->entry = entry; L("flcreate %x %x %x %x -> %p", entry, per, align, hint, f); return reinterpret_cast<RwFreeList*>(f); }
static void  __cdecl M_FLDestroy(RwFreeList* l) { L("fldestroy %p", l); }
static void* __cdecl M_FLAlloc(RwFreeList* l, RwUInt32 hint) { void* p = bump(reinterpret_cast<FL*>(l)->entry); L("flalloc %p h=%x -> %p", l, hint, p); return p; }
static void  __cdecl M_FLFree(RwFreeList* l, void* p) { L("flfree %p %p", l, p); }
#define RwMalloc M_Malloc
#define RwFree M_Free
#define RwFreeListCreate M_FLCreate
#define RwFreeListDestroy M_FLDestroy
#define RwFreeListAlloc M_FLAlloc
#define RwFreeListFree M_FLFree

static void hexlog(const char* tag, const void* p, size_t n) {
    std::string s = tag; char b[4]; const uint8_t* q = static_cast<const uint8_t*>(p);
    for (size_t i = 0; i < n; ++i) { unsigned v = q[i]; if ((int(i) >= g_mask0 && int(i) < g_mask1) || (n == 0x2C && i >= 0x24 && i < 0x28) || (n == 0x10 && ((g_garbFrame && i < 4) || (g_garbGeom && i >= 4 && i < 8)))) v = 0; snprintf(b, sizeof b, "%02x", v); s += b; }
    LOG += s; LOG += '\n';
}
// flujos
static RwStream* const STREAM = reinterpret_cast<RwStream*>(0x70000000);
static RwStream* __cdecl M_WriteHeader(RwStream* s, RwUInt32 type, RwUInt32 size, RwUInt32 ver, RwUInt32 build) { L("hdr %x %x %x %x", type, size, ver, build); return failNow() ? nullptr : s; }
static RwStream* __cdecl M_Write(RwStream* s, const void* p, RwUInt32 n) { hexlog("w ", p, n); g_mask0 = g_mask1 = 0; return failNow() ? nullptr : s; }
static RwStream* __cdecl M_WriteReal(RwStream* s, const RwReal* p, RwUInt32 n) { hexlog("wr ", p, n); return failNow() ? nullptr : s; }
static RwStream* __cdecl M_WriteInt16(RwStream* s, const RwInt16* p, RwUInt32 n) { hexlog("w16 ", p, n); return failNow() ? nullptr : s; }
static RwUInt32 __cdecl M_Read(RwStream* s, void* d, RwUInt32 n) {
    if (failNow()) { L("read %x -> fail", n); return 0; }
    RwUInt32 k = (g_inPos + n <= g_inLen) ? n : RwUInt32(g_inLen - g_inPos); memcpy(d, g_in + g_inPos, k); g_inPos += k; L("read %x -> %x", n, k); return k;
}
static RwStream* __cdecl M_ReadReal(RwStream* s, void* d, RwUInt32 n) { RwUInt32 k = M_Read(s, d, n); return k == n ? s : nullptr; }
static RwBool __cdecl M_FindChunk(RwStream* s, RwUInt32 type, RwUInt32* len, RwUInt32* ver) {
    while (g_inPos + 12 <= g_inLen) {
        RwUInt32 t, sz, id; memcpy(&t, g_in + g_inPos, 4); memcpy(&sz, g_in + g_inPos + 4, 4); memcpy(&id, g_in + g_inPos + 8, 4); g_inPos += 12;
        if (t == type) { if (len) *len = sz; if (ver) *ver = id; L("find %x -> len %x ver %x", type, sz, id); return failNow() ? 0 : 1; }
        g_inPos += sz;
    }
    L("find %x -> none", type); return 0;
}
static RwInt32 __cdecl M_RegSize(const RwPluginRegistry* r, const void* o) { RwInt32 v = (RwInt32)(((uintptr_t)o >> 3) % 0x40); L("regsize %p -> %x", o, v); return v; }
static const RwPluginRegistry* __cdecl M_RegWrite(const RwPluginRegistry* r, RwStream* s, const void* o) { L("regwrite %p %p", r, o); return failNow() ? nullptr : r; }
static const RwPluginRegistry* __cdecl M_RegRead(const RwPluginRegistry* r, RwStream* s, void* o) {
    L("regread %p %p", r, o); StaticRef<RwUInt32>(0xC94C1C) = (g_callNo & 1) ? 0x1234 : 0; StaticRef<RwUInt32>(0xC94C10) = 0x55; return failNow() ? nullptr : r;
}
static void __cdecl M_RegInit(const RwPluginRegistry* r, void* o) { L("reginit %p %p", r, o); }
static void __cdecl M_RegRights(const RwPluginRegistry* r, RwUInt32 a, void* o, RwUInt32 b) { L("regrights %x %p %x", a, o, b); }
static RwInt32 __cdecl M_rwerror(RwInt32 code, ...) { va_list ap; va_start(ap, code); RwUInt32 a1 = va_arg(ap, RwUInt32); va_end(ap); L("rwerror %x a1=%x", code, (g_errArgMask && code == (RwInt32)0x80000013) ? 0 : (code == (RwInt32)0x80000004 ? 0 : a1)); return code ^ 0x1000; }
static void* __cdecl M_ErrorSet(RwError* e) { L("errorset %d %x", e->pluginID, e->errorCode); return nullptr; }
static void __cdecl M_Destroy1(void* s) { L("deinstance %p", s); }
static void __cdecl M_Destroy2(void* s) { L("destroyrecurse %p", s); }
static RwUInt32 __cdecl M_GeomSize(const RpGeometry* g) { RwUInt32 v = (RwUInt32)(((uintptr_t)g >> 4) % 0x100); L("geomsize %p -> %x", g, v); return v; }
static const RpGeometry* __cdecl M_GeomWrite(const RpGeometry* g, RwStream* s) { L("geomwrite %p", g); return failNow() ? nullptr : g; }
static RpGeometry* __cdecl M_GeomAddRef(RpGeometry* g) { L("addref %p", g); return g; }
static RwBool __cdecl M_FrameFind(const rwFrameList* fl, const RwFrame* f, RwInt32* idx) {
    for (RwInt32 i = 0; i < fl->numFrames; ++i) if (fl->frames[i] == f) { *idx = i; L("framefind %p -> %d", f, i); return TRUE; }
    L("framefind %p -> none", f); return 0;
}
static float __cdecl M_InvSqrt(float x) { if (x == 0.0f) return x; return (float)(1.0 / std::sqrt((double)x)); }
// nombres que usa el codigo del proyecto
#define _rwStreamWriteVersionedChunkHeader M_WriteHeader
#define RwStreamWrite M_Write
#define RwStreamWriteReal M_WriteReal
#define RwStreamWriteInt16 M_WriteInt16
#define RwStreamRead M_Read
#define RwStreamReadReal(s, d, n) M_ReadReal(s, d, n)
#define RwStreamReadInt16(s, d, n) M_ReadReal(s, d, n)
#define RwStreamFindChunk M_FindChunk
#define _rwPluginRegistryGetSize M_RegSize
#define _rwPluginRegistryWriteDataChunks M_RegWrite
#define _rwPluginRegistryReadDataChunks M_RegRead
#define _rwPluginRegistryInitObject M_RegInit
#define _rwPluginRegistryInvokeRights M_RegRights
#define _rwerror M_rwerror
#define RwErrorSet M_ErrorSet
#define _rpWorldSectorDeinstanceAll M_Destroy1
#define _rpWorldSectorDestroyRecurse M_Destroy2
#define RpGeometryStreamGetSize M_GeomSize
#define RpGeometryStreamWrite M_GeomWrite
#define RpGeometryAddRef M_GeomAddRef
#define _rwFrameListFindFrame M_FrameFind
#define _rwInvSqrt M_InvSqrt
static RpMeshHeader* _rpMeshHeaderCreate(RwUInt32 size) { return static_cast<RpMeshHeader*>(M_Malloc(size, 0x30502)); }

// ------------------------------------------------------------------------------------------------ original mapeado
static void mapExe() {
    const char* path = getenv("GTASA_EXE"); if (!path) path = "D:\\Grand Theft Auto San Andreas\\gta-sa.exe";
    FILE* f = fopen(path, "rb"); if (!f) { printf("no se abre %s\n", path); exit(2); }
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET); std::vector<uint8_t> d(n); fread(d.data(), 1, n, f); fclose(f);
    uint32_t pe = *(uint32_t*)&d[0x3c]; uint16_t ns = *(uint16_t*)&d[pe + 6], osz = *(uint16_t*)&d[pe + 20]; uint32_t base = *(uint32_t*)&d[pe + 52];
    DWORD oldProt; if (!VirtualProtect((void*)0x401000, 0xE00000, PAGE_EXECUTE_READWRITE, &oldProt)) { printf("VirtualProtect fallo %lu\n", GetLastError()); exit(2); }
    uint8_t* m = (uint8_t*)base;   // el propio arnes reserva 0x401000..0xD00000 como seccion de su imagen (base 0x400000), ver gtaimg
    for (int i = 0; i < ns; ++i) {
        uint32_t o = pe + 24 + osz + 40 * i, vs = *(uint32_t*)&d[o + 8], va = *(uint32_t*)&d[o + 12], rs = *(uint32_t*)&d[o + 16], rp = *(uint32_t*)&d[o + 20];
        (void)vs;
        // los primeros 0x180 bytes de la seccion de codigo del EXE (0x401000..0x40117F) los usa el propio arnes (datos del enlazador): no se copian
        uint32_t skip = (va == 0x1000) ? 0x180 : 0;
        if (rs > skip) memcpy(m + va + skip, &d[rp + skip], rs - skip);
    }
    if (!VirtualAlloc(ENG, 0x10000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE) || !VirtualAlloc(ARENA, ARENA_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)
        || !VirtualAlloc(STREAM, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)) { printf("sin memoria fija\n"); exit(2); }
}
static void patch(uintptr_t at, void* to) { uint8_t* p = (uint8_t*)at; p[0] = 0xE9; *(int32_t*)(p + 1) = (int32_t)((uintptr_t)to - at - 5); }
static void installMocks() {
    *(uint8_t**)0xC97B24 = ENG;
    *(void**)(ENG + 0x134) = (void*)M_Malloc; *(void**)(ENG + 0x138) = (void*)M_Free; *(void**)(ENG + 0x144) = (void*)M_FLAlloc; *(void**)(ENG + 0x148) = (void*)M_FLFree;
    s_meshSerialOffset = 0x200;
    patch(0x801980, (void*)M_FLCreate); patch(0x801B80, (void*)M_FLDestroy);
    patch(0x7ED270, (void*)M_WriteHeader); patch(0x7ECB30, (void*)M_Write); patch(0x7ED3D0, (void*)M_WriteReal); patch(0x7ED480, (void*)M_WriteInt16);
    patch(0x7EC9D0, (void*)M_Read); patch(0x7ED4F0, (void*)M_ReadReal); patch(0x7ED4A0, (void*)M_ReadReal); patch(0x7ED2D0, (void*)M_FindChunk);
    patch(0x808B00, (void*)M_RegSize); patch(0x808B40, (void*)M_RegWrite); patch(0x808980, (void*)M_RegRead); patch(0x8086E0, (void*)M_RegInit); patch(0x808AB0, (void*)M_RegRights);
    patch(0x8088D0, (void*)M_rwerror); patch(0x808820, (void*)M_ErrorSet); patch(0x74ECA0, (void*)M_Destroy1); patch(0x74ED50, (void*)M_Destroy2);
    patch(0x74CE20, (void*)M_GeomSize); patch(0x74CED0, (void*)M_GeomWrite); patch(0x74CCB0, (void*)M_GeomAddRef); patch(0x8076E0, (void*)M_FrameFind);
    patch(0x7EDB90, (void*)M_InvSqrt); patch(0x758920, (void*)_rpMeshHeaderCreate);
}
static void resetRun(int failAt) {
    memset(ARENA, 0, 0x100000); memset(ARENA + (16u << 20), 0, g_prevBump + 64); g_prevBump = 0; g_bump = 0; LOG.clear(); g_failAt = failAt; g_failCount = 0; g_inPos = 0; g_mask0 = g_mask1 = 0; g_callNo = 0;
    StaticRef<RwUInt32>(0xC94C1C) = 0; StaticRef<RwUInt32>(0xC94C10) = 0;
    *(RwUInt32*)(ENG + 0x200) = 0; *(RwUInt32*)(ENG + 0x204) = 0;   // numero de serie de mallas y lista libre de tiras
}

// ------------------------------------------------------------------------------------------------ codigo del proyecto
inline RpAtomic* ClumpAtomicOfLink(RwLLLink* node) { return reinterpret_cast<RpAtomic*>(reinterpret_cast<RwUInt8*>(node) - 0x40); }
static RwUInt32 RpWorldSectorStreamDataSize(void* s, RpWorld* w, RwUInt32 f) { return ((RwUInt32(__cdecl*)(void*, RpWorld*, RwUInt32))0x762440)(s, w, f); }
static RwUInt32 RpWorldPlaneStreamSize(void* s, RpWorld* w, RwUInt32 f) { return ((RwUInt32(__cdecl*)(void*, RpWorld*, RwUInt32))0x762040)(s, w, f); }
#include "core_misc.inc"
#include "core_matrix.inc"
#include "core_sector.inc"
#include "core_clump.inc"
#include "core_tex.inc"
// ayudantes del strip: se usan los ORIGINALES (se prueba solo la funcion nueva)
static RpBuildStripNode** _rpBuildStripNodesCreate(RwUInt32 n, RwFreeList** fl, RpBuildStripEdge** eh, RpBuildMeshTriangle* t) { return ((RpBuildStripNode** (__cdecl*)(RwUInt32, RwFreeList**, RpBuildStripEdge**, RpBuildMeshTriangle*))0x75A090)(n, fl, eh, t); }
static void _rpBuildStripNodeConsume(RpBuildStripNode* n, RpBuildStripNode** h, RwInt32 p) { ((void(__cdecl*)(RpBuildStripNode*, RpBuildStripNode**, RwInt32))0x75A250)(n, h, p); }
static RwUInt32 _rpBuildStripExtend(RpBuildStripList* s, RpBuildStripEdge* e, RpBuildStripNode** h, RpBuildMeshTriangle* t, RwInt32 p) { return ((RwUInt32(__cdecl*)(RpBuildStripList*, RpBuildStripEdge*, RpBuildStripNode**, RpBuildMeshTriangle*, RwInt32))0x75A370)(s, e, h, t, p); }
static RwBool _rpBuildStripListJoin(RpBuildStripList** l, RwInt32 m) { return ((RwBool(__cdecl*)(RpBuildStripList**, RwInt32))0x75A660)(l, m); }
static int __cdecl BuildMeshTriangleCompare(const void* a, const void* b) { return ((int(__cdecl*)(const void*, const void*))0x759640)(a, b); }
inline RwFreeList*& MeshStripFreeListSlot() { return *reinterpret_cast<RwFreeList**>(reinterpret_cast<uintptr_t>(static_cast<RwGlobals*>(RwEngineInstance)) + static_cast<uintptr_t>(s_meshSerialOffset) + 4); }
static RwUInt32 s_dummy;
#include "core_strip.inc"

// ------------------------------------------------------------------------------------------------ pruebas
static std::mt19937 rng;
static uint32_t R(uint32_t n) { return n ? rng() % n : 0; }
static int g_bad, g_total;
static void compare(const char* name, const std::string& la, const std::string& lb, uintptr_t ra, uintptr_t rb, const uint8_t* ma, const uint8_t* mb, size_t mn) {
    ++g_total; bool ok = la == lb && ra == rb && memcmp(ma, mb, mn) == 0;
    if (ok) return;
    ++g_bad;
    if (g_bad <= 3) {
        printf("DIFERENCIA en %s (prueba %d): ret orig=%p cpp=%p mem=%s\n", name, g_total, (void*)ra, (void*)rb, memcmp(ma, mb, mn) == 0 ? "igual" : "distinta");
        size_t i = 0; while (i < la.size() && i < lb.size() && la[i] == lb[i]) ++i;
        size_t s = la.rfind('\n', i); s = (s == std::string::npos) ? 0 : s + 1;
        if (getenv("VERBOSO")) printf("=== LOG ORIG\n%s=== LOG CPP\n%s===\n", la.c_str(), lb.c_str());
        printf("--- orig desde la linea distinta:\n%.400s\n--- cpp:\n%.400s\n", la.c_str() + s, lb.c_str() + (s < lb.size() ? s : 0));
        for (size_t k = 0; k < mn; ++k) if (ma[k] != mb[k]) { printf("primer byte de memoria distinto en +%zx: orig=%02x cpp=%02x\n", k, ma[k], mb[k]); break; }
    }
}

// Con entradas NaN el signo/carga del NaN resultante no se reproduce (x87 propaga el operando, SSE devuelve el indefinido): se tratan como iguales.
static bool sameOrBothNaN(const RwV3d& a, const RwV3d& b) {
    const float* p = &a.x; const float* q = &b.x;
    for (int i = 0; i < 3; ++i) { if (std::isnan(p[i]) && std::isnan(q[i])) continue; if (memcmp(&p[i], &q[i], 4) != 0) return false; }
    return true;
}
static void testMatrix(int n) {
    unsigned cw; _controlfp_s(&cw, _PC_24, _MCW_PC);
    float pool[] = { 0.f, 1.f, -1.f, 0.5f, -0.5f, 0.999999f, -0.999999f, 2.f, 1e-20f, 1e20f, 3.4e38f, -3.4e38f, std::nanf(""), INFINITY, -INFINITY, 0.1f, -0.1f, 0.3333333f };
    for (int t = 0; t < n; ++t) {
        float m[16]; for (int i = 0; i < 16; ++i) m[i] = (R(3) == 0) ? pool[R(sizeof pool / sizeof *pool)] : (float)((int)R(2001) - 1000) / (R(2) ? 1000.f : 7.f);
        if (R(4) == 0) m[5] = m[0];
        if (R(4) == 0) m[10] = m[5];
        if (R(8) == 0) m[10] = m[0];
        RwV3d a = { 7, 8, 9 }, b = { 7, 8, 9 }; resetRun(0);
        const void* ra = ((const void*(__cdecl*)(const void*, RwV3d*))0x7F2930)(m, &a);
        const void* rb = _rwMatrixQueryRotateHalfTurnAxis(reinterpret_cast<const RwMatrix*>(m), &b);
        ++g_total; if (ra != (void*)m || rb != (void*)m || !sameOrBothNaN(a, b)) { ++g_bad; if (g_bad <= 5) printf("DIFERENCIA matriz: orig=(%a,%a,%a) cpp=(%a,%a,%a) m0=%g m5=%g m10=%g\n", a.x, a.y, a.z, b.x, b.y, b.z, m[0], m[5], m[10]); }
    }
}

#include "harness2.inc"
