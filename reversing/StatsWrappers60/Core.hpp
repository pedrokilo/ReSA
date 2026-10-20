#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <initializer_list>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "Gate.hpp"

// Nucleo del banco StatsWrappers60. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}
// y OriginalDependencies499::HookRoute. Plantillas: reversing/FileLoaderLoad2dEffect27 y reversing/DecisionMakeDecision27 (misma estructura de compuertas).
//
// Banco par a par de CUATRO funciones de CStats (estaticas, cdecl, `ret` normal): FindLeastFavoriteRadioStation (0x559010), BuildStatLine (0x559230), UpdateRespectStat (0x55BC50)
// y ConstructStatLine (0x55A780). Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO el hook de esa funcion encendido. En ambos lados se sustituyen, SOLO durante la
// llamada y con el MISMO falso (Fakes.hpp), los colaboradores (parches de entrada en la direccion del EXE Y en el cuerpo de la DLL, calculado ANTES de parchear):
//   BuildStatLine:     CLocalisation::Metric, CText::Get, GxtCharToAscii (0x69F7E0, solo EXE), GxtCharStrcpy, 0x719240 (quita marcas, solo EXE), AsciiToGxtChar, sprintf del EXE (0x821BB5, solo EXE)
//   ConstructStatLine: lo anterior mas CLocalisation::GermanGame, CStats::GetStatValue (falso con fild/fld como el original), BuildStatLine (la funcion hermana), ConvertToMins / ConvertToSecs,
//                      GetPercentageProgress, PopulateFavoriteRadioStationList, FindMost/FindLeastFavoriteRadioStation, CAudioEngine::GetRadioStationNameKey, FindPlayerPed, CPed::GetWeaponSkill
//   UpdateRespectStat: CTagManager::GetPercentageTagged, CStats::CheckForThreshold, CStats::DisplayScriptStatUpdateMessage, CStats::CheckForStatsMessage
//   FindLeastFavoriteRadioStation: ninguno.
// NO se valida aqui el cuerpo real de ningun colaborador (solo que la funcion los llama igual); ni el sprintf real del EXE (su formato; el falso escribe una representacion fija).
// La palabra de control x87 se fija a 24 bits, al mas cercano y con TODAS las excepciones enmascaradas (estado del juego). EAX/ECX/EDX de salida: solo se registran
// (reversing/Dependencies25A/residue_scan.py: los llamadores del EXE de FindLeast y ConstructStatLine leen solo EAX; BuildStatLine y UpdateRespectStat no se leen; ver README.md).
namespace StatsWrappers60Live {
using namespace Stw60;
using HookRoute = OriginalDependencies499::HookRoute;
inline bool done{};
inline std::ofstream log;
inline void ModuleMarker() {}
inline std::uintptr_t ModuleOf(std::uintptr_t address) { MEMORY_BASIC_INFORMATION i{}; return VirtualQuery(reinterpret_cast<void*>(address), &i, sizeof(i)) == sizeof(i) ? std::uintptr_t(i.AllocationBase) : 0; }
inline std::uintptr_t Resolve(std::uintptr_t address) { for (unsigned i = 0; i < 16 && *reinterpret_cast<uint8*>(address) == 0xE9; ++i) address += 5 + *reinterpret_cast<int32*>(address + 1); return address; }
inline void Write(const json& row) { RestoreFloatingState(); ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); RestoreFloatingState(); }
// true: EAX/ECX/EDX de salida forman parte de la igualdad (ningun llamador del EXE los lee; ver README.md): por defecto solo se registran en original_residue/candidate_residue.
inline constexpr bool kCompareVolatileRegisters = false;
inline constexpr bool kCompareDeadX87Registers = false;

struct Watch { std::uintptr_t address; std::vector<uint8> saved; Watch(std::uintptr_t a, std::size_t n) : address(a), saved(ReversingProbe::Bytes(reinterpret_cast<void*>(a), n)) {} bool Intact() const { return saved == ReversingProbe::Bytes(reinterpret_cast<void*>(address), saved.size()); } };

struct Pair { std::uintptr_t exe, dll; };
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline std::uintptr_t Body(std::uintptr_t p) { for (int k = 0; k < 4 && *reinterpret_cast<const uint8*>(p) == 0xE9; ++k) { const std::uintptr_t t = p + 5 + *reinterpret_cast<const int32*>(p + 1); if (t >= 0x400000 && t < 0x1000000) break; p = t; } return p; }

