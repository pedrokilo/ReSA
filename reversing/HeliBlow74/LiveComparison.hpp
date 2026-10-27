#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <new>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Heli.h"
#include "Entity/Ped/Ped.h"
#include "Camera.h"
#include "FireManager.h"
#include "CarCtrl.h"
#include "Wanted.h"
#include "PlayerInfo.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par HeliBlow74 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CHeli::BlowUpCar (0x6C6D30, hook VIRTUAL, ranura 41 de la vtable 0x871680; thiscall (causante, bool), `ret 8`). Original = bytes del EXE con el hook V apagado; candidato = DLL con SOLO ese hook encendido
// (la entrada del EXE salta al cuerpo de la DLL y la ranura de la vtable del EXE apunta a el). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (con hook RH, apagado durante
// el banco): rand 0x821B1E (sin hook), FindPlayerPed 0x56E210, FindPlayerVehicle 0x56E0D0, CStats::IncrementStat 0x55C180, CVisibilityPlugins::SetClumpForAllAtomicsFlag 0x7322F0, CDamageManager::FuckCarCompletely 0x6C25D0,
// CAutomobile::SetBumperDamage 0x6B1350 / SetDoorDamage 0x6B1600 / SpawnFlyingComponent 0x6A8580, RwFrameForAllObjects 0x7F1200 (sin hook; la envoltura de la DLL llama a la direccion del EXE), CCamera::CamShake 0x50A9F0,
// CVehicle::KillPedsInVehicle 0x6D1C80 / ChangeLawEnforcerState 0x6D2330, CFireManager::StartFire 0x53A050, CDarkel::RegisterCarBlownUpByPlayer 0x43DF20 y CExplosion::AddExplosion 0x736A50.
// El falso de RwFrameForAllObjects no ejecuta la funcion de retorno (0x6A0750 en el EXE, GetCurrentAtomicObjectCB en la DLL; se registra como «la funcion de retorno esperada»): escribe el atomico (o nulo) en el destino,
// como haria ella. Esa funcion de retorno no se valida aqui (la valida el banco de CAutomobile). Los globales de la funcion (jugador, bandera de noticias, contadores de ambulancias y camiones, hora) se fijan por caso y se restauran.

