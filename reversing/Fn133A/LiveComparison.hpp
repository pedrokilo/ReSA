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

// Banco par a par Fn133A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Seis funciones del EXE de formas variadas (ver Fixtures.hpp para la lista y el esquema de familias), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedGlobalInstall /
// RH_ScopedOverloadedInstall: enlace de la entrada del EXE hacia la DLL). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40) y, donde hace falta,
// memoria global real del juego (se guarda al empezar y se restaura tras cada lado). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair);
// las rutinas de RenderWare (0x7F2D20, 0x7F2CF0, 0x7EE410) llevan el falso solo en la direccion del EXE (la DLL las llama por esa direccion). Los constructores de CTaskSimpleStandStill (0x62F310) y de
// CTaskComplexSmartFleePoint (0x65BD20) corren REALES en ambos lados (con su hook, como en el ASI normal) y se observan los bytes del objeto creado; CVector2D::Normalise (0x44E480) corre real en el
// EXE y la DLL lo lleva en linea (X87Math.cpp), asi que se compara contra el original.
// Se observan: los bytes cambiados de las tres arenas y de la memoria global, los eventos de los falsos (con sus argumentos), el valor devuelto (cuando el EXE lo define) y el estado x87
// (debe quedar vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F133ALive {
using namespace F133AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x5A7A20, 0x72FC70, 0x694850, 0x616920, 0x65C530, 0x477DA0};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F133AFix::Arena(WIN);
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
enum : unsigned { kRegRs, kRegTimer, kRegDbg };
inline constexpr uintptr_t kRsAt = 0xC17044, kTimerAt = 0xB7CB84, kDbgAt = 0x859CF8;

// ================================================================================================ falsos de los colaboradores
inline DWORD gBenchThread{};
// estado de los falsos (se rellena en Setup)
inline u32 gModeIdx{}, gVmi[6]{};
inline u32 gPvlRet[2]{}, gPvlZ[2]{}, gPvlCall{};
inline u32 gBlockOff{}, gNewNull{}, gRand{}, gAngleBits{};

// Lee un dword solo si el puntero cae dentro de una arena registrada
inline u32 SafeBits(const void* p) {
    for (const auto& t : tagTable) if (uintptr_t(p) >= t.base && uintptr_t(p) - t.base + 4 <= t.size) return BitsOf(p);
    return 0xDEAD0000u;
}
// RwEngineGetCurrentVideoMode (0x7F2D20, cdecl, sin argumentos): evento [direccion]; devuelve el indice del caso
inline int __cdecl FkCurMode() { PushEv(0x7F2D20, 0); return int(gModeIdx); }
// RwEngineGetVideoModeInfo (0x7F2CF0, cdecl, RwVideoMode* e indice): evento [direccion, indice]; rellena los seis dwords (ancho, alto, profundidad, banderas, frecuencia, formato); devuelve el puntero
inline void* __cdecl FkModeInfo(u32* out, u32 idx) {
    Ev& e = PushEv(0x7F2CF0, 1); e.v[0] = idx;
    for (int i = 0; i < 6; ++i) out[i] = gVmi[i];
    return out;
}
// RwCameraSetViewWindow (0x7EE410, cdecl, camara y RwV2d*): evento [direccion, camara (etiqueta), bits de x, bits de y]; devuelve la camara
inline void* __cdecl FkSetViewWindow(void* camera, const u32* vw) { Ev& e = PushEv(0x7EE410, 3); e.v[0] = Tag(camera); e.v[1] = vw[0]; e.v[2] = vw[1]; return camera; }
// CWorld::ProcessVerticalLine (0x5674E0, cdecl, 11 dwords): evento [direccion, bits del origen (3), bits de la distancia, los seis bools y el ultimo puntero]; la i-esima llamada devuelve EAX = gPvlRet[i] y,
// si AL != 0, escribe gPvlZ[i] en la z del punto de colision (+8)
inline u32 __cdecl FkPvl(const u32* origin, u32 dist, u8* colPoint, void* hit, u32 b5, u32 b6, u32 b7, u32 b8, u32 b9, u32 b10, u32 last) {
    (void)hit;
    Ev& e = PushEv(0x5674E0, 11);
    e.v[0] = origin[0]; e.v[1] = origin[1]; e.v[2] = origin[2]; e.v[3] = dist;
    e.v[4] = b5; e.v[5] = b6; e.v[6] = b7; e.v[7] = b8; e.v[8] = b9; e.v[9] = b10; e.v[10] = last;
    const unsigned i = gPvlCall < 2 ? gPvlCall : 1;
    ++gPvlCall;
    if (gPvlRet[i] & 0xFFu) memcpy(colPoint + 8, &gPvlZ[i], 4);
    return gPvlRet[i];
}
// CTask::operator new (0x61A5A0, cdecl, tamano): evento [direccion, tamano]; devuelve el bloque del caso (arena B) o nulo
inline void* __cdecl FkNew(u32 size) { Ev& e = PushEv(0x61A5A0, 1); e.v[0] = size; return gNewNull ? nullptr : reinterpret_cast<void*>(arenaB.Obj(gBlockOff)); }
// rand() del CRT del EXE (0x821B1E) y CGeneral::GetRandomNumber de la DLL: evento [direccion]; devuelve el valor del caso (el EXE enmascara con 0xFFFF; la DLL usa AX)
inline u32 __cdecl FkRand() { PushEv(0x821B1E, 0); return gRand; }
// CGeneral::GetRadianAngleBetweenPoints (0x53CBE0, cdecl, cuatro floats): evento [direccion, bits de x1, y1, x2, y2]; devuelve el angulo del caso en ST0
inline float __cdecl FkAngle(u32 x1, u32 y1, u32 x2, u32 y2) {
    Ev& e = PushEv(0x53CBE0, 4); e.v[0] = x1; e.v[1] = y1; e.v[2] = x2; e.v[3] = y2;
    return std::bit_cast<float>(gAngleBits);
}
// CTheScripts::DrawDebugAngledSquare (0x486990, cdecl, ocho floats): evento [direccion, ocho dwords crudos]
inline void __cdecl FkDraw(u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8) {
    Ev& e = PushEv(0x486990, 8);
    e.v[0] = a1; e.v[1] = a2; e.v[2] = a3; e.v[3] = a4; e.v[4] = a5; e.v[5] = a6; e.v[6] = a7; e.v[7] = a8;
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
};

