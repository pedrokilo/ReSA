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
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn118A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Ocho funciones (hook S), cada una con 16384 pares EXE/DLL:
//   0  CMemoryMgr::Malloc                0x72F420  cdecl (tamano, hint). Colaborador SIN hook RH: malloc del CRT del EXE (0x824257, cdecl): falso en esa direccion (los hilos ajenos pasan por un trampolin al original).
//   1  CMemoryMgr::Realloc               0x72F440  cdecl (puntero, tamano, hint). Colaborador: realloc del CRT (0x824269), falso.
//   2  CMemoryMgr::Calloc                0x72F460  cdecl (num, size, hint). Colaborador: calloc del CRT (0x824416), falso.
//   3  CMemoryMgr::MallocAlign           0x72F4C0  cdecl (tamano, align, hint). Colaborador: malloc del CRT (falso, devuelve una base dentro de una arena alineada a 4096).
//   4  CMemoryMgr::FreeAlign             0x72F4F0  cdecl (puntero). Colaborador: free del CRT (0x82413F), falso.
//   5  CDecision::CDecision              0x6040C0  thiscall; devuelve this. Colaborador REAL (sin falso): CDecision::SetDefault 0x600530 (hook RH apagado: el EXE corre sus bytes y la DLL su cuerpo).
//   6  CTaskComplexLeaveCar::ComputeTargetDoor 0x63BAB0 thiscall (ped), `ret 4`. Colaborador (con hook RH): CCarEnterExit::ComputeTargetDoorToExit 0x64F110 (falso identico en el EXE y en el cuerpo de la DLL).
//   7  CWaterLevel::CalculateWavesOnlyForCoordinate2 0x6E8550 cdecl (x, y, altura*, grandes, pequenas). Colaborador (con hook RH): ...2Native 0x6E7210 (falso identico).
// Falsos identicos en la direccion del EXE y en el cuerpo de la DLL cuando el colaborador tiene hook RH (apagado durante el banco). Registran sus argumentos EN ORDEN y los punteros a arenas como etiquetas.
// Memoria real del juego: ninguna (todo el estado vive en arenas propias).

namespace F118ALive {
using namespace F118AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x72F420, 0x72F440, 0x72F460, 0x72F4C0, 0x72F4F0, 0x6040C0, 0x63BAB0, 0x6E8550};
inline constexpr uintptr_t kCrtMalloc = 0x824257, kCrtRealloc = 0x824269, kCrtCalloc = 0x824416, kCrtFree = 0x82413F;
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas (alineadas a 4096: las direcciones absolutas de MallocAlign son deterministas)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F118AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Pattern(u32 kind, u32 seed) { if (kind == 0) Fill(seed); else memset(m.data(), kind == 1 ? 0x00 : (kind == 2 ? 0xFF : 0xCD), N); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia, sin los rangos [desplazamiento, longitud) respecto al objeto
    json Changes(std::initializer_list<std::pair<int64, int64>> skip = {}) const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            const int64 off = int64(i) - int64(kGuard);
            bool masked = false;
            for (const auto& s : skip) masked |= off >= s.first && off < s.first + s.second;
            if (m[i] != r[i] && !masked) out.push_back(json::array({off, m[i]}));
        }
        return out;
    }
};
inline Arena<kObjWin> objA;          // 0: objeto / bloque principal
inline Arena<kAWin> aA;              // 1: puntero de entrada (vehiculo, valor guardado, altura)
inline Arena<kBWin> bA;              // 2: valor que devuelven los falsos / ped

// Estado del caso en curso para los falsos
struct State { const C* c{}; DWORD tid{}; u32 ret{}; };
inline State g;
inline uint8 *mallocTramp{}, *reallocTramp{}, *callocTramp{}, *freeTramp{};

