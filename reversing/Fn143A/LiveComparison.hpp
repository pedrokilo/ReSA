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

// Banco par a par Fn143A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 22 funciones diminutas del EXE (cuatro destructores con borrado de 27 y 30 bytes, cuatro cuerpos de destructor y dos de constructor, siete ranuras triviales de CEventGunShot, tres tipos de evento o tarea y
// dos saltos RW por el dispositivo abierto; ver Fixtures.hpp para la lista y el esquema de familias), cada una con 16384 pares EXE/DLL.
// Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedGlobalInstall) o V (RH_ScopedVMTInstall: ranura de la vtable del EXE; el Virtual lleva dentro un Simple sobre la entrada del EXE, que es lo que el banco
// enciende). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40) y, donde hace falta, memoria global real del juego (se guarda al empezar y se
// restaura tras cada lado). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): cuerpos de destructor y de constructor (con hook o funciones
// propias de la DLL sin hook, ver Update) y operator delete (CTask 0x61A5B0 con hook; CEvent 0x4B5630 sin hook, funcion de la DLL; CRT 0x8214BD sin hook, la DLL lo llama por su direccion).
// Los saltos RW (1 y 2) leen el puntero del motor 0xC97B24, que el banco apunta a un RwGlobals falso en la arena B con funciones falsas del dispositivo (+0x28 linea, +0x34 primitiva indexada; +0x2C y +0x30 registran un evento inesperado).
// Se observan: los bytes cambiados de las tres arenas y de la memoria global, los eventos de los falsos (con sus argumentos), el valor devuelto (EAX completo, o AL en la funcion booleana) y el estado x87
// (vacio salvo ST0 de GetLocalSoundLevel, que se compara registro a registro entre EXE y DLL y contra el oraculo). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Funciones propias de la DLL sin hook (cuerpos de destructor): las exportan los .cpp de cada clase.
uintptr_t Lote143AEventGroupDestructorBody();            // 0x4AB9B0 (CEventGroup, llamado por CEventGlobalGroup), EventGlobalGroup.cpp
uintptr_t Lote143AMedicTreatInjuredPedDestructorBody();  // 0x658C30 (CTaskComplexMedicTreatInjuredPed), TaskComplexMedicTreatInjuredPed.cpp

namespace F143ALive {
using namespace F143AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x4ABAA0, 0x734EA0, 0x734EC0, 0x4C4A60, 0x4C5540, 0x4C5920, 0x4AC6B0, 0x4AC6C0, 0x4AC6D0, 0x4AC6E0, 0x4AC700, 0x4AC710, 0x4AC830,
                                                   0x6A0650, 0x6A0660, 0x6A06D0, 0x61C310, 0x61C320, 0x61C390, 0x658AD0, 0x658AE0, 0x659FC0};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F143AFix::Arena(WIN);
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

// Memoria global del juego que el banco toca: el puntero del motor RenderWare (0xC97B24), que leen los dos saltos RW (funciones 1 y 2); se guarda al empezar y se restaura tras cada lado.
enum : unsigned { kRegEngine };
inline constexpr uintptr_t kEngineAddr = 0xC97B24;

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

