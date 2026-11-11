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

// Banco par a par Fn160A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 23 funciones del EXE (destructores con borrado de siete tareas y eventos, constantes virtuales de tres tareas y dos eventos, Clone de tres tareas y dos eventos, MakeAbortable y ControlSubTask de tareas complejas,
// cuerpos de destructor de dos eventos, CPathNode::GetNodeCoors y la comparacion con la cadena vacia; ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall /
// RH_ScopedInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la
// direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): cuerpos de destructor (con hook propio, `H`: el hook se apaga y el falso va en la envoltura de la DLL; con hook y funcion propia, `F`: el cuerpo del
// hook; sin hook, `U`: falso en la direccion del EXE y en la envoltura de la DLL), CTask::operator delete 0x61A5B0, CEvent::operator delete 0x4B5630, CTask::operator new 0x61A5A0, CEvent::operator new 0x4B5620 y
// los constructores de CTaskSimpleSwim (0x688930), CTaskComplexGoToPointAnyMeans (0x66B790), CTaskSimpleClimb (0x67A110) y CEventPedCollisionWithPed (0x4AC990), todos con hook. MakeAbortable no tiene colaboradores
// con hook: la subtarea es un objeto de la arena cuya vtable (del banco) lleva en la ranura 6 una funcion falsa que anota ECX y los tres argumentos.
// Se observan: los bytes cambiados de las tres arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos; de los constructores solo
// la palabra o el byte que lee el constructor real), el valor devuelto (this, el objeto nuevo o la subtarea como etiqueta de arena; EAX completo en constantes, MakeAbortable y la cadena vacia) y el estado x87 (siempre
// vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Funciones propias de la DLL (envolturas) que exportan los .cpp de cada clase para que el banco las sustituya.
uintptr_t Lote160ABody_CTaskSimplePlayerOnFoot();          // 0x6857D0 (con hook propio, Destructor)
uintptr_t Lote160ABody_CTaskSimpleSwim();                  // 0x688A00 (sin hook)
uintptr_t Lote160ABody_CTaskComplexGoToPointAnyMeans();    // 0x66B830 (con hook propio, Destructor)
uintptr_t Lote160ABody_CTaskComplexGangLeader();           // 0x65DF30 (con hook propio, Destructor)

