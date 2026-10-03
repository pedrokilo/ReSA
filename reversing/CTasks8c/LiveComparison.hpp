#pragma once
// Banco dentro del motor: tareas complejas C y golpes (lista de 8, 48 funciones).
// CTaskComplexMoveBackAndJump (constructor, destructor, CreateSubTask, Clone, GetTaskType, ControlSubTask,
// CreateFirstSubTask, CreateNextSubTask), CTaskComplexObserveTrafficLights (constructor, destructor, Clone, GetTaskType,
// MakeAbortable, ControlSubTask, CreateFirstSubTask, CreateNextSubTask), CTaskComplexReactToGunAimedAt (constructor,
// destructor, CreateSubTask, Clone, GetTaskType, ControlSubTask, CreateFirstSubTask, CreateNextSubTask),
// CTaskComplexWalkRoundFire (constructor, destructor, ComputeDetourTarget, Clone, GetTaskType, ControlSubTask,
// CreateFirstSubTask, CreateNextSubTask), CTaskSimpleBeHit (constructor, destructor, FinishAnimBeHitCB, StartAnim, Clone,
// GetTaskType, MakeAbortable, ProcessPed) y CTaskSimpleBikeJacked (constructor, destructor, FinishAnimBikeHitCB, Clone,
// GetTaskType, MakeAbortable, ProcessPed, SetPedPosition).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 3 peatones (inteligencia, clump con tipo 2 y estadisticas propias), 2 vehiculos (vtable falsa: hueco 47 GetRideAnimData),
// tareas falsas (vtable trazada), evento falso, asociaciones de animacion falsas y tarea de pelea falsa.
// Se simulan y trazan: CPed::Say/DisablePedSpeech/EnablePedSpeech/SetRadioStation/SetPedPositionInCar, SayJacked,
// SayJacking, CVehicle::IsDriver, CAEVehicleAudioEntity::PlayerAboutToExitVehicleAsDriver, CAEPedAudioEntity::AddAudioEvent,
// CAnimManager::BlendAnimation, RpAnimBlendClumpGetAssociation, CAnimBlendAssociation::SetFinishCallback/SetCurrentTime,
// CPedIntelligence::GetTaskFighting, CEventGroup::Add, CTrafficLights::LightForPeds,
// CPedGeometryAnalyser::ComputeRouteRoundSphere, CGeneral::GetRadianAngleBetweenPoints/LimitRadianAngle/
// GetNodeHeadingFromVector y el RNG (rand 0x821B1E y CGeneral::GetRandomNumber).
// Pool de tareas: m_LastFreeSlot y estados restaurados y slots libres con 0xCD en cada ejecucion; el reloj se fija por caso
// y se restaura. Se compara la memoria de los fixtures (punteros traducidos a nombres), las tareas nuevas, las
// referencias, el retorno y la traza.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/Task.h"
#include "TaskComplexMoveBackAndJump.h"
#include "TaskComplexObserveTrafficLights.h"
#include "TaskComplexReactToGunAimedAt.h"
#include "TaskComplexWalkRoundFire.h"
#include "TaskSimpleBeHit.h"
#include "TaskSimpleBikeJacked.h"
#include "TaskSimpleFight.h"
#include "TaskUtilityLineUpPedWithCar.h"
#include "PedGeometryAnalyser.h"
#include "PedIntelligence.h"
#include "TrafficLights.h"
#include "Events/EventGroup.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks8cLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }

constexpr int NPEDS=3, NVEH=2, FAKES=3;
// tareas falsas: los campos de control quedan por encima de 0x50 (la falsa de CTaskSimpleGoToPoint se lee hasta 0x1C)
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_SUB=0x64, FK_TYPE=0x68, FK_CLONE=0x6C;
// desplazamientos crudos del EXE
constexpr size_t E_POS=4, E_MATRIX=0x14, E_CLUMP=0x18, E_TYPE=0x36;                                 // CEntity
constexpr size_t PH_MOVE=0x44, PH_COLLVEL=0xE0, AUDIO=0x138;                                       // CPhysical / audio
constexpr size_t P_FLAGS=0x46C, P_INTEL=0x47C, P_ANIMGROUP=0x4D4, P_ROT_CUR=0x558, P_ROT_AIM=0x55C,
                 P_VEHICLE=0x58C, P_PEDTYPE=0x598, P_STATS=0x59C;                                  // CPed
