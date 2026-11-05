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
#include "Events/EventGroupEvent.h"
#include "Events/EventDamage.h"
#include "Pools/EventPool.h"
#include "Pools/ObjectPool.h"
#include "Pools/VehiclePool.h"
#include "WaterLevel.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn100A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Nueve funciones (hook S), cada una con 16384 pares EXE/DLL:
//   0  CTask::IsGoToTask                 0x61A360  cdecl (tarea), bool en AL. Sin colaboradores: GetTaskType (ranura 4 de una vtabla propia en una arena, falsa) se llama una o dos veces.
//   1  CMemoryMgr::Free                  0x72F430  cdecl (puntero). Colaborador SIN hook RH: free del CRT del EXE (0x82413F, cdecl): falso en esa direccion (los hilos ajenos pasan por un trampolin al original).
//   2  CEvent::operator new              0x4B5620  cdecl (tamano). Pool de eventos (0xB744AC) FALSO en arenas (cabecera de 0x14 bytes, estados, almacenamiento); corre de verdad CEventPool::New (0x4B5570, con hook RH apagado: el EXE corre sus bytes).
//   3  CObject::operator new             0x5A1EE0  cdecl (tamano). Igual con el pool de objetos (0xB7449C) y 0x5A1C20.
//   4  CVehicle::operator new(tam, ref)  0x6E2D60  cdecl (tamano, ref). Igual con el pool de vehiculos (0xB74494); el EXE corre 0x6D5DD0 (sin hook) y la DLL el NewAt en linea.
//   5  CWaterLevel LerpRenPar            0x6E5280  cdecl (destino oculto, rp1, rp2, t), destino en EAX. Sin colaboradores (toda la aritmetica x87 corre de verdad en ambos lados).
//   6  CEventGroupEvent::CEventGroupEvent 0x4ADFD0 thiscall (ped, evento), `ret 8`, this en EAX. Colaborador (con hook RH): CEntity::RegisterReference 0x571B70 (falso identico en el EXE y en el cuerpo de la DLL).
//   7  CEventGroupEvent::~CEventGroupEvent 0x4AE070 thiscall. Colaboradores: CEntity::CleanUpOldReference 0x571A00 (falso identico) y la ranura 0 de la vtabla del evento (falsa, vtabla propia).
//   8  CEventDamage::~CEventDamage       0x4AD960  thiscall. Colaborador: 0x571A00 (falso identico).
// Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (los colaboradores con hook RH estan apagados durante el banco). Registran sus argumentos EN ORDEN y los punteros a arenas como etiquetas.
// NO se sustituyen (corren de verdad en ambos lados): toda la aritmetica de la funcion 5 y toda la logica de pool de las funciones 2, 3 y 4.
// Memoria real del juego (se rellena y se restaura tras cada lado): los tres punteros de pool (0xB744AC, 0xB7449C, 0xB74494), que durante cada llamada apuntan a una cabecera de pool falsa.

namespace F100ALayout {
VALIDATE_OFFSET(CEventGroupEvent, m_ped, 0xC);
VALIDATE_OFFSET(CEventGroupEvent, m_event, 0x10);
VALIDATE_OFFSET(CEventDamage, m_pSourceEntity, 0x14);
static_assert(sizeof(CEventGroupEvent) == 0x14 && sizeof(CEventDamage) == 0x44 && sizeof(CCutsceneObject) == 0x19C && sizeof(CHeli) == 0xA18, "tamanos de ranura de pool");
static_assert(offsetof(CRenPar, z) == 0 && offsetof(CRenPar, bigWaves) == 4 && offsetof(CRenPar, smallWaves) == 8 && offsetof(CRenPar, flowX) == 0xC && offsetof(CRenPar, flowY) == 0xD && sizeof(CRenPar) == 0x10, "CRenPar");
}

