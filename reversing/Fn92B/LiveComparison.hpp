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
#include "Events/EventScanner.h"
#include "Events/Event.h"
#include "Events/EventPotentialWalkIntoObject.h"
#include "Tasks/TaskTypes/TaskSimpleGoTo.h"
#include "EntityScanner.h"
#include "ObjectScanner.h"
#include "Entity/Object/Object.h"
#include "Models/ModelInfo.h"
#include "Models/BaseModelInfo.h"
#include "Collision/ColModel.h"
#include "Entity/Ped/Ped.h"
#include "Timer.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn92B dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Funcion 0 (hook S): CObjectPotentialCollisionScanner::ScanForObjectPotentialCollisionEvents 0x606890 (thiscall: ECX = escaner (CTaskTimer de 0xC bytes), ped en pila, `ret 4`; void).
// Falsos identicos en la direccion del EXE y, cuando el colaborador tiene hook RH (apagado durante el banco) o cuerpo C++ propio, en el cuerpo de la DLL:
//   CTaskManager::GetSimplestActiveTask 0x6819D0 (dos consultas: estado de movimiento y punto de destino), CObjectScanner::ScanForObjectsInRange 0x5FFF30 (sin hook; la DLL tiene cuerpo C++ propio
//   y su cuerpo recibe el mismo falso: escribe el objeto mas cercano en +0x4C del escaner temporal), CEntityScanner::Clear 0x5FF9D0 (hook; lo piden el destructor de la DLL y la salida del EXE),
//   CEntity::GetBoundCentre 0x534250 (hook "vec"; devuelve el centro por el puntero oculto), CPedGeometryAnalyser::GetIsLineOfSightClear 0x5F5A30 (ped; escribe la distancia) y 0x5F2F00 (v3d),
//   CEventGroup::Add 0x4AB420, CEntity::RegisterReference 0x571B70 y CleanUpOldReference 0x571A00 (los pide el constructor/destructor de CEventPotentialWalkIntoObject 0x4AE5D0/0x4AE670).
// NO se sustituyen (corren de verdad en ambos lados): el temporizador, CPed::IsPlayer 0x5DF8F0, CTask::IsGoToTask 0x61A360 y los GetTaskType virtuales (arenas de tarea con vtabla propia, ranura 4 con el tipo en +0x20),
// el constructor del escaner (0x606010 frente al C++ de la DLL, sin efectos observables fuera de la pila), los constructores y destructores del evento y toda la aritmetica de la funcion (x87 del EXE frente al C++ de la DLL).
// La tabla de punteros de modelos 0xA9B0C8: cada caso apunta la ranura del indice de modelo del objeto (con signo, movsx) a una arena de informacion de modelo cuyo +0x14 apunta a una colision con el radio de la esfera delimitadora en +0x24.
// Memoria real del juego (se rellena y se restaura tras cada lado): CTimer::m_snTimeInMilliseconds 0xB7CB84 y las ocho ranuras de la tabla de modelos elegidas por kModelIdx.

namespace F92BLayout {
static_assert(offsetof(CObjectPotentialCollisionScanner, m_timer) == 0 && sizeof(CObjectPotentialCollisionScanner) == 0xC, "campos del escaner");
static_assert(offsetof(CTaskTimer, m_nStartTime) == 0 && offsetof(CTaskTimer, m_nInterval) == 4 && offsetof(CTaskTimer, m_bStarted) == 8 && offsetof(CTaskTimer, m_bStopped) == 9, "CTaskTimer");
static_assert(sizeof(CObjectScanner) == 0x50, "CObjectScanner mide 0x50");
static_assert(offsetof(CObject, m_nObjectFlags) == 0x140, "banderas del objeto");
static_assert(offsetof(CEntity, m_nModelIndex) == 0x22, "indice de modelo");
static_assert(offsetof(CBaseModelInfo, m_pColModel) == 0x14, "colision del modelo");
static_assert(offsetof(CColModel, m_boundSphere) == 0x18 && offsetof(CSphere, m_fRadius) == 0xC, "radio de la esfera delimitadora de la colision (+0x24)");
static_assert(offsetof(CTaskSimpleGoTo, m_moveState) == 8 && offsetof(CTaskSimpleGoTo, m_vecTargetPoint) == 0xC, "tarea de ir a un punto");
}

