#pragma once
// Infraestructura comun del banco RadarFinal3 (estilo HudCamFinal2 / CamHud1): trazas sin reservas, zonas globales guardadas/restauradas,
// registro de colaboradores simulados (EntryPatch en la direccion del EXE + simbolo de la DLL + su cuerpo) activados por funcion probada.
// Solo traza el hilo del banco (owner/Mine()); los hilos ajenos (audio) reciben un resultado inocuo y no alteran nada.
#include <fstream>
#include <float.h>
#include <set>
#include <bit>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Radar.h"
#include "Hud.h"
#include "Cam.h"
#include "Camera.h"
#include "Font.h"
#include "Sprite2d.h"
#include "Pad.h"
#include "World.h"
#include "General.h"
#include "MenuManager.h"
#include "GameLogic.h"
#include "TxdStore.h"
#include "Timer.h"
#include "Pickups.h"
#include "Pickup.h"
#include "CutsceneMgr.h"
#include "RwHelper.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Models/ModelInfo.h"
#include "Models/BaseModelInfo.h"
#include "Scripts/TheScripts.h"
#include "../ProbeRuntime.hpp"

namespace RadarFinal3Live {
using Hook = ReversibleHooks::ReversibleHook::Base;
inline bool done{}; inline std::ofstream log;
// Hilo del banco: otros hilos del juego (audio) llaman a funciones parcheadas; sus llamadas no se trazan ni cambian estado.
inline DWORD owner{};
inline bool Mine() { return GetCurrentThreadId() == owner; }
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
inline uint64 Fnv(const void* p, size_t n, uint64 h = 1469598103934665603ull) { auto* b = static_cast<const uint8*>(p); for (size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 1099511628211ull; } return h; }
inline uint32 P(const void* p) { return uint32(uintptr_t(p)); }
inline uint32 Bits(float f) { return std::bit_cast<uint32>(f); }
inline float Flt(uint32 b) { return std::bit_cast<float>(b); }
inline uint32 Rnd(uint32& s) { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s; }
inline uint32 Seed(unsigned i, uint32 salt) { uint32 s = i * 2654435761u + salt * 0x9E3779B9u + 1u; if (!s) s = 1; for (int k = 0; k < 4; ++k) Rnd(s); return s; }
template<class T> inline T& At(uint8* base, size_t off) { return *reinterpret_cast<T*>(base + off); }
template<class T> inline T& Mem(uintptr_t address) { return *reinterpret_cast<T*>(address); }
inline std::string Hex(const void* p, size_t n) {
    static const char* d = "0123456789abcdef"; std::string s(n * 2, '0'); auto* b = static_cast<const uint8*>(p);
    for (size_t i = 0; i < n; ++i) { s[2 * i] = d[b[i] >> 4]; s[2 * i + 1] = d[b[i] & 15]; }
    return s;
}
inline std::string HexAt(uintptr_t address, size_t n) { return Hex(reinterpret_cast<const void*>(address), n); }

// Buffer con guardas de 16 bytes a cada lado (se comparan enteros: detectan escrituras fuera de rango).
template<size_t N> struct Buf {
    alignas(16) std::array<uint8, N + 32> raw{};
    uint8* p() { return raw.data() + 16; }
    void Fill(uint8 v) { raw.fill(v); }
    std::string Dump() const { return Hex(raw.data(), raw.size()); }
};

// Floats de los fixtures: moderados (el x87 conserva el exponente extendido y SSE no) y sin NaN/Inf de senalizacion.
inline float PickNear(uint32& s) { static constexpr float t[] = { 0.f, 0.5f, -0.5f, 1.f, -1.f, 2.f, -2.f, 3.f, -3.f, 1.5f, -1.5f }; return t[Rnd(s) % std::size(t)]; }
inline float PickRand(uint32& s) { const uint32 r = Rnd(s); return std::bit_cast<float>((r & 0x80000000u) | ((110u + Rnd(s) % 30u) << 23) | (r & 0x7FFFFFu)); }
inline float PickM(uint32& s) { return Rnd(s) % 100 < 45 ? PickNear(s) : PickRand(s); }
inline float PickUnit(uint32& s) { return float(int32(Rnd(s) % 2001) - 1000) / 1000.f; }
// Flotante uniforme en [lo, hi) con resolucion de 1/1024 (no es un valor "bonito": ejercita el redondeo)
inline float PickIn(uint32& s, float lo, float hi) { return lo + (hi - lo) * float(Rnd(s) % 100000u) / 100000.f; }

// --- registro de trazas sin reservas ---
struct Rec { uint32 tag; uint32 n; uint32 a[24]; };
inline std::array<Rec, 16384> recs{}; inline size_t nrec{}; inline bool recOverflow{};
inline void Push(uint32 t, std::initializer_list<uint32> v = {}) {
    if (!Mine()) return;
    if (nrec >= recs.size()) { recOverflow = true; return; }
    auto& r = recs[nrec++]; r = {}; r.tag = t; for (auto x : v) if (r.n < 24) r.a[r.n++] = x;
}
inline json Recs() { json r = json::array(); for (size_t i = 0; i < nrec; ++i) { json e = json::array({ recs[i].tag }); for (uint32 k = 0; k < recs[i].n; ++k) e.push_back(recs[i].a[k]); r.push_back(e); } return r; }
// Texto terminado en NUL -> huella (el puntero cambia entre original y candidato solo si el contenido difiere)
inline uint32 TextHash(const void* p) {
    if (!p) return 0;
    auto* b = static_cast<const uint8*>(p); size_t n = 0; while (n < 600 && b[n]) ++n;
    return uint32(Fnv(b, n)) ^ uint32(n * 0x01000193u);
}

enum Tag : uint32 {
    T_RW = 1, T_IM2D, T_SETVERTS, T_SETVERTSRECT, T_SETMASK, T_POLY, T_DRAWRECT, T_SPRITERECT, T_SPRITE9, T_PRINT, T_TEXTGET, T_REQMODEL, T_REMMODEL,
    T_PUSHTXD, T_SETTXD, T_POPTXD, T_FINDSLOT, T_SETTEX, T_DELETE, T_FIRSTTEX, T_CONE, T_MARKERSET, T_DEBUGLINE, T_ONMISSION, T_COOP, T_CENTRE, T_HEADING,
    T_SPEED, T_REMOTE, T_ENEXPOS, T_ZONELOCK, T_GANGCAN, T_ISHIDDEN, T_LOADWB, T_SAVEWB, T_ALLOCMATRIX, T_UPDATEMATRIX, T_NORMALISE, T_DRAWENTITY,
    T_DRAWCOORD, T_DRAWSPRITE, T_DRAWROT, T_YOUARE, T_SETUPAIR, T_LEGENDADD, T_TRACEHEIGHT, T_MARKER, T_DRAWMAP, T_DRAWRADARMAP, T_SECTION, T_MASK,
    T_AREA, T_GANGOVERLAY, T_STREAMXY, T_CALCSINCOS, T_CTRLSTART
};

// --- zonas globales que el banco toca (se guardan al empezar y se restauran tras cada ejecucion) ---
constexpr uintptr_t RADAR_REGION = 0xBA8300, RADAR_REGION_SIZE = 0x2070;   // 0xBA8300..0xBAA370: todo el estado propio de CRadar (blips, textos, sprites, estaticas)
constexpr uintptr_t PLAYERS = 0xB7CD98, PLAYERS_SIZE = 0x320, PLAYER_FOCUS = 0xB7CD74;
constexpr uintptr_t MODEL_INFO_PTRS = 0xA9B0C8;
constexpr int32 FAKE_MODEL_A = 19995, FAKE_MODEL_B = 19996, FAKE_MODEL_C = 19997, NEVADA_MODEL = 553;   // ranuras de ModelInfo que se intercambian y se restauran
constexpr uintptr_t PICKUPS = 0x9788C0;
constexpr uintptr_t CAM_REGION = 0xB6F028, CAM_REGION_SIZE = 0x9F8;        // TheCamera (hasta m_mCameraMatrix y un poco mas)
constexpr uintptr_t ZONE_REGION = 0xBA1DE0, ZONE_REGION_SIZE = 0x4960;      // TotalNumberOfZoneInfos, ZoneInfoArray, ZonesRevealed, zonas de navegacion
constexpr uintptr_t FE_REGION = 0xBA6740, FE_REGION_SIZE = 0xE0;            // FrontEndMenuManager (campos del mapa/raton/blips)
struct Zone { uintptr_t address; size_t size; };
inline std::vector<Zone> globals = {
    {RADAR_REGION, RADAR_REGION_SIZE}, {0x8D0920, 0x14}, {FE_REGION, FE_REGION_SIZE}, {ZONE_REGION, ZONE_REGION_SIZE},
    {0xB7CB4C, 4}, {0xB7CB5C, 4}, {0xB7CB7C, 4}, {0xB7CB84, 4}, {0xB7CB49, 1},     // CTimer: contador de fotogramas, paso, tiempo en pausa, tiempo, pausa del usuario
    {0xC17044, 8}, {0xC97B24, 4}, {0xC80460, 4},                                   // RsGlobal.maximumWidth/Height, motor RW, NearScreenZ
    {0xA444A0, 8},                                                                 // CTheScripts: bDisplayHud, HideAllFrontEndMapBlips, RadarZoomValue, bPlayerIsOffTheMap...
    {0x859CF8, 1}, {0x96AB90, 1}, {0x9654B0, 1}, {0xB72914, 4}, {0x8A5E50, 4},     // DbgFlag, bGangWarsActive, ms_disableStreaming, currArea, n2PlayerPedInFocus
    {PLAYERS, PLAYERS_SIZE}, {PLAYER_FOCUS, 1},
    {CAM_REGION, CAM_REGION_SIZE}, {0xB73418, 0x28},                               // TheCamera y estado del raton (nuevo + antiguo)
    {0xC8800C, 4}, {0x96A7D8, 4}, {0xB74490, 0x10},                                // punteros de pools: TXD, EntryExit, ped/vehiculo/edificio/objeto
    {MODEL_INFO_PTRS + 4u * uint32(FAKE_MODEL_A), 4}, {MODEL_INFO_PTRS + 4u * uint32(FAKE_MODEL_B), 4}, {MODEL_INFO_PTRS + 4u * uint32(FAKE_MODEL_C), 4},
    {MODEL_INFO_PTRS + 4u * uint32(NEVADA_MODEL), 4},
    {PICKUPS, 8 * 0x20},                                                           // los primeros pickups (Draw3dMarkers)
};
inline std::vector<std::vector<uint8>> savedGlobals;
inline void SaveGlobals() { savedGlobals.clear(); for (auto& z : globals) savedGlobals.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(z.address), z.size)); }
inline void RestoreGlobals() { for (size_t j = 0; j < globals.size(); ++j) if (globals[j].size) std::memcpy(reinterpret_cast<void*>(globals[j].address), savedGlobals[j].data(), savedGlobals[j].size()); }

