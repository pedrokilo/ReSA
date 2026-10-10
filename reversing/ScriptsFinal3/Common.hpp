#pragma once
// Infraestructura comun del banco ScriptsFinal3 (estilo HudCamFinal2/PickWeap1): trazas sin reservas, zonas globales guardadas/restauradas,
// registro de colaboradores simulados (EntryPatch en la direccion del EXE + simbolo de la DLL + su cuerpo) activados por funcion probada.
// Solo traza el hilo del banco (owner/Mine()); los hilos ajenos (audio) reciben un resultado inocuo y no alteran nada.
// Todo el estado de CTheScripts vive en una sola zona contigua del EXE (0xA43C78..0xA95780): se guarda al empezar y se restaura tras CADA ejecucion.
#include <fstream>
#include <float.h>
#include <set>
#include <bit>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Scripts/TheScripts.h"
#include "Scripts/RunningScript.h"
#include "Scripts/Scripted2dEffects.h"
#include "LoadingScreen.h"
#include "Checkpoints.h"
#include "Checkpoint.h"
#include "3dMarkers.h"
#include "Shadows.h"
#include "Entity/Vehicle/Heli.h"
#include "Entity/Vehicle/Plane.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Object/Object.h"
#include "Entity/Building.h"
#include "FileMgr.h"
#include "GenericGameStorage.h"
#include "Streaming.h"
#include "TxdStore.h"
#include "Population.h"
#include "PedGroups.h"
#include "PedGroupMembership.h"
#include "PedIntelligence.h"
#include "Events/EventGroup.h"
#include "Events/EventScriptCommand.h"
#include "Tasks/TaskManager.h"
#include "CarCtrl.h"
#include "VehicleRecording.h"
#include "ScriptsForBrains.h"
#include "StreamedScripts.h"
#include "FireManager.h"
#include "World.h"
#include "Timer.h"
#include "Replay.h"
#include "Sprite2d.h"
#include "MenuManager.h"
#include "Models/ModelInfo.h"
#include "Models/BaseModelInfo.h"
#include "Pools/Pools.h"
#include "../ProbeRuntime.hpp"

