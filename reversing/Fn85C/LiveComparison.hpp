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
#include "Garage.h"
#include "Garages.h"
#include "World.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Collision/ColModel.h"
#include "Collision/CollisionData.h"
#include "Tasks/TaskTypes/TaskSimpleFight.h"
#include "CutsceneMgr.h"
#include "X87Math.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn85C dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Cuatro funciones (hook S): CGarage::IsAnyCarBlockingDoor 0x4494F0, CGarage::StoreAndRemoveCarsForThisImpoundingGarage 0x449A50, CTaskSimpleFight::BeHitWhileBlocking 0x61C650 y CTaskSimpleFight::ChooseAttackAI 0x624A40.
// Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (con su hook RH apagado durante el banco):
//   IsAnyCarBlockingDoor -> CGarage::IsEntityTouching3D 0x448EE0 (thiscall, ret 4), CEntity::GetColModel 0x535300 (thiscall), CMatrix * CVector 0x59C890 (cdecl; sin hook: la DLL lo llama por su direccion del EXE) y
//                           CGarage::IsPointInsideGarage(CVector, float) 0x4487D0 (thiscall, ret 10h; el hook del EXE es GarageIsPointInsideRadiusRegs y la DLL de C++ llama al metodo miembro: se parchea tambien su cuerpo);
//   StoreAndRemoveCarsForThisImpoundingGarage -> CGarage::IsPointInsideGarage(CVector) 0x448740 (ret 0Ch; idem), EntityHasASphereWayOutsideGarage 0x449050 (ret 8), CStoredCar::StoreCar 0x449760 (ret 4),
//                           CPlayerInfo::CancelPlayerEnteringCars 0x56E860 (ret 4), CWorld::Remove 0x563280 (cdecl) y el destructor virtual (tabla de funciones virtuales propia, ranura 0);
//   BeHitWhileBlocking -> ninguno (corre de verdad con los ayudantes x87 de X87Math.cpp en la DLL frente a los x87 del EXE);
//   ChooseAttackAI -> rand() del CRT del EXE 0x821B1E (sin hook: la DLL lo llama por su direccion), CPlayerPed::GetWantedLevel 0x41BE60 y CTaskSimpleFight::TargetInReach 0x61D6F0 (sin reconstruir: por su direccion).
// Registran sus argumentos EN ORDEN y devuelven lo que dice el caso. El pool de vehiculos (ds:[0xB74494]) se sustituye por uno propio de hasta 8 ranuras de 0xA18 bytes. ChooseAttackAI lee el byte bajo de las
// banderas de la fila (combo - 4) de la tabla de combos (0xC17154 + (combo - 4) * 0x88): el banco lo fija por caso y lo restaura.

namespace F85CLayout {
static_assert(offsetof(CGarage, m_fDoorPosition) == 0x38 && offsetof(CGarage, m_nType) == 0x4C && sizeof(CGarage) == F85CFix::kGarWin, "CGarage");
static_assert(offsetof(CEntity, m_matrix) == 0x14 && offsetof(CEntity, m_nModelIndex) == 0x22, "CEntity: matriz en +0x14");
static_assert(offsetof(CVehicle, m_nCreatedBy) == 0x4A4, "CVehicle: m_nCreatedBy en +0x4A4");
static_assert(offsetof(CColModel, m_pColData) == 0x2C, "CColModel: m_pColData en +0x2C");
static_assert(offsetof(CCollisionData, m_nNumSpheres) == 0 && offsetof(CCollisionData, m_pSpheres) == 8 && sizeof(CColSphere) == 0x14 && offsetof(CColSphere, m_fRadius) == 0xC, "CCollisionData / CColSphere");
static_assert(offsetof(CTaskSimpleFight, m_pTargetEntity) == 0x18 && offsetof(CTaskSimpleFight, m_pAnim) == 0x1C && offsetof(CTaskSimpleFight, m_nComboSet) == 0x24 && offsetof(CTaskSimpleFight, m_nLastCommand) == 0x27, "CTaskSimpleFight");
}

