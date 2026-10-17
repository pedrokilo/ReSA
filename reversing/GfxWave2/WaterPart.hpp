#pragma once
// GfxWave2 - parte Wl: CWaterLevel (22 funciones pendientes de docs/hooks.csv).
// Original (hook desactivado) frente a candidato (hook activo) sobre estado falso: los arrays de CWaterLevel son globales reales
// que se rellenan en cada caso (Globals() los guarda/restaura); los colaboradores se doblan con EntryPatch locales (Iso) que
// solo viven durante cada ejecucion y que trazan sus argumentos crudos; nada dibuja (RwIm3D*/LittleTest se doblan).
// Trazas con etiquetas 3000+. Fuera del banco (ver notes_Wl.md): WaterLevelInitialise, RenderBoatWakes, SplitWaterTriangleAlongYLine.
#include "GfxCommon.hpp"
#include "WaterLevel.h"
#include "Renderer.h"
#include "Camera.h"
#include "Timer.h"
#include "Weather.h"
#include "Game.h"

namespace GfxWave2Live { namespace Wl {
using ReversingProbe::EntryPatch;

// ---------------------------------------------------------------------------------------------------------------------
// Tabla
// ---------------------------------------------------------------------------------------------------------------------
inline const std::array<Fn, 22> fns{{
    {"CWaterLevel","Shutdown",0x6E59E0,0,-1,64},
    {"CWaterLevel","AddWaterLevelVertex",0x6E5A40,0,-1,1024},
    {"CWaterLevel","AddWaterLevelTriangle",0x6E7D40,0,-1,1024},
    {"CWaterLevel","AddWaterLevelQuad",0x6E7EF0,0,-1,1024},
    {"CWaterLevel","BlockHit",0x6E6CA0,0,-1,512},
    {"CWaterLevel","CalculateWavesOnlyForCoordinate",0x6E6EF0,0,-1,1024},
    {"CWaterLevel","GetWaterLevel-",0x6EB690,0,-1,1024},
    {"CWaterLevel","MarkQuadsAndPolysToBeRendered",0x6E5810,0,-1,1024},
    {"CWaterLevel","PreRenderWater",0x6EB710,0,-1,512},
    {"CWaterLevel","RenderFlatWaterRectangle",0x6EBEC0,0,-1,1024},
    {"CWaterLevel","RenderFlatWaterRectangle_OneLayer",0x6E9940,0,-1,1024},
    {"CWaterLevel","RenderFlatWaterTriangle",0x6EE080,0,-1,512},
    {"CWaterLevel","RenderFlatWaterTriangle_OneLayer",0x6E8ED0,0,-1,1024},
    {"CWaterLevel","RenderWakeSegment",0x6EA260,0,-1,1024},
    {"CWaterLevel","RenderWaterRectangle",0x6EC5D0,0,-1,1024},
    {"CWaterLevel","RenderWaterTriangle",0x6EE240,0,-1,1024},
    {"CWaterLevel","ScanThroughBlocks",0x6E6D10,0,-1,256},
    {"CWaterLevel","SetCameraRange",0x6E9C80,0,-1,512},
    {"CWaterLevel","SetUpWaterFog",0x6EA9F0,0,-1,1024},
    {"CWaterLevel","SplitWaterRectangleAlongXLine",0x6E73A0,0,-1,512},
    {"CWaterLevel","SplitWaterRectangleAlongYLine",0x6ED6D0,0,-1,512},
    {"CWaterLevel","SplitWaterTriangleAlongXLine",0x6ECF00,0,-1,512},
}};

// ---------------------------------------------------------------------------------------------------------------------
// Utilidades
// ---------------------------------------------------------------------------------------------------------------------
inline uint32 HpOne(uint64 h, const CRenPar& p) {
    const uint32 a[5]{ Bits(p.z), Bits(p.bigWaves), Bits(p.smallWaves), uint32(uint8(p.flowX)), uint32(uint8(p.flowY)) };
    return uint32(Fnv(a, sizeof(a), h));
}
inline uint32 Hp(std::initializer_list<CRenPar> l) {
    uint64 h = 1469598103934665603ull;
    for (const auto& p : l) {
        const uint32 a[5]{ Bits(p.z), Bits(p.bigWaves), Bits(p.smallWaves), uint32(uint8(p.flowX)), uint32(uint8(p.flowY)) };
        h = Fnv(a, sizeof(a), h);
    }
    return uint32(h);
}
inline uint32 U(int32 v) { return uint32(v); }

// Unidad de control x87: 24 bits durante la llamada (como el motor), nrec a cero.
struct Ctl {
    Hook* h; bool cand; uintptr_t addr; unsigned savedFp; bool sw{};
    Ctl(Hook* hook, bool c, uintptr_t a) : h(hook), cand(c), addr(a) { savedFp = _controlfp(0, 0); _controlfp(_PC_24, _MCW_PC); nrec = 0; }
    void Switch() { h->State(cand); sw = ReversingProbe::JumpsToDll(addr) == cand; }
    json Finish(json r) { r["switches"] = sw; r["fp"] = _controlfp(0, 0) & _MCW_PC; r["trace"] = Recs(); return r; }
    ~Ctl() { _controlfp(savedFp, _MCW_PC); }
};

// Desactiva (durante la ejecucion) hooks de otras funciones de CWaterLevel que el EXE llamaria por su entrada.
struct HooksOff {
    std::vector<std::pair<Hook*, bool>> v;
    HooksOff(std::initializer_list<const char*> names) {
        for (auto* n : names) if (auto* h = FindHook("CWaterLevel", n)) { v.push_back({ h, h->Hooked() }); h->State(false); }
    }
    ~HooksOff() { for (auto it = v.rbegin(); it != v.rend(); ++it) it->first->State(it->second); }
};

// Parches locales de colaboradores: entrada del EXE + simbolo de la DLL (+ cuerpo si es un thunk E9).
struct IsoSite { uintptr_t exe; uintptr_t dll; void* stub; };
struct Iso {
    std::vector<std::unique_ptr<EntryPatch>> p;
    Iso(std::initializer_list<IsoSite> l) {
        std::vector<uintptr_t> bodies; for (auto& s : l) bodies.push_back(s.dll ? Body(s.dll) : 0);  // regla 11: Body() antes de parchear
        size_t k = 0;
        for (auto& s : l) {
            p.push_back(std::make_unique<EntryPatch>(s.exe, s.stub));
            if (s.dll) {
                p.push_back(std::make_unique<EntryPatch>(s.dll, s.stub));
                if (bodies[k] != s.dll) p.push_back(std::make_unique<EntryPatch>(bodies[k], s.stub));
            }
            ++k;
        }
    }
};
template<class F> inline void* SP(F f) { return reinterpret_cast<void*>(f); }
template<class F> inline uintptr_t DA(F f) { return std::bit_cast<uintptr_t>(f); }

inline int RgI(uint32& s, int lo, int hi) { return lo + int(Rnd(s) % uint32(hi - lo + 1)); }
inline float PickZ(uint32& s) {
    static constexpr float t[] = { 0.f, 1.5f, -3.f, 950.f, 949.5f, 950.5f, 5.f, 12.25f, -0.f, 951.f, 100.125f, 2.f };
    return t[Rnd(s) % std::size(t)];
}
inline CRenPar RandPar(uint32& s) {
    CRenPar p;
    p.z = (Rnd(s) % 100 < 75) ? PickZ(s) : float(int(Rnd(s) % 4001) - 2000) * 0.0625f;
    p.bigWaves = (Rnd(s) % 3) ? 0.f : float(Rnd(s) % 9) * 0.125f;
    p.smallWaves = (Rnd(s) % 3) ? 0.f : float(Rnd(s) % 9) * 0.0625f;
    p.flowX = int8(Rnd(s)); p.flowY = int8(Rnd(s));
    return p;
}

// ---------------------------------------------------------------------------------------------------------------------
// Estado global (rangos reales que se guardan/restauran)
// ---------------------------------------------------------------------------------------------------------------------
inline std::vector<GlobalRange> Globals() {
    return {
        { 0xC22910, 1021 * 20 },                       // m_aVertices
        { 0xC22884, 0x1C },                            // contadores tri/quad/vertices + m_CurrentFlow + m_CurrentDesiredFlow
        { 0xC22854, 48 },                              // WaterTriangles
        { 0xC21C90, 301 * 10 },                        // WaterQuads
        { 0xC21B70, 288 },                             // m_BlockPolyInfo
        { 0xC215F8, 1400 },                            // m_PolyCombos
        { 0xC215F0, 4 },                               // NumWaterZonePolys
        { 0xC214D0, 0x120 },                           // bloques fuera del mundo: Y[70], X[70], contador
        { 0xC228A0, 0x20 },                            // m_nWaterConfiguration..texturas
        { 0xC1F950, 0x10 },                            // CameraRange*
        { 0xC21168, 0x20 },                            // colores y desplazamientos de textura
        { 0xC21188, 840 },                             // ms_WaterFog
        { 0xC228D4, 0x18 },                            // desplazamiento de UV de estelas, bandera de niebla, indice
        { 0xC4B950, 8 },                               // contadores del buffer temporal
        { 0xC4B958, 8192 },                            // aTempBufferIndices
        { 0xC4D958, sizeof(TempVertexBuffer) },        // TempBufferVertices
        { 0x8D37D0, 0x30 },                            // DETAILEDWATERDIST, bSplitBigPolys, niebla
        { 0x8D3808, 8 },                               // WaterLayerAlpha
        { 0xB6F02C, 0x14 },                            // posicion y puntero a matriz de TheCamera
        { 0xB6F97C, 4 },                               // TheCamera.m_pRwCamera
        { 0xB72914, 4 },                               // CGame::currArea
        { 0xB7CB4C, 0x40 },                            // CTimer (contador, tiempo, paso)
        { 0xC812E0, 0x10 },                            // CWeather::SunGlare, Wavyness, WindClipped
    };
}

// ---------------------------------------------------------------------------------------------------------------------
// Colaboradores doblados (trazas crudas)
// ---------------------------------------------------------------------------------------------------------------------
inline int scenario{};
inline void __cdecl StTriFlat(int32 a, int32 b, CRenPar p, int32 c, int32 d, CRenPar q, int32 e, int32 f, CRenPar r) { if (!Mine()) return; Push(3001, { U(a), U(b), U(c), U(d), U(e), U(f), Hp({ p, q, r }) }); }
inline void __cdecl StTriSplitX(int32 sp, int32 a, int32 b, CRenPar p, int32 c, int32 d, CRenPar q, int32 e, int32 f, CRenPar r) { if (!Mine()) return; Push(3002, { U(sp), U(a), U(b), U(c), U(d), U(e), U(f), Hp({ p, q, r }) }); }
inline void __cdecl StTriSplitY(int32 sp, int32 a, int32 b, CRenPar p, int32 c, int32 d, CRenPar q, int32 e, int32 f, CRenPar r) { if (!Mine()) return; Push(3003, { U(sp), U(a), U(b), U(c), U(d), U(e), U(f), Hp({ p, q, r }) }); }
inline void __cdecl StTriHigh(int32 a, int32 b, CRenPar p, int32 c, int32 d, CRenPar q, int32 e, int32 f, CRenPar r) { if (!Mine()) return; Push(3004, { U(a), U(b), U(c), U(d), U(e), U(f), Hp({ p, q, r }) }); }
inline void __cdecl StTriRender(int32 a, int32 b, CRenPar p, int32 c, int32 d, CRenPar q, int32 e, int32 f, CRenPar r) { if (!Mine()) return; Push(3013, { U(a), U(b), U(c), U(d), U(e), U(f), Hp({ p, q, r }) }); }
inline void __cdecl StTriOne(int32 a, int32 b, CRenPar p, int32 c, int32 d, CRenPar q, int32 e, int32 f, CRenPar r, int32 layer) { if (!Mine()) return; Push(3012, { U(a), U(b), U(c), U(d), U(e), U(f), Hp({ p, q, r }), U(layer) }); }
inline void __cdecl StRectRender(int32 x0, int32 x1, int32 y0, int32 y1, CRenPar p, CRenPar q, CRenPar r, CRenPar t) { if (!Mine()) return; Push(3005, { U(x0), U(x1), U(y0), U(y1), Hp({ p, q, r, t }) }); }
inline void __cdecl StRectFlat(int32 x0, int32 x1, int32 y0, int32 y1, CRenPar p, CRenPar q, CRenPar r, CRenPar t) { if (!Mine()) return; Push(3006, { U(x0), U(x1), U(y0), U(y1), Hp({ p, q, r, t }) }); }
inline void __cdecl StRectHigh(int32 x0, int32 x1, int32 y0, int32 y1, CRenPar p, CRenPar q, CRenPar r, CRenPar t) { if (!Mine()) return; Push(3009, { U(x0), U(x1), U(y0), U(y1), Hp({ p, q, r, t }) }); }
inline void __cdecl StRectSplitX(int32 sp, int32 x0, int32 x1, int32 y0, int32 y1, CRenPar p, CRenPar q, CRenPar r, CRenPar t) { if (!Mine()) return; Push(3007, { U(sp), U(x0), U(x1), U(y0), U(y1), Hp({ p, q, r, t }) }); }
inline void __cdecl StRectOne(int32 x0, int32 x1, int32 y0, int32 y1, CRenPar p, CRenPar q, CRenPar r, CRenPar t, int32 layer) { if (!Mine()) return; Push(3011, { U(x0), U(x1), U(y0), U(y1), Hp({ p, q, r, t }), U(layer) }); }
inline void __cdecl StFog(int32 a, int32 b, int32 c, int32 d) { if (!Mine()) return; Push(3010, { U(a), U(b), U(c), U(d) }); }
inline void __cdecl StMark(int32 x, int32 y, uint32 raw) { if (!Mine()) return; Push(3014, { U(x), U(y), raw & 0xFFu }); }
inline void __cdecl StScanWorld(CVector2D* pts, int32 n, void* fn) {
    if (!Mine()) return;
    Push(3016, { U(n), U(P(fn) == 0x6E6CA0u || fn == reinterpret_cast<void*>(&CWaterLevel::BlockHit)) /* callback BlockHit: exe o DLL */, Bits(pts[0].x), Bits(pts[0].y), Bits(pts[1].x), Bits(pts[1].y) });
    Push(3017, { Bits(pts[2].x), Bits(pts[2].y), Bits(pts[3].x), Bits(pts[3].y), Bits(pts[4].x), Bits(pts[4].y) });
}
inline bool __cdecl StCanSee() { if (!Mine()) return false; const bool r = scenario % 7 != 0; Push(3018, { r }); return r; }
inline void __cdecl StFind() { if (!Mine()) return; Push(3019); }
inline void __cdecl StBeach() { if (!Mine()) return; Push(3020); }
inline void __cdecl StScanBlocks() { if (!Mine()) return; Push(3021); }
inline RwBool __cdecl StTexDestroy(RwTexture* t) { if (!Mine()) return TRUE; Push(3022, { P(t) }); return TRUE; }
inline void __cdecl StLittle() { if (!Mine()) return; Push(3023); }
inline void* __cdecl StTransform(RwIm3DVertex* v, RwUInt32 n, RwMatrix* m, RwUInt32 flags) {
    if (!Mine()) return nullptr;
    const bool ok = scenario % 5 != 0; Push(3024, { P(v), n, P(m), flags, uint32(Fnv(v, n * sizeof(RwIm3DVertex))), ok }); return ok ? v : nullptr;
}
inline RwBool __cdecl StRenderIdx(RwPrimitiveType t, RwImVertexIndex* idx, RwInt32 n) { if (!Mine()) return TRUE; Push(3025, { uint32(t), uint32(n), uint32(Fnv(idx, size_t(n) * sizeof(RwImVertexIndex))) }); return TRUE; }
inline RwBool __cdecl StEnd() { if (!Mine()) return TRUE; Push(3026); return TRUE; }

// GetWaterLevelNoWaves / AddWaveToResult / FindPlayerCoors dobles controlados por el caso.
inline bool nwRet{}; inline float nwLevel{}, nwBig{}, nwSmall{}; inline CVector fpCoords{};
inline bool __cdecl StNoWaves(CVector pos, float* lvl, float* big, float* smallW) {
    if (!Mine()) return false;
    Push(3027, { Bits(pos.x), Bits(pos.y), Bits(pos.z), big != nullptr, smallW != nullptr, nwRet });
    if (!nwRet) return false;
    *lvl = nwLevel; if (big) *big = nwBig; if (smallW) *smallW = nwSmall; return true;
}
inline void __cdecl StAddWave(float x, float y, float* lvl, float a, float b, CVector* n) {
    if (!Mine()) return;
    Push(3028, { Bits(x), Bits(y), Bits(*lvl), Bits(a), Bits(b), n != nullptr });
    *lvl = *lvl + 0.25f; if (n) { *n = CVector{ 1.f, 2.f, 3.f }; }
}
inline CVector* __cdecl StPlayerCoors(CVector* out, int32 id) { if (Mine()) Push(3029, { U(id) }); *out = fpCoords; return out; }

inline std::vector<Stub> Stubs() { return {}; }  // Todos los parches viven en Iso (por caso).

// Sitios habituales
#define WL_SITE(exe, fn, st) IsoSite{ exe, DA(&CWaterLevel::fn), SP(&st) }
#define RW_SITES IsoSite{ 0x541330, DA(&LittleTest), SP(&StLittle) }, IsoSite{ 0x7EF450, 0, SP(&StTransform) }, IsoSite{ 0x7EF550, 0, SP(&StRenderIdx) }, IsoSite{ 0x7EF520, 0, SP(&StEnd) }

// ---------------------------------------------------------------------------------------------------------------------
// Casos
// ---------------------------------------------------------------------------------------------------------------------
inline void FillVertices(uint32& s, int n) {
    std::memset(&CWaterLevel::m_aVertices, 0xCD, sizeof(CWaterLevel::m_aVertices));
    for (int k = 0; k < n; ++k) {
        auto& v = CWaterLevel::m_aVertices[k];
        v.x = int16(RgI(s, -40, 40) * 50); v.y = int16(RgI(s, -40, 40) * 50); v.rp = RandPar(s);
    }
}
inline json VertsState(int upto) {
    // campos reales (el EXE copia el relleno de CWaterVertex desde basura de pila) + resto en bruto
    uint64 h = 1469598103934665603ull; upto = std::min(upto, 1021);
    for (int k = 0; k < upto; ++k) { const auto& v = CWaterLevel::m_aVertices[k]; const uint32 a[2]{ uint32(uint16(v.x)), uint32(uint16(v.y)) }; h = Fnv(a, sizeof(a), h); h = HpOne(h, v.rp) ; }
    const auto* base = reinterpret_cast<const uint8*>(&CWaterLevel::m_aVertices);
    return json{ {"fields", h}, {"rest", Fnv(base + size_t(upto) * 20, size_t(1021 - upto) * 20)} };
}

// ---- Shutdown ----
inline json CaseShutdown(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x3001);
    Ctl c(hook, cand, addr);
    CWaterLevel::texWaterclear256 = (Rnd(s) & 1) ? reinterpret_cast<RwTexture*>(0x10001000 + (Rnd(s) & 0xFF0)) : nullptr;
    CWaterLevel::texSeabd32 = (Rnd(s) & 1) ? reinterpret_cast<RwTexture*>(0x20002000 + (Rnd(s) & 0xFF0)) : nullptr;
    CWaterLevel::texWaterwake = (Rnd(s) & 1) ? reinterpret_cast<RwTexture*>(0x30003000 + (Rnd(s) & 0xFF0)) : nullptr;
    Iso iso{ IsoSite{ 0x7F3820, 0, SP(&StTexDestroy) } };
    c.Switch();
    reinterpret_cast<void(__cdecl*)()>(addr)();
    return c.Finish(json{ {"tex", json::array({ P(CWaterLevel::texWaterclear256), P(CWaterLevel::texSeabd32), P(CWaterLevel::texWaterwake) })} });
}

// ---- AddWaterLevelVertex ----
inline json CaseAddVertex(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x3002);
    Ctl c(hook, cand, addr);
    const int n = RgI(s, 0, 29);
    FillVertices(s, n);
    CWaterLevel::NumWaterVertices = uint32(n);
    static constexpr int32 coords[]{ -4000, -3500, -3001, -3000, -2999, -2000, -1, 0, 1, 777, 2999, 3000, 3001, 4000 };
    int32 X, Y; CRenPar P = RandPar(s);
    if (n > 0 && Rnd(s) % 100 < 45) {                       // reutilizar un vertice (coordenadas y z iguales -> acierto)
        const auto& v = CWaterLevel::m_aVertices[Rnd(s) % n]; X = v.x; Y = v.y;
        if (Rnd(s) % 100 < 60) P.z = v.rp.z;
    } else { X = coords[Rnd(s) % std::size(coords)]; Y = coords[Rnd(s) % std::size(coords)]; if (Rnd(s) % 4 == 0) { X = RgI(s, -3200, 3200); Y = RgI(s, -3200, 3200); } }
    c.Switch();
    const uint32 ret = reinterpret_cast<uint32(__cdecl*)(int32, int32, CRenPar)>(addr)(X, Y, P);
    return c.Finish(json{ {"ret", ret}, {"n", CWaterLevel::NumWaterVertices}, {"verts", VertsState(n + 1)} });
}

