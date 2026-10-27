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
#include "Core/Matrix.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par HeliGen74 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CHeli::GenerateHeli (0x6C6520, hook S, cdecl (objetivo, bool), `ret` del llamador; devuelve el helicoptero en EAX). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo
// de la DLL (con hook RH, apagado durante el banco): rand 0x821B1E (sin hook; la DLL lo llama por su direccion), CVehicle::operator new 0x6E2D50, el constructor de CHeli 0x6C4190 (el cuerpo real se localiza siguiendo
// la primera llamada de la envoltura `Constructor` del hook; la reserva se construye sin comprobar el nulo del constructor, como el EXE), CWorld::FindPlayerPed 0x56E210, CWorld::FindGroundZForCoord 0x569660,
// CMatrix::SetTranslate 0x59AF40 (sin hook en la tabla: la DLL llama a la funcion miembro), CWorld::Add 0x563220 y CVehicle::SetUpDriver 0x6D1A50. El helicoptero es la arena de un CHeli completo.
// Corren de verdad en ambos lados: nada mas (las demas operaciones son aritmetica propia de la funcion). El orden de las llamadas se registra en los eventos.

namespace Hg74Layout {
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(sizeof(CHeli) == Hg74Fix::kHeliWin, "la arena del helicoptero es un CHeli completo");
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_TargetEntity) == 0x41C && offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCarMission) == 0x3BA
    && offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCruiseSpeed) == 0x3D0, "CAutoPilot: objetivo +0x41C, mision +0x3BA, velocidad de crucero +0x3D0");
static_assert(offsetof(CHeli, m_fMaxAltitude) == 0x9A8 && offsetof(CHeli, m_fMinAltitude) == 0x9B0 && offsetof(CAutomobile, m_fHeliRotorSpeed) == 0x84C, "CHeli: alturas en +0x9A8/+0x9B0 y velocidad del rotor en +0x84C");
// Posiciones de los campos de bits y de las filas de la matriz: se comprueban en ejecucion
inline bool LayoutOK() {
    alignas(16) static std::array<uint8, 0xA40> mem{};
    bool ok = true;
    mem.fill(0);
    reinterpret_cast<CHeli*>(mem.data())->vehicleFlags.bIsLocked = true;
    ok &= mem[0x428] == 0x08 && mem[0x429] == 0 && mem[0x42A] == 0;                  // bit 3 de +0x428
    mem.fill(0);
    mem[0x36] = 0x05;
    reinterpret_cast<CHeli*>(mem.data())->SetStatus(STATUS_PHYSICS);
    ok &= mem[0x36] == 0x1D;                                                          // estado en los bits 3..7 de +0x36, tipo en 0..2
    mem.fill(0);
    auto* m = reinterpret_cast<CMatrix*>(mem.data());
    ok &= uintptr_t(&m->GetRight().x) == uintptr_t(mem.data()) && uintptr_t(&m->GetForward().x) == uintptr_t(mem.data()) + 0x10 && uintptr_t(&m->GetUp().x) == uintptr_t(mem.data()) + 0x20;
    ok &= uintptr_t(&m->GetPosition().x) == uintptr_t(mem.data()) + 0x30;
    return ok;
}
}

namespace Hg74Live {
using namespace Hg74Fix;
inline constexpr uintptr_t kEntry = 0x6C6520;
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }

// ================================================================================================ arenas
template<size_t N, size_t G = kGuard> struct Arena {
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj() const { return uintptr_t(m.data()) + G; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + G + off, &v, 4); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + G + off, 4); return v; }
    void Hide32(size_t off) { memcpy(m.data() + G + off, r.data() + G + off, 4); }   // el campo es un puntero: se observa etiquetado y no entre los bytes cambiados
    json Changes() const { return ArenaChanges(m.data(), r.data(), N, G); }
    void Tagged() const { TagAdd(m.data(), N); }
};
inline Arena<kArena(kHeliWin)> heliA;
inline Arena<kArena(kTgtWin)> tgtA;
inline Arena<kArena(kMatWin)> matH, matT;

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

// ================================================================================================ GenerateHeli (0x6C6520)
struct State { const C* c{}; };
inline State g;
// Eventos: 1 CVehicle::operator new(tamano); 2 ctor de CHeli(this, modelo, creado por); 3 rand(); 4 FindGroundZForCoord(x, y); 5 CMatrix::SetTranslate(this, x, y, z); 6 FindPlayerPed(id);
// 7 CWorld::Add(entidad); 8 SetUpDriver(this, tipo de ped, bool, bool). Los punteros dentro de una arena se etiquetan.
inline void* __cdecl FakeVehNew(size_t size) { Ev& e = PushEv(1, 1); e.v[0] = u32(size); return reinterpret_cast<void*>(heliA.Obj()); }
inline void* __fastcall FakeHeliCtor(void* self, void*, u32 model, u32 created) {
    Ev& e = PushEv(2, 3); e.v[0] = Tag(self); e.v[1] = model; e.v[2] = created;
    return self;
}
inline u32 __cdecl FakeRand() { PushEv(3, 0); return g.c->rnd; }
inline float __cdecl FakeGroundZ(u32 x, u32 y) { Ev& e = PushEv(4, 2); e.v[0] = x; e.v[1] = y; return F(g.c->gz); }
inline void __fastcall FakeSetTranslate(void* self, void*, u32 x, u32 y, u32 z) { Ev& e = PushEv(5, 4); e.v[0] = Tag(self); e.v[1] = x; e.v[2] = y; e.v[3] = z; }
inline u32 __cdecl FakeFindPlayer(u32 id) { Ev& e = PushEv(6, 1); e.v[0] = id; return g.c->player; }
inline void __cdecl FakeWorldAdd(void* entity) { Ev& e = PushEv(7, 1); e.v[0] = Tag(entity); }
inline u32 __fastcall FakeSetUpDriver(void* self, void*, u32 type, u32 a, u32 b) { Ev& e = PushEv(8, 4); e.v[0] = Tag(self); e.v[1] = type; e.v[2] = a; e.v[3] = b; return 0u; }