namespace F85CLive {
using namespace F85CFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x4494F0, 0x449A50, 0x61C650, 0x624A40};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint8& G8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }
template<class M> uintptr_t MA(M m) { static_assert(sizeof(M) == sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a, &m, sizeof(a)); return a; }

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { const uint16 w = uint16(v); memcpy(m.data() + kGuard + off, &w, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    u32 Get16(size_t off) const { uint16 v; memcpy(&v, m.data() + kGuard + off, 2); return v; }
    u32 Get8(size_t off) const { return m[kGuard + off]; }
    json Changes() const { return ArenaChanges(m.data(), r.data(), N, kGuard); }
};
inline Arena<kGarArena> garA;
inline Arena<kPoolArena> poolA;
inline Arena<kMapArena> mapA;
inline Arena<kPoolStructArena> poolStructA;
inline Arena<kMatPoolArena> matPoolA;
inline Arena<kColArena> colA;
inline Arena<kDataArena> dataA;
inline Arena<kSphArena> sphA;
inline Arena<kCarsArena> carsA;
inline Arena<kVtArena> vtA;
inline Arena<kTaskArena> taskA;
inline Arena<kAnimArena> animA;
inline Arena<kHierArena> hierA;
inline Arena<kPedLiteArena> vicA, creA;
inline Arena<kMatArena> vmatA, cmatA;
inline Arena<kPedArena> pedA;
inline Arena<kTargetArena> targetA;

// Estado del caso en curso para los falsos
struct State { const C* c{}; unsigned touch{}, xform{}, ipr{}, ipp{}, esph{}, curVeh{}; };
inline State g;
inline u32 Junk(u32 bit) { return (bit & 1u) | 0xA5A5A500u; }
inline u32 BitOf(u32 mask, unsigned k) { return (mask >> (k < 31 ? k : 31)) & 1u; }

// Eventos: 1 IsEntityTouching3D [this, entidad]; 2 GetColModel [this]; 3 CMatrix * CVector [matriz, vector]; 4 IsPointInsideGarage(radio) [this, x, y, z, radio]; 5 IsPointInsideGarage(punto) [this, x, y, z];
//          8 EntityHasASphereWayOutsideGarage [this, vehiculo, radio]; 9 StoreCar [this, vehiculo]; 10 CancelPlayerEnteringCars [this, vehiculo]; 11 CWorld::Remove [entidad]; 12 destructor [this, bandera];
//          17 rand; 18 GetWantedLevel [this]; 19 TargetInReach [this, ped]
inline u32 __fastcall FkTouch(void* self, void*, void* ent) {
    Ev& e = PushEv(1, 2); e.v[0] = Tag(self); e.v[1] = Tag(ent);
    return Junk(BitOf(g.c->tmask, g.touch++));
}
inline void* __fastcall FkCol(void* self, void*) {
    Ev& e = PushEv(2, 1); e.v[0] = Tag(self);
    const uintptr_t off = uintptr_t(self) - poolA.Obj();
    const unsigned idx = off % kSlotStride == 0 && off / kSlotStride < kPoolSlots ? unsigned(off / kSlotStride) : 0;
    g.curVeh = idx;
    return reinterpret_cast<void*>(colA.Obj(idx * 0x30));
}
inline u32* __cdecl FkXform(u32* out, void* mat, void* vec) {
    Ev& e = PushEv(3, 2); e.v[0] = Tag(mat); e.v[1] = Tag(vec);
    const unsigned k = g.xform++;
    for (u32 i = 0; i < 3; ++i) out[i] = Rand(g.c->seed, k * 4 + i + 1);
    return out;
}
#pragma runtime_checks("", off)
// Como el original (0x4487D0, ret 10h): ECX y EDX quedan intactos.
__declspec(safebuffers) inline u32 __fastcall FkIpr(void* self, void* entryEdx, u32 x, u32 y, u32 z, u32 r) {
    Ev& e = PushEv(4, 5); e.v[0] = Tag(self); e.v[1] = x; e.v[2] = y; e.v[3] = z; e.v[4] = r;
    const unsigned k = g.ipr++;
    if (g.c->reOn && k == g.c->reK) dataA.Put16(g.curVeh * 0x10, g.c->reVal);
    return ResidueRegsCpp(u32(uintptr_t(self)), u32(uintptr_t(entryEdx)), Junk(BitOf(g.c->rmask, k)));
}
#pragma runtime_checks("", restore)
inline bool __cdecl FakeInsideC(void* self, u32 x, u32 y, u32 z) {
    const unsigned k = g.ipp++;
    Ev& e = PushEv(5, 4); e.v[0] = Tag(self); e.v[1] = x; e.v[2] = y; e.v[3] = z;
    return ((g.c->imask >> (k < 31 ? k : 31)) & 1u) != 0;
}
// El original (0x448740, ret 0Ch) deja EDX intacto (un __fastcall de C++ lo destruiria).
__declspec(naked) inline void FakeInside() {
    __asm {
        push edx
        push dword ptr [esp+16]
        push dword ptr [esp+16]
        push dword ptr [esp+16]
        push ecx
        call FakeInsideC
        add esp,16
        pop edx
        ret 0Ch
    }
}
inline u32 __fastcall FkEsph(void* self, void*, void* veh, u32 radius) {
    Ev& e = PushEv(8, 3); e.v[0] = Tag(self); e.v[1] = Tag(veh); e.v[2] = radius;
    return Junk(BitOf(g.c->esmask, g.esph++));
}
inline void __fastcall FkStore(void* self, void*, void* veh) {
    Ev& e = PushEv(9, 2); e.v[0] = Tag(self); e.v[1] = Tag(veh);
    const uintptr_t off = uintptr_t(self) - carsA.Obj();
    if (off % 0x40 == 0 && off < 4 * 0x40) carsA.Put16(off + 0x12, 0x4000u + u32(off / 0x40));
}
inline void __fastcall FkCancel(void* self, void*, void* veh) { Ev& e = PushEv(10, 2); e.v[0] = Tag(self); e.v[1] = Tag(veh); }
inline void __cdecl FkRemove(void* ent) { Ev& e = PushEv(11, 1); e.v[0] = Tag(ent); }
inline void* __fastcall FkDtor(void* self, void*, u32 flag) { Ev& e = PushEv(12, 2); e.v[0] = Tag(self); e.v[1] = flag; return self; }
inline int __cdecl FkRand() { PushEv(17, 0); return int(g.c->randv); }
inline u32 __fastcall FkWanted(void* self, void*) { Ev& e = PushEv(18, 1); e.v[0] = Tag(self); return g.c->wlevel; }
inline u32 __fastcall FkReach(void* self, void*, void* ped) {
    Ev& e = PushEv(19, 2); e.v[0] = Tag(self); e.v[1] = Tag(ped);
    return (g.c->reach & 1u) | (g.c->reach & ~0xFFu);
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
    garA.Fill(c.seed * 13u + 2u);
    poolA.Fill(c.seed * 13u + 40u);
    mapA.Fill(c.seed * 13u + 41u);
    poolStructA.Fill(c.seed * 13u + 42u);
    matPoolA.Fill(c.seed * 13u + 43u);
    colA.Fill(c.seed * 13u + 44u);
    dataA.Fill(c.seed * 13u + 45u);
    sphA.Fill(c.seed * 13u + 46u);
    carsA.Fill(c.seed * 13u + 30u);
    vtA.Fill(c.seed * 13u + 31u);
    taskA.Fill(c.seed * 13u + 50u);
    animA.Fill(c.seed * 13u + 51u);
    hierA.Fill(c.seed * 13u + 52u);
    vicA.Fill(c.seed * 13u + 53u);
    creA.Fill(c.seed * 13u + 54u);
    vmatA.Fill(c.seed * 13u + 55u);
    cmatA.Fill(c.seed * 13u + 56u);
    pedA.Fill(c.seed * 13u + 57u);
    targetA.Fill(c.seed * 13u + 58u);
}
inline void SnapArenas() {
    garA.Snap(); poolA.Snap(); mapA.Snap(); poolStructA.Snap(); matPoolA.Snap(); colA.Snap(); dataA.Snap(); sphA.Snap(); carsA.Snap(); vtA.Snap();
    taskA.Snap(); animA.Snap(); hierA.Snap(); vicA.Snap(); creA.Snap(); vmatA.Snap(); cmatA.Snap(); pedA.Snap(); targetA.Snap();
}
inline void PoolSetup(const C& c) {
    for (unsigned sl = 0; sl < kPoolSlots; ++sl) {
        poolA.Put32(sl * kSlotStride + 0x14, c.pmat[sl] ? u32(matPoolA.Obj(sl * 0x48)) : 0u);
        mapA.Put8(sl, c.pmap[sl]);
    }
    poolStructA.Put32(0, u32(poolA.Obj()));
    poolStructA.Put32(4, u32(mapA.Obj()));
    poolStructA.Put32(8, c.psize);
    poolStructA.Put32(0xC, 0xFFFFFFFFu);
    G32(kPoolAddr) = u32(poolStructA.Obj());
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    C c = MakeC(fn, n);
    if (fn == 1) c.pinfo = kPlayerInfoBase + u32(G8(kPlayerFocusAddr)) * 0x190u;
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    const int8 csetSigned = int8(c.cset);
    const uintptr_t meleeFlagAddr = kMeleeFlagsBase + uintptr_t(int32(csetSigned) - 4) * 0x88u;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        SetupArenas(c);
        TagClear();
        std::vector<u32> args; u32 seedEcx = c.ecx; uint32 popBytes = 0;
        uint8 savedMelee = 0;
        switch (fn) {
        case 0:
            TagAdd(garA.m.data(), garA.m.size());                       // 0
            TagAdd(poolA.m.data(), poolA.m.size());                     // 1
            TagAdd(matPoolA.m.data(), matPoolA.m.size());               // 2
            TagAdd(colA.m.data(), colA.m.size());                       // 3
            TagAdd(dataA.m.data(), dataA.m.size());                     // 4
            TagAdd(sphA.m.data(), sphA.m.size());                       // 5
            TagAdd(mapA.m.data(), mapA.m.size());                       // 6
            TagAdd(poolStructA.m.data(), poolStructA.m.size());         // 7
            PoolSetup(c);
            for (unsigned sl = 0; sl < kPoolSlots; ++sl) {
                colA.Put32(sl * 0x30 + 0x2C, u32(dataA.Obj(sl * 0x10)));
                dataA.Put32(sl * 0x10, c.nsph[sl]);
                dataA.Put32(sl * 0x10 + 8, u32(sphA.Obj(sl * kMaxSph * 0x14)));
                for (unsigned j = 0; j < kMaxSph; ++j) sphA.Put32((sl * kMaxSph + j) * 0x14 + 0xC, c.sphR[sl][j]);
            }
            seedEcx = u32(garA.Obj());
            break;
        case 1:
            TagAdd(garA.m.data(), garA.m.size());                       // 0
            TagAdd(poolA.m.data(), poolA.m.size());                     // 1
            TagAdd(carsA.m.data(), carsA.m.size());                     // 2
            TagAdd(vtA.m.data(), vtA.m.size());                         // 3
            TagAdd(matPoolA.m.data(), matPoolA.m.size());               // 4
            TagAdd(mapA.m.data(), mapA.m.size());                       // 5
            TagAdd(poolStructA.m.data(), poolStructA.m.size());         // 6
            PoolSetup(c);
            vtA.Put32(0, u32(uintptr_t(&FkDtor)));
            for (unsigned sl = 0; sl < kPoolSlots; ++sl) {
                poolA.Put32(sl * kSlotStride, u32(vtA.Obj()));
                poolA.Put8(sl * kSlotStride + 0x4A4, c.createdBy[sl]);
                for (unsigned k = 0; k < 3; ++k) {
                    if (c.pmat[sl]) matPoolA.Put32(sl * 0x48 + 0x30 + 4 * k, c.ppos[sl][k]);
                    else poolA.Put32(sl * kSlotStride + 4 + 4 * k, c.ppos[sl][k]);
                }
            }
            for (unsigned k = 0; k < 4; ++k) carsA.Put16(0x40 * k + 0x12, c.cmodel[k]);
            args = { u32(carsA.Obj()), c.maxSlot };
            seedEcx = u32(garA.Obj());
            break;
        case 2:
            TagAdd(taskA.m.data(), taskA.m.size());                     // 0
            TagAdd(animA.m.data(), animA.m.size());                     // 1
            TagAdd(hierA.m.data(), hierA.m.size());                     // 2
            TagAdd(vicA.m.data(), vicA.m.size());                       // 3
            TagAdd(creA.m.data(), creA.m.size());                       // 4
            TagAdd(vmatA.m.data(), vmatA.m.size());                     // 5
            TagAdd(cmatA.m.data(), cmatA.m.size());                     // 6
            taskA.Put32(0x1C, c.animNull ? 0u : u32(animA.Obj()));
            taskA.Put8(0x24, c.cset);
            taskA.Put8(0x27, c.lastCmd);
            animA.Put32(0x14, u32(hierA.Obj()));
            animA.Put32(0x20, c.cur);
            animA.Put8(0x2E, c.animFlags);
            hierA.Put32(0x10, c.total);
            vicA.Put32(0x14, u32(vmatA.Obj()));
            for (unsigned k = 0; k < 3; ++k) {
                vmatA.Put32(0x10 + 4 * k, c.fwd[k]);
                vmatA.Put32(0x30 + 4 * k, c.vpos[k]);
                cmatA.Put32(0x30 + 4 * k, c.cpos[k]);
            }
            creA.Put32(0x14, c.cHasMat ? u32(cmatA.Obj()) : 0u);
            if (!c.cHasMat) for (unsigned k = 0; k < 3; ++k) creA.Put32(4 + 4 * k, c.cpos[k]);
            args = { u32(vicA.Obj()), u32(creA.Obj()), c.combo | (c.comboHi << 8), c.move | (c.moveHi << 8) };
            seedEcx = u32(taskA.Obj());
            break;
        default:
            TagAdd(taskA.m.data(), taskA.m.size());                     // 0
            TagAdd(pedA.m.data(), pedA.m.size());                       // 1
            TagAdd(targetA.m.data(), targetA.m.size());                 // 2
            taskA.Put32(0x18, c.tkind == 0 ? 0u : u32(targetA.Obj()));
            taskA.Put8(0x24, c.cset);
            pedA.Put8(0x72E, c.pbyte);
            targetA.Put8(0x36, c.ttype);
            targetA.Put32(0x480, c.tpd ? 0x13572468u : 0u);
            targetA.Put32(0x540, c.health);
            savedMelee = G8(meleeFlagAddr);
            G8(meleeFlagAddr) = uint8(c.tflags);
            args = { u32(pedA.Obj()) };
            seedEcx = u32(taskA.Obj());
            break;
        }
        SnapArenas();
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, seedEcx, c.edx, c.stack, popBytes); });
        json obs;
        obs["ret"] = fn == 0 ? (sides[s].reg.eax & 0xFFu) : (fn == 2 ? (sides[s].reg.eax & 0xFFu) : (fn == 3 ? (sides[s].reg.eax & 0xFFFFu) : 0u));
        bool meleeRestored = true;
        if (fn == 3) {
            obs["melee_flag_unchanged"] = G8(meleeFlagAddr) == uint8(c.tflags);
            G8(meleeFlagAddr) = savedMelee;
            meleeRestored = G8(meleeFlagAddr) == savedMelee;
        }
        switch (fn) {
        case 0:
            obs["pool_changes"] = poolA.Changes(); obs["matpool_changes"] = matPoolA.Changes(); obs["col_changes"] = colA.Changes();
            obs["data_changes"] = dataA.Changes(); obs["sph_changes"] = sphA.Changes(); obs["gar_changes"] = garA.Changes();
            break;
        case 1:
            obs["cars_changes"] = carsA.Changes(); obs["pool_changes"] = poolA.Changes(); obs["matpool_changes"] = matPoolA.Changes(); obs["gar_changes"] = garA.Changes();
            break;
        case 2:
            obs["task_changes"] = taskA.Changes(); obs["anim_changes"] = animA.Changes(); obs["hier_changes"] = hierA.Changes();
            obs["vic_changes"] = vicA.Changes(); obs["cre_changes"] = creA.Changes(); obs["vmat_changes"] = vmatA.Changes(); obs["cmat_changes"] = cmatA.Changes();
            break;
        default:
            obs["task_changes"] = taskA.Changes(); obs["ped_changes"] = pedA.Changes(); obs["target_changes"] = targetA.Changes();
            break;
        }
        obs["events"] = EvsJson();
        rows[s] = CloseSide(env, sides[s], obs);
        if (!meleeRestored) rows[s]["globals_restored"] = false;
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
    if (!std::filesystem::exists(dir / "fn85c-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn85c-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn85c"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Rutas RH de las funciones probadas: se construyen ANTES que nada; el constructor deja el hook apagado. El cuerpo de la DLL se calcula aqui, antes de crear las rutas de los colaboradores y los parches de entrada.
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
    const Pair pTouch = MakeCollab(env, 0x448EE0, hooksFound), pCol = MakeCollab(env, 0x535300, hooksFound), pXform = MakeCollab(env, 0x59C890, hooksFound), pIpr = MakeCollab(env, 0x4487D0, hooksFound);
    const Pair pIpp = MakeCollab(env, 0x448740, hooksFound), pEsph = MakeCollab(env, 0x449050, hooksFound), pStore = MakeCollab(env, 0x449760, hooksFound);
    const Pair pCancel = MakeCollab(env, 0x56E860, hooksFound), pRemove = MakeCollab(env, 0x563280, hooksFound);
    const Pair pRand = MakeCollab(env, 0x821B1E, hooksFound), pWanted = MakeCollab(env, 0x41BE60, hooksFound), pReach = MakeCollab(env, 0x61D6F0, hooksFound);
    env.stubs[0] = {S(pTouch, reinterpret_cast<void*>(&FkTouch)), S(pCol, reinterpret_cast<void*>(&FkCol)), S(pXform, reinterpret_cast<void*>(&FkXform)), S(pIpr, reinterpret_cast<void*>(&FkIpr))};
    env.stubs[1] = {S(pIpp, reinterpret_cast<void*>(&FakeInside)), S(pEsph, reinterpret_cast<void*>(&FkEsph)), S(pStore, reinterpret_cast<void*>(&FkStore)), S(pCancel, reinterpret_cast<void*>(&FkCancel)), S(pRemove, reinterpret_cast<void*>(&FkRemove))};
    env.stubs[2] = {};
    env.stubs[3] = {S(pRand, reinterpret_cast<void*>(&FkRand)), S(pWanted, reinterpret_cast<void*>(&FkWanted)), S(pReach, reinterpret_cast<void*>(&FkReach))};
    // La DLL de C++ llama a los metodos miembro IsPointInsideGarage(CVector) y (CVector, float), no al cuerpo del hook del EXE (GarageIsPointInsideRegs / GarageIsPointInsideRadiusRegs): se parchean tambien.
    env.dllOnly.push_back(S(Pair{0, Body(MA(static_cast<bool (CGarage::*)(CVector)>(&CGarage::IsPointInsideGarage))), true}, reinterpret_cast<void*>(&FakeInside)));
    env.dllOnly.push_back(S(Pair{0, Body(MA(static_cast<bool (CGarage::*)(CVector, float)>(&CGarage::IsPointInsideGarage))), true}, reinterpret_cast<void*>(&FkIpr)));
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", f == 2 ? json::array({"ayudantes x87 de X87Math.cpp (Veh83DSub, Veh83DMul, Veh83DAdd)"}) : (f == 3 ? json::array({"ayudantes x87 de X87Math.cpp (Veh83DMul, Veh83DStore)"}) : json::array())},
            {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]},
            {"dll_only", f == 0 ? json::array({"CGarage::IsPointInsideGarage(CVector, float)"}) : (f == 1 ? json::array({"CGarage::IsPointInsideGarage(CVector)"}) : json::array())}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
