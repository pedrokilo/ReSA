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
#include "Tasks/TaskTypes/TaskSimpleDuck.h"
#include "Tasks/TaskTypes/TaskSimpleGoToPointFine.h"
#include "Tasks/TaskTypes/TaskSimpleInAir.h"
#include "Tasks/TaskManager.h"
#include "Models/VehicleModelInfo.h"
#include "Animation/AnimBlendAssociation.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "MBlur.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn108A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Diecisiete funciones pequenas (hook S), cada una con 16384 pares EXE/DLL (ver Fixtures.hpp para la lista, las convenciones y las familias de casos).
// Colaboradores (todos con hook RH, apagado durante el banco; falsos identicos en la direccion del EXE y en el cuerpo de la DLL):
//   14  CAnimBlendAssociation::SetFinishCallback 0x4CEBE0 (thiscall, 2 en pila, `ret 8`)
//   15 y 16  CAEVehicleAudioEntity::GetVehicleTypeForAudio 0x4F4F00 (thiscall, sin argumentos) y CPed::Say 0x5EFFE0 (thiscall, 6 en pila, `ret 18h`)
// Por la vtabla de las arenas (sin tocar codigo del EXE): ranura 2 (CTask::GetSubTask) en las cadenas de tareas de las funciones 11 y 12, y la ranura 7 (CAEPedSpeechAudioEntity::IsPedFemaleForAudio, en ped+0x294) en la 16.
// NO se sustituye nada mas: lo demas corre de verdad en ambos lados (comparaciones de float del EXE x87 frente al C++ de la DLL).
// Memoria real del juego (se fija y se restaura tras cada lado): CMBlur::Drunkness 0xC73C58 y CTimer::ms_fTimeScale 0xB7CB64.

namespace F108ALayout {
static_assert(offsetof(CTaskSimpleDuck, m_ShotWhizzingCounter) == 0xE && offsetof(CTaskSimpleDuck, m_DuckAnim) == 0x10 && offsetof(CTaskSimpleDuck, m_MoveAnim) == 0x14 && offsetof(CTaskSimpleDuck, m_bIsFinished) == 0x18
    && offsetof(CTaskSimpleDuck, m_bIsAborting) == 0x19 && offsetof(CTaskSimpleDuck, m_bNeedToSetDuckFlag) == 0x1A && offsetof(CTaskSimpleDuck, m_MoveCmd) == 0x1C, "CTaskSimpleDuck");
static_assert(offsetof(CAnimBlendAssociation, m_BlendAmount) == 0x18 && offsetof(CAnimBlendAssociation, m_BlendDelta) == 0x1C, "CAnimBlendAssociation");
static_assert(offsetof(CTaskSimpleGoToPointFine, m_fMoveRatio) == 0x20, "CTaskSimpleGoToPointFine");
static_assert(offsetof(CVehicleModelInfo, m_fWheelSizeFront) == 0x40 && offsetof(CVehicleModelInfo, m_fWheelSizeRear) == 0x44, "CVehicleModelInfo");
static_assert(offsetof(CTaskManager, m_aPrimaryTasks) == 0 && TASK_PRIMARY_MAX == 5, "CTaskManager");
static_assert(offsetof(CVehicle, m_vehicleAudio) == 0x138, "CVehicle");   // CPed::m_pedSpeech (+0x294) es protegido: lo comprueba el banco con la etiqueta del evento de IsPedFemaleForAudio
}