inline json PolyState(bool quad, int count) {
    json r = json::array();
    const auto* base = quad ? reinterpret_cast<const uint8*>(&CWaterLevel::WaterQuads) : reinterpret_cast<const uint8*>(&CWaterLevel::WaterTriangles);
    const size_t sz = quad ? 10 : 8;
    r.push_back(Fnv(base, sz * size_t(count)));
    return r;
}

// ---- AddWaterLevelTriangle ----
inline json CaseAddTriangle(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x3003);
    Ctl c(hook, cand, addr);
    HooksOff off{ "AddWaterLevelVertex" };
    const int n = RgI(s, 0, 24); FillVertices(s, n);
    CWaterLevel::NumWaterVertices = uint32(n);
    std::memset(&CWaterLevel::WaterTriangles, 0xAA, sizeof(CWaterLevel::WaterTriangles));
    const int t = RgI(s, 0, 4); CWaterLevel::NumWaterTriangles = uint32(t);
    int32 x[3], y[3]; const int kind = RgI(s, 0, 9);
    const int x0 = RgI(s, -3300, 3300), y0 = RgI(s, -3300, 3300), w = RgI(s, 1, 400) * (Rnd(s) & 1 ? 1 : -1), h = RgI(s, 1, 400) * (Rnd(s) & 1 ? 1 : -1);
    if (kind < 6) {          // triangulo con dos vertices en la misma Y y uno en la misma X
        x[0] = x0; y[0] = y0; x[1] = x0 + w; y[1] = y0; x[2] = (Rnd(s) & 1) ? x0 : x0 + w; y[2] = y0 + h;
    } else if (kind == 6) {  // degenerado: X iguales
        x[0] = x[1] = x[2] = x0; y[0] = y0; y[1] = y0 + h; y[2] = y0 + 2 * h;
    } else if (kind == 7) {  // degenerado: Y iguales
        y[0] = y[1] = y[2] = y0; x[0] = x0; x[1] = x0 + w; x[2] = x0 + 2 * w;
    } else {                 // arbitrario (el EXE no comprueba la forma)
        for (int k = 0; k < 3; ++k) { x[k] = RgI(s, -3200, 3200); y[k] = RgI(s, -3200, 3200); }
    }
    for (int k = 2; k > 0; --k) { const int j = int(Rnd(s) % uint32(k + 1)); std::swap(x[k], x[j]); std::swap(y[k], y[j]); }
    CRenPar p[3]{ RandPar(s), RandPar(s), RandPar(s) };
    if (n > 0 && Rnd(s) % 3 == 0) { const auto& v = CWaterLevel::m_aVertices[0]; x[1] = v.x; y[1] = v.y; p[1].z = v.rp.z; }
    const uint32 flags = Rnd(s) % 8;
    c.Switch();
    reinterpret_cast<void(__cdecl*)(int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar, uint32)>(addr)(x[0], y[0], p[0], x[1], y[1], p[1], x[2], y[2], p[2], flags);
    return c.Finish(json{ {"nv", CWaterLevel::NumWaterVertices}, {"nt", CWaterLevel::NumWaterTriangles}, {"tri", PolyState(false, 6)}, {"verts", VertsState(int(CWaterLevel::NumWaterVertices) + 1)} });
}

