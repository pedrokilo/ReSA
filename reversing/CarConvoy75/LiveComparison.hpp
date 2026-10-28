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
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Bike.h"
#include "Entity/Ped/Ped.h"
#include "PedIntelligence.h"
#include "Core/Matrix.h"
#include "Tasks/TaskTypes/TaskComplexDriveWander.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par CarConvoy75 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CCarCtrl::CreateConvoy (0x42C740, hook S, cdecl (plantilla, tipo de ped), `add esp,8` del llamador 0x431DCC; resultado en AL). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en
// el cuerpo de la DLL (con hook RH, apagado durante el banco): rand 0x821B1E (sin hook), GetNewVehicleDependingOnCarModel 0x421440, CWorld::ProcessVerticalLine 0x5674E0, CWorld::FindObjectsKindaColliding 0x568B80,
// SetUpDriverAndPassengersForVehicle 0x4217C0, CMatrix::operator= 0x59BBC0, CWorld::Add 0x563220, PlaceOnRoadProperly de moto 0x6BEEB0 y de automovil 0x6AF420, CTask::operator new 0x61A5A0, el constructor de
// CTaskComplexCarDriveWander 0x63CB10, CTaskManager::SetTask 0x681AF0 (sin hook: la DLL llama a la direccion del EXE) y CEntity::RegisterReference 0x571B70. Los metodos virtuales de los coches nuevos (destructor,
// ranura 0; GetHeightAboveRoad, ranura 53 = +0xD4) se sustituyen con una vtable falsa propia en la arena, comun a ambos lados. Todo lo demas es la propia funcion.

namespace Cv75Layout {
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCruiseSpeed) == 0x3D0 && offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCarDrivingStyle) == 0x3B9
    && offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCarMission) == 0x3BA && offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_speed) == 0x3C8
    && offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_TargetEntity) == 0x41C, "CAutoPilot");
static_assert(offsetof(CVehicle, m_pDriver) == 0x460 && offsetof(CVehicle, m_nForcedRandomRouteSeed) == 0x45E && offsetof(CEntity, m_nRandomSeed) == 0x20, "CVehicle: ocupantes y semillas");
static_assert(offsetof(CVehicle, m_nVehicleType) == 0x590 && offsetof(CPhysical, m_vecMoveSpeed) == 0x44, "CVehicle: tipo +0x590, velocidad +0x44");
struct PedLayout : CPed { static constexpr size_t Intelligence(); };   // miembro protected
constexpr size_t PedLayout::Intelligence() { return offsetof(PedLayout, m_pIntelligence); }
static_assert(PedLayout::Intelligence() == 0x47C && offsetof(CPedIntelligence, m_TaskMgr) == 4, "CPed / CPedIntelligence");
// Posiciones de campos de bits: se comprueban en ejecucion
inline bool LayoutOK() {
    alignas(16) static std::array<uint8, 0x5A0> mem{};
    bool ok = true;
    auto* v = reinterpret_cast<CVehicle*>(mem.data());
    mem.fill(0); v->vehicleFlags.bPartOfConvoy = true; ok &= mem[0x42B] == 0x08 && mem[0x42A] == 0 && mem[0x42C] == 0;
    mem.fill(0); mem[0x36] = 0x05; v->SetStatus(STATUS_PHYSICS); ok &= mem[0x36] == 0x1D;
    mem.fill(0); mem[0x36] = 0xFD; v->SetStatus(v->GetStatus()); ok &= mem[0x36] == 0xFD;
    return ok;
}
}

namespace Cv75Live {
using namespace Cv75Fix;
inline constexpr uintptr_t kEntry = 0x42C740;
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
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + G + off, 4); return v; }
    void Hide32(size_t off) { memcpy(m.data() + G + off, r.data() + G + off, 4); }   // el campo es un puntero: se observa etiquetado y no entre los bytes cambiados
    json Changes() const { return ArenaChanges(m.data(), r.data(), N, G); }
    void Tagged() const { TagAdd(m.data(), N); }
};
// Indices de etiqueta: 0 plantilla, 1..3 coches, 4 matriz de la plantilla, 5..7 matrices de los coches, 8..10 conductores, 11..13 inteligencias, 14..16 tareas, 17 informacion de modelo, 18 modelo de colision
inline Arena<kArena(kVehWin)> tA;
inline std::array<Arena<kArena(kVehWin)>, 3> cA;
inline Arena<kArena(kMatWin)> tM;
inline std::array<Arena<kArena(kMatWin)>, 3> cM;
inline std::array<Arena<kArena(kPedWin)>, 3> cdA;
inline std::array<Arena<kArena(kIntelWin)>, 3> ciA;
inline std::array<Arena<kArena(kTaskWin)>, 3> tkA;
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

