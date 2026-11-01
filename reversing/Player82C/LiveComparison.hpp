#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <memory>
#include <utility>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Entity/Ped/PlayerPed.h"
#include "PlayerPedData.h"
#include "Animation/AnimBlendAssociation.h"
#include "Animation/AnimManager.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "Camera.h"
#include "World.h"
#include "Timer.h"
#include "Pad.h"
#include "Clothes.h"
#include "Stats.h"
#include "Cheat.h"
#include "PedIntelligence.h"
#include "X87Math.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Player82C dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CPlayerPed::SetRealMoveAnim (0x60A9C0, hook S, thiscall sin argumentos, `ret`). Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en la direccion del EXE y
// en el cuerpo de la DLL): RpAnimBlendClumpGetAssociation 0x4D68B0, CAnimManager::BlendAnimation 0x4D4610 y AddAnimation 0x4D3AA0 (entregan animaciones nuevas de arenas), CAnimBlendAssociation::SetCurrentTime
// 0x4CEA80 y SetDeleteCallback 0x4CEBC0, CPed::RestoreHeadingRate 0x5DFD60, CPlayerPed::GetPadFromPlayer 0x609560, CPad::GetForceCameraBehindPlayer 0x540AE0 y AimWeaponLeftRight 0x541040,
// CPedIntelligence::GetTaskFighting 0x600F30 / GetTaskUseGun 0x600F70 / GetTaskDuck 0x6010A0 / GetTaskThrow 0x600FB0 / GetTaskJetPack 0x601110, CWorld::TestSphereAgainstWorld 0x569E20,
// CClothes::GetDefaultPlayerMotionGroup 0x5A81B0, CStats::UpdateStatsWhenSprinting 0x55C660 y UpdateStatsWhenRunning 0x55C6F0 y CPlayerPed::GetButtonSprintResults 0x60A820 (este deja EDX intacto como el
// original: su llamador en 0x60B42C conserva en EDX la animacion de sprint). El destructor virtual de las animaciones (ranura 0 de una tabla falsa) registra el borrado.
// Datos del caso: jugador (matriz, posicion, banderas +0x470, grupo +0x4D4, estado +0x534, +0x560), datos del jugador (+0x14, +0x18, +0x34, +0x3C), 10 animaciones existentes (o ausentes) mas 4 nuevas,
// tres jerarquias, camara activa (0xB6F081) y modos (0xB6F1A8 + 0x238 * i), reloj, escala de tiempo (0xB7CB64) y la trampa de adrenalina (0x969177).
// NaN: la x87 y SSE propagan cargas utiles de NaN distintas; todo float observado se canonicaliza (cualquier NaN = 0x7FC00000); ver README.md.

namespace Pm82CLayout {
struct PmPlayer : CPlayerPed {
    static constexpr size_t OffIntelligence() { return offsetof(PmPlayer, m_pIntelligence); }
    static constexpr size_t OffPlayerData() { return offsetof(PmPlayer, m_pPlayerData); }
    static constexpr size_t OffGroup() { return offsetof(PmPlayer, m_nAnimGroup); }
    static constexpr size_t OffMove() { return offsetof(PmPlayer, m_nMoveState); }
    static constexpr size_t OffHeading() { return offsetof(PmPlayer, m_fHeadingChangeRate); }
    static constexpr size_t OffMatrix() { return offsetof(PmPlayer, m_matrix); }
    static constexpr size_t OffPlacement() { return offsetof(PmPlayer, m_placement); }
};
static_assert(PmPlayer::OffIntelligence() == 0x47C && PmPlayer::OffPlayerData() == 0x480 && PmPlayer::OffGroup() == 0x4D4 && PmPlayer::OffMove() == 0x534
              && PmPlayer::OffHeading() == 0x560 && PmPlayer::OffMatrix() == 0x14 && PmPlayer::OffPlacement() == 4, "CPlayerPed / CPed");
static_assert(offsetof(CPlayerPedData, m_fMoveBlendRatio) == 0x14 && offsetof(CPlayerPedData, m_fTimeCanRun) == 0x18 && offsetof(CPlayerPedData, m_nPlayerFlags) == 0x34
              && offsetof(CPlayerPedData, m_nAdrenalineEndTime) == 0x3C, "CPlayerPedData");
static_assert(offsetof(CAnimBlendAssociation, m_BlendHier) == 0x14 && offsetof(CAnimBlendAssociation, m_BlendAmount) == 0x18 && offsetof(CAnimBlendAssociation, m_BlendDelta) == 0x1C
              && offsetof(CAnimBlendAssociation, m_CurrentTime) == 0x20 && offsetof(CAnimBlendAssociation, m_Speed) == 0x24 && offsetof(CAnimBlendAssociation, m_TimeStep) == 0x28
              && offsetof(CAnimBlendAssociation, m_Flags) == 0x2E, "CAnimBlendAssociation");
static_assert(offsetof(CCamera, m_nActiveCam) == 0x59 && offsetof(CCam, m_nMode) == 0xC, "CCamera::m_nActiveCam (0xB6F081) y CCam::m_nMode");
}

