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

// Banco par a par Fn218A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 32 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall con direccion). El banco invoca la direccion del EXE con objetos sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40).
// Colaboradores: falso identico en la direccion del EXE y en el cuerpo de la DLL para los cuerpos de destructor (con hook, funciones del propio lote: los de los FxInfo en 1..9 y los de los interp en 23..26) que llaman los destructores con borrado;
// falso solo en la direccion del EXE para operator delete (0x8214BD: la DLL lo llama por ella). Los cuerpos de destructor de los FxInfo llaman al destructor del interp (0x4A8460, 0x4A87F0, 0x4A89B0, 0x4A8B70; con hook del propio lote), que aqui es REAL:
// el EXE corre sus bytes (hook apagado) y la DLL lo hace en linea. Las vtables que deja cada lado (la del EXE o una del ASI) se normalizan a la esperada del EXE (`vtable_ok`).
// Se observan: los bytes cambiados de las cuatro arenas, los eventos de los falsos (con sus argumentos y un volcado del objeto), el valor devuelto (punteros como etiqueta de arena) y el estado x87 (siempre vacio).
// EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F218ALive {
using namespace F218AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F218AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = u8(v); }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;

// ================================================================================================ falsos de los colaboradores
inline DWORD gBenchThread{};
inline const u8* gObj{};                        // objeto bajo prueba (arena A): los falsos vuelcan sus seis primeros dwords al entrar
inline u32 gBodyRet{};                          // EAX que devuelve el falso del cuerpo de destructor
inline u32 gExpVt0{}, gExpVt8{};                // vtables esperadas del EXE en +0 y +8 tras la funcion
inline uintptr_t gAsi{};
inline u32 NormVt(u32 v, u32 expected) { return (v == expected || ModuleOf(v) == gAsi) ? kVtMark : v; }
inline void Put32At(u8* p, u32 v) { memcpy(p, &v, 4); }

// operator delete (0x8214BD) del CRT del EXE (cdecl, un puntero CRUDO): evento [direccion, argumento (etiqueta o crudo)]
inline void __cdecl FkDelete(void* p) { Ev& e = PushEv(0x8214BD, 1); e.v[0] = Tag(p); }
// cuerpos de destructor (thiscall, sin argumentos): evento [direccion, this como etiqueta, seis dwords del objeto]; no escriben nada; devuelven un EAX que ningun llamador lee
template<u32 Addr> inline u32 __fastcall FkBody(void* self, void* /*edx*/) {
    Ev& e = PushEv(Addr, 7);
    e.v[0] = Tag(self);
    for (u32 i = 0; i < 6; ++i) e.v[1 + i] = BitsOf(gObj + 4 * i);
    return gBodyRet;
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
    std::array<u32, 8> args{};
    u32 argCount{}, popBytes{}, ecx{};
    bool floatRet{};
};

// Prepara arenas y objeto del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    const Kind kind = kKind[c.fn];
    arenaA.Pattern(a[2], a[92]); arenaB.Pattern(a[2], a[93]); arenaC.Pattern(a[2], a[94]); arenaD.Pattern(a[2], a[95]);
    const u32 o = a[0];
    for (u32 i = 0; i < 6; ++i) if (a[40 + i]) arenaA.Put32(o + 4 * i, a[4 + i]);
    gObj = reinterpret_cast<const u8*>(arenaA.Obj(o));
    gBodyRet = 0; gExpVt0 = ExpVt0(c.fn); gExpVt8 = ExpVt8(c.fn);
    call.ecx = u32(arenaA.Obj(o));
    if (kind == kSdd) {
        gBodyRet = a[14];
        call.args[0] = a[13]; call.argCount = 1; call.popBytes = 0;   // `ret 4`
    }
    return call;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data())), u32(uintptr_t(arenaD.m.data()))});   // direcciones reales de las arenas (para el oraculo)
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap(); arenaD.Snap();
        SnapRegions();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        json obs;
        if (gExpVt0) {   // la vtable de la DLL (en el ASI) o la del EXE se normaliza a la esperada del EXE (en +0, y en +8 si la funcion toca un interp embebido)
            const u32 o = c.a[0];
            const u32 v0 = arenaA.Get32(o), v8 = arenaA.Get32(o + 8);
            const auto okv = [&](u32 v, u32 expected) { return candidate ? (v == expected || ModuleOf(v) == env.asi) : v == expected; };
            const bool ok = okv(v0, gExpVt0) && (!gExpVt8 || okv(v8, gExpVt8));
            obs["vtable_ok"] = ok;
            if (ok) { arenaA.Put32(o, gExpVt0); if (gExpVt8) arenaA.Put32(o + 8, gExpVt8); }
        }
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kSdd: case kICtor: case kLi: obs["ret"] = Tag(uintptr_t(eax)); break;   // this como etiqueta de arena
        default: obs["ret"] = nullptr; break;                              // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}



// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN218A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN218A_CASES", buffer, sizeof(buffer)) > 0) {
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
// Colaborador con hook V (RH_ScopedVMTInstall): la ruta usa el cuerpo de la DLL de la ranura (la funcion que la DLL llama de forma no virtual) y el hook se apaga.
inline Pair MakeCollabV(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name; uintptr_t vbody = 0;
    if (FindHook(exe, cat, name, &vbody)) {
        if (vbody) env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, vbody));
        else env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}, {"virtual", vbody != 0}});
        if (env.collab.back()->Registered()) { p.dll = Body(env.collab.back()->Body()); p.hooked = true; }
    }
    return p;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn218a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn218a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn218a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); gAsi = env.asi; env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name; uintptr_t vbody = 0;
        const bool found = FindHook(kEntries[f], cat, name, &vbody);
        if (found) {
            if (vbody) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], vbody));
            else env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        }
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0 && ModuleOf(env.route[f]->Body()) == env.asi;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}, {"virtual", vbody != 0}}) : json(nullptr);
        collaborators[kFn[f]] = {{"found", found}, {"body", found ? env.route[f]->Body() : 0}, {"body_module", found ? ModuleOf(env.route[f]->Body()) : 0}, {"asi", env.asi}, {"cat", cat}, {"name", name}};
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
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (operator delete 0x8214BD, delete[] 0x8213AE, operator new[] 0x821195): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador REAL (el EXE corre sus bytes con el hook apagado; la DLL lo hace en linea): solo se apaga su hook y se comprueba la ruta
    auto Real = [&](uintptr_t exe) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        return it->second;
    };
    using BodyFake = u32 (__fastcall*)(void*, void*);
    // los diez cuerpos de FxInfo (indice 0..9; el 0 es de SpriteRect, cuyo destructor con borrado es del lote 217A: no es colaborador de este banco) y los cuatro de interp
    const std::array<BodyFake, kNFxBody> fxFakes{&FkBody<0x4A76C0>, &FkBody<0x4A7730>, &FkBody<0x4A77A0>, &FkBody<0x4A7810>, &FkBody<0x4A7880>, &FkBody<0x4A78F0>, &FkBody<0x4A7960>, &FkBody<0x4A79D0>, &FkBody<0x4A7A40>, &FkBody<0x4A7AB0>};
    const std::array<BodyFake, kNIBody> iFakes{&FkBody<0x4A8460>, &FkBody<0x4A87F0>, &FkBody<0x4A89B0>, &FkBody<0x4A8B70>};
    for (unsigned j = 0; j < kNFxBody; ++j) env.reals[kFxBody0 + j] = {Real(kDtorInterp[j])};
    for (unsigned j = 0; j < kNFxSdd; ++j) env.stubs[kFxSdd0 + j] = {F(kFxBodyAddr[j + 1], fxFakes[j + 1]), ByExeAddress(0x8214BD, &FkDelete)};
    for (unsigned j = 0; j < kNISdd; ++j) env.stubs[kISdd0 + j] = {F(kIBodyAddr[j], iFakes[j]), ByExeAddress(0x8214BD, &FkDelete)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) {
        switch (kKind[f]) {
        case kBody: dllReal[f] = "toda la funcion; el destructor del interp se hace en linea (el EXE corre el real)"; break;
        case kSdd: dllReal[f] = "toda la funcion salvo el cuerpo del destructor (con hook, falso identico) y operator delete (0x8214BD, falso solo en la direccion del EXE: la DLL lo llama por ella)"; break;
        default: dllReal[f] = "toda la funcion (sin colaboradores)"; break;
        }
    }
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        for (const auto& pr : env.reals[f]) {
            env.watches.emplace_back(pr.exe, 16);
            real.push_back({{"exe", pr.exe}, {"dll", pr.dll != 0}, {"hooked", pr.hooked}, {"dll_addr", pr.dll}, {"dll_module", ModuleOf(pr.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Sin memoria global tocada por estas funciones (los falsos no escriben memoria del juego)
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    Run(env, ready, registered, bodies, collaborators);
}
}
