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

// Banco par a par Fn145A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 22 funciones del EXE (CAEWMADecoder::Shutdown y 21 destructores con borrado de 27 y 30 bytes: CAEWMADecoder y 20 tareas; ver Fixtures.hpp), cada una con 16384 pares EXE/DLL.
// Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40) y, en Shutdown, memoria global real del
// juego (0xB6BAD4..0xB6BADB, se guarda al empezar y se restaura tras cada lado) y la IAT (FreeLibrary falso solo alrededor de la llamada: casilla 0x858110 del EXE y casilla del ASI). Falsos de los colaboradores: el mismo
// falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): cuerpos de destructor (con hook propio, `H`: el hook se apaga y el falso va en la envoltura de la DLL; sin hook, `U`: funcion propia de la DLL) y
// operator delete (CTask 0x61A5B0 con hook; CRT 0x8214BD sin hook, la DLL lo llama por esa direccion).
// Se observan: los bytes cambiados de las tres arenas y de la memoria global, los eventos de los falsos (con sus argumentos), el valor devuelto (this como etiqueta de arena en los destructores con borrado) y el estado x87
// (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Envolturas propias de la DLL del cuerpo de cada destructor: las exportan los .cpp de cada clase.
uintptr_t Lote145ABody_CAEWMADecoder();  // 0x502760 (con hook)
uintptr_t Lote145ABody_CTaskComplexCarSlowBeDraggedOutAndStandUp();  // 0x648690 (con hook)
uintptr_t Lote145ABody_CTaskSimpleBikeJacked();  // 0x648C40 (con hook)
uintptr_t Lote145ABody_CTaskComplexWanderFlee();  // 0x65B370 (con hook)
uintptr_t Lote145ABody_CTaskComplexFleePoint();  // 0x65B410 (con hook)
uintptr_t Lote145ABody_CTaskComplexFollowPointRoute();  // 0x66E050 (sin hook)
uintptr_t Lote145ABody_CTaskComplexFollowNodeRoute();  // 0x66EB70 (con hook)
uintptr_t Lote145ABody_CTaskComplexHitByGunResponse();  // 0x631E30 (con hook)
uintptr_t Lote145ABody_CTaskComplexSunbathe();  // 0x632050 (con hook)
uintptr_t Lote145ABody_CTaskSimpleGangDriveBy();  // 0x6218C0 (sin hook)
uintptr_t Lote145ABody_CTaskComplexDestroyCar();  // 0x621CB0 (con hook)
uintptr_t Lote145ABody_CTaskSimplePlayerOnFire();  // 0x6335B0 (con hook)
uintptr_t Lote145ABody_CTaskSimpleIKManager();  // 0x633830 (con hook)
uintptr_t Lote145ABody_CTaskComplexGangJoinRespond();  // 0x661720 (con hook)
uintptr_t Lote145ABody_CTaskComplexProstituteSolicit();  // 0x661AF0 (con hook)
uintptr_t Lote145ABody_CTaskComplexWaitForDryWeather();  // 0x632830 (con hook)
uintptr_t Lote145ABody_CTaskComplexWaitForBus();  // 0x632930 (con hook)
uintptr_t Lote145ABody_CTaskComplexAttractorPartnerWait();  // 0x633290 (con hook)
uintptr_t Lote145ABody_CTaskComplexUseSequence();  // 0x6396C0 (sin hook)
uintptr_t Lote145ABody_CTaskComplexWanderCriminal();  // 0x48E720 (con hook)
uintptr_t Lote145ABody_CTaskComplexGoToAttractor();  // 0x66B6A0 (con hook)

namespace F145ALive {
using namespace F145AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x502BF0, 0x502C40, 0x64C930, 0x64C950, 0x65D0C0, 0x65D0E0, 0x671610, 0x671730, 0x638160, 0x638180, 0x6275E0, 0x628780, 0x639310, 0x639330, 0x664A80, 0x664AA0, 0x638900, 0x638920, 0x638F00, 0x639F90, 0x48E700, 0x670400};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F145AFix::Arena(WIN);
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

