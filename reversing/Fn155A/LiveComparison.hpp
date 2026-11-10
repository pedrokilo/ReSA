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

// Banco par a par Fn155A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 17 funciones del EXE (PickAmbientGunFire de CAEGlobalWeaponAudioEntity; cuatro destructores con borrado de tareas y dos de eventos; cuerpos de destructor de CTaskSimpleUseGun y CEventLeaderExitedCarAsDriver; cinco constantes
// virtuales y dos AffectsPed de CEventDontJoinPlayerGroup y CEventLeaderExitedCarAsDriver; Clone de CEventDontJoinPlayerGroup; ComputeAnimID de CTaskSimpleCarSlowBeDraggedOut; ver Fixtures.hpp), cada una con 16384 pares EXE/DLL.
// Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40). Falsos de los
// colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): cuerpos de destructor (con hook propio, `H`: el hook se apaga y el falso va en la envoltura de la DLL; con hook y
// funcion del hook, `F`; otra funcion probada del lote, `Rt`), CTask::operator delete 0x61A5B0 y CEvent::operator delete 0x4B5630 (con hook), el RNG del audio 0x4D9C10, CEntity::CleanUpOldReference 0x571A00,
// CAnimBlendAssociation::SetDeleteCallback 0x4CEBC0, ~CTask 0x61A3A0, CEvent::operator new 0x4B5620, el constructor de CEventDontJoinPlayerGroup 0x6090E0 y CVehicleAnimGroup::GetGroup 0x6E3B00.
// Se observan: los bytes cambiados de las tres arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto
// (this o la salida como etiqueta de arena; EAX completo en constantes; solo AL en bool) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Funciones propias de la DLL (envolturas) que exportan los .cpp de cada clase para que el banco las sustituya.
uintptr_t Lote155ABody_CTaskSimpleGunControl();                 // 0x61F4D0 (con hook)
uintptr_t Lote155ABody_CEventDontJoinPlayerGroup();             // 0x609140 (con hook)
uintptr_t Lote155ABody_CTaskSimpleCarSetPedSlowDraggedOut();    // 0x648430 (con hook)
uintptr_t Lote155ABody_CTaskComplexCarSlowBeDraggedOut();       // 0x64C580 (con hook)

namespace F155ALive {
using namespace F155AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F155AFix::Arena(WIN);
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
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this] (la vtable de la base)
inline u32 gNew{};                   // lo que devuelve el CEvent::operator new falso (puntero a la arena B o nulo)
inline std::array<u32, 3> gRand{};   // valores que devuelve el RNG falso, en orden
inline unsigned gRandN{};
inline DWORD gBenchThread{};

