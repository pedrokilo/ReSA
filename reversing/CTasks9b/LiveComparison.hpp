#pragma once
// Banco dentro del motor: tareas complejas B (lista de 9, 45 funciones).
// CTaskComplexFallAndGetUp (constructores, CreateSubTask, IsFalling, SetDownTime, MakeAbortable, ControlSubTask,
// CreateFirstSubTask, CreateNextSubTask), CTaskComplexInvestigateDeadPed (constructor, destructor, Clone, GetTaskType,
// ComputeTargetPos, CreateSubTask, ControlSubTask, CreateFirstSubTask, CreateNextSubTask), CTaskComplexKillPedGroupOnFoot
// (constructor, destructor, Clone, GetTaskType, ComputeNearestLivingGroupPed, CreateSubTask, ControlSubTask,
// CreateFirstSubTask, CreateNextSubTask), CTaskComplexMedicTreatInjuredPed (constructor, Clone, CreateSubTask,
// CreateDealWithNextAccidentTask, FindNearestAccident, CalcTargetPosWithOffset, ControlSubTask, CreateFirstSubTask,
// CreateNextSubTask) y CTaskComplexOnFire (constructor, destructor, Clone, GetTaskType, ComputeFireDamage, CreateSubTask,
// ControlSubTask, CreateFirstSubTask, CreateNextSubTask).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 8 peatones (con inteligencia propia), 2 vehiculos (vtable falsa con trampas), 3 accidentes, fuego, datos de jugador,
// nivel de busqueda, grupo falso (GetPedsGroup) y tareas falsas (vtable trazada).
// Se simulan y trazan: CPed::Say, IKChainManager_c::LookAt, CInterestingEvents::Add, CPedGroups::GetPedsGroup,
// CPedGroups::AreInSameGroup, CAccidentManager::GetNearestFreeAccident(ExceptThisOne), CPed::GetBonePosition,
// CPedGeometryAnalyser::IsEntityBlockingTarget, CPedDamageResponseCalculator::ComputeDamageResponse,
// CLocalisation::PedsOnFire, CFire::Extinguish, CEventHandler::RegisterKill, FindPlayerPed y el RNG.
// Pool de tareas: m_LastFreeSlot y estados se restauran y los slots libres se rellenan con 0xCD en cada ejecucion.
// CTimer::m_snTimeInMilliseconds, CTimer::ms_fTimeStep y CPedGroups::ms_groups (los miembros del grupo a matar se
// escriben en el grupo real) se guardan y se restauran.
// Se compara la memoria de los fixtures (punteros traducidos a nombres), las tareas nuevas del pool, las referencias,
// el retorno y la traza.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/Task.h"
#include "TaskComplexFallAndGetUp.h"
#include "TaskComplexInvestigateDeadPed.h"
#include "TaskComplexKillPedGroupOnFoot.h"
#include "TaskComplexMedicTreatInjuredPed.h"
#include "TaskComplexOnFire.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "AccidentManager.h"
#include "PedGroups.h"
#include "Localisation.h"
#include "Fire.h"
#include "PedDamageResponseCalculator.h"
#include "PedGeometryAnalyser.h"
#include "Events/EventHandler.h"
#include "InterestingEvents.h"
#include "Ragdoll/IKChainManager.h"
#include "PedIntelligence.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks9bLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }

constexpr int NPEDS=8, NVEH=2, FAKES=4, NACC=3;
// tareas falsas: los campos de control quedan por encima de 0x50 (GoToPoint escribe hasta 0x28)
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_SUB=0x64, FK_TYPE=0x68, FK_CLONE=0x6C;
// desplazamientos crudos del EXE
constexpr size_t E_MATRIX=0x14, E_TYPE=0x36, PH_FLAGS=0x40;                                                  // CEntity/CPhysical
constexpr size_t P_FLAGS=0x46C, P_INTEL=0x47C, P_PDATA=0x480, P_CREATED=0x484, P_ANIMGRP=0x4D4, P_STATE=0x530,
                 P_HEALTH=0x540, P_PEDTYPE=0x598, P_TARGET=0x71C, P_FIRE=0x730, P_FIREMULT=0x734;             // CPed