// ================================================================================================ CreateConvoy (0x42C740)
struct State { const C* c{}; unsigned getNew{}, vertical{}, height{}, find{}, task{}; };
inline State g;
// Eventos (kind): 1 rand(); 2 GetNewVehicle(modelo, creado por); 3 ProcessVerticalLine(punto, distancia, ...); 4 GetHeightAboveRoad(this); 5 FindObjectsKindaColliding(punto, radio, ...); 6 destructor virtual (this, indicador);
// 7 CMatrix::operator=(this, origen); 8 CWorld::Add(entidad); 9 PlaceOnRoadProperly de moto; 10 PlaceOnRoadProperly de automovil; 11 SetUpDriverAndPassengers(...); 12 CTask::operator new(tamano);
// 13 constructor de la tarea de conduccion; 14 SetTask(gestor, tarea, ranura, bool); 15 RegisterReference(this); 99 metodo virtual inesperado. Los punteros dentro de una arena se etiquetan; los punteros a la pila
// de la funcion probada solo se registran como "no nulos". Los falsos sucesivos toman los datos del caso en orden de llamada.
inline u32 NonNull(const void* p) { return p ? 1u : 0u; }
inline u32 __cdecl FakeRand() { PushEv(1, 0); return g.c->rnd; }
inline void* __cdecl FakeGetNew(u32 model, u32 createdBy) {
    Ev& e = PushEv(2, 2); e.v[0] = model; e.v[1] = createdBy;
    const unsigned i = g.getNew++;
    return i < 3 && !((g.c->null_mask >> i) & 1) ? reinterpret_cast<void*>(cA[i].Obj()) : nullptr;
}
inline u32 __cdecl FakeVertical(const u32* pt, u32 dist, void* col, void* ent, u32 buildings, u32 vehicles, u32 peds, u32 objects, u32 dummies, u32 seeThrough, u32 poly) {
    Ev& e = PushEv(3, 13);
    e.v[0] = pt[0]; e.v[1] = pt[1]; e.v[2] = pt[2]; e.v[3] = dist; e.v[4] = NonNull(col); e.v[5] = NonNull(ent);
    e.v[6] = buildings; e.v[7] = vehicles; e.v[8] = peds; e.v[9] = objects; e.v[10] = dummies; e.v[11] = seeThrough; e.v[12] = poly;
    const unsigned i = g.vertical++ % 6;
    const u32 hit = g.c->hit[i];
    if (hit & 0xFFu) reinterpret_cast<u32*>(col)[2] = g.c->hitz[i];   // CColPoint::m_vecPoint.z
    return hit;
}
inline float __fastcall FakeHeight(void* self, void*) { Ev& e = PushEv(4, 1); e.v[0] = Tag(self); return F(g.c->height[g.height++ % 3]); }
inline void __cdecl FakeFind(const u32* pt, u32 radius, u32 b2d, i16* count, u32 maxCount, void* list, u32 buildings, u32 vehicles, u32 peds, u32 objects, u32 dummies) {
    Ev& e = PushEv(5, 13);
    e.v[0] = pt[0]; e.v[1] = pt[1]; e.v[2] = pt[2]; e.v[3] = radius; e.v[4] = b2d; e.v[5] = NonNull(count); e.v[6] = maxCount; e.v[7] = u32(uintptr_t(list));
    e.v[8] = buildings; e.v[9] = vehicles; e.v[10] = peds; e.v[11] = objects; e.v[12] = dummies;
    *count = i16(g.c->col[g.find++ % 3] & 0xFFFFu);   // solo 16 bits: el contador del EXE es una palabra en la pila
}
inline void* __fastcall FakeDelete(void* self, void*, u32 flag) { Ev& e = PushEv(6, 2); e.v[0] = Tag(self); e.v[1] = flag; return self; }
inline void __fastcall FakeTrap(void* self, void*) { Ev& e = PushEv(99, 1); e.v[0] = Tag(self); }
inline void __fastcall FakeMatAssign(void* self, void*, void* src) { Ev& e = PushEv(7, 2); e.v[0] = Tag(self); e.v[1] = Tag(src); }
inline void __cdecl FakeWorldAdd(void* entity) { Ev& e = PushEv(8, 1); e.v[0] = Tag(entity); }
inline void __fastcall FakePlaceBike(void* self, void*) { Ev& e = PushEv(9, 1); e.v[0] = Tag(self); }
inline void __fastcall FakePlaceAuto(void* self, void*) { Ev& e = PushEv(10, 1); e.v[0] = Tag(self); }
inline void __cdecl FakeSetUp(void* veh, u32 pedType, u32 a3, u32 a4, u32 a5, u32 passengers) { Ev& e = PushEv(11, 6); e.v[0] = Tag(veh); e.v[1] = pedType; e.v[2] = a3; e.v[3] = a4; e.v[4] = a5; e.v[5] = passengers; }
inline void* __cdecl FakeTaskNew(u32 size) { Ev& e = PushEv(12, 1); e.v[0] = size; const unsigned i = g.task++; return i < 3 && !g.c->ctask_null[i] ? reinterpret_cast<void*>(tkA[i].Obj()) : nullptr; }
inline void* __fastcall FakeWander(void* self, void*, void* veh, u32 style, u32 speed) { Ev& e = PushEv(13, 4); e.v[0] = Tag(self); e.v[1] = Tag(veh); e.v[2] = style; e.v[3] = speed; return self; }
inline void __fastcall FakeSetTask(void* self, void*, void* task, u32 slot, u32 flag) { Ev& e = PushEv(14, 4); e.v[0] = Tag(self); e.v[1] = Tag(task); e.v[2] = slot; e.v[3] = flag & 0xFFu; }
inline void __fastcall FakeRegister(void* self, void*, void*) { Ev& e = PushEv(15, 1); e.v[0] = Tag(self); }
inline constexpr size_t kVtblSlots = 66;
inline std::array<void*, kVtblSlots> vtbl{};
inline void InitVtbl() {
    for (auto& s : vtbl) s = reinterpret_cast<void*>(&FakeTrap);
    vtbl[0] = reinterpret_cast<void*>(&FakeDelete);
    vtbl[53] = reinterpret_cast<void*>(&FakeHeight);   // +0xD4: CVehicle::GetHeightAboveRoad
}

