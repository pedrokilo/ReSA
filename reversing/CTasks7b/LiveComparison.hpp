#pragma once
// Banco dentro del motor: tareas complejas B (lista de 7, 35 funciones).
// CTaskComplexKillPedOnFootStealth, CTaskComplexLeaveAnyCar, CTaskComplexSeekEntityAiming y
// CTaskComplexSmartFleeEntity (constructor, destructor, Clone, GetTaskType, ControlSubTask, CreateFirstSubTask,
// CreateNextSubTask) y CTaskComplexPartnerGreet (constructor, destructor, Clone, GetTaskType, CreateFirstSubTask,
// GetPartnerSequence, StreamRequiredAnims).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 4 peatones (con inteligencia propia), 2 vehiculos (vtable falsa), tareas falsas (vtable trazada).
// Se simulan y trazan: CPed::Say, CTaskComplexKillPedOnFoot::CreateFirstSubTask/ControlSubTask/CreateSubTask
// (0x62B490/0x626260/0x625E70, sin reconstruir), CPedGeometryAnalyser::CanPedTargetPed,
// CPedIntelligence::CanSeeEntityWithLights, CTaskComplexPartner::CreateFirstSubTask (0x681F20),
// CTaskComplexGangLeader::ShouldLoadGangAnims, CAnimManager::GetAnimationBlockIndex/AddAnimBlockRef/RemoveAnimBlockRef,
// CStreaming::RequestModel, CTaskSimpleUseGun::ControlGun, CInterestingEvents::Add, IKChainManager_c::IsLooking/LookAt,
// CTaskComplexSmartFleePoint::SetFleePosition, CEventGroup::Add y el RNG.
// Pool de tareas: m_LastFreeSlot y estados se restauran y los slots libres se rellenan con 0xCD en cada ejecucion.
// CTimer::m_snTimeInMilliseconds y CPedAcquaintanceScanner::ms_fThresholdDotProduct (0xC0B034) se guardan y se
// restauran. Se compara la memoria de los fixtures (punteros traducidos a nombres), las tareas nuevas del pool, las
// referencias, el retorno y la traza.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/Task.h"
#include "Tasks/TaskComplexSequence.h"
#include "TaskComplexKillPedOnFootStealth.h"
#include "TaskComplexLeaveAnyCar.h"
#include "TaskComplexPartnerGreet.h"
#include "TaskComplexSeekEntityAiming.h"
#include "TaskComplexSmartFleeEntity.h"
#include "TaskComplexSmartFleePoint.h"
#include "TaskComplexGangLeader.h"
#include "TaskSimpleUseGun.h"
#include "PedGeometryAnalyser.h"
#include "PedIntelligence.h"
#include "InterestingEvents.h"
#include "Ragdoll/IKChainManager.h"
#include "Events/EventGroup.h"
#include "EventSeenPanickedPed.h"
#include "Animation/AnimManager.h"
#include "Streaming.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks7bLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }

constexpr int NPEDS=4, NVEH=2, FAKES=4;
constexpr size_t FAKE_SIZE=0x50, FK_ABORT=0x40, FK_SUB=0x44, FK_TYPE=0x48, FK_CLONE=0x4C;
// desplazamientos crudos del EXE
constexpr size_t E_POS=4, E_MATRIX=0x14, E_TYPE=0x36;                                            // CEntity
constexpr size_t P_FLAGS=0x46C, P_INTEL=0x47C, P_ANIMGROUP=0x4D4, P_HEALTH=0x540, P_VEHICLE=0x58C; // CPed
constexpr size_t I_ATTACK=0x18;                                                                    // CPedIntelligence: tarea secundaria de ataque
constexpr uintptr_t TIMER=0xB7CB84, DOT_THRESHOLD=0xC0B034, INTERESTING=0xC0B058, IKCHAIN=0xC15448;
constexpr uintptr_t VT_KPS=0x86D8C0, VT_LAC=0x858EB0, VT_PG=0x87078C, VT_SEA=0x870C94, VT_SFE=0x86F784;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<uint8,0x100> object{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }

