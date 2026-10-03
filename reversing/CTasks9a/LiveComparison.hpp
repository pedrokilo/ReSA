#pragma once
// Banco dentro del motor: tareas complejas A (lista de 9, 45 funciones).
// CTaskComplexCopInCar y CTaskComplexEvasiveDiveAndGetUp (constructor, destructor, Clone, GetTaskType, MakeAbortable,
// ControlSubTask, CreateFirstSubTask, CreateNextSubTask, CreateSubTask), CTaskComplexEnterCarAsDriverTimed y
// CTaskComplexEnterCarAsPassengerTimed (constructor, destructor, Clone, GetTaskType, MakeAbortable, StopTimer, ControlSubTask,
// CreateFirstSubTask, CreateNextSubTask) y CTaskComplexFacial (constructor, Clone, GetTaskType, MakeAbortable,
// ControlSubTask, CreateFirstSubTask, CreateNextSubTask, SetRequest, StopAll).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 4 peatones (inteligencia y clump propios), 2 vehiculos (vtable falsa con trampas, handling propio), evento falso
// (vtable con GetEventType), CWanted falso, asociacion de animacion falsa y tareas falsas (vtable trazada, MakeAbortable
// con una secuencia de respuestas).
// Se simulan y trazan: RNG, RpAnimBlendClumpGetAssociation, CEventHandler::IsTemporaryEvent, CVehicle::IsDriver,
// IsUpsideDown, IsOnItsSide, ChangeLawEnforcerState, CCarAI::FindPoliceCarMissionForWantedLevel/SpeedForWantedLevel,
// FindPlayerWanted, CWanted::CanCopJoinPursuit, CPedIntelligence::SetPedDecisionMakerType y
// CCarEnterExit::ComputePassengerIndexFromCarDoor.
// Pool de tareas: m_LastFreeSlot y estados se restauran y los slots libres se rellenan con 0xCD en cada ejecucion.
// CTimer::m_snTimeInMilliseconds y CTimer::ms_fTimeStep se guardan y se restauran.
// Se compara la memoria de los fixtures (punteros traducidos a nombres), las tareas nuevas del pool, las referencias,
// el retorno y la traza.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/Task.h"
#include "TaskComplexCopInCar.h"
#include "TaskComplexEnterCarAsDriverTimed.h"
#include "TaskComplexEnterCarAsPassengerTimed.h"
#include "TaskComplexEvasiveDiveAndGetUp.h"
#include "TaskComplexFacial.h"
#include "TaskSimpleFacial.h"
#include "Events/EventHandler.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "CarEnterExit.h"
#include "CarAI.h"
#include "Wanted.h"
#include "PedIntelligence.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks9aLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }

constexpr int NPEDS=4, NVEH=2, FAKES=4;
// tareas falsas: campos de control por encima de 0x50 (las tareas reales escriben como mucho hasta 0x18 en ellas)
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_SUB=0x64, FK_TYPE=0x68, FK_CLONE=0x6C, FK_CNT=0x70;
// desplazamientos crudos del EXE
constexpr size_t E_MATRIX=0x14, E_RWOBJ=0x18, E_TYPE=0x36, E_MOVESPEED=0x44;                         // CEntity/CPhysical
constexpr size_t P_FLAGS=0x46C, P_FLAGS3=0x474, P_INTEL=0x47C, P_STATE=0x530, P_ROT=0x558, P_AIM=0x55C,
                 P_VEHICLE=0x58C, P_PEDTYPE=0x598;                                                    // CPed
constexpr size_t V_HANDLING=0x384, V_MISSION=0x3BA, V_CRUISE=0x3D0, V_DRIVER=0x460, V_PASS=0x464,
                 V_NUMPASS=0x484, V_MAXPASS=0x488, V_FIRE=0x490, V_HEALTH=0x4C0;                     // CVehicle