namespace ScriptsFinal3Live {
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
// Coordenadas de mundo moderadas (los productos y sumas no desbordan)
inline float PickWorld(uint32& s) { return float(int32(Rnd(s) % 6001) - 3000) / 4.f; }

// --- registro de trazas sin reservas ---
struct Rec { uint32 tag; uint32 n; uint32 a[32]; };
inline std::array<Rec, 8192> recs{}; inline size_t nrec{};
inline void Push(uint32 t, std::initializer_list<uint32> v = {}) {
    if (!Mine() || nrec >= recs.size()) return;
    auto& r = recs[nrec++]; r = {}; r.tag = t; for (auto x : v) if (r.n < 32) r.a[r.n++] = x;
}
inline json Recs() { json r = json::array(); for (size_t i = 0; i < nrec; ++i) { json e = json::array({ recs[i].tag }); for (uint32 k = 0; k < recs[i].n; ++k) e.push_back(recs[i].a[k]); r.push_back(e); } return r; }
// Texto terminado en NUL -> huella
inline uint32 TextHash(const void* p) {
    if (!p) return 0;
    auto* b = static_cast<const uint8*>(p); size_t n = 0; while (n < 600 && b[n]) ++n;
    return uint32(Fnv(b, n)) ^ uint32(n * 0x01000193u);
}

// Etiquetas de la traza (una por colaborador simulado)
enum Tag : uint32 {
    T_REGREF = 1, T_CLEANREF, T_WORLDADD, T_WORLDREMOVE, T_REPLACEMODEL, T_SETUPBIG, T_CENTRE, T_BRAINSTART, T_BRAINRANGE, T_BRAINREQUEST, T_REQUESTMODEL,
    T_SAVE, T_LOAD, T_SETDIR, T_OPENFILE, T_READFILE, T_CLOSEFILE, T_INIT2D, T_INITSEQ, T_INITGROUPS, T_INITFRIENDS, T_INITGROUPQ, T_READSTREAMED,
    T_FINDTXD, T_REMOVETXD, T_SPRITEDELETE, T_MODELINFO, T_PLACEMARKER, T_DELETECP, T_MARKERSET, T_SEARCHCONE, T_GROUNDZ, T_INDICATOR, T_VERTLINE,
    T_DRAWRECT, T_SPRITEDRAW, T_SPRITEDRAW8, T_WINDOWEDTEXT, T_WINDOW, T_RWSTATE, T_RWLINE, T_SCRIPTPROCESS, T_UPSIDEDOWN, T_STUCKCARS, T_CLEANUPCOLL,
    T_NEWCHUNK, T_RECORDEVENT, T_SPHERES, T_SEARCHPROC, T_BRAINPROC, T_UPDMATRIX, T_UPDSIMPLE, T_UPDRWFRAME, T_ANIMBLOCK, T_ANIMGET, T_ANIMSET, T_ANIMCONVERT,
    T_SETCREATEDBY, T_JOINCAR, T_GETGROUP, T_ISFOLLOWER, T_REMOVEMEMBER, T_GETEVENT, T_TASKTYPE, T_ISPLAYER, T_PEDDELETABLE, T_VEHDELETABLE, T_REMOVEINTERESTING,
    T_STOPREC, T_SETVEHCREATED, T_REMOVEDRIVER, T_REMOVEPASSENGER, T_ISLAW, T_CHANGELAW, T_REMOVEREFS, T_REMOVEPED, T_FINDOBJECTS, T_PROCESSCOL, T_GETCOLMODEL,
    T_OBJDELETE, T_ANIMASSOC, T_PEDDTOR, T_VEHDTOR
};

// --- zonas globales que el banco toca (se guardan al empezar y se restauran tras cada ejecucion) ---
constexpr uintptr_t SCR_BASE = 0xA43C78, SCR_END = 0xA95780;        // todo el estado de CTheScripts (+ la tabla estatica de CColPoint de ClearSpaceForMissionEntity)
constexpr uintptr_t SCRIPT_SPACE = 0xA49960, SCRIPTS_ARRAY = 0xA8B430, SEARCHLIGHTS = 0xA94D68, WAITING = 0xA476B0;
constexpr uintptr_t POOLS_PTR = 0xB74490;                           // ped, vehiculo, edificio, objeto, dummy (5 punteros)
constexpr uintptr_t PLAYERS = 0xB7CD98, PLAYER_FOCUS = 0xB7CD74;
constexpr int32 FAKE_MODEL = 19995;                                 // ranura de ModelInfo sin usar (se intercambia y se restaura)
constexpr uintptr_t MODEL_INFO_PTRS = 0xA9B0C8, FAKE_MODEL_SLOT = MODEL_INFO_PTRS + 4u * uint32(FAKE_MODEL);
constexpr uintptr_t STREAM_INFO_LOAD = 0x964DF8;                    // CStreamingInfo[SCM + 0x6676] (20 bytes); el estado de carga es el byte +0x10
struct Zone { uintptr_t address; size_t size; };
inline std::vector<Zone> globals = {
    {SCR_BASE, SCR_END - SCR_BASE},
    {0xB6EC2C, 1},                                                   // gAllowScriptedFixedCameraCollision
    {POOLS_PTR, 0x14},                                               // punteros a los pools
    {0xB71F80, 0x964},                                               // gFireManager
    {0xC098D0, 0x18}, {0xC0AFF4, 0x40}, {0xC3A120, 0xC0},            // CPedGroups, CDecisionMakerTypes, CScripted2dEffects (ids y activos)
    {0xA43088, 1},                                                   // CReplay::Mode
    {0xB7CB5C, 4}, {0xB7CB58, 4}, {0xB7CB54, 4}, {0xB7CB84, 4},                   // CTimer: paso, paso anterior, milisegundos
    {0xC17044, 8},                                                   // RsGlobal.maximumWidth/Height
    {0xC97B24, 4},                                                   // RwEngineInstance
    {0xBB4A70, 2}, {0xC0EC24, 4},                                    // CObject::nNoTempObjects, CPopulation::ms_nTotalMissionPeds
    {PLAYER_FOCUS, 1}, {PLAYERS, 0x320},                             // jugadores
    {0xB72910, 1},                                                   // CGame::bMissionPackGame
    {STREAM_INFO_LOAD, 96 * 20 + 32},                                // estados de carga de los modelos de scripts de cerebro
    {0xC8800C, 4},                                                   // CTxdStore::ms_pTxdPool
    {FAKE_MODEL_SLOT, 4},                                            // ranura de ModelInfo falsa
};
inline std::vector<std::vector<uint8>> savedGlobals;
inline void SaveGlobals() { savedGlobals.clear(); for (auto& z : globals) savedGlobals.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(z.address), z.size)); }
inline void RestoreGlobals() { for (size_t j = 0; j < globals.size(); ++j) if (globals[j].size) std::memcpy(reinterpret_cast<void*>(globals[j].address), savedGlobals[j].data(), savedGlobals[j].size()); }
inline uint64 RegionHash() { return Fnv(reinterpret_cast<void*>(SCR_BASE), SCR_END - SCR_BASE); }

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
    nrec = 0;
    hook->State(cand);
    const bool switches = ReversingProbe::JumpsToDll(addr) == cand;   // despues de State(), no antes
    const unsigned savedFP = _controlfp(0, 0);
    _controlfp(_PC_24, _MCW_PC);
    json out = body();
    const unsigned fp = _controlfp(0, 0);
    _controlfp(savedFP, _MCW_PC | _MCW_RC);
    out["switches"] = switches; out["fp"] = fp; out["trace"] = Recs();
    return out;
}

// --- colaboradores simulados: registro y activacion por funcion probada ---
struct Site { const char* name; uintptr_t exeAddr, dll, stub, dllStub, body; };
inline std::vector<Site> sites;
template<class Fp> inline uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
template<class F> inline void AddSite(const char* name, uintptr_t exeAddr, uintptr_t dll, F stub) {
    const auto st = std::bit_cast<uintptr_t>(stub);
    sites.push_back({ name, exeAddr, dll, st, st, 0 });
}
// Variante con doble distinto para el lado de la DLL (distinta convencion de retorno o de argumentos)
template<class F, class G> inline void AddSite(const char* name, uintptr_t exeAddr, uintptr_t dll, F stub, G dllStub) {
    sites.push_back({ name, exeAddr, dll, std::bit_cast<uintptr_t>(stub), std::bit_cast<uintptr_t>(dllStub), 0 });
}
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