struct Collabs {
    Pair getStat{}, metric{}, german{}, populate{}, findMost{}, findLeast{}, progress{}, mins{}, secs{}, build{}, get{}, g2a{}, strcpy_{}, strip{}, a2g{}, sprintf_{};
    Pair stationKey{}, findPed{}, weapon{}, tagged{}, threshold{}, display{}, checkMsg{};
};
struct Env {
    std::uintptr_t asi{}, game{};
    std::unique_ptr<HookRoute> route[4];     // 0 Find, 1 Build, 2 Resp, 3 Cons
    std::vector<Watch> watches;
    Collabs c{};
    bool WatchesIntact() const { return std::ranges::all_of(watches, [](const auto& w) { return w.Intact(); }); }
};
// Parches de entrada activos SOLO durante la llamada (RAII, orden inverso).
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>, 64> p; int n{};
    template<class F> void Add(const Pair& pr, F stub) { if (pr.exe) p[n++].emplace(pr.exe, stub); if (pr.dll && pr.dll != pr.exe) p[n++].emplace(pr.dll, stub); }
    ~Patches() { for (int i = n - 1; i >= 0; --i) p[i].reset(); }
};
inline void InstallText(Env& e, Patches& p) {
    p.Add(e.c.get, &FakeGet); p.Add(e.c.g2a, &FakeG2A); p.Add(e.c.strcpy_, &FakeStrcpy); p.Add(e.c.strip, &FakeStrip); p.Add(e.c.a2g, &FakeA2G); p.Add(e.c.sprintf_, &FakeSprintf);
    p.Add(e.c.metric, &FakeMetric);
}
inline void InstallNone(Env&, Patches&) {}
inline void InstallBuild(Env& e, Patches& p) { InstallText(e, p); }
inline void InstallCons(Env& e, Patches& p) {
    InstallText(e, p);
    p.Add(e.c.german, &FakeGerman); p.Add(e.c.getStat, &FakeGetStat); p.Add(e.c.build, &FakeBuild); p.Add(e.c.mins, &FakeMins); p.Add(e.c.secs, &FakeSecs);
    p.Add(e.c.progress, &FakeProgress); p.Add(e.c.populate, &FakePopulate); p.Add(e.c.findMost, &FakeFindMost); p.Add(e.c.findLeast, &FakeFindLeast);
    p.Add(e.c.stationKey, &FakeStationKey); p.Add(e.c.findPed, &FakeFindPed); p.Add(e.c.weapon, &FakeWeaponSkill);
}
inline void InstallResp(Env& e, Patches& p) {
    p.Add(e.c.tagged, &FakeTagged); p.Add(e.c.threshold, &FakeThreshold); p.Add(e.c.display, &FakeDisplay); p.Add(e.c.checkMsg, &FakeCheckMsg);
}

struct Side { Reg reg{}; std::array<uint8, 108> fp{}; uint32 mxcsr{}; bool routeOK{}; };
// Ruta: original = hook de la ruta apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). `exeOnly`: direcciones del EXE que no pueden saltar a la DLL
// (0x69F7E0, 0x719240 y 0x821BB5 no tienen version en la DLL). Los parches de los colaboradores se instalan despues de comprobar la ruta y se quitan antes de volver.
template<class Install, class Gatecall>
Side Execute(Env& env, HookRoute& route, std::uintptr_t entry, std::initializer_list<std::uintptr_t> exeOnly, bool candidate, Install install, Gatecall gate) {
    Side side{};
    auto call = [&] {
        bool ok = candidate ? (ReversingProbe::JumpsToDll(entry) && ModuleOf(Resolve(entry)) == env.asi) : !ReversingProbe::JumpsToDll(entry);
        for (const std::uintptr_t a : exeOnly) ok &= !ReversingProbe::JumpsToDll(a);
        side.routeOK = ok;
        {
            Patches patches;
            install(env, patches);
            side.reg = gate(entry);
        }
        SaveFP(side.fp.data()); side.mxcsr = ReadMXCSR();
        return 0;
    };
    route.Invoke(candidate, call);
    return side;
}
inline json CommonRow(const Side& side, const std::array<uint8, 108>& fp) {
    return {{"post_floating_state", HexBytes(fp.data(), 108)}, {"post_mxcsr_control", side.mxcsr & 0xFFC0u}, {"stack_restored", side.reg.stack}, {"nonvolatile_preserved", side.reg.nonvolatile},
            {"direction_flag_clear", (side.reg.eflags & 0x400u) == 0}, {"route_ok", side.routeOK}};
}
struct Totals { unsigned pairs{}; unsigned failures{}; bool passed{true}; };
inline void Finish(Totals& t, const char* function, unsigned n, const json& input, json rows[2], Side sides[2]) {
    bool ok = rows[0] == rows[1];
    for (const char* k : {"stack_restored", "nonvolatile_preserved", "direction_flag_clear", "route_ok", "globals_restored", "floating_state_restored"}) ok &= rows[0][k] == true && rows[1][k] == true;
    const auto fa = NormalizeFp(sides[0].fp), fb = NormalizeFp(sides[1].fp);
    t.passed &= ok; t.failures += !ok;
    json row = {{"event", "comparison"}, {"function", function}, {"case", n}, {"pair", t.pairs++}, {"input", input}, {"original", rows[0]}, {"candidate", rows[1]}, {"fp_difference", FpDifference(fa, fb)},
                {"original_residue", ResidueJson(sides[0].reg)}, {"candidate_residue", ResidueJson(sides[1].reg)}, {"equal", ok}, {"accepted", ok}};
    Write(row);
}

