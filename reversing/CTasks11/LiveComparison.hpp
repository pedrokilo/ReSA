#pragma once
// Banco dentro del motor: respuesta editable, conducir, ser arrastrado y huir a un punto (lista de 11, 44 funciones).
// CEventEditableResponse (constructor, Clone, HasEditableResponse, WillRespond, InformVehicleOccupants,
// InformRespectedFriends, InformGroup, TriggerLookAt, ComputeResponseTaskType ped/grupo, ComputeResponseTaskOfType),
// CTaskComplexCarDrive (dos constructores, destructor, Clone, ControlSubTask, CreateFirstSubTask, CreateNextSubTask,
// CreateSubTask, CreateSubTaskCannotGetInCar, Drive, SetUpCar), CTaskComplexCarSlowBeDraggedOut (constructor,
// destructor, Clone, GetTaskType, MakeAbortable, ControlSubTask, CreateFirstSubTask, CreateNextSubTask, CreateSubTask,
// CreateTaskUtilityLineUpPedWithCar, PrepareVehicleForPedExit) y CTaskComplexFleePoint (constructor, destructor, Clone,
// GetTaskType, MakeAbortable, ControlSubTask, CreateFirstSubTask, CreateNextSubTask, CreateSubTask, ComputeTargetPoint,
// SetFleePosition).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 6 peatones (inteligencia propia, conocidos, datos de jugador y CWanted falsos), 2 vehiculos (vtable falsa con
// CanPedStepOutCar trazado), tareas falsas (vtable trazada), evento falso derivado de CEventEditableResponse (vtable
// copiada de la del EXE 0x85AB80 en cada ejecucion con GetEventType, GetSourceEntity, DoInformVehicleOccupants,
// CloneEditable y el destructor trazados; clones en buffers propios) y un CPedGroup falso.
// Se simulan y trazan: rand/CGeneral::GetRandomNumber, CPed::Say, IKChainManager_c::LookAt/IsLooking/AbortLookAt,
// CEventGroup::Add, CInformFriendsEventQueue::Add, CInformGroupEventQueue::Add, CPedGroups::GetPedsGroup,
// CEventSource::ComputeEventSourceType, CDecisionMakerTypes::MakeDecision (peaton y grupo), FindPlayerPed,
// CTaskSimpleCarSetPedSlowDraggedOut::ProcessPed y operator new/delete (solo durante la llamada, registro sin reservas).
// Pool de tareas: m_LastFreeSlot y estados restaurados y slots libres con 0xCD en cada ejecucion; las tareas nuevas se
// serializan y se liberan sin destructor (antes se limpian las referencias de los fixtures). Se guarda y restaura el reloj.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Tasks/Task.h"
#include "Events/EventEditableResponse.h"
#include "Events/EventGroup.h"
#include "TaskComplexCarDrive.h"
#include "TaskComplexCarSlowBeDraggedOut.h"
#include "TaskComplexFleePoint.h"
#include "TaskSimpleCarSetPedSlowDraggedOut.h"
#include "InformGroupEventQueue.h"
#include "InformFriendsEventQueue.h"
#include "PedGroups.h"
#include "Ragdoll/IKChainManager.h"
#include "PedIntelligence.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks11Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }
inline uintptr_t Body(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

constexpr int NPEDS=6, NVEH=2, FAKES=3, NCLONES=4, NLINE=4;
// tareas falsas: campos de control por encima de 0x50
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_SUB=0x64, FK_TYPE=0x68, FK_CLONE=0x6C, FK_CNT=0x70;
// desplazamientos crudos del EXE
constexpr size_t E_MATRIX=0x14, E_TYPE=0x36;                                                                   // CEntity
constexpr size_t P_FLAGS=0x46C, P_INTEL=0x47C, P_PDATA=0x480, P_ACQ=0x4E0, P_STATE=0x530, P_ROT=0x558, P_VEH=0x58C, P_PEDTYPE=0x598; // CPed
constexpr size_t V_AP_STYLE=0x3B9, V_AP_MISSION=0x3BA, V_AP_SPEED=0x3D0, V_DRIVER=0x460, V_PASS=0x464, V_MAXPASS=0x488, V_SUBTYPE=0x590; // CVehicle
constexpr size_t I_EVGROUP=0x68, I_NUMSCAN=0xC4, I_RADIUS=0xC8, I_PEDENTS=0x130;                               // CPedIntelligence
constexpr size_t EV_TYPE=0x30, EV_SRC=0x34, EV_INFORM=0x38;                                                    // evento falso (control)
constexpr size_t G_MEMBERS=0xC, G_LEADER=0x28, PEDGROUP_SIZE=0x2D4;                                           // CPedGroup (pertenencia en +8)
constexpr uintptr_t TIMER=0xB7CB84, IKCHAIN=0xC15448;
constexpr uintptr_t VT_EER=0x85AB80, VT_DRIVE=0x86E934, VT_SLOW=0x86EF54, VT_FLEE=0x86F6CC;
constexpr uint32 IN_VEH=0x100u;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,0x40>,NCLONES> clones{};
alignas(16) inline std::array<std::array<uint8,0x40>,NLINE> lineArena{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<uint8,PEDGROUP_SIZE> group{};
alignas(16) inline std::array<uint8,0x10> playerData{};
alignas(16) inline std::array<uint8,0x40> wanted{};
alignas(16) inline std::array<uint8,0x10> vecBuf{};
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
inline std::array<void*,20> eventVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(vehVtbl.data()))return "veh_vt"; if(v==uintptr_t(eventVtbl.data()))return "event_vt";
    if(v==IKCHAIN)return "ik_chain_manager";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n);
                              if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NCLONES;++n)if(auto s=in(clones[n].data(),clones[n].size(),"clone"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NLINE;++n)if(auto s=in(lineArena[n].data(),lineArena[n].size(),"lineup"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(group.data(),group.size(),"group"))return *s;
    if(auto s=in(playerData.data(),playerData.size(),"pdata"))return *s; if(auto s=in(wanted.data(),wanted.size(),"wanted"))return *s;
    if(auto s=in(vecBuf.data(),vecBuf.size(),"vec"))return *s;
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    // locales de la pila (resultado de MakeDecision en ComputeResponseTaskOfType, tareas temporales): no comparables
    { const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); if(v>=uintptr_t(tib->StackLimit)&&v<uintptr_t(tib->StackBase))return "stack"; }
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); for(auto& v:vehs)r.push_back(RefList(v.data())); return r; }
// tarea serializada sin vptr (se compara el tipo), todo el slot de 0x80
inline json SerTask(const void* p,size_t bytes=TASK_SLOT) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType(); const auto* w=static_cast<const uint32*>(p);
    json words=json::array(); for(size_t k=1;k<bytes/4;++k)words.push_back(Name(w[k]));
    return {{"type",type},{"words",words}};
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{};
inline uint32 cloneCalls{}, friendsMask{}, friendsCalls{}, groupQueueRet{}, lookMask{}, lookCalls{}, stepOutMask{}, stepOutCalls{};
inline int32 srcTypeRet{}; inline int16 decTask{}, decFacial{}; inline std::array<int32,4> decSeq{}; inline uint32 decCalls{};
inline uintptr_t pedsGroupRet{};
// registro sin reservas para operator new/delete (la traza json reservaria memoria a traves del operator new parcheado)
struct Rec { uint32 kind, a; };
inline std::array<Rec,16> recs{}; inline uint32 nrec{}, newCalls{};
inline void Push(uint32 k,uint32 a){ if(nrec<recs.size())recs[nrec++]={k,a}; }
inline void* __cdecl FakeNew(size_t n){ Push(1,uint32(n)); if(n>0x40||newCalls>=NLINE)return nullptr; auto* p=lineArena[newCalls++].data(); return p; }
inline void __cdecl FakeDelete(void* p){ Push(2,uint32(uintptr_t(p))); }
inline void FlushRecs(){ for(uint32 k=0;k<nrec;++k)trace.push_back({recs[k].kind==1 ? "new":"delete",recs[k].kind==1 ? json(recs[k].a) : Name(recs[k].a)}); nrec=0; }