namespace Hb74Layout {
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(sizeof(CHeli) == Hb74Fix::kHeliWin, "la arena del helicoptero es un CHeli completo");
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCarMission) == 0x3BA, "CAutoPilot::m_nCarMission en +0x3BA");
static_assert(offsetof(CAutomobile, m_damageManager) == 0x5A0 && offsetof(CAutomobile, m_aCarNodes) + 5 * sizeof(void*) == 0x65C && offsetof(CAutomobile, autoFlags) == 0x868, "CAutomobile: danos en +0x5A0, nodo 5 en +0x65C, banderas en +0x868");
static_assert(offsetof(CVehicle, m_fHealth) == 0x4C0 && offsetof(CVehicle, m_nTimeWhenBlowedUp) == 0x4D8 && offsetof(CVehicle, m_wBombTimer) == 0x4DE, "CVehicle: salud +0x4C0, hora de la explosion +0x4D8, temporizador de la bomba +0x4DE");
static_assert(offsetof(CPlayerInfo, m_nHavocCaused) == 0x140 && offsetof(CPlayerInfo, m_fCurrentChaseValue) == 0x148 && sizeof(CPlayerInfo) == 0x190, "CPlayerInfo: caos +0x140, persecucion +0x148, tamano 0x190");
// Posiciones de los campos de bits y direcciones de los globales: se comprueban en ejecucion (why = comprobaciones fallidas)
inline std::string why;
inline bool LayoutOK() {
    alignas(16) static std::array<uint8, 0xA40> mem{};
    why.clear();
    auto* h = reinterpret_cast<CHeli*>(mem.data());
    auto chk = [&](const char* name, bool cond) { if (!cond) { why += name; why += ';'; } };
    auto one = [&](const char* name, auto set, size_t byte, uint8 value) {
        mem.fill(0); set();
        size_t other = 0; for (size_t i = 0x400; i < 0x880; ++i) other += mem[i] != 0;
        chk(name, mem[byte] == value && other == 1);
    };
    one("canBeDamaged", [&] { h->vehicleFlags.bCanBeDamaged = true; }, 0x42A, 0x20);
    one("ambulance", [&] { h->vehicleFlags.bIsAmbulanceOnDuty = true; }, 0x428, 0x02);
    one("firetruck", [&] { h->vehicleFlags.bIsFireTruckOnDuty = true; }, 0x428, 0x04);
    one("engine", [&] { h->vehicleFlags.bEngineOn = true; }, 0x428, 0x10);
    one("lights", [&] { h->vehicleFlags.bLightsOn = true; }, 0x428, 0x40);
    one("siren", [&] { h->vehicleFlags.bSirenOrAlarm = true; }, 0x42D, 0x80);
    one("taxi", [&] { h->autoFlags.bTaxiLight = true; }, 0x868, 0x01);
    one("bomb", [&] { h->m_nBombOnBoard = 7; }, 0x4A8, 0x07);
    one("override", [&] { h->m_nOverrideLights = 3; }, 0x4A8, 0x18);
    mem.fill(0);
    h->physicalFlags.bRenderScorched = true;
    chk("scorched", mem[0x43] == 0x20 && mem[0x40] == 0 && mem[0x41] == 0 && mem[0x42] == 0);              // bit 29 de +0x40
    mem.fill(0);
    h->SetStatus(STATUS_WRECKED);
    chk("status", mem[0x36] == 0x28);                                                                     // estado en los bits 3..7 de +0x36
    mem.fill(0);
    h->m_nModelIndex = 0x1E8;
    chk("model", mem[0x22] == 0xE8 && mem[0x23] == 0x01);                                                  // indice de modelo (palabra) en +0x22
    mem.fill(0);
    h->m_bUsesCollision = true; h->m_bIsVisible = true;
    chk("entityFlags", mem[0x1C] == 0x81);                                                                // bits 0 y 7 de +0x1C
    chk("camera", uintptr_t(&TheCamera) == 0xB6F028);
    chk("fire", uintptr_t(&gFireManager) == 0xB71F80);
    chk("news", uintptr_t(&CWanted::UseNewsHeliInAdditionToPolice) == 0xB7CB8C);
    chk("counters", uintptr_t(&CCarCtrl::NumAmbulancesOnDuty) == 0x9690A8 && uintptr_t(&CCarCtrl::NumFireTrucksOnDuty) == 0x9690AC);
    chk("players", uintptr_t(&CWorld::PlayerInFocus) == 0xB7CD74 && uintptr_t(&CWorld::Players[0]) == 0xB7CD98);
    return why.empty();
}
}

namespace Hb74Live {
using namespace Hb74Fix;
inline constexpr uintptr_t kEntry = 0x6C6D30;
inline constexpr uintptr_t kVtable = 0x871680;
inline constexpr unsigned kSlot = 41;
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint8& G8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }

// ================================================================================================ arenas
template<size_t N, size_t G = kGuard> struct Arena {
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj() const { return uintptr_t(m.data()) + G; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + G + off, &v, 4); }
    void Put16(size_t off, u32 v) { const uint16 w = uint16(v); memcpy(m.data() + G + off, &w, 2); }
    void Put8(size_t off, u32 v) { m[G + off] = uint8(v); }
    json Changes() const { return ArenaChanges(m.data(), r.data(), N, G); }
    void Tagged() const { TagAdd(m.data(), N); }
};
inline Arena<kArena(kHeliWin)> heliA;
inline Arena<kArena(kMatWin)> matA;
inline Arena<kArena(kEntWin)> pedP, vehP, otherE;
inline Arena<kArena(kAtomWin)> atomA;

