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
#include "Events/EventAttractor.h"
#include "Events/EventScriptedAttractor.h"
#include "AttractorScanner.h"
#include "Scripts/Scripted2dEffects.h"
#include "Tasks/TaskTypes/TaskSimpleGoTo.h"
#include "Entity/Ped/Ped.h"
#include "Timer.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn92C dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Funcion 0 (hook S): CAttractorScanner::ScanForAttractors 0x6060A0 (thiscall: ECX = escaner de 0x90 bytes, ped en pila, `ret 4`; void).
// Falsos identicos en la direccion del EXE y, cuando el colaborador tiene hook RH (apagado durante el banco), en el cuerpo de la DLL:
//   CTaskManager::GetActiveTask 0x681720 (hook), CEventGroup::Add 0x4AB420 (hook; devuelve un valor del caso), CEntity::RegisterReference 0x571B70 y CleanUpOldReference 0x571A00 (hooks; los piden el constructor y el
//   destructor de los eventos), y sin hook (la DLL los llama por su direccion del EXE): 0x6034B0 (explorador de la lista de un sector; thiscall, lista y ped en pila, `ret 8`), 0x5FFFD0 (explorador de un efecto; thiscall,
//   efecto, 0 y ped en pila, `ret 0Ch`), 0x5FE960 (la lista de usuarios admite el tipo de ped; thiscall, un argumento, `ret 4`, devuelve AL), 0x600180 (mejor candidato; thiscall, &efecto y &entidad en pila, `ret 8`) y
//   0x603570 (usuario del efecto; cdecl, un argumento, devuelve el ped).
// NO se sustituyen (corren de verdad en ambos lados): el temporizador en linea, CAttractorScanner::Clear 0x5FFF90 (hook activo en ambos lados, validado aparte), CTask::GetTaskType (arena de tarea con vtabla propia, ranura 4 con
// el tipo en +0x20), los constructores y destructores de CEventAttractor (0x4AF350, hook activo) y CEventScriptedAttractor frente al destructor del EXE 0x4AF400, y toda la aritmetica de la funcion (x87 del EXE frente al C++).
// Memoria real del juego (se rellena y se restaura tras cada lado): CTimer::m_snTimeInMilliseconds 0xB7CB84, CScripted2dEffects::ms_activated 0xC3A1A0 (64 bytes) y ms_userLists 0xC3A200 (64 * 0x24 bytes).

namespace F92CLayout {
static_assert(offsetof(CAttractorScanner, field_0) == 0 && offsetof(CAttractorScanner, field_4) == 4 && offsetof(CAttractorScanner, m_pEffectInUse) == 0x10 && offsetof(CAttractorScanner, field_14) == 0x14, "campos del escaner");
static_assert(offsetof(CAttractorScanner, field_18) == 0x18 && offsetof(CAttractorScanner, field_40) == 0x40 && offsetof(CAttractorScanner, field_68) == 0x68 && sizeof(CAttractorScanner) == 0x90, "tablas del escaner");
static_assert(offsetof(CTaskTimer, m_nStartTime) == 0 && offsetof(CTaskTimer, m_nInterval) == 4 && offsetof(CTaskTimer, m_bStarted) == 8 && offsetof(CTaskTimer, m_bStopped) == 9, "CTaskTimer");
static_assert(sizeof(tUserList) == 0x24 && offsetof(tUserList, m_bUseList) == 0x20, "lista de usuarios");
static_assert(sizeof(C2dEffect) == 0x40 && offsetof(C2dEffectPedAttractor, m_nAttractorType) == 0x34, "efecto atractor");
}

namespace F92CLive {
using namespace F92CFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x6060A0};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint16& G16(uintptr_t a) { return *reinterpret_cast<uint16*>(a); }
inline uint8& G8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F92CFix::Arena(WIN);
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
inline Arena<kTaskWin> actA;
inline Arena<kEffWin> effA;
inline Arena<kEntWin> entA;
// etiquetas de arenas (orden de TagAdd en CaseRun): 0 escaner, 1 ped, 2 matriz del ped, 3 inteligencia, 4 tarea activa, 5 efecto, 6 entidad
// valores crudos (no son arenas): efecto en uso "otro" 0x00BEEF00, dword +0x14 "otro" 0x00FEED00, entidad "otra" 0x00C0FFEE, usuario "otro" 0x00ABCD00
inline constexpr u32 kOtherInUse = 0x00BEEF00u, kOtherF14 = 0x00FEED00u, kOtherEntity = 0x00C0FFEEu, kOtherUser = 0x00ABCD00u;

// Estado del caso en curso para los falsos
struct State { const C* c{}; unsigned ulCalls{}; };
inline State g;
// Los punteros a la pila (temporales del EXE o de la DLL) no son comparables entre lados: se etiquetan todos igual
inline u32 TagS(uintptr_t v) {
    const u32 t = Tag(v);
    const uintptr_t lo = __readfsdword(8), hi = __readfsdword(4);
    if (t == u32(v) && v >= lo && v < hi) return 0x7F000000u;
    return t;
}

