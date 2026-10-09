#pragma once
// Banco dentro del motor (estilo GfxWave2/Small2/PadMouse1, enganchado en CGame::Process): ShadowsWave3.
// Clase CShadows: las dos funciones que dejaban la clase INCOMPLETA tras GfxWave2:
//   RenderStoredShadows (0x70A960) y CastPlayerShadowSectorList<CPtrListSingleLink<CPhysical*>> (0x70A470).
// Original (hook desactivado = codigo del EXE) frente a candidato (hook activo = DLL) sobre OBJETOS FALSOS; el dibujo de RenderWare
// (RwRenderStateSet por el puntero del motor, RwIm3D*) y los colaboradores de almacenamiento/agua se doblan con trazas y argumentos crudos.
// Los globales reales tocados se guardan al empezar, se restauran tras cada ejecucion y se verifican al final (state_restored).
// Hilos ajenos: ver Mine() en SwCommon.hpp.
#include <fstream>
#include <float.h>
#include "SwCommon.hpp"
#include "Shadows.h"
#include "WaterLevel.h"
#include "Physical.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "Models/ModelInfo.h"
#include "Models/BaseModelInfo.h"
#include "Collision/ColModel.h"
#include "Pools/PtrNodeSingleLinkPool.h"

namespace ShadowsWave3Live {
using std::bit_cast;

// Tabla: {clase, nombre, direccion EXE, vt, slot, casos}   (run_local.py la extrae de aqui)
//   {"CShadows","RenderStoredShadows",0x70A960,0,-1,512},
//   {"CShadows","CastPlayerShadowSectorList<CPtrListSingleLink<CPhysical*>>",0x70A470,0,-1,1024},

constexpr uintptr_t MAIN = 0xC403A8, MAIN_END = 0xC4B6B0, MAIN_SIZE = MAIN_END - MAIN;   // g_ShadowVertices .. aPermanentShadows (incluye 0xC40410/14, almacenadas y contador)
constexpr uintptr_t STORED_N = 0xC403DC, STORED = 0xC40430, STORED_SIZE = 0x34;
constexpr uintptr_t BUF_CNT = 0xC4B950, BUF_IDX = 0xC4B958, BUF_IDX_SIZE = 0x2000, BUF_VTX = 0xC4D958, BUF_VTX_SIZE = 0x12000;
constexpr uintptr_t ENGINE = 0xC97B24, SCAN = 0xB7CD78, CURRAREA = 0xB72914, POOLS = 0xB74484, MI_PTRS = 0xA9B0C8, SECTORS = 0xB7D0B8;
constexpr int MODEL0 = 19900, NMODELS = 8;
constexpr size_t NENT = 6, NNODES = 64;

template<class T> inline T& G(uintptr_t a) { return *reinterpret_cast<T*>(a); }
// multiplos exactos de 1/8 en [-32,32] y de vez en cuando un valor raro (inf / NaN silencioso / magnitudes grandes)
inline float Qf(uint32& s) { if (Rnd(s) % 64 == 0) return PickF(s); return float(int(Rnd(s) % 513) - 256) * 0.125f; }
// multiplos exactos de 1/8 en [-32,32], siempre finitos
inline float Q8(uint32& s) { return float(int(Rnd(s) % 513) - 256) * 0.125f; }
inline float Pick(uint32& s, std::initializer_list<float> l) { return *(l.begin() + Rnd(s) % l.size()); }
inline bool Chance(uint32& s, uint32 pct) { return Rnd(s) % 100 < pct; }
inline constexpr float kQNaN = bit_cast<float>(0x7FC00000u);

// ---------------------------------------------------------------------------------------------------------------------
// memoria falsa (direcciones fijas: iguales en las dos ejecuciones)
// ---------------------------------------------------------------------------------------------------------------------
inline std::array<Buf<0x40>, 3> texs;
inline Buf<0x80> rtFake;
// parte CastPlayer
inline std::array<Buf<0x100>, NENT> ents;
inline std::array<Buf<0x60>, NENT> mats;
inline std::array<Buf<0x40>, NMODELS> mis;
inline std::array<Buf<0x30>, NMODELS> cols;
inline Buf<0x10> listBuf, dayNight;
alignas(16) inline std::array<uint8, NNODES * 8> nodeStore{};
inline std::array<uint8, NNODES> nodeStates{};
alignas(16) inline std::array<uint8, 0x40> nodeHdr{};
inline std::array<void*, 64> entVt{};

// agua falsa: resultados de GetWaterLevelNoWaves por llamada
struct WaterRet { bool ok; float level, bigW, smallW; };
inline std::array<WaterRet, 64> waterRets{}; inline size_t waterCall{};

// ---------------------------------------------------------------------------------------------------------------------
// stubs (solo registran si el hilo es el del banco)
// ---------------------------------------------------------------------------------------------------------------------
inline uint32 EntIdx(const void* e) {
    const auto u = uintptr_t(e), b = uintptr_t(ents[0].p());
    if (u >= b && u < b + NENT * sizeof(ents[0])) return uint32((u - b) / sizeof(ents[0]));
    return P(e);
}
inline RwBool __cdecl RenderStateStub(RwRenderState st, void* v) { Push(2060, { uint32(st), P(v) }); return TRUE; }
inline void* __cdecl Im3DTransformStub(RwIm3DVertex* v, RwUInt32 n, RwMatrix* m, RwUInt32 flags) {
    if (!Mine()) return nullptr; Push(2061, { P(v), n, P(m), flags, uint32(Fnv(v, sizeof(RwIm3DVertex) * n)) }); return reinterpret_cast<void*>(1);
}
inline RwBool __cdecl Im3DRenderStub(RwPrimitiveType t, RwImVertexIndex* idx, RwInt32 n) { Push(2062, { uint32(t), P(idx), uint32(n), uint32(Fnv(idx, sizeof(RwImVertexIndex) * size_t(n < 0 ? 0 : n))) }); return TRUE; }
inline RwBool __cdecl Im3DEndStub() { Push(2063); return TRUE; }

// CastShadowSectorList (0x70A630): lista, rect(4), posn, front(2), side(2), intensidad, rgb, zDist, escala, ppPolyBunch, pDayNight, tipo
inline void __cdecl CastShadowStub(void* list, float ax, float ay, float bx, float by, const CVector* p, float fx, float fy, float sx, float sy, int32 inten, uint32 r, uint32 g, uint32 b,
                                   float zd, float sc, void* pp, uint8* dn, int32 type) {
    if (!Mine()) return;
    Push(3010, { P(list) - uint32(SECTORS), Bits(ax), Bits(ay), Bits(bx), Bits(by), Bits(p->x), Bits(p->y), Bits(p->z) });
    Push(3011, { Bits(fx), Bits(fy), Bits(sx), Bits(sy), uint32(inten) & 0xFFFF, r & 0xFF, g & 0xFF, b & 0xFF });
    Push(3012, { Bits(zd), Bits(sc), P(pp), uint32(dn != nullptr), uint32(type) });
}
// CastRealTimeShadowSectorList (0x70A7E0): ..., zDist, escala, ppPolyBunch, realTimeShadow, pDayNight
inline void __cdecl CastRealTimeStub(void* list, float ax, float ay, float bx, float by, const CVector* p, float fx, float fy, float sx, float sy, int32 inten, uint32 r, uint32 g, uint32 b,
                                     float zd, float sc, void* pp, void* rt, uint8* dn) {
    if (!Mine()) return;
    Push(3020, { P(list) - uint32(SECTORS), Bits(ax), Bits(ay), Bits(bx), Bits(by), Bits(p->x), Bits(p->y), Bits(p->z) });
    Push(3021, { Bits(fx), Bits(fy), Bits(sx), Bits(sy), uint32(inten) & 0xFFFF, r & 0xFF, g & 0xFF, b & 0xFF });
    Push(3022, { Bits(zd), Bits(sc), P(pp), P(rt), uint32(dn != nullptr) });
}
// CastPlayerShadowSectorList (0x70A470) como colaborador de RenderStoredShadows
inline void __cdecl CastPlayerStub(void* list, float ax, float ay, float bx, float by, const CVector* p, float fx, float fy, float sx, float sy, int32 inten, uint32 r, uint32 g, uint32 b,
                                   float zd, float sc, void* pp, uint8* dn, int32 type) {
    if (!Mine()) return;
    Push(3030, { P(list) - uint32(SECTORS), Bits(ax), Bits(ay), Bits(bx), Bits(by), Bits(p->x), Bits(p->y), Bits(p->z) });
    Push(3031, { Bits(fx), Bits(fy), Bits(sx), Bits(sy), uint32(inten) & 0xFFFF, r & 0xFF, g & 0xFF, b & 0xFF });
    Push(3032, { Bits(zd), Bits(sc), P(pp), uint32(dn != nullptr), uint32(type) });
}
// CWaterLevel::GetWaterLevelNoWaves (0x6E8580): CVector por valor = 3 floats en la pila
inline bool __cdecl WaterLevelStub(float x, float y, float z, float* level, float* bigW, float* smallW) {
    if (!Mine()) return false;
    Push(3040, { Bits(x), Bits(y), Bits(z), uint32(level != nullptr), uint32(bigW != nullptr), uint32(smallW != nullptr) });
    const auto& r = waterRets[waterCall++ % waterRets.size()];
    if (r.ok) { *level = r.level; *bigW = r.bigW; *smallW = r.smallW; }
    return r.ok;
}
// 0x6E8550 (envoltorio del EXE sobre 0x6E7210) y CWaterLevel::CalculateWavesOnlyForCoordinate2 de la DLL: (x, y, &nivel_entra_altura_sale, grandes, pequenas)
inline void __cdecl WavesStub(int32 x, int32 y, float* inout, float bigW, float smallW) {
    if (!Mine()) return;
    Push(3050, { uint32(x), uint32(y), Bits(*inout), Bits(bigW), Bits(smallW) });
    *inout = (*inout + bigW * 0.5f) + (smallW * 0.25f + float(x) * 0.015625f) + float(y) * 0.03125f;   // determinista y dependiente de todos los argumentos
}
// CastShadowEntityXY (0x7086B0) como colaborador de CastPlayerShadowSectorList
inline void __cdecl CastEntityStub(void* e, float ax, float ay, float bx, float by, const CVector* p, float fx, float fy, float sx, float sy, int32 inten, uint32 r, uint32 g, uint32 b,
                                   float zd, float sc, void* pp, uint8* dn, int32 type) {
    if (!Mine()) return;
    Push(3100, { EntIdx(e), Bits(ax), Bits(ay), Bits(bx), Bits(by), Bits(p->x), Bits(p->y), Bits(p->z) });
    Push(3101, { Bits(fx), Bits(fy), Bits(sx), Bits(sy), uint32(inten) & 0xFFFF, r & 0xFF, g & 0xFF, b & 0xFF });
    Push(3102, { Bits(zd), Bits(sc), P(pp), P(dn), uint32(type) });
}
// CEntity::GetBoundRect (slot 9 de la vtable falsa; thiscall con puntero de salida oculto, el llamado limpia 4 bytes)
inline CRect* __fastcall BoundRectStub(void* self, void*, CRect* out) {
    std::memcpy(out, static_cast<uint8*>(self) + 0x80, 16);
    Push(3200, { EntIdx(self) });
    return out;
}
inline uint32 __cdecl TrapStub() { Push(3999); return 0; }

// puntero RwRenderStateSet del motor
struct RsGuard {
    void** slot; void* saved;
    RsGuard() { slot = reinterpret_cast<void**>(*reinterpret_cast<uintptr_t*>(ENGINE) + 0x20); saved = *slot; *slot = reinterpret_cast<void*>(&RenderStateStub); }
    ~RsGuard() { *slot = saved; }
};

// ---------------------------------------------------------------------------------------------------------------------
// ejecucion y resultado
// ---------------------------------------------------------------------------------------------------------------------
struct Ctx { bool sw; unsigned fp; };
template<class F> inline Ctx CallIt(Hook* hook, bool cand, uintptr_t addr, F&& fn) {
    nrec = 0; overflow = false; waterCall = 0; hook->State(cand);
    Ctx c{ ReversingProbe::JumpsToDll(addr) == cand, 0 };
    const unsigned saved = _controlfp(0, 0); _controlfp(_PC_24, _MCW_PC);
    fn();
    c.fp = _controlfp(0, 0); _controlfp(saved, _MCW_PC | _MCW_RC);
    return c;
}
inline json Out(const Ctx& c, json extra) {
    extra["switches"] = c.sw; extra["fp"] = c.fp; extra["trace"] = Recs();
    return extra;
}

// =====================================================================================================================
// RenderStoredShadows
// =====================================================================================================================
// Coordenada del mundo: valores sobre la rejilla de sectores (50 m), bordes del recorte [0,119], lejos y (poco) NaN/inf.
inline float PosCoord(uint32& s) {
    switch (Rnd(s) % 20) {
    case 0: return Pick(s, { 0.f, -3000.f, 3000.f, 2950.f, -2950.f, 49.99f, 50.f, -50.f, 2999.5f, -2999.5f, 100.f, -100.f });
    case 1: return Pick(s, { 3500.f, -3500.f, 100000.f, -100000.f, 1e7f, -1e7f });
    case 2: return Chance(s, 60) ? kQNaN : Pick(s, { std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity() });
    default: return float(int(Rnd(s) % 12401) - 6200) * 0.5f;
    }
}
// Componente de vector delantero/lateral
inline float SizeComp(uint32& s, bool smallW) {
    if (smallW) return Pick(s, { 0.f, 0.5f, -0.5f, 1.f, -1.f, 1.5f, -2.f, 2.5f, 3.f, -3.f, 0.25f, 4.f, -4.f, 5.f, 0.75f, -0.75f });
    switch (Rnd(s) % 20) {
    case 0: return 0.f;
    case 1: return kQNaN;
    case 2: return std::numeric_limits<float>::infinity();
    case 3: case 4: case 5: return Q8(s);
    default: return Pick(s, { 0.5f, -0.5f, 1.f, -1.f, 2.f, -2.f, 3.5f, 6.f, -8.f, 12.f, 25.f, -30.f, 60.f, 0.f });
    }
}
inline json CaseRender(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x70A9);
    std::memset(reinterpret_cast<void*>(MAIN), 0, MAIN_SIZE);
    std::memset(reinterpret_cast<void*>(BUF_CNT), 0, 8);
    std::memset(reinterpret_cast<void*>(BUF_IDX), 0xCD, BUF_IDX_SIZE);
    std::memset(reinterpret_cast<void*>(BUF_VTX), 0xCD, BUF_VTX_SIZE);
    for (size_t k = 0; k < texs.size(); ++k) { texs[k].Fill(0xCD); At<uint32>(texs[k].p(), 0) = 0x7A100000u + uint32(k) * 0x10000u; }
    rtFake.Fill(0xCD);
    G<uint16>(SCAN) = uint16(1 + Rnd(s) % 3000);   // lejos de 0xFFFF: AdvanceCurrentScanCode no llama a ResetScanCodes (0x563470, recorre el mundo real)
    const bool flush = (I % 64) == 7;               // caso grande de agua: > 512 casillas -> vacia el buffer de 0x800 vertices a mitad de la pasada
    uint32 n;
    { const uint32 r = Rnd(s) % 100; n = flush ? 1u : r < 8 ? 0u : r < 33 ? 1u : r < 78 ? 2u + Rnd(s) % 3u : 5u + Rnd(s) % 6u; }
    G<uint16>(STORED_N) = uint16(n);
    uint8 prevType = 1; uint32 prevTex = 0;
    for (uint32 k = 0; k < n; ++k) {
        uint8* sh = reinterpret_cast<uint8*>(STORED) + k * STORED_SIZE;
        const bool water = flush || Chance(s, 35);
        const bool bld = Chance(s, 50), hasRt = Chance(s, 20);
        const uint8 type = Chance(s, 70) ? prevType : uint8(Chance(s, 90) ? Rnd(s) % 11 : Rnd(s) & 0xFF);
        const uint32 tex = Chance(s, 70) ? prevTex : Rnd(s) % 3;
        prevType = type; prevTex = tex;
        if (flush) { At<float>(sh, 0x00) = float(int(Rnd(s) % 401) - 200); At<float>(sh, 0x04) = float(int(Rnd(s) % 401) - 200); }
        else { At<float>(sh, 0x00) = PosCoord(s); At<float>(sh, 0x04) = PosCoord(s); }
        At<float>(sh, 0x08) = Q8(s);
        float fx, fy, sx, sy;
        if (flush) { sx = 24.f; sy = 24.f; fx = 0.f; fy = 0.f; }
        else {
            const bool smallW = water || Chance(s, 60);
            fx = SizeComp(s, smallW); fy = SizeComp(s, smallW); sx = SizeComp(s, smallW); sy = SizeComp(s, smallW);
            if (Chance(s, 4)) { sx = 0.f; sy = 0.f; }   // 1/|lado|^2 = inf
            if (Chance(s, 4)) { fx = 0.f; fy = 0.f; }
        }
        At<float>(sh, 0x0C) = fx; At<float>(sh, 0x10) = fy; At<float>(sh, 0x14) = sx; At<float>(sh, 0x18) = sy;
        At<float>(sh, 0x1C) = Qf(s); At<float>(sh, 0x20) = Qf(s);
        At<void*>(sh, 0x24) = texs[tex].p();
        At<void*>(sh, 0x28) = hasRt ? static_cast<void*>(rtFake.p()) : nullptr;
        At<uint16>(sh, 0x2C) = Chance(s, 30) ? uint16(0x8000u + (Rnd(s) & 0x7FFF)) : uint16(Rnd(s) & 0xFFFF);
        At<uint8>(sh, 0x2E) = type;
        At<uint8>(sh, 0x2F) = uint8(Rnd(s)); At<uint8>(sh, 0x30) = uint8(Rnd(s)); At<uint8>(sh, 0x31) = uint8(Rnd(s));
        At<uint8>(sh, 0x32) = uint8((Rnd(s) & 0xFA) | (water ? 1u : 0u) | (bld ? 4u : 0u));   // bit1 (ya dibujada) al azar: la funcion lo borra al empezar
        const float z = At<float>(sh, 0x08);
        auto& wr = waterRets[k];
        wr.ok = flush || Chance(s, 85);
        wr.level = flush ? z - 1.f : Chance(s, 3) ? kQNaN : z + Pick(s, { -3.f, -1.f, -0.25f, 0.f, 0.f, 0.5f, 2.f, -0.125f });
        wr.bigW = Pick(s, { 0.f, 0.5f, 1.f, -1.f, 0.25f, 2.f }); wr.smallW = Pick(s, { 0.f, 0.25f, -0.5f, 1.f, 0.125f });
    }
    RsGuard g;
    const auto c = CallIt(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); });
    return Out(c, { {"main", Fnv(reinterpret_cast<void*>(MAIN), MAIN_SIZE)}, {"stored_n", G<uint16>(STORED_N)}, {"scan", G<uint16>(SCAN)},
                    {"buf_cnt", ReversingProbe::Bytes(reinterpret_cast<void*>(BUF_CNT), 8)}, {"buf_idx", Fnv(reinterpret_cast<void*>(BUF_IDX), BUF_IDX_SIZE)},
                    {"buf_vtx", Fnv(reinterpret_cast<void*>(BUF_VTX), BUF_VTX_SIZE)}, {"vtx_head", ReversingProbe::Bytes(reinterpret_cast<void*>(BUF_VTX), 0x24 * 16)}, {"counters", ReversingProbe::Bytes(reinterpret_cast<void*>(0xC40410), 8)} });
}