inline json CloseSide(Env& env, Side& side, json obs) {
    auto fp = NormalizeFp(side.fp);
    obs["globals_hash"] = RegionsHash();
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

// ================================================================================================ BlowUpCar (0x6C6D30)
struct State { const C* c{}; };
inline State g;
// Eventos: 1 FindPlayerPed(id); 2 FindPlayerVehicle(id, bool); 3 rand(); 4 IncrementStat(estadistica, float); 5 SetClumpForAllAtomicsFlag(clump, bandera); 6 FuckCarCompletely(this, bool);
// 7 SetBumperDamage(this, panel, bool); 8 SetDoorDamage(this, puerta, bool); 9 SpawnFlyingComponent(this, nodo, tipo); 10 RwFrameForAllObjects(marco, funcion de retorno esperada?, destino*);
// 11 CamShake(camara, fuerza, x, y, z); 12 KillPedsInVehicle(this); 13 ChangeLawEnforcerState(this, bool); 14 StartFire(gestor, objetivo, creador, tamano, bool, tiempo, generaciones);
// 15 RegisterCarBlownUpByPlayer(vehiculo, jugador); 16 AddExplosion(victima, creador, tipo, x, y, z, vida, sonido, sacudida, visible). Los punteros dentro de una arena se etiquetan.
inline u32 StackMark(const void* p) { return Tag(p) == u32(uintptr_t(p)) ? 0xF00D0001u : Tag(p); }
inline u32 PlayerPtr(u32 kind, u32 raw, const uintptr_t arenaPtr) { return kind == 0 ? u32(arenaPtr) : kind == 1 ? 0u : kind == 2 ? u32(otherE.Obj()) : raw; }
inline u32 __cdecl FakeRand() { PushEv(3, 0); return g.c->rnd; }
inline u32 __cdecl FakeFindPed(u32 id) { Ev& e = PushEv(1, 1); e.v[0] = id; return PlayerPtr(g.c->fppKind, g.c->fppRaw, pedP.Obj()); }
inline u32 __cdecl FakeFindVeh(u32 id, u32 flag) { Ev& e = PushEv(2, 2); e.v[0] = id; e.v[1] = flag; return PlayerPtr(g.c->fpvKind, g.c->fpvRaw, vehP.Obj()); }
inline void __cdecl FakeStat(u32 stat, u32 value) { Ev& e = PushEv(4, 2); e.v[0] = stat; e.v[1] = value; }
inline void __cdecl FakeSetFlag(u32 clump, u32 flag) { Ev& e = PushEv(5, 2); e.v[0] = clump; e.v[1] = flag; }
inline void __fastcall FakeFuck(void* self, void*, u32 flag) { Ev& e = PushEv(6, 2); e.v[0] = Tag(self); e.v[1] = flag; }
inline void __fastcall FakeBumper(void* self, void*, u32 panel, u32 flag) { Ev& e = PushEv(7, 3); e.v[0] = Tag(self); e.v[1] = panel; e.v[2] = flag; }
inline void __fastcall FakeDoor(void* self, void*, u32 door, u32 flag) { Ev& e = PushEv(8, 3); e.v[0] = Tag(self); e.v[1] = door; e.v[2] = flag; }
inline u32 __fastcall FakeSpawn(void* self, void*, u32 node, u32 type) { Ev& e = PushEv(9, 3); e.v[0] = Tag(self); e.v[1] = node; e.v[2] = type; return 0u; }
inline uintptr_t gCallbackDll{}, gCallbackThunk{};   // GetCurrentAtomicObjectCB de la DLL: cuerpo y enlace incremental (la direccion que recibe el falso es la del enlace)
inline u32 __cdecl FakeFrames(u32 frame, u32 callback, u32* data) {
    Ev& e = PushEv(10, 3); e.v[0] = frame; e.v[1] = (callback == 0x6A0750u || callback == gCallbackDll || callback == gCallbackThunk) ? 0xCB000001u : callback; e.v[2] = StackMark(data);
    *data = g.c->atomic ? u32(atomA.Obj()) : 0u;
    return frame;
}
inline void __fastcall FakeCam(void* self, void*, u32 strength, u32 x, u32 y, u32 z) { Ev& e = PushEv(11, 5); e.v[0] = Tag(self); e.v[1] = strength; e.v[2] = x; e.v[3] = y; e.v[4] = z; }
inline void __fastcall FakeKill(void* self, void*) { Ev& e = PushEv(12, 1); e.v[0] = Tag(self); }
inline void __fastcall FakeLaw(void* self, void*, u32 flag) { Ev& e = PushEv(13, 2); e.v[0] = Tag(self); e.v[1] = flag; }
inline u32 __fastcall FakeFire(void* self, void*, u32 target, u32 creator, u32 size, u32 a3, u32 time, u32 gens) {
    Ev& e = PushEv(14, 7); e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(target)); e.v[2] = Tag(uintptr_t(creator)); e.v[3] = size; e.v[4] = a3; e.v[5] = time; e.v[6] = gens;
    return 0u;
}
inline void __cdecl FakeDarkel(void* vehicle, u32 player) { Ev& e = PushEv(15, 2); e.v[0] = Tag(vehicle); e.v[1] = player; }
inline void __cdecl FakeExplosion(u32 victim, u32 creator, u32 type, u32 x, u32 y, u32 z, u32 life, u32 sound, u32 shake, u32 visible) {
    Ev& e = PushEv(16, 10); e.v[0] = Tag(uintptr_t(victim)); e.v[1] = Tag(uintptr_t(creator)); e.v[2] = type; e.v[3] = x; e.v[4] = y; e.v[5] = z; e.v[6] = life; e.v[7] = sound; e.v[8] = shake; e.v[9] = visible;
}