namespace F92BLive {
using namespace F92BFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x606890};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint16& G16(uintptr_t a) { return *reinterpret_cast<uint16*>(a); }
inline uint8& G8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F92BFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { const uint16 w = uint16(v); memcpy(m.data() + kGuard + off, &w, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia, sin los bytes de hasta dos campos de puntero [lo, hi) (cambian por direccion)
    json Changes(size_t lo1 = 0, size_t hi1 = 0, size_t lo2 = 0, size_t hi2 = 0) const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            const int64 off = int64(i) - int64(kGuard);
            const bool masked = (hi1 && off >= int64(lo1) && off < int64(hi1)) || (hi2 && off >= int64(lo2) && off < int64(hi2));
            if (m[i] != r[i] && !masked) out.push_back(json::array({off, m[i]}));
        }
        return out;
    }
};
inline Arena<kScanWin> scanA;
inline Arena<kPedWin> pedA;
inline Arena<kMatWin> pmatA;
inline Arena<kIntelWin> intelA;
inline Arena<kTaskWin> t1A, t2A;
inline Arena<kObjWin> objA;
inline Arena<kMatWin> omatA;
inline Arena<kModelWin> minfoA;
inline Arena<kColWin> colA;
// etiquetas de arenas (orden de TagAdd en CaseRun): 0 escaner, 1 ped, 2 matriz del ped, 3 inteligencia, 4/5 tareas, 6 objeto, 7 matriz del objeto, 8 informacion de modelo, 9 colision

// Estado del caso en curso para los falsos
struct State { const C* c{}; unsigned simplestCalls{}; };
inline State g;
// Los punteros a la pila (temporales del EXE o de la DLL) no son comparables entre lados: se etiquetan todos igual
inline u32 TagS(uintptr_t v) {
    const u32 t = Tag(v);
    const uintptr_t lo = __readfsdword(8), hi = __readfsdword(4);
    if (t == u32(v) && v >= lo && v < hi) return 0x7F000000u;
    return t;
}
inline void Vec3(Ev& e, u32 at, const void* p) { memcpy(&e.v[at], p, 12); }

