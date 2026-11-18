#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <new>
#include <set>
#include <map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn238A dentro del juego (lote 238A): Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
//   0 ExeProtectionStub::MD5Transform 0x4012E9 (cdecl, sin colaboradores)
//   1 CRunningScript::ProcessCommands1700To1799 0x496E00 (thiscall, comando en la pila, resultado en AL)
// Los casos los genera fixture_cases.py (lineas JSON en fn238a-cases.jsonl, junto al EXE): este archivo los lee tal cual. El banco invoca la direccion del EXE con objetos sinteticos en cuatro arenas propias
// (A: script, cabeceras de pools y datos; B: 4 peds; C: 2 vehiculos y 3 objetos; D: memoria de las tareas, dos CVehicleModelInfo, matrices) segun el caso (escrituras + retornos programados). En el despachador
// los colaboradores (Collabs.inc, generado por collabs.py) son un falso GENERICO (una pequena rutina de maquina por colaborador, identica en la direccion del EXE y en el cuerpo de la DLL con el hook apagado) que
// anota [id, ECX (si es thiscall), argumentos de pila, lo apuntado por los punteros marcados] y devuelve el valor programado; unos pocos tienen efecto (CollectParameters escribe ScriptParams, ReadTextLabelFromScript
// el texto del caso, CreateMissionTrain y FindNodeCoorsForScript sus salidas). Corren REALES las reservas de los pools, la aritmetica de CVector y los constructores de las tareas.

