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

// Banco par a par Fn141A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 24 funciones diminutas del EXE (accesores, cuerpos de constructor y de destructor y destructores con borrado; ver Fixtures.hpp para la lista y el esquema de familias), cada una con 16384 pares
// EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall) o V (RH_ScopedVMTInstall: ranura de la vtable del EXE; el Virtual lleva dentro un Simple sobre la entrada del EXE, que es lo que
// el banco enciende). El banco invoca la direccion del EXE con un objeto sintetico en una arena propia (A, con guardas de 0x40) y, donde hace falta, memoria global real del juego (se guarda al
// empezar y se restaura tras cada lado). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): los cuerpos con hook RH (~CEntity,
// ~CTaskComplex, ~CAutomobile, ~CTaskComplexUseEffect, ...) llevan el falso en el cuerpo del hook; los que no lo tienen (~CQuadBike 0x6CDC30 y CVehicle::operator delete 0x6E2D90) llevan el falso en
// la funcion propia de la DLL (accesor Lote141AQuadBikeDestructorBody y &CVehicle::operator delete), como en Fn125A.
// Se observan: los bytes cambiados de la arena y de la memoria global, los eventos de los falsos (con sus argumentos), el valor devuelto (EAX completo, la etiqueta de arena para `this`, o nada en
// los void y en las dos funciones que devuelven float en ST0) y el estado x87 (vacio, salvo ST0 en las dos de CWeaponInfo). EAX/ECX/EDX tras la llamada quedan en original_residue /
// candidate_residue (informativos).
uintptr_t Lote141AQuadBikeDestructorBody();   // 0x6CDC30, QuadBike.cpp

namespace F141ALive {
using namespace F141AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x5D6830, 0x5D6E90, 0x532540, 0x532560, 0x608F00, 0x608F20, 0x631910, 0x631940, 0x6CE440, 0x6CED10, 0x5F6900, 0x5F6920, 0x633290, 0x638FD0, 0x63CE70, 0x645350,
                                                   0x63D100, 0x6436E0, 0x668630, 0x66DD50, 0x636830, 0x636850, 0x636910, 0x636930};

// ================================================================================================ arena (alineada a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F141AFix::Arena(WIN);
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
inline Arena<kWin> arenaA;

// Regiones de memoria global del juego que el banco toca (indices fijos; ver regionsOf): 0xC02C14.. (indice de trabajo y dword de CCustomBuildingDNPipeline) y 0x8D12BC (desplazamiento del plugin)
enum : unsigned { kReg0, kReg1 };
inline constexpr uintptr_t kRegAt0 = 0xC02C14, kRegAt1 = 0x8D12BC;
inline constexpr size_t kRegBytes0 = 8, kRegBytes1 = 4;

// ================================================================================================ falsos de los colaboradores
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this] (la vtable de la base)
inline u32 gRet{};                   // EAX que devuelven los falsos (no es this)
inline u32 gEntByte{};               // byte que el falso del constructor de CEntity deja en +0x36
inline DWORD gBenchThread{};

