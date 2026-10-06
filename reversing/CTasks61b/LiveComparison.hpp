#pragma once
// Banco dentro del motor: 9 clases de tareas (68 funciones sin validar o recien reconstruidas):
// CTaskComplexWanderCriminal (constructor, destructor, Clone, GetWanderType, LookForCarsToSteal, ScanForStuff),
// CTaskComplexWanderProstitute (constructor, Clone, CreateFirstSubTask, GetWanderType, ScanForStuff),
// CTaskComplexEvasiveCower y CTaskComplexEvasiveStep (constructor, destructor, Clone, GetTaskType, CreateSubTask,
// CreateFirstSubTask, CreateNextSubTask, ControlSubTask), CTaskComplexBeInCouple (constructor, destructor, Clone, GetTaskType,
// AbortArmIK, MakeAbortable, CreateFirstSubTask, CreateNextSubTask, ControlSubTask), CTaskComplexSignalAtPed (constructor,
// destructor, Clone, GetTaskType, CreateFirstSubTask, CreateNextSubTask, ControlSubTask), CTaskComplexCrossRoadLookAndAchieveHeading
// y CTaskComplexDestroyCar (constructor, destructor, Clone, GetTaskType, CreateSubTask, MakeAbortable, CreateFirstSubTask,
// CreateNextSubTask, ControlSubTask) y CTaskSimpleChoking (constructor, destructor, Clone, GetTaskType, MakeAbortable, ProcessPed,
// UpdateChoke).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 3 peatones (matriz casi siempre, clump, inteligencia, datos de jugador, armas, vehiculo, tipo), 3 vehiculos (vtable falsa con
// trampas, handling propio), 4 tareas falsas (GetTaskType, MakeAbortable y destructor trazados; tambien hacen de tareas de la
// pareja, de caminar al lado y de esperar), 2 eventos falsos (tipo, prioridad y respuesta editable trazados) y 3 asociaciones de
// animacion falsas. Las funciones tocadas se simulan y trazan: RNG (EXE y DLL), CEventGroup::Add (los eventos de la pila se
// serializan por campos), CCarEnterExit::IsVehicleStealable, CTaskComplexProstituteSolicit::IsTaskValid, CTaskComplexWander::
// CreateFirstSubTask, IKChainManager (IsArmPointing, AbortPointArm, IsLooking, LookAt, PointArm), CPed (IsAlive, IsPlayer,
// StopPlayingHandSignal, SetCurrentWeapon, Say), CPedIntelligence::FindTaskByType, CTaskManager::FindActiveTaskByType,
// CWeapon::IsTypeMelee, CTaskComplexGangLeader::ShouldLoadGangAnims, GetAnimationBlockIndex, Add/RemoveAnimBlockRef,
// CStreaming::RequestModel, CAnimManager::BlendAnimation, CGeneral::GetRadianAngleBetweenPoints y LimitRadianAngle.
// Las subtareas que crean las funciones salen del pool real con el constructor que toque (EXE o DLL) y se serializan enteras.
// Pool de tareas: m_LastFreeSlot y estados restaurados y slots libres con 0xCD en cada ejecucion; lo nuevo se libera sin
// destructor. Se guardan y restauran el reloj, el paso de tiempo y los dos primeros bloques de animacion. Constructores sobre
// buffer con 0xCD comparado entero (tipo en vez de vptr).
#include <fstream>
#include <new>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Tasks/Task.h"
#include "Tasks/TaskTypes/TaskComplexWanderCriminal.h"
#include "Tasks/TaskTypes/TaskComplexWanderProstitute.h"
#include "Tasks/TaskTypes/TaskComplexEvasiveCower.h"
#include "Tasks/TaskTypes/TaskComplexEvasiveStep.h"
#include "Tasks/TaskTypes/TaskComplexBeInCouple.h"
#include "Tasks/TaskTypes/TaskComplexSignalAtPed.h"
#include "Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.h"
#include "Tasks/TaskTypes/TaskComplexDestroyCar.h"
#include "Tasks/TaskTypes/TaskSimpleChoking.h"
#include "Tasks/TaskTypes/TaskComplexProstituteSolicit.h"
#include "Tasks/TaskTypes/TaskComplexGangLeader.h"
#include "Ragdoll/IKChainManager.h"
#include "PedIntelligence.h"
#include "Animation/AnimManager.h"
#include "Events/EventGroup.h"
#include "Events/EventVehicleToSteal.h"
#include "CarEnterExit.h"
#include "Streaming.h"
#include "Weapon.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

#define CTASKS61B_COMMON_TAIL \
    const auto before=Snapshot(); \
    for(auto* h:hooks)h->State(candidate); \
    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address; \
    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate); \
    json out={{"switches",switches}}; json ret=nullptr; \
    uintptr_t retPtr{}; bool isPtr=false; auto Ptr=[&](const void* p)->json{ retPtr=uintptr_t(p); isPtr=true; return nullptr; }; \
    auto callTask=[&](CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped); }; \
    auto callVoid=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); };
#define CTASKS61B_FINISH \
    for(auto* h:hooks)h->State(false); \
    { auto* pool=GetTaskPool(); std::vector<CTask*> fresh; \
      for(size_t k=0;k<occT.size();++k)if(!occT[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);} \
      json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t)); out["tasks"]=tasks; } \
    if(isPtr)ret=Name(retPtr); \
    out["object"]=Region(o,objSize,true); \
    if(nm=="Constructor")out["type"]=reinterpret_cast<CTask*>(o)->GetTaskType(); \
    out["changes"]=Changes(before); out["refs"]=AllRefs(); \
    out["ret"]=ret; out["trace"]=trace; out["rng"]=rngCalls; \
    CleanRefs(); \
    FreeFresh(TaskHdr(),occT); \
    out["clean"]=Occ(TaskHdr())==occT; \
    restoreGlobals(); \
    return out;