// pool de tareas (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot)
inline uint8* PoolHdr() { return reinterpret_cast<uint8*>(GetTaskPool()); }
inline uint8* PoolStorage() { return *reinterpret_cast<uint8**>(PoolHdr()); }
inline uint8* PoolStates() { return *reinterpret_cast<uint8**>(PoolHdr()+4); }
inline int32 PoolCap() { return *reinterpret_cast<int32*>(PoolHdr()+8); }
inline int32& PoolLast() { return At<int32>(PoolHdr(),0xC); }
constexpr size_t TASK_SLOT=0x80;
inline bool IsPoolTask(uintptr_t v) {
    const auto s=uintptr_t(PoolStorage()); if(v<s||v>=s+size_t(PoolCap())*TASK_SLOT||(v-s)%TASK_SLOT)return false;
    return !(PoolStates()[(v-s)/TASK_SLOT]&0x80);
}

inline std::array<void*,16> fakeVtbl{};
inline std::array<void*,96> vehVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(vehVtbl.data()))return "veh_vt";
    if(v==INTERESTING)return "interesting_events"; if(v==IKCHAIN)return "ik_chain_manager";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n);
                              if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"object"))return *s;
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); for(auto& v:vehs)r.push_back(RefList(v.data())); return r; }

// tarea serializada sin vptr (se compara el tipo); los bytes 0x39..0x3B de CTaskComplexSequence son relleno
inline json SerTask(const void* p,int depth,size_t bytes=TASK_SLOT) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType(); const auto* w=static_cast<const uint32*>(p);
    json words=json::array();
    for(size_t k=1;k<bytes/4;++k){ const uint32 v=w[k];
        if(type==TASK_COMPLEX_SEQUENCE&&k*4==0x38)words.push_back(v&0xFF);
        else if(depth>=0&&depth<3&&IsPoolTask(v)&&!freshIds.contains(v))words.push_back(SerTask(reinterpret_cast<const void*>(uintptr_t(v)),depth+1));
        else words.push_back(Name(v)); }
    return {{"type",type},{"words",words}};
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{};
inline uint8 canTargetRet{}, shouldLoadRet{}, isLookingRet{}; inline float canSeeRet{}; inline int32 blockIdx{};
inline uintptr_t kpofFirstRet{}, kpofControlRet{}, kpofSubRet{}, partnerFirstRet{};

// tareas falsas
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(t)}); return At<CTask*>(t,FK_CLONE); }
inline CTask* __fastcall FSub(CTask* t,void*) { return At<CTask*>(t,FK_SUB); }
inline bool __fastcall FSimple(CTask*,void*) { return true; }
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) { trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e)}); return At<uint8>(t,FK_ABORT)!=0; }
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<size_t S,int... N> void FillTraps(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }

// dependencias simuladas
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline int16 __fastcall FakeSay(CPed* p,void*,int ctx,uint32 delay,float prob,uint32 overrideSilence,uint32 forceAudible,uint32 frontEnd) {
    trace.push_back({"say",Name(p),ctx,delay,F(prob),overrideSilence&0xFF,forceAudible&0xFF,frontEnd&0xFF}); return 0;
}
inline CTask* __fastcall FakeKpofFirst(void* t,void*,CPed* p) { trace.push_back({"kpof_create_first",Name(t),Name(p)}); return reinterpret_cast<CTask*>(kpofFirstRet); }
inline CTask* __fastcall FakeKpofControl(void* t,void*,CPed* p) { trace.push_back({"kpof_control",Name(t),Name(p)}); return reinterpret_cast<CTask*>(kpofControlRet); }
inline CTask* __fastcall FakeKpofSub(void* t,void*,int32 type,CPed* p) { trace.push_back({"kpof_create_sub",Name(t),type,Name(p)}); return reinterpret_cast<CTask*>(kpofSubRet); }
inline bool __cdecl FakeCanTarget(CPed& a,CPed& b,uint32 dir) { trace.push_back({"can_ped_target_ped",Name(&a),Name(&b),dir&0xFF,canTargetRet}); return canTargetRet!=0; }
inline float __fastcall FakeCanSee(void* in,void*,CEntity* e,int32 u) { trace.push_back({"can_see_with_lights",Name(in),Name(e),u,F(canSeeRet)}); return canSeeRet; }
inline CTask* __fastcall FakePartnerFirst(void* t,void*,CPed* p) { trace.push_back({"partner_create_first",Name(t),Name(p)}); return reinterpret_cast<CTask*>(partnerFirstRet); }
inline bool __cdecl FakeShouldLoad() { trace.push_back({"should_load_gang_anims",shouldLoadRet}); return shouldLoadRet!=0; }
inline int32 __cdecl FakeBlockIndex(const char* name) { std::string s; for(int k=0;k<16&&name[k];++k)s+=name[k]; trace.push_back({"anim_block_index",s,blockIdx}); return blockIdx; }
inline void __cdecl FakeAddBlockRef(int32 i) { trace.push_back({"add_anim_block_ref",i}); }
inline void __cdecl FakeRemoveBlockRef(int32 i) { trace.push_back({"remove_anim_block_ref",i}); }
inline void __cdecl FakeRequestModel(int32 id,int32 flags) { trace.push_back({"request_model",id,flags}); }
inline bool __fastcall FakeControlGun(void* g,void*,CPed* p,CEntity* e,uint32 cmd) { trace.push_back({"control_gun",Name(g),Name(p),Name(e),cmd&0xFF}); return true; }
inline void __fastcall FakeInteresting(void* ie,void*,int32 type,CEntity* e) { trace.push_back({"interesting_event",Name(ie),type,Name(e)}); }
inline bool __fastcall FakeIsLooking(void* m,void*,CPed* p) { trace.push_back({"is_looking",Name(m),Name(p),isLookingRet}); return isLookingRet!=0; }
inline void __fastcall FakeLookAt(void* m,void*,const char* purpose,CPed* p,CEntity* e,int32 time,int32 bone,CVector* off,uint32 torso,float speed,int32 blend,int32 prio,uint32 force) {
    std::string s; for(int k=0;k<32&&purpose&&purpose[k];++k)s+=purpose[k];
    trace.push_back({"look_at",Name(m),s,Name(p),Name(e),time,bone,Name(off),torso&0xFF,F(speed),blend,prio,force&0xFF});
}
inline int8 __fastcall FakeSetFlee(void* t,void*,const CVector* pos,float dist,uint32 scream) { trace.push_back({"set_flee_position",Name(t),Name(pos),F(pos->x),F(pos->y),F(pos->z),F(dist),scream&0xFF}); return 0; }
inline CEvent* __fastcall FakeEventAdd(void* grp,void*,CEvent* e,uint32 valid) {
    const int32 type=e ? int32(e->GetEventType()) : -1;
    trace.push_back({"event_group_add",uint32(uintptr_t(grp)==uintptr_t(GetEventGlobalGroup())),type,valid&0xFF,type==EVENT_SEEN_PANICKED_PED ? Name(static_cast<CEventSeenPanickedPed*>(e)->m_ped) : json(nullptr)});
    return nullptr;
}

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=35;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexKillPedOnFootStealth","Clone",0x623400,0x86D8C0,1,256},
    {"CTaskComplexKillPedOnFootStealth","Constructor",0x620F70,0,-1,256},
    {"CTaskComplexKillPedOnFootStealth","ControlSubTask",0x62B840,0x86D8C0,10,1024},
    {"CTaskComplexKillPedOnFootStealth","CreateFirstSubTask",0x62B810,0x86D8C0,9,128},
    {"CTaskComplexKillPedOnFootStealth","CreateNextSubTask",0x62B790,0x86D8C0,8,512},
    {"CTaskComplexKillPedOnFootStealth","Destructor",0x620FF0,0,-1,128},
    {"CTaskComplexKillPedOnFootStealth","GetTaskType",0x620FE0,0x86D8C0,4,64},
    {"CTaskComplexLeaveAnyCar","Clone",0x4211C0,0x858EB0,1,256},
    {"CTaskComplexLeaveAnyCar","Constructor",0x421150,0,-1,256},
    {"CTaskComplexLeaveAnyCar","ControlSubTask",0x63BDC0,0x858EB0,10,64},
    {"CTaskComplexLeaveAnyCar","CreateFirstSubTask",0x63BCE0,0x858EB0,9,256},
    {"CTaskComplexLeaveAnyCar","CreateNextSubTask",0x63BCD0,0x858EB0,8,64},
    {"CTaskComplexLeaveAnyCar","Destructor",0x421270,0,-1,128},
    {"CTaskComplexLeaveAnyCar","GetTaskType",0x421240,0x858EB0,4,64},
    {"CTaskComplexPartnerGreet","Clone",0x684DA0,0x87078C,1,256},
    {"CTaskComplexPartnerGreet","Constructor",0x684210,0,-1,256},
    {"CTaskComplexPartnerGreet","CreateFirstSubTask",0x6825A0,0x87078C,9,64},
    {"CTaskComplexPartnerGreet","Destructor",0x684280,0,-1,128},
    {"CTaskComplexPartnerGreet","GetPartnerSequence",0x682630,0x87078C,13,1024},
    {"CTaskComplexPartnerGreet","GetTaskType",0x681E00,0x87078C,4,64},
    {"CTaskComplexPartnerGreet","StreamRequiredAnims",0x6825B0,0x87078C,11,512},
    {"CTaskComplexSeekEntityAiming","Clone",0x695820,0x870C94,1,256},
    {"CTaskComplexSeekEntityAiming","Constructor",0x694B90,0,-1,256},
    {"CTaskComplexSeekEntityAiming","ControlSubTask",0x694C70,0x870C94,10,1024},
    {"CTaskComplexSeekEntityAiming","CreateFirstSubTask",0x695D30,0x870C94,9,256},
    {"CTaskComplexSeekEntityAiming","CreateNextSubTask",0x695C20,0x870C94,8,256},
    {"CTaskComplexSeekEntityAiming","Destructor",0x694C10,0,-1,128},
    {"CTaskComplexSeekEntityAiming","GetTaskType",0x694C00,0x870C94,4,64},
    {"CTaskComplexSmartFleeEntity","Clone",0x65CF40,0x86F784,1,256},
    {"CTaskComplexSmartFleeEntity","Constructor",0x65C430,0,-1,256},
    {"CTaskComplexSmartFleeEntity","ControlSubTask",0x65C780,0x86F784,10,1024},
    {"CTaskComplexSmartFleeEntity","CreateFirstSubTask",0x65C6F0,0x86F784,9,512},
    {"CTaskComplexSmartFleeEntity","CreateNextSubTask",0x65C680,0x86F784,8,256},
    {"CTaskComplexSmartFleeEntity","Destructor",0x65C4D0,0,-1,128},
    {"CTaskComplexSmartFleeEntity","GetTaskType",0x65C4C0,0x86F784,4,64},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks7b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks7b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks7b");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks7b-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // indices de bloques de animacion reales (solo se lee su byte IsLoaded): los 10 primeros y el de "gangs"
    std::array<int32,11> blocks{}; for(int k=0;k<10;++k)blocks[k]=k; blocks[10]=CAnimManager::GetAnimationBlockIndex("gangs");
    if(blocks[10]<0||blocks[10]>=NUM_ANIM_BLOCKS)blocks[10]=0;
    FillTraps(fakeVtbl,std::make_integer_sequence<int,16>{}); FillTraps(vehVtbl,std::make_integer_sequence<int,96>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{DOT_THRESHOLD,4}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    const int32 lf0=PoolLast();
    const std::vector<uint8> statesStart(PoolStates(),PoolStates()+PoolCap());
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch s1(bit_cast<uintptr_t>(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say)),FakeSay), s1e(0x5EFFE0,FakeSay);
        EntryPatch k1e(0x62B490,FakeKpofFirst), k2e(0x626260,FakeKpofControl), k3e(0x625E70,FakeKpofSub), p1e(0x681F20,FakePartnerFirst);
        EntryPatch g1(bit_cast<uintptr_t>(&CPedGeometryAnalyser::CanPedTargetPed),FakeCanTarget), g1e(0x5F1C40,FakeCanTarget);
        EntryPatch g2(bit_cast<uintptr_t>(&CPedIntelligence::CanSeeEntityWithLights),FakeCanSee), g2e(0x605550,FakeCanSee);
        EntryPatch a1(bit_cast<uintptr_t>(&CTaskComplexGangLeader::ShouldLoadGangAnims),FakeShouldLoad), a1e(0x65E7F0,FakeShouldLoad);
        EntryPatch a2(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)),FakeBlockIndex), a2e(0x4D3990,FakeBlockIndex);
        EntryPatch a3(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef),FakeAddBlockRef), a3e(0x4D3FB0,FakeAddBlockRef);
        EntryPatch a4(bit_cast<uintptr_t>(&CAnimManager::RemoveAnimBlockRef),FakeRemoveBlockRef), a4e(0x4D3FD0,FakeRemoveBlockRef);
        EntryPatch a5(bit_cast<uintptr_t>(&CStreaming::RequestModel),FakeRequestModel), a5e(0x4087E0,FakeRequestModel);
        EntryPatch u1(bit_cast<uintptr_t>(&CTaskSimpleUseGun::ControlGun),FakeControlGun), u1e(0x61E040,FakeControlGun);
        EntryPatch i1(bit_cast<uintptr_t>(&CInterestingEvents::Add),FakeInteresting), i1e(0x602590,FakeInteresting);
        EntryPatch i2(bit_cast<uintptr_t>(&IKChainManager_c::IsLooking),FakeIsLooking), i2e(0x6181A0,FakeIsLooking);
        EntryPatch i3(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),FakeLookAt), i3e(0x618970,FakeLookAt);
        EntryPatch f1(bit_cast<uintptr_t>(&CTaskComplexSmartFleePoint::SetFleePosition),FakeSetFlee), f1e(0x65C3C0,FakeSetFlee);
        EntryPatch e1(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),FakeEventAdd), e1e(0x4AB420,FakeEventAdd);

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
                    PoolLast()=lf0;
                    if(Occupancy()==occupancyStart)std::memcpy(PoolStates(),statesStart.data(),statesStart.size());
                    for(int32 k=0;k<PoolCap();++k)if(PoolStates()[k]&0x80)std::memset(PoolStorage()+k*TASK_SLOT,0xCD,TASK_SLOT);
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7B7Bu; if(!st)st=1; trace=json::array(); seed=uint32(i);
                    rngCalls=0; freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    const uint32 now=At<uint32>(reinterpret_cast<void*>(TIMER),0);
                    At<float>(reinterpret_cast<void*>(DOT_THRESHOLD),0)=next()%4 ? uni(-1.f,1.f) : 0.5f;
                    // peatones: matriz (siempre para el 0), inteligencia propia, salud, grupo de animacion
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                        p[E_TYPE]=uint8(3|(next()%8)<<3); At<void*>(p,P_INTEL)=intels[n].data();
                        if(n==0||next()%4){ At<void*>(p,E_MATRIX)=pedMats[n].data();
                            for(int r=0;r<3;++r)At<CVector>(pedMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            At<CVector>(pedMats[n].data(),0x30)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)}; }
                        else At<CVector>(p,E_POS)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                        At<float>(p,P_HEALTH)=next()%3 ? uni(1.f,100.f) : (next()%2 ? 0.f : -5.f);
                        At<int32>(p,P_ANIMGROUP)=int32(next()%60);
                    }
                    // posicion compartida ocasional (direccion nula al normalizar)
                    if(next()%8==0){ const int b=1+int(next()%(NPEDS-1)); At<CVector>(Ped(b),E_POS)=Ped(0)->GetPosition(); if(At<void*>(Ped(b),E_MATRIX))At<CVector>(pedMats[b].data(),0x30)=Ped(0)->GetPosition(); }
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%8)<<3); At<void*>(v,E_MATRIX)=vehMats[n].data();
                        for(int r=0;r<3;++r)At<CVector>(vehMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                    }
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,FAKE_SIZE); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_PAUSE+n; fakes[n][FK_ABORT]=uint8(next()%2); }
                    std::memset(object.data(),0xCD,object.size());
                    canTargetRet=uint8(next()%3!=0); shouldLoadRet=uint8(next()%2); isLookingRet=uint8(next()%3==0);
                    { const uint32 m=next()%6; canSeeRet=m==0 ? 0.f : m==1 ? -0.f : m==2 ? -1.f : m==3 ? qnan : uni(0.f,1.f); }
                    blockIdx=blocks[next()%blocks.size()];
                    kpofFirstRet=uintptr_t(Fake(2)); kpofControlRet=uintptr_t(Fake(3)); kpofSubRet=next()%2 ? uintptr_t(Fake(2)) : 0; partnerFirstRet=next()%2 ? uintptr_t(Fake(2)) : 0;

                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches}}; json ret=nullptr; std::vector<CTask*> roots;
                    auto* o=object.data(); auto* p0=Ped(0);
                    auto collectFresh=[&](CTask* r){
                        for(auto* h:hooks)h->State(false);
                        auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                        for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                        json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t,-1)); out["tasks"]=tasks;
                        if(r&&freshIds.contains(uintptr_t(r)))roots.push_back(r);
                    };
                    auto setTask=[&](uintptr_t vtbl,size_t size){ for(size_t b=0;b<size;++b)o[b]=uint8(next()); At<uintptr_t>(o,0)=vtbl; At<uint32>(o,4)=0; At<uint32>(o,8)=0; };
                    auto setSub=[&](const int32* types,size_t n,bool allowNull){
                        if(allowNull&&next()%8==0){ At<CTask*>(o,8)=nullptr; return; }
                        At<CTask*>(o,8)=Fake(0); At<int32>(fakes[0].data(),FK_TYPE)=types[next()%n]; };
                    auto typeOf=[&](void* p){ return reinterpret_cast<CTask*>(p)->GetTaskType(); };
                    auto callTask=[&](CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    auto callClone=[&]{ return reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); };
                    auto callVoid=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); };
                    auto inVeh=[&](CPed* p,bool on){ auto& fl=At<uint32>(p,P_FLAGS); fl=on ? (fl|0x100u) : (fl&~0x100u); };
                    auto pickEntity=[&](bool allowVeh)->CEntity*{ const uint32 m=next()%6; if(m==0)return nullptr; if(allowVeh&&m==1)return Veh(int(next()%NVEH)); return Ped(1+int(next()%(NPEDS-1))); };
                    auto entPos=[&](CEntity* e){ return e ? e->GetPosition() : CVector{}; };

                    if(nm=="GetTaskType") {
                        setTask(fn.vt,0x40);
                        ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        out["object"]=Region(o,0x40,true);
                    }
                    else if(cls=="CTaskComplexKillPedOnFootStealth") {
                        // base 0x38 bytes (+0x10 objetivo con referencia); +0x38 posicion del objetivo, +0x44 instante inalcanzable
                        if(nm=="Constructor"){
                            CPed* t=static_cast<CPed*>(next()%4 ? pickEntity(false) : nullptr);
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CPed*)>(target)(o,t));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x48,true); out["type"]=typeOf(o);
                        } else {
                            setTask(VT_KPS,0x48);
                            CPed* t=static_cast<CPed*>(pickEntity(false)); At<CPed*>(o,0x10)=t; Reg(t,o+0x10);
                            { const uint32 m=next()%6; At<CVector>(o,0x38)=m==0 ? CVector{0.f,0.f,-9999.9004f} : m==1 ? CVector{1.f,2.f,std::bit_cast<float>(0xC61C3F9Bu)} :
                                                            m==2 ? CVector{1.f,2.f,std::bit_cast<float>(0xC61C3F99u)} : m==3 ? CVector{1.f,2.f,qnan} : CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(-5.f,10.f)}; }
                            { const uint32 m=next()%5; At<uint32>(o,0x44)=m<2 ? 0xFFFFFFFFu : m==2 ? now-next()%5000 : m==3 ? now-5000u-next()%3u+1u : now-5001u-next()%20000; }
                            if(next()%16==0)At<uint32>(o,0x44)=now+next()%100;
                            static const int32 types[]{TASK_COMPLEX_SEQUENCE,TASK_COMPLEX_INVESTIGATE_DISTURBANCE,TASK_SIMPLE_PAUSE,TASK_COMPLEX_SEQUENCE,TASK_COMPLEX_INVESTIGATE_DISTURBANCE};
                            if(nm=="Destructor"){ At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else {
                                setSub(types,5,true);
                                if(nm=="ControlSubTask"&&next()%8==0){ At<CPed*>(o,0x10)=nullptr; CleanRefs(); }
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x48,true); out["sub"]=Region(fakes[0].data(),0x30,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexLeaveAnyCar") {
                        // +0xC retardo, +0x10 salida sensata, +0x11 forzar salida
                        if(nm=="Constructor"){
                            const int32 delay=next()%4 ? int32(next()%3000) : -int32(next()%100); const uint32 a=next()%2, b=next()%2;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,uint32,uint32)>(target)(o,delay,a|(next()&0xFF00),b|(next()&0xFF00)));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x14,true); out["type"]=typeOf(o);
                        } else {
                            setTask(VT_LAC,0x14);
                            if(nm=="Destructor"){ At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else {
                                static const int32 types[]{TASK_COMPLEX_LEAVE_CAR,TASK_NONE};
                                setSub(types,2,false);
                                inVeh(p0,next()%4!=0); At<CVehicle*>(p0,P_VEHICLE)=next()%4 ? Veh(int(next()%NVEH)) : nullptr;
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x14,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexPartnerGreet") {
                        // base CTaskComplexPartner 0x70 bytes (+0x38 companero, +0x58 habla primero, +0x5E bloque, +0x6E referenciado);
                        // +0x70 sin escribir, +0x74 tipo de saludo
                        auto setName=[&]{ static const char* names[]{"gangs","ped",""}; const char* s=names[next()%3]; std::memset(o+0x5E,0,16); std::memcpy(o+0x5E,s,std::strlen(s)); };
                        if(nm=="Constructor"){
                            static const char cmd[]="Greet";
                            CPed* part=static_cast<CPed*>(next()%4 ? pickEntity(false) : nullptr); const uint32 lead=next()%2; const float dist=next()%3 ? uni(0.f,3.f) : 1.f;
                            const int32 hs=int32(next()%8)-1; const CVector pt{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,const char*,CPed*,uint32,float,int32,CVector)>(target)(o,cmd,part,lead,dist,hs,pt));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x78,true); out["type"]=typeOf(o);
                        } else {
                            setTask(VT_PG,0x78); setName();
                            CPed* part=static_cast<CPed*>(pickEntity(false)); At<CPed*>(o,0x38)=part; Reg(part,o+0x38);
                            o[0x58]=uint8(next()%2); o[0x6E]=uint8(next()%2); At<int32>(o,0x74)=next()%8==0 ? int32(next()%20)-8 : int32(next()%6);
                            if(nm=="Destructor"){ At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="GetPartnerSequence"){ auto* r=callClone(); collectFresh(r); ret=Name(r); out["rng"]=rngCalls; }
                            else if(nm=="StreamRequiredAnims"){
                                callVoid(); out["block"]=blockIdx; out["loaded"]=At<uint8>(reinterpret_cast<const void*>(0xB5D4B0+32*blockIdx),0);
                            } else {
                                static const int32 types[]{TASK_SIMPLE_PAUSE,TASK_COMPLEX_SEQUENCE};
                                setSub(types,2,false); auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x78,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexSeekEntityAiming") {
                        // +0xC entidad (referencia), +0x10 radio de busqueda, +0x14 radio de apuntado
                        if(nm=="Constructor"){
                            CEntity* e=pickEntity(true); const float sr=next()%3 ? uni(0.f,20.f) : 5.f, ar=next()%3 ? uni(0.f,20.f) : 10.f;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CEntity*,float,float)>(target)(o,e,sr,ar));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x18,true); out["type"]=typeOf(o);
                        } else {
                            setTask(VT_SEA,0x18);
                            CEntity* e=pickEntity(true); At<CEntity*>(o,0xC)=e; Reg(e,o+0xC);
                            At<float>(o,0x10)=uni(0.f,30.f); At<float>(o,0x14)=uni(0.f,30.f);
                            if(nm=="Destructor"){ At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else {
                                static const int32 types[]{TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_SEEK_ENTITY,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_SEEK_ENTITY,TASK_SIMPLE_PAUSE};
                                setSub(types,5,false);
                                if(nm=="ControlSubTask"){
                                    if(!e){ e=Ped(1); At<CEntity*>(o,0xC)=e; Reg(e,o+0xC); }
                                    // distancia exacta al radio ocasional: (radio, 0) en 2D
                                    const CVector ep=entPos(e); const uint32 m=next()%6;
                                    if(m==0){ const float r=At<float>(o,0x10); At<CVector>(pedMats[0].data(),0x30)=CVector{ep.x+r,ep.y,ep.z+1.f}; }
                                    else if(m==1){ const float r=At<float>(o,0x14); At<CVector>(pedMats[0].data(),0x30)=CVector{ep.x,ep.y+r,ep.z}; }
                                    else if(m==2){ At<float>(o,0x14)=qnan; }
                                    // tarea secundaria de ataque: ninguna, UseGun falsa u otra tarea falsa
                                    const uint32 g=next()%3; At<CTask*>(intels[0].data(),I_ATTACK)=g==0 ? nullptr : Fake(1);
                                    At<int32>(fakes[1].data(),FK_TYPE)=g==1 ? TASK_SIMPLE_USE_GUN : TASK_SIMPLE_THROW_PROJECTILE;
                                }
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                if(nm=="ControlSubTask"){
                                    auto* slot=At<CTask*>(intels[0].data(),I_ATTACK);
                                    out["attack_slot"]=slot&&freshIds.contains(uintptr_t(slot)) ? json(SerTask(slot,0,0x40)) : Name(slot);
                                    if(slot&&freshIds.contains(uintptr_t(slot))){ roots.push_back(slot); At<CTask*>(intels[0].data(),I_ATTACK)=nullptr; }
                                }
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x18,true); out["sub"]=Region(fakes[0].data(),0x30,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexSmartFleeEntity") {
                        // +0xC entidad (referencia), +0x10 posicion, +0x1C tiempo, +0x20 distancia segura, +0x24 gritar, +0x28 periodo,
                        // +0x2C tolerancia, +0x30 estado de movimiento, temporizador +0x34 inicio, +0x38 intervalo, +0x3C iniciado, +0x3D parado
                        static const int32 moves[]{7,7,6,5,4,8,0,3,0x7FFFFFFF,-1};
                        if(nm=="Constructor"){
                            CEntity* e=pickEntity(true); const uint32 scream=next()%2; const float safe=uni(0.f,100.f); const int32 time=next()%3 ? 1000000 : int32(next()%20000);
                            const int32 period=next()%4 ? 1000 : int32(next()%3000)-500; const float tol=next()%3 ? 1.f : uni(0.f,5.f);
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CEntity*,uint32,float,int32,int32,float)>(target)(o,e,scream|(next()&0xFF00),safe,time,period,tol));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x40,true); out["type"]=typeOf(o);
                        } else {
                            setTask(VT_SFE,0x40);
                            CEntity* e=pickEntity(true); At<CEntity*>(o,0xC)=e; Reg(e,o+0xC);
                            At<CVector>(o,0x10)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                            At<int32>(o,0x1C)=int32(next()%100000); At<float>(o,0x20)=uni(0.f,100.f); o[0x24]=uint8(next()%2);
                            At<int32>(o,0x28)=next()%4 ? 1000 : int32(next()%3000)-500; At<float>(o,0x2C)=next()%3 ? 1.f : uni(0.f,5.f);
                            At<int32>(o,0x30)=moves[next()%10];
                            At<uint32>(o,0x34)=now-next()%3000; At<int32>(o,0x38)=next()%8==0 ? -int32(next()%100) : int32(next()%2000);
                            o[0x3C]=uint8(next()%4!=0); o[0x3D]=uint8(next()%4==0);
                            if(nm=="Destructor"){ At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else {
                                static const int32 types[]{TASK_COMPLEX_SMART_FLEE_POINT,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_SMART_FLEE_POINT,TASK_SIMPLE_PAUSE};
                                setSub(types,4,false);
                                if(nm=="CreateFirstSubTask") inVeh(p0,next()%4==0);
                                if(nm=="ControlSubTask"&&e){
                                    // cambio de posicion exactamente igual a la tolerancia ocasional
                                    const CVector ep=entPos(e); const uint32 m=next()%5;
                                    if(m==0){ const float t=At<float>(o,0x2C); At<CVector>(o,0x10)=CVector{ep.x+t,ep.y,ep.z}; }
                                    else if(m==1){ At<CVector>(o,0x10)=ep; }
                                    else if(m==2){ At<uint32>(o,0x34)=now-uint32(At<int32>(o,0x38)); }
                                }
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x40,true); out["sub"]=Region(fakes[0].data(),0x30,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    for(auto* h:hooks)h->State(false);
                    // limpieza: tareas nuevas alcanzables (raices) y referencias; despues el pool debe quedar como antes
                    const size_t traceBefore=trace.size();
                    for(auto* t:roots)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    CleanRefs();
                    out["ret"]=ret; out["trace"]=trace; out["delete_trace"]=delTrace; out["rng_calls"]=rngCalls;
                    out["clean"]=Occupancy()==occ;
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
    CleanRefs(); restoreGlobals(); PoolLast()=lf0;
    for(int32 k=0;k<PoolCap();++k)if(PoolStates()[k]&0x80)std::memset(PoolStorage()+k*TASK_SLOT,0xCD,TASK_SLOT);
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&globalsIntact();
    if(restored)std::memcpy(PoolStates(),statesStart.data(),statesStart.size());
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
