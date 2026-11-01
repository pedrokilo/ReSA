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
#include "Camera.h"
#include "World.h"
#include "WeaponInfo.h"
#include "Timer.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupIntelligence.h"
#include "PedIntelligence.h"
#include "Events/Event.h"
#include "Events/EventGroup.h"
#include "Events/EventGroupEvent.h"
#include "Events/EventGunAimedAt.h"
#include "Tasks/TaskManager.h"
#include "Pools/Pools.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Player82B dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CPlayerPed::Compute3rdPersonMouseTarget (0x60B650, hook S, thiscall, un bool de pila, `ret 4`). Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en la
// direccion del EXE y en el cuerpo de la DLL): GetWeaponSkill 0x5E6580, GetWeaponInfo 0x743C60, Find3rdPersonCamTargetVector 0x514970, ProcessLineOfSight 0x56BA00, ResetLineTestOptions 0x5631C0,
// CPed::IsAlive 0x5E0170, CleanUpOldReference 0x571A00, RegisterReference 0x571B70, IsInSeeingRange 0x600C60, CPed::Say 0x5EFFE0, GetPedsGroup 0x5F7E80, AreInSameGroup 0x5F7F40,
// CPedGroupIntelligence::AddEvent 0x5F7470 y CEventGroup::Add 0x4AB420 (con hook) y CEvent::operator new 0x4B5620 (sin hook: la entrada del EXE sigue siendo codigo original; entrega una ranura real del pool de eventos).
// Corren de verdad en ambos lados: los constructores y destructores de CEventGunAimedAt (0x4B0700/0x4B07B0) y CEventGroupEvent (0x4ADFD0/0x4AE070) sobre la pila o la arena del evento reservado
// (el vptr de los objetos difiere entre el EXE y la DLL: no se compara; los campos escritos si). La tabla virtual de las tareas pendientes es falsa (48 ranuras; la ranura 4, GetTaskType, devuelve el valor del caso).
// Datos del caso: jugador, peatones (golpeado, anterior, otro), inteligencia y tareas del golpeado, grupo, tres estructuras CWeaponInfo falsas (una por llamada), tres camaras (0xB6F32C + 0x238 * i), el byte del
// raton (0xB6EC2E), la camara activa (0xB6F081), el reloj (0xB7CB84) y las banderas de trazado (0xB7CD68).
// NaN: la x87 y SSE propagan cargas utiles de NaN distintas; todo float observado se canonicaliza (cualquier NaN = 0x7FC00000); ver README.md.

namespace Pm82BLayout {
struct PmPlayer : CPlayerPed {
    static constexpr size_t OffTarget() { return offsetof(PmPlayer, m_p3rdPersonMouseTarget); }
    static constexpr size_t OffTime() { return offsetof(PmPlayer, field_7A0); }
    static constexpr size_t OffSlot() { return offsetof(PmPlayer, m_nActiveWeaponSlot); }
    static constexpr size_t OffWeapons() { return offsetof(PmPlayer, m_aWeapons); }
    static constexpr size_t OffMatrix() { return offsetof(PmPlayer, m_matrix); }
    static constexpr size_t OffPlacement() { return offsetof(PmPlayer, m_placement); }
    static constexpr size_t OffIntelligence() { return offsetof(PmPlayer, m_pIntelligence); }
};
static_assert(PmPlayer::OffTarget() == 0x79C && PmPlayer::OffTime() == 0x7A0 && PmPlayer::OffSlot() == 0x718 && PmPlayer::OffWeapons() == 0x5A0 && PmPlayer::OffMatrix() == 0x14
              && PmPlayer::OffPlacement() == 4 && PmPlayer::OffIntelligence() == 0x47C && sizeof(CWeapon) == 0x1C, "CPlayerPed / CPed");
static_assert(offsetof(CPedIntelligence, m_TaskMgr) == 4 && offsetof(CPedIntelligence, m_eventGroup) == 0x68 && offsetof(CPedGroup, m_groupIntelligence) == 0x30, "inteligencia y grupo");
static_assert(sizeof(CEventGunAimedAt) == 0x18 && offsetof(CEventGunAimedAt, m_AimedBy) == 0x14 && sizeof(CEventGroupEvent) == 0x14 && offsetof(CEventGroupEvent, m_ped) == 0xC
              && offsetof(CEventGroupEvent, m_event) == 0x10 && offsetof(CEvent, m_nTimeActive) == 4 && offsetof(CEvent, m_bValid) == 8, "eventos");
static_assert(offsetof(CCamera, m_nActiveCam) == 0x59, "CCamera::m_nActiveCam (0xB6F081)");
}

