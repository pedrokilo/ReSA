#pragma once
// Banco dentro del motor: tareas complejas A (lista de 10, 40 funciones).
// CTaskComplexDestroyCarArmed, CTaskComplexDestroyCarMelee y CTaskComplexKillPedOnFootMelee (constructor, destructor, Clone,
// GetTaskType, MakeAbortable, CalculateSearchPositionAndRanges, ControlSubTask, CreateFirstSubTask, CreateNextSubTask,
// CreateSubTask) y CTaskComplexLeaveCarAsPassengerWait (constructor, destructor, Clone, GetTaskType, ControlSubTask,
// CreateFirstSubTask, CreateNextSubTask, CreateSubTask, GetNextPassengerToLeave, SetPedInZerothPassengerSeat).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 4 peatones (inteligencia propia con tarea de lucha secundaria falsa, armas, salud, flags), 2 vehiculos (vtable falsa con
// trampas, modelo MODEL), tareas falsas (vtable trazada, MakeAbortable con una mascara de respuestas), evento falso
// (GetEventType trazado, vehiculo en +0x14) y CBaseModelInfo/CColModel falsos (se cambia temporalmente
// CModelInfo::ms_modelInfoPtrs[MODEL]).
// Se simulan y trazan: rand/CGeneral::GetRandomNumber, CPed::Say, CPed::SetMoveState, CInterestingEvents::Add,
// CTaskSimpleStandStill::ProcessPed (tarea local), CTaskSimpleCarSetPedOut::ProcessPed y
// CTaskSimpleCarSetPedInAsPassenger::ProcessPed (tareas locales: se trazan sus campos).
// Pool de tareas: m_LastFreeSlot y estados restaurados y slots libres con 0xCD en cada ejecucion; las tareas nuevas se
// serializan y se liberan sin destructor (antes se limpian las referencias de los fixtures). Se guardan y restauran el
// reloj y el puntero de ms_modelInfoPtrs[MODEL].
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Tasks/Task.h"
#include "TaskComplexDestroyCarArmed.h"
#include "TaskComplexDestroyCarMelee.h"
#include "TaskComplexKillPedOnFootMelee.h"
#include "TaskComplexLeaveCarAsPassengerWait.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleFight.h"
#include "InterestingEvents.h"
#include "PedIntelligence.h"
#include "WeaponInfo.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks10aLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }
inline uintptr_t Body(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

constexpr int NPEDS=4, NVEH=2, FAKES=3;
// tareas falsas: campos de control por encima de 0x50 (las tareas reales escriben como mucho hasta 0x18 en ellas)
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_SUB=0x64, FK_TYPE=0x68, FK_CLONE=0x6C, FK_CNT=0x70;
// desplazamientos crudos del EXE
constexpr size_t E_MATRIX=0x14, E_RWOBJ=0x18, E_MODEL=0x22, E_TYPE=0x36, E_PFLAGS=0x40, E_MOVESPEED=0x44; // CEntity/CPhysical
constexpr size_t P_FLAGS=0x46C, P_INTEL=0x47C, P_HEALTH=0x540, P_AIM=0x55C, P_PEDTYPE=0x598, P_WEAPONS=0x5A0, P_WEAPSLOT=0x718; // CPed
constexpr size_t V_PASS=0x464, V_NUMPASS=0x484, V_MAXPASS=0x488, V_HEALTH=0x4C0;                       // CVehicle
constexpr size_t I_SECONDARY=0x18;                                                                     // intel+4 (gestor) +0x14
constexpr size_t EV_VEH=0x14, EV_TYPE=0x4C;                                                            // evento falso
constexpr uintptr_t TIMER=0xB7CB84, MODELPTRS=0xA9B0C8, INTEREST=0xC0B058;
constexpr int MODEL=400;
constexpr uintptr_t VT_ARMED=0x86D9C4, VT_MELEE=0x86D994, VT_KILL=0x86D8EC, VT_WAIT=0x86E7A4;
constexpr uint32 STAY=0x200000u, KINDA=0x400000u;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,0x40>,NPEDS> clumps{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<uint8,0x50> eventBuf{};
alignas(16) inline std::array<uint8,0x60> mi{}, col{};
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
    if(v==INTEREST)return "interesting_events";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n);
                              if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n); if(auto s=in(clumps[n].data(),clumps[n].size(),"clump"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(eventBuf.data(),eventBuf.size(),"event"))return *s;
    if(auto s=in(mi.data(),mi.size(),"model_info"))return *s; if(auto s=in(col.data(),col.size(),"col_model"))return *s;
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    // el constructor de CTaskComplexDestroyCarArmed del EXE registra la referencia en su parametro de la pila (sin FIX_BUGS
    // la DLL lo replica): la direccion no es comparable entre ejecuciones y se registra como "stack"
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
inline int16 __fastcall FakeSay(CPed* p,void*,int ctx,uint32 delay,float prob,uint32 overrideSilence,uint32 forceAudible,uint32 frontEnd) {
    trace.push_back({"say",Name(p),ctx,delay,F(prob),overrideSilence&0xFF,forceAudible&0xFF,frontEnd&0xFF}); return -1;
}
inline void __fastcall FakeSetMoveState(CPed* p,void*,int32 s) { trace.push_back({"set_move_state",Name(p),s}); }
inline void __fastcall FakeInteresting(void* ie,void*,int32 type,CEntity* e) { trace.push_back({"interesting_event",Name(ie),type,Name(e)}); }
// tareas locales en la pila: solo se trazan sus campos
inline bool __fastcall FakeStandStillProcess(CTask* t,void*,CPed* p) {
    trace.push_back({"stand_still_process",Name(p),At<int32>(t,8),At<uint8>(t,0x18),At<uint8>(t,0x19),F(At<float>(t,0x1C))}); return false;
}
inline bool __fastcall FakeSetPedOutProcess(CTask* t,void*,CPed* p) {
    trace.push_back({"set_ped_out_process",Name(p),Name(At<void*>(t,8)),At<int32>(t,0xC),At<uint8>(t,0x10),At<uint8>(t,0x11),At<uint8>(t,0x12),At<uint8>(t,0x13),At<uint8>(t,0x14),At<uint8>(t,0x15)});
    return true;
}
inline bool __fastcall FakeSetPedInProcess(CTask* t,void*,CPed* p) {
    trace.push_back({"set_ped_in_passenger_process",Name(p),At<uint8>(t,8),Name(At<void*>(t,0xC)),Name(At<void*>(t,0x10)),At<int32>(t,0x14),Name(At<void*>(t,0x18)),At<uint8>(t,0x1C),At<uint8>(t,0x1D),At<uint8>(t,0x1E)});
    return true;
}

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t OurVirtual(const char* cls,const char* fn){ auto* h=FindHook(cls,fn); if(!h||h->Symbol()[0]!='V')return 0; return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress()); }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=40;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexDestroyCarArmed","CalculateSearchPositionAndRanges",0x628C80,0,-1,256},
    {"CTaskComplexDestroyCarArmed","Clone",0x623600,0x86D9C4,1,128},
    {"CTaskComplexDestroyCarArmed","Constructor",0x621F50,0,-1,128},
    {"CTaskComplexDestroyCarArmed","ControlSubTask",0x628FA0,0x86D9C4,10,1024},
    {"CTaskComplexDestroyCarArmed","CreateFirstSubTask",0x62E0A0,0x86D9C4,9,512},
    {"CTaskComplexDestroyCarArmed","CreateNextSubTask",0x62DF20,0x86D9C4,8,1024},
    {"CTaskComplexDestroyCarArmed","CreateSubTask",0x628DA0,0,-1,512},
    {"CTaskComplexDestroyCarArmed","Destructor",0x622010,0,-1,128},
    {"CTaskComplexDestroyCarArmed","GetTaskType",0x622000,0x86D9C4,4,64},
    {"CTaskComplexDestroyCarArmed","MakeAbortable",0x622070,0x86D9C4,6,256},
    {"CTaskComplexDestroyCarMelee","CalculateSearchPositionAndRanges",0x6289F0,0,-1,256},
    {"CTaskComplexDestroyCarMelee","Clone",0x6235A0,0x86D994,1,128},
    {"CTaskComplexDestroyCarMelee","Constructor",0x621D10,0,-1,128},
    {"CTaskComplexDestroyCarMelee","ControlSubTask",0x62DDB0,0x86D994,10,1024},
    {"CTaskComplexDestroyCarMelee","CreateFirstSubTask",0x62DB20,0x86D994,9,512},
    {"CTaskComplexDestroyCarMelee","CreateNextSubTask",0x62DC20,0x86D994,8,1024},
    {"CTaskComplexDestroyCarMelee","CreateSubTask",0x628A70,0,-1,512},
    {"CTaskComplexDestroyCarMelee","Destructor",0x621DA0,0,-1,128},
    {"CTaskComplexDestroyCarMelee","GetTaskType",0x621D90,0x86D994,4,64},
    {"CTaskComplexDestroyCarMelee","MakeAbortable",0x621E00,0x86D994,6,1024},
    {"CTaskComplexKillPedOnFootMelee","CalculateSearchPositionAndRanges",0x621100,0,-1,256},
    {"CTaskComplexKillPedOnFootMelee","Clone",0x623460,0x86D8EC,1,128},
    {"CTaskComplexKillPedOnFootMelee","Constructor",0x621000,0,-1,128},
    {"CTaskComplexKillPedOnFootMelee","ControlSubTask",0x626D90,0x86D8EC,10,1024},
    {"CTaskComplexKillPedOnFootMelee","CreateFirstSubTask",0x62BE30,0x86D8EC,9,512},
    {"CTaskComplexKillPedOnFootMelee","CreateNextSubTask",0x62BC10,0x86D8EC,8,1024},
    {"CTaskComplexKillPedOnFootMelee","CreateSubTask",0x626AC0,0,-1,512},
    {"CTaskComplexKillPedOnFootMelee","Destructor",0x621090,0,-1,128},
    {"CTaskComplexKillPedOnFootMelee","GetTaskType",0x621080,0x86D8EC,4,64},
    {"CTaskComplexKillPedOnFootMelee","MakeAbortable",0x6210F0,0x86D8EC,6,256},
    {"CTaskComplexLeaveCarAsPassengerWait","Clone",0x63D8C0,0x86E7A4,1,128},
    {"CTaskComplexLeaveCarAsPassengerWait","Constructor",0x63B430,0,-1,128},
    {"CTaskComplexLeaveCarAsPassengerWait","ControlSubTask",0x644050,0x86E7A4,10,512},
    {"CTaskComplexLeaveCarAsPassengerWait","CreateFirstSubTask",0x644010,0x86E7A4,9,512},
    {"CTaskComplexLeaveCarAsPassengerWait","CreateNextSubTask",0x643F90,0x86E7A4,8,256},
    {"CTaskComplexLeaveCarAsPassengerWait","CreateSubTask",0x640BE0,0,-1,512},
    {"CTaskComplexLeaveCarAsPassengerWait","Destructor",0x63B4A0,0,-1,128},
    {"CTaskComplexLeaveCarAsPassengerWait","GetNextPassengerToLeave",0x63B500,0,-1,256},
    {"CTaskComplexLeaveCarAsPassengerWait","GetTaskType",0x63B490,0x86E7A4,4,64},
    {"CTaskComplexLeaveCarAsPassengerWait","SetPedInZerothPassengerSeat",0x63B540,0,-1,128},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks10a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks10a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks10a");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks10a-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    const uintptr_t dllStandStill=OurVirtual("CTaskSimpleStandStill","ProcessPed");
    if(!dllStandStill){Write({{"event","error"},{"reason","virtual hook not found"},{"function","CTaskSimpleStandStill::ProcessPed"}});return;}
    FillTraps(fakeVtbl,std::make_integer_sequence<int,16>{}); FillTraps(vehVtbl,std::make_integer_sequence<int,96>{}); FillTraps(eventVtbl,std::make_integer_sequence<int,16>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    eventVtbl[1]=reinterpret_cast<void*>(&FEventType);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: reloj y ms_modelInfoPtrs[MODEL]; se guarda y se restaura en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{MODELPTRS+4*MODEL,4}};
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
        EntryPatch s2(Body(bit_cast<uintptr_t>(&CPed::SetMoveState)),FakeSetMoveState), s2e(0x5DEC00,FakeSetMoveState);
        EntryPatch s3(Body(bit_cast<uintptr_t>(&CInterestingEvents::Add)),FakeInteresting), s3e(0x602590,FakeInteresting);
        EntryPatch s4(Body(dllStandStill),FakeStandStillProcess), s4e(0x62F370,FakeStandStillProcess);
        // ProcessPed de CTaskSimpleCarSetPedOut/CarSetPedInAsPassenger estan reconstruidos: se parchea la direccion del EXE y tambien el cuerpo de la DLL
        EntryPatch s5(Body(OurVirtual("CTaskSimpleCarSetPedOut","ProcessPed")),FakeSetPedOutProcess), s5e(0x647D10,FakeSetPedOutProcess), s6(Body(OurVirtual("CTaskSimpleCarSetPedInAsPassenger","ProcessPed")),FakeSetPedInProcess), s6e(0x64B5D0,FakeSetPedInProcess);

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
                    // modelo falso: CColModel con radio de la esfera en +0x24
                    std::memset(mi.data(),0,mi.size()); std::memset(col.data(),0,col.size()); At<void*>(mi.data(),0x14)=col.data();
                    At<float>(col.data(),0x24)=next()%6==0 ? 0.f : uni(0.3f,4.f);
                    At<void*>(reinterpret_cast<void*>(MODELPTRS),4*MODEL)=mi.data();
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,FAKE_SIZE); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_PAUSE; At<uint32>(fakes[n].data(),FK_ABORT)=next(); }
                    // peatones: matriz, inteligencia (tarea de lucha secundaria falsa), clump (tipo 2), tipo, flags, salud, armas
                    static const int32 weaponTypes[]{WEAPON_UNARMED,WEAPON_PISTOL,WEAPON_GRENADE,WEAPON_M4,WEAPON_SHOTGUN,WEAPON_REMOTE_SATCHEL_CHARGE,WEAPON_SNIPERRIFLE,WEAPON_KNIFE};
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                        std::memset(clumps[n].data(),0,clumps[n].size()); clumps[n][0]=2;
                        p[E_TYPE]=uint8(3|(next()%8)<<3); At<void*>(p,P_INTEL)=intels[n].data(); At<void*>(p,E_MATRIX)=pedMats[n].data(); At<void*>(p,E_RWOBJ)=clumps[n].data();
                        for(int r=0;r<3;++r)At<CVector>(pedMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(pedMats[n].data(),0x30)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                        { static const int32 types[]{0,1,4,6,6,7,8}; At<int32>(p,P_PEDTYPE)=types[next()%7]; }
                        if(next()%3==0)At<uint32>(p,P_FLAGS)|=STAY; if(next()%3==0)At<uint32>(p,P_FLAGS)|=KINDA;
                        if(next()%3==0)At<uint32>(p,E_PFLAGS)|=0x100u; // bSubmergedInWater
                        { const uint32 m=next()%6; At<float>(p,P_HEALTH)=m==0 ? 0.f : m==1 ? -5.f : m==2 ? qnan : uni(1.f,200.f); }
                        At<float>(p,P_AIM)=uni(-4.f,4.f);
                        p[P_WEAPSLOT]=uint8(next()%4); for(int s=0;s<4;++s)At<int32>(p,P_WEAPONS+0x1C*s)=weaponTypes[next()%std::size(weaponTypes)];
                        // tarea secundaria de ataque: CTaskSimpleFight falsa (juego de combos en +0x24) o ninguna
                        if(n==0&&next()%4){ At<CTask*>(intels[n].data(),I_SECONDARY)=Fake(1); At<int32>(fakes[1].data(),FK_TYPE)=next()%6 ? TASK_SIMPLE_FIGHT : TASK_SIMPLE_USE_GUN;
                                            At<int8>(fakes[1].data(),0x24)=int8(int32(next()%19)-3); }
                    }
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%8)<<3); At<void*>(v,E_MATRIX)=vehMats[n].data(); At<int16>(v,E_MODEL)=int16(MODEL);
                        for(int r=0;r<3;++r)At<CVector>(vehMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                        for(int s=0;s<8;++s)At<CPed*>(v,V_PASS+4*s)=next()%3==0 ? Ped(int(next()%NPEDS)) : nullptr;
                        v[V_NUMPASS]=uint8(next()%9); v[V_MAXPASS]=uint8(next()%9);
                        { const uint32 m=next()%6; At<float>(v,V_HEALTH)=m==0 ? 0.f : m==1 ? -5.f : m==2 ? qnan : uni(1.f,1000.f); }
                        { const uint32 m=next()%6; At<CVector>(v,E_MOVESPEED)=m==0 ? CVector{} : m==1 ? CVector{qnan,0.f,0.f} : m==2 ? CVector{next()%2 ? 0.15f : std::bit_cast<float>(0x3CB851ECu),0.f,0.f}
                                                                         : m==3 ? CVector{uni(-0.012f,0.012f),uni(-0.012f,0.012f),uni(-0.005f,0.005f)} : CVector{uni(-0.3f,0.3f),uni(-0.3f,0.3f),uni(-0.05f,0.05f)}; }
                    }
                    std::memset(eventBuf.data(),0,eventBuf.size()); At<void*>(eventBuf.data(),0)=eventVtbl.data();
                    { static const int32 et[]{EVENT_VEHICLE_COLLISION,EVENT_VEHICLE_COLLISION,EVENT_VEHICLE_COLLISION,EVENT_POTENTIAL_WALK_INTO_VEHICLE,EVENT_POTENTIAL_WALK_INTO_VEHICLE,EVENT_DAMAGE};
                      At<int32>(eventBuf.data(),EV_TYPE)=et[next()%std::size(et)]; }
                    { const uint32 m=next()%5; At<CVehicle*>(eventBuf.data(),EV_VEH)=m<3 ? Veh(0) : m==3 ? Veh(1) : nullptr; }
                    std::memset(object.data(),0xCD,object.size());

                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches}}; json ret=nullptr; std::vector<int> freshSlots;
                    auto* o=object.data(); auto* p0=Ped(0); auto* p0b=peds[0].data();
                    auto collectFresh=[&]{
                        for(auto* h:hooks)h->State(false);
                        auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                        for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshSlots.push_back(int(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                        json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t)); out["tasks"]=tasks;
                    };
                    auto setTask=[&](uintptr_t vtbl,size_t size){ for(size_t b=0;b<size;++b)o[b]=uint8(next()); At<uintptr_t>(o,0)=vtbl; At<uint32>(o,4)=0; At<uint32>(o,8)=0; };
                    auto setSub=[&](const int32* types,size_t n){ At<CTask*>(o,8)=Fake(0); At<int32>(fakes[0].data(),FK_TYPE)=types[next()%n]; };
                    auto typeOf=[&](void* p){ return reinterpret_cast<CTask*>(p)->GetTaskType(); };
                    auto callTask=[&](CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    auto callSub=[&](int32 type,CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,type,ped); };
                    auto callClone=[&]{ return reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); };
                    auto callVoid=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); };
                    auto callPedVoid=[&](CPed* ped){ reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    auto callAbort=[&](CPed* ped,int32 prio,const CEvent* e){ return reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,ped,prio,e); };
                    auto ctorDone=[&](size_t size){ for(auto* h:hooks)h->State(false); out["object"]=Region(o,size,true); out["type"]=typeOf(o); };
                    auto dtorSub=[&]{ At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; };
                    auto prefill=[&](size_t size){ for(size_t b=0;b<size;++b)o[b]=uint8(next()); };
                    auto pickEvent=[&]()->CEvent*{ return next()%5 ? Ev() : nullptr; };
                    auto finish=[&](size_t size){ for(auto* h:hooks)h->State(false); out["object"]=Region(o,size,true); };
                    auto pedPos=[&](int n){ return At<CVector>(pedMats[n].data(),0x30); };
                    // coloca pos (posicion de un vehiculo o peaton) respecto al peaton 0: a distancia exacta d, d^2, NaN o al azar
                    auto place=[&](CVector& pos,float d){
                        const CVector pp=pedPos(0); const uint32 m=next()%8;
                        if(m==0)pos=CVector{pp.x+d,pp.y,pp.z}; else if(m==1)pos=CVector{pp.x,pp.y-d*d,pp.z}; else if(m==2)pos=CVector{pp.x,pp.y,qnan};
                        else if(m<6)pos=CVector{pp.x+uni(-1.5f,1.5f)*d,pp.y+uni(-1.5f,1.5f)*d,pp.z+uni(-0.5f,0.5f)};
                    };
                    auto timerPick=[&]()->int32{ const uint32 m=next()%6; return m==0 ? -1 : m==1 ? int32(now-3000) : m==2 ? int32(now-3001) : int32(now-next()%6000); };
                    auto pedOut=[&]{ out["ped_aim"]=At<uint32>(p0b,P_AIM); };

                    if(nm=="GetTaskType") {
                        setTask(fn.vt,0x40);
                        ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        out["object"]=Region(o,0x40,true);
                    }
                    else if(cls=="CTaskComplexDestroyCarArmed") {
                        // +0xC crear primera, +0x10 vehiculo (referencia), +0x14/+0x20/+0x2C posiciones, +0x38 rango, +0x40 rango minimo,
                        // +0x48 distancia, +0x4C distancia de GoToPoint, +0x50 GoToPoint, +0x54..0x60
                        if(nm=="Constructor"){
                            prefill(0x64); CVehicle* v=next()%4 ? Veh(0) : nullptr;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,uint32,uint32,uint32)>(target)(o,v,next(),next(),next()));
                            ctorDone(0x64);
                        } else {
                            setTask(VT_ARMED,0x64);
                            const bool needVeh=nm=="CalculateSearchPositionAndRanges";
                            CVehicle* v=(needVeh||next()%8) ? Veh(0) : nullptr; At<CVehicle*>(o,0x10)=v; Reg(v,o+0x10);
                            for(size_t off=0x14;off<0x38;off+=4)At<float>(o,off)=uni(-30.f,30.f);
                            for(size_t off=0x38;off<0x50;off+=4)At<float>(o,off)=uni(0.f,40.f);
                            o[0xC]=uint8(next()%3==0); o[0x50]=uint8(next()%2);
                            // rango del arma activa del peaton 0 (el mismo calculo que CalculateSearchPositionAndRanges)
                            const int32 wtype=At<int32>(p0b,P_WEAPONS+0x1C*p0b[P_WEAPSLOT]);
                            const auto* wi=CWeaponInfo::GetWeaponInfo(eWeaponType(wtype),p0->GetWeaponSkill(eWeaponType(wtype)));
                            const float half=wi->m_fWeaponRange*0.5f, outer=half*1.1f, inner=std::max(0.8f,half*0.9f), targetRange=wi->m_fTargetRange;
                            const float pick[]{outer,inner,targetRange,uni(0.5f,40.f)};
                            place(At<CVector>(vehMats[0].data(),0x30),pick[next()%4]);
                            if(next()%4==0)At<float>(o,0x4C)=(At<CVector>(vehMats[0].data(),0x30)-pedPos(0)).Magnitude();
                            if(nm=="Destructor"){ dtorSub(); callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(); ret=Name(r); }
                            else if(nm=="MakeAbortable"){ At<CTask*>(o,8)=Fake(0); ret=callAbort(p0,int32(next()%4),pickEvent()); for(auto* h:hooks)h->State(false); }
                            else if(nm=="CalculateSearchPositionAndRanges"){ callPedVoid(p0); }
                            else if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_SIMPLE_GUN_CTRL,TASK_SIMPLE_GUN_CTRL,TASK_SIMPLE_GUN_CTRL,TASK_COMPLEX_SEEK_ENTITY,TASK_SIMPLE_PAUSE,
                                                       TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_FINISHED,TASK_SIMPLE_STAND_STILL};
                                auto* r=callSub(t[next()%std::size(t)],p0); collectFresh(); ret=Name(r);
                            }
                            else {
                                static const int32 tn[]{TASK_COMPLEX_SEEK_ENTITY,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_PAUSE,TASK_SIMPLE_PAUSE,TASK_SIMPLE_PAUSE,
                                                        TASK_SIMPLE_GUN_CTRL,TASK_SIMPLE_THROW_CTRL,TASK_FINISHED,TASK_SIMPLE_STAND_STILL};
                                static const int32 tc[]{TASK_COMPLEX_SEEK_ENTITY,TASK_COMPLEX_SEEK_ENTITY,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,
                                                        TASK_SIMPLE_GUN_CTRL,TASK_SIMPLE_PAUSE};
                                if(nm=="ControlSubTask")setSub(tc,std::size(tc)); else setSub(tn,std::size(tn));
                                auto* r=callTask(p0); collectFresh(); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x80,true);
                            }
                            finish(0x64); pedOut();
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexDestroyCarMelee") {
                        // +0xC abortar al terminar, +0xD objetivo nuevo, +0x10 vehiculo (referencia), +0x14 posicion, +0x20 rango de ataque,
                        // +0x24 angulo, +0x28 rango de llegada, +0x2C ultimo ataque, +0x30 temporizador de pausa
                        if(nm=="Constructor"){
                            prefill(0x34); CVehicle* v=next()%4 ? Veh(0) : nullptr;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*)>(target)(o,v));
                            ctorDone(0x34);
                        } else {
                            setTask(VT_MELEE,0x34);
                            const bool mayNull=nm=="Destructor"||nm=="Clone"||nm=="MakeAbortable"||nm=="CreateSubTask";
                            CVehicle* v=(!mayNull||next()%6) ? Veh(0) : nullptr; At<CVehicle*>(o,0x10)=v; Reg(v,o+0x10);
                            At<CVector>(o,0x14)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                            At<float>(o,0x20)=uni(0.f,5.f); At<float>(o,0x24)=uni(-4.f,4.f); At<float>(o,0x28)=uni(0.f,5.f); At<int32>(o,0x30)=timerPick();
                            o[0xC]=uint8(next()%4==0); o[0xD]=uint8(next()%4==0);
                            const float range=At<float>(col.data(),0x24)+0.35f;
                            place(At<CVector>(vehMats[0].data(),0x30),next()%3 ? range : range+0.6f);
                            if(nm=="Destructor"){ dtorSub(); callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(); ret=Name(r); }
                            else if(nm=="MakeAbortable"){
                                At<CTask*>(o,8)=Fake(0);
                                // a veces el peaton queda exactamente detras o delante del vehiculo en la direccion de la velocidad
                                if(next()%4==0){ const CVector vp=At<CVector>(vehMats[0].data(),0x30); At<CVector>(pedMats[0].data(),0x30)=CVector{vp.x-uni(0.f,2.f),vp.y,vp.z}; }
                                ret=callAbort(p0,int32(next()%4),pickEvent()); for(auto* h:hooks)h->State(false);
                            }
                            else if(nm=="CalculateSearchPositionAndRanges"){ callPedVoid(p0); }
                            else if(nm=="CreateSubTask"){
                                static const int32 t[]{TASK_COMPLEX_SEEK_ENTITY,TASK_SIMPLE_PAUSE,TASK_SIMPLE_PAUSE,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,
                                                       TASK_SIMPLE_FIGHT_CTRL,TASK_SIMPLE_FIGHT_CTRL,TASK_FINISHED,TASK_SIMPLE_STAND_STILL};
                                auto* r=callSub(t[next()%std::size(t)],p0); collectFresh(); ret=Name(r);
                            }
                            else {
                                static const int32 tn[]{TASK_SIMPLE_PAUSE,TASK_SIMPLE_PAUSE,TASK_SIMPLE_FIGHT_CTRL,TASK_SIMPLE_FIGHT_CTRL,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,
                                                        TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_COMPLEX_SEEK_ENTITY,TASK_FINISHED,TASK_SIMPLE_STAND_STILL};
                                static const int32 tc[]{TASK_COMPLEX_SEEK_ENTITY,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_FIGHT_CTRL,TASK_SIMPLE_FIGHT_CTRL,TASK_SIMPLE_PAUSE};
                                if(nm=="ControlSubTask")setSub(tc,std::size(tc)); else setSub(tn,std::size(tn));
                                auto* r=callTask(p0); collectFresh(); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x80,true);
                            }
                            finish(0x34); pedOut();
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexKillPedOnFootMelee") {
                        // +0xC objetivo (referencia), +0x10 posicion del objetivo, +0x1C rango de llegada, +0x20 rango de ataque, +0x24 angulo,
                        // +0x28 punto de ataque, +0x2C ultimo ataque, +0x30/+0x31 flags (objetivo nuevo), +0x34 temporizador de pausa
                        if(nm=="Constructor"){
                            prefill(0x38); CPed* t=next()%4 ? Ped(1) : nullptr;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CPed*)>(target)(o,t));
                            ctorDone(0x38);
                        } else {
                            setTask(VT_KILL,0x38);
                            const bool needTarget=nm=="CreateSubTask";
                            CPed* t=(needTarget||next()%8) ? Ped(1) : nullptr; At<CPed*>(o,0xC)=t; Reg(t,o+0xC);
                            At<CVector>(o,0x10)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                            At<float>(o,0x1C)=uni(0.f,5.f); At<float>(o,0x20)=uni(0.f,10.f); At<float>(o,0x24)=uni(-4.f,4.f); At<int32>(o,0x34)=timerPick();
                            o[0x30]=uint8(next()%2); o[0x31]=uint8(next()%4==0);
                            // rango de llegada que calculara CalculateSearchPositionAndRanges (juego de combos de la tarea de lucha falsa)
                            auto* fight=At<CTask*>(intels[0].data(),I_SECONDARY);
                            const int32 idx=fight&&At<int32>(fakes[1].data(),FK_TYPE)==TASK_SIMPLE_FIGHT ? std::max(0,int32(At<int8>(fakes[1].data(),0x24))-4) : 0;
                            const float arrive=CTaskSimpleFight::m_aComboData.data()[idx].m_fRanges;
                            place(At<CVector>(pedMats[1].data(),0x30),next()%3 ? arrive : 8.f);
                            if(nm=="Destructor"){ dtorSub(); callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(); ret=Name(r); }
                            else if(nm=="MakeAbortable"){ At<CTask*>(o,8)=Fake(0); ret=callAbort(p0,int32(next()%3),pickEvent()); for(auto* h:hooks)h->State(false); }
                            else if(nm=="CalculateSearchPositionAndRanges"){ callPedVoid(p0); }
                            else if(nm=="CreateSubTask"){
                                static const int32 ts[]{TASK_SIMPLE_FIGHT_CTRL,TASK_COMPLEX_SEEK_ENTITY,TASK_SIMPLE_PAUSE,TASK_SIMPLE_PAUSE,TASK_SIMPLE_STAND_STILL,
                                                        TASK_COMPLEX_SEQUENCE,TASK_COMPLEX_SEQUENCE,TASK_FINISHED,TASK_SIMPLE_GUN_CTRL};
                                auto* r=callSub(ts[next()%std::size(ts)],p0); collectFresh(); ret=Name(r);
                            }
                            else {
                                static const int32 tn[]{TASK_COMPLEX_SEEK_ENTITY,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_SEQUENCE,TASK_SIMPLE_PAUSE,TASK_SIMPLE_PAUSE,
                                                        TASK_SIMPLE_FIGHT_CTRL,TASK_SIMPLE_FIGHT_CTRL,TASK_FINISHED,TASK_SIMPLE_GUN_CTRL};
                                static const int32 tc[]{TASK_COMPLEX_SEQUENCE,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_PAUSE,TASK_COMPLEX_SEEK_ENTITY,TASK_SIMPLE_FIGHT_CTRL,
                                                        TASK_SIMPLE_FIGHT_CTRL,TASK_SIMPLE_GUN_CTRL};
                                if(nm=="ControlSubTask")setSub(tc,std::size(tc)); else setSub(tn,std::size(tn));
                                auto* r=callTask(p0); collectFresh(); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x80,true);
                            }
                            finish(0x38); pedOut();
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexLeaveCarAsPassengerWait") {
                        // +0xC vehiculo (referencia)
                        auto* v0=vehs[0].data();
                        // a veces el peaton 0 es el primer pasajero
                        if(next()%3==0){ const int s=int(next()%4); for(int k=0;k<s;++k)At<CPed*>(v0,V_PASS+4*k)=nullptr; At<CPed*>(v0,V_PASS+4*s)=p0; if(v0[V_MAXPASS]<=s&&next()%2)v0[V_MAXPASS]=uint8(s+1+next()%3); }
                        if(nm=="Constructor"){
                            prefill(0x10); CVehicle* v=next()%4 ? Veh(0) : nullptr;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*)>(target)(o,v));
                            ctorDone(0x10);
                        } else {
                            setTask(VT_WAIT,0x10);
                            const bool mayNull=nm=="Destructor"||nm=="Clone"||nm=="CreateFirstSubTask"||nm=="CreateNextSubTask";
                            CVehicle* v=(!mayNull||next()%5) ? Veh(0) : nullptr; At<CVehicle*>(o,0xC)=v; Reg(v,o+0xC);
                            if(nm=="Destructor"){ dtorSub(); callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(); ret=Name(r); }
                            else if(nm=="GetNextPassengerToLeave"){ ret=Name(reinterpret_cast<CPed*(__thiscall*)(void*)>(target)(o)); for(auto* h:hooks)h->State(false); }
                            else if(nm=="SetPedInZerothPassengerSeat"){ callPedVoid(Ped(int(next()%NPEDS))); for(auto* h:hooks)h->State(false); }
                            else if(nm=="CreateSubTask"){
                                static const int32 ts[]{TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT,TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_CAR_DRIVE,
                                                        TASK_FINISHED,TASK_SIMPLE_STAND_STILL};
                                auto* r=callSub(ts[next()%std::size(ts)],p0); collectFresh(); ret=Name(r);
                            }
                            else {
                                static const int32 tn[]{TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_STAND_STILL};
                                static const int32 tc[]{TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_CAR_DRIVE,TASK_SIMPLE_CAR_DRIVE,TASK_COMPLEX_LEAVE_CAR};
                                if(nm=="ControlSubTask")setSub(tc,std::size(tc)); else setSub(tn,std::size(tn));
                                auto* r=callTask(p0); collectFresh(); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x80,true);
                            }
                            finish(0x10);
                        }
                        out["refs"]=AllRefs();
                    }
                    for(auto* h:hooks)h->State(false);
                    // desviacion FIX_BUGS documentada: el constructor de CTaskComplexDestroyCarArmed del EXE registra la referencia del
                    // vehiculo en su parametro de la pila ("stack"); la DLL con FIX_BUGS registra el campo +0x10 (object+16 o T0+16)
                    if(cls=="CTaskComplexDestroyCarArmed"&&(nm=="Constructor"||nm=="Clone")&&out.contains("refs")){
                        const std::string field=nm=="Constructor" ? "object+16" : "T0+16";
                        for(auto& l:out["refs"])for(auto& r:l)if(r.is_string()&&(r=="stack"||r==field))r="veh_ref";
                    }
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
