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
#include "Entity/Ped/Ped.h"
#include "PedIntelligence.h"
#include "Tasks/TaskTypes/TaskSimpleFight.h"
#include "CutsceneMgr.h"
#include "X87Math.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn85H dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion (hook S): CTaskSimpleFight::TargetInReach 0x61D6F0 (thiscall, ret 4; devuelve AL). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL
// (con su hook RH apagado durante el banco): CLocalisation::KickingWhenDown 0x56D270 (cdecl), CPed::GetBonePosition 0x5E4280 (thiscall, ret 0xC), CPed::IsAlive 0x5E0170, CPed::IsPlayer 0x5DF8F0,
// CTaskManager::GetSimplestActiveTask 0x6819D0 y CPlaceable::GetMatrix 0x411990 (todos thiscall sin argumentos salvo el hueso). El GetTaskType virtual de la tarea falsa lo da una tabla de funciones virtuales propia.
// Registran sus argumentos EN ORDEN. La aritmetica (restas, productos escalares con 0x40FE60 y 0x40FDB0, suma de la fila de la tabla, fpatan, vuelta de 2pi y comparaciones) corre de verdad en ambos lados;
// las filas de la tabla de combos y 0xC177EC son la memoria real del juego (se rellenan los floats +0x3C de los combos 0..16 y se restauran tras cada lado).
// Mutaciones (una sola vez): 1..5 dentro de la llamada (mutA & 15) a 0x5E4280 (posicion o matriz del ped; posicion de la entidad consultada; rumbo del ped; estado de la entidad; salud de otro ped);
// 6 cambia el objetivo de la tarea al segundo objetivo en la llamada que elige mutD (hueso, vivo, tarea mas simple, primera o segunda matriz); 7 cambia las banderas del objetivo en 0x5E0170;
// 8 conmuta la matriz del objetivo en la primera llamada a 0x411990. En todos los casos el EXE relee despues esa memoria.

namespace F85HLayout {
static_assert(offsetof(CTaskSimpleFight, m_pTargetEntity) == 0x18 && offsetof(CTaskSimpleFight, m_nComboSet) == 0x24, "CTaskSimpleFight: objetivo +0x18, combo +0x24");
static_assert(offsetof(CPed, m_fHealth) == 0x540 && offsetof(CPed, m_fCurrentRotation) == 0x558 && offsetof(CPed, m_fAimingRotation) == 0x55C, "CPed: salud +0x540, rumbo +0x558, rumbo de apuntado +0x55C");
}

namespace F85HLive {
using namespace F85HFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x61D6F0};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint8& G8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia, sin los bytes [maskLo, maskHi) (campos de puntero que cambian por direccion)
    json Changes(size_t maskLo = 0, size_t maskHi = 0) const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            const int64 off = int64(i) - int64(kGuard);
            if (m[i] != r[i] && !(off >= int64(maskLo) && off < int64(maskHi) && maskHi != 0)) out.push_back(json::array({off, m[i]}));
        }
        return out;
    }
};
inline Arena<kTaskArena> taskA;
inline Arena<kPedArena> pedA;
inline Arena<kTgtArena> tgtA, tgt2A;
inline Arena<kMatArena> smatA, tmatA, t2matA, retM1A, retM2A;
inline Arena<kIntArena> intA, tintA, t2intA;
inline Arena<kFTaskArena> ftaskA;
inline Arena<kAssocArena> assocA;
inline Arena<kHierArena> hierA;
inline Arena<kObjArena> objA;
inline std::array<Arena<kOthArena>, kOthers> othA;
inline std::array<Arena<kMatArena>, kOthers> omatA;

// Estado del caso en curso para los falsos
struct State { const C* c{}; bool mutDone[9]{}; unsigned boneCalls{}, matCalls{}; };
inline State g;
inline bool Once(u32 site) { if (g.c->mutSite != site || g.mutDone[site]) return false; g.mutDone[site] = true; return true; }
inline Arena<kTgtArena>& Tgt(unsigned i) { return i ? tgt2A : tgtA; }
inline Arena<kMatArena>& TMat(unsigned i) { return i ? t2matA : tmatA; }
inline unsigned CurTarget() { return taskA.Get32(0x18) == u32(tgt2A.Obj()) ? 1u : 0u; }
// Posicion de una entidad con matriz (+0x14 distinto de 0: matriz +0x30) o compacta (+4)
template<class E, class M> inline void PutPos(E& ent, M& mat, const u32* p) {
    if (ent.Get32(0x14)) { mat.Put32(0x30, p[0]); mat.Put32(0x34, p[1]); mat.Put32(0x38, p[2]); }
    else { ent.Put32(4, p[0]); ent.Put32(8, p[1]); ent.Put32(0xC, p[2]); }
}
template<class E, class M> inline void ToggleMat(E& ent, M& mat) { ent.Put32(0x14, ent.Get32(0x14) ? 0u : u32(mat.Obj())); }
inline int OtherIndex(const void* self) { for (unsigned i = 0; i < kOthers; ++i) if (othA[i].Obj() == uintptr_t(self)) return int(i); return -1; }
inline void PutOtherPos(unsigned k, const u32* p) { omatA[k].Put32(0x30, p[0]); omatA[k].Put32(0x34, p[1]); omatA[k].Put32(0x38, p[2]); }
inline void SwitchTarget() { taskA.Put32(0x18, u32(tgt2A.Obj())); }

