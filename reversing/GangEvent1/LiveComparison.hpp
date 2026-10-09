#pragma once
// Banco GangEvent1 (flujo viejo, estilo Small2/PadMouse1/WorldFinal1): CGroupEventHandler (30 funciones pendientes) y CGangWars
// (32 funciones pendientes) dentro del juego completo (se engancha en CGame::Process con probe_build.py).
// Cada caso ejecuta primero el original (hook desactivado: se llama a la direccion del EXE y corre el codigo del EXE) y
// despues el candidato (hook activo: la misma direccion salta a la DLL) con fixtures identicos y la misma semilla, y compara
// retorno, estado de los objetos falsos, estado global de CGangWars y la traza ordenada de colaboradores (con argumentos).
//
// Todo lo que se toca es propio del banco o se restaura: peatones/vehiculos/grupos/eventos/tareas falsos en buffers propios, arenas
// para CTask::operator new y CTaskAllocator::operator new (rellenas con 0xCD), pools de peatones/vehiculos falsos (se intercambia
// el puntero global solo durante cada llamada), el bloque estatico de CGangWars, las zonas y los datos de poblacion/streaming que
// lee cada funcion (instantanea al empezar y restaurada al terminar) y un hueco del arreglo CPedGroups::ms_groups que hace de
// grupo del jugador (instantanea y restauracion). Los colaboradores se simulan con EntryPatch en la direccion del EXE y en el
// simbolo de la DLL (Body() de todos se calcula ANTES de crear ningun EntryPatch) y solo registran trazas en el hilo del banco.
#include <fstream>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Events/GroupEventHandler.h"
#include "Events/EventHandler.h"
#include "Events/EventGroup.h"
#include "Events/EventScriptCommand.h"
#include "Events/EventCreatePartnerTask.h"
#include "Events/EventEditableResponse.h"
#include "Ragdoll/IKChainManager.h"
#include "Tasks/TaskComplexSequence.h"
#include "Tasks/TaskTypes/TaskComplexStareAtPed.h"
#include "Tasks/TaskTypes/TaskComplexLeaveAnyCar.h"
#include "Tasks/TaskTypes/TaskComplexGangJoinRespond.h"
#include "Tasks/TaskTypes/TaskSimpleGoToPoint.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCar.h"
#include "Tasks/TaskTypes/TaskSimpleCarDrive.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.h"
#include "Tasks/TaskTypes/TaskComplexSmartFleeEntity.h"
#include "Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.h"
#include "Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h"
#include "Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.h"
#include "Tasks/TaskTypes/TaskGoToVehicleAndLean.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFoot.h"
#include "Tasks/TaskTypes/TaskGangHassleVehicle.h"
#include "Tasks/TaskTypes/TaskGangHasslePed.h"
#include "Tasks/TaskTypes/TaskComplexSignalAtPed.h"
#include "Tasks/TaskTypes/TaskComplexPartnerGreet.h"
#include "Tasks/TaskTypes/TaskComplexPartnerDeal.h"
#include "Tasks/TaskTypes/TaskComplexWalkAlongsidePed.h"
#include "Tasks/TaskTypes/TaskComplexDriveWander.h"
#include "Tasks/TaskTypes/TaskComplexWanderGang.h"
#include "Tasks/Allocators/TaskAllocator.h"
#include "Tasks/Allocators/TaskAllocatorPlayerCommandAttack.h"
#include "Tasks/Allocators/TaskAllocatorKillThreatsBasic.h"
#include "Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.h"
#include "Tasks/Allocators/TaskAllocatorKillThreatsDriveby.h"
#include "Pools/PedPool.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedIntelligence.h"
#include "Tasks/TaskManager.h"
#include "Pickups.h"
#include "TheZones.h"
#include "PathFind.h"
#include "CarCtrl.h"
#include "Population.h"
#include "Radar.h"
#include "Hud.h"
#include "Messages.h"
#include "Stats.h"
#include "Text/Text.h"
#include "World.h"
#include "Streaming.h"
#include "References.h"
#include "EntryExitManager.h"
#include "Tasks/TaskTypes/TaskComplexWander.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "Pools/TaskAllocatorPool.h"
#include "Pools/VehiclePool.h"
#include "GangWars.h"
#include "GangWarsSaveStructure.h"
#include "GenericGameStorage.h"
#include "../ProbeRuntime.hpp"

namespace GangEvent1Live {
inline bool done{}; inline std::ofstream log; inline json trace;
// Hilo del banco. Otros hilos del juego (audio, streaming) pueden llamar a colaboradores parcheados (FindPlayerPed, rand...):
// no trazan ni tocan estado compartido (carrera sobre 'trace' = traza espuria y corrupcion de memoria).
inline DWORD owner{};
inline bool Mine() { return GetCurrentThreadId() == owner; }
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
inline void T(json r) { if (Mine()) trace.push_back(std::move(r)); }
inline uint64 Fnv(const void* p, size_t n, uint64 h = 1469598103934665603ull) { const auto* b = static_cast<const uint8*>(p); for (size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 1099511628211ull; } return h; }
inline std::string Hex(uint64 v) { return std::format("{:016x}", v); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline json V(const CVector& v) { return json::array({ F(v.x), F(v.y), F(v.z) }); }
inline float Fl(uint32 bits) { return std::bit_cast<float>(bits); }

// --- nombres de los objetos conocidos (los punteros reales no son comparables entre ejecuciones de la misma sesion si cambian) ---
struct Range { uintptr_t lo, hi; std::string name; };
inline std::vector<Range> ranges;
inline void Reg(const void* p, size_t n, const std::string& name) { ranges.push_back({ uintptr_t(p), uintptr_t(p) + n, name }); }
inline bool Known(uintptr_t v) { for (auto& r : ranges) if (v >= r.lo && v < r.hi) return true; return false; }
inline json Name(uintptr_t v) {
    if (!v) return nullptr;
    for (auto& r : ranges) if (v >= r.lo && v < r.hi) return v == r.lo ? json(r.name) : json(r.name + "+" + std::to_string(v - r.lo));
    return v;
}
inline json NameP(const void* p) { return Name(uintptr_t(p)); }

// --- semilla de la ejecucion, RNG del juego simulado y RNG del banco ---
inline uint32 runSeed{}, rngCalls{};
inline int Rnd() { const uint32 x = (runSeed * 2654435761u + rngCalls++ * 40503u) >> 7; return int(x & 0x7FFF); }
// EXE 0x821B1E (rand del CRT, cdecl, int) y CGeneral::GetRandomNumber (DLL, uint16): misma secuencia y misma traza
inline int __cdecl RandExe() { if (!Mine()) return 0; const int r = Rnd(); T(json::array({ "rand", r })); return r; }
inline uint16 RandDll() { return uint16(RandExe()); }
struct Rng {
    uint64 s;
    explicit Rng(uint64 seed) : s(seed * 0x9E3779B97F4A7C15ull + 0x1234567ull) { Next(); Next(); }
    uint32 Next() { s ^= s >> 12; s ^= s << 25; s ^= s >> 27; return uint32((s * 0x2545F4914F6CDD1Dull) >> 32); }
    int Pick(int n) { return int(Next() % uint32(n)); }
    bool Chance(int pct) { return int(Next() % 100u) < pct; }
};
template<class A> inline auto PickOf(Rng& r, const A& a) { return a[r.Pick(int(std::size(a)))]; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls, const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if (c.Name() == cls) for (auto& h : c.Items()) if (h->Name() == fn) r = h.get(); });
    return r;
}
// Sigue la cadena de saltos E9 (thunk incremental -> cuerpo) pero se detiene si el destino cae dentro del EXE (hook desactivado:
// el inicio de la funcion de la DLL salta al EXE y no es el cuerpo que hay que parchear).
inline uintptr_t Body(uintptr_t a) {
    for (int j = 0; j < 16 && *reinterpret_cast<uint8*>(a) == 0xE9; ++j) {
        const uintptr_t next = a + 5 + *reinterpret_cast<int32*>(a + 1);
        if (next >= 0x400000 && next < 0x1000000) break;
        a = next;
    }
    return a;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };

// =====================================================================================================================
// Parte 1: CGroupEventHandler. Mundo falso: peatones/vehiculos/grupo/eventos/tareas en buffers propios.
// Los desplazamientos son los del EXE (gta-sa.exe 1.0 US): se documentan en cada uso.
// =====================================================================================================================
constexpr int NPED = 12, NVEH = 3, NFT = 6, PL = 12, PGID = 7;
constexpr size_t PEDSZ = 0x800, INTELSZ = 0x2A0;
alignas(16) inline std::array<std::array<uint8, PEDSZ>, NPED + 1> pedBuf;          // 0..11 peatones, 12 jugador
alignas(16) inline std::array<std::array<uint8, INTELSZ>, NPED + 1> intelBuf;      // CPedIntelligence (+4 TaskManager, +0x68 EventGroup)
alignas(16) inline std::array<std::array<uint8, 0x40>, NPED + 1> matBuf;           // matriz falsa: posicion en +0x30
alignas(16) inline std::array<uint8, 0x40> playerData;                             // CPlayerPedData falso: +0x38 = id del grupo del jugador
alignas(16) inline std::array<std::array<uint8, 0xA40>, NVEH> vehBuf;
alignas(16) inline std::array<std::array<uint8, 0x120>, NVEH> handBuf;             // tCarHandlingData falso: +0xDE = grupo de animacion
alignas(16) inline std::array<std::array<uint8, 0x60>, NVEH> fxBuf;                // FxSystem_c falso: +0x50 = estado de reproduccion
alignas(16) inline std::array<uint8, sizeof(CPedGroup) + 0x40> groupBuf;           // grupo propio (+0x20 de guarda al inicio)
alignas(16) inline std::array<uint8, sizeof(CPedGroup)> savedPlayerGroup;          // instantanea del hueco real de CPedGroups::ms_groups
alignas(16) inline std::array<std::array<uint8, 0x50>, 3> evBuf;                   // 0 evento, 1 clon, 2 evento interno de CEventGroupEvent
alignas(16) inline std::array<uint8, 0x40> groupEvBuf;                              // CEventGroupEvent falso
alignas(16) inline std::array<std::array<uint8, 0x40>, NFT> ftBuf;                 // tareas falsas (resultados de busqueda y de ComputeEventResponseTask)
alignas(16) inline std::array<std::array<uint8, 0x100>, 256> taskArena;
alignas(16) inline std::array<std::array<uint8, 0x40>, 6> allocArena;
inline int taskNext{}, allocNext{};
inline std::array<void*, 16> ftVtbl{}, evVtbl{};
inline CPlayerPed* realPlayer{};
inline std::set<int> playerArgs;

// almacenamiento del pool falso de CGangWars (zancada de CCopPed 0x7C4 y de CHeli/CVehicle 0xA18) y de otros objetos falsos
constexpr int NGW = 16;
constexpr size_t PEDSTRIDE = 0x7C4, VEHSTRIDE = 0xA18;
alignas(16) inline std::array<std::array<uint8, PEDSTRIDE>, NGW> gwPed;
alignas(16) inline std::array<std::array<uint8, INTELSZ>, NGW> gwIntel;
alignas(16) inline std::array<uint8, NGW> gwPedState;       // bit 7 = hueco libre
alignas(16) inline std::array<std::array<uint8, VEHSTRIDE>, NGW> gwVeh;
alignas(16) inline std::array<uint8, NGW> gwVehState;
alignas(16) inline std::array<std::array<uint8, 0x40>, 4> gwMat;           // matrices de los coches nuevos
alignas(16) inline std::array<std::array<uint8, 0x20>, 4> gwZone;          // CZone falsa (etiqueta en +0, coordenadas en +0x10)
alignas(16) inline std::array<std::array<uint8, 0x11>, 4> gwInfo;          // CZoneInfo falsa
alignas(16) inline std::array<std::array<uint8, 0x1C>, 8> gwNodes;         // CPathNode falsos

inline uint8* Pd(int i) { return pedBuf[i].data(); }
inline CPed* P(int i) { return reinterpret_cast<CPed*>(pedBuf[i].data()); }
inline uint8* Vh(int i) { return vehBuf[i].data(); }
inline uint8* Grp() { return groupBuf.data() + 0x20; }
inline uint8* PGrp() { return reinterpret_cast<uint8*>(&CPedGroups::ms_groups.data()[PGID]); }
inline uint8* Ev(int i) { return evBuf[i].data(); }
template<class U> inline U& At(uint8* base, size_t off) { return *reinterpret_cast<U*>(base + off); }
template<class U> inline const U& At(const uint8* base, size_t off) { return *reinterpret_cast<const U*>(base + off); }

// 0..11 peatones del mundo de grupos, 12 jugador, 100+k peatones del pool falso de CGangWars, 200+k vehiculos de ese pool, -1 desconocido
inline int PedIdOf(uintptr_t v) {
    for (int i = 0; i <= NPED; ++i) {
        const auto a = uintptr_t(pedBuf[i].data()), b = uintptr_t(intelBuf[i].data());
        if (v >= a && v < a + PEDSZ) return i;
        if (v >= b && v < b + INTELSZ) return i;
    }
    for (int k = 0; k < NGW; ++k) {
        const auto a = uintptr_t(gwPed[k].data()), b = uintptr_t(gwIntel[k].data()), c = uintptr_t(gwVeh[k].data());
        if (v >= a && v < a + PEDSTRIDE) return 100 + k;
        if (v >= b && v < b + INTELSZ) return 100 + k;
        if (v >= c && v < c + VEHSTRIDE) return 200 + k;
    }
    return -1;
}

// ---- tareas / eventos falsos con vtable trazada ----
inline void* __fastcall FtDtor(void* self, void*, uint32 flags) { T(json::array({ "delete_fake_task", NameP(self), flags })); return self; }
template<int N> void* __fastcall FtTrap(void* self, void*) { T(json::array({ "fake_task_unexpected_slot", N, NameP(self) })); return nullptr; }
inline void* __fastcall EvDtor(void* self, void*, uint32 flags) { T(json::array({ "event_delete", NameP(self), flags })); return self; }
inline int __fastcall EvType(void* self, void*) { return At<int32>(reinterpret_cast<uint8*>(self), 0x3C); }
inline void* __fastcall EvClone(void* self, void*) { T(json::array({ "event_clone", NameP(self) })); std::memcpy(Ev(1), self, 0x50); return Ev(1); }
inline bool __fastcall EvEditable(void* self, void*) { return At<uint8>(reinterpret_cast<uint8*>(self), 0x3D) != 0; }
inline void* __fastcall EvSource(void* self, void*) { return At<void*>(reinterpret_cast<uint8*>(self), 0x14); }
template<int N> void* __fastcall EvTrap(void* self, void*) { T(json::array({ "event_unexpected_slot", N, NameP(self) })); return nullptr; }
template<int... N> void FillTraps(std::integer_sequence<int, N...>) { ((ftVtbl[N] = reinterpret_cast<void*>(&FtTrap<N>)), ...); ((evVtbl[N] = reinterpret_cast<void*>(&EvTrap<N>)), ...); }

// ---- arenas de memoria para CTask::operator new y CTaskAllocator::operator new (rellenas con 0xCD al reservar) ----
inline void* __cdecl TaskNew(size_t n) {
    if (!Mine()) return nullptr;
    T(json::array({ "task_new", n }));
    if (taskNext >= int(taskArena.size())) return nullptr;
    auto* p = taskArena[taskNext++].data(); std::memset(p, 0xCD, 0x100); return p;
}
inline void __cdecl TaskDelete(void* p) { if (!Mine()) return; T(json::array({ "task_delete", NameP(p) })); }
inline void* __cdecl AllocNew(size_t n) {
    if (!Mine()) return nullptr;
    T(json::array({ "allocator_new", n }));
    if (allocNext >= int(allocArena.size())) return nullptr;
    auto* p = allocArena[allocNext++].data(); std::memset(p, 0xCD, 0x40); return p;
}
inline void __cdecl AllocDelete(void* p) { if (!Mine()) return; T(json::array({ "allocator_delete", NameP(p) })); }
inline void __fastcall RegisterRef(void* self, void*, CEntity**) { if (!Mine()) return; T(json::array({ "register_reference", NameP(self) })); }
inline void __fastcall CleanupRef(void* self, void*, CEntity**) { if (!Mine()) return; T(json::array({ "cleanup_reference", NameP(self) })); }