// Memoria global del juego que el banco toca: WMCreateSyncReader (0xB6BAD8) y el manejador de wmvcore.dll (0xB6BAD4), que lee y escribe CAEWMADecoder::Shutdown (funcion 0); se guarda al empezar y se restaura tras cada lado.
enum : unsigned { kRegWma };
inline constexpr uintptr_t kWmaHandle = 0xB6BAD4, kWmaReader = 0xB6BAD8, kFreeLibrarySlot = 0x858110;

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
template<size_t... I> inline std::array<void*, kBodyExe.size()> MakeBodyFakes(std::index_sequence<I...>) { return {reinterpret_cast<void*>(&FkBody<kBodyExe[I]>)...}; }
inline const std::array<void*, kBodyExe.size()> kBodyFakes = MakeBodyFakes(std::make_index_sequence<kBodyExe.size()>{});

// FreeLibrary falso (stdcall, un argumento): en el hilo del banco registra [0x858110, manejador] y devuelve gRet; en otros hilos llama a la real.
inline FARPROC gRealFreeLibrary{};
inline BOOL WINAPI FkFreeLibrary(HMODULE h) {
    if (GetCurrentThreadId() != gBenchThread) return reinterpret_cast<BOOL(WINAPI*)(HMODULE)>(gRealFreeLibrary)(h);
    Ev& e = PushEv(0x858110, 1);
    e.v[0] = u32(reinterpret_cast<uintptr_t>(h));
    return BOOL(gRet);
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
    if (c.fn == kIdxShutdown) {   // cdecl sin argumentos: [0xB6BAD4] = manejador, [0xB6BAD8] = WMCreateSyncReader; el FreeLibrary falso devuelve gRet
        PutGlobal32(kWmaHandle, a[3]);
        PutGlobal32(kWmaReader, a[4]);
        gRet = a[5];
        call.argCount = 0; call.popBytes = 0;
        return call;
    }
    // destructores con borrado: banderas en la pila (`ret 4`: lo limpia el callee); [this] = vtable previa
    arenaA.Put32(a[0], a[3]);
    gRet = a[4]; gDtorWrite = a[5];
    call.ecx = u32(arenaA.Obj(a[0]));
    call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
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
        if (IsScalarDeleting(fn)) obs["ret"] = Tag(uintptr_t(eax));   // this como etiqueta de arena
        else obs["ret"] = nullptr;                                    // Shutdown es void
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN145A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN145A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn145a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn145a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn145a"); }
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
    json hooksFound = json::array(), iatInfo = json::object();
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
    // Colaboradores de los destructores con borrado (idx 1..21): cuerpo (envoltura propia de la DLL: H con hook propio, U sin hook) y operator delete (F: CTask 0x61A5B0 con hook; E: CRT 0x8214BD sin hook)
    using BodyFn = uintptr_t (*)();
    static const BodyFn kDllBody[kBodyExe.size()] = {
        &Lote145ABody_CAEWMADecoder,
        &Lote145ABody_CTaskComplexCarSlowBeDraggedOutAndStandUp,
        &Lote145ABody_CTaskSimpleBikeJacked,
        &Lote145ABody_CTaskComplexWanderFlee,
        &Lote145ABody_CTaskComplexFleePoint,
        &Lote145ABody_CTaskComplexFollowPointRoute,
        &Lote145ABody_CTaskComplexFollowNodeRoute,
        &Lote145ABody_CTaskComplexHitByGunResponse,
        &Lote145ABody_CTaskComplexSunbathe,
        &Lote145ABody_CTaskSimpleGangDriveBy,
        &Lote145ABody_CTaskComplexDestroyCar,
        &Lote145ABody_CTaskSimplePlayerOnFire,
        &Lote145ABody_CTaskSimpleIKManager,
        &Lote145ABody_CTaskComplexGangJoinRespond,
        &Lote145ABody_CTaskComplexProstituteSolicit,
        &Lote145ABody_CTaskComplexWaitForDryWeather,
        &Lote145ABody_CTaskComplexWaitForBus,
        &Lote145ABody_CTaskComplexAttractorPartnerWait,
        &Lote145ABody_CTaskComplexUseSequence,
        &Lote145ABody_CTaskComplexWanderCriminal,
        &Lote145ABody_CTaskComplexGoToAttractor,
    };
    static const bool kBodyHooked[kBodyExe.size()] = {true, true, true, true, true, false, true, true, true, false, true, true, true, true, true, true, true, true, false, true, true};
    for (unsigned i = 0; i < kBodyExe.size(); ++i) {
        const unsigned fn = i + 1;
        StubSpec body = kBodyHooked[i] ? H(kBodyExe[i], kDllBody[i](), kBodyFakes[i]) : U(kBodyExe[i], kDllBody[i](), kBodyFakes[i]);
        StubSpec del = fn == kIdxWmaDelete ? E(0x8214BD, &FkDelete<0x8214BD>) : F(0x61A5B0, &FkDelete<0x61A5B0>);
        env.stubs[fn] = {body, del};
    }
    // Shutdown: FreeLibrary por la IAT (casilla del EXE 0x858110 y casillas del ASI que apuntan a la FreeLibrary real)
    gRealFreeLibrary = nullptr;
    {
        const HMODULE mods[2]{GetModuleHandleA("kernel32.dll"), GetModuleHandleA("kernelbase.dll")};
        uintptr_t cand[2]{};
        for (int m = 0; m < 2; ++m) cand[m] = mods[m] ? uintptr_t(GetProcAddress(mods[m], "FreeLibrary")) : 0;
        auto isReal = [&](uintptr_t v) { return v && (v == cand[0] || v == cand[1]); };
        auto* exeSlot = reinterpret_cast<uintptr_t*>(kFreeLibrarySlot);
        const bool exeOk = isReal(*exeSlot);
        if (exeOk) gRealFreeLibrary = reinterpret_cast<FARPROC>(*exeSlot);
        std::vector<uintptr_t*> asiSlots;
        auto* base = reinterpret_cast<uint8*>(env.asi);
        auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew);
        const auto& dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
        if (dir.VirtualAddress)
            for (auto* d = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(base + dir.VirtualAddress); d->Name; ++d)
                for (auto* s = reinterpret_cast<uintptr_t*>(base + d->FirstThunk); *s; ++s)
                    if (isReal(*s)) asiSlots.push_back(s);
        if (!exeOk || asiSlots.empty() || asiSlots.size() > 3) ready = false;
        env.iat[kIdxShutdown].push_back({exeSlot, reinterpret_cast<void*>(&FkFreeLibrary)});
        for (auto* s : asiSlots) env.iat[kIdxShutdown].push_back({s, reinterpret_cast<void*>(&FkFreeLibrary)});
        iatInfo = {{"exe_slot_ok", exeOk}, {"asi_slots", asiSlots.size()}};
    }
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo el cuerpo del destructor (envoltura propia de la DLL, falso identico) y operator delete (falso identico)";
    dllReal[kIdxShutdown] = "toda la funcion (FreeLibrary es un falso en la casilla de la IAT del EXE y en la casilla de la IAT del ASI, solo alrededor de la llamada)";
    dllReal[kIdxWmaDelete] = "toda la funcion salvo el cuerpo ~CAEWMADecoder (0x502760, con hook propio: el hook se apaga y el falso va en la envoltura de la DLL) y operator delete del CRT (0x8214BD, sin hook: la DLL lo llama por esa direccion, un solo parche)";
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
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", f == kIdxShutdown ? iatInfo : json(nullptr)}};
    }
    // Memoria global del juego que toca el banco (se guarda ahora y se restaura tras cada lado)
    regions.clear();
    regions.emplace_back(kWmaHandle, 8);
    regionsOf[kIdxShutdown] = {kRegWma};
    Run(env, ready, registered, bodies, collaborators);
}
}
