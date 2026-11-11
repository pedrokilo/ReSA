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

// Banco par a par Fn165A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 26 funciones del EXE (StopSound y cuerpo del destructor de CAETwinLoopSoundEntity; cuerpo del destructor, seis virtuales y destructor con borrado de CEventSoundQuiet; destructor con borrado de CPedIntelligence;
// GetNode, cuerpo del destructor y destructor con borrado de CAnimBlendStaticAssociation; RpAnimBlendNodeUpdateKeyFrames con EAX; destructores con borrado de seis clases de tareas; cuatro constructores y un
// destructor de las clases de mobiliario; ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall / RH_ScopedGlobalInstall) y V (RH_ScopedVMTInstall).
// El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL
// (MakeCollab / Pair): cuerpos de destructor (con hook, `F`; sin hook y funcion propia de la DLL, `U`; otra funcion probada del lote, `Rt`), CTask::operator delete 0x61A5B0, CEvent::operator delete 0x4B5630,
// CPedIntelligence::operator delete 0x6074E0 y operator delete del CRT 0x8214BD (este ultimo sin hook: la DLL lo llama por la direccion del EXE, `E`), CEntity::CleanUpOldReference 0x571A00,
// CAESound::StopSound 0x4EF1C0, StopSoundAndForget 0x4EF850 y ~CAESound 0x4EF660, CMemoryMgr::Free 0x72F430 y CAnimBlendNode::FindKeyFrame 0x4D0240.
// Se observan: los bytes cambiados de las tres arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto
// (this, EAX completo, solo AL en bool o el ST0 completo en GetLocalSoundLevel) y el estado x87 (vacio salvo en GetLocalSoundLevel). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Funcion propia de la DLL (sin hook) que exporta el .cpp de la clase para que el banco la sustituya.
uintptr_t Lote165ABody_CTaskComplexDragPedFromCar();            // 0x6404D0 (sin hook)

namespace F165ALive {
using namespace F165AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F165AFix::Arena(WIN);
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
inline u8* gThis{};                  // `this` del caso en la arena A (falsos de CAETwinLoopSoundEntity, que observan el objeto)
inline u32 gMutate{}, gMutVal{};     // StopSound falso: tras la primera llamada deja gMutVal en this+0A4h si gMutate
inline unsigned gStopCalls{};
inline DWORD gBenchThread{};
inline u32 Dword(const u8* p) { u32 v; memcpy(&v, p, 4); return v; }
inline bool InTags(const void* p) { const auto v = uintptr_t(p); for (const auto& t : tagTable) if (v >= t.base && v < t.base + t.size) return true; return false; }

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
// CMemoryMgr::Free (0x72F430; cdecl, un puntero cualquiera): evento [direccion, argumento (etiqueta), dword del argumento si cae en una arena, si no 0]. No libera nada.
inline void __cdecl FkFree(void* p) {
    Ev& e = PushEv(0x72F430, 2);
    e.v[0] = Tag(p);
    e.v[1] = InTags(p) ? BitsOf(p) : 0u;
}
// CEntity::CleanUpOldReference (0x571A00; thiscall, un puntero en la pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]. No toca la referencia.
inline void __fastcall FkCleanUp(void* self, void*, void* ref) { Ev& e = PushEv(0x571A00, 2); e.v[0] = Tag(self); e.v[1] = Tag(ref); }
// CAESound::StopSound (0x4EF1C0; thiscall, sin argumentos): evento [direccion, ECX (etiqueta)]; tras la primera llamada deja gMutVal en this+0A4h si gMutate (el EXE lee la segunda voz DESPUES de la primera llamada).
inline void __fastcall FkStop(void* self, void*) {
    Ev& e = PushEv(0x4EF1C0, 1);
    e.v[0] = Tag(self);
    if (++gStopCalls == 1 && gMutate) memcpy(gThis + 0xA4, &gMutVal, 4);
}
// CAESound::StopSoundAndForget (0x4EF850; thiscall, sin argumentos): evento [direccion, ECX (etiqueta), +0A0, +0A4, +88 (dword) y [this] del objeto de la arena A]. No toca nada.
inline void __fastcall FkForget(void* self, void*) {
    Ev& e = PushEv(0x4EF850, 5);
    e.v[0] = Tag(self);
    e.v[1] = Dword(gThis + 0xA0);
    e.v[2] = Dword(gThis + 0xA4);
    e.v[3] = Dword(gThis + 0x88);
    e.v[4] = Dword(gThis);
}
// CAESound::~CAESound (0x4EF660; thiscall, sin argumentos; el EXE la llama sobre this+8): evento [direccion, ECX (etiqueta), [ECX], [this], +0A0, +0A4, +88 (dword)]. Devuelve ECX.
inline void* __fastcall FkSoundDtor(void* self, void*) {
    Ev& e = PushEv(0x4EF660, 6);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    e.v[2] = Dword(gThis);
    e.v[3] = Dword(gThis + 0xA0);
    e.v[4] = Dword(gThis + 0xA4);
    e.v[5] = Dword(gThis + 0x88);
    return self;
}
// CAnimBlendNode::FindKeyFrame (0x4D0240; thiscall, un float en la pila, `ret 4`): evento [direccion, ECX (etiqueta del nodo), bits del float]. Devuelve falso.
inline bool __fastcall FkFindKF(void* self, void*, u32 timeBits) { Ev& e = PushEv(0x4D0240, 2); e.v[0] = Tag(self); e.v[1] = timeBits; return false; }
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
    u32 argCount{}, popBytes{}, ecx{}, eax{};
};

