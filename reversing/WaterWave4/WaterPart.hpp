#pragma once
// WaterWave4 - CWaterLevel: las 4 funciones pendientes (AddWaterLevelQuad, RenderBoatWakes, SplitWaterTriangleAlongYLine, WaterLevelInitialise).
// Original (hook desactivado: codigo del EXE) frente a candidato (hook activo: fuente) sobre estado falso. Los colaboradores se doblan con
// EntryPatch locales (Iso) que solo viven durante cada ejecucion (direccion del EXE + simbolo de la DLL + cuerpo) y que trazan sus argumentos
// crudos; nada dibuja ni abre ficheros reales. Etiquetas de traza 3000+ (copiadas de GfxWave2/WaterPart.hpp) y 4000+ (nuevas).
// Copiado y adaptado de reversing/GfxWave2/WaterPart.hpp (sin editar GfxWave2).
#include "WaterCommon.hpp"
#include "WaterLevel.h"
#include "Renderer.h"
#include "Camera.h"
#include "Timer.h"
#include "Weather.h"
#include "Game.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "TxdStore.h"
#include "Entity/Vehicle/Boat.h"
#include "Collision/ColModel.h"

namespace WaterWave4Live { namespace Wl {
using ReversingProbe::EntryPatch;

// ---------------------------------------------------------------------------------------------------------------------
// Tabla
// ---------------------------------------------------------------------------------------------------------------------
inline const std::array<Fn, 4> fns{{
    {"CWaterLevel","AddWaterLevelQuad",0x6E7EF0,0,-1,1024},
    {"CWaterLevel","RenderBoatWakes",0x6ED9A0,0,-1,512},
    {"CWaterLevel","SplitWaterTriangleAlongYLine",0x6EE5A0,0,-1,1024},
    {"CWaterLevel","WaterLevelInitialise",0x6EAE80,0,-1,256},
}};

// ---------------------------------------------------------------------------------------------------------------------
// Utilidades (de GfxWave2/WaterPart.hpp)
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
inline uint32 Fnv32(const char* s) { return uint32(Fnv(s, std::strlen(s))); }

// Unidad de control x87: 24 bits durante la llamada (como el motor), nrec a cero.
struct Ctl {
    Hook* h; bool cand; uintptr_t addr; unsigned savedFp; bool sw{};
    Ctl(Hook* hook, bool c, uintptr_t a) : h(hook), cand(c), addr(a) { savedFp = _controlfp(0, 0); _controlfp(_PC_24, _MCW_PC); nrec = 0; }
    void Switch() { h->State(cand); sw = ReversingProbe::JumpsToDll(addr) == cand; }   // despues de State(), no antes
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
    ~Iso() { while (!p.empty()) p.pop_back(); }   // orden inverso
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
constexpr uintptr_t ENGINE = 0xC97B24;
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
        { 0xC228A0, 0x24 },                            // m_nWaterConfiguration, tiempo, rasters y texturas (0xC228A0..0xC228C3)
        { 0xC1F950, 0x10 },                            // CameraRange*
        { 0xC21168, 0x20 },                            // colores y desplazamientos de textura
        { 0xC228D4, 0x18 },                            // desplazamiento de UV de estelas, bandera de niebla, indice
        { 0xC4B950, 8 },                               // contadores del buffer temporal
        { 0xC4B958, 8192 },                            // aTempBufferIndices
        { 0xC4D958, sizeof(TempVertexBuffer) },        // TempBufferVertices
        { 0xC27994, 0x10 },                            // lista de barcos con estela (apFrameWakeGeneratingBoats)
        { 0xB6F02C, 0x14 },                            // posicion y puntero a matriz de TheCamera
        { *reinterpret_cast<uintptr_t*>(ENGINE) + 0x20, 4 },   // puntero RwRenderStateSet del motor
    };
}

// ---------------------------------------------------------------------------------------------------------------------
// Colaboradores doblados (trazas crudas)
// ---------------------------------------------------------------------------------------------------------------------
inline int scenario{};
inline void __cdecl StTriRender(int32 a, int32 b, CRenPar p, int32 c, int32 d, CRenPar q, int32 e, int32 f, CRenPar r) { if (!Mine()) return; Push(3013, { U(a), U(b), U(c), U(d), U(e), U(f), Hp({ p, q, r }) }); }
inline void __cdecl StRectRender(int32 x0, int32 x1, int32 y0, int32 y1, CRenPar p, CRenPar q, CRenPar r, CRenPar t) { if (!Mine()) return; Push(3005, { U(x0), U(x1), U(y0), U(y1), Hp({ p, q, r, t }) }); }
// Mismos dobles de RwIm3D*/LittleTest que GfxWave2
inline void __cdecl StLittle() { if (!Mine()) return; Push(3023); }
inline void* __cdecl StTransform(RwIm3DVertex* v, RwUInt32 n, RwMatrix* m, RwUInt32 flags) {
    if (!Mine()) return nullptr;
    const bool ok = scenario % 5 != 0; Push(3024, { P(v), n, P(m), flags, uint32(Fnv(v, n * sizeof(RwIm3DVertex))), ok }); return ok ? v : nullptr;
}
inline RwBool __cdecl StRenderIdx(RwPrimitiveType t, RwImVertexIndex* idx, RwInt32 n) { if (!Mine()) return TRUE; Push(3025, { uint32(t), uint32(n), uint32(Fnv(idx, size_t(n) * sizeof(RwImVertexIndex))) }); return TRUE; }
inline RwBool __cdecl StEnd() { if (!Mine()) return TRUE; Push(3026); return TRUE; }
// RwRenderStateSet: el EXE la llama por el puntero [RwEngineInstance + 0x20] (cdecl, 2 argumentos); la DLL tambien.
inline RwBool __cdecl StRenderState(RwRenderState st, void* v) { if (!Mine()) return TRUE; Push(3045, { uint32(st), P(v) }); return TRUE; }
struct RsGuard {
    void** slot; void* saved;
    RsGuard() { slot = reinterpret_cast<void**>(*reinterpret_cast<uintptr_t*>(ENGINE) + 0x20); saved = *slot; *slot = reinterpret_cast<void*>(&StRenderState); }
    ~RsGuard() { *slot = saved; }
};

#define WL_SITE(exe, fn, st) IsoSite{ exe, DA(&CWaterLevel::fn), SP(&st) }
#define RW_SITES IsoSite{ 0x541330, DA(&LittleTest), SP(&StLittle) }, IsoSite{ 0x7EF450, 0, SP(&StTransform) }, IsoSite{ 0x7EF550, 0, SP(&StRenderIdx) }, IsoSite{ 0x7EF520, 0, SP(&StEnd) }

// ---------------------------------------------------------------------------------------------------------------------
// Utilidades de casos
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
    for (int k = 0; k < upto; ++k) { const auto& v = CWaterLevel::m_aVertices[k]; const uint32 a[2]{ uint32(uint16(v.x)), uint32(uint16(v.y)) }; h = Fnv(a, sizeof(a), h); h = HpOne(h, v.rp); }
    const auto* base = reinterpret_cast<const uint8*>(&CWaterLevel::m_aVertices);
    return json{ {"fields", h}, {"rest", Fnv(base + size_t(upto) * 20, size_t(1021 - upto) * 20)} };
}
inline json PolyState(bool quad, int count) {
    json r = json::array();
    const auto* base = quad ? reinterpret_cast<const uint8*>(&CWaterLevel::WaterQuads) : reinterpret_cast<const uint8*>(&CWaterLevel::WaterTriangles);
    const size_t sz = quad ? 10 : 8;
    r.push_back(Fnv(base, sz * size_t(count)));
    return r;
}
inline json TempState() {
    return json{ {"nv", uiTempBufferVerticesStored}, {"ni", uiTempBufferIndicesStored},
        {"verts", Fnv(&TempBufferVertices, sizeof(TempBufferVertices))}, {"idx", Fnv(&aTempBufferIndices, sizeof(aTempBufferIndices))} };
}
inline void FillTemp(uint32& s) {
    std::memset(&TempBufferVertices, 0xCD, sizeof(TempBufferVertices));
    std::memset(&aTempBufferIndices, 0xCD, sizeof(aTempBufferIndices));
    static constexpr int32 vcs[]{ 0, 3, 40, 2040, 2041, 2042, 2043, 2044, 2045, 2046, 2047 };
    static constexpr int32 ics[]{ 0, 6, 100, 4086, 4087, 4088, 4089, 4090, 4091, 4092, 4093, 4094 };
    uiTempBufferVerticesStored = (Rnd(s) % 3) ? RgI(s, 0, 1500) : vcs[Rnd(s) % std::size(vcs)];
    uiTempBufferIndicesStored = (Rnd(s) % 3) ? RgI(s, 0, 3000) : ics[Rnd(s) % std::size(ics)];
}