namespace CTasks61bLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline json Vec(const CVector& v) { return json::array({F(v.x),F(v.y),F(v.z)}); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }
template<class M> uintptr_t MemberAddress(M m){ static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }
inline uintptr_t Body(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

constexpr int NPEDS=3, NVEH=3, FAKES=4, NASSOC=3, NEVS=2;
// tareas falsas: campos de control por encima de 0x60 (las tareas de la pareja y de caminar usan hasta +0x20)
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_TYPE=0x68, FK_CNT=0x70;
// desplazamientos crudos del EXE
constexpr size_t E_POS=4, E_MATRIX=0x14, E_RWOBJ=0x18, E_TYPE=0x36;                                                  // CEntity
constexpr size_t PH_SPEED=0x44;                                                                                      // CPhysical
constexpr size_t P_FLAGS=0x46C, P_INTEL=0x47C, P_PDATA=0x480, P_ANIMGROUP=0x4D4, P_ROT=0x55C, P_VEH=0x58C,
                 P_PEDTYPE=0x598, P_WEAPONS=0x5A0, P_ACTIVESLOT=0x718;                                               // CPed
constexpr size_t V_HANDLING=0x384, V_HEALTH=0x4C0;                                                                   // CVehicle
constexpr size_t H_MONEY=0xD8, H_SIZE=0x100;                                                                         // tHandlingData
constexpr size_t I_EVGROUP=0x68, I_VEHENTS=0xE0, I_PEDENTS=0x130;                                                    // CPedIntelligence
constexpr size_t PD_PROSTITUTE=0xA8, PD_SIZE=0xC0;                                                                   // CPlayerPedData
constexpr size_t A_BLEND=0x1C, A_SPEED=0x24, A_ANIMID=0x2C, A_FLAGS=0x2E, A_CBTYPE=0x30, A_CBFUNC=0x34, A_CBDATA=0x38, A_SIZE=0x60; // CAnimBlendAssociation
constexpr size_t EV_TYPE=0x30, EV_PRIO=0x34, EV_EDIT=0x38;
constexpr uintptr_t TIMER=0xB7CB84, TIMESTEP=0xB7CB5C, IKMAN=0xC15448, BLOCK0=0xB5D4B0, BLOCK1=0xB5D4D0;
constexpr uintptr_t VT_WC=0x85A23C, VT_WP=0x870148, VT_CW=0x86F3BC, VT_ES=0x86F218, VT_BIC=0x870724, VT_SAP=0x86FA8C, VT_CR=0x86DFF0,
                    VT_DC=0x86D968, VT_CH=0x86D820;
constexpr uint32 IN_VEH=0x100u;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,PD_SIZE>,NPEDS> pdatas{};
alignas(16) inline std::array<std::array<uint8,0x20>,NPEDS> clumps{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<std::array<uint8,H_SIZE>,NVEH> handlings{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,A_SIZE>,NASSOC> assocs{};
alignas(16) inline std::array<std::array<uint8,0x40>,NEVS> fevs{};
alignas(16) inline std::array<uint8,0x100> object{};
alignas(16) inline std::array<uint8,0x20> vecBuf{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }

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
inline uintptr_t dllLo{}, dllHi{}, cbDefaultDll{}, cbDefaultThunk{};
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(vehVtbl.data()))return "veh_vt"; if(v==uintptr_t(eventVtbl.data()))return "event_vt";
    if(v==IKMAN)return "ik_manager";
    // callbacks de animacion: el EXE usa 0x4D1390 (por defecto) y 0x620480 (fin del ahogo); el DLL sus propias funciones
    if(v==0x4D1390||(cbDefaultDll&&v==cbDefaultDll)||(cbDefaultThunk&&v==cbDefaultThunk))return "cb_default";
    if(v==0x620480||(v>=dllLo&&v<dllHi))return "cb_choke";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n);
                              if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n); if(auto s=in(pdatas[n].data(),pdatas[n].size(),"pdata"))return *s+"#"+std::to_string(n);
                              if(auto s=in(clumps[n].data(),clumps[n].size(),"clump"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n);
                             if(auto s=in(handlings[n].data(),handlings[n].size(),"hnd"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NASSOC;++n)if(auto s=in(assocs[n].data(),assocs[n].size(),"assoc"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NEVS;++n)if(auto s=in(fevs[n].data(),fevs[n].size(),"fevent"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(vecBuf.data(),vecBuf.size(),"vec"))return *s;
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
// eventos: los falsos por su buffer; los reales (de la pila) por tipo y solo los campos (sin relleno)
inline json SerEvent(const CEvent* e) {
    if(!e)return nullptr;
    for(int n=0;n<NEVS;++n)if(e==reinterpret_cast<const CEvent*>(fevs[n].data()))return json::array({"fake_event",n,At<uint8>(e,0xC)});
    const int type=const_cast<CEvent*>(e)->GetEventType();
    json r={type,At<int32>(e,4),At<uint8>(e,8)};
    switch(type){
    case EVENT_VEHICLE_TO_STEAL: r.push_back(Name(At<uint32>(e,0xC))); break;
    case EVENT_ACQUAINTANCE_PED_RESPECT: r.push_back(At<uint8>(e,0xC)); r.push_back(At<int16>(e,0xE)); r.push_back(At<int16>(e,0x10)); r.push_back(Name(At<uint32>(e,0x14))); break;
    default: break;
    }
    return r;
}

// buffers de fixtures cuyos cambios se comparan (palabra a palabra, traducidas)
struct Buf { std::string name; uint8* p; size_t n; };
inline std::vector<Buf> Buffers() {
    std::vector<Buf> b;
    for(int n=0;n<NPEDS;++n){ b.push_back({"ped#"+std::to_string(n),peds[n].data(),peds[n].size()}); b.push_back({"pmat#"+std::to_string(n),pedMats[n].data(),pedMats[n].size()});
                              b.push_back({"intel#"+std::to_string(n),intels[n].data(),intels[n].size()}); b.push_back({"pdata#"+std::to_string(n),pdatas[n].data(),pdatas[n].size()}); }
    for(int n=0;n<NVEH;++n){ b.push_back({"veh#"+std::to_string(n),vehs[n].data(),vehs[n].size()}); b.push_back({"vmat#"+std::to_string(n),vehMats[n].data(),vehMats[n].size()});
                             b.push_back({"hnd#"+std::to_string(n),handlings[n].data(),handlings[n].size()}); }
    for(int n=0;n<FAKES;++n)b.push_back({"fk#"+std::to_string(n),fakes[n].data(),fakes[n].size()});
    for(int n=0;n<NASSOC;++n)b.push_back({"assoc#"+std::to_string(n),assocs[n].data(),assocs[n].size()});
    for(int n=0;n<NEVS;++n)b.push_back({"fevent#"+std::to_string(n),fevs[n].data(),fevs[n].size()});
    b.push_back({"vec",vecBuf.data(),vecBuf.size()});
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
inline uint32 mSteal{}, cSteal{}, mValid{}, cValid{}, mAlive{}, cAlive{}, mPlayer{}, cPlayer{}, mMelee{}, cMelee{}, mPointing{}, cPointing{},
              mLooking{}, cLooking{}, mGang{}, cGang{}, assocNext{};
inline int32 blockIdxRet{};
inline uintptr_t baseFirstRet{};
// FindTaskByType: por inteligencia (peatones 0..2) y tipo (BE_IN_COUPLE, WALK_ALONGSIDE_PED, WAIT_FOR_PED); FindActiveTaskByType: BE_IN_COUPLE
inline std::array<std::array<uintptr_t,3>,NPEDS> findTask{};
inline std::array<uintptr_t,NPEDS> findActive{};

// tareas falsas: GetTaskType trazado; MakeAbortable segun una mascara por llamada (contador en el propio buffer)
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline int __fastcall FType(CTask* t,void*) { trace.push_back({"type",Name(t)}); return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) {
    const uint32 k=At<uint32>(t,FK_CNT)++; const bool r=(At<uint32>(t,FK_ABORT)>>(k&31))&1;
    trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e),r}); return r;
}
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<size_t S,int... N> void FillTraps(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// eventos falsos: tipo (slot 1), prioridad (slot 2) y respuesta editable (slot 9)
inline int32 __fastcall EType(CEvent* e,void*) { trace.push_back({"event_type",Name(e)}); return At<int32>(e,EV_TYPE); }
inline int32 __fastcall EPrio(CEvent* e,void*) { trace.push_back({"event_priority",Name(e)}); return At<int32>(e,EV_PRIO); }
inline bool __fastcall EEdit(CEvent* e,void*) { trace.push_back({"event_editable",Name(e)}); return At<uint8>(e,EV_EDIT)&1; }

inline bool Bit(uint32 mask,uint32& calls) { return (mask>>(calls++&31))&1; }
// dependencias simuladas
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline CEvent* __fastcall FakeEventAdd(void* g,void*,CEvent* e,uint32 valid) { trace.push_back({"event_group_add",Name(g),SerEvent(e),valid&0xFF}); return e; }
inline bool __cdecl FakeStealable(const CVehicle* v,const CPed* p) { const bool r=Bit(mSteal,cSteal); trace.push_back({"is_vehicle_stealable",Name(v),Name(p),r}); return r; }
inline bool __cdecl FakeTaskValid(CPed* a,CPed* b) { const bool r=Bit(mValid,cValid); trace.push_back({"is_task_valid",Name(a),Name(b),r}); return r; }
inline CTask* __fastcall FakeWanderFirst(void* t,void*,CPed* p) { trace.push_back({"wander_create_first",Name(t),Name(p),Name(baseFirstRet)}); return reinterpret_cast<CTask*>(baseFirstRet); }
inline bool __stdcall FakeIsArmPointing(int32 arm,CPed* p) { const bool r=Bit(mPointing,cPointing); trace.push_back({"is_arm_pointing",arm,Name(p),r}); return r; }
inline void __stdcall FakeAbortPointArm(int32 arm,CPed* p,int32 blend) { trace.push_back({"abort_point_arm",arm,Name(p),blend}); }
inline bool __fastcall FakeIsLooking(void* m,void*,CPed* p) { const bool r=Bit(mLooking,cLooking); trace.push_back({"is_looking",Name(m),Name(p),r}); return r; }
inline std::string Str(const char* s) { std::string r; for(int k=0;k<40&&s&&s[k];++k)r+=s[k]; return r; }
inline void __fastcall FakeLookAt(void* m,void*,const char* purpose,CPed* p,CEntity* e,int32 time,int32 bone,CVector* off,uint32 torso,float speed,int32 blend,int32 prio,uint32 force) {
    trace.push_back({"look_at",Name(m),Str(purpose),Name(p),Name(e),time,bone,Name(off),torso&0xFF,F(speed),blend,prio,force&0xFF});
}
inline void __fastcall FakePointArm(void* m,void*,const char* purpose,int32 arm,CPed* p,CEntity* e,int32 bone,CVector* off,float speed,int32 blend,float cull) {
    trace.push_back({"point_arm",Name(m),Str(purpose),arm,Name(p),Name(e),bone,off ? Vec(*off) : json(nullptr),F(speed),blend,F(cull)});
}
inline bool __fastcall FakeIsAlive(CPed* p,void*) { const bool r=Bit(mAlive,cAlive); trace.push_back({"is_alive",Name(p),r}); return r; }
inline bool __fastcall FakeIsPlayer(CPed* p,void*) { const bool r=Bit(mPlayer,cPlayer); trace.push_back({"is_player",Name(p),r}); return r; }
inline void __fastcall FakeStopHandSignal(CPed* p,void*) { trace.push_back({"stop_hand_signal",Name(p)}); }
inline void __fastcall FakeSetCurrentWeapon(CPed* p,void*,int32 slot) { trace.push_back({"set_current_weapon",Name(p),slot}); }
inline int16 __fastcall FakeSay(CPed* p,void*,int ctx,uint32 delay,float prob,uint32 overrideSilence,uint32 forceAudible,uint32 frontEnd) {
    trace.push_back({"say",Name(p),int16(ctx),delay,F(prob),overrideSilence&0xFF,forceAudible&0xFF,frontEnd&0xFF}); return -1;
}
inline int32 TypeSlot(int32 t) { return t==TASK_COMPLEX_BE_IN_COUPLE ? 0 : t==TASK_COMPLEX_WALK_ALONGSIDE_PED ? 1 : t==TASK_COMPLEX_WAIT_FOR_PED ? 2 : -1; }
inline int IntelIndex(const void* p) { for(int n=0;n<NPEDS;++n)if(p==intels[n].data())return n; return -1; }
inline CTask* __fastcall FakeFindTask(void* in,void*,int32 type) {
    const int n=IntelIndex(in); const int s=TypeSlot(type); const uintptr_t r=(n>=0&&s>=0) ? findTask[n][s] : 0;
    trace.push_back({"find_task_by_type",Name(in),type,Name(r)}); return reinterpret_cast<CTask*>(r);
}
inline CTask* __fastcall FakeFindActive(void* mgr,void*,int32 type) {
    const int n=IntelIndex(static_cast<uint8*>(mgr)-4); const uintptr_t r=(n>=0&&type==TASK_COMPLEX_BE_IN_COUPLE) ? findActive[n] : 0;
    trace.push_back({"find_active_task_by_type",Name(static_cast<uint8*>(mgr)-4),type,Name(r)}); return reinterpret_cast<CTask*>(r);
}
inline bool __fastcall FakeIsMelee(void* w,void*) { const bool r=Bit(mMelee,cMelee); trace.push_back({"is_type_melee",Name(w),r}); return r; }
inline bool __cdecl FakeShouldLoad() { const bool r=Bit(mGang,cGang); trace.push_back({"should_load_gang_anims",r}); return r; }
inline int32 __cdecl FakeBlockIndex(const char* nm) { trace.push_back({"anim_block_index",Str(nm),blockIdxRet}); return blockIdxRet; }
inline void __cdecl FakeAddBlockRef(int32 i) { trace.push_back({"add_anim_block_ref",i}); }
inline void __cdecl FakeRemoveBlockRef(int32 i) { trace.push_back({"remove_anim_block_ref",i}); }
inline void __cdecl FakeRequestModel(int32 m,int32 f) { trace.push_back({"request_model",m,f}); }
inline CAnimBlendAssociation* __cdecl FakeBlend(RpClump* c,int32 g,int32 a,float d) {
    auto* r=reinterpret_cast<CAnimBlendAssociation*>(assocs[1+(assocNext++%2)].data());
    trace.push_back({"blend_animation",Name(c),g,a,F(d),Name(r)}); return r;
}
inline float __cdecl FakeRadian(float x1,float y1,float x2,float y2) {
    const float r=x1*0.5f+y1*0.25f+x2*0.125f+y2*0.0625f; trace.push_back({"radian_angle",F(x1),F(y1),F(x2),F(y2),F(r)}); return r;
}
inline float __cdecl FakeLimit(float a) { const float r=a*0.75f-0.5f; trace.push_back({"limit_radian",F(a),F(r)}); return r; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=68;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexWanderCriminal","Clone",0,0x85A23C,1,256},
    {"CTaskComplexWanderCriminal","Constructor",0x48E610,0,-1,256},
    {"CTaskComplexWanderCriminal","Destructor",0x48E720,0,-1,256},
    {"CTaskComplexWanderCriminal","GetWanderType",0,0x85A23C,11,256},
    {"CTaskComplexWanderCriminal","LookForCarsToSteal",0x66B4F0,0,-1,1024},
    {"CTaskComplexWanderCriminal","ScanForStuff",0,0x85A23C,12,1024},
    {"CTaskComplexWanderProstitute","Clone",0,0x870148,1,256},
    {"CTaskComplexWanderProstitute","Constructor",0x672690,0,-1,256},
    {"CTaskComplexWanderProstitute","CreateFirstSubTask",0,0x870148,9,256},
    {"CTaskComplexWanderProstitute","GetWanderType",0,0x870148,11,256},
    {"CTaskComplexWanderProstitute","ScanForStuff",0,0x870148,12,1024},
    {"CTaskComplexEvasiveCower","Clone",0,0x86F3BC,1,256},
    {"CTaskComplexEvasiveCower","Constructor",0x655460,0,-1,256},
    {"CTaskComplexEvasiveCower","ControlSubTask",0,0x86F3BC,10,256},
    {"CTaskComplexEvasiveCower","CreateFirstSubTask",0,0x86F3BC,9,256},
    {"CTaskComplexEvasiveCower","CreateNextSubTask",0,0x86F3BC,8,512},
    {"CTaskComplexEvasiveCower","CreateSubTask",0x655570,0,-1,512},
    {"CTaskComplexEvasiveCower","Destructor",0x6554E0,0,-1,256},
    {"CTaskComplexEvasiveCower","GetTaskType",0,0x86F3BC,4,256},
    {"CTaskComplexEvasiveStep","Clone",0,0x86F218,1,256},
    {"CTaskComplexEvasiveStep","Constructor",0x6532D0,0,-1,256},
    {"CTaskComplexEvasiveStep","ControlSubTask",0,0x86F218,10,256},
    {"CTaskComplexEvasiveStep","CreateFirstSubTask",0,0x86F218,9,256},
    {"CTaskComplexEvasiveStep","CreateNextSubTask",0,0x86F218,8,512},
    {"CTaskComplexEvasiveStep","CreateSubTask",0x6533E0,0,-1,512},
    {"CTaskComplexEvasiveStep","Destructor",0x653350,0,-1,256},
    {"CTaskComplexEvasiveStep","GetTaskType",0,0x86F218,4,256},
    {"CTaskComplexBeInCouple","AbortArmIK",0x6837F0,0,-1,256},
    {"CTaskComplexBeInCouple","Clone",0,0x870724,1,256},
    {"CTaskComplexBeInCouple","Constructor",0x6836F0,0,-1,256},
    {"CTaskComplexBeInCouple","ControlSubTask",0,0x870724,10,2048},
    {"CTaskComplexBeInCouple","CreateFirstSubTask",0,0x870724,9,256},
    {"CTaskComplexBeInCouple","CreateNextSubTask",0,0x870724,8,512},
    {"CTaskComplexBeInCouple","Destructor",0x683780,0,-1,256},
    {"CTaskComplexBeInCouple","GetTaskType",0,0x870724,4,256},
    {"CTaskComplexBeInCouple","MakeAbortable",0,0x870724,6,512},
    {"CTaskComplexSignalAtPed","Clone",0,0x86FA8C,1,256},
    {"CTaskComplexSignalAtPed","Constructor",0x660A30,0,-1,256},
    {"CTaskComplexSignalAtPed","ControlSubTask",0,0x86FA8C,10,1024},
    {"CTaskComplexSignalAtPed","CreateFirstSubTask",0,0x86FA8C,9,512},
    {"CTaskComplexSignalAtPed","CreateNextSubTask",0,0x86FA8C,8,512},
    {"CTaskComplexSignalAtPed","Destructor",0x660AB0,0,-1,512},
    {"CTaskComplexSignalAtPed","GetTaskType",0,0x86FA8C,4,256},
    {"CTaskComplexCrossRoadLookAndAchieveHeading","Clone",0,0x86DFF0,1,256},
    {"CTaskComplexCrossRoadLookAndAchieveHeading","Constructor",0x631AE0,0,-1,256},
    {"CTaskComplexCrossRoadLookAndAchieveHeading","ControlSubTask",0,0x86DFF0,10,256},
    {"CTaskComplexCrossRoadLookAndAchieveHeading","CreateFirstSubTask",0,0x86DFF0,9,256},
    {"CTaskComplexCrossRoadLookAndAchieveHeading","CreateNextSubTask",0,0x86DFF0,8,512},
    {"CTaskComplexCrossRoadLookAndAchieveHeading","CreateSubTask",0x631B60,0,-1,512},
    {"CTaskComplexCrossRoadLookAndAchieveHeading","Destructor",0x631B50,0,-1,256},
    {"CTaskComplexCrossRoadLookAndAchieveHeading","GetTaskType",0,0x86DFF0,4,256},
    {"CTaskComplexCrossRoadLookAndAchieveHeading","MakeAbortable",0,0x86DFF0,6,512},
    {"CTaskComplexDestroyCar","Clone",0,0x86D968,1,256},
    {"CTaskComplexDestroyCar","Constructor",0x621C00,0,-1,256},
    {"CTaskComplexDestroyCar","ControlSubTask",0,0x86D968,10,2048},
    {"CTaskComplexDestroyCar","CreateFirstSubTask",0,0x86D968,9,512},
    {"CTaskComplexDestroyCar","CreateNextSubTask",0,0x86D968,8,512},
    {"CTaskComplexDestroyCar","CreateSubTask",0x6287A0,0,-1,1024},
    {"CTaskComplexDestroyCar","Destructor",0x621CB0,0,-1,256},
    {"CTaskComplexDestroyCar","GetTaskType",0,0x86D968,4,256},
    {"CTaskComplexDestroyCar","MakeAbortable",0,0x86D968,6,512},
    {"CTaskSimpleChoking","Clone",0,0x86D820,1,256},
    {"CTaskSimpleChoking","Constructor",0x6202C0,0,-1,256},
    {"CTaskSimpleChoking","Destructor",0x620370,0,-1,512},
    {"CTaskSimpleChoking","GetTaskType",0,0x86D820,4,256},
    {"CTaskSimpleChoking","MakeAbortable",0,0x86D820,6,1024},
    {"CTaskSimpleChoking","ProcessPed",0,0x86D820,7,2048},
    {"CTaskSimpleChoking","UpdateChoke",0x620660,0,-1,1024},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks61b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks61b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks61b");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks61b-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // rango de la imagen del DLL (los callbacks de animacion del DLL se nombran igual que los del EXE) y callback por defecto del DLL
    { HMODULE self{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&CTasks61bLive::FakeRand),&self);
      const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(self); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(reinterpret_cast<const uint8*>(self)+dos->e_lfanew);
      dllLo=uintptr_t(self); dllHi=dllLo+nt->OptionalHeader.SizeOfImage;
      cbDefaultThunk=std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB); cbDefaultDll=Body(cbDefaultThunk); }
    // cuerpo del DLL de CTaskComplexWander::CreateFirstSubTask (el DLL lo llama directo desde CTaskComplexWanderProstitute): hueco 9 de la vtable
    // del DLL de una tarea concreta que no lo sobrescribe
    uintptr_t wanderFirstBody{};
    { alignas(16) uint8 buf[0x80]{}; auto* t=::new(static_cast<void*>(buf)) CTaskComplexWanderCriminal(PEDMOVE_WALK,0,true);
      wanderFirstBody=Body(reinterpret_cast<const uintptr_t*>(*reinterpret_cast<const uintptr_t*>(t))[9]); }
    FillTraps(fakeVtbl,std::make_integer_sequence<int,16>{}); FillTraps(vehVtbl,std::make_integer_sequence<int,96>{}); FillTraps(eventVtbl,std::make_integer_sequence<int,20>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    eventVtbl[1]=reinterpret_cast<void*>(&EType); eventVtbl[2]=reinterpret_cast<void*>(&EPrio); eventVtbl[9]=reinterpret_cast<void*>(&EEdit);
    const auto occTaskStart=Occ(TaskHdr());
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    const int32 lt0=Last(TaskHdr());
    const std::vector<uint8> stTask(States(TaskHdr()),States(TaskHdr())+Cap(TaskHdr()));
    auto resetPools=[&]{ ResetPool(TaskHdr(),TASK_SLOT,lt0,stTask,occTaskStart); };
    // estado global tocado: reloj, paso de tiempo y el byte "cargado" de los dos primeros bloques de animacion; se restauran en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{TIMESTEP,4},{BLOCK0,1},{BLOCK1,1}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(Body(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber)),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch q1(Body(MemberAddress(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add))),FakeEventAdd), q1e(0x4AB420,FakeEventAdd);
        EntryPatch c1(Body(bit_cast<uintptr_t>(&CCarEnterExit::IsVehicleStealable)),FakeStealable), c1e(0x6510D0,FakeStealable);
        EntryPatch c2(Body(bit_cast<uintptr_t>(&CTaskComplexProstituteSolicit::IsTaskValid)),FakeTaskValid), c2e(0x661BB0,FakeTaskValid);
        EntryPatch w1(wanderFirstBody,FakeWanderFirst), w1e(0x6740E0,FakeWanderFirst);
        EntryPatch k1(Body(bit_cast<uintptr_t>(&IKChainManager_c::IsArmPointing)),FakeIsArmPointing), k1e(0x6182B0,FakeIsArmPointing);
        EntryPatch k2(Body(bit_cast<uintptr_t>(&IKChainManager_c::AbortPointArm)),FakeAbortPointArm), k2e(0x6182F0,FakeAbortPointArm);
        EntryPatch k3(Body(MemberAddress(&IKChainManager_c::IsLooking)),FakeIsLooking), k3e(0x6181A0,FakeIsLooking);
        EntryPatch k4(Body(MemberAddress(&IKChainManager_c::LookAt)),FakeLookAt), k4e(0x618970,FakeLookAt);
        EntryPatch k5(Body(MemberAddress(&IKChainManager_c::PointArm)),FakePointArm), k5e(0x618B60,FakePointArm);
        EntryPatch p1(Body(MemberAddress(&CPed::IsAlive)),FakeIsAlive), p1e(0x5E0170,FakeIsAlive);
        EntryPatch p2(Body(MemberAddress(&CPed::IsPlayer)),FakeIsPlayer), p2e(0x5DF8F0,FakeIsPlayer);
        EntryPatch p3(Body(MemberAddress(&CPed::StopPlayingHandSignal)),FakeStopHandSignal), p3e(0x5E0480,FakeStopHandSignal);
        EntryPatch p4(Body(MemberAddress(static_cast<void(CPed::*)(int32)>(&CPed::SetCurrentWeapon))),FakeSetCurrentWeapon), p4e(0x5E61F0,FakeSetCurrentWeapon);
        EntryPatch p5(Body(MemberAddress(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say))),FakeSay), p5e(0x5EFFE0,FakeSay);
        EntryPatch i1(Body(MemberAddress(&CPedIntelligence::FindTaskByType)),FakeFindTask), i1e(0x600EE0,FakeFindTask);
        EntryPatch i2(Body(MemberAddress(&CTaskManager::FindActiveTaskByType)),FakeFindActive), i2e(0x681740,FakeFindActive);
        EntryPatch m1(Body(MemberAddress(&CWeapon::IsTypeMelee)),FakeIsMelee), m1e(0x73B1C0,FakeIsMelee);
        EntryPatch g1(Body(bit_cast<uintptr_t>(&CTaskComplexGangLeader::ShouldLoadGangAnims)),FakeShouldLoad), g1e(0x65E7F0,FakeShouldLoad);
        EntryPatch a1(Body(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex))),FakeBlockIndex), a1e(0x4D3990,FakeBlockIndex);
        EntryPatch a2(Body(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef)),FakeAddBlockRef), a2e(0x4D3FB0,FakeAddBlockRef);
        EntryPatch a3(Body(bit_cast<uintptr_t>(&CAnimManager::RemoveAnimBlockRef)),FakeRemoveBlockRef), a3e(0x4D3FD0,FakeRemoveBlockRef);
        EntryPatch a4(Body(bit_cast<uintptr_t>(&CStreaming::RequestModel)),FakeRequestModel), a4e(0x4087E0,FakeRequestModel);
        EntryPatch a5(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation))),FakeBlend), a5e(0x4D4610,FakeBlend);
        EntryPatch n1(Body(bit_cast<uintptr_t>(static_cast<float(*)(float,float,float,float)>(&CGeneral::GetRadianAngleBetweenPoints))),FakeRadian), n1e(0x53CBE0,FakeRadian);
        EntryPatch n2(Body(bit_cast<uintptr_t>(&CGeneral::LimitRadianAngle)),FakeLimit), n2e(0x53CB50,FakeLimit);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                const float qnan=std::numeric_limits<float>::quiet_NaN();
                auto run=[&](bool candidate)->json {
                    resetPools();
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x6161u; if(!st)st=1; trace=json::array(); seed=uint32(i);
                    rngCalls=cSteal=cValid=cAlive=cPlayer=cMelee=cPointing=cLooking=cGang=assocNext=0; freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    // reloj: normal casi siempre; cerca del desbordamiento en el barrido del criminal y la prostituta solo donde no rompe nada
                    const bool nearWrap=next()%8==0 && cls!="CTaskComplexWanderProstitute";
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=nearWrap ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    const uint32 now=At<uint32>(reinterpret_cast<void*>(TIMER),0);
                    { static const float ts[]{0.f,0.016f,1.f,0.5f,3.f,50.f,1000.f,-2.f,1e-5f,2.5f}; const uint32 m=next()%12;
                      At<float>(reinterpret_cast<void*>(TIMESTEP),0)= m<10 ? ts[m] : m==10 ? qnan : uni(0.f,5.f); }
                    mSteal=next(); mValid=next(); mAlive=next(); mPlayer=next(); mMelee=next(); mPointing=next(); mLooking=next(); mGang=next();
                    blockIdxRet=int32(next()%2); baseFirstRet=0;
                    for(auto& t:findTask)t={0,0,0}; findActive.fill(0);
                    // posicion de un peatón / vehiculo: la matriz si tiene, si no el placement
                    auto pedPos=[&](int n)->CVector&{ return At<void*>(peds[n].data(),E_MATRIX) ? At<CVector>(pedMats[n].data(),0x30) : At<CVector>(peds[n].data(),E_POS); };
                    auto vehPos=[&](int n)->CVector&{ return At<void*>(vehs[n].data(),E_MATRIX) ? At<CVector>(vehMats[n].data(),0x30) : At<CVector>(vehs[n].data(),E_POS); };
                    auto ensureMat=[&](int n){ if(!At<void*>(peds[n].data(),E_MATRIX)){ const CVector pp=At<CVector>(peds[n].data(),E_POS); At<void*>(peds[n].data(),E_MATRIX)=pedMats[n].data(); At<CVector>(pedMats[n].data(),0x30)=pp; } };
                    // peatones: matriz (casi siempre), inteligencia, clump, datos de jugador, tipo, flags, armas, vehiculo
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                        std::memset(pdatas[n].data(),0,pdatas[n].size()); std::memset(clumps[n].data(),0,clumps[n].size()); clumps[n][0]=2;
                        p[E_TYPE]=3; At<void*>(p,P_INTEL)=intels[n].data(); At<void*>(p,E_RWOBJ)=next()%6 ? (void*)clumps[n].data() : nullptr;
                        if(next()%6){ At<void*>(p,E_MATRIX)=pedMats[n].data();
                            for(int r=0;r<3;++r)At<CVector>(pedMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            At<CVector>(pedMats[n].data(),0x30)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)}; }
                        else At<CVector>(p,E_POS)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)};
                        { static const int32 types[]{0,1,4,5,6,6,7,8,9,10,11,18,19}; At<int32>(p,P_PEDTYPE)=types[next()%std::size(types)]; }
                        if(next()%2)At<uint32>(p,P_FLAGS)|=IN_VEH; At<uint32>(p,P_FLAGS)|=next()&0xFFFFFE00u&~IN_VEH&0x7FFFFFFFu;
                        { const uint32 m=next()%4; At<CVehicle*>(p,P_VEH)=m==0 ? nullptr : Veh(int(next()%NVEH)); }
                        { static const int32 grp[]{0,1,13,14,17,18,19,22,23,28,2,3,12,16,20,24,29,40,0x7F}; At<int32>(p,P_ANIMGROUP)=next()%4==0 ? int32(next()%64) : grp[next()%std::size(grp)]; }
                        At<float>(p,P_ROT)=uni(-3.2f,3.2f);
                        p[P_ACTIVESLOT]=uint8(next()%13);
                        for(int w=0;w<13;++w){ auto* wp=p+P_WEAPONS+0x1C*w; At<int32>(wp,0)=int32(next()%48);
                            { static const uint32 ammo[]{0u,1u,0xFFFFFFFFu,0x7FFFFFFFu,0x80000000u,50u,500u}; At<uint32>(wp,0xC)=next()%3==0 ? 0u : next()%4==0 ? next() : ammo[next()%std::size(ammo)]; } }
                        if(next()%3){ At<void*>(p,P_PDATA)=pdatas[n].data(); At<CPed*>(pdatas[n].data(),PD_PROSTITUTE)=next()%3==0 ? Ped(int(next()%NPEDS)) : nullptr; }
                    }
                    // vehiculos: vtable con trampas, matriz, velocidad, handling propio y salud
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%8)<<3);
                        if(next()%6){ At<void*>(v,E_MATRIX)=vehMats[n].data();
                            At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)}; }
                        else At<CVector>(v,E_POS)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)};
                        { const uint32 m=next()%4; const float s=m==0 ? 0.01f : m==1 ? 0.3f : m==2 ? 0.f : uni(0.f,0.2f); At<CVector>(v,PH_SPEED)=CVector{s*uni(-1.f,1.f),s*uni(-1.f,1.f),s*uni(-1.f,1.f)}; }
                        for(auto& b:handlings[n])b=uint8(next());
                        { static const uint32 money[]{0u,1u,100u,5000u,25000u,49999u,50000u,0xFFFFFFFFu}; At<uint32>(handlings[n].data(),H_MONEY)=next()%2 ? money[next()%std::size(money)] : next()%60000u; }
                        At<void*>(v,V_HANDLING)=handlings[n].data();
                        { static const float hp[]{0.f,-1.f,1000.f,500.f,1e-30f,0.001f,-0.f}; const uint32 m=next()%9; At<float>(v,V_HEALTH)= m<7 ? hp[m] : m==7 ? qnan : uni(1.f,1000.f); }
                    }
                    // tareas falsas
                    for(int n=0;n<FAKES;++n){ auto* k=fakes[n].data(); for(size_t b=0;b<FAKE_SIZE;++b)k[b]=uint8(next());
                        At<void*>(k,0)=fakeVtbl.data(); At<uint32>(k,4)=0; At<uint32>(k,8)=0;
                        At<int32>(k,FK_TYPE)=TASK_SIMPLE_PAUSE; At<uint32>(k,FK_ABORT)=next(); At<uint32>(k,FK_CNT)=0; }
                    // asociaciones de animacion falsas: id y banderas
                    for(auto& a:assocs){ for(auto& b:a)b=uint8(next()); At<float>(a.data(),A_BLEND)=uni(-8.f,8.f); At<float>(a.data(),A_SPEED)=uni(0.f,2.f);
                        { static const int16 ids[]{21,10,0,5,21,21}; At<int16>(a.data(),A_ANIMID)=next()%5==0 ? int16(next()) : ids[next()%std::size(ids)]; } }
                    // eventos falsos
                    for(auto& e:fevs){ for(auto& b:e)b=uint8(next()); At<void*>(e.data(),0)=eventVtbl.data(); At<uint32>(e.data(),4)=0; At<uint32>(e.data(),8)=0;
                        e[0xC]=uint8(next()%2); { static const int32 ty[]{9,31,15,1,39,28,0x100,0,10,30,16}; At<int32>(e.data(),EV_TYPE)=ty[next()%std::size(ty)]; }
                        { static const int32 pr[]{56,57,58,0,100,-1,7,30}; At<int32>(e.data(),EV_PRIO)=next()%5==0 ? int32(next()%120) : pr[next()%std::size(pr)]; }
                        e[EV_EDIT]=uint8(next()%4!=0); }
                    std::memset(vecBuf.data(),0,vecBuf.size());
                    std::memset(object.data(),0xCD,object.size());

                    auto* o=object.data(); auto* p0=Ped(0); auto* p0b=peds[0].data();
                    auto setObj=[&](uintptr_t vtbl,size_t size){ for(size_t b=0;b<size;++b)o[b]=uint8(next()); At<uintptr_t>(o,0)=vtbl; At<uint32>(o,4)=0; At<uint32>(o,8)=0; };
                    auto occT=Occ(TaskHdr());
                    auto prio=[&]{ return int32(next()%4); };
                    auto subType=[&](std::initializer_list<int32> ty){ At<CTask*>(o,8)=Fake(0); At<int32>(fakes[0].data(),FK_TYPE)=*(ty.begin()+next()%ty.size()); };
                    auto entity=[&]()->void*{ const uint32 m=next()%5; return m==0 ? nullptr : m==1 ? (void*)Veh(int(next()%NVEH)) : (void*)Ped(int(1+next()%2)); };
                    size_t objSize=0x40;

                    // argumentos preparados antes de la instantanea
                    int32 a0{}, a1{}, a2{}; float f0{}; uint32 b0{}; void* q0{}; const CEvent* ev{};
                    // escaner de vehiculos del peatón 0 para el criminal (ninguno, uno o varios) y alrededor del peatón
                    auto vehScanner=[&]{
                        auto* in=intels[0].data();
                        const uint32 m=next()%4; const int cnt=m==0 ? 0 : m==1 ? 1 : int(2+next()%6);
                        for(int k=0;k<cnt;++k)At<CVehicle*>(in,I_VEHENTS+4*int(next()%16))=Veh(int(next()%NVEH));
                        if(next()%2)At<CVehicle*>(in,I_VEHENTS)=Veh(int(next()%NVEH));
                        if(next()%2)At<CVehicle*>(in,I_VEHENTS+4*15)=Veh(int(next()%NVEH));
                        const CVector pp=pedPos(0); for(int n=0;n<NVEH;++n){ if(next()%2)vehPos(n)=CVector{pp.x+uni(-30.f,30.f),pp.y+uni(-30.f,30.f),pp.z+uni(-6.f,6.f)}; }
                        if(next()%6==0){ vehPos(0)=pedPos(0); vehPos(1)=pedPos(0); } // distancias iguales
                    };
                    // escaner de peatones del peatón 0 para la prostituta
                    auto pedScanner=[&]{
                        auto* in=intels[0].data();
                        for(int s=0;s<16;++s)At<CPed*>(in,I_PEDENTS+4*s)=next()%3 ? nullptr : Ped(int(next()%NPEDS));
                        if(next()%4==0)At<CPed*>(in,I_PEDENTS+4*15)=Ped(int(1+next()%2));
                        // 1 de cada 4: ningun policia entre ellos; los demas pueden tenerlo
                        if(next()%4)for(int n=0;n<NPEDS;++n)if(At<int32>(peds[n].data(),P_PEDTYPE)==6)At<int32>(peds[n].data(),P_PEDTYPE)=4;
                        // velocidad pequeña (alrededor de 0.04 de modulo^2) y datos de jugador / vehiculo en la mayoria
                        for(int n=0;n<NPEDS;++n){ if(next()%4){ At<void*>(peds[n].data(),P_PDATA)=pdatas[n].data(); }
                            if(next()%5)At<CVehicle*>(peds[n].data(),P_VEH)=Veh(int(next()%NVEH)); }
                    };

                    if(cls=="CTaskComplexWanderCriminal"||cls=="CTaskComplexWanderProstitute") {
                        const bool crim=cls=="CTaskComplexWanderCriminal"; objSize=crim ? 0x38 : 0x3C;
                        auto setWander=[&]{ setObj(crim ? VT_WC : VT_WP,objSize); At<uint32>(o,0xC)=next()%8; o[0x10]=uint8(next()); o[0x24]=uint8(next());
                            // temporizador (0x28 inicio, 0x2C intervalo, 0x30 en marcha, 0x31 parado) y 0x34 siguiente exploracion
                            At<uint32>(o,0x28)=next()%2 ? now-next()%300 : now-next()%100000;
                            { static const int32 iv[]{50,0,-1,100,49,51,0x7FFFFFFF,1000}; At<int32>(o,0x2C)=next()%3==0 ? int32(next()%2000) : iv[next()%std::size(iv)]; }
                            o[0x30]=uint8(next()%4!=0); o[0x31]=uint8(next()%4==0);
                            At<uint32>(o,0x34)=crim ? (next()%2 ? now-next()%5000 : now+next()%5000) : now+1+next()%100000;
                            if(!crim)At<uint32>(o,0x38)=next()%4==0 ? now-1 : next()%2 ? now : next()%2 ? now-next()%5000 : now+next()%5000; };
                        a0=int32(next()%8); a1=int32(next()%256); b0=next()%2;
                        if(nm=="ScanForStuff"||nm=="LookForCarsToSteal"||nm=="CreateFirstSubTask"||nm=="Clone"||nm=="Destructor"||nm=="GetWanderType")setWander();
                        if(nm=="ScanForStuff"&&crim){
                            // 0 sin arrancar (primera llamada), 1 vencido y con la exploracion cumplida, 2 vencido sin cumplir, 3 sin vencer, 4 parado
                            switch(next()%5){
                            case 0: o[0x30]=0; break;
                            case 1: o[0x30]=1; o[0x31]=0; At<uint32>(o,0x28)=now-1000; At<int32>(o,0x2C)=50; At<uint32>(o,0x34)=now-next()%3; break;
                            case 2: o[0x30]=1; o[0x31]=0; At<uint32>(o,0x28)=now-1000; At<int32>(o,0x2C)=50; At<uint32>(o,0x34)=now+1+next()%3000; break;
                            case 3: o[0x30]=1; o[0x31]=0; At<uint32>(o,0x28)=now; At<int32>(o,0x2C)=50+int32(next()%1000); At<uint32>(o,0x34)=now-5; break;
                            default: o[0x30]=1; o[0x31]=1; At<uint32>(o,0x28)=now-1000; At<int32>(o,0x2C)=50; At<uint32>(o,0x34)=now-next()%3; break;
                            }
                            vehScanner();
                            { static const uint32 sm[]{0xFFFFFFFFu,0xFFFFFFFFu,0u,0x55555555u,0xAAAAAAAAu}; mSteal=next()%3 ? sm[next()%std::size(sm)] : next(); }
                        }
                        if(nm=="LookForCarsToSteal"){ vehScanner(); { static const uint32 sm[]{0xFFFFFFFFu,0xFFFFFFFFu,0u,0x55555555u,0xAAAAAAAAu}; mSteal=next()%3 ? sm[next()%std::size(sm)] : next(); } }
                        if(nm=="ScanForStuff"&&!crim){
                            pedScanner(); mValid=next()%3 ? 0xFFFFFFFFu : next();
                            // la parte de CTaskComplexWanderStandard sale pronto (0x34 en el futuro); el tiempo de la prostituta alrededor del actual
                            At<uint32>(o,0x34)=now+1+next()%100000;
                            { const uint32 m=next()%5; At<uint32>(o,0x38)= m==0 ? now : m==1 ? now-1 : m==2 ? now+1 : now-next()%5000; }
                            // el precio aleatorio < valor del vehiculo: valores de handling bajos y altos; rand estable
                        }
                        if(nm=="CreateFirstSubTask"){ baseFirstRet=next()%3 ? uintptr_t(Fake(int(next()%FAKES))) : 0; }
                        CTASKS61B_COMMON_TAIL
                        if(nm=="Clone")ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                        else if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,uint32,uint32,uint32)>(target)(o,uint32(a0),uint32(a1),b0));
                        else if(nm=="Destructor")callVoid();
                        else if(nm=="GetWanderType")ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        else if(nm=="LookForCarsToSteal"||nm=="ScanForStuff")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,p0);
                        else if(nm=="CreateFirstSubTask")ret=Ptr(callTask(p0));
                        CTASKS61B_FINISH
                    }
                    if(cls=="CTaskComplexEvasiveCower"||cls=="CTaskComplexEvasiveStep") {
                        const bool cower=cls=="CTaskComplexEvasiveCower"; objSize=0x1C;
                        auto setCS=[&]{ setObj(cower ? VT_CW : VT_ES,objSize); At<CTask*>(o,8)=Fake(0);
                            for(int k=0;k<3;++k)At<float>(o,0xC+4*k)=next()%6==0 ? 0.f : uni(-50.f,50.f); At<void*>(o,0x18)=entity(); };
                        // el vector pasado al constructor
                        At<CVector>(vecBuf.data(),0x10)=CVector{next()%6==0 ? 0.f : uni(-50.f,50.f),next()%6==0 ? 0.f : uni(-50.f,50.f),uni(-5.f,5.f)};
                        q0=entity();
                        static const int32 tyAny[]{TASK_SIMPLE_COWER,TASK_SIMPLE_EVASIVE_STEP,TASK_SIMPLE_ACHIEVE_HEADING,TASK_FINISHED,TASK_NONE,1000,TASK_SIMPLE_PAUSE};
                        if(nm!="Constructor"){ setCS();
                            if(nm=="CreateNextSubTask")subType({TASK_SIMPLE_COWER,TASK_SIMPLE_EVASIVE_STEP,TASK_SIMPLE_ACHIEVE_HEADING,TASK_SIMPLE_PAUSE,TASK_FINISHED,TASK_SIMPLE_ACHIEVE_HEADING});
                            if(nm=="CreateSubTask")a0=tyAny[next()%std::size(tyAny)];
                        }
                        CTASKS61B_COMMON_TAIL
                        if(nm=="Clone")ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                        else if(nm=="GetTaskType")ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        else if(nm=="Destructor")callVoid();
                        else if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,CEntity*,const CVector*)>(target)(o,static_cast<CEntity*>(q0),&At<CVector>(vecBuf.data(),0x10)));
                        else if(nm=="CreateSubTask")ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,int32)>(target)(o,a0));
                        else ret=Ptr(callTask(p0)); // ControlSubTask, CreateFirstSubTask, CreateNextSubTask
                        CTASKS61B_FINISH
                    }
                    if(cls=="CTaskComplexBeInCouple") {
                        objSize=0x20;
                        auto setBic=[&]{ setObj(VT_BIC,objSize); At<CTask*>(o,8)=Fake(0); At<uint32>(o,0xC)=next();
                            { const uint32 m=next()%12; At<CPed*>(o,0x10)= m==0 ? nullptr : m==1 ? Ped(2) : Ped(1); }
                            o[0x14]=uint8(next()%2); o[0x15]=uint8(next()%4!=0); o[0x16]=uint8(next()%4!=0);
                            { static const float g[]{10.f,5.f,2.f,0.5f,100.f,0.f,-1.f}; const uint32 m=next()%9; At<float>(o,0x18)= m<7 ? g[m] : m==7 ? qnan : uni(0.f,15.f); }
                            o[0x1C]=uint8(next()%4==0 ? next() : next()%3); };
                        if(nm=="Constructor"){ q0=next()%6==0 ? nullptr : (void*)Ped(int(1+next()%2)); a0=int32(next()%2); a1=int32(next()%2); a2=int32(next()%2); f0=next()%3==0 ? 10.f : uni(0.f,30.f); }
                        else {
                            setBic();
                            if(nm=="MakeAbortable"){ a0=prio(); ev=next()%4==0 ? nullptr : reinterpret_cast<const CEvent*>(fevs[next()%NEVS].data()); }
                            if(nm=="ControlSubTask"){
                                // pareja viva casi siempre, con su tarea del papel contrario que apunta a este peatón, y tareas activas
                                const bool good=next()%4!=0;
                                if(good)At<CPed*>(o,0x10)=Ped(1);
                                if(next()%8==0)At<CPed*>(o,0x10)=next()%2 ? nullptr : Ped(2);
                                mAlive=next()%7 ? 0xFFFFFFFFu : 0u;
                                { auto* pt=fakes[1].data(); At<CPed*>(pt,0x10)= good||next()%3 ? p0 : Ped(2); pt[0x14]=uint8(good||next()%3 ? (o[0x14] ? 0 : 1) : next()%2); }
                                findTask[1][0]=next()%8 ? uintptr_t(Fake(1)) : 0; findTask[0][1]=next()%3 ? uintptr_t(Fake(2)) : 0; findTask[0][2]=next()%3 ? 0 : uintptr_t(Fake(3));
                                findActive[0]=next()%8 ? uintptr_t(Fake(3)) : 0; findActive[1]=next()%8 ? uintptr_t(Fake(3)) : 0;
                                ensureMat(1);
                                for(int r=0;r<3;++r)At<CVector>(pedMats[1].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                                // distancia entre los dos: bajo, entre 1.5 y 2, empates exactos (1.5, 2 y 3-4-5 con 5) y lejos
                                const CVector pp{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,5.f)}; float ddx{},ddy{};
                                switch(next()%9){
                                case 0: ddx=1.5f; ddy=0.f; break;
                                case 1: ddx=0.f; ddy=-2.f; break;
                                case 2: ddx=3.f; ddy=4.f; At<float>(o,0x18)=5.f; break;
                                case 3: ddx=uni(-1.f,1.f); ddy=uni(-1.f,1.f); break;
                                case 4: { const float r=uni(1.4f,2.6f),a=uni(0.f,6.28f); ddx=r*std::cos(a); ddy=r*std::sin(a); break; }
                                case 5: { const float r=uni(2.5f,9.f),a=uni(0.f,6.28f); ddx=r*std::cos(a); ddy=r*std::sin(a); break; }
                                case 6: { const float r=uni(0.f,30.f),a=uni(0.f,6.28f); ddx=r*std::cos(a); ddy=r*std::sin(a); break; }
                                case 7: ddx=0.5f; ddy=0.5f; break;
                                default: ddx=-2.f; ddy=0.f; break;
                                }
                                pedPos(0)=pp; pedPos(1)=CVector{pp.x+ddx,pp.y+ddy,pp.z+uni(-2.f,2.f)};
                                if(next()%2)At<float>(o,0x18)=next()%2 ? 10.f : 100.f;
                            }
                            if(nm=="CreateNextSubTask"||nm=="CreateFirstSubTask"){ if(next()%6==0)At<CPed*>(o,0x10)=nullptr; }
                        }
                        CTASKS61B_COMMON_TAIL
                        if(nm=="Clone")ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                        else if(nm=="GetTaskType")ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        else if(nm=="Destructor")callVoid();
                        else if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,CPed*,uint32,uint32,uint32,float)>(target)(o,static_cast<CPed*>(q0),uint32(a0),uint32(a1),uint32(a2),f0));
                        else if(nm=="AbortArmIK")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,p0);
                        else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,p0,a0,ev);
                        else ret=Ptr(callTask(p0)); // ControlSubTask, CreateFirstSubTask, CreateNextSubTask
                        CTASKS61B_FINISH
                    }
                    if(cls=="CTaskComplexSignalAtPed") {
                        objSize=0x18;
                        auto setSap=[&]{ setObj(VT_SAP,objSize); At<CTask*>(o,8)=Fake(0); At<CPed*>(o,0xC)=next()%6==0 ? nullptr : Ped(int(1+next()%2));
                            { static const int32 pz[]{-1,0,5,1000,-1}; At<int32>(o,0x10)=next()%4==0 ? int32(next()%5000) : pz[next()%std::size(pz)]; }
                            o[0x14]=uint8(next()%2); o[0x15]=uint8(next()%2); };
                        At<uint8>(reinterpret_cast<void*>(BLOCK0),0)=uint8(next()%2); At<uint8>(reinterpret_cast<void*>(BLOCK1),0)=uint8(next()%2);
                        if(nm=="Constructor"){ q0=next()%6==0 ? nullptr : (void*)Ped(int(1+next()%2)); a0=int32(next()%3==0 ? next() : next()%4); b0=next()%2; }
                        else {
                            setSap();
                            if(nm=="CreateNextSubTask")subType({TASK_COMPLEX_TURN_TO_FACE_ENTITY,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_DO_HAND_SIGNAL,TASK_SIMPLE_PAUSE,TASK_COMPLEX_TURN_TO_FACE_ENTITY,TASK_SIMPLE_DO_HAND_SIGNAL});
                            if(nm=="ControlSubTask"){ At<CTask*>(o,8)=Fake(0); const CVector pp{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,5.f)}; pedPos(0)=pp;
                                if(At<CPed*>(o,0xC))pedPos(int(1+(At<CPed*>(o,0xC)==Ped(2))))=CVector{pp.x+uni(-9.f,9.f),pp.y+uni(-9.f,9.f),pp.z}; }
                            if(nm=="CreateFirstSubTask"&&next()%3==0)mGang=0xFFFFFFFFu;
                        }
                        CTASKS61B_COMMON_TAIL
                        if(nm=="Clone")ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                        else if(nm=="GetTaskType")ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        else if(nm=="Destructor")callVoid();
                        else if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,CPed*,int32,uint32)>(target)(o,static_cast<CPed*>(q0),a0,b0));
                        else ret=Ptr(callTask(p0)); // ControlSubTask, CreateFirstSubTask, CreateNextSubTask
                        CTASKS61B_FINISH
                    }
                    if(cls=="CTaskComplexCrossRoadLookAndAchieveHeading") {
                        objSize=0x14;
                        auto setCr=[&]{ setObj(VT_CR,objSize); At<CTask*>(o,8)=Fake(0); At<int32>(o,0xC)=next()%4==0 ? int32(next()) : int32(next()%5000);
                            At<float>(o,0x10)=next()%8==0 ? 0.f : uni(-7.f,7.f); };
                        if(nm=="Constructor"){ a0=next()%4==0 ? int32(next()) : int32(next()%5000); f0=next()%8==0 ? 0.f : uni(-7.f,7.f); }
                        else {
                            setCr();
                            if(nm=="CreateNextSubTask")subType({TASK_SIMPLE_ANIM,TASK_SIMPLE_ACHIEVE_HEADING,TASK_SIMPLE_PAUSE,TASK_FINISHED,TASK_SIMPLE_ACHIEVE_HEADING,TASK_SIMPLE_ANIM});
                            if(nm=="MakeAbortable"){ At<CTask*>(o,8)=Fake(0); a0=prio(); ev=next()%2 ? reinterpret_cast<const CEvent*>(fevs[0].data()) : nullptr; }
                            if(nm=="CreateSubTask"){ static const int32 ty[]{TASK_SIMPLE_ANIM,TASK_SIMPLE_ACHIEVE_HEADING,TASK_SIMPLE_ACHIEVE_HEADING,TASK_FINISHED,TASK_NONE,1000,TASK_SIMPLE_PAUSE}; a0=ty[next()%std::size(ty)]; }
                        }
                        CTASKS61B_COMMON_TAIL
                        if(nm=="Clone")ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                        else if(nm=="GetTaskType")ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        else if(nm=="Destructor")callVoid();
                        else if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,int32,float)>(target)(o,a0,f0));
                        else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,p0,a0,ev);
                        else if(nm=="CreateSubTask")ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,a0,p0));
                        else ret=Ptr(callTask(p0)); // ControlSubTask, CreateFirstSubTask, CreateNextSubTask
                        CTASKS61B_FINISH
                    }
                    if(cls=="CTaskComplexDestroyCar") {
                        objSize=0x20;
                        auto setDc=[&]{ setObj(VT_DC,objSize); At<CTask*>(o,8)=Fake(0); o[0xC]=uint8(next()%2);
                            At<CVehicle*>(o,0x10)=next()%8==0 ? nullptr : Veh(int(next()%NVEH)); for(size_t w=0x14;w<0x20;w+=4)At<uint32>(o,w)=next()%3==0 ? 0u : next(); };
                        if(nm=="Constructor"){ q0=next()%5==0 ? nullptr : (void*)Veh(int(next()%NVEH)); a0=int32(next()); a1=int32(next()); a2=int32(next()); }
                        else {
                            setDc();
                            // CreateSubTask(LEAVE_CAR) y CreateFirstSubTask usan el vehiculo del peatón sin guarda: siempre lo tiene si el tipo lo necesita
                            if(nm=="CreateNextSubTask")subType({TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_DRAG_PED_FROM_CAR,TASK_COMPLEX_FALL_AND_GET_UP,TASK_SIMPLE_BE_HIT,TASK_COMPLEX_DESTROY_CAR_ARMED,
                                                                TASK_COMPLEX_DESTROY_CAR_MELEE,TASK_SIMPLE_PAUSE,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_DRAG_PED_FROM_CAR});
                            if(nm=="CreateFirstSubTask"){ if(next()%2)At<CVehicle*>(p0b,P_VEH)=Veh(int(next()%NVEH)); }
                            if(nm=="MakeAbortable"){ At<CTask*>(o,8)=Fake(0); a0=prio(); ev=next()%2 ? reinterpret_cast<const CEvent*>(fevs[0].data()) : nullptr; }
                            if(nm=="CreateSubTask"){ static const int32 ty[]{TASK_COMPLEX_DESTROY_CAR_ARMED,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_DESTROY_CAR_MELEE,TASK_FINISHED,TASK_NONE,1000,TASK_SIMPLE_PAUSE};
                                a0=ty[next()%std::size(ty)]; if(a0==TASK_COMPLEX_LEAVE_CAR)At<CVehicle*>(p0b,P_VEH)=Veh(int(next()%NVEH)); }
                            if(nm=="ControlSubTask"){
                                subType({TASK_COMPLEX_DESTROY_CAR_ARMED,TASK_COMPLEX_DESTROY_CAR_ARMED,TASK_COMPLEX_DESTROY_CAR_MELEE,TASK_COMPLEX_DESTROY_CAR_MELEE,TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_PAUSE});
                                if(o[0xC]&&next()%2)o[0xC]=0;
                                // ranuras de armas: la activa y las demas con o sin municion (la busqueda recorre las 13)
                                if(next()%2)for(int w=0;w<13;++w)At<uint32>(p0b,P_WEAPONS+0x1C*w+0xC)=next()%3==0 ? 1u : 0u;
                                if(next()%3==0)for(int w=0;w<13;++w)At<uint32>(p0b,P_WEAPONS+0x1C*w+0xC)=0u;
                                if(next()%2)At<CVehicle*>(p0b,P_VEH)=Veh(int(next()%NVEH)); At<uint32>(p0b,P_FLAGS)|=next()%2 ? IN_VEH : 0u;
                                if(o[0xC]&&!At<CVehicle*>(o,0x10))At<CVehicle*>(o,0x10)=Veh(0);
                                // CreateFirstSubTask (con m_arg0) puede crear LEAVE_CAR con el vehiculo del peatón: nunca es nulo si esta en vehiculo
                                if(o[0xC]&&(At<uint32>(p0b,P_FLAGS)&IN_VEH)&&!At<CVehicle*>(p0b,P_VEH))At<CVehicle*>(p0b,P_VEH)=Veh(1);
                            }
                        }
                        CTASKS61B_COMMON_TAIL
                        if(nm=="Clone")ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                        else if(nm=="GetTaskType")ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        else if(nm=="Destructor")callVoid();
                        else if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,uint32,uint32,uint32)>(target)(o,static_cast<CVehicle*>(q0),uint32(a0),uint32(a1),uint32(a2)));
                        else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,p0,a0,ev);
                        else if(nm=="CreateSubTask")ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,a0,p0));
                        else ret=Ptr(callTask(p0)); // ControlSubTask, CreateFirstSubTask, CreateNextSubTask
                        CTASKS61B_FINISH
                    }
                    if(cls=="CTaskSimpleChoking") {
                        objSize=0x1C;
                        auto setCh=[&]{ setObj(VT_CH,objSize); At<void*>(o,8)=next()%4==0 ? nullptr : (void*)Ped(int(1+next()%2));
                            At<void*>(o,0xC)=next()%4==0 ? nullptr : (void*)assocs[0].data();
                            { static const uint32 tr[]{0u,1u,20u,19u,21u,1000u,0xFFFFFFFFu}; At<uint32>(o,0x10)=next()%3==0 ? next()%3000u : tr[next()%std::size(tr)]; }
                            At<uint32>(o,0x14)=next()%2 ? now-next()%20000 : next();
                            o[0x18]=uint8(next()%2); o[0x19]=uint8(next()%4==0); };
                        if(nm=="Constructor"){ q0=next()%4==0 ? nullptr : (void*)Ped(int(1+next()%2)); b0=next()%2; }
                        else {
                            setCh();
                            if(nm=="MakeAbortable"){ a0=prio(); ev=next()%3==0 ? nullptr : reinterpret_cast<const CEvent*>(fevs[next()%NEVS].data()); }
                            if(nm=="ProcessPed"||nm=="UpdateChoke"){
                                // el EXE usa la matriz del peatón sin guarda (ProcessPed)
                                ensureMat(0);
                                for(int r=0;r<3;++r)At<CVector>(pedMats[0].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                                if(At<void*>(o,8)&&next()%3){ const CVector pp=pedPos(0); const int an=At<void*>(o,8)==peds[2].data() ? 2 : 1;
                                    pedPos(an)=CVector{pp.x+uni(-6.f,6.f),pp.y+uni(-6.f,6.f),pp.z+uni(-6.f,6.f)}; }
                                if(nm=="ProcessPed"){ if(next()%3==0)At<uint32>(o,0x10)=next()%25; if(next()%3==0)o[0x19]=0; if(next()%2)o[0x18]=0; if(next()%2)mPlayer=0; }
                                if(nm=="UpdateChoke"){ q0=next()%4==0 ? nullptr : (next()%3==0 ? (void*)At<void*>(o,8) : (void*)Ped(int(1+next()%2))); b0=next()%2; }
                            }
                        }
                        CTASKS61B_COMMON_TAIL
                        if(nm=="Clone")ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                        else if(nm=="GetTaskType")ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        else if(nm=="Destructor")callVoid();
                        else if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,CPed*,uint32)>(target)(o,static_cast<CPed*>(q0),b0));
                        else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,p0,a0,ev);
                        else if(nm=="ProcessPed")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,p0);
                        else if(nm=="UpdateChoke")reinterpret_cast<void(__thiscall*)(void*,CPed*,CPed*,uint32)>(target)(o,p0,static_cast<CPed*>(q0),b0);
                        CTASKS61B_FINISH
                    }
                    return json{{"switches",false},{"clean",true},{"reason","unknown class"}};
                };
                Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
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
    const bool restored=Occ(TaskHdr())==occTaskStart&&freeRefs()==savedFree&&globalsIntact()&&Last(TaskHdr())==lt0;
    if(restored)std::memcpy(States(TaskHdr()),stTask.data(),stTask.size());
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
#undef CTASKS61B_COMMON_TAIL
#undef CTASKS61B_FINISH