constexpr size_t I_EVENTGROUP=0x68;                                                                // CPedIntelligence
constexpr uintptr_t VT_MBJ=0x86F390, VT_OTL=0x86DF98, VT_REACT=0x870D78, VT_WRF=0x86F3E8, VT_BEHIT=0x86D844, VT_BIKE=0x86EFAC;
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84, EXE_DEFAULT_ANIM_CB=0x4D1390, EXE_BEHIT_CB=0x620900, EXE_BIKE_CB=0x648D30;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,0x40>,NPEDS> clumps{}, stats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<uint8,0x100> object{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAnimBlendAssociation)>,3> anims{};
alignas(16) inline std::array<uint8,0x60> event{}, fight{}, ride{};
alignas(16) inline std::array<uint8,0x20> outBuf{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline void* Anim(int n) { return anims[n].data(); }
inline int ClumpIndex(const void* p) { for(int n=0;n<NPEDS;++n)if(p==clumps[n].data())return n; return -1; }

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
inline std::array<void*,16> eventVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline uintptr_t lineupPre{}, lineupNew{};
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(vehVtbl.data()))return "veh_vt"; if(v==uintptr_t(eventVtbl.data()))return "event_vt";
    if(v==EXE_DEFAULT_ANIM_CB||v==std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB))return "DefaultAnimCB";
    if(v==EXE_BEHIT_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleBeHit::FinishAnimBeHitCB))return "FinishAnimBeHitCB";
    if(v==EXE_BIKE_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleBikeJacked::FinishAnimBikeHitCB))return "FinishAnimBikeHitCB";
    if(lineupPre&&v==lineupPre)return "lineup_pre"; if(lineupNew&&v==lineupNew)return "lineup_new";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n);
                              if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n); if(auto s=in(clumps[n].data(),clumps[n].size(),"clump"))return *s+"#"+std::to_string(n);
                              if(auto s=in(stats[n].data(),stats[n].size(),"stats"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<3;++n)if(auto s=in(anims[n].data(),anims[n].size(),"anim"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(event.data(),event.size(),"event"))return *s;
    if(auto s=in(fight.data(),fight.size(),"fight"))return *s; if(auto s=in(ride.data(),ride.size(),"ride"))return *s;
    if(auto s=in(outBuf.data(),outBuf.size(),"out"))return *s;
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
// punteros a la pila del llamador (locales distintos en el EXE y en la DLL)
inline json NameOrStack(const void* p) { auto n=Name(p); return n.is_number() ? json("stack") : n; }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); for(auto& v:vehs)r.push_back(RefList(v.data())); return r; }
inline json Vec(const CVector& v) { return {F(v.x),F(v.y),F(v.z)}; }
// tarea serializada sin vptr (se compara el tipo)
inline json SerTask(const void* p) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType(); const auto* w=static_cast<const uint32*>(p);
    json words=json::array(); for(size_t k=1;k<TASK_SLOT/4;++k)words.push_back(Name(w[k]));
    return {{"type",type},{"words",words}};
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{}, assocMask{};
inline int16 sayRet{}; inline uint8 lightRet{}, isDriverRet{}, jackedRet{}, jackingRet{}, routeRet{};
inline uintptr_t blendRet{}, fightRet{};
inline CVector routeOut{}, routeAux{};
inline int eventPrio{};

