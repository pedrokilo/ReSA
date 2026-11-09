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

// Banco par a par Fn140A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 21 funciones diminutas del EXE (seis destructores con borrado de 27 y 30 bytes, dos cuerpos de destructor y un trampolin de destructor, dos constructores, copias fuera de linea de accesores de
// CMatrix, CPlaceable y CTimer, la funcion booleana de CBridge, sin / cos con fsin / fcos y tres GetTaskType; ver Fixtures.hpp para la lista y el esquema de familias), cada una con 16384 pares EXE/DLL.
// Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall) o V (RH_ScopedVMTInstall: ranura de la vtable del EXE; el Virtual lleva dentro un Simple sobre la entrada del EXE, que es lo que el banco
// enciende). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40) y, donde hace falta, memoria global real del juego (se guarda al empezar y se
// restaura tras cada lado). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): cuerpos de destructor (con hook o funciones propias de
// la DLL sin hook, ver Update) y operator delete (CTask 0x61A5B0 con hook; CVehicle 0x6E2D90 y CBuilding 0x4040A0 sin hook, funciones de la DLL; CRT 0x8214BD sin hook, la DLL lo llama por su direccion).
// Se observan: los bytes cambiados de las tres arenas y de la memoria global, los eventos de los falsos (con sus argumentos), el valor devuelto (EAX completo, o AL en la funcion booleana) y el estado x87
// (vacio salvo ST0 de sin, cos y GetTimeStep, que se compara registro a registro entre EXE y DLL y contra el oraculo). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Funciones propias de la DLL sin hook (cuerpos de destructor) o con hook cuya llamada interna no pasa por la envoltura: las exportan los .cpp de cada clase.
uintptr_t Lote140APhysicalDestructorBody();            // 0x542450 (CPhysical), Physical.cpp
uintptr_t Lote140ATrailerDestructorBody();             // 0x6CED10 (CTrailer), Trailer.cpp
uintptr_t Lote140AAnimatedBuildingEntityDestructor();  // 0x535E90 (~CEntity llamado por el cuerpo de CAnimatedBuilding), AnimatedBuilding.cpp
uintptr_t Lote140ACarDriveWanderDestructorBody();      // 0x63CB50 (CTaskComplexCarDriveWander), TaskComplexDriveWander.cpp

namespace F140ALive {
using namespace F140AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x4C5620, 0x4C5640, 0x544990, 0x54E8E0, 0x6CF180, 0x6D0450, 0x537A50, 0x537A70, 0x41BCB0, 0x41BCE0, 0x41BCF0, 0x41BD00, 0x41BD10, 0x41BD50, 0x41BD80,
                                                   0x68B9F0, 0x6908C0, 0x63C9C0, 0x643220, 0x65FB50, 0x6642A0};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F140AFix::Arena(WIN);
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

// Memoria global del juego que el banco toca: CTimer::ms_fTimeStep (0xB7CB5C), que lee la copia de GetTimeStep (funcion 13); se guarda al empezar y se restaura tras cada lado.
enum : unsigned { kRegTimeStep };
inline constexpr uintptr_t kTimeStep = 0xB7CB5C;

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos (no es this)
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this] (la vtable de la base)
inline DWORD gBenchThread{};

