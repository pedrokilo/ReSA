#pragma once
// Banco dentro del motor: CTaskComplexUseGoggles (5), CTaskComplexUseAttractor (7),
// CTaskComplexUseAttractorPartner (7), CTaskComplexUseEffect (8) y CTaskComplexUsePairedAttractor (11).
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos: peatones con
// inteligencia/datos de jugador falsos, atractor y evento con vtable propia, subtarea falsa, pares de
// efectos falsos. El gestor de atractores, CScripted2dEffects, CScriptsForBrains, el buscador de
// tareas, las gafas, el audio, el buscado y el RNG se simulan y se trazan. Se compara la memoria de los
// fixtures (punteros traducidos), las tareas creadas, las referencias, el retorno y la traza.
#include <fstream>
#include <unordered_map>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexUseGoggles.h"
#include "TaskComplexUseAttractor.h"
#include "TaskComplexUseAttractorPartner.h"
#include "TaskComplexUseEffect.h"
#include "TaskComplexUsePairedAttractor.h"
#include "TaskSimplePlayerOnFoot.h"
#include "Attractors/PedAttractor.h"
#include "PedAttractorManager.h"
#include "Scripts/Scripted2dEffects.h"
#include "ScriptsForBrains.h"
#include "Tasks/ScriptedBrainTaskStore.h"
#include "InterestingEvents.h"
#include "Audio/Entities/AEPedWeaponAudioEntity.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace Tasks3Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
constexpr int PEDS=3;
alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,PEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,PEDS> mats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,PEDS> intel{};
alignas(16) inline std::array<std::array<uint8,0x40>,PEDS> ptask{};
alignas(16) inline std::array<uint8,0x100> playerData{};
alignas(16) inline std::array<uint8,sizeof(CObject)> objectEnt{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<uint8,0x100> fake{}, fake2{};
alignas(16) inline std::array<uint8,0x40> fx{}, event{};
alignas(16) inline std::array<uint8,0x100> attractor{};
alignas(16) inline std::array<uint8,sizeof(CScriptedEffectPairs)> pairs{};
alignas(16) inline std::array<uint8,sizeof(CWanted)> wanted{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CEntity* Obj() { return reinterpret_cast<CEntity*>(objectEnt.data()); }
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<PEDS;++n){ const auto k="#"+std::to_string(n);
        if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+k; if(auto s=in(mats[n].data(),mats[n].size(),"mat"))return *s+k;
        if(auto s=in(intel[n].data(),intel[n].size(),"intel"))return *s+k; if(auto s=in(ptask[n].data(),ptask[n].size(),"ptask"))return *s+k; }
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(fake.data(),fake.size(),"fake"))return *s; if(auto s=in(fake2.data(),fake2.size(),"fake2"))return *s;
    if(auto s=in(fx.data(),fx.size(),"fx"))return *s; if(auto s=in(event.data(),event.size(),"event"))return *s; if(auto s=in(attractor.data(),attractor.size(),"attractor"))return *s;
    if(auto s=in(pairs.data(),pairs.size(),"pairs"))return *s; if(auto s=in(objectEnt.data(),objectEnt.size(),"objent"))return *s;
    if(auto s=in(playerData.data(),playerData.size(),"pdata"))return *s; if(auto s=in(wanted.data(),wanted.size(),"wanted"))return *s;
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
inline std::string PedHash(int n) { auto copy=peds[n]; const auto off=uintptr_t(&Ped(n)->m_pReferences)-uintptr_t(peds[n].data()); std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size()); }
inline int PedIndexOfIntel(const void* p) { for(int n=0;n<PEDS;++n)if(uintptr_t(p)>=uintptr_t(intel[n].data())&&uintptr_t(p)<uintptr_t(intel[n].data())+intel[n].size())return n; return -1; }

// configuracion por caso de las dependencias simuladas
inline int fakeType{}, attrType{}, eventType{}, scriptIndex{}; inline bool abortRet{}, brainLoaded{}, copJoins{}, departRet{};
inline uintptr_t relevantRet{}, registerRet{}, taskForPedRet{}, findTaskRet{};
inline std::array<uint8,PEDS> findMask{}; inline std::array<int8,NUM_SCRIPTED_2D_EFFECTS> pedUsing{};
inline int seed{}, rngCalls{};