// =====================================================================================================================
// AddWaterLevelQuad (0x6E7EF0)
// =====================================================================================================================
// Un vertice se guarda con la Y fijada a +-3000 si Y <= -3000 o Y >= 3000 (AddWaterLevelVertex 0x6E5A40: fcomp + test ah,41h / test ah,1).
inline int32 StoredY(int32 y) { return y <= -3000 ? -3000 : (y >= 3000 ? 3000 : y); }
// El EXE reparte los 4 vertices en la lista A (los de Y minima) y la B (el resto) con A en [esp+1Ch..] y B en [esp+24h..] (se solapan)
// y lee A[0],A[1],B[0],B[1]: solo es determinista si la Y minima GUARDADA (tras el limite de +-3000) aparece en exactamente 2 o 4 vertices
// (con 1 o 3 el EXE lee pila sin inicializar: A[1] o B[1]). Se cuenta con la Y guardada, no con la de entrada: dos Y de entrada distintas
// (p. ej. -3100 y -3050) acaban ambas en -3000 y convierten un 2+2 en 3+1 (fallo de fixture en la version GfxWave2: 2 de 1024).
inline bool QuadDeterministic(const int32* y) {
    int32 mn = StoredY(y[0]);
    for (int k = 1; k < 4; ++k) mn = std::min(mn, StoredY(y[k]));
    int cnt = 0; for (int k = 0; k < 4; ++k) cnt += StoredY(y[k]) == mn;
    return cnt == 2 || cnt == 4;
}
inline json CaseAddQuad(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x4001);
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
        const int ya = RgI(s, -2900, 2400), yb = ya + RgI(s, 1, 500);
        for (int k = 0; k < 4; ++k) { x[k] = RgI(s, -3000, 3000); y[k] = (k < 2) ? ya : yb; }
    }
    for (int k = 3; k > 0; --k) { const int j = int(Rnd(s) % uint32(k + 1)); std::swap(x[k], x[j]); std::swap(y[k], y[j]); }
    CRenPar p[4]{ RandPar(s), RandPar(s), RandPar(s), RandPar(s) };
    // Deduplicacion: el vertice 1 pasa a ser el vertice 0 del array (misma x, y y z) -> AddWaterLevelVertex devuelve el existente.
    int32 sx = x[1], sy = y[1]; CRenPar sp = p[1]; bool replaced = false;
    if (n > 0 && Rnd(s) % 3 == 0) {
        const auto& v = CWaterLevel::m_aVertices[0];
        x[1] = v.x; y[1] = v.y; p[1].z = v.rp.z; replaced = true;
    }
    const auto EarlyOut = [&] { return (x[0] == x[1] && x[0] == x[2] && x[0] == x[3]) || (y[0] == y[1] && y[0] == y[2] && y[0] == y[3]); };
    if (!EarlyOut() && !QuadDeterministic(y)) {
        if (replaced) { x[1] = sx; y[1] = sy; p[1] = sp; }                     // deshacer la deduplicacion
        if (!EarlyOut() && !QuadDeterministic(y)) {                            // ultimo recurso: dos niveles de Y 2+2
            const int ya = RgI(s, -2900, 2400), yb = ya + RgI(s, 1, 500);
            y[0] = y[1] = ya; y[2] = y[3] = yb;
        }
    }
    const uint32 flags = Rnd(s) % 8;
    c.Switch();
    reinterpret_cast<void(__cdecl*)(int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar, uint32)>(addr)(x[0], y[0], p[0], x[1], y[1], p[1], x[2], y[2], p[2], x[3], y[3], p[3], flags);
    return c.Finish(json{ {"nv", CWaterLevel::NumWaterVertices}, {"nq", CWaterLevel::NumWaterQuads}, {"quads", PolyState(true, 301)}, {"verts", VertsState(int(CWaterLevel::NumWaterVertices) + 1)} });
}

