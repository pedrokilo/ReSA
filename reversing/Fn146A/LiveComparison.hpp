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

// Banco par a par Fn146A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 22 funciones diminutas del EXE (nueve destructores con borrado de tareas y de CAEMFDecoder, Shutdown de CAEMFDecoder, CPad::IsRadioTrackSkipPressed, GetShadowCamera, constructor y destructor de
// CRealTimeShadowManager, operator new / delete de CBuilding, CDummy y CPatrolRoute, el escalado de matriz 0x5A2E60 y el constructor de ProcSurfaceInfo_c; ver Fixtures.hpp para la lista y el esquema de familias),
// cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40) y,
// donde hace falta, memoria global real del juego (el puntero del pool de edificios, dummies o rutas de patrulla; se guarda al empezar y se restaura tras cada lado). Falsos de los colaboradores: el mismo falso en
// la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): cuerpos de destructor (con hook y funcion propia de la DLL distinta del hook, H; con hook cuya funcion llama la DLL directamente, F; o copia del
// EXE sin hook, U), CTask::operator delete (0x61A5B0 con hook), operator delete del CRT (0x8214BD sin hook, la DLL lo llama por esa direccion), CShadowCamera::Destroy (0x705400 con hook), las funciones New de los
// pools (0x403FA0, 0x532630, 0x41B660, con hook) y las dos funciones de QuickTime de Shutdown (0x819840 y 0x819770, sin hook, la DLL las llama por su direccion).
// Los operator delete de los pools leen el puntero del pool (0xB74498, 0xB744A0, 0xB744B4), que el banco apunta a un pool falso de la arena B (+0 almacenamiento, +4 estados, +0xC cursor) con los estados en la arena C.
// Se observan: los bytes cambiados de las tres arenas y de la memoria global, los eventos de los falsos (con sus argumentos), el valor devuelto (EAX completo, `this` como etiqueta o AL en CPad) y el estado x87
// (vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Funciones propias de la DLL (cuerpos de destructor que llaman los destructores con borrado): las exportan los .cpp de cada clase.
uintptr_t Lote146ABeInShopDestructorBody();             // 0x675460 (CTaskInteriorBeInShop), TaskInteriorBeInShop.cpp
uintptr_t Lote146AShopKeeperDestructorBody();           // 0x6756A0 (CTaskInteriorShopKeeper), TaskInteriorShopKeeper.cpp
uintptr_t Lote146AWaitUntilPedIsOutCarDestructorBody(); // 0x6494F0 (CTaskSimpleWaitUntilPedIsOutCar), TaskSimpleWaitUntilPedIsOutCar.cpp
uintptr_t Lote146AWaitUntilPedIsInCarDestructorBody();  // 0x6496A0 (CTaskSimpleWaitUntilPedIsInCar), TaskSimpleWaitUntilPedIsInCar.cpp
uintptr_t Lote146AComplexEvasiveStepDestructorBody();   // 0x653350 (CTaskComplexEvasiveStep), TaskComplexEvasiveStep.cpp
uintptr_t Lote146AEvasiveDiveDestructorBody();          // 0x6535D0 (CTaskSimpleEvasiveDive), TaskSimpleEvasiveDive.cpp
uintptr_t Lote146AGoToPointDestructorBody();            // copia 0x668240 del cuerpo de 0x668630 (CTaskComplexGoToPointAndStandStill), TaskComplexGoToPointAndStandStill.cpp
uintptr_t Lote146AMFDecoderDestructorBody();            // 0x4E77C0 (CAEMFDecoder), AEMFDecoder.cpp

namespace F146ALive {
using namespace F146AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x676480, 0x6764A0, 0x64D1E0, 0x64D200, 0x655EE0, 0x655F00, 0x66DB60, 0x66DB80, 0x4E7C50, 0x4E7850, 0x4E7F20,
                                                   0x7059E0, 0x706770, 0x7067A0, 0x404090, 0x4040A0, 0x5326E0, 0x5326F0, 0x41B810, 0x41B820, 0x5A2E60, 0x5A2E90};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F146AFix::Arena(WIN);
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

