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
#include "Tasks/TaskTypes/TaskGoToVehicleAndLean.h"
#include "Tasks/TaskTypes/TaskComplexBeInCouple.h"
#include "Tasks/TaskCategories.h"
#include "WeaponInfo.h"
#include "GxtChar.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Placeable.h"
#include "Entity/Vehicle/Vehicle.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn111A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Trece funciones pequenas (hook S), cada una con 16384 pares EXE/DLL (ver Fixtures.hpp para la lista, las convenciones y las familias de casos).
// Colaboradores: ninguno tiene direccion fija. Las consultas virtuales (CTask::GetTaskType, ranura 4; CTask::MakeAbortable, ranura 6) van por la vtabla propia de las arenas de tarea (falsos que registran sus argumentos
// y devuelven valores del caso; el resto de ranuras registra eventos inesperados).
// NO se sustituye nada mas: lo demas corre de verdad en ambos lados (copias de cadenas, bits de vehiculo, aritmetica x87 de GetSideOfPartner frente al ayudante de X87Math, tabla de apuntado real).
// Memoria real del juego (se fija y se restaura tras cada lado): el buffer estatico 0xC1BED0 de GxtCharToSafeAscii y la zona de la tabla de desplazamientos de apuntado (0xC8A8A8).

namespace F111ALayout {
static_assert(offsetof(CTaskGoToVehicleAndLean, m_Vehicle) == 0xC, "CTaskGoToVehicleAndLean");
static_assert(offsetof(CTaskComplexBeInCouple, m_partner) == 0x10, "CTaskComplexBeInCouple");
static_assert(offsetof(CWeaponInfo, m_nAimOffsetIndex) == 0x6C, "CWeaponInfo");
static_assert(offsetof(CVehicle, m_nGettingInFlags) == 0x486 && offsetof(CVehicle, m_nGettingOutFlags) == 0x487, "CVehicle");
static_assert(offsetof(CPed, m_matrix) == 0x14, "CPed");
}

namespace F111ALive {
using namespace F111AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x718630, 0x718660, 0x718690, 0x69F910, 0x69F8C0, 0x698680, 0x6985E0, 0x698610, 0x660F60, 0x646CF0, 0x683840, 0x73A280, 0x73A2B0};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F111AFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    // Cambios respecto a la imagen de referencia
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            const int64 off = int64(i) - int64(kGuard);
            if (m[i] != r[i]) out.push_back(json::array({off, m[i]}));
        }
        return out;
    }
};
inline Arena<kTaskWin> taskA;       // 0: tarea (this de DoTidyUp y GetSideOfPartner; argumento de las funciones de CTaskCategories)
inline Arena<kPedWin> pedA;         // 1: ped
inline Arena<kMatWin> pmatA;        // 2: matriz del ped
inline Arena<kPedWin> otherA;       // 3: companero
inline Arena<kMatWin> omatA;        // 4: matriz del companero
inline Arena<kVehWin> vehA;         // 5: vehiculo
inline Arena<kOutWin> outA;         // 6: bytes de salida (bool&)
inline Arena<kSubWin> subA;         // 7: subtarea (vtabla propia)
inline Arena<kStrWin> dstA;         // 8: cadena destino
inline Arena<kStrWin> srcA;         // 9: cadena origen
inline Arena<kWiWin> wiA;           // 10: CWeaponInfo
inline constexpr u32 kRawPed = 0x00BADC00u, kRawVeh = 0x00C0FFEEu;

// Estado del caso en curso para los falsos
struct State { const C* c{}; unsigned taskTypeCalls{}; };
inline State g;

// Eventos: 4 CTask::GetTaskType (ranura 4) [tarea]; 6 CTask::MakeAbortable (ranura 6, `ret 0Ch`) [tarea, ped, prioridad, evento]; 90+k: ranura k no esperada de la vtabla de las tareas.
inline std::array<uintptr_t, 16> taskVt{};
template <unsigned K> inline u32 __fastcall FkVt(void* self, void*) { Ev& e = PushEv(90 + K, 1); e.v[0] = Tag(self); return 0; }
inline u32 __fastcall FkTaskType(void* self, void*) {                                                 // CTask::GetTaskType (ranura 4)
    Ev& e = PushEv(4, 1); e.v[0] = Tag(self);
    const unsigned k = g.taskTypeCalls++;
    return g.c->a[g.c->fn == 5 ? (k < 2 ? k : 2) : 0];
}
inline u32 __fastcall FkAbort(void* self, void*, void* ped, u32 priority, void* event) {              // CTask::MakeAbortable (ranura 6)
    Ev& e = PushEv(6, 4); e.v[0] = Tag(self); e.v[1] = Tag(ped); e.v[2] = priority; e.v[3] = Tag(event);
    return g.c->a[4];
}
inline void InitTaskVt() {
    [&]<unsigned... I>(std::integer_sequence<unsigned, I...>) { ((taskVt[I] = uintptr_t(&FkVt<I>)), ...); }(std::make_integer_sequence<unsigned, 16>{});
    taskVt[4] = uintptr_t(&FkTaskType);
    taskVt[6] = uintptr_t(&FkAbort);
}