// Cuerpo del destructor (thiscall, sin argumentos de pila): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// operator delete (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
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

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    switch (c.fn) {
    case kIdxBaseDtor: case kIdxMatrixThunk: case kIdxAnimBody:   // cuerpos de destructor: [this] = vtable previa; el falso (si lo hay) devuelve gRet y deja gDtorWrite
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = 0; call.popBytes = 0;
        break;
    case kIdxTimeCtor: case kIdxTransCtor: case kIdxRight: case kIdxForward: case kIdxArrestType: case kIdxCarDriveType: case kIdxHassleVehType:   // `this` en la arena A (el EXE no lee la memoria o la escribe)
        arenaA.Put32(a[0], a[3]);
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = 0; call.popBytes = 0;
        break;
    case kIdxGetMatrix:   // [this+0x14] = m_matrix
        arenaA.Put32(a[0] + 0x14, a[3]);
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = 0; call.popBytes = 0;
        break;
    case kIdxBridge:      // cdecl, un argumento (lo limpia el llamador)
        call.args[0] = a[3]; call.argCount = 1; call.popBytes = 4;
        break;
    case kIdxSin: case kIdxCos:   // cdecl, un float en la pila
        call.args[0] = a[3]; call.argCount = 1; call.popBytes = 4;
        break;
    case kIdxTimeStep:    // cdecl sin argumentos; ms_fTimeStep = bits del caso
        PutGlobal32(kTimeStep, a[3]);
        call.argCount = 0; call.popBytes = 0;
        break;
    default:              // destructores con borrado: banderas en la pila (`ret 4`: lo limpia el callee)
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = u32(arenaA.Obj(a[0]));
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
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
        gRet = 0; gDtorWrite = 0;
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
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}};
        obs["gchanges"] = GlobalChanges(before);
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (fn) {
        case kIdxBridge: obs["ret"] = eax & 0xFFu; break;                                                               // bool: AL
        case kIdxBaseDtor: case kIdxMatrixThunk: case kIdxAnimBody: obs["ret"] = nullptr; break;                         // void
        case kIdxSin: case kIdxCos: case kIdxTimeStep: obs["ret"] = nullptr; break;                                      // el resultado esta en ST0 (imagen x87 de la fila)
        case kIdxGetMatrix: case kIdxArrestType: case kIdxCarDriveType: case kIdxHassleVehType: obs["ret"] = eax; break; // EAX completo (puntero crudo / constante)
        default: obs["ret"] = Tag(uintptr_t(eax)); break;                                                                // this (o this + 0x10) como etiqueta de arena
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN140A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN140A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn140a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn140a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn140a"); }
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
    // Colaborador SIN hook RH: la DLL tiene su propia funcion (direccion dada); el falso va en la direccion del EXE y en esa funcion. Se exige que de verdad no tenga hook.
    auto U = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        const uintptr_t body = Body(dllAddress);
        if (!body || ModuleOf(body) != env.asi) ready = false;
        return S(Pair{exe, body, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (operator delete del CRT): un solo parche, en el EXE.
    auto E = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    using VoidDelete = void (*)(void*);
    const uintptr_t vehicleDelete = reinterpret_cast<uintptr_t>(static_cast<VoidDelete>(&CVehicle::operator delete));
    const uintptr_t buildingDelete = reinterpret_cast<uintptr_t>(static_cast<VoidDelete>(&CBuilding::operator delete));
    env.stubs[kIdxPhysical] = {H(0x542450, Lote140APhysicalDestructorBody(), &FkBody<0x542450>), E(0x8214BD, &FkDelete<0x8214BD>)};
    env.stubs[kIdxMatrixThunk] = {F(0x59ACD0, &FkBody<0x59ACD0>)};
    env.stubs[kIdxTrailer] = {U(0x6CED10, Lote140ATrailerDestructorBody(), &FkBody<0x6CED10>), U(0x6E2D90, vehicleDelete, &FkDelete<0x6E2D90>)};
    env.stubs[kIdxAnimDelete] = {F(0x537A70, &FkBody<0x537A70>), U(0x4040A0, buildingDelete, &FkDelete<0x4040A0>)};
    env.stubs[kIdxAnimBody] = {H(0x535E90, Lote140AAnimatedBuildingEntityDestructor(), &FkBody<0x535E90>)};
    env.stubs[kIdxPolice] = {F(0x68D880, &FkBody<0x68D880>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[kIdxWander] = {U(0x63CB50, Lote140ACarDriveWanderDestructorBody(), &FkBody<0x63CB50>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[kIdxHasslePed] = {F(0x65FF60, &FkBody<0x65FF60>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[kIdxPhysical] = "toda la funcion salvo ~CPhysical (0x542450, funcion propia de la DLL con falso identico; el EXE tiene hook en esa direccion, apagado) y operator delete (0x8214BD, sin hook: la DLL lo llama por esa direccion, un solo parche)";
    dllReal[kIdxMatrixThunk] = "toda la funcion salvo CMatrix::Destructor (0x59ACD0, con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kIdxTrailer] = "toda la funcion salvo el cuerpo ~CTrailer (0x6CED10, funcion propia sin hook, falso identico) y CVehicle::operator delete (0x6E2D90, sin hook, falso identico)";
    dllReal[kIdxAnimDelete] = "toda la funcion salvo el cuerpo de CAnimatedBuilding (0x537A70, con hook propio del lote, falso identico) y CBuilding::operator delete (0x4040A0, sin hook, falso identico)";
    dllReal[kIdxAnimBody] = "toda la funcion salvo ~CEntity (0x535E90, con hook; la DLL llama a su funcion propia AnimatedBuildingEntityDestructor, falso identico en la direccion del EXE y en ella)";
    dllReal[kIdxPolice] = "toda la funcion salvo ~CTaskComplexPolicePursuit (0x68D880, envoltura Destructor() con falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)";
    dllReal[kIdxWander] = "toda la funcion salvo el cuerpo ~CTaskComplexCarDriveWander (0x63CB50, funcion propia sin hook, falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)";
    dllReal[kIdxHasslePed] = "toda la funcion salvo ~CTaskGangHasslePed (0x65FF60, envoltura Destructor() con falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)";
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
    regions.emplace_back(kTimeStep, 4);
    regionsOf[kIdxTimeStep] = {kRegTimeStep};
    Run(env, ready, registered, bodies, collaborators);
}
}