inline Snapshot base;
// Ejecuta los casos de una funcion. `callSide(fixture, candidate)` hace la llamada con compuertas y devuelve el Side.
template<class Fx, class Make, class InputOf, class SetupFn, class ObserveFn, class CallFn>
void RunFunction(Env& env, Totals& totals, const char* name, unsigned cases, const std::vector<Region>& allowed, Make make, InputOf inputOf, SetupFn setup, ObserveFn observe, CallFn callSide) {
    for (unsigned n = 0; n < cases; ++n) {
        const Fx f = make(n);
        json rows[2];
        Side sides[2];
        base.Take();
        for (int s = 0; s < 2; ++s) {
            Reg reg{};
            PrepareFloatingState();
            const bool before = env.WatchesIntact();
            rows[s] = RunCase(base, allowed, [&] { setup(f); }, [&] { sides[s] = callSide(f, s == 1); reg = sides[s].reg; return reg; }, [&] { return observe(f, reg); }, reg);
            const bool watchesIntact = before && env.WatchesIntact();
            Snapshot after; after.Take();
            bool memoryRestored = true;
            for (unsigned i = 0; i < kNumRegions; ++i) memoryRestored &= after.data[i] == base.data[i];
            auto fp = NormalizeFp(sides[s].fp);
            json out = CommonRow(sides[s], fp);
            out["observed"] = rows[s];
            if (kCompareVolatileRegisters) { out["observed"]["eax"] = sides[s].reg.eax; out["observed"]["ecx"] = sides[s].reg.ecx; out["observed"]["edx"] = sides[s].reg.edx; }
            RestoreFloatingState();
            out["globals_restored"] = watchesIntact && env.WatchesIntact() && memoryRestored;
            out["floating_state_restored"] = FloatingStateRestored();
            rows[s] = out;
        }
        Finish(totals, name, n, inputOf(f), rows, sides);
    }
    Write({{"event", "function_summary"}, {"function", name}, {"cases", cases}, {"failures", totals.failures}, {"state_restored", env.WatchesIntact()}});
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env, bool ready, const json& registered, const json& bodies) {
    static constexpr const char* kNames[4] = { "CStats::FindLeastFavoriteRadioStation", "CStats::BuildStatLine", "CStats::UpdateRespectStat", "CStats::ConstructStatLine" };
    static constexpr std::uintptr_t kEntries[4] = { 0x559010, 0x559230, 0x55BC50, 0x55A780 };
    for (int i = 0; i < 4; ++i) {
        const std::uintptr_t body = env.route[i] ? env.route[i]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
    }
    // Entradas vigiladas: las cuatro funciones, los colaboradores solo del EXE y las entradas de los colaboradores parcheados (deben seguir siendo el codigo original al volver).
    std::vector<std::uintptr_t> watched{ 0x559010, 0x559230, 0x55BC50, 0x55A780, 0x821BB5, 0x69F7E0, 0x719240 };
    for (const Pair* p : { &env.c.getStat, &env.c.metric, &env.c.german, &env.c.populate, &env.c.findMost, &env.c.progress, &env.c.mins, &env.c.secs, &env.c.get, &env.c.strcpy_, &env.c.a2g,
                           &env.c.stationKey, &env.c.findPed, &env.c.weapon, &env.c.tagged, &env.c.threshold, &env.c.display, &env.c.checkMsg })
        watched.push_back(p->exe);
    for (const std::uintptr_t at : watched) env.watches.emplace_back(at, 16);
    json watchedJson = json::array();
    for (const auto& w : env.watches) watchedJson.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), 16)}});
    json stubbed = json::array();
    for (const Pair* p : { &env.c.getStat, &env.c.metric, &env.c.german, &env.c.populate, &env.c.findMost, &env.c.findLeast, &env.c.progress, &env.c.mins, &env.c.secs, &env.c.build, &env.c.get, &env.c.g2a,
                           &env.c.strcpy_, &env.c.strip, &env.c.a2g, &env.c.sprintf_, &env.c.stationKey, &env.c.findPed, &env.c.weapon, &env.c.tagged, &env.c.threshold, &env.c.display, &env.c.checkMsg })
        stubbed.push_back({{"exe", p->exe}, {"dll", p->dll}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    PrepareFloatingState();
    RestoreFloatingState();
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", 4},
           {"pairs", kCasesFind + kCasesBuild + kCasesRespect + kCasesConstruct},
           {"cases", {{kNames[0], kCasesFind}, {kNames[1], kCasesBuild}, {kNames[2], kCasesRespect}, {kNames[3], kCasesConstruct}}},
           {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
           {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true},
           {"collaborators", {{"exe_only", std::vector<std::uintptr_t>{ 0x69F7E0, 0x719240, 0x821BB5 }}, {"stubbed", stubbed}}},
           {"watched_entries", watchedJson}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game}, {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        const std::initializer_list<std::uintptr_t> exeOnly{ 0x69F7E0, 0x719240, 0x821BB5 };
        RunFunction<FxFind>(env, totals, kNames[0], kCasesFind, kAllowedFind, MakeFind, InputFind, SetupFind,
            [](const FxFind&, const Reg& reg) { return ObserveFind(reg); },
            [&](const FxFind& f, bool candidate) {
                return Execute(env, *env.route[0], kEntries[0], {}, candidate, InstallNone, [&](std::uintptr_t entry) { return Gate(entry, nullptr, 0, f.abi.eax, f.abi.ecx, f.abi.edx, f.abi.stack, 0); });
            });
        BuildArgs args{};
        RunFunction<FxBuild>(env, totals, kNames[1], kCasesBuild, kAllowedBuild, MakeBuild, InputBuild, [&](const FxBuild& f) { args = SetupBuild(f); },
            [](const FxBuild&, const Reg&) { return ObserveBuild(); },
            [&](const FxBuild& f, bool candidate) {
                const uint32 a[5] = { uint32(reinterpret_cast<std::uintptr_t>(args.line)), uint32(reinterpret_cast<std::uintptr_t>(args.p1)), uint32(args.metrics), uint32(reinterpret_cast<std::uintptr_t>(args.p2)), uint32(args.type) };
                return Execute(env, *env.route[1], kEntries[1], exeOnly, candidate, InstallBuild, [&](std::uintptr_t entry) { return Gate(entry, a, 5, f.abi.eax, f.abi.ecx, f.abi.edx, f.abi.stack, 20); });
            });
        RunFunction<FxResp>(env, totals, kNames[2], kCasesRespect, kAllowedResp, MakeResp, InputResp, SetupResp,
            [](const FxResp&, const Reg&) { return ObserveResp(); },
            [&](const FxResp& f, bool candidate) {
                const uint32 a[1] = { f.arg0 };
                return Execute(env, *env.route[2], kEntries[2], {}, candidate, InstallResp, [&](std::uintptr_t entry) { return Gate(entry, a, 1, f.abi.eax, f.abi.ecx, f.abi.edx, f.abi.stack, 4); });
            });
        RunFunction<FxCons>(env, totals, kNames[3], kCasesConstruct, kAllowedCons, MakeCons, InputCons, SetupCons,
            [](const FxCons&, const Reg& reg) { return ObserveCons(reg); },
            [&](const FxCons& f, bool candidate) {
                const uint32 a[2] = { uint32(f.arg0), f.arg1 };
                return Execute(env, *env.route[3], kEntries[3], exeOnly, candidate, InstallCons, [&](std::uintptr_t entry) { return Gate(entry, a, 2, f.abi.eax, f.abi.ecx, f.abi.edx, f.abi.stack, 8); });
            });
    }
    const bool entriesIntact = env.WatchesIntact();
    bool stateRestored = entriesIntact;
    for (auto& r : env.route) if (r) stateRestored &= r->Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntact}});
    Write({{"event", ready && totals.passed && stateRestored ? "complete" : "error"}, {"pairs", totals.pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntact}});
    RestoreFloatingState(); savedFPActive = false;
}
}