// ---- descripcion de tareas y asignadores ----
inline size_t TaskSize(int type) {
    switch (type) {
    case TASK_COMPLEX_LEAVE_CAR_AS_PASSENGER_WAIT: return sizeof(CTaskComplexLeaveCarAsPassengerWait);
    case TASK_COMPLEX_SMART_FLEE_ENTITY:           return sizeof(CTaskComplexSmartFleeEntity);
    case TASK_COMPLEX_LEAVE_CAR:                   return sizeof(CTaskComplexLeaveCar);
    case TASK_COMPLEX_LEAVE_ANY_CAR:               return sizeof(CTaskComplexLeaveAnyCar);
    case TASK_COMPLEX_GANG_JOIN_RESPOND:           return sizeof(CTaskComplexGangJoinRespond);
    case TASK_SIMPLE_GO_TO_POINT:                  return sizeof(CTaskSimpleGoToPoint);
    case TASK_COMPLEX_STARE_AT_PED:                return sizeof(CTaskComplexStareAtPed);
    case TASK_WAIT_FOR_MATCHING_LEADER_AREA_CODES: return sizeof(CTaskSimpleWaitUntilLeaderAreaCodesMatch);
    case TASK_COMPLEX_SEEK_ENTITY:                 return sizeof(CTaskComplexSeekEntity<CEntitySeekPosCalculatorStandard>);
    case TASK_COMPLEX_GOTO_VEHICLE_AND_LEAN:       return sizeof(CTaskGoToVehicleAndLean);
    case TASK_COMPLEX_KILL_PED_ON_FOOT:            return sizeof(CTaskComplexKillPedOnFoot);
    case TASK_COMPLEX_GANG_HASSLE_VEHICLE:         return sizeof(CTaskGangHassleVehicle);
    case TASK_COMPLEX_GANG_HASSLE_PED:             return sizeof(CTaskGangHasslePed);
    case TASK_COMPLEX_SIGNAL_AT_PED:               return sizeof(CTaskComplexSignalAtPed);
    case TASK_COMPLEX_PARTNER_GREET:               return sizeof(CTaskComplexPartnerGreet);
    case TASK_COMPLEX_PARTNER_DEAL:                return sizeof(CTaskComplexPartnerDeal);
    case TASK_COMPLEX_WALK_ALONGSIDE_PED:          return sizeof(CTaskComplexWalkAlongsidePed);
    case TASK_COMPLEX_SEQUENCE:                    return sizeof(CTaskComplexSequence);
    case TASK_COMPLEX_CAR_DRIVE_WANDER:            return sizeof(CTaskComplexCarDriveWander);
    case TASK_SIMPLE_CAR_DRIVE:                    return sizeof(CTaskSimpleCarDrive);
    case TASK_COMPLEX_WANDER:                      return sizeof(CTaskComplexWanderGang);
    default:                                       return 0x10;
    }
}
inline bool IsFakeTask(const void* t) { return *reinterpret_cast<void* const*>(t) == ftVtbl.data(); }
// Palabras del objeto desde +4 (sin vptr); los bytes que siguen siendo 0xCD (relleno de la arena, nunca escritos) se ignoran.
inline json Words(const void* p, size_t size) {
    json w = json::array();
    const auto* d = static_cast<const uint32*>(p);
    for (size_t k = 1; k < size / 4; ++k) {
        const uint32 v = d[k];
        if (Known(v)) { w.push_back(Name(v)); continue; }
        uint32 m = 0;
        for (int b = 0; b < 4; ++b) { const uint8 by = uint8(v >> (8 * b)); if (by != 0xCD) m |= uint32(by) << (8 * b); }
        w.push_back(m);
    }
    return w;
}
inline json Describe(const CTask* t) {
    if (!t) return nullptr;
    if (IsFakeTask(t)) return json{ { "fake", NameP(t) } };
    const int type = int(t->GetTaskType());
    json r = { { "type", type }, { "words", Words(t, TaskSize(type)) } };
    if (type == TASK_COMPLEX_SEQUENCE) {
        json kids = json::array();
        for (auto* q : static_cast<const CTaskComplexSequence*>(t)->m_Tasks) kids.push_back(Describe(q));
        r["tasks"] = kids;
    }
    return r;
}
inline json DescribeAllocator(CTaskAllocator* a) {
    if (!a) return nullptr;
    const auto type = int(a->GetType());
    size_t size = 0x14;
    if (type == int(eTaskAllocatorType::PLAYER_COMMAND_ATTACK)) size = sizeof(CTaskAllocatorPlayerCommandAttack);
    return json{ { "type", type }, { "words", Words(a, size) } };
}
inline json DescribeEvent(const CEvent* e) {
    if (!e) return nullptr;
    const auto* raw = reinterpret_cast<const uint8*>(e);
    const int type = int(e->GetEventType());
    json r = { { "type", type } };
    if (type == EVENT_CREATE_PARTNER_TASK) r["partner"] = json::array({ At<int32>(raw, 0xC), Name(At<uintptr_t>(raw, 0x10)), At<uint8>(raw, 0x14), At<uint32>(raw, 0x18) });
    else if (type == EVENT_SCRIPT_COMMAND) r["script"] = json::array({ At<int32>(raw, 0xC), Describe(At<CTask*>(const_cast<uint8*>(raw), 0x10)), At<uint8>(raw, 0x14) });
    return r;
}

// ---- colaboradores del manejador de eventos de grupo ----
inline CPlayerPed* __cdecl FindPlayerPedStub(int32 id) {
    if (!Mine()) return realPlayer;
    playerArgs.insert(id);   // solo el conjunto de argumentos: el EXE llama dos veces donde la DLL una (sin efectos laterales)
    return reinterpret_cast<CPlayerPed*>(pedBuf[PL].data());
}
// Las tareas que llegan aqui son temporales de pila: el relleno entre campos es basura de pila distinta en el EXE y en la DLL. Igual que
// el SetTask real (que guarda task.Clone()), se describe un CLON reservado en la arena (rellena con 0xCD: el relleno que el
// constructor no escribe queda a 0xCD y se ignora). Los eventos de esa clonacion (reserva, referencias, borrado) se descartan de la
// traza: ocurren igual en las dos ejecuciones y solo consumen huecos de la arena.
inline void __fastcall SetTaskStub(void* self, void*, CPed* ped, CTask* task, void* pairs, int32 slot, int32 force) {
    if (!Mine()) return;
    json desc;
    if (task && !IsFakeTask(task)) {
        const size_t mark = trace.size();
        CTask* const copy = task->Clone();
        desc = Describe(copy);
        delete copy;
        trace.erase(trace.begin() + mark, trace.end());
    } else {
        desc = Describe(task);
    }
    T(json::array({ "set_task", NameP(self), PedIdOf(uintptr_t(ped)), desc, NameP(pairs), slot, force & 0xFF }));
}
struct FindEntry { int ped, idx, type, task; };   // idx: -2 cualquiera; type: -1 cualquiera
inline std::vector<FindEntry> findMap, activeMap;
inline CTask* __fastcall FindTaskByTypeStub(void* self, void*, int32 idx, int32 type) {
    if (!Mine()) return nullptr;
    const int ped = PedIdOf(uintptr_t(self) - 4);
    T(json::array({ "find_task", ped, idx, type }));
    for (auto& e : findMap) if (e.ped == ped && (e.idx == -2 || e.idx == idx) && (e.type == -1 || e.type == type)) return reinterpret_cast<CTask*>(ftBuf[e.task].data());
    return nullptr;
}
inline CTask* __fastcall FindActiveStub(void* self, void*, int32 type) {
    if (!Mine()) return nullptr;
    const int ped = PedIdOf(uintptr_t(self) - 4);
    T(json::array({ "find_active_task", ped, type }));
    for (auto& e : activeMap) if (e.ped == ped && (e.type == -1 || e.type == type)) return reinterpret_cast<CTask*>(ftBuf[e.task].data());
    return nullptr;
}
inline void __fastcall LookAtStub(void* self, void*, const char* purpose, CPed* ped, CEntity* ent, int32 time, int32 bone, CVector* offset, bool torso, float speed, int32 blend, int32 prio, bool force) {
    if (!Mine()) return;
    T(json::array({ "look_at", std::string(purpose ? purpose : "(null)"), PedIdOf(uintptr_t(ped)), PedIdOf(uintptr_t(ent)), time, bone, offset != nullptr, torso, F(speed), blend, prio, force }));
}
inline CEvent* __fastcall EventAddStub(void* group, void*, CEvent* ev, bool valid) {
    if (!Mine()) return nullptr;
    T(json::array({ "event_add", PedIdOf(uintptr_t(group)), DescribeEvent(ev), valid }));
    return nullptr;
}
inline uint32 respMask{};
inline CTask* __cdecl ComputeResponseStub(CPed* ped, CEvent* ev) {
    if (!Mine()) return nullptr;
    const int id = PedIdOf(uintptr_t(ped));
    T(json::array({ "compute_event_response_task", id, NameP(ev), At<int16>(reinterpret_cast<uint8*>(ev), 0xE) }));
    return id >= 0 && id < 32 && ((respMask >> id) & 1) ? reinterpret_cast<CTask*>(ftBuf[id % NFT].data()) : nullptr;
}
inline void __fastcall ComputeResponseTypeStub(void* self, void*, CPed* ped, bool inGroup) {
    if (!Mine()) return;
    const int id = PedIdOf(uintptr_t(ped));
    T(json::array({ "compute_response_task_type", NameP(self), id, inGroup }));
    At<int16>(reinterpret_cast<uint8*>(self), 0xE) = int16(0x600 + id);
}
inline int32 groupIdOfPed = -1; inline void* groupOfPed{};
// (sin traza: la DLL llama dos veces a GetGroup() donde el EXE una, sin efectos laterales)
inline CPedGroup* __cdecl PedsGroupStub(const CPed*) { if (!Mine()) return nullptr; return reinterpret_cast<CPedGroup*>(groupOfPed); }
inline int32 __cdecl GroupIdStub(const CPedGroup*) { if (!Mine()) return -1; return groupIdOfPed; }
// Los dos asignadores validados que no se ejecutan en ComputeEventResponseTasks: solo se comprueba el despacho
inline CTaskAllocator* __cdecl EnteredCarStub(const CEvent* e, CPedGroup* pg, CPed* o) { if (Mine()) T(json::array({ "dispatch_leader_entered_car", NameP(e), NameP(pg), PedIdOf(uintptr_t(o)) })); return nullptr; }
inline CTaskAllocator* __cdecl QuitEnteringStub(const CEvent* e, CPedGroup* pg, CPed* o) { if (Mine()) T(json::array({ "dispatch_leader_quit_entering_car", NameP(e), NameP(pg), PedIdOf(uintptr_t(o)) })); return nullptr; }

inline bool __cdecl EntityValidStub(CEntity* e) { return Known(uintptr_t(e)); }

// ---- construccion del mundo falso ----
inline void SetPos(int i, float x, float y, float z, bool viaMatrix) {
    auto* p = Pd(i);
    auto* mt = reinterpret_cast<float*>(matBuf[i].data() + 0x30);
    auto* pl = reinterpret_cast<float*>(p + 4);   // CPlaceable::m_placement
    At<void*>(p, 0x14) = viaMatrix ? static_cast<void*>(matBuf[i].data()) : nullptr;
    if (viaMatrix) { mt[0] = x; mt[1] = y; mt[2] = z; pl[0] = 9999.f; pl[1] = -9999.f; pl[2] = 7777.f; }
    else           { pl[0] = x; pl[1] = y; pl[2] = z; mt[0] = mt[1] = mt[2] = 5555.f; }
}
inline void SetWeapon(int i, int slot, int type) { At<uint8>(Pd(i), 0x718) = uint8(slot); At<int32>(Pd(i), 0x5A0 + size_t(slot) * 0x1C) = type; }
inline constexpr float COORD[] = { -70.f, -12.f, -8.f, -4.f, -1.f, 0.f, 1.f, 3.f, 4.f, 8.f, 9.f, 12.f, 30.f, 64.f };
inline constexpr int WEAPONS[] = { 0, 1, 22, 24, 30 };                  // 0 y 1 son cuerpo a cuerpo, el resto de fuego
inline constexpr int PEDTYPES[] = { 4, 5, 7, 8, 9, 10, 11, 12, 0, 1, 2 };
inline constexpr int MOVES[] = { 1, 1, 1, 1, 0, 2, 4, 6, 7 };

inline void InitPed(int i, Rng& r) {
    auto* p = Pd(i); std::memset(p, 0, PEDSZ);
    auto* in = intelBuf[i].data(); std::memset(in, 0, INTELSZ);
    std::memset(matBuf[i].data(), 0, 0x40);
    At<uint8>(p, 0x36) = 3;                                   // CEntity::m_nType = ENTITY_TYPE_PED
    At<void*>(p, 0x47C) = in; At<void*>(in, 0) = p;           // m_pIntelligence, CPedIntelligence::m_pPed
    At<uint8>(p, 0x2F) = uint8(r.Pick(4));                    // area code
    At<int32>(p, 0x530) = r.Chance(8) ? (r.Chance(50) ? 54 : 55) : 0;   // m_nPedState (DIE/DEAD en el 8 %)
    At<int32>(p, 0x534) = PickOf(r, MOVES);                   // m_nMoveState
    At<int32>(p, 0x598) = i == PL ? 0 : PickOf(r, PEDTYPES);  // m_nPedType
    SetWeapon(i, r.Pick(3), PickOf(r, WEAPONS));
    for (int k = 0; k < 5; ++k) At<uint32>(p, 0x4E0 + size_t(k) * 4) = (r.Chance(60) ? 1u << r.Pick(14) : 0u) | (r.Chance(30) ? 1u << r.Pick(14) : 0u);   // CAcquaintance
    SetPos(i, PickOf(r, COORD), PickOf(r, COORD), r.Chance(70) ? 0.f : PickOf(r, COORD), r.Chance(40));
}
inline void InitVehicle(int v, Rng& r) {
    auto* p = Vh(v); std::memset(p, 0, vehBuf[v].size());
    std::memset(handBuf[v].data(), 0, handBuf[v].size()); std::memset(fxBuf[v].data(), 0, fxBuf[v].size());
    At<uint8>(p, 0x36) = 2;                                                       // ENTITY_TYPE_VEHICLE
    static constexpr int ANIMGROUPS[] = { 15, 16, 0, 3, 10, 15, 16 };
    At<uint8>(handBuf[v].data(), 0xDE) = uint8(PickOf(r, ANIMGROUPS));
    At<void*>(p, 0x384) = handBuf[v].data();                                      // m_pHandlingData
    At<uint8>(fxBuf[v].data(), 0x50) = uint8(r.Chance(55) ? 0 : r.Chance(60) ? 1 : 2);   // FxSystem_c: estado (0 = FX_PLAYING)
    At<void*>(p, 0x57C) = r.Chance(80) ? static_cast<void*>(fxBuf[v].data()) : nullptr;  // m_pFireParticle
    At<int32>(p, 0x590) = r.Chance(70) ? 0 : 9;                                   // m_nVehicleType
    At<int32>(p, 0x594) = r.Chance(70) ? 0 : (r.Chance(50) ? 9 : 2);              // m_nVehicleSubType (0 = automovil)
    At<uint8>(p, 0x51B) = uint8(r.Chance(50) ? 0 : r.Next());                     // posiciones de acoso libres
}

// Mundo base. `pgPct`: probabilidad de que el grupo que se pasa sea el del jugador (hueco real de ms_groups, con instantanea).
inline void BuildWorld(Rng& r, int leaderPct, int memberPct) {
    ranges.clear(); trace = json::array(); taskNext = allocNext = 0; rngCalls = 0;
    playerArgs.clear(); findMap.clear(); activeMap.clear(); respMask = 0; groupOfPed = nullptr; groupIdOfPed = -1;
    for (auto& a : taskArena) std::memset(a.data(), 0xCD, a.size());
    for (auto& a : allocArena) std::memset(a.data(), 0xCD, a.size());
    for (int i = 0; i <= NPED; ++i) {
        InitPed(i, r);
        Reg(Pd(i), PEDSZ, i == PL ? std::string("pl") : "p" + std::to_string(i));
        Reg(intelBuf[i].data(), INTELSZ, "i" + std::to_string(i));
        Reg(matBuf[i].data(), 0x40, "m" + std::to_string(i));
    }
    for (int v = 0; v < NVEH; ++v) {
        InitVehicle(v, r);
        Reg(Vh(v), vehBuf[v].size(), "v" + std::to_string(v));
        Reg(handBuf[v].data(), handBuf[v].size(), "h" + std::to_string(v));
        Reg(fxBuf[v].data(), fxBuf[v].size(), "fx" + std::to_string(v));
    }
    for (int k = 0; k < NFT; ++k) {
        std::memset(ftBuf[k].data(), 0, ftBuf[k].size()); At<void*>(ftBuf[k].data(), 0) = ftVtbl.data();
        Reg(ftBuf[k].data(), ftBuf[k].size(), "ft" + std::to_string(k));
    }
    for (int k = 0; k < int(taskArena.size()); ++k) Reg(taskArena[k].data(), 0x100, "tk" + std::to_string(k));
    for (int k = 0; k < int(allocArena.size()); ++k) Reg(allocArena[k].data(), 0x40, "ta" + std::to_string(k));
    for (int k = 0; k < 3; ++k) { std::memset(Ev(k), 0, 0x50); At<void*>(Ev(k), 0) = evVtbl.data(); Reg(Ev(k), 0x50, "e" + std::to_string(k)); }
    std::memset(groupEvBuf.data(), 0, groupEvBuf.size()); At<void*>(groupEvBuf.data(), 0) = evVtbl.data(); Reg(groupEvBuf.data(), groupEvBuf.size(), "ge");
    // jugador falso: tipo PLAYER1 (InitPed lo deja en 0), datos del jugador con el id del grupo
    std::memset(playerData.data(), 0, playerData.size()); At<int32>(playerData.data(), 0x38) = PGID;
    At<void*>(Pd(PL), 0x480) = playerData.data(); Reg(playerData.data(), playerData.size(), "pd");
    // grupo propio
    std::memset(groupBuf.data(), 0, groupBuf.size());
    auto* g = Grp(); Reg(g, sizeof(CPedGroup), "G");
    At<void*>(g, 0x08) = g;
    for (int k = 0; k < 7; ++k) At<CPed*>(g, 0xC + size_t(k) * 4) = r.Chance(memberPct) ? P(k) : nullptr;
    At<CPed*>(g, 0x28) = r.Chance(leaderPct) ? P(7) : nullptr;
    At<float>(g, 0x2C) = 60.f;
    At<void*>(g, 0x30) = g;
    At<uint8>(g, 0x2D0) = r.Chance(35) ? 1 : 0;
    At<CPed*>(g, 0x00) = r.Chance(20) ? P(r.Pick(NPED)) : nullptr;
    // grupo del jugador: hueco real de CPedGroups::ms_groups (restaurado al terminar el banco)
    auto* pgp = PGrp(); std::memset(pgp, 0, sizeof(CPedGroup)); Reg(pgp, sizeof(CPedGroup), "PG");
    At<void*>(pgp, 0x08) = pgp;
    for (int k = 0; k < 7; ++k) At<CPed*>(pgp, 0xC + size_t(k) * 4) = r.Chance(25) ? P(8 + r.Pick(4)) : nullptr;
    At<CPed*>(pgp, 0x28) = r.Chance(40) ? P(PL) : nullptr;
    At<void*>(pgp, 0x30) = pgp;
    At<uint8>(pgp, 0x2D0) = r.Chance(30) ? 1 : 0;
}
inline CPedGroup* PickGroup(Rng& r, int playerPct) { return reinterpret_cast<CPedGroup*>(r.Chance(playerPct) ? PGrp() : Grp()); }
inline CPed* Orig(Rng& r) { return r.Chance(90) ? P(8) : nullptr; }
// escribe la tarea del evento y los campos comunes
inline void SetEv(int taskId, int type = 0, bool editable = true) {
    auto* e = Ev(0);
    At<int16>(e, 0xE) = int16(taskId); At<int32>(e, 0x3C) = type; At<uint8>(e, 0x3D) = editable ? 1 : 0;
    At<uint8>(e, 0x8) = 1;
}
inline constexpr int TASKIDS[] = { 1502, 1503, 1505, 1510, 1516, 1504, 1509, 0 };
inline constexpr float DIST[] = { 0.f, 1.f, 2.f, 3.f, 3.9f, 4.f, 4.1f, 5.f, 6.f, 7.f, 7.9f, 8.f, 8.1f, 9.f, 9.9f, 10.f, 10.1f, 11.9f, 12.f, 12.1f, 20.f };
inline constexpr float SMALL[] = { 0.f, 0.f, 0.f, 0.3f, 1.7f, 2.f, 4.f, 6.f };
inline float NaNf() { return std::bit_cast<float>(0x7FC00000u); }

