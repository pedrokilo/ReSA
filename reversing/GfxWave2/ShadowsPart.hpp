#pragma once
// GfxWave2 / parte Sh: CShadows (19 funciones; 2 quedan fuera, ver notes_Sh.md).
// Original (hook desactivado) frente a candidato (hook activo) sobre OBJETOS FALSOS; los arrays estaticos de CShadows
// (0xC403A8-0xC4B6B0), el buffer temporal de RenderBuffer, las tablas del CTimeCycle, la camara, las luces puntuales y los
// contadores de CTimer son globales reales: se declaran en Globals() y el coordinador los guarda/restaura.
// Los colaboradores se doblan con Stubs() (trazas con tags 2000+). RwRenderStateSet se dobla parcheando el puntero del
// motor (*0xC97B24 + 0x20) dentro de cada ejecucion (se restaura al salir).
#include "GfxCommon.hpp"
#include "Shadows.h"
#include "FireManager.h"
#include "PointLights.h"
#include "RealTimeShadow.h"
#include "TimeCycle.h"
#include "Camera.h"
#include "3dMarkers.h"
#include "StencilShadows.h"
#include "TxdStore.h"
#include "Fx/Fx.h"
#include "Pipelines/CustomBuilding/CustomBuildingDNPipeline.h"

namespace GfxWave2Live { namespace Sh {
using std::bit_cast;

constexpr uintptr_t MAIN = 0xC403A8, MAIN_END = 0xC4B6B0, MAIN_SIZE = MAIN_END - MAIN;   // g_ShadowVertices .. aPermanentShadows
constexpr uintptr_t STORED_N = 0xC403DC, STORED = 0xC40430, EMPTY_LIST = 0xC403D8, TEXTURES = 0xC403E0, POLYBUNCHES = 0xC40DF0, STATICS = 0xC4A030, PERMS = 0xC4AC30;
constexpr uintptr_t BUF_CNT = 0xC4B950, BUF_IDX = 0xC4B958, BUF_IDX_SIZE = 0x2000, BUF_VTX = 0xC4D958, BUF_VTX_SIZE = 0x12000;
constexpr uintptr_t CAM_POS = 0xB6F02C, CAM_MATPTR = 0xB6F03C, CAM_FRONT2D = 0xB6F104, CAM_ACTIVE = 0xB6F081, CAM_MODE0 = 0xB6F1A8, CUTSCENE = 0xB5F851;
constexpr uintptr_t TC_STRENGTH = 0xB7C4E8, TC_SIDES = 0xB79ED0, TC_SIDES_SIZE = 0x104, TC_IDX = 0xB79FD0, TC_SUN = 0xB7CA50, TC_SUN_SIZE = 0xC0;
constexpr uintptr_t TIME_MS_ADDR = 0xB7CB84, TIMESTEP_ADDR = 0xB7CB5C, FOG_ADDR = 0xC81300, DNBAL_ADDR = 0x8D12C0, LIGHTS = 0xC3F0D0, LIGHTS_SIZE = 0x610, ENGINE = 0xC97B24;

// ---------------------------------------------------------------------------------------------------------------------
// utilidades de fixtures
// ---------------------------------------------------------------------------------------------------------------------
template<class T> inline T& G(uintptr_t a) { return *reinterpret_cast<T*>(a); }
inline void FillRand(void* p, size_t n, uint32& s) { auto* b = static_cast<uint8*>(p); for (size_t k = 0; k < n; ++k) b[k] = uint8(Rnd(s) >> 8); }
// multiplos exactos de 1/8 en [-32,32] y de vez en cuando un valor raro (inf / NaN silencioso / magnitudes grandes)
inline float Qf(uint32& s) { if (Rnd(s) % 64 == 0) return PickF(s); return float(int(Rnd(s) % 513) - 256) * 0.125f; }
inline float Pick(uint32& s, std::initializer_list<float> l) { return *(l.begin() + Rnd(s) % l.size()); }
inline bool Chance(uint32& s, uint32 pct) { return Rnd(s) % 100 < pct; }

inline Buf<0x7A4> ped, other; inline Buf<0x600> phys; inline Buf<0x4C> rt; inline Buf<0x100> frame; inline Buf<0x6A0> veh; inline Buf<0x50> mat; inline Buf<0x100> hand;
inline Buf<0x30> colModel; inline Buf<0x200> ent; inline Buf<0x40> tex0, tex1, tex2; inline Buf<0x20> outs;
inline CVector boneRet{}, plyrCoords{}; inline void* playerPedRet{}; inline void* playerVehRet{}; inline bool sphereRet{}, hqRet{}; inline int32 fxQ{};
inline void* texRet{}; inline void* frameRet{}; inline uint32 texCounter{};
inline void* im3dRet = reinterpret_cast<void*>(1);

// ---------------------------------------------------------------------------------------------------------------------
// stubs (solo registran si el hilo es el del banco)
// ---------------------------------------------------------------------------------------------------------------------
inline uint32 Hs(const char* s) { return s ? uint32(Fnv(s, std::strlen(s))) : 0u; }
inline void __cdecl TxdPush() { Push(2001); }
inline void __cdecl TxdPop() { Push(2005); }
inline void __cdecl TxdSet(int32 slot) { Push(2003, { uint32(slot) }); }
inline int32 __cdecl TxdFind(const char* name) { Push(2002, { Hs(name) }); return 7; }
inline RwTexture* __cdecl TexRead(const char* name, const char* mask) {
    if (!Mine()) return nullptr;
    Push(2004, { Hs(name), P(mask) }); return reinterpret_cast<RwTexture*>(0x6D100000u + (++texCounter) * 0x40u);
}
inline RwBool __cdecl TexDestroy(RwTexture* t) { Push(2006, { P(t) }); return TRUE; }
inline CFire* __fastcall FireStub(CFireManager*, void*, CVector pos, float size, uint8 unused, CEntity* creator, uint32 time, int8 gen, uint8 unused2) {
    Push(2010, { Bits(pos.x), Bits(pos.y), Bits(pos.z), Bits(size), uint32(unused), P(creator), time, uint32(uint8(gen)) << 8 | unused2 }); return nullptr;
}
inline void __fastcall BoneStub(void* self, void*, CVector* out, int32 bone, bool upd) {
    if (!Mine()) { *out = CVector{}; return; }
    Push(2020, { P(self), uint32(bone), uint32(upd) }); *out = boneRet;
}
inline void* __cdecl PlayerPedStub(int32 id) { if (!Mine()) return nullptr; Push(2021, { uint32(id) }); return playerPedRet; }
inline bool __fastcall SphereStub(void* self, void*, const CVector* pos, float r) {
    if (!Mine()) return false; Push(2022, { P(self), Bits(pos->x), Bits(pos->y), Bits(pos->z), Bits(r) }); return sphereRet;
}
inline int32 __fastcall FxQualityStub(void* self, void*) { if (!Mine()) return *reinterpret_cast<int32*>(uintptr_t(self) + 0x54); return fxQ; }   // getter puro NOTSA (la fuente lo evalua con cortocircuito distinto al EXE; fxQ=0 -> mismo resultado): sin traza
inline RwTexture* __fastcall GetTexStub(void* self, void*) { Push(2030, { P(self) }); return static_cast<RwTexture*>(texRet); }
inline void __fastcall BorderStub(void* self, void*, const RwRGBA* c) { Push(2031, { P(self), c->red, c->green, c->blue, c->alpha }); }
inline void* __fastcall SetLightStub(void* self, void*, float az, float el, bool set) { Push(2032, { P(self), Bits(az), Bits(el), uint32(set) }); return frameRet; }
inline void* __cdecl PlayerVehStub(int32 id, bool remote) { if (!Mine()) return nullptr; Push(2040, { uint32(id), uint32(remote) }); return playerVehRet; }
inline CVector* __cdecl PlayerCoordsStub(CVector* out, int32 id) { Push(2041, { uint32(id) }); *out = Mine() ? plyrCoords : CVector{}; return out; }
inline void* __fastcall ColModelStub(void* self, void*) { if (!Mine()) return nullptr; Push(2042, { P(self) }); return colModel.p(); }
inline bool __cdecl StoreStaticStub(uint32 id, uint32 type, RwTexture* t, const CVector* pos, float fx, float fy, float sx, float sy, int32 intensity, uint32 r, uint32 g, uint32 b,
                                    float zd, float scale, float drawDist, uint32 temporary, float up) {
    Push(2050, { id, type & 0xFF, P(t), Bits(pos->x), Bits(pos->y), Bits(pos->z), Bits(fx), Bits(fy) });
    Push(2051, { Bits(sx), Bits(sy), uint32(intensity) & 0xFFFF, r & 0xFF, g & 0xFF, b & 0xFF, Bits(zd), Bits(scale) });
    Push(2052, { Bits(drawDist), temporary & 0xFF, Bits(up) });
    return true;
}
inline bool __cdecl HighQualityStub() { Push(2053); return hqRet; }
inline RwBool __cdecl RenderStateStub(RwRenderState st, void* v) { Push(2060, { uint32(st), P(v) }); return TRUE; }
inline void* __cdecl Im3DTransformStub(RwIm3DVertex* v, RwUInt32 n, RwMatrix* m, RwUInt32 flags) {
    if (!Mine()) return nullptr; Push(2061, { P(v), n, P(m), flags, uint32(Fnv(v, sizeof(RwIm3DVertex) * n)) }); return im3dRet;
}
inline RwBool __cdecl Im3DRenderStub(RwPrimitiveType t, RwImVertexIndex* idx, RwInt32 n) { Push(2062, { uint32(t), P(idx), uint32(n), uint32(Fnv(idx, sizeof(RwImVertexIndex) * size_t(n < 0 ? 0 : n))) }); return TRUE; }
inline RwBool __cdecl Im3DEndStub() { Push(2063); return TRUE; }
inline void __cdecl MarkerStub(uint32 id, uint32 type, CVector* pos, float size, uint32 r, uint32 g, uint32 b, uint32 a, uint32 pulse, float frac, uint32 rot) {
    Push(2070, { id, type, Bits(pos->x), Bits(pos->y), Bits(pos->z), Bits(size) }); Push(2071, { r & 0xFF, g & 0xFF, b & 0xFF, a & 0xFF, pulse & 0xFFFF, Bits(frac), rot & 0xFFFF });
}

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
    nrec = 0; hook->State(cand);
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
inline json MainState() {
    json j;
    const uint16 n = G<uint16>(STORED_N);
    j["main"] = Fnv(reinterpret_cast<void*>(MAIN), MAIN_SIZE);
    j["stored_n"] = n;
    j["stored"] = ReversingProbe::Bytes(reinterpret_cast<void*>(STORED), 0x34 * std::min<size_t>(size_t(n) + 1, 48));
    j["buf_cnt"] = ReversingProbe::Bytes(reinterpret_cast<void*>(BUF_CNT), 8);
    j["buf_idx"] = Fnv(reinterpret_cast<void*>(BUF_IDX), BUF_IDX_SIZE);
    j["buf_vtx"] = Fnv(reinterpret_cast<void*>(BUF_VTX), BUF_VTX_SIZE);
    return j;
}
inline void SetCamera(uint32& s, float x, float y, float z) { G<float>(CAM_POS) = x; G<float>(CAM_POS + 4) = y; G<float>(CAM_POS + 8) = z; G<uint32>(CAM_MATPTR) = 0; (void)s; }
inline void RandomStoredCount(uint32& s) { static constexpr uint16 c[] = { 0, 1, 5, 46, 47, 48, 49, 60 }; G<uint16>(STORED_N) = Chance(s, 60) ? c[Rnd(s) % std::size(c)] : uint16(Rnd(s) % 48); }
inline void FillMain(uint32& s) { FillRand(reinterpret_cast<void*>(MAIN), MAIN_SIZE, s); }
inline void ClearMain() { std::memset(reinterpret_cast<void*>(MAIN), 0, MAIN_SIZE); }

// =====================================================================================================================
inline json CaseInit(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x7060); FillMain(s); texCounter = 0;
    const auto c = CallIt(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); });
    return Out(c, { {"state", MainState()}, {"verts", ReversingProbe::Bytes(reinterpret_cast<void*>(MAIN), 48)}, {"tex", ReversingProbe::Bytes(reinterpret_cast<void*>(TEXTURES), 48)},
                    {"empty", G<uint32>(EMPTY_LIST)}, {"polys", Fnv(reinterpret_cast<void*>(POLYBUNCHES), 0x9240)} });
}
inline json CaseShutdown(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x706E); FillMain(s);
    for (int k = 0; k < 12; ++k) G<uint32>(TEXTURES + k * 4) = 0x5A000000u + uint32(k) * 0x100u + (Rnd(s) & 0xFF);
    const auto c = CallIt(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); });
    return Out(c, { {"state", MainState()}, {"tex", ReversingProbe::Bytes(reinterpret_cast<void*>(TEXTURES), 48)} });
}
inline json CaseTidy(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x7077); FillMain(s);
    const auto c = CallIt(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); });
    return Out(c, { {"state", MainState()} });
}
inline json CaseStoreTex(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x7073); FillMain(s); RandomStoredCount(s);
    const uint32 type = Chance(s, 70) ? Rnd(s) % 9 : Rnd(s) & 0xFF; auto* t = reinterpret_cast<void*>(Rnd(s) | 1u);
    CVector pos{ Qf(s), Qf(s), Qf(s) };
    const float a = Qf(s), b = Qf(s), cc = Qf(s), d = Qf(s), zd = Qf(s), scale = Qf(s);
    const uint32 inten = Rnd(s) & 0xFFFF, r = Rnd(s) & 0xFF, g = Rnd(s) & 0xFF, bl = Rnd(s) & 0xFF, water = Rnd(s) & 1, bld = Rnd(s) & 1; auto* rts = reinterpret_cast<void*>(Rnd(s) | 1u);
    const auto c = CallIt(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(uint32, void*, const CVector*, float, float, float, float, uint32, uint32, uint32, uint32, float, uint32, float, void*, uint32)>(addr)(
            type, t, &pos, a, b, cc, d, inten, r, g, bl, zd, water, scale, rts, bld); });
    return Out(c, { {"state", MainState()} });
}
inline json CaseStoreType(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x7079); FillMain(s); RandomStoredCount(s);
    const uint32 type = Chance(s, 85) ? Rnd(s) % 10 : Rnd(s) & 0xFF;
    CVector pos{ Qf(s), Qf(s), Qf(s) };
    const float a = Qf(s), b = Qf(s), cc = Qf(s), d = Qf(s);
    const uint32 inten = Rnd(s) & 0xFFFF, r = Rnd(s) & 0xFF, g = Rnd(s) & 0xFF, bl = Rnd(s) & 0xFF;
    const auto c = CallIt(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(uint32, const CVector*, float, float, float, float, uint32, uint32, uint32, uint32)>(addr)(type, &pos, a, b, cc, d, inten, r, g, bl); });
    return Out(c, { {"state", MainState()} });
}
inline json CaseRenderMode(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    const uint32 type = I < 90 ? I % 11 : (I % 2 ? 255u : 9u + I % 4);
    RsGuard g;
    const auto c = CallIt(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(uint32)>(addr)(type); });
    return Out(c, { {"type", type} });
}
inline void FillPerm(uint32& s) {
    FillRand(reinterpret_cast<void*>(PERMS), sizeof(CShadows::aPermanentShadows), s);
    for (size_t k = 0; k < CShadows::aPermanentShadows.size(); ++k) {
        auto& sh = CShadows::aPermanentShadows[k];
        sh.m_nType = static_cast<eShadowType>(Chance(s, 55) ? (Chance(s, 50) ? SHADOW_OIL_1 : SHADOW_OIL_5) : Rnd(s) % 9);
        sh.m_vecPosn = CVector{ float(int(Rnd(s) % 25) - 12) * 0.5f, float(int(Rnd(s) % 25) - 12) * 0.5f, float(Rnd(s) % 5) * 0.25f };
    }
}
inline json CaseRemoveOil(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x7074); ClearMain(); FillPerm(s);
    const float x0 = float(int(Rnd(s) % 25) - 12) * 0.5f, y0 = float(int(Rnd(s) % 25) - 12) * 0.5f;
    float x1 = x0 + float(Rnd(s) % 7) * 0.5f, y1 = y0 + float(Rnd(s) % 7) * 0.5f;
    float x0v = x0, y0v = y0;
    if (Chance(s, 3)) x0v = bit_cast<float>(0x7FC00000u);
    if (Chance(s, 3)) y1 = bit_cast<float>(0x7FC00000u);
    const auto c = CallIt(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(float, float, float, float)>(addr)(x0v, x1, y0v, y1); });
    return Out(c, { {"state", MainState()} });
}
inline json CaseGunShot(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x7075); ClearMain(); FillPerm(s);
    CVector origin{ Qf(s) * 0.25f, Qf(s) * 0.25f, Pick(s, { 0.f, 1.f, 0.5f, -1.f, 2.f, 3.7f }) }, target{ Qf(s) * 0.25f, Qf(s) * 0.25f, Pick(s, { 0.f, 1.f, 0.5f, -1.f, 2.f, 3.7f }) };
    if (Chance(s, 25)) target = origin;
    const auto c = CallIt(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(const CVector*, const CVector*)>(addr)(&origin, &target); });
    return Out(c, { {"state", MainState()} });
}
inline json CasePrintPoly(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x7076); FillMain(s);
    auto* a = reinterpret_cast<CVector*>(Rnd(s) | 1u); auto* b = reinterpret_cast<CVector*>(Rnd(s) | 1u); auto* d = reinterpret_cast<CVector*>(Rnd(s) | 1u);
    const auto c = CallIt(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(CVector*, CVector*, CVector*)>(addr)(a, b, d); });
    return Out(c, { {"state", MainState()} });
}
inline json CaseCalcPed(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x7077);
    outs.Fill(0xCD);
    CVector sun{ Qf(s), Qf(s), Qf(s) };
    if (Chance(s, 4)) { sun.x = 0.f; sun.y = 0.f; }
    if (Chance(s, 10)) { sun.x = Pick(s, { 0.5f, -0.5f, 1.f, -1.f }); sun.y = Pick(s, { 0.f, 0.5f, -1.f, 2.f }); }
    uint8* o = outs.p();
    const auto c = CallIt(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(CVector, float*, float*, float*, float*, float*, float*)>(addr)(sun, &At<float>(o, 0), &At<float>(o, 4), &At<float>(o, 8), &At<float>(o, 12), &At<float>(o, 16), &At<float>(o, 20)); });
    return Out(c, { {"out", outs.Bytes()} });
}
inline json CaseAffect(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x7078);
    outs.Fill(0xCD);
    G<float>(DNBAL_ADDR) = Pick(s, { 0.f, 1.f, 0.5f, 0.25f, 0.75f, -0.5f, 2.f, 0.1f, 0.9f, bit_cast<float>(0x7FC00000u) });
    const uint32 type = Chance(s, 80) ? Rnd(s) % 10 : Rnd(s) & 0xFF, dn = Rnd(s) & 0xFF, r = Rnd(s) & 0xFF, g = Rnd(s) & 0xFF, b = Rnd(s) & 0xFF;
    uint8* o = outs.p();
    const auto c = CallIt(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(uint32, uint32, uint32, uint32, uint32, uint8*, uint8*, uint8*)>(addr)(type, dn, r, g, b, o, o + 1, o + 2); });
    return Out(c, { {"out", outs.Bytes()} });
}

