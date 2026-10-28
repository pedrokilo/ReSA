#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <new>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "CarCtrl.h"
#include "CarAI.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Bike.h"
#include "Entity/Ped/Ped.h"
#include "PedIntelligence.h"
#include "Camera.h"
#include "Core/Matrix.h"
#include "Events/EventAcquaintancePedHate.h"
#include "Tasks/TaskTypes/TaskComplexDriveWander.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par CarPolice75 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CCarCtrl::CreatePoliceChase (0x42C2B0, hook S, cdecl (plantilla, tipo de ped, CNodeAddress), `add esp,0Ch` del llamador 0x431D34; resultado en AL). Colaboradores sustituidos por falsos identicos en la direccion
// del EXE y en el cuerpo de la DLL (con hook RH, apagado durante el banco): rand 0x821B1E (sin hook), ChoosePoliceCarModel 0x421980, GetNewVehicleDependingOnCarModel 0x421440, CPathNode::GetPosition 0x420A10,
// CWorld::ProcessVerticalLine 0x5674E0 (dos veces), CEntity::GetIsOnScreen 0x534540, CCamera::IsSphereVisible 0x420D40 (sin hook: la DLL llama a su propia funcion), CWorld::FindObjectsKindaColliding 0x568B80,
// SetUpDriverAndPassengersForVehicle 0x4217C0, CMatrix::operator= 0x59BBC0, CPlaceable::SetPosn 0x4241C0, CVehicle::ChangeLawEnforcerState 0x6D2330, CWorld::Add 0x563220, PlaceOnRoadProperly de moto 0x6BEEB0 y de
// automovil 0x6AF420, CCarAI::AddPoliceCarOccupants 0x41C070, CTask::operator new 0x61A5A0, el constructor de CTaskComplexCarDriveWander 0x63CB10, CTaskManager::SetTask 0x681AF0 (sin hook: la DLL llama a la direccion
// del EXE), CEventGroup::Add 0x4AB420, CEntity::RegisterReference 0x571B70 y CleanUpOldReference 0x571A00 (el constructor y el destructor del evento corren de verdad en ambos lados). Los metodos virtuales del coche nuevo
// (destructor, ranura 0; GetHeightAboveRoad, ranura 53 = +0xD4) se sustituyen con una vtable falsa propia en la arena, comun a ambos lados. Todo lo demas es la propia funcion.

namespace Cp75Layout {
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCruiseSpeed) == 0x3D0 && offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCarDrivingStyle) == 0x3B9, "CAutoPilot");
static_assert(offsetof(CVehicle, m_pDriver) == 0x460 && offsetof(CVehicle, m_apPassengers) == 0x464 && offsetof(CVehicle, m_nMaxPassengers) == 0x488 && offsetof(CVehicle, m_nExtendedRemovalRange) == 0x4A6, "CVehicle: ocupantes");
static_assert(offsetof(CVehicle, m_nVehicleType) == 0x590 && offsetof(CPhysical, m_vecMoveSpeed) == 0x44, "CVehicle: tipo +0x590, velocidad +0x44");
struct PedLayout : CPed { static constexpr size_t Intelligence(); };   // miembro protected
constexpr size_t PedLayout::Intelligence() { return offsetof(PedLayout, m_pIntelligence); }
static_assert(PedLayout::Intelligence() == 0x47C && offsetof(CPedIntelligence, m_TaskMgr) == 4 && offsetof(CPedIntelligence, m_eventGroup) == 0x68, "CPed / CPedIntelligence");
static_assert(sizeof(CEventAcquaintancePedHate) == 0x18, "evento de 0x18 bytes");
// Posiciones de campos de bits: se comprueban en ejecucion
inline bool LayoutOK() {
    alignas(16) static std::array<uint8, 0x5A0> mem{};
    bool ok = true;
    auto* v = reinterpret_cast<CVehicle*>(mem.data());
    mem.fill(0); v->vehicleFlags.bMadDriver = true; ok &= mem[0x42E] == 0x08 && mem[0x42B] == 0 && mem[0x42C] == 0 && mem[0x42F] == 0;
    mem.fill(0); v->vehicleFlags.bCreatedAsPoliceVehicle = true; ok &= mem[0x42C] == 0x01 && mem[0x42B] == 0 && mem[0x42D] == 0;
    mem.fill(0); v->vehicleFlags.bNeverUseSmallerRemovalRange = true; ok &= mem[0x42C] == 0x40 && mem[0x42B] == 0 && mem[0x42D] == 0;
    mem.fill(0); v->vehicleFlags.bPartOfConvoy = true; ok &= mem[0x42B] == 0x08 && mem[0x42A] == 0 && mem[0x42C] == 0;
    mem.fill(0); mem[0x36] = 0x05; v->SetStatus(STATUS_PHYSICS); ok &= mem[0x36] == 0x1D;
    return ok;
}
}

