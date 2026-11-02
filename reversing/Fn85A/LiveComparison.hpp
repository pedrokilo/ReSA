#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <new>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Garage.h"
#include "Garages.h"
#include "World.h"
#include "Entity/Vehicle/Vehicle.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn85A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Cinco funciones (hook S): JoinGroupFlags 0x609380, CGarage::RightModTypeForThisGarage 0x447720, CGarage::CalcDistToGarageRectangleSquared 0x447D80, CGarage::RestoreCarsForThisHideOut 0x448550 y
// CGarage::IsPlayerOutsideGarage 0x448E50. Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (con su hook RH apagado durante el banco):
// RestoreCarsForThisHideOut -> CStoredCar::RestoreCar 0x447E40 (thiscall) y CWorld::Add 0x563220 (cdecl); IsPlayerOutsideGarage -> FindPlayerVehicle 0x56E0D0 (cdecl, 2 dwords), FindPlayerPed 0x56E210
// (cdecl, 1 dword) y CGarage::IsEntityEntirelyOutside 0x448D30 (thiscall, ret 8). Registran sus argumentos EN ORDEN y devuelven lo que dice el caso. JoinGroupFlags, RightModTypeForThisGarage y
// CalcDistToGarageRectangleSquared no llaman a nada: corren de verdad (los ayudantes x87 de X87Math.cpp en la ultima).

namespace F85Layout {
static_assert(offsetof(CGarage, m_fLeftCoord) == 0x28 && offsetof(CGarage, m_fRightCoord) == 0x2C && offsetof(CGarage, m_fFrontCoord) == 0x30 && offsetof(CGarage, m_fBackCoord) == 0x34, "CGarage: rectangulo en +0x28..+0x34");
static_assert(offsetof(CGarage, m_nType) == 0x4C && sizeof(CGarage) == F85Fix::kGarWin, "CGarage: tipo en +0x4C");
static_assert(offsetof(CStoredCar, m_wModelIndex) == 0x12 && sizeof(CStoredCar) == 0x40, "CStoredCar: modelo en +0x12");
static_assert(offsetof(CEntity, m_nModelIndex) == 0x22, "CEntity: indice de modelo en +0x22");
}

