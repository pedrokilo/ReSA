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

// Banco par a par Fn132A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Nueve funciones del EXE de formas variadas (ver Fixtures.hpp para la lista y el esquema de familias), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedGlobalInstall /
// RH_ScopedNamedInstall: enlace de la entrada del EXE hacia la DLL) o V (RH_ScopedVMTInstall: ranura de la vtable; la ruta lleva dentro un enlace de la entrada, ver FindHook). El banco invoca la direccion del EXE
// con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40) y, donde hace falta, memoria global real del juego (se guarda al empezar y se restaura tras cada lado). Falsos de los colaboradores: el mismo falso
// en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair); el colaborador de la CRT del EXE (rand 0x821B1E) lleva el falso solo en la direccion del EXE (la DLL lo llama por esa direccion).
// Se observan: los bytes cambiados de las tres arenas y de la memoria global, los eventos de los falsos (con sus argumentos), el valor devuelto (cuando el EXE lo define) y el estado x87
// (vacio salvo en SunReflectionDistanceFactor, que devuelve en ST0). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F132ALive {
using namespace F132AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x41B660, 0x611F20, 0x6F9EB0, 0x5D0570, 0x6FBA20, 0x6007A0, 0x683950, 0x684D10, 0x636180};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F132AFix::Arena(WIN);
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

// Regiones de memoria global del juego que el banco toca (indices fijos; ver regionsOf)
enum : unsigned { kRegCursor, kRegScan, kRegCam };
inline constexpr uintptr_t kCursorAt = 0xBD0160, kScanBase = 0xBD0170, kCamBase = 0xB6F02C, kCamMatrixPtrAt = 0xB6F03C;
inline constexpr size_t kScanBytes = 0x940, kCamBytes = 0x14;

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelve el falso de rand (0x821B1E)
inline u32 gAlloc{};                 // valor que devuelve el falso de operator new (0x61A5A0)
inline u32 gCtorMode{}, gCtorVal{};  // EAX de los falsos de constructor: 0 = this, 1 = gCtorVal, 2 = 0, 3 = this + 4
inline u32 gCalls{}, gHash{};        // RwRGBAToPixel: numero de llamadas y FNV-1a de todos los argumentos
inline DWORD gBenchThread{};

inline u32 CtorReturn(void* self) {
    switch (gCtorMode) {
    case 0: return u32(uintptr_t(self));
    case 1: return gCtorVal;
    case 2: return 0u;
    default: return u32(uintptr_t(self)) + 4u;
    }
}
// CTask::operator new (0x61A5A0, cdecl, el tamano en la pila): evento [direccion, tamano]; devuelve gAlloc
inline void* __cdecl FkOpNew(size_t size) {
    Ev& e = PushEv(0x61A5A0, 1);
    e.v[0] = u32(size);
    return reinterpret_cast<void*>(uintptr_t(gAlloc));
}
// CTaskComplexWaitForPed::Constructor (0x683340, thiscall, cuatro dwords de pila, `ret 10h`): evento [direccion, ECX (etiqueta), ped, radio (bits), tiempo, byte de rotacion (dword crudo)]
inline u32 __fastcall FkWaitCtor(void* self, void*, u32 ped, u32 radius, u32 time, u32 rotate) {
    Ev& e = PushEv(0x683340, 5);
    e.v[0] = Tag(self); e.v[1] = ped; e.v[2] = radius; e.v[3] = time; e.v[4] = rotate;
    return CtorReturn(self);
}
// CTaskComplexPartnerDeal::Constructor (0x684190, thiscall, siete dwords de pila, `ret 1Ch`): evento [direccion, ECX (etiqueta), nombre (etiqueta), companero, lider (dword crudo), multiplicador (bits), x, y, z (bits)]
inline u32 __fastcall FkPartnerCtor(void* self, void*, u32 name, u32 partner, u32 lead, u32 dist, u32 x, u32 y, u32 z) {
    Ev& e = PushEv(0x684190, 8);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(name)); e.v[2] = partner; e.v[3] = lead; e.v[4] = dist; e.v[5] = x; e.v[6] = y; e.v[7] = z;
    return CtorReturn(self);
}
// RwRGBAToPixel (0x803740, cdecl, un puntero a RwRGBA y el formato): cuenta y FNV-1a de (rgba, formato) de TODAS las llamadas; evento solo para las 16 primeras [direccion, rgba, formato]; devuelve Rand(rgba, formato)
inline u32 __cdecl FkRgbaToPixel(const void* rgba, u32 format) {
    const u32 v = BitsOf(rgba);
    if (gCalls < 16) {
        Ev& e = PushEv(0x803740, 2);
        e.v[0] = v; e.v[1] = format;
    }
    ++gCalls;
    for (const u32 w : {v, format})
        for (unsigned k = 0; k < 4; ++k) { gHash ^= (w >> (8 * k)) & 0xFFu; gHash *= 16777619u; }
    return Rand(v, format);
}
// CTaskSimple::CTaskSimple (0x61A390, thiscall sin argumentos, `ret`): evento [direccion, ECX (etiqueta), dword de [ECX] al entrar]; NO escribe; devuelve this
inline void* __fastcall FkSimpleCtor(void* self, void*) {
    Ev& e = PushEv(0x61A390, 2);
    e.v[0] = Tag(self); e.v[1] = BitsOf(self);
    return self;
}
// rand (0x821B1E, cdecl, sin argumentos): evento [direccion]; devuelve gRet
inline int __cdecl FkRand() {
    PushEv(0x821B1E, 0);
    return int(gRet);
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
    std::array<u32, 4> args{};
    u32 argCount{}, popBytes{}, ecx{};
};