namespace F160ALive {
using namespace F160AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F160AFix::Arena(WIN);
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
inline u32 gRet{};                   // EAX que devuelven los falsos (no es this)
inline u32 gDtorWrite{};             // dword que el falso del cuerpo o del constructor deja en [this]
inline u32 gNew{};                   // lo que devuelven los falsos de operator new
inline DWORD gBenchThread{};

// operator delete / liberacion (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// operator new (cdecl, un tamano): evento [direccion del EXE, tamano]; devuelve gNew (nulo = sin memoria).
template<u32 ADDR> inline void* __cdecl FkNew(u32 size) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = size;
    return reinterpret_cast<void*>(uintptr_t(gNew));
}
// Cuerpo del destructor (thiscall, sin argumentos de pila): evento [direccion del EXE, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor de CTaskSimpleSwim (0x688930; thiscall, posicion y ped en la pila, `ret 8`): evento [direccion, ECX, posicion (etiqueta), ped (etiqueta)]; deja gDtorWrite en [ECX]; devuelve this (como el real).
inline void* __fastcall FkCtorSwim(void* self, void*, u32 pos, u32 ped) {
    Ev& e = PushEv(0x688930, 3);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(pos)); e.v[2] = Tag(uintptr_t(ped));
    memcpy(self, &gDtorWrite, 4);
    return self;
}
// Constructor 0x66B790 de CTaskComplexGoToPointAnyMeans (thiscall, estado, posicion, vehiculo, radio y modelo en la pila, `ret 14h`): evento [direccion, ECX, estado, posicion (etiqueta), vehiculo (etiqueta), radio, modelo].
inline void* __fastcall FkCtorAny(void* self, void*, u32 state, u32 pos, u32 vehicle, u32 radius, u32 model) {
    Ev& e = PushEv(0x66B790, 6);
    e.v[0] = Tag(self); e.v[1] = state; e.v[2] = Tag(uintptr_t(pos)); e.v[3] = Tag(uintptr_t(vehicle)); e.v[4] = radius; e.v[5] = model;
    memcpy(self, &gDtorWrite, 4);
    return self;
}
// Constructor 0x67A110 de CTaskSimpleClimb (thiscall, entidad, punto, rumbo y tres bytes en la pila, `ret 18h`): evento [direccion, ECX, entidad (etiqueta), punto (etiqueta), rumbo, superficie, altura, forzar]; el
// constructor real solo lee el byte de los tres ultimos argumentos (mov al,byte [esp+..]).
inline void* __fastcall FkCtorClimb(void* self, void*, u32 entity, u32 point, u32 heading, u32 surface, u32 height, u32 force) {
    Ev& e = PushEv(0x67A110, 7);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(entity)); e.v[2] = Tag(uintptr_t(point)); e.v[3] = heading; e.v[4] = surface & 0xFF; e.v[5] = height & 0xFF; e.v[6] = force & 0xFF;
    memcpy(self, &gDtorWrite, 4);
    return self;
}
// Constructor 0x4AC990 de CEventPedCollisionWithPed (thiscall, siete argumentos en la pila, `ret 1Ch`): evento [direccion, ECX, pieza (palabra), intensidad, victima (etiqueta), velocidad (etiqueta), posicion (etiqueta),
// estado (palabra), estado de la victima (palabra)]; el constructor real solo lee las palabras de 16 bits de los tres enteros cortos (mov ax,word [esp+..]); deja gDtorWrite en [ECX]; devuelve gRet (los Clone lo ignoran).
inline u32 __fastcall FkCtorEvent(void* self, void*, u32 piece, u32 damage, u32 victim, u32 velocity, u32 position, u32 state, u32 victimState) {
    Ev& e = PushEv(0x4AC990, 8);
    e.v[0] = Tag(self); e.v[1] = piece & 0xFFFF; e.v[2] = damage; e.v[3] = Tag(uintptr_t(victim)); e.v[4] = Tag(uintptr_t(velocity)); e.v[5] = Tag(uintptr_t(position)); e.v[6] = state & 0xFFFF; e.v[7] = victimState & 0xFFFF;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Ranura 6 (MakeAbortable) de la vtable falsa de la subtarea (thiscall, ped, prioridad y evento en la pila, `ret 0Ch`): evento [0xA001, ECX (etiqueta), ped (etiqueta), prioridad, evento (etiqueta)]; devuelve gRet.
inline u32 __fastcall FkSubAbort(void* self, void*, u32 ped, u32 priority, u32 event) {
    Ev& e = PushEv(0xA001, 4);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(ped)); e.v[2] = priority; e.v[3] = Tag(uintptr_t(event));
    return gRet;
}
// Cualquier otra ranura de la vtable falsa: evento [0xA0FF]; si el codigo llama a otra ranura, el modelo no coincide.
inline u32 __fastcall FkSubWrong(void*, void*) { PushEv(0xA0FF, 0); return 0; }
inline std::array<u32, 16> gFakeVt{};
inline void BuildFakeVt() {
    for (auto& s : gFakeVt) s = u32(reinterpret_cast<uintptr_t>(&FkSubWrong));
    gFakeVt[6] = u32(reinterpret_cast<uintptr_t>(&FkSubAbort));
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
    gRet = 0; gDtorWrite = 0; gNew = 0;
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
    case kEvBody:
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = thisA;
        break;
    case kRetField: {   // thiscall con el ped en la pila (`ret 4`)
        const u32 val = a[3] == 0 ? a[4] : (a[3] == 1 ? u32(arenaB.Obj(a[4])) : (a[3] == 2 ? u32(arenaC.Obj(a[4])) : thisA));
        arenaA.Put32(a[0], a[6]);
        arenaA.Put32(a[0] + 8, val);
        call.ecx = thisA;
        call.args[0] = a[5]; call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kAbort: {   // thiscall con ped, prioridad y evento en la pila (`ret 0Ch` del destino de la ranura 6 de la subtarea)
        const u32 sub = a[3] == 0 ? u32(arenaB.Obj(a[4])) : (a[3] == 1 ? u32(arenaC.Obj(a[4])) : thisA);
        const u32 vt = u32(reinterpret_cast<uintptr_t>(gFakeVt.data()));
        arenaA.Put32(a[0], a[9]);
        arenaA.Put32(a[0] + 8, sub);
        if (a[3] == 0) arenaB.Put32(a[4], vt);
        else if (a[3] == 1) arenaC.Put32(a[4], vt);
        else arenaA.Put32(a[0], vt);
        gRet = a[5];
        call.ecx = thisA;
        call.args[0] = a[6]; call.args[1] = a[7]; call.args[2] = a[8]; call.argCount = 3; call.popBytes = 0;
        break;
    }
    case kClone: {
        for (u32 i = 0; i < kCloneWords[c.fn]; ++i) arenaA.Put32(a[0] + 8 + 4 * i, a[16 + i]);
        arenaA.Put32(a[0] + 8 + 4 * kClonePtr[c.fn], a[3] == 0 ? a[4] : u32(arenaC.Obj(a[4])));
        gNew = a[5] ? u32(arenaB.Obj(a[6])) : 0u;
        gDtorWrite = a[7]; gRet = a[8];
        call.ecx = thisA;
        break;
    }
    case kNode: {   // thiscall con el destino en la pila (`ret 4`)
        arenaA.Put32(a[0] + 8, a[3]);
        arenaA.Put32(a[0] + 0xC, a[4]);
        const u32 out = a[5] == 0 ? u32(arenaB.Obj(a[6])) : (a[5] == 1 ? u32(arenaC.Obj(a[6])) : (a[5] == 2 ? u32(thisA + 8 + i32(a[6])) : u32(arenaA.Obj(a[6]))));
        call.ecx = thisA;
        call.args[0] = out; call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kEmpty:
        arenaA.Put32(a[0], a[3]);
        call.ecx = thisA;
        break;
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
        case kSdd: case kClone: case kRetField: obs["ret"] = Tag(uintptr_t(eax)); break;   // this, el objeto nuevo o la subtarea como etiqueta de arena
        case kConst: case kAbort: case kEmpty: obs["ret"] = eax; break;                      // valor completo de EAX
        default: obs["ret"] = nullptr; break;                                                // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN160A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN160A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn160a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn160a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn160a"); }
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
    BuildFakeVt();
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
    auto DelTask = [&] { return F(0x61A5B0, &FkDelete<0x61A5B0>); };    // CTask::operator delete (con hook)
    auto DelEvent = [&] { return F(0x4B5630, &FkDelete<0x4B5630>); };   // CEvent::operator delete (con hook)
    auto NewTask = [&] { return F(0x61A5A0, &FkNew<0x61A5A0>); };       // CTask::operator new (con hook)
    auto NewEvent = [&] { return F(0x4B5620, &FkNew<0x4B5620>); };      // CEvent::operator new (con hook)
    const auto& K = kBodyFakes;
    env.stubs[0] = {NewTask(), F(0x688930, &FkCtorSwim)};
    env.stubs[1] = {H(0x6857D0, Lote160ABody_CTaskSimplePlayerOnFoot(), K[1]), DelTask()};
    env.stubs[2] = {U(0x688A00, Lote160ABody_CTaskSimpleSwim(), K[2]), DelTask()};
    env.stubs[6] = {NewTask(), F(0x66B790, &FkCtorAny)};
    env.stubs[7] = {H(0x66B830, Lote160ABody_CTaskComplexGoToPointAnyMeans(), K[7]), DelTask()};
    env.stubs[10] = {NewTask(), F(0x67A110, &FkCtorClimb)};
    env.stubs[11] = {F(0x67A1D0, K[11]), DelTask()};
    env.stubs[12] = {H(0x65DF30, Lote160ABody_CTaskComplexGangLeader(), K[12]), DelTask()};
    env.stubs[15] = {F(0x4ACA70, K[15])};
    env.stubs[17] = {NewEvent(), F(0x4AC990, &FkCtorEvent)};
    env.stubs[18] = {F(0x5FED80, K[18]), DelEvent()};
    env.stubs[19] = {F(0x4ACA70, K[19])};
    env.stubs[21] = {NewEvent(), F(0x4AC990, &FkCtorEvent)};
    env.stubs[22] = {F(0x5FEE80, K[22]), DelEvent()};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {3u, 5u, 8u, 9u, 13u, 14u, 16u, 20u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[4] = "toda la funcion; la subtarea es un objeto de la arena con una vtable falsa del banco (ranura 6)";
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