// ---- ejecucion de una funcion del manejador con un caso ----
// El resultado devuelto se describe y se borra; despues se recoge la traza y se resume la memoria de todos los objetos falsos.
inline json CollectGroup(json result, CTaskAllocator* alloc) {
    json out = { { "result", std::move(result) } };
    out["allocator"] = DescribeAllocator(alloc);
    // El destructor deleting del EXE (0x69DB00) libera INLINE en el pool real 0xB744BC con (ptr - base)>>5: con memoria de la arena eso es una
    // violacion de acceso (0x69DB22). Solo se ejecuta el destructor (flag 0, sin liberar); la arena se reinicia en cada ejecucion.
    if (alloc) alloc->~CTaskAllocator();
    json peds = json::array();
    for (int i = 0; i <= NPED; ++i) peds.push_back(Hex(Fnv(Pd(i), PEDSZ)));
    json vehs = json::array();
    for (int v = 0; v < NVEH; ++v) vehs.push_back(Hex(Fnv(Vh(v), vehBuf[v].size())));
    out["peds"] = peds; out["vehicles"] = vehs;
    out["group"] = Hex(Fnv(Grp(), sizeof(CPedGroup))); out["player_group"] = Hex(Fnv(PGrp(), sizeof(CPedGroup)));
    out["events"] = json::array({ Hex(Fnv(Ev(0), 0x50)), Hex(Fnv(Ev(1), 0x50)) });
    out["fake_tasks"] = Hex(Fnv(ftBuf.data(), sizeof(ftBuf)));
    out["group_m_pPed"] = Name(At<uintptr_t>(Grp(), 0));
    json pa = json::array(); for (int a : playerArgs) pa.push_back(a);
    out["player_args"] = pa;
    out["trace"] = trace;
    return out;
}

using AllocFn3 = CTaskAllocator* (__cdecl*)(const void*, CPedGroup*, CPed*);
inline AllocFn3 Ev3(uintptr_t a) { return reinterpret_cast<AllocFn3>(a); }
using AllocFn4 = CTaskAllocator* (__cdecl*)(CPedGroup*, CPed*, CPed*, int32);
inline AllocFn4 Th4(uintptr_t a) { return reinterpret_cast<AllocFn4>(a); }
using AllocFn3p = CTaskAllocator* (__cdecl*)(CPedGroup*, CPed*, CPed*);
inline AllocFn3p Th3(uintptr_t a) { return reinterpret_cast<AllocFn3p>(a); }

inline json RunGroupCase(int fi, int I, ReversibleHooks::ReversibleHook::Base* hook, const Fn& fn, bool cand, bool& switches) {
    Rng r(uint64(I) * 7919u + uint64(fi) * 104729u + 17u);
    runSeed = uint32(I) * 2654435761u + uint32(fi) * 40503u + 7u;
    json result = nullptr; CTaskAllocator* alloc = nullptr;
    const unsigned savedFp = _controlfp(0, 0);
    // ChooseFn: despues de preparar el fixture se conmuta el hook (la comprobacion del salto va despues de State())
    auto go = [&](auto&& call) {
        hook->State(cand);
        switches = ReversingProbe::JumpsToDll(fn.address) == cand;
        _controlfp(_PC_24, _MCW_PC);
        call();
        _controlfp(savedFp, _MCW_PC | _MCW_RC);
    };
    switch (fi) {
    case 0: {   // IsKillTaskAppropriate(pg, threat)
        BuildWorld(r, 80, 60);
        if (r.Chance(35)) for (int i = 0; i <= NPED; ++i) SetWeapon(i, r.Pick(3), r.Chance(70) ? 0 : 1);
        if (r.Chance(25)) SetWeapon(9, 1, 22);
        auto* pg = PickGroup(r, 20); bool ret{};
        go([&] { ret = reinterpret_cast<bool(__cdecl*)(CPedGroup*, CPed*)>(fn.address)(pg, P(9)); });
        result = ret; break;
    }
    case 1: {   // ComputeWalkAlongsideResponse(pg, ped1, ped2)
        BuildWorld(r, 80, 70);
        for (int i = 0; i < NPED; ++i) SetPos(i, PickOf(r, COORD), PickOf(r, COORD), PickOf(r, SMALL), r.Chance(40));
        if (r.Chance(8)) SetPos(r.Pick(7), NaNf(), 0.f, 0.f, r.Chance(50));
        auto* p1 = r.Chance(92) ? P(9) : nullptr; auto* pg = PickGroup(r, 20);
        go([&] { alloc = Th3(fn.address)(pg, p1, P(10)); });
        break;
    }
    case 2: {   // ComputeStareResponse(pg, stareAt, originator, timeout, bias)
        BuildWorld(r, 88, 65);
        for (int i = 0; i < NPED; ++i) SetPos(i, PickOf(r, COORD), PickOf(r, COORD), PickOf(r, SMALL), r.Chance(40));
        SetPos(7, 0.f, 0.f, 0.f, r.Chance(40));
        SetPos(9, PickOf(r, DIST), PickOf(r, SMALL), PickOf(r, SMALL), r.Chance(40));
        if (r.Chance(5)) SetPos(9, NaNf(), 0.f, 0.f, false);
        auto* stare = r.Chance(92) ? P(9) : nullptr; auto* pg = PickGroup(r, 20);
        static constexpr int TIMEOUTS[] = { 0, 1000, 99999999, -5, 4000 };
        static constexpr int BIASES[] = { 0, 1, 1000, 5000, 33000, 10000000, -7 };
        const int to = PickOf(r, TIMEOUTS), bias = PickOf(r, BIASES);
        go([&] { alloc = reinterpret_cast<CTaskAllocator * (__cdecl*)(CPedGroup*, CPed*, CPed*, int32, int32)>(fn.address)(pg, stare, Orig(r), to, bias); });
        break;
    }
    case 3: case 4: {   // ComputeResponseVehicleDamage / ComputeResponseShotFired
        BuildWorld(r, 85, 60);
        static constexpr int IDS[] = { 1502, 1503, 1505, 1510, 1516, 1504, 0 };
        SetEv(PickOf(r, IDS), fi == 3 ? 41 : 15, r.Chance(60));
        void* src = r.Chance(60) ? static_cast<void*>(P(9)) : r.Chance(30) ? static_cast<void*>(Vh(1)) : r.Chance(50) ? static_cast<void*>(P(PL)) : nullptr;
        if (fi == 3) { At<void*>(Ev(0), 0x14) = Vh(0); At<void*>(Ev(0), 0x18) = src; } else At<void*>(Ev(0), 0x14) = src;
        respMask = r.Next();
        for (int i = 0; i < NPED; ++i) SetPos(i, PickOf(r, COORD), PickOf(r, COORD), PickOf(r, SMALL), r.Chance(40));
        SetPos(7, 0.f, 0.f, 0.f, false); SetPos(9, PickOf(r, DIST), 0.f, 0.f, false);
        auto* pg = PickGroup(r, 15);
        go([&] { alloc = Ev3(fn.address)(Ev(0), pg, Orig(r)); });
        break;
    }
    case 5: {   // ComputeResponseSexyPed
        BuildWorld(r, 85, 60);
        static constexpr int IDS[] = { 1504, 1508, 1508, 1504, 0, 1502 };
        SetEv(PickOf(r, IDS), 20);
        auto* pg = PickGroup(r, 30);
        CPed* sexy = P(9);
        const int pick = r.Pick(7);
        if (pick == 0) sexy = nullptr; else if (pick == 1) sexy = P(7); else if (pick == 2) sexy = P(r.Pick(7)); else if (pick == 3) sexy = P(PL); else if (pick == 4) sexy = P(8);
        At<CPed*>(Ev(0), 0x14) = sexy;
        for (int i = 0; i < NPED; ++i) SetPos(i, PickOf(r, COORD), PickOf(r, COORD), PickOf(r, SMALL), r.Chance(40));
        SetPos(7, 0.f, 0.f, 0.f, false);
        if (r.Chance(60) && sexy) SetPos(PedIdOf(uintptr_t(sexy)) < 0 ? 9 : PedIdOf(uintptr_t(sexy)), PickOf(r, DIST), 0.f, 0.f, false);
        go([&] { alloc = Ev3(fn.address)(Ev(0), pg, Orig(r)); });
        break;
    }
    case 6: {   // ComputeResponseSeenCop
        BuildWorld(r, 85, 60);
        static constexpr int IDS[] = { 1502, 1505, 1515, 0, 1504, 1515, 1505 };
        const int id = PickOf(r, IDS);
        SetEv(id, 72);
        At<CPed*>(Ev(0), 0x14) = (id == 1502 || r.Chance(88)) ? P(9) : nullptr;
        respMask = r.Next();
        auto* pg = PickGroup(r, 15);
        go([&] { alloc = Ev3(fn.address)(Ev(0), pg, Orig(r)); });
        break;
    }
    case 7: {   // ComputeResponsePlayerCommand
        BuildWorld(r, 85, 60);
        static constexpr int CMDS[] = { 0, 2, 1, 3, 0, 2 };
        SetEv(1502, 71);
        At<int32>(Ev(0), 0x14) = PickOf(r, CMDS);
        At<CPed*>(Ev(0), 0x18) = r.Chance(75) ? P(9) : nullptr;
        groupIdOfPed = r.Chance(50) ? 3 : -1; groupOfPed = r.Chance(60) ? static_cast<void*>(Grp()) : nullptr;
        auto* pg = PickGroup(r, 15);
        go([&] { alloc = Ev3(fn.address)(Ev(0), pg, Orig(r)); });
        break;
    }
    case 8: {   // ComputeResponsePedThreat
        BuildWorld(r, 85, 60);
        static constexpr int IDS[] = { 1502, 1504, 1505, 1509, 1510, 1516, 1517, 0, 1503 };
        SetEv(PickOf(r, IDS), 36, r.Chance(60));
        CPed* ped = P(9);
        const int pick = r.Pick(6);
        if (pick == 0) ped = nullptr; else if (pick == 1) ped = P(7); else if (pick == 2) ped = P(r.Pick(7)); else if (pick == 3) ped = P(8);
        At<CPed*>(Ev(0), 0x14) = ped; respMask = r.Next();
        for (int i = 0; i < NPED; ++i) SetPos(i, PickOf(r, COORD), PickOf(r, COORD), PickOf(r, SMALL), r.Chance(40));
        SetPos(7, 0.f, 0.f, 0.f, false); SetPos(9, PickOf(r, DIST), 0.f, 0.f, false);
        auto* pg = PickGroup(r, 15);
        go([&] { alloc = Ev3(fn.address)(Ev(0), pg, Orig(r)); });
        break;
    }
    case 9: case 25: case 29: {   // PedFriend / (Greet y DoDeal comparten fixture, ver mas abajo)
        BuildWorld(r, 90, 60);
        static constexpr int IDS[] = { 1506, 1507, 1506, 1507, 0, 1502 };
        // activas: el peaton objetivo tiene tarea de deambular
        CPed* ped = P(9);
        if (fi == 9) {
            SetEv(PickOf(r, IDS), 38);
            const int pick = r.Pick(8);
            if (pick == 0) ped = nullptr; else if (pick == 1) ped = At<CPed*>(Grp(), 0) ? At<CPed*>(Grp(), 0) : P(11); else if (pick == 2) ped = P(r.Pick(7)); else if (pick == 3) ped = P(7); else if (pick == 4) ped = P(PL); else if (pick == 5) ped = At<CPed*>(PGrp(), 0xC) ? At<CPed*>(PGrp(), 0xC) : P(10);
            At<CPed*>(Ev(0), 0x14) = ped;
            if (r.Chance(25)) At<CPed*>(Grp(), 0) = ped;
        } else if (r.Chance(5)) ped = nullptr;
        if (ped) { if (r.Chance(85)) activeMap.push_back({ PedIdOf(uintptr_t(ped)), -3, int(TASK_COMPLEX_WANDER), 0 }); }
        if (r.Chance(15)) { At<int32>(Pd(7), 0x534) = 0; }
        if (r.Chance(12) && ped) At<int32>(reinterpret_cast<uint8*>(ped), 0x598) = r.Chance(50) ? 0 : 1;
        // distancias del objetivo (origen) a los seguidores: 16 y 100 (cuadrados de 4 y 10) y orden de suma z*z + y*y + x*x
        static constexpr float FD[] = { 0.f, 2.f, 3.9f, 4.f, 4.1f, 6.f, 9.f, 10.f, 10.1f, 12.f };
        if (ped) {
            const int pid = PedIdOf(uintptr_t(ped));
            if (pid >= 0) SetPos(pid, 0.f, 0.f, 0.f, r.Chance(40));
            for (int k = 0; k < 7; ++k) if (k != pid) SetPos(k, PickOf(r, FD), PickOf(r, SMALL), PickOf(r, SMALL), r.Chance(40));
        }
        auto* pg = fi == 9 ? PickGroup(r, 25) : PickGroup(r, 0);
        if (fi == 9) {
            go([&] { alloc = Ev3(fn.address)(Ev(0), pg, Orig(r)); });
        } else {
            go([&] { alloc = Th3(fn.address)(pg, ped, Orig(r)); });
        }
        break;
    }
    case 10: {   // ComputeResponseNewGangMember
        BuildWorld(r, 85, 60);
        SetEv(0, 90);
        CPed* m = P(9);
        const int pick = r.Pick(6);
        if (pick == 0) m = nullptr; else if (pick == 1) m = P(r.Pick(7)); else if (pick == 2) m = P(7); else if (pick == 3) m = P(PL);
        At<CPed*>(Ev(0), 0xC) = m;
        if (r.Chance(35)) At<CPed*>(Grp(), 0x28) = P(PL);
        if (r.Chance(10)) At<CPed*>(Grp(), 0x28) = nullptr;
        SetPos(PL, PickOf(r, COORD), PickOf(r, COORD), PickOf(r, COORD), r.Chance(40));
        auto* pg = PickGroup(r, 0);
        go([&] { alloc = Ev3(fn.address)(Ev(0), pg, Orig(r)); });
        break;
    }
    case 11: {   // ComputeResponseLeaderExitedCar: el lider siempre existe (el EXE lo desreferencia)
        BuildWorld(r, 100, 55);
        SetEv(0, 55);
        for (int k = 0; k < 3; ++k) {   // tareas falsas con vehiculo en +0xC
            At<void*>(ftBuf[k].data(), 0xC) = r.Chance(85) ? static_cast<void*>(Vh(r.Pick(NVEH))) : nullptr;
        }
        auto* leaderVeh = r.Chance(70) ? Vh(0) : nullptr;
        At<void*>(Pd(7), 0x58C) = leaderVeh;
        for (int k = 0; k < 7; ++k) {
            auto* p = Pd(k);
            At<void*>(p, 0x58C) = r.Chance(55) ? static_cast<void*>(r.Chance(60) ? Vh(0) : Vh(1 + r.Pick(2))) : nullptr;
            At<uint32>(p, 0x46C) = r.Chance(60) ? 0x100u : 0u;
            if (r.Chance(45)) findMap.push_back({ k, -2, 718, r.Pick(3) });          // espera de pasajero (0x2CE)
            if (r.Chance(45)) findMap.push_back({ k, -2, 700, r.Pick(3) });          // subir como pasajero (0x2BC)
        }
        auto* pg = PickGroup(r, 0);
        go([&] { alloc = Ev3(fn.address)(Ev(0), pg, Orig(r)); });
        break;
    }
    case 12: {   // ComputeResponseLeaderEnterExit: las zonas coinciden (ProcessPed devuelve true; el otro camino llega a CAnimManager)
        BuildWorld(r, 85, 65);
        SetEv(0, 77);
        *reinterpret_cast<int32*>(0x96A7CC) = 0;   // CEntryExitManager::ms_exitEnterState (se restaura con las instantaneas)
        const uint8 area = uint8(r.Pick(4));
        for (int i = 0; i <= NPED; ++i) At<uint8>(Pd(i), 0x2F) = area;
        for (int k = 0; k < 7; ++k) At<uint32>(Pd(k), 0x46C) = r.Chance(40) ? 0x100u : 0u;
        for (int k = 0; k <= 7; ++k) At<uint32>(Pd(k), 0x1C) = r.Chance(50) ? 0u : 0x10u;   // CEntity::m_bUsesCollision (bit 0) inicial
        auto* pg = PickGroup(r, 0);
        go([&] { alloc = Ev3(fn.address)(Ev(0), pg, Orig(r)); });
        break;
    }
    case 13: case 16: case 17: {   // GunAimedAt / Danger / Damage
        BuildWorld(r, 85, 60);
        static constexpr int IDS13[] = { 1502, 1505, 1510, 0, 1502, 1510 };
        static constexpr int IDS16[] = { 1505, 0, 1502, 1505 };
        static constexpr int IDS17[] = { 1502, 1503, 1505, 1510, 0, 1502 };
        SetEv(fi == 13 ? PickOf(r, IDS13) : fi == 16 ? PickOf(r, IDS16) : PickOf(r, IDS17), fi == 13 ? 31 : fi == 16 ? 75 : 9, r.Chance(60));
        void* src = r.Chance(45) ? static_cast<void*>(P(9)) : r.Chance(50) ? static_cast<void*>(P(PL)) : r.Chance(50) ? static_cast<void*>(Vh(0)) : nullptr;
        At<void*>(Ev(0), 0x14) = src;
        respMask = r.Next();
        for (int i = 0; i < NPED; ++i) At<int32>(Pd(i), 0x598) = r.Chance(70) ? PickOf(r, PEDTYPES) : 2;
        auto* pg = PickGroup(r, 15);
        go([&] { alloc = Ev3(fn.address)(Ev(0), pg, Orig(r)); });
        break;
    }
    case 14: {   // ComputeResponseGather
        BuildWorld(r, 85, 65);
        SetEv(0, 76);
        auto* pg = PickGroup(r, 10);
        CPed* orig = r.Chance(90) ? P(8) : P(PL);
        go([&] { alloc = Ev3(fn.address)(Ev(0), pg, orig); });
        break;
    }
    case 15: {   // ComputeResponseDraggedOutCar
        BuildWorld(r, 85, 60);
        static constexpr int IDS[] = { 1502, 1502, 1505, 1510, 1511, 0, 1502 };
        const int id = PickOf(r, IDS);
        SetEv(id, 7, r.Chance(60));
        void* jacker = r.Chance(40) ? static_cast<void*>(P(9)) : r.Chance(60) ? static_cast<void*>(P(PL)) : r.Chance(50) ? static_cast<void*>(Vh(0)) : nullptr;
        At<void*>(Ev(0), 0x14) = jacker;
        if (id == 1511) At<CPed*>(Grp(), 0x28) = P(7);
        respMask = r.Next();
        for (int i = 0; i < NPED; ++i) At<int32>(Pd(i), 0x598) = r.Chance(70) ? PickOf(r, PEDTYPES) : 2;
        auto* pg = PickGroup(r, 0);
        go([&] { alloc = Ev3(fn.address)(Ev(0), pg, Orig(r)); });
        break;
    }
    case 18: {   // ComputeMemberResponses
        BuildWorld(r, 85, 65);
        SetEv(r.Chance(50) ? 1510 : 0, 9, r.Chance(55));
        respMask = r.Next();
        auto* pg = PickGroup(r, 0);
        go([&] { alloc = Ev3(fn.address)(Ev(0), pg, Orig(r)); });
        break;
    }
    case 19: {   // ComputeLeanOnVehicleResponse
        BuildWorld(r, 80, 60);
        SetEv(0, 92);
        At<void*>(Ev(0), 0xC) = Vh(r.Pick(NVEH));
        At<int32>(Ev(0), 0x10) = r.Chance(50) ? 5000 : r.Pick(300);
        auto* pg = PickGroup(r, 0);
        go([&] { alloc = Ev3(fn.address)(Ev(0), pg, Orig(r)); });
        break;
    }
    case 20: case 21: {   // ComputeKillThreatsBasicResponse / ComputeKillPlayerBasicResponse (pg, threat, originator, bool)
        BuildWorld(r, 85, 60);
        if (r.Chance(35)) for (int i = 0; i <= NPED; ++i) SetWeapon(i, r.Pick(3), r.Chance(70) ? 0 : 1);
        for (int i = 0; i < NPED; ++i) SetPos(i, PickOf(r, COORD), PickOf(r, COORD), PickOf(r, SMALL), r.Chance(40));
        SetPos(7, 0.f, 0.f, 0.f, false); SetPos(9, PickOf(r, DIST), 0.f, 0.f, false);
        auto* pg = PickGroup(r, 15); CPed* threat = (fi == 21 && r.Chance(10)) ? nullptr : P(9);
        const bool dmg = r.Chance(50);
        go([&] { alloc = reinterpret_cast<CTaskAllocator * (__cdecl*)(CPedGroup*, CPed*, CPed*, bool)>(fn.address)(pg, threat, Orig(r), dmg); });
        break;
    }
    case 22: {   // ComputeHassleThreatResponse
        BuildWorld(r, 92, 60);
        for (int i = 0; i < NPED; ++i) SetPos(i, PickOf(r, COORD), PickOf(r, COORD), PickOf(r, SMALL), r.Chance(40));
        SetPos(7, 0.f, 0.f, 0.f, r.Chance(40));
        static constexpr float D12[] = { 0.f, 6.f, 11.f, 11.9f, 12.f, 12.1f, 13.f, 20.f };
        SetPos(9, PickOf(r, D12), PickOf(r, SMALL), PickOf(r, SMALL), r.Chance(40));
        if (r.Chance(4)) SetPos(9, NaNf(), 0.f, 0.f, false);
        if (r.Chance(60)) { At<uint32>(Pd(9), 0x46C) = 0x100u; At<void*>(Pd(9), 0x58C) = Vh(r.Pick(NVEH)); }
        auto* pg = PickGroup(r, 15); CPed* threat = r.Chance(95) ? P(9) : nullptr;
        const bool aggressive = r.Chance(50);
        go([&] { alloc = reinterpret_cast<CTaskAllocator * (__cdecl*)(CPedGroup*, CPed*, CPed*, bool)>(fn.address)(pg, threat, Orig(r), aggressive); });
        break;
    }
    case 23: {   // ComputeHassleSexyPedResponse
        BuildWorld(r, 85, 65);
        auto* pg = PickGroup(r, 15); CPed* sexy = r.Chance(92) ? P(9) : nullptr;
        go([&] { alloc = Th3(fn.address)(pg, sexy, Orig(r)); });
        break;
    }
    case 24: {   // ComputeHandSignalResponse
        BuildWorld(r, 85, 65);
        for (int i = 0; i <= 7; ++i) if (r.Chance(25)) At<int32>(Pd(i), 0x598) = r.Chance(50) ? 0 : 1;
        auto* pg = PickGroup(r, 15); CPed* at = r.Chance(92) ? P(9) : nullptr;
        go([&] { alloc = Th3(fn.address)(pg, at, Orig(r)); });
        break;
    }
    case 26: {   // ComputeFleePedResponse(pg, threat, originator, bool)
        BuildWorld(r, 92, 60);
        for (int i = 0; i < NPED; ++i) SetPos(i, PickOf(r, COORD), PickOf(r, COORD), PickOf(r, SMALL), r.Chance(40));
        SetPos(7, 0.f, 0.f, 0.f, r.Chance(40));
        static constexpr float D8[] = { 0.f, 4.f, 7.f, 7.9f, 8.f, 8.1f, 9.f, 12.f, 20.f };
        SetPos(9, PickOf(r, D8), PickOf(r, SMALL), PickOf(r, SMALL), r.Chance(40));
        if (r.Chance(4)) SetPos(9, NaNf(), 0.f, 0.f, false);
        auto* pg = PickGroup(r, 15); CPed* threat = r.Chance(94) ? P(9) : nullptr;
        const bool dmg = r.Chance(50);
        go([&] { alloc = reinterpret_cast<CTaskAllocator * (__cdecl*)(CPedGroup*, CPed*, CPed*, bool)>(fn.address)(pg, threat, Orig(r), dmg); });
        break;
    }
    case 27: {   // ComputeDrivebyResponse
        BuildWorld(r, 85, 60);
        auto* pg = PickGroup(r, 15); CPed* threat = r.Chance(90) ? P(9) : nullptr;
        go([&] { alloc = Th3(fn.address)(pg, threat, Orig(r)); });
        break;
    }
    case 28: {   // ComputeEventResponseTasks(const CEventGroupEvent&, pg)
        BuildWorld(r, 95, 60);
        static constexpr int TYPES[] = { 7, 9, 15, 20, 31, 36, 37, 38, 39, 41, 54, 55, 57, 71, 72, 73, 75, 76, 77, 90, 92, 0, 2, 1000, 8, 56 };
        const int type = PickOf(r, TYPES);
        static constexpr int IDS[] = { 1502, 1503, 1504, 1505, 1506, 1507, 1508, 1509, 1510, 1511, 1515, 1516, 1517, 0 };
        SetEv(PickOf(r, IDS), type, r.Chance(60));
        auto* e = Ev(0);
        void* ptrs[] = { P(9), P(2), P(PL), Vh(0), P(7) };
        void* a14 = PickOf(r, ptrs);
        if (type == 72 || type == 7 || type == 36 || type == 37 || type == 38 || type == 39) { if (r.Chance(90)) a14 = P(9); }
        At<void*>(e, 0x14) = a14;
        At<void*>(e, 0x18) = r.Chance(50) ? static_cast<void*>(P(9)) : static_cast<void*>(Vh(1));
        if (type == 92) { At<void*>(e, 0xC) = Vh(0); At<int32>(e, 0x10) = r.Pick(5000); }
        if (type == 90) At<void*>(e, 0xC) = r.Chance(70) ? static_cast<void*>(P(9)) : static_cast<void*>(P(1));
        if (type == 77) {   // LeaderEnterExit: igual que el caso 12, solo el camino con zonas coincidentes (el otro llega a CAnimManager::BlendAnimation con un ped falso)
            *reinterpret_cast<int32*>(0x96A7CC) = 0;
            const uint8 area = uint8(r.Pick(4));
            for (int i = 0; i <= NPED; ++i) At<uint8>(Pd(i), 0x2F) = area;
        }
        if (type == 71 || type == 76) { At<int32>(e, 0x14) = r.Chance(50) ? 0 : 2; At<void*>(e, 0x18) = r.Chance(70) ? static_cast<void*>(P(9)) : nullptr; }
        if (type == 55 || type == 7) At<CPed*>(Grp(), 0x28) = P(7);
        if (type == 7 && r.Chance(50)) At<void*>(e, 0x14) = P(9);
        At<void*>(groupEvBuf.data(), 0xC) = r.Chance(85) ? static_cast<void*>(P(8)) : static_cast<void*>(P(9));
        At<void*>(groupEvBuf.data(), 0x10) = e;
        respMask = r.Next();
        groupIdOfPed = 3; groupOfPed = nullptr;
        auto* pg = PickGroup(r, 0);
        go([&] { alloc = reinterpret_cast<CTaskAllocator * (__cdecl*)(const void*, CPedGroup*)>(fn.address)(groupEvBuf.data(), pg); });
        break;
    }
    default: break;
    }
    return CollectGroup(std::move(result), alloc);
}