inline void PutGlobal32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }
inline u32 GetGlobal32(uintptr_t at) { u32 v; memcpy(&v, reinterpret_cast<const void*>(at), 4); return v; }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    switch (c.fn) {
    case 0: {   // CPatrolRoutePool::New: el pool (arena A) apunta a los estados (arena B)
        arenaA.Put32(a[0] + 0, a[6]); arenaA.Put32(a[0] + 4, u32(arenaB.Obj(a[3]))); arenaA.Put32(a[0] + 8, a[4]); arenaA.Put32(a[0] + 0xC, a[5]);
        memcpy(reinterpret_cast<void*>(arenaB.Obj(a[3])), c.s.data(), c.s.size());
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = 0; call.popBytes = 0;
        break;
    }
    case 1:     // GetFormationOffset: ECX = base cruda (c.ecx), dx y dy en la pila (`ret 8`)
        call.args[0] = a[3]; call.args[1] = a[4]; call.argCount = 2; call.popBytes = 0;
        break;
    case 2:     // CScriptedEffectPairs::Reset
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = 0; call.popBytes = 0;
        break;
    case 3: {   // JPegScanlineToRaster: raster en A (cFormat en +0x23), cursor global hacia la arena B, scanline del EXE (0xBD0170) con relleno por semilla
        *reinterpret_cast<u8*>(arenaA.Obj(a[0]) + 0x23) = u8(a[7]);
        FillKind(reinterpret_cast<u8*>(kScanBase), kScanBytes, a[2], a[15]);
        PutGlobal32(kCursorAt, u32(arenaB.Obj(a[5])));
        call.args[0] = u32(arenaA.Obj(a[0])); call.args[1] = a[6]; call.args[2] = a[3]; call.args[3] = a[4]; call.argCount = 4; call.popBytes = 16;
        break;
    }
    case 4: {   // SunReflectionDistanceFactor: la camara esta en una matriz de la arena A (+0x30) o en el placement (0xB6F02C)
        if (a[9]) {
            PutGlobal32(kCamMatrixPtrAt, u32(arenaA.Obj(a[10])));
            arenaA.Put32(a[10] + 0x30, a[7]); arenaA.Put32(a[10] + 0x34, a[8]);
            PutGlobal32(kCamBase, a[12]); PutGlobal32(kCamBase + 4, a[13]);
        } else {
            PutGlobal32(kCamMatrixPtrAt, 0);
            PutGlobal32(kCamBase, a[7]); PutGlobal32(kCamBase + 4, a[8]);
        }
        call.args[0] = a[3]; call.args[1] = a[4]; call.args[2] = a[5]; call.args[3] = a[6]; call.argCount = 4; call.popBytes = 16;
        break;
    }
    case 5: {   // CDecision::Choices::Pick: opciones en A (tareas +0, acumuladas +0x18, contador +0x30), `slot` (word) en B e `index` (dword) en C
        memcpy(reinterpret_cast<void*>(arenaA.Obj(a[0])), c.s.data(), 48);
        arenaA.Put32(a[0] + 0x30, a[3]);
        gRet = a[5];
        call.ecx = u32(arenaA.Obj(a[0]));
        call.args[0] = a[4]; call.args[1] = u32(arenaB.Obj(a[6])); call.args[2] = u32(arenaC.Obj(a[7])); call.argCount = 3; call.popBytes = 0;
        break;
    }
    default: {  // Clones (6, 7, 8): el objeto en A; la memoria que devuelve el falso de operator new: nula, en B o en A (solapada con this); EAX de los falsos de constructor segun a[5]
        const u32 self = u32(arenaA.Obj(a[0]));
        if (c.fn == 6) {
            arenaA.Put32(a[0] + 0xC, a[7]); arenaA.Put32(a[0] + 0x10, a[8]); arenaA.Put32(a[0] + 0x14, a[9]); *reinterpret_cast<u8*>(arenaA.Obj(a[0]) + 0x18) = u8(a[16]);
        } else if (c.fn == 7) {
            arenaA.Put32(a[0] + 0x38, a[7]); arenaA.Put32(a[0] + 0x3C, a[8]); arenaA.Put32(a[0] + 0x40, a[9]); arenaA.Put32(a[0] + 0x44, a[10]); arenaA.Put32(a[0] + 0x48, a[11]);
            *reinterpret_cast<u8*>(arenaA.Obj(a[0]) + 0x58) = u8(a[16]);
        } else {
            arenaA.Put32(a[0] + 8, a[7]);
        }
        gAlloc = a[3] == 0 ? 0u : (a[3] == 1 ? u32(arenaB.Obj(a[4])) : u32(arenaA.Obj(a[0]) + a[4]));
        gCtorMode = a[5]; gCtorVal = a[6];
        call.ecx = self; call.argCount = 0; call.popBytes = 0;
        break;
    }
    }
    return call;
}