// Tipo de retorno que se registra de cada funcion: 0 nada (void), 1 AL, 2 AX, 3 EAX numerico, 4 EAX como puntero (etiquetado)
inline constexpr int kRetKind[kFunctions] = {4, 4, 3, 0, 3, 1, 0, 0, 0, 0, 3, 2, 2};

inline json CloseSide(Env& env, Side& side, json obs, unsigned fn) {
    auto fp = NormalizeFp(side.fp);
    obs["globals_hash"] = RegionsHash();
    if (kRetKind[fn] == 1) obs["ret"] = side.reg.eax & 0xFFu;
    if (kRetKind[fn] == 2) obs["ret"] = side.reg.eax & 0xFFFFu;
    if (kRetKind[fn] == 3) obs["ret"] = side.reg.eax;
    if (kRetKind[fn] == 4) obs["ret"] = Tag(uintptr_t(side.reg.eax));
    if (fn == 4) obs["buf"] = HexBytes(reinterpret_cast<const void*>(kSafeBufAddr), 0x101);
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

inline void Put3(auto& arena, size_t off, const u32* p) { arena.Put32(off, p[0]); arena.Put32(off + 4, p[1]); arena.Put32(off + 8, p[2]); }
// Posicion del ped (matriz +0x30 o placement +4): la activa recibe a[act..act+2] y la otra a[alt..alt+2]
inline void PutPed(auto& ped, auto& mat, bool hasMat, const u32* act, const u32* alt) {
    ped.Put32(0x14, hasMat ? u32(mat.Obj()) : 0u);
    if (hasMat) { Put3(mat, 0x30, act); Put3(ped, 4, alt); }
    else { Put3(ped, 4, act); Put3(mat, 0x30, alt); }
}
// Cadena de relleno (bytes distintos de cero, StrByte modo 0) en toda la ventana de la arena y la cadena de la prueba encima
template <size_t WIN> inline void PutString(Arena<WIN>& arena, u32 padSeed, u32 off, u32 len, u32 seed, u32 mode) {
    for (size_t i = 0; i < WIN; ++i) arena.m[kGuard + i] = StrByte(padSeed, u32(i), 0);
    for (u32 i = 0; i < len; ++i) arena.m[kGuard + off + i] = StrByte(seed, i, mode);
    arena.m[kGuard + off + len] = 0;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    InitTaskVt();
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 50u;
        taskA.Fill(sd++); pedA.Fill(sd++); pmatA.Fill(sd++); otherA.Fill(sd++); omatA.Fill(sd++); vehA.Fill(sd++); outA.Fill(sd++); subA.Fill(sd++); dstA.Fill(sd++); srcA.Fill(sd++); wiA.Fill(sd++);
        TagClear();
        TagAdd(taskA.m.data(), taskA.m.size());     // 0
        TagAdd(pedA.m.data(), pedA.m.size());       // 1
        TagAdd(pmatA.m.data(), pmatA.m.size());     // 2
        TagAdd(otherA.m.data(), otherA.m.size());   // 3
        TagAdd(omatA.m.data(), omatA.m.size());     // 4
        TagAdd(vehA.m.data(), vehA.m.size());       // 5
        TagAdd(outA.m.data(), outA.m.size());       // 6
        TagAdd(subA.m.data(), subA.m.size());       // 7
        TagAdd(dstA.m.data(), dstA.m.size());       // 8
        TagAdd(srcA.m.data(), srcA.m.size());       // 9
        TagAdd(wiA.m.data(), wiA.m.size());         // 10
        g = State{}; g.c = &c;
        nev = 0;
        std::vector<u32> args;
        u32 ecx = c.seed, popBytes = 0;
        json aux = json::object();
        // Buffer estatico del EXE (0xC1BED0): patron conocido antes de cada lado
        for (u32 i = 0; i < 0x101; ++i) *reinterpret_cast<uint8*>(kSafeBufAddr + i) = StrByte(c.seed, i, 0);
        taskA.Put32(0, u32(Addr(taskVt.data()))); subA.Put32(0, u32(Addr(taskVt.data())));
        switch (fn) {
        case 0: case 1: case 3:
            PutString(dstA, c.a[8], c.a[2], c.a[0], c.a[4], c.a[6]);
            PutString(srcA, c.a[9], c.a[3], c.a[1], c.a[5], c.a[7]);
            args = { u32(dstA.Obj(c.a[2])), u32(srcA.Obj(c.a[3])) }; popBytes = 8;
            break;
        case 2:
            PutString(srcA, c.a[9], c.a[3], c.a[1], c.a[5], c.a[7]);
            args = { u32(srcA.Obj(c.a[3])) }; popBytes = 4;
            break;
        case 4:
            PutString(srcA, c.a[5], c.a[1], c.a[0], c.a[2], c.a[3]);
            args = { c.a[4] ? 0u : u32(srcA.Obj(c.a[1])) }; popBytes = 4;
            break;
        case 5:
            args = { u32(taskA.Obj()) }; popBytes = 4;
            break;
        case 6:
            outA.Put8(0, c.a[2]); outA.Put8(c.a[1], c.a[3]);
            args = { u32(taskA.Obj()), u32(outA.Obj()), u32(outA.Obj(c.a[1])) }; popBytes = 12;
            break;
        case 7:
            outA.Put8(0, c.a[2]); outA.Put8(c.a[1], c.a[3]);
            args = { c.a[0] == 0 ? u32(taskA.Obj()) : (c.a[0] == 1 ? 0u : kRawPed), u32(outA.Obj()), u32(outA.Obj(c.a[1])) }; popBytes = 12;
            break;
        case 8:
            ecx = u32(taskA.Obj());
            taskA.Put32(0x08, c.a[2] == 0 ? u32(subA.Obj()) : 0u);
            taskA.Put32(0x0C, c.a[1] == 0 ? u32(vehA.Obj()) : 0u);
            vehA.Put8(0x42E, c.a[3]);
            args = { c.a[0] == 0 ? u32(pedA.Obj()) : (c.a[0] == 1 ? 0u : kRawPed) };
            break;
        case 9: {
            const bool valid = c.a[0] - 8u <= 3u;
            vehA.Put8(0x486, c.a[1]); vehA.Put8(0x487, c.a[2]);
            outA.Put8(0, c.a[4]); outA.Put8(c.a[3], c.a[5]);
            const u32 veh = (valid || c.a[6] == 0) ? u32(vehA.Obj()) : (c.a[6] == 1 ? 0u : kRawVeh);
            args = { veh, c.a[0], u32(outA.Obj()), u32(outA.Obj(c.a[3])) }; popBytes = 16;
            break;
        }
        case 10: {
            ecx = u32(taskA.Obj());
            PutPed(pedA, pmatA, c.a[0] != 0, &c.a[1], &c.a[4]);
            if (c.a[13]) {
                taskA.Put32(0x10, u32(pedA.Obj()));
                Put3(pmatA, 0, &c.a[10]);                       // la "derecha" de la matriz del ped (que es la del companero)
            } else {
                taskA.Put32(0x10, u32(otherA.Obj()));
                otherA.Put32(0x14, u32(omatA.Obj()));
                Put3(omatA, 0, &c.a[10]); Put3(omatA, 0x30, &c.a[7]);
                Put3(otherA, 4, &c.a[14]);
            }
            args = { u32(pedA.Obj()) };
            break;
        }
        default: {
            ecx = u32(wiA.Obj());
            wiA.Put32(0x6C, (c.a[2] << 16) | (c.a[1] & 0xFFFFu));
            const int idx = int(int16_t(c.a[1] & 0xFFFFu));
            const uintptr_t row = kAimTableAddr + intptr_t(idx) * 24;
            json words = json::array();
            for (unsigned k = 0; k < 4; ++k) words.push_back(*reinterpret_cast<const uint16_t*>(row + 0x10 + 2 * k));
            aux["table"] = words;
            args = { c.a[0] };
            break;
        }
        }
        taskA.Snap(); pedA.Snap(); pmatA.Snap(); otherA.Snap(); omatA.Snap(); vehA.Snap(); outA.Snap(); subA.Snap(); dstA.Snap(); srcA.Snap(); wiA.Snap();
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        json obs = aux;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, taskA.Changes()); add(1, pedA.Changes()); add(2, pmatA.Changes()); add(3, otherA.Changes()); add(4, omatA.Changes()); add(5, vehA.Changes());
        add(6, outA.Changes()); add(7, subA.Changes()); add(8, dstA.Changes()); add(9, srcA.Changes()); add(10, wiA.Changes());
        obs["changes"] = ch;
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

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn111a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn111a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn111a"); }
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
    const char* dllReal[kFunctions] = {
        "copia byte a byte de cadenas en arenas (sin colaboradores)",
        "copia byte a byte de cadenas en arenas (sin colaboradores)",
        "recorrido de la cadena (sin colaboradores)",
        "copia byte a byte de cadenas en arenas (sin colaboradores)",
        "conversion al buffer estatico del EXE (0xC1BED0, memoria real restaurada)",
        "tres consultas de GetTaskType por la ranura 4 de la vtabla propia de la tarea (falso identico)",
        "una consulta de GetTaskType por la ranura 4 de la vtabla propia de la tarea (falso identico)",
        "sin colaboradores",
        "ranura 6 (MakeAbortable) de la vtabla propia de la subtarea (falso identico); el bit del vehiculo corre de verdad",
        "bits del vehiculo (sin colaboradores)",
        "producto escalar x87 del ayudante de X87Math frente al x87 del EXE (sin colaboradores)",
        "tabla de apuntado real del juego (sin colaboradores)",
        "tabla de apuntado real del juego (sin colaboradores)"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", json::array()}, {"hooks_found", json::array()}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
        env.stubs[f] = {};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
