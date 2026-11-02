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

// Banco par a par Fn85B dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Cuatro metodos de CGarage (hook S): RestoreCarsForThisImpoundingGarage 0x4485C0, CountCarsWithCenterPointWithinGarage 0x4495F0, SlideDoorOpen 0x44A660 y SlideDoorClosed 0x44A750.
// Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (con su hook RH apagado durante el banco):
//   RestoreCarsForThisImpoundingGarage -> CStoredCar::RestoreCar 0x447E40 (thiscall), CWorld::Add 0x563220 (cdecl), CAutomobile::PlaceOnRoadProperly 0x6AF420 y CBike::PlaceOnRoadProperly 0x6BEEB0 (thiscall);
//   CountCarsWithCenterPointWithinGarage -> CGarage::IsPointInsideGarage(CVector) 0x448740 (thiscall, ret 0Ch; el hook del EXE es GarageIsPointInsideRegs y la DLL de C++ llama al metodo miembro: se parchea tambien su cuerpo);
//   SlideDoorOpen / SlideDoorClosed -> CGarage::FindDoorsWithGarage 0x449FF0 (thiscall, ret 8) y CAEDoorAudioEntity::AddAudioEvent 0x4DC860 (thiscall, ret 0x10).
// Registran sus argumentos EN ORDEN y devuelven lo que dice el caso. El pool de vehiculos (ds:[0xB74494]) se sustituye por uno propio de hasta 8 ranuras de 0xA18 bytes; CTimer::ms_fTimeStep (0xB7CB5C) se fija por caso.
// SlideDoorOpen / SlideDoorClosed corren de verdad con los ayudantes x87 de X87Math.cpp (Veh83DMul, Veh83DAdd, Veh83DSub, Veh83DStore) en la DLL y los x87 del EXE en el otro lado.

namespace F85BLayout {
static_assert(offsetof(CGarage, m_fDoorPosition) == 0x38 && offsetof(CGarage, m_nType) == 0x4C && offsetof(CGarage, m_GarageAudio) == 0x50 && sizeof(CGarage) == F85BFix::kGarWin, "CGarage: puerta en +0x38, tipo en +0x4C, audio en +0x50");
static_assert(offsetof(CStoredCar, m_wModelIndex) == 0x12 && sizeof(CStoredCar) == 0x40, "CStoredCar: modelo en +0x12");
static_assert(offsetof(CEntity, m_matrix) == 0x14 && offsetof(CEntity, m_nModelIndex) == 0x22, "CEntity: matriz en +0x14");
}

namespace F85BLive {
using namespace F85BFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x4485C0, 0x4495F0, 0x44A660, 0x44A750};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint8& G8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }
template<class M> uintptr_t MA(M m) { static_assert(sizeof(M) == sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a, &m, sizeof(a)); return a; }

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
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
inline Arena<kCarsArena> carsA;
inline Arena<kPoolArena> poolA;
inline Arena<kMapArena> mapA;
inline Arena<kPoolStructArena> poolStructA;
inline Arena<kMatPoolArena> matPoolA;
inline std::array<Arena<kDoorArena>, 2> doorA;
inline std::array<Arena<kMatArena>, 2> matA;

// Estado del caso en curso para los falsos
struct State { const C* c{}; unsigned rc{}, ic{}; };
inline State g;

