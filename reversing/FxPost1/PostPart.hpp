#pragma once
// FxPost1 - parte Post: CPostEffects (35 funciones pendientes de docs/hooks.csv con validated=0).
// Original (todos los hooks de CPostEffects de la tabla desactivados: EXE puro) frente a candidato (todos activos: DLL), salvo Render,
// donde solo se alterna el hook de Render y sus colaboradores (Fog, ColourFilter, DarknessFilter, Radiosity, SpeedFX, NightVision,
// Grain, InfraredVision, HeatHazeFX, UnderWaterRipple, CCTV, FindPlayerPed/Vehicle, GetLightingFromCol, CCullZones::*NoRain) se doblan
// con trazas. Nada dibuja: se sustituye en memoria RwEngineInstance (0xC97B24) por un dispositivo falso cuyas funciones
// (RenderStateSet/Get, Im2DRenderPrimitive/Indexed) trazan sus argumentos; las funciones de camara y raster (RwCameraEndUpdate,
// RsCameraBeginUpdate, RwRasterPush/Pop/RenderFast/Create/Destroy/Lock/Unlock) y el RNG (exe 0x821B1E y GetRandomNumber de la DLL)
// tambien se doblan (EXE + simbolo de la DLL). RwCameraEndUpdate de la DLL no es un envoltorio del EXE: se dobla su simbolo.
// Fuera del banco: la rama de guardado de foto de Render (E/S de fichero: fopen 0x8232D8 / JPegCompressScreenToFile 0x5D0820).
// Trazas con etiquetas 200..299.
#include "FxPostCommon.hpp"
#include "PostEffects.h"
#include "Fx.h"