// --- StoreShadowForPedObject / StoreRealTimeShadow -------------------------------------------------------------------
inline void SetupCameraAndBone(uint32& s, uint8* body) {
    const float cx = Qf(s) * 0.5f, cy = Qf(s) * 0.5f, cz = Qf(s) * 0.5f;
    SetCamera(s, cx, cy, cz);
    float dx = 0.f, dy = 0.f;
    switch (Rnd(s) % 9) {
    case 0: dx = 15.f; break;                    // igualdad con el limite (225)
    case 1: dx = 14.9999f; break;
    case 2: dx = 7.5f; break;                    // mitad exacta
    case 3: dx = float(int(Rnd(s) % 25) - 12); dy = float(int(Rnd(s) % 25) - 12) * 0.5f; break;
    case 4: dx = float(int(Rnd(s) % 41) - 20); dy = float(int(Rnd(s) % 41) - 20) * 0.5f; break;
    case 5: dx = bit_cast<float>(0x7FC00000u); break;
    case 6: break;
    case 7: dx = -3.5f; dy = 7.25f; break;
    default: dx = 40.f; dy = -9.f; break;
    }
    boneRet = CVector{ cx + dx, cy + dy, cz + Qf(s) * 0.25f };
    At<float>(body, 4) = boneRet.x + 0.25f; At<float>(body, 8) = boneRet.y - 0.5f; At<float>(body, 12) = boneRet.z;
    G<int16>(TC_STRENGTH) = int16(Pick(s, { 0.f, 1.f, 50.f, 100.f, 200.f, 255.f, 1000.f, 32767.f, -5.f, 7.f }));
}
inline json CaseStorePed(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x707B); ClearMain(); RandomStoredCount(s); G<uint16>(STORED_N) %= 49;
    ped.Fill(0xCD); other.Fill(0xA5);
    uint8* p = ped.p();
    At<uint8>(p, 0x36) = uint8((Rnd(s) & 0xF8) | (Chance(s, 25) ? 4u : 3u)); At<void*>(p, 0x14) = nullptr;
    SetupCameraAndBone(s, p);
    const bool isPlayer = Chance(s, 30);
    playerPedRet = isPlayer ? static_cast<void*>(p) : (Chance(s, 50) ? nullptr : static_cast<void*>(other.p()));
    sphereRet = Chance(s, 60); fxQ = 0;
    const float dx = Qf(s), dy = Qf(s), fx = Qf(s), fy = Qf(s), sx = Qf(s), sy = Qf(s);
    const auto c = CallIt(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(void*, float, float, float, float, float, float)>(addr)(p, dx, dy, fx, fy, sx, sy); });
    return Out(c, { {"state", MainState()}, {"ped", ped.Bytes()} });
}
inline json CaseStoreRT(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x707C); ClearMain(); RandomStoredCount(s); G<uint16>(STORED_N) %= 49;
    phys.Fill(0xCD); rt.Fill(0xCD); frame.Fill(0xCD); other.Fill(0xA5);
    uint8* p = phys.p();
    static constexpr uint32 tls[] = { 0, 1, 2, 4, 5 };
    const uint32 tl = Chance(s, 55) ? 3u : tls[Rnd(s) % std::size(tls)];
    At<uint8>(p, 0x36) = uint8((Rnd(s) & 0xF8) | tl); At<void*>(p, 0x14) = nullptr;
    At<void*>(p, 0x134) = Chance(s, 90) ? static_cast<void*>(rt.p()) : nullptr;
    At<uint8>(rt.p(), 5) = uint8(Pick(s, { 0.f, 1.f, 50.f, 100.f, 127.f, 128.f, 200.f, 255.f }));
    SetupCameraAndBone(s, p);
    const bool isPlayer = Chance(s, 30);
    playerPedRet = isPlayer ? static_cast<void*>(p) : (Chance(s, 50) ? nullptr : static_cast<void*>(other.p()));
    sphereRet = Chance(s, 60); fxQ = 0; texRet = reinterpret_cast<void*>(0x6E000000u + (Rnd(s) & 0xFFFF) * 0x10u); frameRet = frame.p();
    for (size_t off : { 0x30u, 0x34u, 0x38u }) At<float>(frame.p(), off) = Chance(s, 5) ? 0.f : PickNear(s);
    for (int k = 0; k < 48; ++k) G<float>(TC_SUN + k * 4) = Chance(s, 40) ? PickF(s) : Pick(s, { 0.f, 0.5f, -0.5f, 1.f, -1.f, 0.25f, -0.75f, 2.f });
    G<int32>(TC_IDX) = int32(Rnd(s) % 16);
    const float dx = Qf(s), dy = Qf(s), fx = Qf(s), fy = Qf(s), sx = Qf(s), sy = Qf(s);
    const auto c = CallIt(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(void*, float, float, float, float, float, float)>(addr)(p, dx, dy, fx, fy, sx, sy); });
    return Out(c, { {"state", MainState()}, {"phys", phys.Bytes()}, {"rt", rt.Bytes()}, {"frame", frame.Bytes()} });
}

