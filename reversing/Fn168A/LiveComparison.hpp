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
#include "General.h"

// Banco par a par Fn168A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 23 funciones del EXE (constantes virtuales, Clone, destructores con borrado y cuerpos de destructor de CEventLeanOnVehicle, CEventPassObject y CEventNewGangMember; constructor desde desplazamiento de
// CEntitySeekPosCalculatorXYOffset y tercera copia del cuerpo del destructor de CEntitySeekPosCalculator; constructor de CEventScanner; CDecisionMakerTypes::CopyDecisionMaker; ver Fixtures.hpp), cada una con 16384
// pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40).
// Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): CEvent::operator new 0x4B5620, CEvent::operator delete 0x4B5630, los constructores de
// CEventLeanOnVehicle 0x65DAF0, CEventPassObject 0x65DC70 y CEventNewGangMember 0x608F70, el cuerpo del destructor de CEventPassObject 0x65DCF0, ~CEvent 0x4AC010, CEntity::CleanUpOldReference 0x571A00,
// CAttractorScanner::Clear 0x5FFF90 y AddDecisionMaker 0x607050 (todos con hook), y rand() (0x821B1E en el EXE; en la DLL, CGeneral::GetRandomNumber, que lo llama). Los cuerpos de los destructores de
// CEventLeanOnVehicle y CEventNewGangMember son otras funciones probadas del lote (Rt). _ftol (0x821B40) corre real.
// Se observan: los bytes cambiados de las tres arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto
// (this o la memoria como etiqueta de arena; EAX completo en constantes y en CopyDecisionMaker; solo AL en bool) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue /
// candidate_residue (informativos).

namespace F168ALive {
using namespace F168AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F168AFix::Arena(WIN);
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
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this]
inline u32 gMem{};                   // memoria que devuelve el falso de operator new
inline u32 gRand{};                  // lo que devuelve el falso de rand()
inline DWORD gBenchThread{};
inline constexpr uintptr_t kTimerAddress = 0xB7CB84;   // CTimer::m_snTimeInMilliseconds

// operator delete / liberacion (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// CEvent::operator new (0x4B5620; cdecl, un argumento): evento [direccion, tamano]; devuelve gMem (memoria de B o nulo).
template<u32 ADDR> inline u32 __cdecl FkNew(u32 size) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = size;
    return gMem;
}
// Cuerpo del destructor y ~CEvent (thiscall, sin argumentos de pila): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CEntity::CleanUpOldReference (0x571A00) (thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]. No escribe nada.
template<u32 ADDR> inline void __fastcall FkRef(void* self, void*, void* ref) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = Tag(ref);
}
// Constructor con un dword de pila (CEventNewGangMember 0x608F70; `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCtor1(void* self, void*, u32 a1) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(a1));
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor con dos dwords de pila (CEventLeanOnVehicle 0x65DAF0 y CEventPassObject 0x65DC70; `ret 8`): evento [direccion, ECX (etiqueta), primer argumento (etiqueta), segundo argumento (dword completo)];
// deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCtor2(void* self, void*, u32 a1, u32 a2) {
    Ev& e = PushEv(ADDR, 3);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(a1)); e.v[2] = a2;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CAttractorScanner::Clear (0x5FFF90; thiscall, sin argumentos de pila): evento [direccion, ECX (etiqueta), los seis dwords de [ECX + 0 .. 0x14] al entrar]. No escribe nada.
inline u32 __fastcall FkClear(void* self, void*) {
    Ev& e = PushEv(0x5FFF90, 7);
    e.v[0] = Tag(self);
    for (u32 i = 0; i < 6; ++i) e.v[1 + i] = BitsOf(reinterpret_cast<const u8*>(self) + 4 * i);
    return 0;
}
// rand() (0x821B1E en el EXE) / CGeneral::GetRandomNumber (DLL): evento [0x821B1E]; devuelve gRand (el llamador solo usa sus 16 bits bajos).
inline u32 __cdecl FkRand() {
    PushEv(0x821B1E, 0);
    return gRand;
}
// AddDecisionMaker (0x607050; thiscall, tres dwords de pila, `ret 0Ch`): evento [direccion, ECX (etiqueta), origen (etiqueta), tipo (dword), byte bajo del tercer argumento]; devuelve gRet.
inline u32 __fastcall FkAddDm(void* self, void*, u32 source, u32 type, u32 mission) {
    Ev& e = PushEv(0x607050, 4);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(source)); e.v[2] = type; e.v[3] = mission & 0xFFu;
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

// EAX que devuelve el falso de un constructor segun el selector (0 la memoria, 1 la memoria + 4, 2 cero, 3 un valor crudo)
inline u32 CtorEax(u32 selector, u32 memory, u32 raw) {
    switch (selector) {
    case 0: return memory;
    case 1: return memory + 4;
    case 2: return 0;
    default: return raw;
    }
}