namespace F238ALive {
using namespace F238AFix;
#include "Collabs.inc"
inline constexpr std::array<uintptr_t, kFunctions> kEntries{0x4012E9, 0x496E00};

template<size_t WIN> struct Arena {
    static constexpr size_t N = F238AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put(size_t off, u32 width, u32 v) { memcpy(m.data() + kGuard + off, &v, width); }
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;
inline Arena<kWin>& ArenaAt(unsigned i) { return i == 0 ? arenaA : (i == 1 ? arenaB : (i == 2 ? arenaC : arenaD)); }
inline u32 Res(u32 v) { return IsPS(v) ? u32(ArenaAt((v >> 16) & 0xFFu).Obj(v & 0xFFFFu)) : v; }

// ================================================================================================ el caso (una linea del archivo de casos)
struct Case {
    json j;
    u32 fn{}, n{}, eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 8> args{};
    u32 argCount{};
    std::vector<u32> p, r;
    std::array<u32, 40> sp{};
    std::array<u32, 4> tx{};
    u32 fill{};
    std::array<u32, 4> seed{};
};
inline std::vector<u32> U32s(const json& a) { std::vector<u32> v; for (const auto& x : a) v.push_back(u32(x.get<int64>() & 0xFFFFFFFF)); return v; }
inline Case ParseCase(const std::string& line) {
    Case c; c.j = json::parse(line);
    const auto& j = c.j;
    c.fn = j["fn"]; c.n = j["n"]; c.eax = u32(j["eax"].get<int64>()); c.ecx = u32(j["ecx"].get<int64>()); c.edx = u32(j["edx"].get<int64>()); c.stack = u32(j["stack"].get<int64>());
    const auto a = U32s(j["args"]); c.argCount = u32(a.size()); for (size_t i = 0; i < a.size() && i < 8; ++i) c.args[i] = a[i];
    c.p = U32s(j["p"]); c.r = U32s(j["r"]);
    const auto sp = U32s(j["sp"]); for (size_t i = 0; i < sp.size() && i < c.sp.size(); ++i) c.sp[i] = sp[i];
    const auto tx = U32s(j["tx"]); for (size_t i = 0; i < tx.size() && i < 4; ++i) c.tx[i] = tx[i];
    c.fill = j["fill"][0]; for (unsigned i = 0; i < 4; ++i) c.seed[i] = u32(j["fill"][1 + i].get<int64>());
    return c;
}

// ================================================================================================ falso generico de los colaboradores
inline DWORD gBenchThread{};
inline std::array<u32, 3 * 256> gRets{};
inline unsigned gNRets{};
inline std::array<u32, 40> gSp{};
inline std::array<u32, 4> gTx{};
inline constexpr unsigned kNCollabs = unsigned(sizeof(kCollabs) / sizeof(kCollabs[0]));
inline std::array<u32, kNCollabs + 1> gCnt{};
inline uintptr_t gAsi{};
inline u32 gFloatBits{};
inline const CollabSpec* FindSpec(u32 id, unsigned* idx = nullptr) {
    for (unsigned i = 0; i < kNCollabs; ++i) if (kCollabs[i].id == id) { if (idx) *idx = i; return &kCollabs[i]; }
    std::abort();
}
inline u32 RetFor(u32 id, u32 n) {
    for (unsigned i = 0; i < gNRets; ++i) if (gRets[3 * i] == id && gRets[3 * i + 1] == n) return Res(gRets[3 * i + 2]);
    return 0;
}
// Evento: [id, ecx (thiscall), argumentos, derefs, (StoreParameters: ScriptParams[0..n))]
inline u32 __cdecl GenericFake(u32 id, u32 ecx, const u32* args) {
    unsigned idx{};
    const CollabSpec& s = *FindSpec(id, &idx);
    Ev& e = PushEv(id, 0);
    const u32 n = gCnt[idx]++;
    u32 k = 0;
    if (s.thiscall) e.v[k++] = Tag(uintptr_t(ecx));
    for (unsigned a = 0; a < s.argc; ++a) { const u32 v = s.mask[a] ? (args[a] & s.mask[a]) : args[a]; e.v[k++] = s.mask[a] ? v : Tag(uintptr_t(v)); }
    for (unsigned a = 0; a < s.argc && a < 11; ++a)
        for (unsigned w = 0; w < s.deref[a]; ++w) e.v[k++] = reinterpret_cast<const u32*>(uintptr_t(args[a]))[w];
    if (s.effect == 2) for (u32 i = 0; i < args[0] && i < 16; ++i) e.v[k++] = reinterpret_cast<const u32*>(0xA43C78)[i];
    e.len = k;
    u32* const params = reinterpret_cast<u32*>(0xA43C78);
    switch (s.effect) {
    case 1: for (u32 i = 0; i < args[0] && i < 32; ++i) params[i] = Res(gSp[i]); break;     // CollectParameters
    case 3: for (unsigned i = 0; i < 4; ++i) reinterpret_cast<u32*>(uintptr_t(args[0]))[i] = gTx[i]; break;   // ReadTextLabelFromScript (16 bytes)
    case 4: *reinterpret_cast<u32*>(uintptr_t(args[5])) = RetFor(id | 0x01000000u, n); break;   // CreateMissionTrain: *outFirstCarriage
    case 5: for (unsigned c = 0; c < 3; ++c) reinterpret_cast<u32*>(uintptr_t(args[0]))[c] = RetFor(id | (0x02000000u + 0x01000000u * c), n);   // FindNodeCoorsForScript: CVector de salida
            *reinterpret_cast<u8*>(uintptr_t(args[2])) = u8(RetFor(id | 0x05000000u, n)); break;                                                        // y *bFound
    case 6: *reinterpret_cast<u32*>(uintptr_t(args[0])) = RetFor(id, n); break;                                                                  // FindNthNodeClosestToCoors: CNodeAddress de salida (por puntero oculto)
    default: break;
    }
    const u32 v = (s.effect == 5 || s.effect == 6) ? args[0] : RetFor(id, n);
    gFloatBits = v;
    return v;
}

// Rutina de maquina por colaborador (memoria ejecutable propia): guarda ECX, llama a GenericFake(id, ecx, &arg0), restaura ECX, deja el float en ST0 si procede y vuelve limpiando la pila segun la convencion
struct Thunks {
    uint8* base{}; size_t used{};
    uintptr_t at[kNCollabs]{};
    Thunks() {
        base = static_cast<uint8*>(VirtualAlloc(nullptr, 0x10000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
        for (unsigned i = 0; i < kNCollabs; ++i) {
            const auto& s = kCollabs[i];
            uint8* p = base + used;
            at[i] = uintptr_t(p);
            auto put = [&](std::initializer_list<uint8> b) { for (auto x : b) *p++ = x; };
            auto put32 = [&](u32 v) { memcpy(p, &v, 4); p += 4; };
            put({0x51});                       // push ecx
            put({0x8D, 0x44, 0x24, 0x08});     // lea eax,[esp+8]
            put({0x50});                       // push eax
            put({0x51});                       // push ecx
            put({0x68}); put32(s.id);          // push id
            put({0xE8}); put32(u32(reinterpret_cast<uintptr_t>(&GenericFake) - (uintptr_t(p) + 4)));   // call GenericFake
            put({0x83, 0xC4, 0x0C});           // add esp,12
            put({0x59});                       // pop ecx
            if (s.ret == 1) { put({0xD9, 0x05}); put32(u32(uintptr_t(&gFloatBits))); }   // fld dword ptr [gFloatBits]
            const unsigned clean = s.thiscall ? 4u * s.argc : 0u;
            if (clean) { put({0xC2, uint8(clean & 0xFF), uint8(clean >> 8)}); } else put({0xC3});
            used = size_t(p - base);
            used = (used + 15) & ~size_t(15);
        }
    }
    ~Thunks() { VirtualFree(base, 0, MEM_RELEASE); }
};

// ================================================================================================ regiones globales
inline void BuildRegions() {
    regions.clear();
    for (const auto& r : kRegionTable) regions.emplace_back(r.at, r.bytes);
}

// ================================================================================================ llamada de un caso
struct Call { std::array<u32, 8> args{}; u32 argCount{}, popBytes{}, ecx{}, eax{}; };
inline Call Setup(const Case& c) {
    Call call; call.ecx = 0; call.eax = c.eax;
    arenaA.Pattern(c.fill, c.seed[0]); arenaB.Pattern(c.fill, c.seed[1]); arenaC.Pattern(c.fill, c.seed[2]); arenaD.Pattern(c.fill, c.seed[3]);
    gCnt.fill(0);
    gNRets = unsigned(c.r.size() / 3);
    if (gNRets > 256) std::abort();
    for (size_t i = 0; i < c.r.size(); ++i) gRets[i] = c.r[i];
    gSp = c.sp; gTx = c.tx;
    for (size_t i = 0; i + 2 < c.p.size(); i += 3) {
        const u32 dest = c.p[i] >> 8, width = c.p[i] & 0xFFu, off = c.p[i + 1], v = c.p[i + 2];
        if (dest == 4) { const u32 w = Res(v); memcpy(reinterpret_cast<void*>(uintptr_t(off)), &w, width); }
        else ArenaAt(dest).Put(off, width, width == 4 ? Res(v) : v);
    }
    call.ecx = Res(c.ecx);
    call.argCount = c.argCount;
    for (u32 i = 0; i < call.argCount; ++i) call.args[i] = Res(c.args[i]);
    call.popBytes = c.fn == 0 ? 4 * call.argCount : 0;   // MD5Transform: cdecl; ProcessCommands1700To1799: thiscall (`ret 4`)
    return call;
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

inline std::vector<std::string> LoadCases(unsigned fn, unsigned nCases) {
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    std::ifstream in(std::filesystem::path(exe).parent_path() / "fn238a-cases.jsonl");
    std::vector<std::string> out; std::string line;
    while (std::getline(in, line)) {
        if (line.size() < 20) continue;
        if (line.compare(0, 8, "{\"fn\":0,") == 0 ? fn == 0 : fn == 1) out.push_back(line);
        if (out.size() >= nCases) break;
    }
    return out;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, const std::string& line) {
    const Case c = ParseCase(line);
    json input = c.j;
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data())), u32(uintptr_t(arenaD.m.data()))});
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap(); arenaD.Snap();
        SnapRegions();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        nev = 0; gThrew = false;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, call.eax, call.ecx, c.edx, c.stack, call.popBytes, false);
        });
        { u8* d = arenaD.m.data() + kGuard + 0x100; const u8* rf = arenaD.r.data() + kGuard + 0x100; for (unsigned o = 0; o < 0x200; o += 4) { u32 v; memcpy(&v, d + o, 4); if (memcmp(d + o, rf + o, 4) != 0 && ((gAsi && ModuleOf(v) == gAsi) || (v >= 0x850000u && v < 0x8F0000u))) { v = 0x56540000u; memcpy(d + o, &v, 4); } } }   // punteros a vtable de la tarea (EXE o DLL): distintos por construccion
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["gbig"] = RegionBigCrc();
        obs["events"] = EvsJson();
        obs["threw"] = gThrew;
        obs["ret"] = fn == 1 ? json(sides[s].reg.eax & 0xFFu) : json(nullptr);
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], c.n, input, rows, sides);
}

