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
#include "TaskComplexSeekEntity.h"

// Banco par a par Fn159A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 23 funciones del EXE (destructores con borrado de once tareas, constantes virtuales de CTaskSimpleDie, CTaskSimpleGetUp y CTaskSimpleLand, cuerpos de destructor de esas tres tareas, tres alias del constructor de
// CBoundingBox, constructor de CAnimatedBuilding, CloneAtomicToClumpCB y CCollisionData::GetNoOfShdwVerts; ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall /
// RH_ScopedInstall / RH_ScopedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40). Falsos de los colaboradores: el
// mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): cuerpos de destructor (con hook propio, `H`: el hook se apaga y el falso va en la envoltura de la DLL; con hook y funcion propia,
// `F`: el cuerpo del hook; sin hook, `U`: falso en la direccion del EXE y en la envoltura de la DLL), CTask::operator delete 0x61A5B0, ~CTask 0x61A3A0, SetFinishCallback 0x4CEBE0, constructor de CBoundingBox 0x40FAF0,
// constructor de CEntity 0x532A90 y RpAtomicSetFrame 0x74BF20 (todos con hook), y RpAtomicClone 0x749E60 y RpClumpAddAtomic 0x74A490 (sin hook: la DLL los llama por su direccion del EXE). 0x403E00 (constructor de
// CBuilding, sin hook) corre REAL en el EXE; la DLL pliega sus efectos.
// Se observan: los bytes cambiados de las tres arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto
// (this o el atomic como etiqueta de arena; EAX completo en constantes y en GetNoOfShdwVerts) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue
// (informativos).

// Funciones propias de la DLL (envolturas) que exportan los .cpp de cada clase para que el banco las sustituya.
uintptr_t Lote159ABody_CTaskSimpleCarJumpOut();                    // 0x647490 (con hook propio, Destructor)
uintptr_t Lote159ABody_CTaskSimpleCarForcePedOut();                // 0x647790 (con hook propio, Destructor)
uintptr_t Lote159ABody_CTaskSimpleCarSlowDragPedOut();             // 0x648070 (con hook propio, Destructor)
uintptr_t Lote159ABody_CTaskComplexGoToCarDoorAndStandStill();     // 0x64A580 (con hook propio, Destructor)
uintptr_t Lote159ABody_CTaskSimpleDieInCar();                      // 0x62FC70 (con hook propio, Destructor)
uintptr_t Lote159ABody_CTaskComplexDieInCar();                     // 0x62FCF0 (sin hook)

namespace F159ALive {
using namespace F159AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F159AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void PutBytes(size_t off, const u8* p, size_t n) { memcpy(m.data() + kGuard + off, p, n); }
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
inline u32 gRet{};                   // EAX que devuelven los falsos (no es this)
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this]
inline u32 gClone{};                 // lo que devuelve el falso de RpAtomicClone
inline u32 gDefaultCb{}, gDefaultCbBody{};   // CDefaultAnimCallback::DefaultAnimCB de la DLL (y su cuerpo): se anota como la del EXE (0x4D1390)
inline DWORD gBenchThread{};