// =====================================================================================================================
// Parte 2: CGangWars. Pools de peatones/vehiculos falsos (el puntero global solo se intercambia durante cada llamada),
// zonas/poblacion/streaming reales con instantanea y restauracion, colaboradores simulados.
// =====================================================================================================================
struct FakePool { void* storage; uint8* states; uint32 capacity; int32 lastFree; uint8 owns; uint8 deal; uint8 pad[2]; };
static_assert(sizeof(FakePool) == 0x14);
inline FakePool pedPool, vehPool;
inline CVector playerCoors{};
inline std::set<int> coorsArgs;
inline uint32 relCount{};                                                  // retorno simulado de ReleasePedsInAttackWave
inline int blipCalls{}, nPed{}, nCar{};
inline float stats[0x120]{};
inline int ioMode{};
inline std::vector<uint8> ioIn; inline size_t ioPos{};
inline uint32 stubFlags{};                                                  // resultados simulados (bits por colaborador)
inline int pickCalls{};

inline int GwIdOf(uintptr_t v) { return PedIdOf(v); }

// ---- colaboradores comunes ----
inline CVector* __cdecl CoorsStub(CVector* out, int32 id) { *out = playerCoors; if (Mine()) coorsArgs.insert(id); return out; }
inline void __cdecl FillZonesStub(bool b) { if (Mine()) T(json::array({ "fill_zones", b })); }
inline const GxtChar* __fastcall TextGetStub(void* self, void*, const char* key) { if (Mine()) T(json::array({ "text_get", std::string(key) })); return reinterpret_cast<const GxtChar*>(key); }
inline void __cdecl MessageStub(const GxtChar* t, uint32 ms, uint16 flag, bool previous) { if (Mine()) T(json::array({ "message", std::string(reinterpret_cast<const char*>(t)), ms, flag, previous })); }
inline void __cdecl BriefStub(const GxtChar* t, int32 a, int32 b, int32 c, int32 d, int32 e, int32 f, const GxtChar* g) {
    if (Mine()) T(json::array({ "brief", std::string(reinterpret_cast<const char*>(t)), a, b, c, d, e, f, g != nullptr }));
}
inline void __cdecl HelpStub(const GxtChar* t, bool a, bool b, bool c) { if (Mine()) T(json::array({ "help", std::string(reinterpret_cast<const char*>(t)), a, b, c })); }
inline void __cdecl StatIncStub(int32 id, float v) { if (Mine()) T(json::array({ "stat_inc", id, F(v) })); }
inline float __cdecl StatGetStub(int32 id) { if (!Mine()) return 0.f; T(json::array({ "stat_get", id })); return stats[id & 0xFF]; }
inline void __cdecl StatSetStub(int32 id, float v) { if (!Mine()) return; T(json::array({ "stat_set", id, F(v) })); stats[id & 0xFF] = v; }
inline int32 __cdecl SetCoordBlipStub(int32 type, CVector pos, uint32 color, int32 display, const char*) { if (!Mine()) return -1; T(json::array({ "coord_blip", type, V(pos), color, display })); return 1000 + blipCalls++; }
inline int32 __cdecl SetEntityBlipStub(int32 type, int32 ref, uint32 color, int32 display) { if (!Mine()) return -1; T(json::array({ "entity_blip", type, ref, color, display })); return 2000 + blipCalls++; }
inline void __cdecl BlipSpriteStub(int32 blip, int32 sprite) { if (Mine()) T(json::array({ "blip_sprite", blip, sprite })); }
inline void __cdecl BlipScaleStub(int32 blip, int32 scale) { if (Mine()) T(json::array({ "blip_scale", blip, scale })); }
inline void __cdecl BlipColourStub(int32 blip, uint32 colour) { if (Mine()) T(json::array({ "blip_colour", blip, colour })); }
inline void __cdecl ClearBlipEntityStub(int32 type, int32 ref) { if (Mine()) T(json::array({ "clear_blip_entity", type, ref })); }
inline void __fastcall CreatedByStub(void* ped, void*, int32 by) { if (!Mine()) return; T(json::array({ "created_by", GwIdOf(uintptr_t(ped)), by })); At<uint8>(reinterpret_cast<uint8*>(ped), 0x484) = uint8(by); }
inline void __cdecl WorldAddStub(CEntity* e) { if (Mine()) T(json::array({ "world_add", GwIdOf(uintptr_t(e)) })); }
inline void* __cdecl PedNewStub(unsigned n) {
    if (!Mine()) return nullptr;
    T(json::array({ "ped_new", n }));
    if (nPed >= NGW - 8) return nullptr;
    const int k = 8 + nPed++;                                  // los peatones nuevos ocupan los huecos 8..NGW-1 del pool falso
    std::memset(gwPed[k].data(), 0, PEDSTRIDE); gwPedState[k] = 0;
    return gwPed[k].data();
}
inline CPed* __fastcall CivilianStub(void* self, void*, int32 type, int32 model) {
    if (!Mine()) return reinterpret_cast<CPed*>(self);
    const int k = GwIdOf(uintptr_t(self)) - 100;
    T(json::array({ "civilian", k + 100, type, model }));
    if (k >= 0 && k < NGW) { At<void*>(gwPed[k].data(), 0x47C) = gwIntel[k].data(); At<void*>(gwIntel[k].data(), 0) = gwPed[k].data(); At<uint8>(gwPed[k].data(), 0x36) = 3; }
    return reinterpret_cast<CPed*>(self);
}
inline void __fastcall GiveDelayedStub(void* ped, void*, int32 weapon, uint32 ammo) { if (Mine()) T(json::array({ "give_delayed_weapon", GwIdOf(uintptr_t(ped)), weapon, ammo })); }
inline void __fastcall SetCurrentWeaponStub(void* ped, void*, int32 weapon) { if (Mine()) T(json::array({ "set_current_weapon", GwIdOf(uintptr_t(ped)), weapon })); }
inline float __cdecl GroundZStub(CVector p, bool* found, CEntity** entity) { if (!Mine()) return 0.f; T(json::array({ "ground_z", V(p), found != nullptr, entity != nullptr })); return std::floor(p.x * 0.5f) + 1.5f; }
inline int32 __cdecl PickupNative(CVector p, uint32 model, ePickupType type, uint32 ammo, uint32 money, bool empty, char* label) {
    if (!Mine()) return 0;
    T(json::array({ "pickup", V(p), model, int(type), ammo, money, empty, label != nullptr })); return 1;
}
inline tPickupReference __cdecl PickupDll(CVector p, uint32 model, ePickupType type, uint32 ammo, uint32 money, bool empty, char* label) { return tPickupReference(PickupNative(p, model, type, ammo, money, empty, label)); }
inline int32 __cdecl PickCarStub(int32 gang) { if (!Mine()) return -1; T(json::array({ "pick_gang_car", gang })); return (stubFlags >> 12) & 1 ? -1 : 440 + (gang & 3); }
inline void* __cdecl CreateCarStub(int32 model, CVector pos, bool cleanup) {
    if (!Mine()) return nullptr;
    T(json::array({ "create_car", model, V(pos), cleanup }));
    const int k = nCar++;
    std::memset(gwVeh[k].data(), 0, VEHSTRIDE); std::memset(gwMat[k].data(), 0, 0x40);
    At<void*>(gwVeh[k].data(), 0x14) = gwMat[k].data(); At<uint8>(gwVeh[k].data(), 0x36) = 2;
    return gwVeh[k].data();
}
inline void __fastcall RoadProperlyStub(void* car, void*) { if (Mine()) T(json::array({ "place_on_road_properly", GwIdOf(uintptr_t(car)) })); }
inline void __fastcall CreatedByVehStub(void* car, void*, int32 by) { if (Mine()) T(json::array({ "vehicle_created_by", GwIdOf(uintptr_t(car)), by })); }
// rutas
inline bool __fastcall NodesLoadedStub(void*, void*, float a, float b, float c, float d) { if (!Mine()) return false; T(json::array({ "nodes_loaded", F(a), F(b), F(c), F(d) })); return !((stubFlags >> 8) & 1); }
inline CNodeAddress* __fastcall FindNodeStub(void*, void*, CNodeAddress* out, CVector pos, int32 type, float maxDist, int32 u2, int32 u3, int32 u4, int32 u5, int32 u6) {
    if (Mine()) T(json::array({ "find_node", V(pos), type, F(maxDist), u2, u3, u4, u5, u6 }));
    *out = CNodeAddress{ uint16(((stubFlags >> 9) & 1) ? 0xFFFF : 3), uint16(0) };
    return out;
}
inline CNodeAddress* __fastcall FindNthNodeStub(void*, void*, CNodeAddress* out, CVector pos, int32 type, float maxDist, int32 a, int32 b, int32 nth, int32 c, int32 d, CNodeAddress* outNode) {
    if (Mine()) T(json::array({ "find_nth_node", V(pos), type, F(maxDist), a, b, nth, c, d, outNode != nullptr }));
    *out = CNodeAddress{ uint16(3), uint16(1 + (nth & 3)) };
    return out;
}
// zonas / secuencia
inline void __cdecl InitSequenceStub(int32 n) { if (Mine()) T(json::array({ "init_sequence", n })); }
inline int32 __cdecl FindSequenceStub(int32 i) { if (!Mine()) return 0; T(json::array({ "find_sequence_element", i })); return (i * 5 + 3) % 13; }
inline float __cdecl Calc2DStub(CZone* a, CZone* b) {
    if (!Mine()) return 0.f;
    const int ia = int((uintptr_t(a) - uintptr_t(&CTheZones::NavigationZoneArray.data()[0])) / sizeof(CZone)), ib = int((uintptr_t(b) - uintptr_t(&CTheZones::NavigationZoneArray.data()[0])) / sizeof(CZone));
    T(json::array({ "zone_distance", ia, ib }));
    static constexpr float VALS[] = { 0.f, 5.f, 9.9f, 10.f, 10.1f, 25.f, 3.f, 1.f };
    return VALS[(ia * 7 + ib * 3 + (stubFlags >> 16)) & 7];
}
inline CZoneInfo* __cdecl ZoneInfoStub(const CVector* p, CZone** outZone) {
    if (!Mine()) return nullptr;
    T(json::array({ "get_zone_info", V(*p) }));
    if (outZone) *outZone = reinterpret_cast<CZone*>(gwZone[0].data());
    return reinterpret_cast<CZoneInfo*>(gwInfo[0].data());
}
// intra-clase (cada una se parchea solo cuando no es la funcion bajo prueba)
inline uint32 __cdecl ReleasePedsStub(bool a, bool b) { if (!Mine()) return 0; T(json::array({ "release_peds", a, b })); return relCount; }
inline void __cdecl ReleaseCarsStub() { if (Mine()) T(json::array({ "release_cars" })); }
inline void __cdecl CheerStub() { if (Mine()) T(json::array({ "cheer_victory" })); }
inline void __cdecl TellStub(bool b) { if (Mine()) T(json::array({ "tell_gang_members", b })); }
inline void __cdecl ClearStreetsStub() { if (Mine()) T(json::array({ "clear_the_streets" })); }
inline void __cdecl EndWarStub(bool b) { if (Mine()) T(json::array({ "end_gang_war", b })); }
inline void __cdecl MakeEnemyStub(int32 gang, int32 density) { if (Mine()) T(json::array({ "make_enemy_gain", gang, density })); }
inline bool __cdecl PickZoneStub() {
    if (!Mine()) return false;
    const bool ok = !((stubFlags >> 4) & 1);
    T(json::array({ "pick_zone_to_attack", ok }));
    if (ok) { CGangWars::pZoneToFightOver = reinterpret_cast<CZone*>(gwZone[1].data()); CGangWars::pZoneInfoToFightOver = reinterpret_cast<CZoneInfo*>(gwInfo[1].data()); CGangWars::Gang1 = (stubFlags >> 5) & 1 ? GANG_VAGOS : GANG_BALLAS; CGangWars::PointOfAttack = CVector{ 120.f, -30.f, 10.f }; }
    return ok;
}
inline bool __cdecl CanStartStub(CZoneInfo* zi) { if (!Mine()) return false; T(json::array({ "can_start_gang_war_here", NameP(zi) })); return !((stubFlags >> 6) & 1); }
inline bool __cdecl PedStreamedStub(eGangID gang) { if (!Mine()) return false; T(json::array({ "ped_streamed", int(gang) })); return !((stubFlags >> 1) & 1); }
inline bool __cdecl PickStreamedPedStub(eGangID gang, int32& out) {
    if (!Mine()) return false;
    ++pickCalls;
    const bool ok = !(((stubFlags >> 2) & 1) && (pickCalls % 3 == 0));
    out = 420 + (gang & 7);
    T(json::array({ "pick_streamed_ped", int(gang), ok, out }));
    return ok;
}
inline float __fastcall FindNearestStub(void* self, void*, CPed** out) {
    if (!Mine()) { return 0.f; }
    T(json::array({ "find_distance_to_nearest_member", NameP(self) }));
    *out = (stubFlags >> 3) & 1 ? nullptr : P(2);
    return 1.f;
}
inline int16 __fastcall SayStub(void* ped, void*, int32 ctx, uint32 delay, float prob, bool a, bool b, bool c) {
    if (!Mine()) return 0;
    T(json::array({ "say", PedIdOf(uintptr_t(ped)), ctx, delay, F(prob), a, b, c })); return 0;
}
inline CTask* __cdecl WanderTaskStub(CPed* ped) { if (!Mine()) return nullptr; T(json::array({ "get_wander_task_by_ped_type", GwIdOf(uintptr_t(ped)) })); return reinterpret_cast<CTask*>(ftBuf[0].data()); }
inline bool __cdecl LoadIoStub(void* data, int32 size) {
    if (!Mine()) return false;
    T(json::array({ "load_data", size }));
    if (ioPos + size <= ioIn.size()) std::memcpy(data, ioIn.data() + ioPos, size);
    ioPos += size; return true;
}
inline bool __cdecl SaveIoStub(void* data, int32 size) {
    if (!Mine()) return false;
    std::vector<uint8> b(static_cast<uint8*>(data), static_cast<uint8*>(data) + size);
    if (size == 0x58) {   // relleno y campos que el EXE deja sin escribir (basura de pila): se enmascaran
        for (int o : { 1, 2, 3, 0x4D, 0x4E, 0x4F }) b[o] = 0;
        if (!CGangWars::pZoneInfoToFightOver) for (int o = 0x10; o < 0x14; ++o) b[o] = 0;
    }
    json words = json::array();
    for (size_t k = 0; k + 4 <= b.size(); k += 4) { uint32 w; std::memcpy(&w, b.data() + k, 4); words.push_back(w); }
    T(json::array({ "save_data", size, words })); return true;
}
inline void __cdecl RequestModelStub(int32 model, int32 flags) { if (Mine()) T(json::array({ "request_model", model, flags })); }