// --- UpdateStaticShadows -----------------------------------------------------------------------------------------------
inline json CaseUpdateStatic(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x7F40); FillMain(s);
    auto* pb = CShadows::aPolyBunches.data();
    static constexpr uint32 nows[] = { 5000, 100000, 20000, 0 };
    G<uint32>(TIME_MS_ADDR) = Chance(s, 10) ? 0xFFFFFFF0u : nows[Rnd(s) % std::size(nows)];
    // Listas DISJUNTAS: cada sombra tiene su propio tramo de 5 bunches [i*5, i*5+5) y la lista vacia vive en [240,360).
    // Antes la cabeza vacia / los tramos se solapaban al azar: tras el primer Free() el ultimo nodo apuntaba a la cabeza
    // vacia anterior (anterior en la cadena de otra sombra) y se formaba un CICLO -> `while (it->m_pNext)` infinito
    // en el EXE y en la fuente (GTA colgado en UpdateStaticShadows caso 2).
    static_assert(MAX_STATIC_SHADOWS * 5 <= 240);
    const size_t emptyStart = 240 + Rnd(s) % 120;
    G<uint32>(EMPTY_LIST) = Chance(s, 20) ? 0u : P(&pb[emptyStart]);
    for (size_t k = 0; k < 360; ++k) {
        const bool inShadowRange = k < 240;
        const size_t blockEnd = inShadowRange ? (k / 5) * 5 + 5 : 360;   // nunca salta de bloque
        pb[k].m_pNext = (Chance(s, 35) || k + 1 >= blockEnd) ? nullptr : &pb[k + 1 + Rnd(s) % std::min<size_t>(3, blockEnd - 1 - k)];
    }
    size_t shIdx = 0;
    for (auto& sh : CShadows::aStaticShadows) {
        sh.m_pPolyBunch = Chance(s, 25) ? nullptr : &pb[shIdx * 5 + Rnd(s) % 5]; ++shIdx;
        sh.m_nId = Rnd(s); sh.m_bJustCreated = Chance(s, 30); sh.m_bTemporaryShadow = Chance(s, 50); sh.m_bRendered = Chance(s, 50);
        static constexpr uint32 deltas[] = { 0, 1, 4999, 5000, 5001, 9000 };
        const uint32 delta = deltas[Rnd(s) % std::size(deltas)];
        sh.m_nTimeCreated = G<uint32>(TIME_MS_ADDR) - delta;
    }
    const auto c = CallIt(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); });
    return Out(c, { {"state", MainState()} });
}