// ---- AddWaterLevelQuad ----
inline json CaseAddQuad(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x3004);
    Ctl c(hook, cand, addr);
    HooksOff off{ "AddWaterLevelVertex" };
    const int n = RgI(s, 0, 24); FillVertices(s, n);
    CWaterLevel::NumWaterVertices = uint32(n);
    std::memset(&CWaterLevel::WaterQuads, 0xAA, sizeof(CWaterLevel::WaterQuads));
    const int q = RgI(s, 0, 5); CWaterLevel::NumWaterQuads = uint32(q);
    int32 x[4], y[4]; const int kind = RgI(s, 0, 9);
    const int x0 = RgI(s, -3300, 3300), y0 = RgI(s, -3300, 3300), w = RgI(s, 1, 500) * (Rnd(s) & 1 ? 1 : -1), h = RgI(s, 1, 500) * (Rnd(s) & 1 ? 1 : -1);
    x[0] = x0; y[0] = y0; x[1] = x0 + w; y[1] = y0; x[2] = x0 + w; y[2] = y0 + h; x[3] = x0; y[3] = y0 + h;
    if (kind == 7) { for (int k = 0; k < 4; ++k) x[k] = x0; }
    else if (kind == 8) { for (int k = 0; k < 4; ++k) y[k] = y0; }
    else if (kind == 9) {
        /* El EXE lee de la pila sin inicializar si la Y minima no aparece en exactamente 2 (ni 4) vertices (lista A de 1 o 3 elementos): fixture con dos niveles de Y, 2+2. */
        const int ya = RgI(s, -2900, 2400), yb = ya + RgI(s, 1, 500);
        for (int k = 0; k < 4; ++k) { x[k] = RgI(s, -3000, 3000); y[k] = (k < 2) ? ya : yb; }
    }
    for (int k = 3; k > 0; --k) { const int j = int(Rnd(s) % uint32(k + 1)); std::swap(x[k], x[j]); std::swap(y[k], y[j]); }
    CRenPar p[4]{ RandPar(s), RandPar(s), RandPar(s), RandPar(s) };
    if (n > 0 && Rnd(s) % 3 == 0) {
        /* Reutilizar el vertice 0 (deduplicacion) solo si la Y minima sigue apareciendo en exactamente 2 vertices (o el quad se descarta por Y/X iguales): si no, el EXE lee la pila sin inicializar (listas A/B de 1 o 3 elementos). */
        const auto& v = CWaterLevel::m_aVertices[0];
        /* AddWaterLevelVertex fija cada Y <= -3000 / >= 3000 en el limite ANTES de que el quad compare las Y minimas: el recuento debe hacerse con las Y ya recortadas (casos 548 y 997 de la corrida 20261015-200341 tenian 3+1 tras recortar). */
        const auto clampY = [](int32 value) { return value <= -3000 ? -3000 : (value >= 3000 ? 3000 : value); };
        int32 ty[4]{ clampY(y[0]), clampY(v.y), clampY(y[2]), clampY(y[3]) };
        const int32 mn = std::min(std::min(ty[0], ty[1]), std::min(ty[2], ty[3]));
        const int cnt = (ty[0] == mn) + (ty[1] == mn) + (ty[2] == mn) + (ty[3] == mn);
        if (cnt == 2 || cnt == 4) { x[1] = v.x; y[1] = v.y; p[1].z = v.rp.z; }
    }
    const uint32 flags = Rnd(s) % 8;
    c.Switch();
    reinterpret_cast<void(__cdecl*)(int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar, uint32)>(addr)(x[0], y[0], p[0], x[1], y[1], p[1], x[2], y[2], p[2], x[3], y[3], p[3], flags);
    return c.Finish(json{ {"nv", CWaterLevel::NumWaterVertices}, {"nq", CWaterLevel::NumWaterQuads}, {"quads", PolyState(true, 301)}, {"verts", VertsState(int(CWaterLevel::NumWaterVertices) + 1)} });
}