// ---- instantanea y restauracion de las zonas globales que se tocan ----
struct Snap { uintptr_t at; std::vector<uint8> bytes; };
inline std::vector<Snap> snaps;
inline void TakeSnaps() {
    snaps.clear();
    auto add = [&](uintptr_t a, size_t n) { snaps.push_back({ a, ReversingProbe::Bytes(reinterpret_cast<void*>(a), n) }); };
    add(0x96AB28, 0xB0);                 // estaticos de CGangWars (incluye el cursor 0x96ABD4)
    add(0x8A5F40, 4);                    // ZoneInfoForTraining
    add(0xBA3794, 0x2F84);               // contador y arreglo de zonas de navegacion
    add(0xBA1DE8, 0x1970);               // contador y arreglo de CZoneInfo
    add(0x8D2618, 120);                  // tabla banda -> grupo de peatones
    add(0xC0ECC0, 64);                   // numero de peatones por grupo (32 grupos)
    add(0xC0F358, 32 * 42);              // modelos por grupo (32 grupos de 21 modelos)
    add(0x8E4CD0 + 400 * 20, 160 * 20);  // estado de streaming de los modelos 400..559
    add(0x96F854 + 3 * 4, 4);            // CPathFind::m_pPathNodes[3]
    add(0xB74490, 8);                    // punteros a los pools de peatones y vehiculos
    add(0x96A7CC, 4);                    // CEntryExitManager::ms_exitEnterState
    add(0xB7CB84, 4);                    // reloj del juego
    add(uintptr_t(PGrp()), sizeof(CPedGroup));   // hueco de ms_groups que hace de grupo del jugador
}
inline void RestoreSnaps() { for (auto& s : snaps) std::memcpy(reinterpret_cast<void*>(s.at), s.bytes.data(), s.bytes.size()); }
inline bool SnapsEqual() { for (auto& s : snaps) if (ReversingProbe::Bytes(reinterpret_cast<void*>(s.at), s.bytes.size()) != s.bytes) return false; return true; }

inline constexpr float FVALS[] = { 0.f, 0.1f, 0.3f, 0.4f, 0.45f, 0.5f, 0.6f, 1.f, 2.1f, 30000.f, 240000.f, 270000.f, 500000.f, -5.f };
inline constexpr uint8 STRENGTHS[] = { 1, 3, 4, 5, 10, 15, 16, 19, 20, 21, 30, 55, 100, 200, 255 };

inline void GwStatics(Rng& r) {
    CGangWars::ZoneInfoForTraining = r.Chance(15) ? -1 : r.Pick(6);
    CGangWars::pDriveByCar = nullptr;
    for (auto& v : CGangWars::GangRatingStrength) v = r.Pick(30);
    for (auto& v : CGangWars::GangRatings) v = r.Pick(5);
    CGangWars::FightTimer = r.Chance(30) ? 0u : uint32(r.Next() >> r.Pick(24));
    CGangWars::TimeTillNextAttack = PickOf(r, FVALS);
    CGangWars::Gang2 = r.Pick(10);
    CGangWars::LastTimeInArea = r.Next() >> 8;
    CGangWars::WarFerocity = r.Pick(6);
    CGangWars::Provocation = PickOf(r, FVALS);
    CGangWars::TimeStarted = r.Next() >> 8;
    CGangWars::State = static_cast<eGangWarState>(r.Pick(7));
    CGangWars::State2 = static_cast<eGangAttackState>(r.Pick(3));
    for (auto& v : CGangWars::aSpecificZones) v = r.Pick(10);
    CGangWars::Difficulty = PickOf(r, FVALS);
    CGangWars::Gang1 = static_cast<eGangID>(r.Pick(10));
    CGangWars::pZoneToFightOver = nullptr; CGangWars::pZoneInfoToFightOver = nullptr;
    CGangWars::bGangWarsActive = r.Chance(50); CGangWars::bTrainingMission = r.Chance(15);
    CGangWars::bPlayerIsCloseby = r.Chance(50); CGangWars::bCanTriggerGangWarWhenOnAMission = r.Chance(50);
    CGangWars::NumSpecificZones = r.Chance(40) ? 0 : r.Pick(6);
    CGangWars::RadarBlip = r.Chance(50) ? 0 : int32(r.Next() >> 20);
    CGangWars::TerritoryUnderControlPercentage = PickOf(r, FVALS);
    CGangWars::bIsPlayerOnAMission = r.Chance(50);
    CGangWars::CoorsOfPlayerAtStartOfWar = CVector{ PickOf(r, COORD), PickOf(r, COORD), PickOf(r, COORD) };
    CGangWars::PointOfAttack = CVector{ PickOf(r, COORD), PickOf(r, COORD), PickOf(r, COORD) };
    CGangWars::PedPickCursor = r.Chance(50) ? r.Pick(8) : int32(r.Next() >> 20);
}
// zonas de navegacion y de informacion reales (con instantanea), tamano pequeno
inline void ZoneFixture(Rng& r, int nZones, int nInfos) {
    *reinterpret_cast<uint16*>(0xBA3794) = uint16(nZones);
    *reinterpret_cast<uint16*>(0xBA1DE8) = uint16(nInfos);
    for (int k = 0; k < nInfos; ++k) {
        auto* b = reinterpret_cast<uint8*>(0xBA1DF0) + k * 0x11; std::memset(b, 0, 0x11);
        for (int g = 0; g < 10; ++g) b[g] = r.Chance(45) ? uint8(0) : PickOf(r, STRENGTHS);
        b[0xA] = uint8(r.Pick(16));
        for (int c = 0xB; c <= 0xE; ++c) b[c] = uint8(r.Next());
        b[0xF] = uint8(r.Next()); b[0x10] = uint8(0xF);
    }
    for (int z = 0; z < nZones; ++z) {
        auto* b = reinterpret_cast<uint8*>(0xBA3798) + z * 0x20; std::memset(b, 0, 0x20);
        b[0] = uint8('A' + z); b[1] = uint8('A' + r.Pick(26)); b[8] = uint8('a' + z);
        int16 x1 = int16(PickOf(r, COORD) * 40), x2 = int16(x1 + 200 + r.Pick(400)), y1 = int16(PickOf(r, COORD) * 40), y2 = int16(y1 + 200 + r.Pick(400));
        if (r.Chance(20)) { x1 = 2400; x2 = 2600; y1 = -1700; y2 = -1600; }   // contiene (2500, -1666): zona excluida
        At<int16>(b, 0x10) = x1; At<int16>(b, 0x12) = y1; At<int16>(b, 0x14) = int16(-5); At<int16>(b, 0x16) = x2; At<int16>(b, 0x18) = y2; At<int16>(b, 0x1A) = int16(200);
        At<int16>(b, 0x1C) = int16(r.Chance(15) || nInfos == 0 ? 0 : r.Pick(nInfos));
    }
}
inline void BuildGwPeds(Rng& r, int livePct, int waveP, int vehP) {
    for (int k = 0; k < NGW; ++k) {
        auto* p = gwPed[k].data(); std::memset(p, 0, PEDSTRIDE);
        auto* in = gwIntel[k].data(); std::memset(in, 0, INTELSZ);
        auto* v = gwVeh[k].data(); std::memset(v, 0, VEHSTRIDE);
        At<uint8>(p, 0x36) = 3; At<void*>(p, 0x47C) = in; At<void*>(in, 0) = p;
        At<int32>(p, 0x598) = PickOf(r, PEDTYPES);
        At<int32>(p, 0x530) = PickOf(r, std::array<int, 6>{ 0, 0, 54, 55, 7, 0 });
        At<uint32>(p, 0x474) = (r.Chance(waveP) ? 0x1000u : 0u) | (r.Chance(40) ? 0x2000u : 0u) | (r.Chance(30) ? 1u : 0u);
        At<uint8>(p, 0x484) = uint8(r.Pick(4));
        At<uint8>(p, 0x71A) = uint8(r.Next());
        for (int a = 0; a < 5; ++a) At<uint32>(p, 0x4E0 + size_t(a) * 4) = r.Next();
        static constexpr float NEAR45[] = { 0.f, 10.f, 44.8f, 44.99f, 45.f, 45.2f, 60.f, -30.f, -44.9f };
        auto* pl = reinterpret_cast<float*>(p + 4); pl[0] = PickOf(r, NEAR45); pl[1] = r.Chance(60) ? 0.f : PickOf(r, SMALL); pl[2] = PickOf(r, SMALL);
        At<uint8>(v, 0x36) = 2; At<uint8>(v, 0x42D) = uint8((r.Chance(vehP) ? 8 : 0) | (r.Chance(30) ? 0x40 : 0)); At<uint8>(v, 0x428) = uint8(r.Next());
        gwPedState[k] = r.Chance(livePct) ? uint8(r.Pick(127)) : uint8(0x80 | r.Pick(127));
        gwVehState[k] = r.Chance(livePct) ? uint8(r.Pick(127)) : uint8(0x80 | r.Pick(127));
    }
    pedPool = FakePool{ gwPed.data(), gwPedState.data(), uint32(NGW), -1, 0, 0, {} };
    vehPool = FakePool{ gwVeh.data(), gwVehState.data(), uint32(NGW), -1, 0, 0, {} };
}
inline void SetGwPos(int k, float x, float y, float z) { auto* pl = reinterpret_cast<float*>(gwPed[k].data() + 4); pl[0] = x; pl[1] = y; pl[2] = z; }