inline void PutRaw32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }

// Prepara arenas del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx; call.eax = c.eax;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    gRet = 0; gDtorWrite = 0; gMutate = 0; gMutVal = 0; gStopCalls = 0;
    const u32 self = u32(arenaA.Obj(a[0]));
    gThis = reinterpret_cast<u8*>(arenaA.Obj(a[0]));
    // Voz de la voz doble segun el modo: 0 nula, 1 en la arena B, 2 cruda, 3 igual que la voz anterior
    const auto voice = [&](u32 mode, u32 offB, u32 raw, u32 other) -> u32 { return mode == 0 ? 0u : (mode == 1 ? u32(arenaB.Obj(offB)) : (mode == 2 ? raw : other)); };
    switch (kKind[c.fn]) {
    case kConst:
    case kConstAL:
        arenaA.Put32(a[0], a[3]);
        call.ecx = self;
        break;
    case kCtor:   // sin datos previos: el relleno de la arena es el estado anterior del objeto
    case kNop:
        call.ecx = self;
        break;
    case kLoad:
        arenaA.Put32(a[0] + 0x14, a[3]);
        arenaA.Put32(a[0] + 0x18, a[4]);
        call.ecx = self;
        break;
    case kFloat:
        arenaA.Put32(a[0] + 0x18, a[3]);
        call.ecx = self;
        break;
    case kSdd:   // thiscall con banderas en la pila (`ret 4`: lo limpia el callee)
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        break;
    case kBodyRef: {
        const u32 entity = a[6] == 0 ? 0u : (a[6] == 1 ? u32(arenaB.Obj(a[7])) : a[8]);
        arenaA.Put32(a[0], a[3]);
        arenaA.Put32(a[0] + 0x14, entity);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        break;
    }
    case kTwinStop: {
        const u32 v0 = voice(a[3], a[7], a[8], 0), v1 = voice(a[4], a[9], a[10], v0);
        arenaA.Put32(a[0] + 0xA0, v0);
        arenaA.Put32(a[0] + 0xA4, v1);
        arenaA.Put32(a[0] + 0x88, a[11]);
        gMutate = a[6] == 1; gMutVal = a[5];
        call.ecx = self;
        break;
    }
    case kTwinDtor: {
        const u32 v0 = voice(a[4], a[7], a[8], 0), v1 = voice(a[5], a[9], a[10], v0);
        arenaA.Put32(a[0], a[6]);
        arenaA.Put32(a[0] + 0x88, a[3]);
        arenaA.Put32(a[0] + 0xA0, v0);
        arenaA.Put32(a[0] + 0xA4, v1);
        call.ecx = self;
        break;
    }
    case kGetNode:   // thiscall con el indice en la pila (`ret 4`)
        arenaA.Put32(a[0] + 0x10, a[3]);
        call.ecx = self;
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 0;
        break;
    case kStaticBody:
    case kStaticSdd: {
        const u32 p = a[6] == 0 ? 0u : (a[6] == 1 ? u32(arenaB.Obj(a[7])) : (a[6] == 2 ? a[8] : self));
        arenaA.Put32(a[0], a[3]);
        arenaA.Put32(a[0] + 0xC, p);
        call.ecx = self;
        if (kKind[c.fn] == kStaticSdd) { call.args[0] = a[9]; call.argCount = 1; call.popBytes = 0; }
        break;
    }
    case kUpdateKF: {   // EAX = contexto; pila: frames, n (cdecl: limpia el llamador)
        const u32 narr = a[4];
        const u32 pedpos = a[5] ? a[16] : 0u;
        arenaA.Put32(a[0], Rand(a[12], 5));
        for (u32 k = 0; k < narr; ++k) arenaA.Put32(a[0] + 4 + 4 * k, u32(arenaC.Obj(a[11] + 0x130 * k)));
        arenaA.Put32(a[0] + 4 + 4 * narr, 0);
        arenaB.Put32(a[7] + 0xC, pedpos);
        const u32 hvModes[4]{0u, 0xFFFu, Rand(a[23], 98) & 0xFFFu, 0x555u};
        const u32 hv = hvModes[Rand(a[23], 99) % 4];
        for (u32 i = 0; i < 12; ++i) arenaB.Put8(a[6] + 0x18 * i, u8((Rand(a[23], i) & 0xF7u) | (((hv >> i) & 1u) ? 8u : 0u)));
        for (u32 k = 0; k < 3; ++k) arenaB.Put32(a[8 + k] + 0x20, a[17 + k]);
        for (u32 k = 0; k < 3; ++k) {
            for (u32 i = 0; i < 12; ++i) {
                const u32 base = a[11] + 0x130 * k + 0x18 * i;
                arenaC.Put32(base + 0x10, ((a[20 + k] >> i) & 1u) ? (Rand(a[22], 100 + k * 12 + i) | 1u) : 0u);
                arenaC.Put32(base + 0x14, Rand(a[22], 200 + k * 12 + i));
            }
            arenaC.Put32(a[11] + 0x130 * k + 0x14, u32(arenaB.Obj(a[8 + k])));
        }
        PutRaw32(kGlobalClump, u32(arenaB.Obj(a[7])));
        call.ecx = c.ecx;
        call.eax = u32(arenaA.Obj(a[0]));
        call.args = {u32(arenaB.Obj(a[6])), a[3], 0, 0, 0, 0, 0, 0}; call.argCount = 2; call.popBytes = 8;
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
            return Gate(entry, call.args.data(), call.argCount, call.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}};
        obs["gchanges"] = json::array();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kConst: case kLoad: case kGetNode: obs["ret"] = eax; break;                                   // valor completo de EAX
        case kConstAL: obs["ret"] = eax & 0xFFu; break;                                                    // bool: solo AL
        case kSdd: case kStaticSdd: case kCtor: obs["ret"] = Tag(uintptr_t(eax)); break;                   // this como etiqueta de arena
        default: obs["ret"] = nullptr; break;                                                              // void (GetLocalSoundLevel devuelve por ST0: va en post_floating_state)
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN165A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN165A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn165a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn165a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn165a"); }
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
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    auto Del = [&] { return F(0x61A5B0, &FkDelete<0x61A5B0>); };   // CTask::operator delete (con hook)
    auto EvDel = [&] { return F(0x4B5630, &FkDelete<0x4B5630>); }; // CEvent::operator delete (con hook)
    auto CrtDel = [&] { return E(0x8214BD, &FkDelete<0x8214BD>); }; // operator delete del CRT (sin hook, la DLL lo llama por la direccion del EXE)
    const auto& K = kBodyFakes;
    env.stubs[0] = {F(0x4EF1C0, &FkStop)};
    env.stubs[1] = {F(0x4EF850, &FkForget), F(0x4EF660, &FkSoundDtor)};
    env.stubs[2] = {F(0x571A00, &FkCleanUp), F(0x4AC480, K[2])};
    env.stubs[8] = {Rt(2, K[8]), EvDel()};
    env.stubs[10] = {F(0x607300, K[10]), F(0x6074E0, &FkDelete<0x6074E0>)};
    env.stubs[12] = {F(0x72F430, &FkFree)};
    env.stubs[13] = {F(0x72F430, &FkFree), CrtDel()};
    env.stubs[14] = {F(0x4D0240, &FkFindKF)};
    env.stubs[15] = {F(0x64FC00, K[15]), CrtDel()};
    env.stubs[16] = {F(0x640330, K[16]), Del()};
    env.stubs[17] = {F(0x640380, K[17]), Del()};
    env.stubs[18] = {F(0x6403D0, K[18]), Del()};
    env.stubs[19] = {F(0x640420, K[19]), Del()};
    env.stubs[20] = {U(0x6404D0, Lote165ABody_CTaskComplexDragPedFromCar(), K[20]), Del()};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {3u, 4u, 5u, 6u, 7u, 9u, 11u, 21u, 22u, 23u, 24u, 25u}) dllReal[f] = "toda la funcion (sin colaboradores)";
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
    // Memoria global del juego: solo gpAnimBlendClump (0xB4EA0C) en RpAnimBlendNodeUpdateKeyFrames, que apunta a un objeto de la arena B y se restaura tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kGlobalClump, 4);
    regionsOf[14].push_back(0);
    Run(env, ready, registered, bodies, collaborators);
}
}
