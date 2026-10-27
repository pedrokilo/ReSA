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
#include "Entity/Vehicle/Heli.h"
#include "Entity/Ped/Ped.h"
#include "PedIntelligence.h"
#include "Tasks/TaskComplexSequence.h"
#include "Tasks/TaskTypes/TaskComplexUseSwatRope.h"
#include "Tasks/TaskTypes/TaskComplexWanderCop.h"
#include "Tasks/TaskManager.h"
#include "Streaming.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par HeliSwat74 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CHeli::SendDownSwat (0x6C69C0, hook S, thiscall sin argumentos). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (con hook RH, apagado durante el banco):
//   rand 0x821B1E (sin hook, la DLL lo llama por su direccion), CHeli::FindSwatPositionRelativeToHeli 0x6C4760 (la DLL llama a la funcion miembro), CWorld::FindGroundZFor3DCoord 0x5696C0, CWaterLevel::GetWaterLevelNoWaves 0x6E8580,
//   CRopes::RegisterRope 0x556B40, CPopulation::AddPed 0x612710, CTask::operator new 0x61A5A0, CTaskComplexSequence ctor 0x632BD0 y AddTask 0x632D10, CTaskComplexUseSwatRope ctor 0x659470,
//   CTaskComplexWanderCop ctor 0x460C80, CGeneral::GetRandomNumberInRange_int32 0x407180 (la DLL lo llama por su direccion), CTaskManager::SetTask 0x681AF0 (en linea en la cabecera: la DLL lo llama por su direccion)
//   y CAnimManager::BlendAnimation 0x4D4610. Los constructores de tareas se localizan siguiendo la primera llamada de la envoltura `Constructor` del hook; la reserva nula no se construye (como el EXE).
// Corren de verdad en el EXE y, en linea, en la DLL: CVector::Magnitude (0x4082C0), la copia/destruccion de CMatrix (0x59BCF0/0x59ACD0), la rotacion 0x59C790 y la suma 0x411A00 (el candidato las calcula con HeliSwat74WorldPosCpp).

namespace Hs74Layout {
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(offsetof(CHeli, m_nNumSwatOccupants) == 0x9B9 && offsetof(CHeli, m_aSwatState) == 0x9BA, "CHeli: ocupantes en +0x9B9 y estados en +0x9BA");
static_assert(offsetof(CPhysical, m_nPhysicalFlags) == 0x40 && offsetof(CPhysical, m_vecMoveSpeed) == 0x44, "CPhysical: banderas en +0x40 y velocidad en +0x44");
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_TargetEntity) == 0x41C, "CAutoPilot::m_TargetEntity en +0x41C");
static_assert(sizeof(CHeli) == Hs74Fix::kHeliWin, "la arena del helicoptero es un CHeli completo");
struct HsPed : CPed { static constexpr size_t OffIntel() { return offsetof(HsPed, m_pIntelligence); } };
static_assert(HsPed::OffIntel() == 0x47C, "CPed::m_pIntelligence en +0x47C");
static_assert(offsetof(CPedIntelligence, m_TaskMgr) == 4, "CPedIntelligence::m_TaskMgr en +4 (el EXE pasa intel + 4)");
static_assert(sizeof(CTaskComplexSequence) == 0x40 && sizeof(CTaskComplexUseSwatRope) == 0x1C && sizeof(CTaskComplexWanderCop) == 0x50, "tamanos de las tareas creadas");
static_assert(0x480 <= Hs74Fix::kPedWin, "la ventana del ped cubre los campos que usa la funcion (hasta +0x47C)");
// Posiciones de los campos de bits y de los accesores privados: se comprueban en ejecucion
inline bool LayoutOK() {
    alignas(16) static std::array<uint8, 0xA40> mem{};
    bool ok = true;
    mem.fill(0);
    reinterpret_cast<CHeli*>(mem.data())->physicalFlags.bSubmergedInWater = true;
    ok &= mem[0x41] == 0x01 && mem[0x40] == 0 && mem[0x42] == 0;                       // bit 8 de +0x40
    mem.fill(0);
    reinterpret_cast<CPed*>(mem.data())->m_bUsesCollision = true;
    ok &= mem[0x1C] == 0x01;                                                           // bit 0 de +0x1C
    mem.fill(0);
    const uint32 marker = 0x12345678u;
    memcpy(mem.data() + 0x18, &marker, 4);
    ok &= reinterpret_cast<CEntity*>(mem.data())->GetRwObject() == reinterpret_cast<RwObject*>(uintptr_t(marker));   // m_pRwObject en +0x18
    mem.fill(0);
    CVector probe{1.0f, 2.0f, 3.0f};
    memcpy(mem.data() + 4, &probe, 12);
    ok &= reinterpret_cast<CEntity*>(mem.data())->GetPosition().y == 2.0f;             // sin matriz: posicion compacta en +4
    mem.fill(0);
    const auto addr = uint32(uintptr_t(&CStreaming::ms_aInfoForModel.data()[285].m_LoadState));
    ok &= addr == 0x8E6314u;                                                           // modelo 285: byte de estado de carga
    return ok;
}
}

