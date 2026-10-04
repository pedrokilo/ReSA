#pragma once
// Banco dentro del motor: tareas complejas A (lista de 6, 42 funciones).
// CTaskAllocatorKillThreatsBasic (constructor, destructor, GetType, ProcessGroup, AllocateTasks, ComputeClosestPeds),
// CTaskComplexBeStill (Clone, destructor, GetTaskType, ControlSubTask, CreateFirstSubTask, CreateNextSubTask),
// CTaskComplexDieInCar (ControlSubTask, CreateFirstSubTask, CreateNextSubTask, CreateSubTask, MakeAbortable,
// PreparePedVehicleForPedDeath), CTaskComplexDriveFireTruck (constructor, Clone, ControlSubTask, CreateFirstSubTask,
// CreateNextSubTask, CreateSubTask), CTaskComplexFallToDeath (constructor, CalcFall, ControlSubTask,
// CreateFirstSubTask, CreateNextSubTask, MakeAbortable), CTaskComplexGoToBoatSteeringWheel (constructor, destructor,
// Clone, ControlSubTask, CreateFirstSubTask, CreateNextSubTask) y CTaskComplexGoToPointAndStandStillTimed
// (constructor, Clone, ControlSubTask, CreateFirstSubTask, MakeAbortable, StopTimer).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 16 peatones (con inteligencia propia), 3 vehiculos (vtable falsa trazada), 2 grupos alineados con
// CPedGroups::ms_groups (el EXE compara grupos por su id), inteligencia de grupo, evento, incendios, clump,
// asociaciones de animacion, edificio de contacto, model info de vehiculo y tareas falsas (vtable trazada).
// Se simulan y trazan: CPedGroups::GetPedsGroup/GetGroupId, CPedGroupIntelligence::FlushTasks/SetEventResponseTask
// (la tarea de la pila se serializa en el momento), CInterestingEvents::Add, CPed::SetPedState,
// CPedIntelligence::ClearTasks, CEventGroup::Add, CEventHandler::IsTemporaryEvent, CVehicle::GetVehicleAppearance y
// CanPedStepOutCar (vtable), CCarCtrl::SwitchVehicleToRealPhysics, CFireManager::FindNearestFire,
// CPhysical::ApplyMoveForce, RpAnimBlendClumpGetAssociation, CAnimManager::BlendAnimation,
// CWorld::GetIsLineOfSightClear/ProcessVerticalLine, CCarEnterExit::IsPlayerToQuitCarEnter y el RNG.
// Pool de tareas: m_LastFreeSlot y estados se restauran y los slots libres se rellenan con 0xCD en cada ejecucion.
// CTimer::m_snTimeInMilliseconds y un hueco libre de CModelInfo::ms_modelInfoPtrs (model info falso) se guardan y se
// restauran. Se compara la memoria de los fixtures (punteros traducidos a nombres), las tareas nuevas del pool, las
// referencias, el retorno y la traza.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/Task.h"
#include "Tasks/TaskComplexSequence.h"
#include "Tasks/Allocators/TaskAllocatorKillThreatsBasic.h"
#include "TaskComplexBeStill.h"
#include "TaskComplexDieInCar.h"
#include "TaskComplexDriveFireTruck.h"
#include "TaskComplexFallToDeath.h"
#include "TaskComplexGoToBoatSteeringWheel.h"
#include "TaskComplexGoToPointAndStandStillTimed.h"
#include "PedGroup.h"
#include "PedGroups.h"
#include "PedGroupIntelligence.h"
#include "PedIntelligence.h"
#include "InterestingEvents.h"
#include "CarCtrl.h"
#include "CarEnterExit.h"
#include "FireManager.h"
#include "Events/EventGroup.h"
#include "Events/EventHandler.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "World.h"
#include "Entity/Vehicle/Automobile.h"
#include "Models/ModelInfo.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks6aLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }

constexpr int NPEDS=16, NVEH=3, FAKES=4, NEVC=12, NFIRE=2, NASSOC=2;
constexpr size_t FAKE_SIZE=0x50, FK_ABORT=0x40, FK_SUB=0x44, FK_TYPE=0x48, FK_CLONE=0x4C, EV_SIZE=0x40, EV_TYPE=0x30;
// desplazamientos crudos del EXE
constexpr size_t E_POS=4, E_MATRIX=0x14, E_RWOBJ=0x18, E_MODEL=0x22, E_TYPE=0x36;                // CEntity
constexpr size_t PH_FLAGS=0x40, PH_MOVE=0x44;                                                   // CPhysical
constexpr size_t P_FLAGS=0x46C, P_FLAGS2=0x470, P_FLAGS4=0x478, P_INTEL=0x47C, P_CREATED=0x484,
                 P_STATE=0x530, P_CONTACT=0x584, P_VEHICLE=0x58C, P_PEDTYPE=0x598;              // CPed
constexpr size_t V_AUTOPILOT=0x390, V_AP_SIZE=0x98, V_DRIVER=0x460, V_PASS=0x464, V_MAXPASS=0x488, V_SUBTYPE=0x594; // CVehicle
constexpr size_t I_HISTORY=0x38, I_EVGROUP=0x68;                                                // CPedIntelligence
constexpr size_t G_MEMBERS=0xC, GROUP_SIZE=0x2D4;                                               // CPedGroup
constexpr uintptr_t TIMER=0xB7CB84, MS_GROUPS=0xC09920, FIRE_MANAGER=0xB71F80;
constexpr uintptr_t VT_KTB=0x870E90, VT_STILL=0x86C7BC, VT_DIC=0x86DE04, VT_FIRE=0x86F570, VT_FALL=0x870520,
                    VT_BOAT=0x86F018, VT_TIMED=0x86FDD4;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<uint8,0x100> object{};