// ================================================================================================ ejecucion
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN238A_CASES", buffer, sizeof(buffer)) > 0) {
        const unsigned v = unsigned(std::atoi(buffer));
        if (v >= kFamilies && v <= kCases && v % kFamilies == 0) return v;
    }
    return kCases;
}
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
    }
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    const unsigned nCases = CasesPerFunction();
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = nCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * nCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        gBenchThread = GetCurrentThreadId();
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            const auto lines = LoadCases(f, nCases);
            if (lines.size() != nCases) { Write({{"event", "error"}, {"why", "casos de entrada insuficientes"}, {"function", kFn[f]}, {"found", lines.size()}}); totals.passed = false; break; }
            for (const auto& line : lines) CaseRun(env, totals, f, line);
            Write({{"event", "function_summary"}, {"function", kFn[f]}, {"cases", nCases}, {"failures", totals.failures - before}, {"state_restored", env.WatchesIntact() && RegionsIntact()}});
        }
    }
    RestoreAllRegions();
    const bool entriesIntact = env.WatchesIntact() && RegionsIntact();
    bool stateRestored = entriesIntact;
    for (auto& r : env.collab) if (r) stateRestored &= r->Restore();
    for (auto& r : env.route) if (r) stateRestored &= r->Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntact}});
    Write({{"event", ready && totals.passed && stateRestored ? "complete" : "error"}, {"pairs", totals.pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntact}});
    RestoreFloatingState(); savedFPActive = false;
    log.flush();
    ExitProcess(ready && totals.passed && stateRestored ? 0u : 1u);
}