// ---- BlockHit ----
inline json CaseBlockHit(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x3005);
    Ctl c(hook, cand, addr);
    std::memset(&CWaterLevel::m_BlocksToBeRenderedOutsideWorldX, 0x7B, sizeof(CWaterLevel::m_BlocksToBeRenderedOutsideWorldX));
    std::memset(&CWaterLevel::m_BlocksToBeRenderedOutsideWorldY, 0x7C, sizeof(CWaterLevel::m_BlocksToBeRenderedOutsideWorldY));
    static constexpr uint32 counts[]{ 0, 1, 5, 68, 69, 70, 71, 72, 3 };
    CWaterLevel::m_NumBlocksOutsideWorldToBeRendered = (Rnd(s) & 1) ? counts[Rnd(s) % std::size(counts)] : Rnd(s) % 73;
    StaticRef<int32>(0xB72914) = int32(Rnd(s) % 3 == 0 ? RgI(s, 1, 3) : 0);
    const int32 bx = RgI(s, -3, 15), by = RgI(s, -3, 15);
    Iso iso{ WL_SITE(0x6E5810, MarkQuadsAndPolysToBeRendered, StMark) };
    c.Switch();
    reinterpret_cast<void(__cdecl*)(int32, int32)>(addr)(bx, by);
    return c.Finish(json{ {"n", CWaterLevel::m_NumBlocksOutsideWorldToBeRendered},
        {"x", Fnv(&CWaterLevel::m_BlocksToBeRenderedOutsideWorldX, sizeof(CWaterLevel::m_BlocksToBeRenderedOutsideWorldX))},
        {"y", Fnv(&CWaterLevel::m_BlocksToBeRenderedOutsideWorldY, sizeof(CWaterLevel::m_BlocksToBeRenderedOutsideWorldY))} });
}