namespace Pm82CLive {
using namespace Pm82CFix;

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(ptrdiff_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put16(ptrdiff_t off,u32 v) {const u16 w=u16(v);memcpy(m.data()+kGuard+off,&w,2);}
    u32 Get32(ptrdiff_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    u32 Get16(ptrdiff_t off) const {u16 v;memcpy(&v,m.data()+kGuard+off,2);return v;}
    // bytes distintos de la referencia fuera de los rangos [lo, hi) dados (desplazamientos respecto al objeto): [[desplazamiento, byte]]
    json Changes(std::initializer_list<std::pair<ptrdiff_t,ptrdiff_t>> skips={}) const {
        json out=json::array();
        for(size_t i=0;i<N;++i)if(m[i]!=r[i]){
            const ptrdiff_t off=ptrdiff_t(i)-ptrdiff_t(kGuard);bool skip=false;
            for(const auto& k:skips)skip|=off>=k.first&&off<k.second;
            if(!skip)out.push_back(json::array({int64(off),m[i]}));
        }
        return out;
    }
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kPedArena> pedA;
inline Arena<kPdArena> pdA;
inline std::array<Arena<kAssocArena>,kAssocs> assocA;
inline std::array<Arena<kAssocArena>,kNewAssocs> newA;
inline std::array<Arena<kHierArena>,3> hierA;
inline Arena<kHierArena> hierOther;
inline Arena<kMatArena> pmatA;
inline Arena<kSmallArena> intelA,padA,clumpA;
inline std::array<void*,48> assocVt{};

inline uintptr_t Addr(auto member) {return Body(std::bit_cast<uintptr_t>(member));}
inline constexpr uintptr_t kActiveCam=0xB6F081,kCamMode=0xB6F1A8,kCamStride=0x238,kTimer=0xB7CB84,kTimeScale=0xB7CB64,kCheat=0x969177;

inline u32 Canon(u32 w) {return (w&0x7F800000u)==0x7F800000u&&(w&0x007FFFFFu)?0x7FC00000u:w;}
inline u32 Rd(const void* p,size_t off) {u32 v;memcpy(&v,static_cast<const u8*>(p)+off,4);return v;}

// Cierra el lado s de un caso (fila comun + restauracion de regiones y estado x87)
inline json CloseSide(Env& env,Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    obs["globals_hash"]=RegionsHash();
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}

// ================================================================================================ SetRealMoveAnim (0x60A9C0)
struct State {const C* c{};unsigned newCount{},aimCalls{};};
inline State g;
enum : u32 {EvGet=1,EvBlend,EvAdd,EvSetTime,EvSetCb,EvRestore,EvGetPad,EvForce,EvAim,EvFight,EvGun,EvDuck,EvThrow,EvJet,EvSphere,EvMotion,EvStatsSprint,EvStatsRun,EvSprint,EvDtor,EvTrap};
inline uintptr_t restoreCb{};

// 0x4D68B0 RpAnimBlendClumpGetAssociation(clump, id), cdecl: la animacion del indice del id o nulo
inline u32 __cdecl FakeGet(u32 clump,u32 id) {
    Ev& e=PushEv(EvGet,2);e.v[0]=Tag(uintptr_t(clump));e.v[1]=id;
    for(unsigned i=0;i<kAssocs;++i)if(kIdOf[i]==id)return (g.c->mask>>i)&1?u32(assocA[i].Obj()):0u;
    return 0;
}
// 0x4D4610 CAnimManager::BlendAnimation(clump, grupo, id, float), cdecl; 0x4D3AA0 AddAnimation(clump, grupo, id), cdecl: la siguiente animacion nueva
inline u32 NextNew() {const unsigned k=g.newCount<kNewAssocs?g.newCount:kNewAssocs-1;++g.newCount;return u32(k);}
inline u32 __cdecl FakeBlend(u32 clump,u32 group,u32 id,u32 delta) {
    const u32 k=NextNew();Ev& e=PushEv(EvBlend,5);e.v[0]=Tag(uintptr_t(clump));e.v[1]=group;e.v[2]=id;e.v[3]=Canon(delta);e.v[4]=k;return u32(newA[k].Obj());
}
inline u32 __cdecl FakeAdd(u32 clump,u32 group,u32 id) {
    const u32 k=NextNew();Ev& e=PushEv(EvAdd,4);e.v[0]=Tag(uintptr_t(clump));e.v[1]=group;e.v[2]=id;e.v[3]=k;return u32(newA[k].Obj());
}
// 0x4CEA80 CAnimBlendAssociation::SetCurrentTime(float), thiscall `ret 4`; 0x4CEBC0 SetDeleteCallback(callback, data), thiscall `ret 8`
inline void __fastcall FakeSetTime(void* self,void*,u32 time) {Ev& e=PushEv(EvSetTime,2);e.v[0]=Tag(self);e.v[1]=Canon(time);}
inline void __fastcall FakeSetCb(void* self,void*,u32 cb,u32 data) {
    Ev& e=PushEv(EvSetCb,3);e.v[0]=Tag(self);e.v[1]=(cb==0x5DFD70u||uintptr_t(cb)==restoreCb||Body(uintptr_t(cb))==restoreCb)?1u:cb;e.v[2]=Tag(uintptr_t(data));
}
// 0x5DFD60 CPed::RestoreHeadingRate(), thiscall
inline void __fastcall FakeRestore(void* self,void*) {Ev& e=PushEv(EvRestore,1);e.v[0]=Tag(self);}
// 0x609560 CPlayerPed::GetPadFromPlayer(), thiscall: la arena del pad
inline u32 __fastcall FakeGetPad(void* self,void*) {Ev& e=PushEv(EvGetPad,1);e.v[0]=Tag(self);return u32(padA.Obj());}
// 0x540AE0 CPad::GetForceCameraBehindPlayer(): AL; 0x541040 CPad::AimWeaponLeftRight(CPed*), thiscall `ret 4`: AX con signo (las tres llamadas devuelven valores distintos)
inline u32 __fastcall FakeForce(void* self,void*) {Ev& e=PushEv(EvForce,1);e.v[0]=Tag(self);return g.c->forceRet;}
inline u32 __fastcall FakeAim(void* self,void*,u32 ped) {Ev& e=PushEv(EvAim,2);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(ped));return g.c->aimRet[g.aimCalls<2?g.aimCalls++:2];}
// 0x600F30 GetTaskFighting, 0x600F70 GetTaskUseGun, 0x6010A0 GetTaskDuck(bool), 0x600FB0 GetTaskThrow, 0x601110 GetTaskJetPack: thiscall, puntero (solo importa si es nulo)
inline u32 __fastcall FakeFight(void* self,void*) {Ev& e=PushEv(EvFight,1);e.v[0]=Tag(self);return g.c->taskRet[0];}
inline u32 __fastcall FakeGun(void* self,void*) {Ev& e=PushEv(EvGun,1);e.v[0]=Tag(self);return g.c->taskRet[1];}
inline u32 __fastcall FakeDuck(void* self,void*,u32 ignore) {Ev& e=PushEv(EvDuck,2);e.v[0]=Tag(self);e.v[1]=ignore&0xFF;return g.c->taskRet[2];}
inline u32 __fastcall FakeThrow(void* self,void*) {Ev& e=PushEv(EvThrow,1);e.v[0]=Tag(self);return g.c->taskRet[3];}
inline u32 __fastcall FakeJet(void* self,void*) {Ev& e=PushEv(EvJet,1);e.v[0]=Tag(self);return g.c->taskRet[4];}
// 0x569E20 CWorld::TestSphereAgainstWorld(CVector, radio, ignorar, 6 bool), cdecl
inline u32 __cdecl FakeSphere(u32 x,u32 y,u32 z,u32 radius,u32 ignore,u32 b0,u32 b1,u32 b2,u32 b3,u32 b4,u32 b5) {
    Ev& e=PushEv(EvSphere,11);e.v[0]=Canon(x);e.v[1]=Canon(y);e.v[2]=Canon(z);e.v[3]=Canon(radius);e.v[4]=Tag(uintptr_t(ignore));
    const u32 bools[6]{b0,b1,b2,b3,b4,b5};for(unsigned i=0;i<6;++i)e.v[5+i]=bools[i]&0xFF;
    return g.c->sphereRet;
}
// 0x5A81B0 CClothes::GetDefaultPlayerMotionGroup(), cdecl: EAX completo
inline u32 __cdecl FakeMotion() {PushEv(EvMotion,0);return g.c->motionRet;}
inline void __cdecl FakeStatsSprint() {PushEv(EvStatsSprint,0);}
inline void __cdecl FakeStatsRun() {PushEv(EvStatsRun,0);}
// 0x60A820 CPlayerPed::GetButtonSprintResults(eSprintType), thiscall `ret 4`: ST0; como el original solo toca EAX/ECX/ST0 (ECX = datos del jugador) y deja EDX intacto
#pragma runtime_checks("", off)
inline float __declspec(safebuffers) __fastcall FakeSprint(void* self,u32 entryEdx,u32 arg) {
    Ev& e=PushEv(EvSprint,2);e.v[0]=Tag(self);e.v[1]=arg;
    return PlayerPedResidueFloatCpp(u32(pdA.Obj()),entryEdx,F(g.c->sprintRet));
}
#pragma runtime_checks("", restore)
// Ranura 0 de la tabla virtual de las animaciones (destructor con borrado, thiscall `ret 4`); cualquier otra se registra
inline u32 __fastcall FakeDtor(void* self,void*,u32 flag) {Ev& e=PushEv(EvDtor,2);e.v[0]=Tag(self);e.v[1]=flag;return 0;}
template<int I> inline u32 __fastcall TrapSlot(void*,void*) {Ev& e=PushEv(EvTrap,1);e.v[0]=I;return 0;}
template<int... Is> inline void FillTraps(std::array<void*,48>& vt,std::integer_sequence<int,Is...>) {((vt[Is]=reinterpret_cast<void*>(&TrapSlot<Is>)),...);}

// Campos modelados de cada objeto (el resto de bytes debe quedar intacto)
inline json AssocJson(const Arena<kAssocArena>& a) {
    return json::array({a.Get16(0x2E),Canon(a.Get32(0x18)),Canon(a.Get32(0x1C)),Canon(a.Get32(0x20)),Canon(a.Get32(0x24)),Canon(a.Get32(0x28))});
}

inline void CaseMove(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        pedA.Fill(c.seed*31u+1u);pdA.Fill(c.seed*31u+2u);pmatA.Fill(c.seed*31u+3u);intelA.Fill(c.seed*31u+4u);padA.Fill(c.seed*31u+5u);clumpA.Fill(c.seed*31u+6u);hierOther.Fill(c.seed*31u+7u);
        for(unsigned i=0;i<kAssocs;++i)assocA[i].Fill(c.seed*31u+10u+i);
        for(unsigned i=0;i<kNewAssocs;++i)newA[i].Fill(c.seed*31u+30u+i);
        for(unsigned i=0;i<3;++i)hierA[i].Fill(c.seed*31u+40u+i);
        clumpA.m[kGuard]=2;   // RwObject::type = rpCLUMP (CEntity::GetRpClump lo comprueba en la DLL de depuracion; el EXE no)
        // jugador: matriz (+0x14), posicion, clump (+0x18), banderas (+0x470), inteligencia, datos del jugador, grupo, estado y velocidad de giro
        pedA.Put32(0x14,c.hasMatrix?u32(pmatA.Obj()):0u);
        for(unsigned i=0;i<3;++i){pedA.Put32(4+4*i,c.pl[i]);pmatA.Put32(0x30+4*i,c.pm[i]);}
        pedA.Put32(0x18,u32(clumpA.Obj()));pedA.Put32(0x470,c.pedFlags);pedA.Put32(0x47C,u32(intelA.Obj()));pedA.Put32(0x480,u32(pdA.Obj()));
        pedA.Put32(0x4D4,c.group);pedA.Put32(0x534,c.moveState);pedA.Put32(0x560,c.headRate);
        pdA.Put32(0x14,c.ratio);pdA.Put32(0x18,c.canRun);pdA.Put32(0x34,c.pdFlags);pdA.Put32(0x3C,c.advEnd);
        for(unsigned i=0;i<kAssocs;++i) {
            auto& a=assocA[i];
            a.Put32(0,u32(uintptr_t(assocVt.data())));
            u32 hier=u32(hierOther.Obj());if(i==aRunStop)hier=u32(hierA[0].Obj());else if(i==aSprint)hier=u32(hierA[1].Obj());else if(i==aWalkStart)hier=u32(hierA[2].Obj());
            a.Put32(0x14,hier);a.Put32(0x18,c.amount[i]);a.Put32(0x1C,c.delta[i]);a.Put32(0x20,c.cur[i]);a.Put32(0x24,c.speed[i]);a.Put32(0x28,c.step[i]);a.Put16(0x2E,c.flags[i]);
        }
        for(unsigned i=0;i<kNewAssocs;++i){auto& a=newA[i];a.Put32(0,u32(uintptr_t(assocVt.data())));a.Put32(0x14,u32(hierOther.Obj()));a.Put16(0x2E,c.newInit[i][0]);a.Put32(0x18,c.newInit[i][1]);a.Put32(0x1C,c.newInit[i][2]);}
        for(unsigned i=0;i<3;++i)hierA[i].Put32(0x10,c.total[i]);
        // globales: camara activa y modos, reloj, escala de tiempo y trampa de adrenalina
        *reinterpret_cast<u8*>(kActiveCam)=u8(c.camIdx);
        for(unsigned k=0;k<3;++k)*reinterpret_cast<u32*>(kCamMode+kCamStride*k)=c.camMode[k];
        *reinterpret_cast<u32*>(kTimer)=c.timer;*reinterpret_cast<u32*>(kTimeScale)=c.timeScale0;*reinterpret_cast<u8*>(kCheat)=u8(c.cheat);
        pedA.Snap();pdA.Snap();pmatA.Snap();intelA.Snap();padA.Snap();clumpA.Snap();hierOther.Snap();
        for(unsigned i=0;i<kAssocs;++i)assocA[i].Snap();
        for(unsigned i=0;i<kNewAssocs;++i)newA[i].Snap();
        for(unsigned i=0;i<3;++i)hierA[i].Snap();
        TagClear();pedA.Tagged();pdA.Tagged();
        for(unsigned i=0;i<kAssocs;++i)assocA[i].Tagged();
        for(unsigned i=0;i<kNewAssocs;++i)newA[i].Tagged();
        for(unsigned i=0;i<3;++i)hierA[i].Tagged();
        hierOther.Tagged();pmatA.Tagged();intelA.Tagged();padA.Tagged();clumpA.Tagged();
        g=State{};g.c=&c;nev=0;
        sides[s]=Execute(env,0,0x60A9C0,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,u32(pedA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["ped"]=json::array({pedA.Get32(0x534),Canon(pedA.Get32(0x560)),pedA.Get32(0x470)});
        obs["pd"]=json::array({Canon(pdA.Get32(0x14)),Canon(pdA.Get32(0x18)),pdA.Get32(0x34),pdA.Get32(0x3C)});
        json assoc=json::array();
        for(unsigned i=0;i<kAssocs;++i)assoc.push_back(AssocJson(assocA[i]));
        for(unsigned i=0;i<kNewAssocs;++i)assoc.push_back(AssocJson(newA[i]));
        obs["assoc"]=assoc;
        obs["globals"]=json::array({Canon(*reinterpret_cast<u32*>(kTimeScale)),*reinterpret_cast<u8*>(kCheat),*reinterpret_cast<u32*>(kTimer)});
        json changes=json::array({pedA.Changes({{0x470,0x474},{0x534,0x538},{0x560,0x564}}),pdA.Changes({{0x14,0x1C},{0x34,0x38}}),pmatA.Changes(),intelA.Changes(),padA.Changes(),clumpA.Changes(),hierOther.Changes()});
        for(unsigned i=0;i<kAssocs;++i)changes.push_back(assocA[i].Changes({{0x18,0x2C},{0x2E,0x30}}));
        for(unsigned i=0;i<kNewAssocs;++i)changes.push_back(newA[i].Changes({{0x18,0x2C},{0x2E,0x30}}));
        for(unsigned i=0;i<3;++i)changes.push_back(hierA[i].Changes());
        obs["changes"]=changes;
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    for(unsigned f=0;f<kFunctions;++f) {
        const uintptr_t body=env.route[f]?env.route[f]->Body():0;
        ready&=body&&ModuleOf(body)==env.asi;
        for(const auto& s:env.stubs[f]) {
            if(s.pair.hooked)ready&=s.pair.dll!=0&&ModuleOf(s.pair.dll)==env.asi;
            else ready&=s.pair.dll==0||ModuleOf(s.pair.dll)==env.asi;
        }
    }
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),w.saved.size())}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseMove(env,totals,c);
            Write({{"event","function_summary"},{"function",kFn[f]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()&&RegionsIntact()}});
        }
    }
    RestoreRegions();
    const bool entriesIntact=env.WatchesIntact()&&RegionsIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"player82c-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"player82c-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("player82c");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    env.route[0].reset(new HookRoute("CPlayerPed","SetRealMoveAnim",0x60A9C0,HookRoute::RegisteredTarget{}));
    registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
    ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    // las globales que el EXE usa deben ser las de la DLL
    for(unsigned k=0;k<3;++k)ready&=uintptr_t(&TheCamera.m_aCams.data()[k].m_nMode)==kCamMode+kCamStride*k;
    ready&=uintptr_t(&TheCamera.m_nActiveCam)==kActiveCam&&uintptr_t(&CTimer::m_snTimeInMilliseconds)==kTimer&&uintptr_t(&CTimer::ms_fTimeScale)==kTimeScale&&uintptr_t(&CCheat::m_aCheatsActive[CHEAT_ADRENALINE_MODE])==kCheat;
    FillTraps(assocVt,std::make_integer_sequence<int,48>{});assocVt[0]=reinterpret_cast<void*>(&FakeDtor);
    restoreCb=Addr(&CPed::RestoreHeadingRateCB);
    env.stubs[0]={
        {Pair{0x4D68B0,Addr(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation))},reinterpret_cast<void*>(&FakeGet)},
        {Pair{0x4D4610,Addr(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation))},reinterpret_cast<void*>(&FakeBlend)},
        {Pair{0x4D3AA0,Addr(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId)>(&CAnimManager::AddAnimation))},reinterpret_cast<void*>(&FakeAdd)},
        {Pair{0x4CEA80,Addr(&CAnimBlendAssociation::SetCurrentTime)},reinterpret_cast<void*>(&FakeSetTime)},
        {Pair{0x4CEBC0,Addr(&CAnimBlendAssociation::SetDeleteCallback)},reinterpret_cast<void*>(&FakeSetCb)},
        {Pair{0x5DFD60,Addr(&CPed::RestoreHeadingRate)},reinterpret_cast<void*>(&FakeRestore)},
        {Pair{0x609560,Addr(&CPlayerPed::GetPadFromPlayer)},reinterpret_cast<void*>(&FakeGetPad)},
        {Pair{0x540AE0,Addr(&CPad::GetForceCameraBehindPlayer)},reinterpret_cast<void*>(&FakeForce)},
        {Pair{0x541040,Addr(&CPad::AimWeaponLeftRight)},reinterpret_cast<void*>(&FakeAim)},
        {Pair{0x600F30,Addr(&CPedIntelligence::GetTaskFighting)},reinterpret_cast<void*>(&FakeFight)},
        {Pair{0x600F70,Addr(&CPedIntelligence::GetTaskUseGun)},reinterpret_cast<void*>(&FakeGun)},
        {Pair{0x6010A0,Addr(&CPedIntelligence::GetTaskDuck)},reinterpret_cast<void*>(&FakeDuck)},
        {Pair{0x600FB0,Addr(&CPedIntelligence::GetTaskThrow)},reinterpret_cast<void*>(&FakeThrow)},
        {Pair{0x601110,Addr(&CPedIntelligence::GetTaskJetPack)},reinterpret_cast<void*>(&FakeJet)},
        {Pair{0x569E20,Addr(&CWorld::TestSphereAgainstWorld)},reinterpret_cast<void*>(&FakeSphere)},
        {Pair{0x5A81B0,Addr(&CClothes::GetDefaultPlayerMotionGroup)},reinterpret_cast<void*>(&FakeMotion)},
        {Pair{0x55C660,Addr(&CStats::UpdateStatsWhenSprinting)},reinterpret_cast<void*>(&FakeStatsSprint)},
        {Pair{0x55C6F0,Addr(&CStats::UpdateStatsWhenRunning)},reinterpret_cast<void*>(&FakeStatsRun)},
        {Pair{0x60A820,Addr(&CPlayerPed::GetButtonSprintResults)},reinterpret_cast<void*>(&FakeSprint)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"destructor virtual de las animaciones: ranura 0 de una tabla falsa (registra el borrado)"})},{"stubbed",stubbed}};
    }
    env.watches.emplace_back(0x60A9C0,16);
    for(const uintptr_t k:{0x858B50u,0x858624u,0x858C58u,0x8D24ECu,0x858B88u,0x858EE8u,0x858C98u,0x858CA0u})env.watches.emplace_back(k,4);   // constantes float
    for(const uintptr_t k:{0x859EF8u,0x859060u})env.watches.emplace_back(k,8);                                                              // constantes double
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