// Prepara arenas del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    gRet = 0; gDtorWrite = 0; gMem = 0; gRand = 0;
    const u32 thisA = u32(arenaA.Obj(a[0]));
    switch (kKind[c.fn]) {
    case kConst:
    case kVtOnly:
        arenaA.Put32(a[0], a[3]);
        call.ecx = thisA;
        break;
    case kConstAL:   // thiscall con un dword de pila (`ret 4`: lo limpia el callee)
        arenaA.Put32(a[0], a[3]);
        call.ecx = thisA;
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 0;
        break;
    case kSdd:       // thiscall con banderas en la pila (`ret 4`)
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = thisA;
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        break;
    case kEvBody:
        arenaA.Put32(a[0], a[3]); arenaA.Put32(a[0] + 0xC, a[6]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = thisA;
        break;
    case kClone:
        arenaA.Put32(a[0] + 0xC, a[8]); arenaA.Put32(a[0] + 0x10, a[9]);
        gMem = a[3] == 0 ? 0u : u32(arenaB.Obj(a[4]));
        gRet = CtorEax(a[7], gMem, a[6]); gDtorWrite = a[5];
        call.ecx = thisA;
        break;
    case kXyCtor: {  // thiscall con un puntero de pila (`ret 4`)
        arenaA.Put32(a[0], a[3]);
        u32 source;
        if (a[4] == 0) {
            arenaB.Put32(a[5], a[6]); arenaB.Put32(a[5] + 4, a[7]); arenaB.Put32(a[5] + 8, a[8]);
            source = u32(arenaB.Obj(a[5]));
        } else {   // origen en A, relativo a `this` (puede solaparse con el objeto o caer en las guardas); se escribe despues del dword previo de `this`
            const i32 srcOffset = i32(a[0]) + i32(a[5]);
            uint8* const dst = arenaA.m.data() + kGuard + srcOffset;
            memcpy(dst, &a[6], 4); memcpy(dst + 4, &a[7], 4); memcpy(dst + 8, &a[8], 4);
            source = u32(uintptr_t(dst));
        }
        call.ecx = thisA;
        call.args[0] = source; call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kScanner:   // thiscall sin argumentos; el tiempo del juego es el dword de CTimer
        gRand = a[4];
        *reinterpret_cast<u32*>(kTimerAddress) = a[3];
        call.ecx = thisA;
        break;
    case kCopyDm:    // thiscall con tres dwords de pila (`ret 0Ch`)
        gRet = a[6];
        call.ecx = thisA;
        call.args[0] = a[3]; call.args[1] = a[4]; call.args[2] = a[5]; call.argCount = 3; call.popBytes = 0;
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
        case kSdd: case kClone: case kXyCtor: case kScanner: obs["ret"] = Tag(uintptr_t(eax)); break;   // this o la memoria como etiqueta de arena
        case kConst: case kCopyDm: obs["ret"] = eax; break;                                           // valor completo de EAX
        case kConstAL: obs["ret"] = eax & 0xFFu; break;                                               // bool: solo AL
        default: obs["ret"] = nullptr; break;                                                         // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN168A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN168A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn168a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn168a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn168a"); }
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
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH cuya llamada, en la DLL, va a una funcion propia (direccion dada) y no a la del EXE: el falso va en la direccion del EXE y en esa funcion (rand: CGeneral::GetRandomNumber)
    auto D = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        Pair p{exe, Body(dllAddress), false};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    const auto& K = kBodyFakes;
    env.stubs[2]  = {F(0x4B5620, &FkNew<0x4B5620>), F(0x65DAF0, &FkCtor2<0x65DAF0>)};
    env.stubs[4]  = {Rt(5, K[4]), F(0x4B5630, &FkDelete<0x4B5630>)};
    env.stubs[5]  = {F(0x571A00, &FkRef<0x571A00>), F(0x4AC010, K[5])};
    env.stubs[9]  = {F(0x4B5620, &FkNew<0x4B5620>), F(0x65DC70, &FkCtor2<0x65DC70>)};
    env.stubs[11] = {F(0x65DCF0, K[11]), F(0x4B5630, &FkDelete<0x4B5630>)};
    env.stubs[15] = {F(0x4B5620, &FkNew<0x4B5620>), F(0x608F70, &FkCtor1<0x608F70>)};
    env.stubs[17] = {Rt(18, K[17]), F(0x4B5630, &FkDelete<0x4B5630>)};
    env.stubs[18] = {F(0x571A00, &FkRef<0x571A00>), F(0x4AC010, K[18])};
    env.stubs[21] = {F(0x5FFF90, &FkClear), D(0x821B1E, std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber), &FkRand)};
    env.reals[21] = {Pair{0x821B40, 0, false}};   // _ftol del CRT del EXE: corre real
    env.stubs[22] = {F(0x607050, &FkAddDm)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {0u, 1u, 3u, 6u, 7u, 8u, 10u, 12u, 13u, 14u, 16u, 19u, 20u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[21] = "toda la funcion salvo CAttractorScanner::Clear y rand (CGeneral::GetRandomNumber); la conversion a entero la hace el propio codigo de cada lado";
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
    // Memoria global del juego que el banco toca: solo el reloj del juego (CTimer, 0xB7CB84), que el constructor de CEventScanner lee; se guarda al empezar y se restaura tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kTimerAddress, 4);
    regionsOf[21].push_back(0);
    Run(env, ready, registered, bodies, collaborators);
}
}