namespace F108ALive {
using namespace F108AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x71D700, 0x71D710, 0x71D720, 0x71D730, 0x71D780, 0x7204C0, 0x6C4750, 0x717820, 0x678E80, 0x65F3A0, 0x6A06F0, 0x681970, 0x681A00, 0x61C420, 0x692340, 0x6497A0, 0x6497F0};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F108AFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia, salvo los dwords que la funcion escribe a proposito (se informan aparte como `fields`)
    json Changes(const std::vector<size_t>& skipOffsets) const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            const int64 off = int64(i) - int64(kGuard);
            bool skip = false;
            for (size_t s : skipOffsets) if (off >= int64(s) && off < int64(s) + 4) skip = true;
            if (!skip && m[i] != r[i]) out.push_back(json::array({off, m[i]}));
        }
        return out;
    }
};
inline Arena<kTaskWin> taskA;        // 0: this de la funcion (tarea, gestor de tareas o informacion de modelo)
inline Arena<kPedWin> pedA;          // 1: ped (jacker / jacked / ped de ApplyRollAndPitch)
inline Arena<kPedWin> pedB;          // 2: segundo ped (jacked de SayJacking; su vptr de audio en +0x294)
inline Arena<kVehWin> vehA;          // 3: vehiculo
inline Arena<kAnimWin> animA;        // 4: animacion de agachado
inline Arena<kAnimWin> animB;        // 5: animacion de movimiento
inline std::array<Arena<kChainWin>, kChainN> chain;   // 6..21: cadenas de tareas
inline constexpr unsigned kArenaCount = 6 + kChainN;
inline constexpr u32 kRawPed = 0x00BADC00u, kRawPed2 = 0x00BADC40u;
inline constexpr u32 kDefaultCb = 0xDEFA0001u;

// Estado del caso en curso para los falsos
struct State { const C* c{}; std::array<u32, kChainN> next{}; };
inline State g;

// Eventos: 2 CTask::GetSubTask (ranura 2) [tarea]; 5 CAnimBlendAssociation::SetFinishCallback 0x4CEBE0 [animacion, callback (kDefaultCb si es DefaultAnimCB), datos];
// 10 GetVehicleTypeForAudio 0x4F4F00 [audio]; 11 IsPedFemaleForAudio (ped+0x294, ranura 7) [audio del ped]; 31 CPed::Say 0x5EFFE0 [ped, 6 argumentos de pila];
// 90+k: ranura k no esperada de la vtabla de las tareas o de la del audio del ped.
inline std::array<uintptr_t, 16> taskVt{}, pedVt{};
template <unsigned K> inline u32 __fastcall FkVt(void* self, void*) { Ev& e = PushEv(90 + K, 1); e.v[0] = Tag(self); return 0; }
inline u32 __fastcall FkSubTask(void* self, void*) {                                                  // CTask::GetSubTask (ranura 2)
    Ev& e = PushEv(2, 1); e.v[0] = Tag(self);
    for (unsigned i = 0; i < kChainN; ++i) if (self == reinterpret_cast<void*>(chain[i].Obj())) return g.next[i];
    return 0;
}
inline u32 __fastcall FkFemale(void* self, void*) {                                                   // IsPedFemaleForAudio (ranura 7)
    Ev& e = PushEv(11, 1); e.v[0] = Tag(self);
    return g.c->a[3];
}
inline u32 __fastcall FkGetVehicleType(void* self, void*) {                                           // 0x4F4F00
    Ev& e = PushEv(10, 1); e.v[0] = Tag(self);
    return g.c->a[0];
}
inline u32 __fastcall FkSay(void* self, void*, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6) {        // 0x5EFFE0 (`ret 18h`)
    Ev& e = PushEv(31, 7); e.v[0] = Tag(self); e.v[1] = a1; e.v[2] = a2; e.v[3] = a3; e.v[4] = a4; e.v[5] = a5; e.v[6] = a6;
    return g.c->a[2];
}
inline void __fastcall FkSetFinish(void* self, void*, void* cb, void* data) {                           // 0x4CEBE0 (`ret 8`)
    Ev& e = PushEv(5, 3); e.v[0] = Tag(self);
    const uintptr_t p = uintptr_t(cb);
    e.v[1] = (p == 0x4D1390 || (p && Body(p) == Body(Addr(&CDefaultAnimCallback::DefaultAnimCB)))) ? kDefaultCb : Tag(cb);
    e.v[2] = Tag(data);
}
inline void InitVt() {
    [&]<unsigned... I>(std::integer_sequence<unsigned, I...>) { (((taskVt[I] = uintptr_t(&FkVt<I>)), (pedVt[I] = uintptr_t(&FkVt<I>))), ...); }(std::make_integer_sequence<unsigned, 16>{});
    taskVt[2] = uintptr_t(&FkSubTask);
    pedVt[7] = uintptr_t(&FkFemale);
}