// Eventos: 2 GetActiveTask 0x681720 [taskmgr]; 4 GetTaskType virtual [tarea]; 5 CEventGroup::Add 0x4AB420 [grupo, tipo, +4, +8, +0xC, +0xE, +0x10, carga util (efecto, entidad, byte +0x1C), valido];
// 8 CEntity::RegisterReference 0x571B70 [entidad, ref]; 9 CEntity::CleanUpOldReference 0x571A00 [entidad, ref]; 21 0x6034B0 [escaner, lista, ped]; 22 0x5FFFD0 [escaner, efecto, cero, ped];
// 23 0x5FE960 [lista de usuarios, tipo de ped]; 24 0x600180 [escaner, &efecto, &entidad, *efecto, *entidad]; 25 0x603570 [efecto]; 90+k: ranura k no esperada de la vtabla de las tareas.
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
inline u32 __fastcall FkActive(void* self, void*) {                                                   // 0x681720
    Ev& e = PushEv(2, 1); e.v[0] = Tag(self);
    return g.c->act ? u32(actA.Obj()) : 0u;
}
inline u32 __fastcall FkAdd(void* self, void*, void* ev, u32 flag) {                                  // 0x4AB420 (ret 8)
    const auto* p = static_cast<const uint8*>(ev);
    const auto type = static_cast<CEvent*>(ev)->GetEventType();
    Ev& e = PushEv(5, 0); e.v[0] = Tag(self); e.v[1] = u32(type);
    e.v[2] = *reinterpret_cast<const u32*>(p + 4); e.v[3] = p[8]; e.v[4] = p[0xC]; e.v[5] = *reinterpret_cast<const uint16*>(p + 0xE); e.v[6] = *reinterpret_cast<const uint16*>(p + 0x10);
    u32 k = 7;
    if (type == EVENT_ATTRACTOR || type == EVENT_SCRIPTED_ATTRACTOR) {
        e.v[k++] = Tag(*reinterpret_cast<const uintptr_t*>(p + 0x14)); e.v[k++] = Tag(*reinterpret_cast<const uintptr_t*>(p + 0x18)); e.v[k++] = p[0x1C];
    }
    e.v[k++] = flag;
    e.len = k;
    return g.c->addRet;
}
inline void __fastcall FkRegister(void* self, void*, void* ref) {                                      // 0x571B70 (ret 4)
    Ev& e = PushEv(8, 2); e.v[0] = Tag(self); e.v[1] = TagS(uintptr_t(ref));
}
inline void __fastcall FkCleanUp(void* self, void*, void* ref) {                                       // 0x571A00 (ret 4)
    Ev& e = PushEv(9, 2); e.v[0] = Tag(self); e.v[1] = TagS(uintptr_t(ref));
}
inline void __fastcall FkScanSector(void* self, void*, void* list, void* ped) {                       // 0x6034B0 (ret 8)
    Ev& e = PushEv(21, 3); e.v[0] = Tag(self); e.v[1] = Tag(list); e.v[2] = Tag(ped);
}
inline void __fastcall FkScanEffect(void* self, void*, void* effect, u32 zero, void* ped) {           // 0x5FFFD0 (ret 0Ch)
    Ev& e = PushEv(22, 4); e.v[0] = Tag(self); e.v[1] = Tag(effect); e.v[2] = zero; e.v[3] = Tag(ped);
}
inline u32 __fastcall FkUserList(void* self, void*, u32 pedType) {                                    // 0x5FE960 (ret 4; devuelve AL)
    Ev& e = PushEv(23, 2); e.v[0] = Tag(self); e.v[1] = pedType;
    const u32 call = g.ulCalls++;
    return (g.c->uljunk << 8) | ((call < 32 ? (g.c->ulmask >> call) : 0u) & 1u);
}
inline void __fastcall FkBest(void* self, void*, void** effect, void** entity) {                      // 0x600180 (ret 8)
    Ev& e = PushEv(24, 5); e.v[0] = Tag(self); e.v[1] = TagS(uintptr_t(effect)); e.v[2] = TagS(uintptr_t(entity)); e.v[3] = u32(uintptr_t(*effect)); e.v[4] = u32(uintptr_t(*entity));
    *effect = g.c->resEffect ? reinterpret_cast<void*>(effA.Obj()) : nullptr;
    *entity = g.c->resEntity == 0 ? nullptr : (g.c->resEntity == 1 ? reinterpret_cast<void*>(entA.Obj()) : reinterpret_cast<void*>(uintptr_t(kOtherEntity)));
}
inline u32 __cdecl FkUser(void* effect) {                                                              // 0x603570 (cdecl, un argumento)
    Ev& e = PushEv(25, 1); e.v[0] = Tag(effect);
    return g.c->user == 0 ? u32(pedA.Obj()) : (g.c->user == 1 ? 0u : kOtherUser);
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
        scanA.Fill(sd++); pedA.Fill(sd++); pmatA.Fill(sd++); intelA.Fill(sd++); actA.Fill(sd++); effA.Fill(sd++); entA.Fill(sd++);
        TagClear();
        TagAdd(scanA.m.data(), scanA.m.size());     // 0
        TagAdd(pedA.m.data(), pedA.m.size());       // 1
        TagAdd(pmatA.m.data(), pmatA.m.size());     // 2
        TagAdd(intelA.m.data(), intelA.m.size());   // 3
        TagAdd(actA.m.data(), actA.m.size());       // 4
        TagAdd(effA.m.data(), effA.m.size());       // 5
        TagAdd(entA.m.data(), entA.m.size());       // 6
        // escaner: bandera, temporizador, efecto en uso, dword +0x14 y las tres tablas
        scanA.Put32(0, c.s0); scanA.Put32(4, c.scan[0]); scanA.Put32(8, c.scan[1]); scanA.Put32(0xC, c.scan[2]);
        scanA.Put32(0x10, c.inuse == 0 ? 0u : (c.inuse == 1 ? u32(effA.Obj()) : kOtherInUse));
        scanA.Put32(0x14, c.f14 == 0 ? 0u : (c.f14 == 1 ? u32(entA.Obj()) : kOtherF14));
        for (unsigned k = 0; k < 30; ++k) scanA.Put32(0x18 + 4 * k, c.arr[k]);
        G32(kTimeAddr) = c.now;
        // ped: posicion en la matriz o en el placement, banderas, modelo (word +0x22), inteligencia y tipo de ped
        pedA.Put32(0x14, c.pmat ? u32(pmatA.Obj()) : 0u);
        if (c.pmat) { Put3(pmatA, 0x30, c.ppos); Put3(pedA, 4, c.palt); }
        else { Put3(pedA, 4, c.ppos); Put3(pmatA, 0x30, c.palt); }
        pedA.Put32(0x46C, c.pflags);
        pedA.Put16(0x22, c.pmodel);
        pedA.Put32(0x47C, u32(intelA.Obj()));
        pedA.Put32(0x598, c.ptype);
        // tarea activa (vtabla propia, ranura 4 = tipo en +0x20) y el efecto que devuelve el falso (byte de tipo en +0x34)
        actA.Put32(0, u32(Addr(taskVt.data()))); actA.Put32(0x20, c.acttype);
        effA.Put32(0x34, c.efType);
        // memoria real del juego: efectos activados y listas de usuarios
        for (unsigned i = 0; i < kEffects; ++i) {
            G8(kActivatedAddr + i) = uint8(c.actv[i]);
            const uintptr_t ul = kUserListsAddr + 0x24 * i;
            memset(reinterpret_cast<void*>(ul), 0, 0x24);
            if (c.actv[i] & 0xFFu) {
                for (unsigned k = 0; k < 4; ++k) G32(ul + 4 * k) = c.ut[i][k];
                G32(ul + 0x20) = c.uf[i];
            }
        }
        std::vector<u32> args = { u32(pedA.Obj()) };
        scanA.Snap(); pedA.Snap(); pmatA.Snap(); intelA.Snap(); actA.Snap(); effA.Snap(); entA.Snap();
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, u32(scanA.Obj()), c.edx, c.stack, 0); });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, scanA.Changes(0x10, 0x18)); add(1, pedA.Changes()); add(2, pmatA.Changes()); add(3, intelA.Changes()); add(4, actA.Changes()); add(5, effA.Changes()); add(6, entA.Changes());
        obs["changes"] = ch;
        // +0x10 (efecto en uso) y +0x14 (dword): se observan como etiquetas de arena (sus bytes no se comparan por direccion)
        obs["ptrs"] = json::array({Tag(uintptr_t(scanA.Get32(0x10))), Tag(uintptr_t(scanA.Get32(0x14)))});
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
    if (!std::filesystem::exists(dir / "fn92c-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn92c-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn92c"); }
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
    env.stubs[0] = {F(0x681720, &FkActive), F(0x6034B0, &FkScanSector), F(0x5FFFD0, &FkScanEffect), F(0x5FE960, &FkUserList), F(0x600180, &FkBest), F(0x603570, &FkUser),
                    F(0x4AB420, &FkAdd), F(0x571B70, &FkRegister), F(0x571A00, &FkCleanUp)};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"temporizador en linea del escaner (CTaskTimer: arrancado, detenido, inicio, intervalo, hora actual del juego), CAttractorScanner::Clear (hook activo en ambos lados), CTask::GetTaskType virtual, constructores y destructores de los eventos de atractor"})},
                                 {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