inline void PutGlobal32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[20]); arenaB.Pattern(a[2], a[21]); arenaC.Pattern(a[2], a[22]);
    gPvlCall = 0; gNewNull = 0; gRand = 0; gAngleBits = 0; gBlockOff = 0; gModeIdx = 0;
    switch (c.fn) {
    case 0: {   // GetClothesModelFromName
        u8* const dst = arenaC.m.data() + kGuard + a[0];
        memcpy(dst, c.s.data(), c.s.size()); dst[c.s.size()] = 0;
        call.args[0] = u32(arenaC.Obj(a[0])); call.argCount = 1; call.popBytes = 4;
        break;
    }
    case 1: {   // CameraSize
        arenaA.Put32(a[0] + 0x60, u32(arenaB.Obj(a[3])));
        arenaA.Put32(a[0] + 0x64, a[5] ? u32(arenaB.Obj(a[3])) : u32(arenaC.Obj(a[4])));
        arenaB.Put32(a[3] + 0xC, a[8]); arenaB.Put32(a[3] + 0x10, a[9]);
        if (!a[5]) { arenaC.Put32(a[4] + 0xC, a[18]); arenaC.Put32(a[4] + 0x10, a[19]); }
        arenaA.Put32(a[6] + 0, Rand(a[23], 1)); arenaA.Put32(a[6] + 4, Rand(a[23], 2)); arenaA.Put32(a[6] + 8, a[10]); arenaA.Put32(a[6] + 12, a[11]);
        FillKind(reinterpret_cast<u8*>(kRsAt), 8, a[2], a[23]);
        gModeIdx = a[15];
        gVmi[0] = a[12]; gVmi[1] = a[13]; gVmi[2] = Rand(a[23], 5); gVmi[3] = a[14]; gVmi[4] = Rand(a[23], 6); gVmi[5] = Rand(a[23], 7);
        call.args[0] = u32(arenaA.Obj(a[0])); call.args[1] = a[7] ? u32(arenaA.Obj(a[6])) : 0u; call.args[2] = a[16]; call.args[3] = a[17];
        call.argCount = 4; call.popBytes = 16;
        break;
    }
    case 2:     // Offsets::Constructor
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = 0; call.popBytes = 0;
        break;
    case 3: {   // FindZCoorForPed
        arenaA.Put32(a[0], a[3]); arenaA.Put32(a[0] + 4, a[4]); arenaA.Put32(a[0] + 8, a[5]);
        gPvlRet[0] = a[6]; gPvlZ[0] = a[7]; gPvlRet[1] = a[8]; gPvlZ[1] = a[9];
        call.args[0] = u32(arenaA.Obj(a[0])); call.argCount = 1; call.popBytes = 4;
        break;
    }
    case 4: {   // CreateSubTask
        const u32 entity = u32(arenaB.Obj(a[3]));
        arenaA.Put32(a[0] + 0xC, entity);
        arenaA.Put32(a[0] + 0x1C, a[7]); arenaA.Put32(a[0] + 0x20, a[8]); arenaA.Put32(a[0] + 0x24, a[9]); arenaA.Put32(a[0] + 0x28, a[10]); arenaA.Put32(a[0] + 0x2C, a[11]);
        if (a[5]) {
            arenaB.Put32(a[3] + 0x14, u32(arenaC.Obj(a[4])));
            arenaC.Put32(a[4] + 0x30, a[16]); arenaC.Put32(a[4] + 0x34, a[17]); arenaC.Put32(a[4] + 0x38, a[18]);
        } else {
            arenaB.Put32(a[3] + 0x14, 0);
            arenaB.Put32(a[3] + 4, a[16]); arenaB.Put32(a[3] + 8, a[17]); arenaB.Put32(a[3] + 12, a[18]);
        }
        PutGlobal32(kTimerAt, a[12]);
        gRand = a[13]; gBlockOff = a[14]; gNewNull = a[15];
        call.ecx = u32(arenaA.Obj(a[0]));
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        break;
    }
    default: {  // IsPointInAngledArea
        arenaA.Put32(a[0], a[3]); arenaA.Put32(a[0] + 4, a[4]);
        reinterpret_cast<u8*>(kDbgAt)[0] = u8(a[11]);
        gAngleBits = a[10];
        call.args[0] = u32(arenaA.Obj(a[0])); call.args[1] = a[5]; call.args[2] = a[6]; call.args[3] = a[7]; call.args[4] = a[8]; call.args[5] = a[9];
        call.argCount = 6; call.popBytes = 24;
        break;
    }
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
        case 0: obs["ret"] = eax; break;
        case 2: case 4: obs["ret"] = Tag(uintptr_t(eax)); break;
        case 3: case 5: obs["ret"] = eax & 0xFFu; break;
        default: obs["ret"] = nullptr; break;
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN133A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN133A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn133a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn133a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn133a"); }
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
    // Colaborador de la CRT del EXE: sin hook y sin funcion propia de la DLL (dll = 0: la DLL llama a la direccion del EXE); el falso va solo en la direccion del EXE
    auto X = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    env.stubs[1] = {X(0x7F2D20, &FkCurMode), X(0x7F2CF0, &FkModeInfo), X(0x7EE410, &FkSetViewWindow)};
    env.stubs[3] = {F(0x5674E0, &FkPvl)};
    env.stubs[4] = {F(0x61A5A0, &FkNew), U(0x821B1E, std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber), &FkRand)};
    env.stubs[5] = {F(0x53CBE0, &FkAngle), F(0x486990, &FkDraw)};
    env.reals[5] = {Pair{0x44E480, 0, false}};
    const char* dllReal[kFunctions] = {
        "toda la funcion (sin colaboradores)",
        "toda la funcion salvo las tres rutinas de RenderWare (0x7F2D20, 0x7F2CF0, 0x7EE410: falsos solo en la direccion del EXE; la DLL las llama por esa direccion); la ventana de vista sale de CameraSizeWindowCpp (X87Math.cpp)",
        "toda la funcion (sin colaboradores)",
        "toda la funcion salvo CWorld::ProcessVerticalLine (0x5674E0, con hook, falso identico)",
        "toda la funcion salvo CTask::operator new (0x61A5A0, con hook, falso identico) y CGeneral::GetRandomNumber (funcion propia de la DLL sin hook; rand() 0x821B1E del EXE: falso identico); los constructores de CTaskSimpleStandStill y CTaskComplexSmartFleePoint corren reales en ambos lados",
        "toda la funcion salvo CGeneral::GetRadianAngleBetweenPoints (0x53CBE0) y CTheScripts::DrawDebugAngledSquare (0x486990), ambos con hook y falso identico; CVector2D::Normalise (0x44E480) corre real en el EXE y va en linea en la DLL (AngledAreaCpp)"};
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
    regions.emplace_back(kRsAt, 8);
    regions.emplace_back(kTimerAt, 4);
    regions.emplace_back(kDbgAt, 1);
    regionsOf[1] = {kRegRs};
    regionsOf[4] = {kRegTimer};
    regionsOf[5] = {kRegDbg};
    Run(env, ready, registered, bodies, collaborators);
}
}