// =====================================================================================================================
// CastPlayerShadowSectorList<CPtrListSingleLink<CPhysical*>>
// =====================================================================================================================
inline json CaseCast(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x70A4);
    // pool de nodos simples LOCAL (la DLL de depuracion exige nodos vivos del pool real; los items CPhysical* no se validan contra pools)
    static_assert(sizeof(CPtrNodeSingleLinkPool) <= 0x40);
    new (nodeHdr.data()) CPtrNodeSingleLinkPool(NNODES, nodeStore.data(), nodeStates.data());   // rellena store con 0xFD y marca todos libres (0x80)
    G<void*>(POOLS) = nodeHdr.data();
    // vtable falsa de entidades: ranura 9 = GetBoundRect; el resto atrapa llamadas inesperadas
    for (auto& v : entVt) v = reinterpret_cast<void*>(&TrapStub);
    entVt[9] = reinterpret_cast<void*>(&BoundRectStub);
    // modelos falsos MODEL0..MODEL0+7 (solo se lee m_pColModel +0x14 y de ella min +0 / max +0xC)
    for (int m = 0; m < NMODELS; ++m) {
        mis[size_t(m)].Fill(0xCD); cols[size_t(m)].Fill(0xCD);
        uint8* c = cols[size_t(m)].p();
        At<float>(c, 0) = -Pick(s, { 0.5f, 1.f, 2.f }); At<float>(c, 4) = -Pick(s, { 0.5f, 1.f, 2.f }); At<float>(c, 8) = Pick(s, { -2.f, -1.f, -0.5f, 0.f, 0.5f, 1.f, kQNaN });
        At<float>(c, 0xC) = Pick(s, { 0.5f, 1.f, 2.f }); At<float>(c, 0x10) = Pick(s, { 0.5f, 1.f, 2.f }); At<float>(c, 0x14) = Pick(s, { -1.f, 0.f, 0.5f, 1.f, 1.5f, 2.f, 3.f, kQNaN });
        At<void*>(mis[size_t(m)].p(), 0x14) = c;
        G<void*>(MI_PTRS + size_t(MODEL0 + m) * 4) = mis[size_t(m)].p();
    }
    G<uint32>(CURRAREA) = uint32(Pick(s, { 0.f, 1.f, 2.f, 13.f, 5.f }));   // CGame::currArea
    G<uint16>(SCAN) = uint16(1 + Rnd(s) % 1000);
    const uint16 scan = G<uint16>(SCAN);
    const uint32 area = G<uint32>(CURRAREA);

    // corner rect, posicion y parametros
    float ax = float(int(Rnd(s) % 21) - 10), ay = float(int(Rnd(s) % 21) - 10);
    float bx = ax + float(Rnd(s) % 12), by = ay + float(Rnd(s) % 12);
    if (Chance(s, 3)) ax = kQNaN;
    if (Chance(s, 3)) by = kQNaN;
    CVector posn{ Q8(s), Q8(s), float(int(Rnd(s) % 13) - 6) * 0.5f };
    float zd = Pick(s, { 0.f, 0.5f, 1.f, 1.5f, 2.f, 3.f, 4.f });
    if (Chance(s, 2)) zd = kQNaN;
    const float fx = Qf(s), fy = Qf(s), sx = Qf(s), sy = Qf(s), sc = Qf(s);
    const int16 inten = int16(Rnd(s)); const uint8 r = uint8(Rnd(s)), g = uint8(Rnd(s)), b = uint8(Rnd(s)); const int32 type = Chance(s, 90) ? int32(Rnd(s) % 11) : int32(Rnd(s) & 0xFF);
    dayNight.Fill(0xCD);
    CPolyBunch* ppStore = nullptr; void* const pp = Chance(s, 50) ? static_cast<void*>(&ppStore) : nullptr;

    // entidades falsas
    const uint32 nEnt = 1 + Rnd(s) % NENT;
    for (uint32 k = 0; k < NENT; ++k) {
        uint8* e = ents[k].p(); uint8* m = mats[k].p();
        ents[k].Fill(0xCD); mats[k].Fill(0xCD);
        At<void*>(e, 0x00) = entVt.data();
        At<void*>(e, 0x14) = m;                       // siempre hay matriz: la rama CPlaceable::GetMatrix -> AllocateMatrix toca el pool de matrices real
        At<uint8>(e, 0x1C) = uint8((Rnd(s) & 0xFE) | (Chance(s, 85) ? 1u : 0u));      // bUsesCollision
        At<uint8>(e, 0x1D) = uint8(Rnd(s));
        At<uint8>(e, 0x1E) = uint8((Rnd(s) & 0xFE) | (Chance(s, 20) ? 1u : 0u));      // bDontCastShadowsOn
        At<int16>(e, 0x22) = int16(MODEL0 + int(Rnd(s) % NMODELS));
        At<uint16>(e, 0x2C) = Chance(s, 20) ? scan : uint16(Rnd(s) % 1000);
        At<uint8>(e, 0x2F) = Chance(s, 70) ? uint8(area) : Chance(s, 50) ? uint8(13) : uint8(Rnd(s) % 8);
        At<float>(m, 0x28) = Pick(s, { 1.f, 1.f, 0.97f, 0.9700001f, 0.9699999f, 0.98f, 0.5f, -1.f, 0.f, kQNaN });   // at.z (CEntity::GetMatrix().GetUp().z)
        At<float>(m, 0x30) = Q8(s); At<float>(m, 0x34) = Q8(s); At<float>(m, 0x38) = float(int(Rnd(s) % 13) - 6) * 0.5f;
        // CRect del bound: left/top/right/bottom en memoria 0,4,8,C (x1,y1,x2,y2) sobre una rejilla entera para provocar igualdades
        float x1 = float(int(Rnd(s) % 31) - 15), y1 = float(int(Rnd(s) % 31) - 15);
        float x2 = x1 + float(Rnd(s) % 8), y2 = y1 + float(Rnd(s) % 8);
        if (Chance(s, 25)) { x1 = ax; } if (Chance(s, 25)) { x2 = bx; } if (Chance(s, 25)) { y1 = by; } if (Chance(s, 25)) { y2 = ay; }
        if (Chance(s, 2)) x2 = kQNaN;
        At<float>(e, 0x80) = x1; At<float>(e, 0x84) = y1; At<float>(e, 0x88) = x2; At<float>(e, 0x8C) = y2;
    }
    // lista: 0..8 nodos del pool local (la cabeza es el primero), items entre las entidades (con repeticion)
    const uint32 len = Chance(s, 10) ? 0u : 1 + Rnd(s) % 8;
    nodeStore.fill(0xCD);
    uint8* lb = listBuf.p(); listBuf.Fill(0xCD); At<uint32>(lb, 0) = 0;
    for (uint32 k = 0; k < len; ++k) {
        uint8* node = nodeStore.data() + size_t(k) * 8;
        nodeStates[k] = uint8(k & 0x7F);   // ocupado (bit 7 a 0)
        At<void*>(node, 0) = ents[Rnd(s) % nEnt].p();
        At<void*>(node, 4) = (k + 1 < len) ? static_cast<void*>(nodeStore.data() + size_t(k + 1) * 8) : nullptr;
    }
    if (len) At<void*>(lb, 0) = nodeStore.data();

    const auto c = CallIt(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(void*, float, float, float, float, CVector*, float, float, float, float, int16, uint8, uint8, uint8, float, float, void*, uint8*, int32)>(addr)(
            lb, ax, ay, bx, by, &posn, fx, fy, sx, sy, inten, r, g, b, zd, sc, pp, dayNight.p(), type); });
    json entHashes = json::array();
    for (size_t k = 0; k < NENT; ++k) entHashes.push_back(Fnv(ents[k].raw.data(), ents[k].raw.size()));
    return Out(c, { {"ents", entHashes}, {"list", Fnv(listBuf.raw.data(), listBuf.raw.size())}, {"nodes", Fnv(nodeStore.data(), nodeStore.size())},
                    {"scan", G<uint16>(SCAN)}, {"pos", ReversingProbe::Bytes(&posn, 12)}, {"dn", dayNight.Bytes()} });
}

