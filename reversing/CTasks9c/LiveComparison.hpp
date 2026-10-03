#pragma once
// Banco dentro del motor: tareas complejas C (lista de 9, 45 funciones).
// CTaskComplexPassObject (AbortIK, Clone, constructor, ControlSubTask, CreateFirstSubTask, CreateNextSubTask, destructor,
// GetTaskType, MakeAbortable), CTaskComplexRoadRage (Clone, constructor, ControlSubTask, CreateFirstSubTask,
// CreateNextSubTask, CreateSubTask, destructor, GetTaskType, MakeAbortable), CTaskComplexScreamInCarThenLeave (lo mismo),
// CTaskComplexSitDownThenIdleThenStandUp (lo mismo) y CTaskComplexWanderCop (constructor, ControlSubTask,
// CreateFirstSubTask, CreateNextSubTask, LookForCarAlarms, LookForCriminals, LookForStolenCopCars, ScanForStuff,
// ShouldPursuePlayer).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 5 peatones (tamano de CCopPed, inteligencia propia con escaner de peatones y gestor de tareas), 2 vehiculos (vtable falsa:
// huecos 57/58 CanPedStepOutCar/CanPedJumpOutCar), tareas falsas (vtable trazada), evento falso, datos de jugador y
// CWanted falsos y un CBaseModelInfo/CColModel falsos (se cambia temporalmente CModelInfo::ms_modelInfoPtrs[MODEL]).
// Se simulan y trazan: rand/CGeneral::GetRandomNumber, CPed::Say, IKChainManager_c::IsArmPointing/AbortPointArm/PointArm,
// CPed::GetEntityThatThisPedIsHolding/GiveDelayedWeapon/SetCurrentWeapon, CTaskManager::FindActiveTaskByType/SetTask/
// SetTaskSecondary (ChangeTaskInSlot en la DLL), CTaskSimpleHoldEntity::DropEntity, CInterestingEvents::Add,
// CTheScripts::IsPlayerOnAMission, CTaskComplexWander::GetWanderTaskByPedType y CreateFirstSubTask/CreateNextSubTask/
// ControlSubTask del padre (el EXE los llama directamente), CAnimManager::GetAnimationBlockIndex/AddAnimBlockRef/
// RemoveAnimBlockRef, CStreaming::RequestModel, FindPlayerPed/FindPlayerVehicle/FindPlayerWanted,
// CPlayerPed::SetWantedLevelNoDrop, CWanted::CanCopJoinPursuit, CWorld::GetIsLineOfSightClear y CEventGroup::Add.
// Pool de tareas: m_LastFreeSlot y estados restaurados y slots libres con 0xCD en cada ejecucion; las tareas nuevas se
// serializan y se liberan sin destructor (antes se limpian las referencias de los fixtures). Se guardan y restauran el
// reloj, el puntero de ms_modelInfoPtrs[MODEL] y los bloques de animacion usados (byte IsLoaded).
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Tasks/Task.h"
#include "Tasks/TaskManager.h"
#include "TaskComplexPassObject.h"
#include "TaskComplexRoadRage.h"
#include "TaskComplexScreamInCarThenLeave.h"
#include "TaskComplexSitDownThenIdleThenStandUp.h"
#include "TaskComplexWanderCop.h"
#include "TaskComplexWander.h"
#include "TaskSimpleHoldEntity.h"
#include "Ragdoll/IKChainManager.h"
#include "InterestingEvents.h"
#include "Events/EventGroup.h"
#include "PedIntelligence.h"
#include "Animation/AnimManager.h"
#include "Entity/Ped/CopPed.h"
#include "Wanted.h"
#include "World.h"
#include "Streaming.h"
#include "Scripts/TheScripts.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks9cLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }

constexpr int NPEDS=5, NVEH=2, FAKES=7, NBLOCKS=4;
// peaton 0: el que ejecuta la tarea (policia en WanderCop); 1..3: otros / candidatos; 4: jugador (FindPlayerPed)
constexpr int PLAYER=4;
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_TYPE=0x68, FK_CLONE=0x6C;
// desplazamientos crudos del EXE
constexpr size_t E_POS=4, E_MATRIX=0x14, E_FLAGS=0x1C, E_MODEL=0x22, E_TYPE=0x36;                     // CEntity
constexpr size_t P_FLAGS=0x46C, P_INTEL=0x47C, P_PLAYERDATA=0x480, P_VEHICLE=0x58C, P_PEDTYPE=0x598,
                 P_WEAPONS=0x5A0, P_WEAPSLOT=0x718, P_DONTPURSUIT=0x79C;                              // CPed / CCopPed
constexpr size_t V_MISSION=0x3BA, V_CRUISE=0x3D0, V_FLAGS428=0x428, V_ALARM=0x45C, V_DRIVER=0x460, V_TYPE=0x590; // CVehicle
constexpr size_t I_TM=4, W_LEVEL=0x2C;
constexpr uintptr_t TIMER=0xB7CB84, IKMAN=0xC15448, INTEREST=0xC0B058, MODELPTRS=0xA9B0C8, ANIMBLOCKS=0xB5D4A0;
constexpr int MODEL=400, BLOCK0=10;
constexpr uintptr_t VT_PASS=0x86FB18, VT_RAGE=0x86D9F0, VT_SCREAM=0x86E8D8, VT_SIT=0x86DF60, VT_COP=0x859C80;