constexpr size_t EV_TYPE=0x4C;                                                                        // evento falso
constexpr uintptr_t TIMER=0xB7CB84, TIMESTEP=0xB7CB5C;
constexpr uintptr_t VT_COP=0x870A2C, VT_EDT=0x86E720, VT_EPT=0x86E74C, VT_EVD=0x86F268, VT_FAC=0x870ADC;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,0x40>,NPEDS> clumps{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<std::array<uint8,0x100>,NVEH> handlings{};
alignas(16) inline std::array<uint8,0x100> object{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<uint8,0x50> eventBuf{};
alignas(16) inline std::array<uint8,0x40> wanted{};
alignas(16) inline std::array<uint8,0x40> assoc{};
alignas(16) inline std::array<uint8,0x20> fireDummy{};
alignas(16) inline std::array<uint8,0x10> dirArg{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline CEvent* Ev() { return reinterpret_cast<CEvent*>(eventBuf.data()); }

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
inline std::array<void*,16> eventVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(vehVtbl.data()))return "veh_vt"; if(v==uintptr_t(eventVtbl.data()))return "event_vt";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n);
                              if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n); if(auto s=in(clumps[n].data(),clumps[n].size(),"clump"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n);
                             if(auto s=in(handlings[n].data(),handlings[n].size(),"handling"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(eventBuf.data(),eventBuf.size(),"event"))return *s;
    if(auto s=in(wanted.data(),wanted.size(),"wanted"))return *s; if(auto s=in(assoc.data(),assoc.size(),"assoc"))return *s;
    if(auto s=in(fireDummy.data(),fireDummy.size(),"fire"))return *s; if(auto s=in(dirArg.data(),dirArg.size(),"dir_arg"))return *s;
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
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
// campos de vehiculo tocados: estado (+0x36), mision y velocidad de crucero del autopiloto
inline json VehOut(int n) { auto* v=vehs[n].data(); return json::array({v[E_TYPE],v[V_MISSION],v[V_CRUISE]}); }

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{};
inline uint8 tempRet{}, isDriverRet{}, upsideRet{}, sideRet{}, canJoinRet{}, assocMask{};
inline int8 missionRet{}; inline int32 speedRet{}, passIdxRet{};

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
// evento falso: slot 1 = GetEventType
inline int32 __fastcall FEventType(CEvent* e,void*) { trace.push_back({"event_type",Name(e)}); return At<int32>(e,EV_TYPE); }

// dependencias simuladas
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline CAnimBlendAssociation* __cdecl FakeGetAssoc(RpClump* c,uint32 id) {
    const bool hit=id==uint32(ANIM_ID_EV_DIVE) ? (assocMask&1)!=0 : id==uint32(ANIM_ID_IDLE_CHAT) ? (assocMask&2)!=0 : id==uint32(ANIM_ID_FACTALK) ? (assocMask&4)!=0 : false;
    trace.push_back({"clump_get_association",Name(c),id,hit}); return hit ? reinterpret_cast<CAnimBlendAssociation*>(assoc.data()) : nullptr;
}
inline bool __cdecl FakeTemporary(const CEvent& e) { trace.push_back({"is_temporary_event",Name(&e),tempRet}); return tempRet!=0; }
inline bool __fastcall FakeIsDriver(const CVehicle* v,void*,const CPed* p) { trace.push_back({"is_driver",Name(v),Name(p),isDriverRet}); return isDriverRet!=0; }
inline bool __fastcall FakeUpsideDown(const CVehicle* v,void*) { trace.push_back({"is_upside_down",Name(v),upsideRet}); return upsideRet!=0; }
inline bool __fastcall FakeOnItsSide(const CVehicle* v,void*) { trace.push_back({"is_on_its_side",Name(v),sideRet}); return sideRet!=0; }
inline void __fastcall FakeLawState(CVehicle* v,void*,uint32 b) { trace.push_back({"change_law_enforcer_state",Name(v),b&0xFF}); }
inline int8 __cdecl FakeMission() { trace.push_back({"police_car_mission",missionRet}); return missionRet; }
inline int32 __cdecl FakeSpeed(CVehicle* v) { trace.push_back({"police_car_speed",Name(v),speedRet}); return speedRet; }
inline CWanted* __cdecl FakeFindWanted(int32 id) { trace.push_back({"find_player_wanted",id}); return reinterpret_cast<CWanted*>(wanted.data()); }
inline bool __fastcall FakeCanJoin(CWanted* w,void*,CCopPed* c) { trace.push_back({"can_cop_join_pursuit",Name(w),Name(c),canJoinRet}); return canJoinRet!=0; }
inline void __fastcall FakeSetDM(CPedIntelligence* i,void*,int32 t) { trace.push_back({"set_decision_maker",Name(i),t}); }
inline int32 __cdecl FakePassIdx(const CVehicle* v,int32 door) { trace.push_back({"passenger_index",Name(v),door,passIdxRet}); return passIdxRet; }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=45;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexCopInCar","Clone",0x68CEC0,0x870A2C,1,256},
    {"CTaskComplexCopInCar","Constructor",0x68C7F0,0,-1,256},
    {"CTaskComplexCopInCar","ControlSubTask",0x68FD50,0x870A2C,10,1024},
    {"CTaskComplexCopInCar","CreateFirstSubTask",0x68FA10,0x870A2C,9,256},
    {"CTaskComplexCopInCar","CreateNextSubTask",0x68FA50,0x870A2C,8,1024},
    {"CTaskComplexCopInCar","CreateSubTask",0x68C9E0,0,-1,512},
    {"CTaskComplexCopInCar","Destructor",0x68C8C0,0,-1,128},
    {"CTaskComplexCopInCar","GetTaskType",0x68C8B0,0x870A2C,4,64},
    {"CTaskComplexCopInCar","MakeAbortable",0x68C940,0x870A2C,6,512},
    {"CTaskComplexEnterCarAsDriverTimed","Clone",0x63D780,0x86E720,1,128},
    {"CTaskComplexEnterCarAsDriverTimed","Constructor",0x63AD70,0,-1,128},
    {"CTaskComplexEnterCarAsDriverTimed","ControlSubTask",0x63AEE0,0x86E720,10,1024},
    {"CTaskComplexEnterCarAsDriverTimed","CreateFirstSubTask",0x6405B0,0x86E720,9,256},
    {"CTaskComplexEnterCarAsDriverTimed","CreateNextSubTask",0x63AED0,0x86E720,8,64},
    {"CTaskComplexEnterCarAsDriverTimed","Destructor",0x63AE00,0,-1,128},
    {"CTaskComplexEnterCarAsDriverTimed","GetTaskType",0x63ADF0,0x86E720,4,64},
    {"CTaskComplexEnterCarAsDriverTimed","MakeAbortable",0x63AE60,0x86E720,6,512},
    {"CTaskComplexEnterCarAsDriverTimed","StopTimer",0x63AFF0,0x86E720,5,256},
    {"CTaskComplexEnterCarAsPassengerTimed","Clone",0x63D7E0,0x86E74C,1,128},
    {"CTaskComplexEnterCarAsPassengerTimed","Constructor",0x63B030,0,-1,128},
    {"CTaskComplexEnterCarAsPassengerTimed","ControlSubTask",0x63B1A0,0x86E74C,10,1024},
    {"CTaskComplexEnterCarAsPassengerTimed","CreateFirstSubTask",0x640660,0x86E74C,9,256},
    {"CTaskComplexEnterCarAsPassengerTimed","CreateNextSubTask",0x63B190,0x86E74C,8,64},
    {"CTaskComplexEnterCarAsPassengerTimed","Destructor",0x63B0C0,0,-1,128},
    {"CTaskComplexEnterCarAsPassengerTimed","GetTaskType",0x63B0B0,0x86E74C,4,64},
    {"CTaskComplexEnterCarAsPassengerTimed","MakeAbortable",0x63B120,0x86E74C,6,512},
    {"CTaskComplexEnterCarAsPassengerTimed","StopTimer",0x63B2E0,0x86E74C,5,256},
    {"CTaskComplexEvasiveDiveAndGetUp","Clone",0x655940,0x86F268,1,128},
    {"CTaskComplexEvasiveDiveAndGetUp","Constructor",0x6536B0,0,-1,256},
    {"CTaskComplexEvasiveDiveAndGetUp","ControlSubTask",0x653810,0x86F268,10,64},
    {"CTaskComplexEvasiveDiveAndGetUp","CreateFirstSubTask",0x6560A0,0x86F268,9,256},
    {"CTaskComplexEvasiveDiveAndGetUp","CreateNextSubTask",0x656000,0x86F268,8,1024},
    {"CTaskComplexEvasiveDiveAndGetUp","CreateSubTask",0x653820,0,-1,512},
    {"CTaskComplexEvasiveDiveAndGetUp","Destructor",0x653740,0,-1,128},
    {"CTaskComplexEvasiveDiveAndGetUp","GetTaskType",0x653730,0x86F268,4,64},
    {"CTaskComplexEvasiveDiveAndGetUp","MakeAbortable",0x6537A0,0x86F268,6,256},
    {"CTaskComplexFacial","Clone",0x6928B0,0x870ADC,1,128},
    {"CTaskComplexFacial","Constructor",0x690D20,0,-1,256},
    {"CTaskComplexFacial","ControlSubTask",0x690FC0,0x870ADC,10,1024},
    {"CTaskComplexFacial","CreateFirstSubTask",0x690F30,0x870ADC,9,256},
    {"CTaskComplexFacial","CreateNextSubTask",0x690DC0,0x870ADC,8,512},
    {"CTaskComplexFacial","GetTaskType",0x690D80,0x870ADC,4,64},
    {"CTaskComplexFacial","MakeAbortable",0x690DA0,0x870ADC,6,256},
    {"CTaskComplexFacial","SetRequest",0x691230,0,-1,128},
    {"CTaskComplexFacial","StopAll",0x691250,0,-1,128},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks9a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks9a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks9a");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks9a-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(fakeVtbl,std::make_integer_sequence<int,16>{}); FillTraps(vehVtbl,std::make_integer_sequence<int,96>{}); FillTraps(eventVtbl,std::make_integer_sequence<int,16>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    eventVtbl[1]=reinterpret_cast<void*>(&FEventType);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{TIMESTEP,4}};
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
        EntryPatch a1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),FakeGetAssoc), a1e(0x4D68B0,FakeGetAssoc);
        EntryPatch t1(bit_cast<uintptr_t>(&CEventHandler::IsTemporaryEvent),FakeTemporary), t1e(0x4BC370,FakeTemporary);
        EntryPatch v1(bit_cast<uintptr_t>(static_cast<bool(CVehicle::*)(const CPed*) const>(&CVehicle::IsDriver)),FakeIsDriver), v1e(0x6D1C40,FakeIsDriver);
        EntryPatch v2(bit_cast<uintptr_t>(&CVehicle::IsUpsideDown),FakeUpsideDown), v2e(0x6D1D90,FakeUpsideDown);
        EntryPatch v3(bit_cast<uintptr_t>(&CVehicle::IsOnItsSide),FakeOnItsSide), v3e(0x6D1DD0,FakeOnItsSide);
        EntryPatch v4(bit_cast<uintptr_t>(&CVehicle::ChangeLawEnforcerState),FakeLawState), v4e(0x6D2330,FakeLawState);
        EntryPatch c1(bit_cast<uintptr_t>(&CCarAI::FindPoliceCarMissionForWantedLevel),FakeMission), c1e(0x41C9D0,FakeMission);
        EntryPatch c2(bit_cast<uintptr_t>(&CCarAI::FindPoliceCarSpeedForWantedLevel),FakeSpeed), c2e(0x41CAA0,FakeSpeed);
        EntryPatch w1(bit_cast<uintptr_t>(&FindPlayerWanted),FakeFindWanted), w1e(0x56E230,FakeFindWanted);
        EntryPatch w2(bit_cast<uintptr_t>(static_cast<bool(CWanted::*)(CCopPed*)>(&CWanted::CanCopJoinPursuit)),FakeCanJoin), w2e(0x562FB0,FakeCanJoin);
        EntryPatch d1(bit_cast<uintptr_t>(&CPedIntelligence::SetPedDecisionMakerType),FakeSetDM), d1e(0x600B50,FakeSetDM);
        EntryPatch e1(bit_cast<uintptr_t>(&CCarEnterExit::ComputePassengerIndexFromCarDoor),FakePassIdx), e1e(0x64F1E0,FakePassIdx);

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
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5A5Au; if(!st)st=1; trace=json::array(); seed=uint32(i);
                    rngCalls=0; freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    const uint32 now=At<uint32>(reinterpret_cast<void*>(TIMER),0);
                    At<float>(reinterpret_cast<void*>(TIMESTEP),0)=next()%2 ? 1.f : uni(0.2f,3.f);
                    // peatones: matriz, inteligencia, clump (tipo 2), tipo, estado, flags, vehiculo y rotacion
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                        std::memset(clumps[n].data(),0,clumps[n].size()); clumps[n][0]=2;
                        p[E_TYPE]=uint8(3|(next()%8)<<3); At<void*>(p,P_INTEL)=intels[n].data(); At<void*>(p,E_MATRIX)=pedMats[n].data(); At<void*>(p,E_RWOBJ)=clumps[n].data();
                        for(int r=0;r<3;++r)At<CVector>(pedMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(pedMats[n].data(),0x30)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                        { static const int32 types[]{0,1,6,6,4,5}; At<int32>(p,P_PEDTYPE)=types[next()%6]; }
                        { static const int32 states[]{1,1,1,1,0x36,0x37,0x32}; At<int32>(p,P_STATE)=states[next()%7]; }
                        if(next()%2)At<uint32>(p,P_FLAGS)|=0x100u; if(next()%4==0)At<uint32>(p,P_FLAGS3)|=0x40u;
                        At<CVehicle*>(p,P_VEHICLE)=next()%3 ? Veh(int(next()%NVEH)) : nullptr;
                        { const uint32 m=next()%6; At<float>(p,P_ROT)=m==0 ? 3.14159274f : m==1 ? -3.14159274f : m==2 ? 0.f : uni(-10.f,10.f); At<float>(p,P_AIM)=uni(-4.f,4.f); }
                    }
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size()); std::memset(handlings[n].data(),0,handlings[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%8)<<3); At<void*>(v,E_MATRIX)=vehMats[n].data(); At<void*>(v,V_HANDLING)=handlings[n].data();
                        At<float>(handlings[n].data(),0x88)=next()%4 ? uni(0.1f,1.2f) : 0.8333333f;
                        for(int r=0;r<3;++r)At<CVector>(vehMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                        v[V_MISSION]=uint8(next()%3 ? next()%60 : 0); v[V_CRUISE]=uint8(next());
                        At<CPed*>(v,V_DRIVER)=next()%3 ? Ped(int(next()%NPEDS)) : nullptr;
                        for(int s=0;s<8;++s)At<CPed*>(v,V_PASS+4*s)=next()%3==0 ? Ped(int(next()%NPEDS)) : nullptr;
                        v[V_NUMPASS]=uint8(next()%9); v[V_MAXPASS]=uint8(next()%9);
                        { const uint32 m=next()%6; At<float>(v,V_HEALTH)=m==0 ? 0.f : m==1 ? -5.f : m==2 ? qnan : uni(1.f,1000.f); }
                        At<void*>(v,V_FIRE)=next()%5==0 ? fireDummy.data() : nullptr;
                        { const uint32 m=next()%5; At<CVector>(v,E_MOVESPEED)=m==0 ? CVector{} : m==1 ? CVector{qnan,0.f,0.f} : CVector{uni(-0.3f,0.3f),uni(-0.3f,0.3f),uni(-0.05f,0.05f)}; }
                    }
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,FAKE_SIZE); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_PAUSE+n; At<uint32>(fakes[n].data(),FK_ABORT)=next(); }
                    std::memset(eventBuf.data(),0,eventBuf.size()); At<void*>(eventBuf.data(),0)=eventVtbl.data();
                    { static const int32 et[]{EVENT_DAMAGE,EVENT_DAMAGE,EVENT_DAMAGE,EVENT_KNOCK_OFF_BIKE}; At<int32>(eventBuf.data(),EV_TYPE)=et[next()%4]; }
                    eventBuf[0xC]=uint8(next()%3!=0); eventBuf[0x40]=uint8(next()%3!=0);
                    std::memset(wanted.data(),0,wanted.size()); At<uint32>(wanted.data(),0x2C)=next()%4==0 ? 0 : 1+next()%6;
                    for(auto& b:assoc)b=uint8(next()); std::memset(fireDummy.data(),0,fireDummy.size());
                    std::memset(object.data(),0xCD,object.size());
                    tempRet=uint8(next()%3==0); isDriverRet=uint8(next()%3!=0); upsideRet=uint8(next()%5==0); sideRet=uint8(next()%5==0); canJoinRet=uint8(next()%3!=0);
                    assocMask=uint8(next()); missionRet=int8(next()%60); speedRet=int32(next()%60); passIdxRet=int32(next()%8);

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
                    auto callAbort=[&](CPed* ped,int32 prio,const CEvent* e){ return reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,ped,prio,e); };
                    auto callStop=[&](const CEvent* e){ reinterpret_cast<void(__thiscall*)(void*,const CEvent*)>(target)(o,e); };
                    auto ctorDone=[&](size_t size){ for(auto* h:hooks)h->State(false); out["object"]=Region(o,size,true); out["type"]=typeOf(o); };
                    auto dtorSub=[&]{ At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; };
                    auto prefill=[&](size_t size){ for(size_t b=0;b<size;++b)o[b]=uint8(next()); };
                    auto pickPrio=[&]{ return int32(next()%3); };
                    auto pickEvent=[&]()->CEvent*{ return next()%3 ? Ev() : nullptr; };
                    // temporizador de tarea en o+off: inicio, intervalo, iniciado, parado (exactamente cumplido a veces)
                    auto setTimer=[&](size_t off){
                        At<uint32>(o,off)=now-next()%6000; At<int32>(o,off+4)=next()%8==0 ? -int32(next()%100) : int32(next()%5000);
                        if(next()%5==0)At<int32>(o,off+4)=int32(now-At<uint32>(o,off));
                        o[off+8]=uint8(next()%4!=0); o[off+9]=uint8(next()%4==0);
                    };
                    auto finish=[&](size_t size){ for(auto* h:hooks)h->State(false); out["object"]=Region(o,size,true); };

                    if(nm=="GetTaskType") {
                        setTask(fn.vt,0x40);
                        ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        out["object"]=Region(o,0x40,true);
                    }
                    else if(cls=="CTaskComplexCopInCar") {
                        // +0xC vehiculo, +0x10 companero, +0x14 sospechoso (referencias), temporizadores +0x18 y +0x24, flags +0x30
                        if(nm=="Constructor"){
                            prefill(0x34);
                            CVehicle* v=next()%4 ? Veh(0) : nullptr; CPed* c=next()%3 ? Ped(1) : nullptr; CPed* s=next()%4 ? Ped(2) : nullptr;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,CPed*,CPed*,uint32)>(target)(o,v,c,s,uint32(next()%2)|(next()&0xFF00)));
                            ctorDone(0x34);
                        } else {
                            setTask(VT_COP,0x34);
                            CVehicle* v=next()%8 ? Veh(0) : nullptr; CPed* c=next()%5 ? Ped(1) : nullptr; CPed* s=next()%8 ? Ped(2) : nullptr;
                            At<CVehicle*>(o,0xC)=v; Reg(v,o+0xC); At<CPed*>(o,0x10)=c; Reg(c,o+0x10); At<CPed*>(o,0x14)=s; Reg(s,o+0x14);
                            setTimer(0x18); setTimer(0x24); o[0x30]=uint8(next());
                            // vehiculo del sospechoso: el otro coche, el mismo o ninguno
                            { const uint32 m=next()%5; At<CVehicle*>(Ped(2),P_VEHICLE)=m<3 ? Veh(1) : m==3 ? Veh(0) : nullptr; }
                            auto needVeh=[&]{ if(!At<CVehicle*>(o,0xC)){ At<CVehicle*>(o,0xC)=Veh(0); Reg(Veh(0),o+0xC); } };
                            auto needSuspect=[&]{ if(!At<CPed*>(o,0x14)){ At<CPed*>(o,0x14)=Ped(2); Reg(Ped(2),o+0x14); } };
                            if(nm=="Destructor"){ dtorSub(); callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="MakeAbortable"){
                                At<CTask*>(o,8)=next()%6 ? Fake(0) : nullptr;
                                { const uint32 m=next()%4; At<CVehicle*>(p0,P_VEHICLE)=m<2 ? At<CVehicle*>(o,0xC) : m==2 ? Veh(1) : nullptr; }
                                ret=callAbort(p0,pickPrio(),pickEvent());
                                for(auto* h:hooks)h->State(false);
                                out["veh"]=json::array({VehOut(0),VehOut(1)});
                            }
                            else if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_SIMPLE_CAR_DRIVE,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_ENTER_CAR_AS_DRIVER,TASK_SIMPLE_STAND_STILL,
                                                       TASK_COMPLEX_ENTER_CAR_AS_PASSENGER,TASK_COMPLEX_POLICE_PURSUIT,TASK_COMPLEX_WANDER,TASK_COMPLEX_CAR_DRIVE_MISSION,
                                                       TASK_COMPLEX_CAR_DRIVE_MISSION,TASK_COMPLEX_CAR_DRIVE_MISSION,TASK_FINISHED,TASK_SIMPLE_PAUSE};
                                const int32 type=t[next()%std::size(t)];
                                // fuera de los fixtures: vehiculo o sospechoso nulos con CAR_DRIVE_MISSION (el EXE los desreferencia)
                                if(type==TASK_COMPLEX_CAR_DRIVE_MISSION){ needVeh(); needSuspect(); }
                                auto* r=reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,type,p0); collectFresh(r); ret=Name(r);
                            }
                            else {
                                const bool control=nm=="ControlSubTask", nextSub=nm=="CreateNextSubTask";
                                if(control||nextSub){
                                    static const int32 tc[]{TASK_COMPLEX_POLICE_PURSUIT,TASK_COMPLEX_POLICE_PURSUIT,TASK_COMPLEX_WANDER,TASK_COMPLEX_CAR_DRIVE_MISSION,
                                                            TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_PAUSE};
                                    static const int32 tn[]{TASK_COMPLEX_CAR_DRIVE_MISSION,TASK_COMPLEX_POLICE_PURSUIT,TASK_COMPLEX_POLICE_PURSUIT,TASK_COMPLEX_POLICE_PURSUIT,
                                                            TASK_SIMPLE_CAR_DRIVE,TASK_COMPLEX_ENTER_CAR_AS_PASSENGER,TASK_COMPLEX_ENTER_CAR_AS_DRIVER,TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_PAUSE};
                                    if(control)setSub(tc,std::size(tc)); else setSub(tn,std::size(tn));
                                    fakes[0][0xC]=uint8(next()); // m_CouldJoinPursuit (bit 2) cuando la falsa hace de persecucion
                                    const int32 st0=At<int32>(fakes[0].data(),FK_TYPE);
                                    // fuera de los fixtures: vehiculo nulo cuando el EXE lo desreferencia (crear CAR_DRIVE_MISSION, salir del coche)
                                    if(st0!=TASK_COMPLEX_POLICE_PURSUIT&&st0!=TASK_COMPLEX_WANDER&&st0!=TASK_COMPLEX_LEAVE_CAR&&st0!=TASK_SIMPLE_PAUSE)needVeh();
                                    if(control&&st0==TASK_COMPLEX_POLICE_PURSUIT&&next()%2)needVeh();
                                    // distancias exactas: vehiculo propio a 4 del peaton, vehiculo del sospechoso a 10 (o NaN)
                                    const CVector pp=p0->GetPosition();
                                    { const uint32 m=next()%5; auto& vp=At<CVector>(vehMats[0].data(),0x30);
                                      if(m==0)vp=CVector{pp.x+4.f,pp.y,pp.z}; else if(m==1)vp=CVector{pp.x,pp.y,qnan}; else if(m==2)vp=CVector{pp.x+uni(-3.f,3.f),pp.y+uni(-2.f,2.f),pp.z}; }
                                    { const uint32 m=next()%5; auto& vp=At<CVector>(vehMats[1].data(),0x30);
                                      if(m==0)vp=CVector{pp.x,pp.y+10.f,pp.z}; else if(m==1)vp=CVector{qnan,pp.y,pp.z}; else if(m==2)vp=CVector{pp.x+uni(-7.f,7.f),pp.y+uni(-7.f,7.f),pp.z}; }
                                    // velocidad del coche del sospechoso con velocidad^2 * paso * 50 exactamente 1
                                    if(next()%6==0){ At<float>(reinterpret_cast<void*>(TIMESTEP),0)=1.f; At<CVector>(vehs[1].data(),E_MOVESPEED)=CVector{0.f,0.f,std::sqrt(0.02f)}; }
                                    if(next()%6==0)At<CPed*>(vehs[0].data(),V_DRIVER)=nullptr;
                                }
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x80,true);
                                out["veh"]=json::array({VehOut(0),VehOut(1)});
                            }
                            finish(0x34);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexEnterCarAsDriverTimed"||cls=="CTaskComplexEnterCarAsPassengerTimed") {
                        // conductor: +0xC vehiculo (referencia), +0x10 intervalo, +0x14 movimiento, temporizador +0x18
                        // pasajero: +0xC vehiculo (referencia), +0x10 tiempo, +0x14 asiento, +0x18 seguir tras caer, +0x1C movimiento, temporizador +0x20
                        const bool drv=cls=="CTaskComplexEnterCarAsDriverTimed"; const size_t size=drv ? 0x24 : 0x2C, tmr=drv ? 0x18 : 0x20;
                        if(nm=="Constructor"){
                            prefill(size);
                            CVehicle* v=next()%4 ? Veh(0) : nullptr; const int32 time=next()%6==0 ? -int32(next()%100) : int32(next()%20000);
                            if(drv)ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32)>(target)(o,v,time));
                            else ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,uint32,int32,uint32)>(target)(o,v,next()%4,time,uint32(next()%2)|(next()&0xFF00)));
                            ctorDone(size);
                        } else {
                            setTask(drv ? VT_EDT : VT_EPT,size);
                            CVehicle* v=next()%6 ? Veh(0) : nullptr; At<CVehicle*>(o,0xC)=v; Reg(v,o+0xC);
                            At<int32>(o,0x10)=next()%6==0 ? -int32(next()%100) : int32(next()%20000);
                            if(drv){ static const int32 ms[]{6,4,7}; At<int32>(o,0x14)=ms[next()%3]; }
                            else { static const int32 seats[]{0,0,0,1,2,3,8}; At<int32>(o,0x14)=seats[next()%7]; o[0x18]=uint8(next()%2); static const int32 ms[]{6,4,7}; At<int32>(o,0x1C)=ms[next()%3]; }
                            setTimer(tmr);
                            if(nm=="Destructor"){ dtorSub(); callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="StopTimer"){ callStop(Ev()); for(auto* h:hooks)h->State(false); }
                            else if(nm=="MakeAbortable"){ At<CTask*>(o,8)=Fake(0); ret=callAbort(p0,pickPrio(),pickEvent()); for(auto* h:hooks)h->State(false); out["sub"]=Region(fakes[0].data(),0x80,true); }
                            else {
                                At<CTask*>(o,8)=Fake(0);
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x80,true);
                            }
                            finish(size);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexEvasiveDiveAndGetUp") {
                        // +0xC vehiculo a esquivar (referencia), +0x10 tiempo inconsciente, +0x14 direccion, +0x20 orientarse antes
                        auto pickDir=[&]{ const uint32 m=next()%6; return m==0 ? CVector{} : m==1 ? CVector{0.f,-1.f,0.f} : m==2 ? CVector{-1.f,0.f,0.f} : CVector{uni(-2.f,2.f),uni(-2.f,2.f),uni(-1.f,1.f)}; };
                        if(nm=="Constructor"){
                            prefill(0x24);
                            CVehicle* v=next()%4 ? Veh(int(next()%NVEH)) : nullptr; At<CVector>(dirArg.data(),0)=pickDir();
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32,CVector*,uint32)>(target)(o,v,int32(next()%5000),reinterpret_cast<CVector*>(dirArg.data()),uint32(next()%2)|(next()&0xFF00)));
                            ctorDone(0x24);
                        } else {
                            setTask(VT_EVD,0x24); CVehicle* v=next()%5 ? Veh(0) : nullptr; At<CVehicle*>(o,0xC)=v; Reg(v,o+0xC);
                            At<int32>(o,0x10)=int32(next()%5000); At<CVector>(o,0x14)=pickDir(); o[0x20]=uint8(next()%2);
                            if(nm=="Destructor"){ dtorSub(); callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="MakeAbortable"){ At<CTask*>(o,8)=Fake(0); ret=callAbort(p0,pickPrio(),pickEvent()); for(auto* h:hooks)h->State(false); out["assoc"]=Region(assoc.data(),0x40); }
                            else if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_SIMPLE_EVASIVE_DIVE,TASK_SIMPLE_PAUSE,TASK_SIMPLE_GET_UP,TASK_SIMPLE_ACHIEVE_HEADING,TASK_SIMPLE_ACHIEVE_HEADING,TASK_FINISHED,TASK_SIMPLE_STAND_STILL};
                                auto* r=reinterpret_cast<CTask*(__thiscall*)(void*,int32)>(target)(o,t[next()%std::size(t)]); collectFresh(r); ret=Name(r);
                            }
                            else {
                                static const int32 t[]{TASK_SIMPLE_ACHIEVE_HEADING,TASK_SIMPLE_EVASIVE_DIVE,TASK_SIMPLE_PAUSE,TASK_SIMPLE_PAUSE,TASK_SIMPLE_GET_UP,TASK_SIMPLE_STAND_STILL};
                                setSub(t,std::size(t));
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x80,true);
                            }
                            finish(0x24);
                            out["ped_rotation"]=json::array({At<uint32>(p0,P_ROT),At<uint32>(p0,P_AIM)});
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexFacial") {
                        // +0xC mascando, +0xD hablando el frame anterior, +0xE abortando, +0x10/+0x14 peticion A y duracion,
                        // +0x18/+0x1C peticion B y duracion
                        auto pickFacial=[&]{ static const int32 ft[]{-1,-1,0,1,3,5,7,8}; return ft[next()%8]; };
                        if(nm=="Constructor"){ prefill(0x20); ret=Name(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o)); ctorDone(0x20); }
                        else {
                            setTask(VT_FAC,0x20); o[0xC]=uint8(next()%2); o[0xD]=uint8(next()%2); o[0xE]=uint8(next()%2);
                            At<int32>(o,0x10)=pickFacial(); At<int32>(o,0x14)=next()%6==0 ? -1 : int32(next()%8000);
                            At<int32>(o,0x18)=pickFacial(); At<int32>(o,0x1C)=next()%6==0 ? -1 : int32(next()%8000);
                            static const int32 t[]{TASK_SIMPLE_FACIAL,TASK_SIMPLE_FACIAL,TASK_SIMPLE_PAUSE};
                            setSub(t,std::size(t));
                            // tarea facial falsa: tipo de expresion en +0x14 (a veces igual que la peticion A)
                            At<int32>(fakes[0].data(),0x14)=next()%2 ? At<int32>(o,0x10) : pickFacial();
                            if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="MakeAbortable"){ ret=callAbort(p0,pickPrio(),pickEvent()); for(auto* h:hooks)h->State(false); }
                            else if(nm=="SetRequest"){
                                const int32 a=pickFacial(), da=int32(next()%8000)-500, b=pickFacial(), db=int32(next()%8000)-500;
                                reinterpret_cast<void(__thiscall*)(void*,int32,int32,int32,int32)>(target)(o,a,da,b,db);
                            }
                            else if(nm=="StopAll"){ callVoid(); }
                            else { auto* r=callTask(p0); collectFresh(r); ret=Name(r); }
                            for(auto* h:hooks)h->State(false);
                            out["sub"]=Region(fakes[0].data(),0x80,true);
                            finish(0x20);
                        }
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