inline constexpr uintptr_t kPlayers = 0xB7CD98, kStep = 0x190;
inline void CaseBlow(Env& env, Totals& t, unsigned n) {
    const C c = MakeC(n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        heliA.Fill(c.seed * 13u + 1u); matA.Fill(c.seed * 13u + 2u); pedP.Fill(c.seed * 13u + 3u); vehP.Fill(c.seed * 13u + 4u); otherE.Fill(c.seed * 13u + 5u); atomA.Fill(c.seed * 13u + 6u);
        heliA.Put32(0x14, c.matPresent ? u32(matA.Obj()) : 0u);
        for (int i = 0; i < 3; ++i) { if (c.matPresent) matA.Put32(0x30 + 4 * i, c.pos[i]); else heliA.Put32(4 + 4 * i, c.pos[i]); }
        heliA.Put32(0x1C, c.ent1c); heliA.Put16(0x22, c.model); heliA.Put8(0x36, c.status); heliA.Put32(0x40, c.phys); heliA.Put8(0x3BA, c.mission);
        heliA.Put32(0x428, c.flags428); heliA.Put8(0x42A, c.flags42A); heliA.Put8(0x42D, c.flags42D); heliA.Put8(0x4A8, c.b4a8); heliA.Put8(0x868, c.b868);
        heliA.Snap(); matA.Snap(); pedP.Snap(); vehP.Snap(); otherE.Snap(); atomA.Snap();
        TagClear(); heliA.Tagged(); matA.Tagged(); pedP.Tagged(); vehP.Tagged(); otherE.Tagged(); atomA.Tagged();
        G32(kTimeAddr) = c.time; G8(kFocusAddr) = uint8(c.focus); G8(kNewsAddr) = uint8(c.news); G32(kAmbAddr) = c.ambulances; G32(kTruckAddr) = c.trucks;
        for (int i = 0; i < 2; ++i) { G32(kPlayers + i * kStep + 0x140) = c.havoc[i]; G32(kPlayers + i * kStep + 0x148) = c.chase[i]; }
        g.c = &c; nev = 0;
        const u32 damager = c.dmgKind == 0 ? 0u : c.dmgKind == 1 ? u32(pedP.Obj()) : c.dmgKind == 2 ? u32(vehP.Obj()) : c.dmgKind == 3 ? u32(otherE.Obj()) : c.dmgKind == 4 ? c.dmgRaw : u32(heliA.Obj());
        const u32 args[2]{damager, c.hide};
        sides[s] = Execute(env, 0, kEntry, candidate, [&](uintptr_t entry) { return Gate(entry, args, 2, c.eax, u32(heliA.Obj()), c.edx, c.stack, 0); });
        json obs;
        json globals = json::object();
        globals["time"] = G32(kTimeAddr); globals["news"] = G8(kNewsAddr); globals["ambulances"] = G32(kAmbAddr); globals["trucks"] = G32(kTruckAddr);
        globals["havoc"] = json::array({G32(kPlayers + 0x140), G32(kPlayers + kStep + 0x140)}); globals["chase"] = json::array({G32(kPlayers + 0x148), G32(kPlayers + kStep + 0x148)});
        obs["globals"] = globals;
        obs["changes"] = json::array({heliA.Changes(), matA.Changes(), pedP.Changes(), vehP.Changes(), otherE.Changes(), atomA.Changes()});
        obs["events"] = EvsJson();
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[0], n, input, rows, sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
        for (const auto& s : env.stubs[f]) {
            if (s.pair.hooked) ready &= s.pair.dll != 0 && ModuleOf(s.pair.dll) == env.asi;
            else ready &= s.pair.dll == 0 || ModuleOf(s.pair.dll) == env.asi;
        }
    }
    const bool layout = Hb74Layout::LayoutOK();
    ready &= layout;
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases}, {"layout_ok", layout}, {"layout_why", Hb74Layout::why},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < kCases; ++c) CaseBlow(env, totals, c);
            Write({{"event", "function_summary"}, {"function", kFn[f]}, {"cases", kCases}, {"failures", totals.failures - before}, {"state_restored", env.WatchesIntact() && RegionsIntact()}});
        }
    }
    RestoreRegions();
    const bool entriesIntact = env.WatchesIntact() && RegionsIntact();
    bool stateRestored = entriesIntact;
    for (auto& r : env.collab) if (r) stateRestored &= r->Restore();
    for (auto& r : env.route) if (r) stateRestored &= r->Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntact}});
    Write({{"event", ready && totals.passed && stateRestored ? "complete" : "error"}, {"pairs", totals.pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntact}});
    RestoreFloatingState(); savedFPActive = false;
}