// Eventos: 40 free [puntero]; 50 malloc [tamano]; 51 realloc [puntero, tamano]; 52 calloc [num, size]; 60 ComputeTargetDoorToExit [vehiculo, ped]; 70 ...2Native [x, y, grandes, pequenas, altura*].
inline void* __cdecl FkMalloc(u32 size) {                                                                // malloc del CRT del EXE (0x824257, cdecl)
    if (GetCurrentThreadId() != g.tid) return reinterpret_cast<void*(__cdecl*)(u32)>(mallocTramp)(size);   // hilos ajenos: el malloc real
    Ev& e = PushEv(50, 1); e.v[0] = size;
    return reinterpret_cast<void*>(uintptr_t(g.ret));
}
inline void* __cdecl FkRealloc(void* p, u32 size) {                                                      // realloc del CRT (0x824269, cdecl)
    if (GetCurrentThreadId() != g.tid) return reinterpret_cast<void*(__cdecl*)(void*, u32)>(reallocTramp)(p, size);
    Ev& e = PushEv(51, 2); e.v[0] = Tag(p); e.v[1] = size;
    return reinterpret_cast<void*>(uintptr_t(g.ret));
}
inline void* __cdecl FkCalloc(u32 num, u32 size) {                                                       // calloc del CRT (0x824416, cdecl)
    if (GetCurrentThreadId() != g.tid) return reinterpret_cast<void*(__cdecl*)(u32, u32)>(callocTramp)(num, size);
    Ev& e = PushEv(52, 2); e.v[0] = num; e.v[1] = size;
    return reinterpret_cast<void*>(uintptr_t(g.ret));
}
inline void __cdecl FkFree(void* p) {                                                                    // free del CRT (0x82413F, cdecl)
    if (GetCurrentThreadId() != g.tid) { reinterpret_cast<void(__cdecl*)(void*)>(freeTramp)(p); return; }
    Ev& e = PushEv(40, 1); e.v[0] = Tag(p);
}
inline u32 __cdecl FkDoorToExit(void* vehicle, void* ped) {                                              // CCarEnterExit::ComputeTargetDoorToExit (0x64F110, cdecl)
    Ev& e = PushEv(60, 2); e.v[0] = Tag(vehicle); e.v[1] = Tag(ped);
    return g.ret;
}
inline void __cdecl FkNative(u32 x, u32 y, u32 bigW, u32 smallW, void* height) {                         // CalculateWavesOnlyForCoordinate2Native (0x6E7210, cdecl)
    Ev& e = PushEv(70, 5); e.v[0] = x; e.v[1] = y; e.v[2] = bigW; e.v[3] = smallW; e.v[4] = Tag(height);
}
// Trampolin para hilos ajenos al banco: copia de las primeras instrucciones de la entrada (n bytes, comprobados) y salto al resto.
inline uint8* MakeTrampoline(uintptr_t entry, const uint8* expect, size_t n) {
    if (memcmp(reinterpret_cast<void*>(entry), expect, n) != 0) return nullptr;
    auto* t = static_cast<uint8*>(VirtualAlloc(nullptr, 32, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!t) return nullptr;
    memcpy(t, expect, n);
    t[n] = 0xE9;
    const int32 rel = int32(entry + n - (uintptr_t(t) + n + 5));
    memcpy(t + n + 1, &rel, 4);
    FlushInstructionCache(GetCurrentProcess(), t, 32);
    return t;
}
inline bool MakeTrampolines() {
    static constexpr uint8 freeB[7]{0x6A, 0x0C, 0x68, 0x38, 0x80, 0x88, 0x00};                        // push 0Ch; push 888038h
    static constexpr uint8 mallocB[10]{0xFF, 0x35, 0xCC, 0xAD, 0xC9, 0x00, 0xFF, 0x74, 0x24, 0x08};   // push [C9ADCCh]; push [esp+8]
    static constexpr uint8 reallocB[7]{0x6A, 0x14, 0x68, 0x58, 0x80, 0x88, 0x00};                     // push 14h; push 888058h
    static constexpr uint8 callocB[7]{0x6A, 0x10, 0x68, 0x68, 0x80, 0x88, 0x00};                      // push 10h; push 888068h
    freeTramp = MakeTrampoline(kCrtFree, freeB, 7);
    mallocTramp = MakeTrampoline(kCrtMalloc, mallocB, 10);
    reallocTramp = MakeTrampoline(kCrtRealloc, reallocB, 7);
    callocTramp = MakeTrampoline(kCrtCalloc, callocB, 7);
    return freeTramp && mallocTramp && reallocTramp && callocTramp;
}

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

inline uintptr_t PtrKind(u32 kind, uintptr_t arena, u32 raw) { return kind == 0 ? arena : (kind == 1 ? 0u : raw); }

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 50u;
        objA.Fill(sd++); aA.Fill(sd++); bA.Fill(sd++);
        TagClear();
        TagAdd(objA.m.data(), objA.m.size());       // 0
        TagAdd(aA.m.data(), aA.m.size());           // 1
        TagAdd(bA.m.data(), bA.m.size());           // 2
        g = State{}; g.c = &c; g.tid = GetCurrentThreadId(); nev = 0;
        std::vector<u32> args;
        u32 ecx = c.seed, popBytes = 0;
        switch (fn) {
        case 0:   // CMemoryMgr::Malloc(size, hint)
            g.ret = u32(PtrKind(c.a[2], bA.Obj(c.a[4]), c.a[3]));
            args = { c.a[0], c.a[1] };
            popBytes = 8;
            break;
        case 1:   // CMemoryMgr::Realloc(ptr, size, hint)
            g.ret = u32(PtrKind(c.a[5], bA.Obj(c.a[7]), c.a[6]));
            args = { u32(PtrKind(c.a[0], aA.Obj(c.a[2]), c.a[1])), c.a[3], c.a[4] };
            popBytes = 12;
            break;
        case 2:   // CMemoryMgr::Calloc(num, size, hint)
            g.ret = u32(PtrKind(c.a[3], bA.Obj(c.a[5]), c.a[4]));
            args = { c.a[0], c.a[1], c.a[2] };
            popBytes = 12;
            break;
        case 3:   // CMemoryMgr::MallocAlign(size, align, hint)
            g.ret = u32(objA.Obj(c.a[3]));
            args = { c.a[0], c.a[1], c.a[2] };
            popBytes = 12;
            break;
        case 4:   // CMemoryMgr::FreeAlign(ptr)
            objA.Put32(c.a[0] - 4, u32(PtrKind(c.a[1], aA.Obj(c.a[3]), c.a[2])));
            args = { u32(objA.Obj(c.a[0])) };
            popBytes = 4;
            break;
        case 5: { // CDecision::CDecision()
            objA.Pattern(c.a[1], sd++);
            for (unsigned i = 0; i < 15; ++i) objA.Put32(c.a[0] + 4 * i, c.a[2 + i]);
            ecx = u32(objA.Obj(c.a[0]));
            popBytes = 0;
            break;
        }
        case 6: { // CTaskComplexLeaveCar::ComputeTargetDoor(ped)
            objA.Put32(c.a[0] + 0xC, u32(PtrKind(c.a[1], aA.Obj(), c.a[2])));
            objA.Put32(c.a[0] + 0x10, c.a[5]);
            g.ret = c.a[6];
            ecx = u32(objA.Obj(c.a[0]));
            args = { u32(PtrKind(c.a[3], bA.Obj(), c.a[4])) };
            popBytes = 0;
            break;
        }
        default:  // CWaterLevel::CalculateWavesOnlyForCoordinate2(x, y, height*, big, small)
            args = { c.a[0], c.a[1], u32(PtrKind(c.a[2], aA.Obj(c.a[4]), c.a[3])), c.a[5], c.a[6] };
            popBytes = 20;
            break;
        }
        objA.Snap(); aA.Snap(); bA.Snap();
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        const u32 eax = sides[s].reg.eax;
        const uintptr_t lo = uintptr_t(objA.m.data()) + 4, hi = uintptr_t(objA.m.data()) + objA.m.size();
        if (fn == 3 && eax >= lo && eax < hi) add(0, objA.Changes({{int64(eax) - int64(objA.Obj()) - 4, 4}}));   // los cuatro bytes de [r - 4] (la base) se comparan como etiqueta
        else add(0, objA.Changes());
        add(1, aA.Changes()); add(2, bA.Changes());
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        if (fn <= 3 || fn == 5) obs["ret"] = Tag(uintptr_t(eax));
        else obs["ret"] = nullptr;
        if (fn == 3) {
            if (eax >= lo && eax < hi) obs["ptrs"] = json::array({Tag(uintptr_t(*reinterpret_cast<const u32*>(uintptr_t(eax) - 4)))});
            else obs["ptrs"] = json::array({"fuera de la arena"});
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
        for (const auto& s : env.stubs[f]) {
            if (s.pair.hooked) ready &= s.pair.dll != 0 && ModuleOf(s.pair.dll) == env.asi;
            else ready &= s.pair.dll == 0 || ModuleOf(s.pair.dll) == env.asi;
        }
        for (const auto& p : env.reals[f]) {
            if (p.hooked) ready &= p.dll != 0 && ModuleOf(p.dll) == env.asi;
            else ready &= p.dll == 0 || ModuleOf(p.dll) == env.asi;
        }
    }
    ready &= freeTramp && mallocTramp && reallocTramp && callocTramp;
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < kCases; ++c) CaseRun(env, totals, f, c);
            Write({{"event", "function_summary"}, {"function", kFn[f]}, {"cases", kCases}, {"failures", totals.failures - before}, {"state_restored", env.WatchesIntact() && RegionsIntact()}});
        }
    }
    RestoreRegions();
    const bool entriesIntact = env.WatchesIntact() && RegionsIntact();
    bool stateRestored = entriesIntact;
    for (auto& r : env.collab) if (r) stateRestored &= r->Restore();
    for (auto& r : env.route) if (r) stateRestored &= r->Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntact}});
    Write({{"event", ready && totals.passed && stateRestored ? "complete" : "error"}, {"pairs", totals.pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntact}});
    RestoreFloatingState(); savedFPActive = false;
}