// =====================================================================================================================
// tabla de funciones y despachador
// =====================================================================================================================
inline const std::array<Fn, 2> fns{{
    {"CShadows","RenderStoredShadows",0x70A960,0,-1,512},
    {"CShadows","CastPlayerShadowSectorList<CPtrListSingleLink<CPhysical*>>",0x70A470,0,-1,1024},
}};

inline json RunCase(size_t f, unsigned i, bool cand, Hook* hook) {
    const auto a = fns[f].address;
    return f == 0 ? CaseRender(i, cand, hook, a) : CaseCast(i, cand, hook, a);
}

// Colaboradores de cada parte (direccion del EXE, simbolo de la DLL, sustituto)
inline std::vector<Stub> StubsRender() {
    using CastFn = void(*)(CPtrListSingleLink<CPhysical*>&, float, float, float, float, CVector*, float, float, float, float, int16, uint8, uint8, uint8, float, float, CPolyBunch**, uint8*, int32);
    return {
        {"RwIm3DTransform", 0x7EF450, bit_cast<uintptr_t>(&RwIm3DTransform), bit_cast<uintptr_t>(&Im3DTransformStub)},
        {"RwIm3DRenderIndexedPrimitive", 0x7EF550, bit_cast<uintptr_t>(&RwIm3DRenderIndexedPrimitive), bit_cast<uintptr_t>(&Im3DRenderStub)},
        {"RwIm3DEnd", 0x7EF520, bit_cast<uintptr_t>(&RwIm3DEnd), bit_cast<uintptr_t>(&Im3DEndStub)},
        {"CShadows::CastShadowSectorList<CPhysical*>", 0x70A630, bit_cast<uintptr_t>(static_cast<CastFn>(&CShadows::CastShadowSectorList<CPtrListSingleLink<CPhysical*>>)), bit_cast<uintptr_t>(&CastShadowStub)},
        {"CShadows::CastRealTimeShadowSectorList<CPhysical*>", 0x70A7E0, bit_cast<uintptr_t>(&CShadows::CastRealTimeShadowSectorList<CPtrListSingleLink<CPhysical*>>), bit_cast<uintptr_t>(&CastRealTimeStub)},
        {"CShadows::CastPlayerShadowSectorList<CPhysical*>", 0x70A470, bit_cast<uintptr_t>(static_cast<CastFn>(&CShadows::CastPlayerShadowSectorList<CPtrListSingleLink<CPhysical*>>)), bit_cast<uintptr_t>(&CastPlayerStub)},
        {"CWaterLevel::GetWaterLevelNoWaves", 0x6E8580, bit_cast<uintptr_t>(&CWaterLevel::GetWaterLevelNoWaves), bit_cast<uintptr_t>(&WaterLevelStub)},
        {"CWaterLevel::CalculateWavesOnlyForCoordinate2", 0x6E8550, bit_cast<uintptr_t>(&CWaterLevel::CalculateWavesOnlyForCoordinate2), bit_cast<uintptr_t>(&WavesStub)},
    };
}
inline std::vector<Stub> StubsCast() {
    return {
        {"CShadows::CastShadowEntityXY", 0x7086B0, bit_cast<uintptr_t>(&CShadows::CastShadowEntityXY), bit_cast<uintptr_t>(&CastEntityStub)},
    };
}