// Memoria global del juego que el banco toca: los punteros de los pools de edificios (0xB74498), dummies (0xB744A0) y rutas de patrulla (0xB744B4), que leen operator new / delete de cada clase (funciones 14..19);
// se guardan al empezar y se restauran tras cada lado.
enum : unsigned { kRegBuilding, kRegDummy, kRegPatrol };
inline constexpr uintptr_t kPoolAddr[3] = {0xB74498, 0xB744A0, 0xB744B4};
inline constexpr unsigned PoolRegion(unsigned fn) { return (fn == kIdxBldNew || fn == kIdxBldDel) ? kRegBuilding : (fn == kIdxDumNew || fn == kIdxDumDel) ? kRegDummy : kRegPatrol; }

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
// Funcion de la libreria sin argumentos (QuickTime): evento [direccion]; devuelve gRet.
template<u32 ADDR> inline u32 __cdecl FkVoid() {
    PushEv(ADDR, 0);
    return gRet;
}
// New del pool (thiscall, ECX = puntero del pool, que puede ser crudo y no se desreferencia): evento [direccion del EXE, ECX (etiqueta de arena o crudo)]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkPoolNew(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
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
    std::array<u32, 5> args{};
    u32 argCount{}, popBytes{}, ecx{};
};

inline void PutGlobal32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    if (IsScalarDeleting(c.fn)) {   // destructores con borrado: banderas en la pila (`ret 4`: lo limpia el callee)
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = u32(arenaA.Obj(a[0]));
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        return call;
    }
    if (IsPoolNew(c.fn)) {          // puntero del pool (global) = pool falso de la arena B o crudo; un argumento de pila (cdecl, lo limpia el llamador)
        const u32 pool = (a[6] & 1) ? a[7] : u32(arenaB.Obj(a[0]));
        PutGlobal32(kPoolAddr[PoolRegion(c.fn)], pool);
        gRet = a[4];
        call.args[0] = a[3]; call.argCount = 1; call.popBytes = 4;
        return call;
    }
    if (IsPoolDelete(c.fn)) {       // pool falso en la arena B: +0 almacenamiento, +4 estados (para que estados[indice] caiga en la arena C), +8 relleno, +0xC cursor
        const i32 e = i32(ElementSize(c.fn));
        const i32 t = i32(a[4]) / e;                                           // indice (division con signo, truncada hacia cero)
        arenaB.Put32(a[0] + 0x0, a[3]);
        arenaB.Put32(a[0] + 0x4, u32(arenaC.Obj(a[6])) - u32(t));
        arenaB.Put32(a[0] + 0x8, a[7]);
        arenaB.Put32(a[0] + 0xC, a[5]);
        PutGlobal32(kPoolAddr[PoolRegion(c.fn)], u32(arenaB.Obj(a[0])));
        call.args[0] = a[3] + a[4]; call.argCount = 1; call.popBytes = 4;      // puntero = almacenamiento + d
        return call;
    }
    switch (c.fn) {
    case kIdxShutdown:     // sin argumentos ni this: solo los dos falsos de QuickTime
        gRet = a[4];
        call.argCount = 0; call.popBytes = 0;
        break;
    case kIdxPadSkip:      // palabras +0x2E (estado nuevo) y +0x5E (estado viejo)
        { const u16 nw = u16(a[3]), ow = u16(a[4]); memcpy(arenaA.m.data() + kGuard + a[0] + 0x2E, &nw, 2); memcpy(arenaA.m.data() + kGuard + a[0] + 0x5E, &ow, 2); }
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = 0; call.popBytes = 0;
        break;
    case kIdxShadowCam: case kIdxMgrCtor: case kIdxSurfCtor:   // el objeto queda con el relleno de la arena
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = 0; call.popBytes = 0;
        break;
    case kIdxMgrDtor:      // dwords +0x44 y +0x4C (las camaras); el falso de CShadowCamera::Destroy los deja en gDtorWrite
        arenaA.Put32(a[0] + 0x44, a[3]); arenaA.Put32(a[0] + 0x4C, a[6]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = 0; call.popBytes = 0;
        break;
    case kIdxScale:        // nueve floats en las tres filas; x, y, z en la pila (thiscall, `ret 0Ch`: lo limpia el callee)
        for (unsigned r = 0; r < 3; ++r) for (unsigned k = 0; k < 3; ++k) arenaA.Put32(a[0] + 16 * r + 4 * k, a[16 + 3 * r + k]);
        call.ecx = u32(arenaA.Obj(a[0])); call.args = {a[3], a[4], a[5], 0u, 0u}; call.argCount = 3; call.popBytes = 0;
        break;
    default: break;
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
        if (IsScalarDeleting(fn) || fn == kIdxShadowCam || fn == kIdxMgrCtor || fn == kIdxSurfCtor) obs["ret"] = Tag(uintptr_t(eax));   // this (o this + 8) como etiqueta de arena
        else if (IsPoolNew(fn)) obs["ret"] = eax;                                                                                         // EAX completo
        else if (fn == kIdxPadSkip) obs["ret"] = eax & 0xFFu;                                                                             // bool: AL
        else obs["ret"] = nullptr;                                                                                                        // void
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN146A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN146A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn146a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn146a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn146a"); }
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
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (operator delete del CRT, funciones de QuickTime): un solo parche, en el EXE.
    auto E = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    env.stubs[0] = {H(0x675460, Lote146ABeInShopDestructorBody(), &FkBody<0x675460>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[1] = {H(0x6756A0, Lote146AShopKeeperDestructorBody(), &FkBody<0x6756A0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[2] = {H(0x6494F0, Lote146AWaitUntilPedIsOutCarDestructorBody(), &FkBody<0x6494F0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[3] = {H(0x6496A0, Lote146AWaitUntilPedIsInCarDestructorBody(), &FkBody<0x6496A0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[4] = {H(0x653350, Lote146AComplexEvasiveStepDestructorBody(), &FkBody<0x653350>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[5] = {H(0x6535D0, Lote146AEvasiveDiveDestructorBody(), &FkBody<0x6535D0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[6] = {F(0x667E70, &FkBody<0x667E70>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[7] = {U(0x668240, Lote146AGoToPointDestructorBody(), &FkBody<0x668240>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[kIdxMfDelete] = {H(0x4E77C0, Lote146AMFDecoderDestructorBody(), &FkBody<0x4E77C0>), E(0x8214BD, &FkDelete<0x8214BD>)};
    env.stubs[kIdxShutdown] = {E(0x819840, &FkVoid<0x819840>), E(0x819770, &FkVoid<0x819770>)};
    env.stubs[kIdxMgrDtor] = {F(0x705400, &FkBody<0x705400>)};
    env.stubs[kIdxBldNew] = {F(0x403FA0, &FkPoolNew<0x403FA0>)};
    env.stubs[kIdxDumNew] = {F(0x532630, &FkPoolNew<0x532630>)};
    env.stubs[kIdxPatNew] = {F(0x41B660, &FkPoolNew<0x41B660>)};
    // colaborador REAL (sin falso): TList_c (lista de objetos) del constructor de ProcSurfaceInfo_c; el EXE corre sus bytes, la DLL lo lleva en linea
    env.reals[kIdxSurfCtor] = {Pair{0x4A8DD0, 0, false}};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion (sin colaboradores)";
    for (unsigned f = 0; f <= 5; ++f) dllReal[f] = "toda la funcion salvo el cuerpo del destructor de la clase (funcion propia de la DLL, falso identico en la direccion del EXE del cuerpo y en esa funcion) y CTask::operator delete (0x61A5B0, con hook, falso identico)";
    dllReal[6] = "toda la funcion salvo el cuerpo del destructor (0x667E70, con hook, funcion de la DLL llamada directamente, falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)";
    dllReal[7] = "toda la funcion salvo el cuerpo del destructor (0x668240, copia sin hook del cuerpo de 0x668630; la DLL llama a GoToPointAndStandStillDestructorBody, falso identico en 0x668240 y en esa funcion) y CTask::operator delete (0x61A5B0, con hook, falso identico)";
    dllReal[kIdxMfDelete] = "toda la funcion salvo el cuerpo del destructor (0x4E77C0, con hook; la DLL llama a una funcion propia, falso identico) y operator delete (0x8214BD, CRT, sin hook: la DLL lo llama por esa direccion, un solo parche)";
    dllReal[kIdxShutdown] = "toda la funcion salvo ExitMovies (0x819840) y TerminateQTML (0x819770), sin hook: la DLL las llama por su direccion (un solo parche en el EXE cada una)";
    dllReal[kIdxMgrDtor] = "toda la funcion salvo CShadowCamera::Destroy (0x705400, con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kIdxBldNew] = "toda la funcion salvo CBuildingPool::New (0x403FA0, con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kIdxDumNew] = "toda la funcion salvo CDummyPool::New (0x532630, con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kIdxPatNew] = "toda la funcion salvo CPatrolRoutePool::New (0x41B660, con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kIdxSurfCtor] = "toda la funcion (la lista de objetos, 0x4A8DD0 en el EXE, es codigo real en el EXE y va en linea en la DLL)";
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
    for (const uintptr_t addr : kPoolAddr) regions.emplace_back(addr, 4);
    regionsOf[kIdxBldNew] = regionsOf[kIdxBldDel] = {kRegBuilding};
    regionsOf[kIdxDumNew] = regionsOf[kIdxDumDel] = {kRegDummy};
    regionsOf[kIdxPatNew] = regionsOf[kIdxPatDel] = {kRegPatrol};
    Run(env, ready, registered, bodies, collaborators);
}
}