// Eventos: 1 GetSimplestActiveTask 0x6819D0 [taskmgr]; 4 GetTaskType virtual [tarea]; 5 CEventGroup::Add 0x4AB420 [grupo, tipo, +4, +8, +0xC, +0xE, +0x10, carga util..., valido];
// 8 CEntity::RegisterReference 0x571B70 [entidad, ref]; 9 CEntity::CleanUpOldReference 0x571A00 [entidad, ref]; 10 CObjectScanner::ScanForObjectsInRange 0x5FFF30 [escaner, ped];
// 11 CEntityScanner::Clear 0x5FF9D0 [escaner]; 12 CEntity::GetBoundCentre 0x534250 [objeto, destino]; 13 GetIsLineOfSightClear 0x5F5A30 [ped, destino, destino.xyz, objeto, &distancia, *distancia];
// 14 GetIsLineOfSightClear 0x5F2F00 [desde, hasta, desde.xyz, hasta.xyz, objeto]; 90+k: ranura k no esperada de la vtabla de las tareas.
inline std::array<uintptr_t, 16> taskVt{};
template <unsigned K> inline u32 __fastcall FkVt(void* self, void*) { Ev& e = PushEv(90 + K, 1); e.v[0] = Tag(self); return 0; }
inline u32 __fastcall FkTaskType(void* self, void*) {                                                 // CTask::GetTaskType (ranura 4, `call [eax+10h]`)
    Ev& e = PushEv(4, 1); e.v[0] = Tag(self);
    return *reinterpret_cast<u32*>(uintptr_t(self) + 0x20);
}
inline void InitTaskVt() {
    [&]<unsigned... I>(std::integer_sequence<unsigned, I...>) { ((taskVt[I] = uintptr_t(&FkVt<I>)), ...); }(std::make_integer_sequence<unsigned, 16>{});
    taskVt[4] = uintptr_t(&FkTaskType);
}
inline u32 __fastcall FkSimplest(void* self, void*) {                                                 // 0x6819D0
    Ev& e = PushEv(1, 1); e.v[0] = Tag(self);
    const bool first = g.simplestCalls++ == 0;
    if (first) return g.c->t1 ? u32(t1A.Obj()) : 0u;
    return g.c->t2 ? u32(t2A.Obj()) : 0u;
}
inline u32 __fastcall FkAdd(void* self, void*, void* ev, u32 flag) {                                  // 0x4AB420 (ret 8)
    const auto* p = static_cast<const uint8*>(ev);
    const auto type = static_cast<CEvent*>(ev)->GetEventType();
    Ev& e = PushEv(5, 0); e.v[0] = Tag(self); e.v[1] = u32(type);
    e.v[2] = *reinterpret_cast<const u32*>(p + 4); e.v[3] = p[8]; e.v[4] = p[0xC]; e.v[5] = *reinterpret_cast<const uint16*>(p + 0xE); e.v[6] = *reinterpret_cast<const uint16*>(p + 0x10);
    u32 k = 7;
    if (type == EVENT_POTENTIAL_WALK_INTO_OBJECT) { e.v[k++] = Tag(*reinterpret_cast<const uintptr_t*>(p + 0x14)); e.v[k++] = *reinterpret_cast<const u32*>(p + 0x18); }
    e.v[k++] = flag;
    e.len = k;
    return 0;
}
inline void __fastcall FkRegister(void* self, void*, void* ref) {                                      // 0x571B70 (ret 4)
    Ev& e = PushEv(8, 2); e.v[0] = Tag(self); e.v[1] = TagS(uintptr_t(ref));
}
inline void __fastcall FkCleanUp(void* self, void*, void* ref) {                                       // 0x571A00 (ret 4)
    Ev& e = PushEv(9, 2); e.v[0] = Tag(self); e.v[1] = TagS(uintptr_t(ref));
}
inline void __fastcall FkObjScan(void* self, void*, void* ped) {                                       // 0x5FFF30 (ret 4): deja el objeto mas cercano en +0x4C
    Ev& e = PushEv(10, 2); e.v[0] = TagS(uintptr_t(self)); e.v[1] = Tag(ped);
    *reinterpret_cast<u32*>(uintptr_t(self) + 0x4C) = g.c->oPresent ? u32(objA.Obj()) : 0u;
}
inline void __fastcall FkClear(void* self, void*) {                                                    // 0x5FF9D0 (ret)
    Ev& e = PushEv(11, 1); e.v[0] = TagS(uintptr_t(self));
}
inline u32 __fastcall FkCentre(void* self, void*, float* out) {                                        // 0x534250 (ret 4; devuelve el destino)
    Ev& e = PushEv(12, 2); e.v[0] = Tag(self); e.v[1] = TagS(uintptr_t(out));
    out[0] = F(g.c->cz[0]); out[1] = F(g.c->cz[1]); out[2] = F(g.c->cz[2]);
    return u32(uintptr_t(out));
}
inline u32 __cdecl FkLos1(void* ped, float* target, void* object, float* dist) {                       // 0x5F5A30 (cdecl, 4 argumentos)
    Ev& e = PushEv(13, 8); e.v[0] = Tag(ped); e.v[1] = Tag(target); Vec3(e, 2, target); e.v[5] = Tag(object); e.v[6] = TagS(uintptr_t(dist)); e.v[7] = BitsOf(dist);
    *reinterpret_cast<u32*>(dist) = g.c->dist;
    return ((g.c->los1junk << 8) | (g.c->los1 & 1u));
}
inline u32 __cdecl FkLos2(float* from, float* to, void* object) {                                      // 0x5F2F00 (cdecl, 3 argumentos)
    Ev& e = PushEv(14, 9); e.v[0] = TagS(uintptr_t(from)); e.v[1] = Tag(to); Vec3(e, 2, from); Vec3(e, 5, to); e.v[8] = Tag(object);
    return ((g.c->los2junk << 8) | (g.c->los2 & 1u));
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

inline void Put3(auto& a, size_t off, const u32* p) { a.Put32(off, p[0]); a.Put32(off + 4, p[1]); a.Put32(off + 8, p[2]); }

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    InitTaskVt();
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 50u;
        scanA.Fill(sd++); pedA.Fill(sd++); pmatA.Fill(sd++); intelA.Fill(sd++); t1A.Fill(sd++); t2A.Fill(sd++); objA.Fill(sd++); omatA.Fill(sd++); minfoA.Fill(sd++); colA.Fill(sd++);
        TagClear();
        TagAdd(scanA.m.data(), scanA.m.size());     // 0
        TagAdd(pedA.m.data(), pedA.m.size());       // 1
        TagAdd(pmatA.m.data(), pmatA.m.size());     // 2
        TagAdd(intelA.m.data(), intelA.m.size());   // 3
        TagAdd(t1A.m.data(), t1A.m.size());         // 4
        TagAdd(t2A.m.data(), t2A.m.size());         // 5
        TagAdd(objA.m.data(), objA.m.size());       // 6
        TagAdd(omatA.m.data(), omatA.m.size());     // 7
        TagAdd(minfoA.m.data(), minfoA.m.size());   // 8
        TagAdd(colA.m.data(), colA.m.size());       // 9
        for (unsigned k = 0; k < 4; ++k) scanA.Put32(4 * k, c.scan[k]);
        G32(kTimeAddr) = c.now;
        // ped: posicion en la matriz o en el placement, inteligencia y tipo de ped
        pedA.Put32(0x14, c.pmat ? u32(pmatA.Obj()) : 0u);
        if (c.pmat) { Put3(pmatA, 0x30, c.ppos); Put3(pedA, 4, c.palt); }
        else { Put3(pedA, 4, c.ppos); Put3(pmatA, 0x30, c.palt); }
        pedA.Put32(0x47C, u32(intelA.Obj()));
        pedA.Put32(0x598, c.ptype);
        // tareas: vtabla propia (ranura 4 = tipo en +0x20), estado de movimiento (+8) de la primera y punto de destino (+0xC) de la segunda
        for (auto* ta : {&t1A, &t2A}) ta->Put32(0, u32(Addr(taskVt.data())));
        t1A.Put32(8, c.t1move); t1A.Put32(0x20, c.t1type); t2A.Put32(0x20, c.t2type);
        Put3(t2A, 0xC, c.ttgt);
        // objeto: banderas, dword +0x1C, indice de modelo (la ranura de la tabla apunta a la arena de modelo), posicion
        objA.Put32(0x140, c.oflags);
        objA.Put32(0x1C, c.o1c);
        objA.Put16(0x22, kModelIdx[c.omodel % 8]);
        objA.Put32(0x14, c.ohasmat ? u32(omatA.Obj()) : 0u);
        if (c.ohasmat) { Put3(omatA, 0x30, c.opos); Put3(objA, 4, c.oalt); }
        else { Put3(objA, 4, c.opos); Put3(omatA, 0x30, c.oalt); }
        minfoA.Put32(0x14, u32(colA.Obj()));
        colA.Put32(0x24, c.radius);
        G32(ModelSlot(c.omodel)) = u32(minfoA.Obj());
        std::vector<u32> args = { u32(pedA.Obj()) };
        scanA.Snap(); pedA.Snap(); pmatA.Snap(); intelA.Snap(); t1A.Snap(); t2A.Snap(); objA.Snap(); omatA.Snap(); minfoA.Snap(); colA.Snap();
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, u32(scanA.Obj()), c.edx, c.stack, 0); });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, scanA.Changes()); add(1, pedA.Changes()); add(2, pmatA.Changes()); add(3, intelA.Changes()); add(4, t1A.Changes()); add(5, t2A.Changes());
        add(6, objA.Changes()); add(7, omatA.Changes()); add(8, minfoA.Changes()); add(9, colA.Changes());
        obs["changes"] = ch;
        obs["events"] = EvsJson();
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
    }
    for (const auto& s : env.dllOnly) ready &= s.pair.dll != 0 && ModuleOf(s.pair.dll) == env.asi;
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
    if (!std::filesystem::exists(dir / "fn92b-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn92b-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn92b"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de la funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
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
    // CObjectScanner::ScanForObjectsInRange 0x5FFF30 no tiene hook RH: la DLL llama a su cuerpo C++ propio, que recibe el mismo falso que la entrada del EXE
    uintptr_t scanBody = 0;
    { auto pm = &CObjectScanner::ScanForObjectsInRange; static_assert(sizeof(pm) == sizeof(scanBody)); memcpy(&scanBody, &pm, sizeof(scanBody)); scanBody = Body(scanBody); }
    env.stubs[0] = {F(0x6819D0, &FkSimplest), S(Pair{0x5FFF30, scanBody, false}, reinterpret_cast<void*>(&FkObjScan)), F(0x5FF9D0, &FkClear), F(0x534250, &FkCentre),
                    F(0x5F5A30, &FkLos1), F(0x5F2F00, &FkLos2), F(0x4AB420, &FkAdd), F(0x571B70, &FkRegister), F(0x571A00, &FkCleanUp)};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"temporizador en linea del escaner (CTaskTimer: arrancado, detenido, inicio, intervalo, hora actual del juego), CPed::IsPlayer, CTask::IsGoToTask con las llamadas virtuales GetTaskType, constructor del escaner y constructor/destructor del evento"})},
                                 {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