namespace Pm82BLive {
using namespace Pm82BFix;

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(ptrdiff_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    u32 Get32(ptrdiff_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    // bytes distintos de la referencia fuera de [skipLo, skipHi) como [desplazamiento respecto al objeto, byte]
    json Changes(ptrdiff_t skipLo=0,ptrdiff_t skipHi=0) const {
        json out=json::array();
        for(size_t i=0;i<N;++i)if(m[i]!=r[i]){const ptrdiff_t off=ptrdiff_t(i)-ptrdiff_t(kGuard);if(!(off>=skipLo&&off<skipHi))out.push_back(json::array({int64(off),m[i]}));}
        return out;
    }
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kPedArena> playerA,hitA,oldA,altA;
inline Arena<kMatArena> pmatA;
inline Arena<kIntelArena> intelA;
inline std::array<Arena<kTaskArena>,3> taskA;
inline Arena<kGroupArena> groupA;
inline std::array<Arena<kInfoArena>,3> infoA;
inline std::array<void*,48> taskVt{};

inline uintptr_t Addr(auto member) {return Body(std::bit_cast<uintptr_t>(member));}
inline constexpr uintptr_t kMouseFlag=0xB6EC2E,kActiveCam=0xB6F081,kCamVecs=0xB6F32C,kCamStride=0x238,kTimer=0xB7CB84,kIgnore=0xB7CD68;

inline u32 Canon(u32 w) {return (w&0x7F800000u)==0x7F800000u&&(w&0x007FFFFFu)?0x7FC00000u:w;}
inline u32 RefTag(const void* ref) {const uintptr_t v=uintptr_t(ref);for(const auto& t:tagTable)if(v>=t.base&&v<t.base+t.size)return Tag(v);return 0xFFFFFFFEu;}
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

// ================================================================================================ Compute3rdPersonMouseTarget (0x60B650)
struct State {const C* c{};unsigned infoCalls{};};
inline State g;
enum : u32 {EvSkill=1,EvInfo,EvFind3,EvLos,EvReset,EvAlive,EvClean,EvReg,EvSee,EvSay,EvGetGroup,EvSame,EvNew,EvDel,EvAddEvent,EvGroupAdd,EvTaskType,EvTrap};

// 0x5E6580 CPed::GetWeaponSkill(), thiscall sin argumentos: devuelve EAX completo
inline u32 __fastcall FakeSkill(void* self,void*) {Ev& e=PushEv(EvSkill,1);e.v[0]=Tag(self);return g.c->skillRet;}
// 0x743C60 CWeaponInfo::GetWeaponInfo(tipo, habilidad), cdecl: una estructura distinta por llamada
inline void* __cdecl FakeInfo(u32 type,u32 skill) {Ev& e=PushEv(EvInfo,2);e.v[0]=type;e.v[1]=skill;const unsigned i=g.infoCalls++<2?g.infoCalls-1:2;return reinterpret_cast<void*>(infoA[i].Obj());}
// 0x514970 CCamera::Find3rdPersonCamTargetVector(float, CVector, CVector&, CVector&), thiscall, `ret 0x18`
inline void __fastcall FakeFind3(void* self,void*,u32 range,u32 ox,u32 oy,u32 oz,u32* outSrc,u32* outTgt) {
    Ev& e=PushEv(EvFind3,5);e.v[0]=uintptr_t(self)==0xB6F028u?1u:0u;e.v[1]=Canon(range);e.v[2]=Canon(ox);e.v[3]=Canon(oy);e.v[4]=Canon(oz);
    for(unsigned i=0;i<3;++i){outSrc[i]=g.c->f3Src[i];outTgt[i]=g.c->f3Tgt[i];}
}
// 0x56BA00 CWorld::ProcessLineOfSight (12 argumentos), cdecl: devuelve EAX completo
inline u32 __cdecl FakeLos(const u32* origin,const u32* target,void*,u32* outEntity,u32 b0,u32 b1,u32 b2,u32 b3,u32 b4,u32 b5,u32 b6,u32 b7) {
    Ev& e=PushEv(EvLos,19);
    for(unsigned i=0;i<3;++i){e.v[i]=Canon(origin[i]);e.v[3+i]=Canon(target[i]);}
    const u32 flags[8]{b0,b1,b2,b3,b4,b5,b6,b7};
    for(unsigned i=0;i<8;++i)e.v[6+i]=flags[i]&0xFF;
    e.v[14]=Tag(uintptr_t(*reinterpret_cast<u32*>(kIgnore)));e.v[15]=*reinterpret_cast<u8*>(0xB7CD6F);e.v[16]=*reinterpret_cast<u8*>(0xB7CD70);e.v[17]=*reinterpret_cast<u8*>(0xB7CD71);
    e.v[18]=Tag(uintptr_t(*outEntity));
    if((g.c->losRet&0xFF)||g.c->losWrites) {
        uintptr_t hit=0;
        switch(g.c->hitMode){case 1:hit=hitA.Obj();break;case 2:hit=playerA.Obj();break;case 3:hit=altA.Obj();break;default:break;}
        *outEntity=u32(hit);
    }
    return g.c->losRet;
}
// 0x5631C0 CWorld::ResetLineTestOptions(), cdecl
inline void __cdecl FakeReset() {PushEv(EvReset,0);}
// 0x5E0170 CPed::IsAlive() const, thiscall: AL
inline u32 __fastcall FakeAlive(void* self,void*) {Ev& e=PushEv(EvAlive,1);e.v[0]=Tag(self);return g.c->aliveRet;}
// 0x571A00 CEntity::CleanUpOldReference(CEntity**) y 0x571B70 RegisterReference(CEntity**): thiscall, `ret 4`; se registra la entidad, la ranura (si es una arena) y su contenido
inline void __fastcall FakeClean(void* self,void*,const u32* ref) {Ev& e=PushEv(EvClean,3);e.v[0]=Tag(self);e.v[1]=RefTag(ref);e.v[2]=Tag(uintptr_t(ref[0]));}
inline void __fastcall FakeReg(void* self,void*,const u32* ref) {Ev& e=PushEv(EvReg,3);e.v[0]=Tag(self);e.v[1]=RefTag(ref);e.v[2]=Tag(uintptr_t(ref[0]));}
// 0x600C60 CPedIntelligence::IsInSeeingRange(const CVector&) const, thiscall `ret 4`: AL
inline u32 __fastcall FakeSee(void* self,void*,const u32* pos) {Ev& e=PushEv(EvSee,5);e.v[0]=Tag(self);e.v[1]=RefTag(pos);for(unsigned i=0;i<3;++i)e.v[2+i]=Canon(pos[i]);return g.c->seeRet;}
// 0x5EFFE0 CPed::Say(contexto, retraso, probabilidad, 3 bool), thiscall `ret 0x18`
inline u32 __fastcall FakeSay(void* self,void*,u32 phrase,u32 delay,u32 prob,u32 b0,u32 b1,u32 b2) {
    Ev& e=PushEv(EvSay,7);e.v[0]=Tag(self);e.v[1]=phrase&0xFFFF;e.v[2]=delay;e.v[3]=Canon(prob);e.v[4]=b0&0xFF;e.v[5]=b1&0xFF;e.v[6]=b2&0xFF;return 0;
}
// 0x5F7E80 CPedGroups::GetPedsGroup(const CPed*), cdecl: grupo del caso o nulo
inline u32 __cdecl FakeGetGroup(u32 ped) {Ev& e=PushEv(EvGetGroup,1);e.v[0]=Tag(uintptr_t(ped));return g.c->groupMode?u32(groupA.Obj()):0u;}
// 0x5F7F40 CPedGroups::AreInSameGroup(a, b), cdecl: AL
inline u32 __cdecl FakeSame(u32 a,u32 b) {Ev& e=PushEv(EvSame,2);e.v[0]=Tag(uintptr_t(a));e.v[1]=Tag(uintptr_t(b));return g.c->sameRet;}
// 0x4B5620 CEvent::operator new(size), cdecl: una ranura real del pool de eventos (el EXE y la DLL la liberan de verdad: el destructor virtual del EXE inlinea el borrado del pool);
// se etiqueta como la arena 13 (cada lado obtiene una direccion distinta, el mismo papel). CEvent::operator delete (0x4B5630) corre real.
inline void* __cdecl FakeNew(u32 size) {
    Ev& e=PushEv(EvNew,1);e.v[0]=size;
    if(g.c->newNull)return nullptr;
    void* const slot=GetEventPool()->New();
    if(slot)TagAdd(slot,kEvWin);
    return slot;
}
// 0x5F7470 CPedGroupIntelligence::AddEvent(CEvent*), thiscall `ret 4`: campos del CEventGroupEvent local (+4 tiempo, +8 valido, +0xC peaton, +0x10 evento)
inline u32 __fastcall FakeAddEvent(void* self,void*,const u8* ev) {
    const u8* const inner=reinterpret_cast<const u8*>(uintptr_t(Rd(ev,0x10)));   // el CEventGunAimedAt reservado (si existe): campos que escribio su constructor
    Ev& e=PushEv(EvAddEvent,inner?11:5);e.v[0]=Tag(self);e.v[1]=Rd(ev,4);e.v[2]=ev[8];e.v[3]=Tag(uintptr_t(Rd(ev,0xC)));e.v[4]=Tag(uintptr_t(Rd(ev,0x10)));
    if(inner){e.v[5]=Rd(inner,4);e.v[6]=inner[8];e.v[7]=inner[0xC];e.v[8]=*reinterpret_cast<const u16*>(inner+0xE);e.v[9]=*reinterpret_cast<const u16*>(inner+0x10);e.v[10]=Tag(uintptr_t(Rd(inner,0x14)));}
    return 1;
}
// 0x4AB420 CEventGroup::Add(CEvent*, bool), thiscall `ret 8`: campos escritos por el constructor de CEventGunAimedAt
inline void* __fastcall FakeGroupAdd(void* self,void*,const u8* ev,u32 valid) {
    Ev& e=PushEv(EvGroupAdd,8);e.v[0]=Tag(self);e.v[1]=Rd(ev,4);e.v[2]=ev[8];e.v[3]=ev[0xC];e.v[4]=*reinterpret_cast<const u16*>(ev+0xE);e.v[5]=*reinterpret_cast<const u16*>(ev+0x10);
    e.v[6]=Tag(uintptr_t(Rd(ev,0x14)));e.v[7]=valid&0xFF;return nullptr;
}
// Tabla virtual falsa de las tareas pendientes: la ranura 4 (GetTaskType) devuelve el valor del caso; cualquier otra se registra
inline u32 __fastcall FakeTaskType(void* self,void*) {
    Ev& e=PushEv(EvTaskType,1);e.v[0]=Tag(self);
    for(unsigned i=0;i<3;++i)if(self==reinterpret_cast<void*>(taskA[i].Obj()))return g.c->taskRet[i];
    return 0;
}
template<int I> inline u32 __fastcall TrapSlot(void*,void*) {Ev& e=PushEv(EvTrap,1);e.v[0]=I;return 0;}
template<int... Is> inline void FillTraps(std::array<void*,48>& vt,std::integer_sequence<int,Is...>) {((vt[Is]=reinterpret_cast<void*>(&TrapSlot<Is>)),...);}

inline void CaseMouse(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        playerA.Fill(c.seed*31u+1u);hitA.Fill(c.seed*31u+2u);oldA.Fill(c.seed*31u+3u);altA.Fill(c.seed*31u+4u);pmatA.Fill(c.seed*31u+5u);intelA.Fill(c.seed*31u+6u);
        for(unsigned i=0;i<3;++i){taskA[i].Fill(c.seed*31u+7u+i);infoA[i].Fill(c.seed*31u+10u+i);}
        groupA.Fill(c.seed*31u+13u);
        // jugador: ranura activa (byte +0x718), tipo del arma de esa ranura, matriz (+0x14), posiciones, objetivo del raton anterior (+0x79C) y su plazo (+0x7A0)
        playerA.m[kGuard+0x718]=u8(c.slot);
        {const int slot=int(int8(c.slot));playerA.Put32(0x5A0+slot*0x1C,c.wType);}
        playerA.Put32(0x14,c.pHasM?u32(pmatA.Obj()):0u);
        for(unsigned i=0;i<3;++i){playerA.Put32(4+4*i,c.pPl[i]);pmatA.Put32(0x30+4*i,c.pM[i]);}
        uintptr_t old=0;switch(c.oldMode){case 1:old=hitA.Obj();break;case 2:old=oldA.Obj();break;case 3:old=altA.Obj();break;default:break;}
        playerA.Put32(0x79C,u32(old));playerA.Put32(0x7A0,c.f7a0);
        // peaton golpeado: inteligencia (+0x47C) con tareas en +4, +8 y +0xC (m_TaskMgr + 0, 4, 8)
        hitA.Put32(0x47C,u32(intelA.Obj()));
        altA.Put32(0x47C,u32(intelA.Obj()));   // el trazado tambien puede golpear a otro peaton
        for(unsigned i=0;i<3;++i){
            intelA.Put32(4+4*i,(c.taskMask>>i)&1?u32(taskA[i].Obj()):0u);
            taskA[i].Put32(0,u32(uintptr_t(taskVt.data())));
        }
        // estructuras CWeaponInfo falsas: llamada 0 (alcance +4), 1 (tipo de disparo +0) y 2 (banderas +0x18)
        infoA[0].Put32(0,c.info0Fire);infoA[0].Put32(4,c.range0);
        infoA[1].Put32(0,c.fire1);infoA[1].Put32(4,c.info1Range);infoA[1].Put32(0x18,c.info1Flags);
        infoA[2].Put32(0,c.info2Fire);infoA[2].Put32(4,c.info2Range);infoA[2].Put32(0x18,c.flags2);
        // camara: byte del raton, camara activa, vectores de las tres camaras, reloj y banderas de trazado
        *reinterpret_cast<u8*>(kMouseFlag)=u8(c.useMouse);*reinterpret_cast<u8*>(kActiveCam)=u8(c.camIdx);
        for(unsigned k=0;k<3;++k)for(unsigned i=0;i<3;++i){*reinterpret_cast<u32*>(kCamVecs+kCamStride*k+4*i)=c.camFront[k][i];*reinterpret_cast<u32*>(kCamVecs+kCamStride*k+0xC+4*i)=c.camSrc[k][i];}
        *reinterpret_cast<u32*>(kTimer)=c.timer;
        *reinterpret_cast<u32*>(kIgnore)=c.ignoreOld;*reinterpret_cast<u8*>(0xB7CD70)=u8(c.carTyres);*reinterpret_cast<u8*>(0xB7CD71)=u8(c.deadPeds);*reinterpret_cast<u8*>(0xB7CD6F)=u8(c.carTyres^c.deadPeds^0x5Au);
        playerA.Snap();hitA.Snap();oldA.Snap();altA.Snap();pmatA.Snap();intelA.Snap();groupA.Snap();
        for(unsigned i=0;i<3;++i){taskA[i].Snap();infoA[i].Snap();}
        TagClear();playerA.Tagged();hitA.Tagged();oldA.Tagged();altA.Tagged();intelA.Tagged();
        for(unsigned i=0;i<3;++i)taskA[i].Tagged();
        groupA.Tagged();for(unsigned i=0;i<3;++i)infoA[i].Tagged();pmatA.Tagged();
        g=State{};g.c=&c;nev=0;
        const u32 meleeArg=c.meleeArg;
        sides[s]=Execute(env,0,0x60B650,candidate,[&](uintptr_t entry){return Gate(entry,&meleeArg,1,c.eax,u32(playerA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["changes"]=json::array({playerA.Changes(0x79C,0x7A4),hitA.Changes(),oldA.Changes(),altA.Changes(),pmatA.Changes(),intelA.Changes(),groupA.Changes(),
                                    taskA[0].Changes(),taskA[1].Changes(),taskA[2].Changes(),infoA[0].Changes(),infoA[1].Changes(),infoA[2].Changes()});
        obs["events"]=EvsJson();
        obs["target"]=Tag(uintptr_t(playerA.Get32(0x79C)));obs["t7a0"]=playerA.Get32(0x7A0);
        obs["globals"]=json::array({Tag(uintptr_t(*reinterpret_cast<u32*>(kIgnore))),*reinterpret_cast<u8*>(0xB7CD6F),*reinterpret_cast<u8*>(0xB7CD70),*reinterpret_cast<u8*>(0xB7CD71)});
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
            for(unsigned c=0;c<kCases;++c)CaseMouse(env,totals,c);
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
    if(!std::filesystem::exists(dir/"player82b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"player82b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("player82b");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    env.route[0].reset(new HookRoute("CPlayerPed","Compute3rdPersonMouseTarget",0x60B650,HookRoute::RegisteredTarget{}));
    registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
    ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    // las posiciones de los vectores de la camara que el EXE usa (0xB6F32C / 0xB6F338 + 0x238 * i) deben ser las de la DLL
    for(unsigned k=0;k<3;++k)ready&=uintptr_t(&TheCamera.m_aCams.data()[k].m_vecFront)==kCamVecs+kCamStride*k&&uintptr_t(&TheCamera.m_aCams.data()[k].m_vecSource)==kCamVecs+0xC+kCamStride*k;
    ready&=uintptr_t(&CCamera::m_bUseMouse3rdPerson)==kMouseFlag&&uintptr_t(&CTimer::m_snTimeInMilliseconds)==kTimer&&uintptr_t(&CWorld::pIgnoreEntity)==kIgnore
          &&uintptr_t(&CWorld::bIncludeBikers)==0xB7CD6F&&uintptr_t(&CWorld::bIncludeCarTyres)==0xB7CD70&&uintptr_t(&CWorld::bIncludeDeadPeds)==0xB7CD71;
    FillTraps(taskVt,std::make_integer_sequence<int,48>{});taskVt[4]=reinterpret_cast<void*>(&FakeTaskType);
    // Colaboradores: con hook (el EXE salta a la DLL en ambos lados); CEvent::operator new/delete sin hook (la entrada del EXE sigue siendo codigo original)
    env.stubs[0]={
        {Pair{0x5E6580,Addr(static_cast<eWeaponSkill(CPed::*)()>(&CPed::GetWeaponSkill))},reinterpret_cast<void*>(&FakeSkill)},
        {Pair{0x743C60,Addr(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo))},reinterpret_cast<void*>(&FakeInfo)},
        {Pair{0x514970,Addr(&CCamera::Find3rdPersonCamTargetVector)},reinterpret_cast<void*>(&FakeFind3)},
        {Pair{0x56BA00,Addr(&CWorld::ProcessLineOfSight)},reinterpret_cast<void*>(&FakeLos)},
        {Pair{0x5631C0,Addr(&CWorld::ResetLineTestOptions)},reinterpret_cast<void*>(&FakeReset)},
        {Pair{0x5E0170,Addr(&CPed::IsAlive)},reinterpret_cast<void*>(&FakeAlive)},
        {Pair{0x571A00,Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference))},reinterpret_cast<void*>(&FakeClean)},
        {Pair{0x571B70,Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference))},reinterpret_cast<void*>(&FakeReg)},
        {Pair{0x600C60,Addr(&CPedIntelligence::IsInSeeingRange)},reinterpret_cast<void*>(&FakeSee)},
        {Pair{0x5EFFE0,Addr(&CPed::Say)},reinterpret_cast<void*>(&FakeSay)},
        {Pair{0x5F7E80,Addr(&CPedGroups::GetPedsGroup)},reinterpret_cast<void*>(&FakeGetGroup)},
        {Pair{0x5F7F40,Addr(&CPedGroups::AreInSameGroup)},reinterpret_cast<void*>(&FakeSame)},
        {Pair{0x4B5620,Addr(static_cast<void*(*)(unsigned)>(&CEvent::operator new)),false},reinterpret_cast<void*>(&FakeNew)},
        {Pair{0x5F7470,Addr(static_cast<bool(CPedGroupIntelligence::*)(CEvent*)>(&CPedGroupIntelligence::AddEvent))},reinterpret_cast<void*>(&FakeAddEvent)},
        {Pair{0x4AB420,Addr(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add))},reinterpret_cast<void*>(&FakeGroupAdd)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"constructores y destructores de CEventGunAimedAt y CEventGroupEvent y CEvent::operator delete (EXE contra DLL; el evento reservado es una ranura real del pool)","calculo de la linea sin vector de la camara (X87Math.cpp Player82BAimLineCpp)"})},{"stubbed",stubbed}};
    }
    env.watches.emplace_back(0x60B650,16);
    for(const uintptr_t k:{0x858B50u})env.watches.emplace_back(k,4);   // 0.0f (comparacion del producto escalar)
    SaveRegions();
    {   // pool de eventos: cabecera (+0 objetos, +4 estados, +8 capacidad, +0xC ultima ranura libre) y bytes de estado (se restauran tras cada lado)
        const uintptr_t pool=*reinterpret_cast<uintptr_t*>(0xB744AC);
        regions.emplace_back(pool,0x14);
        regions.emplace_back(*reinterpret_cast<uintptr_t*>(pool+4),size_t(*reinterpret_cast<int32*>(pool+8)));
        ready&=uintptr_t(GetEventPool())==pool;
    }
    Run(env,ready,registered,bodies,collaborators);
}
}