inline std::vector<GlobalRange> GlobalsAll() {
    return {
        {MAIN, MAIN_SIZE}, {BUF_CNT, 8}, {BUF_IDX, BUF_IDX_SIZE}, {BUF_VTX, BUF_VTX_SIZE},
        {SCAN, 2}, {CURRAREA, 4}, {POOLS, 0x20}, {MI_PTRS + size_t(MODEL0) * 4, size_t(NMODELS) * 4},
        {*reinterpret_cast<uintptr_t*>(ENGINE) + 0x20, 4},
    };
}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "shadowswave3-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "shadowswave3-runtime.jsonl"); Write({ {"event", "waiting_for_player"} }); ReversingProbe::InstallFailureCapture("shadowswave3"); }
    auto* player = FindPlayerPed(); if (!player || CCutsceneMgr::IsRunning()) return; done = true; owner = GetCurrentThreadId();
    std::string only; { std::ifstream f(dir / "shadowswave3-only.txt"); if (f) std::getline(f, only); }
    std::vector<Hook*> hooks(fns.size());
    for (size_t f = 0; f < fns.size(); ++f) if (!(hooks[f] = FindHook(fns[f].cls, fns[f].name))) { Write({ {"event", "error"}, {"reason", "hook not found"}, {"function", std::string(fns[f].cls) + "::" + fns[f].name} }); return; }
    std::vector<bool> enabled(fns.size()), locked(fns.size());
    for (size_t f = 0; f < fns.size(); ++f) { enabled[f] = hooks[f]->Hooked(); locked[f] = hooks[f]->Locked(); hooks[f]->LockState(false); }

    // Los parches de cada parte solo estan activos mientras corre su funcion (RenderStoredShadows parchea 0x70A470, que en la otra parte es la funcion probada).
    struct PartRef { size_t fn; std::vector<Stub> stubs; };
    const std::array<PartRef, 2> parts{{ {0, StubsRender()}, {1, StubsCast()} }};
    std::vector<Stub> stubs; for (auto& p : parts) for (auto& s : p.stubs) stubs.push_back(s);
    bool known = true; json siteInfo = json::array();
    for (auto& s : stubs) {
        const bool ok = s.exeAddr >= 0x401000 && s.exeAddr < 0x1000000 && s.dll >= 0x1000000 && Body(s.dll) >= 0x1000000; known &= ok;
        siteInfo.push_back({ {"name", s.name}, {"exe", s.exeAddr}, {"dll", s.dll}, {"body", Body(s.dll)}, {"ok", ok} });
    }
    std::vector<std::pair<uintptr_t, std::vector<uint8>>> entryBefore;
    for (auto& s : stubs) for (auto a : { s.exeAddr, s.dll, Body(s.dll) }) entryBefore.push_back({ a, ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8) });
    for (auto& fn : fns) entryBefore.push_back({ fn.address, ReversingProbe::Bytes(reinterpret_cast<void*>(fn.address), 8) });
    const auto globals = GlobalsAll();
    std::vector<std::vector<uint8>> saved; for (auto& g : globals) saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(g.addr), g.size));
    auto RestoreGlobals = [&]() { for (size_t j = 0; j < globals.size(); ++j) std::memcpy(reinterpret_cast<void*>(globals[j].addr), saved[j].data(), saved[j].size()); };
    const unsigned savedFP = _controlfp(0, 0);
    Write({ {"event", "begin"}, {"sites", siteInfo}, {"known", known}, {"fp_control", savedFP} });

    bool passed = known; int pair = 0;
    if (known) {
        using ReversingProbe::EntryPatch;
        for (auto& part : parts) {
            std::vector<std::unique_ptr<EntryPatch>> patches;
            // Regla 11: todos los Body() de la parte ANTES de crear ningun EntryPatch.
            std::vector<uintptr_t> bodies; for (auto& s : part.stubs) bodies.push_back(Body(s.dll));
            for (size_t k = 0; k < part.stubs.size(); ++k) {
                const auto& s = part.stubs[k];
                patches.push_back(std::make_unique<EntryPatch>(s.exeAddr, reinterpret_cast<void*>(s.stub)));
                patches.push_back(std::make_unique<EntryPatch>(s.dll, reinterpret_cast<void*>(s.stub)));
                if (bodies[k] != s.dll) patches.push_back(std::make_unique<EntryPatch>(bodies[k], reinterpret_cast<void*>(s.stub)));
            }
            const size_t f = part.fn;
            const auto& fn = fns[f]; int failures = 0; const std::string full = std::string(fn.cls) + "::" + fn.name;
            if (!only.empty() && only != full) { patches.clear(); continue; }
            for (int i = 0; i < fn.cases; ++i) {
                json original = RunCase(f, unsigned(i), false, hooks[f]); RestoreGlobals();
                json candidate = RunCase(f, unsigned(i), true, hooks[f]); RestoreGlobals();
                const bool ok = original == candidate && original["switches"] == true && candidate["switches"] == true;
                const std::string od = original.dump(), cd = candidate.dump();
                passed &= ok; failures += !ok;
                if (ok) Write({ {"event", "comparison"}, {"pair", pair++}, {"function", full}, {"case", i}, {"equal", true}, {"accepted", true},
                                {"original", {{"trace", original["trace"]}, {"ret", original.value("ret", json(nullptr))}, {"state_fnv", Fnv(od.data(), od.size())}}},
                                {"candidate", {{"trace", candidate["trace"]}, {"ret", candidate.value("ret", json(nullptr))}, {"state_fnv", Fnv(cd.data(), cd.size())}}},
                                {"events", original["trace"].size()} });
                else { json diff = json::array(); for (auto& [k, v] : original.items()) if (candidate[k] != v) diff.push_back(k);
                    Write({ {"event", "comparison"}, {"pair", pair++}, {"function", full}, {"case", i}, {"equal", false}, {"accepted", false}, {"differs", diff},
                            {"original", failures <= 2 ? original : json(nullptr)}, {"candidate", failures <= 2 ? candidate : json(nullptr)} }); }
            }
            Write({ {"event", "function_summary"}, {"function", full}, {"failures", failures}, {"cases", fn.cases} });
            patches.clear();   // restaura las entradas de esta parte
        }
    }
    for (size_t f = 0; f < fns.size(); ++f) { hooks[f]->State(enabled[f]); hooks[f]->LockState(locked[f]); }
    RestoreGlobals();
    _controlfp(savedFP, _MCW_PC | _MCW_RC);
    bool restored = _controlfp(0, 0) == savedFP;
    for (size_t j = 0; j < globals.size(); ++j) restored &= saved[j] == ReversingProbe::Bytes(reinterpret_cast<void*>(globals[j].addr), globals[j].size);
    for (auto& [a, b] : entryBefore) restored &= b == ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8);
    Write({ {"event", passed ? "complete" : "error"}, {"pairs", pair}, {"state_restored", restored} });
}
}