namespace Cp75Live {
using namespace Cp75Fix;
inline constexpr uintptr_t kEntry = 0x42C2B0;
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }

// ================================================================================================ arenas
template<size_t N, size_t G = kGuard> struct Arena {
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj() const { return uintptr_t(m.data()) + G; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + G + off, &v, 4); }
    void Put16(size_t off, u32 v) { const u16 w = u16(v); memcpy(m.data() + G + off, &w, 2); }
    void Put8(size_t off, u32 v) { m[G + off] = u8(v); }
    json Changes() const { return ArenaChanges(m.data(), r.data(), N, G); }
    void Tagged() const { TagAdd(m.data(), N); }
};
// Indices de etiqueta: 0 plantilla, 1 coche nuevo, 2 matriz de la plantilla, 3 matriz del coche nuevo, 4 conductor de la plantilla, 5..12 pasajeros de la plantilla, 13 conductor del coche nuevo,
// 14 inteligencia del conductor nuevo, 15 tarea, 16 nodos, 17 informacion de modelo, 18 modelo de colision
inline Arena<kArena(kVehWin)> tA, pA;
inline Arena<kArena(kMatWin)> tM, pM;
inline Arena<kArena(kPedWin)> tdA, pdA;
inline std::array<Arena<kArena(kPedWin)>, 8> psA;
inline Arena<kArena(kIntelWin)> inA;
inline Arena<kArena(kTaskWin)> taskA;
inline Arena<kArena(kNodeWin)> nodeA;
inline Arena<kArena(kInfoWin)> infoA, colA;

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