// ---- CalculateWavesOnlyForCoordinate ----
inline json CaseWaves(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x3006);
    Ctl c(hook, cand, addr);
    StaticRef<float>(0xC812E8) = (Rnd(s) % 5) ? float(Rnd(s) % 33) * 0.0625f : 0.f;     // Wavyness
    StaticRef<float>(0xC812E0) = float(Rnd(s) % 17) * 0.0625f;                          // SunGlare
    StaticRef<uint32>(0xB7CB84) = Rnd(s);
    StaticRef<uint32>(0xC228A4) = (Rnd(s) & 1) ? Rnd(s) : StaticRef<uint32>(0xB7CB84) - Rnd(s) % 100000;
    static constexpr int32 xs[]{ 0, 1, -1, 2, -2, 15, 16, -17, 1000, -1000, 5999, -6000, 123456, -123457, 70001 };
    int32 x = (Rnd(s) & 1) ? xs[Rnd(s) % std::size(xs)] : int32(Rnd(s) % 8001) - 4000;
    int32 y = (Rnd(s) & 1) ? xs[Rnd(s) % std::size(xs)] : int32(Rnd(s) % 8001) - 4000;
    if (i % 257 == 5) x = INT32_MIN;
    const float big = (Rnd(s) % 4) ? float(Rnd(s) % 25) * 0.125f : PickNear(s), smallW = (Rnd(s) % 4) ? float(Rnd(s) % 25) * 0.125f : PickNear(s);
    float out = PickNear(s), col = 1234.5f, glare = -77.25f; CVector n{ PickNear(s), PickNear(s), PickNear(s) };
    c.Switch();
    reinterpret_cast<void(__cdecl*)(int32, int32, float, float, float*, float*, float*, CVector*)>(addr)(x, y, big, smallW, &out, &col, &glare, &n);
    return c.Finish(json{ {"out", Bits(out)}, {"col", Bits(col)}, {"glare", Bits(glare)}, {"n", json::array({ Bits(n.x), Bits(n.y), Bits(n.z) })} });
}

// ---- GetWaterLevel(float, float, float, float&, uint8, CVector*) ----
inline json CaseGetLevel(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x3007);
    Ctl c(hook, cand, addr);
    nwRet = Rnd(s) % 5 != 0;
    static constexpr float deltas[]{ -5.f, 0.f, 2.9f, 3.f, 3.0001f, 10.f, 3.5f };
    const float x = float(RgI(s, -3000, 3000)) * 0.5f, y = float(RgI(s, -3000, 3000)) * 0.5f, z = float(RgI(s, -40, 40)) * 0.75f;
    nwLevel = z + deltas[Rnd(s) % std::size(deltas)];
    nwBig = (Rnd(s) & 1) ? 0.25f : 0.f; nwSmall = (Rnd(s) & 1) ? 0.125f : 0.f;
    static constexpr uint8 touch[]{ 0, 1, 2, 0xFF };
    const uint8 touching = touch[Rnd(s) % 4];
    float level = 55.5f; CVector nrm{ 9.f, 8.f, 7.f }; const bool wantNormal = Rnd(s) & 1;
    Iso iso{ WL_SITE(0x6E8580, GetWaterLevelNoWaves, StNoWaves), WL_SITE(0x6E81E0, AddWaveToResult, StAddWave) };
    c.Switch();
    const bool ret = reinterpret_cast<bool(__cdecl*)(float, float, float, float*, uint8, CVector*)>(addr)(x, y, z, &level, touching, wantNormal ? &nrm : nullptr);
    return c.Finish(json{ {"ret", ret}, {"level", Bits(level)}, {"n", json::array({ Bits(nrm.x), Bits(nrm.y), Bits(nrm.z) })} });
}

// ---- MarkQuadsAndPolysToBeRendered ----
inline void FillPolyWorld(uint32& s) {
    for (int k = 0; k < 144; ++k) {
        const uint32 type = Rnd(s) % 4; uint32 id = 0;
        if (type == 1) id = Rnd(s) % 16; else if (type == 2) id = Rnd(s) % 6; else if (type == 3) id = Rnd(s) % 60;
        reinterpret_cast<uint16*>(&CWaterLevel::m_BlockPolyInfo)[k] = uint16((type << 14) | id);
    }
    auto* cb = reinterpret_cast<uint16*>(&CWaterLevel::m_PolyCombos);
    for (int k = 0; k < 700; ++k) {
        const uint32 r = Rnd(s) % 100;
        if (k >= 696 || r < 15) cb[k] = 0;
        else if (r < 30) cb[k] = uint16((3u << 14) | (Rnd(s) % 10));
        else if (r < 65) cb[k] = uint16((1u << 14) | (Rnd(s) % 16));
        else cb[k] = uint16((2u << 14) | (Rnd(s) % 6));
    }
    std::memset(&CWaterLevel::WaterQuads, 0xAA, sizeof(CWaterLevel::WaterQuads));
    std::memset(&CWaterLevel::WaterTriangles, 0xAA, sizeof(CWaterLevel::WaterTriangles));
    std::memset(&CWaterLevel::m_aVertices, 0xCD, sizeof(CWaterLevel::m_aVertices));
    static constexpr float zs[]{ 0.f, 950.f, 949.99994f, 950.00006f, 951.f, -10.f, 2000.f };
    for (int k = 0; k < 40; ++k) { auto& v = CWaterLevel::m_aVertices[k]; v.x = int16(k); v.y = int16(k); v.rp = {}; v.rp.z = zs[Rnd(s) % std::size(zs)]; }
    for (int k = 0; k < 16; ++k) {
        auto& q = CWaterLevel::WaterQuads[k]; for (auto& v : q.verts) v = uint16(Rnd(s) % 40);
        auto* w = reinterpret_cast<uint8*>(&q) + 8; w[0] = uint8(Rnd(s)); w[1] = uint8(Rnd(s));
    }
    for (int k = 0; k < 6; ++k) {
        auto& t = CWaterLevel::WaterTriangles[k]; for (auto& v : t.verts) v = uint16(Rnd(s) % 40);
        auto* w = reinterpret_cast<uint8*>(&t) + 6; w[0] = uint8(Rnd(s)); w[1] = uint8(Rnd(s));
    }
}
inline json CaseMark(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x3008);
    Ctl c(hook, cand, addr);
    FillPolyWorld(s);
    const int32 bx = RgI(s, 0, 11), by = RgI(s, 0, 11); const bool interior = Rnd(s) & 1;
    c.Switch();
    reinterpret_cast<void(__cdecl*)(int32, int32, bool)>(addr)(bx, by, interior);
    return c.Finish(json{ {"quads", Fnv(&CWaterLevel::WaterQuads, sizeof(CWaterLevel::WaterQuads))}, {"tris", Fnv(&CWaterLevel::WaterTriangles, sizeof(CWaterLevel::WaterTriangles))},
        {"blocks", Fnv(&CWaterLevel::m_BlockPolyInfo, sizeof(CWaterLevel::m_BlockPolyInfo))} });
}

// ---- PreRenderWater ----
inline json CasePreRender(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x3009);
    Ctl c(hook, cand, addr); scenario = int(i);
    StaticRef<uint32>(0xB7CB4C) = (Rnd(s) % 3 == 0) ? 29u + 32u * (Rnd(s) % 100) : Rnd(s);
    static constexpr float steps[]{ 0.f, 0.25f, 0.5f, 1.f, 2.f, 10.f, 100.f, 1000.f, 3000.f };
    StaticRef<float>(0xB7CB5C) = steps[Rnd(s) % std::size(steps)];
    static constexpr float fl[]{ -1.f, -0.5f, -0.0625f, -0.001f, 0.f, 0.001f, 0.0625f, 0.5f, 1.f, 2.f };
    CWaterLevel::m_CurrentFlow = { fl[Rnd(s) % std::size(fl)], fl[Rnd(s) % std::size(fl)] };
    CWaterLevel::m_CurrentDesiredFlow = { fl[Rnd(s) % std::size(fl)], fl[Rnd(s) % std::size(fl)] };
    Iso iso{ IsoSite{ 0x53C4B0, DA(&CGame::CanSeeWaterFromCurrArea), SP(&StCanSee) }, WL_SITE(0x6E6D10, ScanThroughBlocks, StScanBlocks),
             WL_SITE(0x6E9D70, FindNearestWaterAndItsFlow, StFind), WL_SITE(0x6EAB50, HandleBeachToysStuff, StBeach) };
    c.Switch();
    reinterpret_cast<void(__cdecl*)()>(addr)();
    return c.Finish(json{ {"flow", json::array({ Bits(CWaterLevel::m_CurrentFlow.x), Bits(CWaterLevel::m_CurrentFlow.y) })} });
}

