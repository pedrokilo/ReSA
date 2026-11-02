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
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Car84K dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CCarCtrl::GenerateEmergencyServicesCar (0x42F9C0, hook S, cdecl sin argumentos, void). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL
// (con su hook RH apagado durante el banco): CGangWars::GangWarFightingGoingOn 0x443AC0, FindPlayerPed 0x56E210, CAccidentManager::GetInstance 0x56CF20 / GetNumberOfFreeAccidents 0x56CEE0 /
// GetNearestFreeAccident 0x56D050, FindPlayerCoors 0x56E010, CStreaming::StreamAmbulanceAndMedic 0x40A2A0 / StreamFireEngineAndFireman 0x40A400, CCarCtrl::GenerateOneEmergencyServicesCar 0x42B7D0
// y CFireManager::GetNumOfNonScriptFires 0x538F10 / FindNearestFire 0x538F40. Registran sus argumentos EN ORDEN y devuelven lo que dice el caso. Corren de verdad las lecturas de las tablas de modelos
// por nivel (0x407D30 / 0x407DC0, tablas en 0x8A5AB4 y 0x8A5AD4) y la lectura de los contadores y el tiempo del juego.

namespace C84KLive {
using namespace C84KFix;
inline constexpr uintptr_t kEntry = 0x42F9C0;
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint8& G8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = kGuard) const { return uintptr_t(m.data()) + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    json Changes() const { return ArenaChanges(m.data(), r.data(), N, kGuard); }
};
inline Arena<kMgrArena> mgrA;
inline Arena<kPedArena> pedA;
inline Arena<kDataArena> dataA;
inline Arena<kWantArena> wantA;
inline Arena<kAccArena> accA;
inline Arena<kAccPedArena> accPedA;
inline Arena<kMatArena> matA;
inline Arena<kFireArena> fireA;