// Busca el hook Simple registrado para una direccion del EXE (categoria y nombre), para crear su ruta (apagada) y obtener el cuerpo de la DLL.
inline bool FindHook(uintptr_t exe, std::string& cat, std::string& name) {
    bool found = false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items())
            if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
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
    if (!std::filesystem::exists(dir / "fn118a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn118a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn118a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name;
        const bool found = FindHook(kEntries[f], cat, name);
        if (found) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    auto R = [&](uintptr_t exe) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return it->second;
    };
    env.stubs[0] = {F(kCrtMalloc, &FkMalloc)};
    env.stubs[1] = {F(kCrtRealloc, &FkRealloc)};
    env.stubs[2] = {F(kCrtCalloc, &FkCalloc)};
    env.stubs[3] = {F(kCrtMalloc, &FkMalloc)};
    env.stubs[4] = {F(kCrtFree, &FkFree)};
    env.stubs[5] = {};
    env.stubs[6] = {F(0x64F110, &FkDoorToExit)};
    env.stubs[7] = {F(0x6E7210, &FkNative)};
    env.reals[5] = {R(0x600530)};
    ready &= MakeTrampolines();   // despues de construir las rutas de los colaboradores: 0x824257 (CRT malloc) tiene hook RH (VehicleRecording.cpp) y las rutas lo apagan; los trampolines copian los bytes ORIGINALES
    const char* dllReal[kFunctions] = {
        "toda la funcion: llama a malloc del CRT del EXE (0x824257, falso) con el tamano tal cual y devuelve su valor",
        "toda la funcion: llama a realloc del CRT del EXE (0x824269, falso) con (puntero, tamano) tal cual, nulo incluido",
        "toda la funcion: llama a calloc del CRT del EXE (0x824416, falso) con (num, size) tal cual",
        "toda la funcion: malloc(size + align) del CRT (falso, base en una arena alineada a 4096), alineamiento con la mascara ~(align - 1) y la base guardada en [r - 4]",
        "toda la funcion: lee la base guardada en [puntero - 4], la sustituye en el argumento y llama a free del CRT del EXE (0x82413F, falso)",
        "toda la funcion y CDecision::SetDefault (0x600530) corriendo de verdad (EXE: sus bytes; DLL: su cuerpo): los 0x3C bytes del objeto y el valor devuelto (this)",
        "toda la funcion: la comprobacion de la puerta (+0x10) y la llamada a ComputeTargetDoorToExit (falso identico) con (vehiculo +0xC, ped)",
        "toda la funcion: reordena los cinco argumentos y llama a CalculateWavesOnlyForCoordinate2Native (falso identico); no toca memoria por si misma"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        for (const auto& p : env.reals[f]) {
            env.watches.emplace_back(p.exe, 16);
            real.push_back({{"exe", p.exe}, {"dll", p.dll != 0}, {"hooked", p.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