// Sigue la cadena de saltos E9 (thunk incremental -> cuerpo).
// Body() no entra en el EXE: si el destino del salto cae en [0x400000, 0x1000000) (hook desactivado) se queda en la ultima direccion de la DLL.
inline bool InExe(uintptr_t a) { return a >= 0x400000 && a < 0x1000000; }
inline uintptr_t Body(uintptr_t a) {
    for (int j = 0; j < 16 && *reinterpret_cast<uint8*>(a) == 0xE9; ++j) { const uintptr_t t = a + 5 + *reinterpret_cast<int32*>(a + 1); if (InExe(t)) break; a = t; }
    return a;
}
inline Hook* FindHook(const char* cls, const char* fn) {
    Hook* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if (c.Name() == cls) for (auto& h : c.Items()) if (h->Name() == fn) r = h.get(); });
    return r;
}

// Ejecuta un caso: activa/desactiva el hook, fija el x87 a 24 bits como el juego y recoge la traza.
template<class F> inline json Run(Hook* hook, bool cand, uintptr_t addr, F&& body) {
    nrec = 0; recOverflow = false;
    hook->State(cand);
    const bool switches = ReversingProbe::JumpsToDll(addr) == cand;   // despues de State(), no antes
    const unsigned savedFP = _controlfp(0, 0);
    _controlfp(_PC_24, _MCW_PC);
    json out = body();
    const unsigned fp = _controlfp(0, 0);
    _controlfp(savedFP, _MCW_PC | _MCW_RC);
    out["switches"] = switches; out["fp"] = fp; out["trace"] = Recs(); out["overflow"] = recOverflow;
    return out;
}