inline void GwBegin(Rng& r) {
    RestoreSnaps();
    ranges.clear(); trace = json::array(); taskNext = allocNext = 0; rngCalls = 0; coorsArgs.clear(); playerArgs.clear(); findMap.clear(); activeMap.clear(); respMask = 0;
    blipCalls = nPed = nCar = pickCalls = 0; relCount = 0; stubFlags = 0; groupOfPed = nullptr; groupIdOfPed = -1;
    for (auto& a : taskArena) std::memset(a.data(), 0xCD, a.size());
    for (auto& a : allocArena) std::memset(a.data(), 0xCD, a.size());
    for (float& s : stats) s = 0.f;
    for (int k = 0; k < NFT; ++k) { std::memset(ftBuf[k].data(), 0, ftBuf[k].size()); At<void*>(ftBuf[k].data(), 0) = ftVtbl.data(); At<int32>(ftBuf[k].data(), 0xC) = 3 + k; Reg(ftBuf[k].data(), ftBuf[k].size(), "ft" + std::to_string(k)); }
    for (int k = 0; k < int(taskArena.size()); ++k) Reg(taskArena[k].data(), 0x100, "tk" + std::to_string(k));
    for (int k = 0; k < 3; ++k) { std::memset(Ev(k), 0, 0x50); At<void*>(Ev(k), 0) = evVtbl.data(); Reg(Ev(k), 0x50, "e" + std::to_string(k)); }
    for (int i = 0; i <= NPED; ++i) {   // el jugador y los peatones del mundo de grupos tambien existen (FindPlayerPed)
        InitPed(i, r);
        Reg(Pd(i), PEDSZ, i == PL ? std::string("pl") : "p" + std::to_string(i));
        Reg(intelBuf[i].data(), INTELSZ, "i" + std::to_string(i));
    }
    std::memset(playerData.data(), 0, playerData.size()); At<int32>(playerData.data(), 0x38) = PGID; At<void*>(Pd(PL), 0x480) = playerData.data();
    auto* pgp = PGrp(); std::memset(pgp, 0, sizeof(CPedGroup)); Reg(pgp, sizeof(CPedGroup), "PG");
    At<void*>(pgp, 0x08) = pgp; At<void*>(pgp, 0x30) = pgp;
    for (int k = 0; k < 7; ++k) At<CPed*>(pgp, 0xC + size_t(k) * 4) = r.Chance(30) ? P(k) : nullptr;
    for (int k = 0; k < NGW; ++k) {
        Reg(gwPed[k].data(), PEDSTRIDE, "gp" + std::to_string(k)); Reg(gwIntel[k].data(), INTELSZ, "gi" + std::to_string(k));
        Reg(gwVeh[k].data(), VEHSTRIDE, "gv" + std::to_string(k));
    }
    for (int k = 0; k < 4; ++k) { Reg(gwMat[k].data(), 0x40, "gm" + std::to_string(k)); Reg(gwZone[k].data(), 0x20, "gz" + std::to_string(k)); Reg(gwInfo[k].data(), 0x11, "gzi" + std::to_string(k)); std::memset(gwZone[k].data(), 0, 0x20); std::memset(gwInfo[k].data(), 0, 0x11); }
    Reg(reinterpret_cast<void*>(0xBA3798), 380 * 0x20, "Z"); Reg(reinterpret_cast<void*>(0xBA1DF0), 380 * 0x11, "ZI");
    playerCoors = CVector{ PickOf(r, COORD), PickOf(r, COORD), PickOf(r, COORD) };
    *reinterpret_cast<uint32*>(0xB7CB84) = r.Next() >> 8;
    GwStatics(r);
}
inline json GwStaticsJson() {
    json w = json::array(); for (int k = 0; k < 44; ++k) w.push_back(*reinterpret_cast<uint32*>(0x96AB28 + k * 4));
    return w;
}
inline json CollectGw(json result) {
    json out = { { "result", std::move(result) } };
    out["statics"] = GwStaticsJson();
    out["training_idx"] = *reinterpret_cast<int32*>(0x8A5F40);
    json peds = json::array(), vehs = json::array();
    for (int k = 0; k < NGW; ++k) { peds.push_back(Hex(Fnv(gwPed[k].data(), PEDSTRIDE))); vehs.push_back(Hex(Fnv(gwVeh[k].data(), VEHSTRIDE))); }
    out["peds"] = peds; out["vehicles"] = vehs;
    out["zone_infos"] = Hex(Fnv(reinterpret_cast<void*>(0xBA1DE8), 0x196C)); out["zones"] = Hex(Fnv(reinterpret_cast<void*>(0xBA3794), 0x2F84));
    out["fake_zone_infos"] = Hex(Fnv(gwInfo.data(), sizeof(gwInfo))); out["fake_zones"] = Hex(Fnv(gwZone.data(), sizeof(gwZone)));
    out["mats"] = Hex(Fnv(gwMat.data(), sizeof(gwMat)));
    out["stats"] = Hex(Fnv(stats, sizeof(stats)));
    out["fake_tasks"] = Hex(Fnv(ftBuf.data(), sizeof(ftBuf)));
    json ca = json::array(); for (int a : coorsArgs) ca.push_back(a); out["coors_args"] = ca;
    json pa = json::array(); for (int a : playerArgs) pa.push_back(a); out["player_args"] = pa;
    out["pool_states"] = json::array({ Hex(Fnv(gwPedState.data(), NGW)), Hex(Fnv(gwVehState.data(), NGW)) });
    out["group_events"] = json::array({ Hex(Fnv(Ev(0), 0x50)), Hex(Fnv(Ev(1), 0x50)) });
    out["trace"] = trace;
    return out;
}

inline json RunGwCase(int fi, int I, ReversibleHooks::ReversibleHook::Base* hook, const Fn& fn, bool cand, bool& switches) {
    const int k = fi - 30;
    Rng r(uint64(I) * 6007u + uint64(fi) * 99991u + 31u);
    runSeed = uint32(I) * 2654435761u + uint32(fi) * 40503u + 11u;
    GwBegin(r);
    json result = nullptr;
    bool swapPed = false, swapVeh = false;
    const unsigned savedFp = _controlfp(0, 0);
    auto go = [&](auto&& call) {
        hook->State(cand);
        switches = ReversingProbe::JumpsToDll(fn.address) == cand;
        auto** pedSlot = reinterpret_cast<void**>(0xB74490); auto** vehSlot = reinterpret_cast<void**>(0xB74494);
        void* const realPed = *pedSlot; void* const realVeh = *vehSlot;
        if (swapPed) *pedSlot = &pedPool;
        if (swapVeh) *vehSlot = &vehPool;
        _controlfp(_PC_24, _MCW_PC);
        call();
        _controlfp(savedFp, _MCW_PC | _MCW_RC);
        *pedSlot = realPed; *vehSlot = realVeh;
    };
    switch (k) {
    case 0: {   // InitAtStartOfGame
        go([&] { reinterpret_cast<void(__cdecl*)()>(fn.address)(); });
        break;
    }
    case 1: {   // AttackWaveOvercome
        BuildGwPeds(r, 75, 60, 0); swapPed = true;
        playerCoors = CVector{ 0.f, r.Chance(50) ? 0.f : 1.f, 0.f };
        if (r.Chance(10)) for (int j = 0; j < NGW; ++j) At<uint32>(gwPed[j].data(), 0x474) &= ~0x1000u;
        bool ret{};
        go([&] { ret = reinterpret_cast<bool(__cdecl*)()>(fn.address)(); });
        result = ret; break;
    }
    case 2: {   // CalculateTimeTillNextAttack
        uint32 ret{};
        go([&] { ret = reinterpret_cast<uint32(__cdecl*)()>(fn.address)(); });
        result = ret; break;
    }
    case 3: {   // CanPlayerStartAGangWarHere
        const int nz = 2 + r.Pick(8), ni = 1 + r.Pick(8);
        ZoneFixture(r, nz, ni);
        for (auto& v : CGangWars::aSpecificZones) v = r.Pick(nz);
        auto* infos = reinterpret_cast<CZoneInfo*>(0xBA1DF0);
        CZoneInfo* arg = nullptr;
        switch (r.Pick(6)) {
        case 0: arg = nullptr; break;
        case 1: arg = infos + (CGangWars::ZoneInfoForTraining >= 0 ? CGangWars::ZoneInfoForTraining : 0); break;
        case 2: arg = reinterpret_cast<CZoneInfo*>(0xBA1DF0 + 0x11 * static_cast<uint16>(At<int16>(reinterpret_cast<uint8*>(0xBA3798) + 0x20 * CGangWars::aSpecificZones[r.Pick(6)], 0x1C))); break;
        case 3: arg = infos; break;
        case 4: arg = infos + r.Pick(ni); break;
        default: arg = reinterpret_cast<CZoneInfo*>(0xBA1DF0 + 0x11 * static_cast<uint16>(At<int16>(reinterpret_cast<uint8*>(0xBA3798) + 0x20 * CGangWars::aSpecificZones[0], 0x1C))); break;
        }
        bool ret{};
        go([&] { ret = reinterpret_cast<bool(__cdecl*)(CZoneInfo*)>(fn.address)(arg); });
        result = ret; break;
    }
    case 4: {   // CheerVictory
        static constexpr const char* LABELS[] = { "CHC", "LFL", "EBE", "ELS", "JEF", "GLN", "IWD", "GAN", "LMEX", "LIND", "PLS", "SUN", "ELF", "chc", "Sun", "", "XXXX", "LMEXX" };
        const char* lab = PickOf(r, LABELS);
        std::memset(gwZone[0].data(), 0, 0x20); std::strncpy(reinterpret_cast<char*>(gwZone[0].data()), lab, 8);
        std::memcpy(gwZone[0].data() + 8, "ZZZZZZZ", 8);
        CGangWars::pZoneToFightOver = reinterpret_cast<CZone*>(gwZone[0].data());
        stubFlags = r.Chance(20) ? 8u : 0u;
        go([&] { reinterpret_cast<void(__cdecl*)()>(fn.address)(); });
        break;
    }
    case 5: {   // ClearSpecificZonesToTriggerGangWar
        go([&] { reinterpret_cast<void(__cdecl*)()>(fn.address)(); });
        break;
    }
    case 6: {   // ClearTheStreets
        BuildGwPeds(r, 75, 0, 0); swapPed = true;
        for (int j = 0; j < NGW; ++j) {
            std::memset(ftBuf[j % NFT].data() + 8, 0, 0x30);
            if (r.Chance(60)) activeMap.push_back({ 100 + j, -3, int(TASK_COMPLEX_WANDER), j % NFT });
            At<int32>(ftBuf[j % NFT].data(), 0xC) = r.Pick(8);
        }
        go([&] { reinterpret_cast<void(__cdecl*)()>(fn.address)(); });
        result = json::array(); for (int j = 0; j < NFT; ++j) result.push_back(At<int32>(ftBuf[j].data(), 0xC));
        break;
    }
    case 7: {   // CreateDefendingGroup
        BuildGwPeds(r, 40, 0, 0); swapPed = true;
        for (int j = 8; j < NGW; ++j) gwPedState[j] |= 0x80;   // 8..13 son los huecos de los peatones nuevos
        playerCoors = CVector{ 0.f, 0.f, 0.f };
        CGangWars::PointOfAttack = CVector{ PickOf(r, COORD), PickOf(r, COORD), 10.f };
        // Dominio: el EXE crea int((D*0.4+0.6)*10) peatones y desreferencia el resultado de CPed::operator new sin comprobar null (0x44563F);
        // el pool falso tiene 8 huecos (8..15), asi que D <= 0.6 (maximo 8 peatones). El juego real usa D en [0,1].
        static constexpr float DDEF[] = { 0.f, 0.1f, 0.3f, 0.4f, 0.45f, 0.5f, 0.6f, -5.f };
        CGangWars::Difficulty = PickOf(r, DDEF);
        if (r.Chance(10)) CGangWars::Difficulty = -3.f;
        stubFlags = (r.Chance(10) ? 2u : 0u) | (r.Chance(15) ? 4u : 0u) | (r.Chance(10) ? 0x100u : 0u) | (r.Chance(10) ? 0x200u : 0u) | (r.Chance(20) ? 0x1000u : 0u);
        // nodos falsos (area 3): posiciones comprimidas (x8)
        *reinterpret_cast<void**>(0x96F854 + 3 * 4) = gwNodes.data();
        for (int j = 0; j < 8; ++j) { std::memset(gwNodes[j].data(), 0, 0x1C); At<int16>(gwNodes[j].data(), 8) = int16(PickOf(r, COORD) * 8); At<int16>(gwNodes[j].data(), 0xA) = int16(PickOf(r, COORD) * 8); At<int16>(gwNodes[j].data(), 0xC) = int16(r.Pick(100)); }
        playerCoors = CVector{ PickOf(r, COORD), PickOf(r, COORD), 0.f };
        for (int m = 400; m < 560; ++m) *reinterpret_cast<uint8*>(0x8E4CD0 + m * 20) = uint8(r.Chance(70) ? 1 : 0);
        *reinterpret_cast<int32*>(0x8D2618 + int32(CGangWars::Gang1) * 12) = 1 + r.Pick(5);
        *reinterpret_cast<uint16*>(0xC0F358 + (1 + 0) * 42) = 420;
        int32 gangArg = r.Pick(3);
        go([&] { result = reinterpret_cast<bool(__cdecl*)(int32)>(fn.address)(gangArg); });
        break;
    }
    case 8: {   // DoStuffWhenPlayerVictorious
        CGangWars::TimeTillNextAttack = PickOf(r, FVALS);
        go([&] { reinterpret_cast<void(__cdecl*)()>(fn.address)(); });
        break;
    }
    case 9: {   // DoesPlayerControlThisZone
        auto* b = gwInfo[0].data(); std::memset(b, 0, 0x11);
        const uint8 grove = r.Chance(20) ? uint8(0) : PickOf(r, STRENGTHS);
        uint8 ballas = r.Chance(30) ? uint8(0) : PickOf(r, STRENGTHS), vagos = r.Chance(30) ? uint8(0) : PickOf(r, STRENGTHS);
        if (r.Chance(30)) ballas = uint8(grove / 2), vagos = uint8(grove - grove / 2);
        b[0] = ballas; b[1] = grove; b[2] = vagos;
        bool ret{};
        go([&] { ret = reinterpret_cast<bool(__cdecl*)(CZoneInfo*)>(fn.address)(reinterpret_cast<CZoneInfo*>(b)); });
        result = ret; break;
    }
    case 10: case 12: case 13: {   // DontCreateCivilians / GangWarFightingGoingOn / GangWarGoingOn
        bool ret{};
        go([&] { ret = reinterpret_cast<bool(__cdecl*)()>(fn.address)(); });
        result = ret; break;
    }
    case 11: {   // EndGangWar(bool)
        relCount = r.Pick(12);
        CGangWars::pZoneInfoToFightOver = reinterpret_cast<CZoneInfo*>(gwInfo[0].data());
        const bool end = r.Chance(50);
        go([&] { reinterpret_cast<void(__cdecl*)(bool)>(fn.address)(end); });
        break;
    }
    case 14: {   // Load
        ioIn.assign(4 + 0x58, 0); ioPos = 0;
        uint8* s = ioIn.data() + 4;
        const int nz = 6 + r.Pick(4);
        ZoneFixture(r, nz, 6);
        s[0] = uint8(r.Pick(2)); At<int32>(s, 4) = r.Pick(7); At<uint32>(s, 8) = r.Next() >> 8;
        At<int32>(s, 0xC) = r.Chance(30) ? -1 : r.Pick(6); At<int32>(s, 0x10) = r.Chance(30) ? -1 : r.Pick(nz);
        At<float>(s, 0x14) = PickOf(r, COORD); At<float>(s, 0x18) = PickOf(r, COORD); At<float>(s, 0x1C) = PickOf(r, COORD);
        At<int32>(s, 0x20) = r.Pick(10); At<int32>(s, 0x24) = r.Pick(10); At<int32>(s, 0x28) = r.Pick(6); At<uint32>(s, 0x2C) = r.Next() >> 8; At<int32>(s, 0x30) = r.Pick(3);
        At<float>(s, 0x34) = PickOf(r, FVALS); At<float>(s, 0x38) = PickOf(r, COORD); At<float>(s, 0x3C) = PickOf(r, COORD); At<float>(s, 0x40) = PickOf(r, COORD);
        At<uint32>(s, 0x44) = r.Next() >> 4; At<int32>(s, 0x48) = r.Pick(2000); s[0x4C] = uint8(r.Pick(2)); At<float>(s, 0x50) = PickOf(r, FVALS); At<float>(s, 0x54) = PickOf(r, FVALS);
        bool ret{};
        go([&] { ret = reinterpret_cast<bool(__cdecl*)()>(fn.address)(); });
        result = ret; break;
    }
    case 22: {   // Save
        const int nz = 6 + r.Pick(4);
        ZoneFixture(r, nz, 6);
        if (r.Chance(70)) {
            const int zi = r.Pick(6), zn = r.Pick(nz);
            CGangWars::pZoneInfoToFightOver = r.Chance(85) ? reinterpret_cast<CZoneInfo*>(0xBA1DF0 + 0x11 * zi) : reinterpret_cast<CZoneInfo*>(gwInfo[0].data());
            CGangWars::pZoneToFightOver = reinterpret_cast<CZone*>(0xBA3798 + 0x20 * zn);
        }
        bool ret{};
        go([&] { ret = reinterpret_cast<bool(__cdecl*)()>(fn.address)(); });
        result = ret; break;
    }
    case 15: case 27: {   // MakeEnemyGainInfluenceInZone(gang, density) / StrengthenPlayerInfluenceInZone(density)
        auto* b = gwInfo[0].data(); std::memset(b, 0, 0x11);
        for (int g = 0; g < 10; ++g) b[g] = r.Chance(40) ? uint8(0) : PickOf(r, STRENGTHS);
        if (r.Chance(30)) { b[0] = 0; b[2] = 0; }
        if (r.Chance(25)) { b[1] = uint8(54 + r.Pick(3)); }
        b[0xA] = uint8(r.Next()); b[0xF] = uint8(r.Next());
        CGangWars::pZoneInfoToFightOver = reinterpret_cast<CZoneInfo*>(b);
        static constexpr int DENS[] = { 0, 1, 5, 30, 55, 200, -3, 3, 9 };
        const int gang = r.Pick(10), dens = PickOf(r, DENS);
        if (k == 15) go([&] { reinterpret_cast<void(__cdecl*)(int32, int32)>(fn.address)(gang, dens); });
        else go([&] { reinterpret_cast<void(__cdecl*)(int32)>(fn.address)(dens); });
        break;
    }
    case 16: {   // MakePlayerGainInfluenceInZone(float)
        auto* b = gwInfo[0].data(); std::memset(b, 0, 0x11);
        for (int g = 0; g < 10; ++g) b[g] = r.Chance(35) ? uint8(0) : PickOf(r, STRENGTHS);
        if (r.Chance(30)) { b[0] = 0; b[2] = 0; }
        b[0xA] = uint8(r.Next()); b[0xF] = uint8(r.Next());
        CGangWars::pZoneInfoToFightOver = reinterpret_cast<CZoneInfo*>(b);
        static constexpr float MULTS[] = { 0.3f, 1.0f, 0.0f, 0.5f, 0.9f, 0.7f, 1.5f, 0.05f };
        const float mult = PickOf(r, MULTS);
        bool ret{};
        go([&] { ret = reinterpret_cast<bool(__cdecl*)(float)>(fn.address)(mult); });
        result = ret; break;
    }
    case 17: case 18: {   // PedStreamedInForThisGang / PickStreamedInPedForThisGang
        for (int g = 0; g < 10; ++g) *reinterpret_cast<int32*>(0x8D2618 + g * 12) = r.Chance(80) ? 1 + r.Pick(5) : r.Pick(2);
        for (int g = 0; g < 6; ++g) {
            *reinterpret_cast<uint16*>(0xC0ECC0 + g * 2) = uint16(r.Chance(15) ? 0 : 1 + r.Pick(7));   // (la DLL lee uint16 y el EXE int16: solo cuentas reales < 0x8000)
            for (int m = 0; m < 21; ++m) *reinterpret_cast<uint16*>(0xC0F358 + g * 42 + m * 2) = uint16(400 + g * 21 + m);
        }
        for (int m = 400; m < 560; ++m) *reinterpret_cast<uint8*>(0x8E4CD0 + m * 20) = uint8(r.Chance(40) ? 1 : r.Pick(4));
        const eGangID gang = static_cast<eGangID>(r.Pick(10));
        if (k == 17) {
            bool ret{};
            go([&] { ret = reinterpret_cast<bool(__cdecl*)(eGangID)>(fn.address)(gang); });
            result = ret;
        } else {
            int32 out = 0x5A5A5A5A; bool ret{};
            go([&] { ret = reinterpret_cast<bool(__cdecl*)(eGangID, int32&)>(fn.address)(gang, out); });
            result = json::array({ ret, out });
        }
        break;
    }
    case 19: {   // PickZoneToAttack
        const int nz = 3 + r.Pick(7), ni = 2 + r.Pick(7);
        ZoneFixture(r, nz, ni);
        CGangWars::Gang1 = static_cast<eGangID>(r.Pick(10));
        playerCoors = CVector{ PickOf(r, COORD) * 8.f, PickOf(r, COORD) * 8.f, 0.f };
        stubFlags = uint32(r.Pick(8)) << 16;
        bool ret{};
        go([&] { ret = reinterpret_cast<bool(__cdecl*)()>(fn.address)(); });
        result = ret; break;
    }
    case 20: {   // ReleaseCarsInAttackWave
        BuildGwPeds(r, 75, 0, 50); swapVeh = true;
        go([&] { reinterpret_cast<void(__cdecl*)()>(fn.address)(); });
        break;
    }
    case 21: {   // ReleasePedsInAttackWave(bool, bool)
        BuildGwPeds(r, 75, 55, 0); swapPed = true;
        const bool a = r.Chance(50), b = r.Chance(50);
        uint32 ret{};
        go([&] { ret = reinterpret_cast<uint32(__cdecl*)(bool, bool)>(fn.address)(a, b); });
        result = ret; break;
    }
    case 23: case 28: {   // SetGangWarsActive(bool) / SwitchGangWarsActive()
        const bool a = r.Chance(50);
        if (k == 23) go([&] { reinterpret_cast<void(__cdecl*)(bool)>(fn.address)(a); });
        else go([&] { reinterpret_cast<void(__cdecl*)()>(fn.address)(); });
        break;
    }
    case 24: {   // SetSpecificZoneToTriggerGangWar(int)
        CGangWars::NumSpecificZones = r.Pick(6);
        const int zone = r.Chance(80) ? r.Pick(10) : int(r.Next() >> 20);
        go([&] { reinterpret_cast<void(__cdecl*)(int32)>(fn.address)(zone); });
        break;
    }
    case 25: {   // StartDefensiveGangWar
        stubFlags = (r.Chance(30) ? 0x10u : 0u) | (r.Chance(50) ? 0x20u : 0u);
        CGangWars::Gang1 = static_cast<eGangID>(r.Pick(10));
        playerCoors = CVector{ PickOf(r, COORD) * 3.f, PickOf(r, COORD) * 3.f, 0.f };
        go([&] { reinterpret_cast<void(__cdecl*)()>(fn.address)(); });
        break;
    }
    case 26: {   // StartOffensiveGangWar
        auto* b = gwInfo[0].data(); std::memset(b, 0, 0x11);
        for (int g = 0; g < 10; ++g) b[g] = r.Chance(45) ? uint8(0) : PickOf(r, STRENGTHS);
        if (r.Chance(25)) { b[0] = PickOf(r, STRENGTHS); b[2] = b[0]; }
        if (r.Chance(20)) { b[2] = PickOf(r, STRENGTHS); for (int g = 3; g < 10; ++g) b[g] = 0; }
        b[0xA] = uint8(r.Next()); b[0xF] = uint8(r.Next()); b[0x10] = uint8(r.Next());
        stubFlags = r.Chance(30) ? 0x40u : 0u;
        stats[0xB5] = r.Chance(50) ? -1.f : 1.f;
        CGangWars::State2 = r.Chance(70) ? NO_ATTACK : static_cast<eGangAttackState>(1 + r.Pick(2));
        if (r.Chance(20)) CGangWars::bTrainingMission = true;
        *reinterpret_cast<uint32*>(0xB7CB84) = r.Next() >> 8;
        go([&] { reinterpret_cast<void(__cdecl*)()>(fn.address)(); });
        break;
    }
    case 29: {   // TellGangMembersTo(bool)
        BuildGwPeds(r, 75, 0, 0); swapPed = true;
        for (int j = 0; j < NGW; ++j) {
            auto* p = gwPed[j].data();
            At<int32>(p, 0x598) = PickOf(r, std::array<int, 11>{ 0, 1, 4, 7, 8, 9, 12, 16, 17, 7, 9 });
            if (r.Chance(40)) { At<uint32>(p, 0x46C) = 0x100u; At<void*>(p, 0x58C) = gwVeh[(j + 1) % NGW].data(); }
            if (r.Chance(50)) At<void*>(gwVeh[(j + 1) % NGW].data(), 0x460) = p;     // m_pDriver
        }
        const bool ending = r.Chance(60);
        go([&] { reinterpret_cast<void(__cdecl*)(bool)>(fn.address)(ending); });
        break;
    }
    case 30: {   // TellStreamingWhichGangsAreNeeded(uint32&)
        static constexpr float PA[] = { 0.f, 100.f, 149.f, 150.f, 151.f, 300.f };
        CGangWars::PointOfAttack = CVector{ PickOf(r, PA), PickOf(r, SMALL), 10.f };
        playerCoors = CVector{ 0.f, 0.f, 0.f };
        CGangWars::Gang1 = static_cast<eGangID>(r.Pick(10));
        uint32 flags = r.Chance(50) ? 0u : r.Next();
        go([&] { reinterpret_cast<void(__cdecl*)(uint32&)>(fn.address)(flags); });
        result = flags; break;
    }
    case 31: {   // UpdateTerritoryUnderControlPercentage
        const int nz = r.Chance(8) ? 0 : 1 + r.Pick(12), ni = 1 + r.Pick(10);
        ZoneFixture(r, nz, ni);
        for (int g = 0; g < 10; ++g) for (int z = 0; z < ni; ++z) if (r.Chance(15)) { auto* b = reinterpret_cast<uint8*>(0xBA1DF0) + z * 0x11; b[0] = b[2] = uint8(PickOf(r, STRENGTHS)); b[1] = r.Chance(50) ? uint8(b[0] + b[2]) : uint8(0); }
        stats[0xED] = r.Chance(30) ? 0.f : float(r.Pick(8));
        go([&] { reinterpret_cast<void(__cdecl*)()>(fn.address)(); });
        break;
    }
    default: break;
    }
    return CollectGw(std::move(result));
}