// Cuerpo del destructor (thiscall, sin argumentos de pila): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// operator delete / liberacion (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// CEntity::CleanUpOldReference (0x571A00; thiscall, un puntero en la pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]. No toca la referencia.
inline void __fastcall FkCleanUp(void* self, void*, void* ref) { Ev& e = PushEv(0x571A00, 2); e.v[0] = Tag(self); e.v[1] = Tag(ref); }
// CAEAudioUtility::GetRandomNumberInRange<int32> (0x4D9C10; cdecl, dos enteros): evento [direccion, minimo, maximo]; devuelve el siguiente valor de gRand.
inline i32 __cdecl FkRand(i32 lo, i32 hi) {
    Ev& e = PushEv(0x4D9C10, 2);
    e.v[0] = u32(lo);
    e.v[1] = u32(hi);
    return i32(gRand[gRandN < 3 ? gRandN++ : 2]);
}
// CAnimBlendAssociation::SetDeleteCallback (0x4CEBC0; thiscall, dos argumentos de pila, `ret 8`): evento [direccion, ECX (etiqueta), funcion, dato (etiqueta)]. El DefaultAnimCB de la DLL se normaliza al del EXE
// (la direccion de la funcion no es observable por el juego; si el valor no es ninguno de los dos se anota crudo).
inline u32 gDllDefaultCb{};
inline void __fastcall FkSetDelCb(void* self, void*, void* cb, void* data) {
    Ev& e = PushEv(0x4CEBC0, 3);
    const u32 f = u32(uintptr_t(cb));
    e.v[0] = Tag(self);
    e.v[1] = (f == kEvDefaultCb || f == gDllDefaultCb) ? kEvDefaultCb : f;
    e.v[2] = Tag(data);
}
// CEvent::operator new (0x4B5620; cdecl, un tamano): evento [direccion, tamano]; devuelve gNew.
inline void* __cdecl FkNew(u32 size) { Ev& e = PushEv(0x4B5620, 1); e.v[0] = size; return reinterpret_cast<void*>(uintptr_t(gNew)); }
// Constructor de CEventDontJoinPlayerGroup (0x6090E0; thiscall, un puntero en la pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]; deja gDtorWrite en [ECX]; devuelve this.
inline void* __fastcall FkCtor(void* self, void*, void* player) {
    Ev& e = PushEv(0x6090E0, 2);
    e.v[0] = Tag(self);
    e.v[1] = Tag(player);
    memcpy(self, &gDtorWrite, 4);
    return self;
}
// CVehicleAnimGroup::GetGroup (0x6E3B00; thiscall, un identificador de animacion en la pila, `ret 4`): evento [direccion, ECX (crudo), animId]; devuelve gRet.
inline u32 __fastcall FkGetGroup(void* self, void*, u32 animId) { Ev& e = PushEv(0x6E3B00, 2); e.v[0] = u32(uintptr_t(self)); e.v[1] = animId; return gRet; }
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

inline void PutRaw32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }

// Prepara arenas del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    gRet = 0; gDtorWrite = 0; gNew = 0; gRand = {}; gRandN = 0;
    gDllDefaultCb = u32(uintptr_t(&CDefaultAnimCallback::DefaultAnimCB));
    const u32 self = u32(arenaA.Obj(a[0]));
    switch (kKind[c.fn]) {
    case kConst:
        arenaA.Put32(a[0], a[3]);
        call.ecx = self;
        break;
    case kConstAL1:   // thiscall con un argumento de pila (`ret 4`: lo limpia el callee)
        arenaA.Put32(a[0], a[3]);
        call.ecx = self;
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 0;
        break;
    case kEvBody:
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        break;
    case kPick: {     // thiscall con tres punteros en la pila (`ret 0Ch`)
        arenaA.Put32(a[0], a[3]);
        u32 p[3]{u32(arenaB.Obj(a[7])), u32(arenaB.Obj(a[8])), u32(arenaB.Obj(a[9]))};
        switch (a[6]) {
        case 1: p[1] = p[0]; break;
        case 2: p[2] = p[1]; break;
        case 3: p[2] = p[0]; break;
        case 4: p[1] = p[2] = p[0]; break;
        case 5: p[1] = p[0] + 1; break;
        case 6: p[2] = p[0] - 2; break;
        default: break;
        }
        gRand = {a[3], a[4], a[5]};
        call.ecx = self;
        call.args = {p[0], p[1], p[2], 0, 0, 0, 0, 0}; call.argCount = 3; call.popBytes = 0;
        break;
    }
    case kSdd:   // thiscall con banderas en la pila (`ret 4`: lo limpia el callee)
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        break;
    case kUseGunBody: {
        const u32 target = a[6] == 0 ? 0u : (a[6] == 1 ? u32(arenaB.Obj(a[7])) : a[8]);
        const u32 anim = a[9] == 0 ? 0u : (a[9] == 1 ? u32(arenaB.Obj(a[10])) : a[11]);
        arenaA.Put32(a[0], a[3]);
        arenaA.Put32(a[0] + 0x1C, target);
        arenaA.Put32(a[0] + 0x2C, anim);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        break;
    }
    case kClone: {
        const u32 player = a[4] ? u32(arenaC.Obj(a[5])) : a[3];
        arenaA.Put32(a[0] + 0xC, player);
        gNew = a[6] ? u32(arenaB.Obj(a[7])) : 0u;
        gDtorWrite = a[8];
        call.ecx = self;
        break;
    }
    case kAnimId: {   // thiscall con dos punteros en la pila (`ret 8`)
        const u32 veh = u32(arenaB.Obj(a[8])), hand = u32(arenaC.Obj(a[9]));
        u32 groupSlot = u32(arenaB.Obj(a[10])), animSlot = u32(arenaC.Obj(a[11]));
        arenaA.Put32(a[0] + 0x10, veh);
        arenaA.Put32(a[0] + 0x14, a[3]);
        arenaB.Put32(a[8] + kVehicleHandling, hand);
        arenaC.Put32(a[9] + 0xDC, a[16]);
        arenaC.Put8(a[9] + kHandlingGroupByte, u8(a[4]));
        switch (a[7]) {
        case 2: animSlot = self + 0x14; break;
        case 3: animSlot = hand + 0xDC; break;
        case 4: animSlot = hand + 0xDE; break;
        default: break;
        }
        if (a[7] == 0 || a[7] == 1) {
            if (a[7] == 1) groupSlot = animSlot;
            PutRaw32(animSlot, a[5]);
        } else if (a[7] == 5) {
            groupSlot = self + 0x10;
        } else if (a[7] == 6) {
            groupSlot = hand + 0xDE;
        }
        gRet = a[6];
        call.ecx = self;
        call.args = {groupSlot, animSlot, 0, 0, 0, 0, 0, 0}; call.argCount = 2; call.popBytes = 0;
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
        case kConst: obs["ret"] = eax; break;                                                              // valor completo de EAX
        case kConstAL1: obs["ret"] = eax & 0xFFu; break;                                                   // bool: solo AL
        case kSdd: case kClone: obs["ret"] = Tag(uintptr_t(eax)); break;                                   // this / el objeto nuevo como etiqueta de arena
        default: obs["ret"] = nullptr; break;                                                              // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN155A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN155A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn155a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn155a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn155a"); }
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
    auto Del = [&] { return F(0x61A5B0, &FkDelete<0x61A5B0>); };   // CTask::operator delete (con hook)
    auto EvDel = [&] { return F(0x4B5630, &FkDelete<0x4B5630>); }; // CEvent::operator delete (con hook desde el lote 153A)
    const auto& K = kBodyFakes;
    env.stubs[0] = {F(0x4D9C10, &FkRand)};
    env.stubs[1] = {H(0x61F4D0, Lote155ABody_CTaskSimpleGunControl(), K[1]), Del()};
    env.stubs[2] = {F(0x571A00, &FkCleanUp), F(0x4CEBC0, &FkSetDelCb), F(0x61A3A0, K[2])};
    env.stubs[6] = {H(0x609140, Lote155ABody_CEventDontJoinPlayerGroup(), K[6]), EvDel()};
    env.stubs[7] = {F(0x4B5620, &FkNew), F(0x6090E0, &FkCtor)};
    env.stubs[8] = {H(0x648430, Lote155ABody_CTaskSimpleCarSetPedSlowDraggedOut(), K[8]), Del()};
    env.stubs[9] = {H(0x64C580, Lote155ABody_CTaskComplexCarSlowBeDraggedOut(), K[9]), Del()};
    env.stubs[10] = {F(0x6E3B00, &FkGetGroup)};
    env.stubs[15] = {F(0x4AC480, K[15])};
    env.stubs[16] = {Rt(15, K[16]), EvDel()};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {3u, 4u, 5u, 11u, 12u, 13u, 14u}) dllReal[f] = "toda la funcion (sin colaboradores)";
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