// ---- buffer temporal ----
inline void FillTemp(uint32& s, bool tri) {
    std::memset(&TempBufferVertices, 0xCD, sizeof(TempBufferVertices));
    std::memset(&aTempBufferIndices, 0xCD, sizeof(aTempBufferIndices));
    static constexpr int32 vcs[]{ 0, 3, 40, 2040, 2041, 2042, 2043, 2044, 2045, 2046, 2047 };
    static constexpr int32 ics[]{ 0, 6, 100, 4086, 4087, 4088, 4089, 4090, 4091, 4092, 4093, 4094 };
    uiTempBufferVerticesStored = (Rnd(s) % 3) ? RgI(s, 0, 1500) : vcs[Rnd(s) % std::size(vcs)];
    uiTempBufferIndicesStored = (Rnd(s) % 3) ? RgI(s, 0, 3000) : ics[Rnd(s) % std::size(ics)];
    (void)tri;
}
inline void FillWaterColors(uint32& s) {
    CWaterLevel::WaterColor = CRGBA{ uint8(Rnd(s)), uint8(Rnd(s)), uint8(Rnd(s)), uint8(Rnd(s)) };
    CWaterLevel::WaterColorTriangle = CRGBA{ uint8(Rnd(s)), uint8(Rnd(s)), uint8(Rnd(s)), uint8(Rnd(s)) };
    CWaterLevel::WaterLayerAlpha = { Rnd(s), Rnd(s) };
    static constexpr float sh[]{ 0.f, 0.3f, 0.99f, 5.25f, -3.75f, 100.5f, 1234.6f };
    CWaterLevel::TextureShiftFirstU = sh[Rnd(s) % 7]; CWaterLevel::TextureShiftFirstV = sh[Rnd(s) % 7];
    CWaterLevel::TextureShiftSecondU = sh[Rnd(s) % 7]; CWaterLevel::TextureShiftSecondV = sh[Rnd(s) % 7];
    CWaterLevel::DebugWaterColors[0].active = false; CWaterLevel::DebugWaterColors[1].active = false;
}
inline json TempState() {
    return json{ {"nv", uiTempBufferVerticesStored}, {"ni", uiTempBufferIndicesStored},
        {"verts", Fnv(&TempBufferVertices, sizeof(TempBufferVertices))}, {"idx", Fnv(&aTempBufferIndices, sizeof(aTempBufferIndices))} };
}

// ---- RenderFlatWaterRectangle_OneLayer ----
inline json CaseRectOne(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x300A);
    Ctl c(hook, cand, addr); scenario = int(i);
    FillTemp(s, false); FillWaterColors(s);
    const int x0 = RgI(s, -3000, 2800), y0 = RgI(s, -3000, 2800), w = RgI(s, 1, 300), h = RgI(s, 1, 300) * ((Rnd(s) % 4) ? 1 : -1);
    CRenPar p[4]{ RandPar(s), RandPar(s), RandPar(s), RandPar(s) }; const int32 layer = int32(Rnd(s) & 1);
    Iso iso{ RW_SITES };
    c.Switch();
    reinterpret_cast<void(__cdecl*)(int32, int32, int32, int32, CRenPar, CRenPar, CRenPar, CRenPar, int32)>(addr)(x0, x0 + w, y0, y0 + h, p[0], p[1], p[2], p[3], layer);
    return c.Finish(TempState());
}

// ---- RenderFlatWaterTriangle_OneLayer ----
inline json CaseTriOne(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x300B);
    Ctl c(hook, cand, addr); scenario = int(i);
    FillTemp(s, true); FillWaterColors(s);
    const int x0 = RgI(s, -3000, 2800), y0 = RgI(s, -3000, 2800), w = RgI(s, 1, 300), h = RgI(s, 1, 300) * ((Rnd(s) % 4) ? 1 : -1);
    const bool rightCorner = Rnd(s) & 1;
    CRenPar p[3]{ RandPar(s), RandPar(s), RandPar(s) }; const int32 layer = int32(Rnd(s) & 1);
    Iso iso{ RW_SITES };
    c.Switch();
    reinterpret_cast<void(__cdecl*)(int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar, int32)>(addr)(x0, y0, p[0], x0 + w, y0, p[1], rightCorner ? x0 + w : x0, y0 + h, p[2], layer);
    return c.Finish(TempState());
}

// ---- RenderFlatWaterRectangle ----
inline json CaseRectFlat(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x300C);
    Ctl c(hook, cand, addr);
    CWaterLevel::bSplitBigPolys = Rnd(s) % 6 != 0;
    const int x0 = RgI(s, -3000, 2000), y0 = RgI(s, -3000, 2000);
    static constexpr int ws[]{ 1, 40, 167, 168, 169, 170, 336, 337, 500, 1000 };
    const int w = ws[Rnd(s) % std::size(ws)], h = (Rnd(s) % 8 == 0) ? -ws[Rnd(s) % std::size(ws)] : ws[Rnd(s) % std::size(ws)];
    CRenPar p[4]{ RandPar(s), RandPar(s), RandPar(s), RandPar(s) };
    Iso iso{ WL_SITE(0x6E9940, RenderFlatWaterRectangle_OneLayer, StRectOne) };
    c.Switch();
    reinterpret_cast<void(__cdecl*)(int32, int32, int32, int32, CRenPar, CRenPar, CRenPar, CRenPar)>(addr)(x0, x0 + w, y0, y0 + h, p[0], p[1], p[2], p[3]);
    return c.Finish(json::object());
}

// ---- RenderFlatWaterTriangle ----
inline json CaseTriFlat(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x300D);
    Ctl c(hook, cand, addr);
    CWaterLevel::bSplitBigPolys = Rnd(s) % 5 != 0;
    const int x0 = RgI(s, -3000, 2000), y0 = RgI(s, -3000, 2000);
    static constexpr int ws[]{ 1, 40, 167, 168, 169, 170, 336, 500 };
    const int w = ws[Rnd(s) % std::size(ws)], h = ws[Rnd(s) % std::size(ws)] * ((Rnd(s) & 1) ? 1 : -1);
    CRenPar p[3]{ RandPar(s), RandPar(s), RandPar(s) };
    Iso iso{ WL_SITE(0x6E8ED0, RenderFlatWaterTriangle_OneLayer, StTriOne), WL_SITE(0x6ECF00, SplitWaterTriangleAlongXLine, StTriSplitX) };
    c.Switch();
    reinterpret_cast<void(__cdecl*)(int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar)>(addr)(x0, y0, p[0], x0 + w, y0, p[1], (Rnd(s) & 1) ? x0 : x0 + w, y0 + h, p[2]);
    return c.Finish(json::object());
}

