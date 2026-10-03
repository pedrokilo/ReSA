#pragma once
// Banco dentro del motor: tareas complejas A (lista de 8, 48 funciones).
// CTaskComplexEnterBoatAsDriver, CTaskComplexFleeAnyMeans, CTaskComplexFleeEntity y CTaskComplexFollowLeaderAnyMeans
// (constructor, destructor, Clone, GetTaskType, ControlSubTask, CreateFirstSubTask, CreateNextSubTask, CreateSubTask),
// CTaskComplexExtinguishFires (constructor, destructor, Clone, GetTaskType, ControlSubTask, CreateFirstSubTask,
// CreateNextSubTask, GetNearestFire) y CTaskComplexFleeShooting (constructor, destructor, Clone, GetTaskType,
// MakeAbortable, ControlSubTask, CreateFirstSubTask, CreateNextSubTask).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 4 peatones (con inteligencia propia), 2 vehiculos (vtable falsa con trampas y CanPedStepOutCar trazado), 2 fuegos,
// grupo, info de arma, model info y modelo de colision falsos y tareas falsas (vtable trazada).
// Se simulan y trazan: CFireManager::FindNearestFire, CFire::Extinguish, CCarEnterExit::IsVehicleStealable,
// CPed::GetWeaponSkill, CWeaponInfo::GetWeaponInfo, CTaskSimpleUseGun::ControlGun, CPed::Say, CInterestingEvents::Add,
// IKChainManager_c::IsLooking/LookAt, CTaskComplexFleePoint::SetFleePosition, CTaskComplexSmartFleePoint::SetFleePosition,
// CEventGroup::Add y el RNG.
// Pool de tareas: m_LastFreeSlot y estados se restauran y los slots libres se rellenan con 0xCD en cada ejecucion.
// CTimer::m_snTimeInMilliseconds y un hueco libre de CModelInfo::ms_modelInfoPtrs se guardan y se restauran.
// Se compara la memoria de los fixtures (punteros traducidos a nombres), las tareas nuevas del pool, las referencias,
// el retorno y la traza.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/Task.h"
#include "TaskComplexEnterBoatAsDriver.h"
#include "TaskComplexExtinguishFires.h"
#include "TaskComplexFleeAnyMeans.h"
#include "TaskComplexFleeEntity.h"
#include "TaskComplexFleeShooting.h"
#include "TaskComplexFollowLeaderAnyMeans.h"
#include "TaskComplexFleePoint.h"
#include "TaskComplexSmartFleePoint.h"
#include "TaskSimpleUseGun.h"
#include "SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h"
#include "FireManager.h"
#include "Fire.h"
#include "CarEnterExit.h"
#include "WeaponInfo.h"
#include "PedIntelligence.h"
#include "InterestingEvents.h"
#include "Ragdoll/IKChainManager.h"
#include "Events/EventGroup.h"
#include "EventSeenPanickedPed.h"
#include "Events/EventVehicleDamageWeapon.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks8aLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }

constexpr int NPEDS=4, NVEH=2, FAKES=4, NFIRES=2;
// tareas falsas: los campos de control quedan por encima de 0x50 (GoToPoint escribe hasta 0x28, la rueda del barco lee 0x1C)
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_SUB=0x64, FK_TYPE=0x68, FK_CLONE=0x6C;
// desplazamientos crudos del EXE
constexpr size_t E_POS=4, E_MATRIX=0x14, E_MODEL=0x22, E_TYPE=0x36;                                 // CEntity
constexpr size_t P_FLAGS=0x46C, P_INTEL=0x47C, P_STANDING=0x568, P_CONTACT=0x584, P_VEHICLE=0x58C,
                 P_PEDTYPE=0x598, P_WEAPONS=0x5A0, P_SLOT=0x718;                                     // CPed
constexpr size_t V_DRIVER=0x460, V_PASS=0x464, V_MAXPASS=0x488;                                     // CVehicle
constexpr size_t I_ATTACK=0x18, I_GROUP=0x68, I_CLOSEST_VEH=0xD4+0x4C;                             // CPedIntelligence
constexpr uintptr_t TIMER=0xB7CB84, FIRE_MANAGER=0xB71F80, INTERESTING=0xC0B058, IKCHAIN=0xC15448, XYCALC_VT=0x86F8F8;
constexpr uintptr_t VT_EBD=0x86E7D0, VT_EXT=0x870DD0, VT_FAM=0x86F7F0, VT_FE=0x86F708, VT_FS=0x86F7C4, VT_FLAM=0x870C68;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<uint8,0x100> object{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,0x28>,NFIRES> fires{};
alignas(16) inline std::array<uint8,0x40> group{};
alignas(16) inline std::array<uint8,0x80> weaponInfo{};
alignas(16) inline std::array<uint8,0x100> modelInfo{};
alignas(16) inline std::array<uint8,0x40> colModel{};
alignas(16) inline std::array<uint8,0x10> eventArg{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline CFire* FireAt(int n) { return reinterpret_cast<CFire*>(fires[n].data()); }
inline int VehIndex(const void* p) { for(int n=0;n<NVEH;++n)if(p==vehs[n].data())return n; return -1; }

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
inline uintptr_t dllCalcVt{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(vehVtbl.data()))return "veh_vt";
    // vtable de CEntitySeekPosCalculatorXYOffset: la del EXE (0x86F8F8) o la del DLL
    if(v==XYCALC_VT||(dllCalcVt&&v==dllCalcVt))return "xyoffset_calc_vt";
    if(v==INTERESTING)return "interesting_events"; if(v==IKCHAIN)return "ik_chain_manager"; if(v==FIRE_MANAGER)return "fire_manager";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n);
                              if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NFIRES;++n)if(auto s=in(fires[n].data(),fires[n].size(),"fire"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(group.data(),group.size(),"group"))return *s;
    if(auto s=in(weaponInfo.data(),weaponInfo.size(),"winfo"))return *s; if(auto s=in(modelInfo.data(),modelInfo.size(),"mi"))return *s;
    if(auto s=in(colModel.data(),colModel.size(),"col"))return *s; if(auto s=in(eventArg.data(),eventArg.size(),"event_arg"))return *s;
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); for(auto& v:vehs)r.push_back(RefList(v.data())); return r; }
// tarea serializada sin vptr (se compara el tipo); se incluye todo el slot de 0x80 (FollowLeaderAnyMeans escribe en +0x50)
inline json SerTask(const void* p,size_t bytes=TASK_SLOT) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType(); const auto* w=static_cast<const uint32*>(p);
    json words=json::array(); for(size_t k=1;k<bytes/4;++k)words.push_back(Name(w[k]));
    return {{"type",type},{"words",words}};
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{};
inline uint8 stealableRet{}, stepOutRet{}, isLookingRet{}, skillRet{};
inline std::array<uintptr_t,3> fireSeq{}; inline uint32 fireCalls{};

