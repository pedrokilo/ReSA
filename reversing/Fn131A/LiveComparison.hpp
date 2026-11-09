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

// Banco par a par Fn131A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Cuatro funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall: enlace de la entrada del EXE hacia la DLL). El banco invoca la
// direccion del EXE con objetos sinteticos en arenas propias (A, B y C con guardas de 0x40) y, para las listas, con los dos punteros de pool del juego (0xB74484 / 0xB74488) apuntando a pools falsos de la
// arena C (se guardan al empezar y se restauran tras cada lado). Falsos de los colaboradores con hook: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair). CSector::Destructor
// ejecuta REALES los dos Flush (el EXE los suyos, 0x552470 / 0x552400; la DLL sus funciones propias, con hook apagado).
// Se observan: los bytes cambiados de las tres arenas y de la memoria global, los eventos de los falsos (con sus argumentos y el estado del objeto en ese instante) y el estado x87 (debe quedar vacio).
// EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F131ALive {
using namespace F131AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x5DE920, 0x563130, 0x552400, 0x552470};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F131AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(int off = 0) const { return uintptr_t(m.data()) + kGuard + uintptr_t(off); }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(int off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(int off, u16 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC;

// Regiones de memoria global del juego que el banco toca (indices fijos; ver regionsOf): los dos punteros de pool (simple 0xB74484 y doble 0xB74488)
enum : unsigned { kRegPools };
inline constexpr uintptr_t kPoolPtrsAt = 0xB74484;

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos
inline uintptr_t gObj{};
inline DWORD gBenchThread{};
// direccion del objeto de CAEPedAudioEntity del caso (los falsos observan su estado en cada llamada)

// Lee un dword solo si el puntero cae dentro de una arena registrada
inline u32 SafeBits(const void* p) {
    for (const auto& t : tagTable) if (uintptr_t(p) >= t.base && uintptr_t(p) - t.base + 4 <= t.size) return BitsOf(p);
    return 0xDEAD0000u;
}
// Estado del objeto de la funcion 0 en el instante de la llamada de un falso: [direccion, ECX (etiqueta), dword de [ECX] (SafeBits), vtable de la entidad, vtable de la entidad gemela (+0xA8),
// WORD +0x88 de la gemela, sonidos +0xA0 y +0xA4 de la gemela (etiquetas)]
inline void PedEvent(u32 address, void* self) {
    Ev& e = PushEv(address, 7);
    const uint8* const tw = reinterpret_cast<const uint8*>(gObj) + 0xA8;
    uint16 word{}; memcpy(&word, tw + 0x88, 2);
    u32 p0{}, p1{}; memcpy(&p0, tw + 0xA0, 4); memcpy(&p1, tw + 0xA4, 4);
    e.v[0] = Tag(self); e.v[1] = SafeBits(self); e.v[2] = BitsOf(reinterpret_cast<const void*>(gObj)); e.v[3] = BitsOf(tw); e.v[4] = word; e.v[5] = Tag(uintptr_t(p0)); e.v[6] = Tag(uintptr_t(p1));
}
// CAESound::StopSoundAndForget (0x4EF850, thiscall sin argumentos)
inline u32 __fastcall FkStopForget(void* self, void*) { PedEvent(0x4EF850, self); return gRet; }
// CAESound::Destructor (0x4EF660, thiscall sin argumentos)
inline u32 __fastcall FkSoundDtor(void* self, void*) { PedEvent(0x4EF660, self); return gRet; }

// ================================================================================================ cierre de un lado
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

// Descripcion de la llamada de un caso
struct Call {
    std::array<u32, 4> args{};
    u32 argCount{}, popBytes{}, ecx{};
};

inline void PutGlobal32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }
inline int S16(const std::vector<u8>& s, size_t at) { return int(int16(uint16(s[at] | (s[at + 1] << 8)))); }
inline u32 U16(const std::vector<u8>& s, size_t at) { return u32(s[at] | (s[at + 1] << 8)); }

// Puntero de sonido de la funcion 0 desde su especificacion (ver Fixtures.hpp)
inline u32 PtrOf(u32 spec) {
    const u32 v = spec & 0x0FFFFFFFu;
    switch (spec >> 28) {
    case 1: return u32(arenaB.Obj(int(v)));
    case 2: return u32(arenaC.Obj(int(v)));
    case 3: return u32(arenaA.Obj(int(v)));
    case 4: return v;
    default: return 0u;
    }
}

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    if (c.fn == 0) {   // CAEPedAudioEntity::Destructor: el objeto vive en A
        const int o = int(a[0]);
        arenaA.Put16(o + 0xA8 + 0x88, u16(a[3]));
        arenaA.Put32(o + 0xA8 + 0xA0, PtrOf(a[4]));
        arenaA.Put32(o + 0xA8 + 0xA4, PtrOf(a[5]));
        gObj = arenaA.Obj(o); gRet = a[6];
        call.ecx = u32(gObj); call.argCount = 0; call.popBytes = 0;
        return call;
    }
    // Listas: cabezas en A (celda en a[0]), nodos simples en A, nodos dobles en B, pools falsos y estados en C
    const u32 ns = a[3], nd = a[4];
    std::vector<int> so(ns), dof(nd);
    for (u32 i = 0; i < ns; ++i) so[i] = S16(c.s, 2 * i);
    for (u32 i = 0; i < nd; ++i) dof[i] = S16(c.s, 2 * ns + 2 * i);
    const size_t specBase = 2 * size_t(ns) + 2 * size_t(nd);
    for (u32 i = 0; i < ns; ++i) arenaA.Put32(so[i] + 4, i + 1 < ns ? u32(arenaA.Obj(so[i + 1])) : 0u);
    for (u32 i = 0; i < nd; ++i) {
        arenaB.Put32(dof[i] + 4, i + 1 < nd ? u32(arenaB.Obj(dof[i + 1])) : 0u);
        const u32 spec = U16(c.s, specBase + 2 * i);
        arenaB.Put32(dof[i] + 8, spec == 0 ? 0u : (spec == 1 ? (i > 0 ? u32(arenaB.Obj(dof[i - 1])) : 0u) : u32(arenaC.Obj(int(spec)))));
    }
    const u32 headS = ns ? u32(arenaA.Obj(so[0])) : 0u, headD = nd ? u32(arenaB.Obj(dof[0])) : 0u;
    const int o = int(a[0]);
    if (c.fn == 1) { arenaA.Put32(o, headS); arenaA.Put32(o + 4, headD); }
    else if (c.fn == 2) arenaA.Put32(o, headS);
    else arenaA.Put32(o, headD);
    arenaC.Put32(int(kPoolS) + 0x0, u32(arenaA.Obj(0)) - a[5]); arenaC.Put32(int(kPoolS) + 0x4, u32(arenaC.Obj(int(a[7])))); arenaC.Put32(int(kPoolS) + 0xC, a[9]);
    arenaC.Put32(int(kPoolD) + 0x0, u32(arenaB.Obj(0)) - a[6]); arenaC.Put32(int(kPoolD) + 0x4, u32(arenaC.Obj(int(a[8])))); arenaC.Put32(int(kPoolD) + 0xC, a[10]);
    PutGlobal32(0xB74484, u32(arenaC.Obj(int(kPoolS))));
    PutGlobal32(0xB74488, u32(arenaC.Obj(int(kPoolD))));
    call.ecx = u32(arenaA.Obj(o)); call.argCount = 0; call.popBytes = 0;
    return call;
}