// --- colaboradores simulados: registro y activacion por funcion probada ---
struct Site { const char* name; uintptr_t exeAddr, dll, stub, dllStub, body; };
inline std::vector<Site> sites;
template<class F> inline void AddSite(const char* name, uintptr_t exeAddr, uintptr_t dll, F stub) {
    const auto st = std::bit_cast<uintptr_t>(stub);
    sites.push_back({ name, exeAddr, dll, st, st, 0 });
}
// Variante con doble distinto para el lado de la DLL (otra convencion de llamada o EDX conservado en el EXE)
template<class F, class G> inline void AddSite(const char* name, uintptr_t exeAddr, uintptr_t dll, F stub, G dllStub) {
    sites.push_back({ name, exeAddr, dll, std::bit_cast<uintptr_t>(stub), std::bit_cast<uintptr_t>(dllStub), 0 });
}
template<class Fp> inline uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v; std::set<uintptr_t> seen;
    void Add(uintptr_t at, uintptr_t stub) { if (at && seen.insert(at).second) v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at, reinterpret_cast<void*>(stub))); }
    ~Patches() { while (!v.empty()) v.pop_back(); }
};
// Activa los colaboradores indicados (por nombre). Los Body() ya estan calculados.
inline std::unique_ptr<Patches> Use(std::initializer_list<const char*> names) {
    auto p = std::make_unique<Patches>();
    for (auto* n : names) {
        bool found = false;
        for (auto& s : sites) if (std::strcmp(s.name, n) == 0) {
            found = true; p->Add(s.exeAddr, s.stub);
            if (s.dll) { p->Add(s.dll, s.dllStub); if (s.body != s.dll) p->Add(s.body, s.dllStub); }
        }
        if (!found) Write({ {"event", "error"}, {"reason", "site not found"}, {"site", n} });
    }
    return p;
}

using CaseFn = json (*)(unsigned, bool, Hook*, uintptr_t);
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
}