// Funciones falsas del dispositivo RW (cdecl): registran [direccion del EXE del salto, argumentos (punteros como etiquetas de arena)] y devuelven gRet (EAX completo).
inline u32 __cdecl FkIndexed(u32 prim, void* verts, u32 n, void* indices, u32 ni) {
    Ev& e = PushEv(0x734EA0, 5);
    e.v[0] = prim; e.v[1] = Tag(verts); e.v[2] = n; e.v[3] = Tag(indices); e.v[4] = ni;
    return gRet;
}
inline u32 __cdecl FkLine(void* verts, u32 n, u32 v1, u32 v2) {
    Ev& e = PushEv(0x734EC0, 4);
    e.v[0] = Tag(verts); e.v[1] = n; e.v[2] = v1; e.v[3] = v2;
    return gRet;
}
// Ranuras del dispositivo que ninguno de los dos saltos debe usar: dejan un evento que el oraculo no espera
template<u32 K> inline u32 __cdecl FkUnexpected() { PushEv(0xDEAD0000u + K, 0); return 0; }

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
    if (IsRwThunk(c.fn)) {   // RwGlobals falso en la arena B (a[0]); vertices e indices en la arena C o crudos
        arenaB.Put32(a[0] + 0x28, u32(reinterpret_cast<uintptr_t>(&FkLine)));
        arenaB.Put32(a[0] + 0x2C, u32(reinterpret_cast<uintptr_t>(&FkUnexpected<1>)));
        arenaB.Put32(a[0] + 0x30, u32(reinterpret_cast<uintptr_t>(&FkUnexpected<2>)));
        arenaB.Put32(a[0] + 0x34, u32(reinterpret_cast<uintptr_t>(&FkIndexed)));
        PutGlobal32(kEngineAddr, u32(arenaB.Obj(a[0])));
        gRet = a[11];
        const u32 verts = (a[6] & 1) ? u32(arenaC.Obj(a[9])) : a[7];
        const u32 indices = (a[6] & 2) ? u32(arenaC.Obj(a[10])) : a[8];
        if (c.fn == kIdxIndexed) { call.args = {a[3], verts, a[4], indices, a[5]}; call.argCount = 5; call.popBytes = 20; }
        else { call.args = {verts, a[3], a[4], a[5], 0u}; call.argCount = 4; call.popBytes = 16; }
        return call;
    }
    if (IsScalarDeleting(c.fn)) {   // destructores con borrado: banderas en la pila (`ret 4`: lo limpia el callee)
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = u32(arenaA.Obj(a[0]));
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        return call;
    }
    switch (c.fn) {
    case kIdxBaseCtor: case kIdxAtomicCtor: case kIdxVehDtor: case kIdxCollBody: case kIdxWeapBody: case kIdxTreatBody:   // [this] = valor previo; el falso (si lo hay) devuelve gRet y deja gDtorWrite
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = 0; call.popBytes = 0;
        break;
    case kIdxShotSource:   // [this+0x14] = m_firedBy
        arenaA.Put32(a[0] + 0x14, a[3]);
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = 0; call.popBytes = 0;
        break;
    case kIdxShotSound:    // byte [this+0x30] = m_bHasNoSound
        arenaA.m[kGuard + a[0] + 0x30] = u8(a[3]);
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = 0; call.popBytes = 0;
        break;
    case kIdxShotReport:   // thiscall con el peaton en la pila (`ret 4`: lo limpia el callee)
        arenaA.Put32(a[0], a[3]);
        call.ecx = u32(arenaA.Obj(a[0])); call.args[0] = a[3]; call.argCount = 1; call.popBytes = 0;
        break;
    default:               // constantes: `this` en la arena A (el EXE no lee la memoria)
        arenaA.Put32(a[0], a[3]);
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = 0; call.popBytes = 0;
        break;
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
        if (IsScalarDeleting(fn) || fn == kIdxBaseCtor || fn == kIdxAtomicCtor) obs["ret"] = Tag(uintptr_t(eax));                          // this como etiqueta de arena
        else if (IsRwThunk(fn) || fn == kIdxShotSource || fn == kIdxShotType || fn == kIdxShotLife || fn == kIdxShotPrio || fn == kIdxCollType || fn == kIdxWeapType || fn == kIdxTreatType) obs["ret"] = eax;   // EAX completo
        else if (fn == kIdxShotCanInt) obs["ret"] = eax & 0xFFu;                                                                          // bool: AL
        else obs["ret"] = nullptr;                                                                                                        // void (cuerpos y ReportCriminalEvent) o ST0 (GetLocalSoundLevel)
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN143A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN143A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn143a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn143a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn143a"); }
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
    using VoidDelete = void (*)(void*);
    const uintptr_t eventDelete = reinterpret_cast<uintptr_t>(static_cast<VoidDelete>(&CEvent::operator delete));
    env.stubs[kIdxGroupDelete] = {U(0x4AB9B0, Lote143AEventGroupDestructorBody(), &FkBody<0x4AB9B0>), E(0x8214BD, &FkDelete<0x8214BD>)};
    env.stubs[kIdxAtomicCtor] = {F(0x4C4A60, &FkBody<0x4C4A60>)};
    env.stubs[kIdxCollBody] = {F(0x4B1990, &FkBody<0x4B1990>)};
    env.stubs[kIdxCollDelete] = {F(0x6A0650, &FkBody<0x6A0650>), U(0x4B5630, eventDelete, &FkDelete<0x4B5630>)};
    env.stubs[kIdxWeapBody] = {F(0x4B1990, &FkBody<0x4B1990>)};
    env.stubs[kIdxWeapDelete] = {F(0x61C310, &FkBody<0x61C310>), U(0x4B5630, eventDelete, &FkDelete<0x4B5630>)};
    env.stubs[kIdxTreatBody] = {F(0x61A3D0, &FkBody<0x61A3D0>)};
    env.stubs[kIdxMedicDelete] = {U(0x658C30, Lote143AMedicTreatInjuredPedDestructorBody(), &FkBody<0x658C30>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[kIdxGroupDelete] = "toda la funcion salvo el cuerpo de CEventGroup (0x4AB9B0, funcion propia sin hook, falso identico) y operator delete (0x8214BD, sin hook: la DLL lo llama por esa direccion, un solo parche)";
    dllReal[kIdxIndexed] = "toda la funcion (la funcion falsa del dispositivo va en el RwGlobals falso, no es un parche de codigo)";
    dllReal[kIdxLine] = "toda la funcion (la funcion falsa del dispositivo va en el RwGlobals falso, no es un parche de codigo)";
    dllReal[kIdxAtomicCtor] = "toda la funcion salvo el cuerpo del constructor de CBaseModelInfo (0x4C4A60, con hook propio del lote, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kIdxCollBody] = "toda la funcion salvo ~CEventVehicleDamage (0x4B1990, con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kIdxCollDelete] = "toda la funcion salvo el cuerpo de CEventVehicleDamageCollision (0x6A0650, con hook propio del lote, falso identico) y CEvent::operator delete (0x4B5630, sin hook, funcion de la DLL, falso identico)";
    dllReal[kIdxWeapBody] = "toda la funcion salvo ~CEventVehicleDamage (0x4B1990, con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kIdxWeapDelete] = "toda la funcion salvo el cuerpo de CEventVehicleDamageWeapon (0x61C310, con hook propio del lote, falso identico) y CEvent::operator delete (0x4B5630, sin hook, funcion de la DLL, falso identico)";
    dllReal[kIdxTreatBody] = "toda la funcion salvo ~CTaskComplex (0x61A3D0, con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kIdxMedicDelete] = "toda la funcion salvo el cuerpo ~CTaskComplexMedicTreatInjuredPed (0x658C30, funcion propia sin hook, falso identico) y CTask::operator delete (0x61A5B0, con hook, falso identico)";
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
    regions.emplace_back(kEngineAddr, 4);
    regionsOf[kIdxIndexed] = {kRegEngine};
    regionsOf[kIdxLine] = {kRegEngine};
    Run(env, ready, registered, bodies, collaborators);
}
}