// =====================================================================================================================
// Tabla de funciones (el analizador lee estas filas con una expresion regular: sin espacios dentro de las llaves)
// =====================================================================================================================
inline const std::array<Fn, 62> fns{{
    {"CGroupEventHandler","IsKillTaskAppropriate",0x5F7A60,0,-1,256},
    {"CGroupEventHandler","ComputeWalkAlongsideResponse",0x5FA910,0,-1,256},
    {"CGroupEventHandler","ComputeStareResponse",0x5F9BD0,0,-1,384},
    {"CGroupEventHandler","ComputeResponseVehicleDamage",0x5FC070,0,-1,384},
    {"CGroupEventHandler","ComputeResponseShotFired",0x5FBDF0,0,-1,256},
    {"CGroupEventHandler","ComputeResponseSexyPed",0x5FB390,0,-1,384},
    {"CGroupEventHandler","ComputeResponseSeenCop",0x5FBCB0,0,-1,256},
    {"CGroupEventHandler","ComputeResponsePlayerCommand",0x5FB470,0,-1,256},
    {"CGroupEventHandler","ComputeResponsePedThreat",0x5FBB90,0,-1,384},
    {"CGroupEventHandler","ComputeResponsePedFriend",0x5FB2D0,0,-1,384},
    {"CGroupEventHandler","ComputeResponseNewGangMember",0x5F9840,0,-1,256},
    {"CGroupEventHandler","ComputeResponseLeaderExitedCar",0x5F90A0,0,-1,512},
    {"CGroupEventHandler","ComputeResponseLeaderEnterExit",0x5F9710,0,-1,256},
    {"CGroupEventHandler","ComputeResponseGunAimedAt",0x5FBD10,0,-1,384},
    {"CGroupEventHandler","ComputeResponseGather",0x5F99F0,0,-1,192},
    {"CGroupEventHandler","ComputeResponseDraggedOutCar",0x5FBE70,0,-1,384},
    {"CGroupEventHandler","ComputeResponseDanger",0x5FB540,0,-1,192},
    {"CGroupEventHandler","ComputeResponseDamage",0x5FBF50,0,-1,384},
    {"CGroupEventHandler","ComputeMemberResponses",0x5FAA50,0,-1,384},
    {"CGroupEventHandler","ComputeLeanOnVehicleResponse",0x5F9B20,0,-1,192},
    {"CGroupEventHandler","ComputeKillThreatsBasicResponse",0x5FB590,0,-1,256},
    {"CGroupEventHandler","ComputeKillPlayerBasicResponse",0x5FB670,0,-1,256},
    {"CGroupEventHandler","ComputeHassleThreatResponse",0x5F9D50,0,-1,384},
    {"CGroupEventHandler","ComputeHassleSexyPedResponse",0x5FA020,0,-1,256},
    {"CGroupEventHandler","ComputeHandSignalResponse",0x5FA820,0,-1,192},
    {"CGroupEventHandler","ComputeGreetResponse",0x5FA550,0,-1,512},
    {"CGroupEventHandler","ComputeFleePedResponse",0x5FA130,0,-1,384},
    {"CGroupEventHandler","ComputeDrivebyResponse",0x5F7A00,0,-1,128},
    {"CGroupEventHandler","ComputeEventResponseTasks",0x5FC200,0,-1,768},
    {"CGroupEventHandler","ComputeDoDealResponse",0x5FA290,0,-1,512},
    {"CGangWars","InitAtStartOfGame",0x443920,0,-1,64},
    {"CGangWars","AttackWaveOvercome",0x445B30,0,-1,384},
    {"CGangWars","CalculateTimeTillNextAttack",0x443DB0,0,-1,64},
    {"CGangWars","CanPlayerStartAGangWarHere",0x443F80,0,-1,384},
    {"CGangWars","CheerVictory",0x444040,0,-1,192},
    {"CGangWars","ClearSpecificZonesToTriggerGangWar",0x443FF0,0,-1,64},
    {"CGangWars","ClearTheStreets",0x4444B0,0,-1,256},
    {"CGangWars","CreateDefendingGroup",0x4453D0,0,-1,512},
    {"CGangWars","DoStuffWhenPlayerVictorious",0x446400,0,-1,128},
    {"CGangWars","DoesPlayerControlThisZone",0x443AE0,0,-1,192},
    {"CGangWars","DontCreateCivilians",0x4439C0,0,-1,64},
    {"CGangWars","EndGangWar",0x4464C0,0,-1,192},
    {"CGangWars","GangWarFightingGoingOn",0x443AC0,0,-1,64},
    {"CGangWars","GangWarGoingOn",0x443AA0,0,-1,64},
    {"CGangWars","Load",0x5D3EB0,0,-1,192},
    {"CGangWars","MakeEnemyGainInfluenceInZone",0x445FD0,0,-1,384},
    {"CGangWars","MakePlayerGainInfluenceInZone",0x445E80,0,-1,384},
    {"CGangWars","PedStreamedInForThisGang",0x4439D0,0,-1,256},
    {"CGangWars","PickStreamedInPedForThisGang",0x443A20,0,-1,384},
    {"CGangWars","PickZoneToAttack",0x443B00,0,-1,512},
    {"CGangWars","ReleaseCarsInAttackWave",0x445E20,0,-1,192},
    {"CGangWars","ReleasePedsInAttackWave",0x445C30,0,-1,384},
    {"CGangWars","Save",0x5D5530,0,-1,192},
    {"CGangWars","SetGangWarsActive",0x446570,0,-1,192},
    {"CGangWars","SetSpecificZoneToTriggerGangWar",0x444010,0,-1,128},
    {"CGangWars","StartDefensiveGangWar",0x444300,0,-1,256},
    {"CGangWars","StartOffensiveGangWar",0x446050,0,-1,512},
    {"CGangWars","StrengthenPlayerInfluenceInZone",0x445F50,0,-1,256},
    {"CGangWars","SwitchGangWarsActive",0x4465F0,0,-1,64},
    {"CGangWars","TellGangMembersTo",0x444530,0,-1,384},
    {"CGangWars","TellStreamingWhichGangsAreNeeded",0x443D50,0,-1,128},
    {"CGangWars","UpdateTerritoryUnderControlPercentage",0x443DE0,0,-1,384},
}};