// ================================================================================================ CreatePoliceChase (0x42C2B0)
struct State { const C* c{}; unsigned pvl{}; };
inline State g;
// Eventos (kind): 1 rand(); 2 ChoosePoliceCarModel(arg); 3 GetNewVehicle(modelo, creado por); 4 GetPosition(nodo); 5 ProcessVerticalLine(punto, distancia, ...); 6 GetHeightAboveRoad(this); 7 GetIsOnScreen(this);
// 8 IsSphereVisible(camara, origen, radio); 9 FindObjectsKindaColliding(punto, radio, ...); 10 destructor virtual (this, indicador); 11 SetUpDriverAndPassengers(...); 12 CMatrix::operator=(this, origen);
// 13 SetPosn(this, posicion); 14 ChangeLawEnforcerState(this, bool); 15 CWorld::Add(entidad); 16 PlaceOnRoadProperly de moto; 17 PlaceOnRoadProperly de automovil; 18 AddPoliceCarOccupants(coche, bool);
// 19 CTask::operator new(tamano); 20 constructor de la tarea de conduccion; 21 SetTask(gestor, tarea, ranura, bool); 22 RegisterReference(this); 23 CleanUpOldReference(this); 24 CEventGroup::Add(grupo, evento, bool);
// 99 metodo virtual inesperado. Los punteros dentro de una arena se etiquetan; los punteros a la pila de la funcion probada solo se registran como "no nulos".
inline u32 NonNull(const void* p) { return p ? 1u : 0u; }
inline u32 __cdecl FakeRand() { PushEv(1, 0); return g.c->rnd; }
inline i32 __cdecl FakeChoose(u32 a) { Ev& e = PushEv(2, 1); e.v[0] = a; return i32(g.c->model); }
inline void* __cdecl FakeGetNew(u32 model, u32 createdBy) { Ev& e = PushEv(3, 2); e.v[0] = model; e.v[1] = createdBy; return g.c->veh_null ? nullptr : reinterpret_cast<void*>(pA.Obj()); }
inline void* __fastcall FakeGetPos(void* node, void*, u32* out) { Ev& e = PushEv(4, 1); e.v[0] = Tag(node); out[0] = g.c->node_pos[0]; out[1] = g.c->node_pos[1]; out[2] = g.c->node_pos[2]; return out; }
inline u32 __cdecl FakeVertical(const u32* pt, u32 dist, void* col, void* ent, u32 buildings, u32 vehicles, u32 peds, u32 objects, u32 dummies, u32 seeThrough, u32 poly) {
    Ev& e = PushEv(5, 13);
    e.v[0] = pt[0]; e.v[1] = pt[1]; e.v[2] = pt[2]; e.v[3] = dist; e.v[4] = NonNull(col); e.v[5] = NonNull(ent);
    e.v[6] = buildings; e.v[7] = vehicles; e.v[8] = peds; e.v[9] = objects; e.v[10] = dummies; e.v[11] = seeThrough; e.v[12] = poly;
    const bool first = g.pvl++ == 0;
    const u32 hit = first ? g.c->hit1 : g.c->hit2;
    if (hit & 0xFFu) reinterpret_cast<u32*>(col)[2] = first ? g.c->hit1z : g.c->hit2z;   // CColPoint::m_vecPoint.z
    return hit;
}
inline float __fastcall FakeHeight(void* self, void*) { Ev& e = PushEv(6, 1); e.v[0] = Tag(self); return F(g.c->height); }
inline u32 __fastcall FakeOnScreen(void* self, void*) { Ev& e = PushEv(7, 1); e.v[0] = Tag(self); return g.c->on_screen; }
inline u32 __fastcall FakeSphere(void* self, void*, const u32* origin, u32 radius) { Ev& e = PushEv(8, 5); e.v[0] = u32(uintptr_t(self)); e.v[1] = origin[0]; e.v[2] = origin[1]; e.v[3] = origin[2]; e.v[4] = radius; return g.c->visible; }
inline void __cdecl FakeFind(const u32* pt, u32 radius, u32 b2d, i16* count, u32 maxCount, void* list, u32 buildings, u32 vehicles, u32 peds, u32 objects, u32 dummies) {
    Ev& e = PushEv(9, 13);
    e.v[0] = pt[0]; e.v[1] = pt[1]; e.v[2] = pt[2]; e.v[3] = radius; e.v[4] = b2d; e.v[5] = NonNull(count); e.v[6] = maxCount; e.v[7] = u32(uintptr_t(list));
    e.v[8] = buildings; e.v[9] = vehicles; e.v[10] = peds; e.v[11] = objects; e.v[12] = dummies;
    *count = i16(g.c->colliding & 0xFFFFu);   // solo 16 bits: el contador del EXE es una palabra en la pila
}
inline void* __fastcall FakeDelete(void* self, void*, u32 flag) { Ev& e = PushEv(10, 2); e.v[0] = Tag(self); e.v[1] = flag; return self; }
inline void __fastcall FakeTrap(void* self, void*) { Ev& e = PushEv(99, 1); e.v[0] = Tag(self); }
inline void __cdecl FakeSetUp(void* veh, u32 pedType, u32 a3, u32 a4, u32 a5, u32 passengers) { Ev& e = PushEv(11, 6); e.v[0] = Tag(veh); e.v[1] = pedType; e.v[2] = a3; e.v[3] = a4; e.v[4] = a5; e.v[5] = passengers; }
inline void __fastcall FakeMatAssign(void* self, void*, void* src) { Ev& e = PushEv(12, 2); e.v[0] = Tag(self); e.v[1] = Tag(src); }
inline void __fastcall FakeSetPosn(void* self, void*, const u32* pos) { Ev& e = PushEv(13, 4); e.v[0] = Tag(self); e.v[1] = pos[0]; e.v[2] = pos[1]; e.v[3] = pos[2]; }
inline void __fastcall FakeLaw(void* self, void*, u32 flag) { Ev& e = PushEv(14, 2); e.v[0] = Tag(self); e.v[1] = flag & 0xFFu; }
inline void __cdecl FakeWorldAdd(void* entity) { Ev& e = PushEv(15, 1); e.v[0] = Tag(entity); }
inline void __fastcall FakePlaceBike(void* self, void*) { Ev& e = PushEv(16, 1); e.v[0] = Tag(self); }
inline void __fastcall FakePlaceAuto(void* self, void*) { Ev& e = PushEv(17, 1); e.v[0] = Tag(self); }
inline void __cdecl FakeOccupants(void* veh, u32 flag) { Ev& e = PushEv(18, 2); e.v[0] = Tag(veh); e.v[1] = flag & 0xFFu; }
inline void* __cdecl FakeTaskNew(u32 size) { Ev& e = PushEv(19, 1); e.v[0] = size; return g.c->task_null ? nullptr : reinterpret_cast<void*>(taskA.Obj()); }
inline void* __fastcall FakeWander(void* self, void*, void* veh, u32 style, u32 speed) { Ev& e = PushEv(20, 4); e.v[0] = Tag(self); e.v[1] = Tag(veh); e.v[2] = style; e.v[3] = speed; return self; }
inline void __fastcall FakeSetTask(void* self, void*, void* task, u32 slot, u32 flag) { Ev& e = PushEv(21, 4); e.v[0] = Tag(self); e.v[1] = Tag(task); e.v[2] = slot; e.v[3] = flag & 0xFFu; }
inline void __fastcall FakeRegister(void* self, void*, void*) { Ev& e = PushEv(22, 1); e.v[0] = Tag(self); }
inline void __fastcall FakeCleanUp(void* self, void*, void*) { Ev& e = PushEv(23, 1); e.v[0] = Tag(self); }
// El evento vive en la pila de la funcion probada: solo se comparan los campos que su constructor define (+4 dword, +8 byte, +0xC byte, +0xE word, +0x10 word, +0x14 puntero), no el relleno ni la vtable.
inline void* __fastcall FakeGroupAdd(void* self, void*, const u8* ev, u32 flag) {
    Ev& e = PushEv(24, 8);
    e.v[0] = Tag(self); e.v[1] = *reinterpret_cast<const u32*>(ev + 4); e.v[2] = ev[8]; e.v[3] = ev[0xC]; e.v[4] = *reinterpret_cast<const u16*>(ev + 0xE);
    e.v[5] = *reinterpret_cast<const u16*>(ev + 0x10); e.v[6] = Tag(uintptr_t(*reinterpret_cast<const u32*>(ev + 0x14))); e.v[7] = flag & 0xFFu;
    return nullptr;
}
inline constexpr size_t kVtblSlots = 66;
inline std::array<void*, kVtblSlots> vtbl{};
inline void InitVtbl() {
    for (auto& s : vtbl) s = reinterpret_cast<void*>(&FakeTrap);
    vtbl[0] = reinterpret_cast<void*>(&FakeDelete);
    vtbl[53] = reinterpret_cast<void*>(&FakeHeight);   // +0xD4: CVehicle::GetHeightAboveRoad
}