// Busca el hook Simple registrado para una direccion del EXE (categoria y nombre), para crear su ruta (apagada) y obtener el cuerpo de la DLL.
inline bool FindHook(uintptr_t exe, std::string& cat, std::string& name) {
    bool found = false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items())
            if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
            }
    });
    return found;
}
// Colaborador con hook: se crea su ruta (el hook queda apagado) y se toma el cuerpo de la DLL; sin hook (dll = 0) la DLL llama a la direccion del EXE.
inline Pair MakeCollab(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name;
    if (FindHook(exe, cat, name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}});
        if (env.collab.back()->Registered()) { p.dll = Body(env.collab.back()->Body()); p.hooked = true; }
    }
    return p;
}
inline bool Prologue(uintptr_t f) { const auto* b = reinterpret_cast<const uint8*>(f); if (b[0] == 0x8B && b[1] == 0xFF) b += 2; return b[0] == 0x55 && b[1] == 0x8B && b[2] == 0xEC; }
// Primera llamada (E8) de `fn` (dentro del ASI) a una funcion del ASI con prologo push ebp / mov ebp,esp: el cuerpo real del constructor que llama la envoltura `Constructor` del hook.
inline uintptr_t FirstDllCall(uintptr_t asi, uintptr_t fn) {
    fn = Body(fn); if (!fn || ModuleOf(fn) != asi) return 0;
    for (uintptr_t p = fn; p < fn + 0x300; ++p) {
        if (*reinterpret_cast<const uint8*>(p) != 0xE8) continue;
        const uintptr_t t = p + 5 + *reinterpret_cast<const int32*>(p + 1);
        if (ModuleOf(t) == asi && Prologue(Body(t))) return Body(t);
    }
    return 0;
}
template<class M> inline uintptr_t MemberBody(M member) { return Body(std::bit_cast<uintptr_t>(member)); }