// =====================================================================================================================
// SplitWaterTriangleAlongYLine (0x6EE5A0)
// =====================================================================================================================
// RenderWaterTriangle / RenderWaterRectangle se doblan (trazas 3013 / 3005 con X, Y crudos y el hash de los CRenPar, flujo incluido).
inline json CaseSplitY(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x4003);
    Ctl c(hook, cand, addr);
    const int x1 = RgI(s, -3000, 2000), w = RgI(s, 2, 500), y1 = RgI(s, -3000, 2000);
    const int hgt = RgI(s, 1, 400) * ((Rnd(s) & 1) ? 1 : -1);            // Y3 por debajo (> Y1) o por encima (< Y1) de la arista superior
    const bool left = Rnd(s) & 1;                                          // vertice 3 bajo X1 (X1 == X3) o bajo X2 (X2 == X3)
    const int x2 = x1 + w, y3 = y1 + hgt, x3 = left ? x1 : x2;
    const int y2 = (Rnd(s) % 4 == 0) ? y1 + RgI(s, -50, 50) : y1;         // el EXE solo usa Y2 para pasarlo a RenderWaterTriangle
    int sY;
    switch (Rnd(s) % 8) {
    case 0: sY = y1; break;                                                // t = 0
    case 1: sY = y3; break;                                                // t = 1
    case 2: sY = y1 + RgI(s, -400, 400); break;                            // fuera del rango (t < 0 o t > 1)
    default: sY = y1 + ((hgt > 0) ? RgI(s, 0, hgt) : -RgI(s, 0, -hgt)); break;   // dentro
    }
    CRenPar p[3]{ RandPar(s), RandPar(s), RandPar(s) };
    Iso iso{ WL_SITE(0x6EE240, RenderWaterTriangle, StTriRender), WL_SITE(0x6EC5D0, RenderWaterRectangle, StRectRender) };
    c.Switch();
    reinterpret_cast<void(__cdecl*)(int32, int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar)>(addr)(sY, x1, y1, p[0], x2, y2, p[1], x3, y3, p[2]);
    return c.Finish(json{ {"branch", json::array({ y1 < y3, x1 == x3 })} });
}