// ---- RenderWakeSegment ----
inline json CaseWake(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x300E);
    Ctl c(hook, cand, addr); scenario = int(i);
    FillTemp(s, false);
    StaticRef<uint32>(0xB7CB84) = Rnd(s);
    StaticRef<float>(0xC812EC) = float(Rnd(s) % 33) * 0.03125f;                // WindClipped
    StaticRef<float>(0xC228D4) = (Rnd(s) & 1) ? 0.f : float(int(Rnd(s) % 201) - 100) * 0.25f;
    StaticRef<float>(0xC228D8) = (Rnd(s) & 1) ? 0.f : float(int(Rnd(s) % 201) - 100) * 0.25f;
    auto V = [&]() { return CVector2D{ float(int(Rnd(s) % 12001) - 6000) * 0.125f, float(int(Rnd(s) % 12001) - 6000) * 0.125f }; };
    const CVector2D a = V(), b = V(), cc = V(), d = V();
    const float wa = 1.f, wb = 2.f, alphaA = float(Rnd(s) % 300) * 0.9f, alphaB = float(Rnd(s) % 300) * 0.9f, wakeZ = float(int(Rnd(s) % 81) - 40) * 0.5f;
    Iso iso{ RW_SITES };
    c.Switch();
    reinterpret_cast<void(__cdecl*)(const CVector2D&, const CVector2D&, const CVector2D&, const CVector2D&, const float&, const float&, const float&, const float&, const float&)>(addr)(a, b, cc, d, wa, wb, alphaA, alphaB, wakeZ);
    return c.Finish(TempState());
}

// ---- rango de camara aleatorio ----
inline void SetRange(uint32& s) {
    const int minX = 2 * RgI(s, -300, 300), minY = 2 * RgI(s, -300, 300);
    CWaterLevel::CameraRangeMinX = minX; CWaterLevel::CameraRangeMaxX = minX + 2 * RgI(s, 10, 60);
    CWaterLevel::CameraRangeMinY = minY; CWaterLevel::CameraRangeMaxY = minY + 2 * RgI(s, 10, 60);
}
inline int NearRange(uint32& s, int lo, int hi) {   // coordenada cerca de los limites del rango o aleatoria
    static constexpr int d[]{ -1, 0, 1, -60, 60, -300, 300, 5 };
    return (Rnd(s) % 3 == 0) ? RgI(s, lo - 400, hi + 400) : ((Rnd(s) & 1) ? lo : hi) + d[Rnd(s) % std::size(d)];
}

// ---- RenderWaterRectangle ----
inline json CaseRectRender(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x300F);
    Ctl c(hook, cand, addr);
    SetRange(s);
    const int x0 = NearRange(s, CWaterLevel::CameraRangeMinX, CWaterLevel::CameraRangeMaxX), y0 = NearRange(s, CWaterLevel::CameraRangeMinY, CWaterLevel::CameraRangeMaxY);
    const int w = RgI(s, 1, 200), h = RgI(s, 1, 200) * ((Rnd(s) % 5) ? 1 : -1);
    CRenPar p[4]{ RandPar(s), RandPar(s), RandPar(s), RandPar(s) };
    Iso iso{ WL_SITE(0x6EBEC0, RenderFlatWaterRectangle, StRectFlat), WL_SITE(0x6E73A0, SplitWaterRectangleAlongXLine, StRectSplitX),
             WL_SITE(0x6EB810, RenderHighDetailWaterRectangle, StRectHigh), WL_SITE(0x6EA9F0, SetUpWaterFog, StFog) };
    c.Switch();
    reinterpret_cast<void(__cdecl*)(int32, int32, int32, int32, CRenPar, CRenPar, CRenPar, CRenPar)>(addr)(x0, x0 + w, y0, y0 + h, p[0], p[1], p[2], p[3]);
    return c.Finish(json::object());
}

// ---- RenderWaterTriangle ----
inline json CaseTriRender(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x3010);
    Ctl c(hook, cand, addr);
    SetRange(s);
    const int x0 = NearRange(s, CWaterLevel::CameraRangeMinX, CWaterLevel::CameraRangeMaxX), y0 = NearRange(s, CWaterLevel::CameraRangeMinY, CWaterLevel::CameraRangeMaxY);
    const int w = RgI(s, 1, 200), h = RgI(s, 1, 200) * ((Rnd(s) & 1) ? 1 : -1);
    CRenPar p[3]{ RandPar(s), RandPar(s), RandPar(s) };
    Iso iso{ WL_SITE(0x6EE080, RenderFlatWaterTriangle, StTriFlat), WL_SITE(0x6ECF00, SplitWaterTriangleAlongXLine, StTriSplitX),
             WL_SITE(0x6EE5A0, SplitWaterTriangleAlongYLine, StTriSplitY), WL_SITE(0x6EDDC0, RenderHighDetailWaterTriangle, StTriHigh) };
    c.Switch();
    reinterpret_cast<void(__cdecl*)(int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar)>(addr)(x0, y0, p[0], x0 + w, y0, p[1], (Rnd(s) & 1) ? x0 : x0 + w, y0 + h, p[2]);
    return c.Finish(json::object());
}

// ---- ScanThroughBlocks ----
inline json CaseScan(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x3011);
    Ctl c(hook, cand, addr);
    static Buf<0x100> cam; static Buf<0x100> frame;
    cam.Fill(0xCD); frame.Fill(0xCD);
    auto F = [&]() { return float(int(Rnd(s) % 4001) - 2000) * 0.0078125f; };
    At<float>(cam.p(), 0x68) = 0.5f + float(Rnd(s) % 100) * 0.01f; At<float>(cam.p(), 0x6C) = 0.4f + float(Rnd(s) % 100) * 0.01f;
    At<float>(cam.p(), 0x84) = (Rnd(s) % 4) ? float(RgI(s, 100, 1500)) + F() : float(RgI(s, 1, 99));
    At<void*>(cam.p(), 0x04) = frame.p();
    uint8* m = frame.p() + 0x10;                          // matriz de la trama (RwMatrix: right, up, at, pos)
    for (size_t off : { 0x00u, 0x04u, 0x08u, 0x10u, 0x14u, 0x18u, 0x20u, 0x24u, 0x28u }) At<float>(m, off) = ((off == 0x00u || off == 0x14u || off == 0x28u) ? 1.f : 0.f) + F() * 0.25f;
    At<float>(m, 0x30) = float(int(Rnd(s) % 6001) - 3000); At<float>(m, 0x34) = float(int(Rnd(s) % 6001) - 3000); At<float>(m, 0x38) = float(int(Rnd(s) % 100));
    StaticRef<void*>(0xB6F97C) = cam.p();
    CWaterLevel::m_NumBlocksOutsideWorldToBeRendered = Rnd(s);
    Iso iso{ IsoSite{ 0x72CAE0, DA(&CWorldScan::ScanWorld), SP(&StScanWorld) } };
    c.Switch();
    reinterpret_cast<void(__cdecl*)()>(addr)();
    return c.Finish(json{ {"n", CWaterLevel::m_NumBlocksOutsideWorldToBeRendered} });
}

// ---- SetCameraRange ----
inline json CaseCamRange(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x3012);
    Ctl c(hook, cand, addr);
    static Buf<0x60> mat; mat.Fill(0xCD);
    const bool hasMat = Rnd(s) & 1;
    auto P3 = [&]() { return (Rnd(s) % 3) ? float(int(Rnd(s) % 12001) - 6000) * 0.1337f : float(int(Rnd(s) % 41) - 20) * 0.5f; };
    const float px = P3(), py = P3(), pz = P3();
    if (hasMat) { At<float>(mat.p(), 0x30) = px; At<float>(mat.p(), 0x34) = py; At<float>(mat.p(), 0x38) = pz; StaticRef<void*>(0xB6F03C) = mat.p(); }
    else { StaticRef<void*>(0xB6F03C) = nullptr; StaticRef<float>(0xB6F02C) = px; StaticRef<float>(0xB6F030) = py; StaticRef<float>(0xB6F034) = pz; }
    static constexpr int dists[]{ 48, 0, 1, 2, 47, 100, 3000 };
    CWaterLevel::DETAILEDWATERDIST = dists[Rnd(s) % std::size(dists)];
    CWaterLevel::DontUpdateCameraRange = false;
    CWaterLevel::CameraRangeMinX = 111; CWaterLevel::CameraRangeMaxX = 222; CWaterLevel::CameraRangeMinY = 333; CWaterLevel::CameraRangeMaxY = 444;
    c.Switch();
    reinterpret_cast<void(__cdecl*)()>(addr)();
    return c.Finish(json{ {"r", json::array({ CWaterLevel::CameraRangeMinX, CWaterLevel::CameraRangeMaxX, CWaterLevel::CameraRangeMinY, CWaterLevel::CameraRangeMaxY })} });
}