// tareas falsas
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(t)}); return At<CTask*>(t,FK_CLONE); }
inline CTask* __fastcall FSub(CTask* t,void*) { return At<CTask*>(t,FK_SUB); }
inline bool __fastcall FSimple(CTask*,void*) { return true; }
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) { trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e)}); return At<uint8>(t,FK_ABORT)!=0; }
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<size_t S,int... N> void FillTraps(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// vehiculo falso: hueco 47 = GetRideAnimData
inline void* __fastcall FRide(void* v,void*) { trace.push_back({"get_ride_anim_data",Name(v)}); return ride.data(); }
// evento falso: hueco 2 = GetEventPriority
inline int __fastcall EPrio(void* e,void*) { trace.push_back({"event_get_priority",Name(e)}); return eventPrio; }

// dependencias simuladas
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
// el EXE empuja el contexto con basura en los 16 bits altos: solo se traza el int16
inline int16 __fastcall FakeSay(CPed* p,void*,int ctx,uint32 delay,float prob,bool overrideSilence,bool forceAudible,bool frontEnd) {
    trace.push_back({"say",Name(p),int16(ctx),delay,F(prob),overrideSilence,forceAudible,frontEnd,sayRet}); return sayRet;
}
inline void __fastcall FakeDisableSpeech(CPed* p,void*,bool stop) { trace.push_back({"disable_ped_speech",Name(p),stop}); }
inline void __fastcall FakeEnableSpeech(CPed* p,void*) { trace.push_back({"enable_ped_speech",Name(p)}); }
inline void __fastcall FakeRadio(CPed* p,void*) { trace.push_back({"set_radio_station",Name(p)}); }
inline void __fastcall FakePosInCar(CPed* p,void*) { trace.push_back({"set_ped_position_in_car",Name(p)}); }
inline bool __cdecl FakeJacked(CPed* p,CVehicle* v,uint32 delay) { trace.push_back({"say_jacked",Name(p),Name(v),delay}); return jackedRet!=0; }
inline bool __cdecl FakeJacking(CPed* j,CPed* p,CVehicle* v,uint32 delay) { trace.push_back({"say_jacking",Name(j),Name(p),Name(v),delay}); return jackingRet!=0; }
inline bool __fastcall FakeIsDriver(const CVehicle* v,void*,const CPed* p) { trace.push_back({"is_driver",Name(v),Name(p),isDriverRet}); return isDriverRet!=0; }
inline void __fastcall FakeAboutToExit(void* a,void*) { trace.push_back({"player_about_to_exit_vehicle",Name(a)}); }
inline void __fastcall FakePedAudio(void* a,void*,int32 ev,float vol,float speed,void* phys,int32 surf,int32 a7,uint32 maxVol) {
    trace.push_back({"ped_audio_event",Name(a),ev,F(vol),F(speed),Name(phys),surf,a7,maxVol});
}
inline CAnimBlendAssociation* __cdecl FakeBlend(RpClump* c,int32 group,int32 id,float delta) { trace.push_back({"blend_animation",Name(c),group,id,F(delta)}); return reinterpret_cast<CAnimBlendAssociation*>(blendRet); }
inline CAnimBlendAssociation* __cdecl FakeGetAssoc(RpClump* c,uint32 id) {
    const int n=ClumpIndex(c); const bool hit=n>=0&&((assocMask>>((id+uint32(n)*7)%32))&1);
    trace.push_back({"clump_get_association",Name(c),id,hit}); return hit ? reinterpret_cast<CAnimBlendAssociation*>(Anim(2)) : nullptr;
}
inline void __fastcall FakeSetFinishCb(void* a,void*,void* cb,void* data) { trace.push_back({"set_finish_callback",Name(a),Name(cb),Name(data)}); }
inline void __fastcall FakeSetTime(void* a,void*,float t) { trace.push_back({"set_current_time",Name(a),F(t)}); }
inline void* __fastcall FakeFighting(void* in,void*) { trace.push_back({"get_task_fighting",Name(in),Name(fightRet)}); return reinterpret_cast<void*>(fightRet); }
// campos del CEventKnockOffBike (byte 0x35: solo los bits 0-1, el resto queda sin escribir en la pila)
inline CEvent* __fastcall FakeGroupAdd(void* g,void*,CEvent* e,bool valid) {
    json f={int(e->GetEventType()),At<uint32>(e,4),At<uint8>(e,8)};
    for(size_t off=0xC;off<=0x2C;off+=4)f.push_back(At<uint32>(e,off));
    f.push_back(Name(At<uint32>(e,0x30))); f.push_back(At<uint8>(e,0x34)); f.push_back(At<uint8>(e,0x35)&3); f.push_back(At<uint8>(e,0x36)); f.push_back(At<uint8>(e,0x37));
    f.push_back(Name(At<uint32>(e,0x38)));
    trace.push_back({"event_group_add",Name(g),f,valid}); return nullptr;
}
inline uint8 __cdecl FakeLight() { trace.push_back({"light_for_peds",lightRet}); return lightRet; }
inline bool __cdecl FakeRoute(const CPed& p,const CColSphere& s,const CVector& a,const CVector& b,CVector& o5,CVector& o6) {
    trace.push_back({"route_round_sphere",Name(&p),Vec(s.m_vecCenter),F(s.m_fRadius),At<uint8>(&s,0x10),At<uint8>(&s,0x11),At<uint8>(&s,0x12),
                     NameOrStack(&a),Vec(a),NameOrStack(&b),Vec(b),NameOrStack(&o5),NameOrStack(&o6),routeRet});
    o5=routeAux; o6=routeOut; return routeRet!=0;
}
// funciones matematicas de CGeneral: resultado determinista a partir de los bits de los argumentos
inline float __cdecl FakeAngle(float x1,float y1,float x2,float y2) { trace.push_back({"radian_angle_between_points",F(x1),F(y1),F(x2),F(y2)}); return float(std::atan2(double(y1)-double(y2),double(x1)-double(x2))); }
inline float __cdecl FakeLimit(float a) { trace.push_back({"limit_radian_angle",F(a)}); return a*0.5f+0.125f; }
inline uint32 __cdecl FakeNodeHeading(float x,float y) { trace.push_back({"node_heading_from_vector",F(x),F(y)}); return (F(x)^(F(y)*3u))%8u; }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=48;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexMoveBackAndJump","Clone",0x655C80,0x86F390,1,128},
    {"CTaskComplexMoveBackAndJump","Constructor",0x655340,0,-1,128},
    {"CTaskComplexMoveBackAndJump","ControlSubTask",0x655380,0x86F390,10,64},
    {"CTaskComplexMoveBackAndJump","CreateFirstSubTask",0x6577C0,0x86F390,9,128},
    {"CTaskComplexMoveBackAndJump","CreateNextSubTask",0x657770,0x86F390,8,256},
    {"CTaskComplexMoveBackAndJump","CreateSubTask",0x655390,0,-1,256},
    {"CTaskComplexMoveBackAndJump","Destructor",0x655370,0,-1,128},
    {"CTaskComplexMoveBackAndJump","GetTaskType",0x655360,0x86F390,4,64},
    {"CTaskComplexObserveTrafficLights","Clone",0x636410,0x86DF98,1,128},
    {"CTaskComplexObserveTrafficLights","Constructor",0x631790,0,-1,128},
    {"CTaskComplexObserveTrafficLights","ControlSubTask",0x6318E0,0x86DF98,10,256},
    {"CTaskComplexObserveTrafficLights","CreateFirstSubTask",0x6318D0,0x86DF98,9,256},
    {"CTaskComplexObserveTrafficLights","CreateNextSubTask",0x6317E0,0x86DF98,8,512},
    {"CTaskComplexObserveTrafficLights","Destructor",0x6317C0,0,-1,128},
    {"CTaskComplexObserveTrafficLights","GetTaskType",0x6317B0,0x86DF98,4,64},
    {"CTaskComplexObserveTrafficLights","MakeAbortable",0x6317D0,0x86DF98,6,128},
    {"CTaskComplexReactToGunAimedAt","Clone",0x6976E0,0x870D78,1,128},
    {"CTaskComplexReactToGunAimedAt","Constructor",0x6971B0,0,-1,256},
    {"CTaskComplexReactToGunAimedAt","ControlSubTask",0x698550,0x870D78,10,1024},
    {"CTaskComplexReactToGunAimedAt","CreateFirstSubTask",0x698530,0x870D78,9,256},
    {"CTaskComplexReactToGunAimedAt","CreateNextSubTask",0x6983C0,0x870D78,8,1024},
    {"CTaskComplexReactToGunAimedAt","CreateSubTask",0x697B70,0,-1,1024},
    {"CTaskComplexReactToGunAimedAt","Destructor",0x697230,0,-1,128},
    {"CTaskComplexReactToGunAimedAt","GetTaskType",0x697220,0x870D78,4,64},
    {"CTaskComplexWalkRoundFire","Clone",0x655D40,0x86F3E8,1,128},
    {"CTaskComplexWalkRoundFire","ComputeDetourTarget",0x6581B0,0,-1,512},
    {"CTaskComplexWalkRoundFire","Constructor",0x655720,0,-1,256},
    {"CTaskComplexWalkRoundFire","ControlSubTask",0x6585C0,0x86F3E8,10,1024},
    {"CTaskComplexWalkRoundFire","CreateFirstSubTask",0x6584E0,0x86F3E8,9,512},
    {"CTaskComplexWalkRoundFire","CreateNextSubTask",0x655790,0x86F3E8,8,64},
    {"CTaskComplexWalkRoundFire","Destructor",0x655780,0,-1,128},
    {"CTaskComplexWalkRoundFire","GetTaskType",0x655770,0x86F3E8,4,64},
    {"CTaskSimpleBeHit","Clone",0x623290,0x86D844,1,256},
    {"CTaskSimpleBeHit","Constructor",0x620780,0,-1,256},
    {"CTaskSimpleBeHit","Destructor",0x620810,0,-1,256},
    {"CTaskSimpleBeHit","FinishAnimBeHitCB",0x620900,0,-1,128},
    {"CTaskSimpleBeHit","GetTaskType",0x620800,0x86D844,4,64},
    {"CTaskSimpleBeHit","MakeAbortable",0x620890,0x86D844,6,1024},
    {"CTaskSimpleBeHit","ProcessPed",0x620A20,0x86D844,7,1024},
    {"CTaskSimpleBeHit","StartAnim",0x620910,0,-1,1024},
    {"CTaskSimpleBikeJacked","Clone",0x64A200,0x86EFAC,1,256},
    {"CTaskSimpleBikeJacked","Constructor",0x648B90,0,-1,256},
    {"CTaskSimpleBikeJacked","Destructor",0x648C40,0,-1,256},
    {"CTaskSimpleBikeJacked","FinishAnimBikeHitCB",0x648D30,0,-1,128},
    {"CTaskSimpleBikeJacked","GetTaskType",0x648C30,0x86EFAC,4,64},
    {"CTaskSimpleBikeJacked","MakeAbortable",0x648CE0,0x86EFAC,6,256},
    {"CTaskSimpleBikeJacked","ProcessPed",0x64C970,0x86EFAC,7,2048},
    {"CTaskSimpleBikeJacked","SetPedPosition",0x648D00,0x86EFAC,8,256},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks8c-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks8c-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks8c");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks8c-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(fakeVtbl,std::make_integer_sequence<int,12>{}); FillTraps(vehVtbl,std::make_integer_sequence<int,96>{}); FillTraps(eventVtbl,std::make_integer_sequence<int,16>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    vehVtbl[47]=reinterpret_cast<void*>(&FRide); eventVtbl[2]=reinterpret_cast<void*>(&EPrio);
    const auto occupancyStart=Occupancy();
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    const int32 lf0=Last(TaskHdr());
    const std::vector<uint8> statesStart(States(TaskHdr()),States(TaskHdr())+Cap(TaskHdr()));
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch s1(bit_cast<uintptr_t>(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say)),FakeSay), s1e(0x5EFFE0,FakeSay);
        EntryPatch s2(bit_cast<uintptr_t>(&CPed::DisablePedSpeech),FakeDisableSpeech), s2e(0x5EFF60,FakeDisableSpeech);
        EntryPatch s3(bit_cast<uintptr_t>(&CPed::EnablePedSpeech),FakeEnableSpeech), s3e(0x5EFF70,FakeEnableSpeech);
        EntryPatch s4(bit_cast<uintptr_t>(&CPed::SetRadioStation),FakeRadio), s4e(0x5DFD90,FakeRadio);
        EntryPatch s5(bit_cast<uintptr_t>(&CPed::SetPedPositionInCar),FakePosInCar), s5e(0x5DF910,FakePosInCar);
        EntryPatch s6(bit_cast<uintptr_t>(&SayJacked),FakeJacked), s6e(0x6497A0,FakeJacked);
        EntryPatch s7(bit_cast<uintptr_t>(&SayJacking),FakeJacking), s7e(0x6497F0,FakeJacking);
        EntryPatch v1(bit_cast<uintptr_t>(static_cast<bool(CVehicle::*)(const CPed*) const>(&CVehicle::IsDriver)),FakeIsDriver), v1e(0x6D1C40,FakeIsDriver);
        EntryPatch v2(bit_cast<uintptr_t>(&CAEVehicleAudioEntity::PlayerAboutToExitVehicleAsDriver),FakeAboutToExit), v2e(0x4F5BA0,FakeAboutToExit);
        EntryPatch v3(bit_cast<uintptr_t>(&CAEPedAudioEntity::AddAudioEvent),FakePedAudio), v3e(0x4E2BB0,FakePedAudio);
        EntryPatch a1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),FakeBlend), a1e(0x4D4610,FakeBlend);
        EntryPatch a2(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),FakeGetAssoc), a2e(0x4D68B0,FakeGetAssoc);
        EntryPatch a3(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetFinishCallback),FakeSetFinishCb), a3e(0x4CEBE0,FakeSetFinishCb);
        EntryPatch a4(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetCurrentTime),FakeSetTime), a4e(0x4CEA80,FakeSetTime);
        EntryPatch i1(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskFighting),FakeFighting), i1e(0x600F30,FakeFighting);
        EntryPatch e1(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),FakeGroupAdd), e1e(0x4AB420,FakeGroupAdd);
        EntryPatch t1(bit_cast<uintptr_t>(&CTrafficLights::LightForPeds),FakeLight), t1e(0x49D400,FakeLight);
        EntryPatch g1(bit_cast<uintptr_t>(&CPedGeometryAnalyser::ComputeRouteRoundSphere),FakeRoute), g1e(0x5F1890,FakeRoute);
        EntryPatch m1(bit_cast<uintptr_t>(static_cast<float(*)(float,float,float,float)>(&CGeneral::GetRadianAngleBetweenPoints)),FakeAngle), m1e(0x53CBE0,FakeAngle);
        EntryPatch m2(bit_cast<uintptr_t>(&CGeneral::LimitRadianAngle),FakeLimit), m2e(0x53CB50,FakeLimit);
        EntryPatch m3(bit_cast<uintptr_t>(&CGeneral::GetNodeHeadingFromVector),FakeNodeHeading), m3e(0x53CDC0,FakeNodeHeading);

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
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x8C8Cu; if(!st)st=1; trace=json::array(); seed=uint32(i);
                    rngCalls=0; freshIds.clear(); lineupPre=0; lineupNew=0;
                    CleanRefs();
                    timeMs=next()%3 ? 100000u+next()%1000000u : next();
                    // peatones: matriz (o posicion sin matriz), inteligencia, clump (tipo 2), estadisticas, tipo y flags
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                        for(auto& b:clumps[n])b=uint8(next()); clumps[n][0]=2; for(auto& b:stats[n])b=uint8(next());
                        p[E_TYPE]=3; At<void*>(p,P_INTEL)=intels[n].data(); At<void*>(p,E_CLUMP)=clumps[n].data(); At<void*>(p,P_STATS)=stats[n].data();
                        const CVector pos{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        if(next()%4){ At<void*>(p,E_MATRIX)=pedMats[n].data();
                            for(int r=0;r<3;++r)At<CVector>(pedMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            At<CVector>(pedMats[n].data(),0x30)=pos; }
                        else At<CVector>(p,E_POS)=pos;
                        { static const int32 types[]{4,5,7,8,8,6,0,1}; At<int32>(p,P_PEDTYPE)=types[next()%std::size(types)]; }
                        At<uint32>(p,P_FLAGS)=next(); At<int32>(p,P_ANIMGROUP)=int32(next()%120);
                        At<float>(p,P_ROT_CUR)=uni(-6.f,6.f); At<float>(p,P_ROT_AIM)=uni(-6.f,6.f);
                        { const uint32 m=next()%4; At<CVehicle*>(p,P_VEHICLE)=m==0 ? nullptr : Veh(int(m%2)); }
                    }
                    // vehiculos: vtable falsa con trampas (hueco 47 GetRideAnimData), matriz, velocidad e impacto
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%8)<<3); At<void*>(v,E_MATRIX)=vehMats[n].data();
                        for(int r=0;r<3;++r)At<CVector>(vehMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        At<CVector>(v,PH_MOVE)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-0.2f,0.2f)};
                        At<CVector>(v,PH_COLLVEL)=CVector{uni(-3.f,3.f),uni(-3.f,3.f),uni(-1.f,1.f)};
                    }
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,FAKE_SIZE); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_PAUSE+n; fakes[n][FK_ABORT]=uint8(next()%2); }
                    // asociaciones de animacion: bytes aleatorios con floats controlados (+0x1C delta, +0x20 tiempo actual)
                    for(auto& a:anims){ for(auto& b:a)b=uint8(next()); At<float>(a.data(),0x18)=uni(0.f,1.f); At<float>(a.data(),0x1C)=uni(-4.f,4.f);
                        { const uint32 m=next()%6; At<float>(a.data(),0x20)=m==0 ? 0.3f : m==1 ? qnan : m==2 ? std::bit_cast<float>(0x3E99999Bu) : uni(0.f,1.f); }
                        At<float>(a.data(),0x24)=uni(0.f,2.f); At<float>(a.data(),0x28)=uni(0.f,2.f); }
                    for(auto& b:event)b=uint8(next()); At<void*>(event.data(),0)=eventVtbl.data();
                    // tarea de pelea falsa: +0x24 combo (con signo; 4..15 indexa m_aComboData del EXE, solo lectura), +0x25 golpe actual (con signo)
                    for(auto& b:fight)b=uint8(next());
                    { const uint32 m=next()%3; fight[0x24]=uint8(m==0 ? 4+next()%12 : m==1 ? next()%4 : 0x80|next()); }
                    fight[0x25]=uint8(next()%2 ? next()%4 : next());
                    for(auto& b:ride)b=uint8(next()); At<int32>(ride.data(),0)=int32(next()%120);
                    for(auto& b:outBuf)b=uint8(next());
                    { static const int prios[]{0,30,61,62,63,75,100}; eventPrio=prios[next()%std::size(prios)]; }
                    seed=next(); sayRet=int16(next()%3 ? int16(next()%40) : -1); lightRet=uint8(next()%2); isDriverRet=uint8(next()%2);
                    jackedRet=uint8(next()%2); jackingRet=uint8(next()%2); routeRet=uint8(next()%4!=0); assocMask=next();
                    blendRet=uintptr_t(Anim(1)); fightRet=next()%4 ? uintptr_t(fight.data()) : 0;
                    routeOut=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)}; routeAux=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};

                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches}}; json ret=nullptr; std::vector<CTask*> roots;
                    auto* o=object.data(); auto* p0=Ped(0);
                    std::memset(o,0xCD,object.size());
                    auto collectFresh=[&](CTask* r){
                        for(auto* h:hooks)h->State(false);
                        auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                        for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                        json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t)); out["tasks"]=tasks;
                        if(r&&freshIds.contains(uintptr_t(r)))roots.push_back(r);
                    };
                    auto setTask=[&](uintptr_t vtbl,size_t size){ for(size_t b=0;b<size;++b)o[b]=uint8(next()); At<uintptr_t>(o,0)=vtbl; At<uint32>(o,4)=0; if(size>8)At<uint32>(o,8)=0; };
                    auto setSub=[&](){ At<CTask*>(o,8)=Fake(0); };
                    auto typeOf=[&](void* p){ return reinterpret_cast<CTask*>(p)->GetTaskType(); };
                    auto callTask=[&](CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    auto callBool=[&](CPed* ped){ return reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    auto callClone=[&]{ return reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); };
                    auto callDtor=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); };
                    auto callAbort=[&](int32 prio,const void* ev){ return reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const void*)>(target)(o,p0,prio,ev); };
                    auto callCb=[&](void* anim){ reinterpret_cast<void(__cdecl*)(void*,void*)>(target)(anim,o); };
                    auto ctorDone=[&](size_t size){ for(auto* h:hooks)h->State(false); out["object"]=Region(o,size,true); out["type"]=typeOf(o); };
                    auto inVeh=[&](CPed* p,bool on){ auto& fl=At<uint32>(p,P_FLAGS); fl=on ? (fl|0x100u) : (fl&~0x100u); };
                    auto subType=[&](std::initializer_list<int32> l){ At<int32>(fakes[0].data(),FK_TYPE)=*(l.begin()+next()%l.size()); };

                    if(nm=="GetTaskType") {
                        setTask(fn.vt,0x40);
                        ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        out["object"]=Region(o,0x40,true);
                    }
                    else if(cls=="CTaskComplexMoveBackAndJump") {
                        if(nm=="Constructor"){ ret=Name(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o)); ctorDone(0xC); }
                        else {
                            setTask(VT_MBJ,0xC);
                            if(nm=="Destructor"){ At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; callDtor(); }
                            else if(nm=="Clone"){ setSub(); auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_COMPLEX_JUMP,TASK_COMPLEX_JUMP,TASK_SIMPLE_HIT_WALL,TASK_SIMPLE_HIT_WALL,TASK_FINISHED,TASK_SIMPLE_PAUSE,TASK_NONE,0,-1};
                                setSub(); auto* r=reinterpret_cast<CTask*(__thiscall*)(void*,int32)>(target)(o,t[next()%std::size(t)]); collectFresh(r); ret=Name(r);
                            }
                            else {
                                setSub(); subType({TASK_SIMPLE_HIT_WALL,TASK_SIMPLE_HIT_WALL,TASK_COMPLEX_JUMP,TASK_COMPLEX_JUMP,TASK_SIMPLE_PAUSE,TASK_FINISHED});
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                            }
                            out["object"]=Region(o,0xC,true);
                        }
                    }
                    else if(cls=="CTaskComplexObserveTrafficLights") {
                        // +0xC temporizador (inicio, intervalo, iniciado, parado)
                        if(nm=="Constructor"){ ret=Name(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o)); ctorDone(0x18); }
                        else {
                            setTask(VT_OTL,0x18);
                            if(nm=="Destructor"){ At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; callDtor(); }
                            else if(nm=="Clone"){ setSub(); auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="MakeAbortable"){ setSub(); ret=callAbort(int32(next()%4),next()%2 ? event.data() : nullptr); }
                            else {
                                if(nm!="ControlSubTask"&&next()%4==0)At<CTask*>(o,8)=nullptr; else setSub();
                                subType({TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_ANIM,TASK_SIMPLE_ANIM,TASK_SIMPLE_PAUSE});
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                            }
                            out["object"]=Region(o,0x18,true);
                        }
                    }
                    else if(cls=="CTaskComplexReactToGunAimedAt") {
                        // +0xC quien apunta (referencia), +0x10 temporizador del paseo (inicio, intervalo, iniciado +0x18, parado +0x19)
                        auto setReact=[&](bool aimer){ setTask(VT_REACT,0x1C); At<CPed*>(o,0xC)=aimer ? Ped(1) : nullptr; Reg(At<CPed*>(o,0xC),o+0xC);
                            At<uint32>(o,0x10)=next()%4 ? timeMs-next()%20000 : next(); At<int32>(o,0x14)=next()%4 ? 10000 : int32(next()%30000)-5000;
                            o[0x18]=uint8(next()%4!=0); o[0x19]=uint8(next()%4==0); };
                        if(nm=="Constructor"){
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CPed*)>(target)(o,next()%4 ? Ped(1) : nullptr)); ctorDone(0x1C);
                        } else if(nm=="Destructor"){
                            setReact(next()%4!=0); At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; callDtor(); out["object"]=Region(o,0x1C,true);
                        } else {
                            const bool needAimer=nm=="CreateSubTask"||nm=="CreateNextSubTask"||nm=="Clone";
                            setReact(needAimer||next()%6!=0); setSub();
                            inVeh(p0,next()%2==0);
                            if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_SIMPLE_ACHIEVE_HEADING,TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_HANDS_UP,TASK_SIMPLE_PAUSE,TASK_SIMPLE_PAUSE,TASK_SIMPLE_COWER,
                                                       TASK_COMPLEX_SMART_FLEE_ENTITY,TASK_COMPLEX_WANDER,TASK_COMPLEX_WANDER,TASK_FINISHED,TASK_SIMPLE_STAND_STILL,-1};
                                auto* r=reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,t[next()%std::size(t)],p0); collectFresh(r); ret=Name(r);
                            }
                            else {
                                if(nm=="CreateNextSubTask")subType({TASK_SIMPLE_PAUSE,TASK_SIMPLE_PAUSE,TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_ACHIEVE_HEADING,TASK_SIMPLE_ACHIEVE_HEADING,
                                                                    TASK_SIMPLE_COWER,TASK_SIMPLE_HANDS_UP,TASK_COMPLEX_SMART_FLEE_ENTITY,TASK_COMPLEX_WANDER,TASK_SIMPLE_STAND_STILL});
                                else if(nm=="ControlSubTask")subType({TASK_COMPLEX_WANDER,TASK_COMPLEX_WANDER,TASK_COMPLEX_WANDER,TASK_SIMPLE_HANDS_UP});
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                            }
                            out["object"]=Region(o,0x1C,true); out["ped"]=Region(p0,sizeof(CPed));
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexWalkRoundFire") {
                        // +0xC estado de movimiento, +0x10 esfera del fuego (centro, radio), +0x20 destino, +0x2C posicion inicial del peaton
                        static const int32 ms[]{4,6,7,1};
                        auto setFire=[&]{ setTask(VT_WRF,0x38); At<int32>(o,0xC)=ms[next()%std::size(ms)];
                            At<CVector>(o,0x10)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)}; At<float>(o,0x1C)=uni(0.5f,8.f);
                            At<CVector>(o,0x20)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)}; At<CVector>(o,0x2C)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)}; };
                        if(nm=="Constructor"){
                            const CVector fire{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)}, dest{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,const CVector*,float,const CVector*)>(target)(o,ms[next()%std::size(ms)],&fire,uni(0.5f,8.f),&dest));
                            ctorDone(0x38);
                        } else {
                            setFire();
                            if(nm=="Destructor"){ At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; callDtor(); }
                            else if(nm=="Clone"){ setSub(); auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="ComputeDetourTarget"){
                                ret=reinterpret_cast<bool(__thiscall*)(void*,const CPed*,void*)>(target)(o,p0,outBuf.data()); out["out"]=Region(outBuf.data(),outBuf.size());
                            }
                            else {
                                setSub();
                                // subtarea: CTaskSimpleGoToPoint falsa (+0xC destino, +0x18 radio, +0x1C flags)
                                auto* s=fakes[0].data(); At<int32>(s,FK_TYPE)=TASK_SIMPLE_GO_TO_POINT;
                                { const uint32 m=next()%4; At<CVector>(s,0xC)=m==0 ? routeOut : CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                                  if(m==1){ const uint32 k=next()%3; At<CVector>(s,0xC)=routeOut; At<float>(s,0xC+4*k)+=1.f; } }
                                { const uint32 m=next()%4; At<float>(s,0x18)=m<2 ? 0.5f : m==2 ? qnan : uni(0.f,2.f); }
                                s[0x1C]=uint8(next());
                                if(next()%8==0)routeOut.y=qnan;
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                out["sub"]=Region(s,0x20,true);
                            }
                            out["object"]=Region(o,0x38,true);
                        }
                    }
                    else if(cls=="CTaskSimpleBeHit") {
                        // +8 atacante (referencia), +0xC terminada, +0xD anim anadida, +0x10 anim, +0x14 grupo, +0x18 zona, +0x1C dano, +0x20 direccion, +0x24 anim actual
                        static const int32 dirs[]{0,0,0,1,2,3,4,5,6,-1};
                        static const int32 dmgs[]{0,3,5,5,6,10,100,-2};
                        static const int32 zones[]{3,3,4,5,6,7,8,9};
                        auto setHit=[&](bool attacker){ setTask(VT_BEHIT,0x28); At<CPed*>(o,8)=attacker ? Ped(1) : nullptr; Reg(At<CPed*>(o,8),o+8);
                            o[0xC]=uint8(next()%5==0); o[0xD]=uint8(next()%3==0);
                            At<int32>(o,0x10)=next()%3 ? ANIM_ID_NO_ANIMATION_SET : int32(next()%300); At<int32>(o,0x14)=int32(next()%120);
                            At<int32>(o,0x18)=zones[next()%std::size(zones)]; At<int32>(o,0x1C)=dmgs[next()%std::size(dmgs)]; At<int32>(o,0x20)=dirs[next()%std::size(dirs)];
                            At<void*>(o,0x24)=next()%3==0 ? Anim(0) : nullptr; };
                        if(nm=="Constructor"){
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CPed*,int32,int32,int32)>(target)(o,next()%4 ? Ped(1) : nullptr,zones[next()%std::size(zones)],dirs[next()%std::size(dirs)],dmgs[next()%std::size(dmgs)]));
                            ctorDone(0x28);
                        } else {
                            setHit(next()%5!=0);
                            if(nm=="Destructor"){ if(next()%2)At<void*>(o,0x24)=Anim(0); callDtor(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="FinishAnimBeHitCB"){ callCb(Anim(1)); }
                            else if(nm=="MakeAbortable"){
                                if(next()%2)At<void*>(o,0x24)=Anim(0); At<uint16>(Anim(0),0x2E)=uint16(next());
                                static const int32 prios[]{0,0,1,1,2,2,3,7};
                                ret=callAbort(prios[next()%std::size(prios)],next()%3 ? event.data() : nullptr);
                            }
                            else {
                                if(nm=="ProcessPed"&&next()%2){ At<int32>(Ped(1),P_PEDTYPE)=next()%2; At<int32>(p0,P_PEDTYPE)=next()%3 ? 8 : 7; } // atacante jugador y victima de la banda 2
                                if(next()%2)At<int32>(o,0x20)=0;
                                if(nm=="StartAnim"){ reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,p0); }
                                else ret=callBool(p0);
                            }
                            out["object"]=Region(o,0x28,true); out["ped_rot"]={At<uint32>(p0,P_ROT_CUR),At<uint32>(p0,P_ROT_AIM)};
                            out["anim0"]=Region(Anim(0),sizeof(CAnimBlendAssociation));
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskSimpleBikeJacked") {
                        // +8 anim reproducida, +0xC primera anim, +0x10 segunda anim, +0x14 vehiculo (ref), +0x18 puerta, +0x1C tiempo,
                        // +0x20 secuestrador (ref), +0x24 victima conductor, +0x28 CTaskUtilityLineUpPedWithCar
                        static const int32 doors[]{8,10,8,10,9,11,0,18};
                        auto setBike=[&](bool veh,bool jacker){ setTask(VT_BIKE,0x2C); o[8]=uint8(next()%3==0); At<void*>(o,0xC)=next()%3==0 ? Anim(0) : nullptr;
                            { const uint32 m=next()%4; At<int32>(o,0x10)=m==0 ? ANIM_ID_NO_ANIMATION_SET : m==1 ? ANIM_ID_BIKE_FALL_OFF : m==2 ? ANIM_ID_BIKE_FALLR : int32(next()%300); }
                            At<CVehicle*>(o,0x14)=veh ? Veh(int(next()%2)) : nullptr; Reg(At<CVehicle*>(o,0x14),o+0x14);
                            At<int32>(o,0x18)=doors[next()%std::size(doors)]; At<uint32>(o,0x1C)=next()%5000;
                            At<CPed*>(o,0x20)=jacker ? Ped(1) : nullptr; Reg(At<CPed*>(o,0x20),o+0x20); o[0x24]=uint8(next()%2); At<void*>(o,0x28)=nullptr; };
                        if(nm=="Constructor"){
                            CVehicle* v=next()%4 ? Veh(0) : nullptr; CPed* j=next()%4 ? Ped(1) : nullptr;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32,uint32,CPed*,uint32)>(target)(o,v,doors[next()%std::size(doors)],next()%5000,j,next()%2));
                            ctorDone(0x2C);
                        } else {
                            setBike(next()%6!=0,next()%4!=0);
                            // CTaskUtilityLineUpPedWithCar previa en el heap (el destructor la libera)
                            if((nm=="Destructor"||nm=="ProcessPed")&&next()%3==0){
                                auto* l=new CTaskUtilityLineUpPedWithCar(CVector{uni(-2.f,2.f),uni(-2.f,2.f),uni(-2.f,2.f)},int32(next()%100),int32(next()%3),int32(next()%20));
                                lineupPre=uintptr_t(l); At<void*>(o,0x28)=l;
                            }
                            if(nm=="Destructor"){ callDtor(); lineupPre=0; if(At<uintptr_t>(o,0x28))lineupPre=At<uintptr_t>(o,0x28); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="FinishAnimBikeHitCB"){ callCb(Anim(1)); }
                            else if(nm=="MakeAbortable"){ ret=callAbort(int32(next()%4),next()%2 ? event.data() : nullptr); }
                            else {
                                inVeh(p0,next()%4!=0);
                                if(nm=="ProcessPed"&&next()%3==0)At<int32>(p0,P_PEDTYPE)=int32(next()%2); // jugador
                                ret=callBool(p0);
                                if(nm=="ProcessPed"){
                                    const auto l=At<uintptr_t>(o,0x28);
                                    if(l&&l!=lineupPre){ lineupNew=l; out["lineup"]=Region(reinterpret_cast<void*>(l),sizeof(CTaskUtilityLineUpPedWithCar)); }
                                }
                            }
                            out["object"]=Region(o,0x2C,true);
                            out["anims"]={Region(Anim(0),sizeof(CAnimBlendAssociation)),Region(Anim(1),sizeof(CAnimBlendAssociation))};
                            // limpieza de las CTaskUtilityLineUpPedWithCar en el heap (la previa solo si el destructor no la libero)
                            if(lineupNew)delete reinterpret_cast<CTaskUtilityLineUpPedWithCar*>(lineupNew);
                            if(lineupPre&&nm!="Destructor")delete reinterpret_cast<CTaskUtilityLineUpPedWithCar*>(lineupPre);
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
                    out["ret"]=ret; out["trace"]=trace; out["delete_trace"]=delTrace; out["rng"]=rngCalls; out["time"]=timeMs;
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
    const bool timeTouched=timeMs!=savedTime; timeMs=savedTime;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&Last(TaskHdr())==lf0&&timeMs==savedTime;
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"time_was_modified",timeTouched},{"state_restored",restored}});
}
}