// =====================================================================================================================
// RenderBoatWakes (0x6ED9A0): pasada inlineada del EXE sobre la lista de barcos 0xC27994
// =====================================================================================================================
// Barcos falsos (CBoat: +0x14 matriz, +0x22 modelo, +0x644 numero de puntos, +0x648 coordenadas, +0x748 contadores, +0x7C8 velocidades),
// matrices falsas (+0x10 direccion adelante, +0x30 posicion), ColModel falso (maximo X en +0xC), matriz de camara falsa (+0x30).
// Doblados: CBoat::FillBoatList (0x6F2710: fija la lista), CEntity::GetColModel (0x535300), RenderWakeSegment (0x6EA260; trazas 3042-3044),
// RwRenderStateSet (puntero del motor), LittleTest y RwIm3D*.
constexpr size_t kBoatSize = 0x800;
alignas(16) inline std::array<std::array<uint8, kBoatSize>, 4> boatMem{};
alignas(16) inline std::array<std::array<uint8, 0x80>, 4> matMem{};
alignas(16) inline std::array<std::array<uint8, 0x40>, 4> colMem{};
alignas(16) inline std::array<uint8, 0x80> camMatMem{};
inline std::array<void*, 4> listedBoats{};
inline void __cdecl StFillBoats() {
    if (!Mine()) return;
    Push(3040);
    auto* arr = reinterpret_cast<uint32*>(0xC27994);
    for (int k = 0; k < 4; ++k) arr[k] = P(listedBoats[k]);
}
inline void* __fastcall StGetCol(void* self, void*) {
    if (!Mine()) return nullptr;
    for (int k = 0; k < 4; ++k) if (self == boatMem[k].data()) { Push(3041, { uint32(k) }); return colMem[k].data(); }
    Push(3041, { 0xFFFFFFFFu }); return colMem[0].data();
}
inline void __cdecl StWakeSeg(const CVector2D* a, const CVector2D* b, const CVector2D* cc, const CVector2D* d, const float* wA, const float* wB, const float* aA, const float* aB, const float* wz) {
    if (!Mine()) return;
    Push(3042, { Bits(a->x), Bits(a->y), Bits(b->x), Bits(b->y), Bits(cc->x), Bits(cc->y), Bits(d->x), Bits(d->y) });
    Push(3043, { Bits(*wA), Bits(*wB), Bits(*aA), Bits(*aB), Bits(*wz) });
}
inline json CaseBoatWakes(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x4002);
    Ctl c(hook, cand, addr); scenario = int(i);
    FillTemp(s);
    CWaterLevel::waterwakeRaster = reinterpret_cast<RwRaster*>(0x30003000 + (Rnd(s) & 0xFF0));
    // camara: con matriz (+0x30) o sin ella (posicion 0xB6F02C); a veces con NaN en X (comparaciones con NaN: dist > 50 es falso)
    const float cx = float(int(Rnd(s) % 8001) - 4000) * 0.25f, cy = float(int(Rnd(s) % 8001) - 4000) * 0.25f, cz = float(int(Rnd(s) % 101) - 50);
    const bool camNaN = Rnd(s) % 64 == 0;
    camMatMem.fill(0xCD);
    if (Rnd(s) & 1) {
        At<float>(camMatMem.data(), 0x30) = camNaN ? std::bit_cast<float>(0x7FC00000u) : cx; At<float>(camMatMem.data(), 0x34) = cy; At<float>(camMatMem.data(), 0x38) = cz;
        StaticRef<void*>(0xB6F03C) = camMatMem.data();
    } else {
        StaticRef<float>(0xB6F02C) = camNaN ? std::bit_cast<float>(0x7FC00000u) : cx; StaticRef<float>(0xB6F030) = cy; StaticRef<float>(0xB6F034) = cz;
        StaticRef<void*>(0xB6F03C) = nullptr;
    }
    static constexpr float distDX[]{ 0.f, 30.f, 0.f, 50.f, 5.f, 49.f, 0.f, 60.f, 79.f, 80.f, 0.f, 45.f, 200.f, 3.f };
    static constexpr float distDY[]{ 0.f, 40.f, 50.f, 0.f, 5.f, 0.f, 49.9f, 0.f, 0.f, 0.f, 80.f, 40.f, 100.f, 4.f };
    static constexpr uint16 counts[]{ 0, 1, 2, 3, 4, 5, 8, 16, 31, 32 };
    const int nListed = (Rnd(s) % 10 == 0) ? 0 : RgI(s, 1, 4);
    const bool gap = nListed >= 3 && Rnd(s) % 6 == 0;                       // hueco: el EXE para en el primer nulo
    listedBoats.fill(nullptr);
    for (int b = 0; b < nListed; ++b) listedBoats[b] = boatMem[b].data();
    if (gap) listedBoats[1] = nullptr;
    for (int b = 0; b < 4; ++b) {
        auto* m = boatMem[b].data(); std::memset(m, 0xCD, kBoatSize);
        auto* mat = matMem[b].data(); std::memset(mat, 0xCD, matMem[b].size());
        auto* col = colMem[b].data(); std::memset(col, 0xCD, colMem[b].size());
        At<float>(col, 0xC) = (Rnd(s) % 40 == 0) ? std::bit_cast<float>(0x7FC00000u) : float(int(Rnd(s) % 400) + 1) * 0.03125f;     // max.x de la caja
        At<void*>(m, 0x14) = mat;                                          // el EXE no comprueba que exista (lee [m+0x10]/[m+0x14] siempre)
        const float ang = float(Rnd(s) % 628) * 0.01f;
        At<float>(mat, 0x10) = (Rnd(s) % 5) ? std::cos(ang) : float(int(Rnd(s) % 7) - 3);   // adelante.x (a veces no unitaria)
        At<float>(mat, 0x14) = (Rnd(s) % 5) ? std::sin(ang) : float(int(Rnd(s) % 7) - 3);
        const int dk = int(Rnd(s) % std::size(distDX));
        const float sgnx = (Rnd(s) & 1) ? 1.f : -1.f, sgny = (Rnd(s) & 1) ? 1.f : -1.f;
        const float bx = cx + sgnx * distDX[dk], by = cy + sgny * distDY[dk];
        At<float>(mat, 0x30) = bx; At<float>(mat, 0x34) = by; At<float>(mat, 0x38) = 0.f;
        At<uint16>(m, 0x22) = (Rnd(s) % 4 == 0) ? uint16(0x1CC) : uint16(400 + Rnd(s) % 211);
        const uint16 N = counts[Rnd(s) % std::size(counts)];
        At<uint16>(m, 0x644) = N;
        float px = bx + float(int(Rnd(s) % 21) - 10), py = by + float(int(Rnd(s) % 21) - 10);
        for (int k = 0; k < 32; ++k) {
            static constexpr float steps[]{ 0.f, 1.f, 2.9f, 3.f, 3.1f, 5.f, 12.f, 12.9f, 13.f, 13.1f, 20.f, 40.f };
            float dx, dy;
            switch (Rnd(s) % 6) {
            case 0: { static constexpr float ax[]{ 3.f, 0.f, 5.f, 12.f, 0.f, 9.f, -3.f, -5.f }; static constexpr float ay[]{ 0.f, 3.f, 12.f, 5.f, 13.f, 12.f, 0.f, -12.f }; const int t = int(Rnd(s) % std::size(ax)); dx = ax[t]; dy = ay[t]; break; }
            case 1: dx = float(int(Rnd(s) % 301) - 150) * 0.1f; dy = float(int(Rnd(s) % 301) - 150) * 0.1f; break;
            case 2: dx = 0.f; dy = 0.f; break;
            case 3: dx = steps[Rnd(s) % std::size(steps)] * ((Rnd(s) & 1) ? 1.f : -1.f); dy = 0.f; break;
            case 4: dx = 0.f; dy = steps[Rnd(s) % std::size(steps)] * ((Rnd(s) & 1) ? 1.f : -1.f); break;
            default: dx = float(int(Rnd(s) % 81) - 40); dy = float(int(Rnd(s) % 81) - 40); break;
            }
            px += dx; py += dy;
            At<float>(m, 0x648 + 8 * k) = px; At<float>(m, 0x64C + 8 * k) = py;
            At<float>(m, 0x748 + 4 * k) = (Rnd(s) % 8 == 0) ? ((Rnd(s) & 1) ? 150.f : 0.f) : float(Rnd(s) % 1501) * 0.1f;
            At<uint8>(m, 0x7C8 + k) = (Rnd(s) % 8 == 0) ? uint8((Rnd(s) & 1) ? 255 : 0) : uint8(Rnd(s));
        }
    }
    Iso iso{ IsoSite{ 0x6F2710, DA(&CBoat::FillBoatList), SP(&StFillBoats) }, IsoSite{ 0x535300, DA(&CEntity::GetColModel), SP(&StGetCol) },
             WL_SITE(0x6EA260, RenderWakeSegment, StWakeSeg), RW_SITES };
    RsGuard rs;
    c.Switch();
    reinterpret_cast<void(__cdecl*)()>(addr)();
    json boats = json::array(); for (int b = 0; b < 4; ++b) boats.push_back(Fnv(boatMem[b].data(), kBoatSize));
    json r = TempState(); r["boats"] = boats;
    return c.Finish(std::move(r));
}