// Tipo de retorno que se registra de cada funcion: 0 nada (void o ST0), 1 AL (bool), 2 EAX como puntero (etiquetado)
inline constexpr int kRetKind[kFunctions] = {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 0, 1, 1};

inline json CloseSide(Env& env, Side& side, json obs, unsigned fn) {
    auto fp = NormalizeFp(side.fp);
    obs["globals_hash"] = RegionsHash();
    obs["drunk_final"] = G32(kDrunkAddr);
    obs["scale_final"] = G32(kScaleAddr);
    if (kRetKind[fn] == 1) obs["al"] = side.reg.eax & 0xFFu;
    if (kRetKind[fn] == 2) obs["eax_tag"] = Tag(uintptr_t(side.reg.eax));
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

inline u32 PedArg(u32 kind) { return kind == 0 ? u32(pedA.Obj()) : (kind == 1 ? 0u : (kind == 2 ? kRawPed : kRawPed2)); }

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    InitVt();
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 50u;
        taskA.Fill(sd++); pedA.Fill(sd++); pedB.Fill(sd++); vehA.Fill(sd++); animA.Fill(sd++); animB.Fill(sd++);
        for (auto& ch : chain) ch.Fill(sd++);
        TagClear();
        TagAdd(taskA.m.data(), taskA.m.size());     // 0
        TagAdd(pedA.m.data(), pedA.m.size());       // 1
        TagAdd(pedB.m.data(), pedB.m.size());       // 2
        TagAdd(vehA.m.data(), vehA.m.size());       // 3
        TagAdd(animA.m.data(), animA.m.size());     // 4
        TagAdd(animB.m.data(), animB.m.size());     // 5
        for (auto& ch : chain) TagAdd(ch.m.data(), ch.m.size());   // 6..21
        g = State{}; g.c = &c; nev = 0;
        std::vector<u32> args;
        u32 ecx = c.seed, popBytes = 0;
        std::vector<std::array<size_t, 2>> fields;          // (arena, offset) de los dwords que la funcion escribe a proposito
        G32(kDrunkAddr) = c.a[40]; G32(kScaleAddr) = c.a[41];
        auto setChain = [&](unsigned base, unsigned len) {   // cadena de `len` tareas a partir de chain[base]
            for (unsigned i = 0; i < len; ++i) {
                chain[base + i].Put32(0, u32(Addr(taskVt.data())));
                g.next[base + i] = i + 1 < len ? u32(chain[base + i + 1].Obj()) : 0u;
            }
        };
        switch (fn) {
        case 0: for (unsigned k = 0; k < 7; ++k) args.push_back(c.a[k]); popBytes = 28; break;
        case 1: case 3: case 5: args = { c.a[0] }; popBytes = 4; break;
        case 2: case 4: case 6: case 7: break;
        case 8: ecx = u32(taskA.Obj()); args = { PedArg(c.a[0]) }; break;
        case 9: ecx = u32(taskA.Obj()); taskA.Put32(0x20, c.a[1]); args = { c.a[0] }; fields = {{0, 0x20}}; break;
        case 10: ecx = u32(taskA.Obj()); taskA.Put32(0x40, c.a[1]); taskA.Put32(0x44, c.a[2]); args = { c.a[0] }; break;
        case 11: {
            setChain(0, c.a[0]);
            args = { c.a[0] ? u32(chain[0].Obj()) : 0u }; popBytes = 4;
            break;
        }
        case 12: {
            ecx = u32(taskA.Obj());
            for (unsigned sl = 0; sl < 5; ++sl) {
                const unsigned len = c.a[1 + sl];
                taskA.Put32(4 * sl, len ? u32(chain[sl * 3].Obj()) : 0u);
                setChain(sl * 3, len);
            }
            args = { c.a[0] };
            break;
        }
        case 13:
            ecx = u32(taskA.Obj());
            taskA.Put32(0x0C, c.a[0]); taskA.Put32(0x18, c.a[1]); taskA.Put32(0x1C, c.a[2]);
            taskA.Put32(0x10, c.a[3] == 0 ? u32(animA.Obj()) : 0u);
            animA.Put32(0x18, c.a[4]);
            break;
        case 14:
            ecx = u32(taskA.Obj());
            taskA.Put32(0x10, c.a[0] == 0 ? u32(animA.Obj()) : 0u);
            taskA.Put32(0x14, c.a[1] == 0 ? u32(animB.Obj()) : 0u);
            taskA.Put32(0x18, c.a[2]);
            animB.Put32(0x1C, c.a[3]);
            animA.Put32(0x1C, c.a[5]);
            args = { PedArg(c.a[4]) };
            fields = {{0, 0x10}, {0, 0x14}, {0, 0x18}, {4, 0x1C}, {5, 0x1C}};
            break;
        case 15: args = { u32(pedA.Obj()), u32(vehA.Obj()), c.a[1] }; popBytes = 12; break;
        default:
            pedB.Put32(0x294, u32(Addr(pedVt.data())));
            args = { u32(pedA.Obj()), u32(pedB.Obj()), u32(vehA.Obj()), c.a[1] }; popBytes = 16;
            break;
        }
        taskA.Snap(); pedA.Snap(); pedB.Snap(); vehA.Snap(); animA.Snap(); animB.Snap();
        for (auto& ch : chain) ch.Snap();
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        auto skipFor = [&](unsigned idx) { std::vector<size_t> v; for (const auto& f : fields) if (f[0] == idx) v.push_back(f[1]); return v; };
        add(0, taskA.Changes(skipFor(0))); add(1, pedA.Changes(skipFor(1))); add(2, pedB.Changes(skipFor(2))); add(3, vehA.Changes(skipFor(3)));
        add(4, animA.Changes(skipFor(4))); add(5, animB.Changes(skipFor(5)));
        for (unsigned i = 0; i < kChainN; ++i) add(6 + i, chain[i].Changes({}));
        obs["changes"] = ch;
        json fl = json::array();
        for (const auto& f : fields) {
            u32 v = 0;
            switch (f[0]) { case 0: v = taskA.Get32(f[1]); break; case 4: v = animA.Get32(f[1]); break; case 5: v = animB.Get32(f[1]); break; default: break; }
            fl.push_back(json::array({f[0], f[1], v}));
        }
        obs["fields"] = fl;
        obs["events"] = EvsJson();
        rows[s] = CloseSide(env, sides[s], obs, fn);
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
    if (!std::filesystem::exists(dir / "fn108a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn108a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn108a"); }
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
    env.stubs[14] = {F(0x4CEBE0, &FkSetFinish)};
    env.stubs[15] = {F(0x4F4F00, &FkGetVehicleType), F(0x5EFFE0, &FkSay)};
    env.stubs[16] = {F(0x4F4F00, &FkGetVehicleType), F(0x5EFFE0, &FkSay)};
    const char* dllReal[kFunctions] = {
        "ninguna: NOP (la DLL y el EXE no tocan memoria)",
        "devuelve true", "devuelve true",
        "comparaciones de float (ordenadas) del propio cuerpo y escritura de CMBlur::Drunkness",
        "CTimer::ms_fTimeScale = 1.0 y CMBlur::Drunkness = 0",
        "ninguna: NOP", "ninguna: NOP", "ninguna: NOP", "ninguna: NOP (ret 4 vacio)",
        "escritura de this+0x20", "seleccion del tamano de rueda por el byte bajo del argumento, devuelto en ST0",
        "recorrido por la ranura 2 (GetSubTask) de la vtabla propia de cada tarea de la cadena (falso identico)",
        "lectura del hueco primario y recorrido por la ranura 2 de la vtabla propia de cada tarea (falso identico)",
        "comparaciones de float y de bytes/palabras del propio cuerpo; la mezcla de la animacion se lee de la arena",
        "escrituras de la tarea; SetFinishCallback (0x4CEBE0) falso identico",
        "tipo de audio (0x4F4F00) y CPed::Say (0x5EFFE0) falsos identicos",
        "tipo de audio (0x4F4F00), IsPedFemaleForAudio (ranura 7 de la vtabla propia, ped+0x294) y CPed::Say (0x5EFFE0) falsos identicos"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
