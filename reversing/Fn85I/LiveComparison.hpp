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
#include "Weapon.h"
#include "WeaponInfo.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "AnimBlendAssociation.h"
#include "PlayerPedData.h"
#include "AnimManager.h"
#include "AnimBlock.h"
#include "AnimBlendAssocGroup.h"
#include "Streaming.h"
#include "Stats.h"
#include "PedIntelligence.h"
#include "Tasks/TaskTypes/TaskSimpleFight.h"
#include "CutsceneMgr.h"
#include "X87Math.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn85I dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion (hook S): CTaskSimpleFight::StartAnim 0x623B10 (thiscall, ret 8; no devuelve nada que lean los llamadores). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo
// de la DLL (con su hook RH apagado durante el banco): CAnimBlendAssociation::SetDeleteCallback 0x4CEBC0 / SetFinishCallback 0x4CEBE0 / SetCurrentTime 0x4CEA80, CAnimManager::BlendAnimation 0x4D4610,
// CPed::IsPlayer 0x5DF8F0, CPed::SetMoveState 0x5DEC00, CStats::GetFatAndMuscleModifier 0x559AF0 (el hook del EXE es un envoltorio: el falso va tambien en el cuerpo del metodo de la DLL),
// CTaskSimpleFight::FindTargetOnGround 0x61C9B0, CPedIntelligence::GetTaskDuck 0x6010A0 y, para el combo disponible que el EXE lleva en linea (0x61C7F0 en la DLL), CWeaponInfo::GetWeaponInfo 0x743C60,
// CAnimManager::GetAnimBlockName 0x4D3A30, GetAnimationBlock(const char*) 0x4D3940, AddAnimBlockRef 0x4D3FB0, CStreaming::RequestModel 0x4087E0 y, sin hook, RemoveAnimBlockRef 0x4D3FD0.
// Registran sus argumentos EN ORDEN. Los bloques de animacion son los reales de ms_aAnimBlocks (0..15: solo se lee y se escribe su byte IsLoaded), el arreglo de grupos 0xB4EA34 apunta a una arena,
// las 12 filas de la tabla de combos 0xC170D0 se fijan por caso (grupo +0, campo +0x44 y banderas altas +0x85) y todo se restaura. BlendAnimation devuelve una asociacion de arena; el falso de
// SetMoveState escribe el estado en +0x534 del ped.
// Mutaciones (una sola vez por caso): 1 BlendAnimation cambia el comando siguiente (+0x26); 2 SetDeleteCallback/SetFinishCallback con la callback del combate cambian la animacion de la tarea (+0x1C) a la
// alternativa; 3 idem cambian el combo (+0x24); 4 FindTargetOnGround cambia los datos de jugador del ped (+0x480) a los alternativos; 5 la primera IsPlayer idem; 6 GetFatAndMuscleModifier cambia la animacion
// de la tarea; 7 la primera IsPlayer cambia el comando siguiente. En todos los casos el EXE relee esa memoria despues.

