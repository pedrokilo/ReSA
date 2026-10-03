#pragma once
// Banco dentro del motor: tareas y grupos (lista de 70, 28 funciones). Asignadores de tareas por defecto de grupo
// (Chat, FollowAnyMeans, FollowLimited, StandStill y los GetType), CPedGroups::Process/RemoveGroup,
// CTaskAllocatorKillThreatsBasicRandomGroup, CTaskComplexDragPedFromCar, CTaskSequences::Init/CleanUpForShutdown,
// CTaskSimpleFinishBrain, CTaskSimpleLeaveGroup, CTaskSimplePlayHandSignalAnim, CTaskSimpleScratchHead y CTimeModelInfo.
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos: peatones, grupo,
// inteligencia de grupo, vehiculos, manejo, clumps, animaciones, objetos de mano y tareas falsas (vtable trazada) en
// buffers propios. CPedGroups::ms_groups/ms_activeGroups/mision/muertes y CTaskSequences::ms_taskSequence/ms_bIsOpened/
// ms_iActiveSequence se guardan, se rellenan con contenido falso y se restauran tras cada ejecucion; igual con
// CObject::nNoTempObjects. Se simulan y trazan: rand, CTheScripts::IsPlayerOnAMission, CPedGroupMembership::Process,
// CPedGroupIntelligence::Process, CPedGroup::Flush, CPedGroups::GetPedsGroup, CPedGroupIntelligence::FlushTasks y
// SetEventResponseTask (se clona y serializa la tarea recibida), CInterestingEvents::Add, printf del EXE,
// CCarEnterExit::ComputeTargetDoorToExit, CTaskComplexEnterCar::CreateFirstSubTask/CreateSubTask/ControlSubTask,
// IKChainManager_c::IsArmPointing, CPed::GetEntityThatThisPedIsHolding, CAnimManager::BlendAnimation/GetAnimAssociation/
// AddAnimation, CAnimBlendAssociation::SetFinishCallback, CObject::operator new, el constructor de CHandObject y
// CWorld::Add. Se compara la memoria de los fixtures (punteros traducidos a nombres), las tareas nuevas del pool,
// las referencias, los globales tocados, el retorno y la traza.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupIntelligence.h"
#include "Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.h"
#include "Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.h"
#include "Tasks/TaskSequences.h"
#include "TaskComplexDragPedFromCar.h"
#include "TaskSimpleFinishBrain.h"
#include "TaskSimpleLeaveGroup.h"
#include "TaskSimplePlayHandSignalAnim.h"
#include "TaskSimpleScratchHead.h"
#include "TaskSimpleAnim.h"
#include "Models/TimeModelInfo.h"
#include "Scripts/TheScripts.h"
#include "InterestingEvents.h"
#include "CarEnterExit.h"
#include "Ragdoll/IKChainManager.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Entity/Object/HandObject.h"
#include "World.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace Tasks70Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

constexpr int NPEDS=16, FAKES=8;
constexpr size_t FK_CLONE=0x34, FK_TYPE=0x38, FK_ABORT=0x3C;   // configuracion por objeto dentro de cada tarea falsa
// desplazamientos crudos del EXE
constexpr size_t P_CLUMP=0x18, P_FLAGS2=0x46C;                   // CPed: m_pRwObject, banderas (bit 8 = bInVehicle)
constexpr size_t G_MEMBERSHIP=0x8, G_MEMBERS=0xC, G_INTEL=0x30, G_PAIRS=0x21C, G_DM=0x2C8, G_MISSION=0x2D0; // CPedGroup
constexpr size_t I_PAIRS=0xC, I_SECONDARY=0xAC;                  // CPedGroupIntelligence
constexpr uintptr_t GROUPS=0xC09920, ACTIVE=0xC098E0, ONMISSION=0xC098E8, KILLS=0xC098EC;
constexpr size_t GROUP_SZ=0x2D4, GROUPS_BYTES=GROUP_SZ*8;
constexpr uintptr_t SEQS=0xC178F0, OPENED=0xC17898, ACTIVE_SEQ=0x8D2E98; constexpr size_t SEQ_BYTES=64*0x40;
constexpr uintptr_t NO_TEMP=0xBB4A70, EXE_FINISH_CB=0x61A8A0, INTERESTING=0xC0B058;
constexpr uintptr_t SENTINEL=0x5EED0000;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> mats{};
alignas(16) inline std::array<uint8,sizeof(CPedGroup)> group{};
alignas(16) inline std::array<uint8,sizeof(CPedGroupIntelligence)> intel{};
alignas(16) inline std::array<uint8,sizeof(CVehicle)> veh{}, oldVeh{};
alignas(16) inline std::array<uint8,sizeof(tHandlingData)> handling{};
alignas(16) inline std::array<uint8,0x200> ent{};                   // entidad sostenida (GetEntityThatThisPedIsHolding)
alignas(16) inline std::array<uint8,0x100> object{};                // objeto probado (tarea, asignador, model info)
alignas(16) inline std::array<std::array<uint8,0x40>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,0x40>,3> clumps{};   // 0 peaton, 1-2 manos (tipo rpCLUMP en +0)
alignas(16) inline std::array<uint8,sizeof(CAnimBlendAssociation)> assoc{};
alignas(16) inline std::array<uint8,0x40> sassoc{}, hier{};
alignas(16) inline std::array<std::array<uint8,0x190>,2> hands{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline CVehicle* Veh() { return reinterpret_cast<CVehicle*>(veh.data()); }
inline CVehicle* OldVeh() { return reinterpret_cast<CVehicle*>(oldVeh.data()); }
inline uint8* Groups() { return reinterpret_cast<uint8*>(GROUPS); }
inline uint8* Seqs() { return reinterpret_cast<uint8*>(SEQS); }

// pool de tareas (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot)
inline uint8* PoolHdr() { return reinterpret_cast<uint8*>(GetTaskPool()); }
inline uint8* PoolStorage() { return *reinterpret_cast<uint8**>(PoolHdr()); }
inline uint8* PoolStates() { return *reinterpret_cast<uint8**>(PoolHdr()+4); }
inline int32 PoolCap() { return *reinterpret_cast<int32*>(PoolHdr()+8); }
constexpr size_t TASK_SLOT=0x80;
inline bool IsPoolTask(uintptr_t v) {
    const auto s=uintptr_t(PoolStorage()); if(v<s||v>=s+size_t(PoolCap())*TASK_SLOT||(v-s)%TASK_SLOT)return false;
    return !(PoolStates()[(v-s)/TASK_SLOT]&0x80);
}

inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==EXE_FINISH_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleAnim::FinishRunAnimCB))return "FinishRunAnimCB";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(mats[n].data(),mats[n].size(),"mat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<3;++n)if(auto s=in(clumps[n].data(),clumps[n].size(),"clump"))return *s+"#"+std::to_string(n);
    for(int n=0;n<2;++n)if(auto s=in(hands[n].data(),hands[n].size(),"hand"))return *s+"#"+std::to_string(n);
    if(auto s=in(group.data(),group.size(),"group"))return *s; if(auto s=in(intel.data(),intel.size(),"intel"))return *s;
    if(auto s=in(veh.data(),veh.size(),"veh"))return *s; if(auto s=in(oldVeh.data(),oldVeh.size(),"oldveh"))return *s;
    if(auto s=in(handling.data(),handling.size(),"handling"))return *s; if(auto s=in(ent.data(),ent.size(),"ent"))return *s;
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(assoc.data(),assoc.size(),"assoc"))return *s;
    if(auto s=in(sassoc.data(),sassoc.size(),"sassoc"))return *s; if(auto s=in(hier.data(),hier.size(),"hier"))return *s;
    if(v>=GROUPS&&v<GROUPS+GROUPS_BYTES)return "groups#"+std::to_string((v-GROUPS)/GROUP_SZ)+"+"+std::to_string((v-GROUPS)%GROUP_SZ);
    if(v>=SEQS&&v<SEQS+SEQ_BYTES)return "seq#"+std::to_string((v-SEQS)/0x40)+"+"+std::to_string((v-SEQS)%0x40);
    if(v>=0xC09908&&v<0xC09920)return "allocator+"+std::to_string(v-0xC09908);
    if(v==INTERESTING)return "interesting_events";
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
template<size_t N> std::string EntHash(const std::array<uint8,N>& buf) {
    auto copy=buf; const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(buf.data())->m_pReferences)-uintptr_t(buf.data());
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }

// tarea serializada sin vptr (se compara el tipo); words 1..31 del hueco del pool (los bytes sin escribir son 0xCD)
// Mascaras: la z del desplazamiento de las tareas de seguir al lider queda sin escribir en la pila del EXE;
// los bytes 0x39..0x3B de CTaskComplexSequence son relleno.
inline json Word(int type,size_t off,uint32 v,int depth);
inline json SerTask(const void* p,int depth) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType(); const auto* w=static_cast<const uint32*>(p);
    json words=json::array(); for(size_t k=1;k<TASK_SLOT/4;++k)words.push_back(Word(type,k*4,w[k],depth));
    return {{"type",type},{"words",words}};
}
inline json Word(int type,size_t off,uint32 v,int depth) {
    if(type==TASK_COMPLEX_SEQUENCE&&off==0x38)return v&0xFF;
    if((type==TASK_COMPLEX_FOLLOW_LEADER_ANY_MEANS||type==TASK_COMPLEX_FOLLOW_LEADER_IN_FORMATION)&&off==0x1C)return "z";
    if(depth>=0&&depth<3&&IsPoolTask(v)&&!freshIds.contains(v))return SerTask(reinterpret_cast<const void*>(uintptr_t(v)),depth+1);
    return Name(v);
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{};
inline uint32 missionMask{}, missionCalls{}, clearMask{};
inline uintptr_t pedsGroupRet{};
inline int doorRet{};
inline uint32 armMask{}, armCalls{}, handNewMask{}, handNewCalls{};
inline uintptr_t holdRet{};

// tareas falsas: cada objeto lleva su resultado de Clone, su tipo y su respuesta a MakeAbortable
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(t)}); return At<CTask*>(t,FK_CLONE); }
inline CTask* __fastcall FSub(CTask*,void*) { return nullptr; }
inline bool __fastcall FSimple(CTask*,void*) { return false; }
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int prio,const CEvent* e) { trace.push_back({"make_abortable",Name(t),Name(p),prio,Name(e)}); return At<uint8>(t,FK_ABORT)!=0; }
template<int N> void* __fastcall FTrap(CTask*,void*) { trace.push_back({"unexpected_slot",N}); return nullptr; }
inline std::array<void*,12> fakeVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }

// RNG
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
// CPedGroups
inline bool __cdecl FakeOnMission() { const bool r=(missionMask>>(missionCalls++%32))&1; trace.push_back({"on_mission",r}); return r; }
inline void __fastcall FakeMembershipProcess(uint8* m,void*) {
    trace.push_back({"membership_process",Name(m)});
    const auto g=uintptr_t(m)-G_MEMBERSHIP; // simula la salida de miembros: deja el grupo vacio si su bit esta activo
    if(g>=GROUPS&&g<GROUPS+GROUPS_BYTES&&(g-GROUPS)%GROUP_SZ==0&&((clearMask>>((g-GROUPS)/GROUP_SZ))&1))std::memset(m+4,0,8*4);
}
inline void __fastcall FakeIntelligenceProcess(void* i,void*) { trace.push_back({"intelligence_process",Name(i)}); }
inline void __fastcall FakeGroupFlush(void* g,void*) { trace.push_back({"group_flush",Name(g)}); }
// asignador KillThreats
inline CPedGroup* __cdecl FakeGetPedsGroup(const CPed* p) { trace.push_back({"get_peds_group",Name(p)}); return reinterpret_cast<CPedGroup*>(pedsGroupRet); }
inline int __cdecl FakePrintf(const char*,...) { return 0; } // mensaje de depuracion del EXE; la DLL usa su propio log
inline void __fastcall FakeInteresting(void* self,void*,int32 type,CEntity* e) { trace.push_back({"interesting_event",Name(self),type,Name(e)}); }
inline void FlushImpl(CPedTaskPair* pairs,CPed* ped) {
    trace.push_back({"flush_tasks",Name(pairs),Name(ped)});
    for(int n=0;n<TOTAL_PED_GROUP_MEMBERS;++n)if(!ped||pairs[n].Ped==ped){ if(pairs[n].Task)trace.push_back({"flushed",n,Name(pairs[n].Task)}); pairs[n].Task=nullptr; }
}
inline void __fastcall FakeFlushExe(void*,void*,CPedTaskPair* pairs,CPed* ped) { FlushImpl(pairs,ped); } // EXE: ret 8
inline void __cdecl FakeFlushDll(CPedTaskPair* pairs,CPed* ped) { FlushImpl(pairs,ped); }
inline void __fastcall FakeSetResponse(void* in,void*,CPed* ped,uint32 hasMain,const CTask* mainTask,uint32 hasSec,const CTask*,int32 slot) {
    json r={"set_event_response",Name(in),Name(ped),uint8(hasMain),uint8(hasSec)};
    if(uint8(hasSec))r.push_back(slot);
    // la tarea llega en la pila (bytes sin escribir no comparables): se clona, se serializa y se borra el clon
    if(uint8(hasMain)&&mainTask){ CTask* c=mainTask->Clone(); r.push_back(SerTask(c,0)); delete c; }
    trace.push_back(r);
}
// CTaskComplexDragPedFromCar
inline int __cdecl FakeDoorToExit(const CVehicle* v,const CPed* p) { trace.push_back({"door_to_exit",Name(v),Name(p)}); return doorRet; }
inline CTask* __fastcall FakeEnterFirst(void* self,void*,CPed* p) { trace.push_back({"enter_car_first",Name(self),Name(p)}); return Fake(1); }
inline CTask* __fastcall FakeEnterSub(void* self,void*,int32 type,CPed* p) { trace.push_back({"enter_car_sub",Name(self),type,Name(p)}); return Fake(2); }
inline CTask* __fastcall FakeEnterControl(void* self,void*,CPed* p) { trace.push_back({"enter_car_control",Name(self),Name(p)}); return Fake(3); }
// CTaskSimplePlayHandSignalAnim
inline bool __stdcall FakeArmPointing(int32 arm,CPed* p) { const bool r=(armMask>>(armCalls++%32))&1; trace.push_back({"arm_pointing",arm,Name(p),r}); return r; }
inline CEntity* __fastcall FakeHolding(CPed* p,void*) { trace.push_back({"holding",Name(p)}); return reinterpret_cast<CEntity*>(holdRet); }
inline CAnimBlendAssociation* __cdecl FakeBlend(RpClump* c,int32 grp,int32 id,float delta) { trace.push_back({"blend_animation",Name(c),grp,id,F(delta)}); return reinterpret_cast<CAnimBlendAssociation*>(assoc.data()); }
inline void __fastcall FakeSetFinishCb(void* a,void*,void* cb,void* data) { trace.push_back({"set_finish_callback",Name(a),Name(cb),Name(data)}); }
inline void* __cdecl FakeObjectNew(size_t n) { const uint32 k=handNewCalls++; trace.push_back({"object_new",n}); return (handNewMask>>k)&1 ? nullptr : hands[k%2].data(); }
inline void* __fastcall FakeHandCtor(uint8* self,void*,int32 model,CPed* p,uint32 left) {
    trace.push_back({"hand_ctor",Name(self),model,Name(p),uint8(left)});
    std::memset(self,0,0x18C); At<void*>(self,0x18)=clumps[self==hands[1].data() ? 2 : 1].data(); return self;
}
inline void __cdecl FakeWorldAdd(CEntity* e) { trace.push_back({"world_add",Name(e)}); }
inline void* __cdecl FakeGetAnimAssoc(int32 grp,int32 id) { trace.push_back({"get_anim_association",grp,id}); At<void*>(sassoc.data(),0x10)=hier.data()+4*(grp&7); return sassoc.data(); }
inline void* __cdecl FakeAddAnimation(RpClump* c,void* h,int32 flags) { trace.push_back({"add_animation",Name(c),Name(h),flags}); return nullptr; }
inline uintptr_t LocalAddress(HMODULE module,const char* name) { auto p=reinterpret_cast<uintptr_t>(GetProcAddress(module,name)); if(p&&*reinterpret_cast<uint8*>(p)==0xE9)p+=5+*reinterpret_cast<int32*>(p+1); return p; }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=28;
inline const std::array<Fn,NFNS> fns{{
    {"CPedGroupDefaultTaskAllocatorChat","AllocateDefaultTasks",0,0x86C774,0,1024},
    {"CPedGroupDefaultTaskAllocatorChat","GetType",0,0x86C774,1,64},
    {"CPedGroupDefaultTaskAllocatorFollowAnyMeans","AllocateDefaultTasks",0,0x86C75C,0,1024},
    {"CPedGroupDefaultTaskAllocatorFollowAnyMeans","GetType",0,0x86C75C,1,64},
    {"CPedGroupDefaultTaskAllocatorFollowLimited","AllocateDefaultTasks",0,0x86C764,0,1024},
    {"CPedGroupDefaultTaskAllocatorFollowLimited","GetType",0,0x86C764,1,64},
    {"CPedGroupDefaultTaskAllocatorRandom","GetType",0,0x86C77C,1,64},
    {"CPedGroupDefaultTaskAllocatorSitInLeaderCar","GetType",0,0x86C784,1,64},
    {"CPedGroupDefaultTaskAllocatorStandStill","AllocateDefaultTasks",0,0x86C76C,0,1024},
    {"CPedGroupDefaultTaskAllocatorStandStill","GetType",0,0x86C76C,1,64},
    {"CPedGroups","Process",0x5FC800,0,-1,1024},
    {"CPedGroups","RemoveGroup",0x5FB870,0,-1,256},
    {"CTaskAllocatorKillThreatsBasicRandomGroup","AllocateTasks",0,0x86C80C,1,1024},
    {"CTaskAllocatorKillThreatsBasicRandomGroup","GetType",0,0x86C80C,5,64},
    {"CTaskComplexDragPedFromCar","ControlSubTask",0,0x86EB6C,10,512},
    {"CTaskComplexDragPedFromCar","CreateFirstSubTask",0,0x86EB6C,9,1024},
    {"CTaskSequences","CleanUpForShutdown",0x632DD0,0,-1,128},
    {"CTaskSequences","Init",0x632D90,0,-1,128},
    {"CTaskSimpleFinishBrain","Clone",0,0x859D20,1,128},
    {"CTaskSimpleFinishBrain","GetTaskType",0,0x859D20,4,32},
    {"CTaskSimpleLeaveGroup","Constructor",0x463540,0,-1,64},
    {"CTaskSimpleLeaveGroup","Destructor",0x463560,0,-1,64},
    {"CTaskSimplePlayHandSignalAnim","ProcessPed",0,0x86D5B8,7,1024},
    {"CTaskSimplePlayHandSignalAnim","StartAnim",0x61AF60,0,-1,1024},
    {"CTaskSimpleScratchHead","Clone",0,0x85A100,1,128},
    {"CTaskSimpleScratchHead","IsInterruptable",0,0x85A100,9,64},
    {"CTimeModelInfo","GetModelType",0,0x85BCB0,4,64},
    {"CTimeModelInfo","GetTimeInfo",0,0x85BCB0,5,64},
}};
// instancias estaticas de los asignadores por defecto (0xC09908..0xC0991C) segun su vtable
inline uintptr_t AllocatorInstance(uintptr_t vt) {
    switch(vt){ case 0x86C75C: return 0xC09908; case 0x86C764: return 0xC0990C; case 0x86C76C: return 0xC09910;
                case 0x86C774: return 0xC09914; case 0x86C77C: return 0xC09918; case 0x86C784: return 0xC0991C; default: return 0; }
}

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(int n=0;n<NPEDS;++n)clean(Ped(n)); clean(Veh()); clean(OldVeh()); clean(ent.data());
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"tasks70-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"tasks70-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("tasks70");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"tasks70-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    HMODULE module{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&Update),&module);
    const uintptr_t enterFirstDll=LocalAddress(module,"?CreateFirstSubTask@CTaskComplexEnterCar@@UAEPAVCTask@@PAVCPed@@@Z");
    const uintptr_t enterControlDll=LocalAddress(module,"?ControlSubTask@CTaskComplexEnterCar@@UAEPAVCTask@@PAVCPed@@@Z");
    const uintptr_t handCtorDll=LocalAddress(module,"??0CHandObject@@QAE@HPAVCPed@@_N@Z");
    if(!enterFirstDll||!enterControlDll||!handCtorDll){Write({{"event","error"},{"reason","missing dll export"},{"exports",{enterFirstDll,enterControlDll,handCtorDll}}});return;}
    FillTraps(std::make_integer_sequence<int,12>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    const std::vector<uint8> savedGroups(Groups(),Groups()+GROUPS_BYTES), savedSeqs(Seqs(),Seqs()+SEQ_BYTES);
    std::array<uint8,8> savedActive{}; std::memcpy(savedActive.data(),reinterpret_cast<void*>(ACTIVE),8);
    std::array<uint8,64> savedOpened{}; std::memcpy(savedOpened.data(),reinterpret_cast<void*>(OPENED),64);
    const uint8 savedOnMission=At<uint8>(reinterpret_cast<void*>(ONMISSION),0); const uint32 savedKills=At(reinterpret_cast<void*>(KILLS),0);
    const int32 savedActiveSeq=At<int32>(reinterpret_cast<void*>(ACTIVE_SEQ),0); const uint16 savedNoTemp=At<uint16>(reinterpret_cast<void*>(NO_TEMP),0);
    auto restoreGlobals=[&]{
        std::memcpy(Groups(),savedGroups.data(),GROUPS_BYTES); std::memcpy(Seqs(),savedSeqs.data(),SEQ_BYTES);
        std::memcpy(reinterpret_cast<void*>(ACTIVE),savedActive.data(),8); std::memcpy(reinterpret_cast<void*>(OPENED),savedOpened.data(),64);
        At<uint8>(reinterpret_cast<void*>(ONMISSION),0)=savedOnMission; At(reinterpret_cast<void*>(KILLS),0)=savedKills;
        At<int32>(reinterpret_cast<void*>(ACTIVE_SEQ),0)=savedActiveSeq; At<uint16>(reinterpret_cast<void*>(NO_TEMP),0)=savedNoTemp;
    };
    auto globalsIntact=[&]{
        return std::memcmp(Groups(),savedGroups.data(),GROUPS_BYTES)==0&&std::memcmp(Seqs(),savedSeqs.data(),SEQ_BYTES)==0
            &&std::memcmp(reinterpret_cast<void*>(ACTIVE),savedActive.data(),8)==0&&std::memcmp(reinterpret_cast<void*>(OPENED),savedOpened.data(),64)==0
            &&At<uint8>(reinterpret_cast<void*>(ONMISSION),0)==savedOnMission&&At(reinterpret_cast<void*>(KILLS),0)==savedKills
            &&At<int32>(reinterpret_cast<void*>(ACTIVE_SEQ),0)==savedActiveSeq&&At<uint16>(reinterpret_cast<void*>(NO_TEMP),0)==savedNoTemp;
    };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch g1(bit_cast<uintptr_t>(&CTheScripts::IsPlayerOnAMission),FakeOnMission), g1e(0x464D50,FakeOnMission);
        EntryPatch g2(bit_cast<uintptr_t>(&CPedGroupMembership::Process),FakeMembershipProcess), g2e(0x5FBA60,FakeMembershipProcess);
        EntryPatch g3(bit_cast<uintptr_t>(&CPedGroupIntelligence::Process),FakeIntelligenceProcess), g3e(0x5FC4A0,FakeIntelligenceProcess);
        EntryPatch g4(bit_cast<uintptr_t>(&CPedGroup::Flush),FakeGroupFlush), g4e(0x5FB790,FakeGroupFlush);
        EntryPatch k1(bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup),FakeGetPedsGroup), k1e(0x5F7E80,FakeGetPedsGroup);
        EntryPatch k2(0x821982,FakePrintf);
        EntryPatch k3(bit_cast<uintptr_t>(&CInterestingEvents::Add),FakeInteresting), k3e(0x602590,FakeInteresting);
        EntryPatch k4(bit_cast<uintptr_t>(&CPedGroupIntelligence::FlushTasks),FakeFlushDll), k4e(0x5F79C0,FakeFlushExe);
        EntryPatch k5(bit_cast<uintptr_t>(static_cast<void(CPedGroupIntelligence::*)(CPed*,bool,const CTask&,bool,const CTask&,eSecondaryTask)>(&CPedGroupIntelligence::SetEventResponseTask)),FakeSetResponse),
                   k5e(0x5F8510,FakeSetResponse);
        EntryPatch d1(bit_cast<uintptr_t>(&CCarEnterExit::ComputeTargetDoorToExit),FakeDoorToExit), d1e(0x64F110,FakeDoorToExit);
        EntryPatch d2(enterFirstDll,FakeEnterFirst), d2e(0x643A60,FakeEnterFirst);
        EntryPatch d3(bit_cast<uintptr_t>(&CTaskComplexEnterCar::CreateSubTask),FakeEnterSub), d3e(0x63E040,FakeEnterSub);
        EntryPatch d4(enterControlDll,FakeEnterControl), d4e(0x63A890,FakeEnterControl);
        EntryPatch h1(bit_cast<uintptr_t>(&IKChainManager_c::IsArmPointing),FakeArmPointing), h1e(0x6182B0,FakeArmPointing);
        EntryPatch h2(bit_cast<uintptr_t>(&CPed::GetEntityThatThisPedIsHolding),FakeHolding), h2e(0x5E02E0,FakeHolding);
        EntryPatch h3(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),FakeBlend), h3e(0x4D4610,FakeBlend);
        EntryPatch h4(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetFinishCallback),FakeSetFinishCb), h4e(0x4CEBE0,FakeSetFinishCb);
        EntryPatch h5(bit_cast<uintptr_t>(static_cast<void*(*)(size_t)>(&CObject::operator new)),FakeObjectNew), h5e(0x5A1EE0,FakeObjectNew);
        EntryPatch h6(handCtorDll,FakeHandCtor), h6e(0x59EEB0,FakeHandCtor);
        EntryPatch h7(bit_cast<uintptr_t>(&CWorld::Add),FakeWorldAdd), h7e(0x563220,FakeWorldAdd);
        EntryPatch h8(bit_cast<uintptr_t>(static_cast<CAnimBlendStaticAssociation*(*)(AssocGroupId,AnimationId)>(&CAnimManager::GetAnimAssociation)),FakeGetAnimAssoc), h8e(0x4D3A60,FakeGetAnimAssoc);
        EntryPatch h9(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,CAnimBlendHierarchy*,int32)>(&CAnimManager::AddAnimation)),FakeAddAnimation), h9e(0x4D4330,FakeAddAnimation);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            const bool isAllocator=cls.starts_with("CPedGroupDefaultTaskAllocator");
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot;
                    // slots libres con 0xCD: los bytes que el constructor no escribe quedan iguales en ambas ejecuciones
                    { auto& lf=At<int32>(PoolHdr(),0xC); static const int32 lf0=lf; lf=lf0;
                      for(int32 k=0;k<PoolCap();++k)if(PoolStates()[k]&0x80)std::memset(PoolStorage()+k*TASK_SLOT,0xCD,TASK_SLOT); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7070u; if(!st)st=1; trace=json::array(); seed=uint32(i); rngCalls=0;
                    missionCalls=0; armCalls=0; handNewCalls=0; freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    // peatones
                    for(int n=0;n<NPEDS;++n){
                        std::memset(peds[n].data(),0,peds[n].size()); std::memset(mats[n].data(),0,mats[n].size());
                        auto* p=Ped(n); p->SetType(ENTITY_TYPE_PED);
                        if(next()%3==0){ p->m_matrix=reinterpret_cast<CMatrixLink*>(mats[n].data()); CMatrix m{}; m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)}; std::memcpy(static_cast<CMatrix*>(p->m_matrix),&m,sizeof(CMatrix)); }
                        else p->SetPosn(uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f));
                        p->m_fCurrentRotation=uni(-3.2f,3.2f);
                        { static const ePedType types[]{PED_TYPE_PLAYER1,PED_TYPE_PLAYER2,PED_TYPE_CIVMALE,PED_TYPE_GANG1,PED_TYPE_GANG2,PED_TYPE_COP}; p->m_nPedType=next()%4 ? types[2+next()%4] : types[next()%2]; }
                        { static const ePedState states[]{PEDSTATE_IDLE,PEDSTATE_IDLE,PEDSTATE_IDLE,PEDSTATE_DIE,PEDSTATE_DEAD}; p->m_nPedState=states[next()%5]; }
                        { static const eWeaponType wt[]{WEAPON_UNARMED,WEAPON_BRASSKNUCKLE,WEAPON_KNIFE,WEAPON_PISTOL,WEAPON_M4,WEAPON_SHOTGUN};
                          p->m_nActiveWeaponSlot=uint8(next()%13); p->m_aWeapons[p->m_nActiveWeaponSlot].m_Type=wt[next()%6]; }
                        At<void*>(p,P_CLUMP)=clumps[0].data();
                    }
                    for(auto& c:clumps){ std::memset(c.data(),0,c.size()); c[0]=2; } // rpCLUMP
                    // tareas falsas
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,fakes[n].size()); At<void*>(fakes[n].data(),0)=fakeVtbl.data();
                        At<CTask*>(fakes[n].data(),FK_CLONE)=nullptr; At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_PAUSE+n; fakes[n][FK_ABORT]=uint8(next()%4!=0); }
                    std::memset(object.data(),0,object.size()); std::memset(group.data(),0,group.size()); std::memset(intel.data(),0,intel.size());
                    std::memset(veh.data(),0,veh.size()); std::memset(oldVeh.data(),0,oldVeh.size()); std::memset(handling.data(),0,handling.size());
                    std::memset(ent.data(),0,ent.size()); std::memset(assoc.data(),0,assoc.size()); std::memset(sassoc.data(),0,sassoc.size()); std::memset(hier.data(),0,hier.size());
                    for(auto& h:hands)std::memset(h.data(),0xAB,h.size());
                    Veh()->SetType(ENTITY_TYPE_VEHICLE); OldVeh()->SetType(ENTITY_TYPE_VEHICLE); static_cast<CEntity*>(static_cast<void*>(ent.data()))->SetType(ENTITY_TYPE_OBJECT);
                    // grupo falso: pertenencia e inteligencia apuntan al grupo; miembros 0..7 = ped(k) o nulo;
                    // pares por defecto con peatones distintos entre si (miembro k, nulo o ped(8+k))
                    auto* g=group.data(); At<void*>(g,G_MEMBERSHIP)=g; At<void*>(g,G_INTEL)=g;
                    auto* members=reinterpret_cast<CPed**>(g+G_MEMBERS); auto* pairs=reinterpret_cast<CPedTaskPair*>(g+G_PAIRS);
                    for(int k=0;k<8;++k)members[k]=next()%3 ? Ped(k) : nullptr;
                    for(int k=0;k<8;++k){ const uint32 r=next()%5; pairs[k].Ped=r<3 ? members[k] : r==3 ? nullptr : Ped(8+k); pairs[k].Task=nullptr; }
                    g[0x4]=uint8(next()%2); // m_bMembersEnterLeadersVehicle

                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches}}; json ret=nullptr; std::vector<CTask*> roots;

                    if(isAllocator&&nm=="GetType") {
                        ret=reinterpret_cast<int32(__thiscall*)(uintptr_t)>(target)(AllocatorInstance(fn.vt));
                    }
                    else if(isAllocator) {
                        CPed* argPed=nullptr;
                        switch(next()%6){ case 0: case 1: break; case 2: case 3: argPed=pairs[next()%8].Ped; break; case 4: argPed=members[7]; break; default: argPed=Ped(next()%NPEDS); }
                        const auto pedsBefore=peds;
                        reinterpret_cast<void(__thiscall*)(uintptr_t,CPedGroup*,CPed*)>(target)(AllocatorInstance(fn.vt),reinterpret_cast<CPedGroup*>(g),argPed);
                        for(auto* h:hooks)h->State(false);
                        auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                        for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                        json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t,-1));
                        json refs=json::array(); for(int n=0;n<NPEDS;++n)refs.push_back(RefList(Ped(n)));
                        bool pedsSame=true; for(int n=0;n<NPEDS;++n)pedsSame=pedsSame&&EntHash(peds[n])==EntHash(pedsBefore[n]);
                        out["arg_ped"]=Name(argPed); out["group"]=Region(g,group.size()); out["tasks"]=tasks; out["refs"]=refs; out["peds_intact"]=pedsSame;
                        for(int k=0;k<8;++k)if(pairs[k].Task&&freshIds.contains(uintptr_t(pairs[k].Task))&&std::ranges::find(roots,pairs[k].Task)==roots.end())roots.push_back(pairs[k].Task);
                        ret=int(fresh.size());
                    }
                    else if(cls=="CPedGroups") {
                        // grupos reales con contenido falso: miembros (punteros no desreferenciados), DM, bandera de mision, activos
                        for(int k=0;k<8;++k){ auto* gr=Groups()+k*GROUP_SZ; auto* m=reinterpret_cast<CPed**>(gr+G_MEMBERS);
                            const uint32 mode=next()%4; for(int j=0;j<8;++j)m[j]=mode==0 ? nullptr : (next()%3 ? Ped((k+j)%NPEDS) : nullptr);
                            At<int32>(gr,G_DM)=int32(next()%12)-1; gr[G_MISSION]=uint8(next()%3==0); }
                        for(int k=0;k<8;++k)At<uint8>(reinterpret_cast<void*>(ACTIVE),k)=uint8(next()%4!=0);
                        At<uint8>(reinterpret_cast<void*>(ONMISSION),0)=uint8(next()%2);
                        { static const uint32 kills[]{0,7,8,8,9,1}; At(reinterpret_cast<void*>(KILLS),0)=next()%4 ? kills[next()%6] : next()%20; }
                        missionMask=next(); if(next()%4==0)missionMask=next()%2 ? 0xFFFFFFFFu : 0u; clearMask=next()%3 ? next()&0xFF : 0;
                        if(nm=="Process")reinterpret_cast<void(__cdecl*)()>(target)();
                        else { const int32 id=int32(next()%8); out["id"]=id; reinterpret_cast<void(__cdecl*)(int32)>(target)(id); }
                        json gs=json::array();
                        for(int k=0;k<8;++k){ auto* gr=Groups()+k*GROUP_SZ; gs.push_back({At<int32>(gr,G_DM),gr[G_MISSION],Region(gr+G_MEMBERS,32)}); }
                        out["groups"]=gs; out["groups_hash"]=Hash(Groups(),GROUPS_BYTES);
                        out["active"]=ReversingProbe::Bytes(reinterpret_cast<void*>(ACTIVE),8);
                        out["on_mission"]=At<uint8>(reinterpret_cast<void*>(ONMISSION),0); out["kills"]=At(reinterpret_cast<void*>(KILLS),0);
                    }
                    else if(cls=="CTaskAllocatorKillThreatsBasicRandomGroup") {
                        auto* o=object.data(); At<uintptr_t>(o,0)=0x86C80C;
                        for(size_t b=4;b<sizeof(CTaskAllocatorKillThreatsBasicRandomGroup);++b)o[b]=uint8(next());
                        if(nm=="GetType") ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        else {
                            // grupo de la amenaza: ms_groups[k] con miembros falsos (ped 8..15); los miembros del grupo propio son ped 0..7
                            const int k=int(next()%8); auto* threatGroup=Groups()+k*GROUP_SZ; auto* tm=reinterpret_cast<CPed**>(threatGroup+G_MEMBERS);
                            for(int j=0;j<8;++j)tm[j]=next()%3 ? Ped(8+j) : nullptr;
                            const uint32 mode=next()%8; // 0-2 amenaza sin grupo, 3-6 grupo distinto, 7 mismo grupo
                            CPed* threat=next()%10 ? Ped(8+next()%8) : nullptr; At<CPed*>(o,4)=threat;
                            pedsGroupRet=mode<3 ? 0 : uintptr_t(threatGroup);
                            auto* in=intel.data(); At<void*>(in,0)=mode==7 ? (void*)threatGroup : (void*)g;
                            auto* pp=reinterpret_cast<CPedTaskPair*>(in+I_PAIRS); auto* sp=reinterpret_cast<CPedTaskPair*>(in+I_SECONDARY);
                            for(int j=0;j<8;++j){ pp[j].Ped=next()%3 ? members[j] : nullptr; pp[j].Task=next()%3==0 ? Fake(j%FAKES) : nullptr;
                                                  sp[j].Ped=next()%3 ? members[j] : nullptr; sp[j].Task=next()%4==0 ? Fake((j+3)%FAKES) : nullptr; }
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(target)(o,in);
                            out["intel"]=Region(in,intel.size()); out["threat_group"]=Region(threatGroup,GROUP_SZ);
                            json refs=json::array(); for(int n=0;n<NPEDS;++n)refs.push_back(RefList(Ped(n))); out["refs"]=refs;
                        }
                        out["object"]=Region(o,sizeof(CTaskAllocatorKillThreatsBasicRandomGroup),true); out["group"]=Region(g,group.size());
                    }
                    else if(cls=="CTaskComplexDragPedFromCar") {
                        auto* o=object.data(); At<uintptr_t>(o,0)=0x86EB6C; At<CTask*>(o,8)=Fake(0);
                        for(size_t b=0x10;b<0x50;++b)o[b]=uint8(next());
                        if(next()%3)o[0x28]=0; // m_NumGettingInSet
                        CPed* dragged=Ped(0); CPed* dragger=Ped(1);
                        if(next()%4){ At<CVehicle*>(o,0xC)=OldVeh(); OldVeh()->RegisterReference(reinterpret_cast<CEntity**>(o+0xC)); } else At<CVehicle*>(o,0xC)=nullptr;
                        if(next()%6){ At<CPed*>(o,0x50)=dragged; dragged->RegisterReference(reinterpret_cast<CEntity**>(o+0x50)); } else At<CPed*>(o,0x50)=nullptr;
                        dragged->m_pVehicle=next()%5 ? Veh() : nullptr; At<uint32>(dragged,P_FLAGS2)=next()%4 ? (next()|0x100u) : (next()&~0x100u);
                        auto* v=Veh(); v->m_pHandlingData=reinterpret_cast<tHandlingData*>(handling.data());
                        At<uint32>(handling.data(),0xCC)=next()%3==0 ? (next()|0x200u) : (next()&~0x200u);
                        { static const eVehicleType types[]{VEHICLE_TYPE_AUTOMOBILE,VEHICLE_TYPE_BIKE,VEHICLE_TYPE_QUAD,VEHICLE_TYPE_BMX,VEHICLE_TYPE_HELI,VEHICLE_TYPE_MTRUCK};
                          v->m_nVehicleType=types[next()%6]; v->m_nVehicleSubType=types[next()%6]; }
                        v->m_nMaxPassengers=uint8(next()%9);
                        { const uint32 m=next()%4; v->m_pDriver=m==0 ? dragged : m==1 ? Ped(2) : m==2 ? nullptr : dragger; }
                        for(auto& ps:v->m_apPassengers)ps=next()%5==0 ? Ped(3+next()%3) : nullptr;
                        if(next()%3==0)v->m_apPassengers[next()%8]=dragged;
                        { static const int doors[]{8,9,10,11,-1,0}; doorRet=doors[next()%6]; }
                        auto* r=reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,dragger);
                        ret=Name(r); out["object"]=Region(o,0x54,true);
                        out["refs"]=json::array({RefList(Veh()),RefList(OldVeh()),RefList(dragged),RefList(dragger)});
                        out["fakes"]=json::array(); for(int n=0;n<FAKES;++n)out["fakes"].push_back(Region(fakes[n].data(),fakes[n].size(),true));
                    }
                    else if(cls=="CTaskSequences") {
                        for(int s=0;s<64;++s){ auto* e=Seqs()+s*0x40;
                            for(size_t b=0xC;b<0x40;++b)e[b]=uint8(next());
                            const bool used=next()%4==0; for(int t=0;t<8;++t)At<CTask*>(e,0x10+4*t)=used&&next()%2 ? Fake(next()%FAKES) : nullptr;
                            At<uint8>(reinterpret_cast<void*>(OPENED),s)=uint8(next()%3==0); }
                        At<int32>(reinterpret_cast<void*>(ACTIVE_SEQ),0)=int32(next()%66)-1;
                        reinterpret_cast<void(__cdecl*)()>(target)();
                        out["seqs"]=Hash(Seqs(),SEQ_BYTES); out["opened"]=ReversingProbe::Bytes(reinterpret_cast<void*>(OPENED),64);
                        out["active_seq"]=At<int32>(reinterpret_cast<void*>(ACTIVE_SEQ),0);
                        json heads=json::array(); for(int s=0;s<64;++s)heads.push_back(Region(Seqs()+s*0x40+0xC,0x34)); out["seq_words"]=heads;
                    }
                    else if(cls=="CTaskSimpleFinishBrain"||cls=="CTaskSimpleScratchHead") {
                        auto* o=object.data(); At<uintptr_t>(o,0)=fn.vt; for(size_t b=4;b<0x24;++b)o[b]=uint8(next());
                        if(nm=="Clone"){
                            auto* r=reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o);
                            for(auto* h:hooks)h->State(false);
                            auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                            for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                            json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t,-1)); out["tasks"]=tasks;
                            ret=Name(r); if(r&&freshIds.contains(uintptr_t(r)))roots.push_back(r);
                        }
                        else if(nm=="GetTaskType") ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        else ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,Ped(0));
                        out["object"]=Region(o,0x24,true);
                    }
                    else if(cls=="CTaskSimpleLeaveGroup") {
                        auto* o=object.data(); std::memset(o,0xCD,0x20);
                        if(nm=="Constructor"){
                            auto* r=reinterpret_cast<void*(__thiscall*)(void*)>(target)(o);
                            ret=Name(r); out["type"]=reinterpret_cast<CTask*>(o)->GetTaskType(); // vptr del EXE o de la DLL: se compara el tipo
                        } else {
                            At<uintptr_t>(o,0)=0x859CFC; At<uint32>(o,4)=next()%2 ? uint32(uintptr_t(Fake(next()%FAKES))) : 0;
                            reinterpret_cast<void(__thiscall*)(void*)>(target)(o);
                        }
                        out["object"]=Region(o,0x20,true);
                    }
                    else if(cls=="CTaskSimplePlayHandSignalAnim") {
                        auto* o=object.data(); At<uintptr_t>(o,0)=0x86D5B8;
                        const bool proc=nm=="ProcessPed";
                        At<void*>(o,8)=proc ? (next()%3==0 ? (void*)assoc.data() : nullptr) : (next()%2 ? reinterpret_cast<void*>(SENTINEL+1) : nullptr);
                        o[0xC]=uint8(next()&0xFE)|uint8(next()%4==0);
                        At<int32>(o,0x10)=next()%3==0 ? -1 : int32(next()%400);
                        At<float>(o,0x14)=uni(-8.f,8.f); o[0x18]=uint8(next()%2); o[0x19]=uint8(next()); o[0x1A]=uint8(next()); o[0x1B]=uint8(next());
                        At<uint32>(o,0x1C)=SENTINEL+2; At<uint32>(o,0x20)=SENTINEL+3;
                        { static const uint16 counts[]{0,148,149,150,151,0xFFFF,10}; At<uint16>(reinterpret_cast<void*>(NO_TEMP),0)=next()%4 ? counts[next()%7] : uint16(next()); }
                        armMask=next()%3 ? 0 : next(); holdRet=next()%5==0 ? uintptr_t(ent.data()) : 0; handNewMask=next()%5==0 ? next()&3 : 0;
                        CPed* p=Ped(0);
                        if(proc) ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,p);
                        else reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,p);
                        out["object"]=Region(o,0x24,true); out["no_temp"]=At<uint16>(reinterpret_cast<void*>(NO_TEMP),0);
                        out["hands"]=json::array({Hash(hands[0].data(),hands[0].size()),Hash(hands[1].data(),hands[1].size())});
                        out["assoc"]=Hash(assoc.data(),assoc.size()); out["sassoc"]=Region(sassoc.data(),sassoc.size());
                    }
                    else if(cls=="CTimeModelInfo") {
                        auto* o=object.data(); At<uintptr_t>(o,0)=0x85BCB0; for(size_t b=4;b<0x24;++b)o[b]=uint8(next());
                        if(nm=="GetModelType") ret=int(reinterpret_cast<uint8(__thiscall*)(void*)>(target)(o));
                        else ret=Name(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o));
                        out["object"]=Region(o,0x24,true);
                    }
                    for(auto* h:hooks)h->State(false);
                    // limpieza: tareas nuevas alcanzables (raices) y referencias; despues el pool debe quedar como antes
                    const size_t traceBefore=trace.size();
                    for(auto* t:roots)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    CleanRefs();
                    out["ret"]=ret; out["trace"]=trace; out["delete_trace"]=delTrace; out["rng"]=rngCalls; out["clean"]=Occupancy()==occ;
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
    CleanRefs(); restoreGlobals();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&globalsIntact();
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