inline void CaseConvoy(Env& env, Totals& t, unsigned n) {
    const C c = MakeC(n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        tA.Fill(c.seed * 13u + 1u); tM.Fill(c.seed * 13u + 2u); infoA.Fill(c.seed * 13u + 3u); colA.Fill(c.seed * 13u + 4u);
        for (unsigned i = 0; i < 3; ++i) { cA[i].Fill(c.seed * 13u + 10u + i); cM[i].Fill(c.seed * 13u + 20u + i); cdA[i].Fill(c.seed * 13u + 30u + i); ciA[i].Fill(c.seed * 13u + 40u + i); tkA[i].Fill(c.seed * 13u + 50u + i); }
        // plantilla
        tA.Put32(0, u32(uintptr_t(vtbl.data())));
        tA.Put32(0x14, u32(tM.Obj()));
        tA.Put16(0x22, c.tmodel);
        tA.Put8(0x3D0, c.cruise);
        tA.Put32(0x3C8, c.speed);
        for (int i = 0; i < 3; ++i) { tM.Put32(0x10 + 4 * i, c.fwd[i]); tM.Put32(0x30 + 4 * i, c.pos[i]); }
        infoA.Put32(0x14, u32(colA.Obj()));
        colA.Put32(0x24, c.radius);
        // coches
        for (unsigned i = 0; i < 3; ++i) {
            cA[i].Put32(0, u32(uintptr_t(vtbl.data())));
            cA[i].Put32(0x14, c.cmat[i] ? u32(cM[i].Obj()) : 0u);
            cA[i].Put32(0x590, c.ctype[i]);
            cA[i].Put32(0x460, u32(cdA[i].Obj()));
            cdA[i].Put32(0x47C, u32(ciA[i].Obj()));
        }
        tA.Snap(); tM.Snap(); infoA.Snap(); colA.Snap();
        for (unsigned i = 0; i < 3; ++i) { cA[i].Snap(); cM[i].Snap(); cdA[i].Snap(); ciA[i].Snap(); tkA[i].Snap(); }
        G32(0xA9B0C8 + 4 * c.tmodel) = u32(infoA.Obj());
        TagClear(); tA.Tagged(); for (auto& a : cA) a.Tagged(); tM.Tagged(); for (auto& a : cM) a.Tagged(); for (auto& a : cdA) a.Tagged(); for (auto& a : ciA) a.Tagged(); for (auto& a : tkA) a.Tagged(); infoA.Tagged(); colA.Tagged();
        g = State{}; g.c = &c; nev = 0;
        const u32 args[2]{u32(tA.Obj()), c.ped_type};
        sides[s] = Execute(env, 0, kEntry, candidate, [&](uintptr_t entry) { return Gate(entry, args, 2, c.eax, c.ecx, c.edx, c.stack, 8); });
        json obs;
        obs["ret"] = sides[s].reg.eax & 0xFFu;
        json targets = json::array();
        for (auto& a : cA) { targets.push_back(Tag(uintptr_t(a.Get32(0x41C)))); a.Hide32(0x41C); }   // m_autoPilot.m_TargetEntity: puntero al coche anterior
        obs["targets"] = targets;
        json all = json::array({tA.Changes()});
        for (auto& a : cA) all.push_back(a.Changes());
        all.push_back(tM.Changes());
        for (auto& a : cM) all.push_back(a.Changes());
        for (auto& a : cdA) all.push_back(a.Changes());
        for (auto& a : ciA) all.push_back(a.Changes());
        for (auto& a : tkA) all.push_back(a.Changes());
        all.push_back(infoA.Changes()); all.push_back(colA.Changes());
        obs["changes"] = all;
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
    const bool layout = Cv75Layout::LayoutOK();
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
            for (unsigned c = 0; c < kCases; ++c) CaseConvoy(env, totals, c);
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
template<class M> inline uintptr_t MemberBody(M member) { return Body(std::bit_cast<uintptr_t>(member)); }
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

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "carconvoy75-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "carconvoy75-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("carconvoy75"); }
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
    const Pair pGetNew = MakeCollab(env, 0x421440, hooksFound), pVertical = MakeCollab(env, 0x5674E0, hooksFound), pFind = MakeCollab(env, 0x568B80, hooksFound), pSetUp = MakeCollab(env, 0x4217C0, hooksFound);
    const Pair pMat{0x59BBC0, MemberBody(static_cast<void (CMatrix::*)(const CMatrix&)>(&CMatrix::operator=)), false}, pAdd = MakeCollab(env, 0x563220, hooksFound), pBike = MakeCollab(env, 0x6BEEB0, hooksFound), pAuto = MakeCollab(env, 0x6AF420, hooksFound);
    const Pair pNew = MakeCollab(env, 0x61A5A0, hooksFound);
    Pair pWander = MakeCollab(env, 0x63CB10, hooksFound); pWander.dll = pWander.dll ? FirstDllCall(env.asi, pWander.dll) : 0;   // el cuerpo real del constructor: primera llamada de la envoltura `Constructor`
    const Pair pSetTask = [&] { Pair p = MakeCollab(env, 0x681AF0, hooksFound); p.dll = 0; p.hooked = false; return p; }();   // definido en linea en la cabecera: la DLL llama a la direccion del EXE; el hook (si existe) queda apagado
    const Pair pReg = MakeCollab(env, 0x571B70, hooksFound);
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    auto V = [](auto f) { return reinterpret_cast<void*>(f); };
    env.stubs[0] = {S(pRand, V(&FakeRand)), S(pGetNew, V(&FakeGetNew)), S(pVertical, V(&FakeVertical)), S(pFind, V(&FakeFind)), S(pSetUp, V(&FakeSetUp)), S(pMat, V(&FakeMatAssign)), S(pAdd, V(&FakeWorldAdd)),
        S(pBike, V(&FakePlaceBike)), S(pAuto, V(&FakePlaceAuto)), S(pNew, V(&FakeTaskNew)), S(pWander, V(&FakeWander)), S(pSetTask, V(&FakeSetTask)), S(pReg, V(&FakeRegister))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"ninguno: la aritmetica de X87Math y los accesos a campos corren de verdad"})}, {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry, 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