namespace F85ILayout {
struct FightPed : CPed {
    static constexpr size_t OffSlot() { return offsetof(FightPed, m_nActiveWeaponSlot); }
    static constexpr size_t OffStyle() { return offsetof(FightPed, m_nFightingStyle); }
    static constexpr size_t OffWeapons() { return offsetof(FightPed, m_aWeapons); }
    static constexpr size_t OffIntel() { return offsetof(FightPed, m_pIntelligence); }
    static constexpr size_t OffPd() { return offsetof(FightPed, m_pPlayerData); }
};
static_assert(FightPed::OffSlot() == 0x718 && FightPed::OffStyle() == 0x72D && FightPed::OffWeapons() == 0x5A0 && sizeof(CWeapon) == 0x1C && offsetof(CWeapon, m_Type) == 0, "CPed: armas +0x5A0 (0x1C), ranura +0x718, estilo +0x72D");
static_assert(offsetof(CPed, m_nAnimGroup) == 0x4D4 && FightPed::OffIntel() == 0x47C && FightPed::OffPd() == 0x480 && offsetof(CPed, m_nMoveState) == 0x534 && offsetof(CPed, m_nSwimmingMoveState) == 0x538, "CPed: +0x4D4, +0x47C, +0x480, +0x534, +0x538");
static_assert(offsetof(CTaskSimpleFight, m_bIsFinished) == 0x8 && offsetof(CTaskSimpleFight, m_bAnimsReferenced) == 0xA && offsetof(CTaskSimpleFight, m_nRequiredAnimGroup) == 0xC && offsetof(CTaskSimpleFight, m_nContinueStrike) == 0x14
              && offsetof(CTaskSimpleFight, m_pAnim) == 0x1C && offsetof(CTaskSimpleFight, m_pIdleAnim) == 0x20 && offsetof(CTaskSimpleFight, m_nComboSet) == 0x24 && offsetof(CTaskSimpleFight, m_nCurrentMove) == 0x25
              && offsetof(CTaskSimpleFight, m_nNextCommand) == 0x26 && offsetof(CTaskSimpleFight, m_nLastCommand) == 0x27, "CTaskSimpleFight: +8, +0xA, +0xC, +0x14, +0x1C, +0x20, +0x24..+0x27");
static_assert(offsetof(CAnimBlendAssociation, m_BlendAmount) == 0x18 && offsetof(CAnimBlendAssociation, m_BlendDelta) == 0x1C && offsetof(CAnimBlendAssociation, m_CurrentTime) == 0x20
              && offsetof(CAnimBlendAssociation, m_Speed) == 0x24 && offsetof(CAnimBlendAssociation, m_Flags) == 0x2E, "CAnimBlendAssociation: +0x18, +0x1C, +0x20, +0x24, +0x2E");
static_assert(offsetof(CPlayerPedData, m_vecFightMovement) == 0xC && offsetof(CPlayerPedData, m_fMoveBlendRatio) == 0x14, "CPlayerPedData: +0xC / +0x10, +0x14");
static_assert(offsetof(CWeaponInfo, m_nBaseCombo) == 0x6E, "CWeaponInfo: combo base +0x6E");
static_assert(offsetof(CAnimBlock, IsLoaded) == 0x10 && sizeof(CAnimBlock) == 0x20 && sizeof(CAnimBlendAssocGroup) == 0x14 && offsetof(CAnimBlendAssocGroup, m_AnimBlock) == 0, "bloques de 0x20 (IsLoaded +0x10), grupos de 0x14 (bloque +0)");
static_assert(sizeof(CMeleeInfo) == 0x88 && offsetof(CMeleeInfo, m_fGroundLoop) == 0x44 && offsetof(CMeleeInfo, m_wFlags) == 0x84, "CMeleeInfo: 0x88 bytes, campo +0x44, banderas +0x84 (+0x85 = byte alto)");
}

namespace F85ILive {
using namespace F85IFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x623B10};
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
    u32 Get8(size_t off) const { return m[kGuard + off]; }
    // Cambios respecto a la imagen de referencia, sin los bytes [maskLo, maskHi) (campos de puntero que se observan aparte)
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
inline Arena<kPdArena> pdA, pdB;
inline std::array<Arena<kAssocArena>, kAssocs> asA;
inline Arena<kIntArena> intA;
inline Arena<kInfoArena> infoA;
inline Arena<kNameArena> nameA;
inline Arena<kGroupsArena> groupsA;

// Estado del caso en curso para los falsos
struct State { const C* c{}; bool mutDone[8]{}; unsigned playerCalls{}; };
inline State g;
inline bool Mut(u32 site) { if (g.c->mutSite != site || g.mutDone[site]) return false; g.mutDone[site] = true; return true; }
inline constexpr u32 kMeleeCB = 0x61DAE0;