// ---- SetUpWaterFog ----
inline json CaseFog(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x3013);
    Ctl c(hook, cand, addr);
    CWaterLevel::m_bWaterFog = Rnd(s) % 8 != 0;
    static constexpr uint32 idxs[]{ 0, 1, 5, 68, 69, 70, 71 };
    CWaterLevel::gWaterFogIndex = (Rnd(s) & 1) ? idxs[Rnd(s) % std::size(idxs)] : Rnd(s) % 72;
    std::memset(&CWaterLevel::ms_WaterFog, 0xCD, sizeof(CWaterLevel::ms_WaterFog));
    static constexpr float hs[]{ 0.f, 3.f, 10.f, 50.f, -4.f };
    CWaterLevel::m_fWaterFogHeight = hs[Rnd(s) % std::size(hs)];
    CWaterLevel::gbPlayerIsInsideWaterFog = Rnd(s) & 1;
    nwRet = Rnd(s) % 4 != 0; nwLevel = float(int(Rnd(s) % 41) - 20) * 0.5f; nwBig = (Rnd(s) % 3) ? 0.f : 0.5f; nwSmall = (Rnd(s) % 3) ? 0.f : 0.25f;
    const int x0 = RgI(s, -50, 50) * 2, y0 = RgI(s, -50, 50) * 2, w = RgI(s, 1, 60) * 2, h = RgI(s, 1, 60) * 2;
    fpCoords = CVector{ float(x0 + RgI(s, -10, w + 10)), float(y0 + RgI(s, -10, h + 10)), float(RgI(s, -20, 40)) * 0.5f };
    Iso iso{ WL_SITE(0x6E8580, GetWaterLevelNoWaves, StNoWaves), IsoSite{ 0x56E010, DA(&FindPlayerCoors), SP(&StPlayerCoors) } };
    c.Switch();
    reinterpret_cast<void(__cdecl*)(int32, int32, int32, int32)>(addr)(x0, y0, x0 + w, y0 + h);
    return c.Finish(json{ {"idx", CWaterLevel::gWaterFogIndex}, {"inside", CWaterLevel::gbPlayerIsInsideWaterFog}, {"fog", Fnv(&CWaterLevel::ms_WaterFog, sizeof(CWaterLevel::ms_WaterFog))} });
}

// ---- SplitWaterRectangleAlongXLine ----
inline json CaseRectSplitX(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x3014);
    Ctl c(hook, cand, addr);
    const int x0 = RgI(s, -3000, 2000), w = RgI(s, 2, 500), y0 = RgI(s, -3000, 2000), h = RgI(s, 1, 400) * ((Rnd(s) % 4) ? 1 : -1);
    const int sp = x0 + RgI(s, 1, w - 1);
    CRenPar p[4]{ RandPar(s), RandPar(s), RandPar(s), RandPar(s) };
    Iso iso{ WL_SITE(0x6EC5D0, RenderWaterRectangle, StRectRender) };
    c.Switch();
    reinterpret_cast<void(__cdecl*)(int32, int32, int32, int32, int32, CRenPar, CRenPar, CRenPar, CRenPar)>(addr)(sp, x0, x0 + w, y0, y0 + h, p[0], p[1], p[2], p[3]);
    return c.Finish(json::object());
}

// ---- SplitWaterRectangleAlongYLine ----
inline json CaseRectSplitY(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x3015);
    Ctl c(hook, cand, addr);
    const int x0 = RgI(s, -3000, 2000), w = RgI(s, 1, 500), y0 = RgI(s, -3000, 2000), h = RgI(s, 2, 400) * ((Rnd(s) % 3) ? 1 : -1);
    const int sp = y0 + ((h > 0) ? RgI(s, 1, h - 1) : -RgI(s, 1, -h - 1));
    CRenPar p[4]{ RandPar(s), RandPar(s), RandPar(s), RandPar(s) };
    Iso iso{ WL_SITE(0x6EC5D0, RenderWaterRectangle, StRectRender) };
    c.Switch();
    reinterpret_cast<void(__cdecl*)(int32, int32, int32, int32, int32, CRenPar, CRenPar, CRenPar, CRenPar)>(addr)(sp, x0, x0 + w, y0, y0 + h, p[0], p[1], p[2], p[3]);
    return c.Finish(json::object());
}

// ---- SplitWaterTriangleAlongXLine ----
inline json CaseTriSplitX(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x3016);
    Ctl c(hook, cand, addr);
    const int x0 = RgI(s, -3000, 2000), w = RgI(s, 2, 500), y0 = RgI(s, -3000, 2000), h = RgI(s, 1, 400) * ((Rnd(s) & 1) ? 1 : -1);
    const int sp = x0 + RgI(s, 1, w - 1);
    const bool left = Rnd(s) & 1;                              // vertice 3 bajo X1 (true) o bajo X2 (false)
    CRenPar p[3]{ RandPar(s), RandPar(s), RandPar(s) };
    Iso iso{ WL_SITE(0x6EE240, RenderWaterTriangle, StTriRender), WL_SITE(0x6EC5D0, RenderWaterRectangle, StRectRender) };
    c.Switch();
    reinterpret_cast<void(__cdecl*)(int32, int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar)>(addr)(sp, x0, y0, p[0], x0 + w, y0, p[1], left ? x0 : x0 + w, y0 + h, p[2]);
    return c.Finish(json::object());
}

// ---------------------------------------------------------------------------------------------------------------------
inline json Run(size_t f, unsigned i, bool cand, Hook* hook) {
    const auto a = fns[f].address;
    switch (f) {
    case 0:  return CaseShutdown(i, cand, hook, a);
    case 1:  return CaseAddVertex(i, cand, hook, a);
    case 2:  return CaseAddTriangle(i, cand, hook, a);
    case 3:  return CaseAddQuad(i, cand, hook, a);
    case 4:  return CaseBlockHit(i, cand, hook, a);
    case 5:  return CaseWaves(i, cand, hook, a);
    case 6:  return CaseGetLevel(i, cand, hook, a);
    case 7:  return CaseMark(i, cand, hook, a);
    case 8:  return CasePreRender(i, cand, hook, a);
    case 9:  return CaseRectFlat(i, cand, hook, a);
    case 10: return CaseRectOne(i, cand, hook, a);
    case 11: return CaseTriFlat(i, cand, hook, a);
    case 12: return CaseTriOne(i, cand, hook, a);
    case 13: return CaseWake(i, cand, hook, a);
    case 14: return CaseRectRender(i, cand, hook, a);
    case 15: return CaseTriRender(i, cand, hook, a);
    case 16: return CaseScan(i, cand, hook, a);
    case 17: return CaseCamRange(i, cand, hook, a);
    case 18: return CaseFog(i, cand, hook, a);
    case 19: return CaseRectSplitX(i, cand, hook, a);
    case 20: return CaseRectSplitY(i, cand, hook, a);
    default: return CaseTriSplitX(i, cand, hook, a);
    }
}
#undef WL_SITE
#undef RW_SITES
}}
