// Banco de ANFITRION (sin juego) del banco CBDNRenderCB30: compila el TEXTO REAL de CustomPipeRenderCB extraido de
// source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp (host_check.py lo extrae a host_function.inc) contra una maqueta minima de los tipos y de los colaboradores de
// RenderWare/D3D (falsos con la MISMA traza que Core.hpp), lo ejecuta con las mismas fixtures (Fixtures.hpp) y vuelca lo observado en el mismo esquema que Core.hpp; host_check.py lo
// compara con fixture_oracle.py. Es una comprobacion de la FUENTE C++ contra el oraculo (que se modela desde el EXE); no sustituye al banco EXE-vs-DLL. Argumento: envOff.
#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <float.h>
#include <string>
#include "Fixtures.hpp"
using namespace CBDNRenderFix;
using uint8 = std::uint8_t; using uint32 = std::uint32_t; using int32 = std::int32_t; using int8 = std::int8_t; using DWORD = std::uint32_t; using RwBool = int; using RwUInt32 = std::uint32_t;

// ---- maqueta de tipos (disposicion identica a la del juego: ver Fixtures.hpp)
struct RwTexture {};
struct RwRGBA { u8 red, green, blue, alpha; };
struct RwSurfaceProperties { float ambient, specular, diffuse; };
struct RpMaterial { RwTexture* texture; RwRGBA color; void* pipeline; RwSurfaceProperties surfaceProps; };
struct RwResEntry { u8 pad[0x18]; };
struct RxD3D9VertexStream { void* vertexBuffer; u32 offset, stride, geometryFlags; };
struct RxD3D9ResEntryHeader { u32 serialNumber, numMeshes; void* indexBuffer; u32 primType; RxD3D9VertexStream vertexStream[2]; RwBool useOffsets; void* vertexDeclaration; u32 totalNumIndex, totalNumVertex; };
struct RxD3D9InstanceData { u32 numIndex, minVert; RpMaterial* material; u32 vertexAlpha; u32 pad[5]; };
struct D3DMATRIX { float _11, _12, _13, _14, _21, _22, _23, _24, _31, _32, _33, _34, _41, _42, _43, _44; };
static_assert(sizeof(RwResEntry) == 0x18 && sizeof(RxD3D9ResEntryHeader) == 0x40 && sizeof(RxD3D9InstanceData) == 0x24 && sizeof(D3DMATRIX) == 64, "disposicion distinta de la del juego");
enum { D3DRS_LIGHTING = 137, D3DRS_TEXTUREFACTOR = 60 };
enum { D3DTSS_COLOROP = 1, D3DTSS_COLORARG1 = 2, D3DTSS_COLORARG2 = 3, D3DTSS_ALPHAOP = 4, D3DTSS_ALPHAARG1 = 5, D3DTSS_ALPHAARG2 = 6, D3DTSS_TEXCOORDINDEX = 11, D3DTSS_TEXTURETRANSFORMFLAGS = 24,
       D3DTSS_COLORARG0 = 26, D3DTSS_TCI_CAMERASPACENORMAL = 0x10000 };
enum { D3DTOP_DISABLE = 1, D3DTOP_SELECTARG2 = 3, D3DTOP_MULTIPLYADD = 25 };
enum { D3DTA_DIFFUSE = 0, D3DTA_CURRENT = 1, D3DTA_TEXTURE = 2, D3DTA_TFACTOR = 3 };
enum { D3DTTFF_DISABLE = 0 };
enum { rxGEOMETRY_PRELIT = 8 };
enum { rwRENDERSTATETEXTUREADDRESS = 2, rwTEXTUREADDRESSWRAP = 1 };
#define RWRSTATE(a) (reinterpret_cast<void*>(a))