// --- RenderExtraPlayerShadows ------------------------------------------------------------------------------------------
inline json CaseExtra(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x7FA0); ClearMain(); veh.Fill(0xCD); mat.Fill(0xCD); hand.Fill(0xCD); colModel.Fill(0xCD);
    G<uint16>(STORED_N) = uint16(Rnd(s) % 49);
    uint8* v = veh.p(); uint8* m = mat.p();
    At<int16>(v, 0x22) = int16(Chance(s, 10) ? 441 : 400 + int(Rnd(s) % 50));
    static constexpr uint32 vtypes[] = { 9, 8, 5 };
    At<uint32>(v, 0x590) = Chance(s, 20) ? vtypes[Rnd(s) % 3] : Rnd(s) % 5;
    reinterpret_cast<CVehicle*>(v)->m_pHandlingData = reinterpret_cast<tHandlingData*>(hand.p());
    static constexpr eVehicleHandlingModelFlags flags[] = { VEHICLE_HANDLING_MODEL_NONE, VEHICLE_HANDLING_MODEL_NONE, VEHICLE_HANDLING_MODEL_NONE, VEHICLE_HANDLING_MODEL_IS_BIKE,
                                                              VEHICLE_HANDLING_MODEL_IS_HELI, VEHICLE_HANDLING_MODEL_IS_PLANE, VEHICLE_HANDLING_MODEL_IS_BOAT };
    reinterpret_cast<tHandlingData*>(hand.p())->m_nModelFlags = flags[Rnd(s) % std::size(flags)];
    At<void*>(v, 0x14) = m; At<uint32>(m, 0x40) = 0; At<uint32>(m, 0x44) = 0;
    const float ang = float(int(Rnd(s) % 8)) * 0.25f;   // vectores fwd/right cualquiera (no hace falta ortonormal)
    At<float>(m, 0x00) = Pick(s, { 1.f, 0.f, -1.f, 0.5f, ang }); At<float>(m, 0x04) = Pick(s, { 0.f, 1.f, -0.5f, ang }); At<float>(m, 0x08) = 0.f;
    At<float>(m, 0x10) = Pick(s, { 0.f, 1.f, -1.f, 0.75f, -ang }); At<float>(m, 0x14) = Pick(s, { 1.f, 0.f, 0.5f, -1.f, ang }); At<float>(m, 0x18) = 0.f;
    At<float>(m, 0x28) = Pick(s, { 1.f, 1.f, -1.f, 0.f, 0.5f, -0.5f, bit_cast<float>(0x7FC00000u) });
    At<float>(m, 0x30) = Qf(s) * 0.5f; At<float>(m, 0x34) = Qf(s) * 0.5f; At<float>(m, 0x38) = Qf(s) * 0.25f;
    uint8* cm = colModel.p();
    for (size_t off : { 0x00u, 0x04u, 0x08u }) At<float>(cm, off) = -Pick(s, { 0.5f, 1.f, 1.25f, 2.4f, 0.f });
    for (size_t off : { 0x0Cu, 0x10u, 0x14u }) At<float>(cm, off) = Pick(s, { 0.5f, 1.f, 1.5f, 2.6f, 0.f, 0.1f });
    playerVehRet = Chance(s, 90) ? static_cast<void*>(v) : nullptr;
    plyrCoords = CVector{ At<float>(m, 0x30) + Qf(s) * 0.125f, At<float>(m, 0x34) + Qf(s) * 0.125f, Qf(s) * 0.25f };
    G<int16>(TC_STRENGTH + 2) = int16(Pick(s, { 0.f, 1.f, 50.f, 100.f, 255.f, -100.f, 7.f, 80.f }));
    G<int16>(TC_STRENGTH) = 100;
    FillRand(reinterpret_cast<void*>(LIGHTS), LIGHTS_SIZE, s);
    const uint32 n = Rnd(s) % 7; G<uint32>(LIGHTS) = n;
    for (uint32 k = 0; k < 32; ++k) {
        auto& l = CPointLights::aLights[k];
        l.m_vecPosn = plyrCoords + CVector{ Qf(s) * 0.25f, Qf(s) * 0.25f, Qf(s) * 0.25f };
        l.m_fRadius = Pick(s, { 0.f, 1.f, 5.f, 20.f, 30.f, 12.5f, bit_cast<float>(0x7FC00000u) });
        l.m_fColorRed = Pick(s, { 0.f, 0.f, 1.f, 0.5f }); l.m_fColorGreen = Pick(s, { 0.f, 1.f, 0.25f }); l.m_fColorBlue = Pick(s, { 0.f, 1.f, 0.75f });
        l.m_nType = static_cast<ePointLightType>(Chance(s, 80) ? 0 : 1 + int(Rnd(s) % 2)); l.m_bGenerateShadows = Chance(s, 75);
    }
    RsGuard g;
    const auto c = CallIt(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); });
    return Out(c, { {"state", MainState()}, {"veh", veh.Bytes()} });
}