// Cuerpo de un destructor (o constructor) base (thiscall, sin argumentos de pila): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en
// [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor de CEntity (0x532A90, thiscall, sin argumentos de pila): evento [0x532A90, ECX (etiqueta), dword de [ECX] al entrar, byte de +0x36 al entrar]; deja gDtorWrite en [ECX] y gEntByte en
// +0x36; devuelve gRet.
inline u32 __fastcall FkEntityCtor(void* self, void*) {
    Ev& e = PushEv(0x532A90, 3);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    e.v[2] = *(reinterpret_cast<const u8*>(self) + 0x36);
    memcpy(self, &gDtorWrite, 4);
    *(reinterpret_cast<u8*>(self) + 0x36) = u8(gEntByte);
    return gRet;
}
// operator delete de la clase (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
inline void* BodyFake(u32 addr) {
    switch (addr) {
    case 0x535E90: return reinterpret_cast<void*>(&FkBody<0x535E90>);
    case 0x61A3B0: return reinterpret_cast<void*>(&FkBody<0x61A3B0>);
    case 0x6CDC30: return reinterpret_cast<void*>(&FkBody<0x6CDC30>);
    case 0x6A61E0: return reinterpret_cast<void*>(&FkBody<0x6A61E0>);
    case 0x5F6920: return reinterpret_cast<void*>(&FkBody<0x5F6920>);
    case 0x69C780: return reinterpret_cast<void*>(&FkBody<0x69C780>);
    case 0x61A3D0: return reinterpret_cast<void*>(&FkBody<0x61A3D0>);
    case 0x633300: return reinterpret_cast<void*>(&FkBody<0x633300>);
    case 0x63CA40: return reinterpret_cast<void*>(&FkBody<0x63CA40>);
    case 0x643490: return reinterpret_cast<void*>(&FkBody<0x643490>);
    case 0x63D1A0: return reinterpret_cast<void*>(&FkBody<0x63D1A0>);
    case 0x668870: return reinterpret_cast<void*>(&FkBody<0x668870>);
    case 0x636850: return reinterpret_cast<void*>(&FkBody<0x636850>);
    case 0x632270: return reinterpret_cast<void*>(&FkBody<0x632270>);
    case 0x636930: return reinterpret_cast<void*>(&FkBody<0x636930>);
    case 0x532A90: return reinterpret_cast<void*>(&FkEntityCtor);
    default: std::abort();
    }
}
inline void* DeleteFake(u32 addr) {
    switch (addr) {
    case 0x6E2D90: return reinterpret_cast<void*>(&FkDelete<0x6E2D90>);
    case 0x69D990: return reinterpret_cast<void*>(&FkDelete<0x69D990>);
    case 0x61A5B0: return reinterpret_cast<void*>(&FkDelete<0x61A5B0>);
    default: std::abort();
    }
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

// Prepara la arena y la memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    const u32 o = a[0];
    arenaA.Pattern(a[2], a[12]);
    switch (kShape[c.fn]) {
    case kShG0:   // cdecl, un argumento (lo limpia el llamador)
        FillKind(reinterpret_cast<u8*>(kRegAt0), kRegBytes0, a[2], a[15]);
        PutGlobal32(kRegAt0, a[4]); PutGlobal32(kRegAt0 + 4, a[5]);
        call.args[0] = a[3]; call.argCount = 1; call.popBytes = 4;
        break;
    case kShG1:   // cdecl, un argumento: la geometria es (arena A + a[0]) - desplazamiento del plugin (con vuelta modulo 2^32)
        PutGlobal32(kRegAt1, a[3]);
        arenaA.Put32(o, a[4]);
        call.args[0] = u32(arenaA.Obj(o)) - a[3]; call.argCount = 1; call.popBytes = 4;
        break;
    case kShDummyCtor:
        // el contenido previo de [this] y de +0x36 es el del relleno de la arena (el falso lo registra al entrar)
        gDtorWrite = a[3]; gEntByte = a[4]; gRet = a[5];
        call.ecx = u32(arenaA.Obj(o)); call.argCount = 0; call.popBytes = 0;
        break;
    case kShBody:
        arenaA.Put32(o, a[3]);
        gDtorWrite = a[4]; gRet = a[5];
        call.ecx = u32(arenaA.Obj(o)); call.argCount = 0; call.popBytes = 0;
        break;
    case kShFloat:   // thiscall con un argumento (`ret 4`)
        arenaA.Put32(o + 0x40, a[4]); arenaA.Put32(o + 0x44, a[5]); arenaA.Put32(o + 0x4C, a[6]); arenaA.Put32(o + 0x50, a[7]);
        call.ecx = u32(arenaA.Obj(o));
        call.args[0] = a[3]; call.argCount = 1; call.popBytes = 0;
        break;
    case kShObsCtor:   // thiscall con dos argumentos (`ret 8`)
        gDtorWrite = a[5]; gRet = a[6];
        call.ecx = u32(arenaA.Obj(o));
        call.args[0] = a[3]; call.args[1] = a[4]; call.argCount = 2; call.popBytes = 0;
        break;
    case kShConst:
        arenaA.Put32(o, a[3]);
        call.ecx = u32(arenaA.Obj(o)); call.argCount = 0; call.popBytes = 0;
        break;
    case kShScalar:   // thiscall con un argumento (`ret 4`)
        arenaA.Put32(o, a[6]);
        gDtorWrite = a[8]; gRet = a[9];
        call.ecx = u32(arenaA.Obj(o));
        call.args[0] = a[7]; call.argCount = 1; call.popBytes = 0;
        break;
    }
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
        gRet = 0; gDtorWrite = 0; gEntByte = 0;
        const Call call = Setup(c);
        arenaA.Snap();
        std::vector<std::vector<u8>> before;
        for (const auto i : regionsOf[fn]) before.push_back(ReversingProbe::Bytes(regions[i].Ptr(), regions[i].bytes));
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}};
        obs["gchanges"] = GlobalChanges(before);
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kShape[fn]) {
        case kShG0: case kShBody: case kShFloat: obs["ret"] = nullptr; break;                   // void (o float en ST0, visible en el estado x87)
        case kShDummyCtor: case kShObsCtor: case kShScalar: obs["ret"] = Tag(uintptr_t(eax)); break;   // `this` como etiqueta de arena
        default: obs["ret"] = eax; break;                                                       // EAX completo (constante y lectura de la geometria)
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN141A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN141A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn141a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn141a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn141a"); }
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
    auto F = [&](uintptr_t exe, void* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(it->second, fake);
    };
    // Colaborador SIN hook RH: la DLL tiene su propia funcion (direccion dada); el falso va en la direccion del EXE y en esa funcion. Se exige que de verdad no tenga hook.
    auto U = [&](uintptr_t exe, uintptr_t dllAddress, void* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        const uintptr_t body = Body(dllAddress);
        if (!body || ModuleOf(body) != env.asi) ready = false;
        return S(Pair{exe, body, false}, fake);
    };
    using VoidDelete = void (*)(void*);
    const uintptr_t vehicleDelete = reinterpret_cast<uintptr_t>(static_cast<VoidDelete>(&CVehicle::operator delete));
    std::array<std::string, kFunctions> dllRealText;
    for (unsigned f = 0; f < kFunctions; ++f) {
        const u32 body = kCollab[f], del = kDelete[f];
        std::string text = "toda la funcion";
        if (body) {
            env.stubs[f].push_back(body == 0x6CDC30 ? U(body, Lote141AQuadBikeDestructorBody(), BodyFake(body)) : F(body, BodyFake(body)));
            text += body == 0x6CDC30 ? " salvo ~CQuadBike (0x6CDC30, funcion propia sin hook, falso identico)" : " salvo el colaborador 0x" + [&] { char b[16]; std::snprintf(b, sizeof(b), "%X", body); return std::string(b); }() + " (con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
        }
        if (del) {
            env.stubs[f].push_back(del == 0x6E2D90 ? U(del, vehicleDelete, DeleteFake(del)) : F(del, DeleteFake(del)));
            text += " y operator delete 0x" + [&] { char b[16]; std::snprintf(b, sizeof(b), "%X", del); return std::string(b); }() + (del == 0x6E2D90 ? " (CVehicle, sin hook, falso identico)" : " (con hook, falso identico)");
        }
        dllRealText[f] = text;
    }
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllRealText[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    // Memoria global del juego que toca el banco (se guarda ahora y se restaura tras cada lado)
    regions.clear();
    regions.emplace_back(kRegAt0, kRegBytes0);
    regions.emplace_back(kRegAt1, kRegBytes1);
    regionsOf[0] = {kReg0};
    regionsOf[1] = {kReg1};
    Run(env, ready, registered, bodies, collaborators);
}
}