// =====================================================================================================================
// WaterLevelInitialise (0x6EAE80): lee DATA\water.dat -> fichero en memoria
// =====================================================================================================================
// NO se abre el fichero real: CFileMgr::OpenFile (0x538900), CFileLoader::LoadLine(FILESTREAM) (0x536F80) y CFileMgr::CloseFile (0x5389D0) se doblan
// con un fichero en memoria (lineas ya "saneadas" como las deja LoadLine: sin comas ni caracteres de control ni espacios iniciales); el sscanf del
// CRT del EXE (0x8220AD) se ejecuta de verdad en las dos ramas. AddWaterLevelQuad/Triangle (0x6E7EF0/0x6E7D40), FillQuadsAndTrianglesList (0x6E7B30),
// CTxdStore::Push/Pop/SetCurrentTxd/FindTxdSlot(const char*) (0x7316A0/0x7316B0/0x7319C0/0x731850) y RwTextureRead (0x7F3AC0) se doblan con trazas.
// Otros hilos que lleguen a OpenFile/CloseFile usan el CRT real; LoadLine de otro hilo devuelve nullptr.
alignas(16) inline std::array<uint8, 32> fakeFile{};
inline std::vector<std::string> iniLines; inline size_t iniPos{};
alignas(16) inline std::array<std::array<uint8, 0x80>, 3> texExisting{};   // texturas ya cargadas (la funcion lee su campo de raster)
alignas(16) inline std::array<std::array<uint8, 0x80>, 3> texNew{};        // las que devuelve el doble de RwTextureRead
inline FILE* __cdecl StOpen(const char* path, const char* mode) {
    if (!Mine()) return fopen(path, mode);
    Push(4010, { Fnv32(path), Fnv32(mode) }); iniPos = 0; return reinterpret_cast<FILE*>(fakeFile.data());
}
inline char* __cdecl StLoadLine(FILE* f) {
    if (!Mine() || f != reinterpret_cast<FILE*>(fakeFile.data())) return nullptr;
    if (iniPos >= iniLines.size()) { Push(4011, { 0u }); return nullptr; }
    Push(4011, { 1u + uint32(iniPos) }); return iniLines[iniPos++].data();
}
inline int __cdecl StClose(FILE* f) {
    if (!Mine() || f != reinterpret_cast<FILE*>(fakeFile.data())) return f ? fclose(f) : 0;
    Push(4012, { uint32(iniPos) }); return 0;
}
inline void __cdecl StAddQuad(int32 x1, int32 y1, CRenPar p1, int32 x2, int32 y2, CRenPar p2, int32 x3, int32 y3, CRenPar p3, int32 x4, int32 y4, CRenPar p4, uint32 flags) {
    if (!Mine()) return;
    Push(4020, { U(x1), U(y1), U(x2), U(y2), U(x3), U(y3), U(x4), U(y4) });
    Push(4021, { Hp({ p1 }), Hp({ p2 }), Hp({ p3 }), Hp({ p4 }), flags });
}
inline void __cdecl StAddTri(int32 x1, int32 y1, CRenPar p1, int32 x2, int32 y2, CRenPar p2, int32 x3, int32 y3, CRenPar p3, uint32 flags) {
    if (!Mine()) return;
    Push(4022, { U(x1), U(y1), U(x2), U(y2), U(x3), U(y3) });
    Push(4023, { Hp({ p1 }), Hp({ p2 }), Hp({ p3 }), flags });
}
inline void __cdecl StFillList() { if (!Mine()) return; Push(4030); }
inline void __cdecl StTxdPush() { if (!Mine()) return; Push(4031); }
inline void __cdecl StTxdPop() { if (!Mine()) return; Push(4032); }
inline int32 __cdecl StTxdFind(const char* name) { if (!Mine()) return -1; Push(4033, { Fnv32(name) }); return 4242; }
inline void __cdecl StTxdSet(int32 slot) { if (!Mine()) return; Push(4034, { U(slot) }); }
inline RwTexture* __cdecl StTexRead(const char* name, const char* mask) {
    if (!Mine()) return nullptr;
    Push(4035, { Fnv32(name), mask != nullptr });
    int k = 0; if (std::strcmp(name, "seabd32") == 0) k = 1; else if (std::strcmp(name, "waterwake") == 0) k = 2;
    return reinterpret_cast<RwTexture*>(texNew[k].data());
}

