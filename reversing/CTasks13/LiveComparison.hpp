#pragma once
// Banco dentro del motor: seguir ruta, lider de banda, persecucion policial y secuencia (lista de 13, 51 funciones).
// CTaskComplexFollowPointRoute (constructor, Clone, GetTaskType, MakeAbortable, CreateNextSubTask, CreateFirstSubTask,
// ControlSubTask, GetSubTaskType, CreateTask, GetLastWaypoint, GetNextWaypoint, CalcGoToTaskType, CalcBlendRatio),
// CTaskComplexGangLeader (constructor, destructor, Clone, MakeAbortable, CreateNextSubTask, CreateFirstSubTask,
// ControlSubTask, ScanForStuff, GetRandomGangAmbientAnim, ShouldLoadGangAnims, DoGangAbuseSpeech, TryToPassObject),
// CTaskComplexPolicePursuit (constructor, destructor, Clone, GetTaskType, MakeAbortable, CreateNextSubTask,
// CreateFirstSubTask, ControlSubTask, CreateSubTask, SetWeapon, ClearPursuit, SetPursuit, PersistPursuit) y
// CTaskComplexSequence (constructor, Clone, MakeAbortable, CreateNextSubTask virtual e interna, CreateFirstSubTask,
// ControlSubTask, AddTask x2, Flush, Contains, SetCanBeEmptied, DeRegister).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 6 peatones (el 4 y el 5 hacen de jugadores 0 y 1: CWorld::Players[0/1].m_pPed se cambian y se restauran en cada
// ejecucion), 2 vehiculos (vtable falsa con trampas, modelo con colision real), 10 tareas falsas (vtable trazada), un
// evento falso (GetEventType trazado), un CPedGroup y un CWanted falsos, clump/frame/asociaciones/FX falsos y una ruta
// de puntos dentro de un buffer mayor (las lecturas fuera de rango del EXE caen en bytes conocidos).
// Se simulan y trazan: RNG, FindPlayerWanted, CWanted (SetPursuitCop, RemovePursuitCop, RemoveExcessPursuitCops,
// IsInPursuit), CCullZones::NoPolice, CPed::SetCurrentWeapon, CPed::Say, IsEntityBlockingTarget,
// CStreaming::IsVeryBusy/RequestModel, Add/RemoveAnimBlockRef, GetDistSqOfClosestPathNodeToPed,
// SetDefaultTaskAllocatorType, CreateFxSystem/AttachToBone/PlayAndKill, IKChainManager (IsLooking, LookAt,
// AbortLookAt), GetEntityThatThisPedIsHolding, IsPlayingHandSignal, RpAnimBlendClumpGetAssociation,
// SetTaskSecondary (EXE) / ChangeTaskInSlot (DLL), GetClosestGroupPed, CPedGroupIntelligence::AddEvent, CEventGroup::Add,
// CPedGroups::GetPedsGroup, AddFollower y CPedGroup::Process. FindPlayerPed devuelve CWorld::Players[...].m_pPed sin
// trazar (el EXE lo lee en linea en ShouldLoadGangAnims).
// Pools de tareas, rutas de puntos y eventos: m_LastFreeSlot y estados restaurados y slots libres con 0xCD en cada
// ejecucion; lo nuevo se serializa y se libera sin destructor. Se guarda y restaura el reloj.
// Fuera del banco: varios vehiculos validos en ScanForStuff (FIX_BUGS: el DLL para tras el primer evento) y la rama de ControlSubTask de GangLeader que en el EXE devuelve StandStill(500) tras
// SetDefaultTaskAllocatorType (el DLL devuelve nullptr a proposito, ver el comentario del codigo).
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Tasks/Task.h"
#include "Tasks/TaskComplexSequence.h"
#include "Tasks/TaskTypes/TaskComplexFollowPointRoute.h"
#include "Tasks/TaskTypes/TaskComplexGangLeader.h"
#include "Tasks/TaskTypes/TaskComplexPolicePursuit.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundCar.h"
#include "Tasks/TaskTypes/TaskComplexWander.h"
#include "Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.h"
#include "Ragdoll/IKChainManager.h"
#include "PedIntelligence.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "Animation/AnimManager.h"
#include "Fx/FxManager.h"
#include "Fx/FxSystem.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupMembership.h"
#include "PedGroupIntelligence.h"
#include "PedGeometryAnalyser.h"
#include "Events/EventGroup.h"
#include "Wanted.h"
#include "CullZones.h"
#include "Streaming.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "Pools/PointRoutePool.h"
#include "Pools/EventPool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks13Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline json Vec(const CVector& v) { return json::array({F(v.x),F(v.y),F(v.z)}); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }
template<class M> uintptr_t MemberAddress(M m){ static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }
inline uintptr_t Body(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

constexpr int NPEDS=6, NVEH=2, FAKES=10, NASSOC=4;
// tareas falsas: campos de control por encima de 0x60 (GoToPoint usa hasta +0x24)
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_SUB=0x64, FK_TYPE=0x68, FK_CLONE=0x6C, FK_CNT=0x70;
// desplazamientos crudos del EXE
constexpr size_t E_POS=4, E_MATRIX=0x14, E_RWOBJ=0x18, E_FLAGS=0x1C, E_MODEL=0x22, E_TYPE=0x36;                // CEntity
constexpr size_t PH_SPEED=0x44;                                                                                 // CPhysical
constexpr size_t P_FLAGS=0x46C, P_FLAGS3=0x474, P_INTEL=0x47C, P_PDATA=0x480, P_CREATEDBY=0x484, P_STATE=0x530,
                 P_MOVESTATE=0x534, P_HEALTH=0x540, P_STANDING=0x568, P_VEH=0x58C, P_PEDTYPE=0x598, P_WEAPONS=0x5A0,
                 P_ACTIVESLOT=0x718;                                                                            // CPed
constexpr size_t V_FLAGS42E=0x42E, V_CREATEDBY=0x4A4, V_TYPE=0x590, V_SUBTYPE=0x594;                            // CVehicle
constexpr size_t I_TASKMGR=4, I_VEHENTS=0xE0, I_PEDENTS=0x130, I_STATIC=0x274;                                  // CPedIntelligence
constexpr size_t G_MEMBERS=0xC, PEDGROUP_SIZE=0x2D4, WANTED_SIZE=0x29C, W_LEVEL=0x2C;
constexpr size_t EV_TYPE=0x30, ROUTE_OFF=0x40;
constexpr uintptr_t TIMER=0xB7CB84;
constexpr uintptr_t VT_FPR=0x870078, VT_GL=0x86F8FC, VT_PP=0x8709D4, VT_SEQ=0x86E200, CV_STD=0x859DC8;
constexpr uint32 IN_VEH=0x100u, HIT_LAST=0x800u, STAY_PLACE=0x200000u, NO_IK=0x400000u;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,0x40>,NASSOC> assocs{};
alignas(16) inline std::array<uint8,0x100> object{};
alignas(16) inline std::array<uint8,0x100> routeArena{};
alignas(16) inline std::array<uint8,PEDGROUP_SIZE> group{};
alignas(16) inline std::array<uint8,WANTED_SIZE> wanted{};
alignas(16) inline std::array<uint8,0x40> playerData{};
alignas(16) inline std::array<uint8,0x40> clump{};
alignas(16) inline std::array<uint8,0x60> frame{};
alignas(16) inline std::array<uint8,0x40> fx{};
alignas(16) inline std::array<uint8,0x40> event{};
alignas(16) inline std::array<uint8,0x20> vecBuf{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline uint8* Route() { return routeArena.data()+ROUTE_OFF; }

// pools (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot)
constexpr size_t TASK_SLOT=0x80, PROUTE_SLOT=sizeof(CPointRoute), EVENT_SLOT=0x44;
inline uint8* TaskHdr() { return reinterpret_cast<uint8*>(GetTaskPool()); }
inline uint8* PRouteHdr() { return reinterpret_cast<uint8*>(GetPointRoutePool()); }
inline uint8* EventHdr() { return reinterpret_cast<uint8*>(GetEventPool()); }
inline uint8* Storage(uint8* h) { return *reinterpret_cast<uint8**>(h); }
inline uint8* States(uint8* h) { return *reinterpret_cast<uint8**>(h+4); }
inline int32 Cap(uint8* h) { return *reinterpret_cast<int32*>(h+8); }
inline int32& Last(uint8* h) { return At<int32>(h,0xC); }
inline int SlotIndex(uint8* h,size_t slot,uintptr_t v) {
    const auto s=uintptr_t(Storage(h)); if(v<s||v>=s+size_t(Cap(h))*slot||(v-s)%slot)return -1; return int((v-s)/slot);
}
inline std::vector<bool> Occ(uint8* h) { std::vector<bool> r(size_t(Cap(h))); for(size_t k=0;k<r.size();++k)r[k]=!(States(h)[k]&0x80); return r; }
inline void ResetPool(uint8* h,size_t slot,int32 last,const std::vector<uint8>& statesStart,const std::vector<bool>& occStart) {
    Last(h)=last;
    if(Occ(h)==occStart)std::memcpy(States(h),statesStart.data(),statesStart.size());
    for(int32 k=0;k<Cap(h);++k)if(States(h)[k]&0x80)std::memset(Storage(h)+k*slot,0xCD,slot);
}
inline void FreeFresh(uint8* h,const std::vector<bool>& before) { for(size_t k=0;k<before.size();++k)if(!before[k]&&!(States(h)[k]&0x80))States(h)[k]|=0x80; }

inline std::array<void*,16> fakeVtbl{};
inline std::array<void*,96> vehVtbl{};
inline std::array<void*,20> eventVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline uintptr_t dllStdVt{};
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(vehVtbl.data()))return "veh_vt"; if(v==uintptr_t(eventVtbl.data()))return "event_vt";
    if(v==CV_STD||v==dllStdVt)return "std_calc_vt";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n);
                              if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NASSOC;++n)if(auto s=in(assocs[n].data(),assocs[n].size(),"assoc"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(routeArena.data(),routeArena.size(),"route"))return *s;
    if(auto s=in(group.data(),group.size(),"group"))return *s; if(auto s=in(wanted.data(),wanted.size(),"wanted"))return *s;
    if(auto s=in(playerData.data(),playerData.size(),"pdata"))return *s; if(auto s=in(clump.data(),clump.size(),"clump"))return *s;
    if(auto s=in(frame.data(),frame.size(),"frame"))return *s; if(auto s=in(fx.data(),fx.size(),"fx"))return *s;
    if(auto s=in(event.data(),event.size(),"event"))return *s; if(auto s=in(vecBuf.data(),vecBuf.size(),"vec"))return *s;
    if(const int k=SlotIndex(PRouteHdr(),PROUTE_SLOT,v);k>=0)return "proute#"+std::to_string(k);
    { const auto es=uintptr_t(Storage(EventHdr())); if(v>=es&&v<es+size_t(Cap(EventHdr()))*EVENT_SLOT)return "evpool+"+std::to_string((v-es)%EVENT_SLOT); }
    if(SlotIndex(TaskHdr(),TASK_SLOT,v)>=0)return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    // locales de la pila (eventos y vectores temporales): no comparables
    { const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); if(v>=uintptr_t(tib->StackLimit)&&v<uintptr_t(tib->StackBase))return "stack"; }
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); for(auto& v:vehs)r.push_back(RefList(v.data())); return r; }
inline json SerTask(const void* p) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType();
    return {{"type",type},{"words",Region(p,TASK_SLOT,true)}};
}
// eventos reales (de la pila o del pool): tipo por la vtable y solo los campos (sin relleno)
inline json SerEvent(const CEvent* e) {
    if(!e)return nullptr;
    const int type=const_cast<CEvent*>(e)->GetEventType();
    json r={type,At<int32>(e,4),At<uint8>(e,8)};
    switch(type){
    case EVENT_GROUP_EVENT: r.push_back(Name(At<uint32>(e,0xC))); r.push_back(SerEvent(At<const CEvent*>(e,0x10))); break;
    case EVENT_LEAN_ON_VEHICLE: r.push_back(Name(At<uint32>(e,0xC))); r.push_back(At<int32>(e,0x10)); break;
    case EVENT_SCRIPT_COMMAND: r.push_back(At<int32>(e,0xC)); r.push_back(At<const CTask*>(e,0x10) ? SerTask(At<const CTask*>(e,0x10)) : json(nullptr)); r.push_back(At<uint8>(e,0x14)); break;
    case EVENT_CREATE_PARTNER_TASK: r.push_back(At<int32>(e,0xC)); r.push_back(Name(At<uint32>(e,0x10))); r.push_back(At<uint8>(e,0x14)); r.push_back(At<uint32>(e,0x18)); break;
    case EVENT_PASS_OBJECT: r.push_back(Name(At<uint32>(e,0xC))); r.push_back(At<uint8>(e,0x10)); break;
    case EVENT_VEHICLE_TO_STEAL: r.push_back(Name(At<uint32>(e,0xC))); break;
    default: break;
    }
    return r;
}

