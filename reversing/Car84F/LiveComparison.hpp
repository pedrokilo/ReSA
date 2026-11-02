#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <new>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "CarCtrl.h"
#include "CarAI.h"
#include "PathFind.h"
#include "World.h"
#include "Entity/Vehicle/Automobile.h"
#include "Core/Matrix.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Car84F dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CCarCtrl::GenerateOneEmergencyServicesCar (0x42B7D0, hook S, cdecl (modelo, posicion x, y, z), `ret` del llamador, devuelve el CAutomobile en EAX). Colaboradores sustituidos por falsos identicos
// en la direccion del EXE y en el cuerpo de la DLL (con su hook RH apagado durante el banco): FindPlayerCentreOfWorld 0x56E250 (cdecl, id), CCarCtrl::GenerateCarCreationCoors2 0x424210 (cdecl, 15 dwords),
// CWorld::FindObjectsKindaColliding 0x568B80 (cdecl, 11 dwords), CWorld::ProcessVerticalLine 0x5674E0 (cdecl, 11 dwords), CVehicle::operator new 0x6E2D50, el constructor de CAutomobile 0x6B0A90 (el cuerpo
// real se localiza siguiendo la primera llamada de la envoltura `Constructor` del hook), CEntity::GetDistanceFromCentreOfMassToBaseOfModel 0x536BE0, CAutomobile::PlaceOnRoadProperly 0x6AF420, CWorld::Add 0x563220
// y CCarAI::AddFiretruckOccupants 0x41C600 / AddAmbulanceOccupants 0x41C4A0 / AddPoliceCarOccupants 0x41C070. Registran sus argumentos EN ORDEN y devuelven lo que dice el caso. El vehiculo es la arena de un
// CAutomobile completo cuyo constructor falso solo deja el puntero a la tabla virtual (un destructor falso en la ranura 0) y el de la matriz (otra arena). Corren de verdad en ambos lados: la aritmetica propia
// de la funcion (ayudantes x87 de X87Math.cpp) y la lectura de la tabla de nodos de ThePaths (arenas del banco).