// Observa las regiones globales de la funcion: [[region, desplazamiento, byte nuevo]] respecto a la imagen tras preparar el caso
inline json GlobalChanges(const std::vector<std::vector<u8>>& before) {
    json out = json::array();
    const auto& idx = regionsOf[curFn];
    for (size_t k = 0; k < idx.size(); ++k) {
        const auto& r = regions[idx[k]];
        for (size_t i = 0; i < r.bytes; ++i)
            if (r.Ptr()[i] != before[k][i]) out.push_back(json::array({u32(idx[k]), u32(i), r.Ptr()[i]}));
    }
    return out;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        gRet = 0; gObj = 0;
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap();
        std::vector<std::vector<u8>> before;
        for (const auto i : regionsOf[fn]) before.push_back(ReversingProbe::Bytes(regions[i].Ptr(), regions[i].bytes));
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["gchanges"] = GlobalChanges(before);
        RestoreRegions();   // devuelve cuanto antes los punteros de pool reales (el resto de la observacion asigna memoria)
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}};
        obs["events"] = EvsJson();
        obs["ret"] = nullptr;   // EAX tras la llamada: ningun llamador lo lee (residuos informativos)
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN131A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN131A_CASES", buffer, sizeof(buffer)) > 0) {
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
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR},
        {"arena_bases", json::array({uintptr_t(arenaA.m.data()), uintptr_t(arenaB.m.data()), uintptr_t(arenaC.m.data())})}});
    Totals totals;
    if (ready) {
        gBenchThread = GetCurrentThreadId();
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < nCases; ++c) CaseRun(env, totals, f, c);
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
}

// Busca el hook registrado para una direccion del EXE (categoria y nombre). Hooks Simple (direccion del EXE) y Virtual (direccion de la funcion del EXE que ocupa la ranura): de un Virtual devuelve ademas
// el cuerpo de la DLL (la entrada de su vtabla), que HookRoute usa como candidato. Un Virtual lleva dentro un Simple sobre la misma direccion (Virtual::Switch).
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
    if (!std::filesystem::exists(dir / "fn131a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn131a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn131a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
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
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en el EXE y en el cuerpo de la DLL
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    env.stubs[0] = {F(0x4EF850, &FkStopForget), F(0x4EF660, &FkSoundDtor)};
    // CSector::Destructor ejecuta REALES los dos Flush: el EXE los suyos (0x552470, 0x552400) y la DLL sus funciones propias (cuerpo de los hooks de las funciones 3 y 2, apagados durante esta funcion)
    env.reals[1] = {Pair{0x552470, Body(env.route[3]->Body()), true}, Pair{0x552400, Body(env.route[2]->Body()), true}};
    const char* dllReal[kFunctions] = {
        "toda la funcion salvo CAESound::StopSoundAndForget (0x4EF850) y CAESound::Destructor (0x4EF660), ambos con hook (apagado) y falso identico; el destructor de la entidad gemela (0x4F2B80) va expandido en linea",
        "toda la funcion; los dos Flush (0x552470 y 0x552400) se ejecutan reales: el EXE los suyos y la DLL sus funciones propias (con hook, apagado en esta funcion)",
        "toda la funcion (sin colaboradores; el pool simple de la arena C sustituye al de CPools)",
        "toda la funcion (sin colaboradores; el pool doble de la arena C sustituye al de CPools)"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        for (const auto& r : env.reals[f]) {
            env.watches.emplace_back(r.exe, 16);
            real.push_back({{"exe", r.exe}, {"dll", r.dll != 0}, {"hooked", r.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    // Memoria global del juego que toca el banco (se guarda ahora y se restaura tras cada lado)
    regions.clear();
    regions.emplace_back(kPoolPtrsAt, 8);
    regionsOf[1] = {kRegPools};
    regionsOf[2] = {kRegPools};
    regionsOf[3] = {kRegPools};
    Run(env, ready, registered, bodies, collaborators);
}
}
