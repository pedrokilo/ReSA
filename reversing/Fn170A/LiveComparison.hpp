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

// Banco par a par Fn170A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 22 funciones del EXE (ver Fixtures.hpp: GetTaskType de FireTruck, SwatRope y Fight; cuerpos de destructor de FireTruck, Prostitute y GangDriveBy; constructores de SwatRope y UseGun; destructores con borrado de SwatRope, UseGun, Prostitute y
// PlayHandSignalAnim; CreateSubTask de ObserveTrafficLightsAndAchieveHeading; Clone de KillAllThreats; GetBaseComboByName y GetAnimLoopFire de CWeaponInfo; Reverse de CPointRoute y CNodeRoute; destructor de CPatrolRoute; copia del cuerpo
// del destructor de CEntitySeekPosCalculator), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall / RH_ScopedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con
// objetos sinteticos en arenas propias (A, B, C con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): ~CTask 0x61A3A0, base de CTaskSimple 0x61A390,
// base y cuerpo de ~CTaskComplex 0x61A3B0 y 0x61A3D0, CTask::operator new 0x61A5A0 y delete 0x61A5B0, CEntity::CleanUpOldReference 0x571A00 y RegisterReference 0x571B70, CAnimBlendAssociation::SetDeleteCallback 0x4CEBC0,
// CAnimManager::RemoveAnimBlockRef 0x4D3FD0, el constructor de CTaskSimpleAchieveHeading 0x667E20 y los cuerpos de destructor 0x6594D0, 0x61DF30, 0x6726F0 y 0x61BDF0 (todos con hook).
// Memoria global del juego: el puntero de la tabla de grupos de animacion 0xB4EA34 (CTaskSimpleGangDriveBy::DestructorBody), que el banco guarda, rellena y restaura.
// Se observan: los bytes cambiados de las tres arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto
// (this o el objeto nuevo como etiqueta de arena; EAX completo en constantes y en GetBaseComboByName; AL en las funciones que devuelven bool) y el estado x87 (vacio, salvo ST0 en GetAnimLoopFire).
// EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F170ALive {
using namespace F170AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F170AFix::Arena(WIN);
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
inline u32 gRet{};                   // EAX que devuelven los falsos (no es this)
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this]
inline u32 gTaskNew{};               // lo que devuelve CTask::operator new
inline u32 gDefaultCb{}, gDefaultCbBody{};   // CDefaultAnimCallback::DefaultAnimCB de la DLL (y su cuerpo): se anota como la del EXE (0x4D1390)
inline DWORD gBenchThread{};

// operator delete / liberacion (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// Cuerpo del destructor o del constructor base (thiscall, sin argumentos de pila): evento [direccion del EXE, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CEntity::CleanUpOldReference (0x571A00) / RegisterReference (0x571B70) (thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]. No escribe nada.
template<u32 ADDR> inline void __fastcall FkRef(void* self, void*, void* ref) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = Tag(ref);
}
// CTask::operator new (0x61A5A0; cdecl, el tamano): evento [direccion, tamano]; devuelve gTaskNew (un objeto de la arena B o nulo).
inline void* __cdecl FkTaskNew(u32 size) {
    Ev& e = PushEv(0x61A5A0, 1);
    e.v[0] = size;
    return reinterpret_cast<void*>(uintptr_t(gTaskNew));
}
// Constructor de CTaskSimpleAchieveHeading (0x667E20; thiscall, tres dwords de pila, `ret 0Ch`): evento [direccion, ECX (etiqueta), rumbo, velocidad, tolerancia (bits)]; devuelve gRet. No escribe nada.
inline u32 __fastcall FkAchCtor(void* self, void*, u32 heading, u32 rate, u32 tolerance) {
    Ev& e = PushEv(0x667E20, 4);
    e.v[0] = Tag(self); e.v[1] = heading; e.v[2] = rate; e.v[3] = tolerance;
    return gRet;
}
// CAnimManager::RemoveAnimBlockRef (0x4D3FD0; cdecl, el indice del bloque): evento [direccion, indice].
inline void __cdecl FkRemoveRef(u32 index) {
    Ev& e = PushEv(0x4D3FD0, 1);
    e.v[0] = index;
}
// CAnimBlendAssociation::SetDeleteCallback (0x4CEBC0; thiscall, dos dwords de pila, `ret 8`): evento [direccion, ECX (etiqueta), funcion (la DefaultAnimCB de la DLL se anota como 0x4D1390), datos]. No escribe nada.
inline void __fastcall FkSetDelete(void* self, void*, u32 callback, u32 data) {
    Ev& e = PushEv(0x4CEBC0, 3);
    e.v[0] = Tag(self);
    e.v[1] = (callback == gDefaultCb || callback == gDefaultCbBody) ? 0x4D1390u : callback;
    e.v[2] = data;
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
};

