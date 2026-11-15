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

// Banco par a par Fn197A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 32 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall / RH_ScopedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion
// del EXE con objetos sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair):
// CEvent::operator new 0x4B5620 y operator delete 0x4B5630, constructores de CEvent 0x4ABFC0 y de CEventEditableResponse 0x4AC450, cuerpos de ~CEvent 0x4AC010 y de ~CEventEditableResponse 0x4AC480,
// CTask::operator new 0x61A5A0 y operator delete 0x61A5B0, cuerpo del destructor de CTaskComplexUseEffect 0x632270, constructores 0x48E750 (CTaskSimplePause), 0x61ABE0 (CTaskSimpleRunTimedAnim) y 0x61A900
// (CTaskSimpleRunAnim), rand() 0x821B1E (en la DLL CGeneral::GetRandomNumber; el 0x407180 del EXE corre real), GetTimeLeft 0x62EB80 y ShouldLoadSunbatheAnims 0x632190 (todos con hook); los cuerpos de destructor de las clases de eventos (0x5FF520, 0x5FF5E0, 0x5FF6A0, 0x5FF760) que
// llaman los destructores con borrado son otras funciones probadas del lote (`Rt`); el cuerpo 0x5FF460 de CEventHealthLow NO tiene hook en este lote (lo valida el lote 196A): corre como codigo real del EXE (y el
// de la DLL como funcion propia). Memoria global del juego tocada (restaurada tras cada lado): el byte del jugador en foco 0xB7CD74 y la tabla de jugadores 0xB7CD98.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto
// (punteros como etiqueta de arena; EAX completo en constantes; solo AL en bool; bits del float de ST0 en GetLocalSoundLevel) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en
// original_residue / candidate_residue (informativos).