// Eventos: 1 KickingWhenDown []; 2 GetBonePosition [entidad, hueso, bandera]; 3 IsAlive [entidad]; 4 IsPlayer [ped]; 5 GetSimplestActiveTask [gestor de tareas]; 6 GetMatrix [entidad]; 7 GetTaskType [tarea].
inline u32 __cdecl FkKick() {                                                                       // 0x56D270
    PushEv(1, 0);
    return g.c->kick;
}
inline void __fastcall FkBone(void* self, void*, float* out, int bone, int flag) {                  // 0x5E4280 (ret 0xC)
    Ev& e = PushEv(2, 3); e.v[0] = Tag(self); e.v[1] = u32(bone) & 0xFFFFu; e.v[2] = u32(flag) & 0xFFu;
    const int k = OtherIndex(self);
    const u32* b = k >= 0 ? &g.c->obone[k * 3] : (self == reinterpret_cast<void*>(tgt2A.Obj()) ? &g.c->ebone[3] : &g.c->ebone[0]);
    memcpy(out, b, 12);
    if (g.boneCalls++ == (g.c->mutA & 15u)) {
        if (Once(1)) { if (g.c->mutB & 1u) ToggleMat(pedA, smatA); else PutPos(pedA, smatA, g.c->mutPos); }
        if (Once(2)) {
            if (k >= 0) PutOtherPos(unsigned(k), g.c->mutPos);
            else if (self == reinterpret_cast<void*>(tgt2A.Obj())) PutPos(tgt2A, t2matA, g.c->mutPos);
            else PutPos(tgtA, tmatA, g.c->mutPos);
        }
        if (Once(3)) pedA.Put32(0x558, g.c->mutB);
        if (k >= 0 && Once(4)) othA[unsigned(k)].Put32(0x530, g.c->mutB);
        if (Once(5)) othA[g.c->mutC & 15u].Put32(0x540, g.c->mutB);
    }
    if (g.c->mutSite == 6 && g.c->mutD % 5u == 0 && g.boneCalls == 1 && !g.mutDone[6]) { Once(6); SwitchTarget(); }
}
inline u32 __fastcall FkAlive(void* self, void*) {                                                  // 0x5E0170 (AL)
    Ev& e = PushEv(3, 1); e.v[0] = Tag(self);
    if (g.c->mutSite == 6 && g.c->mutD % 5u == 1 && Once(6)) SwitchTarget();
    if (Once(7)) Tgt(CurTarget()).Put32(0x46C, g.c->mutB);
    return g.c->alive;
}
inline u32 __fastcall FkPlayer(void* self, void*) {                                                 // 0x5DF8F0 (AL)
    Ev& e = PushEv(4, 1); e.v[0] = Tag(self);
    return g.c->isplayer;
}
inline u32 __fastcall FkSimplest(void* self, void*) {                                               // 0x6819D0
    Ev& e = PushEv(5, 1); e.v[0] = Tag(self);
    if (g.c->mutSite == 6 && g.c->mutD % 5u == 2 && Once(6)) SwitchTarget();
    return g.c->smode ? u32(ftaskA.Obj()) : 0u;
}
inline u32 __fastcall FkTaskType(void* self, void*) {                                               // GetTaskType virtual (ranura 4)
    Ev& e = PushEv(7, 1); e.v[0] = Tag(self);
    return g.c->stype;
}
inline void __fastcall FkDummy(void*, void*) {}
inline void* gVtable[8]{reinterpret_cast<void*>(&FkDummy), reinterpret_cast<void*>(&FkDummy), reinterpret_cast<void*>(&FkDummy), reinterpret_cast<void*>(&FkDummy), reinterpret_cast<void*>(&FkTaskType),
                        reinterpret_cast<void*>(&FkDummy), reinterpret_cast<void*>(&FkDummy), reinterpret_cast<void*>(&FkDummy)};