static int32 envPluginOffset;
struct CCustomCarEnvMapPipeline { static inline int32& ms_envMapPluginOffset = envPluginOffset; };
alignas(16) static D3DMATRIX g_matrix;
template <class T> T& StaticRef(std::uintptr_t a) {
    static float f255, f254;
    if (a == 0x859A3C) { f255 = std::bit_cast<float>(0x3B808081u); return reinterpret_cast<T&>(f255); }
    if (a == 0x86BE90) { f254 = 254.0f; return reinterpret_cast<T&>(f254); }
    if (a == 0xC02C28) return reinterpret_cast<T&>(g_matrix);
    std::abort();
}
static D3DMATRIX& DNEnvMapTexMatrix = g_matrix;

alignas(16) static std::array<u8, kResArena> resArena;
alignas(16) static std::array<std::array<u8, kMatArena>, kNumMats> matArenas;
alignas(16) static std::array<std::array<u8, kEnvArena>, kNumEnvs> envArenas;
alignas(16) static std::array<u8, kObjArena> objArena;
static constexpr u32 kObjRaw = 0x0BADF00Du;
static u32 P(const void* p) { return u32(std::uintptr_t(p)); }
static u8* Res() { return resArena.data() + kResAt; }
static u8* Mat(u32 i) { return matArenas[i].data() + kMatAt; }
static u8* EnvBlk(u32 j) { return envArenas[j].data() + kEnvAt; }
static u8* Obj() { return objArena.data() + kObjAt; }
static u32 Dw(const u8* p) { u32 v; std::memcpy(&v, p, 4); return v; }
static void SetDw(u8* p, u32 v) { std::memcpy(p, &v, 4); }
static u32 curLighting;
static std::string trace;
static void Push(u32 k, u32 a = 0, u32 b = 0, u32 c = 0, u32 d = 0) { char t[128]; std::snprintf(t, sizeof t, "%s[%u,%u,%u,%u,%u]", trace.empty() ? "" : ",", k, a, b, c, d); trace += t; }
static u32 MeshIndex(const void* mesh) { return (P(mesh) - (P(Res()) + kEntryHdr + kHeaderSize)) / kMeshSize; }
static u32 ObjLabel(const void* p) { const u32 v = P(p), base = P(Obj()); return v >= base && v < base + kObjSize ? v - base : v; }

// ---- colaboradores (los mismos falsos que Core.hpp, sobre la maqueta)
static void _rwD3D9EnableClippingIfNeeded(void* object, RwUInt32 type) { Push(1, ObjLabel(object), type); }
static void RwD3D9GetRenderState(RwUInt32 state, void* value) { Push(2, state); *static_cast<u32*>(value) = curLighting; }
static void RwD3D9SetTexture(RwTexture* tex, RwUInt32 stage) { Push(3, P(tex), stage); }
static void RwD3D9SetRenderState(RwUInt32 state, RwUInt32 value) { Push(4, state, value); }
static void RwD3D9SetTextureStageState(RwUInt32 stage, RwUInt32 type, RwUInt32 value) { Push(5, stage, type, value); }
static void _rwD3D9SetStreams(const RxD3D9VertexStream* s, RwBool useOffsets) { Push(7, P(s) - (P(Res()) + kEntryHdr), u32(useOffsets)); }
static void RwRenderStateSet(int state, void* value) { Push(9, u32(state), P(value)); }
static void RwD3D9SetSurfaceProperties(const RwSurfaceProperties* surf, const RwRGBA* color, RwUInt32 flags) {
    u32 mi = 0xFFFFFFFFu, so = 0, co = 0;
    for (u32 i = 0; i < kNumMats; ++i) { const u32 b = P(Mat(i)); if (P(surf) >= b && P(surf) < b + kMatWin) { mi = i; so = P(surf) - b; co = P(color) - b; } }
    Push(11, mi, so | (co << 16), flags);
}
static void DNSetIndices(void* ib) { Push(6, P(ib)); }
static void DNSetVertexDeclaration(void* d) { Push(8, P(d)); }
static void DNVertexAlphaEnable(RwBool enable) { Push(10, u32(enable)); }
static void DNInstanceDataRender(RxD3D9ResEntryHeader* h, RxD3D9InstanceData* m) { Push(12, P(h) - P(Res()), MeshIndex(m)); }
static void DNInstanceDataRenderLighting(RxD3D9ResEntryHeader* h, RxD3D9InstanceData* m, uint32 flags, RwTexture* tex) { Push(13, P(h) - P(Res()), MeshIndex(m), flags, P(tex)); }

