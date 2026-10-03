#pragma once
// Banco dentro del motor: tareas complejas A (lista de 7, 42 funciones).
// CTaskComplexChat (constructor, destructor, Clone, GetTaskType, ControlSubTask, CreateFirstSubTask, CreateNextSubTask),
// CTaskComplexDiveFromAttachedEntityAndGetUp, CTaskComplexDrivePointRoute, CTaskComplexHitResponse y CTaskComplexInWater
// (constructor, destructor, Clone, GetTaskType, ControlSubTask, CreateFirstSubTask, CreateNextSubTask) y
// CTaskComplexGoToPointAndStandStill (constructor, GoToPoint, Clone, GetTaskType, ControlSubTask, CreateFirstSubTask,
// CreateNextSubTask).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 6 peatones (con inteligencia propia), 2 vehiculos (vtable falsa con trampas), 2 grupos, tarea de chat del companero,
// ruta de puntos y tareas falsas (vtable trazada).
// Se simulan y trazan: CPed::Say, CPed::GetPedTalking, CPed::DettachPedFromEntity, CInterestingEvents::Add,
// CPedIntelligence::FindTaskByType, CPedGroups::GetPedsGroup, CPopulation::IsSunbather,
// CPedGeometryAnalyser::ComputeEntityHitSide/ComputeEntityDirs/ComputeEntityBoundingBoxPlanes y el RNG.
// Pools de tareas y de rutas de puntos: m_LastFreeSlot y estados se restauran y los slots libres se rellenan con 0xCD
// en cada ejecucion; las rutas nuevas se serializan y se liberan. Se compara la memoria de los fixtures (punteros
// traducidos a nombres), las tareas y rutas nuevas, las referencias, el retorno y la traza.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/Task.h"
#include "TaskComplexChat.h"
#include "TaskComplexDiveFromAttachedEntityAndGetUp.h"
#include "TaskComplexDrivePointRoute.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskComplexHitResponse.h"
#include "TaskComplexInWater.h"
#include "PedGeometryAnalyser.h"
#include "PedGroups.h"
#include "PedIntelligence.h"
#include "Population.h"
#include "InterestingEvents.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "Pools/PointRoutePool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks7aLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }

constexpr int NPEDS=6, NVEH=2, FAKES=3, NGRP=2;
// tareas falsas: los campos de control quedan por encima de 0x50 (CTaskSimpleSwim se lee hasta 0x4C)
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_SUB=0x64, FK_TYPE=0x68, FK_CLONE=0x6C;
// desplazamientos crudos del EXE
constexpr size_t E_POS=4, E_MATRIX=0x14, E_RWOBJ=0x18, E_MODEL=0x22, E_TYPE=0x36;                  // CEntity
constexpr size_t PH_MOVE=0x44, PH_ATTACHED=0xFC;                                                  // CPhysical
constexpr size_t P_FLAGS=0x46C, P_FLAGS2=0x470, P_INTEL=0x47C, P_PLAYERDATA=0x480, P_CREATED=0x484,
                 P_VEHICLE=0x58C, P_PEDTYPE=0x598;                                                // CPed