// operator delete / liberacion (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// Cuerpo del destructor, ~CTask, constructor de CBoundingBox y de CEntity (thiscall, sin argumentos de pila): evento [direccion del EXE, ECX (etiqueta de arena), dword que tenia [ECX] al entrar];
// deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CAnimBlendAssociation::SetFinishCallback (0x4CEBE0; thiscall, dos dwords de pila, `ret 8`): evento [direccion, ECX (etiqueta), funcion (la DefaultAnimCB de la DLL se anota como 0x4D1390), datos]. No escribe nada.
inline void __fastcall FkSetFinish(void* self, void*, u32 callback, u32 data) {
    Ev& e = PushEv(0x4CEBE0, 3);
    e.v[0] = Tag(self);
    e.v[1] = (callback == gDefaultCb || callback == gDefaultCbBody) ? 0x4D1390u : callback;
    e.v[2] = data;
}
// RpAtomicClone (0x749E60; cdecl, un puntero): evento [direccion, atomic (etiqueta)]; devuelve gClone.
inline u32 __cdecl FkRpAtomicClone(u32 atomic) {
    Ev& e = PushEv(0x749E60, 1);
    e.v[0] = Tag(uintptr_t(atomic));
    return gClone;
}
// RpAtomicSetFrame (0x74BF20; cdecl, atomic y marco): evento [direccion, atomic (etiqueta), marco (etiqueta)]; devuelve gRet.
inline u32 __cdecl FkRpAtomicSetFrame(u32 atomic, u32 frame) {
    Ev& e = PushEv(0x74BF20, 2);
    e.v[0] = Tag(uintptr_t(atomic));
    e.v[1] = Tag(uintptr_t(frame));
    return gRet;
}
// RpClumpAddAtomic (0x74A490; cdecl, clump y atomic): evento [direccion, clump (etiqueta), atomic (etiqueta)]; devuelve gRet.
inline u32 __cdecl FkRpClumpAddAtomic(u32 clump, u32 atomic) {
    Ev& e = PushEv(0x74A490, 2);
    e.v[0] = Tag(uintptr_t(clump));
    e.v[1] = Tag(uintptr_t(atomic));
    return gRet;
}
template<size_t... I> inline std::array<void*, kFunctions> MakeBodyFakes(std::index_sequence<I...>) { return {reinterpret_cast<void*>(&FkBody<kBodyExe[I]>)...}; }
inline const std::array<void*, kFunctions> kBodyFakes = MakeBodyFakes(std::make_index_sequence<kFunctions>{});


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