// Eventos: 1 SetDeleteCallback [asoc, callback, datos]; 2 SetFinishCallback [asoc, callback, datos]; 3 SetCurrentTime [asoc, float]; 4 BlendAnimation [clump, grupo, id, retardo]; 5 IsPlayer [ped];
// 6 SetMoveState [ped, estado]; 7 GetFatAndMuscleModifier [estadistica]; 8 GetWeaponInfo [tipo, habilidad]; 9 GetAnimBlockName [grupo]; 10 GetAnimationBlock [nombre]; 11 AddAnimBlockRef [indice];
// 12 RemoveAnimBlockRef [indice]; 13 RequestModel [modelo, banderas]; 14 FindTargetOnGround [tarea, ped]; 15 GetTaskDuck [inteligencia, argumento].
// Los colaboradores thiscall son __fastcall (ECX, EDX, pila) que limpian su pila; el resto son __cdecl, como los originales (el EXE no lee ECX ni EDX tras ninguno).
inline void __fastcall FkDelete(void* self, void*, u32 cb, u32 data) {                            // 0x4CEBC0 (ret 8)
    Ev& e = PushEv(1, 3); e.v[0] = Tag(self); e.v[1] = cb; e.v[2] = Tag(uintptr_t(data));
    if (cb == kMeleeCB) { if (Mut(2)) taskA.Put32(0x1C, u32(asA[1].Obj())); if (Mut(3)) taskA.Put8(0x24, g.c->mutB); }
}
inline void __fastcall FkFinish(void* self, void*, u32 cb, u32 data) {                            // 0x4CEBE0 (ret 8)
    Ev& e = PushEv(2, 3); e.v[0] = Tag(self); e.v[1] = cb; e.v[2] = Tag(uintptr_t(data));
    if (cb == kMeleeCB) { if (Mut(2)) taskA.Put32(0x1C, u32(asA[1].Obj())); if (Mut(3)) taskA.Put8(0x24, g.c->mutB); }
}
inline void __fastcall FkTime(void* self, void*, u32 bits) { Ev& e = PushEv(3, 2); e.v[0] = Tag(self); e.v[1] = bits; }          // 0x4CEA80 (ret 4)
inline void* __cdecl FkBlend(u32 clump, u32 group, u32 id, u32 delta) {                           // 0x4D4610
    Ev& e = PushEv(4, 4); e.v[0] = Tag(uintptr_t(clump)); e.v[1] = group; e.v[2] = id; e.v[3] = delta;
    if (Mut(1)) taskA.Put8(0x26, g.c->mutB);
    return reinterpret_cast<void*>(asA[3].Obj());
}
inline u32 __fastcall FkPlayer(void* self, void*) {                                               // 0x5DF8F0 (AL)
    Ev& e = PushEv(5, 1); e.v[0] = Tag(self);
    if (++g.playerCalls == 1) { if (Mut(7)) taskA.Put8(0x26, g.c->mutB); if (Mut(5)) pedA.Put32(0x480, u32(pdB.Obj())); }
    return g.c->isplayer;
}
inline void __fastcall FkMoveState(void* self, void*, u32 state) {                                // 0x5DEC00 (ret 4)
    Ev& e = PushEv(6, 2); e.v[0] = Tag(self); e.v[1] = state;
    pedA.Put32(0x534, state);
}
inline float __cdecl FkStat(u32 stat) {                                                           // 0x559AF0 (ST0)
    Ev& e = PushEv(7, 1); e.v[0] = stat;
    if (Mut(6)) taskA.Put32(0x1C, u32(asA[1].Obj()));
    return std::bit_cast<float>(g.c->stat);
}
inline void* __cdecl FkInfo(u32 type, u32 skill) { Ev& e = PushEv(8, 2); e.v[0] = type; e.v[1] = skill; return reinterpret_cast<void*>(infoA.Obj()); }                       // 0x743C60
inline const char* __cdecl FkName(u32 group) { Ev& e = PushEv(9, 1); e.v[0] = group; return reinterpret_cast<const char*>(nameA.Obj(size_t(group & 0x3Fu) * 4)); }          // 0x4D3A30
inline void* __cdecl FkBlock(const char* name) { Ev& e = PushEv(10, 1); e.v[0] = Tag(name); return reinterpret_cast<void*>(uintptr_t(kBlocksAddr) + 0x20u * g.c->lookup); }  // 0x4D3940
inline void __cdecl FkAddRef(u32 idx) { Ev& e = PushEv(11, 1); e.v[0] = idx; }                                                                                              // 0x4D3FB0
inline void __cdecl FkRemoveRef(u32 idx) { Ev& e = PushEv(12, 1); e.v[0] = idx; }                                                                                           // 0x4D3FD0 (sin hook)
inline void __cdecl FkRequest(u32 model, u32 flags) { Ev& e = PushEv(13, 2); e.v[0] = model; e.v[1] = flags; }                                                            // 0x4087E0
inline void __fastcall FkFind(void* self, void*, u32 ped) {                                       // 0x61C9B0 (ret 4)
    Ev& e = PushEv(14, 2); e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(ped));
    if (Mut(4)) pedA.Put32(0x480, u32(pdB.Obj()));
}
inline u32 __fastcall FkDuck(void* self, void*, u32 arg) {                                        // 0x6010A0 (ret 4)
    Ev& e = PushEv(15, 2); e.v[0] = Tag(self); e.v[1] = arg;
    return g.c->duck ? u32(intA.Obj()) : 0u;
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
    taskA.Fill(c.seed * 13u + 50u); pedA.Fill(c.seed * 13u + 57u); pdA.Fill(c.seed * 13u + 60u); pdB.Fill(c.seed * 13u + 61u);
    for (unsigned i = 0; i < kAssocs; ++i) asA[i].Fill(c.seed * 13u + 70u + i);
    intA.Fill(c.seed * 13u + 80u); infoA.Fill(c.seed * 13u + 81u); nameA.Fill(c.seed * 13u + 82u); groupsA.Fill(c.seed * 13u + 83u);
}
inline void SnapArenas() {
    taskA.Snap(); pedA.Snap(); pdA.Snap(); pdB.Snap();
    for (unsigned i = 0; i < kAssocs; ++i) asA[i].Snap();
    intA.Snap(); infoA.Snap(); nameA.Snap(); groupsA.Snap();
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
        TagAdd(pdA.m.data(), pdA.m.size());         // 2
        TagAdd(pdB.m.data(), pdB.m.size());         // 3
        for (unsigned i = 0; i < kAssocs; ++i) TagAdd(asA[i].m.data(), asA[i].m.size());   // 4..7 (antigua, alternativa, de reposo, nueva)
        TagAdd(infoA.m.data(), infoA.m.size());     // 8
        TagAdd(nameA.m.data(), nameA.m.size());     // 9
        TagAdd(groupsA.m.data(), groupsA.m.size()); // 10
        TagAdd(intA.m.data(), intA.m.size());       // 11
        taskA.Put8(0x8, c.finished); taskA.Put8(0xA, c.ref); taskA.Put32(0xC, c.cur); taskA.Put8(0x14, c.cont);
        taskA.Put32(0x1C, c.hasAnim ? u32(asA[0].Obj()) : 0u); taskA.Put32(0x20, c.hasIdle ? u32(asA[2].Obj()) : 0u);
        taskA.Put8(0x24, c.combo); taskA.Put8(0x25, c.move); taskA.Put8(0x26, c.cmd); taskA.Put8(0x27, c.last);
        pedA.Put32(0x18, c.clump); pedA.Put32(0x46C, c.pflags); pedA.Put32(0x47C, u32(intA.Obj())); pedA.Put32(0x480, c.hasPd ? u32(pdA.Obj()) : 0u);
        pedA.Put32(0x4D4, c.agroup); pedA.Put32(0x534, c.mstate); pedA.Put32(0x538, c.mswim);
        pedA.Put8(0x718, c.slot);
        pedA.Put32(size_t(i32(0x5A0) + i32(int8(c.slot)) * 0x1C), c.wtype);
        pedA.Put8(0x72D, c.fstyle);
        for (unsigned i = 0; i < 3; ++i) { pdA.Put32(0xC + 4 * i, c.pdA[i]); pdB.Put32(0xC + 4 * i, c.pdB[i]); }
        for (unsigned i = 0; i < kAssocs; ++i) {
            asA[i].Put32(0x18, c.aAmt[i]); asA[i].Put32(0x1C, c.aDlt[i]); asA[i].Put32(0x20, c.aTime[i]); asA[i].Put32(0x24, c.aSpd[i]); asA[i].Put32(0x2C, c.aFl[i]);
        }
        infoA.Put8(0x6E, c.binfo);
        for (unsigned gi = 0; gi < kGroups; ++gi) groupsA.Put32(gi * 0x14, c.gblk[gi] ? u32(kBlocksAddr + 0x20u * (c.gblk[gi] - 1)) : 0u);
        G32(kGroupsAddr) = u32(groupsA.Obj());
        for (unsigned k = 0; k < 16; ++k) G8(kBlocksAddr + 0x20u * k + 0x10u) = uint8(c.loaded[k]);
        for (unsigned i = 0; i < 12; ++i) {
            G32(kComboAddr + i * 0x88) = c.rowGroup[i]; G8(kComboAddr + i * 0x88 + 0x85) = uint8(c.rowFlags[i]); G32(kComboAddr + i * 0x88 + 0x44) = c.rowLoop[i];
        }
        const std::vector<u32> args = { u32(pedA.Obj()), c.attack };
        SnapArenas();
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, u32(taskA.Obj()), c.edx, c.stack, 0); });
        json obs;
        obs["anim"] = Tag(uintptr_t(taskA.Get32(0x1C))); obs["idle"] = Tag(uintptr_t(taskA.Get32(0x20))); obs["pd"] = Tag(uintptr_t(pedA.Get32(0x480)));
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, taskA.Changes(0x1C, 0x24)); add(1, pedA.Changes(0x480, 0x484)); add(2, pdA.Changes()); add(3, pdB.Changes());
        for (unsigned i = 0; i < kAssocs; ++i) add(4 + i, asA[i].Changes());
        add(8, infoA.Changes()); add(9, nameA.Changes()); add(10, groupsA.Changes()); add(11, intA.Changes());
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
    if (!std::filesystem::exists(dir / "fn85i-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn85i-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn85i"); }
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
    const Pair pDel = MakeCollab(env, 0x4CEBC0, hooksFound), pFin = MakeCollab(env, 0x4CEBE0, hooksFound), pTime = MakeCollab(env, 0x4CEA80, hooksFound), pBlend = MakeCollab(env, 0x4D4610, hooksFound),
               pPlayer = MakeCollab(env, 0x5DF8F0, hooksFound), pMove = MakeCollab(env, 0x5DEC00, hooksFound), pFind = MakeCollab(env, 0x61C9B0, hooksFound), pDuck = MakeCollab(env, 0x6010A0, hooksFound),
               pInfo = MakeCollab(env, 0x743C60, hooksFound), pName = MakeCollab(env, 0x4D3A30, hooksFound), pBlock = MakeCollab(env, 0x4D3940, hooksFound), pAdd = MakeCollab(env, 0x4D3FB0, hooksFound),
               pRequest = MakeCollab(env, 0x4087E0, hooksFound);
    // GetFatAndMuscleModifier (0x559AF0): el hook del EXE es el envoltorio StatsGetFatAndMuscleModifierExe; la DLL llama a CStats::GetFatAndMuscleModifier, asi que el falso va en el cuerpo de ese metodo
    Pair pStat = MakeCollab(env, 0x559AF0, hooksFound);
    pStat.dll = Body(Addr(&CStats::GetFatAndMuscleModifier)); pStat.hooked = true;
    // RemoveAnimBlockRef (0x4D3FD0): sin hook RH; la DLL llama a su propia funcion C++, asi que el mismo falso va tambien en su cuerpo
    const Pair pRemove{0x4D3FD0, Body(Addr(&CAnimManager::RemoveAnimBlockRef)), false};
    env.stubs[0] = {S(pDel, reinterpret_cast<void*>(&FkDelete)), S(pFin, reinterpret_cast<void*>(&FkFinish)), S(pTime, reinterpret_cast<void*>(&FkTime)), S(pBlend, reinterpret_cast<void*>(&FkBlend)),
                    S(pPlayer, reinterpret_cast<void*>(&FkPlayer)), S(pMove, reinterpret_cast<void*>(&FkMoveState)), S(pStat, reinterpret_cast<void*>(&FkStat)), S(pFind, reinterpret_cast<void*>(&FkFind)),
                    S(pDuck, reinterpret_cast<void*>(&FkDuck)), S(pInfo, reinterpret_cast<void*>(&FkInfo)), S(pName, reinterpret_cast<void*>(&FkName)), S(pBlock, reinterpret_cast<void*>(&FkBlock)),
                    S(pAdd, reinterpret_cast<void*>(&FkAddRef)), S(pRemove, reinterpret_cast<void*>(&FkRemoveRef)), S(pRequest, reinterpret_cast<void*>(&FkRequest))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"GetAvailableComboSet (el EXE lo lleva en linea), filas de la tabla de combos, comparaciones ordenadas de los floats de las animaciones y banderas de las asociaciones"})},
                                 {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