inline void CaseGen(Env& env, Totals& t, unsigned n) {
    const C c = MakeC(n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        heliA.Fill(c.seed * 13u + 1u); tgtA.Fill(c.seed * 13u + 2u); matH.Fill(c.seed * 13u + 3u); matT.Fill(c.seed * 13u + 4u);
        heliA.Put32(0x14, u32(matH.Obj()));      // matriz del helicoptero (la deja puesta el constructor, aqui falso)
        tgtA.Put32(0x14, c.tgtMat ? u32(matT.Obj()) : 0u);
        for (int i = 0; i < 3; ++i) { if (c.tgtMat) matT.Put32(0x30 + 4 * i, c.base[i]); else tgtA.Put32(4 + 4 * i, c.base[i]); }
        heliA.Snap(); tgtA.Snap(); matH.Snap(); matT.Snap();
        TagClear(); heliA.Tagged(); tgtA.Tagged(); matH.Tagged(); matT.Tagged();
        g.c = &c; nev = 0;
        const u32 args[2]{u32(tgtA.Obj()), c.news};
        sides[s] = Execute(env, 0, kEntry, candidate, [&](uintptr_t entry) { return Gate(entry, args, 2, c.eax, c.ecx, c.edx, c.stack, 8); });
        json obs;
        obs["ret"] = Tag(uintptr_t(sides[s].reg.eax));
        obs["target_entity"] = Tag(uintptr_t(heliA.Get32(0x41C)));   // m_autoPilot.m_TargetEntity: puntero a la entidad objetivo
        heliA.Hide32(0x41C);
        obs["changes"] = json::array({heliA.Changes(), tgtA.Changes(), matH.Changes(), matT.Changes()});
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
    const bool layout = Hg74Layout::LayoutOK();
    ready &= layout;
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases}, {"layout_ok", layout},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < kCases; ++c) CaseGen(env, totals, c);
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

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "heligen74-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "heligen74-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("heligen74"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de la funcion probada: se construye ANTES que nada; el constructor deja el hook apagado. El cuerpo de la DLL se calcula aqui, antes de crear las rutas de los colaboradores y los parches de entrada.
    {
        std::string cat, name;
        const bool found = FindHook(kEntry, cat, name);
        if (found) env.route[0].reset(new HookRoute(cat.c_str(), name.c_str(), kEntry, HookRoute::RegisteredTarget{}));
        registered[kFn[0]] = found && env.route[0]->Registered(); bodies[kFn[0]] = found && env.route[0]->Body() != 0;
        ready &= found && env.route[0]->Registered() && env.route[0]->Body() && ModuleOf(env.route[0]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[0]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntry}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    Pair pRand{0x821B1E, 0, false};
    const Pair pNew = MakeCollab(env, 0x6E2D50, hooksFound);
    Pair pCtor = MakeCollab(env, 0x6C4190, hooksFound); pCtor.dll = pCtor.dll ? FirstDllCall(env.asi, pCtor.dll) : 0;   // el cuerpo real del constructor: primera llamada de la envoltura `Constructor`
    const Pair pPlayer = MakeCollab(env, 0x56E210, hooksFound), pGround = MakeCollab(env, 0x569660, hooksFound);
    Pair pTranslate = MakeCollab(env, 0x59AF40, hooksFound);
    if (!pTranslate.dll) pTranslate.dll = MemberBody(static_cast<void (CMatrix::*)(CVector)>(&CMatrix::SetTranslate));   // sin hook en la tabla: la DLL llama a la funcion miembro
    const Pair pAdd = MakeCollab(env, 0x563220, hooksFound), pDriver = MakeCollab(env, 0x6D1A50, hooksFound);
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    env.stubs[0] = {S(pRand, reinterpret_cast<void*>(&FakeRand)), S(pNew, reinterpret_cast<void*>(&FakeVehNew)), S(pCtor, reinterpret_cast<void*>(&FakeHeliCtor)), S(pPlayer, reinterpret_cast<void*>(&FakeFindPlayer)),
        S(pGround, reinterpret_cast<void*>(&FakeGroundZ)), S(pTranslate, reinterpret_cast<void*>(&FakeSetTranslate)), S(pAdd, reinterpret_cast<void*>(&FakeWorldAdd)), S(pDriver, reinterpret_cast<void*>(&FakeSetUpDriver))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"ninguno: todo lo demas es aritmetica de la funcion"})}, {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry, 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
