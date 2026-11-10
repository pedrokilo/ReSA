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

// Banco par a par Fn154A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 15 funciones del EXE (tres destructores con borrado de tareas, GetTaskType y cuerpo del destructor de CTaskSimpleFall, dos copias de CEventHandlerHistory::Flush, cuatro virtuales triviales de CEventObjectCollision,
// IsHeadOnCollision, CPedIntelligence::AddInterestingEntity, CMatrix::AttachRW y la copia del destructor de CAEAudioEntity en 0x506CC0), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall /
// RH_ScopedNamedInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE
// y en el cuerpo de la DLL (MakeCollab / Pair): cuerpos de destructor (con hook propio, `H`: el hook se apaga y el falso va en la envoltura de la DLL; sin hook, `U`: funcion propia de la DLL; funcion con hook, `F`), CTask::operator delete
// 0x61A5B0, ~CTask 0x61A3A0, SetFinishCallback 0x4CEBE0, Normalise 0x59C910, CleanUpOldReference 0x571A00, RegisterReference 0x571B70, UpdateRwMatrix 0x59AD70, ~CAESound 0x4EF660 y RwMatrixDestroy 0x7F2A20 (sin hook, la DLL lo llama
// por esa direccion). Los destructores virtuales de Flush son entradas de una vtable falsa en la arena C. Se observan: los bytes cambiados de las tres arenas, los eventos de los falsos (con sus argumentos), el valor devuelto
// (this como etiqueta de arena; EAX completo en constantes; solo AL en bool) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Funciones propias de la DLL (envolturas) que exportan los .cpp de cada clase para que el banco las sustituya.
uintptr_t Lote154ABody_CTaskComplexSignalAtPed();           // 0x660AB0 (con hook)
uintptr_t Lote154ABody_CTaskGoToVehicleAndLean();           // 0x660EE0 (con hook)
uintptr_t Lote154ABody_CTaskComplexFallAndGetUp();          // 0x678790 (sin hook)