// --- RenderStaticShadows -------------------------------------------------------------------------------------------------
inline json CaseRenderStatic(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x8300); ClearMain();
    tex0.Fill(0xCD); tex1.Fill(0xCD); tex2.Fill(0xCD);
    At<uint32>(tex0.p(), 0) = 0x7A100000u; At<uint32>(tex1.p(), 0) = 0x7A200000u; At<uint32>(tex2.p(), 0) = 0x7A300000u;
    void* texs[3] = { tex0.p(), tex1.p(), tex2.p() };
    auto* pb = CShadows::aPolyBunches.data(); size_t used = 0;
    const uint32 maxChain = Chance(s, 15) ? 7 : 3;
    G<float>(FOG_ADDR) = Pick(s, { 0.f, 0.5f, 1.f, -0.5f, 2.f, 0.25f });
    G<float>(DNBAL_ADDR) = Pick(s, { 0.f, 1.f, 0.5f, 0.3f, 0.8f });
    G<uint32>(BUF_CNT) = Rnd(s) % 500; G<uint32>(BUF_CNT + 4) = Rnd(s) % 500;
    for (auto& sh : CShadows::aStaticShadows) {
        sh.m_nType = static_cast<eShadowType>(Chance(s, 90) ? 1 + Rnd(s) % 8 : Rnd(s) % 10);
        sh.m_pTexture = static_cast<RwTexture*>(texs[Rnd(s) % 3]);
        sh.m_nRed = uint8(Rnd(s)); sh.m_nGreen = uint8(Rnd(s)); sh.m_nBlue = uint8(Rnd(s)); sh.m_nIntensity = uint16(Chance(s, 15) ? 0x8000u + (Rnd(s) & 0x7FFF) : Rnd(s) & 0xFF);
        sh.m_nDayNightIntensity = uint8(Rnd(s)); sh.m_bRendered = Chance(s, 50); sh.m_bJustCreated = Chance(s, 50); sh.m_bTemporaryShadow = Chance(s, 50);
        sh.m_pPolyBunch = nullptr;
        if (Chance(s, 70)) {
            const uint32 len = 1 + Rnd(s) % maxChain; CPolyBunch* prev = nullptr;
            for (uint32 k = 0; k < len && used < 360; ++k) {
                auto& pbk = pb[used++]; pbk.m_pNext = nullptr;
                pbk.m_wNumVerts = int16(3 + Rnd(s) % 5);
                for (auto& v : pbk.m_avecPosn) v = CVector{ Qf(s), Qf(s), Qf(s) };
                for (auto& u : pbk.m_aU) u = uint8(Rnd(s)); for (auto& vv : pbk.m_aV) vv = uint8(Rnd(s));
                if (prev) prev->m_pNext = &pbk; else sh.m_pPolyBunch = &pbk;
                prev = &pbk;
            }
        }
    }
    RsGuard g;
    const auto c = CallIt(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); });
    return Out(c, { {"state", MainState()} });
}