namespace F100ALive {
using namespace F100AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x61A360, 0x72F430, 0x4B5620, 0x5A1EE0, 0x6E2D60, 0x6E5280, 0x4ADFD0, 0x4AE070, 0x4AD960};
inline constexpr uintptr_t kCrtFree = 0x82413F;
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F100AFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Pattern(u32 kind, u32 seed) { if (kind == 0) Fill(seed); else memset(m.data(), kind == 1 ? 0x00 : (kind == 2 ? 0xFF : 0xCD), N); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    u32 Was32(size_t off) const { u32 v; memcpy(&v, r.data() + kGuard + off, 4); return v; }
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
inline Arena<kObjWin> objA;          // 0: tarea, puntero a liberar u objeto (this) de las funciones 6, 7 y 8
inline Arena<kOutWin> outA;          // 1: destino de LerpRenPar
inline Arena<kPoolWin> poolA;        // 2: cabecera del pool falso
inline Arena<kStatesWin> stA;        // 3: estados de las ranuras
inline Arena<kStorageWin> stoA;      // 4: almacenamiento del pool
inline Arena<kPedWin> pedA;          // 5: ped / entidad de origen
inline Arena<kEvWin> evA;            // 6: evento con vtabla falsa

// Estado del caso en curso para los falsos
struct State { const C* c{}; unsigned typeCalls{}; DWORD tid{}; };
inline State g;
inline std::array<uintptr_t, 16> taskVt{}, evVt{};
inline uint8* freeTramp{};

// Eventos: 10 GetTaskType [this]; 31 RegisterReference 0x571B70 [this, referencia]; 32 CleanUpOldReference 0x571A00 [this, referencia]; 33 destructor con borrado del evento (ranura 0) [this, indicador]; 40 free [puntero]; 90+K ranura K no esperada [this].
template <unsigned K> inline u32 __fastcall FkVtUnexpected(void* self, void*) { Ev& e = PushEv(90 + K, 1); e.v[0] = Tag(self); return 0; }
inline u32 __fastcall FkTaskType(void* self, void*) {                                                    // ranura 4 de la vtabla de la tarea
    Ev& e = PushEv(10, 1); e.v[0] = Tag(self);
    const unsigned k = g.typeCalls++;
    return k < 2 ? g.c->a[k] : 0u;
}
inline void __fastcall FkRegisterRef(void* self, void*, void* ref) {                                      // 0x571B70 (`ret 4`)
    Ev& e = PushEv(31, 2); e.v[0] = Tag(self); e.v[1] = Tag(ref);
}
inline void __fastcall FkCleanUpRef(void* self, void*, void* ref) {                                       // 0x571A00 (`ret 4`)
    Ev& e = PushEv(32, 2); e.v[0] = Tag(self); e.v[1] = Tag(ref);
}
inline void __fastcall FkEventDtor(void* self, void*, u32 flag) {                                         // ranura 0 de la vtabla del evento (`ret 4`)
    Ev& e = PushEv(33, 2); e.v[0] = Tag(self); e.v[1] = flag;
}
inline void __cdecl FkFree(void* p) {                                                                     // free del CRT del EXE (0x82413F, cdecl)
    if (GetCurrentThreadId() != g.tid) { reinterpret_cast<void(__cdecl*)(void*)>(freeTramp)(p); return; }  // hilos ajenos: el free real
    Ev& e = PushEv(40, 1); e.v[0] = Tag(p);
}
inline bool MakeFreeTrampoline() {
    const uint8 expect[7]{0x6A, 0x0C, 0x68, 0x38, 0x80, 0x88, 0x00};   // push 0Ch; push 888038h
    if (memcmp(reinterpret_cast<void*>(kCrtFree), expect, 7) != 0) return false;
    freeTramp = static_cast<uint8*>(VirtualAlloc(nullptr, 32, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!freeTramp) return false;
    memcpy(freeTramp, expect, 7);
    freeTramp[7] = 0xE9;
    const int32 rel = int32(kCrtFree + 7 - (uintptr_t(freeTramp) + 12));
    memcpy(freeTramp + 8, &rel, 4);
    FlushInstructionCache(GetCurrentProcess(), freeTramp, 16);
    return true;
}
inline void InitVtables() {
    [&]<unsigned... I>(std::integer_sequence<unsigned, I...>) { ((taskVt[I] = uintptr_t(&FkVtUnexpected<I>), evVt[I] = uintptr_t(&FkVtUnexpected<I + 16>)), ...); }(std::make_integer_sequence<unsigned, 16>{});
    taskVt[4] = uintptr_t(&FkTaskType);
    evVt[0] = uintptr_t(&FkEventDtor);
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
inline uintptr_t PoolCell(unsigned fn) { return fn == 2 ? kEventPoolCell : (fn == 3 ? kObjectPoolCell : kVehiclePoolCell); }

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 50u;
        objA.Fill(sd++); outA.Fill(sd++); poolA.Fill(sd++); stA.Fill(sd++); stoA.Fill(sd++); pedA.Fill(sd++); evA.Fill(sd++);
        TagClear();
        TagAdd(objA.m.data(), objA.m.size());       // 0
        TagAdd(outA.m.data(), outA.m.size());       // 1
        TagAdd(poolA.m.data(), poolA.m.size());     // 2
        TagAdd(stA.m.data(), stA.m.size());         // 3
        TagAdd(stoA.m.data(), stoA.m.size());       // 4
        TagAdd(pedA.m.data(), pedA.m.size());       // 5
        TagAdd(evA.m.data(), evA.m.size());         // 6
        g = State{}; g.c = &c; g.tid = GetCurrentThreadId(); nev = 0;
        std::vector<u32> args;
        u32 ecx = c.seed, popBytes = 0;
        size_t thisOff = 0;
        switch (fn) {
        case 0:   // CTask::IsGoToTask(task)
            objA.Put32(0, u32(Addr(taskVt.data())));
            args = { u32(objA.Obj()) };
            popBytes = 4;
            break;
        case 1:   // CMemoryMgr::Free(pointer)
            args = { u32(PtrKind(c.a[0], objA.Obj(c.a[2]), c.a[1])) };
            popBytes = 4;
            break;
        case 2: case 3: case 4: {   // operator new de pool: pool falso en arenas
            poolA.Put32(0, u32(stoA.Obj())); poolA.Put32(4, u32(stA.Obj())); poolA.Put32(8, c.a[1]); poolA.Put32(0xC, c.a[2]);
            poolA.Put8(0x10, 0); poolA.Put8(0x11, 1);
            for (unsigned i = 0; i < 8; ++i) stA.Put8(i, c.a[3 + i]);
            G32(PoolCell(fn)) = u32(poolA.Obj());
            if (fn == 4) { args = { c.a[0], c.a[11] }; popBytes = 8; }
            else { args = { c.a[0] }; popBytes = 4; }
            break;
        }
        case 5:   // CWaterLevel LerpRenPar(&out, rp1, rp2, t)
            for (unsigned i = 0; i < 4; ++i) outA.Put32(4 * i, c.a[9 + i]);
            args = { u32(outA.Obj()), c.a[0], c.a[1], c.a[2], c.a[3], c.a[4], c.a[5], c.a[6], c.a[7], c.a[8] };
            popBytes = 40;
            break;
        case 6: {   // CEventGroupEvent::CEventGroupEvent(ped, event)
            objA.Pattern(c.a[5], sd++);
            thisOff = c.a[4];
            for (unsigned i = 0; i < 5; ++i) objA.Put32(thisOff + 4 * i, c.a[6 + i]);
            const u32 ped = u32(PtrKind(c.a[0], pedA.Obj(), c.a[1])), ev = u32(PtrKind(c.a[2], evA.Obj(), c.a[3]));
            ecx = u32(objA.Obj(thisOff));
            args = { ped, ev };
            break;
        }
        case 7: {   // CEventGroupEvent::~CEventGroupEvent()
            objA.Pattern(c.a[4], sd++);
            thisOff = c.a[3];
            evA.Put32(0, u32(Addr(evVt.data())));
            objA.Put32(thisOff, c.a[5]); objA.Put32(thisOff + 4, c.a[6]); objA.Put32(thisOff + 8, c.a[7]);
            objA.Put32(thisOff + 0xC, u32(PtrKind(c.a[0], pedA.Obj(), c.a[1])));
            objA.Put32(thisOff + 0x10, c.a[2] == 0 ? u32(evA.Obj()) : 0u);
            ecx = u32(objA.Obj(thisOff));
            break;
        }
        default: {  // CEventDamage::~CEventDamage()
            objA.Pattern(c.a[3], sd++);
            thisOff = c.a[2];
            for (unsigned i = 0; i < 5; ++i) objA.Put32(thisOff + 4 * i, c.a[4 + i]);
            objA.Put32(thisOff + 0x14, u32(PtrKind(c.a[0], pedA.Obj(), c.a[1])));
            ecx = u32(objA.Obj(thisOff));
            break;
        }
        }
        objA.Snap(); outA.Snap(); poolA.Snap(); stA.Snap(); stoA.Snap(); pedA.Snap(); evA.Snap();
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        const int64 to = int64(thisOff);
        if (fn == 6 || fn == 7) add(0, objA.Changes({{to, 4}, {to + 0xC, 8}}));
        else if (fn == 8) add(0, objA.Changes({{to, 4}, {to + 0x14, 4}}));
        else add(0, objA.Changes());
        add(1, outA.Changes()); add(2, poolA.Changes()); add(3, stA.Changes()); add(4, stoA.Changes()); add(5, pedA.Changes()); add(6, evA.Changes());
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        if (fn == 0) obs["ret"] = sides[s].reg.eax & 0xFFu;
        else if (fn >= 2 && fn <= 6) obs["ret"] = Tag(uintptr_t(sides[s].reg.eax));
        else obs["ret"] = nullptr;
        if (fn >= 6) {
            const auto word = [&](size_t off) { return objA.Get32(thisOff + off); };
            if (fn == 8) obs["ptrs"] = json::array({Tag(uintptr_t(word(0x14)))});
            else obs["ptrs"] = json::array({Tag(uintptr_t(word(0xC))), Tag(uintptr_t(word(0x10)))});
            obs["vptr_changed"] = objA.Get32(thisOff) != objA.Was32(thisOff);
        }
        if (fn == 6) {
            // la vtabla que dejo escrita el constructor: GetEventType (ranura 1), GetEventPriority (ranura 2) y GetLifeTime (ranura 3)
            auto* const self = reinterpret_cast<void*>(objA.Obj(thisOff));
            auto* const vt = *reinterpret_cast<uintptr_t**>(self);
            json v = json::array();
            for (unsigned k = 1; k <= 3; ++k) v.push_back(int32(reinterpret_cast<int(__thiscall*)(void*)>(vt[k])(self)));
            obs["vt"] = v;
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
    ready &= freeTramp != nullptr;
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

// Los tres punteros de pool del banco son los que leen GetEventPool / GetObjectPool / GetVehiclePool de la DLL
inline bool PoolCellsAreTheGlobals() {
    bool ok = true;
    const struct { uintptr_t cell; uintptr_t (*get)(); } cells[]{
        {kEventPoolCell, [] { return uintptr_t(GetEventPool()); }}, {kObjectPoolCell, [] { return uintptr_t(GetObjectPool()); }}, {kVehiclePoolCell, [] { return uintptr_t(GetVehiclePool()); }}};
    for (const auto& c : cells) {
        const u32 saved = G32(c.cell);
        G32(c.cell) = 0x12345678u;
        ok &= c.get() == 0x12345678u;
        G32(c.cell) = saved;
    }
    return ok;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn100a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn100a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn100a"); }
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
    InitVtables();
    ready &= MakeFreeTrampoline();
    ready &= PoolCellsAreTheGlobals();
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
    env.stubs[0] = {};
    env.stubs[1] = {F(kCrtFree, &FkFree)};
    env.stubs[2] = {};
    env.stubs[3] = {};
    env.stubs[4] = {};
    env.stubs[5] = {};
    env.stubs[6] = {F(0x571B70, &FkRegisterRef)};
    env.stubs[7] = {F(0x571A00, &FkCleanUpRef)};
    env.stubs[8] = {F(0x571A00, &FkCleanUpRef)};
    env.reals[2] = {R(0x4B5570)};
    env.reals[3] = {R(0x5A1C20)};
    env.reals[4] = {R(0x6D5DD0)};
    const char* dllReal[kFunctions] = {
        "toda la funcion: las dos comparaciones del tipo (entero completo) y el orden de las llamadas a la ranura 4 de la vtabla (falsa, vtabla propia)",
        "toda la funcion: llama a free del CRT del EXE (0x82413F, falso) con el puntero tal cual, nulo incluido",
        "toda la funcion y CEventPool::New (busqueda con vuelta unica, ocupacion del estado y contador de 7 bits) sobre un pool falso en arenas",
        "toda la funcion y CObjectPool::New sobre un pool falso en arenas",
        "toda la funcion y el NewAt en linea (estado, contador de 7 bits y ultima ranura libre) sobre un pool falso en arenas; el EXE corre 0x6D5DD0",
        "toda la aritmetica x87 (1 - t en registro, productos y suma sin guardar, guardado como float) y los dos bytes de flujo a cero",
        "toda la funcion: campos, vtabla propia (GetEventType, prioridad y vida por sus ranuras) y la llamada a RegisterReference (falso identico)",
        "toda la funcion: las dos liberaciones (CleanUpOldReference falso identico y el destructor virtual con borrado del evento, vtabla falsa) y su orden",
        "toda la funcion: la llamada a CleanUpOldReference (falso identico) con la entidad de origen y la direccion del campo"};
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