alignas(16) inline std::array<uint8,GROUP_SIZE*3> groupPool{};
alignas(16) inline std::array<uint8,sizeof(CPedGroupIntelligence)> gintel{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<uint8,EV_SIZE> event{};
alignas(16) inline std::array<std::array<uint8,EV_SIZE>,NEVC> evClones{};
alignas(16) inline std::array<std::array<uint8,0x28>,NFIRE> fires{};
alignas(16) inline std::array<uint8,0x40> clump{};
alignas(16) inline std::array<std::array<uint8,0x40>,NASSOC> assocs{};
alignas(16) inline std::array<uint8,0x40> building{};
alignas(16) inline std::array<uint8,0x100> modelInfo{};
alignas(16) inline std::array<uint8,0x400> vehStruct{};
alignas(16) inline std::array<uint8,0x40> outBuf{};
inline size_t groupOff{};
inline uint8* Group(int g) { return groupPool.data()+groupOff+GROUP_SIZE*g; }
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline int PedIndex(const void* p) { for(int n=0;n<NPEDS;++n)if(p==peds[n].data())return n; return -1; }

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

inline std::array<void*,12> fakeVtbl{}, eventVtbl{};
inline std::array<void*,96> vehVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(eventVtbl.data()))return "event_vt"; if(v==uintptr_t(vehVtbl.data()))return "veh_vt";
    if(v==FIRE_MANAGER)return "fire_manager";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n);
                              if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NEVC;++n)if(auto s=in(evClones[n].data(),evClones[n].size(),"evc"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NFIRE;++n)if(auto s=in(fires[n].data(),fires[n].size(),"fire"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NASSOC;++n)if(auto s=in(assocs[n].data(),assocs[n].size(),"assoc"))return *s+"#"+std::to_string(n);
    for(int g=0;g<2;++g)if(auto s=in(Group(g),GROUP_SIZE,"group"))return *s+"#"+std::to_string(g);
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(gintel.data(),gintel.size(),"gintel"))return *s;
    if(auto s=in(event.data(),event.size(),"event"))return *s; if(auto s=in(clump.data(),clump.size(),"clump"))return *s;
    if(auto s=in(building.data(),building.size(),"building"))return *s; if(auto s=in(modelInfo.data(),modelInfo.size(),"mi"))return *s;
    if(auto s=in(vehStruct.data(),vehStruct.size(),"vstruct"))return *s; if(auto s=in(outBuf.data(),outBuf.size(),"out"))return *s;
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
// tarea de la pila: solo los campos que escribe el constructor (secuencia completa, o los dos primeros campos)
inline json SerStack(const CTask* t) {
    if(!t)return nullptr; const int type=const_cast<CTask*>(t)->GetTaskType();
    if(type==TASK_COMPLEX_SEQUENCE)return SerTask(t,0,0x40);
    return {{"type",type},{"words",{Name(At<uint32>(t,0xC)),Name(At<uint32>(t,0x10))}}};
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{}, losMask{}, losCalls{}, vlMask{}, vlCalls{}, fireCalls{}, assocMask{}, evcCount{};
inline std::array<float,4> vlZ{}; inline std::array<uintptr_t,4> fireSeq{};
inline std::array<uintptr_t,NPEDS> groupOf{};
inline uint8 tempRet{}, quitRet{}, stepOut{}; inline int32 appearance{};

// tareas falsas
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(t)}); return At<CTask*>(t,FK_CLONE); }
inline CTask* __fastcall FSub(CTask* t,void*) { return At<CTask*>(t,FK_SUB); }
inline bool __fastcall FSimple(CTask*,void*) { return true; }
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) { trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e)}); return At<uint8>(t,FK_ABORT)!=0; }
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<int... N> void FillTraps(std::array<void*,sizeof...(N)>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// evento falso: tipo en EV_TYPE; Clone copia el evento en el siguiente buffer de clones
inline void* __fastcall EvDtor(void* e,void*,unsigned f) { trace.push_back({"delete_event",Name(e),f}); return e; }
inline int32 __fastcall EvType(void* e,void*) { return At<int32>(e,EV_TYPE); }
inline void* __fastcall EvClone(void* e,void*) { void* c=evcCount<NEVC ? evClones[evcCount++].data() : nullptr; trace.push_back({"clone_event",Name(e),Name(c)}); if(c)std::memcpy(c,e,EV_SIZE); return c; }
// vehiculo: vtable falsa, solo CanPedStepOutCar (hueco 57)
inline bool __fastcall VehStepOut(void* v,void*,uint32 ignore) { trace.push_back({"can_step_out",Name(v),ignore&0xFF,stepOut}); return stepOut!=0; }

// dependencias simuladas
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline CPedGroup* __cdecl FakePedsGroup(const CPed* p) { const int n=PedIndex(p); const uintptr_t g=n>=0 ? groupOf[n] : 0; trace.push_back({"peds_group",Name(p),Name(g)}); return reinterpret_cast<CPedGroup*>(g); }
inline int32 __cdecl FakeGroupId(const CPedGroup* g) { const int32 id=int32(uint32(uintptr_t(g))-uint32(MS_GROUPS))/int32(GROUP_SIZE); trace.push_back({"group_id",Name(g)}); return id; }
inline void __fastcall FakeFlushExe(void* in,void*,void* pairs,CPed* p) { trace.push_back({"flush_tasks",Name(pairs),Name(p)}); }
inline void __fastcall FakeFlushDll(void*,void*,void* pairs,CPed* p) { // DLL ya es __thiscall (ret 8), como el EXE
    trace.push_back({"flush_tasks",Name(pairs),Name(p)}); }
inline void __fastcall FakeEventResponse(void* in,void*,CPed* ped,uint32 hasMain,const CTask* main,uint32 hasSec,const CTask* sec,int32 slot) {
    trace.push_back({"group_event_response",Name(in),Name(ped),uint8(hasMain),SerStack(main),uint8(hasSec),sec ? json(const_cast<CTask*>(sec)->GetTaskType()) : json(nullptr),slot});
}
inline void __fastcall FakeInteresting(void* ie,void*,int32 type,CEntity* e) { trace.push_back({"interesting_event",type,Name(e)}); }
inline void __fastcall FakeSetPedState(CPed* p,void*,int32 s) { trace.push_back({"set_ped_state",Name(p),s}); }
inline void __fastcall FakeClearTasks(void* in,void*,uint32 a,uint32 b) { trace.push_back({"clear_tasks",Name(in),a&0xFF,b&0xFF}); }
inline CEvent* __fastcall FakeEventAdd(void* grp,void*,CEvent* e,uint32 valid) {
    trace.push_back({"event_group_add",Name(grp),Name(e),uint8(valid),e ? Region(e,EV_SIZE,true) : json(nullptr)}); return nullptr;
}
inline bool __cdecl FakeTemporary(const CEvent& e) { trace.push_back({"is_temporary_event",Name(&e),tempRet}); return tempRet!=0; }
inline int32 __fastcall FakeAppearance(const CVehicle* v,void*) { trace.push_back({"vehicle_appearance",Name(v),appearance}); return appearance; }
inline void __cdecl FakeRealPhysics(CVehicle* v) { trace.push_back({"switch_real_physics",Name(v)}); }
inline CFire* __fastcall FakeNearestFire(void* mgr,void*,const CVector* p,uint32 a,uint32 b) {
    const uintptr_t r=fireCalls<fireSeq.size() ? fireSeq[fireCalls] : 0; ++fireCalls;
    trace.push_back({"nearest_fire",Name(mgr),F(p->x),F(p->y),F(p->z),a&0xFF,b&0xFF,Name(r)}); return reinterpret_cast<CFire*>(r);
}
inline void __fastcall FakeMoveForce(void* ph,void*,CVector f) { trace.push_back({"apply_move_force",Name(ph),F(f.x),F(f.y),F(f.z)}); }
inline CAnimBlendAssociation* __cdecl FakeGetAssoc(RpClump* c,uint32 id) {
    const uint32 k=id%7; void* r=(assocMask>>k)&1 ? assocs[k%NASSOC].data() : nullptr;
    trace.push_back({"get_association",Name(c),id,Name(r)}); return static_cast<CAnimBlendAssociation*>(r);
}
inline CAnimBlendAssociation* __cdecl FakeBlend(RpClump* c,int32 grp,int32 id,float delta) { trace.push_back({"blend_animation",Name(c),grp,id,F(delta)}); return nullptr; }
inline bool __cdecl FakeLos(const CVector& a,const CVector& b,uint32 bu,uint32 ve,uint32 pe,uint32 ob,uint32 du,uint32 st,uint32 ca) {
    const bool r=((losMask>>(losCalls++%32))&1)!=0;
    trace.push_back({"los",F(a.x),F(a.y),F(a.z),F(b.x),F(b.y),F(b.z),bu&0xFF,ve&0xFF,pe&0xFF,ob&0xFF,du&0xFF,st&0xFF,ca&0xFF,r}); return r;
}
inline bool __cdecl FakeVertical(const CVector& o,float dist,CColPoint& cp,CEntity*& ent,uint32 bu,uint32 ve,uint32 pe,uint32 ob,uint32 du,uint32 st,void* poly) {
    const uint32 k=vlCalls++; const bool r=((vlMask>>(k%32))&1)!=0;
    trace.push_back({"vertical_line",F(o.x),F(o.y),F(o.z),F(dist),bu&0xFF,ve&0xFF,pe&0xFF,ob&0xFF,du&0xFF,st&0xFF,Name(poly),r});
    if(r){ cp.m_vecPoint.z=vlZ[k%4]; ent=reinterpret_cast<CEntity*>(building.data()); }
    return r;
}
inline bool __cdecl FakeQuit(const CPed* p,const CVehicle* v,int32 t,CTask* task) { trace.push_back({"quit_car_enter",Name(p),Name(v),t,Name(task),quitRet}); return quitRet!=0; }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=42;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskAllocatorKillThreatsBasic","AllocateTasks",0x69D170,0x870E90,1,1024},
    {"CTaskAllocatorKillThreatsBasic","ComputeClosestPeds",0x69C850,0,-1,1024},
    {"CTaskAllocatorKillThreatsBasic","Constructor",0x69C710,0,-1,128},
    {"CTaskAllocatorKillThreatsBasic","Destructor",0x69C780,0,-1,128},
    {"CTaskAllocatorKillThreatsBasic","GetType",0x69C770,0x870E90,5,64},
    {"CTaskAllocatorKillThreatsBasic","ProcessGroup",0x69C7E0,0x870E90,2,512},
    {"CTaskComplexBeStill","Clone",0x5F6680,0x86C7BC,1,64},
    {"CTaskComplexBeStill","ControlSubTask",0x62F9F0,0x86C7BC,10,64},
    {"CTaskComplexBeStill","CreateFirstSubTask",0x62F910,0x86C7BC,9,256},
    {"CTaskComplexBeStill","CreateNextSubTask",0x62F8D0,0x86C7BC,8,256},
    {"CTaskComplexBeStill","Destructor",0x5F6700,0,-1,64},
    {"CTaskComplexBeStill","GetTaskType",0x5F66D0,0x86C7BC,4,64},
    {"CTaskComplexDieInCar","ControlSubTask",0x6377B0,0x86DE04,10,512},
    {"CTaskComplexDieInCar","CreateFirstSubTask",0x6375F0,0x86DE04,9,1024},
    {"CTaskComplexDieInCar","CreateNextSubTask",0x637850,0x86DE04,8,256},
    {"CTaskComplexDieInCar","CreateSubTask",0x62FD50,0,-1,256},
    {"CTaskComplexDieInCar","MakeAbortable",0x62FCC0,0x86DE04,6,128},
    {"CTaskComplexDieInCar","PreparePedVehicleForPedDeath",0x62FD00,0,-1,256},
    {"CTaskComplexDriveFireTruck","Clone",0x659BC0,0x86F570,1,128},
    {"CTaskComplexDriveFireTruck","Constructor",0x659310,0,-1,128},
    {"CTaskComplexDriveFireTruck","ControlSubTask",0x65B1E0,0x86F570,10,512},
    {"CTaskComplexDriveFireTruck","CreateFirstSubTask",0x65B140,0x86F570,9,256},
    {"CTaskComplexDriveFireTruck","CreateNextSubTask",0x65B090,0x86F570,8,256},
    {"CTaskComplexDriveFireTruck","CreateSubTask",0x65A240,0,-1,256},
    {"CTaskComplexFallToDeath","CalcFall",0x6796C0,0,-1,1024},
    {"CTaskComplexFallToDeath","Constructor",0x679040,0,-1,256},
    {"CTaskComplexFallToDeath","ControlSubTask",0x679510,0x870520,10,512},
    {"CTaskComplexFallToDeath","CreateFirstSubTask",0x679120,0x870520,9,256},
    {"CTaskComplexFallToDeath","CreateNextSubTask",0x679270,0x870520,8,1024},
    {"CTaskComplexFallToDeath","MakeAbortable",0x6790C0,0x870520,6,256},
    {"CTaskComplexGoToBoatSteeringWheel","Clone",0x64A350,0x86F018,1,128},
    {"CTaskComplexGoToBoatSteeringWheel","Constructor",0x649090,0,-1,128},
    {"CTaskComplexGoToBoatSteeringWheel","ControlSubTask",0x64E3B0,0x86F018,10,1024},
    {"CTaskComplexGoToBoatSteeringWheel","CreateFirstSubTask",0x64E390,0x86F018,9,256},
    {"CTaskComplexGoToBoatSteeringWheel","CreateNextSubTask",0x64E350,0x86F018,8,128},
    {"CTaskComplexGoToBoatSteeringWheel","Destructor",0x649100,0,-1,128},
    {"CTaskComplexGoToPointAndStandStillTimed","Clone",0x66CF30,0x86FDD4,1,128},
    {"CTaskComplexGoToPointAndStandStillTimed","Constructor",0x6685E0,0,-1,256},
    {"CTaskComplexGoToPointAndStandStillTimed","ControlSubTask",0x66DCE0,0x86FDD4,10,1024},
    {"CTaskComplexGoToPointAndStandStillTimed","CreateFirstSubTask",0x66DC90,0x86FDD4,9,256},
    {"CTaskComplexGoToPointAndStandStillTimed","MakeAbortable",0x668640,0x86FDD4,6,256},
    {"CTaskComplexGoToPointAndStandStillTimed","StopTimer",0x6686A0,0x86FDD4,5,128},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks6a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks6a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks6a");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks6a-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // grupos falsos alineados con CPedGroups::ms_groups: el EXE compara &ms_groups[GetGroupId(g)] con el grupo
    { const uint32 base=uint32(uintptr_t(groupPool.data())); const uint32 rem=(base-uint32(MS_GROUPS))%uint32(GROUP_SIZE); groupOff=rem ? GROUP_SIZE-rem : 0; }
    // hueco libre de CModelInfo::ms_modelInfoPtrs para el model info falso del vehiculo
    int32 miSlot=-1; for(int32 k=19999;k>=15000;--k)if(!CModelInfo::ms_modelInfoPtrs[k]){miSlot=k;break;}
    if(miSlot<0){Write({{"event","error"},{"reason","no free model info slot"}});return;}
    FillTraps(fakeVtbl,std::make_integer_sequence<int,12>{}); FillTraps(eventVtbl,std::make_integer_sequence<int,12>{}); FillTraps(vehVtbl,std::make_integer_sequence<int,96>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    eventVtbl[0]=reinterpret_cast<void*>(&EvDtor); eventVtbl[1]=reinterpret_cast<void*>(&EvType); eventVtbl[4]=reinterpret_cast<void*>(&EvClone);
    vehVtbl[57]=reinterpret_cast<void*>(&VehStepOut);
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
        EntryPatch g1e(0x5F7E80,FakePedsGroup), g2e(0x5F7EE0,FakeGroupId), g3e(0x602590,FakeInteresting), g4e(0x423FC0,FakeRealPhysics);
        EntryPatch g5(bit_cast<uintptr_t>(&CPedGroupIntelligence::FlushTasks),FakeFlushDll), g5e(0x5F79C0,FakeFlushExe);
        EntryPatch g6(bit_cast<uintptr_t>(static_cast<void(CPedGroupIntelligence::*)(CPed*,bool,const CTask&,bool,const CTask&,eSecondaryTask)>(&CPedGroupIntelligence::SetEventResponseTask)),FakeEventResponse), g6e(0x5F8510,FakeEventResponse);
        EntryPatch s1(bit_cast<uintptr_t>(&CPed::SetPedState),FakeSetPedState), s1e(0x5E4500,FakeSetPedState);
        EntryPatch s2(bit_cast<uintptr_t>(&CPedIntelligence::ClearTasks),FakeClearTasks), s2e(0x601420,FakeClearTasks);
        EntryPatch s3(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),FakeEventAdd), s3e(0x4AB420,FakeEventAdd);
        EntryPatch s4(bit_cast<uintptr_t>(&CEventHandler::IsTemporaryEvent),FakeTemporary), s4e(0x4BC370,FakeTemporary);
        EntryPatch s5(bit_cast<uintptr_t>(&CVehicle::GetVehicleAppearance),FakeAppearance), s5e(0x6D1080,FakeAppearance);
        EntryPatch s6(bit_cast<uintptr_t>(&CFireManager::FindNearestFire),FakeNearestFire), s6e(0x538F40,FakeNearestFire);
        EntryPatch s7(bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),FakeMoveForce), s7e(0x5429F0,FakeMoveForce);
        EntryPatch s8(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),FakeGetAssoc), s8e(0x4D68B0,FakeGetAssoc);
        EntryPatch s9(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),FakeBlend), s9e(0x4D4610,FakeBlend);
        EntryPatch w1(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),FakeLos), w1e(0x56A490,FakeLos);
        EntryPatch w2(bit_cast<uintptr_t>(&CWorld::ProcessVerticalLine),FakeVertical), w2e(0x5674E0,FakeVertical);
        EntryPatch q1(bit_cast<uintptr_t>(&CCarEnterExit::IsPlayerToQuitCarEnter),FakeQuit), q1e(0x64F240,FakeQuit);

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
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x6A6Au; if(!st)st=1; trace=json::array(); seed=uint32(i);
                    rngCalls=0; losCalls=0; vlCalls=0; fireCalls=0; evcCount=0; freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    const uint32 now=At<uint32>(reinterpret_cast<void*>(TIMER),0);
                    CModelInfo::ms_modelInfoPtrs[miSlot]=reinterpret_cast<CBaseModelInfo*>(modelInfo.data());
                    // peatones: matriz (siempre para el 0), inteligencia propia, estado, tipo y grupo
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                        p[E_TYPE]=3; At<void*>(p,P_INTEL)=intels[n].data(); At<void*>(p,E_RWOBJ)=clump.data();
                        if(n==0||next()%4){ At<void*>(p,E_MATRIX)=pedMats[n].data();
                            At<CVector>(pedMats[n].data(),0x0)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            At<CVector>(pedMats[n].data(),0x10)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            At<CVector>(pedMats[n].data(),0x20)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            At<CVector>(pedMats[n].data(),0x30)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)}; }
                        else At<CVector>(p,E_POS)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        { static const int32 types[]{4,4,5,7,8,0,1}; At<int32>(p,P_PEDTYPE)=types[next()%7]; }
                        { static const int32 states[]{1,1,1,1,0x36,0x37,7}; At<int32>(p,P_STATE)=states[next()%7]; }
                        groupOf[n]=uintptr_t(Group(n<8 ? 0 : 1));
                    }
                    // empates de distancia ocasionales
                    if(next()%4==0){ const int a=int(next()%NPEDS), b=int(next()%NPEDS); At<CVector>(Ped(b),E_POS)=Ped(a)->GetPosition(); if(At<void*>(Ped(b),E_MATRIX))At<CVector>(pedMats[b].data(),0x30)=Ped(a)->GetPosition(); }
                    // vehiculos: vtable falsa, matriz, autopiloto, modelo falso
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%8)<<3); At<uint16>(v,E_MODEL)=uint16(miSlot);
                        At<void*>(v,E_MATRIX)=vehMats[n].data();
                        for(int r=0;r<3;++r)At<CVector>(vehMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        for(size_t b=0;b<V_AP_SIZE;++b)v[V_AUTOPILOT+b]=uint8(next());
                        { static const int32 sub[]{0,0,3,4,5,9,1}; At<int32>(v,V_SUBTYPE)=sub[next()%7]; }
                    }
                    std::memset(modelInfo.data(),0,modelInfo.size()); At<void*>(modelInfo.data(),0x5C)=vehStruct.data();
                    { static const int32 vt[]{5,5,0,3,4}; At<int32>(modelInfo.data(),0x3C)=vt[next()%5]; }
                    for(size_t k=0;k<vehStruct.size()/4;++k)At<float>(vehStruct.data(),4*k)=uni(-3.f,3.f);
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,FAKE_SIZE); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_PAUSE+n; fakes[n][FK_ABORT]=uint8(next()%2); }
                    std::memset(event.data(),0,event.size()); At<void*>(event.data(),0)=eventVtbl.data(); At<int32>(event.data(),EV_TYPE)=next()%3 ? 9 : int32(next()%40);
                    for(size_t b=4;b<EV_TYPE;++b)event[b]=uint8(next());
                    for(auto& c:evClones)std::memset(c.data(),0,c.size());
                    for(auto& fi:fires){ for(auto& b:fi)b=uint8(next()); At<CVector>(fi.data(),4)=CVector{uni(-80.f,80.f),uni(-80.f,80.f),uni(0.f,20.f)}; }
                    std::memset(clump.data(),0,clump.size()); clump[0]=2;
                    for(auto& a:assocs)for(auto& b:a)b=uint8(next());
                    std::memset(building.data(),0,building.size()); building[E_TYPE]=1;
                    std::memset(outBuf.data(),0x5A,outBuf.size());
                    std::memset(gintel.data(),0,gintel.size()); At<void*>(gintel.data(),0)=Group(0);
                    std::memset(groupPool.data(),0,groupPool.size());
                    for(int g=0;g<2;++g)for(int k=0;k<8;++k)At<CPed*>(Group(g),G_MEMBERS+4*k)=next()%4 ? Ped(8*g+k) : nullptr;
                    std::memset(object.data(),0xCD,object.size());
                    losMask=next(); vlMask=next(); for(auto& z:vlZ){ const uint32 m=next()%6; z=m==0 ? -99.f : m==1 ? -100.f : uni(-20.f,40.f); }
                    for(auto& fs:fireSeq){ const uint32 m=next()%4; fs=m==0 ? 0 : uintptr_t(fires[m%NFIRE].data()); }
                    assocMask=next(); tempRet=uint8(next()%2); quitRet=uint8(next()%3==0); stepOut=uint8(next()%2);
                    { static const int32 ap[]{1,1,2,0,3}; appearance=ap[next()%5]; }

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
                    auto setSub=[&](){ At<CTask*>(o,8)=Fake(0); };
                    auto typeOf=[&](void* p){ return reinterpret_cast<CTask*>(p)->GetTaskType(); };
                    auto callTask=[&](CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    auto callClone=[&]{ return reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); };
                    auto inVeh=[&](CPed* p,bool on){ auto& fl=At<uint32>(p,P_FLAGS); fl=on ? (fl|0x100u) : (fl&~0x100u); };
                    auto pedOut=[&]{ out["ped_flags"]=Region(reinterpret_cast<uint8*>(p0)+P_FLAGS,0x10); };
                    auto makeAbortable=[&]{ const int32 prio=int32(next()%4); const CEvent* ev=next()%2 ? reinterpret_cast<const CEvent*>(event.data()) : nullptr;
                                            return reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,p0,prio,ev); };

                    if(nm=="GetTaskType"||nm=="GetType") {
                        setTask(fn.vt,0x40);
                        ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        out["object"]=Region(o,0x40,true);
                    }
                    else if(cls=="CTaskAllocatorKillThreatsBasic") {
                        // +4 amenaza (referencia), +8 inicio, +0xC intervalo, +0x10 iniciado, +0x11 parado
                        auto pickThreat=[&]()->CPed*{
                            switch(next()%6){
                            case 0: return nullptr;
                            case 1: return Ped(int(next()%8));                                     // del propio grupo
                            case 2: { const int n=8+int(next()%8); groupOf[n]=0; return Ped(n); }  // sin grupo
                            default: return Ped(8+int(next()%8));                                   // del grupo rival
                            }
                        };
                        if(nm=="ComputeClosestPeds"){
                            std::fill_n(reinterpret_cast<CPed**>(outBuf.data()),8,reinterpret_cast<CPed*>(Fake(3)));
                            const int g1=int(next()%2); const int g2=next()%8==0 ? g1 : 1-g1;
                            reinterpret_cast<void(__cdecl*)(void*,void*,CPed**)>(target)(Group(g1),Group(g2),reinterpret_cast<CPed**>(outBuf.data()));
                            out["closest"]=Region(outBuf.data(),0x20);
                        } else if(nm=="Constructor"){
                            CPed* t=next()%4 ? Ped(8+int(next()%8)) : nullptr;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CPed*)>(target)(o,t));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x14,true); out["type"]=int32(reinterpret_cast<CTaskAllocator*>(o)->GetType());
                        } else {
                            for(size_t b=0;b<0x14;++b)o[b]=uint8(next()); At<uintptr_t>(o,0)=VT_KTB;
                            CPed* t=pickThreat(); At<CPed*>(o,4)=t; Reg(t,o+4);
                            At<uint32>(o,8)=now-next()%8000; At<int32>(o,0xC)=next()%8==0 ? -int32(next()%100) : int32(next()%6000);
                            o[0x10]=uint8(next()%3!=0); o[0x11]=uint8(next()%3==0);
                            if(nm=="Destructor") reinterpret_cast<void(__thiscall*)(void*)>(target)(o);
                            else if(nm=="ProcessGroup") ret=Name(reinterpret_cast<void*(__thiscall*)(void*,void*)>(target)(o,gintel.data()));
                            else reinterpret_cast<void(__thiscall*)(void*,void*)>(target)(o,gintel.data());
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x14,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexBeStill") {
                        if(nm=="Destructor"){
                            setTask(VT_STILL,0x10); At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr;
                            reinterpret_cast<void(__thiscall*)(void*)>(target)(o); out["object"]=Region(o,0x10,true);
                        } else {
                            setTask(VT_STILL,0x10); setSub();
                            if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="ControlSubTask") ret=Name(callTask(p0));
                            else {
                                inVeh(p0,next()%2); At<CVehicle*>(p0,P_VEHICLE)=next()%4 ? Veh(0) : nullptr;
                                if(nm=="CreateNextSubTask"){ static const int32 t[]{TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_PAUSE,TASK_COMPLEX_BE_STILL}; At<int32>(fakes[0].data(),FK_TYPE)=t[next()%4]; }
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                            }
                            out["object"]=Region(o,0x10,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexDieInCar") {
                        // +0xC arma, +0x10 tiempo, +0x14 desfase, +0x18 preparado, +0x19 actualizar tiempo
                        setTask(VT_DIC,0x1C); setSub();
                        { static const int32 w[]{51,51,0,22,31,53,49}; At<int32>(o,0xC)=w[next()%7]; }
                        At<uint32>(o,0x10)=now-next()%5000; At<uint32>(o,0x14)=next()%3 ? 2000u : next()%6000; o[0x18]=uint8(next()%3!=0); o[0x19]=uint8(next()%3==0);
                        // vehiculo del peaton 0: conductor, pasajeros y numero maximo de pasajeros
                        auto* v=vehs[0].data(); At<CVehicle*>(p0,P_VEHICLE)=next()%8 ? Veh(0) : nullptr; inVeh(p0,next()%4!=0);
                        { const uint32 m=next()%4; At<CPed*>(v,V_DRIVER)=m==0 ? nullptr : m==1 ? Ped(1) : p0; }
                        for(int k=0;k<8;++k){ const uint32 m=next()%4; At<CPed*>(v,V_PASS+4*k)=m==0 ? nullptr : m==1 ? p0 : Ped(2+k); }
                        v[V_MAXPASS]=uint8(next()%9);
                        if(next()%3==0)At<uint8>(p0,P_FLAGS4)|=0x80; else At<uint8>(p0,P_FLAGS4)&=0x7F;
                        At<uint8>(p0,P_CREATED)=uint8(1+next()%2);
                        if(nm=="CreateSubTask"){
                            static const int32 t[]{TASK_COMPLEX_LEAVE_CAR_AND_DIE,TASK_SIMPLE_DIE_IN_CAR,TASK_SIMPLE_CAR_DRIVE,TASK_FINISHED,TASK_SIMPLE_PAUSE,TASK_SIMPLE_DIE_IN_CAR};
                            const int32 type=t[next()%6]; if(type!=TASK_FINISHED&&type!=TASK_SIMPLE_PAUSE)At<CVehicle*>(p0,P_VEHICLE)=Veh(0);
                            auto* r=reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,type,p0); collectFresh(r); ret=Name(r);
                        } else if(nm=="PreparePedVehicleForPedDeath"){
                            reinterpret_cast<void(__thiscall*)(void*,CVehicle*)>(target)(o,Veh(0));
                        } else if(nm=="MakeAbortable"){
                            ret=makeAbortable();
                        } else {
                            if(nm=="CreateFirstSubTask"){
                                At<CVehicle*>(p0,P_VEHICLE)=Veh(0);
                                auto* hist=intels[0].data()+I_HISTORY; const uint32 m=next()%4;
                                At<void*>(hist,4)=m==1||m==3 ? (void*)event.data() : nullptr; At<void*>(hist,8)=m==2 ? (void*)event.data() : nullptr;
                            } else if(nm=="ControlSubTask"){
                                At<CVehicle*>(p0,P_VEHICLE)=Veh(0);
                                static const int32 t[]{TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_DIE_IN_CAR}; At<int32>(fakes[0].data(),FK_TYPE)=t[next()%4];
                            } else {
                                static const int32 t[]{TASK_SIMPLE_DIE_IN_CAR,TASK_COMPLEX_LEAVE_CAR_AND_DIE,TASK_COMPLEX_LEAVE_CAR_AND_DIE,TASK_SIMPLE_PAUSE}; At<int32>(fakes[0].data(),FK_TYPE)=t[next()%4];
                                if(At<CVehicle*>(p0,P_VEHICLE)==nullptr&&next()%2)At<CVehicle*>(p0,P_VEHICLE)=Veh(0);
                            }
                            auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                        }
                        out["object"]=Region(o,0x1C,true); pedOut();
                        out["autopilot"]=Region(v+V_AUTOPILOT,V_AP_SIZE); out["veh_status"]=v[E_TYPE];
                        json cl=json::array(); for(auto& c:evClones)cl.push_back(Region(c.data(),EV_SIZE)); out["event_clones"]=cl;
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexDriveFireTruck") {
                        // +0xC vehiculo, +0x10 conductor (byte), +0x14 companero, +0x18 incendio
                        if(nm=="Constructor"){
                            CVehicle* v=next()%4 ? Veh(0) : nullptr; CPed* part=next()%3 ? Ped(1) : nullptr; const uint32 drv=next()%2;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,CPed*,uint32)>(target)(o,v,part,drv));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x20,true); out["type"]=typeOf(o);
                        } else {
                            setTask(VT_FIRE,0x1C); setSub(); At<CVehicle*>(o,0xC)=Veh(0); o[0x10]=uint8(next()%3!=0); At<CPed*>(o,0x14)=next()%2 ? Ped(1) : nullptr;
                            At<void*>(o,0x18)=next()%4 ? (void*)fires[0].data() : nullptr;
                            inVeh(p0,next()%4!=0); At<CVehicle*>(p0,P_VEHICLE)=next()%5 ? Veh(next()%2) : nullptr;
                            fires[0][0]=uint8(next()); fires[1][0]=uint8(next());
                            if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_SIMPLE_CAR_DRIVE,TASK_COMPLEX_USE_WATER_CANNON,TASK_COMPLEX_CAR_DRIVE_TO_POINT,TASK_COMPLEX_CAR_DRIVE_WANDER,TASK_FINISHED,TASK_SIMPLE_PAUSE};
                                const int32 type=t[next()%6]; if(type==TASK_COMPLEX_CAR_DRIVE_TO_POINT)At<void*>(o,0x18)=fires[1].data();
                                auto* r=reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,type,p0); collectFresh(r); ret=Name(r);
                            } else {
                                if(nm=="ControlSubTask"){ static const int32 t[]{TASK_COMPLEX_CAR_DRIVE_TO_POINT,TASK_COMPLEX_CAR_DRIVE_TO_POINT,TASK_COMPLEX_CAR_DRIVE_WANDER,TASK_SIMPLE_CAR_DRIVE};
                                    At<int32>(fakes[0].data(),FK_TYPE)=t[next()%4]; if(!At<void*>(o,0x18))At<void*>(o,0x18)=fires[0].data(); }
                                else if(nm=="CreateNextSubTask"){ static const int32 t[]{TASK_COMPLEX_CAR_DRIVE_WANDER,TASK_COMPLEX_CAR_DRIVE_TO_POINT,TASK_COMPLEX_USE_WATER_CANNON,TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_PAUSE};
                                    At<int32>(fakes[0].data(),FK_TYPE)=t[next()%5]; if(!At<void*>(o,0x18))At<void*>(o,0x18)=fires[0].data(); }
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                            }
                            out["object"]=Region(o,0x1C,true); out["sub"]=Region(fakes[0].data(),0x30,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexFallToDeath") {
                        // +0xC direccion de caida (vector), +0x18 y +0x1C animaciones, +0x20 direccion (byte), +0x21 flags
                        auto zVal=[&]{ static const float zs[]{0.f,-0.f,0.005f,-0.005f,0.01f,-0.01f,0.05f,-0.05f,0.1f,-0.1f,0.2f,-2.f};
                            const uint32 m=next()%16; if(m<12)return zs[m]; if(m==12)return qnan;
                            const float b=next()%2 ? 0.1f : 0.01f; const float c=std::bit_cast<float>(std::bit_cast<uint32>(b)+(next()%2 ? 1u : uint32(-1))); return next()%2 ? c : -c; };
                        auto setFall=[&]{ setTask(VT_FALL,0x24); setSub(); At<CVector>(o,0xC)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            { static const int32 an[]{-1,-1,24,25,26,27,0,3}; At<int32>(o,0x18)=an[next()%8]; At<int32>(o,0x1C)=an[next()%8]; }
                            o[0x20]=uint8(next()%4); o[0x21]=uint8(next()); };
                        if(nm=="Constructor"){
                            const int32 dir=next()%4 ? int32(next()%4) : int32(next()%0x300); const CVector pos{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            const uint32 a=next()%2, b=next()%2;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,const CVector*,uint32,uint32)>(target)(o,dir,&pos,a,b));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x28,true); out["type"]=typeOf(o);
                        } else if(nm=="CalcFall"){
                            inVeh(p0,next()%8==0); At<uint32>(p0,PH_FLAGS)=next()%8==0 ? 0x100u : 0u;
                            At<uint32>(outBuf.data(),0)=0xA5A5A5A5u; At<uint32>(outBuf.data(),4)=0xA5A5A5A5u;
                            ret=reinterpret_cast<bool(__cdecl*)(CPed*,int32*,uint32*)>(target)(p0,&At<int32>(outBuf.data(),0),&At<uint32>(outBuf.data(),4));
                            out["outs"]=Region(outBuf.data(),8);
                        } else {
                            setFall();
                            if(nm=="MakeAbortable") ret=makeAbortable();
                            else {
                                At<float>(p0,PH_MOVE+8)=zVal();
                                if(nm=="CreateNextSubTask"){
                                    static const int32 t[]{TASK_SIMPLE_IN_AIR,TASK_SIMPLE_LAND,TASK_SIMPLE_LAND,TASK_SIMPLE_PAUSE}; At<int32>(fakes[0].data(),FK_TYPE)=t[next()%4];
                                    { const uint32 m=next()%4; At<void*>(p0,P_CONTACT)=m==0 ? nullptr : m==1 ? (void*)Veh(1) : (void*)building.data(); }
                                } else if(nm=="ControlSubTask"){
                                    At<uint32>(p0,PH_FLAGS)=next()%4==0 ? 0x100u : 0u; At<uint32>(p0,P_FLAGS2)=next();
                                    static const int32 t[]{TASK_SIMPLE_DEAD,TASK_SIMPLE_LAND,TASK_SIMPLE_IN_AIR}; At<int32>(fakes[0].data(),FK_TYPE)=t[next()%3];
                                } else { At<uint32>(p0,P_FLAGS)=next(); }
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                            }
                            out["object"]=Region(o,0x24,true); pedOut(); out["flags2"]=Region(reinterpret_cast<uint8*>(p0)+P_FLAGS2,4);
                            json an=json::array(); for(auto& a:assocs)an.push_back(Region(a.data(),a.size())); out["assocs"]=an;
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexGoToBoatSteeringWheel") {
                        // +0xC punto objetivo, +0x18 vehiculo, +0x1C punto alcanzado, +0x20 inicio de la primera subtarea
                        if(nm=="Constructor"){
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*)>(target)(o,next()%4 ? Veh(0) : nullptr));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x28,true); out["type"]=typeOf(o);
                        } else {
                            setTask(VT_BOAT,0x24); At<CVehicle*>(o,0x18)=Veh(0); o[0x1C]=uint8(next()%2);
                            if(nm=="Destructor"){
                                At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; if(next()%4==0)At<CVehicle*>(o,0x18)=nullptr; Reg(At<CVehicle*>(o,0x18),o+0x18);
                                reinterpret_cast<void(__thiscall*)(void*)>(target)(o);
                            } else {
                                setSub();
                                if(nm=="Clone"){ if(next()%4==0)At<CVehicle*>(o,0x18)=nullptr; auto* r=callClone(); collectFresh(r); ret=Name(r); }
                                else {
                                    if(nm=="CreateNextSubTask"){ static const int32 t[]{TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_PAUSE}; At<int32>(fakes[0].data(),FK_TYPE)=t[next()%3]; }
                                    else if(nm=="ControlSubTask"){
                                        // subtarea: CTaskComplexGoToPointAndStandStill falsa (+0x10 punto, +0x1C radio, +0x20 radio de paso, +0x24 flags)
                                        auto* s=fakes[0].data(); At<int32>(s,FK_TYPE)=TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL;
                                        const CVector base=Veh(0)->GetPosition();
                                        At<CVector>(s,0x10)=CVector{base.x+uni(-4.f,4.f),base.y+uni(-4.f,4.f),base.z+uni(-4.f,4.f)};
                                        At<float>(s,0x1C)=uni(0.f,2.f); At<float>(s,0x20)=next()%2 ? 2.f : uni(0.f,4.f); s[0x24]=uint8(next());
                                        { const uint32 m=next()%4; At<CTask*>(s,FK_SUB)=m==0 ? nullptr : Fake(1); At<int32>(fakes[1].data(),FK_TYPE)=m==1 ? TASK_SIMPLE_STAND_STILL : TASK_SIMPLE_GO_TO_POINT; }
                                        if(next()%8==0&&!At<CTask*>(s,FK_SUB))At<CVehicle*>(o,0x18)=nullptr;
                                        At<uint32>(o,0x20)=now-next()%5000;
                                    }
                                    auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                }
                                out["sub"]=Region(fakes[0].data(),0x30,true);
                            }
                            out["object"]=Region(o,0x24,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexGoToPointAndStandStillTimed") {
                        // base: +0xC estado de movimiento, +0x10 punto, +0x1C radio, +0x20 radio de paso, +0x24 flags;
                        // +0x28 tiempo, temporizador: +0x2C inicio, +0x30 intervalo, +0x34 iniciado, +0x35 parado
                        static const int32 ms[]{4,6,7,4,6,1};
                        if(nm=="Constructor"){
                            const int32 m=ms[next()%6]; const CVector pos{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,20.f)};
                            const float rad=next()%3 ? uni(0.f,3.f) : 1.5f, msr=next()%3 ? 2.f : uni(0.f,5.f); const int32 t=next()%4 ? int32(next()%10000) : -int32(next()%100);
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,const CVector*,float,float,int32)>(target)(o,m,&pos,rad,msr,t));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x40,true); out["type"]=typeOf(o);
                        } else {
                            setTask(VT_TIMED,0x38); setSub(); At<int32>(o,0xC)=ms[next()%6];
                            const CVector pp=p0->GetPosition(); At<CVector>(o,0x10)=CVector{pp.x+uni(-30.f,30.f),pp.y+uni(-30.f,30.f),pp.z+uni(-3.f,3.f)};
                            At<float>(o,0x1C)=uni(0.f,3.f); At<float>(o,0x20)=uni(0.f,12.f);
                            At<int32>(o,0x28)=next()%4 ? int32(next()%10000) : -int32(next()%100);
                            At<uint32>(o,0x2C)=now-next()%8000; At<int32>(o,0x30)=next()%8==0 ? -int32(next()%100) : int32(next()%6000);
                            o[0x34]=uint8(next()%4!=0); o[0x35]=uint8(next()%3==0);
                            if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="StopTimer"){ reinterpret_cast<void(__thiscall*)(void*,const void*)>(target)(o,event.data()); }
                            else if(nm=="MakeAbortable"){ ret=makeAbortable(); }
                            else {
                                inVeh(p0,next()%4==0); At<CVehicle*>(p0,P_VEHICLE)=next()%4 ? Veh(0) : nullptr;
                                if(nm=="ControlSubTask"){
                                    static const int32 t[]{TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_LEAVE_CAR};
                                    auto* s=fakes[0].data(); At<int32>(s,FK_TYPE)=t[next()%4]; At<CVector>(s,0xC)=CVector{pp.x+uni(-30.f,30.f),pp.y+uni(-30.f,30.f),pp.z};
                                    if(next()%4==0)o[0x24]&=0xFB;
                                }
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                            }
                            out["object"]=Region(o,0x38,true); out["sub"]=Region(fakes[0].data(),0x30,true);
                            out["ped_pos"]=Region(At<void*>(p0,E_MATRIX) ? pedMats[0].data()+0x30 : peds[0].data()+E_POS,12);
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
                    out["ret"]=ret; out["trace"]=trace; out["delete_trace"]=delTrace; out["rng"]=rngCalls;
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