// Eventos: 1 RestoreCar [this, llamada]; 2 CWorld::Add [entidad, byte +0x42D, 4 modelos]; 3 CAutomobile::PlaceOnRoadProperly [this]; 4 CBike::PlaceOnRoadProperly [this];
//          5 IsPointInsideGarage [this, x, y, z]; 6 FindDoorsWithGarage [this, *primera, *segunda (valores de entrada)]; 7 AddAudioEvent [this, evento, posicion, volumen, velocidad]
inline void* __fastcall FakeRestore(void* self, void*) {
    const unsigned call = g.rc++;
    Ev& e = PushEv(1, 2); e.v[0] = Tag(self); e.v[1] = call;
    const uintptr_t off = uintptr_t(self) - carsA.Obj();
    const unsigned k = off % 0x40 == 0 && off < 4 * 0x40 ? unsigned(off / 0x40) : 0;
    return g.c->rret[k] ? reinterpret_cast<void*>(vehA[k].Obj()) : nullptr;
}
inline void __cdecl FakeAdd(void* entity) {
    Ev& e = PushEv(2, 6); e.v[0] = Tag(entity);
    e.v[1] = *(static_cast<uint8*>(entity) + 0x42D);
    for (unsigned k = 0; k < 4; ++k) e.v[2 + k] = carsA.Get16(0x40 * k + 0x12);
}
inline void __fastcall FakePlaceAuto(void* self, void*) { Ev& e = PushEv(3, 1); e.v[0] = Tag(self); }
inline void __fastcall FakePlaceBike(void* self, void*) { Ev& e = PushEv(4, 1); e.v[0] = Tag(self); }
inline bool __cdecl FakeInsideC(void* self, u32 x, u32 y, u32 z) {
    const unsigned k = g.ic++;
    Ev& e = PushEv(5, 4); e.v[0] = Tag(self); e.v[1] = x; e.v[2] = y; e.v[3] = z;
    return ((g.c->imask >> (k < 31 ? k : 31)) & 1u) != 0;
}
// El original (0x448740, ret 0Ch) deja EDX intacto y el bucle del EXE de 0x44961C lo usa de contador a traves de la llamada: el falso tambien (un __fastcall de C++ lo destruiria y cortaria el bucle del EXE).
__declspec(naked) inline void FakeInside() {
    __asm {
        push edx
        push dword ptr [esp+16]
        push dword ptr [esp+16]
        push dword ptr [esp+16]
        push ecx
        call FakeInsideC
        add esp,16
        pop edx
        ret 0Ch
    }
}
inline void __fastcall FakeFindDoors(void* self, void*, void** first, void** second) {
    Ev& e = PushEv(6, 3); e.v[0] = Tag(self); e.v[1] = Tag(*first); e.v[2] = Tag(*second);
    *first = g.c->door1 == 0 ? nullptr : reinterpret_cast<void*>(doorA[0].Obj());
    *second = g.c->door2 == 0 ? nullptr : reinterpret_cast<void*>(doorA[1].Obj());
}
inline void __fastcall FakeAudio(void* self, void*, u32 event, void* posn, u32 volume, u32 speed) {
    Ev& e = PushEv(7, 5); e.v[0] = Tag(self); e.v[1] = event; e.v[2] = Tag(posn); e.v[3] = volume; e.v[4] = speed;
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
    carsA.Fill(c.seed * 13u + 30u);
    poolA.Fill(c.seed * 13u + 40u);
    mapA.Fill(c.seed * 13u + 41u);
    poolStructA.Fill(c.seed * 13u + 42u);
    matPoolA.Fill(c.seed * 13u + 43u);
    for (unsigned k = 0; k < 2; ++k) { doorA[k].Fill(c.seed * 13u + 50u + k); matA[k].Fill(c.seed * 13u + 60u + k); }
}
inline void SnapArenas() {
    garA.Snap(); carsA.Snap(); poolA.Snap(); mapA.Snap(); poolStructA.Snap(); matPoolA.Snap();
    for (auto& v : vehA) v.Snap();
    for (auto& d : doorA) d.Snap();
    for (auto& m : matA) m.Snap();
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        SetupArenas(c);
        TagClear();
        TagAdd(garA.m.data(), garA.m.size());                       // 0
        for (auto& v : vehA) TagAdd(v.m.data(), v.m.size());        // 1..4
        TagAdd(carsA.m.data(), carsA.m.size());                     // 5
        TagAdd(poolA.m.data(), poolA.m.size());                     // 6
        for (auto& d : doorA) TagAdd(d.m.data(), d.m.size());       // 7, 8
        for (auto& m : matA) TagAdd(m.m.data(), m.m.size());        // 9, 10
        TagAdd(mapA.m.data(), mapA.m.size());                       // 11
        TagAdd(poolStructA.m.data(), poolStructA.m.size());         // 12
        TagAdd(matPoolA.m.data(), matPoolA.m.size());               // 13
        std::vector<u32> args; u32 seedEcx = c.ecx; uint32 popBytes = 0;
        switch (fn) {
        case 0:
            for (unsigned k = 0; k < 4; ++k) { carsA.Put16(0x40 * k + 0x12, c.cmodel[k]); vehA[k].Put8(0x42D, c.vb[k]); vehA[k].Put32(0x594, c.vtype[k]); }
            args = { u32(carsA.Obj()) };
            break;
        case 1: {
            for (unsigned sl = 0; sl < kPoolSlots; ++sl) {
                poolA.Put32(sl * kSlotStride + 0x14, c.pmat[sl] ? u32(matPoolA.Obj(sl * 0x48)) : 0u);
                for (unsigned k = 0; k < 3; ++k) {
                    if (c.pmat[sl]) matPoolA.Put32(sl * 0x48 + 0x30 + 4 * k, c.ppos[sl][k]);
                    else poolA.Put32(sl * kSlotStride + 4 + 4 * k, c.ppos[sl][k]);
                }
                mapA.Put8(sl, c.pmap[sl]);
            }
            poolStructA.Put32(0, u32(poolA.Obj()));
            poolStructA.Put32(4, u32(mapA.Obj()));
            poolStructA.Put32(8, c.psize);
            poolStructA.Put32(0xC, 0xFFFFFFFFu);
            G32(kPoolAddr) = u32(poolStructA.Obj());
            const u32 ignored = c.ign == 0 ? 0u : (c.ign <= kPoolSlots ? u32(poolA.Obj((c.ign - 1) * kSlotStride)) : (c.ign == 9 ? u32(vehA[0].Obj()) : 0x12345678u));
            args = { ignored };
            seedEcx = u32(garA.Obj());
            break;
        }
        default:
            garA.Put8(0x4C, c.gtype);
            garA.Put32(0x38, c.pos);
            G32(kTimestepAddr) = c.ts;
            doorA[0].Put32(0x14, c.door1 == 1 ? u32(matA[0].Obj()) : 0u);
            doorA[1].Put32(0x14, 0u);
            seedEcx = u32(garA.Obj());
            break;
        }
        SnapArenas();
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, seedEcx, c.edx, c.stack, popBytes); });
        json obs;
        obs["ret"] = fn == 1 ? sides[s].reg.eax : (sides[s].reg.eax & 0xFFu);
        if (fn == 0) {
            obs["cars_changes"] = carsA.Changes();
            json vc = json::array(); for (auto& v : vehA) vc.push_back(v.Changes());
            obs["veh_changes"] = vc;
        } else if (fn == 1) {
            obs["pool_changes"] = poolA.Changes();
            obs["matpool_changes"] = matPoolA.Changes();
            obs["gar_changes"] = garA.Changes();
        } else {
            obs["gar_changes"] = garA.Changes();
            json dc = json::array(); for (auto& d : doorA) dc.push_back(d.Changes());
            obs["door_changes"] = dc;
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
    for (const auto& s : env.dllOnly) ready &= s.pair.dll != 0 && ModuleOf(s.pair.dll) == env.asi;
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
    if (!std::filesystem::exists(dir / "fn85b-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn85b-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn85b"); }
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
    const Pair pAuto = MakeCollab(env, 0x6AF420, hooksFound), pBike = MakeCollab(env, 0x6BEEB0, hooksFound);
    const Pair pInside = MakeCollab(env, 0x448740, hooksFound);
    const Pair pFind = MakeCollab(env, 0x449FF0, hooksFound), pAudio = MakeCollab(env, 0x4DC860, hooksFound);
    env.stubs[0] = {S(pRestore, reinterpret_cast<void*>(&FakeRestore)), S(pAdd, reinterpret_cast<void*>(&FakeAdd)), S(pAuto, reinterpret_cast<void*>(&FakePlaceAuto)), S(pBike, reinterpret_cast<void*>(&FakePlaceBike))};
    env.stubs[1] = {S(pInside, reinterpret_cast<void*>(&FakeInside))};
    env.stubs[2] = {S(pFind, reinterpret_cast<void*>(&FakeFindDoors)), S(pAudio, reinterpret_cast<void*>(&FakeAudio))};
    env.stubs[3] = env.stubs[2];
    // La DLL de C++ de CountCars... llama al metodo miembro IsPointInsideGarage(CVector), no al cuerpo del hook del EXE (GarageIsPointInsideRegs): se parchea tambien.
    env.dllOnly.push_back(S(Pair{0, Body(MA(static_cast<bool (CGarage::*)(CVector)>(&CGarage::IsPointInsideGarage))), true}, reinterpret_cast<void*>(&FakeInside)));
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", f >= 2 ? json::array({"ayudantes x87 de X87Math.cpp (Veh83DMul, Veh83DAdd, Veh83DSub, Veh83DStore)"}) : json::array()},
            {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", f == 1 ? json::array({"CGarage::IsPointInsideGarage(CVector)"}) : json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