namespace Hs74Live {
using namespace Hs74Fix;
inline constexpr uintptr_t kEntry = 0x6C69C0;
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint8& G8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }

// ================================================================================================ arenas
template<size_t N, size_t G = kGuard> struct Arena {
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj() const { return uintptr_t(m.data()) + G; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + G + off, &v, 4); }
    void Put8(size_t off, u32 v) { m[G + off] = uint8(v); }
    json Changes() const { return ArenaChanges(m.data(), r.data(), N, G); }
    void Tagged() const { TagAdd(m.data(), N); }
};
inline Arena<2 * kHeliGuard + kHeliWin, kHeliGuard> heliA;
inline Arena<kArena(kTgtWin)> tgtA;
inline Arena<kArena(kMatWin)> matH, matT;
inline Arena<kArena(kPedWin)> pedA;
inline Arena<kArena(kIntelWin)> intelA;
inline Arena<kArena(kClumpWin)> clumpA;
inline Arena<kArena(kSeqWin)> seqA;
inline Arena<kArena(kRopeWin)> ropeA;
inline Arena<kArena(kWanderWin)> wanderA;

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

// ================================================================================================ SendDownSwat (0x6C69C0)
struct State { const C* c{}; };
inline State g;
// Eventos: 1 rand(); 2 FindSwatPositionRelativeToHeli(this, indice) (el puntero de salida es de pila); 3 FindGroundZFor3DCoord(x, y, z, p4, p5); 4 GetWaterLevelNoWaves(x, y, z, nivel*, p5, p6);
// 5 RegisterRope(id, tipo, x, y, z, expira, segmentos, indicadores, soporte, tiempo); 6 AddPed(tipo, modelo, posicion*, x, y, z, vagar); 7 CTask::operator new(tamano); 8 ctor de la secuencia(this);
// 9 ctor de UseSwatRope(this, id, helicoptero); 10 AddTask(this, tarea); 11 GetRandomNumberInRange(min, max); 12 ctor de WanderCop(this, estado, direccion); 13 SetTask(gestor, tarea, ranura, sin usar);
// 14 BlendAnimation(clump, grupo, anim, retraso). Los punteros dentro de una arena se etiquetan; los de pila se registran como «fuera de las arenas».
inline u32 StackMark(const void* p) { return Tag(p) == u32(uintptr_t(p)) ? 0xF00D0001u : Tag(p); }
inline u32 __cdecl FakeRand() { PushEv(1, 0); return g.c->rnd; }
inline void* __fastcall FakeSwat(void* self, void*, u32* out, i32 index) {
    Ev& e = PushEv(2, 2); e.v[0] = Tag(self); e.v[1] = u32(index);
    out[0] = g.c->rel[0]; out[1] = g.c->rel[1]; out[2] = g.c->rel[2];
    return out;
}
inline float __cdecl FakeGround(u32 x, u32 y, u32 z, u32 a, u32 b) {
    Ev& e = PushEv(3, 5); e.v[0] = x; e.v[1] = y; e.v[2] = z; e.v[3] = a; e.v[4] = b;
    return F(g.c->ground);
}
inline u32 __cdecl FakeWater(u32 x, u32 y, u32 z, u32* level, u32 a, u32 b) {
    Ev& e = PushEv(4, 6); e.v[0] = x; e.v[1] = y; e.v[2] = z; e.v[3] = StackMark(level); e.v[4] = a; e.v[5] = b;
    if (!g.c->water) return 0u;
    *level = g.c->waterLvl;
    return 1u;
}
inline u32 __cdecl FakeRope(u32 id, u32 type, u32 x, u32 y, u32 z, u32 expires, u32 segments, u32 flags, u32 holder, u32 time) {
    Ev& e = PushEv(5, 10); e.v[0] = Tag(uintptr_t(id)); e.v[1] = type; e.v[2] = x; e.v[3] = y; e.v[4] = z; e.v[5] = expires; e.v[6] = segments; e.v[7] = flags; e.v[8] = holder; e.v[9] = time;
    return g.c->ropeRet & 1u;
}
inline void* __cdecl FakeAddPed(u32 type, u32 model, const u32* pos, u32 wander) {
    Ev& e = PushEv(6, 7); e.v[0] = type; e.v[1] = model; e.v[2] = StackMark(pos); e.v[3] = pos[0]; e.v[4] = pos[1]; e.v[5] = pos[2]; e.v[6] = wander;
    return reinterpret_cast<void*>(pedA.Obj());
}
inline void* __cdecl FakeNew(size_t size) {
    Ev& e = PushEv(7, 1); e.v[0] = u32(size);
    if (size == 0x40) return g.c->nullSeq ? nullptr : reinterpret_cast<void*>(seqA.Obj());
    if (size == 0x1C) return g.c->nullRope ? nullptr : reinterpret_cast<void*>(ropeA.Obj());
    if (size == 0x50) return g.c->nullWander ? nullptr : reinterpret_cast<void*>(wanderA.Obj());
    return nullptr;
}
inline void* __fastcall FakeSeqCtor(void* self, void*) { Ev& e = PushEv(8, 1); e.v[0] = Tag(self); *reinterpret_cast<u32*>(self) = 0xC0DE0008u; return self; }
inline void* __fastcall FakeRopeCtor(void* self, void*, u32 id, u32 heli) {
    Ev& e = PushEv(9, 3); e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(id)); e.v[2] = Tag(uintptr_t(heli));
    *reinterpret_cast<u32*>(self) = 0xC0DE0009u; return self;
}
inline u32 __fastcall FakeAddTask(void* self, void*, u32 task) { Ev& e = PushEv(10, 2); e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(task)); return 1u; }
inline u32 __cdecl FakeRange(u32 lo, u32 hi) { Ev& e = PushEv(11, 2); e.v[0] = lo; e.v[1] = hi; return g.c->range; }
inline void* __fastcall FakeWanderCtor(void* self, void*, u32 state, u32 dir) {
    Ev& e = PushEv(12, 3); e.v[0] = Tag(self); e.v[1] = state; e.v[2] = dir;
    *reinterpret_cast<u32*>(self) = 0xC0DE000Cu; return self;
}
inline void __fastcall FakeSetTask(void* self, void*, u32 task, u32 slot, u32 unused) {
    Ev& e = PushEv(13, 4); e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(task)); e.v[2] = slot; e.v[3] = unused;
}
inline void __cdecl FakeBlend(u32 clump, u32 group, u32 anim, u32 delta) { Ev& e = PushEv(14, 4); e.v[0] = Tag(uintptr_t(clump)); e.v[1] = group; e.v[2] = anim; e.v[3] = delta; }