constexpr size_t G_LEADER=0x28;                                                                   // CPedGroup (membresia +8, lider +0x20)
constexpr uintptr_t VT_CHAT=0x8706A0, VT_DIVE=0x85A358, VT_DPR=0x86EBF8, VT_GTP=0x86FDA8, VT_HIT=0x86E040, VT_WATER=0x86E4EC;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<uint8,0x100> object{};
alignas(16) inline std::array<uint8,0x40> ptask{};
alignas(16) inline std::array<std::array<uint8,0x40>,NGRP> groups{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<uint8,0x40> clump{};
alignas(16) inline std::array<uint8,0x40> playerData{};
alignas(16) inline std::array<uint8,sizeof(CPointRoute)> routeIn{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline int PedIndex(const void* p) { for(int n=0;n<NPEDS;++n)if(p==peds[n].data())return n; return -1; }
inline int IntelIndex(const void* p) { for(int n=0;n<NPEDS;++n)if(p==intels[n].data())return n; return -1; }

// pools (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot)
constexpr size_t TASK_SLOT=0x80, ROUTE_SLOT=sizeof(CPointRoute);
inline uint8* TaskHdr() { return reinterpret_cast<uint8*>(GetTaskPool()); }
inline uint8* RouteHdr() { return reinterpret_cast<uint8*>(GetPointRoutePool()); }
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
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(vehVtbl.data()))return "veh_vt";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n);
                              if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NGRP;++n)if(auto s=in(groups[n].data(),groups[n].size(),"group"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(ptask.data(),ptask.size(),"ptask"))return *s;
    if(auto s=in(clump.data(),clump.size(),"clump"))return *s; if(auto s=in(playerData.data(),playerData.size(),"pdata"))return *s;
    if(auto s=in(routeIn.data(),routeIn.size(),"route_in"))return *s;
    if(const int k=SlotIndex(RouteHdr(),ROUTE_SLOT,v);k>=0)return "route#"+std::to_string(k);
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); for(auto& v:vehs)r.push_back(RefList(v.data())); return r; }
// tarea serializada sin vptr (se compara el tipo)
inline json SerTask(const void* p) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType(); const auto* w=static_cast<const uint32*>(p);
    json words=json::array(); for(size_t k=1;k<TASK_SLOT/4;++k)words.push_back(Name(w[k]));
    return {{"type",type},{"words",words}};
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{};
inline uint8 talking{}, sunbather{};
inline std::array<uintptr_t,NPEDS> findRet{}, groupOf{};
inline int32 hitSide{};
inline std::array<CVector,4> dirsOut{}, planesOut{}; inline std::array<float,4> dotsOut{};

// tareas falsas
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(t)}); return At<CTask*>(t,FK_CLONE); }
inline CTask* __fastcall FSub(CTask* t,void*) { return At<CTask*>(t,FK_SUB); }
inline bool __fastcall FSimple(CTask*,void*) { return true; }
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) { trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e)}); return At<uint8>(t,FK_ABORT)!=0; }
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<int... N> void FillTraps(std::array<void*,sizeof...(N)>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }

// dependencias simuladas
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
// el EXE empuja el contexto con basura en los 16 bits altos (eax): solo se traza el int16
inline int16 __fastcall FakeSay(CPed* p,void*,int ctx,uint32 delay,float prob,bool overrideSilence,bool forceAudible,bool frontEnd) {
    trace.push_back({"say",Name(p),int16(ctx),delay,F(prob),overrideSilence,forceAudible,frontEnd}); return 0;
}
inline bool __fastcall FakeTalking(CPed* p,void*) { trace.push_back({"ped_talking",Name(p),talking}); return talking!=0; }
inline void __fastcall FakeDettach(CPed* p,void*) { trace.push_back({"dettach_ped",Name(p)}); }
inline void __fastcall FakeInteresting(void* ie,void*,int32 type,CEntity* e) { trace.push_back({"interesting_event",type,Name(e)}); }
inline CTask* __fastcall FakeFindTask(CPedIntelligence* in,void*,int32 type) {
    const int n=IntelIndex(in); const uintptr_t r=n>=0 ? findRet[n] : 0; trace.push_back({"find_task_by_type",Name(in),type,Name(r)}); return reinterpret_cast<CTask*>(r);
}
inline CPedGroup* __cdecl FakePedsGroup(const CPed* p) { const int n=PedIndex(p); const uintptr_t g=n>=0 ? groupOf[n] : 0; trace.push_back({"peds_group",Name(p),Name(g)}); return reinterpret_cast<CPedGroup*>(g); }
inline bool __cdecl FakeSunbather(int32 model) { trace.push_back({"is_sunbather",model,sunbather}); return sunbather!=0; }
inline int32 __cdecl FakeHitSide(const CPed& p,CEntity& e) { trace.push_back({"entity_hit_side",Name(&p),Name(&e),hitSide}); return hitSide; }
inline CVector* __cdecl FakeDirs(const CEntity& e,CVector* out) {
    trace.push_back({"entity_dirs",Name(&e),"stack"}); for(int k=0;k<4;++k)out[k]=dirsOut[k]; return out;
}
inline void __cdecl FakePlanes(float z,CEntity& e,CVector (*planes)[4],float* dots) {
    trace.push_back({"entity_bb_planes",F(z),Name(&e),"stack","stack"}); /* salidas en la pila del llamador */ for(int k=0;k<4;++k){ (*planes)[k]=planesOut[k]; dots[k]=dotsOut[k]; }
}

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline std::vector<bool> RouteOccupancy() { auto* p=GetPointRoutePool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=42;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexChat","Clone",0x682DA0,0x8706A0,1,128},
    {"CTaskComplexChat","Constructor",0x682CB0,0,-1,256},
    {"CTaskComplexChat","ControlSubTask",0x683060,0x8706A0,10,1024},
    {"CTaskComplexChat","CreateFirstSubTask",0x682E20,0x8706A0,9,512},
    {"CTaskComplexChat","CreateNextSubTask",0x682E10,0x8706A0,8,64},
    {"CTaskComplexChat","Destructor",0x682D40,0,-1,128},
    {"CTaskComplexChat","GetTaskType",0x682D30,0x8706A0,4,64},
    {"CTaskComplexDiveFromAttachedEntityAndGetUp","Clone",0x492E70,0x85A358,1,128},
    {"CTaskComplexDiveFromAttachedEntityAndGetUp","Constructor",0x492E20,0,-1,128},
    {"CTaskComplexDiveFromAttachedEntityAndGetUp","ControlSubTask",0x655710,0x85A358,10,64},
    {"CTaskComplexDiveFromAttachedEntityAndGetUp","CreateFirstSubTask",0x657820,0x85A358,9,1024},
    {"CTaskComplexDiveFromAttachedEntityAndGetUp","CreateNextSubTask",0x6577F0,0x85A358,8,256},
    {"CTaskComplexDiveFromAttachedEntityAndGetUp","Destructor",0x492F10,0,-1,128},
    {"CTaskComplexDiveFromAttachedEntityAndGetUp","GetTaskType",0x492EE0,0x85A358,4,64},
    {"CTaskComplexDrivePointRoute","Clone",0x6439C0,0x86EBF8,1,256},
    {"CTaskComplexDrivePointRoute","Constructor",0x6433E0,0,-1,512},
    {"CTaskComplexDrivePointRoute","ControlSubTask",0x63D0C0,0x86EBF8,10,64},
    {"CTaskComplexDrivePointRoute","CreateFirstSubTask",0x63D030,0x86EBF8,9,256},
    {"CTaskComplexDrivePointRoute","CreateNextSubTask",0x63CF90,0x86EBF8,8,512},
    {"CTaskComplexDrivePointRoute","Destructor",0x643490,0,-1,128},
    {"CTaskComplexDrivePointRoute","GetTaskType",0x643480,0x86EBF8,4,64},
    {"CTaskComplexGoToPointAndStandStill","Clone",0x66CEA0,0x86FDA8,1,256},
    {"CTaskComplexGoToPointAndStandStill","Constructor",0x668120,0,-1,1024},
    {"CTaskComplexGoToPointAndStandStill","ControlSubTask",0x668570,0x86FDA8,10,1024},
    {"CTaskComplexGoToPointAndStandStill","CreateFirstSubTask",0x66DC40,0x86FDA8,9,256},
    {"CTaskComplexGoToPointAndStandStill","CreateNextSubTask",0x66DBA0,0x86FDA8,8,1024},
    {"CTaskComplexGoToPointAndStandStill","GetTaskType",0x668230,0x86FDA8,4,64},
    {"CTaskComplexGoToPointAndStandStill","GoToPoint",0x46FE60,0,-1,1024},
    {"CTaskComplexHitResponse","Clone",0x6365E0,0x86E040,1,128},
    {"CTaskComplexHitResponse","Constructor",0x631D70,0,-1,128},
    {"CTaskComplexHitResponse","ControlSubTask",0x631DC0,0x86E040,10,64},
    {"CTaskComplexHitResponse","CreateFirstSubTask",0x638040,0x86E040,9,256},
    {"CTaskComplexHitResponse","CreateNextSubTask",0x631DB0,0x86E040,8,64},
    {"CTaskComplexHitResponse","Destructor",0x631DA0,0,-1,128},
    {"CTaskComplexHitResponse","GetTaskType",0x631D90,0x86E040,4,64},
    {"CTaskComplexInWater","Clone",0x6370B0,0x86E4EC,1,64},
    {"CTaskComplexInWater","Constructor",0x6350D0,0,-1,64},
    {"CTaskComplexInWater","ControlSubTask",0x635420,0x86E4EC,10,64},
    {"CTaskComplexInWater","CreateFirstSubTask",0x635200,0x86E4EC,9,1024},
    {"CTaskComplexInWater","CreateNextSubTask",0x635110,0x86E4EC,8,512},
    {"CTaskComplexInWater","Destructor",0x635100,0,-1,64},
    {"CTaskComplexInWater","GetTaskType",0x6350F0,0x86E4EC,4,64},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks7a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks7a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks7a");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks7a-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(fakeVtbl,std::make_integer_sequence<int,12>{}); FillTraps(vehVtbl,std::make_integer_sequence<int,96>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    const auto occupancyStart=Occupancy(), routeOccStart=RouteOccupancy();
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    const int32 lf0=Last(TaskHdr()), lr0=Last(RouteHdr());
    const std::vector<uint8> statesStart(States(TaskHdr()),States(TaskHdr())+Cap(TaskHdr()));
    const std::vector<uint8> routeStatesStart(States(RouteHdr()),States(RouteHdr())+Cap(RouteHdr()));
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch s1(bit_cast<uintptr_t>(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say)),FakeSay), s1e(0x5EFFE0,FakeSay);
        EntryPatch s2(bit_cast<uintptr_t>(&CPed::GetPedTalking),FakeTalking), s2e(0x5EFF50,FakeTalking);
        EntryPatch s3(bit_cast<uintptr_t>(&CPed::DettachPedFromEntity),FakeDettach), s3e(0x5E7EC0,FakeDettach);
        EntryPatch s4e(0x602590,FakeInteresting); // CInterestingEvents::Add del DLL llama al EXE
        EntryPatch s5(bit_cast<uintptr_t>(&CPedIntelligence::FindTaskByType),FakeFindTask), s5e(0x600EE0,FakeFindTask);
        EntryPatch s6(bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup),FakePedsGroup), s6e(0x5F7E80,FakePedsGroup);
        EntryPatch s7(bit_cast<uintptr_t>(static_cast<bool(*)(eModelID)>(&CPopulation::IsSunbather)),FakeSunbather), s7e(0x611760,FakeSunbather);
        EntryPatch g1(bit_cast<uintptr_t>(static_cast<int32(*)(const CPed&,CEntity&)>(&CPedGeometryAnalyser::ComputeEntityHitSide)),FakeHitSide), g1e(0x5F3BC0,FakeHitSide);
        EntryPatch g2(bit_cast<uintptr_t>(&CPedGeometryAnalyser::ComputeEntityDirs),FakeDirs), g2e(0x5F1500,FakeDirs);
        EntryPatch g3(bit_cast<uintptr_t>(&CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes),FakePlanes), g3e(0x5F3660,FakePlanes);

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
                    ResetPool(RouteHdr(),ROUTE_SLOT,lr0,routeStatesStart,routeOccStart,RouteOccupancy());
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7A7Au; if(!st)st=1; trace=json::array(); seed=uint32(i);
                    rngCalls=0; freshIds.clear();
                    CleanRefs();
                    // peatones: matriz, inteligencia propia, tipo
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                        p[E_TYPE]=3; At<void*>(p,P_INTEL)=intels[n].data(); At<void*>(p,E_RWOBJ)=clump.data(); At<void*>(p,E_MATRIX)=pedMats[n].data();
                        for(int r=0;r<3;++r)At<CVector>(pedMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(pedMats[n].data(),0x30)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        { static const int32 types[]{4,4,5,7,0,1}; At<int32>(p,P_PEDTYPE)=types[next()%6]; }
                        At<int16>(p,E_MODEL)=int16(next()%300);
                        groupOf[n]=0; findRet[n]=0;
                    }
                    // vehiculos: vtable falsa con trampas, matriz y velocidad
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%8)<<3); At<void*>(v,E_MATRIX)=vehMats[n].data();
                        for(int r=0;r<3;++r)At<CVector>(vehMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        At<CVector>(v,PH_MOVE)=next()%6 ? CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-0.2f,0.2f)} : CVector{0.f,0.f,0.f};
                    }
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,FAKE_SIZE); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_PAUSE+n; fakes[n][FK_ABORT]=uint8(next()%2); }
                    for(auto& g:groups)std::memset(g.data(),0,g.size());
                    std::memset(clump.data(),0,clump.size()); clump[0]=2;
                    std::memset(playerData.data(),0x5A,playerData.size());
                    std::memset(ptask.data(),0,ptask.size()); At<uintptr_t>(ptask.data(),0)=VT_CHAT;
                    for(size_t b=0;b<routeIn.size();++b)routeIn[b]=uint8(next());
                    for(size_t k=1;k<routeIn.size()/4;++k)At<float>(routeIn.data(),4*k)=uni(-100.f,100.f);
                    At<uint32>(routeIn.data(),0)=next()%9;
                    std::memset(object.data(),0xCD,object.size());
                    talking=uint8(next()%2); sunbather=uint8(next()%4==0); hitSide=int32(next()%4);
                    for(auto& d:dirsOut)d=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                    for(auto& d:planesOut)d=CVector{uni(-1.f,1.f),uni(-1.f,1.f),next()%4 ? 0.f : uni(-1.f,1.f)};
                    for(auto& d:dotsOut)d=uni(-60.f,60.f);

                    const auto occ=Occupancy(); const auto routeOcc=RouteOccupancy();
                    // la ruta del destructor sale del pool de rutas (el destructor la libera)
                    CPointRoute* preRoute{};
                    if(cls=="CTaskComplexDrivePointRoute"&&nm=="Destructor"&&next()%4){
                        preRoute=static_cast<CPointRoute*>(CPointRoute::operator new(sizeof(CPointRoute)));
                        if(preRoute)std::memcpy(preRoute,routeIn.data(),sizeof(CPointRoute));
                    }
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
                    auto setSub=[&](){ At<CTask*>(o,8)=Fake(0); };
                    auto typeOf=[&](void* p){ return reinterpret_cast<CTask*>(p)->GetTaskType(); };
                    auto callTask=[&](CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    auto callClone=[&]{ return reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); };
                    auto callDtor=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); };
                    auto inVeh=[&](CPed* p,bool on){ auto& fl=At<uint32>(p,P_FLAGS); fl=on ? (fl|0x100u) : (fl&~0x100u); };
                    auto ctorDone=[&](size_t size){ for(auto* h:hooks)h->State(false); out["object"]=Region(o,size,true); out["type"]=typeOf(o); };
                    auto pedOut=[&]{ out["ped"]=Region(p0,sizeof(CPed)); };

                    if(nm=="GetTaskType") {
                        setTask(fn.vt,0x40);
                        ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        out["object"]=Region(o,0x40,true);
                    }
                    else if(cls=="CTaskComplexChat") {
                        // +0xC hablador (byte), +0x10 companero (referencia), +0x14 etapa, +0x18 contexto (int16), +0x1A terminada, +0x1C contador
                        // contextos 164, 167 y 168 fuera: FIX_BUGS cambia el rango aleatorio de CreateFirstSubTask (0..3 -> 0..4)
                        static const int16 ctxs[]{-1,-1,-2,-32768,0,5,100,163,165,166,169,170,300,32767};
                        auto setChat=[&]{ setTask(VT_CHAT,0x20); o[0xC]=uint8(next()%2); At<CPed*>(o,0x10)=next()%8 ? Ped(1) : nullptr;
                            At<int32>(o,0x14)=int32(next()%5)-1; At<int16>(o,0x18)=ctxs[next()%std::size(ctxs)]; o[0x1A]=uint8(next()%3==0);
                            At<int32>(o,0x1C)=next()%2 ? 45+int32(next()%10) : int32(next()%100)-20; Reg(At<CPed*>(o,0x10),o+0x10); };
                        if(nm=="Constructor"){
                            const uint32 a=next()%2; CPed* b=next()%4 ? Ped(1) : nullptr; const int32 c=int32(next()%8)-2; const int32 d=ctxs[next()%std::size(ctxs)];
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,uint32,CPed*,int32,int32)>(target)(o,a,b,c,d));
                            ctorDone(0x20);
                        } else if(nm=="Destructor"){
                            setChat(); At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; callDtor(); out["object"]=Region(o,0x20,true);
                        } else {
                            setChat(); setSub();
                            if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else {
                                if(nm=="ControlSubTask"){
                                    findRet[1]=next()%4 ? uintptr_t(ptask.data()) : 0;
                                    ptask[0xC]=uint8(next()%2); At<int32>(ptask.data(),0x14)=int32(next()%5)-1; ptask[0x1A]=uint8(next()%3==0);
                                }
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                            }
                            out["object"]=Region(o,0x20,true); out["ptask"]=Region(ptask.data(),0x20,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexDiveFromAttachedEntityAndGetUp") {
                        // +0xC vehiculo a evitar (siempre nulo), +0x10 tiempo inconsciente, +0x14 direccion, +0x20 orientarse primero
                        auto setDive=[&]{ setTask(VT_DIVE,0x24); At<void*>(o,0xC)=nullptr; At<int32>(o,0x10)=int32(next()%5000);
                            At<CVector>(o,0x14)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; o[0x20]=uint8(next()%2); };
                        if(nm=="Constructor"){
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32)>(target)(o,next()%4 ? int32(next()%5000) : -int32(next()%100)));
                            ctorDone(0x24);
                        } else if(nm=="Destructor"){
                            setDive(); At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; callDtor(); out["object"]=Region(o,0x24,true);
                        } else {
                            setDive(); setSub();
                            if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else {
                                At<float>(p0,0x558)=uni(-6.f,6.f); At<float>(p0,0x55C)=uni(-6.f,6.f); At<float>(p0,0x77C)=uni(-6.f,6.f);
                                if(nm=="CreateNextSubTask"){
                                    static const int32 t[]{TASK_SIMPLE_ACHIEVE_HEADING,TASK_SIMPLE_ACHIEVE_HEADING,TASK_SIMPLE_EVASIVE_DIVE,TASK_SIMPLE_PAUSE,TASK_SIMPLE_GET_UP};
                                    At<int32>(fakes[0].data(),FK_TYPE)=t[next()%std::size(t)];
                                } else if(nm=="CreateFirstSubTask"){
                                    const uint32 m=next()%5; At<void*>(p0,PH_ATTACHED)=m==0 ? nullptr : m==1 ? (void*)Ped(1) : (void*)Veh(m%2);
                                    // velocidad alineada con la direccion del lado golpeado en la mitad de los casos
                                    if(m>=2&&next()%2){ const CVector d=dirsOut[hitSide]; const float s=next()%2 ? uni(0.f,1.f) : -uni(0.f,1.f); At<CVector>(Veh(m%2),PH_MOVE)=CVector{d.x*s,d.y*s,d.z*s}; }
                                    if(next()%8==0)At<CVector>(Veh(m%2),PH_MOVE)=CVector{0.f,0.f,0.f};
                                    if(next()%8==0){ dotsOut[1]=-dotsOut[3]; planesOut[1]=planesOut[3]; } // empates
                                }
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                pedOut();
                                if(nm=="CreateFirstSubTask"){ // diagnostico: datos de los stubs de geometria
                                    json dg=json::array(); for(auto& d:dirsOut)dg.push_back({F(d.x),F(d.y),F(d.z)});
                                    json pl=json::array(); for(int k=0;k<4;++k)pl.push_back({F(planesOut[k].x),F(planesOut[k].y),F(planesOut[k].z),F(dotsOut[k])});
                                    out["diag"]={{"hit_side",hitSide},{"dirs",dg},{"planes",pl}};
                                }
                            }
                            out["object"]=Region(o,0x24,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexDrivePointRoute") {
                        // +0xC vehiculo, +0x10 ruta, +0x14 velocidad, +0x18 modo, +0x1C modelo, +0x20 radio, +0x24 estilo (dword, bytes altos a cero), +0x28 indice
                        auto setRoute=[&]{ setTask(VT_DPR,0x2C); At<CVehicle*>(o,0xC)=next()%4 ? Veh(0) : nullptr; At<void*>(o,0x10)=routeIn.data();
                            At<float>(o,0x14)=uni(0.f,40.f); At<uint32>(o,0x18)=next()%4; At<int32>(o,0x1C)=next()%3 ? -1 : int32(400+next()%200);
                            At<float>(o,0x20)=uni(0.f,10.f); At<uint32>(o,0x24)=next()%5; At<uint32>(o,0x28)=next()%7; };
                        if(nm=="Constructor"){
                            if(next()%16==0)At<uint32>(routeIn.data(),0)=0xFFFFFFFFu;
                            CVehicle* v=next()%4 ? Veh(0) : nullptr; const float sp=uni(0.f,40.f), rad=uni(0.f,10.f); const uint32 mode=next()%4, style=next()%5;
                            const int32 model=next()%3 ? -1 : int32(400+next()%200);
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,const void*,float,uint32,int32,float,uint32)>(target)(o,v,routeIn.data(),sp,mode,model,rad,style));
                            ctorDone(0x2C);
                        } else if(nm=="Destructor"){
                            setRoute(); At<void*>(o,0x10)=preRoute; At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; callDtor(); out["object"]=Region(o,0x2C,true);
                        } else {
                            setRoute(); setSub();
                            if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else {
                                if(nm=="CreateNextSubTask"&&next()%3==0)At<uint32>(routeIn.data(),0)=At<uint32>(o,0x28)+1;
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                            }
                            out["object"]=Region(o,0x2C,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexGoToPointAndStandStill") {
                        // +0xC estado de movimiento, +0x10 punto, +0x1C radio, +0x20 radio de paso, +0x24 flags
                        static const int32 ms[]{4,6,7,6,7,1,5,-1};
                        auto radius=[&]{ const uint32 m=next()%8; return m==0 ? qnan : m==1 ? 0.5f : m==2 ? 1.f : m==3 ? 1.5f : uni(0.f,3.f); };
                        auto msRadius=[&]{ const uint32 m=next()%6; return m==0 ? qnan : m<3 ? 2.f : uni(0.f,12.f); };
                        const CVector pp=p0->GetPosition();
                        if(nm=="Constructor"||nm=="GoToPoint"){
                            const CVector pos{pp.x+uni(-30.f,30.f),pp.y+uni(-30.f,30.f),pp.z+uni(-3.f,3.f)};
                            const float r=radius(), msr=msRadius();
                            if(nm=="GoToPoint"){ setTask(VT_GTP,0x28); At<int32>(o,0xC)=ms[next()%std::size(ms)]; At<CVector>(o,0x10)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                                                  At<float>(o,0x1C)=radius(); At<float>(o,0x20)=msRadius(); }
                            // valores previos que coinciden con los argumentos (sin actualizacion) o radio NaN
                            if(next()%3==0)At<CVector>(o,0x10)=pos; else if(next()%4==0){ const uint32 k=next()%3; At<CVector>(o,0x10)=pos; At<float>(o,0x10+4*k)+=1.f; } // difiere en una componente
                            if(next()%3==0)At<float>(o,0x20)=msr;
                            if(next()%5==0)At<float>(o,0x1C)=qnan;
                            if(nm=="Constructor"){
                                ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,const CVector*,float,float,uint32,uint32)>(target)(o,ms[next()%std::size(ms)],&pos,r,msr,next()%2,next()%2));
                                ctorDone(0x28);
                            } else {
                                reinterpret_cast<void(__thiscall*)(void*,const CVector*,float,float,uint32)>(target)(o,&pos,r,msr,next()%4==0);
                                out["object"]=Region(o,0x28,true);
                            }
                        } else {
                            setTask(VT_GTP,0x28); setSub(); At<int32>(o,0xC)=ms[next()%std::size(ms)];
                            At<CVector>(o,0x10)=CVector{pp.x+uni(-30.f,30.f),pp.y+uni(-30.f,30.f),pp.z+uni(-3.f,3.f)};
                            At<float>(o,0x1C)=uni(0.f,3.f); At<float>(o,0x20)=next()%2 ? 2.f : uni(0.f,12.f);
                            inVeh(p0,next()%3==0); At<CVehicle*>(p0,P_VEHICLE)=Veh(0);
                            if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else {
                                auto* s=fakes[0].data();
                                At<int32>(s,0x8)=int32(next()%8); At<CVector>(s,0xC)=CVector{pp.x+uni(-30.f,30.f),pp.y+uni(-30.f,30.f),pp.z}; s[0x20]=uint8(next());
                                if(nm=="CreateNextSubTask"){
                                    static const int32 t[]{TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_PAUSE,TASK_FINISHED};
                                    At<int32>(s,FK_TYPE)=t[next()%std::size(t)];
                                } else if(nm=="ControlSubTask"){
                                    static const int32 t[]{TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_LEAVE_CAR};
                                    At<int32>(s,FK_TYPE)=t[next()%std::size(t)];
                                }
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                out["sub"]=Region(s,0x30,true);
                            }
                            out["object"]=Region(o,0x28,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexHitResponse") {
                        // +0xC lado del golpe
                        static const int32 sides[]{0,1,2,3,0,1,2,3,4,-1,100};
                        if(nm=="Constructor"){
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32)>(target)(o,sides[next()%std::size(sides)]));
                            ctorDone(0x10);
                        } else {
                            setTask(VT_HIT,0x10); At<int32>(o,0xC)=sides[next()%std::size(sides)];
                            if(nm=="Destructor"){ At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; callDtor(); }
                            else {
                                setSub();
                                if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                                else { auto* r=callTask(p0); collectFresh(r); ret=Name(r); }
                            }
                            out["object"]=Region(o,0x10,true);
                        }
                    }
                    else if(cls=="CTaskComplexInWater") {
                        if(nm=="Constructor"){
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o));
                            ctorDone(0xC);
                        } else {
                            setTask(VT_WATER,0xC);
                            if(nm=="Destructor"){ At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; callDtor(); }
                            else {
                                setSub();
                                if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                                else {
                                    At<uint32>(p0,P_FLAGS)=next(); At<uint32>(p0,P_FLAGS2)=next();
                                    if(nm=="CreateNextSubTask"){
                                        // subtarea: CTaskSimpleSwim falsa (+0x38 entidad, +0x3C punto de agarre, +0x48 angulo, +0x4C superficie)
                                        auto* s=fakes[0].data(); At<int32>(s,FK_TYPE)=next()%4 ? TASK_SIMPLE_SWIM : TASK_SIMPLE_CLIMB;
                                        { const uint32 m=next()%4; At<void*>(s,0x38)=m==0 ? nullptr : m==1 ? (void*)Ped(2) : (void*)Veh(1); }
                                        const float pz=p0->GetPosition().z; const uint32 m=next()%6;
                                        At<CVector>(s,0x3C)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),m==0 ? pz+1.4f : m==1 ? qnan : pz+uni(-1.f,3.f)};
                                        At<float>(s,0x48)=uni(-3.2f,3.2f); s[0x4C]=uint8(next()%180);
                                    } else if(nm=="CreateFirstSubTask"){
                                        // grupo y lider (con o sin datos de jugador), creado por mision, tarea GoToPoint primaria
                                        groupOf[0]=next()%3 ? uintptr_t(groups[0].data()) : 0;
                                        { const uint32 m=next()%4; At<CPed*>(groups[0].data(),G_LEADER)=m==0 ? nullptr : m==1 ? p0 : Ped(1); }
                                        At<void*>(Ped(1),P_PLAYERDATA)=next()%2 ? (void*)playerData.data() : nullptr;
                                        At<void*>(p0,P_PLAYERDATA)=next()%4==0 ? (void*)playerData.data() : nullptr;
                                        At<uint8>(p0,P_CREATED)=uint8(1+next()%2);
                                        findRet[0]=next()%3 ? uintptr_t(fakes[1].data()) : 0; At<int32>(fakes[1].data(),FK_TYPE)=TASK_SIMPLE_GO_TO_POINT;
                                        At<CVector>(fakes[1].data(),0xC)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(-5.f,30.f)};
                                    }
                                    auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                                    out["ped_flags"]=Region(reinterpret_cast<uint8*>(p0)+P_FLAGS,0x8);
                                }
                            }
                            out["object"]=Region(o,0xC,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    for(auto* h:hooks)h->State(false);
                    // rutas nuevas del pool (antes de borrar las tareas, que pueden liberarlas)
                    { auto* rp=GetPointRoutePool(); json routes=json::array();
                      for(size_t k=0;k<routeOcc.size();++k)if(!routeOcc[k]&&!rp->IsFreeSlotAtIndex(k)){ const auto* r=Storage(RouteHdr())+k*ROUTE_SLOT; routes.push_back({{"slot",k},{"words",Region(r,ROUTE_SLOT)}}); }
                      out["routes"]=routes; }
                    // limpieza: tareas nuevas alcanzables (raices), rutas sobrantes y referencias; despues los pools deben quedar como antes
                    const size_t traceBefore=trace.size();
                    for(auto* t:roots)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    { auto* rp=GetPointRoutePool(); json leaked=json::array();
                      for(size_t k=0;k<routeOcc.size();++k)if(!routeOcc[k]&&!rp->IsFreeSlotAtIndex(k)){ leaked.push_back(k); CPointRoute::operator delete(rp->GetAt(k),sizeof(CPointRoute)); }
                      out["routes_left"]=leaked; }
                    CleanRefs();
                    out["ret"]=ret; out["trace"]=trace; out["delete_trace"]=delTrace; out["rng"]=rngCalls;
                    out["clean"]=Occupancy()==occ&&RouteOccupancy()==routeOcc;
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
    ResetPool(RouteHdr(),ROUTE_SLOT,lr0,routeStatesStart,routeOccStart,RouteOccupancy());
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=Occupancy()==occupancyStart&&RouteOccupancy()==routeOccStart&&freeRefs()==savedFree&&Last(TaskHdr())==lf0&&Last(RouteHdr())==lr0;
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