inline bool FindHook(uintptr_t exe, std::string& cat, std::string& name, uintptr_t* virtualBody = nullptr) {
    bool found = false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items()) {
            if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
            } else if (virtualBody && h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Virtual
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
                *virtualBody = reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get())->GetHookOurAddress());
            }
        }
    });
    return found;
}

inline std::map<uintptr_t, uintptr_t> gRealOf;
// Con los hooks aun encendidos: si la direccion de la DLL de un hook Simple es una rutina de conservacion de ECX (memoria dinamica que empieza por `push ecx`), anota la funcion real a la que llama
inline void PreResolve(uintptr_t asi, uintptr_t game, uintptr_t exe) {
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items()) {
            if (h->Type() != ReversibleHooks::ReversibleHook::Base::HookType::Simple) continue;
            auto* sh = static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get());
            if (reinterpret_cast<uintptr_t>(sh->GetHookGTAAddress()) != exe) continue;
            const uintptr_t b = reinterpret_cast<uintptr_t>(sh->GetHookOurAddress());
            if (!b || ModuleOf(b) == asi || ModuleOf(b) == game || *reinterpret_cast<const uint8*>(b) != 0x51) continue;
            const uint8* q = reinterpret_cast<const uint8*>(b) + 1;
            while (q[0] == 0xFF) q += (q[1] == 0xB4) ? 7 : 4;
            if (q[0] == 0xE8) gRealOf[exe] = uintptr_t(q + 5) + *reinterpret_cast<const int32*>(q + 1);
        }
    });
}
inline Pair MakeCollab(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name;
    if (FindHook(exe, cat, name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}});
        if (env.collab.back()->Registered()) {
            uintptr_t b = env.collab.back()->Body();
            // RH_ScopedInstall con `stackArguments` instala una pequena rutina que conserva ECX y llama a la funcion real (la DLL llama a la real, no a la rutina): se resolvio al arrancar, con los hooks encendidos
            const auto it = gRealOf.find(exe);
            if (it != gRealOf.end()) b = it->second;
            p.dll = Body(b); p.hooked = true;
        }
    }
    return p;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn238a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn238a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn238a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    static Env env;
    static Thunks thunks;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); gAsi = env.asi; env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name; uintptr_t vbody = 0;
        const bool found = FindHook(kEntries[f], cat, name, &vbody);
        if (found) {
            if (vbody) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], vbody));
            else env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        }
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}, {"virtual", vbody != 0}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) {
        for (unsigned f = 0; f < kFunctions; ++f) collaborators[kFn[f]] = {{"own_hook", ownHooks[kFn[f]]}, {"body", env.route[f] ? env.route[f]->Body() : 0}, {"body_module", env.route[f] ? ModuleOf(env.route[f]->Body()) : 0}};
        Run(env, false, registered, bodies, collaborators);
        return;
    }
    for (unsigned i = 0; i < kNCollabs; ++i) PreResolve(env.asi, env.game, kCollabs[i].id);
    for (const auto& r : kReals) PreResolve(env.asi, env.game, r.id);
    // Falsos del despachador (fn 1): uno por colaborador, identico en el EXE y en el cuerpo de la DLL
    unsigned notHooked = 0;
    for (unsigned i = 0; i < kNCollabs; ++i) {
        Pair p = MakeCollab(env, kCollabs[i].id, hooksFound);
        if (!p.hooked || !p.dll) ++notHooked;
        env.stubs[1].push_back(StubSpec{p, reinterpret_cast<void*>(thunks.at[i])});
    }
    for (const auto& r : kReals) {
        Pair p = MakeCollab(env, r.id, hooksFound);
        env.reals[1].push_back(p);
    }
    json stubbed = json::array(), real = json::array();
    for (unsigned f = 0; f < kFunctions; ++f) {
        env.watches.emplace_back(kEntries[f], 16);
        if (f != 1) { collaborators[kFn[f]] = {{"dll_real", json::array({"toda la funcion (sin colaboradores)"})}, {"stubbed", json::array()}, {"real", json::array()}, {"hooks_found", json::array()}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}}; continue; }
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}, {"dll_bytes", HexBytes(reinterpret_cast<void*>(s.pair.dll), 12)}});
        }
        for (const auto& pr : env.reals[f]) {
            env.watches.emplace_back(pr.exe, 16);
            real.push_back({{"exe", pr.exe}, {"dll", pr.dll != 0}, {"hooked", pr.hooked}, {"dll_addr", pr.dll}, {"dll_module", ModuleOf(pr.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"toda la funcion salvo los colaboradores falseados (falso generico identico en la direccion del EXE y en el cuerpo de la DLL); reservas de pools, aritmetica de CVector y constructores de las tareas reales"})},
            {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}, {"not_hooked_or_unresolved", notHooked}};
    }
    BuildRegions();
    // CPointRoutePool (0xB744B0): CTaskComplexDrivePointRoute reserva una CPointRoute al construirse; se fija y restaura el pool entero para que ambos lados reserven la misma ranura
    if (const u32 pool = *reinterpret_cast<const u32*>(0xB744B0)) {
        const u32 stor = *reinterpret_cast<const u32*>(pool), flags = *reinterpret_cast<const u32*>(pool + 4), cap = *reinterpret_cast<const u32*>(pool + 8);
        regions.emplace_back(pool, 0x14); regions.emplace_back(flags, cap); regions.emplace_back(stor, cap * 0x64);
    }
    for (auto& v : regionsOf) v.clear();
    for (unsigned f = 0; f < kFunctions; ++f) for (size_t gi = 0; gi < regions.size(); ++gi) regionsOf[f].push_back(gi);
    Run(env, ready, registered, bodies, collaborators);
}
}