inline json CloseSide(Env& env, Side& side, json obs) {
    auto fp = NormalizeFp(side.fp);
    obs["globals_hash"] = RegionsHash();
    obs["statics"] = json::array({ G32(0x9690B0), G32(0x9690B4), G32(0x9690A8), G32(0x9690AC) });
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

// ================================================================================================ GenerateEmergencyServicesCar (0x42F9C0)
struct State { const C* c{}; unsigned st{}, gn{}; };
inline State g;
// Evento 1 = CGangWars::GangWarFightingGoingOn []. cdecl; devuelve el bool en AL
inline u32 __cdecl FakeGang() { PushEv(1, 0); return g.c->gw & 1u; }
// Evento 2 = FindPlayerPed [id]. cdecl; devuelve el ped del jugador (arena)
inline void* __cdecl FakePed(i32 id) { Ev& e = PushEv(2, 1); e.v[0] = u32(id); return reinterpret_cast<void*>(pedA.Obj()); }
// Evento 3 = CAccidentManager::GetInstance []. cdecl; devuelve el gestor (arena)
inline void* __cdecl FakeInstance() { PushEv(3, 0); return reinterpret_cast<void*>(mgrA.Obj()); }
// Evento 4 = GetNumberOfFreeAccidents [this]. thiscall; devuelve el contador del caso (con signo)
inline i32 __fastcall FakeNumFree(void* self, void*) { Ev& e = PushEv(4, 1); e.v[0] = Tag(self); return i32(g.c->nfree); }
// Evento 5 = FindPlayerCoors [id]. cdecl con retorno de CVector por puntero oculto (salida, id); escribe la posicion del jugador y devuelve la salida
inline u32* __cdecl FakeCoors(u32* out, i32 id) {
    Ev& e = PushEv(5, 1); e.v[0] = u32(id);
    out[0] = g.c->player[0]; out[1] = g.c->player[1]; out[2] = g.c->player[2];
    return out;
}
// Evento 6 = GetNearestFreeAccident [this, x, y, z, bool]. thiscall (ret 8); devuelve el herido (arena) o nulo
inline void* __fastcall FakeNearest(void* self, void*, const u32* posn, u32 flag) {
    Ev& e = PushEv(6, 5); e.v[0] = Tag(self); e.v[1] = posn[0]; e.v[2] = posn[1]; e.v[3] = posn[2]; e.v[4] = flag & 0xFFu;
    return g.c->nearest & 1u ? reinterpret_cast<void*>(accA.Obj()) : nullptr;
}
// Eventos 7 y 8 = StreamAmbulanceAndMedic / StreamFireEngineAndFireman [bool]. cdecl; devuelve el bool de la llamada n-esima (compartiendo el contador)
inline u32 __cdecl FakeStreamAmb(u32 flag) { Ev& e = PushEv(7, 1); e.v[0] = flag & 0xFFu; const unsigned k = g.st < 2 ? g.st : 1; ++g.st; return g.c->sret[k] & 1u; }
inline u32 __cdecl FakeStreamFire(u32 flag) { Ev& e = PushEv(8, 1); e.v[0] = flag & 0xFFu; const unsigned k = g.st < 2 ? g.st : 1; ++g.st; return g.c->sret[k] & 1u; }
// Evento 9 = CCarCtrl::GenerateOneEmergencyServicesCar [modelo, x, y, z]. cdecl; devuelve el vehiculo (un valor no nulo cualquiera) o nulo
inline void* __cdecl FakeGenerate(u32 model, u32 x, u32 y, u32 z) {
    Ev& e = PushEv(9, 4); e.v[0] = model; e.v[1] = x; e.v[2] = y; e.v[3] = z;
    const unsigned k = g.gn < 2 ? g.gn : 1; ++g.gn;
    return g.c->gret[k] & 1u ? reinterpret_cast<void*>(uintptr_t(0x10000000u)) : nullptr;
}
// Evento 10 = CFireManager::GetNumOfNonScriptFires [this]. thiscall; el EXE devuelve AX: se devuelve la palabra completa del caso (los 16 bits altos son basura)
inline u32 __fastcall FakeNumFires(void* self, void*) { Ev& e = PushEv(10, 1); e.v[0] = Tag(self); return g.c->nfires; }
// Evento 11 = CFireManager::FindNearestFire [this, x, y, z, b1, b2]. thiscall (ret 0xC); devuelve el fuego (arena) o nulo
inline void* __fastcall FakeNearestFire(void* self, void*, const u32* pos, u32 b1, u32 b2) {
    Ev& e = PushEv(11, 6); e.v[0] = Tag(self); e.v[1] = pos[0]; e.v[2] = pos[1]; e.v[3] = pos[2]; e.v[4] = b1 & 0xFFu; e.v[5] = b2 & 0xFFu;
    return g.c->firePresent & 1u ? reinterpret_cast<void*>(fireA.Obj()) : nullptr;
}

inline void SetupWorld(const C& c) {
    dataA.Put32(0, u32(wantA.Obj()));
    pedA.Put32(0x480, u32(dataA.Obj()));
    wantA.Put32(0x2C, c.wl);
    accA.Put32(0, u32(accPedA.Obj()));
    accPedA.Put32(0x14, c.hasMat & 1u ? u32(matA.Obj()) : 0u);
    for (u32 k = 0; k < 3; ++k) { accPedA.Put32(4 + 4 * k, c.ppos[k]); matA.Put32(0x30 + 4 * k, c.mpos[k]); fireA.Put32(4 + 4 * k, c.fpos[k]); }
}

inline void CaseRun(Env& env, Totals& t, unsigned n) {
    const C c = MakeC(n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        mgrA.Fill(c.seed * 13u + 7u); pedA.Fill(c.seed * 13u + 8u); dataA.Fill(c.seed * 13u + 9u); wantA.Fill(c.seed * 13u + 10u); accA.Fill(c.seed * 13u + 11u);
        accPedA.Fill(c.seed * 13u + 12u); matA.Fill(c.seed * 13u + 13u); fireA.Fill(c.seed * 13u + 14u);
        SetupWorld(c);
        mgrA.Snap(); pedA.Snap(); dataA.Snap(); wantA.Snap(); accA.Snap(); accPedA.Snap(); matA.Snap(); fireA.Snap();
        G8(kAllowAddr) = uint8(c.allow); G32(kMaxAddr) = c.maxc;
        G32(0x969094) = c.random; G32(0x969098) = c.law; G32(0x96909C) = c.mission; G32(0x9690A0) = c.parked;
        G32(0x9690A8) = c.amb; G32(0x9690AC) = c.fire; G32(0x9690B0) = c.lastA; G32(0x9690B4) = c.lastF;
        G32(kAreaAddr) = c.area; G32(kLevelAddr) = c.level; G32(kTimeAddr) = c.now;
        TagClear(); TagAdd(mgrA.m.data(), mgrA.m.size());
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, 0, kEntry, candidate, [&](uintptr_t entry) { return Gate(entry, nullptr, 0, c.eax, c.ecx, c.edx, c.stack, 0); });
        json obs;
        obs["changes"] = json::array({ mgrA.Changes(), pedA.Changes(), dataA.Changes(), wantA.Changes(), accA.Changes(), accPedA.Changes(), matA.Changes(), fireA.Changes() });
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
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    json tables = {{"ambulance", json::array()}, {"fire", json::array()}};
    for (unsigned i = 0; i < 8; ++i) { tables["ambulance"].push_back(G32(0x8A5AB4 + 4 * i)); tables["fire"].push_back(G32(0x8A5AD4 + 4 * i)); }
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", true},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game}, {"model_tables", tables},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < kCases; ++c) CaseRun(env, totals, c);
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
    if (!std::filesystem::exists(dir / "car84k-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "car84k-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("car84k"); }
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
    const Pair pGang = MakeCollab(env, 0x443AC0, hooksFound), pPed = MakeCollab(env, 0x56E210, hooksFound), pInst = MakeCollab(env, 0x56CF20, hooksFound), pNumFree = MakeCollab(env, 0x56CEE0, hooksFound);
    const Pair pCoors = MakeCollab(env, 0x56E010, hooksFound), pNearest = MakeCollab(env, 0x56D050, hooksFound), pStreamA = MakeCollab(env, 0x40A2A0, hooksFound), pStreamF = MakeCollab(env, 0x40A400, hooksFound);
    const Pair pGen = MakeCollab(env, 0x42B7D0, hooksFound), pNumFires = MakeCollab(env, 0x538F10, hooksFound), pNearFire = MakeCollab(env, 0x538F40, hooksFound);
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    env.stubs[0] = {S(pGang, reinterpret_cast<void*>(&FakeGang)), S(pPed, reinterpret_cast<void*>(&FakePed)), S(pInst, reinterpret_cast<void*>(&FakeInstance)), S(pNumFree, reinterpret_cast<void*>(&FakeNumFree)),
        S(pCoors, reinterpret_cast<void*>(&FakeCoors)), S(pNearest, reinterpret_cast<void*>(&FakeNearest)), S(pStreamA, reinterpret_cast<void*>(&FakeStreamAmb)), S(pStreamF, reinterpret_cast<void*>(&FakeStreamFire)),
        S(pGen, reinterpret_cast<void*>(&FakeGenerate)), S(pNumFires, reinterpret_cast<void*>(&FakeNumFires)), S(pNearFire, reinterpret_cast<void*>(&FakeNearestFire))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"tablas de modelos por nivel de zona (0x8A5AB4 y 0x8A5AD4; 0x407D30 / 0x407DC0 en el EXE)", "contadores, tiempo y nivel de zona del juego (globales restauradas tras cada lado)"})},
            {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry, 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