alignas(16) inline std::array<std::array<uint8,sizeof(CCopPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<uint8,0x60> object{}, event{};
alignas(16) inline std::array<uint8,0x40> playerData{}, wanted{}, mi{}, col{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline int VehIndex(const void* v) { for(int n=0;n<NVEH;++n)if(v==vehs[n].data())return n; return -1; }

// pool de tareas (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot)
constexpr size_t TASK_SLOT=0x80;
inline uint8* TaskHdr() { return reinterpret_cast<uint8*>(GetTaskPool()); }
inline uint8* Storage(uint8* h) { return *reinterpret_cast<uint8**>(h); }
inline uint8* States(uint8* h) { return *reinterpret_cast<uint8**>(h+4); }
inline int32 Cap(uint8* h) { return *reinterpret_cast<int32*>(h+8); }
inline int32& Last(uint8* h) { return At<int32>(h,0xC); }
inline int SlotIndex(uint8* h,size_t slot,uintptr_t v) {
    const auto s=uintptr_t(Storage(h)); if(v<s||v>=s+size_t(Cap(h))*slot||(v-s)%slot)return -1; return int((v-s)/slot);
}
inline bool IsPoolTask(uintptr_t v) { const int k=SlotIndex(TaskHdr(),TASK_SLOT,v); return k>=0&&!(States(TaskHdr())[k]&0x80); }
inline void ResetPool(uint8* h,size_t slot,int32 last,const std::vector<uint8>& statesStart,const std::vector<bool>& occStart,const std::vector<bool>& occNow) {
    Last(h)=last;
    if(occNow==occStart)std::memcpy(States(h),statesStart.data(),statesStart.size());
    for(int32 k=0;k<Cap(h);++k)if(States(h)[k]&0x80)std::memset(Storage(h)+k*slot,0xCD,slot);
}

inline std::array<void*,12> fakeVtbl{};
inline std::array<void*,96> vehVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(vehVtbl.data()))return "veh_vt";
    if(v==IKMAN)return "ikman"; if(v==INTEREST)return "interesting_events";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n);
                              if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(event.data(),event.size(),"event"))return *s;
    if(auto s=in(playerData.data(),playerData.size(),"player_data"))return *s; if(auto s=in(wanted.data(),wanted.size(),"wanted"))return *s;
    if(auto s=in(mi.data(),mi.size(),"model_info"))return *s; if(auto s=in(col.data(),col.size(),"col_model"))return *s;
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); for(auto& v:vehs)r.push_back(RefList(v.data())); return r; }
inline json Vec(const CVector& v) { return {F(v.x),F(v.y),F(v.z)}; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
template<size_t N> std::string EntHash(const std::array<uint8,N>& buf) {
    auto copy=buf; const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(buf.data())->m_pReferences)-uintptr_t(buf.data());
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}
inline json AllHashes() {
    json r=json::array(); for(auto& p:peds)r.push_back(EntHash(p)); for(auto& v:vehs)r.push_back(EntHash(v));
    for(auto& i:intels)r.push_back(Hash(i.data(),i.size())); return r;
}
// tarea serializada sin vptr (se compara el tipo)
inline json SerTask(const void* p) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType();
    return {{"type",type},{"words",Region(p,TASK_SLOT,true)}};
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{}, stepMask{}, jumpMask{}, losMask{}, losCalls{};
inline int16 sayRet{}; inline bool armRet{}, missionRet{}, canJoinRet{};
inline uintptr_t holdingRet{}, findActiveRet{}, wanderRet{}, playerVehRet{};
inline int32 blockRet{};