// --- RenderIndicatorShadow / StoreCarLightShadow / StoreShadowForPole -----------------------------------------------------
inline json CaseIndicator(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x70CC);
    const uint32 id = Rnd(s); CVector pos{ Qf(s), Qf(s), Qf(s) };
    float fx = Qf(s), sx = Qf(s), fy = Qf(s), sy = Qf(s);
    switch (Rnd(s) % 6) { case 0: sx = -fx; break; case 1: fx = 0.f; sx = 0.f; break; case 2: fx = 0.f; sx = -0.f; break; case 3: fx = -0.f; sx = 0.f; break; default: break; }
    const uint32 inten = Rnd(s) & 0xFFFF; auto* t = reinterpret_cast<void*>(Rnd(s) | 1u); const uint32 type = Rnd(s) % 9;
    const auto c = CallIt(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(uint32, uint32, void*, const CVector*, float, float, float, float, uint32)>(addr)(id, type, t, &pos, fx, fy, sx, sy, inten); });
    return Out(c, { {"pos", ReversingProbe::Bytes(&pos, 12)} });
}
inline json CaseCarLight(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x70C5); ClearMain(); RandomStoredCount(s); G<uint16>(STORED_N) %= 49; veh.Fill(0xCD);
    uint8* v = veh.p();
    const float cx = Qf(s) * 0.5f, cy = Qf(s) * 0.5f; SetCamera(s, cx, cy, Qf(s));
    G<uint8>(CAM_ACTIVE) = 0; G<uint16>(CAM_MODE0) = uint16(Pick(s, { 1.f, 37.f, 4.f, 15.f, 18.f, 0.f })); G<uint8>(CUTSCENE) = Chance(s, 15) ? 1 : 0;
    float fnx = Pick(s, { 1.f, 0.f, -1.f, 0.6f, 0.8f }), fny = Pick(s, { 0.f, 1.f, -1.f, 0.8f, -0.6f });
    G<float>(CAM_FRONT2D) = fnx; G<float>(CAM_FRONT2D + 4) = fny;
    float dx = 0.f, dy = 0.f;
    switch (Rnd(s) % 8) {
    case 0: dx = 27.f; break;        // igualdad con el limite (729)
    case 1: dx = 20.25f; break;      // limite del fundido
    case 2: dx = 18.f; dy = 0.f; break;
    case 3: dx = 26.5f; dy = 3.f; break;
    case 4: dx = float(int(Rnd(s) % 61) - 30); dy = float(int(Rnd(s) % 61) - 30) * 0.5f; break;
    case 5: dx = -3.f; break;
    case 6: dx = bit_cast<float>(0x7FC00000u); break;
    default: dx = float(int(Rnd(s) % 21) - 10); dy = float(int(Rnd(s) % 21) - 10); break;
    }
    CVector posn{ cx + dx, cy + dy, Qf(s) };
    float maxView = Pick(s, { -1.f, 0.f, 0.5f, 1.f, -0.5f, 0.9f });
    if (Chance(s, 25)) maxView = -((posn.y - cy) * fny + (posn.x - cx) * fnx);   // igualdad exacta con el producto escalar
    At<float>(v, 0x44) = Pick(s, { 0.f, 0.4f, 0.1f, 0.5f, 0.3f }); At<float>(v, 0x48) = Pick(s, { 0.f, 0.1f }); At<float>(v, 0x4C) = Pick(s, { 0.f, 0.05f });
    G<float>(TIMESTEP_ADDR) = Pick(s, { 1.f, 0.5f, 0.0333f, 2.f });
    playerVehRet = Chance(s, 35) ? static_cast<void*>(v) : (Chance(s, 50) ? nullptr : static_cast<void*>(other.p()));
    fxQ = 0;
    const int32 id = int32(Rnd(s) % 8); auto* t = reinterpret_cast<void*>(Rnd(s) | 1u);
    const float fx = Qf(s), fy = Qf(s), sx = Qf(s), sy = Qf(s); const uint32 r = Rnd(s) & 0xFF, g = Rnd(s) & 0xFF, b = Rnd(s) & 0xFF;
    const auto c = CallIt(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(void*, int32, void*, const CVector*, float, float, float, float, uint32, uint32, uint32, float)>(addr)(v, id, t, &posn, fx, fy, sx, sy, r, g, b, maxView); });
    return Out(c, { {"state", MainState()}, {"veh", veh.Bytes()} });
}
inline json CasePole(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x70C7); ClearMain(); ent.Fill(0xCD); mat.Fill(0xCD);
    uint8* e = ent.p(); uint8* m = mat.p();
    At<void*>(e, 0x14) = m; At<uint32>(m, 0x40) = 0; At<uint32>(m, 0x44) = 0;
    At<float>(m, 0x00) = Pick(s, { 1.f, 0.f, 0.6f }); At<float>(m, 0x04) = Pick(s, { 0.f, 1.f, 0.8f }); At<float>(m, 0x10) = Pick(s, { 0.f, 1.f, -0.8f }); At<float>(m, 0x14) = Pick(s, { 1.f, 0.f, 0.6f });
    At<float>(m, 0x28) = Pick(s, { 1.f, 0.5f, 0.4999f, 0.75f, -1.f, 0.9f, 0.6f, bit_cast<float>(0x7FC00000u) });
    At<float>(m, 0x30) = Qf(s); At<float>(m, 0x34) = Qf(s); At<float>(m, 0x38) = Qf(s);
    G<int16>(TC_STRENGTH + 4) = int16(Pick(s, { 0.f, 1.f, 100.f, 255.f, -3.f, 64.f, 127.f }));
    hqRet = Chance(s, 20);
    for (int k = 0; k < 48; ++k) G<float>(TC_SUN + k * 4) = Pick(s, { 0.f, 0.5f, -0.5f, 1.f, -1.f, 0.25f, -0.75f });
    FillRand(reinterpret_cast<void*>(TC_SIDES), 0x80, s);
    for (int k = 0; k < 32; ++k) G<float>(TC_SIDES + k * 4) = Pick(s, { 0.f, 0.5f, -0.5f, 1.f, -1.f, 0.3f });
    G<int32>(TC_IDX) = int32(Rnd(s) % 16);
    const float ox = Qf(s), oy = Qf(s), oz = Qf(s), ph = Pick(s, { 0.f, 1.f, 2.5f, 6.f, -4.f, 8.f }), pw = Pick(s, { 0.f, 0.25f, 1.f, 0.7f }); const uint32 lid = Rnd(s) & 0xFF;
    const auto c = CallIt(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(void*, float, float, float, float, float, uint32)>(addr)(e, ox, oy, oz, ph, pw, lid); });
    return Out(c, { {"state", MainState()}, {"ent", ent.Bytes()} });
}