namespace F154ALive {
using namespace F154AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F154AFix::Arena(WIN);
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
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this] (la vtable de la base)
inline u32 gLife{};                  // valor que devuelve el GetLifeTime falso
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
// CAnimBlendAssociation::SetFinishCallback (0x4CEBE0; thiscall, dos dwords de pila, `ret 8`): evento [direccion, ECX (etiqueta), callback, dato]. No escribe nada.
inline void __fastcall FkSetFinish(void* self, void*, u32 callback, u32 data) { Ev& e = PushEv(0x4CEBE0, 3); e.v[0] = Tag(self); e.v[1] = callback; e.v[2] = data; }
// CEntity::CleanUpOldReference (0x571A00; thiscall, un puntero en la pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]. No toca la referencia.
inline void __fastcall FkCleanUp(void* self, void*, void* ref) { Ev& e = PushEv(0x571A00, 2); e.v[0] = Tag(self); e.v[1] = Tag(ref); }
// CEntity::RegisterReference (0x571B70; igual): evento [direccion, ECX (etiqueta), argumento (etiqueta)].
inline void __fastcall FkRegRef(void* self, void*, void* ref) { Ev& e = PushEv(0x571B70, 2); e.v[0] = Tag(self); e.v[1] = Tag(ref); }
// RwMatrixDestroy (0x7F2A20; cdecl, un puntero): evento [direccion, puntero (etiqueta)]. No lee nada.
inline void __cdecl FkDestroy(void* p) { Ev& e = PushEv(0x7F2A20, 1); e.v[0] = Tag(p); }
// CMatrix::UpdateRwMatrix (0x59AD70; thiscall, un puntero, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)].
inline void __fastcall FkUpdateRw(void* self, void*, void* m) { Ev& e = PushEv(0x59AD70, 2); e.v[0] = Tag(self); e.v[1] = Tag(m); }
// CVector::Normalise (0x59C910; thiscall sin argumentos, `ret`): evento [direccion, x, y, z de entrada]; deja gNorm en el vector. No toca la x87.
inline u32 gNorm[3]{};
inline void __fastcall FkNormalise(u32* v, void*) { Ev& e = PushEv(0x59C910, 3); e.v[0] = v[0]; e.v[1] = v[1]; e.v[2] = v[2]; v[0] = gNorm[0]; v[1] = gNorm[1]; v[2] = gNorm[2]; }
// ~CAESound (0x4EF660; thiscall sin argumentos, `ret`): evento [direccion, ECX (etiqueta)].
inline void __fastcall FkSoundDtor(void* self, void*) { Ev& e = PushEv(0x4EF660, 1); e.v[0] = Tag(self); }
// Destructor virtual con borrado falso de los eventos de Flush (ranura 0 de la vtable falsa; thiscall, `push 1`, `ret 4`): evento [identificador, ECX (etiqueta), bandera, +4, +8 y +0xC del historial (etiquetas)].
inline uintptr_t gHist{};
inline void __fastcall FkEvDtor(void* self, void*, u32 flag) {
    Ev& e = PushEv(kEvDtor, 5);
    e.v[0] = Tag(self); e.v[1] = flag;
    e.v[2] = Tag(uintptr_t(BitsOf(reinterpret_cast<void*>(gHist + 4))));
    e.v[3] = Tag(uintptr_t(BitsOf(reinterpret_cast<void*>(gHist + 8))));
    e.v[4] = Tag(uintptr_t(BitsOf(reinterpret_cast<void*>(gHist + 0xC))));
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
    gRet = 0; gDtorWrite = 0; gLife = 0;
    const u32 self = u32(arenaA.Obj(a[0]));
    switch (kKind[c.fn]) {
    case kConst:
        arenaA.Put32(a[0], a[3]);
        call.ecx = self;
        break;
    case kConstAL1:   // thiscall(evento) `ret 4`
        arenaA.Put32(a[0], a[3]);
        call.ecx = self;
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 0;
        break;
    case kSdd:   // thiscall con banderas en la pila (`ret 4`: lo limpia el callee)
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        break;
    case kFallBody:
        arenaA.Put32(a[0], a[3]);
        arenaA.Put32(a[0] + 0x14, a[6]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        break;
    case kFlush: {
        const u32 objs[3]{u32(arenaB.Obj(a[6])), u32(arenaB.Obj(a[7])), u32(arenaB.Obj(a[8]))};
        const u32 vt = u32(arenaC.Obj(a[9]));
        const u32 non = a[3] ? objs[0] : 0u;
        const u32 tmp = a[4] == 0 ? 0u : (a[4] == 1 ? objs[1] : objs[0]);
        const u32 sto = a[5] == 0 ? 0u : (a[5] == 1 ? objs[2] : (a[5] == 2 ? objs[0] : tmp));
        arenaA.Put32(a[0], a[10]); arenaA.Put32(a[0] + 4, non); arenaA.Put32(a[0] + 8, tmp); arenaA.Put32(a[0] + 0xC, sto);
        for (const u32 ob : {non, tmp, sto}) if (ob) memcpy(reinterpret_cast<void*>(ob), &vt, 4);
        arenaC.Put32(a[9], u32(uintptr_t(&FkEvDtor)));
        gHist = uintptr_t(self);
        call.ecx = self;
        break;
    }
    case kHeadOn:   // thiscall(ped) `ret 4`
        for (unsigned i = 0; i < 3; ++i) { arenaA.Put32(a[0] + 0x18 + 4 * i, a[3 + i]); arenaC.Put32(a[7] + 0x10 + 4 * i, a[8 + i]); gNorm[i] = a[16 + i]; }
        arenaB.Put32(a[6] + 0x14, u32(arenaC.Obj(a[7])));
        call.ecx = self;
        call.args[0] = u32(arenaB.Obj(a[6])); call.argCount = 1; call.popBytes = 0;
        break;
    case kAddInt:   // thiscall(entidad) `ret 4`
        for (unsigned i = 0; i < 3; ++i) arenaA.Put32(a[0] + 0x288 + 4 * i, a[3 + i]);
        call.ecx = self;
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        break;
    case kAttach:   // thiscall(matriz, bandera) `ret 8`
        arenaA.Put32(a[0] + 0x40, a[3]); arenaA.Put32(a[0] + 0x44, a[4]);
        call.ecx = self;
        call.args[0] = a[5]; call.args[1] = a[6]; call.argCount = 2; call.popBytes = 0;
        break;
    case kAudioDtor:
        arenaA.Put32(a[0], a[3]);
        call.ecx = self;
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
        case kConst: obs["ret"] = eax; break;                                                              // valor completo de EAX
        case kConstAL1: case kHeadOn: obs["ret"] = eax & 0xFFu; break;                                     // bool: solo AL
        case kSdd: obs["ret"] = Tag(uintptr_t(eax)); break;                                                // this como etiqueta de arena
        default: obs["ret"] = nullptr; break;                                                              // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN154A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN154A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn154a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn154a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn154a"); }
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
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    auto Del = [&] { return F(0x61A5B0, &FkDelete<0x61A5B0>); };   // CTask::operator delete (con hook)
    const auto& K = kBodyFakes;
    env.stubs[0] = {H(0x660AB0, Lote154ABody_CTaskComplexSignalAtPed(), K[0]), Del()};
    env.stubs[1] = {H(0x660EE0, Lote154ABody_CTaskGoToVehicleAndLean(), K[1]), Del()};
    env.stubs[2] = {U(0x678790, Lote154ABody_CTaskComplexFallAndGetUp(), K[2]), Del()};
    env.stubs[4] = {F(0x4CEBE0, &FkSetFinish), F(0x61A3A0, K[4])};
    env.stubs[11] = {F(0x59C910, &FkNormalise)};
    env.stubs[12] = {F(0x571A00, &FkCleanUp), F(0x571B70, &FkRegRef)};
    env.stubs[13] = {E(0x7F2A20, &FkDestroy), F(0x59AD70, &FkUpdateRw)};
    env.stubs[14] = {F(0x4EF660, &FkSoundDtor)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {3u, 5u, 6u, 7u, 8u, 9u, 10u}) dllReal[f] = "toda la funcion (sin colaboradores)";
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
    // Sin memoria global del juego: todo ocurre en las tres arenas propias
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    Run(env, ready, registered, bodies, collaborators);
}
}