// tareas falsas (destructor 0, Clone 1, GetTaskType 4, MakeAbortable 6)
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f&0xFF}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(t)}); return At<CTask*>(t,FK_CLONE); }
inline CTask* __fastcall FSub(CTask*,void*) { return nullptr; }
inline bool __fastcall FSimple(CTask*,void*) { return true; }
inline int __fastcall FType(CTask* t,void*) { trace.push_back({"type_fake",Name(t)}); return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) { trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e)}); return At<uint8>(t,FK_ABORT)!=0; }
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<size_t S,int... N> void FillTraps(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// vehiculo falso: hueco 57 = CanPedStepOutCar, 58 = CanPedJumpOutCar
inline bool __fastcall VStep(CVehicle* v,void*,uint32 ignore) { const int n=VehIndex(v); const bool r=n>=0&&((stepMask>>n)&1); trace.push_back({"can_ped_step_out_car",Name(v),ignore&0xFF,r}); return r; }
inline bool __fastcall VJump(CVehicle* v,void*,CPed* p) { const int n=VehIndex(v); const bool r=n>=0&&((jumpMask>>n)&1); trace.push_back({"can_ped_jump_out_car",Name(v),Name(p),r}); return r; }

// dependencias simuladas
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
// el EXE empuja el contexto con basura en los 16 bits altos: solo se traza el int16
inline int16 __fastcall FakeSay(CPed* p,void*,int ctx,uint32 delay,float prob,bool overrideSilence,bool forceAudible,bool frontEnd) {
    trace.push_back({"say",Name(p),int16(ctx),delay,F(prob),overrideSilence,forceAudible,frontEnd}); return sayRet;
}
inline bool __stdcall FakeIsArmPointing(int32 arm,CPed* p) { trace.push_back({"is_arm_pointing",arm,Name(p),armRet}); return armRet; }
inline void __stdcall FakeAbortPointArm(int32 arm,CPed* p,int32 blend) { trace.push_back({"abort_point_arm",arm,Name(p),blend}); }
inline void __fastcall FakePointArm(void* m,void*,const char* why,int32 arm,CPed* p,CEntity* e,int32 bone,CVector* off,float speed,int32 blend,float cull) {
    trace.push_back({"point_arm",Name(m),why ? std::string(why) : std::string("<null>"),arm,Name(p),Name(e),bone,Name(off),off ? Vec(*off) : json(nullptr),F(speed),blend,F(cull)});
}
inline CEntity* __fastcall FakeHolding(CPed* p,void*) { trace.push_back({"entity_ped_is_holding",Name(p),Name(holdingRet)}); return reinterpret_cast<CEntity*>(holdingRet); }
inline CTask* __fastcall FakeFindActive(void* tm,void*,int32 type) { trace.push_back({"find_active_task_by_type",Name(tm),type,Name(findActiveRet)}); return reinterpret_cast<CTask*>(findActiveRet); }
inline void __fastcall FakeDrop(void* t,void*,CPed* p,uint32 b) { trace.push_back({"drop_entity",Name(t),Name(p),b&0xFF}); }
inline void __fastcall FakeSetTask(void* tm,void*,CTask* t,int32 idx,uint32) { trace.push_back({"set_task",Name(tm),Name(t),idx}); }
inline void __fastcall FakeSetTaskSecondary(void* tm,void*,CTask* t,int32 idx) { trace.push_back({"set_task",Name(tm),Name(t),5+idx}); }
inline void __fastcall FakeChangeSlot(void* tm,void*,CTask** slot,CTask* t) { trace.push_back({"set_task",Name(tm),Name(t),int32((uintptr_t(slot)-uintptr_t(tm))/4)}); }
struct TaskManagerAccess : CTaskManager { static uintptr_t ChangeSlotAddress() { return std::bit_cast<uintptr_t>(&TaskManagerAccess::ChangeTaskInSlot); } };
inline void __fastcall FakeGiveDelayed(CPed* p,void*,int32 type,uint32 ammo) { trace.push_back({"give_delayed_weapon",Name(p),type,ammo}); }
inline void __fastcall FakeSetCurrentWeapon(CPed* p,void*,int32 type) { trace.push_back({"set_current_weapon",Name(p),type}); }
inline void __fastcall FakeInteresting(void* ie,void*,int32 type,CEntity* e) { trace.push_back({"interesting_event",Name(ie),type,Name(e)}); }
inline bool __cdecl FakeOnMission() { trace.push_back({"is_player_on_a_mission",missionRet}); return missionRet; }
inline CTaskComplexWander* __cdecl FakeWanderByType(CPed* p) { trace.push_back({"wander_task_by_ped_type",Name(p)}); return reinterpret_cast<CTaskComplexWander*>(wanderRet); }
inline CTask* __fastcall FakeWanderFirst(void* t,void*,CPed* p) { trace.push_back({"wander_create_first_sub_task",Name(t),Name(p)}); return Fake(3); }
inline CTask* __fastcall FakeWanderNext(void* t,void*,CPed* p) { trace.push_back({"wander_create_next_sub_task",Name(t),Name(p)}); return Fake(3); }
inline CTask* __fastcall FakeWanderControl(void* t,void*,CPed* p) { trace.push_back({"wander_control_sub_task",Name(t),Name(p)}); return Fake(3); }
inline int32 __cdecl FakeBlockIndex(const char* name) { trace.push_back({"anim_block_index",name ? std::string(name) : std::string("<null>"),blockRet}); return blockRet; }
inline void __cdecl FakeAddBlockRef(int32 idx) { trace.push_back({"add_anim_block_ref",idx}); }
inline void __cdecl FakeRemoveBlockRef(int32 idx) { trace.push_back({"remove_anim_block_ref",idx}); }
inline void __cdecl FakeRequestModel(int32 id,int32 flags) { trace.push_back({"request_model",id,flags}); }
inline CPlayerPed* __cdecl FakeFindPed(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(Ped(PLAYER)); }
inline CVehicle* __cdecl FakeFindVeh(int32 id,bool remote) { trace.push_back({"find_player_vehicle",id,remote,Name(playerVehRet)}); return reinterpret_cast<CVehicle*>(playerVehRet); }
inline CWanted* __cdecl FakeFindWanted(int32 id) { trace.push_back({"find_player_wanted",id}); return reinterpret_cast<CWanted*>(wanted.data()); }
inline void __fastcall FakeSetWantedNoDrop(CPed* p,void*,int32 level) { trace.push_back({"set_wanted_level_no_drop",Name(p),level}); }
inline bool __fastcall FakeCanJoin(void* w,void*,CPed* cop) { trace.push_back({"can_cop_join_pursuit",Name(w),Name(cop),canJoinRet}); return canJoinRet; }
inline bool __cdecl FakeLos(const CVector& a,const CVector& b,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7) {
    const bool r=((losMask>>(losCalls++%32))&1)!=0;
    trace.push_back({"line_of_sight",Vec(a),Vec(b),uint8(b1),uint8(b2),uint8(b3),uint8(b4),uint8(b5),uint8(b6),uint8(b7),r}); return r;
}
// CEventPedToChase/CEventPedToFlee: +4 tiempo activo, +8 valido, +0xC peaton (el resto es pila)
inline CEvent* __fastcall FakeGroupAdd(void* g,void*,CEvent* e,uint32 valid) {
    trace.push_back({"event_group_add",Name(g),int(e->GetEventType()),At<int32>(e,4),At<uint8>(e,8),Name(At(e,0xC)),valid&0xFF}); return nullptr;
}

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t OurVirtual(const char* cls,const char* fn){ auto* h=FindHook(cls,fn); if(!h||h->Symbol()[0]!='V')return 0; return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress()); }
inline uintptr_t Follow(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=45;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexPassObject","AbortIK",0x661290,0,-1,256},
    {"CTaskComplexPassObject","Clone",0,0x86FB18,1,128},
    {"CTaskComplexPassObject","Constructor",0x6611B0,0,-1,128},
    {"CTaskComplexPassObject","ControlSubTask",0,0x86FB18,10,1024},
    {"CTaskComplexPassObject","CreateFirstSubTask",0,0x86FB18,9,1024},
    {"CTaskComplexPassObject","CreateNextSubTask",0,0x86FB18,8,512},
    {"CTaskComplexPassObject","Destructor",0x661230,0,-1,128},
    {"CTaskComplexPassObject","GetTaskType",0,0x86FB18,4,64},
    {"CTaskComplexPassObject","MakeAbortable",0,0x86FB18,6,256},
    {"CTaskComplexRoadRage","Clone",0,0x86D9F0,1,128},
    {"CTaskComplexRoadRage","Constructor",0x6220A0,0,-1,128},
    {"CTaskComplexRoadRage","ControlSubTask",0,0x86D9F0,10,512},
    {"CTaskComplexRoadRage","CreateFirstSubTask",0,0x86D9F0,9,512},
    {"CTaskComplexRoadRage","CreateNextSubTask",0,0x86D9F0,8,1024},
    {"CTaskComplexRoadRage","CreateSubTask",0x629080,0,-1,1024},
    {"CTaskComplexRoadRage","Destructor",0x622110,0,-1,128},
    {"CTaskComplexRoadRage","GetTaskType",0,0x86D9F0,4,64},
    {"CTaskComplexRoadRage","MakeAbortable",0,0x86D9F0,6,256},
    {"CTaskComplexScreamInCarThenLeave","Clone",0,0x86E8D8,1,128},
    {"CTaskComplexScreamInCarThenLeave","Constructor",0x63C240,0,-1,128},
    {"CTaskComplexScreamInCarThenLeave","ControlSubTask",0,0x86E8D8,10,1024},
    {"CTaskComplexScreamInCarThenLeave","CreateFirstSubTask",0,0x86E8D8,9,256},
    {"CTaskComplexScreamInCarThenLeave","CreateNextSubTask",0,0x86E8D8,8,512},
    {"CTaskComplexScreamInCarThenLeave","CreateSubTask",0x642510,0,-1,1024},
    {"CTaskComplexScreamInCarThenLeave","Destructor",0x63C2C0,0,-1,128},
    {"CTaskComplexScreamInCarThenLeave","GetTaskType",0,0x86E8D8,4,64},
    {"CTaskComplexScreamInCarThenLeave","MakeAbortable",0,0x86E8D8,6,256},
    {"CTaskComplexSitDownThenIdleThenStandUp","Clone",0,0x86DF60,1,128},
    {"CTaskComplexSitDownThenIdleThenStandUp","Constructor",0x631460,0,-1,128},
    {"CTaskComplexSitDownThenIdleThenStandUp","ControlSubTask",0,0x86DF60,10,512},
    {"CTaskComplexSitDownThenIdleThenStandUp","CreateFirstSubTask",0,0x86DF60,9,128},
    {"CTaskComplexSitDownThenIdleThenStandUp","CreateNextSubTask",0,0x86DF60,8,512},
    {"CTaskComplexSitDownThenIdleThenStandUp","CreateSubTask",0x631550,0,-1,256},
    {"CTaskComplexSitDownThenIdleThenStandUp","Destructor",0x6314A0,0,-1,128},
    {"CTaskComplexSitDownThenIdleThenStandUp","GetTaskType",0,0x86DF60,4,64},
    {"CTaskComplexSitDownThenIdleThenStandUp","MakeAbortable",0,0x86DF60,6,256},
    {"CTaskComplexWanderCop","Constructor",0x460C80,0,-1,256},
    {"CTaskComplexWanderCop","ControlSubTask",0,0x859C80,10,1024},
    {"CTaskComplexWanderCop","CreateFirstSubTask",0,0x859C80,9,1024},
    {"CTaskComplexWanderCop","CreateNextSubTask",0,0x859C80,8,1024},
    {"CTaskComplexWanderCop","LookForCarAlarms",0x66B1B0,0,-1,1024},
    {"CTaskComplexWanderCop","LookForCriminals",0x66B300,0,-1,1024},
    {"CTaskComplexWanderCop","LookForStolenCopCars",0x66B290,0,-1,256},
    {"CTaskComplexWanderCop","ScanForStuff",0,0x859C80,12,1024},
    {"CTaskComplexWanderCop","ShouldPursuePlayer",0x66B160,0,-1,256},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }
inline CVector& PosRef(void* e) { auto* m=At<uint8*>(e,E_MATRIX); return m ? At<CVector>(m,0x30) : At<CVector>(e,E_POS); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks9c-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks9c-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks9c");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks9c-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    const uintptr_t dllWanderFirst=OurVirtual("CTaskComplexWander","CreateFirstSubTask"), dllWanderNext=OurVirtual("CTaskComplexWander","CreateNextSubTask"),
                    dllWanderControl=OurVirtual("CTaskComplexWander","ControlSubTask");
    if(!dllWanderFirst||!dllWanderNext||!dllWanderControl){Write({{"event","error"},{"reason","virtual hook not found"}});return;}
    FillTraps(fakeVtbl,std::make_integer_sequence<int,12>{}); FillTraps(vehVtbl,std::make_integer_sequence<int,96>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    vehVtbl[57]=reinterpret_cast<void*>(&VStep); vehVtbl[58]=reinterpret_cast<void*>(&VJump);
    const auto occupancyStart=Occupancy();
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    const int32 lf0=Last(TaskHdr());
    const std::vector<uint8> statesStart(States(TaskHdr()),States(TaskHdr())+Cap(TaskHdr()));
    // estado global tocado: reloj, ms_modelInfoPtrs[MODEL] y los bloques de animacion BLOCK0..BLOCK0+3
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{MODELPTRS+4*MODEL,4},{ANIMBLOCKS+0x20*BLOCK0,0x20*NBLOCKS}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    auto& timeMs=*reinterpret_cast<uint32*>(TIMER);
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch s1(bit_cast<uintptr_t>(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say)),FakeSay), s1e(0x5EFFE0,FakeSay);
        EntryPatch k1(bit_cast<uintptr_t>(&IKChainManager_c::IsArmPointing),FakeIsArmPointing), k1e(0x6182B0,FakeIsArmPointing);
        EntryPatch k2(bit_cast<uintptr_t>(&IKChainManager_c::AbortPointArm),FakeAbortPointArm), k2e(0x6182F0,FakeAbortPointArm);
        EntryPatch k3(bit_cast<uintptr_t>(static_cast<void(IKChainManager_c::*)(const char*,eIKArm,CPed*,CEntity*,eBoneTag32,CVector*,float,int32,float) const>(&IKChainManager_c::PointArm)),FakePointArm), k3e(0x618B60,FakePointArm);
        EntryPatch p1(bit_cast<uintptr_t>(&CPed::GetEntityThatThisPedIsHolding),FakeHolding), p1e(0x5E02E0,FakeHolding);
        EntryPatch p2(bit_cast<uintptr_t>(&CPed::GiveDelayedWeapon),FakeGiveDelayed), p2e(0x5E89B0,FakeGiveDelayed);
        EntryPatch p3(bit_cast<uintptr_t>(static_cast<void(CPed::*)(eWeaponType)>(&CPed::SetCurrentWeapon)),FakeSetCurrentWeapon), p3e(0x5E6280,FakeSetCurrentWeapon);
        EntryPatch t1(bit_cast<uintptr_t>(&CTaskManager::FindActiveTaskByType),FakeFindActive), t1e(0x681740,FakeFindActive);
        EntryPatch t2(TaskManagerAccess::ChangeSlotAddress(),FakeChangeSlot), t2e(0x681AF0,FakeSetTask), t3e(0x681B60,FakeSetTaskSecondary);
        EntryPatch t4(bit_cast<uintptr_t>(&CTaskSimpleHoldEntity::DropEntity),FakeDrop), t4e(0x6930F0,FakeDrop);
        EntryPatch i1(bit_cast<uintptr_t>(&CInterestingEvents::Add),FakeInteresting), i1e(0x602590,FakeInteresting);
        EntryPatch m1(bit_cast<uintptr_t>(&CTheScripts::IsPlayerOnAMission),FakeOnMission), m1e(0x464D50,FakeOnMission);
        EntryPatch w1(bit_cast<uintptr_t>(&CTaskComplexWander::GetWanderTaskByPedType),FakeWanderByType), w1e(0x673D00,FakeWanderByType);
        EntryPatch w2(Follow(dllWanderFirst),FakeWanderFirst), w2e(0x6740E0,FakeWanderFirst);
        EntryPatch w3(Follow(dllWanderNext),FakeWanderNext), w3e(0x674140,FakeWanderNext);
        EntryPatch w4(Follow(dllWanderControl),FakeWanderControl), w4e(0x674C30,FakeWanderControl);
        EntryPatch a1(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)),FakeBlockIndex), a1e(0x4D3990,FakeBlockIndex);
        EntryPatch a2(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef),FakeAddBlockRef), a2e(0x4D3FB0,FakeAddBlockRef);
        EntryPatch a3(bit_cast<uintptr_t>(&CAnimManager::RemoveAnimBlockRef),FakeRemoveBlockRef), a3e(0x4D3FD0,FakeRemoveBlockRef);
        EntryPatch a4(bit_cast<uintptr_t>(&CStreaming::RequestModel),FakeRequestModel), a4e(0x4087E0,FakeRequestModel);
        EntryPatch f1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPed), f1e(0x56E210,FakeFindPed);
        EntryPatch f2(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),FakeFindVeh), f2e(0x56E0D0,FakeFindVeh);
        EntryPatch f3(bit_cast<uintptr_t>(&FindPlayerWanted),FakeFindWanted), f3e(0x56E230,FakeFindWanted);
        EntryPatch c1(bit_cast<uintptr_t>(&CPlayerPed::SetWantedLevelNoDrop),FakeSetWantedNoDrop), c1e(0x609F30,FakeSetWantedNoDrop);
        EntryPatch c2(bit_cast<uintptr_t>(static_cast<bool(CWanted::*)(CCopPed*)>(&CWanted::CanCopJoinPursuit)),FakeCanJoin), c2e(0x562FB0,FakeCanJoin);
        EntryPatch l1(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),FakeLos), l1e(0x56A490,FakeLos);
        EntryPatch e1(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),FakeGroupAdd), e1e(0x4AB420,FakeGroupAdd);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                const float qnan=std::numeric_limits<float>::quiet_NaN();
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot;
                    // slots libres con 0xCD: los bytes que los constructores no escriben quedan iguales en ambas ejecuciones
                    ResetPool(TaskHdr(),TASK_SLOT,lf0,statesStart,occupancyStart,Occupancy());
                    restoreGlobals();
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x9C9Cu; if(!st)st=1; trace=json::array(); seed=uint32(i);
                    rngCalls=0; losCalls=0; freshIds.clear();
                    CleanRefs();
                    timeMs=next()%3 ? 100000u+next()%1000000u : next();
                    auto randPos=[&]{ return CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)}; };
                    auto randDir=[&]{ CVector d{uni(-1.f,1.f),uni(-1.f,1.f),uni(-0.3f,0.3f)}; const float m=std::sqrt(d.x*d.x+d.y*d.y+d.z*d.z); return m>0.01f ? CVector{d.x/m,d.y/m,d.z/m} : CVector{1.f,0.f,0.f}; };
                    // peatones: matriz (o posicion sin matriz), inteligencia, tipo, flags, vehiculo y armas
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                        p[E_TYPE]=3; At<void*>(p,P_INTEL)=intels[n].data();
                        if(next()%4||(cls=="CTaskComplexWanderCop"&&n==0)){ At<void*>(p,E_MATRIX)=pedMats[n].data();
                            for(int r=0;r<3;++r)At<CVector>(pedMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; }
                        PosRef(p)=randPos();
                        { static const int32 types[]{4,5,6,7,8,16,20,0,1}; At<int32>(p,P_PEDTYPE)=types[next()%std::size(types)]; }
                        At<uint32>(p,P_FLAGS)=next();
                        { const uint32 m=next()%3; At<CVehicle*>(p,P_VEHICLE)=m==0 ? nullptr : Veh(int(m-1)); }
                        p[P_WEAPSLOT]=uint8(next()%13); for(int k=0;k<13;++k)At<int32>(p,P_WEAPONS+0x1C*k)=next()%3 ? 0 : int32(next()%40);
                        p[P_DONTPURSUIT]=uint8(next()%4==0);
                    }
                    At<void*>(Ped(PLAYER),P_PLAYERDATA)=playerData.data();
                    for(auto& b:playerData)b=uint8(next()); At<void*>(playerData.data(),0)=wanted.data();
                    for(auto& b:wanted)b=uint8(next());
                    { static const uint32 levels[]{0,0,1,2,4,6,0x80000001u,0xFFFFFFFFu}; At<uint32>(wanted.data(),W_LEVEL)=levels[next()%std::size(levels)]; }
                    // vehiculos: vtable falsa (huecos 57/58), matriz, modelo MODEL, conductor, alarma, tipo y flags
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%8)<<3); At<void*>(v,E_MATRIX)=vehMats[n].data();
                        for(int r=0;r<3;++r)At<CVector>(vehMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(vehMats[n].data(),0x30)=randPos(); At<int16>(v,E_MODEL)=int16(MODEL);
                        At<CPed*>(v,V_DRIVER)=next()%3 ? Ped(3) : nullptr;
                        { const uint32 m=next()%4; At<uint16>(v,V_ALARM)=uint16(m==0 ? 0 : m==1 ? 0xFFFF : 1+next()%30000); }
                        { static const uint32 vt[]{0,0,0,1,3,5}; At<uint32>(v,V_TYPE)=vt[next()%std::size(vt)]; }
                        v[V_FLAGS428]=uint8(next()); v[V_MISSION]=uint8(next()); v[V_CRUISE]=uint8(next());
                    }
                    // CBaseModelInfo falso: +0x14 CColModel (radio de la esfera en +0x24)
                    for(auto& b:mi)b=uint8(next()); At<void*>(mi.data(),0x14)=col.data();
                    for(auto& b:col)b=uint8(next()); At<float>(col.data(),0x24)=uni(0.5f,6.f);
                    At<void*>(reinterpret_cast<void*>(MODELPTRS),4*MODEL)=mi.data();
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,FAKE_SIZE); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_PAUSE+n; fakes[n][FK_ABORT]=uint8(next()%2); }
                    At<CTask*>(fakes[2].data(),FK_CLONE)=Fake(5);
                    for(auto& b:event)b=uint8(next());
                    seed=next(); sayRet=int16(next()%40); armRet=next()%2; missionRet=next()%2; canJoinRet=next()%2;
                    stepMask=next(); jumpMask=next(); losMask=next();
                    { const uint32 m=next()%4; holdingRet=m==0 ? 0 : m==1 ? uintptr_t(Veh(1)) : uintptr_t(Ped(2)); }
                    findActiveRet=next()%4 ? uintptr_t(Fake(2)) : 0; At<CPed*>(fakes[2].data(),8)=next()%4 ? Ped(2) : nullptr;
                    wanderRet=next()%4 ? uintptr_t(Fake(1)) : 0;
                    { const uint32 m=next()%3; playerVehRet=m==0 ? 0 : uintptr_t(Veh(int(m-1))); }
                    blockRet=BLOCK0+int32(next()%NBLOCKS);
                    for(int n=0;n<NBLOCKS;++n)At<uint8>(reinterpret_cast<void*>(ANIMBLOCKS),0x20*(BLOCK0+n)+0x10)=uint8(next()%2);

                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches}}; json ret=nullptr;
                    auto* o=object.data(); auto* p0=Ped(0);
                    std::memset(o,0xCD,object.size());
                    std::vector<int> freshSlots;
                    auto collectFresh=[&]{
                        for(auto* h:hooks)h->State(false);
                        auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                        for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshSlots.push_back(int(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                        json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t)); out["tasks"]=tasks;
                    };
                    auto setTask=[&](uintptr_t vtbl,size_t size){ for(size_t b=0;b<size;++b)o[b]=uint8(next()); At<uintptr_t>(o,0)=vtbl; At<uint32>(o,4)=0; At<uint32>(o,8)=0; };
                    auto setSub=[&](){ At<CTask*>(o,8)=Fake(0); };
                    auto subType=[&](std::initializer_list<int32> l){ At<int32>(fakes[0].data(),FK_TYPE)=*(l.begin()+next()%l.size()); };
                    // temporizador: inicio, intervalo, iniciado y parado (bytes 0/1)
                    auto setTimer=[&](size_t off){ At<uint32>(o,off)=next()%4 ? timeMs-next()%8000 : next(); At<int32>(o,off+4)=next()%4 ? int32(next()%6000) : int32(next());
                        o[off+8]=uint8(next()%4!=0); o[off+9]=uint8(next()%3==0); };
                    auto typeOf=[&](void* p){ return reinterpret_cast<CTask*>(p)->GetTaskType(); };
                    auto callTask=[&](CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    auto callClone=[&]{ return reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); };
                    auto callDtor=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); };
                    auto callAbort=[&](int32 prio,const void* ev){ return reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const void*)>(target)(o,p0,prio,ev); };
                    auto ctorDone=[&](size_t size){ for(auto* h:hooks)h->State(false); out["object"]=Region(o,size,true); out["type"]=typeOf(o); };
                    auto taskRet=[&](CTask* r){ collectFresh(); ret=Name(r); };
                    auto inVeh=[&](CPed* p,bool on){ auto& fl=At<uint32>(p,P_FLAGS); fl=on ? (fl|0x100u) : (fl&~0x100u); };
                    auto prio=[&]{ return int32(next()%4); };
                    auto ev=[&]()->const void*{ return next()%2 ? event.data() : nullptr; };

                    if(nm=="GetTaskType") {
                        setTask(fn.vt,0x40);
                        ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        out["object"]=Region(o,0x40,true);
                    }
                    else if(cls=="CTaskComplexPassObject") {
                        // +0xC m_ped (referencia), +0x10 isPasser, +0x14 destino, +0x20 temporizador, +0x2C entidad a pasar
                        // (el destructor no la limpia en el EXE: FIX_BUGS en la DLL; se deja nula en el destructor)
                        auto setPass=[&](bool ped,bool entity){ setTask(VT_PASS,0x30); At<CPed*>(o,0xC)=ped ? Ped(1) : nullptr; Reg(At<CPed*>(o,0xC),o+0xC);
                            o[0x10]=uint8(next()%2); At<CVector>(o,0x14)=randPos(); setTimer(0x20);
                            At<void*>(o,0x2C)=entity ? (next()%2 ? static_cast<void*>(Ped(2)) : static_cast<void*>(Veh(0))) : nullptr; };
                        if(nm=="Constructor"){
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CPed*,uint32)>(target)(o,next()%4 ? Ped(1) : nullptr,next()%2)); ctorDone(0x30);
                        } else {
                            setPass(nm=="Destructor" ? next()%4!=0 : next()%6!=0,nm!="Destructor"&&next()%4!=0);
                            if(nm=="Destructor"){ callDtor(); }
                            else if(nm=="Clone"){ taskRet(callClone()); }
                            else if(nm=="AbortIK"){ reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,p0); }
                            else if(nm=="MakeAbortable"){ setSub(); ret=callAbort(prio(),ev()); }
                            else {
                                setSub();
                                if(nm=="CreateNextSubTask")subType({TASK_COMPLEX_TURN_TO_FACE_ENTITY,TASK_COMPLEX_TURN_TO_FACE_ENTITY,TASK_COMPLEX_TURN_TO_FACE_ENTITY,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_PAUSE});
                                if(nm=="CreateFirstSubTask"&&next()%3){
                                    // el otro peaton cerca (umbral 1.5 m), a veces con NaN
                                    CVector d{uni(-1.3f,1.3f),uni(-1.3f,1.3f),uni(-0.6f,0.6f)}; if(next()%16==0)d.y=qnan;
                                    const CVector b=PosRef(p0); PosRef(Ped(1))=CVector{b.x+d.x,b.y+d.y,b.z+d.z};
                                }
                                taskRet(callTask(p0));
                            }
                            out["object"]=Region(o,0x30,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexRoadRage") {
                        // +0xC objetivo (referencia)
                        auto setRage=[&](bool w){ setTask(VT_RAGE,0x10); At<CPed*>(o,0xC)=w ? Ped(1) : nullptr; Reg(At<CPed*>(o,0xC),o+0xC); };
                        if(nm=="Constructor"){
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CPed*)>(target)(o,next()%4 ? Ped(1) : nullptr)); ctorDone(0x10);
                        } else {
                            setRage(next()%6!=0);
                            if(nm=="Destructor"){ callDtor(); }
                            else if(nm=="Clone"){ taskRet(callClone()); }
                            else if(nm=="MakeAbortable"){ setSub(); ret=callAbort(prio(),ev()); }
                            else if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_COMPLEX_TURN_TO_FACE_ENTITY,TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_SHAKE_FIST,TASK_COMPLEX_ENTER_CAR_AS_DRIVER,
                                                       TASK_COMPLEX_SEEK_ENTITY,TASK_COMPLEX_SEEK_ENTITY,TASK_COMPLEX_DESTROY_CAR,TASK_COMPLEX_DESTROY_CAR,
                                                       TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_FINISHED,TASK_SIMPLE_PAUSE,0,-1};
                                setSub(); taskRet(reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,t[next()%std::size(t)],p0));
                            }
                            else {
                                setSub();
                                if(nm=="CreateNextSubTask")subType({TASK_COMPLEX_TURN_TO_FACE_ENTITY,TASK_COMPLEX_DESTROY_CAR,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_SIMPLE_SHAKE_FIST,
                                                                    TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_ENTER_CAR_AS_DRIVER,TASK_COMPLEX_SEEK_ENTITY,TASK_COMPLEX_SEEK_ENTITY,TASK_SIMPLE_PAUSE});
                                if(nm=="ControlSubTask"){
                                    subType({TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_COMPLEX_DESTROY_CAR,TASK_COMPLEX_DESTROY_CAR,TASK_SIMPLE_PAUSE});
                                    // el objetivo alrededor de 20 m, a veces con NaN
                                    if(next()%3){ const CVector b=PosRef(p0), d=randDir(); const float r=uni(14.f,26.f);
                                        PosRef(Ped(1))=CVector{b.x+d.x*r,b.y+d.y*r,next()%16==0 ? qnan : b.z+d.z*r}; }
                                }
                                if(nm=="CreateFirstSubTask"&&next()%2){ At<CVehicle*>(p0,P_VEHICLE)=Veh(0); inVeh(p0,true); }
                                taskRet(callTask(p0));
                            }
                            out["object"]=Region(o,0x10,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexScreamInCarThenLeave") {
                        // +0xC vehiculo (referencia), +0x10 puerta, +0x14 temporizador
                        static const int32 doors[]{0,8,10,11,9,18};
                        auto setScream=[&](bool veh){ setTask(VT_SCREAM,0x20); At<CVehicle*>(o,0xC)=veh ? Veh(int(next()%2)) : nullptr; Reg(At<CVehicle*>(o,0xC),o+0xC);
                            At<int32>(o,0x10)=doors[next()%std::size(doors)]; setTimer(0x14); };
                        if(nm=="Constructor"){
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32)>(target)(o,next()%4 ? Veh(0) : nullptr,doors[next()%std::size(doors)])); ctorDone(0x20);
                        } else if(nm=="CreateSubTask"){
                            static const int32 t[]{TASK_COMPLEX_LEAVE_CAR_AND_FLEE,TASK_COMPLEX_LEAVE_CAR_AND_FLEE,TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_CAR_DRIVE,TASK_FINISHED,TASK_SIMPLE_PAUSE,0};
                            const int32 type=t[next()%std::size(t)];
                            setScream(type==TASK_COMPLEX_LEAVE_CAR_AND_FLEE||next()%4!=0); setSub();
                            taskRet(reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,type,p0));
                            out["object"]=Region(o,0x20,true);
                        } else {
                            // el vehiculo hace falta salvo en ControlSubTask (alli puede ser nulo) y en el destructor
                            setScream(nm=="ControlSubTask"||nm=="Destructor" ? next()%5!=0 : true);
                            if(nm=="Destructor"){ callDtor(); }
                            else if(nm=="Clone"){ taskRet(callClone()); }
                            else if(nm=="MakeAbortable"){ setSub(); ret=callAbort(prio(),ev()); }
                            else {
                                setSub();
                                if(nm=="CreateNextSubTask")subType({TASK_COMPLEX_LEAVE_CAR_AND_FLEE,TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_PAUSE});
                                if(nm=="ControlSubTask")subType({TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_CAR_DRIVE,TASK_COMPLEX_LEAVE_CAR_AND_FLEE});
                                taskRet(callTask(p0));
                            }
                            out["object"]=Region(o,0x20,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexSitDownThenIdleThenStandUp") {
                        // +0xC duracion, +0x10 sentarse en escalon, +0x11 instantaneo, +0x12 referencia al bloque "attractors" (bytes 0/1)
                        auto setSit=[&]{ setTask(VT_SIT,0x14); At<int32>(o,0xC)=int32(next()%20000)-1000; o[0x10]=uint8(next()%2); o[0x11]=uint8(next()%2); o[0x12]=uint8(next()%2); };
                        if(nm=="Constructor"){
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,uint32,uint32)>(target)(o,int32(next()%20000)-1000,next()%2,next()%2)); ctorDone(0x14);
                        } else {
                            setSit();
                            if(nm=="Destructor"){ callDtor(); }
                            else if(nm=="Clone"){ taskRet(callClone()); }
                            else if(nm=="MakeAbortable"){ setSub(); ret=callAbort(prio(),ev()); }
                            else if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_SIMPLE_SIT_DOWN,TASK_SIMPLE_SIT_IDLE,TASK_SIMPLE_STAND_UP,TASK_FINISHED,TASK_SIMPLE_STAND_STILL,0};
                                setSub(); taskRet(reinterpret_cast<CTask*(__thiscall*)(void*,int32)>(target)(o,t[next()%std::size(t)]));
                            }
                            else {
                                setSub();
                                if(nm=="CreateNextSubTask")subType({TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_SIT_DOWN,TASK_SIMPLE_SIT_IDLE,TASK_SIMPLE_STAND_UP,TASK_SIMPLE_PAUSE});
                                taskRet(callTask(p0));
                            }
                            out["object"]=Region(o,0x14,true);
                        }
                    }
                    else if(cls=="CTaskComplexWanderCop") {
                        // padre CTaskComplexWander hasta 0x28; +0x28 tarea GoToPointAndStandStill guardada, +0x2C temporizador de exploracion,
                        // +0x38/+0x3C proximas busquedas de criminales / alarmas, +0x40 ultimo criminal, +0x44 temporizador de la subtarea
                        static const int32 ms[]{4,6,7,1};
                        auto setCop=[&]{ setTask(VT_COP,0x50); At<int32>(o,0xC)=ms[next()%std::size(ms)];
                            At<CTask*>(o,0x28)=next()%3==0 ? Fake(2) : nullptr; setTimer(0x2C);
                            At<uint32>(o,0x38)=next()%2 ? timeMs-next()%2000 : timeMs+next()%2000; At<uint32>(o,0x3C)=next()%2 ? timeMs-next()%2000 : timeMs+next()%2000;
                            At<CPed*>(o,0x40)=next()%3==0 ? Ped(1+int(next()%3)) : nullptr; setTimer(0x44); };
                        // el policia (peaton 0): tipo 6 casi siempre
                        At<int32>(p0,P_PEDTYPE)=next()%5 ? 6 : 4;
                        // escaner de peatones del policia (16 huecos) y tarea activa de los candidatos 1..3
                        { auto** ents=reinterpret_cast<CPedIntelligence*>(intels[0].data())->GetPedEntities();
                          for(int k=0;k<16;++k)ents[k]=next()%3==0 ? reinterpret_cast<CEntity*>(Ped(1+int(next()%3))) : nullptr; }
                        for(int c=1;c<=3;++c){
                            { static const int32 types[]{7,8,12,16,20,20,6,4}; At<int32>(Ped(c),P_PEDTYPE)=types[next()%std::size(types)]; }
                            if(next()%4){ At<CTask*>(intels[c].data(),I_TM+4*(next()%5))=Fake(3+c); At<int32>(fakes[3+c].data(),FK_TYPE)=next()%3 ? TASK_COMPLEX_WANDER : TASK_SIMPLE_STAND_STILL; }
                            // cerca del policia (umbral 10 m), delante o detras, a veces con NaN
                            if(next()%4){ const CVector b=PosRef(p0), d=randDir(); const float r=uni(0.f,13.f);
                                PosRef(Ped(c))=CVector{b.x+d.x*r,next()%16==0 ? qnan : b.y+d.y*r,b.z+d.z*r}; }
                        }
                        // vehiculo del jugador alrededor de 20 m del policia
                        if(next()%2){ const CVector b=PosRef(p0), d=randDir(); const float r=uni(10.f,30.f); At<CVector>(vehMats[0].data(),0x30)=CVector{b.x+d.x*r,b.y+d.y*r,b.z+d.z*r}; }
                        if(nm=="Constructor"){
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,uint32)>(target)(o,ms[next()%std::size(ms)],next()%256)); ctorDone(0x50);
                        } else {
                            setCop();
                            static const std::initializer_list<int32> subs{TASK_COMPLEX_POLICE_PURSUIT,TASK_COMPLEX_POLICE_PURSUIT,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_PAUSE,TASK_COMPLEX_WANDER};
                            At<int32>(fakes[2].data(),FK_TYPE)=next()%2 ? TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL : TASK_SIMPLE_PAUSE;
                            if(nm=="ShouldPursuePlayer"){
                                if(next()%4)setSub(); subType(subs);
                                ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,p0);
                            }
                            else if(nm=="LookForCarAlarms"||nm=="LookForStolenCopCars"||nm=="LookForCriminals"){
                                setSub(); reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,p0);
                            }
                            else if(nm=="ScanForStuff"){
                                setSub(); subType(subs); reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,p0);
                            }
                            else {
                                if(nm=="CreateFirstSubTask"&&next()%2)At<CTask*>(o,8)=nullptr; else { setSub(); subType(subs); }
                                taskRet(callTask(p0));
                            }
                            out["object"]=Region(o,0x50,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    for(auto* h:hooks)h->State(false);
                    out["ents"]=AllHashes(); out["fakes"]=json::array({Region(fakes[0].data(),0x20,true),Region(fakes[2].data(),0x20,true)});
                    out["anim_blocks"]=ReversingProbe::Bytes(reinterpret_cast<void*>(ANIMBLOCKS+0x20*BLOCK0),0x20*NBLOCKS);
                    // limpieza: referencias de los fixtures y slots de las tareas nuevas (sin destructor); el pool debe quedar como antes
                    out["ret"]=ret; out["trace"]=trace; out["rng"]=rngCalls; out["time"]=timeMs;
                    CleanRefs();
                    for(int k:freshSlots)GetTaskPool()->SetFreeAt(size_t(k),true);
                    restoreGlobals();
                    out["clean"]=Occupancy()==occ;
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
    CleanRefs();
    ResetPool(TaskHdr(),TASK_SLOT,lf0,statesStart,occupancyStart,Occupancy());
    const bool timeTouched=timeMs!=At<uint32>(saved[0].data(),0); restoreGlobals();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&Last(TaskHdr())==lf0&&globalsIntact();
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"time_was_modified",timeTouched},{"state_restored",restored}});
}
}