// =====================================================================================================================
// Parches de colaboradores: se calculan TODOS los Body() antes de crear ningun EntryPatch; despues cada funcion probada
// parchea solo los colaboradores que usa (nunca ella misma ni otra funcion que ejecute de verdad).
// =====================================================================================================================
enum Tag {
    T_PLAYERPED, T_SETTASK, T_FINDTASK, T_FINDACTIVE, T_RAND, T_LOOKAT, T_EVADD, T_COMPRESP, T_COMPTYPE, T_PEDSGROUP, T_GROUPID,
    T_TASKNEW, T_TASKDEL, T_ALLOCNEW, T_ALLOCDEL, T_REGREF, T_CLEANREF, T_ENTVALID, T_ENTEREDCAR, T_QUITENTERING,
    T_COORS, T_FILLZONES, T_TEXT, T_MSG, T_BRIEF, T_HELP, T_STATINC, T_STATGET, T_STATSET, T_COORDBLIP, T_ENTBLIP, T_SPRITE, T_SCALE,
    T_COLOUR, T_CLEARBLIP, T_CREATEDBY, T_WORLDADD, T_PEDNEW, T_CIVILIAN, T_GIVEDELAYED, T_SETWEAPON, T_GROUNDZ, T_PICKUP, T_PICKCAR,
    T_CREATECAR, T_ROAD, T_VEHCREATED, T_NODESLOADED, T_FINDNODE, T_FINDNTH, T_INITSEQ, T_FINDSEQ, T_CALC2D, T_ZONEINFO, T_RELPEDS,
    T_RELCARS, T_CHEER, T_TELL, T_CLEARSTREETS, T_ENDWAR, T_MAKEENEMY, T_PICKZONE, T_CANSTART, T_PEDSTREAMED, T_PICKSTREAMED,
    T_NEAREST, T_SAY, T_WANDERTASK, T_LOADIO, T_SAVEIO, T_REQUESTMODEL, T_COUNT
};
struct Spec { uintptr_t exe{}, dll{}, body{}; void* stubExe{}; void* stubDll{}; };
inline std::array<Spec, T_COUNT> specs;
template<class A, class B> inline void AddSpec(Tag t, uintptr_t exe, uintptr_t dll, A stubExe, B stubDll) {
    specs[t] = Spec{ exe, dll, dll ? Body(dll) : 0, reinterpret_cast<void*>(stubExe), reinterpret_cast<void*>(stubDll) };
}
template<class A> inline void AddSpec(Tag t, uintptr_t exe, uintptr_t dll, A stub) { AddSpec(t, exe, dll, stub, stub); }
using std::bit_cast;
inline void BuildSpecs() {
    AddSpec(T_PLAYERPED, 0x56E210, bit_cast<uintptr_t>(static_cast<CPlayerPed * (*)(int32)>(&FindPlayerPed)), &FindPlayerPedStub);
    AddSpec(T_SETTASK, 0x5F7540, bit_cast<uintptr_t>(&CPedGroupIntelligence::SetTask), &SetTaskStub);
    AddSpec(T_FINDTASK, 0x6817D0, bit_cast<uintptr_t>(&CTaskManager::FindTaskByType), &FindTaskByTypeStub);
    AddSpec(T_FINDACTIVE, 0x681740, bit_cast<uintptr_t>(&CTaskManager::FindActiveTaskByType), &FindActiveStub);
    AddSpec(T_RAND, 0x821B1E, bit_cast<uintptr_t>(&CGeneral::GetRandomNumber), &RandExe, &RandDll);
    AddSpec(T_LOOKAT, 0x618970, bit_cast<uintptr_t>(&IKChainManager_c::LookAt), &LookAtStub);
    AddSpec(T_EVADD, 0x4AB420, bit_cast<uintptr_t>(static_cast<CEvent * (CEventGroup::*)(CEvent*, bool)>(&CEventGroup::Add)), &EventAddStub);
    AddSpec(T_COMPRESP, 0x4C4220, bit_cast<uintptr_t>(static_cast<CTask * (*)(const CPed&, const CEvent&)>(&CEventHandler::ComputeEventResponseTask)), &ComputeResponseStub);
    AddSpec(T_COMPTYPE, 0x4B56C0, bit_cast<uintptr_t>(static_cast<void (CEventEditableResponse::*)(CPed*, bool)>(&CEventEditableResponse::ComputeResponseTaskType)), &ComputeResponseTypeStub);
    AddSpec(T_PEDSGROUP, 0x5F7E80, bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup), &PedsGroupStub);
    AddSpec(T_GROUPID, 0x5F7EE0, bit_cast<uintptr_t>(&CPedGroups::GetGroupId), &GroupIdStub);
    AddSpec(T_TASKNEW, 0x61A5A0, bit_cast<uintptr_t>(&CTask::operator new), &TaskNew);
    AddSpec(T_TASKDEL, 0x61A5B0, bit_cast<uintptr_t>(&CTask::operator delete), &TaskDelete);
    AddSpec(T_ALLOCNEW, 0x69D980, bit_cast<uintptr_t>(&CTaskAllocator::operator new), &AllocNew);
    AddSpec(T_ALLOCDEL, 0x69D990, bit_cast<uintptr_t>(&CTaskAllocator::operator delete), &AllocDelete);
    AddSpec(T_REGREF, 0x571B70, bit_cast<uintptr_t>(static_cast<void (CEntity::*)(CEntity**)>(&CEntity::RegisterReference)), &RegisterRef);
    AddSpec(T_CLEANREF, 0x571A00, bit_cast<uintptr_t>(static_cast<void (CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)), &CleanupRef);
    AddSpec(T_ENTVALID, 0x533310, bit_cast<uintptr_t>(&IsEntityPointerValid), &EntityValidStub);
    AddSpec(T_ENTEREDCAR, 0x5F8900, bit_cast<uintptr_t>(&CGroupEventHandler::ComputeResponseLeaderEnteredCar), &EnteredCarStub);
    AddSpec(T_QUITENTERING, 0x5F9530, bit_cast<uintptr_t>(&CGroupEventHandler::ComputeResponsLeaderQuitEnteringCar), &QuitEnteringStub);
    // --- CGangWars ---
    AddSpec(T_COORS, 0x56E010, bit_cast<uintptr_t>(static_cast<CVector (*)(int32)>(&FindPlayerCoors)), &CoorsStub);
    AddSpec(T_FILLZONES, 0x572440, bit_cast<uintptr_t>(&CTheZones::FillZonesWithGangColours), &FillZonesStub);
    AddSpec(T_TEXT, 0x6A0050, bit_cast<uintptr_t>(&CText::Get), &TextGetStub);
    AddSpec(T_MSG, 0x69F0B0, bit_cast<uintptr_t>(&CMessages::AddMessageQ), &MessageStub);
    AddSpec(T_BRIEF, 0x69DD50, bit_cast<uintptr_t>(&CMessages::AddToPreviousBriefArray), &BriefStub);
    AddSpec(T_HELP, 0x588BE0, bit_cast<uintptr_t>(&CHud::SetHelpMessage), &HelpStub);
    AddSpec(T_STATINC, 0x55C180, bit_cast<uintptr_t>(&CStats::IncrementStat), &StatIncStub);
    AddSpec(T_STATGET, 0x558E40, bit_cast<uintptr_t>(static_cast<float (*)(eStats)>(&CStats::GetStatValue)), &StatGetStub);
    AddSpec(T_STATSET, 0x55A070, bit_cast<uintptr_t>(&CStats::SetStatValue), &StatSetStub);
    AddSpec(T_COORDBLIP, 0x583820, bit_cast<uintptr_t>(&CRadar::SetCoordBlip), &SetCoordBlipStub);
    AddSpec(T_ENTBLIP, 0x5839A0, bit_cast<uintptr_t>(&CRadar::SetEntityBlip), &SetEntityBlipStub);
    AddSpec(T_SPRITE, 0x583D70, bit_cast<uintptr_t>(&CRadar::SetBlipSprite), &BlipSpriteStub);
    AddSpec(T_SCALE, 0x583CC0, bit_cast<uintptr_t>(&CRadar::ChangeBlipScale), &BlipScaleStub);
    AddSpec(T_COLOUR, 0x583AB0, bit_cast<uintptr_t>(&CRadar::ChangeBlipColour), &BlipColourStub);
    AddSpec(T_CLEARBLIP, 0x587C60, bit_cast<uintptr_t>(static_cast<void (*)(eBlipType, int32)>(&CRadar::ClearBlipForEntity)), &ClearBlipEntityStub);
    AddSpec(T_CREATEDBY, 0x5E47E0, bit_cast<uintptr_t>(&CPed::SetCharCreatedBy), &CreatedByStub);
    AddSpec(T_WORLDADD, 0x563220, bit_cast<uintptr_t>(&CWorld::Add), &WorldAddStub);
    AddSpec(T_PEDNEW, 0x5E4720, bit_cast<uintptr_t>(static_cast<void* (*)(unsigned)>(&CPed::operator new)), &PedNewStub);
    AddSpec(T_CIVILIAN, 0x5DDB70, uintptr_t(0), &CivilianStub);   // la DLL llama al constructor del EXE por direccion
    AddSpec(T_GIVEDELAYED, 0x5E89B0, bit_cast<uintptr_t>(&CPed::GiveDelayedWeapon), &GiveDelayedStub);
    AddSpec(T_SETWEAPON, 0x5E6280, bit_cast<uintptr_t>(static_cast<void (CPed::*)(eWeaponType)>(&CPed::SetCurrentWeapon)), &SetCurrentWeaponStub);
    AddSpec(T_GROUNDZ, 0x5696C0, bit_cast<uintptr_t>(&CWorld::FindGroundZFor3DCoord), &GroundZStub);
    AddSpec(T_PICKUP, 0x456F20, bit_cast<uintptr_t>(&CPickups::GenerateNewOne), &PickupNative, &PickupDll);
    AddSpec(T_PICKCAR, 0x614490, bit_cast<uintptr_t>(&CPopulation::PickGangCar), &PickCarStub);
    AddSpec(T_CREATECAR, 0x431F80, bit_cast<uintptr_t>(&CCarCtrl::CreateCarForScript), &CreateCarStub);
    AddSpec(T_ROAD, 0x6AF420, bit_cast<uintptr_t>(&CAutomobile::PlaceOnRoadProperly), &RoadProperlyStub);
    AddSpec(T_VEHCREATED, 0x6D5D70, bit_cast<uintptr_t>(&CVehicle::SetVehicleCreatedBy), &CreatedByVehStub);
    AddSpec(T_NODESLOADED, 0x44DD10, bit_cast<uintptr_t>(&CPathFind::AreNodesLoadedForArea), &NodesLoadedStub);
    AddSpec(T_FINDNODE, 0x44F460, uintptr_t(0), &FindNodeStub);   // el envoltorio de la DLL llama a esta direccion del EXE (puntero oculto de retorno)
    AddSpec(T_FINDNTH, 0x44F8C0, uintptr_t(0), &FindNthNodeStub);
    AddSpec(T_INITSEQ, 0x421740, bit_cast<uintptr_t>(&CCarCtrl::InitSequence), &InitSequenceStub);
    AddSpec(T_FINDSEQ, 0x421770, bit_cast<uintptr_t>(&CCarCtrl::FindSequenceElement), &FindSequenceStub);
    AddSpec(T_CALC2D, 0x5725B0, bit_cast<uintptr_t>(&CTheZones::Calc2DDistanceBetween2Zones), &Calc2DStub);
    AddSpec(T_ZONEINFO, 0x572400, bit_cast<uintptr_t>(static_cast<CZoneInfo * (*)(const CVector&, CZone**)>(&CTheZones::GetZoneInfo)), &ZoneInfoStub);
    AddSpec(T_RELPEDS, 0x445C30, bit_cast<uintptr_t>(&CGangWars::ReleasePedsInAttackWave), &ReleasePedsStub);
    AddSpec(T_RELCARS, 0x445E20, bit_cast<uintptr_t>(&CGangWars::ReleaseCarsInAttackWave), &ReleaseCarsStub);
    AddSpec(T_CHEER, 0x444040, bit_cast<uintptr_t>(&CGangWars::CheerVictory), &CheerStub);
    AddSpec(T_TELL, 0x444530, bit_cast<uintptr_t>(&CGangWars::TellGangMembersTo), &TellStub);
    AddSpec(T_CLEARSTREETS, 0x4444B0, bit_cast<uintptr_t>(&CGangWars::ClearTheStreets), &ClearStreetsStub);
    AddSpec(T_ENDWAR, 0x4464C0, bit_cast<uintptr_t>(&CGangWars::EndGangWar), &EndWarStub);
    AddSpec(T_MAKEENEMY, 0x445FD0, bit_cast<uintptr_t>(&CGangWars::MakeEnemyGainInfluenceInZone), &MakeEnemyStub);
    AddSpec(T_PICKZONE, 0x443B00, bit_cast<uintptr_t>(&CGangWars::PickZoneToAttack), &PickZoneStub);
    AddSpec(T_CANSTART, 0x443F80, bit_cast<uintptr_t>(&CGangWars::CanPlayerStartAGangWarHere), &CanStartStub);
    AddSpec(T_PEDSTREAMED, 0x4439D0, bit_cast<uintptr_t>(&CGangWars::PedStreamedInForThisGang), &PedStreamedStub);
    AddSpec(T_PICKSTREAMED, 0x443A20, bit_cast<uintptr_t>(&CGangWars::PickStreamedInPedForThisGang), &PickStreamedPedStub);
    AddSpec(T_NEAREST, 0x5FB0A0, bit_cast<uintptr_t>(&CPedGroup::FindDistanceToNearestMember), &FindNearestStub);
    AddSpec(T_SAY, 0x5EFFE0, bit_cast<uintptr_t>(&CPed::Say), &SayStub);
    AddSpec(T_WANDERTASK, 0x673D00, bit_cast<uintptr_t>(&CTaskComplexWander::GetWanderTaskByPedType), &WanderTaskStub);
    AddSpec(T_LOADIO, 0x5D1300, bit_cast<uintptr_t>(static_cast<bool (*)(void*, int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)), &LoadIoStub);
    AddSpec(T_SAVEIO, 0x5D1270, bit_cast<uintptr_t>(static_cast<bool (*)(void*, int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)), &SaveIoStub);
    AddSpec(T_REQUESTMODEL, 0x4087E0, bit_cast<uintptr_t>(&CStreaming::RequestModel), &RequestModelStub);
}
inline std::vector<int> TagsFor(int fi) {
    const std::vector<int> groupTags{ T_PLAYERPED, T_SETTASK, T_FINDTASK, T_FINDACTIVE, T_RAND, T_LOOKAT, T_EVADD, T_COMPRESP, T_COMPTYPE, T_PEDSGROUP,
                                      T_GROUPID, T_TASKNEW, T_TASKDEL, T_ALLOCNEW, T_ALLOCDEL, T_REGREF, T_CLEANREF, T_ENTVALID };
    if (fi < 30) {
        auto t = groupTags;
        if (fi == 28) { t.push_back(T_ENTEREDCAR); t.push_back(T_QUITENTERING); }
        return t;
    }
    switch (fi - 30) {
    case 1:  return { T_COORS, T_CREATEDBY };
    case 2:  return { T_RAND };
    case 4:  return { T_PLAYERPED, T_NEAREST, T_SAY };
    case 5:  return { T_FILLZONES };
    case 6:  return { T_FINDACTIVE };
    case 7:  return { T_PEDSTREAMED, T_PICKSTREAMED, T_NODESLOADED, T_FINDNODE, T_FINDNTH, T_COORS, T_PLAYERPED, T_RAND, T_GROUNDZ, T_PEDNEW, T_CIVILIAN,
                      T_CREATEDBY, T_WORLDADD, T_TASKNEW, T_TASKDEL, T_REGREF, T_CLEANREF, T_EVADD, T_GIVEDELAYED, T_SETWEAPON, T_ENTBLIP, T_SCALE, T_COLOUR,
                      T_PICKCAR, T_CREATECAR, T_ROAD, T_VEHCREATED, T_PICKUP, T_REQUESTMODEL };
    case 8:  return { T_RELPEDS, T_RELCARS, T_CHEER, T_TELL, T_TEXT, T_MSG, T_BRIEF, T_STATINC, T_FILLZONES };
    case 11: return { T_RAND, T_RELPEDS, T_RELCARS, T_MAKEENEMY, T_FILLZONES };
    case 14: return { T_LOADIO };
    case 22: return { T_SAVEIO };
    case 15: case 16: case 27: return { T_STATINC };
    case 19: return { T_INITSEQ, T_FINDSEQ, T_CALC2D, T_COORS, T_RAND };
    case 20: return { T_VEHCREATED };
    case 21: return { T_CLEARBLIP, T_CREATEDBY, T_WANDERTASK, T_EVADD, T_TASKNEW, T_TASKDEL, T_REGREF, T_CLEANREF };
    case 23: case 28: return { T_FILLZONES, T_RAND, T_ENDWAR };
    case 24: return { T_FILLZONES };
    case 25: return { T_PICKZONE, T_TEXT, T_HELP, T_COORS, T_COORDBLIP, T_SPRITE, T_RAND };
    case 26: return { T_COORS, T_ZONEINFO, T_CANSTART, T_TEXT, T_MSG, T_BRIEF, T_STATGET, T_CLEARSTREETS, T_TELL };
    case 29: return { T_PLAYERPED, T_RAND, T_EVADD, T_TASKNEW, T_TASKDEL, T_REGREF, T_CLEANREF };
    case 30: return { T_COORS };
    case 31: return { T_STATGET, T_STATSET };
    default: return {};
    }
}
using PatchList = std::vector<std::unique_ptr<ReversingProbe::EntryPatch>>;
inline PatchList MakePatches(const std::vector<int>& tags) {
    PatchList v;
    for (int t : tags) {
        const auto& s = specs[t];
        v.push_back(std::make_unique<ReversingProbe::EntryPatch>(s.exe, s.stubExe));
        if (s.dll) {
            v.push_back(std::make_unique<ReversingProbe::EntryPatch>(s.dll, s.stubDll));
            if (s.body != s.dll) v.push_back(std::make_unique<ReversingProbe::EntryPatch>(s.body, s.stubDll));
        }
    }
    return v;
}

inline json Summary(const json& o) {
    const std::string d = o.dump();
    return json{ { "digest", Hex(Fnv(d.data(), d.size())) }, { "trace_events", o["trace"].size() }, { "result", o["result"] } };
}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "gangevent1-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "gangevent1-runtime.jsonl"); Write({ { "event", "waiting_for_player" } }); ReversingProbe::InstallFailureCapture("gangevent1"); }
    auto* player = FindPlayerPed(); if (!player || CCutsceneMgr::IsRunning()) return;
    done = true; owner = GetCurrentThreadId(); realPlayer = player;
    std::string only; { std::ifstream f(dir / "gangevent1-only.txt"); if (f) std::getline(f, only); }

    // comprobaciones del entorno
    if (GetPedPool()->GetObjectSize() != PEDSTRIDE || GetVehiclePool()->GetObjectSize() != VEHSTRIDE || GetTaskPool() == nullptr) {
        Write({ { "event", "error" }, { "reason", "tamano de objeto de pool inesperado" } }); return;
    }
    std::vector<ReversibleHooks::ReversibleHook::Base*> hooks(fns.size()); std::vector<bool> enabled(fns.size()), locked(fns.size());
    for (size_t i = 0; i < fns.size(); ++i) {
        hooks[i] = FindHook(fns[i].cls, fns[i].name);
        if (!hooks[i]) { Write({ { "event", "error" }, { "reason", "hook not found" }, { "function", std::string(fns[i].cls) + "::" + fns[i].name } }); return; }
        enabled[i] = hooks[i]->Hooked(); locked[i] = hooks[i]->Locked();
    }
    for (size_t i = 0; i < fns.size(); ++i) hooks[i]->LockState(false);

    // vtables falsas
    FillTraps(std::make_integer_sequence<int, 16>{});
    ftVtbl[0] = reinterpret_cast<void*>(&FtDtor);
    evVtbl[0] = reinterpret_cast<void*>(&EvDtor); evVtbl[1] = reinterpret_cast<void*>(&EvType); evVtbl[4] = reinterpret_cast<void*>(&EvClone);
    evVtbl[9] = reinterpret_cast<void*>(&EvEditable); evVtbl[10] = reinterpret_cast<void*>(&EvSource);

    TakeSnaps();
    const unsigned savedFP = _controlfp(0, 0);
    const auto referencesBefore = ReversingProbe::Bytes(&CReferences::pEmptyList, 4);
    BuildSpecs();
    Write({ { "event", "begin" }, { "fp_control", savedFP }, { "functions", fns.size() } });

    bool passed = true; int pair = 0;
    for (size_t f = 0; f < fns.size(); ++f) {
        const auto& fn = fns[f]; const std::string full = std::string(fn.cls) + "::" + fn.name;
        if (!only.empty() && only != full) continue;
        int failures = 0;
        {
            auto patches = MakePatches(TagsFor(int(f)));
            for (int i = 0; i < fn.cases; ++i) {
                json r[2]; bool sw[2]{};
                for (int side = 0; side < 2; ++side) {
                    r[side] = f < 30 ? RunGroupCase(int(f), i, hooks[f], fn, side != 0, sw[side]) : RunGwCase(int(f), i, hooks[f], fn, side != 0, sw[side]);
                }
                const bool equal = r[0] == r[1];
                const bool ok = equal && sw[0] && sw[1];
                passed &= ok; failures += !ok;
                json rec = { { "event", "comparison" }, { "pair", pair++ }, { "function", full }, { "case", i }, { "equal", equal }, { "accepted", ok } };
                if (ok) {   // pares iguales: resumen compacto (huella del resultado completo, numero de eventos y retorno)
                    rec["original"] = Summary(r[0]); rec["candidate"] = Summary(r[1]);
                } else if (failures <= 3) { rec["original"] = std::move(r[0]); rec["candidate"] = std::move(r[1]); }
                else { rec["original"] = Summary(r[0]); rec["candidate"] = Summary(r[1]); }
                if (!ok) rec["switches"] = json::array({ sw[0], sw[1] });
                Write(rec);
            }
            hooks[f]->State(enabled[f]);
        }   // se restauran las entradas parcheadas
        Write({ { "event", "function_summary" }, { "function", full }, { "failures", failures }, { "cases", fn.cases } });
    }
    RestoreSnaps();
    for (size_t i = 0; i < fns.size(); ++i) { hooks[i]->State(enabled[i]); hooks[i]->LockState(locked[i]); }
    _controlfp(savedFP, _MCW_PC | _MCW_RC);
    bool restored = SnapsEqual() && _controlfp(0, 0) == savedFP && referencesBefore == ReversingProbe::Bytes(&CReferences::pEmptyList, 4);
    Write({ { "event", passed ? "complete" : "error" }, { "pairs", pair }, { "state_restored", restored } });
}
}