inline std::string F(float v) { char b[48]; std::snprintf(b, sizeof(b), "%.4f", v); return b; }
inline std::string Sp(uint32& s) { static const char* t[]{ " ", "  ", " ", "\t", "   " }; return t[Rnd(s) % 5]; }
// Un vertice de water.dat: x y z flowX flowY bigWaves smallWaves
inline std::string Vtx(uint32& s) {
    const float x = float(int(Rnd(s) % 6401) - 3200) + float(Rnd(s) % 4) * 0.25f * ((Rnd(s) & 1) ? 1.f : -1.f);
    const float y = float(int(Rnd(s) % 6401) - 3200) + float(Rnd(s) % 4) * 0.25f * ((Rnd(s) & 1) ? 1.f : -1.f);
    const float z = (Rnd(s) % 4) ? float(int(Rnd(s) % 41) - 10) + float(Rnd(s) % 2) * 0.5f : 0.f;
    const float fx = float(int(Rnd(s) % 81) - 40) * 0.0625f, fy = float(int(Rnd(s) % 81) - 40) * 0.0625f;   // flujo en [-2.5, 2.5]: *64 envuelve el byte
    const float bw = float(Rnd(s) % 13) * 0.125f, sw = float(Rnd(s) % 9) * 0.0625f;
    std::string r;
    const float vals[7]{ x, y, z, fx, fy, bw, sw };
    for (int k = 0; k < 7; ++k) { if (k) r += Sp(s); r += F(vals[k]); }
    return r;
}
inline void BuildFile(uint32& s) {
    iniLines.clear();
    const int n = RgI(s, 2, 40);
    for (int k = 0; k < n; ++k) {
        switch (Rnd(s) % 10) {
        case 0: iniLines.push_back(";comentario " + std::to_string(Rnd(s) % 100)); break;
        case 1: { static const char* t[]{ "", "*", "* fin", "p", "poligono 1 2 3", ";" }; iniLines.push_back(t[Rnd(s) % 6]); break; }
        case 2: case 3: case 4: { std::string l; for (int v = 0; v < 4; ++v) { if (v) l += Sp(s); l += Vtx(s); } if (Rnd(s) & 1) l += Sp(s) + std::to_string(Rnd(s) % 8); iniLines.push_back(l); break; }
        case 5: { std::string l; for (int v = 0; v < 4; ++v) { if (v) l += Sp(s); l += Vtx(s); } static const int fl[]{ 0, 1, 2, 3, 255, 256, 7 }; l += Sp(s) + std::to_string(fl[Rnd(s) % 7]); iniLines.push_back(l); break; }
        default: { std::string l; for (int v = 0; v < 3; ++v) { if (v) l += Sp(s); l += Vtx(s); } if (Rnd(s) & 1) l += Sp(s) + std::to_string(Rnd(s) % 8); iniLines.push_back(l); break; }
        }
    }
}
inline json CaseInit(unsigned i, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(i, 0x4004);
    Ctl c(hook, cand, addr);
    std::memset(&CWaterLevel::m_BlockPolyInfo, 0xAB, sizeof(CWaterLevel::m_BlockPolyInfo));
    CWaterLevel::NumWaterTriangles = 777; CWaterLevel::NumWaterQuads = 778; CWaterLevel::NumWaterVertices = 779; CWaterLevel::NumWaterZonePolys = 999;
    CWaterLevel::m_nWaterConfiguration = Rnd(s) & 1;                       // 0 -> "DATA\water.dat", 1 -> "DATA\water1.dat" (otro valor: el EXE usa pila sin inicializar)
    for (int k = 0; k < 3; ++k) {
        std::memset(texExisting[k].data(), 0xCD, texExisting[k].size()); std::memset(texNew[k].data(), 0xCD, texNew[k].size());
        At<void*>(texExisting[k].data(), 0) = reinterpret_cast<void*>(0x41000000 + 0x100 * k);          // raster de la textura ya cargada
        At<void*>(texNew[k].data(), 0) = reinterpret_cast<void*>(0x42000000 + 0x100 * k);               // raster de la que devuelve RwTextureRead
    }
    CWaterLevel::texWaterclear256 = (Rnd(s) & 1) ? reinterpret_cast<RwTexture*>(texExisting[0].data()) : nullptr;
    CWaterLevel::texSeabd32       = (Rnd(s) & 1) ? reinterpret_cast<RwTexture*>(texExisting[1].data()) : nullptr;
    CWaterLevel::texWaterwake     = (Rnd(s) & 1) ? reinterpret_cast<RwTexture*>(texExisting[2].data()) : nullptr;
    CWaterLevel::waterclear256Raster = reinterpret_cast<RwRaster*>(0xDEAD0001); CWaterLevel::seabd32Raster = reinterpret_cast<RwRaster*>(0xDEAD0002); CWaterLevel::waterwakeRaster = reinterpret_cast<RwRaster*>(0xDEAD0003);
    BuildFile(s);
    Iso iso{ IsoSite{ 0x538900, DA(&CFileMgr::OpenFile), SP(&StOpen) },
             IsoSite{ 0x536F80, DA(static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine)), SP(&StLoadLine) },
             IsoSite{ 0x5389D0, DA(&CFileMgr::CloseFile), SP(&StClose) },
             WL_SITE(0x6E7EF0, AddWaterLevelQuad, StAddQuad), WL_SITE(0x6E7D40, AddWaterLevelTriangle, StAddTri), WL_SITE(0x6E7B30, FillQuadsAndTrianglesList, StFillList),
             IsoSite{ 0x7316A0, DA(&CTxdStore::PushCurrentTxd), SP(&StTxdPush) }, IsoSite{ 0x7316B0, DA(&CTxdStore::PopCurrentTxd), SP(&StTxdPop) },
             IsoSite{ 0x731850, DA(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot)), SP(&StTxdFind) }, IsoSite{ 0x7319C0, DA(&CTxdStore::SetCurrentTxd), SP(&StTxdSet) },
             IsoSite{ 0x7F3AC0, 0, SP(&StTexRead) } };
    c.Switch();
    reinterpret_cast<void(__cdecl*)()>(addr)();
    return c.Finish(json{
        {"counts", json::array({ CWaterLevel::NumWaterTriangles, CWaterLevel::NumWaterQuads, CWaterLevel::NumWaterVertices, CWaterLevel::NumWaterZonePolys })},
        {"blocks", Fnv(&CWaterLevel::m_BlockPolyInfo, sizeof(CWaterLevel::m_BlockPolyInfo))},
        {"cfg", CWaterLevel::m_nWaterConfiguration},
        {"tex", json::array({ P(CWaterLevel::texWaterclear256), P(CWaterLevel::texSeabd32), P(CWaterLevel::texWaterwake) })},
        {"raster", json::array({ P(CWaterLevel::waterclear256Raster), P(CWaterLevel::seabd32Raster), P(CWaterLevel::waterwakeRaster) })},
        {"lines", iniLines.size()}, {"consumed", iniPos} });
}