inline void PutRaw32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }
inline constexpr uintptr_t kGroupTableGlobal = 0xB4EA34;   // CAnimManager::ms_aAnimAssocGroups (puntero a la tabla de grupos de animacion)

// Prepara arenas del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    gRet = 0; gDtorWrite = 0; gTaskNew = 0;
    const u32 o = a[0];
    const u32 self = u32(arenaA.Obj(o));
    call.ecx = self;
    const auto ent = [&](u32 mode, u32 off, u32 raw, Arena<kWin>& arena) { return mode == 0 ? 0u : (mode == 1 ? u32(arena.Obj(off)) : raw); };
    switch (kKind[c.fn]) {
    case kConst:
        arenaA.Put32(o, a[3]);
        break;
    case kBool:
        arenaA.Put32(o, a[3]);
        call.args[0] = a[4]; call.argCount = 1;
        break;
    case kFtBody:
        arenaA.Put32(o, a[3]);
        arenaA.Put32(o + 0xC, ent(a[4], a[5], a[6], arenaB)); arenaA.Put32(o + 0x10, a[16]); arenaA.Put32(o + 0x14, ent(a[7], a[8], a[9], arenaC)); arenaA.Put32(o + 0x18, a[17]);
        gRet = a[10]; gDtorWrite = a[11];
        break;
    case kSwatCtor:
        arenaA.Put32(o, a[3]); arenaA.Put32(o + 0xC, a[10]); arenaA.Put32(o + 0x10, a[5]); arenaA.Put32(o + 0x14, a[6]); arenaA.Put32(o + 0x18, a[7]);
        gRet = a[8]; gDtorWrite = a[9];
        call.args[0] = a[4]; call.argCount = 1;
        break;
    case kSdd:
        arenaA.Put32(o, a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kObsSub:
        arenaA.Put32(o, a[5]); arenaA.Put32(o + 0x10, a[4]);
        gTaskNew = a[6] == 0 ? 0u : u32(arenaB.Obj(a[7]));
        gDtorWrite = a[10];
        gRet = a[8] == 0 ? gTaskNew : (a[8] == 1 ? a[9] : 0u);
        call.args[0] = a[3]; call.argCount = 1;
        break;
    case kGunCtor:
        arenaA.Put32(o, a[3]);
        gRet = a[17]; gDtorWrite = a[18];
        call.args = {ent(a[4], a[5], a[6], arenaB), a[7], a[8], a[9], a[10], a[11], a[16], 0}; call.argCount = 7;
        break;
    case kCombo:
        for (size_t i = 0; i < c.s.size(); ++i) arenaC.Put8(o + i, c.s[i]);
        call.args[0] = u32(arenaC.Obj(o)); call.argCount = 1; call.popBytes = 4;   // cdecl: limpia el llamador
        break;
    case kFloat:
        arenaA.Put32(o + 0x48, a[4]); arenaA.Put32(o + 0x54, a[5]);
        call.args[0] = a[3]; call.argCount = 1;
        break;
    case kRoutePt: case kRouteNode:
        arenaA.Put32(o, a[3]);
        break;
    case kEmpty: case kVtOnly:
        arenaA.Put32(o, a[3]);
        break;
    case kBodyBase:
        arenaA.Put32(o, a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        break;
    case kGangBody: {
        arenaA.Put32(o, a[3]); arenaA.Put32(o + 0x8, a[4]); arenaA.Put32(o + 0x28, a[5]);
        arenaA.Put32(o + 0x2C, ent(a[8], a[9], a[10], arenaB));
        arenaA.Put32(o + 0x34, a[16] == 0 ? 0u : a[17]);
        const bool used = ((a[4] >> 8) & 0xFFu) != 0;
        if (used) arenaC.Put32(a[7] + 20u * a[5], a[6]);
        PutRaw32(kGroupTableGlobal, u32(arenaC.Obj(a[7])));
        gRet = a[19]; gDtorWrite = a[18];
        break;
    }
    case kKillClone:
        arenaA.Put32(o + 0xC, a[3]); arenaA.Put32(o + 0x10, a[4]); arenaA.Put32(o + 0x14, a[5]);
        gTaskNew = a[6] == 0 ? 0u : u32(arenaB.Obj(a[7]));
        gDtorWrite = a[8]; gRet = a[9];
        break;
    }
    return call;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data()))});   // direcciones reales de las arenas (para el oraculo)
    if (kKind[fn] == kGangBody) input["group_table_before"] = BitsOf(reinterpret_cast<const void*>(kGroupTableGlobal));   // valor previo del puntero (informativo)
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
        case kConst: case kCombo: obs["ret"] = eax; break;                                                   // valor completo de EAX
        case kBool: obs["ret"] = eax & 0xFFu; break;                                                         // solo AL
        case kSdd: case kSwatCtor: case kObsSub: case kGunCtor: case kKillClone: obs["ret"] = Tag(uintptr_t(eax)); break;   // this / el objeto nuevo como etiqueta de arena
        default: obs["ret"] = nullptr; break;                                                                // void (o float en ST0, visible en el estado x87)
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN170A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN170A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn170a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn170a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn170a"); }
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
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    auto Del = [&] { return F(0x61A5B0, &FkDelete<0x61A5B0>); };   // CTask::operator delete (con hook)
    env.stubs[1] = {F(0x571A00, &FkRef<0x571A00>), F(0x61A3D0, &FkBody<0x61A3D0>)};
    env.stubs[2] = {F(0x61A3B0, &FkBody<0x61A3B0>)};
    env.stubs[6] = {F(0x6594D0, &FkBody<0x6594D0>), Del()};
    env.stubs[7] = {F(0x61A5A0, &FkTaskNew), F(0x61A3B0, &FkBody<0x61A3B0>), F(0x667E20, &FkAchCtor)};
    env.stubs[8] = {F(0x61A390, &FkBody<0x61A390>), F(0x571B70, &FkRef<0x571B70>)};
    env.stubs[9] = {F(0x61DF30, &FkBody<0x61DF30>), Del()};
    env.stubs[12] = {Rt(13, &FkBody<0x6726F0>), Del()};
    env.stubs[13] = {F(0x61A3D0, &FkBody<0x61A3D0>)};
    env.stubs[14] = {F(0x61BDF0, &FkBody<0x61BDF0>), Del()};
    env.stubs[20] = {F(0x4D3FD0, &FkRemoveRef), F(0x4CEBC0, &FkSetDelete), F(0x571A00, &FkRef<0x571A00>), F(0x61A3A0, &FkBody<0x61A3A0>)};
    env.stubs[21] = {F(0x61A5A0, &FkTaskNew), F(0x61A3B0, &FkBody<0x61A3B0>)};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({env.stubs[f].empty() ? "toda la funcion (sin colaboradores)" : "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)"})},
                                 {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", nullptr}};
    }
    // Memoria global: el puntero de la tabla de grupos de animacion, que solo toca CTaskSimpleGangDriveBy::DestructorBody (indice 20)
    regions.clear();
    regions.emplace_back(kGroupTableGlobal, 4);
    for (auto& v : regionsOf) v.clear();
    regionsOf[20] = {0};
    Run(env, ready, registered, bodies, collaborators);
}
}