inline void CasePolice(Env& env, Totals& t, unsigned n) {
    const C c = MakeC(n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        tA.Fill(c.seed * 13u + 1u); pA.Fill(c.seed * 13u + 2u); tM.Fill(c.seed * 13u + 3u); pM.Fill(c.seed * 13u + 4u); tdA.Fill(c.seed * 13u + 5u); pdA.Fill(c.seed * 13u + 6u);
        for (unsigned i = 0; i < 8; ++i) psA[i].Fill(c.seed * 13u + 7u + i);
        inA.Fill(c.seed * 13u + 20u); taskA.Fill(c.seed * 13u + 21u); nodeA.Fill(c.seed * 13u + 22u); infoA.Fill(c.seed * 13u + 23u); colA.Fill(c.seed * 13u + 24u);
        // plantilla
        tA.Put32(0, u32(uintptr_t(vtbl.data())));
        tA.Put32(0x14, c.tmat ? u32(tM.Obj()) : 0u);
        tA.Put32(0x10, c.head);
        tA.Put8(0x3D0, c.cruise);
        tA.Put32(0x460, u32(tdA.Obj()));
        for (unsigned i = 0; i < 8; ++i) tA.Put32(0x464 + 4 * i, (c.pmask >> i) & 1 ? u32(psA[i].Obj()) : 0u);
        tA.Put8(0x488, c.pcount);
        for (int i = 0; i < 3; ++i) tM.Put32(0x10 + 4 * i, c.fwd[i]);
        // coche nuevo
        pA.Put32(0, u32(uintptr_t(vtbl.data())));
        pA.Put32(0x14, u32(pM.Obj()));
        pA.Put16(0x22, c.pmodel);
        pA.Put32(0x590, c.ptype);
        pA.Put32(0x460, u32(pdA.Obj()));
        pdA.Put32(0x47C, u32(inA.Obj()));
        infoA.Put32(0x14, u32(colA.Obj()));
        colA.Put32(0x24, c.radius);
        tA.Snap(); pA.Snap(); tM.Snap(); pM.Snap(); tdA.Snap(); pdA.Snap(); for (auto& a : psA) a.Snap(); inA.Snap(); taskA.Snap(); nodeA.Snap(); infoA.Snap(); colA.Snap();
        // globales que lee la funcion: 0xC0BC38, tabla de carga, tabla de modelos y tabla de nodos de ruta
        G32(0xC0BC38) = c.gangs;
        if (i32(c.model) >= 0 && c.model < kModels) *reinterpret_cast<uint8*>(0x8E4CD0 + c.model * 20) = u8(c.loaded);
        G32(0xA9B0C8 + 4 * c.pmodel) = u32(infoA.Obj());
        G32(0x96F854 + 4 * c.area) = u32(nodeA.Obj());
        TagClear(); tA.Tagged(); pA.Tagged(); tM.Tagged(); pM.Tagged(); tdA.Tagged(); for (auto& a : psA) a.Tagged(); pdA.Tagged(); inA.Tagged(); taskA.Tagged(); nodeA.Tagged(); infoA.Tagged(); colA.Tagged();
        g.c = &c; g.pvl = 0; nev = 0;
        const u32 args[3]{u32(tA.Obj()), c.ped_type, c.area | (c.node << 16)};
        sides[s] = Execute(env, 0, kEntry, candidate, [&](uintptr_t entry) { return Gate(entry, args, 3, c.eax, c.ecx, c.edx, c.stack, 12); });
        json obs;
        obs["ret"] = sides[s].reg.eax & 0xFFu;
        obs["changes"] = json::array({tA.Changes(), pA.Changes(), tM.Changes(), pM.Changes(), tdA.Changes(), pdA.Changes(), inA.Changes(), taskA.Changes(), nodeA.Changes(), infoA.Changes(), colA.Changes()});
        json peds = json::array(); for (auto& a : psA) peds.push_back(a.Changes());
        obs["passengers"] = peds;
        obs["events"] = EvsJson();
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[0], n, input, rows, sides);
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
    const bool layout = Cp75Layout::LayoutOK();
    ready &= layout;
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases}, {"layout_ok", layout},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < kCases; ++c) CasePolice(env, totals, c);
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
// Colaborador con hook: se crea su ruta (el hook queda apagado) y se toma el cuerpo de la DLL; sin hook (dll = 0) la DLL llama a la direccion del EXE.
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
inline bool Prologue(uintptr_t f) { const auto* b = reinterpret_cast<const uint8*>(f); if (b[0] == 0x8B && b[1] == 0xFF) b += 2; return b[0] == 0x55 && b[1] == 0x8B && b[2] == 0xEC; }
// Primera llamada (E8) de `fn` (dentro del ASI) a una funcion del ASI con prologo push ebp / mov ebp,esp: el cuerpo real del constructor que llama la envoltura `Constructor` del hook.
inline uintptr_t FirstDllCall(uintptr_t asi, uintptr_t fn) {
    fn = Body(fn); if (!fn || ModuleOf(fn) != asi) return 0;
    for (uintptr_t p = fn; p < fn + 0x300; ++p) {
        if (*reinterpret_cast<const uint8*>(p) != 0xE8) continue;
        const uintptr_t t = p + 5 + *reinterpret_cast<const int32*>(p + 1);
        if (ModuleOf(t) == asi && Prologue(Body(t))) return Body(t);
    }
    return 0;
}
template<class M> inline uintptr_t MemberBody(M member) { return Body(std::bit_cast<uintptr_t>(member)); }

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "carpolice75-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "carpolice75-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("carpolice75"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de la funcion probada: se construye ANTES que nada; el constructor deja el hook apagado. El cuerpo de la DLL se calcula aqui, antes de crear las rutas de los colaboradores y los parches de entrada.
    {
        std::string cat, name;
        const bool found = FindHook(kEntry, cat, name);
        if (found) env.route[0].reset(new HookRoute(cat.c_str(), name.c_str(), kEntry, HookRoute::RegisteredTarget{}));
        registered[kFn[0]] = found && env.route[0]->Registered(); bodies[kFn[0]] = found && env.route[0]->Body() != 0;
        ready &= found && env.route[0]->Registered() && env.route[0]->Body() && ModuleOf(env.route[0]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[0]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntry}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    InitVtbl();
    const Pair pRand{0x821B1E, 0, false};
    const Pair pChoose = MakeCollab(env, 0x421980, hooksFound), pGetNew = MakeCollab(env, 0x421440, hooksFound), pGetPos = MakeCollab(env, 0x420A10, hooksFound), pVertical = MakeCollab(env, 0x5674E0, hooksFound);
    const Pair pOnScreen = MakeCollab(env, 0x534540, hooksFound);
    const Pair pSphere = MakeCollab(env, 0x420D40, hooksFound);   // con hook desde el lote 76 (CamSphere76)   // con hook desde el lote 76 (CamSphere76): el EXE salta a la DLL y el falso tambien parchea el cuerpo de la DLL
    const Pair pFind = MakeCollab(env, 0x568B80, hooksFound), pSetUp = MakeCollab(env, 0x4217C0, hooksFound), pMat = MakeCollab(env, 0x59BBC0, hooksFound)   /* con hook desde el lote 76 (MatrixOps76) */, pPosn = MakeCollab(env, 0x4241C0, hooksFound);
    const Pair pLaw = MakeCollab(env, 0x6D2330, hooksFound), pAdd = MakeCollab(env, 0x563220, hooksFound), pBike = MakeCollab(env, 0x6BEEB0, hooksFound), pAuto = MakeCollab(env, 0x6AF420, hooksFound);
    const Pair pOcc = MakeCollab(env, 0x41C070, hooksFound), pNew = MakeCollab(env, 0x61A5A0, hooksFound);
    Pair pWander = MakeCollab(env, 0x63CB10, hooksFound); pWander.dll = pWander.dll ? FirstDllCall(env.asi, pWander.dll) : 0;   // el cuerpo real del constructor: primera llamada de la envoltura `Constructor`
    const Pair pSetTask = [&] { Pair p = MakeCollab(env, 0x681AF0, hooksFound); p.dll = 0; p.hooked = false; return p; }();   // definido en linea en la cabecera: la DLL llama a la direccion del EXE; el hook (si existe) queda apagado
    const Pair pGroup = MakeCollab(env, 0x4AB420, hooksFound), pReg = MakeCollab(env, 0x571B70, hooksFound), pClean = MakeCollab(env, 0x571A00, hooksFound);
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    auto V = [](auto f) { return reinterpret_cast<void*>(f); };
    env.stubs[0] = {S(pRand, V(&FakeRand)), S(pChoose, V(&FakeChoose)), S(pGetNew, V(&FakeGetNew)), S(pGetPos, V(&FakeGetPos)), S(pVertical, V(&FakeVertical)), S(pOnScreen, V(&FakeOnScreen)),
        S(pSphere, V(&FakeSphere)), S(pFind, V(&FakeFind)), S(pSetUp, V(&FakeSetUp)), S(pMat, V(&FakeMatAssign)), S(pPosn, V(&FakeSetPosn)), S(pLaw, V(&FakeLaw)), S(pAdd, V(&FakeWorldAdd)),
        S(pBike, V(&FakePlaceBike)), S(pAuto, V(&FakePlaceAuto)), S(pOcc, V(&FakeOccupants)), S(pNew, V(&FakeTaskNew)), S(pWander, V(&FakeWander)), S(pSetTask, V(&FakeSetTask)),
        S(pGroup, V(&FakeGroupAdd)), S(pReg, V(&FakeRegister)), S(pClean, V(&FakeCleanUp))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"ninguno: constructor y destructor del evento (CEventAcquaintancePed), CVector/CMatrix locales y la aritmetica de X87Math corren de verdad"})}, {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry, 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