// Prepara arenas del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    gRet = 0; gDtorWrite = 0; gClone = 0;
    const u32 thisA = u32(arenaA.Obj(a[0]));
    switch (kKind[c.fn]) {
    case kConst:
        arenaA.Put32(a[0], a[3]);
        call.ecx = thisA;
        break;
    case kSdd:       // thiscall con banderas en la pila (`ret 4`)
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = thisA;
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        break;
    case kAnimBody: {
        const u32 anim = a[6] == 0 ? 0u : (a[6] == 1 ? u32(arenaB.Obj(a[7])) : a[8]);
        arenaA.Put32(a[0], a[3]);
        arenaA.Put32(a[0] + (c.fn == 14 ? 0x24 : (c.fn == 15 ? 0xC : 8)), anim);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = thisA;
        break;
    }
    case kBBox:
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = thisA;
        break;
    case kBuildCtor:
        arenaA.Put32(a[0], a[3]); arenaA.Put32(a[0] + 0x1C, a[6]); arenaA.Put32(a[0] + 0x34, a[7]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = thisA;
        break;
    case kCloneAtomic: {   // cdecl(atomic, clump): lo limpia el llamador
        const u32 frame = a[3] == 0 ? u32(arenaB.Obj(a[4])) : (a[3] == 1 ? u32(arenaC.Obj(a[4])) : (a[3] == 2 ? thisA : u32(arenaA.Obj(a[4]))));
        const u32 root = a[5] == 0 ? a[6] : (a[5] == 1 ? u32(arenaB.Obj(a[6])) : (a[5] == 2 ? u32(arenaC.Obj(a[6])) : 0u));
        gClone = a[7] == 0 ? u32(arenaC.Obj(a[8])) : (a[7] == 1 ? a[8] : 0u);
        gRet = a[10];
        // primero la raiz del marco (+A0h), despues el puntero al marco en el atomic (+4): el orden importa si se solapan
        if (a[3] == 0) arenaB.Put32(a[4] + 0xA0, root);
        else if (a[3] == 1) arenaC.Put32(a[4] + 0xA0, root);
        else arenaA.Put32(a[3] == 2 ? a[0] + 0xA0 : a[4] + 0xA0, root);
        arenaA.Put32(a[0] + 4, frame);
        call.ecx = c.ecx;
        call.args[0] = thisA; call.args[1] = a[9]; call.argCount = 2; call.popBytes = 8;
        break;
    }
    case kShadow: {
        arenaA.Put32(a[0] + 0x20, a[4]);
        u32 ptr = a[5];
        if (a[3] >= 2) {
            auto& arena = a[3] == 2 ? arenaB : arenaC;
            arena.PutBytes(a[6], c.s.data(), c.s.size());
            ptr = u32(arena.Obj(a[6]));
        }
        arenaA.Put32(a[0] + 0x2C, ptr);
        call.ecx = thisA;
        break;
    }
    }
    return call;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data()))});   // direcciones reales de las arenas (para el oraculo)
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
        case kSdd: case kBBox: case kBuildCtor: case kCloneAtomic: obs["ret"] = Tag(uintptr_t(eax)); break;   // this o el atomic como etiqueta de arena
        case kConst: case kShadow: obs["ret"] = eax; break;                                                      // valor completo de EAX
        default: obs["ret"] = nullptr; break;                                                                    // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN159A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN159A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn159a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn159a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn159a"); }
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
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}, {"virtual", vbody != 0}, {"body", env.route[f]->Body()}, {"body_module", ModuleOf(env.route[f]->Body())}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { collaborators["own_hooks"] = ownHooks; Run(env, false, registered, bodies, collaborators); return; }
    gDefaultCb = std::bit_cast<u32>(&CDefaultAnimCallback::DefaultAnimCB); gDefaultCbBody = u32(Body(gDefaultCb));
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
    // Colaborador SIN hook RH que la DLL llama en una funcion propia (direccion dada): codigo original en el EXE (comprobado), falso en la direccion del EXE y en esa funcion de la DLL.
    auto U = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;   // no debe tener hook
        Pair p{exe, Body(dllAddress), false};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Rutina del EXE sin hook que la DLL llama por su direccion del EXE (RenderWare): falso solo en la direccion del EXE.
    auto X = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;   // no debe tener hook
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    auto Del = [&] { return F(0x61A5B0, &FkDelete<0x61A5B0>); };   // CTask::operator delete (con hook)
    const auto& K = kBodyFakes;
    env.stubs[0] = {H(0x647490, Lote159ABody_CTaskSimpleCarJumpOut(), K[0]), Del()};
    env.stubs[1] = {H(0x647790, Lote159ABody_CTaskSimpleCarForcePedOut(), K[1]), Del()};
    env.stubs[2] = {F(0x647950, K[2]), Del()};
    env.stubs[3] = {H(0x648070, Lote159ABody_CTaskSimpleCarSlowDragPedOut(), K[3]), Del()};
    env.stubs[4] = {H(0x64A580, Lote159ABody_CTaskComplexGoToCarDoorAndStandStill(), K[4]), Del()};
    env.stubs[5] = {H(0x62FC70, Lote159ABody_CTaskSimpleDieInCar(), K[5]), Del()};
    env.stubs[6] = {U(0x62FCF0, Lote159ABody_CTaskComplexDieInCar(), K[6]), Del()};
    env.stubs[7] = {F(0x678160, K[7]), Del()};
    env.stubs[8] = {F(0x678310, K[8]), Del()};
    env.stubs[9] = {F(0x6790B0, K[9]), Del()};
    env.stubs[10] = {F(0x679AF0, K[10]), Del()};
    for (unsigned f : {14u, 15u, 16u}) env.stubs[f] = {F(0x4CEBE0, &FkSetFinish), F(0x61A3A0, K[f])};
    for (unsigned f : {17u, 18u, 19u}) env.stubs[f] = {F(0x40FAF0, K[f])};
    env.stubs[20] = {F(0x532A90, K[20])};
    env.reals[20] = {Pair{0x403E00, 0, false}};     // constructor de CBuilding: sin hook, codigo del EXE; la DLL pliega sus efectos
    env.stubs[21] = {X(0x749E60, &FkRpAtomicClone), F(0x74BF20, &FkRpAtomicSetFrame), X(0x74A490, &FkRpClumpAddAtomic)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {11u, 12u, 13u, 22u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[20] = "toda la funcion salvo el constructor de CEntity falseado; los efectos de 0x403E00 (constructor de CBuilding) se pliegan en la funcion";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        for (const auto& r : env.reals[f]) {
            env.watches.emplace_back(r.exe, 16);
            real.push_back({{"exe", r.exe}, {"dll", r.dll != 0}, {"hooked", r.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", nullptr}};
    }
    // Ninguna funcion toca memoria global del juego: todo ocurre en las tres arenas propias
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    Run(env, ready, registered, bodies, collaborators);
}
}