// subtarea falsa
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(uintptr_t(t)),f}); return t; }
inline CTask* __fastcall FSub(CTask*,void*) { return nullptr; }
inline bool __fastcall FSimple(CTask*,void*) { return false; }
inline int __fastcall FType(CTask* t,void*) { trace.push_back({"get_task_type",Name(uintptr_t(t))}); return fakeType; }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int prio,const CEvent* e) { trace.push_back({"make_abortable",Name(uintptr_t(t)),Name(uintptr_t(p)),prio,Name(uintptr_t(e))}); return abortRet; }
template<int N> void* __fastcall FTrap(CTask*,void*) { trace.push_back({"unexpected_slot",N}); return nullptr; }
inline std::array<void*,12> fakeVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// atractor y evento falsos
inline int __fastcall AType(void* a,void*) { trace.push_back({"attractor_get_type"}); return attrType; }
template<int N> void* __fastcall ATrap(void*,void*) { trace.push_back({"attractor_unexpected_slot",N}); return nullptr; }
inline std::array<void*,16> attrVtbl{};
template<int... N> void FillATraps(std::integer_sequence<int,N...>) { ((attrVtbl[N]=reinterpret_cast<void*>(&ATrap<N>)),...); }
inline int __fastcall EType(void*,void*) { trace.push_back({"event_get_type"}); return eventType; }
inline std::array<void*,16> eventVtbl{};

