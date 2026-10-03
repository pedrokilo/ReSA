#pragma once
// Banco dentro del motor: tareas complejas (lista de 43, 40 funciones).
// CTaskComplexAttractorPartnerWait, CTaskComplexEnterAnyCarAsDriver y CTaskComplexSeekEntityAnyMeansXYOffset (Clone,
// ControlSubTask, CreateFirstSubTask, CreateNextSubTask, GetTaskType), CTaskComplexCarDriveMission (Clone, constructor,
// destructor, GetTaskType, SetUpCar), CTaskComplexCarDriveWander (Clone, constructor, CreateSubTaskCannotGetInCar,
// GetTaskType, SetUpCar), CTaskComplexDie (constructor, CreateFirstSubTask, CreateNextSubTask, MakeAbortable,
// SayDeathSample), CTaskComplexGetUpAndStandStill (constructor, ControlSubTask, CreateFirstSubTask, CreateNextSubTask,
// CreateSubTask) y CTaskComplexObserveTrafficLightsAndAchieveHeading (Clone, ControlSubTask, CreateFirstSubTask,
// CreateNextSubTask, MakeAbortable).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// peatones, vehiculos, inteligencia (escaner de vehiculos), par de efectos y tareas falsas (vtable trazada).
// Se simulan y trazan: CPed::Say, CPed::SetPedState, CPedIntelligence::ClearTasks, CLocalisation::Blood,
// CAEPedAudioEntity::AddAudioEvent, CCarCtrl::JoinCarWithRoadSystem, CCarEnterExit::IsVehicleStealable,
// CTaskComplexWander::GetWanderTaskByPedType, CEntitySeekPosCalculatorXYOffset::ComputeEntitySeekPos y el RNG.
// Pool de tareas: m_LastFreeSlot y estados se restauran y los slots libres se rellenan con 0xCD en cada ejecucion.
// CTimer::m_snTimeInMilliseconds, CCarCtrl::bCarIsBeingCreated y CTaskSequences::ms_taskSequence (contador de
// referencias que incrementa CTaskComplexUseSequence) se guardan y se restauran. Se compara la memoria de los
// fixtures (punteros traducidos a nombres), las tareas nuevas del pool, las referencias, el retorno y la traza.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Tasks/Task.h"
#include "Tasks/TaskSequences.h"
#include "Tasks/TaskComplexSequence.h"
#include "TaskComplexAttractorPartnerWait.h"
#include "TaskComplexCarDriveMission.h"
#include "TaskComplexDriveWander.h"
#include "TaskComplexDie.h"
#include "TaskComplexEnterAnyCarAsDriver.h"
#include "TaskComplexGetUpAndStandStill.h"
#include "TaskComplexObserveTrafficLightsAndAchieveHeading.h"
#include "TaskComplexWander.h"
#include "Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.h"
#include "Scripted2dEffects.h"
#include "CarCtrl.h"
#include "CarEnterExit.h"
#include "Localisation.h"
#include "PedIntelligence.h"
#include "Audio/Entities/AEPedAudioEntity.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks43Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }

constexpr int NPEDS=4, NVEH=6, FAKES=4;
constexpr size_t FAKE_SIZE=0x50, FK_ABORT=0x40, FK_TYPE=0x48, FK_CLONE=0x4C;
// desplazamientos crudos del EXE
constexpr size_t E_POS=4, E_MATRIX=0x14, E_TYPE=0x36, E_STATUS=0x36;                          // CEntity
constexpr size_t P_FLAGS=0x46C, P_INTEL=0x47C, P_VEHICLE=0x58C, P_PEDTYPE=0x598;               // CPed
constexpr size_t V_AUTOPILOT=0x390, V_AP_SIZE=0xA0, V_SUBTYPE=0x594;                          // CVehicle (autopiloto + flags 0x428..0x42F)
constexpr size_t I_SCANNER=0xD4, SC_ENTITIES=0xC, SC_CLOSEST=0x4C;                             // CPedIntelligence / CEntityScanner
constexpr uintptr_t TIMER=0xB7CB84, CAR_CREATED=0x9690CC, SEQS=0xC178F0; constexpr size_t SEQ_SIZE=0x40, NSEQ=64;
constexpr uintptr_t VT_APW=0x86E284, VT_MISSION=0x86E9A4, VT_WANDER=0x86E96C, VT_DIE=0x86DE78, VT_EAC=0x86EA14,
                    VT_GETUP=0x870404, VT_OTL=0x86DFC4, VT_SEEK=0x870CF0, VT_XYCALC=0x86F8F8;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<uint8,0x100> object{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<uint8,0x40> pairBuf{};
alignas(16) inline std::array<uint8,0x40> event{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline uintptr_t dllCalcVt{};

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

inline std::array<void*,12> fakeVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt";
    if(v==VT_XYCALC||(dllCalcVt&&v==dllCalcVt))return "xy_calc_vt";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(intel.data(),intel.size(),"intel"))return *s;
    if(auto s=in(pairBuf.data(),pairBuf.size(),"pair"))return *s; if(auto s=in(event.data(),event.size(),"event"))return *s;
    if(auto s=in(reinterpret_cast<void*>(SEQS),SEQ_SIZE*NSEQ,"seq"))return *s;
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); for(auto& v:vehs)r.push_back(RefList(v.data())); return r; }

// tarea serializada sin vptr (se compara el tipo)
inline json SerTask(const void* p,int depth,size_t bytes=TASK_SLOT) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType(); const auto* w=static_cast<const uint32*>(p);
    json words=json::array();
    for(size_t k=1;k<bytes/4;++k){ const uint32 v=w[k];
        if(depth>=0&&depth<3&&IsPoolTask(v)&&!freshIds.contains(v))words.push_back(SerTask(reinterpret_cast<const void*>(uintptr_t(v)),depth+1)); else words.push_back(Name(v)); }
    return {{"type",type},{"words",words}};
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{}, calcCalls{};
inline uint8 bloodRet{}; inline uint32 stealMask{}; inline uintptr_t wanderRet{};

// tareas falsas
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(t)}); return At<CTask*>(t,FK_CLONE); }
inline CTask* __fastcall FSub(CTask*,void*) { return nullptr; }
inline bool __fastcall FSimple(CTask*,void*) { return true; }
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) { trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e)}); return At<uint8>(t,FK_ABORT)!=0; }
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }

// dependencias simuladas
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline int16 __fastcall FakeSay(CPed* p,void*,int ctx,uint32 delay,float prob,bool overrideSilence,bool forceAudible,bool frontEnd) {
    trace.push_back({"say",Name(p),ctx,delay,F(prob),overrideSilence,forceAudible,frontEnd}); return 0;
}
inline void __fastcall FakeAudio(void* ae,void*,int32 ev,float vol,float speed,void* phys,uint32 surf,int32 a7,uint32 maxVol) {
    trace.push_back({"add_audio_event",Name(ae),ev,F(vol),F(speed),Name(phys),surf&0xFF,a7,maxVol});
}
inline bool __cdecl FakeBlood() { trace.push_back({"blood",bloodRet}); return bloodRet!=0; }
inline void __fastcall FakeSetPedState(CPed* p,void*,int32 s) { trace.push_back({"set_ped_state",Name(p),s}); }
inline void __fastcall FakeClearTasks(void* in,void*,uint32 a,uint32 b) { trace.push_back({"clear_tasks",Name(in),a&0xFF,b&0xFF}); }
inline void __cdecl FakeJoin(CVehicle* v) { trace.push_back({"join_road_system",Name(v)}); }
inline int VehIndex(const void* v) { for(int n=0;n<NVEH;++n)if(v==vehs[n].data())return n; return -1; }
inline bool __cdecl FakeStealable(const CVehicle* v,const CPed* p) { const int n=VehIndex(v); const bool r=n>=0&&((stealMask>>n)&1); trace.push_back({"is_vehicle_stealable",Name(v),Name(p),r}); return r; }
inline CTaskComplexWander* __cdecl FakeWanderByType(CPed* p) { trace.push_back({"wander_task_by_ped_type",Name(p)}); return reinterpret_cast<CTaskComplexWander*>(wanderRet); }
inline void __fastcall FakeCompute(void* calc,void*,const CPed* seeker,const CEntity* target,CVector* out) {
    trace.push_back({"compute_seek_pos",Name(calc),Name(seeker),Name(target)});
    const uint32 c=calcCalls++; *out=CVector{float(int(seed%97))-48.5f+0.25f*float(c),float(int(seed%89))*0.5f-20.f,float(int(seed%13))+0.125f};
}

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t OurVirtual(const char* cls,const char* fn){ auto* h=FindHook(cls,fn); if(!h||h->Symbol()[0]!='V')return 0; return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress()); }
inline uintptr_t Follow(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=40;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexAttractorPartnerWait","Clone",0,0x86E284,1,256},
    {"CTaskComplexAttractorPartnerWait","ControlSubTask",0,0x86E284,10,64},
    {"CTaskComplexAttractorPartnerWait","CreateFirstSubTask",0,0x86E284,9,256},
    {"CTaskComplexAttractorPartnerWait","CreateNextSubTask",0,0x86E284,8,64},
    {"CTaskComplexAttractorPartnerWait","GetTaskType",0,0x86E284,4,64},
    {"CTaskComplexCarDriveMission","Clone",0,0x86E9A4,1,256},
    {"CTaskComplexCarDriveMission","Constructor",0x63CC30,0,-1,256},
    {"CTaskComplexCarDriveMission","Destructor",0x63CCC0,0,-1,256},
    {"CTaskComplexCarDriveMission","GetTaskType",0,0x86E9A4,4,64},
    {"CTaskComplexCarDriveMission","SetUpCar",0,0x86E9A4,11,512},
    {"CTaskComplexCarDriveWander","Clone",0x63DD00,0x86E96C,1,256},
    {"CTaskComplexCarDriveWander","Constructor",0x63CB10,0,-1,256},
    {"CTaskComplexCarDriveWander","CreateSubTaskCannotGetInCar",0x643240,0x86E96C,12,256},
    {"CTaskComplexCarDriveWander","GetTaskType",0x63CB40,0x86E96C,4,64},
    {"CTaskComplexCarDriveWander","SetUpCar",0x63CB60,0x86E96C,11,512},
    {"CTaskComplexDie","Constructor",0x630040,0,-1,512},
    {"CTaskComplexDie","CreateFirstSubTask",0,0x86DE78,9,1024},
    {"CTaskComplexDie","CreateNextSubTask",0,0x86DE78,8,512},
    {"CTaskComplexDie","MakeAbortable",0,0x86DE78,6,256},
    {"CTaskComplexDie","SayDeathSample",0x630100,0,-1,256},
    {"CTaskComplexEnterAnyCarAsDriver","Clone",0,0x86EA14,1,64},
    {"CTaskComplexEnterAnyCarAsDriver","ControlSubTask",0,0x86EA14,10,64},
    {"CTaskComplexEnterAnyCarAsDriver","CreateFirstSubTask",0,0x86EA14,9,1024},
    {"CTaskComplexEnterAnyCarAsDriver","CreateNextSubTask",0,0x86EA14,8,64},
    {"CTaskComplexEnterAnyCarAsDriver","GetTaskType",0,0x86EA14,4,64},
    {"CTaskComplexGetUpAndStandStill","Constructor",0x678130,0,-1,64},
    {"CTaskComplexGetUpAndStandStill","ControlSubTask",0,0x870404,10,64},
    {"CTaskComplexGetUpAndStandStill","CreateFirstSubTask",0,0x870404,9,64},
    {"CTaskComplexGetUpAndStandStill","CreateNextSubTask",0,0x870404,8,256},
    {"CTaskComplexGetUpAndStandStill","CreateSubTask",0x678170,0,-1,256},
    {"CTaskComplexObserveTrafficLightsAndAchieveHeading","Clone",0,0x86DFC4,1,256},
    {"CTaskComplexObserveTrafficLightsAndAchieveHeading","ControlSubTask",0,0x86DFC4,10,64},
    {"CTaskComplexObserveTrafficLightsAndAchieveHeading","CreateFirstSubTask",0,0x86DFC4,9,128},
    {"CTaskComplexObserveTrafficLightsAndAchieveHeading","CreateNextSubTask",0,0x86DFC4,8,256},
    {"CTaskComplexObserveTrafficLightsAndAchieveHeading","MakeAbortable",0,0x86DFC4,6,256},
    {"CTaskComplexSeekEntityAnyMeansXYOffset","Clone",0,0x870CF0,1,512},
    {"CTaskComplexSeekEntityAnyMeansXYOffset","ControlSubTask",0,0x870CF0,10,1024},
    {"CTaskComplexSeekEntityAnyMeansXYOffset","CreateFirstSubTask",0,0x870CF0,9,512},
    {"CTaskComplexSeekEntityAnyMeansXYOffset","CreateNextSubTask",0,0x870CF0,8,64},
    {"CTaskComplexSeekEntityAnyMeansXYOffset","GetTaskType",0,0x870CF0,4,64},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks43-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks43-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks43");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks43-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    const uintptr_t dllCompute=OurVirtual("CEntitySeekPosCalculatorXYOffset","ComputeEntitySeekPos");
    if(!dllCompute){Write({{"event","error"},{"reason","hook not found"},{"function","CEntitySeekPosCalculatorXYOffset::ComputeEntitySeekPos"}});return;}
    { CEntitySeekPosCalculatorXYOffset tmp{}; dllCalcVt=*reinterpret_cast<const uintptr_t*>(&tmp); }
    FillTraps(std::make_integer_sequence<int,12>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{CAR_CREATED,1},{SEQS,SEQ_SIZE*NSEQ}};
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
        EntryPatch s2(bit_cast<uintptr_t>(&CAEPedAudioEntity::AddAudioEvent),FakeAudio), s2e(0x4E2BB0,FakeAudio);
        EntryPatch s3(bit_cast<uintptr_t>(&CLocalisation::Blood),FakeBlood), s3e(0x56D230,FakeBlood);
        EntryPatch s4(bit_cast<uintptr_t>(&CPed::SetPedState),FakeSetPedState), s4e(0x5E4500,FakeSetPedState);
        EntryPatch s5(bit_cast<uintptr_t>(&CPedIntelligence::ClearTasks),FakeClearTasks), s5e(0x601420,FakeClearTasks);
        EntryPatch s6(bit_cast<uintptr_t>(&CCarCtrl::JoinCarWithRoadSystem),FakeJoin), s6e(0x42F5A0,FakeJoin);
        EntryPatch s7(bit_cast<uintptr_t>(&CCarEnterExit::IsVehicleStealable),FakeStealable), s7e(0x6510D0,FakeStealable);
        EntryPatch s8(bit_cast<uintptr_t>(&CTaskComplexWander::GetWanderTaskByPedType),FakeWanderByType), s8e(0x673D00,FakeWanderByType);
        EntryPatch s9(Follow(dllCompute),FakeCompute), s9e(0x6946A0,FakeCompute);

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
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x4343u; if(!st)st=1; trace=json::array(); seed=uint32(i); rngCalls=0; calcCalls=0; freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    const uint32 now=At<uint32>(reinterpret_cast<void*>(TIMER),0);
                    std::memset(intel.data(),0,intel.size());
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size());
                        p[E_TYPE]=3; At<void*>(p,P_INTEL)=intel.data();
                        if(n==0||next()%4){ At<void*>(p,E_MATRIX)=pedMats[n].data();
                            At<CVector>(pedMats[n].data(),0x0)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            At<CVector>(pedMats[n].data(),0x10)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            At<CVector>(pedMats[n].data(),0x30)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)}; }
                        else At<CVector>(p,E_POS)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        { static const int32 types[]{0,1,4,5,7,8,6}; At<int32>(p,P_PEDTYPE)=types[next()%7]; }
                    }
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        v[E_TYPE]=uint8(2|(next()%32)<<3);
                        if(next()%5){ At<void*>(v,E_MATRIX)=vehMats[n].data(); At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)}; }
                        else At<CVector>(v,E_POS)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        for(size_t b=0;b<V_AP_SIZE;++b)v[V_AUTOPILOT+b]=uint8(next());
                        { static const int32 sub[]{0,0,3,4,9,5,6,1}; At<int32>(v,V_SUBTYPE)=sub[next()%8]; }
                    }
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,FAKE_SIZE); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_PAUSE+n; fakes[n][FK_ABORT]=uint8(next()%2); }
                    for(auto& b:pairBuf)b=uint8(next()); std::memset(event.data(),0,event.size());
                    std::memset(object.data(),0xCD,object.size());
                    bloodRet=uint8(next()%2); stealMask=next(); wanderRet=next()%4 ? uintptr_t(Fake(1)) : 0;
                    At<uint8>(reinterpret_cast<void*>(CAR_CREATED),0)=uint8(next()%3==0);

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
                    auto setSub=[&](){ At<CTask*>(o,8)=next()%4 ? Fake(0) : nullptr; };
                    auto typeOf=[&](void* p){ return reinterpret_cast<CTask*>(p)->GetTaskType(); };
                    auto callTask=[&](CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    auto pickEnt=[&]()->CEntity*{ switch(next()%5){ case 0: return nullptr; case 1: return Ped(2); case 2: return Ped(1); default: return Veh(1+next()%2); } };
                    auto speedVal=[&]{ const uint32 m=next()%10; return m==0 ? -uni(0.f,300.f) : m==1 ? uni(255.f,600.f) : m==2 ? float(next()%256) : uni(0.f,60.f); };
                    // CTaskComplexCarDrive (+0x0C vehiculo, +0x10 velocidad, +0x14 modelo, +0x18 estilo, +0x1C conductor, +0x1D..0x1F originales, +0x20 preparado)
                    auto setCarDrive=[&](uintptr_t vtbl,size_t size,bool needVeh){
                        setTask(vtbl,size); At<CVehicle*>(o,0xC)=needVeh||next()%5 ? Veh(0) : nullptr; At<float>(o,0x10)=speedVal();
                        At<int32>(o,0x14)=next()%3 ? -1 : int32(400+next()%200); At<int32>(o,0x18)=int32(next()%6); o[0x1C]=uint8(next()%2); o[0x20]=uint8(next()%3==0);
                    };
                    auto autopilots=[&]{ json ap=json::array(); for(int n=0;n<3;++n){ ap.push_back(Region(vehs[n].data()+V_AUTOPILOT,V_AP_SIZE)); ap.push_back(vehs[n][E_STATUS]); } out["autopilot"]=ap; };
                    // CTaskComplexDie: arma, grupo, animacion, mezcla, velocidad, flags (sigilo/caida/barandilla) y direccion
                    auto setDie=[&]{
                        setTask(VT_DIE,0x28);
                        { static const int32 w[]{49,50,51,52,53,54,53,53,54,0,22,31,37,46}; At<int32>(o,0xC)=w[next()%14]; }
                        At<int32>(o,0x10)=int32(next()%40); At<int32>(o,0x14)=int32(next()%300); At<float>(o,0x18)=uni(0.f,8.f); At<float>(o,0x1C)=uni(0.f,2.f);
                        At<uint32>(o,0x20)=next(); At<int32>(o,0x24)=int32(next()%4);
                        auto& fl=At<uint32>(p0,P_FLAGS); fl=next()%2 ? (fl|0x100u) : (fl&~0x100u);
                        if(next()%3==0)At<uint8>(p0,0x478)|=0x80; else At<uint8>(p0,0x478)&=0x7F;
                        At<CVehicle*>(p0,P_VEHICLE)=next()%4 ? Veh(0) : nullptr;
                    };
                    auto pedOut=[&]{ out["ped_flags"]=Region(reinterpret_cast<uint8*>(p0)+P_FLAGS,0x14); };

                    if(nm=="GetTaskType") {
                        setTask(fn.vt,0x40);
                        ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        out["object"]=Region(o,0x40,true);
                    }
                    else if(nm=="ControlSubTask"&&cls!="CTaskComplexSeekEntityAnyMeansXYOffset") {
                        setTask(fn.vt,0x40); setSub();
                        ret=Name(callTask(p0)); out["object"]=Region(o,0x40,true);
                    }
                    else if(nm=="CreateNextSubTask"&&(cls=="CTaskComplexAttractorPartnerWait"||cls=="CTaskComplexEnterAnyCarAsDriver"||cls=="CTaskComplexSeekEntityAnyMeansXYOffset")) {
                        setTask(fn.vt,0x40); setSub();
                        auto* r=callTask(p0); collectFresh(r); ret=Name(r); out["object"]=Region(o,0x40,true);
                    }
                    else if(cls=="CTaskComplexAttractorPartnerWait") {
                        // par de efectos: Effect, WaitingTask (indice de secuencia), PartnerTask, modo
                        for(int k=0;k<2;++k)At<int32>(pairBuf.data(),0x10*k+4)=int32(next()%NSEQ);
                        setTask(VT_APW,0x14); o[0xC]=uint8(next()%2); At<void*>(o,0x10)=pairBuf.data();
                        auto* r=nm=="Clone" ? reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o) : callTask(p0);
                        collectFresh(r); ret=Name(r); out["object"]=Region(o,0x20,true);
                        json rc=json::array(); for(int k=0;k<2;++k){ const int32 id=At<int32>(pairBuf.data(),0x10*k+4); rc.push_back(At<uint32>(reinterpret_cast<void*>(SEQS+id*SEQ_SIZE),0x3C)-At<uint32>(saved[2].data(),id*SEQ_SIZE+0x3C)); } out["seq_refs"]=rc;
                    }
                    else if(cls=="CTaskComplexCarDriveMission"||cls=="CTaskComplexCarDriveWander") {
                        const bool mission=cls=="CTaskComplexCarDriveMission";
                        if(nm=="Constructor"){
                            CVehicle* v=next()%5 ? Veh(0) : nullptr; const int32 style=int32(next()%6); const float sp=speedVal();
                            if(mission){ CEntity* tg=pickEnt(); const int32 mis=int32(next()%60);
                                ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,CEntity*,int32,int32,float)>(target)(o,v,tg,mis,style,sp)); }
                            else ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32,float)>(target)(o,v,style,sp));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x40,true); out["type"]=typeOf(o);
                        } else {
                            setCarDrive(mission ? VT_MISSION : VT_WANDER,0x40,nm=="SetUpCar");
                            if(mission){ At<CEntity*>(o,0x24)=pickEnt(); At<int32>(o,0x28)=int32(next()%60); }
                            if(nm=="Destructor"){
                                At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; Reg(At<CEntity*>(o,0xC),o+0xC); Reg(At<CEntity*>(o,0x24),o+0x24);
                                reinterpret_cast<void(__thiscall*)(void*)>(target)(o);
                            } else if(nm=="SetUpCar"){
                                reinterpret_cast<void(__thiscall*)(void*)>(target)(o);
                            } else if(nm=="Clone"){
                                auto* r=reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); collectFresh(r); ret=Name(r);
                            } else { // CreateSubTaskCannotGetInCar
                                auto* r=callTask(next()%4 ? p0 : Ped(1)); collectFresh(r); ret=Name(r);
                            }
                            out["object"]=Region(o,0x30,true);
                        }
                        autopilots(); out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexDie") {
                        if(nm=="Constructor"){
                            static const int32 w[]{49,50,51,53,54,0,22}; const int32 wt=w[next()%7]; const int32 grp=int32(next()%40), anim=int32(next()%300);
                            const float bl=uni(0.f,8.f), spd=uni(0.f,2.f);
                            auto bv=[&]{ const uint32 b=next()%2; return next()%4==0 ? (next()&0xFFFFFF00u)|b : b; };
                            const uint32 stealth=bv(), fall=bv(), rail=bv(); const int32 d=next()%4 ? int32(next()%4) : int32(next());
                            if(next()%2)std::memset(o,0,0x30);
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,int32,int32,float,float,uint32,uint32,int32,uint32)>(target)(o,wt,grp,anim,bl,spd,stealth,fall,d,rail));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x30,true); out["type"]=typeOf(o);
                        } else if(nm=="SayDeathSample"){
                            setDie(); { const uint32 m=next()%4; if(m==0)At<int32>(o,0xC)=int32(next()%60); }
                            reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,p0);
                            out["object"]=Region(o,0x28,true);
                        } else if(nm=="MakeAbortable"){
                            setDie(); At<CTask*>(o,8)=Fake(0);
                            const int32 prio=int32(next()%4); const CEvent* ev=next()%2 ? reinterpret_cast<const CEvent*>(event.data()) : nullptr;
                            ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,p0,prio,ev);
                            out["object"]=Region(o,0x28,true);
                        } else {
                            setDie();
                            if(nm=="CreateNextSubTask"){
                                At<CTask*>(o,8)=Fake(0);
                                static const int32 types[]{TASK_SIMPLE_DIE,TASK_SIMPLE_DROWN,0xD6,TASK_COMPLEX_DIE_IN_CAR,TASK_SIMPLE_DROWN_IN_CAR,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_PAUSE,0xD3,0xD9};
                                At<int32>(fakes[0].data(),FK_TYPE)=types[next()%10];
                            }
                            auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                            out["object"]=Region(o,0x28,true);
                        }
                        pedOut(); out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexEnterAnyCarAsDriver") {
                        setTask(VT_EAC,0x10);
                        if(nm=="Clone"){ auto* r=reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); collectFresh(r); ret=Name(r); }
                        else {
                            auto* sc=intel.data()+I_SCANNER;
                            At<CVehicle*>(sc,SC_CLOSEST)=next()%3 ? Veh(next()%NVEH) : nullptr;
                            const uint32 fill=next()%4;
                            for(int k=0;k<16;++k)At<CVehicle*>(sc,SC_ENTITIES+4*k)=(fill==0||next()%3==0) ? nullptr : Veh(next()%NVEH);
                            if(const int ci=VehIndex(At<CVehicle*>(sc,SC_CLOSEST));ci>=0&&next()%4==0)stealMask&=~(1u<<ci);
                            // empates de distancia y NaN ocasionales
                            if(next()%4==0){ auto* a=vehs[1].data(); auto* b=vehs[2].data(); At<void*>(b,E_MATRIX)=At<void*>(a,E_MATRIX) ? (void*)vehMats[2].data() : nullptr;
                                const CVector pa=reinterpret_cast<CVehicle*>(a)->GetPosition(); if(At<void*>(b,E_MATRIX))At<CVector>(vehMats[2].data(),0x30)=pa; else At<CVector>(b,E_POS)=pa; }
                            if(next()%16==0){ auto* a=vehs[3].data(); if(At<void*>(a,E_MATRIX))At<float>(vehMats[3].data(),0x34)=qnan; else At<float>(a,E_POS+4)=qnan; }
                            auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                        }
                        out["object"]=Region(o,0x10,true); out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexGetUpAndStandStill") {
                        if(nm=="Constructor"){
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x20,true); out["type"]=typeOf(o);
                        } else {
                            setTask(VT_GETUP,0x10); At<CTask*>(o,8)=Fake(0);
                            CTask* r{};
                            if(nm=="CreateSubTask"){ static const int32 types[]{TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_GET_UP,TASK_FINISHED,TASK_SIMPLE_STAND_STILL+1,0,TASK_SIMPLE_GET_UP,2000};
                                r=reinterpret_cast<CTask*(__thiscall*)(void*,int32)>(target)(o,types[next()%7]); }
                            else {
                                static const int32 types[]{TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_GET_UP,TASK_SIMPLE_GET_UP,TASK_SIMPLE_PAUSE,TASK_SIMPLE_STAND_STILL+1};
                                At<int32>(fakes[0].data(),FK_TYPE)=types[next()%5]; fakes[0][8]=uint8(next()%2);
                                r=callTask(p0);
                            }
                            collectFresh(r); ret=Name(r); out["object"]=Region(o,0x10,true);
                        }
                    }
                    else if(cls=="CTaskComplexObserveTrafficLightsAndAchieveHeading") {
                        setTask(VT_OTL,0x14); At<int32>(o,0xC)=int32(next()%20000)-1000; At<float>(o,0x10)=uni(-7.f,7.f); At<CTask*>(o,8)=Fake(0);
                        if(nm=="MakeAbortable"){
                            const int32 prio=int32(next()%4); const CEvent* ev=next()%2 ? reinterpret_cast<const CEvent*>(event.data()) : nullptr;
                            ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,p0,prio,ev);
                        } else {
                            if(nm=="CreateNextSubTask"){ static const int32 types[]{TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS,TASK_SIMPLE_ACHIEVE_HEADING,TASK_SIMPLE_PAUSE,TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS_AND_ACHIEVE_HEADING};
                                At<int32>(fakes[0].data(),FK_TYPE)=types[next()%4]; }
                            auto* r=nm=="Clone" ? reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o) : callTask(p0);
                            collectFresh(r); ret=Name(r);
                        }
                        out["object"]=Region(o,0x14,true);
                    }
                    else if(cls=="CTaskComplexSeekEntityAnyMeansXYOffset") {
                        // +0x0C entidad, +0x10 periodo, +0x14 radio, +0x18 temporizador, +0x24 calculador (vtable del EXE) y desplazamiento
                        setTask(VT_SEEK,0x34);
                        CEntity* ent=pickEnt(); if(!ent&&nm!="ControlSubTask")ent=Ped(2);
                        At<CEntity*>(o,0xC)=ent; At<int32>(o,0x10)=next()%8==0 ? -int32(next()%100) : int32(next()%3000);
                        At<float>(o,0x14)=uni(0.f,20.f);
                        At<uint32>(o,0x18)=now-next()%4000; At<int32>(o,0x1C)=next()%8==0 ? -1 : int32(next()%4000);
                        o[0x20]=uint8(next()%4!=0); o[0x21]=uint8(next()%4==0);
                        At<uintptr_t>(o,0x24)=VT_XYCALC; At<CVector>(o,0x28)=CVector{uni(-5.f,5.f),uni(-5.f,5.f),uni(-1.f,1.f)};
                        At<CTask*>(o,8)=Fake(0);
                        CTask* r{};
                        if(nm=="Clone")r=reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o);
                        else r=callTask(next()%4 ? p0 : Ped(1));
                        collectFresh(r); ret=Name(r);
                        out["object"]=Region(o,0x34,true); out["sub"]=Region(fakes[0].data(),0x30,true); out["refs"]=AllRefs();
                    }
                    for(auto* h:hooks)h->State(false);
                    // limpieza: tareas nuevas alcanzables (raices) y referencias; despues el pool debe quedar como antes
                    const size_t traceBefore=trace.size();
                    for(auto* t:roots){ if(t->GetTaskType()==TASK_COMPLEX_USE_SEQUENCE)At<int32>(t,0xC)=-1; delete t; } // sin vaciar la secuencia real
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
