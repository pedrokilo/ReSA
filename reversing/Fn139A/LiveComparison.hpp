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

// Banco par a par Fn139A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 25 funciones diminutas del EXE (retornos de constante, un retorno x87, accesores, cuerpos de destructor y de constructor, un thunk a otro destructor y dos stubs; ver Fixtures.hpp para la lista y
// el esquema de familias), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall) o V (RH_ScopedVMTInstall: ranura de la vtable del EXE; el Virtual lleva dentro
// un Simple sobre la entrada del EXE, que es lo que el banco enciende). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40). Falsos de los
// colaboradores: el mismo falso en el EXE y en el cuerpo de la DLL (MakeCollab / Pair): ~CTaskComplex (0x61A3D0), ~CEventGunShot (0x4AC720) y ~CAEPoliceScannerAudioEntity (0x4E6E00), los tres con hook.
// Se observan: los bytes cambiados de las tres arenas, los eventos de los falsos (con sus argumentos, ECX y el dword de [ECX] al entrar), el valor devuelto (EAX completo, o AL en las funciones que
// devuelven bool), el estado x87 (vacio, o con ST0 = 55.0f en GetLocalSoundLevel), ECX tras la llamada en CEventSpecial (el llamador lo usa) y el contrato de ABI. EAX/ECX/EDX tras la llamada quedan
// en original_residue / candidate_residue (informativos).

namespace F139ALive {
using namespace F139AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x4AE390, 0x4AE3A0, 0x4AE3B0, 0x4AE5C0, 0x4B0D70, 0x4B0D80, 0x4B0D90, 0x4B0EE0, 0x56F200, 0x571970, 0x571980, 0x6300B0, 0x6300C0, 0x630580, 0x4B1960,
                                                  0x4B1970, 0x4B1980, 0x4B1B50, 0x4C56E0, 0x4C56F0, 0x4B2030, 0x4B2040, 0x4B2050, 0x4B2060, 0x4B2070};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F139AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
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
inline u32 gRet{};                   // EAX que devuelven los falsos
inline DWORD gBenchThread{};

// Cuerpos de destructor con hook (thiscall sin argumentos de pila, `ret`): evento [direccion, ECX (etiqueta), dword de [ECX] al entrar (la vtable que dejo el cuerpo del destructor)]; devuelven gRet.
// ~CTaskComplex (0x61A3D0), ~CEventGunShot (0x4AC720)
inline u32 __fastcall FkComplexDtor(void* self, void*) {
    Ev& e = PushEv(0x61A3D0, 2);
    e.v[0] = Tag(self); e.v[1] = BitsOf(self);
    return gRet;
}
inline u32 __fastcall FkGunShotDtor(void* self, void*) {
    Ev& e = PushEv(0x4AC720, 2);
    e.v[0] = Tag(self); e.v[1] = BitsOf(self);
    return gRet;
}
// ~CAEPoliceScannerAudioEntity (0x4E6E00): CWanted::DestructorBody pasa ECX = this + 0x21C; solo se observa la etiqueta (el dword apuntado no es una vtable de este caso)
inline u32 __fastcall FkPoliceScannerDtor(void* self, void*) {
    Ev& e = PushEv(0x4E6E00, 1);
    e.v[0] = Tag(self);
    return gRet;
}

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

// Prepara arenas del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    auto thisCall = [&](u32 argCount) {
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = argCount; call.popBytes = 0;   // `ret n` del callee: lo limpia el EXE o la DLL
    };
    switch (c.fn) {
    case kIdxPwvPriorityOver: case kIdxLeaderPriorityOver:   // thiscall con un argumento (`ret 4`)
        arenaA.Put32(a[0], a[4]);
        thisCall(1); call.args[0] = a[3];
        break;
    case kIdxWanted:       // ECX = this; el thunk suma 0x21C y salta al falso de ~CAEPoliceScannerAudioEntity
        gRet = a[4];
        thisCall(0);
        break;
    case kIdxStubA: case kIdxStubB:   // thiscall con tres argumentos de pila (`ret 0Ch`); ECX: el objeto de la arena (a[6] = 0) o el crudo del caso (1)
        arenaA.Put32(a[0], a[7]);
        thisCall(3); if (a[6]) call.ecx = c.ecx;
        call.args[0] = a[3]; call.args[1] = a[4]; call.args[2] = a[5];
        break;
    case kIdxDieDtor: case kIdxGunShotDtor:
        arenaA.Put32(a[0], a[3]);
        gRet = a[4];
        thisCall(0);
        break;
    case kIdxSpecialDtor: case kIdxLodDtor:
        arenaA.Put32(a[0], a[3]);
        thisCall(0);
        break;
    case kIdxDieControl:   // [this+8] = subtarea, [this] = vtable, un argumento (ped)
        arenaA.Put32(a[0], a[5]); arenaA.Put32(a[0] + 8, a[3]);
        thisCall(1); call.args[0] = a[4];
        break;
    case kIdxClumpCtor:    // dword previo en +0 y en +8
        arenaA.Put32(a[0], a[3]); arenaA.Put32(a[0] + 8, a[4]);
        thisCall(0);
        break;
    default:               // constantes y accesores sin pila: `this` en la arena A (el EXE no lee la memoria)
        arenaA.Put32(a[0], a[3]);
        thisCall(0);
        break;
    }
    return call;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        gRet = 0;
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
        switch (fn) {
        case kIdxPwvPriorityOver: case kIdxLeaderPriorityOver: case kIdxStubA: case kIdxStubB: case kIdxCanBeInterrupted: obs["ret"] = eax & 0xFFu; break;   // bool: AL
        case kIdxWanted: case kIdxDieDtor: case kIdxSpecialDtor: case kIdxLodDtor: case kIdxGunShotDtor: obs["ret"] = nullptr; break;   // void
        case kIdxSoundLevel: obs["ret"] = nullptr; break;   // float en ST0: se compara la imagen x87
        case kIdxClumpCtor: obs["ret"] = Tag(uintptr_t(eax)); break;   // this
        default: obs["ret"] = eax; break;   // EAX completo (constantes y ControlSubTask)
        }
        if (fn == kIdxSpecialDtor) obs["ecx_after"] = Tag(uintptr_t(sides[s].reg.ecx));   // contrato de registros: el llamador (0x4B6750) usa ECX tras la llamada
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN139A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN139A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn139a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn139a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn139a"); }
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
    env.stubs[kIdxDieDtor] = {F(0x61A3D0, &FkComplexDtor)};
    env.stubs[kIdxGunShotDtor] = {F(0x4AC720, &FkGunShotDtor)};
    env.stubs[kIdxWanted] = {F(0x4E6E00, &FkPoliceScannerDtor)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[kIdxDieDtor] = "toda la funcion salvo ~CTaskComplex (cuerpo, 0x61A3D0, con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kIdxGunShotDtor] = "toda la funcion salvo ~CEventGunShot (cuerpo, 0x4AC720, con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kIdxWanted] = "toda la funcion salvo ~CAEPoliceScannerAudioEntity (0x4E6E00, con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kIdxClumpCtor] = "toda la funcion (el constructor de CBaseModelInfo, 0x4C4A60, sin hook, va en linea en la DLL: sus escrituras se observan en los bytes del objeto)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        if (f == kIdxClumpCtor) env.watches.emplace_back(0x4C4A60, 16);   // el EXE sigue llamando a este constructor de CBaseModelInfo (sin hook): debe seguir intacto
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
    regions.clear();   // este lote no toca memoria global del juego
    Run(env, ready, registered, bodies, collaborators);
}
}