namespace F197ALive {
using namespace F197AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F197AFix::Arena(WIN);
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
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos de cuerpo y de constructor
inline u32 gDtorWrite{};             // dword que el falso del cuerpo / constructor deja en [this]
inline u32 gNew{};                   // memoria que devuelve el falso de operator new (0 = sin memoria)
inline u32 gRand{}, gShould{}, gSubType{};   // lo que devuelven el falso del azar, el de ShouldLoadSunbatheAnims y el GetTaskType de la subtarea
inline std::array<u32, 2> gTL{};     // lo que devuelven la primera y la segunda llamada a GetTimeLeft
inline unsigned tlCalls{};
inline DWORD gBenchThread{};

// operator delete / liberacion (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// operator new (cdecl, un tamano): evento [direccion, tamano]; devuelve gNew.
template<u32 ADDR> inline u32 __cdecl FkNew(u32 size) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = size;
    return gNew;
}
// Cuerpo de destructor / constructor sin argumentos de pila (thiscall): evento [direccion del EXE, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor con un argumento de pila (thiscall, `ret 4`): evento [direccion, ECX (etiqueta), argumento (crudo)]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCtor1(void* self, void*, u32 a) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = a;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor con seis dwords de pila (thiscall, `ret 18h`): evento [direccion, ECX (etiqueta), a..f (crudos)]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCtor6(void* self, void*, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f) {
    Ev& ev = PushEv(ADDR, 7);
    ev.v[0] = Tag(self); ev.v[1] = a; ev.v[2] = b; ev.v[3] = c; ev.v[4] = d; ev.v[5] = e; ev.v[6] = f;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor con ocho dwords de pila (thiscall, `ret 20h`): evento [direccion, ECX (etiqueta), a..h (crudos)]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCtor8(void* self, void*, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, u32 g, u32 h) {
    Ev& ev = PushEv(ADDR, 9);
    ev.v[0] = Tag(self); ev.v[1] = a; ev.v[2] = b; ev.v[3] = c; ev.v[4] = d; ev.v[5] = e; ev.v[6] = f; ev.v[7] = g; ev.v[8] = h;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// rand() (0x821B1E en el EXE) / CGeneral::GetRandomNumber (DLL): evento [0x821B1E]; devuelve gRand (los llamadores solo usan sus 16 bits bajos). El EXE corre su 0x407180 REAL (sin hook: se apaga el de la DLL,
// que nadie llama) con este rand falso y la DLL hace la conversion en linea (CGeneral::GetRandomIntInRange) con este mismo falso: se compara la conversion de ambos lados.
inline u32 __cdecl FkRand() {
    PushEv(0x821B1E, 0);
    return gRand;
}
// CTaskTimer::GetTimeLeft (0x62EB80; thiscall, sin argumentos): evento [direccion, ECX (etiqueta)]; devuelve gTL[0] la primera vez y gTL[1] las siguientes.
inline u32 __fastcall FkTimeLeft(void* self, void*) {
    Ev& e = PushEv(0x62EB80, 1);
    e.v[0] = Tag(self);
    return gTL[tlCalls++ == 0 ? 0 : 1];
}
// ShouldLoadSunbatheAnims (0x632190; cdecl, sin argumentos): evento [direccion]; devuelve gShould (el llamador solo mira AL).
inline u32 __cdecl FkShould() {
    PushEv(0x632190, 0);
    return gShould;
}
// vtable propia de la subtarea: ranura 4 (GetTaskType) devuelve gSubType; las demas anotan una llamada inesperada [90 + k, ECX]
inline std::array<uintptr_t, 16> taskVt{};
template<unsigned K> inline u32 __fastcall FkVt(void* self, void*) { Ev& e = PushEv(90 + K, 1); e.v[0] = Tag(self); return 0; }
inline u32 __fastcall FkTaskType(void* self, void*) { Ev& e = PushEv(94, 1); e.v[0] = Tag(self); return gSubType; }
inline void InitTaskVt() {
    [&]<unsigned... I>(std::integer_sequence<unsigned, I...>) { ((taskVt[I] = uintptr_t(&FkVt<I>)), ...); }(std::make_integer_sequence<unsigned, 16>{});
    taskVt[4] = uintptr_t(&FkTaskType);
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

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    gRet = 0; gDtorWrite = 0; gNew = 0; gRand = 0; gShould = 0; gSubType = 0; gTL = {}; tlCalls = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kConst:
    case kConstF:
        call.floatRet = kKind[c.fn] == kConstF;
        break;
    case kCtor:
    case kBody:
        gRet = a[4]; gDtorWrite = a[5];
        break;
    case kClone:
        gNew = a[4] == 0 ? 0u : u32(arenaB.Obj(a[5]));
        gRet = a[6]; gDtorWrite = a[7];
        break;
    case kSdd:
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kShould: {
        const u32 ped0 = u32(arenaB.Obj(a[5])), ped1 = u32(arenaD.Obj(a[12]));
        const u32 veh0 = a[7] == 0 ? 0u : u32(arenaC.Obj(a[8])), veh1 = a[14] == 0 ? 0u : u32(arenaC.Obj(a[15]));
        arenaB.Put32(a[5] + 0x46C, a[6]); arenaB.Put32(a[5] + 0x58C, veh0);
        arenaD.Put32(a[12] + 0x46C, a[13]); arenaD.Put32(a[12] + 0x58C, veh1);
        for (u32 i = 0; i < 3; ++i) { arenaC.Put32(a[8] + 0x44 + 4 * i, a[9 + i]); arenaC.Put32(a[15] + 0x44 + 4 * i, a[16 + i]); }
        *reinterpret_cast<uint8*>(kFocusAddr) = uint8(a[4]);
        *reinterpret_cast<u32*>(kPlayersAddr) = ped0;
        *reinterpret_cast<u32*>(kPlayersAddr + kPlayerStride) = ped1;
        break;
    }
    case kCreateSub: {
        const u32 sub = a[6] == 0 ? 0u : u32(arenaB.Obj(a[7]));
        arenaA.Put32(o + 8, sub);
        if (sub) arenaB.Put32(a[7], u32(uintptr_t(taskVt.data())));
        gSubType = a[8];
        arenaA.Put32(o + 0x20, a[9]);
        arenaA.Put32(o + 0x24, u32(arenaC.Obj(a[11]))); arenaA.Put32(o + 0x28, u32(arenaC.Obj(a[13])));
        arenaC.Put32(a[11] + 0x10, a[10]); arenaC.Put32(a[13] + 0x10, a[12]);
        gNew = a[14] == 0 ? 0u : u32(arenaD.Obj(a[15]));
        gRet = a[16]; gDtorWrite = a[17]; gRand = a[18]; gTL = {a[19], a[20]}; gShould = a[21];
        call.args[0] = a[4]; call.args[1] = a[5]; call.argCount = 2;
        break;
    }
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
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        const u32 floatBits = gFloatRet;
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = json::array();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kCtor: case kClone: case kSdd: case kCreateSub: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case kConst: obs["ret"] = eax; break;                                                          // valor completo de EAX
        case kConstF: obs["ret"] = floatBits; break;                                                   // bits del float de ST0
        case kShould: obs["ret"] = eax & 0xFFu; break;                                                 // bool: solo AL
        default: obs["ret"] = nullptr; break;                                                          // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN197A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN197A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn197a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn197a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn197a"); }
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
    // Colaborador con hook V (la DLL lo llama de forma no virtual: el cuerpo es la funcion de la ranura)
    auto FV = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollabV(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
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
    InitTaskVt();
    // Colaborador SIN hook RH cuya llamada, en la DLL, va a una funcion propia (direccion dada) y no a la del EXE: el falso va en la direccion del EXE y en esa funcion (rand: CGeneral::GetRandomNumber)
    auto D = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        Pair p{exe, Body(dllAddress), false};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    env.stubs[0] = {F(0x4B5620, &FkNew<0x4B5620>), F(0x4AC450, &FkBody<0x4AC450>)};
    env.stubs[1] = {F(0x4AC480, &FkBody<0x4AC480>), F(0x4B5630, &FkDelete<0x4B5630>)};
    env.reals[1] = {Pair{0x5FF460, 0, false}};   // cuerpo de ~CEventHealthLow: SIN hook en este lote (lo valida el lote 196A); el EXE corre su codigo y la DLL su funcion propia (que llama al falso de 0x4AC480)
    env.stubs[2] = {F(0x4AC450, &FkBody<0x4AC450>)};
    env.stubs[3] = {F(0x4AC480, &FkBody<0x4AC480>)};
    env.stubs[4] = {F(0x4B5620, &FkNew<0x4B5620>), F(0x4AC450, &FkBody<0x4AC450>)};
    env.stubs[5] = {Rt(3, &FkBody<0x5FF520>), F(0x4B5630, &FkDelete<0x4B5630>)};
    env.stubs[6] = {F(0x4AC450, &FkBody<0x4AC450>)};
    env.stubs[7] = {F(0x4AC480, &FkBody<0x4AC480>)};
    env.stubs[8] = {F(0x4B5620, &FkNew<0x4B5620>), F(0x4AC450, &FkBody<0x4AC450>)};
    env.stubs[9] = {Rt(7, &FkBody<0x5FF5E0>), F(0x4B5630, &FkDelete<0x4B5630>)};
    env.stubs[10] = {F(0x4AC450, &FkBody<0x4AC450>)};
    env.stubs[11] = {F(0x4AC480, &FkBody<0x4AC480>)};
    env.stubs[12] = {F(0x4B5620, &FkNew<0x4B5620>), F(0x4AC450, &FkBody<0x4AC450>)};
    env.stubs[13] = {Rt(11, &FkBody<0x5FF6A0>), F(0x4B5630, &FkDelete<0x4B5630>)};
    env.stubs[14] = {F(0x4ABFC0, &FkBody<0x4ABFC0>)};
    env.stubs[15] = {F(0x4AC010, &FkBody<0x4AC010>)};
    env.stubs[18] = {F(0x4B5620, &FkNew<0x4B5620>), F(0x4ABFC0, &FkBody<0x4ABFC0>)};
    env.stubs[20] = {Rt(15, &FkBody<0x5FF760>), F(0x4B5630, &FkDelete<0x4B5630>)};
    env.stubs[21] = {F(0x4ABFC0, &FkBody<0x4ABFC0>)};
    env.stubs[24] = {F(0x4AC010, &FkBody<0x4AC010>)};
    env.stubs[30] = {F(0x61A5A0, &FkNew<0x61A5A0>), D(0x821B1E, std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber), &FkRand), F(0x62EB80, &FkTimeLeft), Rt(29, &FkShould), F(0x48E750, &FkCtor1<0x48E750>), F(0x61ABE0, &FkCtor8<0x61ABE0>), F(0x61A900, &FkCtor6<0x61A900>)};
    {   // 0x407180 (GetRandomNumberInRange<int>) tiene hook RH en la DLL que ninguna funcion probada llama: se apaga (ruta) y el EXE corre su codigo REAL (la DLL convierte en linea con GetRandomIntInRange)
        const Pair real = MakeCollab(env, 0x407180, hooksFound);
        if (!real.hooked || !real.dll) ready = false;
        env.reals[30] = {real};
    }
    env.stubs[31] = {F(0x632270, &FkBody<0x632270>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {16u, 17u, 19u, 22u, 23u, 25u, 26u, 27u, 28u, 29u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[1] = "toda la funcion salvo 0x4AC480 y 0x4B5630 (falsos); el cuerpo 0x5FF460 (sin hook en este lote) corre real: codigo del EXE / funcion propia de la DLL";
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
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada: el jugador en foco y la tabla de jugadores (kShould); se restauran tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kFocusAddr, 1); regions.emplace_back(kPlayersAddr, kPlayerStride * kPlayerCount);
    regionsOf[29] = {regions.size() - 2, regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}