constexpr size_t V_PASS=0x464, V_MAXPASS=0x488;                                                               // CVehicle
constexpr size_t I_PRIMARY=0x4, I_VEHSCAN=0xE0;                                                               // CPedIntelligence
constexpr uintptr_t TIMER=0xB7CB84, TIMESTEP=0xB7CB5C, PEDGROUPS=0xC09920, INTERESTING=0xC0B058, IKCHAIN=0xC15448;
constexpr size_t PEDGROUP_SIZE=0x2D4;
constexpr uintptr_t VT_FAG=0x870454, VT_IDP=0x870D3C, VT_KPG=0x86DA48, VT_MTI=0x86F518, VT_OF=0x86E308;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<uint8,0x100> object{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,8>,NACC> accidents{};
alignas(16) inline std::array<uint8,0x28> fire{};
alignas(16) inline std::array<uint8,0x10> playerData{};
alignas(16) inline std::array<uint8,0x40> wanted{};
alignas(16) inline std::array<uint8,0x40> group{};
alignas(16) inline std::array<uint8,0x10> vecBuf{};
alignas(16) inline std::array<uint8,0x10> respBuf{};
alignas(16) inline std::array<uint8,0x10> eventArg{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline CAccident* Acc(int n) { return reinterpret_cast<CAccident*>(accidents[n].data()); }
inline int PedIndex(const void* p) { for(int n=0;n<NPEDS;++n)if(p==peds[n].data())return n; return -1; }
inline int EntIndex(const void* p) { if(int n=PedIndex(p);n>=0)return n; for(int n=0;n<NVEH;++n)if(p==vehs[n].data())return NPEDS+n; return -1; }

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
    for(int n=0;n<NACC;++n)if(auto s=in(accidents[n].data(),accidents[n].size(),"acc"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(fire.data(),fire.size(),"fire"))return *s;
    if(auto s=in(playerData.data(),playerData.size(),"pdata"))return *s; if(auto s=in(wanted.data(),wanted.size(),"wanted"))return *s;
    if(auto s=in(group.data(),group.size(),"group"))return *s; if(auto s=in(vecBuf.data(),vecBuf.size(),"vec"))return *s;
    if(auto s=in(respBuf.data(),respBuf.size(),"resp"))return *s; if(auto s=in(eventArg.data(),eventArg.size(),"event_arg"))return *s;
    if(v>=PEDGROUPS&&v<PEDGROUPS+8*PEDGROUP_SIZE)return "pedgroups+"+std::to_string(v-PEDGROUPS);
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); for(auto& v:vehs)r.push_back(RefList(v.data())); return r; }
// tarea serializada sin vptr (se compara el tipo); se incluye todo el slot de 0x80
inline json SerTask(const void* p,size_t bytes=TASK_SLOT) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType(); const auto* w=static_cast<const uint32*>(p);
    json words=json::array(); for(size_t k=1;k<bytes/4;++k)words.push_back(Name(w[k]));
    return {{"type",type},{"words",words}};
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{};
inline uint8 pedsOnFireRet{}, healthZeroRet{};
inline uint64 sameGroupMask{}; inline uint32 blockingMask{}; inline uintptr_t pedsGroupRet{};
inline std::array<uintptr_t,4> accSeq{}; inline uint32 accCalls{};
inline std::array<std::array<CVector,2>,NPEDS> bonePos{};
inline float respHealthRet{};

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
    trace.push_back({"say",Name(p),int16(ctx),delay,F(prob),overrideSilence&0xFF,forceAudible&0xFF,frontEnd&0xFF}); return 0;
}
inline void __fastcall FakeLookAt(void* m,void*,const char* purpose,CPed* p,CEntity* e,int32 time,int32 bone,CVector* off,uint32 torso,float speed,int32 blend,int32 prio,uint32 force) {
    std::string s; for(int k=0;k<32&&purpose&&purpose[k];++k)s+=purpose[k];
    trace.push_back({"look_at",Name(m),s,Name(p),Name(e),time,bone,Name(off),torso&0xFF,F(speed),blend,prio,force&0xFF});
}
inline void __fastcall FakeInteresting(void* ie,void*,int32 type,CEntity* e) { trace.push_back({"interesting_event",Name(ie),type,Name(e)}); }
inline CPedGroup* __cdecl FakePedsGroup(const CPed* p) { trace.push_back({"get_peds_group",Name(p),Name(pedsGroupRet)}); return reinterpret_cast<CPedGroup*>(pedsGroupRet); }
inline bool __cdecl FakeSameGroup(const CPed* a,const CPed* b) {
    const int i=PedIndex(a), j=PedIndex(b); const bool r=i>=0&&j>=0&&((sameGroupMask>>(i*8+j))&1);
    trace.push_back({"are_in_same_group",Name(a),Name(b),r}); return r;
}
inline CAccident* NextAcc() { const uintptr_t r=accCalls<accSeq.size() ? accSeq[accCalls] : 0; ++accCalls; return reinterpret_cast<CAccident*>(r); }
inline CAccident* __fastcall FakeNearestFree(void*,void*,CVector* pos,uint32 ignoreHeadless) {
    auto* r=NextAcc(); trace.push_back({"nearest_free_accident",Name(pos),F(pos->x),F(pos->y),F(pos->z),ignoreHeadless&0xFF,Name(r)}); return r;
}
inline CAccident* __fastcall FakeNearestFreeExcept(void*,void*,CVector* pos,CAccident* except,uint32 ignoreHeadless) {
    auto* r=NextAcc(); trace.push_back({"nearest_free_accident_except",Name(pos),F(pos->x),F(pos->y),F(pos->z),Name(except),ignoreHeadless&0xFF,Name(r)}); return r;
}
// las dos sobrecargas del DLL y la del EXE (0x5E4280) tienen la misma forma: this, CVector* destino, hueso, actualizar
inline CVector* __fastcall FakeBonePos(CPed* p,void*,CVector* out,int32 bone,uint32 update) {
    const int n=PedIndex(p); *out=n>=0 ? bonePos[n][bone==BONE_L_THIGH ? 0 : 1] : CVector{};
    trace.push_back({"bone_position",Name(p),bone,update&0xFF}); return out;
}
inline bool __cdecl FakeBlocking(CEntity* e,const CVector* point,float dist) {
    const int n=EntIndex(e); const bool r=n>=0&&((blockingMask>>n)&1);
    trace.push_back({"is_entity_blocking_target",Name(e),Name(point),F(point->x),F(point->y),F(point->z),F(dist),r}); return r;
}
inline void __fastcall FakeComputeResp(void* calc,void*,CPed* p,CPedDamageResponse* resp,uint32 speak) {
    // la respuesta de ControlSubTask es local (direccion de pila distinta en EXE y DLL): solo se nombra el buffer propio
    const json where=uintptr_t(resp)>=uintptr_t(respBuf.data())&&uintptr_t(resp)<uintptr_t(respBuf.data())+respBuf.size() ? Name(resp) : json("local");
    trace.push_back({"compute_damage_response",Name(At(calc,0)),At(calc,4),At(calc,8),At(calc,0xC),At<uint8>(calc,0x10),Name(p),where,
                     F(resp->m_fDamageHealth),F(resp->m_fDamageArmor),resp->m_bHealthZero,resp->m_bForceDeath,resp->m_bDamageCalculated,resp->m_bCheckIfAffectsPed,speak&0xFF});
    resp->m_fDamageHealth=respHealthRet; resp->m_bHealthZero=healthZeroRet!=0; resp->m_bDamageCalculated=true;
}
inline bool __cdecl FakePedsOnFire() { trace.push_back({"peds_on_fire",pedsOnFireRet}); return pedsOnFireRet!=0; }
inline void __fastcall FakeExtinguish(CFire* f,void*) { trace.push_back({"extinguish",Name(f)}); }
inline void __cdecl FakeRegisterKill(const CPed* p,const CEntity* e,int32 weapon,uint32 headShot) { trace.push_back({"register_kill",Name(p),Name(e),weapon,headShot&0xFF}); }
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(Ped(3)); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=45;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexFallAndGetUp","Constructor",0x6786C0,0,-1,128},
    {"CTaskComplexFallAndGetUp","Constructor2",0x678700,0,-1,128},
    {"CTaskComplexFallAndGetUp","ControlSubTask",0x6788F0,0x870454,10,64},
    {"CTaskComplexFallAndGetUp","CreateFirstSubTask",0x678870,0x870454,9,128},
    {"CTaskComplexFallAndGetUp","CreateNextSubTask",0x67CB70,0x870454,8,128},
    {"CTaskComplexFallAndGetUp","CreateSubTask",0x678900,0,-1,128},
    {"CTaskComplexFallAndGetUp","IsFalling",0x6787D0,0,-1,128},
    {"CTaskComplexFallAndGetUp","MakeAbortable",0x6787F0,0x870454,6,512},
    {"CTaskComplexFallAndGetUp","SetDownTime",0x6787A0,0,-1,128},
    {"CTaskComplexInvestigateDeadPed","Clone",0x697680,0x870D3C,1,128},
    {"CTaskComplexInvestigateDeadPed","ComputeTargetPos",0x696EE0,0,-1,512},
    {"CTaskComplexInvestigateDeadPed","Constructor",0x696DF0,0,-1,128},
    {"CTaskComplexInvestigateDeadPed","ControlSubTask",0x697A00,0x870D3C,10,1024},
    {"CTaskComplexInvestigateDeadPed","CreateFirstSubTask",0x697990,0x870D3C,9,256},
    {"CTaskComplexInvestigateDeadPed","CreateNextSubTask",0x697840,0x870D3C,8,512},
    {"CTaskComplexInvestigateDeadPed","CreateSubTask",0x696F90,0,-1,512},
    {"CTaskComplexInvestigateDeadPed","Destructor",0x696E80,0,-1,128},
    {"CTaskComplexInvestigateDeadPed","GetTaskType",0x696E70,0x870D3C,4,64},
    {"CTaskComplexKillPedGroupOnFoot","Clone",0x6237C0,0x86DA48,1,128},
    {"CTaskComplexKillPedGroupOnFoot","ComputeNearestLivingGroupPed",0x622450,0,-1,1024},
    {"CTaskComplexKillPedGroupOnFoot","Constructor",0x622240,0,-1,128},
    {"CTaskComplexKillPedGroupOnFoot","ControlSubTask",0x622310,0x86DA48,10,64},
    {"CTaskComplexKillPedGroupOnFoot","CreateFirstSubTask",0x629630,0x86DA48,9,512},
    {"CTaskComplexKillPedGroupOnFoot","CreateNextSubTask",0x629570,0x86DA48,8,512},
    {"CTaskComplexKillPedGroupOnFoot","CreateSubTask",0x622320,0,-1,256},
    {"CTaskComplexKillPedGroupOnFoot","Destructor",0x6222B0,0,-1,128},
    {"CTaskComplexKillPedGroupOnFoot","GetTaskType",0x6222A0,0x86DA48,4,64},
    {"CTaskComplexMedicTreatInjuredPed","CalcTargetPosWithOffset",0x658D20,0,-1,256},
    {"CTaskComplexMedicTreatInjuredPed","Clone",0x659AF0,0x86F518,1,128},
    {"CTaskComplexMedicTreatInjuredPed","Constructor",0x658BA0,0,-1,128},
    {"CTaskComplexMedicTreatInjuredPed","ControlSubTask",0x65ABF0,0x86F518,10,1024},
    {"CTaskComplexMedicTreatInjuredPed","CreateDealWithNextAccidentTask",0x65A020,0,-1,512},
    {"CTaskComplexMedicTreatInjuredPed","CreateFirstSubTask",0x659FE0,0x86F518,9,128},
    {"CTaskComplexMedicTreatInjuredPed","CreateNextSubTask",0x65A990,0x86F518,8,1024},
    {"CTaskComplexMedicTreatInjuredPed","CreateSubTask",0x658DB0,0,-1,512},
    {"CTaskComplexMedicTreatInjuredPed","FindNearestAccident",0x658CC0,0,-1,128},
    {"CTaskComplexOnFire","Clone",0x636E30,0x86E308,1,64},
    {"CTaskComplexOnFire","ComputeFireDamage",0x6333D0,0,-1,512},
    {"CTaskComplexOnFire","Constructor",0x633390,0,-1,64},
    {"CTaskComplexOnFire","ControlSubTask",0x639200,0x86E308,10,1024},
    {"CTaskComplexOnFire","CreateFirstSubTask",0x6391F0,0x86E308,9,128},
    {"CTaskComplexOnFire","CreateNextSubTask",0x6391B0,0x86E308,8,128},
    {"CTaskComplexOnFire","CreateSubTask",0x633470,0,-1,256},
    {"CTaskComplexOnFire","Destructor",0x6333C0,0,-1,64},
    {"CTaskComplexOnFire","GetTaskType",0x6333B0,0x86E308,4,64},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks9b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks9b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks9b");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks9b-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(fakeVtbl,std::make_integer_sequence<int,16>{}); FillTraps(vehVtbl,std::make_integer_sequence<int,96>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{TIMESTEP,4},{PEDGROUPS,8*PEDGROUP_SIZE}};
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
        EntryPatch i1(bit_cast<uintptr_t>(&CInterestingEvents::Add),FakeInteresting), i1e(0x602590,FakeInteresting);
        EntryPatch i3(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),FakeLookAt), i3e(0x618970,FakeLookAt);
        EntryPatch g1(bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup),FakePedsGroup), g1e(0x5F7E80,FakePedsGroup);
        EntryPatch g2(bit_cast<uintptr_t>(&CPedGroups::AreInSameGroup),FakeSameGroup), g2e(0x5F7F40,FakeSameGroup);
        EntryPatch a1(bit_cast<uintptr_t>(&CAccidentManager::GetNearestFreeAccident),FakeNearestFree), a1e(0x56D050,FakeNearestFree);
        EntryPatch a2(bit_cast<uintptr_t>(&CAccidentManager::GetNearestFreeAccidentExceptThisOne),FakeNearestFreeExcept), a2e(0x56CF90,FakeNearestFreeExcept);
        EntryPatch b1(bit_cast<uintptr_t>(static_cast<void(CPed::*)(CVector*,eBoneTag,bool)>(&CPed::GetBonePosition)),FakeBonePos);
        EntryPatch b2(bit_cast<uintptr_t>(static_cast<CVector(CPed::*)(eBoneTag,bool)>(&CPed::GetBonePosition)),FakeBonePos), b1e(0x5E4280,FakeBonePos);
        EntryPatch k1(bit_cast<uintptr_t>(&CPedGeometryAnalyser::IsEntityBlockingTarget),FakeBlocking), k1e(0x5F3970,FakeBlocking);
        EntryPatch d1(bit_cast<uintptr_t>(&CPedDamageResponseCalculator::ComputeDamageResponse),FakeComputeResp), d1e(0x4B5AC0,FakeComputeResp);
        EntryPatch d2(bit_cast<uintptr_t>(&CLocalisation::PedsOnFire),FakePedsOnFire), d2e(0x56D2D0,FakePedsOnFire);
        EntryPatch d3(bit_cast<uintptr_t>(&CFire::Extinguish),FakeExtinguish), d3e(0x5393F0,FakeExtinguish);
        EntryPatch d4(bit_cast<uintptr_t>(&CEventHandler::RegisterKill),FakeRegisterKill), d4e(0x4B9340,FakeRegisterKill);
        EntryPatch p1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed), p1e(0x56E210,FakeFindPlayerPed);

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
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x9B9Bu; if(!st)st=1; trace=json::array(); seed=uint32(i);
                    rngCalls=0; accCalls=0; freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    const uint32 now=At<uint32>(reinterpret_cast<void*>(TIMER),0);
                    At<float>(reinterpret_cast<void*>(TIMESTEP),0)=next()%4 ? 1.f : uni(0.f,3.f);
                    // peatones: matriz, inteligencia propia, tipo, estado, vida, datos de jugador
                    std::memset(playerData.data(),0,playerData.size()); std::memset(wanted.data(),0,wanted.size());
                    At<void*>(playerData.data(),0)=wanted.data(); wanted[0x18]=uint8(next()%3 ? 0 : 1+next()%4);
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                        p[E_TYPE]=uint8(3|(next()%8)<<3); At<void*>(p,P_INTEL)=intels[n].data(); At<void*>(p,E_MATRIX)=pedMats[n].data();
                        for(int r=0;r<3;++r)At<CVector>(pedMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        // posiciones: aleatorias o copiadas de otro peaton (empates de distancia)
                        At<CVector>(pedMats[n].data(),0x30)=n>1&&next()%6==0 ? At<CVector>(pedMats[next()%n].data(),0x30) : CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                        { static const int32 types[]{4,4,5,7,6,0,1}; At<int32>(p,P_PEDTYPE)=types[next()%7]; }
                        { static const int32 states[]{PEDSTATE_IDLE,PEDSTATE_IDLE,PEDSTATE_IDLE,PEDSTATE_DIE,PEDSTATE_DEAD,PEDSTATE_ARRESTED}; At<int32>(p,P_STATE)=states[next()%6]; }
                        { const uint32 m=next()%8; At<float>(p,P_HEALTH)=m==0 ? 0.f : m==1 ? -5.f : m==2 ? qnan : uni(1.f,100.f); }
                        p[P_CREATED]=uint8(next()%2 ? 1 : 2); At<void*>(p,P_PDATA)=playerData.data();
                        At<int32>(p,P_ANIMGRP)=int32(next()%60);
                        bonePos[n][0]=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)}; bonePos[n][1]=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                    }
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%8)<<3); At<void*>(v,E_MATRIX)=vehMats[n].data();
                        for(int r=0;r<3;++r)At<CVector>(vehMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                    }
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,FAKE_SIZE); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_PAUSE+n; fakes[n][FK_ABORT]=uint8(next()%2); }
                    for(int n=0;n<NACC;++n){ auto* a=accidents[n].data(); std::memset(a,0,8); At<CPed*>(a,0)=next()%4 ? Ped(4+n) : nullptr; a[4]=uint8(next()%2); a[5]=uint8(next()%4==0); }
                    for(auto& r:accSeq){ const uint32 m=next()%5; r=m==0 ? 0 : uintptr_t(Acc(int(m%NACC))); }
                    std::memset(fire.data(),0,fire.size()); At<CEntity*>(fire.data(),0x14)=next()%4 ? (CEntity*)Ped(2) : nullptr;
                    std::memset(group.data(),0,group.size()); std::memset(vecBuf.data(),0,vecBuf.size()); std::memset(eventArg.data(),0,eventArg.size());
                    for(size_t b=0;b<respBuf.size();++b)respBuf[b]=uint8(next()%2 ? 0 : next());
                    std::memset(object.data(),0xCD,object.size());
                    pedsOnFireRet=uint8(next()%3!=0); healthZeroRet=uint8(next()%2); respHealthRet=uni(0.f,50.f);
                    sameGroupMask=(uint64(next())<<32|next())&(uint64(next())<<32|next()); blockingMask=next()%3 ? 0 : next();
                    pedsGroupRet=0;

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
                    auto pos=[&](void* e)->CVector&{ return At<CVector>(At<void*>(e,E_MATRIX),0x30); };
                    auto ctorDone=[&](size_t size){ for(auto* h:hooks)h->State(false); out["object"]=Region(o,size,true); out["type"]=typeOf(o); };
                    auto dtorSub=[&]{ At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; };
                    // subtarea falsa con forma de CTaskComplexGoToPointAndStandStill (+0xC estado, +0x10 destino, +0x1C/+0x20 radios, +0x24 flags)
                    auto goToSub=[&](int32 moveState){ auto* s=fakes[0].data(); At<int32>(s,0xC)=moveState; At<CVector>(s,0x10)=CVector{uni(-40.f,40.f),uni(-40.f,40.f),uni(0.f,10.f)};
                        At<float>(s,0x1C)=next()%2 ? 0.5f : uni(0.f,3.f); At<float>(s,0x20)=next()%2 ? 2.f : uni(0.f,4.f); s[0x24]=uint8(next()); };

                    if(nm=="GetTaskType") {
                        setTask(fn.vt,0x40);
                        ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        out["object"]=Region(o,0x40,true);
                    }
                    else if(cls=="CTaskComplexFallAndGetUp") {
                        // +0xC animacion, +0x10 grupo, +0x14 tiempo en el suelo
                        if(nm=="Constructor"){
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,int32,int32)>(target)(o,int32(next()%300),int32(next()%12),next()%4 ? int32(next()%5000) : -1));
                            ctorDone(0x18);
                        } else if(nm=="Constructor2"){
                            static const int32 dirs[]{0,1,2,3,0,1,2,3,-1,4,7};
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,int32)>(target)(o,dirs[next()%std::size(dirs)],next()%4 ? int32(next()%5000) : -1));
                            ctorDone(0x18);
                        } else {
                            setTask(VT_FAG,0x18); At<int32>(o,0xC)=int32(next()%300); At<int32>(o,0x10)=int32(next()%12); At<int32>(o,0x14)=int32(next()%5000);
                            static const int32 subs[]{TASK_SIMPLE_FALL,TASK_SIMPLE_FALL,TASK_SIMPLE_GET_UP,TASK_SIMPLE_PAUSE};
                            if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_SIMPLE_FALL,TASK_SIMPLE_GET_UP,TASK_FINISHED,TASK_SIMPLE_PAUSE,TASK_NONE};
                                auto* r=callSub1(t[next()%std::size(t)]); collectFresh(r); ret=Name(r);
                            } else if(nm=="IsFalling"){
                                setSub(subs,std::size(subs)); if(next()%4==0)At<CTask*>(o,8)=nullptr;
                                ret=reinterpret_cast<bool(__thiscall*)(void*)>(target)(o);
                            } else if(nm=="SetDownTime"){
                                setSub(subs,std::size(subs)); for(size_t b=0x10;b<0x30;++b)fakes[0][b]=uint8(next());
                                reinterpret_cast<void(__thiscall*)(void*,int32)>(target)(o,next()%4 ? int32(next()%5000) : -int32(next()%100));
                                out["sub"]=Region(fakes[0].data(),0x30,true);
                            } else if(nm=="MakeAbortable"){
                                setSub(subs,std::size(subs));
                                { static const int32 states[]{PEDSTATE_IDLE,PEDSTATE_IDLE,PEDSTATE_ARRESTED,PEDSTATE_DIE}; At<int32>(p0,P_STATE)=states[next()%4]; }
                                At<uint8>(p0,0x474)=uint8(next()%4==0 ? 0x40u|next() : next()&~0x40u);
                                const int32 prio=int32(next()%4);
                                ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,p0,prio,next()%2 ? reinterpret_cast<CEvent*>(eventArg.data()) : nullptr);
                            } else {
                                if(nm=="CreateNextSubTask"||nm=="ControlSubTask")setSub(subs,std::size(subs));
                                if(nm=="ControlSubTask"&&next()%4==0)At<CTask*>(o,8)=nullptr;
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x18,true);
                        }
                    }
                    else if(cls=="CTaskComplexInvestigateDeadPed") {
                        // +0xC peaton muerto (referencia), temporizador +0x10 inicio, +0x14 intervalo, +0x18 iniciado, +0x19 parado,
                        // +0x1C destino, +0x28 rumbo, +0x2C posicion inicial, +0x38 duracion de la mirada
                        if(nm=="Constructor"){
                            CPed* d=next()%4 ? Ped(1+int(next()%3)) : nullptr;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CPed*)>(target)(o,d));
                            ctorDone(0x3C);
                        } else {
                            setTask(VT_IDP,0x3C);
                            // muerto nulo solo donde el EXE no lo desreferencia
                            CPed* d=(nm=="Destructor"||nm=="Clone"||nm=="ControlSubTask")&&next()%6==0 ? nullptr : Ped(1);
                            At<CPed*>(o,0xC)=d; Reg(d,o+0xC);
                            At<uint32>(o,0x10)=now-next()%3000; At<int32>(o,0x14)=next()%8==0 ? -int32(next()%100) : int32(next()%2000);
                            o[0x18]=uint8(next()%3==0); o[0x19]=uint8(next()%3==0);
                            const CVector dp=pos(Ped(1));
                            At<CVector>(o,0x1C)=next()%2 ? CVector{dp.x+uni(-1.5f,1.5f),dp.y+uni(-1.5f,1.5f),dp.z+uni(-0.5f,0.5f)} : CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                            At<float>(o,0x28)=uni(-3.2f,3.2f); At<CVector>(o,0x2C)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)}; At<int32>(o,0x38)=int32(5000+next()%2000);
                            // posicion del peaton respecto al muerto: igual (vector nulo), NaN o cerca
                            { const uint32 m=next()%8; auto& pp=pos(p0);
                              if(m==0)pp=dp; else if(m==1)pp=CVector{dp.x,qnan,dp.z}; else if(m<5)pp=CVector{dp.x+uni(-4.f,4.f),dp.y+uni(-4.f,4.f),dp.z+uni(-1.f,1.f)}; }
                            if(nm=="Destructor"){ dtorSub(); callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="ComputeTargetPos"){
                                for(auto& b:vecBuf)b=uint8(next());
                                reinterpret_cast<void(__thiscall*)(void*,CPed*,CVector*)>(target)(o,p0,reinterpret_cast<CVector*>(vecBuf.data()));
                                out["vec"]=Region(vecBuf.data(),0xC);
                            } else if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_COMPLEX_SIGNAL_AT_PED,TASK_FINISHED,TASK_SIMPLE_ACHIEVE_HEADING,
                                                       TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_ANIM,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_PAUSE};
                                auto* r=callSub2(t[next()%std::size(t)],p0); collectFresh(r); ret=Name(r);
                            } else {
                                if(nm=="CreateNextSubTask"){
                                    static const int32 t[]{TASK_SIMPLE_ACHIEVE_HEADING,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_ANIM,TASK_SIMPLE_GO_TO_POINT,TASK_COMPLEX_SIGNAL_AT_PED,
                                                           TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_PAUSE};
                                    setSub(t,std::size(t));
                                } else if(nm=="ControlSubTask"){
                                    static const int32 t[]{TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_ANIM,TASK_SIMPLE_PAUSE};
                                    setSub(t,std::size(t)); goToSub(int32(next()%8));
                                }
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x30,true);
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x3C,true);
                        }
                        out["refs"]=AllRefs(); out["rng"]=rngCalls;
                    }
                    else if(cls=="CTaskComplexKillPedGroupOnFoot") {
                        // +0xC grupo a matar, +0x10 objetivo (referencia)
                        const int32 grp=int32(next()%8);
                        // miembros del grupo a matar (grupo real, restaurado despues): peatones 0..7 o huecos
                        auto* members=reinterpret_cast<CPed**>(PEDGROUPS+grp*PEDGROUP_SIZE+0xC);
                        for(int k=0;k<8;++k)members[k]=next()%4 ? Ped(int(next()%NPEDS)) : nullptr;
                        // grupo del peaton (GetPedsGroup): lider (indice 7) y su objetivo
                        if(next()%4){ pedsGroupRet=uintptr_t(group.data());
                            const uint32 m=next()%6; CPed* leader=m==0 ? nullptr : m==1 ? p0 : Ped(1+int(next()%(NPEDS-1)));
                            At<CPed*>(group.data(),0xC+7*4)=leader;
                            if(leader&&leader!=p0){ const uint32 t=next()%4; At<CEntity*>(leader,P_TARGET)=t==0 ? nullptr : t==1 ? (CEntity*)Veh(0) : (CEntity*)Ped(int(next()%NPEDS)); } }
                        if(nm=="Constructor"){
                            CPed* t=next()%4 ? Ped(1+int(next()%3)) : nullptr;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,CPed*)>(target)(o,grp,t));
                            ctorDone(0x14);
                        } else {
                            setTask(VT_KPG,0x14); At<int32>(o,0xC)=grp;
                            CPed* t=next()%3 ? Ped(1+int(next()%(NPEDS-1))) : nullptr; At<CPed*>(o,0x10)=t; Reg(t,o+0x10);
                            if(nm=="Destructor"){ dtorSub(); callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="ComputeNearestLivingGroupPed"){
                                const uint32 flag=next()%4==0;
                                ret=Name(reinterpret_cast<CPed*(__thiscall*)(void*,CPed*,uint32)>(target)(o,p0,flag|(next()&0xFF00)));
                            } else if(nm=="CreateSubTask"){
                                static const int32 ty[]{TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_NONE,TASK_SIMPLE_LOOK_ABOUT,TASK_FINISHED,TASK_SIMPLE_PAUSE};
                                auto* r=callSub2(ty[next()%std::size(ty)],p0); collectFresh(r); ret=Name(r);
                            } else {
                                static const int32 ty[]{TASK_NONE,TASK_SIMPLE_LOOK_ABOUT,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_SIMPLE_PAUSE};
                                setSub(ty,std::size(ty));
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x14,true);
                        }
                        out["refs"]=AllRefs(); out["rng"]=rngCalls;
                    }
                    else if(cls=="CTaskComplexMedicTreatInjuredPed") {
                        // +0xC vehiculo (referencia), +0x10 companero (referencia), +0x14 conductor, +0x18 accidente, +0x1C posicion, +0x28 ya salio
                        if(nm=="Constructor"){
                            CVehicle* v=next()%4 ? Veh(int(next()%NVEH)) : nullptr; CPed* pt=next()%4 ? Ped(1) : nullptr;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,CPed*,uint32)>(target)(o,v,pt,uint32(next()%2)|(next()&0xFF00)));
                            ctorDone(0x2C);
                        } else {
                            setTask(VT_MTI,0x2C);
                            CVehicle* v=(nm=="CreateFirstSubTask"||nm=="Clone")&&next()%3==0 ? nullptr : Veh(0);
                            CPed* pt=next()%5 ? Ped(1) : nullptr;
                            At<CVehicle*>(o,0xC)=v; Reg(v,o+0xC); At<CPed*>(o,0x10)=pt; Reg(pt,o+0x10);
                            o[0x14]=uint8(next()%2); { const uint32 m=next()%4; At<CAccident*>(o,0x18)=m==0 ? nullptr : Acc(int(m-1)); }
                            At<CVector>(o,0x1C)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)}; o[0x28]=uint8(next()%3==0);
                            inVeh(p0,next()%4!=0);
                            if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="CalcTargetPosWithOffset"){
                                reinterpret_cast<void(__thiscall*)(void*,CPed*,CPed*)>(target)(o,p0,Ped(4+int(next()%3)));
                            } else if(nm=="FindNearestAccident"){
                                At<CVector>(vecBuf.data(),0)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                                reinterpret_cast<void(__thiscall*)(void*,CVector*)>(target)(o,reinterpret_cast<CVector*>(vecBuf.data()));
                            } else if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_COMPLEX_TREAT_ACCIDENT,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_ENTER_CAR_AS_PASSENGER,TASK_COMPLEX_ENTER_CAR_AS_DRIVER,
                                                       TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_CAR_DRIVE,TASK_COMPLEX_CAR_DRIVE_TO_POINT,TASK_COMPLEX_CAR_DRIVE_WANDER,
                                                       TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_COMPLEX_WANDER,TASK_FINISHED,TASK_SIMPLE_PAUSE};
                                auto* r=callSub1(t[next()%std::size(t)]); collectFresh(r); ret=Name(r);
                            } else if(nm=="CreateDealWithNextAccidentTask"){
                                static const int32 t[]{TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_PAUSE};
                                setSub(t,std::size(t)); goToSub(int32(next()%8)); if(next()%4==0)At<CTask*>(o,8)=nullptr;
                                // fuera de los fixtures: accidente con peaton y subtarea que no es GO_TO_POINT_AND_STAND_STILL
                                // (el EXE devuelve el peaton como tarea y pierde la creada; FIX_BUGS devuelve la tarea)
                                if(accSeq[0]&&reinterpret_cast<CAccident*>(accSeq[0])->m_pPed){ At<CTask*>(o,8)=Fake(0); At<int32>(fakes[0].data(),FK_TYPE)=TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL; }
                                CAccident* except=next()%3 ? Acc(int(next()%NACC)) : nullptr;
                                auto* r=reinterpret_cast<CTask*(__thiscall*)(void*,CPed*,CAccident*)>(target)(o,p0,except); collectFresh(r); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x30,true);
                            } else if(nm=="CreateFirstSubTask"){
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                            } else if(nm=="CreateNextSubTask"){
                                static const int32 t[]{TASK_COMPLEX_TREAT_ACCIDENT,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_ENTER_CAR_AS_DRIVER,TASK_COMPLEX_ENTER_CAR_AS_PASSENGER,
                                                       TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_PAUSE};
                                setSub(t,std::size(t)); const int32 sub=At<int32>(fakes[0].data(),FK_TYPE);
                                // fuera de los fixtures: conductor sin accidente al salir del coche (el EXE desreferencia m_pAccident nulo)
                                if(sub==TASK_COMPLEX_LEAVE_CAR&&o[0x14]&&!At<CAccident*>(o,0x18))At<CAccident*>(o,0x18)=Acc(int(next()%NACC));
                                if(sub==TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL){ goToSub(int32(next()%8));
                                    // distancia 2D exacta de 1, |z| exacto de 2, NaN o aleatoria
                                    const CVector pp=pos(p0); const uint32 m=next()%6;
                                    At<CVector>(o,0x1C)=m==0 ? CVector{pp.x-1.f,pp.y,pp.z} : m==1 ? CVector{pp.x,pp.y,pp.z+2.f} : m==2 ? CVector{pp.x,qnan,pp.z} :
                                                        CVector{pp.x+uni(-1.5f,1.5f),pp.y+uni(-1.5f,1.5f),pp.z+uni(-3.f,3.f)};
                                    // CreateDealWithNextAccidentTask llega con la subtarea GO_TO_POINT: sin el camino del fallo del EXE
                                }
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x30,true);
                            } else {
                                static const int32 t[]{TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_CAR_DRIVE,TASK_COMPLEX_CAR_DRIVE_TO_POINT,TASK_COMPLEX_CAR_DRIVE_WANDER,
                                                       TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_PAUSE};
                                setSub(t,std::size(t)); const int32 sub=At<int32>(fakes[0].data(),FK_TYPE);
                                // pasajeros del vehiculo (el companero o no)
                                auto* vb=vehs[0].data(); for(int s=0;s<8;++s)At<CPed*>(vb,V_PASS+4*s)=next()%4==0 ? Ped(next()%2 ? 1 : 2+int(next()%3)) : nullptr; vb[V_MAXPASS]=uint8(next()%9);
                                // tarea activa del companero: falsa con forma de tarea de medico (+0x18 accidente, +0x1C posicion) en un slot primario
                                if(pt&&next()%4){ auto* f1=fakes[1].data(); At<int32>(f1,FK_TYPE)=next()%4 ? TASK_COMPLEX_MEDIC_TREAT_INJURED_PED : TASK_SIMPLE_PAUSE;
                                    { const uint32 m=next()%4; At<CAccident*>(f1,0x18)=m==0 ? nullptr : Acc(int(m-1)); }
                                    const CVector vp=pos(Veh(0)); const uint32 m=next()%4;
                                    At<CVector>(f1,0x1C)=m==0 ? CVector{vp.x+10.f,vp.y,vp.z} : m==1 ? CVector{vp.x,vp.y,qnan} : CVector{vp.x+uni(-14.f,14.f),vp.y+uni(-14.f,14.f),vp.z+uni(-2.f,2.f)};
                                    At<CTask*>(intels[1].data(),I_PRIMARY+4*(next()%5))=Fake(1); }
                                if(sub==TASK_COMPLEX_CAR_DRIVE_TO_POINT){ const CVector vp=pos(Veh(0)); const uint32 m=next()%4;
                                    At<CVector>(o,0x1C)=m==0 ? CVector{vp.x,vp.y+10.f,vp.z} : m==1 ? CVector{qnan,vp.y,vp.z} : CVector{vp.x+uni(-14.f,14.f),vp.y+uni(-14.f,14.f),vp.z+uni(-2.f,2.f)}; }
                                if(sub==TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL){
                                    static const int32 moves[]{PEDMOVE_WALK,PEDMOVE_RUN,PEDMOVE_RUN,PEDMOVE_SPRINT}; goToSub(moves[next()%4]);
                                    const CVector pp=pos(p0); const uint32 m=next()%4;
                                    At<CVector>(o,0x1C)=m==0 ? CVector{pp.x+5.f,pp.y,pp.z} : m==1 ? CVector{pp.x,pp.y,qnan} : CVector{pp.x+uni(-7.f,7.f),pp.y+uni(-7.f,7.f),pp.z+uni(-1.f,1.f)};
                                    // entidades del escaner de vehiculos (con huecos)
                                    for(int s=0;s<16;++s){ const uint32 e=next()%6; At<CEntity*>(intels[0].data(),I_VEHSCAN+4*s)=e<3 ? nullptr : e==3 ? (CEntity*)Veh(int(next()%NVEH)) : (CEntity*)Ped(2+int(next()%3)); }
                                }
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x30,true);
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x2C,true);
                            json acc=json::array(); for(auto& a:accidents)acc.push_back(Region(a.data(),8)); out["accidents"]=acc;
                        }
                        out["refs"]=AllRefs(); out["rng"]=rngCalls; out["acc_calls"]=accCalls;
                    }
                    else if(cls=="CTaskComplexOnFire") {
                        // peaton en llamas: flags fisicos (+0x40: bit 8 sumergido, bit 19 a prueba de fuego), fuego +0x730, multiplicador +0x734
                        At<uint32>(p0,PH_FLAGS)=(next()%4==0 ? 0x100u : 0u)|(next()%4==0 ? 0x80000u : 0u)|(next()&0x0001E000u);
                        At<CFire*>(p0,P_FIRE)=next()%4 ? reinterpret_cast<CFire*>(fire.data()) : nullptr; At<float>(p0,P_FIREMULT)=next()%3 ? 1.f : uni(0.f,4.f);
                        if(nm=="Constructor"){
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o));
                            ctorDone(0xC);
                        } else if(nm=="ComputeFireDamage"){
                            reinterpret_cast<void(__cdecl*)(CPed*,CPedDamageResponse*)>(target)(p0,reinterpret_cast<CPedDamageResponse*>(respBuf.data()));
                            for(auto* h:hooks)h->State(false);
                            out["resp"]=Region(respBuf.data(),0xC);
                        } else {
                            setTask(VT_OF,0xC);
                            if(nm=="Destructor"){ dtorSub(); callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_COMPLEX_DIE,TASK_COMPLEX_SMART_FLEE_ENTITY,TASK_FINISHED,TASK_SIMPLE_PAUSE};
                                auto* r=callSub1(t[next()%std::size(t)]); collectFresh(r); ret=Name(r);
                            } else {
                                static const int32 t[]{TASK_COMPLEX_SMART_FLEE_ENTITY,TASK_COMPLEX_SMART_FLEE_ENTITY,TASK_COMPLEX_DIE,TASK_SIMPLE_PAUSE};
                                setSub(t,std::size(t));
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x30,true);
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0xC,true);
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