inline void CaseSwat(Env& env, Totals& t, unsigned n) {
    const C c = MakeC(n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        heliA.Fill(c.seed * 13u + 1u); tgtA.Fill(c.seed * 13u + 2u); matH.Fill(c.seed * 13u + 3u); matT.Fill(c.seed * 13u + 4u); pedA.Fill(c.seed * 13u + 5u); intelA.Fill(c.seed * 13u + 6u);
        clumpA.Fill(c.seed * 13u + 7u); seqA.Fill(c.seed * 13u + 8u); ropeA.Fill(c.seed * 13u + 9u); wanderA.Fill(c.seed * 13u + 10u);
        // helicoptero: matriz (+0x14), banderas (+0x40), velocidad (+0x44), objetivo (+0x41C), ocupantes (+0x9B9)
        heliA.Put32(0x14, u32(matH.Obj())); heliA.Put32(0x40, c.phys); heliA.Put8(0x9B9, c.numSwat); heliA.Put32(0x41C, u32(tgtA.Obj()));
        for (int i = 0; i < 3; ++i) { heliA.Put32(0x44 + 4 * i, c.spd[i]); matH.Put32(0x30 + 4 * i, c.hel[i]); }
        static constexpr size_t rowOff[3]{0x00, 0x10, 0x20};
        for (int r = 0; r < 3; ++r) for (int i = 0; i < 3; ++i) matH.Put32(rowOff[r] + 4 * i, c.mat[3 * r + i]);
        // objetivo: con matriz la posicion esta en +0x30 de la matriz; sin ella, en +4 de la entidad
        tgtA.Put32(0x14, c.tgtMat ? u32(matT.Obj()) : 0u);
        for (int i = 0; i < 3; ++i) { if (c.tgtMat) matT.Put32(0x30 + 4 * i, c.tgt[i]); else tgtA.Put32(4 + 4 * i, c.tgt[i]); }
        // ped devuelto por AddPed: clump (+0x18), banderas de la entidad (+0x1C), inteligencia (+0x47C)
        pedA.Put32(0x18, u32(clumpA.Obj())); pedA.Put32(0x1C, c.pedFlags); pedA.Put32(0x47C, u32(intelA.Obj()));
        heliA.Snap(); tgtA.Snap(); matH.Snap(); matT.Snap(); pedA.Snap(); intelA.Snap(); clumpA.Snap(); seqA.Snap(); ropeA.Snap(); wanderA.Snap();
        TagClear(); heliA.Tagged(); tgtA.Tagged(); matH.Tagged(); matT.Tagged(); pedA.Tagged(); intelA.Tagged(); clumpA.Tagged(); seqA.Tagged(); ropeA.Tagged(); wanderA.Tagged();
        G8(kStreamAddr) = uint8(c.stream);
        g.c = &c; nev = 0;
        const u32 args[1]{0u};
        sides[s] = Execute(env, 0, kEntry, candidate, [&](uintptr_t entry) { return Gate(entry, args, 0, c.eax, u32(heliA.Obj()), c.edx, c.stack, 0); });
        json obs;
        obs["ret_al"] = sides[s].reg.eax & 0xFFu;
        obs["changes"] = json::array({heliA.Changes(), tgtA.Changes(), matH.Changes(), matT.Changes(), pedA.Changes(), intelA.Changes(), clumpA.Changes(), seqA.Changes(), ropeA.Changes(), wanderA.Changes()});
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
    const bool layout = Hs74Layout::LayoutOK();
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
            for (unsigned c = 0; c < kCases; ++c) CaseSwat(env, totals, c);
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
    if (!std::filesystem::exists(dir / "heliswat74-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "heliswat74-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("heliswat74"); }
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
    auto Ctor = [&](uintptr_t addr) { Pair p = MakeCollab(env, addr, hooksFound); p.dll = p.dll ? FirstDllCall(env.asi, p.dll) : 0; return p; };
    auto ExeOnly = [&](uintptr_t addr) { Pair p = MakeCollab(env, addr, hooksFound); p.dll = 0; p.hooked = false; return p; };   // la DLL llama por la direccion del EXE: el hook (si existe) queda apagado
    Pair pRand{0x821B1E, 0, false};
    Pair pSwat = MakeCollab(env, 0x6C4760, hooksFound); pSwat.dll = MemberBody(&CHeli::FindSwatPositionRelativeToHeli);   // la DLL llama a la funcion miembro, no a la envoltura del hook
    const Pair pGround = MakeCollab(env, 0x5696C0, hooksFound), pWater = MakeCollab(env, 0x6E8580, hooksFound), pRope = MakeCollab(env, 0x556B40, hooksFound), pPed = MakeCollab(env, 0x612710, hooksFound);
    const Pair pNew = MakeCollab(env, 0x61A5A0, hooksFound), pSeq = Ctor(0x632BD0), pAdd = MakeCollab(env, 0x632D10, hooksFound), pRopeTask = Ctor(0x659470), pWander = Ctor(0x460C80);
    const Pair pRange = ExeOnly(0x407180), pSet = ExeOnly(0x681AF0), pBlend = MakeCollab(env, 0x4D4610, hooksFound);
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    env.stubs[0] = {S(pRand, reinterpret_cast<void*>(&FakeRand)), S(pSwat, reinterpret_cast<void*>(&FakeSwat)), S(pGround, reinterpret_cast<void*>(&FakeGround)), S(pWater, reinterpret_cast<void*>(&FakeWater)),
        S(pRope, reinterpret_cast<void*>(&FakeRope)), S(pPed, reinterpret_cast<void*>(&FakeAddPed)), S(pNew, reinterpret_cast<void*>(&FakeNew)), S(pSeq, reinterpret_cast<void*>(&FakeSeqCtor)),
        S(pAdd, reinterpret_cast<void*>(&FakeAddTask)), S(pRopeTask, reinterpret_cast<void*>(&FakeRopeCtor)), S(pWander, reinterpret_cast<void*>(&FakeWanderCtor)), S(pRange, reinterpret_cast<void*>(&FakeRange)),
        S(pSet, reinterpret_cast<void*>(&FakeSetTask)), S(pBlend, reinterpret_cast<void*>(&FakeBlend))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"CVector::Magnitude 0x4082C0 (real en el EXE; en linea en la DLL)", "CMatrix copia/destructor 0x59BCF0/0x59ACD0, rotacion 0x59C790 y suma 0x411A00 (reales en el EXE; HeliSwat74WorldPosCpp en la DLL)"})},
            {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry, 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