// tareas falsas
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(t)}); return At<CTask*>(t,FK_CLONE); }
inline CTask* __fastcall FSub(CTask* t,void*) { return At<CTask*>(t,FK_SUB); }
inline bool __fastcall FSimple(CTask*,void*) { return true; }
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) { trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e)}); return At<uint8>(t,FK_ABORT)!=0; }
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<size_t S,int... N> void FillTraps(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// vtable de vehiculo: slot 57 (0xE4) = CanPedStepOutCar
inline bool __fastcall FakeStepOut(const CVehicle* v,void*,uint32 ignore) { trace.push_back({"can_ped_step_out_car",Name(v),ignore&0xFF,stepOutRet}); return stepOutRet!=0; }

// dependencias simuladas
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline int16 __fastcall FakeSay(CPed* p,void*,int ctx,uint32 delay,float prob,uint32 overrideSilence,uint32 forceAudible,uint32 frontEnd) {
    trace.push_back({"say",Name(p),int16(ctx),delay,F(prob),overrideSilence&0xFF,forceAudible&0xFF,frontEnd&0xFF}); return 0;
}
inline CFire* __fastcall FakeNearestFire(void* mgr,void*,const CVector* p,uint32 a,uint32 b) {
    const uintptr_t r=fireCalls<fireSeq.size() ? fireSeq[fireCalls] : 0; ++fireCalls;
    trace.push_back({"nearest_fire",Name(mgr),F(p->x),F(p->y),F(p->z),a&0xFF,b&0xFF,Name(r)}); return reinterpret_cast<CFire*>(r);
}
inline void __fastcall FakeExtinguish(CFire* f,void*) { trace.push_back({"extinguish",Name(f)}); }
inline bool __cdecl FakeStealable(const CVehicle* v,const CPed* p) { trace.push_back({"is_vehicle_stealable",Name(v),Name(p),stealableRet}); return stealableRet!=0; }
inline uint32 __fastcall FakeSkill(CPed* p,void*) { trace.push_back({"weapon_skill",Name(p),skillRet}); return skillRet; }
inline CWeaponInfo* __cdecl FakeWeaponInfo(int32 type,uint32 skill) { trace.push_back({"weapon_info",type,skill&0xFF}); return reinterpret_cast<CWeaponInfo*>(weaponInfo.data()); }
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
    json fields=nullptr;
    if(type==EVENT_SEEN_PANICKED_PED)fields=Name(static_cast<CEventSeenPanickedPed*>(e)->m_ped);
    // CEventVehicleDamageWeapon: campos escritos (el relleno de la pila no se compara)
    else if(type==EVENT_VEHICLE_DAMAGE_WEAPON)fields={At<uint32>(e,4),At<uint8>(e,8),At<uint8>(e,0xC),At<int16>(e,0xE),At<int16>(e,0x10),Name(At<uint32>(e,0x14)),Name(At<uint32>(e,0x18)),At<int32>(e,0x1C)};
    trace.push_back({"event_group_add",uintptr_t(grp)==uintptr_t(GetEventGlobalGroup()) ? json("global") : Name(grp),type,valid&0xFF,fields});
    return nullptr;
}

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=48;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexEnterBoatAsDriver","Clone",0x63D920,0x86E7D0,1,128},
    {"CTaskComplexEnterBoatAsDriver","Constructor",0x63B5E0,0,-1,128},
    {"CTaskComplexEnterBoatAsDriver","ControlSubTask",0x63B6B0,0x86E7D0,10,64},
    {"CTaskComplexEnterBoatAsDriver","CreateFirstSubTask",0x640ED0,0x86E7D0,9,256},
    {"CTaskComplexEnterBoatAsDriver","CreateNextSubTask",0x640E60,0x86E7D0,8,256},
    {"CTaskComplexEnterBoatAsDriver","CreateSubTask",0x63B6C0,0,-1,256},
    {"CTaskComplexEnterBoatAsDriver","Destructor",0x63B650,0,-1,128},
    {"CTaskComplexEnterBoatAsDriver","GetTaskType",0x63B640,0x86E7D0,4,64},
    {"CTaskComplexExtinguishFires","Clone",0x6977B0,0x870DD0,1,128},
    {"CTaskComplexExtinguishFires","Constructor",0x6975C0,0,-1,64},
    {"CTaskComplexExtinguishFires","ControlSubTask",0x698330,0x870DD0,10,1024},
    {"CTaskComplexExtinguishFires","CreateFirstSubTask",0x698290,0x870DD0,9,512},
    {"CTaskComplexExtinguishFires","CreateNextSubTask",0x698110,0x870DD0,8,1024},
    {"CTaskComplexExtinguishFires","Destructor",0x6975F0,0,-1,64},
    {"CTaskComplexExtinguishFires","GetNearestFire",0x697600,0,-1,1024},
    {"CTaskComplexExtinguishFires","GetTaskType",0x6975E0,0x870DD0,4,64},
    {"CTaskComplexFleeAnyMeans","Clone",0x65D030,0x86F7F0,1,256},
    {"CTaskComplexFleeAnyMeans","Constructor",0x65CC60,0,-1,256},
    {"CTaskComplexFleeAnyMeans","ControlSubTask",0x65D780,0x86F7F0,10,1024},
    {"CTaskComplexFleeAnyMeans","CreateFirstSubTask",0x65D730,0x86F7F0,9,256},
    {"CTaskComplexFleeAnyMeans","CreateNextSubTask",0x65D540,0x86F7F0,8,512},
    {"CTaskComplexFleeAnyMeans","CreateSubTask",0x65D2E0,0,-1,512},
    {"CTaskComplexFleeAnyMeans","Destructor",0x65CD10,0,-1,128},
    {"CTaskComplexFleeAnyMeans","GetTaskType",0x65CD00,0x86F7F0,4,64},
    {"CTaskComplexFleeEntity","Clone",0x65CE50,0x86F708,1,256},
    {"CTaskComplexFleeEntity","Constructor",0x65B930,0,-1,256},
    {"CTaskComplexFleeEntity","ControlSubTask",0x65BBB0,0x86F708,10,1024},
    {"CTaskComplexFleeEntity","CreateFirstSubTask",0x65BB00,0x86F708,9,256},
    {"CTaskComplexFleeEntity","CreateNextSubTask",0x65BAC0,0x86F708,8,128},
    {"CTaskComplexFleeEntity","CreateSubTask",0x65BA30,0,-1,128},
    {"CTaskComplexFleeEntity","Destructor",0x65B9D0,0,-1,128},
    {"CTaskComplexFleeEntity","GetTaskType",0x65B9C0,0x86F708,4,64},
    {"CTaskComplexFleeShooting","Clone",0x65CFB0,0x86F7C4,1,256},
    {"CTaskComplexFleeShooting","Constructor",0x65C900,0,-1,256},
    {"CTaskComplexFleeShooting","ControlSubTask",0x65CA90,0x86F7C4,10,1024},
    {"CTaskComplexFleeShooting","CreateFirstSubTask",0x65CA70,0x86F7C4,9,256},
    {"CTaskComplexFleeShooting","CreateNextSubTask",0x65CA60,0x86F7C4,8,128},
    {"CTaskComplexFleeShooting","Destructor",0x65C9A0,0,-1,128},
    {"CTaskComplexFleeShooting","GetTaskType",0x65C990,0x86F7C4,4,64},
    {"CTaskComplexFleeShooting","MakeAbortable",0x65CA00,0x86F7C4,6,256},
    {"CTaskComplexFollowLeaderAnyMeans","Clone",0x6957B0,0x870C68,1,128},
    {"CTaskComplexFollowLeaderAnyMeans","Constructor",0x694AA0,0,-1,128},
    {"CTaskComplexFollowLeaderAnyMeans","ControlSubTask",0x696DA0,0x870C68,10,256},
    {"CTaskComplexFollowLeaderAnyMeans","CreateFirstSubTask",0x696D80,0x870C68,9,128},
    {"CTaskComplexFollowLeaderAnyMeans","CreateNextSubTask",0x696CC0,0x870C68,8,256},
    {"CTaskComplexFollowLeaderAnyMeans","CreateSubTask",0x696680,0,-1,256},
    {"CTaskComplexFollowLeaderAnyMeans","Destructor",0x694B30,0,-1,128},
    {"CTaskComplexFollowLeaderAnyMeans","GetTaskType",0x694B20,0x870C68,4,64},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks8a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks8a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks8a");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks8a-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    { CEntitySeekPosCalculatorXYOffset calc{}; dllCalcVt=*reinterpret_cast<uintptr_t*>(&calc); }
    // hueco libre de CModelInfo::ms_modelInfoPtrs para el model info falso de los vehiculos
    int32 miSlot=-1; for(int32 k=19999;k>=15000;--k)if(!CModelInfo::ms_modelInfoPtrs[k]){miSlot=k;break;}
    if(miSlot<0){Write({{"event","error"},{"reason","no free model info slot"}});return;}
    FillTraps(fakeVtbl,std::make_integer_sequence<int,16>{}); FillTraps(vehVtbl,std::make_integer_sequence<int,96>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    vehVtbl[57]=reinterpret_cast<void*>(&FakeStepOut);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{uintptr_t(&CModelInfo::ms_modelInfoPtrs[miSlot]),4}};
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
        EntryPatch f1(bit_cast<uintptr_t>(&CFireManager::FindNearestFire),FakeNearestFire), f1e(0x538F40,FakeNearestFire);
        EntryPatch f2(bit_cast<uintptr_t>(&CFire::Extinguish),FakeExtinguish), f2e(0x5393F0,FakeExtinguish);
        EntryPatch c1(bit_cast<uintptr_t>(&CCarEnterExit::IsVehicleStealable),FakeStealable), c1e(0x6510D0,FakeStealable);
        EntryPatch w1(bit_cast<uintptr_t>(static_cast<eWeaponSkill(CPed::*)()>(&CPed::GetWeaponSkill)),FakeSkill), w1e(0x5E6580,FakeSkill);
        EntryPatch w2(bit_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)),FakeWeaponInfo), w2e(0x743C60,FakeWeaponInfo);
        EntryPatch u1(bit_cast<uintptr_t>(&CTaskSimpleUseGun::ControlGun),FakeControlGun), u1e(0x61E040,FakeControlGun);
        EntryPatch i1(bit_cast<uintptr_t>(&CInterestingEvents::Add),FakeInteresting), i1e(0x602590,FakeInteresting);
        EntryPatch i2(bit_cast<uintptr_t>(&IKChainManager_c::IsLooking),FakeIsLooking), i2e(0x6181A0,FakeIsLooking);
        EntryPatch i3(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),FakeLookAt), i3e(0x618970,FakeLookAt);
        EntryPatch p1(bit_cast<uintptr_t>(&CTaskComplexFleePoint::SetFleePosition),FakeSetFlee), p1e(0x65B7B0,FakeSetFlee);
        EntryPatch p2(bit_cast<uintptr_t>(&CTaskComplexSmartFleePoint::SetFleePosition),FakeSetFlee), p2e(0x65C3C0,FakeSetFlee);
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
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x8A8Au; if(!st)st=1; trace=json::array(); seed=uint32(i);
                    rngCalls=0; fireCalls=0; freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    const uint32 now=At<uint32>(reinterpret_cast<void*>(TIMER),0);
                    CModelInfo::ms_modelInfoPtrs[miSlot]=reinterpret_cast<CBaseModelInfo*>(modelInfo.data());
                    std::memset(modelInfo.data(),0,modelInfo.size()); std::memset(colModel.data(),0,colModel.size());
                    At<void*>(modelInfo.data(),0x14)=colModel.data(); At<float>(colModel.data(),0x24)=next()%4 ? uni(0.5f,6.f) : 2.f;
                    // peatones: matriz, inteligencia propia, tipo, armas
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                        p[E_TYPE]=uint8(3|(next()%8)<<3); At<void*>(p,P_INTEL)=intels[n].data(); At<void*>(p,E_MATRIX)=pedMats[n].data();
                        for(int r=0;r<3;++r)At<CVector>(pedMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(pedMats[n].data(),0x30)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                        { static const int32 types[]{4,4,5,7,0,1}; At<int32>(p,P_PEDTYPE)=types[next()%6]; }
                        for(int s=0;s<13;++s){ static const int32 w[]{0,22,42,42,24,31}; At<int32>(p,P_WEAPONS+0x1C*s)=w[next()%6]; }
                        p[P_SLOT]=uint8(next()%13);
                    }
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%8)<<3); At<void*>(v,E_MATRIX)=vehMats[n].data(); At<int16>(v,E_MODEL)=int16(miSlot);
                        for(int r=0;r<3;++r)At<CVector>(vehMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                    }
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,FAKE_SIZE); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_PAUSE+n; fakes[n][FK_ABORT]=uint8(next()%2); }
                    for(int n=0;n<NFIRES;++n){ for(auto& b:fires[n])b=uint8(next()); At<CVector>(fires[n].data(),4)=CVector{uni(-40.f,40.f),uni(-40.f,40.f),uni(-5.f,15.f)}; }
                    std::memset(group.data(),0,group.size()); std::memset(eventArg.data(),0,eventArg.size());
                    std::memset(weaponInfo.data(),0,weaponInfo.size()); At<uint32>(weaponInfo.data(),0x18)=next();
                    std::memset(object.data(),0xCD,object.size());
                    stealableRet=uint8(next()%3!=0); stepOutRet=uint8(next()%3!=0); isLookingRet=uint8(next()%3==0); skillRet=uint8(next()%4);
                    for(auto& r:fireSeq){ const uint32 m=next()%4; r=m==0 ? 0 : uintptr_t(FireAt(int(m%2))); }

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
                        json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t)); out["tasks"]=tasks;
                        if(r&&freshIds.contains(uintptr_t(r)))roots.push_back(r);
                    };
                    auto setTask=[&](uintptr_t vtbl,size_t size){ for(size_t b=0;b<size;++b)o[b]=uint8(next()); At<uintptr_t>(o,0)=vtbl; At<uint32>(o,4)=0; At<uint32>(o,8)=0; };
                    auto setSub=[&](const int32* types,size_t n){ At<CTask*>(o,8)=Fake(0); At<int32>(fakes[0].data(),FK_TYPE)=types[next()%n]; };
                    auto typeOf=[&](void* p){ return reinterpret_cast<CTask*>(p)->GetTaskType(); };
                    auto callTask=[&](CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    auto callClone=[&]{ return reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); };
                    auto callVoid=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); };
                    auto callSub1=[&](int32 type){ return reinterpret_cast<CTask*(__thiscall*)(void*,int32)>(target)(o,type); };
                    auto callSub2=[&](int32 type,CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,type,ped); };
                    auto inVeh=[&](CPed* p,bool on){ auto& fl=At<uint32>(p,P_FLAGS); fl=on ? (fl|0x100u) : (fl&~0x100u); };
                    auto pickEntity=[&](bool allowVeh)->CEntity*{ const uint32 m=next()%6; if(m==0)return nullptr; if(allowVeh&&m==1)return Veh(int(next()%NVEH)); return Ped(1+int(next()%(NPEDS-1))); };
                    auto entPos=[&](CEntity* e){ return e ? e->GetPosition() : CVector{}; };
                    auto ctorDone=[&](size_t size){ for(auto* h:hooks)h->State(false); out["object"]=Region(o,size,true); out["type"]=typeOf(o); };
                    auto dtorSub=[&]{ At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; };
                    // tarea secundaria de ataque nueva: se serializa, se borra despues y se quita del slot
                    auto attackOut=[&]{
                        auto* slot=At<CTask*>(intels[0].data(),I_ATTACK);
                        out["attack_slot"]=slot&&freshIds.contains(uintptr_t(slot)) ? json(SerTask(slot,0x40)) : Name(slot);
                        if(slot&&freshIds.contains(uintptr_t(slot))){ roots.push_back(slot); At<CTask*>(intels[0].data(),I_ATTACK)=nullptr; }
                    };
                    // base CTaskComplexSmartFleeEntity: +0xC entidad, +0x10 posicion, +0x1C tiempo, +0x20 distancia segura, +0x24 gritar,
                    // +0x28 periodo, +0x2C tolerancia, +0x30 estado de movimiento, temporizador +0x34
                    auto setSmartFlee=[&](uintptr_t vtbl,size_t size,CEntity* e){
                        setTask(vtbl,size); At<CEntity*>(o,0xC)=e; Reg(e,o+0xC);
                        At<CVector>(o,0x10)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                        At<int32>(o,0x1C)=int32(next()%100000); At<float>(o,0x20)=uni(0.f,100.f); o[0x24]=uint8(next()%2);
                        At<int32>(o,0x28)=next()%4 ? 1000 : int32(next()%3000)-500; At<float>(o,0x2C)=next()%3 ? 1.f : uni(0.f,5.f);
                        { static const int32 moves[]{7,7,6,4}; At<int32>(o,0x30)=moves[next()%4]; }
                        At<uint32>(o,0x34)=now-next()%3000; At<int32>(o,0x38)=int32(next()%2000); o[0x3C]=uint8(next()%4!=0); o[0x3D]=uint8(next()%4==0);
                    };

                    if(nm=="GetTaskType") {
                        setTask(fn.vt,0x40);
                        ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        out["object"]=Region(o,0x40,true);
                    }
                    else if(cls=="CTaskComplexEnterBoatAsDriver") {
                        // +0xC barco (referencia)
                        if(nm=="Constructor"){
                            CVehicle* v=next()%4 ? Veh(int(next()%NVEH)) : nullptr;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*)>(target)(o,v));
                            ctorDone(0x10);
                        } else {
                            setTask(VT_EBD,0x10); CVehicle* v=next()%6 ? Veh(0) : nullptr; At<CVehicle*>(o,0xC)=v; Reg(v,o+0xC);
                            if(nm=="Destructor"){ dtorSub(); callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_COMPLEX_GET_ON_BOAT_SEAT,TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER,TASK_COMPLEX_GO_TO_BOAT_STEERING_WHEEL,TASK_FINISHED,TASK_SIMPLE_PAUSE,TASK_NONE};
                                auto* r=callSub1(t[next()%std::size(t)]); collectFresh(r); ret=Name(r);
                            } else {
                                static const int32 t[]{TASK_COMPLEX_GO_TO_BOAT_STEERING_WHEEL,TASK_COMPLEX_GO_TO_BOAT_STEERING_WHEEL,TASK_COMPLEX_GET_ON_BOAT_SEAT,TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER,TASK_SIMPLE_PAUSE};
                                setSub(t,std::size(t)); fakes[0][0x1C]=uint8(next()%2 ? 0 : 1+next()%255);
                                // entidad de contacto / de apoyo: el barco, otro vehiculo, un peaton o nada
                                auto pick=[&]()->void*{ const uint32 m=next()%4; return m==0 ? (void*)v : m==1 ? (void*)Veh(1) : m==2 ? (void*)Ped(1) : nullptr; };
                                At<void*>(p0,P_CONTACT)=pick(); At<void*>(p0,P_STANDING)=pick();
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x30,true);
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x10,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexExtinguishFires") {
                        // +0xC fuego a apagar; posicion de los fuegos en +4
                        auto fireNear=[&](int n,uint32 m){ // distancia exacta de 20, NaN o aleatoria
                            const CVector pp=p0->GetPosition();
                            if(m==0)At<CVector>(fires[n].data(),4)=CVector{pp.x+20.f,pp.y,pp.z};
                            else if(m==1)At<CVector>(fires[n].data(),4)=CVector{pp.x,pp.y,qnan};
                            else if(m==2)At<CVector>(fires[n].data(),4)=CVector{pp.x+uni(-12.f,12.f),pp.y+uni(-12.f,12.f),pp.z+uni(-2.f,2.f)};
                        };
                        fireNear(0,next()%8); fireNear(1,next()%8);
                        if(nm=="Constructor"){
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o));
                            ctorDone(0x10);
                        } else if(nm=="GetNearestFire"){
                            setTask(VT_EXT,0x10); At<CFire*>(o,0xC)=FireAt(0);
                            ret=Name(reinterpret_cast<CFire*(__thiscall*)(void*,CPed*)>(target)(o,p0));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x10,true);
                        } else {
                            setTask(VT_EXT,0x10);
                            { const uint32 m=next()%3; At<CFire*>(o,0xC)=m==0 ? nullptr : FireAt(int(m%2)); }
                            if(nm=="Destructor"){ dtorSub(); callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else {
                                static const int32 t[]{TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_GUN_CTRL,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_PAUSE};
                                setSub(t,std::size(t));
                                // fuera de los fixtures: fuego cercano y m_fireToExt nulos a la vez con STAND_STILL/GUN_CTRL
                                // (el EXE y el DLL llaman a Extinguish con un puntero nulo)
                                if(nm=="CreateNextSubTask"&&!At<CFire*>(o,0xC))At<CFire*>(o,0xC)=FireAt(int(next()%2));
                                if(nm=="ControlSubTask"){ auto* s=fakes[0].data(); At<CVector>(s,0x10)=CVector{uni(-40.f,40.f),uni(-40.f,40.f),uni(0.f,10.f)};
                                    At<float>(s,0x1C)=next()%2 ? 1.5f : uni(0.f,3.f); At<float>(s,0x20)=next()%2 ? 2.f : uni(0.f,4.f); s[0x24]=uint8(next()); At<int32>(s,0xC)=int32(next()%8); }
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x30,true);
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x10,true);
                        }
                        out["refs"]=AllRefs(); out["fire_calls"]=fireCalls;
                    }
                    else if(cls=="CTaskComplexFleeAnyMeans") {
                        // base SmartFleeEntity (0x40) + 0x40 disparar, +0x44 tiempo de disparo, +0x48 recuperacion, +0x4C distancia para robar,
                        // +0x50 vehiculo a robar (referencia)
                        auto setFam=[&]{
                            setSmartFlee(VT_FAM,0x54,pickEntity(true)); o[0x40]=uint8(next()%2); At<int32>(o,0x44)=int32(next()%8000); At<int32>(o,0x48)=int32(next()%8000);
                            At<float>(o,0x4C)=next()%3 ? 20.f : uni(0.f,40.f); At<CVehicle*>(o,0x50)=nullptr;
                        };
                        if(nm=="Constructor"){
                            CEntity* e=pickEntity(true); const uint32 shoot=next()%2; const float safe=uni(0.f,100.f); const int32 time=next()%3 ? 1000000 : int32(next()%20000);
                            const int32 st1=next()%2 ? 4000 : int32(next()%9000), st2=next()%2 ? 4000 : int32(next()%9000); const float steal=next()%2 ? 20.f : uni(0.f,40.f);
                            const int32 period=next()%4 ? 1000 : int32(next()%3000)-500; const float tol=next()%3 ? 1.f : uni(0.f,5.f);
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CEntity*,uint32,float,int32,int32,int32,float,int32,float)>(target)(o,e,shoot|(next()&0xFF00),safe,time,st1,st2,steal,period,tol));
                            ctorDone(0x54);
                        } else {
                            setFam();
                            if(nm=="Destructor"){ dtorSub(); callVoid(); } // m_stealVeh nulo: FIX_BUGS limpia su referencia en el destructor
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else {
                                inVeh(p0,next()%3==0); At<CVehicle*>(p0,P_VEHICLE)=next()%4 ? Veh(0) : nullptr;
                                if(nm=="CreateSubTask"){
                                    static const int32 t[]{TASK_COMPLEX_FLEE_SHOOTING,TASK_COMPLEX_WANDER,TASK_COMPLEX_STEAL_CAR,TASK_COMPLEX_CAR_DRIVE_MISSION,TASK_COMPLEX_GO_TO_POINT_SHOOTING,TASK_FINISHED,TASK_SIMPLE_PAUSE};
                                    const int32 type=t[next()%std::size(t)];
                                    if(type==TASK_COMPLEX_GO_TO_POINT_SHOOTING||next()%2){ At<CVehicle*>(o,0x50)=Veh(1); Reg(Veh(1),o+0x50); }
                                    auto* r=callSub2(type,p0); collectFresh(r); ret=Name(r);
                                } else if(nm=="ControlSubTask"){
                                    static const int32 t[]{TASK_COMPLEX_FLEE_SHOOTING,TASK_COMPLEX_WANDER,TASK_COMPLEX_FLEE_SHOOTING,TASK_COMPLEX_WANDER,TASK_SIMPLE_PAUSE};
                                    setSub(t,std::size(t));
                                    if(next()%4==0){ At<CVehicle*>(o,0x50)=Veh(1); Reg(Veh(1),o+0x50); }
                                    if(next()%8==0){ CleanRefs(); At<CEntity*>(o,0xC)=nullptr; if(At<CVehicle*>(o,0x50))Reg(Veh(1),o+0x50); }
                                    if(!At<CEntity*>(o,0xC)&&next()%2){ At<CEntity*>(o,0xC)=Ped(1); Reg(Ped(1),o+0xC); }
                                    CVehicle* closest=next()%5 ? Veh(0) : nullptr; At<CVehicle*>(intels[0].data(),I_CLOSEST_VEH)=closest;
                                    const CVector pp=p0->GetPosition();
                                    if(auto* e=At<CEntity*>(o,0xC)){ // distancia exacta, NaN o lejos
                                        const uint32 m=next()%6; const float d=At<float>(o,0x4C);
                                        auto& pos=e->GetPosition();
                                        if(m==0)pos=CVector{pp.x+d,pp.y,pp.z}; else if(m==1)pos=CVector{pp.x,qnan,pp.z}; else if(m<4)pos=CVector{pp.x+uni(-80.f,80.f),pp.y+uni(-80.f,80.f),pp.z};
                                    }
                                    if(closest){ // vehiculo delante, detras, en la misma posicion o en NaN
                                        const uint32 m=next()%8; const CVector fw=At<CVector>(pedMats[0].data(),0x10); const float s=uni(1.f,10.f);
                                        At<CVector>(vehMats[0].data(),0x30)=m<3 ? CVector{pp.x+fw.x*s,pp.y+fw.y*s,pp.z+fw.z*s} : m==3 ? pp : m==4 ? CVector{qnan,pp.y,pp.z} :
                                                                             CVector{pp.x+uni(-20.f,20.f),pp.y+uni(-20.f,20.f),pp.z+uni(-2.f,2.f)};
                                        // ocupantes: conductor y pasajeros (peatones con o sin datos de jugador por tipo)
                                        auto* v=vehs[0].data(); At<CPed*>(v,V_DRIVER)=next()%4 ? Ped(1+int(next()%3)) : nullptr;
                                        for(int s2=0;s2<8;++s2)At<CPed*>(v,V_PASS+4*s2)=next()%3==0 ? Ped(1+int(next()%3)) : nullptr;
                                        v[V_MAXPASS]=uint8(next()%9);
                                    }
                                    auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                } else {
                                    static const int32 t[]{TASK_COMPLEX_STEAL_CAR,TASK_COMPLEX_CAR_DRIVE_MISSION,TASK_COMPLEX_GO_TO_POINT_SHOOTING,TASK_COMPLEX_WANDER,TASK_COMPLEX_FLEE_SHOOTING,TASK_SIMPLE_PAUSE};
                                    setSub(t,std::size(t));
                                    if(next()%2){ At<CVehicle*>(o,0x50)=Veh(1); Reg(Veh(1),o+0x50); }
                                    auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                }
                                out["sub"]=Region(fakes[0].data(),0x30,true); out["rng"]=rngCalls;
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x54,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexFleeEntity") {
                        // +0xC entidad (referencia), +0x10 posicion, +0x1C tiempo, temporizador +0x20 inicio, +0x24 intervalo, +0x28 iniciado,
                        // +0x29 parado, +0x2C distancia segura, +0x30 gritar, +0x34 periodo, +0x38 tolerancia
                        if(nm=="Constructor"){
                            CEntity* e=pickEntity(true); const uint32 scream=next()%2; const float safe=next()%2 ? 60.f : uni(0.f,100.f); const int32 time=next()%3 ? 1000000 : int32(next()%20000);
                            const int32 period=next()%4 ? 1000 : int32(next()%3000)-500; const float tol=next()%3 ? 1.f : uni(0.f,5.f);
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CEntity*,uint32,float,int32,int32,float)>(target)(o,e,scream|(next()&0xFF00),safe,time,period,tol));
                            ctorDone(0x3C);
                        } else {
                            setTask(VT_FE,0x3C); CEntity* e=pickEntity(true);
                            // fuera de los fixtures: CreateFirstSubTask sin entidad (el EXE la desreferencia; el DLL devuelve nullptr)
                            if(nm=="CreateFirstSubTask"&&!e)e=Ped(1);
                            At<CEntity*>(o,0xC)=e; Reg(e,o+0xC);
                            At<CVector>(o,0x10)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)}; At<int32>(o,0x1C)=int32(next()%100000);
                            At<uint32>(o,0x20)=now-next()%3000; At<int32>(o,0x24)=next()%8==0 ? -int32(next()%100) : int32(next()%2000); o[0x28]=uint8(next()%4!=0); o[0x29]=uint8(next()%4==0);
                            At<float>(o,0x2C)=uni(0.f,100.f); o[0x30]=uint8(next()%2); At<int32>(o,0x34)=next()%4 ? 1000 : int32(next()%3000)-500;
                            At<float>(o,0x38)=next()%3 ? 1.f : next()%6 ? uni(0.f,5.f) : qnan;
                            if(nm=="Destructor"){ dtorSub(); callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_COMPLEX_FLEE_POINT,TASK_COMPLEX_FLEE_POINT,TASK_FINISHED,TASK_SIMPLE_PAUSE};
                                auto* r=callSub1(t[next()%std::size(t)]); collectFresh(r); ret=Name(r);
                            } else {
                                static const int32 t[]{TASK_COMPLEX_FLEE_POINT,TASK_COMPLEX_FLEE_POINT,TASK_SIMPLE_PAUSE};
                                setSub(t,std::size(t));
                                if(nm=="ControlSubTask"&&e){ // cambio de posicion igual a la tolerancia, sin cambio o temporizador justo cumplido
                                    const CVector ep=entPos(e); const uint32 m=next()%5;
                                    if(m==0){ const float t2=At<float>(o,0x38); At<CVector>(o,0x10)=CVector{ep.x+t2,ep.y,ep.z}; }
                                    else if(m==1){ At<CVector>(o,0x10)=ep; }
                                    else if(m==2){ At<uint32>(o,0x20)=now-uint32(At<int32>(o,0x24)); }
                                }
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x30,true);
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x3C,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexFleeShooting") {
                        // base SmartFleeEntity (0x40) + 0x40 tiempo de disparo, +0x44 recuperacion, temporizador de arma +0x48 inicio, +0x4C intervalo,
                        // +0x50 iniciado, +0x51 parado
                        if(nm=="Constructor"){
                            CEntity* e=pickEntity(true); const uint32 scream=next()%2; const float safe=uni(0.f,100.f); const int32 time=next()%3 ? 1000000 : int32(next()%20000);
                            const int32 st1=int32(next()%9000), st2=int32(next()%9000);
                            const int32 period=next()%4 ? 1000 : int32(next()%3000)-500; const float tol=next()%3 ? 1.f : uni(0.f,5.f);
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CEntity*,uint32,float,int32,int32,int32,int32,float)>(target)(o,e,scream|(next()&0xFF00),safe,time,st1,st2,period,tol));
                            ctorDone(0x54);
                        } else {
                            setSmartFlee(VT_FS,0x54,pickEntity(true)); At<int32>(o,0x40)=next()%8==0 ? -int32(next()%100) : int32(next()%8000); At<int32>(o,0x44)=next()%8==0 ? -int32(next()%100) : int32(next()%8000);
                            At<uint32>(o,0x48)=now-next()%6000; At<int32>(o,0x4C)=next()%6==0 ? -1 : int32(next()%4000); o[0x50]=uint8(next()%4!=0); o[0x51]=uint8(next()%4==0);
                            if(nm=="Destructor"){ dtorSub(); callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else {
                                static const int32 t[]{TASK_COMPLEX_SMART_FLEE_POINT,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_SMART_FLEE_POINT,TASK_SIMPLE_PAUSE};
                                setSub(t,std::size(t));
                                // tarea secundaria de ataque: ninguna, UseGun falsa (ultimo comando en +0x10) u otra tarea falsa
                                const uint32 g=next()%3; At<CTask*>(intels[0].data(),I_ATTACK)=g==0 ? nullptr : Fake(1);
                                At<int32>(fakes[1].data(),FK_TYPE)=g==1 ? TASK_SIMPLE_USE_GUN : TASK_SIMPLE_THROW_PROJECTILE; fakes[1][0x10]=uint8(next()%2 ? 3 : next()%8);
                                if(nm=="MakeAbortable"){
                                    static const int32 prios[]{0,1,2};
                                    ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,p0,prios[next()%3],next()%2 ? reinterpret_cast<CEvent*>(eventArg.data()) : nullptr);
                                    for(auto* h:hooks)h->State(false);
                                } else {
                                    if(nm=="CreateFirstSubTask")inVeh(p0,next()%4==0);
                                    if(nm=="ControlSubTask"){ if(auto* e=At<CEntity*>(o,0xC)){
                                        const CVector ep=entPos(e); const uint32 m=next()%5;
                                        if(m==0){ const float t2=At<float>(o,0x2C); At<CVector>(o,0x10)=CVector{ep.x+t2,ep.y,ep.z}; }
                                        else if(m==1){ At<CVector>(o,0x10)=ep; }
                                        else if(m==2){ At<uint32>(o,0x48)=now-uint32(At<int32>(o,0x4C)); } } }
                                    auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                    if(nm=="ControlSubTask")attackOut();
                                }
                                out["sub"]=Region(fakes[0].data(),0x30,true);
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x54,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexFollowLeaderAnyMeans") {
                        // +0xC grupo, +0x10 lider (referencia), +0x14 posicion
                        if(nm=="Constructor"){
                            CPedGroup* g=next()%4 ? reinterpret_cast<CPedGroup*>(group.data()) : nullptr; CPed* l=next()%4 ? Ped(1+int(next()%3)) : nullptr;
                            const CVector pos{uni(-5.f,5.f),uni(-5.f,5.f),uni(-1.f,1.f)};
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CPedGroup*,CPed*,const CVector*)>(target)(o,g,l,&pos));
                            ctorDone(0x20);
                        } else {
                            setTask(VT_FLAM,0x20); At<void*>(o,0xC)=next()%4 ? (void*)group.data() : nullptr;
                            CPed* l=next()%6 ? Ped(1) : nullptr;
                            // fuera de los fixtures: CreateNextSubTask sin lider (el EXE lee los flags del lider nulo)
                            if(nm=="CreateNextSubTask"&&!l)l=Ped(1);
                            At<CPed*>(o,0x10)=l; Reg(l,o+0x10); At<CVector>(o,0x14)=CVector{uni(-5.f,5.f),uni(-5.f,5.f),uni(-1.f,1.f)};
                            inVeh(p0,next()%2==0); inVeh(Ped(1),next()%2==0); At<CVehicle*>(p0,P_VEHICLE)=next()%4 ? Veh(0) : nullptr;
                            if(nm=="Destructor"){ dtorSub(); callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_COMPLEX_FOLLOW_LEADER_IN_FORMATION,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_CAR_DRIVE_TIMED,TASK_COMPLEX_SEEK_ENTITY_ANY_MEANS,TASK_FINISHED,TASK_SIMPLE_PAUSE};
                                auto* r=callSub2(t[next()%std::size(t)],p0); collectFresh(r); ret=Name(r);
                            } else {
                                static const int32 t[]{TASK_COMPLEX_FOLLOW_LEADER_IN_FORMATION,TASK_COMPLEX_SEEK_ENTITY_ANY_MEANS,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_CAR_DRIVE_TIMED,TASK_SIMPLE_PAUSE};
                                setSub(t,std::size(t));
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x30,true);
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x20,true);
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