// =====================================================================================================================
// tabla de funciones y despachador
// =====================================================================================================================
inline const std::array<Fn, 19> fns{{
    {"CShadows","Init",0x706CD0,0,-1,64},
    {"CShadows","Shutdown",0x706ED0,0,-1,64},
    {"CShadows","TidyUpShadows",0x707770,0,-1,64},
    {"CShadows","StoreShadowToBeRendered-Texture",0x707390,0,-1,512},
    {"CShadows","StoreShadowToBeRendered-Type",0x707930,0,-1,384},
    {"CShadows","SetRenderModeForShadowType",0x707460,0,-1,96},
    {"CShadows","RemoveOilInArea",0x7074F0,0,-1,384},
    {"CShadows","GunShotSetsOilOnFire",0x707550,0,-1,384},
    {"CShadows","PrintDebugPoly",0x7076B0,0,-1,64},
    {"CShadows","CalcPedShadowValues",0x7076C0,0,-1,512},
    {"CShadows","AffectColourWithLighting",0x707850,0,-1,768},
    {"CShadows","StoreShadowForPedObject",0x707B40,0,-1,512},
    {"CShadows","StoreRealTimeShadow",0x707CA0,0,-1,512},
    {"CShadows","UpdateStaticShadows",0x707F40,0,-1,256},
    {"CShadows","RenderExtraPlayerShadows",0x707FA0,0,-1,384},
    {"CShadows","RenderStaticShadows",0x708300,0,-1,192},
    {"CShadows","RenderIndicatorShadow",0x70CCB0,0,-1,256},
    {"CShadows","StoreCarLightShadow",0x70C500,0,-1,512},
    {"CShadows","StoreShadowForPole",0x70C750,0,-1,384},
}};

inline json Run(size_t f, unsigned i, bool cand, Hook* hook) {
    const auto a = fns[f].address;
    switch (f) {
    case 0: return CaseInit(i, cand, hook, a);
    case 1: return CaseShutdown(i, cand, hook, a);
    case 2: return CaseTidy(i, cand, hook, a);
    case 3: return CaseStoreTex(i, cand, hook, a);
    case 4: return CaseStoreType(i, cand, hook, a);
    case 5: return CaseRenderMode(i, cand, hook, a);
    case 6: return CaseRemoveOil(i, cand, hook, a);
    case 7: return CaseGunShot(i, cand, hook, a);
    case 8: return CasePrintPoly(i, cand, hook, a);
    case 9: return CaseCalcPed(i, cand, hook, a);
    case 10: return CaseAffect(i, cand, hook, a);
    case 11: return CaseStorePed(i, cand, hook, a);
    case 12: return CaseStoreRT(i, cand, hook, a);
    case 13: return CaseUpdateStatic(i, cand, hook, a);
    case 14: return CaseExtra(i, cand, hook, a);
    case 15: return CaseRenderStatic(i, cand, hook, a);
    case 16: return CaseIndicator(i, cand, hook, a);
    case 17: return CaseCarLight(i, cand, hook, a);
    default: return CasePole(i, cand, hook, a);
    }
}