// Observa las regiones globales de la funcion: [[region, desplazamiento, byte nuevo]] respecto a la imagen tras preparar el caso (el cursor de JPeg se observa aparte, como etiqueta)
inline json GlobalChanges(const std::vector<std::vector<u8>>& before) {
    json out = json::array();
    const auto& idx = regionsOf[curFn];
    for (size_t k = 0; k < idx.size(); ++k) {
        if (idx[k] == kRegCursor) continue;
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
        gRet = 0; gAlloc = 0; gCtorMode = 0; gCtorVal = 0; gCalls = 0; gHash = 2166136261u;
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
        switch (fn) {
        case 0: case 1: obs["ret"] = eax; break;
        case 6: case 7: case 8: obs["ret"] = Tag(uintptr_t(eax)); break;
        default: obs["ret"] = nullptr; break;
        }
        if (fn == 2) obs["residue"] = json::array({sides[s].reg.eax, Tag(uintptr_t(sides[s].reg.ecx)), sides[s].reg.edx});   // el bucle de Init del EXE (0x6FA740) conserva ECX = this tras la llamada
        if (fn == 3) {
            obs["calls"] = json::array({gCalls, gHash});
            obs["cursor"] = Tag(uintptr_t(GetGlobal32(kCursorAt)));
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN132A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN132A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn132a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn132a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn132a"); }
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
    // Colaborador de la CRT del EXE: sin hook y sin funcion propia de la DLL (dll = 0: la DLL llama a la direccion del EXE); el falso va solo en la direccion del EXE
    auto X = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    env.stubs[3] = {F(0x803740, &FkRgbaToPixel)};
    env.stubs[5] = {X(0x821B1E, &FkRand)};
    env.stubs[6] = {F(0x61A5A0, &FkOpNew), F(0x683340, &FkWaitCtor)};
    env.stubs[7] = {F(0x61A5A0, &FkOpNew), F(0x684190, &FkPartnerCtor)};
    env.stubs[8] = {F(0x61A5A0, &FkOpNew), F(0x61A390, &FkSimpleCtor)};
    const char* dllReal[kFunctions] = {
        "toda la funcion (sin colaboradores)",
        "toda la funcion (sin colaboradores)",
        "toda la funcion (sin colaboradores)",
        "toda la funcion salvo RwRGBAToPixel (0x803740, con hook, falso identico); el buffer de la scanline y el cursor son memoria real del juego",
        "toda la funcion: la cuenta va en X87Math (SunReflectionFactorCpp); camara en memoria real del juego (matriz o placement)",
        "toda la funcion salvo rand (0x821B1E, CRT del EXE: falso solo en la direccion del EXE, la DLL lo llama por esa direccion)",
        "toda la funcion salvo CTask::operator new (0x61A5A0, con hook) y el constructor de CTaskComplexWaitForPed (0x683340, con hook): falsos identicos",
        "toda la funcion salvo CTask::operator new (0x61A5A0, con hook) y el constructor de CTaskComplexPartnerDeal (0x684190, con hook): falsos identicos",
        "toda la funcion salvo CTask::operator new (0x61A5A0, con hook) y el constructor de CTaskSimple (0x61A390, con hook): falsos identicos"};
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
    regions.emplace_back(kCursorAt, 4);
    regions.emplace_back(kScanBase, kScanBytes);
    regions.emplace_back(kCamBase, kCamBytes);
    regionsOf[3] = {kRegCursor, kRegScan};
    regionsOf[4] = {kRegCam};
    Run(env, ready, registered, bodies, collaborators);
}
}