// tareas falsas: MakeAbortable devuelve el bit FK_CNT-esimo de la mascara FK_ABORT
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(t)}); return At<CTask*>(t,FK_CLONE); }
inline CTask* __fastcall FSub(CTask* t,void*) { return At<CTask*>(t,FK_SUB); }
inline bool __fastcall FSimple(CTask*,void*) { return true; }
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) {
    const uint32 k=At<uint32>(t,FK_CNT)++; const bool r=(At<uint32>(t,FK_ABORT)>>(k&31))&1;
    trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e),r}); return r;
}
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<size_t S,int... N> void FillTraps(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// vehiculo falso: slot 57 = CanPedStepOutCar
inline bool __fastcall FStepOut(CVehicle* v,void*,uint32 ignore) { const bool r=(stepOutMask>>(stepOutCalls++&31))&1; trace.push_back({"can_ped_step_out_car",Name(v),ignore&0xFF,r}); return r; }
// evento falso (slots 0, 1, 10, 13 y 16; el resto se copia de la vtable del EXE)
inline void* __fastcall EDtor(CEvent* e,void*,unsigned f) { trace.push_back({"delete_event",Name(e),f}); return e; }
inline int32 __fastcall EType(CEvent* e,void*) { trace.push_back({"event_type",Name(e)}); return At<int32>(e,EV_TYPE); }
inline CEntity* __fastcall ESource(CEvent* e,void*) { trace.push_back({"source_entity",Name(e)}); return At<CEntity*>(e,EV_SRC); }
inline bool __fastcall EInform(CEvent* e,void*,CPed* p) { trace.push_back({"do_inform_vehicle_occupants",Name(e),Name(p)}); return At<uint8>(e,EV_INFORM)!=0; }
inline CEvent* __fastcall ECloneEditable(CEvent* e,void*) { auto* c=clones[cloneCalls++%NCLONES].data(); trace.push_back({"clone_editable",Name(e),Name(c)}); return reinterpret_cast<CEvent*>(c); }
inline json EvFields(const void* e) { return {At<uint8>(e,0xC),At<int16>(e,0xE),At<int16>(e,0x10)}; }

// dependencias simuladas
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline int16 __fastcall FakeSay(CPed* p,void*,int ctx,uint32 delay,float prob,uint32 overrideSilence,uint32 forceAudible,uint32 frontEnd) {
    trace.push_back({"say",Name(p),int16(ctx),delay,F(prob),overrideSilence&0xFF,forceAudible&0xFF,frontEnd&0xFF}); return -1;
}
inline void __fastcall FakeLookAt(void* m,void*,const char* purpose,CPed* p,CEntity* e,int32 time,int32 bone,CVector* off,uint32 torso,float speed,int32 blend,int32 prio,uint32 force) {
    std::string s; for(int k=0;k<32&&purpose&&purpose[k];++k)s+=purpose[k];
    trace.push_back({"look_at",Name(m),s,Name(p),Name(e),time,bone,Name(off),torso&0xFF,F(speed),blend,prio,force&0xFF});
}
inline bool __fastcall FakeIsLooking(void* m,void*,CPed* p) { const bool r=(lookMask>>(lookCalls++&31))&1; trace.push_back({"is_looking",Name(m),Name(p),r}); return r; }
inline void __fastcall FakeAbortLookAt(void* m,void*,CPed* p,uint32 blend) { trace.push_back({"abort_look_at",Name(m),Name(p),blend}); }
inline CEvent* __fastcall FakeEventAdd(void* g,void*,CEvent* e,uint32 valid) { trace.push_back({"event_group_add",Name(g),Name(e),EvFields(e),valid&0xFF}); return e; }
inline bool __cdecl FakeFriendsAdd(CPed* p,CEvent* e) { const bool r=(friendsMask>>(friendsCalls++&31))&1; trace.push_back({"inform_friends_add",Name(p),Name(e),EvFields(e),r}); return r; }
inline bool __cdecl FakeGroupQueueAdd(CPed* p,CPedGroup* g,CEventEditableResponse* e) { trace.push_back({"inform_group_add",Name(p),Name(g),Name(e),EvFields(e),groupQueueRet!=0}); return groupQueueRet!=0; }
inline CPedGroup* __cdecl FakePedsGroup(const CPed* p) { trace.push_back({"get_peds_group",Name(p),Name(pedsGroupRet)}); return reinterpret_cast<CPedGroup*>(pedsGroupRet); }
inline int32 __cdecl FakeSourceType(const CEvent* e,const CPed* p) { trace.push_back({"compute_event_source_type",Name(e),Name(p)}); return srcTypeRet; }
inline void __fastcall FakeDecisionPed(void* self,void*,CPed* p,int32 et,int32 src,uint32 inVeh,int32 a1,int32 a2,int32 a3,int32 seek,uint32 grp,int16* task,int16* facial) {
    // los locales de ComputeResponseTaskOfType no estan inicializados en el EXE (la DLL los pone a -1): valor previo enmascarado
    const auto prev=[](int16* v)->json{ return Name(v)=="stack" ? json("uninit") : json(*v); };
    trace.push_back({"make_decision_ped",Name(p),et,src,inVeh&0xFF,a1,a2,a3,seek,grp&0xFF,Name(task),Name(facial),prev(task),prev(facial)});
    *task=decTask; *facial=decFacial;
}
inline int32 __fastcall FakeDecisionGroup(void* self,void*,CPedGroup* g,int32 et,int32 src,uint32 inVeh,int32 t1,int32 t2,int32 t3,int32 t4) {
    const int32 r=decSeq[decCalls++%decSeq.size()]; trace.push_back({"make_decision_group",Name(g),et,src,inVeh&0xFF,t1,t2,t3,t4,r}); return r;
}
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(Ped(5)); }
inline bool __fastcall FakeSetPedSlowProcess(CTask* t,void*,CPed* p) { trace.push_back({"set_ped_slow_dragged_out_process",Name(p),Name(At<void*>(t,8)),At<int32>(t,0xC)}); return true; }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t OurVirtual(const char* cls,const char* fn){ auto* h=FindHook(cls,fn); if(!h||h->Symbol()[0]!='V')return 0; return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress()); }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=44;
inline const std::array<Fn,NFNS> fns{{
    {"CEventEditableResponse","Clone",0x420ED0,0x85AB80,4,128},
    {"CEventEditableResponse","ComputeResponseTaskOfType",0x4B5730,0,-1,256},
    {"CEventEditableResponse","ComputeResponseTaskType-group",0x4B57A0,0,-1,512},
    {"CEventEditableResponse","ComputeResponseTaskType-ped",0x4B56C0,0,-1,256},
    {"CEventEditableResponse","Constructor",0x4AC450,0,-1,64},
    {"CEventEditableResponse","HasEditableResponse",0x420EF0,0x85AB80,9,64},
    {"CEventEditableResponse","InformGroup",0x4B7DF0,0,-1,256},
    {"CEventEditableResponse","InformRespectedFriends",0x4B2B00,0,-1,1024},
    {"CEventEditableResponse","InformVehicleOccupants",0x4AC4A0,0,-1,512},
    {"CEventEditableResponse","TriggerLookAt",0x4AC5A0,0,-1,128},
    {"CEventEditableResponse","WillRespond",0x4AC490,0,-1,64},
    {"CTaskComplexCarDrive","Clone",0x63DC90,0x86E934,1,128},
    {"CTaskComplexCarDrive","Constructor_0",0x63C9D0,0,-1,128},
    {"CTaskComplexCarDrive","Constructor_1",0x63C940,0,-1,128},
    {"CTaskComplexCarDrive","ControlSubTask",0x645240,0x86E934,10,512},
    {"CTaskComplexCarDrive","CreateFirstSubTask",0x645100,0x86E934,9,512},
    {"CTaskComplexCarDrive","CreateNextSubTask",0x644E20,0x86E934,8,1024},
    {"CTaskComplexCarDrive","CreateSubTask",0x642FA0,0,-1,512},
    {"CTaskComplexCarDrive","CreateSubTaskCannotGetInCar",0x643200,0x86E934,12,64},
    {"CTaskComplexCarDrive","Destructor",0x63CA40,0,-1,128},
    {"CTaskComplexCarDrive","Drive",0x63CAD0,0x86E934,13,64},
    {"CTaskComplexCarDrive","SetUpCar",0x63CAE0,0x86E934,11,64},
    {"CTaskComplexCarSlowBeDraggedOut","Clone",0x64A120,0x86EF54,1,128},
    {"CTaskComplexCarSlowBeDraggedOut","Constructor",0x648490,0,-1,128},
    {"CTaskComplexCarSlowBeDraggedOut","ControlSubTask",0x648510,0x86EF54,10,64},
    {"CTaskComplexCarSlowBeDraggedOut","CreateFirstSubTask",0x64C8B0,0x86EF54,9,512},
    {"CTaskComplexCarSlowBeDraggedOut","CreateNextSubTask",0x64C810,0x86EF54,8,512},
    {"CTaskComplexCarSlowBeDraggedOut","CreateSubTask",0x64C6A0,0,-1,256},
    {"CTaskComplexCarSlowBeDraggedOut","CreateTaskUtilityLineUpPedWithCar",0x648520,0,-1,128},
    {"CTaskComplexCarSlowBeDraggedOut","Destructor",0x64C580,0,-1,128},
    {"CTaskComplexCarSlowBeDraggedOut","GetTaskType",0x648500,0x86EF54,4,64},
    {"CTaskComplexCarSlowBeDraggedOut","MakeAbortable",0x64C600,0x86EF54,6,256},
    {"CTaskComplexCarSlowBeDraggedOut","PrepareVehicleForPedExit",0x6485A0,0,-1,256},
    {"CTaskComplexFleePoint","Clone",0x65CDE0,0x86F6CC,1,128},
    {"CTaskComplexFleePoint","ComputeTargetPoint",0x65B820,0,-1,512},
    {"CTaskComplexFleePoint","Constructor",0x65B390,0,-1,128},
    {"CTaskComplexFleePoint","ControlSubTask",0x65B660,0x86F6CC,10,1024},
    {"CTaskComplexFleePoint","CreateFirstSubTask",0x65D100,0x86F6CC,9,256},
    {"CTaskComplexFleePoint","CreateNextSubTask",0x65D150,0x86F6CC,8,512},
    {"CTaskComplexFleePoint","CreateSubTask",0x65B460,0,-1,256},
    {"CTaskComplexFleePoint","Destructor",0x65B410,0,-1,64},
    {"CTaskComplexFleePoint","GetTaskType",0x65B400,0x86F6CC,4,64},
    {"CTaskComplexFleePoint","MakeAbortable",0x65B420,0x86F6CC,6,256},
    {"CTaskComplexFleePoint","SetFleePosition",0x65B7B0,0,-1,512},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks11-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks11-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks11");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks11-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    const uintptr_t dllSetPedSlow=OurVirtual("CTaskSimpleCarSetPedSlowDraggedOut","ProcessPed");
    if(!dllSetPedSlow){Write({{"event","error"},{"reason","virtual hook not found"},{"function","CTaskSimpleCarSetPedSlowDraggedOut::ProcessPed"}});return;}
    FillTraps(fakeVtbl,std::make_integer_sequence<int,16>{}); FillTraps(vehVtbl,std::make_integer_sequence<int,96>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    vehVtbl[57]=reinterpret_cast<void*>(&FStepOut);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: el reloj; se guarda y se restaura en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4}};
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
        EntryPatch r1(Body(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber)),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch s1(Body(bit_cast<uintptr_t>(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say))),FakeSay), s1e(0x5EFFE0,FakeSay);
        EntryPatch k1(Body(bit_cast<uintptr_t>(&IKChainManager_c::LookAt)),FakeLookAt), k1e(0x618970,FakeLookAt);
        EntryPatch k2(Body(bit_cast<uintptr_t>(&IKChainManager_c::IsLooking)),FakeIsLooking), k2e(0x6181A0,FakeIsLooking);
        EntryPatch k3(Body(bit_cast<uintptr_t>(&IKChainManager_c::AbortLookAt)),FakeAbortLookAt), k3e(0x618280,FakeAbortLookAt);
        EntryPatch e1(Body(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add))),FakeEventAdd), e1e(0x4AB420,FakeEventAdd);
        EntryPatch q1(Body(bit_cast<uintptr_t>(&CInformFriendsEventQueue::Add)),FakeFriendsAdd), q1e(0x4AC1E0,FakeFriendsAdd);
        EntryPatch q2(Body(bit_cast<uintptr_t>(&CInformGroupEventQueue::Add)),FakeGroupQueueAdd), q2e(0x4B7CD0,FakeGroupQueueAdd);
        EntryPatch g1(Body(bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup)),FakePedsGroup), g1e(0x5F7E80,FakePedsGroup);
        // ComputeEventSourceType y las dos MakeDecision de la DLL llaman a la direccion del EXE
        EntryPatch d1e(0x4ABAC0,FakeSourceType), d2e(0x606E70,FakeDecisionPed), d3e(0x606F80,FakeDecisionGroup);
        EntryPatch p1(Body(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed))),FakeFindPlayerPed), p1e(0x56E210,FakeFindPlayerPed);
        EntryPatch t1(Body(dllSetPedSlow),FakeSetPedSlowProcess), t1e(0x64C220,FakeSetPedSlowProcess);

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
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1111u; if(!st)st=1; trace=json::array(); seed=uint32(i);
                    rngCalls=0; freshIds.clear(); cloneCalls=0; friendsCalls=0; lookCalls=0; stepOutCalls=0; decCalls=0; nrec=0; newCalls=0;
                    CleanRefs(); restoreGlobals();
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    const uint32 now=At<uint32>(reinterpret_cast<void*>(TIMER),0);
                    friendsMask=next(); groupQueueRet=next()%3!=0; lookMask=next(); stepOutMask=next();
                    { static const int32 srcs[]{0,1,2,3,4,5,-1}; srcTypeRet=srcs[next()%std::size(srcs)]; }
                    { static const int16 tk[]{200,300,1200,1700,405,-1,0x7123}; decTask=tk[next()%std::size(tk)]; decFacial=int16(int32(next()%30)-1); }
                    for(auto& d:decSeq){ static const int32 tk[]{200,200,300,1200,1700,0x12345,-1}; d=tk[next()%std::size(tk)]; }
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,FAKE_SIZE); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_PAUSE; At<uint32>(fakes[n].data(),FK_ABORT)=next(); }
                    // datos de jugador del peaton 5 (FindPlayerPed): CWanted con el nivel en +0x2C
                    std::memset(playerData.data(),0,playerData.size()); std::memset(wanted.data(),0,wanted.size());
                    At<void*>(playerData.data(),0)=wanted.data(); At<int32>(wanted.data(),0x2C)=next()%3 ? 0 : int32(1+next()%6);
                    // peatones: matriz, inteligencia, tipo, flags, vehiculo, conocidos, estado, rotacion
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                        p[E_TYPE]=uint8(3|(next()%8)<<3); At<void*>(p,P_INTEL)=intels[n].data(); At<void*>(p,E_MATRIX)=pedMats[n].data();
                        for(int r=0;r<3;++r)At<CVector>(pedMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(pedMats[n].data(),0x30)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                        { static const int32 types[]{0,1,4,6,6,7,8,5,6}; At<int32>(p,P_PEDTYPE)=types[next()%std::size(types)]; }
                        if(next()%2)At<uint32>(p,P_FLAGS)|=IN_VEH;
                        { const uint32 m=next()%4; At<CVehicle*>(p,P_VEH)=m==0 ? nullptr : m==3 ? Veh(1) : Veh(0); }
                        At<uint32>(p,P_ACQ)=next()&next(); At<int32>(p,P_STATE)=int32(next()%60); At<float>(p,P_ROT)=uni(-4.f,4.f);
                    }
                    At<int32>(peds[5].data(),P_PEDTYPE)=int32(next()%2); At<void*>(peds[5].data(),P_PDATA)=playerData.data();
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%32)<<3); At<void*>(v,E_MATRIX)=vehMats[n].data();
                        for(int r=0;r<3;++r)At<CVector>(vehMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        { const uint32 m=next()%6; if(m==0)At<float>(vehMats[n].data(),0x28)=0.3f; else if(m==1)At<float>(vehMats[n].data(),0x28)=0.f; else if(m==2)At<float>(vehMats[n].data(),0x28)=-0.f; }
                        At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                        At<int32>(v,V_SUBTYPE)=next()%4==0 ? 9 : next()%6==0 ? 5 : 0;
                        v[V_AP_STYLE]=uint8(next()); v[V_AP_MISSION]=uint8(next()); v[V_AP_SPEED]=uint8(next());
                        At<CPed*>(v,V_DRIVER)=next()%3 ? Ped(int(next()%NPEDS)) : nullptr;
                        for(int s=0;s<8;++s)At<CPed*>(v,V_PASS+4*s)=next()%3==0 ? Ped(int(next()%NPEDS)) : nullptr;
                        v[V_MAXPASS]=uint8(next()%9);
                    }
                    std::memset(group.data(),0,group.size()); std::memset(vecBuf.data(),0,vecBuf.size());
                    for(auto& c:clones){ for(auto& b:c)b=uint8(next()); At<void*>(c.data(),0)=eventVtbl.data(); }
                    for(auto& l:lineArena)std::memset(l.data(),0xCD,l.size());
                    std::memset(object.data(),0xCD,object.size());

                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    // vtable del evento falso: la del EXE (con el estado actual de los hooks) y los slots trazados
                    { const auto* ev=reinterpret_cast<void* const*>(VT_EER); for(int k=0;k<17;++k)eventVtbl[k]=ev[k]; for(int k=17;k<20;++k)eventVtbl[k]=nullptr;
                      eventVtbl[0]=reinterpret_cast<void*>(&EDtor); eventVtbl[1]=reinterpret_cast<void*>(&EType); eventVtbl[10]=reinterpret_cast<void*>(&ESource);
                      eventVtbl[13]=reinterpret_cast<void*>(&EInform); eventVtbl[16]=reinterpret_cast<void*>(&ECloneEditable); }
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches}}; json ret=nullptr; std::vector<int> freshSlots;
                    auto* o=object.data(); auto* p0=Ped(0); auto* p0b=peds[0].data();
                    auto hooksOff=[&]{ for(auto* h:hooks)h->State(false); };
                    auto collectFresh=[&]{
                        hooksOff();
                        auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                        for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshSlots.push_back(int(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                        json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t)); out["tasks"]=tasks;
                    };
                    auto setTask=[&](uintptr_t vtbl,size_t size){ for(size_t b=0;b<size;++b)o[b]=uint8(next()); At<uintptr_t>(o,0)=vtbl; At<uint32>(o,4)=0; At<uint32>(o,8)=0; };
                    auto setSub=[&](const int32* types,size_t n){ At<CTask*>(o,8)=Fake(0); At<int32>(fakes[0].data(),FK_TYPE)=types[next()%n]; };
                    auto typeOf=[&](void* p){ return reinterpret_cast<CTask*>(p)->GetTaskType(); };
                    auto callTask=[&](CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    auto callSub1=[&](int32 type){ return reinterpret_cast<CTask*(__thiscall*)(void*,int32)>(target)(o,type); };
                    auto callSub2=[&](int32 type,CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,type,ped); };
                    auto callClone=[&]{ return reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); };
                    auto callVoid=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); };
                    auto callPedVoid=[&](CPed* ped){ reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    auto callAbort=[&](CPed* ped,int32 prio,const CEvent* e){ return reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,ped,prio,e); };
                    auto ctorDone=[&](size_t size){ hooksOff(); out["object"]=Region(o,size,true); out["type"]=typeOf(o); };
                    auto dtorSub=[&]{ At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; };
                    auto prefill=[&]{ std::memset(o,0xCD,object.size()); };
                    auto finish=[&](size_t size){ hooksOff(); out["object"]=Region(o,size,true); };
                    auto pedPos=[&](int n)->CVector&{ return At<CVector>(pedMats[n].data(),0x30); };
                    auto vehState=[&]{ json r=json::array(); for(int n=0;n<NVEH;++n){ auto* v=vehs[n].data(); r.push_back({v[E_TYPE],v[V_AP_STYLE],v[V_AP_MISSION],v[V_AP_SPEED]}); } return r; };
                    auto pedState=[&]{ return json{Region(pedMats[0].data(),0x40),At<int32>(p0b,P_STATE),At<uint32>(p0b,P_FLAGS)}; };
                    // peaton 0 dentro o fuera de un vehiculo concreto
                    auto pedInVeh=[&](CVehicle* v,bool flag){ At<CVehicle*>(p0b,P_VEH)=v; auto& fl=At<uint32>(p0b,P_FLAGS); fl=flag ? (fl|IN_VEH) : (fl&~IN_VEH); };
                    auto pickVehState=[&]{ const uint32 m=next()%6; pedInVeh(m==0||m==1 ? Veh(0) : m==2 ? Veh(1) : m==3 ? nullptr : Veh(0), m==0||m==2||(m==3&&next()%2)); };
                    auto withHeap=[&](auto&& call){ { EntryPatch nw(0x82119A,FakeNew), dl(0x8214BD,FakeDelete); call(); } FlushRecs(); };

                    if(nm=="GetTaskType") {
                        setTask(fn.vt,0x40);
                        ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        out["object"]=Region(o,0x40,true);
                    }
                    else if(cls=="CEventEditableResponse") {
                        // +4 tiempo activo, +8 valido, +0xC anadir al grupo, +0xE tarea, +0x10 expresion; control a partir de +0x30
                        if(nm=="Constructor"){
                            prefill(); ret=Name(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o));
                            hooksOff(); out["object"]=Region(o,0x20,true);
                        } else {
                            for(size_t b=0;b<0x40;++b)o[b]=uint8(next()); At<void*>(o,0)=eventVtbl.data();
                            o[0xC]=uint8(next()%3!=0);
                            { static const int16 tk[]{200,200,300,1200,1700,405,-1,0}; At<int16>(o,0xE)=tk[next()%std::size(tk)]; }
                            { static const int32 et[]{0,9,13,31,33,44,62}; At<int32>(o,EV_TYPE)=et[next()%std::size(et)]; }
                            { const uint32 m=next()%5; At<CEntity*>(o,EV_SRC)=m==0 ? nullptr : m==1 ? (CEntity*)Veh(0) : (CEntity*)Ped(int(1+next()%4)); }
                            o[EV_INFORM]=uint8(next()%4!=0);
                            auto* ev=reinterpret_cast<CEvent*>(o);
                            if(nm=="Clone"){ ret=Name(reinterpret_cast<CEvent*(__thiscall*)(void*)>(target)(o)); hooksOff(); }
                            else if(nm=="HasEditableResponse"||nm=="WillRespond"){ ret=reinterpret_cast<bool(__thiscall*)(void*)>(target)(o); hooksOff(); }
                            else if(nm=="TriggerLookAt"){ callPedVoid(Ped(int(next()%3))); hooksOff(); }
                            else if(nm=="InformGroup"){
                                pedsGroupRet=next()%4 ? uintptr_t(group.data()) : 0;
                                callPedVoid(p0); hooksOff();
                            }
                            else if(nm=="InformVehicleOccupants"){
                                // si el peaton esta en un vehiculo siempre lo tiene (el EXE lee el conductor sin comprobar)
                                if(At<uint32>(p0b,P_FLAGS)&IN_VEH){ if(!At<CVehicle*>(p0b,P_VEH))At<CVehicle*>(p0b,P_VEH)=Veh(0); }
                                auto* v=reinterpret_cast<uint8*>(At<CVehicle*>(p0b,P_VEH));
                                if(v&&next()%3==0)At<CPed*>(v,V_DRIVER)=p0;
                                if(v&&next()%3==0)At<CPed*>(v,V_PASS+4*(next()%8))=p0;
                                callPedVoid(p0); hooksOff();
                            }
                            else if(nm=="InformRespectedFriends"){
                                // 16 entidades del escaner: peatones 1..5 (o el propio 0) en posiciones a la distancia exacta del radio
                                auto* in=intels[0].data();
                                At<uint32>(in,I_NUMSCAN)=next()%6==0 ? 0u : uint32(next()%20);
                                const uint32 rm=next()%6; const float radius=rm==0 ? qnan : rm==1 ? 0.f : uni(0.f,40.f); At<float>(in,I_RADIUS)=radius;
                                for(int s=0;s<16;++s)At<CPed*>(in,I_PEDENTS+4*s)=next()%3==0 ? nullptr : Ped(next()%8==0 ? 0 : int(1+next()%5));
                                if(next()%3==0)At<int32>(p0b,P_PEDTYPE)=6; else if(next()%5==0)At<int32>(p0b,P_PEDTYPE)=int32(next()%2);
                                for(int n=1;n<NPEDS;++n){
                                    const uint32 m=next()%5; const CVector pp=pedPos(0);
                                    if(m==0)pedPos(n)=CVector{pp.x+radius,pp.y,pp.z}; else if(m==1)pedPos(n)=CVector{pp.x,pp.y,pp.z-radius};
                                    else if(m==2)pedPos(n)=CVector{pp.x+uni(-1.f,1.f)*radius,pp.y+uni(-1.f,1.f)*radius,pp.z};
                                    if(next()%8==0)At<void*>(peds[n].data(),E_MATRIX)=nullptr; // posicion de la colocacion (+4, ceros)
                                }
                                callPedVoid(p0); hooksOff();
                            }
                            else if(nm=="ComputeResponseTaskType-ped"){
                                reinterpret_cast<void(__thiscall*)(void*,CPed*,uint32)>(target)(o,p0,next()%2 ? 1u : 0xFFFFFF00u|uint32(next()%2)); hooksOff();
                            }
                            else if(nm=="ComputeResponseTaskOfType"){
                                static const int32 tk[]{300,1200,1700,405,200,-1};
                                const int32 tt=tk[next()%std::size(tk)]; if(next()%2)decTask=int16(tt);
                                ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32)>(target)(o,p0,tt); hooksOff();
                            }
                            else if(nm=="ComputeResponseTaskType-group"){
                                // lider (a veces jugador) y miembros 0..6
                                auto* g=group.data();
                                for(int s=0;s<7;++s)At<CPed*>(g,G_MEMBERS+4*s)=next()%3==0 ? Ped(int(1+next()%5)) : nullptr;
                                if(next()%3==0)for(int s=0;s<7;++s)At<CPed*>(g,G_MEMBERS+4*s)=nullptr;
                                At<CPed*>(g,G_LEADER)=next()%4==0 ? nullptr : next()%2 ? Ped(5) : Ped(int(1+next()%4));
                                reinterpret_cast<void(__thiscall*)(void*,void*)>(target)(o,g); hooksOff();
                            }
                            out["event"]=Region(o,0x30,true);
                            json cl=json::array(); for(auto& c:clones)cl.push_back(Region(c.data(),0x20,true)); out["clones"]=cl;
                            (void)ev;
                        }
                    }
                    else if(cls=="CTaskComplexCarDrive") {
                        // +0xC vehiculo (referencia), +0x10 velocidad, +0x14 modelo, +0x18 estilo, +0x1C como conductor,
                        // +0x1D..0x1F estado original del piloto, +0x20 coche preparado
                        if(nm=="Constructor_0"){
                            prefill(); CVehicle* v=next()%4 ? Veh(0) : nullptr;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*)>(target)(o,v));
                            ctorDone(0x24);
                        } else if(nm=="Constructor_1"){
                            prefill(); CVehicle* v=next()%4 ? Veh(0) : nullptr;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,float,int32,int32)>(target)(o,v,uni(0.f,60.f),next()%3 ? -1 : int32(next()%700),int32(next()%8)));
                            ctorDone(0x24);
                        } else {
                            setTask(VT_DRIVE,0x24);
                            // CreateSubTask(CAR_DRIVE) llama a SetUpCar, que lee el vehiculo sin comprobar (EXE y DLL)
                            const bool needVeh=nm=="SetUpCar"||nm=="CreateSubTask";
                            CVehicle* v=(needVeh||next()%4) ? Veh(int(next()%4==0)) : nullptr; At<CVehicle*>(o,0xC)=v; Reg(v,o+0xC);
                            At<float>(o,0x10)=uni(0.f,60.f);
                            { const uint32 m=next()%5; At<int32>(o,0x14)=m==0 ? -1 : m==1 ? -2 : m==2 ? int32(400+next()%200) : m==3 ? 0 : -1; }
                            At<int32>(o,0x18)=int32(next()%8); o[0x1C]=uint8(next()%2); o[0x20]=uint8(next()%2);
                            pickVehState();
                            if(nm=="Destructor"){ dtorSub(); callVoid(); finish(0x24); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(); ret=Name(r); finish(0x24); }
                            else if(nm=="SetUpCar"){ callVoid(); finish(0x24); }
                            else if(nm=="Drive"){ setSub(std::array<int32,1>{TASK_SIMPLE_CAR_DRIVE}.data(),1); ret=Name(callTask(p0)); finish(0x24); }
                            else if(nm=="CreateSubTaskCannotGetInCar"){ auto* r=callTask(p0); collectFresh(); ret=Name(r); finish(0x24); }
                            else if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_COMPLEX_ENTER_CAR_AS_PASSENGER,TASK_COMPLEX_ENTER_CAR_AS_DRIVER,TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_CAR_DRIVE,
                                                       TASK_COMPLEX_LEAVE_ANY_CAR,TASK_COMPLEX_ENTER_ANY_CAR_AS_DRIVER,TASK_SIMPLE_CREATE_CAR_AND_GET_IN,TASK_COMPLEX_WANDER,
                                                       TASK_FINISHED,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_GO_TO_POINT_ANY_MEANS,706};
                                if(next()%6==0)At<void*>(p0b,E_MATRIX)=nullptr; // posicion de la colocacion
                                auto* r=callSub2(t[next()%std::size(t)],p0); collectFresh(); ret=Name(r); finish(0x24);
                            }
                            else {
                                static const int32 tn[]{TASK_COMPLEX_ENTER_CAR_AS_PASSENGER,TASK_COMPLEX_ENTER_CAR_AS_DRIVER,TASK_SIMPLE_CAR_DRIVE,TASK_COMPLEX_LEAVE_ANY_CAR,
                                                        TASK_COMPLEX_ENTER_ANY_CAR_AS_DRIVER,TASK_COMPLEX_ENTER_ANY_CAR_AS_DRIVER,TASK_SIMPLE_CREATE_CAR_AND_GET_IN,
                                                        TASK_SIMPLE_CREATE_CAR_AND_GET_IN,TASK_COMPLEX_GO_TO_POINT_ANY_MEANS,TASK_SIMPLE_STAND_STILL,706};
                                static const int32 tc[]{TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_CAR_DRIVE,TASK_COMPLEX_GO_TO_POINT_ANY_MEANS,TASK_COMPLEX_GO_TO_POINT_ANY_MEANS,
                                                        TASK_COMPLEX_ENTER_CAR_AS_DRIVER,TASK_SIMPLE_STAND_STILL};
                                if(nm=="ControlSubTask")setSub(tc,std::size(tc)); else setSub(tn,std::size(tn));
                                // sin vehiculo, ENTER_CAR_* con el peaton dentro crearia CAR_DRIVE y SetUpCar leeria un puntero nulo
                                { const int32 ty=At<int32>(fakes[0].data(),FK_TYPE);
                                  if(!At<CVehicle*>(o,0xC)&&(ty==TASK_COMPLEX_ENTER_CAR_AS_PASSENGER||ty==TASK_COMPLEX_ENTER_CAR_AS_DRIVER))At<uint32>(p0b,P_FLAGS)&=~IN_VEH; }
                                auto* r=callTask(p0); collectFresh(); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x80,true); finish(0x24);
                            }
                        }
                        out["vehs"]=vehState(); out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexCarSlowBeDraggedOut") {
                        // +0xC vehiculo (referencia), +0x10 puerta, +0x14 arrastrado por si mismo, +0x18 utilidad de alineacion (operator new)
                        static const int32 doors[]{10,10,8,9,11,0};
                        if(nm=="Constructor"){
                            prefill(); CVehicle* v=next()%4 ? Veh(0) : nullptr;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32,uint32)>(target)(o,v,doors[next()%std::size(doors)],next()%2 ? 1u : 0xFFFFFF00u|uint32(next()%2)));
                            ctorDone(0x1C);
                        } else {
                            setTask(VT_SLOW,0x1C);
                            CVehicle* v=Veh(int(next()%4==0)); if(nm=="Destructor"||nm=="Clone"||nm=="ControlSubTask")v=next()%5 ? v : nullptr;
                            At<CVehicle*>(o,0xC)=v; Reg(v,o+0xC);
                            At<int32>(o,0x10)=doors[next()%std::size(doors)]; o[0x14]=uint8(next()%2);
                            At<void*>(o,0x18)=next()%3 ? lineArena[3].data() : nullptr;
                            pickVehState();
                            // sin subtarea: el destructor de la falsa trazaria en json con operator new parcheado
                            if(nm=="Destructor"){ At<CTask*>(o,8)=nullptr; withHeap([&]{ callVoid(); }); finish(0x1C); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(); ret=Name(r); finish(0x1C); }
                            else if(nm=="ControlSubTask"){ At<CTask*>(o,8)=Fake(0); ret=Name(callTask(p0)); finish(0x1C); }
                            else if(nm=="MakeAbortable"){ At<CTask*>(o,8)=Fake(0); ret=callAbort(p0,int32(next()%4),next()%3 ? reinterpret_cast<CEvent*>(vecBuf.data()) : nullptr); finish(0x1C); }
                            else if(nm=="PrepareVehicleForPedExit"){ callPedVoid(p0); finish(0x1C); }
                            else if(nm=="CreateTaskUtilityLineUpPedWithCar"){ withHeap([&]{ callPedVoid(p0); }); finish(0x1C); }
                            else if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_SIMPLE_CAR_FALL_OUT,TASK_FINISHED,TASK_SIMPLE_CAR_SET_PED_SLOW_DRAGGED_OUT,TASK_SIMPLE_CAR_SET_PED_OUT,
                                                       TASK_SIMPLE_CAR_SLOW_BE_DRAGGED_OUT,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_CAR_QUICK_BE_DRAGGED_OUT};
                                auto* r=callSub1(t[next()%std::size(t)]); collectFresh(); ret=Name(r); finish(0x1C);
                            }
                            else if(nm=="CreateFirstSubTask"){ CTask* r{}; withHeap([&]{ r=callTask(p0); }); collectFresh(); ret=Name(r); finish(0x1C); }
                            else {
                                static const int32 tn[]{TASK_SIMPLE_CAR_FALL_OUT,TASK_SIMPLE_CAR_SET_PED_OUT,TASK_SIMPLE_CAR_SLOW_BE_DRAGGED_OUT,TASK_SIMPLE_CAR_SET_PED_SLOW_DRAGGED_OUT,
                                                        TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_CAR_QUICK_BE_DRAGGED_OUT};
                                setSub(tn,std::size(tn));
                                auto* r=callTask(p0); collectFresh(); ret=Name(r); finish(0x1C);
                            }
                            out["sub"]=Region(fakes[0].data(),0x80,true);
                        }
                        json la=json::array(); for(auto& l:lineArena)la.push_back(Region(l.data(),0x1C)); out["lineup"]=la;
                        out["vehs"]=vehState(); out["ped"]=pedState(); out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexFleePoint") {
                        // +0xC punto del que huir, +0x18 destino, +0x24 duracion, +0x28 temporizador (inicio, intervalo, iniciado, parado),
                        // +0x34 distancia segura, +0x38 gritar, +0x39 nueva posicion
                        auto pickDur=[&]()->int32{ const uint32 m=next()%5; return m==0 ? -1 : m==1 ? -int32(2+next()%100) : m==2 ? 0 : int32(next()%20000); };
                        if(nm=="Constructor"){
                            prefill(); auto& pt=At<CVector>(vecBuf.data(),0); pt=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,const CVector*,uint32,float,int32)>(target)(o,&pt,next()%2 ? 1u : 0xFFFFFF00u|uint32(next()%2),uni(0.f,40.f),pickDur()));
                            ctorDone(0x3C);
                        } else {
                            setTask(VT_FLEE,0x3C);
                            At<CVector>(o,0xC)=next()%6==0 ? pedPos(0) : CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                            At<CVector>(o,0x18)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                            At<int32>(o,0x24)=pickDur();
                            At<uint32>(o,0x28)=next()%2 ? now-next()%30000 : now; At<int32>(o,0x2C)=pickDur(); o[0x30]=uint8(next()%3!=0); o[0x31]=uint8(next()%4==0);
                            if(next()%5==0)At<uint32>(o,0x28)=now-uint32(At<int32>(o,0x2C)); // justo cumplido
                            { const uint32 m=next()%6; At<float>(o,0x34)=m==0 ? qnan : m==1 ? 0.f : uni(0.f,12.f); }
                            o[0x38]=uint8(next()%2); o[0x39]=uint8(next()%3==0);
                            pickVehState();
                            if(nm=="Destructor"){ dtorSub(); callVoid(); finish(0x3C); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(); ret=Name(r); finish(0x3C); }
                            else if(nm=="MakeAbortable"){ At<CTask*>(o,8)=Fake(0); ret=callAbort(p0,int32(next()%4),next()%3 ? reinterpret_cast<CEvent*>(vecBuf.data()) : nullptr); finish(0x3C); }
                            else if(nm=="ComputeTargetPoint"){ if(next()%8==0)At<void*>(p0b,E_MATRIX)=nullptr; callPedVoid(p0); finish(0x3C); }
                            else if(nm=="SetFleePosition"){
                                auto& pt=At<CVector>(vecBuf.data(),0); const uint32 m=next()%5;
                                pt=m<2 ? At<CVector>(o,0xC) : CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                                if(m==1){ const uint32 c=next()%4; if(c<3)At<float>(vecBuf.data(),4*c)=next()%2 ? qnan : uni(-30.f,30.f); }
                                if(next()%5==0)At<float>(o,0xC+4*(next()%3))=qnan;
                                const float sd=next()%2 ? At<float>(o,0x34) : next()%4==0 ? qnan : uni(0.f,12.f);
                                reinterpret_cast<void(__thiscall*)(void*,const CVector*,float,uint32)>(target)(o,&pt,sd,next()%2 ? 1u : 0xFFFFFF00u|uint32(next()%2));
                                finish(0x3C);
                            }
                            else if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_GO_TO_POINT,TASK_COMPLEX_LEAVE_ANY_CAR,TASK_COMPLEX_SEQUENCE,
                                                       TASK_FINISHED,TASK_SIMPLE_STAND_STILL};
                                auto* r=callSub1(t[next()%std::size(t)]); collectFresh(); ret=Name(r); finish(0x3C);
                            }
                            else {
                                static const int32 tn[]{TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_COMPLEX_SEQUENCE,TASK_COMPLEX_LEAVE_ANY_CAR,TASK_SIMPLE_GO_TO_POINT,
                                                        TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_STAND_STILL};
                                static const int32 tc[]{TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,
                                                        TASK_COMPLEX_LEAVE_ANY_CAR,TASK_COMPLEX_SEQUENCE};
                                if(nm=="ControlSubTask")setSub(tc,std::size(tc)); else setSub(tn,std::size(tn));
                                // a veces el destino queda a la distancia segura exacta del punto del que se huye (no se puede fijar
                                // antes de ComputeTargetPoint, pero el rango aleatorio cubre los dos lados)
                                auto* r=callTask(p0); collectFresh(); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x80,true); finish(0x3C);
                            }
                        }
                        out["ped"]=pedState(); out["refs"]=AllRefs();
                    }
                    hooksOff();
                    // limpieza: referencias de los fixtures y slots de las tareas nuevas (sin destructor); el pool debe quedar como antes
                    out["ret"]=ret; out["trace"]=trace; out["rng_calls"]=rngCalls;
                    CleanRefs();
                    for(int k:freshSlots)GetTaskPool()->SetFreeAt(size_t(k),true);
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