namespace C84FLayout {
static_assert(sizeof(CPathNode) == 0x1C && offsetof(CPathNode, m_vPos) == 8, "CPathNode: 0x1C bytes y posicion en +8");
static_assert(offsetof(CPathFind, m_pPathNodes) == 0x804, "CPathFind: tabla de nodos en +0x804");
static_assert(offsetof(CPlaceable, m_matrix) == 0x14 && offsetof(CPlaceable, m_placement) == 4, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(sizeof(CAutomobile) == C84FFix::kVehWin && sizeof(CMatrix) == C84FFix::kMatWin, "arenas del vehiculo y de la matriz");
inline bool LayoutOK() {
    alignas(16) static std::array<uint8, 0x988> mem{};
    bool ok = true;
    mem.fill(0);
    reinterpret_cast<CAutomobile*>(mem.data())->vehicleFlags.bIsAmbulanceOnDuty = true;
    ok &= mem[0x428] == 0x02 && mem[0x429] == 0 && mem[0x42A] == 0 && mem[0x42B] == 0;
    mem.fill(0);
    reinterpret_cast<CAutomobile*>(mem.data())->vehicleFlags.bIsFireTruckOnDuty = true;
    ok &= mem[0x428] == 0x04;
    mem.fill(0);
    reinterpret_cast<CAutomobile*>(mem.data())->vehicleFlags.bSirenOrAlarm = true;
    ok &= mem[0x42D] == 0x80 && mem[0x42C] == 0 && mem[0x42E] == 0;
    mem.fill(0);
    reinterpret_cast<CAutomobile*>(mem.data())->m_vecMoveSpeed = CVector{ 1.0f, 2.0f, 3.0f };
    ok &= *reinterpret_cast<float*>(mem.data() + 0x44) == 1.0f && *reinterpret_cast<float*>(mem.data() + 0x4C) == 3.0f;
    mem.fill(0);
    reinterpret_cast<CPathNode*>(mem.data())->m_bWaterNode = true;
    ok &= mem[0x18] == 0x80;
    return ok;
}
}

namespace C84FLive {
using namespace C84FFix;
inline constexpr uintptr_t kEntry = 0x42B7D0;
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint8& G8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = kGuard) const { return uintptr_t(m.data()) + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { const uint16 w = uint16(v); memcpy(m.data() + kGuard + off, &w, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    u32 Get8(size_t off) const { return m[kGuard + off]; }
    json Changes() const { return ArenaChanges(m.data(), r.data(), N, kGuard); }
};
inline Arena<kNodeArena> nodeA;
inline Arena<kVehArena> vehA;
inline Arena<kMatArena> matA;
inline Arena<kPlayerArena> playerA;

inline json CloseSide(Env& env, Side& side, json obs) {
    auto fp = NormalizeFp(side.fp);
    obs["globals_hash"] = RegionsHash();
    obs["statics"] = json::array({ G32(kAmbAddr), G32(kFireAddr) });
    obs["pif"] = G8(kPifAddr);
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

// ================================================================================================ GenerateOneEmergencyServicesCar (0x42B7D0)
struct State { const C* c{}; unsigned gen{}, fo{}, pvl{}; };
inline State g;
inline void* FakeVtable[1];
// Estado observable del vehiculo en un instante: matriz (+0, +4, +8, +0x10, +0x14, +0x18, +0x20, +0x24, +0x28, +0x30, +0x34, +0x38), velocidad (+0x44, +0x48, +0x4C), banderas +0x428 y +0x42D y los contadores
inline void VehSnap(Ev& e, unsigned at) {
    static constexpr unsigned moff[12]{0x00, 0x04, 0x08, 0x10, 0x14, 0x18, 0x20, 0x24, 0x28, 0x30, 0x34, 0x38};
    for (unsigned i = 0; i < 12; ++i) e.v[at + i] = matA.Get32(moff[i]);
    e.v[at + 12] = vehA.Get32(0x44); e.v[at + 13] = vehA.Get32(0x48); e.v[at + 14] = vehA.Get32(0x4C);
    e.v[at + 15] = vehA.Get8(0x428); e.v[at + 16] = vehA.Get8(0x42D);
    e.v[at + 17] = G32(kAmbAddr); e.v[at + 18] = G32(kFireAddr);
}
// Evento 1 = FindPlayerCentreOfWorld [id]. cdecl; devuelve el puntero al vector del jugador
inline u32* __cdecl FakeFindPlayer(u32 id) { Ev& e = PushEv(1, 1); e.v[0] = id; return reinterpret_cast<u32*>(playerA.Obj()); }
// Evento 2 = GenerateCarCreationCoors2 [llamada, x, y, z, radio, a3, a4, a5 & 0xFF, a6, a7, a12 & 0xFF, a13 & 0xFF, area del nodo 1, area del nodo 2]. cdecl, 15 dwords
inline bool __cdecl FakeGen(u32 px, u32 py, u32 pz, u32 rad, u32 a3, u32 a4, u32 f5, u32 a6, u32 a7, u32* org, uint16* n1, uint16* n2, u32* a11, u32 a12, u32 a13) {
    const unsigned k = g.gen < kCalls ? g.gen : kCalls - 1;
    Ev& e = PushEv(2, 14);
    e.v[0] = g.gen; e.v[1] = px; e.v[2] = py; e.v[3] = pz; e.v[4] = rad; e.v[5] = a3; e.v[6] = a4; e.v[7] = f5 & 0xFFu; e.v[8] = a6; e.v[9] = a7; e.v[10] = a12 & 0xFFu; e.v[11] = a13 & 0xFFu;
    e.v[12] = n1[0]; e.v[13] = n2[0];
    ++g.gen;
    const C& c = *g.c;
    org[0] = c.gorg[k][0]; org[1] = c.gorg[k][1]; org[2] = c.gorg[k][2];
    n1[0] = uint16(c.gn1[k]); n1[1] = uint16(c.gn1[k] >> 16);
    n2[0] = uint16(c.gn2[k]); n2[1] = uint16(c.gn2[k] >> 16);
    *a11 = c.gfrac[k];
    return (c.gret[k] & 1u) != 0;
}
// Evento 3 = CWorld::FindObjectsKindaColliding [llamada, x, y, z, radio, b2D & 0xFF, maximo, lista, edificios, vehiculos, peds, objetos, ficticios]. cdecl, 11 dwords; escribe el contador (int16)
inline void __cdecl FakeFindObjects(const u32* p, u32 radius, u32 b2d, int16* count, u32 maxc, u32 list, u32 bl, u32 ve, u32 pe, u32 ob, u32 du) {
    Ev& e = PushEv(3, 13);
    e.v[0] = g.fo; e.v[1] = p[0]; e.v[2] = p[1]; e.v[3] = p[2]; e.v[4] = radius; e.v[5] = b2d & 0xFFu; e.v[6] = maxc & 0xFFFFu; e.v[7] = list;
    e.v[8] = bl & 0xFFu; e.v[9] = ve & 0xFFu; e.v[10] = pe & 0xFFu; e.v[11] = ob & 0xFFu; e.v[12] = du & 0xFFu;
    const unsigned k = g.fo < kCalls ? g.fo : kCalls - 1;
    ++g.fo;
    *count = int16(g.c->cnt[k]);
}
// Evento 4 = CWorld::ProcessVerticalLine [llamada, x, y, z, distancia, edificios, vehiculos, peds, objetos, ficticios, atravesar, puntero de poligono]. cdecl, 11 dwords; si hay golpe escribe x, y, z del punto de colision
inline bool __cdecl FakePvl(const u32* p, u32 dist, u32* col, u32* ent, u32 bl, u32 ve, u32 pe, u32 ob, u32 du, u32 see, u32 poly) {
    (void)ent;
    const unsigned k = g.pvl < 2 ? g.pvl : 1;
    Ev& e = PushEv(4, 11);
    e.v[0] = g.pvl; e.v[1] = p[0]; e.v[2] = p[1]; e.v[3] = p[2]; e.v[4] = dist; e.v[5] = bl & 0xFFu; e.v[6] = ve & 0xFFu; e.v[7] = pe & 0xFFu; e.v[8] = ob & 0xFFu; e.v[9] = du & 0xFFu;
    e.v[10] = (see & 0xFFu) | (poly << 8);
    ++g.pvl;
    const C& c = *g.c;
    if (!(c.phit[k] & 1u)) return false;
    col[0] = c.pxy[k][0]; col[1] = c.pxy[k][1]; col[2] = c.pz[k];
    return true;
}
// Evento 5 = CVehicle::operator new [tamano]. Devuelve la arena del vehiculo
inline void* __cdecl FakeNew(size_t size) { Ev& e = PushEv(5, 1); e.v[0] = u32(size); return reinterpret_cast<void*>(vehA.Obj()); }
// Evento 6 = ctor de CAutomobile [this, modelo, creado por, preparar suspension & 0xFF]. thiscall (ret 0xC); deja la tabla virtual falsa y la matriz y toma la imagen de referencia
inline void* __fastcall FakeCtor(void* self, void*, u32 model, u32 created, u32 setup) {
    Ev& e = PushEv(6, 4); e.v[0] = Tag(self); e.v[1] = model; e.v[2] = created; e.v[3] = setup & 0xFFu;
    void* const vt = FakeVtable;
    memcpy(static_cast<uint8*>(self) + 0, &vt, 4);
    const u32 mat = u32(matA.Obj());
    memcpy(static_cast<uint8*>(self) + 0x14, &mat, 4);
    vehA.Snap(); matA.Snap();
    return self;
}
// Evento 7 = CEntity::GetDistanceFromCentreOfMassToBaseOfModel [this]. thiscall; devuelve el float del caso por ST0
inline float __fastcall FakeDist(void* self, void*) { Ev& e = PushEv(7, 1); e.v[0] = Tag(self); return F(g.c->dist); }
// Eventos 8 (PlaceOnRoadProperly [this + estado]) y 9 (CWorld::Add [entidad + estado])
inline void __fastcall FakePlace(void* self, void*) { Ev& e = PushEv(8, 20); e.v[0] = Tag(self); VehSnap(e, 1); }
inline void __cdecl FakeAdd(void* entity) { Ev& e = PushEv(9, 20); e.v[0] = Tag(entity); VehSnap(e, 1); }
// Eventos 10 (AddFiretruckOccupants), 11 (AddAmbulanceOccupants) y 12 (AddPoliceCarOccupants [vehiculo, bool, estado])
inline void __cdecl FakeFire(void* veh) { Ev& e = PushEv(10, 20); e.v[0] = Tag(veh); VehSnap(e, 1); }
inline void __cdecl FakeAmb(void* veh) { Ev& e = PushEv(11, 20); e.v[0] = Tag(veh); VehSnap(e, 1); }
inline void __cdecl FakePolice(void* veh, u32 always) { Ev& e = PushEv(12, 21); e.v[0] = Tag(veh); e.v[1] = always & 0xFFu; VehSnap(e, 2); }
// Evento 13 = destructor virtual con borrado (ranura 0 de la tabla virtual falsa) [this, indicador]. thiscall (ret 4)
inline void* __fastcall FakeDeleting(void* self, void*, u32 flags) { Ev& e = PushEv(13, 21); e.v[0] = Tag(self); e.v[1] = flags & 0xFFu; VehSnap(e, 2); return self; }

inline uintptr_t NodePtr(u32 s) { return nodeA.Obj() + size_t(s) * kNodesPer * kNodeBytes; }

inline void SetupWorld(const C& c) {
    nodeA.Fill(c.seed * 13u + 2u);
    for (u32 i = 0; i < 72; ++i) SetPtr(kRPathNodes, i, 0);
    for (u32 s = 0; s < kSlots; ++s) SetPtr(kRPathNodes, c.areas[s], NodePtr(s));
    for (u32 i = 0; i < kSlots * kNodesPer; ++i) {
        const size_t o = size_t(i) * kNodeBytes;
        nodeA.Put16(o + 0xC, c.nodeZ[i]);
        nodeA.Put8(o + 0x18, c.nodeFlag[i]);
    }
}

inline void CaseRun(Env& env, Totals& t, unsigned n) {
    const C c = MakeC(n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        vehA.Fill(c.seed * 13u + 8u); matA.Fill(c.seed * 13u + 9u); playerA.Fill(c.seed * 13u + 10u);
        SetupWorld(c);
        for (u32 k = 0; k < 3; ++k) playerA.Put32(4 * k, c.player[k]);
        nodeA.Snap(); vehA.Snap(); matA.Snap(); playerA.Snap();
        G32(kAmbAddr) = c.amb; G32(kFireAddr) = c.fire; G8(kPifAddr) = uint8(c.pif);
        TagClear(); TagAdd(vehA.m.data(), vehA.m.size());
        g = State{}; g.c = &c; nev = 0;
        FakeVtable[0] = reinterpret_cast<void*>(&FakeDeleting);
        const u32 args[4]{ c.model, c.posn[0], c.posn[1], c.posn[2] };
        sides[s] = Execute(env, 0, kEntry, candidate, [&](uintptr_t entry) { return Gate(entry, args, 4, c.eax, c.ecx, c.edx, c.stack, 16); });
        json obs;
        obs["ret"] = Tag(uintptr_t(sides[s].reg.eax));
        obs["veh_changes"] = vehA.Changes();
        obs["mat_changes"] = matA.Changes();
        obs["node_changes"] = nodeA.Changes();
        obs["player_changes"] = playerA.Changes();
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
    ready &= C84FLayout::LayoutOK();
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", true},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < kCases; ++c) CaseRun(env, totals, c);
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
    if (!std::filesystem::exists(dir / "car84f-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "car84f-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("car84f"); }
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
    const Pair pPlayer = MakeCollab(env, 0x56E250, hooksFound), pGen = MakeCollab(env, 0x424210, hooksFound), pFind = MakeCollab(env, 0x568B80, hooksFound), pPvl = MakeCollab(env, 0x5674E0, hooksFound);
    const Pair pNew = MakeCollab(env, 0x6E2D50, hooksFound);
    Pair pCtor = MakeCollab(env, 0x6B0A90, hooksFound); pCtor.dll = pCtor.dll ? FirstDllCall(env.asi, pCtor.dll) : 0;   // el cuerpo real del constructor: primera llamada de la envoltura `Constructor`
    const Pair pDist = MakeCollab(env, 0x536BE0, hooksFound), pPlace = MakeCollab(env, 0x6AF420, hooksFound), pAdd = MakeCollab(env, 0x563220, hooksFound);
    const Pair pFire = MakeCollab(env, 0x41C600, hooksFound), pAmb = MakeCollab(env, 0x41C4A0, hooksFound), pPolice = MakeCollab(env, 0x41C070, hooksFound);
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    env.stubs[0] = {S(pPlayer, reinterpret_cast<void*>(&FakeFindPlayer)), S(pGen, reinterpret_cast<void*>(&FakeGen)), S(pFind, reinterpret_cast<void*>(&FakeFindObjects)), S(pPvl, reinterpret_cast<void*>(&FakePvl)),
        S(pNew, reinterpret_cast<void*>(&FakeNew)), S(pCtor, reinterpret_cast<void*>(&FakeCtor)), S(pDist, reinterpret_cast<void*>(&FakeDist)), S(pPlace, reinterpret_cast<void*>(&FakePlace)),
        S(pAdd, reinterpret_cast<void*>(&FakeAdd)), S(pFire, reinterpret_cast<void*>(&FakeFire)), S(pAmb, reinterpret_cast<void*>(&FakeAmb)), S(pPolice, reinterpret_cast<void*>(&FakePolice))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"ayudantes x87 de X87Math.cpp (Veh83DMul, Veh83DAdd, Veh83DSub, Veh83DDiv, Veh83DSqrt, Veh83DStore)", "tabla de nodos de ThePaths: arenas del banco"})},
            {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry, 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
