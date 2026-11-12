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

// Banco par a par Fn175A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 4 funciones del EXE (cuerpo del destructor de CQuadTreeNode, constructor y copia de CPatrolRoute y constructor de CTaskComplexFollowPatrolRoute; ver Fixtures.hpp), cada una con 16384 pares EXE/DLL.
// Hook S (RH_ScopedInstall / RH_ScopedNamedInstall / RH_ScopedNamedGlobalInstall). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40). Falsos de los colaboradores:
// el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): Flush de listas simples 0x552400 (con hook), liberacion en el pool de nodos 0x552BC0 (sin hook: funcion propia de la DLL),
// constructor base de CTaskComplex 0x61A3B0 (con hook), CPatrolRoute::operator new 0x41B810 (con hook) y las otras dos funciones probadas del lote (0x66D440 y 0x66D4B0, con su propia ruta). El constructor de
// CPatrolRoute (0x66D440) llama a rutinas reales del EXE (0x821D30 y 0x46AE30) que la DLL no llama: se comprueba que siguen siendo codigo del EXE (reals).
// Se observan: los bytes cambiados de las tres arenas (con las direcciones reales de las arenas y el valor de [0xB745BC], que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos),
// el valor devuelto (this como etiqueta de arena en los constructores; nada en las otras) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F175ALive {
using namespace F175AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F175AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u8 v) { m[kGuard + off] = v; }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC;

// ================================================================================================ falsos de los colaboradores
inline u32 gNew{};                   // lo que devuelve el CPatrolRoute::operator new falso (puntero a la arena B o nulo)
inline u32 gCtorMode{};              // EAX del constructor 0x66D440 falso: 0 el propio nodo, otro valor gCtorOther
inline u32 gCtorOther{};
inline u32 gCtorWrite{};             // dword que el constructor falso deja en el nodo
inline std::array<u32, 3> gBaseW{};  // dwords que el constructor base falso deja en +0, +4 y +8
inline DWORD gBenchThread{};

// CPtrListSingleLink::Flush (0x552400; thiscall, sin argumentos de pila, `ret`): evento [direccion, ECX (etiqueta)]. No toca la lista.
inline void __fastcall FkFlush(void* list, void*) { Ev& e = PushEv(0x552400, 1); e.v[0] = Tag(list); }
// Liberacion de un nodo en el pool de nodos (0x552BC0; thiscall, un puntero en la pila, `ret 4`): evento [direccion, ECX (el pool, crudo), nodo (etiqueta)]. No toca el pool.
inline void __fastcall FkPoolFree(void* pool, void*, void* node) { Ev& e = PushEv(0x552BC0, 2); e.v[0] = u32(uintptr_t(pool)); e.v[1] = Tag(node); }
// Constructor base de CTaskComplex (0x61A3B0; thiscall, sin argumentos de pila, `ret`): evento [direccion, ECX (etiqueta)]; deja tres dwords del caso en +0, +4 y +8; devuelve this.
inline void* __fastcall FkBaseCtor(void* self, void*) {
    Ev& e = PushEv(0x61A3B0, 1);
    e.v[0] = Tag(self);
    memcpy(self, &gBaseW[0], 4);
    memcpy(static_cast<u8*>(self) + 4, &gBaseW[1], 4);
    memcpy(static_cast<u8*>(self) + 8, &gBaseW[2], 4);
    return self;
}
// CPatrolRoute::operator new (0x41B810; cdecl, un tamano): evento [direccion, tamano]; devuelve gNew.
inline void* __cdecl FkNewRoute(u32 size) { Ev& e = PushEv(0x41B810, 1); e.v[0] = size; return reinterpret_cast<void*>(uintptr_t(gNew)); }
// Constructor de CPatrolRoute (0x66D440; thiscall, sin argumentos de pila, `ret`): evento [direccion, ECX (etiqueta), dword que tenia [ECX]]; deja gCtorWrite en [ECX]; devuelve this o gCtorOther.
inline void* __fastcall FkRouteCtor(void* self, void*) {
    Ev& e = PushEv(0x66D440, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gCtorWrite, 4);
    return gCtorMode == 0 ? self : reinterpret_cast<void*>(uintptr_t(gCtorOther));
}
// CPatrolRoute::Assign (0x66D4B0; thiscall, un puntero en la pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]. No copia nada.
inline void __fastcall FkRouteAssign(void* self, void*, void* src) { Ev& e = PushEv(0x66D4B0, 2); e.v[0] = Tag(self); e.v[1] = Tag(src); }

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
    std::array<u32, 8> args{};
    u32 argCount{}, popBytes{}, ecx{};
};

inline void PutRaw32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }

// Prepara arenas del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    gNew = 0; gCtorMode = 0; gCtorOther = 0; gCtorWrite = 0; gBaseW = {};
    const u32 self = u32(arenaA.Obj(a[0]));
    switch (kKind[c.fn]) {
    case kQuad: {
        const auto slotOff = [&](u32 k) { return kQuadBase + a[8] + kQuadSlot * k; };
        const auto childAddr = [&](u32 e) -> u32 {
            if (e == 0) return 0u;
            return e <= 8 ? u32(arenaB.Obj(slotOff(e - 1))) : u32(arenaC.Obj(slotOff(e - 9)));
        };
        arenaA.Put32(a[0] + 0x10, a[3]);
        for (u32 j = 0; j < 4; ++j) arenaA.Put32(a[0] + 0x14 + 4 * j, childAddr(a[4 + j]));
        for (u32 s = 0; s < 8; ++s)
            for (u32 j = 0; j < 4; ++j) arenaB.Put32(slotOff(s) + 0x14 + 4 * j, childAddr(a[16 + 4 * s + j]));
        for (u32 k = 0; k < 8; ++k)
            for (u32 j = 0; j < 4; ++j) arenaC.Put32(slotOff(k) + 0x14 + 4 * j, 0u);
        call.ecx = self;
        break;
    }
    case kPrCtor:
        arenaA.Put32(a[0], a[3]);
        call.ecx = self;
        break;
    case kPrAssign: {
        auto& src = a[17] == 0 ? arenaA : (a[17] == 1 ? arenaB : arenaC);
        u8 image[kSrcLen];
        for (u32 k = 0; k < kSrcLen; ++k) {
            const u8 b = u8(Rand(a[4], k >> 2) >> (8 * (k & 3)));
            image[k] = b ? b : u8(1);
        }
        for (u32 j = 0; j < 24; ++j) {
            const u32 start = 4 + 0x28 * (j >> 1) + ((j & 1) ? 0x18u : 0u);
            const u32 mod = (j & 1) ? 0x18u : (a[5] ? 0x30u : 0x18u);
            image[start + Rand(a[4] + 0x100u, j) % mod] = 0;
        }
        memcpy(image, &a[3], 4);
        memcpy(src.m.data() + kGuard + a[18], image, kSrcLen);
        call.ecx = self;
        call.args[0] = u32(src.Obj(a[18])); call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kFpr: {
        arenaA.Put32(a[0], a[3]);
        arenaA.Put32(a[0] + 0x20, a[4]);
        const u32 route = a[6] == 0 ? u32(arenaB.Obj(a[7])) : (a[6] == 1 ? u32(arenaC.Obj(a[8])) : (a[6] == 2 ? a[9] : 0u));
        gNew = a[17] ? u32(arenaB.Obj(a[18])) : 0u;
        gCtorMode = a[19] == 0 ? 0u : 1u;
        gCtorOther = a[19] == 1 ? u32(arenaC.Obj(a[20])) : (a[19] == 3 ? a[21] : 0u);
        gBaseW = {a[22], a[23], a[24]};
        gCtorWrite = a[25];
        call.ecx = self;
        call.args = {a[5], route, a[10], a[11], a[16], 0, 0, 0}; call.argCount = 5; call.popBytes = 0;
        break;
    }
    }
    return call;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data()))});   // direcciones reales de las arenas (para el oraculo)
    { u32 pool{}; memcpy(&pool, reinterpret_cast<const void*>(uintptr_t(kPoolGlobal)), 4); input["pool"] = pool; }                      // valor de ms_pQuadTreeNodePool (lo ve el falso del 0x552BC0)
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}};
        obs["gchanges"] = json::array();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kPrCtor: case kFpr: obs["ret"] = Tag(uintptr_t(eax)); break;                                   // this como etiqueta de arena
        default: obs["ret"] = nullptr; break;                                                              // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN175A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN175A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn175a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn175a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn175a"); }
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
    // Colaborador CON hook RH cuya llamada, en la funcion probada, va a una funcion propia de la DLL distinta de la envoltura del hook (direccion dada): el hook se apaga (ruta) y el falso va en la
    // direccion del EXE y en esa funcion.
    auto H = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked) ready = false;
        Pair p = it->second;
        p.dll = Body(dllAddress); p.hooked = true;
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH: la DLL tiene su propia funcion (direccion dada); el falso va en la direccion del EXE y en esa funcion. Se exige que de verdad no tenga hook.
    auto U = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        const uintptr_t body = Body(dllAddress);
        if (!body || ModuleOf(body) != env.asi) ready = false;
        return S(Pair{exe, body, false}, reinterpret_cast<void*>(fake));
    };
    // Rutina real del EXE que la DLL no llama (el constructor de CPatrolRoute del EXE usa el vector constructor de la CRT): solo se comprueba que siga siendo codigo del EXE sin hook.
    auto Real = [&](uintptr_t exe) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return Pair{exe, 0, false};
    };
    using QuadNode = CQuadTreeNode<void*>;
    env.stubs[0] = {F(0x552400, &FkFlush), U(0x552BC0, reinterpret_cast<uintptr_t>(&QuadNode::PoolFreeNodeExe), &FkPoolFree)};
    env.reals[1] = {Real(0x821D30), Real(0x46AE30)};
    env.stubs[3] = {F(0x61A3B0, &FkBaseCtor), F(0x41B810, &FkNewRoute), Rt(1, &FkRouteCtor), Rt(2, &FkRouteAssign)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    dllReal[1] = "toda la funcion (el EXE llama ademas al vector constructor de la CRT 0x821D30 y al constructor de entrada 0x46AE30, que la DLL sustituye por escrituras directas del mismo efecto)";
    dllReal[2] = "toda la funcion (sin colaboradores)";
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
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", nullptr}};
    }
    // Sin memoria global del juego: todo ocurre en las tres arenas propias (los colaboradores que tocarian el pool de eventos estan falseados)
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    Run(env, ready, registered, bodies, collaborators);
}
}
