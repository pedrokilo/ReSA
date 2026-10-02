#pragma once
// Banco dentro del motor: CTaskComplexTurnToFaceEntityOrCoord (9), CTaskComplexUseEffectRunning (2),
// CTaskComplexUseEffectSprinting (2), CTaskComplexUseClosestFreeScriptedAttractorRun (4) y Sprint
// (Constructor, Destructor). Original (hooks del lote desactivados) frente a candidato con fixtures
// identicos (peaton/entidad con matriz, subtarea falsa). Se compara la memoria de los fixtures, las
// tareas creadas (punteros traducidos), las referencias, el retorno y la traza.
#include <fstream>
#include <unordered_map>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskComplexUseEffectRunning.h"
#include "TaskComplexUseEffectSprinting.h"
#include "TaskComplexUseClosestFreeScriptedAttractorRun.h"
#include "TaskComplexUseClosestFreeScriptedAttractorSprint.h"
#include "TaskSimpleAchieveHeading.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace Tasks2Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
constexpr int PEDS=3;
alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,PEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,PEDS> mats{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<uint8,0x100> fake{};
alignas(16) inline std::array<uint8,0x40> fx{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<PEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(mats[n].data(),mats[n].size(),"mat"))return *s+"#"+std::to_string(n); }
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(fake.data(),fake.size(),"fake"))return *s; if(auto s=in(fx.data(),fx.size(),"fx"))return *s;
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
// subtarea falsa: AchieveHeading (simple), con vtable propia
inline int fakeType{TASK_SIMPLE_ACHIEVE_HEADING};
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",f}); return t; }
inline CTask* __fastcall FSub(CTask*,void*) { return nullptr; }
inline bool __fastcall FSimple(CTask*,void*) { return true; }
inline int __fastcall FType(CTask*,void*) { return fakeType; }
inline bool __fastcall FAbort(CTask*,void*,CPed* p,int prio,const CEvent* e) { trace.push_back({"make_abortable",Name(uintptr_t(p)),prio}); return true; }
template<int N> void* __fastcall FTrap(CTask*,void*) { trace.push_back({"unexpected_slot",N}); return nullptr; }
inline std::array<void*,12> fakeVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; int slot; int cases; uintptr_t vtbl; size_t size; };
constexpr uintptr_t VT_TURN=0x86FF94, VT_RUN=0x86E594, VT_SPR=0x86E5C0, VT_ARUN=0x85B904, VT_ASPR=0x85B930;
inline const std::array<Fn,19> fns{{
    {"CTaskComplexTurnToFaceEntityOrCoord","Clone",0,1,256,VT_TURN,sizeof(CTaskComplexTurnToFaceEntityOrCoord)},
    {"CTaskComplexTurnToFaceEntityOrCoord","ComputeTargetHeading",0x66B9D0,-1,2048,VT_TURN,sizeof(CTaskComplexTurnToFaceEntityOrCoord)},
    {"CTaskComplexTurnToFaceEntityOrCoord","Constructor-Coords",0x66B890,-1,256,VT_TURN,sizeof(CTaskComplexTurnToFaceEntityOrCoord)},
    {"CTaskComplexTurnToFaceEntityOrCoord","Constructor-Entity",0x66B910,-1,256,VT_TURN,sizeof(CTaskComplexTurnToFaceEntityOrCoord)},
    {"CTaskComplexTurnToFaceEntityOrCoord","ControlSubTask",0,10,2048,VT_TURN,sizeof(CTaskComplexTurnToFaceEntityOrCoord)},
    {"CTaskComplexTurnToFaceEntityOrCoord","CreateFirstSubTask",0,9,1024,VT_TURN,sizeof(CTaskComplexTurnToFaceEntityOrCoord)},
    {"CTaskComplexTurnToFaceEntityOrCoord","CreateNextSubTask",0,8,64,VT_TURN,sizeof(CTaskComplexTurnToFaceEntityOrCoord)},
    {"CTaskComplexTurnToFaceEntityOrCoord","Destructor",0x66B960,-1,256,VT_TURN,sizeof(CTaskComplexTurnToFaceEntityOrCoord)},
    {"CTaskComplexTurnToFaceEntityOrCoord","GetTaskType",0,4,32,VT_TURN,sizeof(CTaskComplexTurnToFaceEntityOrCoord)},
    {"CTaskComplexUseEffectRunning","Clone",0,1,256,VT_RUN,sizeof(CTaskComplexUseEffectRunning)},
    {"CTaskComplexUseEffectRunning","GetTaskType",0,4,32,VT_RUN,sizeof(CTaskComplexUseEffectRunning)},
    {"CTaskComplexUseEffectSprinting","Clone",0,1,256,VT_SPR,sizeof(CTaskComplexUseEffectSprinting)},
    {"CTaskComplexUseEffectSprinting","GetTaskType",0,4,32,VT_SPR,sizeof(CTaskComplexUseEffectSprinting)},
    {"CTaskComplexUseClosestFreeScriptedAttractorRun","Clone",0,1,128,VT_ARUN,sizeof(CTaskComplexUseClosestFreeScriptedAttractorRun)},
    {"CTaskComplexUseClosestFreeScriptedAttractorRun","Constructor",0x4B8790,-1,128,VT_ARUN,sizeof(CTaskComplexUseClosestFreeScriptedAttractorRun)},
    {"CTaskComplexUseClosestFreeScriptedAttractorRun","Destructor",0x4B8840,-1,128,VT_ARUN,sizeof(CTaskComplexUseClosestFreeScriptedAttractorRun)},
    {"CTaskComplexUseClosestFreeScriptedAttractorRun","GetTaskType",0,4,32,VT_ARUN,sizeof(CTaskComplexUseClosestFreeScriptedAttractorRun)},
    {"CTaskComplexUseClosestFreeScriptedAttractorSprint","Constructor",0x4B8850,-1,128,VT_ASPR,sizeof(CTaskComplexUseClosestFreeScriptedAttractorSprint)},
    {"CTaskComplexUseClosestFreeScriptedAttractorSprint","Destructor",0x4B8900,-1,128,VT_ASPR,sizeof(CTaskComplexUseClosestFreeScriptedAttractorSprint)},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"tasks2-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"tasks2-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("tasks2");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::array<ReversibleHooks::ReversibleHook::Base*,19> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(std::make_integer_sequence<int,12>{}); fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[2]=reinterpret_cast<void*>(&FSub); fakeVtbl[3]=reinterpret_cast<void*>(&FSimple);
    fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    std::array<bool,19> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0;
        for(int i=0;i<fn.cases;++i) {
            uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
            auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
            auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot
                    { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0;
                      // slots libres con 0xCD: el New del EXE no escribe en el objeto y los bytes sin usar quedarian con restos
                      auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* st=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                      for(int32 k=0;k<cap;++k)if(st[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7777u; trace=json::array();
                for(int n=0;n<PEDS;++n)while(auto* r=Ped(n)->m_pReferences)Ped(n)->CleanUpOldReference(r->m_ppEntity);
                for(int n=0;n<PEDS;++n){ std::memset(peds[n].data(),0,peds[n].size()); auto* p=Ped(n); p->SetType(ENTITY_TYPE_PED);
                    if(next()%5){ p->m_matrix=reinterpret_cast<CMatrixLink*>(mats[n].data()); CMatrix m{}; m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,5.f)}; std::memcpy(static_cast<CMatrix*>(p->m_matrix),&m,sizeof(CMatrix)); }
                    else p->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,5.f));
                    p->m_fCurrentRotation=uni(-3.f,3.f); }
                for(auto& b:fx)b=uint8(next()); std::memset(fake.data(),0,fake.size()); *reinterpret_cast<void**>(fake.data())=fakeVtbl.data();
                for(size_t b=0;b<object.size();++b)object[b]=0xCD;
                std::memset(object.data(),0,fn.size); *reinterpret_cast<uintptr_t*>(object.data())=fn.vtbl; *reinterpret_cast<void**>(object.data()+8)=fake.data();
                auto* o=object.data();
                if(fn.vtbl==VT_TURN){ auto* t=reinterpret_cast<CTaskComplexTurnToFaceEntityOrCoord*>(o); t->m_bFaceEntity=next()%3!=0; t->m_EntityToFace=next()%4 ? Ped(1+next()%2):nullptr;
                    t->m_CoordsToFace=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,5.f)}; t->m_fChangeRateMult=uni(0.f,1.f); t->m_fMaxHeading=uni(0.f,0.5f);
                    if(t->m_EntityToFace)t->m_EntityToFace->RegisterReference(&t->m_EntityToFace); }
                if(fn.vtbl==VT_RUN||fn.vtbl==VT_SPR){ auto* t=reinterpret_cast<CTaskComplexUseEffect*>(o); t->m_2dFx=reinterpret_cast<C2dEffectPedAttractor*>(fx.data()); t->m_Entity=next()%4 ? Ped(1):nullptr;
                    t->m_MoveState=eMoveState(next()%8); if(t->m_Entity)t->m_Entity->RegisterReference(&t->m_Entity); }
                if(fn.vtbl==VT_ARUN||fn.vtbl==VT_ASPR){ for(size_t b=12;b<fn.size;++b)o[b]=uint8(next()); }
                const auto occ=Occupancy();
                for(auto* h:hooks)h->State(candidate);
                const auto vt=reinterpret_cast<uintptr_t*>(fn.vtbl); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                const std::string nm=fn.name; void* self=object.data(); CPed* ped=Ped(0); uintptr_t rawRet=0; json ret=nullptr;
                if(nm=="Constructor-Coords"){ std::memset(object.data(),0xCD,object.size()); CVector c{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,5.f)};
                    rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CEntity*,float,float)>(target)(self,next()%4 ? Ped(1):nullptr,uni(0.f,1.f),uni(0.f,1.f))); (void)c; }
                else if(nm=="Constructor-Entity"){ std::memset(object.data(),0xCD,object.size()); static CVector c; c=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,5.f)};
                    rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const CVector&,float,float)>(target)(self,c,uni(0.f,1.f),uni(0.f,1.f))); }
                else if(nm=="Constructor"){ std::memset(object.data(),0xCD,object.size()); rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(target)(self)); }
                else if(nm=="Destructor")reinterpret_cast<void*(__thiscall*)(void*)>(target)(self);
                else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(self));
                else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(self);
                else if(nm=="ComputeTargetHeading"){ auto* t=reinterpret_cast<CTaskComplexTurnToFaceEntityOrCoord*>(self); if(t->m_bFaceEntity&&!t->m_EntityToFace)t->m_bFaceEntity=false;
                    ret=std::bit_cast<uint32>(reinterpret_cast<float(__thiscall*)(void*,CPed*)>(target)(self,ped)); }
                else rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(self,ped));
                freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                if(ret.is_null()&&nm!="Destructor")ret=Name(rawRet);
                json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",t->GetTaskType()},{"words",Region(t,0x28,true)}});
                json objectWords=Region(object.data(),fn.size,true);
                if(nm.starts_with("Constructor"))objectWords.push_back(reinterpret_cast<CTask*>(self)->GetTaskType()); // vptr EXE o DLL: se compara el tipo
                json refs=json::array(); for(int n=0;n<PEDS;++n){json l=json::array();for(auto* r=Ped(n)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));refs.push_back(l);}
                for(auto* t:fresh)delete t;
                const bool clean=Occupancy()==occ;
                return {{"ret",ret},{"object",objectWords},{"tasks",tasks},{"fake",Region(fake.data(),0x40,true)},{"refs",refs},{"trace",trace},{"clean",clean},{"switches",switches}};
            };
            const auto original=run(false), candidate=run(true);
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true;
            passed&=ok; failures+=!ok;
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
            else Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",false},{"accepted",false},{"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}});
        }
        Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"failures",failures},{"cases",fn.cases}});
    }
    for(int n=0;n<PEDS;++n)while(auto* r=Ped(n)->m_pReferences)Ped(n)->CleanUpOldReference(r->m_ppEntity);
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree}});
}
}