// buffers de fixtures cuyos cambios se comparan (palabra a palabra, traducidas)
struct Buf { std::string name; uint8* p; size_t n; };
inline std::vector<Buf> Buffers() {
    std::vector<Buf> b;
    for(int n=0;n<NPEDS;++n){ b.push_back({"ped#"+std::to_string(n),peds[n].data(),peds[n].size()}); b.push_back({"pmat#"+std::to_string(n),pedMats[n].data(),pedMats[n].size()});
                              b.push_back({"intel#"+std::to_string(n),intels[n].data(),intels[n].size()}); }
    for(int n=0;n<NVEH;++n){ b.push_back({"veh#"+std::to_string(n),vehs[n].data(),vehs[n].size()}); b.push_back({"vmat#"+std::to_string(n),vehMats[n].data(),vehMats[n].size()}); }
    for(int n=0;n<FAKES;++n)b.push_back({"fk#"+std::to_string(n),fakes[n].data(),fakes[n].size()});
    b.push_back({"route",routeArena.data(),routeArena.size()}); b.push_back({"group",group.data(),group.size()});
    b.push_back({"wanted",wanted.data(),wanted.size()}); b.push_back({"pdata",playerData.data(),playerData.size()});
    b.push_back({"event",event.data(),event.size()}); b.push_back({"vec",vecBuf.data(),vecBuf.size()});
    return b;
}
inline std::vector<std::vector<uint8>> Snapshot() { std::vector<std::vector<uint8>> s; for(auto& b:Buffers())s.push_back(ReversingProbe::Bytes(b.p,b.n)); return s; }
inline json Changes(const std::vector<std::vector<uint8>>& before) {
    json r=json::array(); const auto bufs=Buffers();
    for(size_t k=0;k<bufs.size();++k)for(size_t w=0;w+4<=bufs[k].n;w+=4)
        if(std::memcmp(bufs[k].p+w,before[k].data()+w,4))r.push_back({bufs[k].name,w,Name(At<uint32>(bufs[k].p,w))});
    return r;
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{};
inline uint32 mPursuitCop{}, cPursuitCop{}, mInPursuit{}, cInPursuit{}, mNoPolice{}, cNoPolice{}, mBlocking{}, cBlocking{},
              mBusy{}, cBusy{}, mLooking{}, cLooking{}, mHandSignal{}, cHandSignal{}, mAssoc{}, cAssoc{}, mHolding{}, cHolding{}, distCalls{};
inline uintptr_t holdingRet{}, closestRet{}, pedsGroupRet{}, fxRet{}; inline float closestDist{}; inline std::array<float,4> pathDist{};

// tareas falsas: GetTaskType trazado; MakeAbortable segun una mascara por llamada (contador en el propio buffer)
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(t)}); return At<CTask*>(t,FK_CLONE); }
inline CTask* __fastcall FSub(CTask* t,void*) { return At<CTask*>(t,FK_SUB); }
inline bool __fastcall FSimple(CTask*,void*) { return false; }
inline int __fastcall FType(CTask* t,void*) { trace.push_back({"type",Name(t)}); return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) {
    const uint32 k=At<uint32>(t,FK_CNT)++; const bool r=(At<uint32>(t,FK_ABORT)>>(k&31))&1;
    trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e),r}); return r;
}
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<size_t S,int... N> void FillTraps(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
inline int32 __fastcall EType(CEvent* e,void*) { trace.push_back({"event_type",Name(e)}); return At<int32>(e,EV_TYPE); }

inline bool Bit(uint32 mask,uint32& calls) { return (mask>>(calls++&31))&1; }
// dependencias simuladas
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { return CWorld::Players[id<0 ? CWorld::PlayerInFocus : id].m_pPed; }
inline CWanted* __cdecl FakeFindPlayerWanted(int32 id) { trace.push_back({"find_player_wanted",id}); return reinterpret_cast<CWanted*>(wanted.data()); }
inline void __fastcall FakeRemovePursuitCop(void* w,void*,CCopPed* c) { trace.push_back({"remove_pursuit_cop",Name(w),Name(c)}); }
inline bool __fastcall FakeSetPursuitCop(void* w,void*,CCopPed* c) { const bool r=Bit(mPursuitCop,cPursuitCop); trace.push_back({"set_pursuit_cop",Name(w),Name(c),r}); return r; }
inline void __fastcall FakeRemoveExcess(void* w,void*) { trace.push_back({"remove_excess_pursuit_cops",Name(w)}); }
inline bool __fastcall FakeIsInPursuit(void* w,void*,CCopPed* c) { const bool r=Bit(mInPursuit,cInPursuit); trace.push_back({"is_in_pursuit",Name(w),Name(c),r}); return r; }
inline bool __cdecl FakeNoPolice() { const bool r=Bit(mNoPolice,cNoPolice); trace.push_back({"no_police",r}); return r; }
inline void __fastcall FakeSetCurrentWeapon(CPed* p,void*,int32 type) { trace.push_back({"set_current_weapon",Name(p),type}); }
inline int16 __fastcall FakeSay(CPed* p,void*,int ctx,uint32 delay,float prob,uint32 overrideSilence,uint32 forceAudible,uint32 frontEnd) {
    trace.push_back({"say",Name(p),int16(ctx),delay,F(prob),overrideSilence&0xFF,forceAudible&0xFF,frontEnd&0xFF}); return -1;
}
inline bool __cdecl FakeBlocking(CEntity* e,const CVector* pt,float r) { const bool b=Bit(mBlocking,cBlocking); trace.push_back({"is_entity_blocking_target",Name(e),Name(pt),Vec(*pt),F(r),b}); return b; }
inline bool __cdecl FakeIsVeryBusy() { const bool b=Bit(mBusy,cBusy); trace.push_back({"is_very_busy",b}); return b; }
inline void __cdecl FakeAddBlockRef(int32 i) { trace.push_back({"add_anim_block_ref",i}); }
inline void __cdecl FakeRemoveBlockRef(int32 i) { trace.push_back({"remove_anim_block_ref",i}); }
inline void __cdecl FakeRequestModel(int32 m,int32 f) { trace.push_back({"request_model",m,f}); }
inline float __fastcall FakePathDist(void* t,void*,CPed* p) { const float d=pathDist[distCalls++%pathDist.size()]; trace.push_back({"path_dist",Name(t),Name(p),F(d)}); return d; }
inline void __fastcall FakeSetAllocator(void* i,void*,int32 t) { trace.push_back({"set_default_allocator",Name(i),t}); }
inline FxSystem_c* __fastcall FakeCreateFx(void* m,void*,const char* nm,const CVector* p,RwMatrix* mat,uint32 ignore) {
    std::string s; for(int k=0;k<16&&nm&&nm[k];++k)s+=nm[k];
    trace.push_back({"create_fx",s,Vec(*p),Name(mat),ignore&0xFF,Name(fxRet)}); return reinterpret_cast<FxSystem_c*>(fxRet);
}
inline void __fastcall FakeAttachToBone(void* f,void*,CEntity* e,int32 bone) { trace.push_back({"attach_to_bone",Name(f),Name(e),bone}); }
inline void __fastcall FakePlayAndKill(void* f,void*) { trace.push_back({"play_and_kill",Name(f)}); }
inline bool __fastcall FakeIsLooking(void* m,void*,CPed* p) { const bool r=Bit(mLooking,cLooking); trace.push_back({"is_looking",Name(p),r}); return r; }
inline void __fastcall FakeLookAt(void* m,void*,const char* purpose,CPed* p,CEntity* e,int32 time,int32 bone,CVector* off,uint32 torso,float speed,int32 blend,int32 prio,uint32 force) {
    std::string s; for(int k=0;k<32&&purpose&&purpose[k];++k)s+=purpose[k];
    trace.push_back({"look_at",s,Name(p),Name(e),time,bone,Name(off),torso&0xFF,F(speed),blend,prio,force&0xFF});
}
inline void __fastcall FakeAbortLookAt(void* m,void*,CPed* p,uint32 blend) { trace.push_back({"abort_look_at",Name(p),blend}); }
inline CEntity* __fastcall FakeHolding(CPed* p,void*) { const bool b=Bit(mHolding,cHolding); trace.push_back({"holding",Name(p),b}); return b ? reinterpret_cast<CEntity*>(holdingRet) : nullptr; }
inline bool __fastcall FakeHandSignal(CPed* p,void*) { const bool b=Bit(mHandSignal,cHandSignal); trace.push_back({"hand_signal",Name(p),b}); return b; }
inline CAnimBlendAssociation* __cdecl FakeGetAssoc(RpClump* c,uint32 id) {
    const uint32 k=cAssoc; const bool b=Bit(mAssoc,cAssoc); auto* a=b ? assocs[k%NASSOC].data() : nullptr;
    trace.push_back({"get_association",Name(c),id,Name(a)}); return reinterpret_cast<CAnimBlendAssociation*>(a);
}
inline void __fastcall FakeSetSecondaryExe(void* mgr,void*,CTask* t,int32 idx) { trace.push_back({"set_task_secondary",Name(mgr),Name(t),idx}); }
inline void __fastcall FakeChangeInSlotDll(void* mgr,void*,CTask** slot,CTask* t) {
    trace.push_back({"set_task_secondary",Name(mgr),Name(t),int32((uintptr_t(slot)-(uintptr_t(mgr)+0x14))/4)});
}
inline CPed* __fastcall FakeClosestGroupPed(void* g,void*,CPed* p,float* out) {
    trace.push_back({"closest_group_ped",Name(g),Name(p),Name(closestRet)}); if(closestRet&&out)*out=closestDist; return reinterpret_cast<CPed*>(closestRet);
}
inline bool __fastcall FakeAddGroupEvent(void* i,void*,CEvent* e) { trace.push_back({"group_add_event",Name(i),SerEvent(e)}); return true; }
inline CEvent* __fastcall FakeEventAdd(void* g,void*,CEvent* e,uint32 valid) { trace.push_back({"event_group_add",Name(g),SerEvent(e),valid&0xFF}); return e; }
inline CPedGroup* __cdecl FakePedsGroup(const CPed* p) { trace.push_back({"get_peds_group",Name(p),Name(pedsGroupRet)}); return reinterpret_cast<CPedGroup*>(pedsGroupRet); }
inline void __fastcall FakeAddFollower(void* m,void*,CPed* p) { trace.push_back({"add_follower",Name(m),Name(p)}); }
inline void __fastcall FakeGroupProcess(void* g,void*) { trace.push_back({"group_process",Name(g)}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

// ChangeTaskInSlot es protegido: se toma su direccion desde una clase derivada
struct TaskManagerAccess : CTaskManager { static uintptr_t ChangeInSlot() { return MemberAddress(&TaskManagerAccess::ChangeTaskInSlot); } };

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=51;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexFollowPointRoute","CalcBlendRatio",0x66E710,0,-1,512},
    {"CTaskComplexFollowPointRoute","CalcGoToTaskType",0x66E150,0,-1,512},
    {"CTaskComplexFollowPointRoute","Clone",0,0x870078,1,128},
    {"CTaskComplexFollowPointRoute","Constructor",0x671510,0,-1,256},
    {"CTaskComplexFollowPointRoute","ControlSubTask",0,0x870078,10,1024},
    {"CTaskComplexFollowPointRoute","CreateFirstSubTask",0,0x870078,9,1024},
    {"CTaskComplexFollowPointRoute","CreateNextSubTask",0,0x870078,8,1024},
    {"CTaskComplexFollowPointRoute","CreateTask",0x669090,0,-1,512},
    {"CTaskComplexFollowPointRoute","GetLastWaypoint",0x669340,0,-1,256},
    {"CTaskComplexFollowPointRoute","GetNextWaypoint",0x669420,0,-1,256},
    {"CTaskComplexFollowPointRoute","GetSubTaskType",0x668FE0,0,-1,256},
    {"CTaskComplexFollowPointRoute","GetTaskType",0,0x870078,4,64},
    {"CTaskComplexFollowPointRoute","MakeAbortable",0,0x870078,6,512},
    {"CTaskComplexGangLeader","Clone",0,0x86F8FC,1,128},
    {"CTaskComplexGangLeader","Constructor",0x65DED0,0,-1,128},
    {"CTaskComplexGangLeader","ControlSubTask",0,0x86F8FC,10,1024},
    {"CTaskComplexGangLeader","CreateFirstSubTask",0,0x86F8FC,9,256},
    {"CTaskComplexGangLeader","CreateNextSubTask",0,0x86F8FC,8,512},
    {"CTaskComplexGangLeader","Destructor",0x65DF30,0,-1,128},
    {"CTaskComplexGangLeader","DoGangAbuseSpeech",0x65E860,0,-1,256},
    {"CTaskComplexGangLeader","GetRandomGangAmbientAnim",0x65E730,0,-1,256},
    {"CTaskComplexGangLeader","MakeAbortable",0,0x86F8FC,6,128},
    {"CTaskComplexGangLeader","ScanForStuff",0,0x86F8FC,11,1024},
    {"CTaskComplexGangLeader","ShouldLoadGangAnims",0x65E7F0,0,-1,128},
    {"CTaskComplexGangLeader","TryToPassObject",0x65EA50,0,-1,128},
    {"CTaskComplexPolicePursuit","ClearPursuit",0x68BD90,0,-1,64},
    {"CTaskComplexPolicePursuit","Clone",0,0x8709D4,1,64},
    {"CTaskComplexPolicePursuit","Constructor",0x68BA70,0,-1,64},
    {"CTaskComplexPolicePursuit","ControlSubTask",0,0x8709D4,10,1024},
    {"CTaskComplexPolicePursuit","CreateFirstSubTask",0,0x8709D4,9,256},
    {"CTaskComplexPolicePursuit","CreateNextSubTask",0,0x8709D4,8,64},
    {"CTaskComplexPolicePursuit","CreateSubTask",0x68D910,0,-1,256},
    {"CTaskComplexPolicePursuit","Destructor",0x68D880,0,-1,128},
    {"CTaskComplexPolicePursuit","GetTaskType",0,0x8709D4,4,64},
    {"CTaskComplexPolicePursuit","MakeAbortable",0,0x8709D4,6,128},
    {"CTaskComplexPolicePursuit","PersistPursuit",0x68BDC0,0,-1,512},
    {"CTaskComplexPolicePursuit","SetPursuit",0x68BBD0,0,-1,512},
    {"CTaskComplexPolicePursuit","SetWeapon",0x68BAD0,0,-1,512},
    {"CTaskComplexSequence","AddTask-0",0x632D10,0,-1,128},
    {"CTaskComplexSequence","AddTask-1",0x632D50,0,-1,128},
    {"CTaskComplexSequence","Clone",0,0x86E200,1,128},
    {"CTaskComplexSequence","Constructor",0x632BD0,0,-1,64},
    {"CTaskComplexSequence","Contains",0x41BF10,0,-1,128},
    {"CTaskComplexSequence","ControlSubTask",0,0x86E200,10,64},
    {"CTaskComplexSequence","CreateFirstSubTask",0,0x86E200,9,128},
    {"CTaskComplexSequence","CreateNextSubTask-Internal",0x632C70,0,-1,512},
    {"CTaskComplexSequence","CreateNextSubTask-V",0,0x86E200,8,512},
    {"CTaskComplexSequence","DeRegister",0x636BC0,0,-1,128},
    {"CTaskComplexSequence","Flush",0x632C10,0,-1,128},
    {"CTaskComplexSequence","MakeAbortable",0,0x86E200,6,128},
    {"CTaskComplexSequence","SetCanBeEmptied",0x463610,0,-1,128},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks13-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks13-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks13");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks13-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    { CEntitySeekPosCalculatorStandard a{}; dllStdVt=*reinterpret_cast<const uintptr_t*>(&a); }
    // modelo de vehiculo con colision cargada (CreateSubTask(SEEK_ENTITY) lee su radio)
    int16 vehModel=-1; for(int32 m=400;m<612&&vehModel<0;++m)if(auto* mi=CModelInfo::GetModelInfo(m);mi&&mi->GetColModel())vehModel=int16(m);
    if(vehModel<0){Write({{"event","error"},{"reason","no vehicle model with collision"}});return;}
    // MI_GANG_DRINK / MI_GANG_SMOKE (words del EXE)
    const int16 miDrink=At<int16>(reinterpret_cast<void*>(0x8CD728),0), miSmoke=At<int16>(reinterpret_cast<void*>(0x8CD72C),0);
    FillTraps(fakeVtbl,std::make_integer_sequence<int,16>{}); FillTraps(vehVtbl,std::make_integer_sequence<int,96>{}); FillTraps(eventVtbl,std::make_integer_sequence<int,20>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    eventVtbl[1]=reinterpret_cast<void*>(&EType);
    const auto occTaskStart=Occ(TaskHdr()), occPRStart=Occ(PRouteHdr()), occEvStart=Occ(EventHdr());
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    const int32 lt0=Last(TaskHdr()), lp0=Last(PRouteHdr()), le0=Last(EventHdr());
    const std::vector<uint8> stTask(States(TaskHdr()),States(TaskHdr())+Cap(TaskHdr()));
    const std::vector<uint8> stPR(States(PRouteHdr()),States(PRouteHdr())+Cap(PRouteHdr()));
    const std::vector<uint8> stEv(States(EventHdr()),States(EventHdr())+Cap(EventHdr()));
    auto resetPools=[&]{ ResetPool(TaskHdr(),TASK_SLOT,lt0,stTask,occTaskStart); ResetPool(PRouteHdr(),PROUTE_SLOT,lp0,stPR,occPRStart); ResetPool(EventHdr(),EVENT_SLOT,le0,stEv,occEvStart); };
    // estado global tocado: el reloj y los peatones de los jugadores 0 y 1; se restauran en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{uintptr_t(&CWorld::Players[0].m_pPed),4},{uintptr_t(&CWorld::Players[1].m_pPed),4}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    const uint8 focus=CWorld::PlayerInFocus;
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(Body(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber)),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch p1(Body(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed))),FakeFindPlayerPed), p1e(0x56E210,FakeFindPlayerPed);
        EntryPatch p2(Body(bit_cast<uintptr_t>(static_cast<CWanted*(*)(int32)>(&FindPlayerWanted))),FakeFindPlayerWanted), p2e(0x56E230,FakeFindPlayerWanted);
        EntryPatch w1(Body(MemberAddress(static_cast<void(CWanted::*)(CCopPed*)>(&CWanted::RemovePursuitCop))),FakeRemovePursuitCop), w1e(0x562C10,FakeRemovePursuitCop);
        EntryPatch w2(Body(MemberAddress(&CWanted::SetPursuitCop)),FakeSetPursuitCop), w2e(0x563060,FakeSetPursuitCop);
        EntryPatch w3(Body(MemberAddress(&CWanted::RemoveExcessPursuitCops)),FakeRemoveExcess), w3e(0x562C40,FakeRemoveExcess);
        EntryPatch w4(Body(MemberAddress(&CWanted::IsInPursuit)),FakeIsInPursuit), w4e(0x562330,FakeIsInPursuit);
        EntryPatch c1(Body(bit_cast<uintptr_t>(&CCullZones::NoPolice)),FakeNoPolice), c1e(0x72DD50,FakeNoPolice);
        EntryPatch s1(Body(MemberAddress(static_cast<void(CPed::*)(eWeaponType)>(&CPed::SetCurrentWeapon))),FakeSetCurrentWeapon), s1e(0x5E6280,FakeSetCurrentWeapon);
        EntryPatch s2(Body(MemberAddress(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say))),FakeSay), s2e(0x5EFFE0,FakeSay);
        EntryPatch g1(Body(bit_cast<uintptr_t>(&CPedGeometryAnalyser::IsEntityBlockingTarget)),FakeBlocking), g1e(0x5F3970,FakeBlocking);
        EntryPatch t1(Body(bit_cast<uintptr_t>(&CStreaming::IsVeryBusy)),FakeIsVeryBusy), t1e(0x4076A0,FakeIsVeryBusy);
        EntryPatch t2(Body(bit_cast<uintptr_t>(&CStreaming::RequestModel)),FakeRequestModel), t2e(0x4087E0,FakeRequestModel);
        EntryPatch a1(Body(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef)),FakeAddBlockRef), a1e(0x4D3FB0,FakeAddBlockRef);
        EntryPatch a2(Body(bit_cast<uintptr_t>(&CAnimManager::RemoveAnimBlockRef)),FakeRemoveBlockRef), a2e(0x4D3FD0,FakeRemoveBlockRef);
        EntryPatch a3(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation))),FakeGetAssoc), a3e(0x4D68B0,FakeGetAssoc);
        EntryPatch d1(Body(MemberAddress(&CTaskComplexWander::GetDistSqOfClosestPathNodeToPed)),FakePathDist), d1e(0x669FF0,FakePathDist);
        EntryPatch d2(Body(MemberAddress(&CPedGroupIntelligence::SetDefaultTaskAllocatorType)),FakeSetAllocator), d2e(0x5FBB70,FakeSetAllocator);
        EntryPatch f1(Body(MemberAddress(static_cast<FxSystem_c*(FxManager_c::*)(const char*,const CVector&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem))),FakeCreateFx), f1e(0x4A9BE0,FakeCreateFx);
        EntryPatch f2(Body(MemberAddress(&FxSystem_c::AttachToBone)),FakeAttachToBone), f2e(0x4AA400,FakeAttachToBone);
        EntryPatch f3(Body(MemberAddress(&FxSystem_c::PlayAndKill)),FakePlayAndKill), f3e(0x4AA3D0,FakePlayAndKill);
        EntryPatch k1(Body(MemberAddress(&IKChainManager_c::IsLooking)),FakeIsLooking), k1e(0x6181A0,FakeIsLooking);
        EntryPatch k2(Body(MemberAddress(&IKChainManager_c::LookAt)),FakeLookAt), k2e(0x618970,FakeLookAt);
        EntryPatch k3(Body(MemberAddress(&IKChainManager_c::AbortLookAt)),FakeAbortLookAt), k3e(0x618280,FakeAbortLookAt);
        EntryPatch h1(Body(MemberAddress(&CPed::GetEntityThatThisPedIsHolding)),FakeHolding), h1e(0x5E02E0,FakeHolding);
        EntryPatch h2(Body(MemberAddress(&CPed::IsPlayingHandSignal)),FakeHandSignal), h2e(0x5E0460,FakeHandSignal);
        // el SetTaskSecondary del DLL es en linea y llama a ChangeTaskInSlot
        EntryPatch m1(Body(TaskManagerAccess::ChangeInSlot()),FakeChangeInSlotDll), m1e(0x681B60,FakeSetSecondaryExe);
        EntryPatch q1(Body(MemberAddress(&CPedGroup::GetClosestGroupPed)),FakeClosestGroupPed), q1e(0x5FACD0,FakeClosestGroupPed);
        EntryPatch q2(Body(MemberAddress(static_cast<bool(CPedGroupIntelligence::*)(CEvent*)>(&CPedGroupIntelligence::AddEvent))),FakeAddGroupEvent), q2e(0x5F7470,FakeAddGroupEvent);
        EntryPatch q3(Body(MemberAddress(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add))),FakeEventAdd), q3e(0x4AB420,FakeEventAdd);
        EntryPatch q4(Body(bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup)),FakePedsGroup), q4e(0x5F7E80,FakePedsGroup);
        EntryPatch q5(Body(MemberAddress(&CPedGroupMembership::AddFollower)),FakeAddFollower), q5e(0x5F8020,FakeAddFollower);
        EntryPatch q6(Body(MemberAddress(&CPedGroup::Process)),FakeGroupProcess), q6e(0x5FC7E0,FakeGroupProcess);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                const float qnan=std::numeric_limits<float>::quiet_NaN();
                auto run=[&](bool candidate)->json {
                    resetPools();
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1313u; if(!st)st=1; trace=json::array(); seed=uint32(i);
                    rngCalls=cPursuitCop=cInPursuit=cNoPolice=cBlocking=cBusy=cLooking=cHandSignal=cAssoc=cHolding=distCalls=0; freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    const uint32 now=At<uint32>(reinterpret_cast<void*>(TIMER),0);
                    mPursuitCop=next(); mInPursuit=next(); mNoPolice=next(); mBlocking=next(); mBusy=next(); mLooking=next();
                    mHandSignal=next(); mAssoc=next(); mHolding=next();
                    // peatones: matriz (casi siempre), inteligencia, clump, tipo, flags, armas, vehiculo, salud
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                        p[E_TYPE]=3; At<void*>(p,P_INTEL)=intels[n].data(); At<void*>(p,E_RWOBJ)=clump.data();
                        if(next()%6){ At<void*>(p,E_MATRIX)=pedMats[n].data();
                            for(int r=0;r<3;++r)At<CVector>(pedMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            At<CVector>(pedMats[n].data(),0x30)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)}; }
                        else At<CVector>(p,E_POS)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)};
                        At<uint32>(p,E_FLAGS)=next()%4 ? 0x80u : 0u; // bIsVisible
                        { static const int16 models[]{103,105,0,7,102,104,106}; At<int16>(p,E_MODEL)=models[next()%std::size(models)]; }
                        { static const int32 types[]{0,1,4,7,8,9,10,11,12,13,14,15,16,17,6}; At<int32>(p,P_PEDTYPE)=types[next()%std::size(types)]; }
                        if(next()%3==0)At<uint32>(p,P_FLAGS)|=IN_VEH; if(next()%3==0)At<uint32>(p,P_FLAGS)|=HIT_LAST; if(next()%3==0)At<uint32>(p,P_FLAGS)|=STAY_PLACE;
                        if(next()%3==0)At<uint32>(p,P_FLAGS3)|=NO_IK;
                        { const uint32 m=next()%4; At<CVehicle*>(p,P_VEH)=m==0 ? nullptr : m==3 ? Veh(1) : Veh(0); }
                        At<int32>(p,P_STATE)=next()%5==0 ? 49 : int32(next()%60); At<int32>(p,P_MOVESTATE)=int32(next()%8);
                        { const uint32 m=next()%6; At<float>(p,P_HEALTH)=m==0 ? 0.f : m==1 ? -5.f : m==2 ? qnan : uni(1.f,100.f); }
                        p[P_CREATEDBY]=uint8(next()%3);
                        At<int8>(p,P_ACTIVESLOT)=int8(next()%13); for(int w=0;w<13;++w)At<int32>(p,P_WEAPONS+0x1C*w)=next()%3 ? 0 : int32(next()%47);
                        At<int32>(intels[n].data(),I_STATIC)=int32(next()%40);
                    }
                    // jugadores: peatones 4 y 5 (datos de jugador con grupo 0..7)
                    std::memset(playerData.data(),0,playerData.size()); At<int32>(playerData.data(),0x38)=int32(next()%8);
                    At<void*>(peds[4].data(),P_PDATA)=playerData.data(); At<void*>(peds[5].data(),P_PDATA)=playerData.data();
                    At<CEntity*>(peds[4].data(),P_STANDING)=next()%3==0 ? (CEntity*)Veh(1) : nullptr;
                    CWorld::Players[0].m_pPed=reinterpret_cast<CPlayerPed*>(Ped(4)); CWorld::Players[1].m_pPed=next()%3==0 ? reinterpret_cast<CPlayerPed*>(Ped(5)) : nullptr;
                    if(!CWorld::Players[focus].m_pPed)CWorld::Players[focus].m_pPed=reinterpret_cast<CPlayerPed*>(Ped(4));
                    // vehiculos: vtable con trampas, matriz, velocidad, subtipo, estado, creador, modelo con colision
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%3==0 ? 4u : uint32(next()%8))<<3); At<void*>(v,E_MATRIX)=vehMats[n].data();
                        for(int r=0;r<3;++r)At<CVector>(vehMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)};
                        { const uint32 m=next()%4; const float s=m==0 ? 0.01f : m==1 ? 0.3f : m==2 ? 0.f : uni(0.f,0.2f); At<CVector>(v,PH_SPEED)=CVector{s*uni(-1.f,1.f),s*uni(-1.f,1.f),s*uni(-1.f,1.f)}; }
                        At<int32>(v,V_TYPE)=next()%4==0 ? 5 : 0; At<int32>(v,V_SUBTYPE)=next()%4==0 ? 5 : 0;
                        v[V_FLAGS42E]=uint8(next()%4==0); v[V_CREATEDBY]=uint8(next()%4==0 ? 2 : 1); v[0x429]=uint8(next()); // bIsBig (bit 2)
                        At<int16>(v,E_MODEL)=vehModel;
                    }
                    // tareas falsas
                    for(int n=0;n<FAKES;++n){ auto* k=fakes[n].data(); for(size_t b=0;b<FAKE_SIZE;++b)k[b]=uint8(next());
                        At<void*>(k,0)=fakeVtbl.data(); At<uint32>(k,4)=0; At<uint32>(k,8)=0;
                        for(size_t w=0xC;w<0x24;w+=4)At<float>(k,w)=uni(-20.f,20.f);
                        At<int32>(k,FK_TYPE)=TASK_SIMPLE_PAUSE; At<uint32>(k,FK_ABORT)=next(); At<uint32>(k,FK_CNT)=0; At<uint32>(k,FK_CLONE)=0; At<uint32>(k,FK_SUB)=0; }
                    for(auto& a:assocs){ for(auto& b:a)b=uint8(next()); At<float>(a.data(),0x20)=next()%5==0 ? 0.5f : uni(0.f,1.f); }
                    std::memset(clump.data(),0,clump.size()); clump[0]=2; At<void*>(clump.data(),4)=frame.data();
                    for(auto& b:frame)b=uint8(next());
                    std::memset(fx.data(),0,fx.size()); fxRet=next()%2 ? uintptr_t(fx.data()) : 0;
                    std::memset(group.data(),0,group.size()); for(int s=0;s<8;++s)At<CPed*>(group.data(),G_MEMBERS+4*s)=next()%2 ? Ped(int(next()%NPEDS)) : nullptr;
                    std::memset(wanted.data(),0,wanted.size()); { static const int32 lv[]{0,1,1,2,3,6,-1}; At<int32>(wanted.data(),W_LEVEL)=lv[next()%std::size(lv)]; }
                    for(auto& b:routeArena)b=uint8(next());
                    for(size_t w=0;w<routeArena.size();w+=4)At<float>(routeArena.data(),w)=uni(-20.f,20.f);
                    At<uint32>(Route(),0)=next()%10==0 ? 0u : uint32(1+next()%8);
                    for(auto& b:event)b=uint8(next()); At<void*>(event.data(),0)=eventVtbl.data();
                    std::memset(vecBuf.data(),0,vecBuf.size());
                    std::memset(object.data(),0xCD,object.size());
                    holdingRet=closestRet=pedsGroupRet=0; closestDist=0.f;
                    for(auto& d:pathDist){ const uint32 m=next()%4; d=m==0 ? 2.f : m==1 ? qnan : uni(2.f,30.f); }

                    auto* o=object.data(); auto* p0=Ped(0); auto* p0b=peds[0].data();
                    auto setObj=[&](uintptr_t vtbl,size_t size){ for(size_t b=0;b<size;++b)o[b]=uint8(next()); At<uintptr_t>(o,0)=vtbl; At<uint32>(o,4)=0; At<uint32>(o,8)=0; };
                    auto timer=[&](size_t off){ At<uint32>(o,off)=next()%2 ? now-next()%30000 : now; At<int32>(o,off+4)=next()%8==0 ? -1 : int32(next()%20000); o[off+8]=uint8(next()%4!=0); o[off+9]=uint8(next()%8==0); };
                    auto prio=[&]{ return int32(next()%4); };
                    auto occT=Occ(TaskHdr()), occP=Occ(PRouteHdr()), occE=Occ(EventHdr());

                    // argumentos preparados antes de la instantanea
                    int32 a0{}, a1{}; float f0{}, f1{}; uint32 b0{}, b1{}, b2{}; void* q0{}; void* q1{}; const CEvent* ev{};
                    if(cls=="CTaskComplexSequence") {
                        // +0xC indice, +0x10 tareas[8], +0x30 modo, +0x34 repeticiones, +0x38 vaciar, +0x3C usos
                        if(nm!="Constructor"){
                            setObj(VT_SEQ,0x40); At<CTask*>(o,8)=next()%4 ? Fake(9) : nullptr;
                            const uint32 used=next()%9; for(int k=0;k<8;++k)At<CTask*>(o,0x10+4*k)=k<int(used)&&next()%8 ? Fake(k) : (k<int(used) ? nullptr : (next()%6==0 ? Fake(k) : nullptr));
                            for(int k=0;k<8;++k){ static const int32 ty[]{TASK_SIMPLE_PAUSE,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_WANDER,TASK_SIMPLE_GO_TO_POINT};
                                At<int32>(fakes[k].data(),FK_TYPE)=ty[next()%4]; At<CTask*>(fakes[k].data(),FK_CLONE)=next()%4 ? Fake(8) : nullptr; }
                            { static const uint32 md[]{0,0,1,1,2,3,0xFFFFFFFFu}; At<uint32>(o,0x30)=md[next()%std::size(md)]; }
                            At<int32>(o,0xC)=int32(next()%8); At<int32>(o,0x34)=int32(next()%4); o[0x38]=uint8(next()%2); At<uint32>(o,0x3C)=next()%3 ? 0u : uint32(1+next()%3);
                            // modo con repeticion: la tarea 0 existe siempre (el EXE la clona sin comprobar al volver al principio)
                            if(At<uint32>(o,0x30)!=0&&!At<CTask*>(o,0x10))At<CTask*>(o,0x10)=Fake(0);
                            if(nm=="AddTask-0"||nm=="AddTask-1"){ q0=next()%6 ? (void*)Fake(8) : nullptr; a0=int32(next()%10); }
                            if(nm=="Contains"){ static const int32 ty[]{TASK_SIMPLE_PAUSE,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_WANDER,TASK_SIMPLE_GO_TO_POINT,TASK_NONE}; a0=ty[next()%5]; }
                            if(nm=="SetCanBeEmptied")b0=next()%2;
                            if(nm=="DeRegister")At<uint32>(o,0x3C)=next()%3 ? 1u : uint32(next()%4);
                            if(nm=="CreateNextSubTask-Internal"){ At<int32>(vecBuf.data(),0)=int32(next()%8); At<int32>(vecBuf.data(),4)=int32(next()%4); }
                            if(nm=="MakeAbortable"){ At<CTask*>(o,8)=Fake(9); a0=prio(); ev=next()%2 ? reinterpret_cast<const CEvent*>(event.data()) : nullptr; }
                        }
                    }
                    else if(cls=="CTaskComplexFollowPointRoute") {
                        // +0xC modo, +0x10 vueltas, +0x14 estado de movimiento, +0x18 radio, +0x1C frenado, +0x20 ruta, +0x24 punto,
                        // +0x28 banderas, +0x2C..+0x38 mezcla
                        auto pickMode=[&]{ static const uint32 md[]{0,1,2,3,0,1,2,3,5}; return md[next()%std::size(md)]; };
                        auto pickMove=[&]{ return int32(next()%3==0 ? next()%8 : 4+next()%4); };
                        if(nm=="Constructor"){
                            a0=pickMove(); a1=int32(pickMode()); f0=uni(0.f,2.f); f1=next()%3==0 ? 0.f : uni(0.f,5.f); b0=next()%2; b1=next()%2; b2=next()%2;
                            for(size_t b=0x28;b<0x2C;++b)o[b]=uint8(next()); // bits que el constructor conserva
                        } else {
                            setObj(VT_FPR,0x3C);
                            const uint32 num=At<uint32>(Route(),0);
                            At<void*>(o,4)=next()%3==0 ? (void*)Fake(1) : nullptr; At<int32>(fakes[1].data(),FK_TYPE)=next()%2 ? TASK_COMPLEX_WALK_ROUND_CAR : TASK_SIMPLE_PAUSE;
                            // padre WalkRoundCar: GoingForDoor (real, en linea en el DLL) lee +0xD bit 0, +0x1C vehiculo y su bIsBig
                            fakes[1][0xD]=uint8(next()); At<CVehicle*>(fakes[1].data(),0x1C)=next()%3 ? Veh(int(next()%2)) : nullptr;
                            At<CTask*>(o,8)=Fake(0);
                            At<uint32>(o,0xC)=pickMode(); { static const int32 tr[]{0,0,1,1,2,-1}; At<int32>(o,0x10)=tr[next()%std::size(tr)]; }
                            At<int32>(o,0x14)=pickMove(); At<float>(o,0x18)=uni(0.f,2.f); At<float>(o,0x1C)=next()%3==0 ? 0.f : uni(0.f,5.f);
                            At<void*>(o,0x20)=Route(); At<uint32>(o,0x24)=num ? uint32(next()%(num+1)) : uint32(next()%2);
                            if(At<uint32>(o,0x24)>8)At<uint32>(o,0x24)=8;
                            At<uint32>(o,0x28)=next();
                            for(size_t w=0x2C;w<0x3C;w+=4)At<float>(o,w)=next()%4==0 ? 0.f : uni(0.f,6.f);
                            // a veces el peaton cerca del punto actual (distancias dentro de los radios de mezcla)
                            if(next()%3==0&&At<void*>(p0b,E_MATRIX)){ const CVector pt=At<CVector>(Route(),4+12*(At<uint32>(o,0x24)%8));
                                At<CVector>(pedMats[0].data(),0x30)=CVector{pt.x+uni(-3.f,3.f),pt.y+uni(-3.f,3.f),pt.z+uni(-1.f,1.f)}; }
                            { static const int32 ty[]{TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT_FINE,TASK_COMPLEX_LEAVE_CAR,TASK_NONE,TASK_SIMPLE_STAND_STILL,
                                                      TASK_FINISHED,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_PAUSE};
                              At<int32>(fakes[0].data(),FK_TYPE)=ty[next()%std::size(ty)]; }
                            if(nm=="CreateTask"){ static const int32 ty[]{TASK_SIMPLE_GO_TO_POINT_FINE,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_GO_TO_POINT,TASK_NONE,
                                                                       TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_LEAVE_CAR,TASK_FINISHED,1000};
                                a0=ty[next()%std::size(ty)]; }
                            if(nm=="CalcGoToTaskType"){ static const int32 ty[]{TASK_SIMPLE_GO_TO_POINT,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_FINISHED,TASK_NONE};
                                a0=ty[next()%std::size(ty)]; At<uint32>(o,0x24)=uint32(next()%9); }
                            if(nm=="CalcBlendRatio")b0=next()%2;
                            if(nm=="MakeAbortable"){
                                a0=prio(); ev=next()%4 ? reinterpret_cast<const CEvent*>(event.data()) : nullptr;
                                { static const int32 et[]{1,56,1,56,2,0}; At<int32>(event.data(),EV_TYPE)=et[next()%std::size(et)]; }
                                At<CVehicle*>(event.data(),0x14)=next()%4 ? Veh(int(next()%2)) : nullptr;
                                { static const int32 ty[]{TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT_FINE,TASK_SIMPLE_PAUSE}; At<int32>(fakes[0].data(),FK_TYPE)=ty[next()%3]; }
                            }
                        }
                    }
                    else if(cls=="CTaskComplexGangLeader") {
                        // +0xC banda, +0x10/+0x1C/+0x28 temporizadores (deambular, explorar, exhalar), +0x34 animaciones referenciadas
                        auto setGl=[&]{ setObj(VT_GL,0x38); At<CTask*>(o,8)=next()%5 ? Fake(0) : nullptr; At<void*>(o,0xC)=group.data(); timer(0x10); timer(0x1C); timer(0x28); o[0x34]=uint8(next()%2); };
                        if(nm=="Constructor"){ q0=group.data(); }
                        else if(nm=="GetRandomGangAmbientAnim"){
                            q0=Ped(int(next()%NPEDS)); const uint32 m=next()%4; q1=m==0 ? nullptr : (void*)Veh(1);
                            At<int16>(vehs[1].data(),E_MODEL)=m==1 ? miDrink : m==2 ? miSmoke : vehModel;
                        }
                        else if(nm=="ShouldLoadGangAnims"){ CWorld::Players[focus].m_pPed=reinterpret_cast<CPlayerPed*>(Ped(4)); }
                        else if(nm=="DoGangAbuseSpeech"){ q0=Ped(int(next()%NPEDS)); q1=Ped(int(next()%NPEDS)); }
                        else if(nm=="TryToPassObject"){
                            q0=Ped(int(next()%NPEDS)); q1=group.data(); closestRet=next()%4 ? uintptr_t(Ped(int(1+next()%3))) : 0;
                            { const uint32 m=next()%5; closestDist=m==0 ? 4.f : m==1 ? qnan : m==2 ? std::nextafter(4.f,0.f) : uni(0.f,8.f); }
                        }
                        else {
                            setGl();
                            if(nm=="MakeAbortable"){ a0=prio(); ev=nullptr; }
                            if(nm=="CreateNextSubTask"||nm=="CreateFirstSubTask"){
                                static const int32 ty[]{TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_HANDSIGNAL_ANIM,TASK_SIMPLE_PAUSE}; At<int32>(fakes[0].data(),FK_TYPE)=ty[next()%3];
                                if(next()%3==0)for(int s=0;s<8;++s)At<CPed*>(group.data(),G_MEMBERS+4*s)=next()%4 ? Ped(int(next()%NPEDS)) : nullptr;
                            }
                            if(nm=="ScanForStuff"){
                                // exploracion: vehiculos y peatones del escaner del peaton 0 (algunos dentro de los limites)
                                auto* in=intels[0].data();
                                // un solo vehiculo en el escaner: con FIX_BUGS el DLL para tras el primer evento (el EXE sigue)
                                if(next()%3)At<CVehicle*>(in,I_VEHENTS+4*int(next()%16))=Veh(int(next()%2));
                                for(int s=0;s<16;++s)At<CPed*>(in,I_PEDENTS+4*s)=next()%3 ? nullptr : Ped(int(1+next()%3));
                                for(int n=0;n<NVEH;++n){ if(next()%2){ const CVector pp=p0->GetPosition(); At<CVector>(vehMats[n].data(),0x30)=CVector{pp.x+uni(-12.f,12.f),pp.y+uni(-12.f,12.f),pp.z+uni(-6.f,6.f)}; }
                                    if(next()%2){ vehs[n][E_TYPE]=uint8(2|4<<3); At<int32>(vehs[n].data(),V_SUBTYPE)=0; vehs[n][V_FLAGS42E]=0; vehs[n][V_CREATEDBY]=1; } }
                                for(int n=1;n<4;++n){ if(next()%2){ peds[n][P_CREATEDBY]=1; At<int32>(peds[n].data(),P_PEDTYPE)=At<int32>(p0b,P_PEDTYPE); }
                                    if(next()%2){ const CVector pp=p0->GetPosition(); auto& pos=At<void*>(peds[n].data(),E_MATRIX) ? At<CVector>(pedMats[n].data(),0x30) : At<CVector>(peds[n].data(),E_POS);
                                        pos=CVector{pp.x+uni(-12.f,12.f),pp.y+uni(-12.f,12.f),pp.z}; } }
                                { const uint32 m=next()%3; pedsGroupRet=m==0 ? 0 : m==1 ? uintptr_t(group.data()) : uintptr_t(wanted.data()); }
                                for(int s=0;s<8;++s)At<CPed*>(group.data(),G_MEMBERS+4*s)=next()%2 ? Ped(int(next()%NPEDS)) : nullptr;
                                // temporizador de exploracion: sin arrancar, cumplido o no
                                { const uint32 m=next()%3; if(m==0)o[0x24]=0; else if(m==1){ o[0x24]=1; At<uint32>(o,0x1C)=now-1000; At<int32>(o,0x20)=500; } }
                            }
                            if(nm=="ControlSubTask"){
                                // exploracion en marcha y sin cumplir (ScanForStuff tiene sus propios casos)
                                At<uint32>(o,0x1C)=now; At<int32>(o,0x20)=100000; o[0x24]=1; o[0x25]=0;
                                CWorld::Players[focus].m_pPed=reinterpret_cast<CPlayerPed*>(Ped(4));
                                { static const int32 ty[]{TASK_COMPLEX_WANDER,TASK_COMPLEX_WANDER,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_PAUSE}; At<int32>(fakes[0].data(),FK_TYPE)=ty[next()%4]; }
                                At<CTask*>(o,8)=Fake(0);
                                if(next()%2){ At<uint32>(o,0x10)=now-5000; At<int32>(o,0x14)=1000; o[0x18]=1; o[0x19]=0; }
                                if(next()%2){ At<uint32>(o,0x28)=now-5000; At<int32>(o,0x2C)=1000; o[0x30]=1; o[0x31]=0; }
                                if(next()%5==0)At<void*>(p0b,E_RWOBJ)=nullptr;
                                At<int32>(p0b,P_MOVESTATE)=next()%4==0 ? 6+int32(next()%2) : 4;
                                if(next()%3)o[0x34]=1;
                                holdingRet=uintptr_t(Veh(1)); { const uint32 m=next()%3; At<int16>(vehs[1].data(),E_MODEL)=m==0 ? miDrink : m==1 ? miSmoke : vehModel; }
                                closestRet=next()%3 ? uintptr_t(Ped(int(1+next()%3))) : 0; closestDist=uni(0.f,6.f);
                                mHolding=next()%2 ? 0xAAAAAAA9u : next();
                                // semilla con la que el sorteo de 0..500 cae en 200 (pasar objeto) o en 51..55/100 (animaciones)
                                if(next()%3==0)seed=uint32(i*7+3);
                            }
                            if(nm=="Destructor")o[0x34]=uint8(next()%2);
                        }
                    }
                    else if(cls=="CTaskComplexPolicePursuit") {
                        // +0xC banderas (bit0 control de carretera, bit1 zona sin policia, bit2 podria unirse), +0x10 perseguidor, +0x14 perseguido
                        auto setPp=[&]{ setObj(VT_PP,0x18); At<CTask*>(o,8)=next()%5 ? Fake(0) : nullptr; o[0xC]=uint8(next());
                            At<CPed*>(o,0x10)=next()%3 ? p0 : nullptr; { const uint32 m=next()%3; At<CPed*>(o,0x14)=m==0 ? nullptr : m==1 ? Ped(4) : Ped(1); } Reg(At<CPed*>(o,0x14),o+0x14); };
                        if(nm!="Constructor"&&nm!="ClearPursuit"&&nm!="SetWeapon")setPp();
                        if(nm=="ClearPursuit"){ q0=p0; if(next()%3==0)CWorld::Players[focus].m_pPed=nullptr; }
                        if(nm=="Destructor"){ if(next()%3==0)CWorld::Players[focus].m_pPed=nullptr; }
                        if(nm=="MakeAbortable"){ At<CTask*>(o,8)=Fake(0); a0=prio(); ev=next()%2 ? reinterpret_cast<const CEvent*>(event.data()) : nullptr; }
                        if(nm=="SetWeapon"){ q0=p0; At<int32>(peds[4].data(),P_WEAPONS+0x1C*At<int8>(peds[4].data(),P_ACTIVESLOT))=int32(next()%47);
                            if(next()%2)At<int32>(p0b,P_WEAPONS+0x1C*1)=3; if(next()%2)At<int32>(p0b,P_WEAPONS+0x1C*3)=25; }
                        if(nm=="SetPursuit"||nm=="CreateFirstSubTask"){
                            CWorld::Players[1].m_pPed=next()%2 ? reinterpret_cast<CPlayerPed*>(Ped(5)) : nullptr; if(next()%6==0)CWorld::Players[0].m_pPed=nullptr;
                            for(int n=4;n<6;++n)At<CVehicle*>(peds[n].data(),P_VEH)=Veh(n-4);
                        }
                        if(nm=="CreateSubTask"){ static const int32 ty[]{TASK_COMPLEX_ARREST_PED,TASK_COMPLEX_SEEK_ENTITY,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_SCRATCH_HEAD,TASK_FINISHED,1000};
                            a0=ty[next()%std::size(ty)]; if(a0==TASK_COMPLEX_SEEK_ENTITY)At<CVehicle*>(p0b,P_VEH)=Veh(int(next()%2)); }
                        if(nm=="ControlSubTask"){
                            { static const int32 ty[]{TASK_COMPLEX_ARREST_PED,TASK_COMPLEX_ARREST_PED,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_PAUSE}; At<int32>(fakes[0].data(),FK_TYPE)=ty[next()%4]; }
                            At<CTask*>(o,8)=Fake(0);
                            if(next()%2){ auto* v=At<CVehicle*>(p0b,P_VEH); if(v){ const CVector pp=p0->GetPosition(); At<CVector>(vehMats[v==Veh(1)].data(),0x30)=CVector{pp.x+uni(-30.f,30.f),pp.y+uni(-30.f,30.f),pp.z}; } }
                            if(next()%2)At<int32>(p0b,P_WEAPONS+0x1C*At<int8>(p0b,P_ACTIVESLOT))=22;
                        }
                    }
                    const auto before=Snapshot();

                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches}}; json ret=nullptr; size_t objSize=0x40;
                    // retornos puntero: se traducen despues de nombrar las tareas nuevas
                    uintptr_t retPtr{}; bool isPtr=false; auto Ptr=[&](const void* p)->json{ retPtr=uintptr_t(p); isPtr=true; return nullptr; };
                    auto callTask=[&](CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    auto callVoid=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); };
                    auto callAbort=[&]{ return reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,p0,a0,ev); };
                    if(nm=="GetTaskType"){ ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o); }
                    else if(nm=="Clone"){ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o)); }
                    else if(nm=="MakeAbortable"){ ret=callAbort(); }
                    else if(nm=="ControlSubTask"||nm=="CreateFirstSubTask"||nm=="CreateNextSubTask"||nm=="CreateNextSubTask-V"){ ret=Ptr(callTask(p0)); }
                    else if(cls=="CTaskComplexSequence"){
                        if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o));
                        else if(nm=="AddTask-0")ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(target)(o,q0);
                        else if(nm=="AddTask-1")reinterpret_cast<void(__thiscall*)(void*,int32,void*)>(target)(o,a0,q0);
                        else if(nm=="Contains")ret=reinterpret_cast<bool(__thiscall*)(void*,int32)>(target)(o,a0);
                        else if(nm=="SetCanBeEmptied")reinterpret_cast<void(__thiscall*)(void*,uint32)>(target)(o,0xFFFFFF00u|b0);
                        else if(nm=="CreateNextSubTask-Internal")ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*,int32*,int32*)>(target)(o,p0,&At<int32>(vecBuf.data(),0),&At<int32>(vecBuf.data(),4)));
                        else callVoid(); // Flush, DeRegister
                    }
                    else if(cls=="CTaskComplexFollowPointRoute"){
                        objSize=0x3C;
                        if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,int32,const void*,int32,float,float,uint32,uint32,uint32)>(target)(o,a0,Route(),a1,f0,f1,0xFFFFFF00u|b0,0xFFFFFF00u|b1,0xFFFFFF00u|b2));
                        else if(nm=="GetSubTaskType")ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        else if(nm=="CreateTask")ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,a0,p0));
                        else if(nm=="GetLastWaypoint"||nm=="GetNextWaypoint"){ auto* r=reinterpret_cast<CVector*(__thiscall*)(void*,CVector*,CPed*)>(target)(o,&At<CVector>(vecBuf.data(),0x10),p0); ret=Ptr(r); }
                        else if(nm=="CalcGoToTaskType")ret=reinterpret_cast<int32(__thiscall*)(void*,CPed*,int32)>(target)(o,p0,a0);
                        else if(nm=="CalcBlendRatio")ret=F(reinterpret_cast<float(__thiscall*)(void*,CPed*,uint32)>(target)(o,p0,0xFFFFFF00u|b0));
                    }
                    else if(cls=="CTaskComplexGangLeader"){
                        objSize=0x38;
                        if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,void*)>(target)(o,q0));
                        else if(nm=="Destructor")callVoid();
                        else if(nm=="ScanForStuff")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,p0);
                        else if(nm=="GetRandomGangAmbientAnim")ret=reinterpret_cast<int32(__cdecl*)(void*,void*)>(target)(q0,q1);
                        else if(nm=="ShouldLoadGangAnims")ret=reinterpret_cast<bool(__cdecl*)()>(target)();
                        else if(nm=="DoGangAbuseSpeech")reinterpret_cast<void(__cdecl*)(void*,void*)>(target)(q0,q1);
                        else if(nm=="TryToPassObject")ret=Ptr(reinterpret_cast<CPed*(__cdecl*)(void*,void*)>(target)(q0,q1));
                    }
                    else if(cls=="CTaskComplexPolicePursuit"){
                        objSize=0x18;
                        if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o));
                        else if(nm=="Destructor")callVoid();
                        else if(nm=="ClearPursuit")reinterpret_cast<void(__stdcall*)(void*)>(target)(q0);
                        else if(nm=="SetWeapon")reinterpret_cast<void(__stdcall*)(void*)>(target)(q0);
                        else if(nm=="SetPursuit")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,p0);
                        else if(nm=="PersistPursuit")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,p0);
                        else if(nm=="CreateSubTask")ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,a0,p0));
                    }
                    for(auto* h:hooks)h->State(false);

                    // tareas, rutas y eventos nuevos (se nombran antes de serializar el objeto)
                    { auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                      for(size_t k=0;k<occT.size();++k)if(!occT[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                      json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t)); out["tasks"]=tasks; }
                    { json routes=json::array();
                      for(size_t k=0;k<occP.size();++k)if(!occP[k]&&!(States(PRouteHdr())[k]&0x80))routes.push_back({{"proute",k},{"words",Region(Storage(PRouteHdr())+k*PROUTE_SLOT,PROUTE_SLOT)}});
                      json events=json::array();
                      for(size_t k=0;k<occE.size();++k)if(!occE[k]&&!(States(EventHdr())[k]&0x80))events.push_back(SerEvent(reinterpret_cast<CEvent*>(Storage(EventHdr())+k*EVENT_SLOT)));
                      out["routes"]=routes; out["events"]=events; }
                    if(isPtr)ret=Name(retPtr);
                    out["object"]=Region(o,objSize,true);
                    if(nm=="Constructor")out["type"]=reinterpret_cast<CTask*>(o)->GetTaskType();
                    out["changes"]=Changes(before); out["refs"]=AllRefs();
                    // limpieza: referencias de los fixtures y slots nuevos (sin destructor); los pools deben quedar como antes
                    out["ret"]=ret; out["trace"]=trace; out["rng"]=rngCalls;
                    CleanRefs();
                    FreeFresh(TaskHdr(),occT); FreeFresh(PRouteHdr(),occP); FreeFresh(EventHdr(),occE);
                    out["clean"]=Occ(TaskHdr())==occT&&Occ(PRouteHdr())==occP&&Occ(EventHdr())==occE;
                    restoreGlobals();
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    CleanRefs(); restoreGlobals(); resetPools();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=Occ(TaskHdr())==occTaskStart&&Occ(PRouteHdr())==occPRStart&&Occ(EventHdr())==occEvStart&&freeRefs()==savedFree&&globalsIntact()
                        &&Last(TaskHdr())==lt0&&Last(PRouteHdr())==lp0&&Last(EventHdr())==le0;
    if(restored){ std::memcpy(States(TaskHdr()),stTask.data(),stTask.size()); std::memcpy(States(PRouteHdr()),stPR.data(),stPR.size()); std::memcpy(States(EventHdr()),stEv.data(),stEv.size()); }
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