struct CCustomBuildingDNPipeline { static void CustomPipeRenderCB(RwResEntry* entry, void* object, uint8 type, uint32 flags); };
#include "host_function.inc"

static bool GuardOk(const u8* a, std::size_t size, std::size_t at, std::size_t len) { for (std::size_t i = 0; i < size; ++i) { if (i >= at && i < at + len) continue; if (a[i] != u8(kGuardByte)) return false; } return true; }
static u32 SlotPtr(u32 label) { return label == kEnvSlotNull ? 0u : (label >= kEnvSlotBase && label < kEnvSlotBase + kNumEnvs ? P(EnvBlk(label - kEnvSlotBase)) : label); }
static void Build(const Fx& f, u32 envOff) {
    resArena.fill(u8(kGuardByte)); objArena.fill(u8(kGuardByte));
    for (auto& a : matArenas) a.fill(u8(kGuardByte));
    for (auto& a : envArenas) a.fill(u8(kGuardByte));
    std::memcpy(Res(), f.res.data(), kResSize);
    for (u32 m = 0; m < kMaxMeshes; ++m) SetDw(Res() + kEntryHdr + kHeaderSize + m * kMeshSize + 8, P(Mat(f.meshMat[m])));
    for (u32 i = 0; i < kNumMats; ++i) { const auto img = MatImage(f, i, envOff); std::memcpy(Mat(i), img.data(), kMatWin); SetDw(Mat(i) + envOff, SlotPtr(f.matSlot[i])); }
    for (u32 j = 0; j < kNumEnvs; ++j) std::memcpy(EnvBlk(j), f.env[j].data(), kEnvSize);
    std::memset(Obj(), 0x3C, kObjSize);
    std::memcpy(&g_matrix, f.mtx.data(), kMtxSize);
}

int main(int argc, char** argv) {
    const u32 envOff = argc >= 2 ? u32(std::strtoul(argv[1], nullptr, 0)) : 0x28;
    envPluginOffset = int32(envOff);
    _controlfp(_PC_24 | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM);
    for (unsigned n = 0; n < kCases; ++n) {
        const Fx f = Make(n);
        const bool fits = f.numMeshes <= kMaxMeshes || f.numMeshes >= 0x80000000u;
        if (!fits) { std::printf("{}\n"); continue; }
        Build(f, envOff);
        const auto resRef = resArena; const auto matRef = matArenas; const auto envRef = envArenas; const auto objRef = objArena;
        trace.clear(); curLighting = f.lighting;
        void* const object = f.objKind == 0 ? static_cast<void*>(Obj() + kObjProbeOffset) : (f.objKind == 1 ? nullptr : reinterpret_cast<void*>(std::uintptr_t(kObjRaw)));
        CCustomBuildingDNPipeline::CustomPipeRenderCB(reinterpret_cast<RwResEntry*>(Res()), object, uint8(f.typeArg), f.flags);
        std::printf("{\"trace\":[%s],\"matrix\":\"%s\",\"res_ok\":%s,\"mats_ok\":%s,\"envs_ok\":%s,\"obj_ok\":%s}\n", trace.c_str(), Hex(reinterpret_cast<const u8*>(&g_matrix), kMtxSize).c_str(),
                    resArena == resRef ? "true" : "false", matArenas == matRef ? "true" : "false", envArenas == envRef ? "true" : "false", objArena == objRef ? "true" : "false");
    }
}