namespace FxPost1Live { namespace Post {
using ReversingProbe::EntryPatch;

// ---------------------------------------------------------------------------------------------------------------------
// Tabla
// ---------------------------------------------------------------------------------------------------------------------
inline const std::array<Fn, 35> fns{{
    {"CPostEffects","CCTV",0x702F40,0,-1,256},
    {"CPostEffects","Close",0x7010C0,0,-1,64},
    {"CPostEffects","ColourFilter",0x703650,0,-1,256},
    {"CPostEffects","DarknessFilter",0x702F00,0,-1,256},
    {"CPostEffects","DoScreenModeDependentInitializations",0x7046D0,0,-1,128},
    {"CPostEffects","DrawQuad",0x700EC0,0,-1,512},
    {"CPostEffects","FilterFX_RestoreDayNightBalance",0x7034D0,0,-1,128},
    {"CPostEffects","FilterFX_StoreAndSetDayNightBalance",0x7034B0,0,-1,128},
    {"CPostEffects","Fog",0x704150,0,-1,512},
    {"CPostEffects","Grain",0x7037C0,0,-1,192},
    {"CPostEffects","HeatHazeFXInit",0x701450,0,-1,256},
    {"CPostEffects","ImmediateModeFilterStuffInitialize",0x703CC0,0,-1,256},
    {"CPostEffects","ImmediateModeRenderStatesReStore",0x700E00,0,-1,128},
    {"CPostEffects","ImmediateModeRenderStatesSet",0x700D70,0,-1,64},
    {"CPostEffects","ImmediateModeRenderStatesStore",0x700CC0,0,-1,128},
    {"CPostEffects","InfraredVision",0x703F80,0,-1,256},
    {"CPostEffects","InfraredVisionRestoreLightsForHeatObjects",0x701410,0,-1,64},
    {"CPostEffects","InfraredVisionSetLightsForDefaultObjects",0x701430,0,-1,64},
    {"CPostEffects","InfraredVisionStoreAndSetLightsForHeatObjects",0x701320,0,-1,512},
    {"CPostEffects","Initialise",0x704630,0,-1,64},
    {"CPostEffects","IsVisionFXActive",0x7034F0,0,-1,64},
    {"CPostEffects","NightVision",0x7011C0,0,-1,256},
    {"CPostEffects","NightVisionSetLights",0x7012E0,0,-1,64},
    {"CPostEffects","Render",0x7046E0,0,-1,1024},
    {"CPostEffects","ScriptCCTVSwitch",0x7011B0,0,-1,64},
    {"CPostEffects","ScriptDarknessFilterSwitch",0x701170,0,-1,128},
    {"CPostEffects","ScriptHeatHazeFXSwitch",0x701160,0,-1,64},
    {"CPostEffects","ScriptInfraredVisionSwitch",0x701140,0,-1,64},
    {"CPostEffects","ScriptNightVisionSwitch",0x701120,0,-1,64},
    {"CPostEffects","ScriptResetForEffects",0x7010F0,0,-1,64},
    {"CPostEffects","SetFilterMainColour",0x703520,0,-1,256},
    {"CPostEffects","SetSpeedFXManualSpeedCurrentFrame",0x700BE0,0,-1,256},
    {"CPostEffects","SetupBackBufferVertex",0x7043D0,0,-1,256},
    {"CPostEffects","UnderWaterRipple",0x7039C0,0,-1,512},
    {"CPostEffects","Update",0x7046A0,0,-1,128},
}};

// ---------------------------------------------------------------------------------------------------------------------
// Mundo falso
// ---------------------------------------------------------------------------------------------------------------------
struct PWorld {
    alignas(16) std::array<uint8, 0x400> device{}, camera{};
    alignas(16) std::array<std::array<uint8, 0x80>, 10> rasters{};      // 0 camara, 1 front buffer, 2 grano, 3 usuario, 4 nubes, 5 libre, 6..9 creadas
    alignas(16) std::array<uint8, 256 * 256 * 4 + 64> pixels{};
    Buf<0x40> tex;                       // RwTexture falsa (raster en +0)
    Buf<0x800> ped;                      // CPed falso
    Buf<0xA00> veh;                      // CVehicle/CAutomobile falso
    Buf<0x48> vehMatrix;                 // CMatrix falsa del vehiculo
    CVector speed{};
    float lights[8]{}; size_t nLight{};
    bool camNoRain{}, playerNoRain{}, hasPlayer{}, hasVehicle{};
    uint32 getTable[32]{};
    size_t nCreated{};
    bool badCall{};
    uint32 rngState{}, rngCount{}, rngHash{};
};
inline PWorld PW;
inline uint8* Ras(size_t k) { return PW.rasters[k].data(); }
inline RwRaster* RasP(size_t k) { return reinterpret_cast<RwRaster*>(Ras(k)); }
inline void SetRaster(size_t k, int w, int h, int depth) { std::memset(Ras(k), 0xAA, 0x80); At<int32>(Ras(k), 0x0C) = w; At<int32>(Ras(k), 0x10) = h; At<int32>(Ras(k), 0x14) = depth; }
inline uint32 RId(const void* p) {
    if (!p) return 0;
    const auto a = uintptr_t(p);
    for (size_t k = 0; k < PW.rasters.size(); ++k) if (a == uintptr_t(Ras(k))) return uint32(0x1000 + k);
    if (a == uintptr_t(PW.pixels.data())) return 0x2000;
    if (a == uintptr_t(PW.tex.p())) return 0x2001;
    if (a == uintptr_t(PW.camera.data())) return 0x2002;
    return a > 0x10000 ? 0x3000 : uint32(a);       // valores pequenos (estados del renderizador) se conservan; punteros ajenos se anulan
}
inline float& Fl(uintptr_t a) { return *reinterpret_cast<float*>(a); }
inline int32& In(uintptr_t a) { return *reinterpret_cast<int32*>(a); }
inline uint32& Ud(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint8& By(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }
inline uint32 PtrVal(const void* p) { return uint32(uintptr_t(p)); }

// ---------------------------------------------------------------------------------------------------------------------
// Dobles. Dispositivo falso (RwEngineInstance): cdecl, el llamante limpia la pila.
// ---------------------------------------------------------------------------------------------------------------------
inline RwBool __cdecl DevSet(int state, void* value) { Push(200, { U(state), RId(value) }); return TRUE; }
inline RwBool __cdecl DevGet(int state, void* out) { Push(201, { U(state) }); *static_cast<uint32*>(out) = PW.getTable[state & 31]; return TRUE; }
inline RwBool __cdecl DevPrim(int type, void* v, int n) { Push(202, { U(type), U(n), n > 0 && n <= 4096 ? Fnv32(v, size_t(n) * 0x1C) : 0u }); return TRUE; }
inline RwBool __cdecl DevIdx(int type, void* v, int nv, void* idx, int ni) {
    Push(203, { U(type), U(nv), U(ni), nv > 0 && nv <= 4096 ? Fnv32(v, size_t(nv) * 0x1C) : 0u, ni > 0 && ni <= 8192 ? Fnv32(idx, size_t(ni) * 2) : 0u });
    return TRUE;
}
// Camara y raster (EXE; las envolturas de la DLL llaman a estas direcciones; RwCameraEndUpdate y RsCameraBeginUpdate de la DLL se parchean aparte).
inline void* __cdecl CamEnd(void* c) { Push(210, { RId(c) }); return c; }
inline RwBool __cdecl CamBegin(void* c) { Push(211, { RId(c) }); return TRUE; }
inline void* __cdecl RasPush(void* r) { Push(212, { RId(r) }); return r; }
inline void* __cdecl RasFast(void* r, int x, int y) { Push(213, { RId(r), U(x), U(y) }); return r; }
inline void* __cdecl RasPop() { Push(214, {}); return nullptr; }
inline void* __cdecl RasCreate(int w, int h, int depth, int flags) {
    Push(215, { U(w), U(h), U(depth), U(flags) });
    if (!Mine() || PW.nCreated >= 4) { PW.badCall = true; return nullptr; }
    const size_t k = 6 + PW.nCreated++;
    SetRaster(k, w, h, depth);
    At<int32>(Ras(k), 0x1C) = flags;
    return Ras(k);
}
inline RwBool __cdecl RasDestroy(void* r) { Push(216, { RId(r) }); return TRUE; }
inline void* __cdecl RasLock(void* r, uint32 level, int mode) { Push(217, { RId(r), level & 0xFF, U(mode) }); std::memset(PW.pixels.data(), 0xCD, PW.pixels.size()); return PW.pixels.data(); }
inline void* __cdecl RasUnlock(void* r) { Push(218, { RId(r) }); return r; }
// RNG: cuenta y acumula un hash en vez de trazar cada valor (Grain/Initialise llaman 65536 veces).
inline int __cdecl FakeRand() {
    if (!Mine()) return 0;
    PW.rngState = PW.rngState * 1103515245u + 12345u;
    const int v = int((PW.rngState >> 16) & 0x7FFF);
    ++PW.rngCount; PW.rngHash = (PW.rngHash ^ uint32(v)) * 16777619u;
    return v;
}
inline uint16 __cdecl FakeRandomNumber() { return uint16(FakeRand()); }
inline void __cdecl FakeSrand(uint32) { }
// Luces de visiones (app_light.cpp): trazan y, en StoreAndSet, el color de calor vigente en ese instante.
inline void __cdecl LightNV() { Push(220, {}); }
inline void __cdecl LightIRDefault() { Push(221, {}); }
inline void __cdecl LightIRStore() { Push(222, { Bits(Fl(0x8D50BC)), Bits(Fl(0x8D50C0)), Bits(Fl(0x8D50C4)), Bits(Fl(0x8D50C8)) }); }
inline void __cdecl LightIRRestore() { Push(223, {}); }
inline CVector* __cdecl FakeSpeed(int32 id) { Push(230, { U(id) }); return &PW.speed; }
// Colaboradores de Render
inline void __cdecl RFog() { Push(250, {}); }
inline void __cdecl RColour(uint32 a, uint32 b) { Push(251, { a, b }); }
inline void __cdecl RDark(int32 alpha) { Push(252, { U(alpha) }); }
inline void __cdecl RRadio(int32 a, int32 b, int32 c, int32 d) { Push(253, { U(a), U(b), U(c), U(d) }); }
inline void __cdecl RSpeed(float v) { Push(254, { Bits(v) }); }
inline void __cdecl RNight() { Push(255, {}); }
inline void __cdecl RGrain(int32 strength, uint32 update) { Push(256, { U(strength) & 0xFF, update & 0xFF }); }   // solo el byte bajo llega a DrawQuad
inline void __cdecl RInfra(uint32 a, uint32 b) { Push(257, { a, b }); }
inline void __cdecl RHaze(float intensity, uint32 mask) { Push(258, { Bits(intensity), mask & 0xFF }); }
inline void __cdecl RRipple(uint32 color, float x, float y, float s, float v, float fr) { Push(259, { color & 0x00FFFFFF, Bits(x), Bits(y), Bits(s), Bits(v), Bits(fr) }); }   // el alfa del EXE es basura de pila
inline void __cdecl RCctv() { Push(260, {}); }
inline void* __cdecl FakePlayerPed(int32 id) { Push(240, { U(id) }); return PW.hasPlayer ? PW.ped.p() : nullptr; }
inline void* __cdecl FakePlayerVeh(int32 id, uint32 remote) { Push(241, { U(id), remote & 0xFF }); return PW.hasVehicle ? PW.veh.p() : nullptr; }
inline float __fastcall FakeLight(void*, void*, uint32 interior) { Push(242, { interior & 0xFF }); const float v = PW.lights[PW.nLight % 8]; ++PW.nLight; return v; }
inline bool __cdecl FakeCamNoRain() { Push(243, {}); return PW.camNoRain; }
inline bool __cdecl FakePlayerNoRain() { Push(244, {}); return PW.playerNoRain; }

// Parches comunes a todos los casos de la parte
inline std::vector<IsoSite> SitesCommon() {
    return {
        { 0x7EE180, DA(&RwCameraEndUpdate), SP(CamEnd) },
        { 0x619450, DA(&RsCameraBeginUpdate), SP(CamBegin) },
        { 0x7FB060, 0, SP(RasPush) },
        { 0x7FAF50, 0, SP(RasFast) },
        { 0x7FB110, 0, SP(RasPop) },
        { 0x7FB230, 0, SP(RasCreate) },
        { 0x7FB020, 0, SP(RasDestroy) },
        { 0x7FB2D0, 0, SP(RasLock) },
        { 0x7FAEC0, 0, SP(RasUnlock) },
        { 0x821B1E, 0, SP(FakeRand) },
        { 0, DA(&CGeneral::GetRandomNumber), SP(FakeRandomNumber) },
        { 0x821B11, 0, SP(FakeSrand) },
        { 0x735F70, DA(&SetLightsForNightVision), SP(LightNV) },
        { 0x735F20, DA(&SetLightsForInfraredVisionDefaultObjects), SP(LightIRDefault) },
        { 0x735E70, DA(&StoreAndSetLightsForInfraredVisionHeatObjects), SP(LightIRStore) },
        { 0x735EF0, DA(&RestoreLightsForInfraredVisionHeatObjects), SP(LightIRRestore) },
        { 0x56E090, DA(&FindPlayerSpeed), SP(FakeSpeed) },
    };
}
// Parches solo durante Render
inline std::vector<IsoSite> SitesRender() {
    return {
        { 0x704150, DA(&CPostEffects::Fog), SP(RFog) },
        { 0x703650, DA(&CPostEffects::ColourFilter), SP(RColour) },
        { 0x702F00, DA(&CPostEffects::DarknessFilter), SP(RDark) },
        { 0x702080, DA(&CPostEffects::Radiosity), SP(RRadio) },
        { 0x7030A0, DA(&CPostEffects::SpeedFX), SP(RSpeed) },
        { 0x7011C0, DA(&CPostEffects::NightVision), SP(RNight) },
        { 0x7037C0, DA(&CPostEffects::Grain), SP(RGrain) },
        { 0x703F80, DA(&CPostEffects::InfraredVision), SP(RInfra) },
        { 0x701780, DA(&CPostEffects::HeatHazeFX), SP(RHaze) },
        { 0x7039C0, DA(&CPostEffects::UnderWaterRipple), SP(RRipple) },
        { 0x702F40, DA(&CPostEffects::CCTV), SP(RCctv) },
        { 0x56E210, DA(&FindPlayerPed), SP(FakePlayerPed) },
        { 0x56E0D0, DA(&FindPlayerVehicle), SP(FakePlayerVeh) },
        { 0x5447B0, DA(&CPhysical::GetLightingFromCol), SP(FakeLight) },
        { 0x72DDB0, DA(&CCullZones::CamNoRain), SP(FakeCamNoRain) },
        { 0x72DDC0, DA(&CCullZones::PlayerNoRain), SP(FakePlayerNoRain) },
    };
}

// ---------------------------------------------------------------------------------------------------------------------
// Estado global (rangos reales que se guardan/restauran por ejecucion y al final)
// ---------------------------------------------------------------------------------------------------------------------
inline std::vector<GlobalRange> Globals() {
    return {
        { 0xC40150, 0x158 },          // ms_imf
        { 0xC400D8, 0x70 },           // cc_vertices
        { 0xC402B0, 0x88 },           // estaticos de CPostEffects (banderas, contadores, angulos, fuerza de lluvia...)
        { 0xC3F860, 0x878 },          // DayNightBalance guardado + hpS/hpY/hpX
        { 0x8D5090, 0x178 },          // estaticos inicializados de CPostEffects
        { 0x8D12C0, 4 },              // CCustomBuildingDNPipeline::m_fDNBalanceParam
        { 0x8D37D5, 1 },              // CWaterLevel::m_bWaterFogScript
        { 0xC17044, 8 },              // RsGlobal.maximumWidth/Height
        { 0xC1703C, 4 },              // Scene.m_pRwCamera
        { 0xC6BFC8, 4 },              // CClouds::ms_vc.texture
        { 0xB7C4CA, 6 },              // color inferior del cielo
        { 0xB7C518, 0x28 },           // colores PostFx del ciclo de tiempo + intensidad minima
        { 0xB7CB5C, 4 },              // CTimer::ms_fTimeStep
        { 0xB7CB84, 4 },              // CTimer::m_snTimeInMilliseconds
        { 0xC812D8, 0x5C },           // CWeather (Rain, HeatHaze, UnderWaterness, WaterDepth...)
        { 0xC4B950, 8 },              // contadores del buffer temporal
        { 0xC4B958, 0x2000 },         // indices temporales
        { 0xC5F958, 0x7000 },         // vertices 2D temporales
        { 0xC97B24, 4 },              // RwEngineInstance
        { 0xB5F851, 2 },              // CCutsceneMgr::ms_running / ms_cutsceneProcessing
        { 0xC8A7C1, 1 },              // CWeapon::ms_bTakePhoto
        { 0xBA67A7, 1 },              // FrontEndMenuManager.m_bIsSaveDone
        { 0xBA6830, 1 },              // m_bSavePhotos
        { 0xBA677B, 1 },              // m_bActivateMenuNextFrame
        { 0xC7C710, 8 },              // CSpecialFX
        { 0xA9AF38, 1 },              // g_fxMan.m_bHeatHazeEnabled
        { 0xB72914, 4 },              // CGame::currArea
        { 0xB6F02C, 0x14 },           // TheCamera: posicion y puntero a matriz
        { 0x8CD060, 4 },              // gfLaRiotsLightMult
    };
}

// ---------------------------------------------------------------------------------------------------------------------
// Fixture
// ---------------------------------------------------------------------------------------------------------------------
struct Dim { int w, h; };
inline void FillSane(uintptr_t addr, size_t bytes, uint32& s) { for (size_t o = 0; o + 4 <= bytes; o += 4) Fl(addr + o) = PickSmall(s); }
inline void ScreenFixture(uint32& s, bool tall = false) {
    static constexpr Dim screens[] = { {640,448},{800,600},{1024,768},{1280,720},{1366,768},{1600,900},{1920,1080},{2560,1440},{1280,1024},{640,480},{1024,1024},{3840,2160},{1152,864},{720,576} };
    const auto d = Pick(s, screens); (void)tall;
    In(0xC17044) = d.w; In(0xC17048) = d.h;
}
inline int RasterDim(uint32& s) { static constexpr int d[] = { 64, 100, 128, 200, 256, 300, 320, 512, 640, 800, 1000, 1024, 1280, 1366, 1920, 2048, 600, 768 }; return Pick(s, d); }
inline float StepPick(uint32& s) { static constexpr float t[] = { 0.25f, 0.5f, 1.f, 2.f, 3.5f, 0.016f, 10.f, 0.f, 40.f, 400.f }; return Pick(s, t); }

// Prepara el mundo comun: dispositivo, camara, rasters, tiempos y dimensiones. 'front' = coloca el front buffer.
inline void BaseFixture(uint32& s, bool front = true) {
    PW.nCreated = 0; PW.badCall = false; PW.rngCount = 0; PW.rngHash = 2166136261u; PW.rngState = Rnd(s) | 1u; PW.nLight = 0;
    PW.device.fill(0); PW.camera.fill(0);
    auto* g = reinterpret_cast<RwGlobals*>(PW.device.data());
    g->dOpenDevice.fpRenderStateSet = reinterpret_cast<RwRenderStateSetFunction>(DevSet);
    g->dOpenDevice.fpRenderStateGet = reinterpret_cast<RwRenderStateGetFunction>(DevGet);
    g->dOpenDevice.fpIm2DRenderPrimitive = reinterpret_cast<RwIm2DRenderPrimitiveFunction>(DevPrim);
    g->dOpenDevice.fpIm2DRenderIndexedPrimitive = reinterpret_cast<RwIm2DRenderIndexedPrimitiveFunction>(DevIdx);
    static constexpr float zn[] = { 0.f, 0.5f, 1.f, 0.99999f, 0.25f };
    g->dOpenDevice.zBufferNear = Pick(s, zn);
    for (auto& v : PW.getTable) v = Rnd(s) % 9;
    auto* cam = reinterpret_cast<RwCamera*>(PW.camera.data());
    static constexpr float np[] = { 0.05f, 0.25f, 0.3f, 1.f, 2.f, 0.1f, 0.7f };
    cam->nearPlane = Pick(s, np);
    SetRaster(0, RasterDim(s), RasterDim(s), Chance(s, 80) ? 32 : 16);
    SetRaster(1, RasterDim(s), RasterDim(s), 32);
    SetRaster(2, 256, 256, 32);
    SetRaster(3, RasterDim(s), RasterDim(s), 32);
    SetRaster(4, 128, 128, 32);
    SetRaster(5, 64, 64, 32);
    cam->frameBuffer = RasP(0);
    Ud(0xC1703C) = PtrVal(PW.camera.data());
    Ud(0xC402D8) = front ? PtrVal(Ras(1)) : 0u;
    Ud(0xC402B0) = PtrVal(Ras(2));
    PW.tex.Fill(0); At<void*>(PW.tex.p(), 0) = Ras(4);
    Ud(0xC6BFC8) = PtrVal(PW.tex.p());
    ScreenFixture(s);
    Fl(0xB7CB5C) = StepPick(s);
    Ud(0xB7CB84) = Chance(s, 25) ? (0x80000000u | Rnd(s)) : (Rnd(s) >> Rnd(s) % 20);
    FillSane(0xC40150, 0x158, s);                       // ms_imf: floats finitos (los enteros se fijan por funcion)
    FillSane(0xC400D8, 0x70, s);                        // cc_vertices
    Ud(0xC97B24) = PtrVal(PW.device.data());
    for (auto* b : { &By(0xC402B7), &By(0xC402B8), &By(0xC402B9) }) *b = uint8(Chance(s, 35));
}
inline void ImfInts(uint32& s) {                          // enteros de ms_imf: tamanos del buffer y raster
    In(0xC4015C) = RgI(s, 1, 2048); In(0xC40160) = RgI(s, 1, 2048);
    Ud(0xC40158) = Chance(s, 90) ? PtrVal(Ras(1)) : 0u;
    for (uintptr_t a = 0xC40280; a < 0xC402A8; a += 4) Ud(a) = Rnd(s) % 9;
    for (int k = 0; k < 6; ++k) Ud(0xC401E8 + 0x1C * k) = Rnd(s);   // colores de los vertices del quad (DrawQuad sobrescribe los cuatro primeros)
}

// ---------------------------------------------------------------------------------------------------------------------
// Ejecucion
// ---------------------------------------------------------------------------------------------------------------------
template<class R, class... A> inline R Cdecl(uintptr_t addr, A... a) { return reinterpret_cast<R(__cdecl*)(A...)>(addr)(a...); }
inline void SetAllHooks(const std::vector<Hook*>& hooks, bool cand) { for (auto* h : hooks) if (h->Hooked() != cand) h->State(cand); }

inline json Run(size_t f, unsigned i, bool cand, const std::vector<Hook*>& hooks, Hook* ownHook, const std::vector<GlobalRange>& globals, const std::vector<Saved>& initial) {
    const auto& fn = fns[f]; const std::string_view nm = fn.name;
    uint32 s = Seed(i, uint32(f) + 301);
    RestoreRanges(initial);
    Ctl ctl;
    BaseFixture(s);
    json ret = nullptr, extra = json::object();
    const auto A = fn.address;
    const bool isRender = nm == "Render";
    std::function<void()> call;

    if (nm == "ScriptCCTVSwitch" || nm == "ScriptHeatHazeFXSwitch" || nm == "ScriptInfraredVisionSwitch" || nm == "ScriptNightVisionSwitch") {
        for (uintptr_t a : { 0xC402B8u, 0xC402B9u, 0xC402BAu, 0xC402C4u, 0xC402C5u }) By(a) = uint8(Chance(s, 50));
        const bool v = Chance(s, 50);
        call = [=]() { Cdecl<void>(A, v); };
    } else if (nm == "ScriptDarknessFilterSwitch") {
        static constexpr int32 alphas[] = { -1, -5, 0, 1, 100, 170, 255, 256, 300, 1000, INT32_MIN, INT32_MAX, -2, 254 };
        By(0xC402C4) = uint8(Chance(s, 50)); In(0x8D5204) = int32(Rnd(s)); In(0x8D50F4) = RgI(s, 0, 255);
        const bool v = Chance(s, 50); const int32 a = Pick(s, alphas);
        call = [=]() { Cdecl<void>(A, v, a); };
    } else if (nm == "ScriptResetForEffects") {
        for (uintptr_t a : { 0xC402B8u, 0xC402B9u, 0xC402BAu, 0xC402C4u, 0xC402C5u }) By(a) = uint8(Chance(s, 50));
        By(0x8D37D5) = uint8(Chance(s, 50));
        call = [=]() { Cdecl<void>(A); };
    } else if (nm == "IsVisionFXActive") {
        call = [=, &ret]() { ret = Cdecl<bool>(A); };
    } else if (nm == "FilterFX_RestoreDayNightBalance" || nm == "FilterFX_StoreAndSetDayNightBalance") {
        Fl(0x8D12C0) = PickSmall(s); Fl(0x8D50A4) = PickSmall(s); Fl(0xC3F860) = PickSmall(s);
        call = [=]() { Cdecl<void>(A); };
    } else if (nm == "SetSpeedFXManualSpeedCurrentFrame") {
        const float v = PickF(s);
        Fl(0xC402C8) = PickSmall(s);
        call = [=]() { Cdecl<void>(A, v); };
    } else if (nm == "ImmediateModeRenderStatesStore") {
        call = [=]() { Cdecl<void>(A); };
    } else if (nm == "ImmediateModeRenderStatesSet") {
        call = [=]() { Cdecl<void>(A); };
    } else if (nm == "ImmediateModeRenderStatesReStore") {
        for (uintptr_t a = 0xC40280; a < 0xC402A8; a += 4) Ud(a) = Rnd(s) % 9;
        call = [=]() { Cdecl<void>(A); };
    } else if (nm == "DrawQuad") {
        ImfInts(s);
        Fl(0xC40150) = PickSmall(s);
        const float x1 = PickF(s), y1 = PickF(s), x2 = PickFinite(s), y2 = PickFinite(s);
        const uint32 r = Rnd(s) & 0xFF, gg = Rnd(s) & 0xFF, b = Rnd(s) & 0xFF, a = Rnd(s) & 0xFF;
        void* ras = Chance(s, 30) ? nullptr : (Chance(s, 50) ? Ras(3) : Ras(4));
        call = [=]() { reinterpret_cast<void(__cdecl*)(float, float, float, float, uint8, uint8, uint8, uint8, RwRaster*)>(A)(x1, y1, x2, y2, uint8(r), uint8(gg), uint8(b), uint8(a), static_cast<RwRaster*>(ras)); };
    } else if (nm == "DarknessFilter") {
        ImfInts(s); Fl(0xC40150) = PickSmall(s);
        const int32 alpha = Chance(s, 70) ? RgI(s, 0, 255) : int32(Rnd(s));
        call = [=]() { Cdecl<void>(A, alpha); };
    } else if (nm == "ColourFilter") {
        const uint32 c1 = Rnd(s), c2 = Rnd(s);
        call = [=]() { reinterpret_cast<void(__cdecl*)(RwRGBA, RwRGBA)>(A)(std::bit_cast<RwRGBA>(c1), std::bit_cast<RwRGBA>(c2)); };
    } else if (nm == "SetFilterMainColour") {
        const uint32 c = Rnd(s); void* ras = Chance(s, 60) ? Ras(1) : nullptr;
        call = [=]() { reinterpret_cast<void(__cdecl*)(RwRaster*, RwRGBA)>(A)(static_cast<RwRaster*>(ras), std::bit_cast<RwRGBA>(c)); };
    } else if (nm == "NightVision") {
        static constexpr float counts[] = { 0.f, 0.5f, 1.f, 2.9f, 3.f, 7.25f, 30.5f, 50.f, -1.f, -0.f, 1e-3f, 12.f, 4.f };
        ImfInts(s);
        Fl(0xC40300) = Chance(s, 4) ? std::bit_cast<float>(0x7FC00000u) : Pick(s, counts);
        call = [=]() { Cdecl<void>(A); };
    } else if (nm == "NightVisionSetLights" || nm == "InfraredVisionSetLightsForDefaultObjects" || nm == "InfraredVisionRestoreLightsForHeatObjects") {
        call = [=]() { Cdecl<void>(A); };
    } else if (nm == "InfraredVision") {
        static constexpr float radius[] = { 0.003f, 0.f, 0.01f, 0.05f, -0.002f, 0.0001f };
        ImfInts(s); Fl(0x8D50B8) = Pick(s, radius);
        for (uintptr_t a = 0xC40164; a <= 0xC40170; a += 4) Fl(a) = PickSmall(s);
        const uint32 c = Rnd(s), m = Rnd(s);
        call = [=]() { reinterpret_cast<void(__cdecl*)(RwRGBA, RwRGBA)>(A)(std::bit_cast<RwRGBA>(c), std::bit_cast<RwRGBA>(m)); };
    } else if (nm == "InfraredVisionStoreAndSetLightsForHeatObjects") {
        auto* ped = PW.ped.p();
        PW.ped.Fill(0xCD);
        At<int32>(ped, 0x530) = Chance(s, 60) ? 0x37 : RgI(s, 0, 60);
        const uint32 now = Ud(0xB7CB84);
        static constexpr uint32 deltas[] = { 0, 1, 100, 5000, 10000, 123456, 0x01000001u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFF00u, 0xFFFFFFFFu, 0x00FFFFFFu };
        At<uint32>(ped, 0x750) = now - Pick(s, deltas);
        By(0xC402B9) = uint8(Chance(s, 75)); By(0xC402B7) = uint8(Chance(s, 20));
        for (uintptr_t a : { 0x8D50BCu, 0x8D50C0u, 0x8D50C4u, 0x8D50C8u }) Fl(a) = Chance(s, 50) ? PickSmall(s) : float(Rnd(s) % 1000) * 0.001f;
        void* p = ped;
        call = [=]() { Cdecl<void>(A, p); };
    } else if (nm == "Fog") {
        static const CVector speeds[] = { {0,0,0}, {0.06f,0,0}, {0.0599999f,0,0}, {0.061f,0,0}, {0.03f,0.03f,0.03f}, {0,0,0.07f}, {1,2,3}, {-0.06f,0,0}, {0.0424f,0.0424f,0}, {0.0346f,0.0346f,0.0346f} };
        PW.speed = Chance(s, 5) ? CVector{ std::bit_cast<float>(0x7FC00000u), 0, 0 } : Pick(s, speeds);
        static constexpr float radii[] = { 0.f, 1.f, 100.f, 159.9f, 160.f, 200.f, -5.f, 40.f, 0.1f };
        Fl(0xC402E4) = Pick(s, radii); Fl(0xC402DC) = PickSmall(s) * 20.f;
        for (int k = 0; k < 3; ++k) *reinterpret_cast<uint16*>(0xB7C4CA + 2 * k) = uint16(Rnd(s));
        ImfInts(s); Fl(0xC40150) = PickSmall(s);
        call = [=]() { Cdecl<void>(A); };
    } else if (nm == "CCTV") {
        ImfInts(s); Fl(0xC40150) = PickSmall(s); Ud(0x8D50FC) = Rnd(s);
        call = [=]() { Cdecl<void>(A); };
    } else if (nm == "Grain") {
        ImfInts(s); Fl(0xC40150) = PickSmall(s);
        Ud(0xC4031C) = Rnd(s) >> 8;
        const int32 strength = Chance(s, 70) ? RgI(s, 0, 255) : int32(Rnd(s)); const bool update = Chance(s, 60);
        call = [=]() { reinterpret_cast<void(__cdecl*)(int32, bool)>(A)(strength, update); };
    } else if (nm == "UnderWaterRipple") {
        static constexpr float xo[] = { 0.f, 0.5f, 1.f, 4.f, 3.99f, 12.5f, 100.f };
        static constexpr float sp[] = { 0.0015f, 0.f, 0.01f, 0.5f };
        static constexpr float fr[] = { 0.04f, 0.f, 1.f, 0.25f };
        const int h = Chance(s, 6) ? (Chance(s, 50) ? 0 : -10) : RgI(s, 1, 700);
        SetRaster(1, RasterDim(s), h, 32);
        /* Como el original: el avance del bucle es ftol(y + yoffset) (0x703B61); con yoffset < 1 y no avanza, el bucle no termina y desborda el array de vertices (0xC5F958) hasta pisar RwEngineInstance (AV 0x703B2E): yo >= 1 */
        const float yo = h > 0 ? std::max(1.f, float(h) / 300.f) + float(Rnd(s) % 1000) * 0.03f : 4.f;
        ImfInts(s); Fl(0xC40150) = PickSmall(s);
        const uint32 col = Rnd(s);
        call = [=]() { reinterpret_cast<void(__cdecl*)(CRGBA, float, float, float, float, float)>(A)(std::bit_cast<CRGBA>(col), xo[(i * 7) % std::size(xo)], yo, 64.f, sp[(i / 3) % std::size(sp)], fr[(i / 5) % std::size(fr)]); };
    } else if (nm == "HeatHazeFXInit") {
        static constexpr int32 types[] = { -1, 0, 1, 2, 3, 4, 5, 100, -7 };
        Ud(0xC402D8) = PtrVal(Ras(1));
        In(0xC402BC) = Pick(s, types); In(0x8D50E4) = Chance(s, 25) ? In(0xC402BC) : Pick(s, types);
        call = [=]() { Cdecl<void>(A); };
    } else if (nm == "ImmediateModeFilterStuffInitialize") {
        Ud(0xC402D8) = PtrVal(Ras(1));
        call = [=]() { Cdecl<void>(A); };
    } else if (nm == "SetupBackBufferVertex" || nm == "DoScreenModeDependentInitializations" || nm == "Update" || nm == "Initialise" || nm == "Close") {
        // Como el original: DoScreenModeDependentInitializations (0x7046D0) llama a ImmediateModeFilterStuffInitialize (0x703D1F mov edx,[eax+0Ch]) sin comprobar que el front buffer exista: nunca se prueba con puntero nulo.
        uint32 mode = Rnd(s) % 3; if (nm == "DoScreenModeDependentInitializations" && mode == 0) mode = 1;
        // 0 sin front buffer, 1 con las mismas dimensiones que la camara (potencia de dos siguiente), 2 con otras
        if (mode == 0) Ud(0xC402D8) = 0;
        else {
            const int cw = At<int32>(Ras(0), 0x0C), ch = At<int32>(Ras(0), 0x10);
            SetRaster(1, mode == 1 ? cw : RasterDim(s), mode == 1 ? ch : RasterDim(s), 32);
            Ud(0xC402D8) = PtrVal(Ras(1));
        }
        static constexpr int32 types[] = { -1, 0, 1, 2, 3, 4 };
        In(0xC402BC) = Pick(s, types); In(0x8D50E4) = Chance(s, 30) ? In(0xC402BC) : Pick(s, types);
        static constexpr float rain[] = { 0.f, 0.0001f, 0.5f, 1.f, -1.f, 100.f };
        Fl(0xC81324) = Chance(s, 3) ? std::bit_cast<float>(0x7FC00000u) : Pick(s, rain); By(0xC402D1) = uint8(Rnd(s) & 1);
        if (nm == "Close") Ud(0xC402B0) = Chance(s, 90) ? PtrVal(Ras(2)) : 0u;
        call = [=]() { Cdecl<void>(A); };
    } else if (isRender) {
        // ---- fixture de Render ----
        static constexpr float tsList[] = { 0.25f, 0.5f, 1.f, 2.f, 3.5f, 10.f, 400.f };
        static constexpr float lightMults[] = { 1.f, 0.5f, 1.5f, 2.f, 0.f, 3.7f };
        static constexpr float colourMults[] = { 1.f, 0.5f, 2.f, 1.25f };
        static constexpr float rains[] = { 0.f, 0.5f, 1.f, 0.3f, -1.f, 0.0078f };
        static constexpr float unders[] = { 0.f, 0.3f, 0.535f, 0.54f, 1.f, 0.5349f };
        static constexpr float depths[] = { 0.f, 10.f, 89.9f, 90.f, 120.f, -5.f };
        static constexpr float manual[] = { 0.f, 0.f, 0.5f, 1.2f, -1.f };
        static constexpr float tires[] = { -1.f, 0.f, 5.f, -0.f, -0.001f };
        static constexpr float gas[] = { 0.f, 0.5f, 1.f, -1.f, 2.f };
        static constexpr float zs[] = { 0.f, 100.f, 900.f, 900.01f, 1000.f, 899.99f, -50.f };
        Fl(0xB7CB5C) = Pick(s, tsList);
        By(0xC402CF) = uint8(Chance(s, 12));
        Ud(0xC40330) = Chance(s, 30) ? 0u : 1u;
        Fl(0xC4032C) = Chance(s, 3) ? std::bit_cast<float>(0x7FC00000u) : (Chance(s, 50) ? Fl(0x8D516C) : PickSmall(s) * 0.2f);
        By(0xC402C6) = uint8(Chance(s, 40));
        for (uintptr_t a = 0xB7C518; a <= 0xB7C534; a += 4) Fl(a) = Chance(s, 4) ? std::bit_cast<float>(0x7FC00000u) : (float(int(Rnd(s) % 700) - 80) * (Chance(s, 50) ? 1.f : 0.37f));
        for (uintptr_t a : { 0xC402B8u, 0xC402B9u }) By(a) = uint8(Chance(s, 22));
        Fl(0x8CD060) = Pick(s, lightMults);
        PW.hasPlayer = Chance(s, 80);
        { const float step = Fl(0xB7CB5C) * Fl(0x8D5168), base = Fl(0xC4032C);
          for (int k = 0; k < 8; ++k) PW.lights[k] = std::isnan(base) ? 0.2f : base + (k % 2 ? 1.f : -1.f) * step * float(Rnd(s) % 5) * 0.5f + (Chance(s, 20) ? 0.3f : 0.f); }
        By(0x8D518C) = uint8(Chance(s, 70));
        Fl(0x8D5160) = Pick(s, colourMults); Fl(0x8D5164) = Pick(s, colourMults);
        By(0xC402C4) = uint8(Chance(s, 30)); In(0x8D5204) = RgI(s, 0, 255); In(0x8D50F8) = RgI(s, 0, 100);
        By(0xC402C7) = uint8(Chance(s, 15));
        By(0x8D5100) = uint8(Chance(s, 70)); By(0x8D5108) = uint8(Chance(s, 85)); By(0x8D5109) = uint8(Chance(s, 85));
        Fl(0xC402C8) = Pick(s, manual);
        PW.hasVehicle = Chance(s, 75);
        { auto* v = PW.veh.p(); PW.veh.Fill(0); PW.vehMatrix.Fill(0);
          static constexpr int32 vtypes[] = { 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
          At<int32>(v, 0x594) = Pick(s, vtypes);
          At<uint32>(v, 0x38C) = Chance(s, 60) ? 0x80000u : 0u;
          At<float>(v, 0x8A4) = Chance(s, 3) ? std::bit_cast<float>(0x7FC00000u) : Pick(s, tires);
          At<float>(v, 0x49C) = Pick(s, gas);
          At<void*>(v, 0x14) = PW.vehMatrix.p();
          for (int k = 0; k < 3; ++k) { At<float>(PW.vehMatrix.p(), 0x10 + 4 * k) = PickSmall(s); At<float>(v, 0x44 + 4 * k) = PickSmall(s) * 0.3f; }
          PW.speed = CVector{ PickSmall(s) * 0.02f, PickSmall(s) * 0.02f, PickSmall(s) * 0.02f }; }
        By(0xB5F851) = uint8(Chance(s, 15)); By(0xB5F852) = uint8(Chance(s, 15));
        Fl(0x8D50B0) = Chance(s, 50) ? 50.f : PickSmall(s); Fl(0xC40300) = PickSmall(s);
        By(0xC402CC) = uint8(Chance(s, 50)); By(0xC402CE) = uint8(Chance(s, 50));
        for (uintptr_t a : { 0x8D5110u, 0x8D510Cu, 0x8D5114u, 0x8D5118u }) In(a) = RgI(s, 0, 6);
        In(0xB7C53C) = RgI(s, 0, 255);
        By(0xC402D1) = uint8(Chance(s, 50)); In(0xC40328) = Chance(s, 40) ? 0 : RgI(s, 0, 140);
        Fl(0xC81324) = Chance(s, 3) ? std::bit_cast<float>(0x7FC00000u) : Pick(s, rains);
        PW.camNoRain = Chance(s, 25); PW.playerNoRain = Chance(s, 25);
        Fl(0xC8132C) = Pick(s, unders);        // UnderWaterness
        In(0xB72914) = Chance(s, 70) ? 0 : RgI(s, 1, 5);
        Ud(0xB6F03C) = 0; Fl(0xB6F02C + 8) = Pick(s, zs);
        By(0xC402B4) = uint8(Chance(s, 40)); Ud(0x8D5094) = Rnd(s);
        By(0xC402BA) = uint8(Chance(s, 20)); Fl(0xC812DC) = Chance(s, 50) ? 0.f : float(Rnd(s) % 100) * 0.01f; Fl(0xC812D8) = float(Rnd(s) % 100) * 0.01f;
        By(0xA9AF38) = uint8(Chance(s, 40));
        By(0xC402D3) = uint8(Chance(s, 35)); Fl(0xC81330) = Pick(s, depths); By(0x8D5144) = uint8(Chance(s, 60));
        Fl(0xC40324) = Chance(s, 20) ? 30.f : PickSmall(s) * 2.f; Ud(0x8D5140) = Rnd(s);
        By(0xC402C5) = uint8(Chance(s, 25));
        // foto: se evita la rama con E/S (s_WaitForOneFrame con ms_bTakePhoto vigente tras el borrado por m_bIsSaveDone)
        By(0xC8A7C1) = uint8(Chance(s, 40)); By(0xBA67A7) = uint8(Chance(s, 40)); By(0xC40321) = uint8(Chance(s, 40)); By(0xBA6830) = uint8(Chance(s, 50)); By(0xBA677B) = uint8(Chance(s, 50));
        By(0xC40320) = uint8(Chance(s, 50)); By(0xC402D0) = uint8(Chance(s, 50)); Ud(0xC7C710) = Rnd(s); Ud(0xC7C714) = Rnd(s);
        if (By(0xC40321) && By(0xC8A7C1) && !By(0xBA67A7)) By(0xC40321) = 0;
        call = [=]() { Cdecl<void>(A); };
    } else {
        return json{ {"switches", false}, {"error", "funcion sin fixture"} };
    }

    // ---- activacion y ejecucion ----
    if (isRender) { if (ownHook->Hooked() != cand) ownHook->State(cand); } else SetAllHooks(hooks, cand);
    const bool switches = ReversingProbe::JumpsToDll(A) == cand;
    nrec = 0;
    call();
    const unsigned fp = _controlfp(0, 0) & _MCW_PC;

    json out;
    out["switches"] = switches; out["fp"] = fp; out["ret"] = ret; out["extra"] = extra;
    out["regions"] = HashRanges(globals);
    out["fake"] = json::array({ Fnv32(PW.device.data(), PW.device.size()), Fnv32(PW.camera.data(), PW.camera.size()), Fnv32(PW.rasters.data(), sizeof(PW.rasters)), Fnv32(PW.pixels.data(), PW.pixels.size()), Fnv32(PW.tex.raw.data(), PW.tex.raw.size()), Fnv32(PW.ped.raw.data(), PW.ped.raw.size()), Fnv32(PW.veh.raw.data(), PW.veh.raw.size()), Fnv32(PW.vehMatrix.raw.data(), PW.vehMatrix.raw.size()) });
    out["rng"] = json::array({ PW.rngCount, PW.rngHash });
    out["created"] = PW.nCreated; out["bad_call"] = PW.badCall;
    out["trace"] = Recs();
    return out;
}

// Comprobaciones de disposicion usadas por el banco (se registran en el evento 'begin').
inline json Layout(bool& ok) {
    RwGlobals g{}; RwCamera c{};
    const auto off = [](const void* base, const void* p) { return int(reinterpret_cast<const uint8*>(p) - reinterpret_cast<const uint8*>(base)); };
    json j = {
        {"fpRenderStateSet", off(&g, &g.dOpenDevice.fpRenderStateSet)}, {"fpRenderStateGet", off(&g, &g.dOpenDevice.fpRenderStateGet)},
        {"fpIm2DRenderPrimitive", off(&g, &g.dOpenDevice.fpIm2DRenderPrimitive)}, {"fpIm2DRenderIndexedPrimitive", off(&g, &g.dOpenDevice.fpIm2DRenderIndexedPrimitive)},
        {"zBufferNear", off(&g, &g.dOpenDevice.zBufferNear)}, {"frameBuffer", off(&c, &c.frameBuffer)}, {"nearPlane", off(&c, &c.nearPlane)},
        {"sizeof_camera", sizeof(RwCamera)}, {"sizeof_globals", sizeof(RwGlobals)}, {"sizeof_imf", sizeof(CPostEffects::imf)}
    };
    ok = j["fpRenderStateSet"] == 0x20 && j["fpRenderStateGet"] == 0x24 && j["fpIm2DRenderPrimitive"] == 0x30 && j["fpIm2DRenderIndexedPrimitive"] == 0x34 && j["zBufferNear"] == 0x18
        && j["frameBuffer"] == 0x60 && j["nearPlane"] == 0x80 && sizeof(RwCamera) <= 0x400 && sizeof(CPostEffects::imf) == 0x158;
    return j;
}
}}