namespace F85Live {
using namespace F85Fix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x609380, 0x447720, 0x447D80, 0x448550, 0x448E50};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint8& G8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = kGuard) const { return uintptr_t(m.data()) + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { const uint16 w = uint16(v); memcpy(m.data() + kGuard + off, &w, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    u32 Get16(size_t off) const { uint16 v; memcpy(&v, m.data() + kGuard + off, 2); return v; }
    u32 Get8(size_t off) const { return m[kGuard + off]; }
    json Changes() const { return ArenaChanges(m.data(), r.data(), N, kGuard); }
};
inline Arena<kGarArena> garA;
inline std::array<Arena<kVehArena>, 4> vehA;
inline std::array<Arena<kMiArena>, kModelSlots> miA;
inline Arena<kCarsArena> carsA;
inline Arena<kPedArena> pedA;

// Estado del caso en curso para los falsos
struct State { const C* c{}; unsigned fpv{}, rc{}; };
inline State g;

// Eventos: 1 FindPlayerVehicle [llamada, id, vivo & 0xFF]; 2 FindPlayerPed [id]; 3 IsEntityEntirelyOutside [this, entidad, radio]; 4 RestoreCar [this, llamada]; 5 CWorld::Add [entidad, byte +0x42D, 4 modelos]
inline void* __cdecl FakeFindVeh(u32 id, u32 alive) {
    const unsigned k = g.fpv < 2 ? g.fpv : 1;
    Ev& e = PushEv(1, 3); e.v[0] = g.fpv; e.v[1] = id; e.v[2] = alive & 0xFFu;
    ++g.fpv;
    return (k == 0 ? g.c->fv1 : g.c->fv2) ? reinterpret_cast<void*>(vehA[0].Obj()) : nullptr;
}
inline void* __cdecl FakeFindPed(u32 id) { Ev& e = PushEv(2, 1); e.v[0] = id; return g.c->fp ? reinterpret_cast<void*>(pedA.Obj()) : nullptr; }
inline bool __fastcall FakeOutside(void* self, void*, void* entity, u32 radius) {
    Ev& e = PushEv(3, 3); e.v[0] = Tag(self); e.v[1] = Tag(entity); e.v[2] = radius;
    return (g.c->outRet & 1u) != 0;
}
inline void* __fastcall FakeRestore(void* self, void*) {
    const unsigned call = g.rc++;
    Ev& e = PushEv(4, 2); e.v[0] = Tag(self); e.v[1] = call;
    const uintptr_t off = uintptr_t(self) - carsA.Obj();
    const unsigned k = off % 0x40 == 0 && off < 4 * 0x40 ? unsigned(off / 0x40) : 0;
    return g.c->rret[k] ? reinterpret_cast<void*>(vehA[k].Obj()) : nullptr;
}
inline void __cdecl FakeAdd(void* entity) {
    Ev& e = PushEv(5, 6); e.v[0] = Tag(entity);
    e.v[1] = *(static_cast<uint8*>(entity) + 0x42D);
    for (unsigned k = 0; k < 4; ++k) e.v[2 + k] = carsA.Get16(0x40 * k + 0x12);
}

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

inline void SetupArenas(const C& c) {
    garA.Fill(c.seed * 13u + 2u);
    for (unsigned k = 0; k < 4; ++k) vehA[k].Fill(c.seed * 13u + 8u + k);
    for (unsigned s = 0; s < kModelSlots; ++s) miA[s].Fill(c.seed * 13u + 20u + s);
    carsA.Fill(c.seed * 13u + 30u);
    pedA.Fill(c.seed * 13u + 31u);
}
inline void SnapArenas() {
    garA.Snap(); carsA.Snap(); pedA.Snap();
    for (auto& v : vehA) v.Snap();
    for (auto& m : miA) m.Snap();
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        SetupArenas(c);
        TagClear(); TagAdd(garA.m.data(), garA.m.size()); for (auto& v : vehA) TagAdd(v.m.data(), v.m.size()); TagAdd(pedA.m.data(), pedA.m.size()); TagAdd(carsA.m.data(), carsA.m.size());
        std::vector<u32> args; u32 seedEcx = c.ecx; uint32 popBytes = 0;
        switch (fn) {
        case 0:
            for (unsigned k = 0; k < 4; ++k) G8(kJoinAddr + k) = uint8(c.jb[k]);
            break;
        case 1: {
            garA.Put8(0x4C, c.gtype);
            for (unsigned k = 0; k < kModelSlots; ++k) {
                miA[k].Put8(0x4A, c.mhid[k]);
                G32(kModelInfoPtrs + 4 * kModelIdx[k]) = u32(miA[k].Obj());
            }
            for (unsigned h = 0; h < kHandlingSlots; ++h) G32(kHandlingTable + h * 0xE0 + 0xD0) = c.hflags[h];
            vehA[0].Put16(0x22, kModelIdx[c.modelSlot]);
            args = { c.hasVeh ? u32(vehA[0].Obj()) : 0u };
            seedEcx = u32(garA.Obj());
            break;
        }
        case 2:
            for (unsigned k = 0; k < 4; ++k) garA.Put32(0x28 + 4 * k, c.rect[k]);
            args = { c.cx, c.cy };
            seedEcx = u32(garA.Obj());
            break;
        case 3:
            for (unsigned k = 0; k < 4; ++k) { carsA.Put16(0x40 * k + 0x12, c.cmodel[k]); vehA[k].Put8(0x42D, c.vb[k]); }
            args = { u32(carsA.Obj()) };
            seedEcx = u32(garA.Obj());
            break;
        default:
            args = { c.radius };
            seedEcx = u32(garA.Obj());
            break;
        }
        SnapArenas();
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, seedEcx, c.edx, c.stack, popBytes); });
        json obs;
        const u32 al = sides[s].reg.eax & 0xFFu;
        if (fn == 2) {
            const bool dead = FpRegisterIsDead(sides[s].fp, 0);
            obs["st0"] = dead ? std::string("dead") : HexBytes(sides[s].fp.data() + 28, 10);
        } else {
            obs["ret"] = al;
        }
        if (fn == 3) {
            obs["cars_changes"] = carsA.Changes();
            json vc = json::array(); for (auto& v : vehA) vc.push_back(v.Changes());
            obs["veh_changes"] = vc;
        }
        obs["events"] = EvsJson();
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
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
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", true},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < kCases; ++c) CaseRun(env, totals, f, c);
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
// Colaborador: si la DLL tiene hook (apagado) se parchea tambien el cuerpo de la DLL; si no (dll = 0), la DLL llama a la direccion del EXE.
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

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn85a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn85a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn85a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Rutas RH de las funciones probadas: se construyen ANTES que nada; el constructor deja el hook apagado. El cuerpo de la DLL se calcula aqui, antes de crear las rutas de los colaboradores y los parches de entrada.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name;
        const bool found = FindHook(kEntries[f], cat, name);
        if (found) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    const Pair pRestore = MakeCollab(env, 0x447E40, hooksFound), pAdd = MakeCollab(env, 0x563220, hooksFound);
    const Pair pFpv = MakeCollab(env, 0x56E0D0, hooksFound), pFpp = MakeCollab(env, 0x56E210, hooksFound), pOut = MakeCollab(env, 0x448D30, hooksFound);
    env.stubs[3] = {S(pRestore, reinterpret_cast<void*>(&FakeRestore)), S(pAdd, reinterpret_cast<void*>(&FakeAdd))};
    env.stubs[4] = {S(pFpv, reinterpret_cast<void*>(&FakeFindVeh)), S(pFpp, reinterpret_cast<void*>(&FakeFindPed)), S(pOut, reinterpret_cast<void*>(&FakeOutside))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", f == 2 ? json::array({"ayudantes x87 de X87Math.cpp (Veh83DSub, Veh83DMul, Veh83DAdd)"}) : json::array()},
            {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