inline u32 __fastcall FkMatrix(void* self, void*) {                                                 // 0x411990
    Ev& e = PushEv(6, 1); e.v[0] = Tag(self);
    const unsigned call = g.matCalls++;
    if (g.c->mutSite == 6 && g.c->mutD % 5u == 3u + call && call < 2 && Once(6)) SwitchTarget();
    if (call == 0 && Once(8)) ToggleMat(Tgt(CurTarget()), TMat(CurTarget()));
    return u32(call == 0 ? retM1A.Obj() : retM2A.Obj());
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

inline void SetupArenas(const C& c) {
    taskA.Fill(c.seed * 13u + 50u); pedA.Fill(c.seed * 13u + 57u); tgtA.Fill(c.seed * 13u + 60u); tgt2A.Fill(c.seed * 13u + 61u);
    smatA.Fill(c.seed * 13u + 62u); tmatA.Fill(c.seed * 13u + 63u); t2matA.Fill(c.seed * 13u + 64u); retM1A.Fill(c.seed * 13u + 65u); retM2A.Fill(c.seed * 13u + 66u);
    intA.Fill(c.seed * 13u + 67u); tintA.Fill(c.seed * 13u + 68u); t2intA.Fill(c.seed * 13u + 69u);
    ftaskA.Fill(c.seed * 13u + 70u); assocA.Fill(c.seed * 13u + 71u); hierA.Fill(c.seed * 13u + 72u); objA.Fill(c.seed * 13u + 73u);
    for (unsigned i = 0; i < kOthers; ++i) { othA[i].Fill(c.seed * 13u + 100u + i); omatA[i].Fill(c.seed * 13u + 140u + i); }
}
inline void SnapArenas() {
    taskA.Snap(); pedA.Snap(); tgtA.Snap(); tgt2A.Snap(); smatA.Snap(); tmatA.Snap(); t2matA.Snap(); retM1A.Snap(); retM2A.Snap();
    intA.Snap(); tintA.Snap(); t2intA.Snap(); ftaskA.Snap(); assocA.Snap(); hierA.Snap(); objA.Snap();
    for (unsigned i = 0; i < kOthers; ++i) { othA[i].Snap(); omatA[i].Snap(); }
}
inline void Put3(auto& a, size_t off, const u32* p) { a.Put32(off, p[0]); a.Put32(off + 4, p[1]); a.Put32(off + 8, p[2]); }
// Posicion en la ubicacion activa (matriz +0x30 con matriz, compacta +4 sin ella) y en la otra
template<class E, class M> inline void PutBoth(E& ent, M& mat, bool matOn, const u32* pos, const u32* alt) {
    if (matOn) { Put3(mat, 0x30, pos); Put3(ent, 4, alt); } else { Put3(ent, 4, pos); Put3(mat, 0x30, alt); }
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        SetupArenas(c);
        TagClear();
        TagAdd(taskA.m.data(), taskA.m.size());     // 0
        TagAdd(pedA.m.data(), pedA.m.size());       // 1
        TagAdd(tgtA.m.data(), tgtA.m.size());       // 2
        TagAdd(tgt2A.m.data(), tgt2A.m.size());     // 3
        TagAdd(smatA.m.data(), smatA.m.size());     // 4
        TagAdd(tmatA.m.data(), tmatA.m.size());     // 5
        TagAdd(t2matA.m.data(), t2matA.m.size());   // 6
        TagAdd(intA.m.data(), intA.m.size());       // 7
        TagAdd(tintA.m.data(), tintA.m.size());     // 8
        TagAdd(t2intA.m.data(), t2intA.m.size());   // 9
        TagAdd(ftaskA.m.data(), ftaskA.m.size());   // 10
        TagAdd(assocA.m.data(), assocA.m.size());   // 11
        TagAdd(hierA.m.data(), hierA.m.size());     // 12
        TagAdd(retM1A.m.data(), retM1A.m.size());   // 13
        TagAdd(retM2A.m.data(), retM2A.m.size());   // 14
        TagAdd(objA.m.data(), objA.m.size());       // 15
        for (unsigned i = 0; i < kOthers; ++i) TagAdd(othA[i].m.data(), othA[i].m.size());     // 16..31
        for (unsigned i = 0; i < kOthers; ++i) TagAdd(omatA[i].m.data(), omatA[i].m.size());   // 32..47
        taskA.Put8(0x24, c.combo); taskA.Put32(0x18, c.tmode ? u32(tgtA.Obj()) : 0u);
        // objetivos 0 y 1
        for (unsigned e = 0; e < 2; ++e) {
            auto& ent = Tgt(e); auto& mat = TMat(e);
            ent.Put8(0x36, c.etype[e]); ent.Put32(0x46C, c.eflags[e]); ent.Put32(0x590, c.e590[e]);
            ent.Put32(0x47C, u32((e ? t2intA : tintA).Obj()));
            ent.Put32(0x14, c.emat[e] ? u32(mat.Obj()) : 0u);
            PutBoth(ent, mat, c.emat[e] != 0, &c.epos[e * 3], &c.ealt[e * 3]);
        }
        objA.Put8(0x36, c.etype[2]); objA.Put32(0x590, c.e590[2]);
        // ped
        pedA.Put32(0x14, c.pmat ? u32(smatA.Obj()) : 0u); pedA.Put32(0x47C, u32(intA.Obj())); pedA.Put32(0x558, c.rot); pedA.Put32(0x55C, c.aim);
        pedA.Put32(0x568, c.ptarg == 0 ? 0u : (c.ptarg == 1 ? u32(tgtA.Obj()) : u32(objA.Obj())));
        PutBoth(pedA, smatA, c.pmat != 0, c.ppos, c.palt);
        for (unsigned i = 0; i < kOthers; ++i) {
            intA.Put32(0x130 + 4 * i, c.opres[i] ? u32(othA[i].Obj()) : 0u);
            othA[i].Put32(0x14, u32(omatA[i].Obj())); othA[i].Put32(0x530, c.ostate[i]); othA[i].Put32(0x540, c.ohealth[i]);
            Put3(omatA[i], 0x30, &c.opos[i * 3]); Put3(omatA[i], 0, &c.orf[i * 6]); Put3(omatA[i], 0x10, &c.orf[i * 6 + 3]);
        }
        // tarea falsa, animacion y matrices devueltas
        ftaskA.Put32(0, u32(reinterpret_cast<uintptr_t>(&gVtable[0]))); ftaskA.Put32(0x14, c.smode == 3 ? u32(assocA.Obj()) : 0u);
        assocA.Put32(0x14, u32(hierA.Obj())); assocA.Put32(0x20, c.acur); hierA.Put32(0x10, c.atotal);
        Put3(retM1A, 0, c.m1r); Put3(retM1A, 0x10, c.m1f); Put3(retM2A, 0, c.m2r); Put3(retM2A, 0x10, c.m2f);
        for (u32 j = 0; j < kCombos; ++j) G32(0xC170D0 + (static_cast<int32>(j) - 4) * 0x88 + 0x3C) = c.rowf[j];
        G32(0xC177EC) = c.limit;
        const std::vector<u32> args = { u32(pedA.Obj()) };
        SnapArenas();
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, u32(taskA.Obj()), c.edx, c.stack, 0); });
        json obs;
        obs["ret"] = int(sides[s].reg.eax & 0xFFu);
        obs["target"] = Tag(uintptr_t(taskA.Get32(0x18)));
        obs["mats"] = json::array({pedA.Get32(0x14) != 0, tgtA.Get32(0x14) != 0, tgt2A.Get32(0x14) != 0});
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, taskA.Changes(0x18, 0x1C)); add(1, pedA.Changes(0x14, 0x18)); add(2, tgtA.Changes(0x14, 0x18)); add(3, tgt2A.Changes(0x14, 0x18));
        add(4, smatA.Changes()); add(5, tmatA.Changes()); add(6, t2matA.Changes()); add(15, objA.Changes());
        for (unsigned i = 0; i < kOthers; ++i) add(16 + i, othA[i].Changes());
        for (unsigned i = 0; i < kOthers; ++i) add(32 + i, omatA[i].Changes());
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
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", true},
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
    if (!std::filesystem::exists(dir / "fn85h-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn85h-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn85h"); }
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
    const Pair pKick = MakeCollab(env, 0x56D270, hooksFound), pBone = MakeCollab(env, 0x5E4280, hooksFound), pAlive = MakeCollab(env, 0x5E0170, hooksFound),
               pPlayer = MakeCollab(env, 0x5DF8F0, hooksFound), pSimplest = MakeCollab(env, 0x6819D0, hooksFound), pMatrix = MakeCollab(env, 0x411990, hooksFound);
    env.stubs[0] = {S(pKick, reinterpret_cast<void*>(&FkKick)), S(pBone, reinterpret_cast<void*>(&FkBone)), S(pAlive, reinterpret_cast<void*>(&FkAlive)),
                    S(pPlayer, reinterpret_cast<void*>(&FkPlayer)), S(pSimplest, reinterpret_cast<void*>(&FkSimplest)), S(pMatrix, reinterpret_cast<void*>(&FkMatrix))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"aritmetica de alcance, productos escalares, vuelta de 2pi y comparaciones (con los ayudantes x87 de 24 bits); 0x40FE60 y 0x40FDB0 del EXE frente a los ayudantes de la DLL"})},
                                 {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