// Comprobacion de la disposicion de campos de la DLL frente a los desplazamientos del EXE usados arriba.
inline json Layout(bool& ok) {
    json lay = json::object(); ok = true;
    const auto Chk = [&](const char* n, uint32 got, uint32 want) { lay[n] = got; if (got != want) { ok = false; lay[std::string(n) + "_expected"] = want; } };
    alignas(16) static std::array<uint8, 0x900> cb{};
    const auto OffOf = [&](const void* m) { return uint32(static_cast<const uint8*>(m) - cb.data()); };
    auto* B = reinterpret_cast<CBoat*>(cb.data());
    Chk("CBoat.m_matrix", OffOf(&B->m_matrix), 0x14);
    Chk("CBoat.m_placement.pos", OffOf(&B->m_placement.m_vPosn), 4);
    Chk("CBoat.m_nModelIndex", OffOf(&B->m_nModelIndex), 0x22);
    Chk("CBoat.m_NumWakeCoords", OffOf(&B->m_NumWakeCoords), 0x644);
    Chk("CBoat.m_WakeCoords", OffOf(B->m_WakeCoords.data()), 0x648);
    Chk("CBoat.m_WakePtCounters", OffOf(B->m_WakePtCounters.data()), 0x748);
    Chk("CBoat.m_WakeBoatSpeed", OffOf(B->m_WakeBoatSpeed.data()), 0x7C8);
    auto* ml = reinterpret_cast<CMatrixLink*>(cb.data());
    Chk("CMatrix.forward", OffOf(&ml->GetForward()), 0x10);
    Chk("CMatrix.pos", OffOf(&ml->GetPosition()), 0x30);
    auto* cm = reinterpret_cast<CColModel*>(cb.data());
    Chk("CColModel.bbox.max.x", OffOf(&cm->GetBoundingBox().m_vecMax.x), 0xC);
    Chk("CBoat::apFrameWakeGeneratingBoats", uint32(reinterpret_cast<uintptr_t>(&CBoat::apFrameWakeGeneratingBoats)), 0xC27994);
    Chk("TheCamera.m_matrix", uint32(reinterpret_cast<uintptr_t>(&TheCamera.m_matrix)), 0xB6F03C);
    Chk("TheCamera.m_placement.pos", uint32(reinterpret_cast<uintptr_t>(&TheCamera.m_placement.m_vPosn)), 0xB6F02C);
    Chk("waterwakeRaster", uint32(reinterpret_cast<uintptr_t>(&CWaterLevel::waterwakeRaster)), 0xC228B8);
    Chk("fakeFile < 2GB", uint32(reinterpret_cast<uintptr_t>(fakeFile.data()) < 0x80000000u), 1);   // OpenFile: jl sobre el valor devuelto
    return lay;
}

// ---------------------------------------------------------------------------------------------------------------------
inline json Run(size_t f, unsigned i, bool cand, Hook* hook) {
    const auto a = fns[f].address;
    switch (f) {
    case 0:  return CaseAddQuad(i, cand, hook, a);
    case 1:  return CaseBoatWakes(i, cand, hook, a);
    case 2:  return CaseSplitY(i, cand, hook, a);
    default: return CaseInit(i, cand, hook, a);
    }
}
#undef WL_SITE
#undef RW_SITES
}}