inline ReversibleHooks::ReversibleHook::Base* FindHookByName(const char* cls, const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if (c.Name() == cls) for (auto& h : c.Items()) if (h->Name() == fn) r = h.get(); });
    return r;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "heliblow74-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "heliblow74-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("heliblow74"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    env.vtSlot = kVtable + 4 * kSlot;
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta VIRTUAL: el cuerpo del candidato es el destino vivo del hook V. Se construye ANTES que nada; el constructor deja el hook apagado.
    {
        auto* hook = FindHookByName("CHeli", "BlowUpCar");
        uintptr_t body = 0;
        if (hook && !strcmp(hook->Symbol(), "V")) body = uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
        else ready = false;
        env.route[0].reset(new HookRoute("CHeli", "BlowUpCar", kEntry, body));
        registered[kFn[0]] = env.route[0]->Registered(); bodies[kFn[0]] = env.route[0]->Body() != 0;
        ready &= env.route[0]->Registered() && env.route[0]->Body() && ModuleOf(env.route[0]->Body()) == env.asi;
        ownHooks[kFn[0]] = hook ? json({{"class", "CHeli"}, {"name", "BlowUpCar"}, {"address", kEntry}, {"type", hook->Symbol()}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    gCallbackThunk = reinterpret_cast<uintptr_t>(&GetCurrentAtomicObjectCB);
    gCallbackDll = Body(gCallbackThunk);
    const Pair pRand{0x821B1E, 0, false};
    const Pair pPed = MakeCollab(env, 0x56E210, hooksFound), pVeh = MakeCollab(env, 0x56E0D0, hooksFound), pStat = MakeCollab(env, 0x55C180, hooksFound), pFlag = MakeCollab(env, 0x7322F0, hooksFound);
    const Pair pFuck = MakeCollab(env, 0x6C25D0, hooksFound), pBump = MakeCollab(env, 0x6B1350, hooksFound), pDoor = MakeCollab(env, 0x6B1600, hooksFound), pSpawn = MakeCollab(env, 0x6A8580, hooksFound);
    const Pair pFrames = MakeCollab(env, 0x7F1200, hooksFound);   // sin hook: la envoltura de la DLL llama a la direccion del EXE
    const Pair pCam = MakeCollab(env, 0x50A9F0, hooksFound), pKill = MakeCollab(env, 0x6D1C80, hooksFound), pLaw = MakeCollab(env, 0x6D2330, hooksFound), pFire = MakeCollab(env, 0x53A050, hooksFound);
    const Pair pDarkel = MakeCollab(env, 0x43DF20, hooksFound), pExplosion = MakeCollab(env, 0x736A50, hooksFound);
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    env.stubs[0] = {S(pRand, reinterpret_cast<void*>(&FakeRand)), S(pPed, reinterpret_cast<void*>(&FakeFindPed)), S(pVeh, reinterpret_cast<void*>(&FakeFindVeh)), S(pStat, reinterpret_cast<void*>(&FakeStat)),
        S(pFlag, reinterpret_cast<void*>(&FakeSetFlag)), S(pFuck, reinterpret_cast<void*>(&FakeFuck)), S(pBump, reinterpret_cast<void*>(&FakeBumper)), S(pDoor, reinterpret_cast<void*>(&FakeDoor)),
        S(pSpawn, reinterpret_cast<void*>(&FakeSpawn)), S(pFrames, reinterpret_cast<void*>(&FakeFrames)), S(pCam, reinterpret_cast<void*>(&FakeCam)), S(pKill, reinterpret_cast<void*>(&FakeKill)),
        S(pLaw, reinterpret_cast<void*>(&FakeLaw)), S(pFire, reinterpret_cast<void*>(&FakeFire)), S(pDarkel, reinterpret_cast<void*>(&FakeDarkel)), S(pExplosion, reinterpret_cast<void*>(&FakeExplosion))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"ninguno: GetCurrentAtomicObjectCB (0x6A0750) no se ejecuta, el falso de RwFrameForAllObjects escribe su resultado"})}, {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry, 16); env.watches.emplace_back(env.vtSlot, 4);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