inline std::vector<Stub> Stubs() {
    using FireFn = CFire*(CFireManager::*)(CVector, float, uint8, CEntity*, uint32, int8, uint8);
    return {
        {"CTxdStore::PushCurrentTxd", 0x7316A0, bit_cast<uintptr_t>(&CTxdStore::PushCurrentTxd), bit_cast<uintptr_t>(&TxdPush)},
        {"CTxdStore::PopCurrentTxd", 0x7316B0, bit_cast<uintptr_t>(&CTxdStore::PopCurrentTxd), bit_cast<uintptr_t>(&TxdPop)},
        {"CTxdStore::SetCurrentTxd", 0x7319C0, bit_cast<uintptr_t>(&CTxdStore::SetCurrentTxd), bit_cast<uintptr_t>(&TxdSet)},
        {"CTxdStore::FindTxdSlot", 0x731850, bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot)), bit_cast<uintptr_t>(&TxdFind)},
        {"RwTextureRead", 0x7F3AC0, bit_cast<uintptr_t>(&RwTextureRead), bit_cast<uintptr_t>(&TexRead)},
        {"RwTextureDestroy", 0x7F3820, bit_cast<uintptr_t>(&RwTextureDestroy), bit_cast<uintptr_t>(&TexDestroy)},
        {"CFireManager::StartFire", 0x539F00, bit_cast<uintptr_t>(static_cast<FireFn>(&CFireManager::StartFire)), bit_cast<uintptr_t>(&FireStub)},
        {"CPed::GetBonePosition", 0x5E4280, bit_cast<uintptr_t>(static_cast<void(CPed::*)(CVector*, eBoneTag, bool)>(&CPed::GetBonePosition)), bit_cast<uintptr_t>(&BoneStub)},
        {"FindPlayerPed", 0x56E210, bit_cast<uintptr_t>(&FindPlayerPed), bit_cast<uintptr_t>(&PlayerPedStub)},
        {"CCamera::IsSphereVisible", 0x420D40, bit_cast<uintptr_t>(static_cast<bool(CCamera::*)(const CVector&, float)>(&CCamera::IsSphereVisible)), bit_cast<uintptr_t>(&SphereStub)},
        {"Fx_c::GetFxQuality", 0x49EA50, bit_cast<uintptr_t>(&Fx_c::GetFxQuality), bit_cast<uintptr_t>(&FxQualityStub)},
        {"CRealTimeShadow::GetShadowRwTexture", 0x7059F0, bit_cast<uintptr_t>(&CRealTimeShadow::GetShadowRwTexture), bit_cast<uintptr_t>(&GetTexStub)},
        {"CRealTimeShadow::DrawBorderAroundTexture", 0x705A00, bit_cast<uintptr_t>(&CRealTimeShadow::DrawBorderAroundTexture), bit_cast<uintptr_t>(&BorderStub)},
        {"CRealTimeShadow::SetLightProperties", 0x705900, bit_cast<uintptr_t>(&CRealTimeShadow::SetLightProperties), bit_cast<uintptr_t>(&SetLightStub)},
        {"FindPlayerVehicle", 0x56E0D0, bit_cast<uintptr_t>(&FindPlayerVehicle), bit_cast<uintptr_t>(&PlayerVehStub)},
        {"FindPlayerCoors", 0x56E010, bit_cast<uintptr_t>(static_cast<CVector(*)(int32)>(&FindPlayerCoors)), bit_cast<uintptr_t>(&PlayerCoordsStub)},
        {"CEntity::GetColModel", 0x535300, bit_cast<uintptr_t>(&CEntity::GetColModel), bit_cast<uintptr_t>(&ColModelStub)},
        {"CShadows::StoreStaticShadow", 0x70BA00, bit_cast<uintptr_t>(&CShadows::StoreStaticShadow), bit_cast<uintptr_t>(&StoreStaticStub)},
        {"GraphicsHighQuality", 0x70F9B0, bit_cast<uintptr_t>(static_cast<bool(*)()>(&::GraphicsHighQuality)), bit_cast<uintptr_t>(&HighQualityStub)},
        {"RwIm3DTransform", 0x7EF450, bit_cast<uintptr_t>(&RwIm3DTransform), bit_cast<uintptr_t>(&Im3DTransformStub)},
        {"RwIm3DRenderIndexedPrimitive", 0x7EF550, bit_cast<uintptr_t>(&RwIm3DRenderIndexedPrimitive), bit_cast<uintptr_t>(&Im3DRenderStub)},
        {"RwIm3DEnd", 0x7EF520, bit_cast<uintptr_t>(&RwIm3DEnd), bit_cast<uintptr_t>(&Im3DEndStub)},
        {"C3dMarkers::PlaceMarkerSet", 0x725BA0, bit_cast<uintptr_t>(&C3dMarkers::PlaceMarkerSet), bit_cast<uintptr_t>(&MarkerStub)},
    };
}

inline std::vector<GlobalRange> Globals() {
    return {
        {MAIN, MAIN_SIZE}, {BUF_CNT, 8}, {BUF_IDX, BUF_IDX_SIZE}, {BUF_VTX, BUF_VTX_SIZE},
        {TC_STRENGTH, 6}, {TC_SIDES, TC_SIDES_SIZE}, {TC_SUN, TC_SUN_SIZE},
        {0xB6F028, sizeof(CCamera)}, {CUTSCENE, 1}, {LIGHTS, LIGHTS_SIZE},
        {TIME_MS_ADDR, 4}, {TIMESTEP_ADDR, 4}, {FOG_ADDR, 4}, {DNBAL_ADDR, 4},
        {*reinterpret_cast<uintptr_t*>(ENGINE) + 0x20, 4},
    };
}
}}