// dependencias
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((uint32(seed)*2654435761u+uint32(rngCalls++)*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline void __fastcall TakeOff(CPed* p,void*) { trace.push_back({"take_off_goggles",Name(uintptr_t(p))}); }
inline void __fastcall PutOn(CPed* p,void*) { trace.push_back({"put_on_goggles",Name(uintptr_t(p))}); }
inline void __fastcall WeaponEvent(void* e,void*,int id) { trace.push_back({"weapon_audio_event",Name(uintptr_t(e)),id}); }
inline CTask* __fastcall FindTask(void* in,void*,int type) { trace.push_back({"find_task_by_type",Name(uintptr_t(in)),type}); return reinterpret_cast<CTask*>(findTaskRet); }
inline int32 __fastcall Zelda(void* t,void*,CPlayerPed* p,bool b) { trace.push_back({"player_control_zelda",Name(uintptr_t(t)),Name(uintptr_t(p)),b}); return 0; }
inline CTask* __fastcall FindActive(void* tm,void*,int type) {
    const int n=PedIndexOfIntel(tm); trace.push_back({"find_active_task",Name(uintptr_t(tm)),type}); if(n<0)return nullptr;
    const int bit=type==TASK_COMPLEX_USE_PAIRED_ATTRACTOR ? 0 : type==TASK_COMPLEX_USE_ATTRACTOR_PARTNER ? 1 : type==TASK_COMPLEX_ATTRACTOR_PARTNER_WAIT ? 2 : -1;
    if(bit<0||!(findMask[n]>>bit&1))return nullptr;
    return bit==0 ? reinterpret_cast<CTask*>(ptask[n].data()) : reinterpret_cast<CTask*>(fake2.data());
}
inline bool __fastcall Depart(void* m,void*,CPed* p,CPedAttractor* a) { trace.push_back({"broadcast_departure",Name(uintptr_t(p)),Name(uintptr_t(a))}); return departRet; }
inline bool __fastcall DeRegister(void* m,void*,CPed* p,CPedAttractor* a) { trace.push_back({"deregister_ped",Name(uintptr_t(p)),Name(uintptr_t(a))}); return departRet; }
inline const CPedAttractor* __fastcall Relevant(void* m,void*,const CPed* p,const C2dEffectPedAttractor* f,const CEntity* e) { trace.push_back({"get_relevant_attractor",Name(uintptr_t(p)),Name(uintptr_t(f)),Name(uintptr_t(e))}); return reinterpret_cast<const CPedAttractor*>(relevantRet); }
inline CPedAttractor* __fastcall Register(void* m,void*,CPed* p,C2dEffectPedAttractor* f,CEntity* e,int ms) { trace.push_back({"register_ped",Name(uintptr_t(p)),Name(uintptr_t(f)),Name(uintptr_t(e)),ms}); return reinterpret_cast<CPedAttractor*>(registerRet); }
inline CPed* __fastcall PedUsing(void* m,void*,const C2dEffectPedAttractor* f,const CEntity* e) {
    trace.push_back({"get_ped_using_effect",Name(uintptr_t(f)),Name(uintptr_t(e))});
    const auto idx=(intptr_t(f)-intptr_t(&CScripted2dEffects::ms_effects[0]))/intptr_t(sizeof(C2dEffect));
    if(idx<0||idx>=NUM_SCRIPTED_2D_EFFECTS||pedUsing[idx]<0)return nullptr; return Ped(pedUsing[idx]);
}
inline CTask* __fastcall TaskForPed(void* a,void*,CPed* p) { trace.push_back({"get_task_for_ped",Name(uintptr_t(a)),Name(uintptr_t(p))}); return reinterpret_cast<CTask*>(taskForPedRet); }
inline void __fastcall Interesting(void* ie,void*,int type,CEntity* e) { trace.push_back({"interesting_event",type,Name(uintptr_t(e))}); }
inline bool __fastcall BrainLoaded(void*,void*,const char* name) { trace.push_back({"brain_loaded",std::string(name,strnlen(name,8))}); return brainLoaded; }
inline void __fastcall StartBrain(void*,void*,const char* name,CPed* p,bool b) { trace.push_back({"start_brain",std::string(name,strnlen(name,8)),Name(uintptr_t(p)),b}); }
inline void __cdecl StoreClear(CPed* p) { trace.push_back({"brain_store_clear",Name(uintptr_t(p))}); }
inline int32 __cdecl GetIndex(const C2dEffectPedAttractor* f) { trace.push_back({"fx_get_index",Name(uintptr_t(f))}); return scriptIndex; }
inline CScriptedEffectPairs* __cdecl GetPairs(const C2dEffectPedAttractor* f) { trace.push_back({"fx_get_pairs",Name(uintptr_t(f))}); return reinterpret_cast<CScriptedEffectPairs*>(pairs.data()); }
inline CWanted* __cdecl FindWanted(int32 id) { trace.push_back({"find_player_wanted",id}); return reinterpret_cast<CWanted*>(wanted.data()); }
inline bool __fastcall CopJoins(void* w,void*,CCopPed* c) { trace.push_back({"can_cop_join_pursuit",Name(uintptr_t(w)),Name(uintptr_t(c))}); return copJoins; }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
enum Kind { GOG, ATT, PAR, EFF, PAI };
struct Fn { const char* cls; const char* name; uintptr_t address; int slot; int cases; Kind kind; };
constexpr uintptr_t VT[]{0x86E4A8,0x86E134,0x86E2B0,0x86E0D8,0x86E258};
constexpr size_t SZ[]{sizeof(CTaskComplexUseGoggles),sizeof(CTaskComplexUseAttractor),sizeof(CTaskComplexUseAttractorPartner),sizeof(CTaskComplexUseEffect),sizeof(CTaskComplexUsePairedAttractor)};
inline const std::array<Fn,38> fns{{
    {"CTaskComplexUseGoggles","Clone",0,1,128,GOG},{"CTaskComplexUseGoggles","GetTaskType",0,4,32,GOG},
    {"CTaskComplexUseGoggles","CreateNextSubTask",0,8,256,GOG},{"CTaskComplexUseGoggles","CreateFirstSubTask",0,9,256,GOG},
    {"CTaskComplexUseGoggles","ControlSubTask",0,10,512,GOG},
    {"CTaskComplexUseAttractor","Constructor",0x6326F0,-1,64,ATT},{"CTaskComplexUseAttractor","Destructor",0x632720,-1,64,ATT},
    {"CTaskComplexUseAttractor","Clone",0,1,128,ATT},{"CTaskComplexUseAttractor","GetTaskType",0,4,32,ATT},
    {"CTaskComplexUseAttractor","CreateNextSubTask",0,8,1024,ATT},{"CTaskComplexUseAttractor","CreateFirstSubTask",0,9,2048,ATT},
    {"CTaskComplexUseAttractor","ControlSubTask",0,10,512,ATT},
    {"CTaskComplexUseAttractorPartner","Constructor",0x6332C0,-1,64,PAR},{"CTaskComplexUseAttractorPartner","Destructor",0x633300,-1,64,PAR},
    {"CTaskComplexUseAttractorPartner","Clone",0,1,128,PAR},{"CTaskComplexUseAttractorPartner","GetTaskType",0,4,32,PAR},
    {"CTaskComplexUseAttractorPartner","CreateNextSubTask",0,8,64,PAR},{"CTaskComplexUseAttractorPartner","CreateFirstSubTask",0,9,256,PAR},
    {"CTaskComplexUseAttractorPartner","ControlSubTask",0,10,64,PAR},
    {"CTaskComplexUseEffect","Constructor",0x6321F0,-1,128,EFF},{"CTaskComplexUseEffect","Destructor",0x632270,-1,256,EFF},
    {"CTaskComplexUseEffect","Clone",0,1,128,EFF},{"CTaskComplexUseEffect","GetTaskType",0,4,32,EFF},
    {"CTaskComplexUseEffect","MakeAbortable",0,6,2048,EFF},{"CTaskComplexUseEffect","CreateNextSubTask",0,8,512,EFF},
    {"CTaskComplexUseEffect","CreateFirstSubTask",0,9,1024,EFF},{"CTaskComplexUseEffect","ControlSubTask",0,10,1024,EFF},
    {"CTaskComplexUsePairedAttractor","Constructor",0x6331A0,-1,64,PAI},{"CTaskComplexUsePairedAttractor","Destructor",0x6331E0,-1,128,PAI},
    {"CTaskComplexUsePairedAttractor","CreateSubTask",0x638D30,-1,1024,PAI},{"CTaskComplexUsePairedAttractor","SetPartnership",0x6331F0,-1,256,PAI},
    {"CTaskComplexUsePairedAttractor","ClearPartnership",0x633220,-1,256,PAI},{"CTaskComplexUsePairedAttractor","SeekPartnership",0x638AE0,-1,2048,PAI},
    {"CTaskComplexUsePairedAttractor","Clone",0,1,128,PAI},{"CTaskComplexUsePairedAttractor","GetTaskType",0,4,32,PAI},
    {"CTaskComplexUsePairedAttractor","CreateNextSubTask",0,8,2048,PAI},{"CTaskComplexUsePairedAttractor","CreateFirstSubTask",0,9,256,PAI},
    {"CTaskComplexUsePairedAttractor","ControlSubTask",0,10,2048,PAI},
}};

inline void CleanRefs() {
    for(int n=0;n<PEDS;++n)while(auto* r=Ped(n)->m_pReferences)Ped(n)->CleanUpOldReference(r->m_ppEntity);
    while(auto* r=Obj()->m_pReferences)Obj()->CleanUpOldReference(r->m_ppEntity);
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"tasks3-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"tasks3-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("tasks3");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::array<ReversibleHooks::ReversibleHook::Base*,38> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(std::make_integer_sequence<int,12>{}); fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[2]=reinterpret_cast<void*>(&FSub); fakeVtbl[3]=reinterpret_cast<void*>(&FSimple);
    fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    FillATraps(std::make_integer_sequence<int,16>{}); attrVtbl[0]=reinterpret_cast<void*>(&AType);
    for(auto& e:eventVtbl)e=reinterpret_cast<void*>(&ATrap<99>); eventVtbl[1]=reinterpret_cast<void*>(&EType);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    auto* seqs=reinterpret_cast<uint8*>(0xC178F0); const std::vector<uint8> savedSeqs(seqs,seqs+64*64);
    const auto savedUseAgain=CScripted2dEffects::ms_useAgainFlags;
    // GetEffect de la DLL valida (solo en depuracion) que el efecto sea un atractor: se marcan todos durante el banco
    auto* effects=reinterpret_cast<uint8*>(&CScripted2dEffects::ms_effects[0]); const std::vector<uint8> savedEffects(effects,effects+sizeof(CScripted2dEffects::ms_effects));
    for(auto& e:CScripted2dEffects::ms_effects)reinterpret_cast<C2dEffectBase&>(e).m_Type=EFFECT_ATTRACTOR;
    std::array<bool,38> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch d1(bit_cast<uintptr_t>(&CPed::TakeOffGoggles),TakeOff), d1e(0x5E6010,TakeOff);
        EntryPatch d2(bit_cast<uintptr_t>(&CPed::PutOnGoggles),PutOn), d2e(0x5E3AE0,PutOn);
        EntryPatch d3(bit_cast<uintptr_t>(&CAEPedWeaponAudioEntity::AddAudioEvent),WeaponEvent), d3e(0x4E69F0,WeaponEvent);
        EntryPatch d4(bit_cast<uintptr_t>(&CPedIntelligence::FindTaskByType),FindTask), d4e(0x600EE0,FindTask);
        EntryPatch d5(bit_cast<uintptr_t>(&CTaskSimplePlayerOnFoot::PlayerControlZelda),Zelda), d5e(0x6883D0,Zelda);
        EntryPatch d6(bit_cast<uintptr_t>(&CTaskManager::FindActiveTaskByType),FindActive), d6e(0x681740,FindActive);
        EntryPatch d7(bit_cast<uintptr_t>(static_cast<bool(CPedAttractorManager::*)(CPed*,CPedAttractor*)>(&CPedAttractorManager::BroadcastDeparture)),Depart), d7e(0x5EC980,Depart);
        EntryPatch d8(bit_cast<uintptr_t>(static_cast<bool(CPedAttractorManager::*)(CPed*,CPedAttractor*)>(&CPedAttractorManager::DeRegisterPed)),DeRegister), d8e(0x5EC850,DeRegister);
        EntryPatch d9(bit_cast<uintptr_t>(static_cast<const CPedAttractor*(CPedAttractorManager::*)(const CPed*,const C2dEffectPedAttractor*,const CEntity*)>(&CPedAttractorManager::GetRelevantAttractor)),Relevant), d9e(0x5EBF50,Relevant);
        EntryPatch d10(bit_cast<uintptr_t>(&CPedAttractorManager::RegisterPedWithAttractor),Register), d10e(0x5EFCA0,Register);
        EntryPatch d11(bit_cast<uintptr_t>(static_cast<CPed*(CPedAttractorManager::*)(const C2dEffectPedAttractor*,const CEntity*)>(&CPedAttractorManager::GetPedUsingEffect)),PedUsing), d11e(0x5EBE50,PedUsing);
        EntryPatch d12(bit_cast<uintptr_t>(&CPedAttractor::GetTaskForPed),TaskForPed), d12e(0x5EC500,TaskForPed);
        EntryPatch d13(0x602590,Interesting);
        EntryPatch d14(bit_cast<uintptr_t>(&CScriptsForBrains::HasAttractorScriptBrainWithThisNameLoaded),BrainLoaded), d14e(0x46AB20,BrainLoaded);
        EntryPatch d15(bit_cast<uintptr_t>(&CScriptsForBrains::StartAttractorScriptBrainWithThisName),StartBrain), d15e(0x46B390,StartBrain);
        EntryPatch d16(bit_cast<uintptr_t>(static_cast<void(*)(CPed*)>(&CScriptedBrainTaskStore::Clear)),StoreClear), d16e(0x635850,StoreClear);
        EntryPatch d17(bit_cast<uintptr_t>(&CScripted2dEffects::GetIndex),GetIndex), d17e(0x6F9F60,GetIndex);
        EntryPatch d18(bit_cast<uintptr_t>(&CScripted2dEffects::GetEffectPairs),GetPairs), d18e(0x6FA840,GetPairs);
        EntryPatch d19(bit_cast<uintptr_t>(&FindPlayerWanted),FindWanted), d19e(0x56E230,FindWanted);
        EntryPatch d20(bit_cast<uintptr_t>(static_cast<bool(CWanted::*)(CCopPed*)>(&CWanted::CanCopJoinPursuit)),CopJoins), d20e(0x562FB0,CopJoins);
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x3333u; trace=json::array(); seed=i; rngCalls=0;
                    CleanRefs();
                    // peatones
                    for(int n=0;n<PEDS;++n){ std::memset(peds[n].data(),0,peds[n].size()); std::memset(intel[n].data(),0,intel[n].size()); auto* p=Ped(n); p->SetType(ENTITY_TYPE_PED);
                        if(next()%5){ p->m_matrix=reinterpret_cast<CMatrixLink*>(mats[n].data()); CMatrix m{}; m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-3.f,3.f),uni(-3.f,3.f),uni(0.f,2.f)}; std::memcpy(static_cast<CMatrix*>(p->m_matrix),&m,sizeof(CMatrix)); }
                        else p->SetPosn(uni(-3.f,3.f),uni(-3.f,3.f),uni(0.f,2.f));
                        auto* raw=peds[n].data();
                        *reinterpret_cast<void**>(raw+0x47C)=intel[n].data();
                        *reinterpret_cast<uint32*>(raw+0x474)=next();
                        *reinterpret_cast<int32*>(raw+0x534)=int32(next()%8);
                        *reinterpret_cast<int32*>(raw+0x598)=next()%3==0 ? 6 : int32(next()%20);
                        *reinterpret_cast<int16*>(raw+0x756)=int16(next()%4==0 ? -int32(next()%50) : int32(next()%60));
                        *reinterpret_cast<void**>(raw+0x4FC)=next()%2 ? objectEnt.data() : nullptr;
                        findMask[n]=uint8(next()%8);
                        std::memset(ptask[n].data(),0,ptask[n].size()); *reinterpret_cast<uintptr_t*>(ptask[n].data())=VT[PAI];
                        if(next()%3==0){ *reinterpret_cast<CPed**>(ptask[n].data()+0x10)=Ped(next()%PEDS); } }
                    for(auto& b:playerData)b=uint8(next()); *reinterpret_cast<float*>(playerData.data()+0x18)=uni(-5.f,5.f);
                    *reinterpret_cast<void**>(peds[0].data()+0x480)=playerData.data();
                    // objeto (entidad), atractor, efectos, evento, buscado
                    std::memset(objectEnt.data(),0,objectEnt.size()); Obj()->SetType(next()%3 ? ENTITY_TYPE_OBJECT : ENTITY_TYPE_BUILDING);
                    *reinterpret_cast<uint32*>(objectEnt.data()+0x140)=next();
                    for(auto& b:attractor)b=uint8(next()); *reinterpret_cast<void**>(attractor.data())=attrVtbl.data();
                    *reinterpret_cast<void**>(attractor.data()+4)=next()%2 ? (void*)fx.data() : (void*)&CScripted2dEffects::ms_effects[next()%NUM_SCRIPTED_2D_EFFECTS];
                    { const uint32 m=next()%4; *reinterpret_cast<void**>(attractor.data()+8)=m==0 ? nullptr : m==1 ? (void*)Ped(1) : (void*)objectEnt.data(); }
                    for(int k=0;k<8;++k)attractor[0x84+k]=uint8(k<7 && next()%8 ? 'A'+next()%26 : 0);
                    attrType=next()%12==0 ? 10+int(next()%3) : int(next()%10);
                    for(auto& b:fx)b=uint8(next());
                    for(auto& b:pairs)b=uint8(next());
                    { auto* pp=reinterpret_cast<CScriptedEffectPairs*>(pairs.data()); pp->NumPairs=int32(next()%4==0 ? 0 : 1+next()%4);
                      for(auto& pr:pp->Pairs){ for(auto& e:pr.Effects){ e.Effect=next()%4==0 ? -1 : int32(next()%NUM_SCRIPTED_2D_EFFECTS); e.PartnerTask=int32(next()%64); e.WaitingTask=int32(next()%64);
                          e.PartnerUseMode=CScriptedEffectPair::eMode(next()%5==0 ? 2 : next()%2); } pr.UsePartnerImmediately=next()%2; } }
                    std::memset(event.data(),0,event.size()); *reinterpret_cast<void**>(event.data())=eventVtbl.data(); eventType=int(next()%6);
                    { const uint32 m=next()%4; *reinterpret_cast<CPed**>(event.data()+0x14)=m==0 ? nullptr : Ped(1+m%2); }
                    std::memset(wanted.data(),0,wanted.size()); *reinterpret_cast<uint32*>(wanted.data()+0x2C)=next()%3;
                    for(auto& u:pedUsing)u=int8(next()%3==0 ? -1 : int(1+next()%2));
                    for(auto& u:CScripted2dEffects::ms_useAgainFlags)u=next()%2;
                    fakeType=0; abortRet=next()%4!=0; brainLoaded=next()%3!=0; copJoins=next()%2; departRet=next()%2; scriptIndex=next()%5==0 ? -1 : int(next()%NUM_SCRIPTED_2D_EFFECTS);
                    relevantRet=next()%2 ? uintptr_t(attractor.data()) : next()%2 ? uintptr_t(fake2.data()) : 0;
                    registerRet=next()%3 ? uintptr_t(attractor.data()) : 0;
                    taskForPedRet=next()%4 ? uintptr_t(fake2.data()) : 0;
                    findTaskRet=next()%2 ? uintptr_t(fake2.data()) : 0;
                    std::memset(fake.data(),0,fake.size()); *reinterpret_cast<void**>(fake.data())=fakeVtbl.data();
                    std::memset(fake2.data(),0,fake2.size()); *reinterpret_cast<void**>(fake2.data())=fakeVtbl.data();
                    *reinterpret_cast<CVector*>(fake.data()+0x10)=CVector{uni(-3.f,3.f),uni(-3.f,3.f),uni(0.f,2.f)};
                    // tarea probada
                    std::memset(object.data(),0xCD,object.size()); std::memset(object.data(),0,SZ[fn.kind]);
                    *reinterpret_cast<uintptr_t*>(object.data())=VT[fn.kind]; *reinterpret_cast<void**>(object.data()+8)=fake.data();
                    auto* o=object.data(); auto W=[&](size_t off)->uint32&{return *reinterpret_cast<uint32*>(o+off);};
                    switch(fn.kind){
                    case GOG: { static const int types[]{TASK_SIMPLE_GOGGLES_ON,TASK_SIMPLE_GOGGLES_OFF,TASK_SIMPLE_STAND_STILL}; fakeType=types[next()%3]; break; }
                    case ATT: W(0xC)=uint32(uintptr_t(attractor.data())); fakeType=TASK_SIMPLE_STAND_STILL; break;
                    case PAR: o[0xC]=uint8(next()%2); W(0x10)=uint32(uintptr_t(&reinterpret_cast<CScriptedEffectPairs*>(pairs.data())->Pairs[next()%4])); break;
                    case EFF: {
                        W(0xC)=uint32(uintptr_t(fx.data()));
                        { const uint32 m=next()%4; auto* e=m==0 ? nullptr : m==1 ? (CEntity*)Ped(1) : Obj(); W(0x10)=uint32(uintptr_t(e)); if(e)e->RegisterReference(reinterpret_cast<CEntity**>(o+0x10)); }
                        W(0x14)=next()%3 ? uint32(uintptr_t(attractor.data())) : 0;
                        { const uint32 m=next()%3; auto* p=m==0 ? nullptr : Ped(m-1); W(0x18)=uint32(uintptr_t(p)); if(p)p->RegisterReference(reinterpret_cast<CEntity**>(o+0x18)); }
                        W(0x1C)=next()%8; o[0x20]=uint8(next()%3==0);
                        fakeType=next()%2 ? TASK_COMPLEX_GO_TO_ATTRACTOR : TASK_SIMPLE_STAND_STILL; break; }
                    case PAI: {
                        W(0xC)=uint32(uintptr_t(attractor.data()));
                        { const uint32 m=next()%3; auto* p=m==0 ? nullptr : Ped(m); W(0x10)=uint32(uintptr_t(p)); if(p)p->RegisterReference(reinterpret_cast<CEntity**>(o+0x10)); }
                        o[0x14]=uint8(next()%2);
                        W(0x18)=(W(0x10)||next()%2) ? uint32(uintptr_t(&reinterpret_cast<CScriptedEffectPairs*>(pairs.data())->Pairs[next()%4])) : 0;
                        W(0x1C)=next()%8; W(0x20)=next()%100;
                        { static const int types[]{TASK_COMPLEX_USE_ATTRACTOR_PARTNER,TASK_COMPLEX_ATTRACTOR_PARTNER_WAIT,TASK_COMPLEX_USE_SCRIPTED_ATTRACTOR,TASK_SIMPLE_STAND_STILL}; fakeType=types[next()%4]; }
                        // el original se cae si no es lider, no hay compañero y hay que crear una tarea de compañero
                        if(!W(0x10)&&W(0x18)==0&&(fakeType==TASK_COMPLEX_USE_ATTRACTOR_PARTNER))W(0x18)=uint32(uintptr_t(&reinterpret_cast<CScriptedEffectPairs*>(pairs.data())->Pairs[0]));
                        break; }
                    }
                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(VT[fn.kind]); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const std::string nm=fn.name; void* self=o; CPed* ped=Ped(0); uintptr_t rawRet=0; json ret=nullptr;
                    if(nm=="Constructor"){ std::memset(o,0xCD,object.size());
                        if(fn.kind==PAR)rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,bool,const void*)>(target)(self,next()%2,&reinterpret_cast<CScriptedEffectPairs*>(pairs.data())->Pairs[next()%4]));
                        else if(fn.kind==EFF){ const uint32 m=next()%3; rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,CEntity*)>(target)(self,fx.data(),m==0 ? nullptr : m==1 ? (CEntity*)Ped(1) : Obj())); }
                        else rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*)>(target)(self,attractor.data())); }
                    else if(nm=="Destructor")reinterpret_cast<void*(__thiscall*)(void*)>(target)(self);
                    else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(self));
                    else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(self);
                    else if(nm=="MakeAbortable"){ const int prio=int(next()%3); const void* ev=next()%4 ? event.data() : nullptr; ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const void*)>(target)(self,ped,prio,ev); }
                    else if(nm=="CreateSubTask"){ static const int types[]{TASK_FINISHED,TASK_COMPLEX_USE_ATTRACTOR_PARTNER,TASK_COMPLEX_ATTRACTOR_PARTNER_WAIT,TASK_COMPLEX_USE_SCRIPTED_ATTRACTOR,TASK_SIMPLE_STAND_STILL};
                        int tt=types[next()%5]; if((tt==TASK_COMPLEX_USE_ATTRACTOR_PARTNER||tt==TASK_COMPLEX_ATTRACTOR_PARTNER_WAIT)&&(!W(0x18)||(!o[0x14]&&!W(0x10))))tt=TASK_FINISHED;
                        rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,int,CPed*)>(target)(self,tt,ped)); }
                    else if(nm=="SetPartnership"){ if(W(0x10)){ Ped(W(0x10)==uint32(uintptr_t(Ped(1))) ? 1 : 2)->CleanUpOldReference(reinterpret_cast<CEntity**>(o+0x10)); W(0x10)=next()%2 ? W(0x10) : 0; if(W(0x10))reinterpret_cast<CPed*>(uintptr_t(W(0x10)))->RegisterReference(reinterpret_cast<CEntity**>(o+0x10)); }
                        reinterpret_cast<void(__thiscall*)(void*,CPed*,bool,const void*)>(target)(self,Ped(1+next()%2),next()%2,next()%2 ? (void*)&reinterpret_cast<CScriptedEffectPairs*>(pairs.data())->Pairs[next()%4] : nullptr); }
                    else if(nm=="ClearPartnership")reinterpret_cast<void(__thiscall*)(void*)>(target)(self);
                    else if(nm=="SeekPartnership")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(self,ped);
                    else {
                        if(fn.kind==PAI&&nm=="CreateNextSubTask"&&fakeType==TASK_COMPLEX_USE_ATTRACTOR_PARTNER&&!W(0x18))W(0x18)=uint32(uintptr_t(&reinterpret_cast<CScriptedEffectPairs*>(pairs.data())->Pairs[1]));
                        rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(self,ped)); }
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    if(ret.is_null()&&nm!="Destructor")ret=Name(rawRet);
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",t->GetTaskType()},{"words",Region(t,0x38,true)}});
                    json objectWords=Region(o,SZ[fn.kind],true);
                    if(nm=="Constructor")objectWords.push_back(reinterpret_cast<CTask*>(self)->GetTaskType()); // vptr EXE o DLL: se compara el tipo
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(int n=0;n<PEDS;++n)refs.push_back(refList(Ped(n))); refs.push_back(refList(Obj()));
                    json pedWords=json::array(); for(int n=0;n<PEDS;++n)pedWords.push_back({PedHash(n),*reinterpret_cast<int16*>(peds[n].data()+0x756),*reinterpret_cast<uint32*>(peds[n].data()+0x474)});
                    json ptasks=json::array(); for(int n=0;n<PEDS;++n)ptasks.push_back(Region(ptask[n].data(),0x24,true));
                    const auto seqsNow=Hash(seqs,64*64);
                    for(auto* t:fresh)delete t;
                    const bool clean=Occupancy()==occ;
                    const auto seqsAfter=Hash(seqs,64*64); std::memcpy(seqs,savedSeqs.data(),savedSeqs.size());
                    json r={{"ret",ret},{"object",objectWords},{"tasks",tasks},{"fake",Region(fake.data(),0x40,true)},{"refs",refs},{"peds",pedWords},{"ptasks",ptasks},
                            {"pdata",ReversingProbe::Bytes(playerData.data(),playerData.size())},{"attractor",Region(attractor.data(),sizeof(CPedAttractor),true)},
                            {"seqs",seqsNow},{"seqs_after",seqsAfter},{"trace",trace},{"rng",rngCalls},{"clean",clean},{"switches",switches}};
                    CleanRefs();
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true;
                passed&=ok; failures+=!ok;
                auto strip=[](json j){ return j; };
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? strip(original):json(nullptr)},{"candidate",failures<=2 ? strip(candidate):json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"failures",failures},{"cases",fn.cases}});
        }
    }
    CleanRefs();
    CScripted2dEffects::ms_useAgainFlags=savedUseAgain; std::memcpy(effects,savedEffects.data(),savedEffects.size()); std::memcpy(seqs,savedSeqs.data(),savedSeqs.size());
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree}});
}
}
