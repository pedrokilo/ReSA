#pragma once
// Banco dentro del motor: tareas complejas B (lista de 8, 48 funciones).
// CTaskComplexGetOnBoatSeat, CTaskComplexHitByGunResponse y CTaskComplexInvestigateDisturbance (constructor, destructor,
// Clone, GetTaskType, MakeAbortable, ControlSubTask, CreateFirstSubTask, CreateNextSubTask) y CTaskComplexLeaveBoat,
// CTaskComplexLeaveCarAndFlee y CTaskComplexLeaveCarAndWander (constructor, destructor, Clone, GetTaskType, CreateSubTask,
// ControlSubTask, CreateFirstSubTask, CreateNextSubTask).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 4 peatones (con inteligencia propia), 2 vehiculos (vtable falsa con trampas, handling falso y model info falso en un
// hueco libre de CModelInfo::ms_modelInfoPtrs), eventos falsos en la historia del gestor de eventos y tareas falsas
// (vtable trazada).
// Se simulan y trazan: CPed::Say, CPedIntelligence::SetTaskDuckSecondary, IKChainManager_c::LookAt,
// CTaskComplexWander::GetWanderTaskByPedType (devuelve una tarea falsa; CTaskManager::SetTask es real sobre la
// inteligencia del fixture) y el RNG.
// Pool de tareas: m_LastFreeSlot y estados se restauran y los slots libres se rellenan con 0xCD en cada ejecucion.
// Se compara la memoria de los fixtures (punteros traducidos a nombres), las tareas nuevas del pool, las referencias,
// el retorno y la traza.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/Task.h"
#include "TaskComplexGetOnBoatSeat.h"
#include "TaskComplexHitByGunResponse.h"
#include "TaskComplexInvestigateDisturbance.h"
#include "TaskComplexLeaveBoat.h"
#include "TaskComplexLeaveCarAndFlee.h"
#include "TaskComplexLeaveCarAndWander.h"
#include "TaskComplexWander.h"
#include "PedIntelligence.h"
#include "Ragdoll/IKChainManager.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks8bLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }

constexpr int NPEDS=4, NVEH=2, FAKES=3, NEV=2;
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_SUB=0x64, FK_TYPE=0x68, FK_CLONE=0x6C;
constexpr size_t EV_SIZE=0x20, EV_TYPE=0x10;
// desplazamientos crudos del EXE
constexpr size_t E_MATRIX=0x14, E_RWOBJ=0x18, E_MODEL=0x22, E_TYPE=0x36;                          // CEntity
constexpr size_t P_FLAGS=0x46C, P_INTEL=0x47C, P_ANIMGROUP=0x4D4, P_VEHICLE=0x58C;                // CPed
constexpr size_t V_HANDLING=0x384, V_TYPE=0x590, H_FLAGS=0xCC;                                     // CVehicle / tHandlingData
constexpr size_t I_PRIMARY_DEFAULT=0x14, I_HIST_NONTEMP=0x3C, I_HIST_TEMP=0x40;                    // CPedIntelligence
constexpr size_t MI_COL=0x14, COL_RADIUS=0x24;                                                     // CBaseModelInfo / CColModel
constexpr uintptr_t VT_BOAT=0x86F044, VT_HBG=0x86E06C, VT_INV=0x870010, VT_LB=0x86E854, VT_LCF=0x86E8AC, VT_LCW=0x86E880;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<std::array<uint8,0x100>,NVEH> handlings{};
alignas(16) inline std::array<uint8,0x40> modelInfo{};
alignas(16) inline std::array<uint8,0x40> colModel{};
alignas(16) inline std::array<uint8,0x100> object{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,EV_SIZE>,NEV> events{};
alignas(16) inline std::array<uint8,0x40> clump{};
alignas(16) inline CVector argVec{};
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
inline bool IsPoolTask(uintptr_t v) { const int k=SlotIndex(TaskHdr(),TASK_SLOT,v); return k>=0&&!(States(TaskHdr())[k]&0x80); }
inline void ResetPool(uint8* h,size_t slot,int32 last,const std::vector<uint8>& statesStart,const std::vector<bool>& occStart,const std::vector<bool>& occNow) {
    Last(h)=last;
    if(occNow==occStart)std::memcpy(States(h),statesStart.data(),statesStart.size());
    for(int32 k=0;k<Cap(h);++k)if(States(h)[k]&0x80)std::memset(Storage(h)+k*slot,0xCD,slot);
}

inline std::array<void*,12> fakeVtbl{}, eventVtbl{};
inline std::array<void*,96> vehVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(vehVtbl.data()))return "veh_vt"; if(v==uintptr_t(eventVtbl.data()))return "event_vt";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n);
                              if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n);
                              if(auto s=in(handlings[n].data(),handlings[n].size(),"handling"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NEV;++n)if(auto s=in(events[n].data(),events[n].size(),"ev"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(clump.data(),clump.size(),"clump"))return *s;
    if(auto s=in(modelInfo.data(),modelInfo.size(),"model_info"))return *s; if(auto s=in(colModel.data(),colModel.size(),"col_model"))return *s;
    if(auto s=in(&argVec,sizeof(argVec),"arg_vec"))return *s;
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

inline uint32 seed{}, rngCalls{};

// tareas falsas
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(t)}); return At<CTask*>(t,FK_CLONE); }
inline CTask* __fastcall FSub(CTask* t,void*) { return At<CTask*>(t,FK_SUB); }
inline bool __fastcall FSimple(CTask*,void*) { return true; }
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) { trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e)}); return At<uint8>(t,FK_ABORT)!=0; }
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<int... N> void FillTraps(std::array<void*,sizeof...(N)>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// eventos falsos: el EXE consulta el tipo una vez por comparacion
inline void* __fastcall EvDtor(void* e,void*,unsigned f) { trace.push_back({"delete_event",Name(e),f}); return e; }
inline int32 __fastcall EvType(void* e,void*) { trace.push_back({"event_type",Name(e)}); return At<int32>(e,EV_TYPE); }

// dependencias simuladas
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
// el EXE empuja el contexto con basura en los 16 bits altos: solo se traza el int16
inline int16 __fastcall FakeSay(CPed* p,void*,int ctx,uint32 delay,float prob,uint32 overrideSilence,uint32 forceAudible,uint32 frontEnd) {
    trace.push_back({"say",Name(p),int16(ctx),delay,F(prob),overrideSilence&0xFF,forceAudible&0xFF,frontEnd&0xFF}); return 0;
}
inline void __fastcall FakeDuck(CPedIntelligence* in,void*,uint32 len) { trace.push_back({"duck_secondary",Name(in),len&0xFFFF}); }
inline void __fastcall FakeLookAt(void* m,void*,const char* purpose,CPed* p,CEntity* e,int32 time,int32 bone,CVector* off,uint32 torso,float speed,int32 blend,int32 prio,uint32 force) {
    std::string s; for(int k=0;k<32&&purpose&&purpose[k];++k)s+=purpose[k];
    trace.push_back({"look_at",Name(m),s,Name(p),Name(e),time,bone,Name(off),torso&0xFF,F(speed),blend,prio,force&0xFF});
}
inline CTaskComplexWander* __cdecl FakeWander(CPed* p) { trace.push_back({"wander_task_by_ped_type",Name(p)}); return reinterpret_cast<CTaskComplexWander*>(Fake(2)); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=48;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexGetOnBoatSeat","Clone",0x64A3B0,0x86F044,1,128},
    {"CTaskComplexGetOnBoatSeat","Constructor",0x649210,0,-1,256},
    {"CTaskComplexGetOnBoatSeat","ControlSubTask",0x6493D0,0x86F044,10,64},
    {"CTaskComplexGetOnBoatSeat","CreateFirstSubTask",0x649310,0x86F044,9,256},
    {"CTaskComplexGetOnBoatSeat","CreateNextSubTask",0x649300,0x86F044,8,64},
    {"CTaskComplexGetOnBoatSeat","Destructor",0x649280,0,-1,128},
    {"CTaskComplexGetOnBoatSeat","GetTaskType",0x649270,0x86F044,4,64},
    {"CTaskComplexGetOnBoatSeat","MakeAbortable",0x6492E0,0x86F044,6,256},
    {"CTaskComplexHitByGunResponse","Clone",0x636640,0x86E06C,1,256},
    {"CTaskComplexHitByGunResponse","Constructor",0x631DD0,0,-1,256},
    {"CTaskComplexHitByGunResponse","ControlSubTask",0x631F70,0x86E06C,10,64},
    {"CTaskComplexHitByGunResponse","CreateFirstSubTask",0x631E50,0x86E06C,9,512},
    {"CTaskComplexHitByGunResponse","CreateNextSubTask",0x631E40,0x86E06C,8,64},
    {"CTaskComplexHitByGunResponse","Destructor",0x631E30,0,-1,128},
    {"CTaskComplexHitByGunResponse","GetTaskType",0x631DF0,0x86E06C,4,64},
    {"CTaskComplexHitByGunResponse","MakeAbortable",0x631E00,0x86E06C,6,256},
    {"CTaskComplexInvestigateDisturbance","Clone",0x66D3D0,0x870010,1,256},
    {"CTaskComplexInvestigateDisturbance","Constructor",0x66C720,0,-1,256},
    {"CTaskComplexInvestigateDisturbance","ControlSubTask",0x66CA30,0x870010,10,64},
    {"CTaskComplexInvestigateDisturbance","CreateFirstSubTask",0x66C820,0x870010,9,1024},
    {"CTaskComplexInvestigateDisturbance","CreateNextSubTask",0x6749F0,0x870010,8,1024},
    {"CTaskComplexInvestigateDisturbance","Destructor",0x66C7A0,0,-1,128},
    {"CTaskComplexInvestigateDisturbance","GetTaskType",0x66C790,0x870010,4,64},
    {"CTaskComplexInvestigateDisturbance","MakeAbortable",0x66C800,0x870010,6,256},
    {"CTaskComplexLeaveBoat","Clone",0x63DA60,0x86E854,1,128},
    {"CTaskComplexLeaveBoat","Constructor",0x63BB60,0,-1,256},
    {"CTaskComplexLeaveBoat","ControlSubTask",0x63BC40,0x86E854,10,64},
    {"CTaskComplexLeaveBoat","CreateFirstSubTask",0x642270,0x86E854,9,256},
    {"CTaskComplexLeaveBoat","CreateNextSubTask",0x63BC30,0x86E854,8,64},
    {"CTaskComplexLeaveBoat","CreateSubTask",0x63BC50,0,-1,256},
    {"CTaskComplexLeaveBoat","Destructor",0x63BBD0,0,-1,128},
    {"CTaskComplexLeaveBoat","GetTaskType",0x63BBC0,0x86E854,4,64},
    {"CTaskComplexLeaveCarAndFlee","Clone",0x63DB40,0x86E8AC,1,256},
    {"CTaskComplexLeaveCarAndFlee","Constructor",0x63BF90,0,-1,256},
    {"CTaskComplexLeaveCarAndFlee","ControlSubTask",0x63C090,0x86E8AC,10,64},
    {"CTaskComplexLeaveCarAndFlee","CreateFirstSubTask",0x6424C0,0x86E8AC,9,512},
    {"CTaskComplexLeaveCarAndFlee","CreateNextSubTask",0x642390,0x86E8AC,8,1024},
    {"CTaskComplexLeaveCarAndFlee","CreateSubTask",0x63C0A0,0,-1,512},
    {"CTaskComplexLeaveCarAndFlee","Destructor",0x63C030,0,-1,128},
    {"CTaskComplexLeaveCarAndFlee","GetTaskType",0x63C020,0x86E8AC,4,64},
    {"CTaskComplexLeaveCarAndWander","Clone",0x63DAD0,0x86E880,1,256},
    {"CTaskComplexLeaveCarAndWander","Constructor",0x63BDD0,0,-1,256},
    {"CTaskComplexLeaveCarAndWander","ControlSubTask",0x63BEB0,0x86E880,10,64},
    {"CTaskComplexLeaveCarAndWander","CreateFirstSubTask",0x642340,0x86E880,9,512},
    {"CTaskComplexLeaveCarAndWander","CreateNextSubTask",0x6422C0,0x86E880,8,1024},
    {"CTaskComplexLeaveCarAndWander","CreateSubTask",0x63BEC0,0,-1,512},
    {"CTaskComplexLeaveCarAndWander","Destructor",0x63BE50,0,-1,128},
    {"CTaskComplexLeaveCarAndWander","GetTaskType",0x63BE40,0x86E880,4,64},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks8b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks8b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks8b");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks8b-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // hueco libre de CModelInfo::ms_modelInfoPtrs para el model info falso de los vehiculos
    int32 miSlot=-1; for(int32 k=19999;k>=15000;--k)if(!CModelInfo::ms_modelInfoPtrs[k]){miSlot=k;break;}
    if(miSlot<0){Write({{"event","error"},{"reason","no free model info slot"}});return;}
    FillTraps(fakeVtbl,std::make_integer_sequence<int,12>{}); FillTraps(eventVtbl,std::make_integer_sequence<int,12>{}); FillTraps(vehVtbl,std::make_integer_sequence<int,96>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    eventVtbl[0]=reinterpret_cast<void*>(&EvDtor); eventVtbl[1]=reinterpret_cast<void*>(&EvType);
    const auto occupancyStart=Occupancy();
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{uintptr_t(&CModelInfo::ms_modelInfoPtrs[miSlot]),4}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    const int32 lf0=Last(TaskHdr());
    const std::vector<uint8> statesStart(States(TaskHdr()),States(TaskHdr())+Cap(TaskHdr()));
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch s1(bit_cast<uintptr_t>(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say)),FakeSay), s1e(0x5EFFE0,FakeSay);
        EntryPatch s2(bit_cast<uintptr_t>(&CPedIntelligence::SetTaskDuckSecondary),FakeDuck), s2e(0x601230,FakeDuck);
        EntryPatch s3(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),FakeLookAt), s3e(0x618970,FakeLookAt);
        EntryPatch s4(bit_cast<uintptr_t>(&CTaskComplexWander::GetWanderTaskByPedType),FakeWander), s4e(0x673D00,FakeWander);

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
                    restoreGlobals();
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5B5Bu; if(!st)st=1; trace=json::array(); seed=uint32(i);
                    rngCalls=0; freshIds.clear();
                    CleanRefs();
                    // peatones: matriz, inteligencia propia, grupo de animaciones, vehiculo
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                        p[E_TYPE]=3; At<void*>(p,P_INTEL)=intels[n].data(); At<void*>(p,E_RWOBJ)=clump.data(); At<void*>(p,E_MATRIX)=pedMats[n].data();
                        for(int r=0;r<3;++r)At<CVector>(pedMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(pedMats[n].data(),0x30)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        At<int16>(p,E_MODEL)=int16(next()%300); At<int32>(p,P_ANIMGROUP)=int32(next()%120);
                        At<void*>(intels[n].data(),0)=p; // m_pPed de la inteligencia
                    }
                    // vehiculos: vtable falsa con trampas, matriz, handling falso, tipo y model info falso
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size()); std::memset(handlings[n].data(),0,handlings[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%8)<<3); At<void*>(v,E_MATRIX)=vehMats[n].data();
                        for(int r=0;r<3;++r)At<CVector>(vehMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        At<void*>(v,V_HANDLING)=handlings[n].data(); At<uint32>(handlings[n].data(),H_FLAGS)=next();
                        { static const int32 types[]{0,5,5,9,3,5}; At<int32>(v,V_TYPE)=types[next()%6]; }
                        At<int16>(v,E_MODEL)=int16(miSlot);
                    }
                    std::memset(modelInfo.data(),0,modelInfo.size()); std::memset(colModel.data(),0,colModel.size());
                    At<void*>(modelInfo.data(),MI_COL)=colModel.data(); At<float>(colModel.data(),COL_RADIUS)=next()%8 ? uni(0.f,12.f) : 0.f;
                    CModelInfo::ms_modelInfoPtrs[miSlot]=reinterpret_cast<CBaseModelInfo*>(modelInfo.data());
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,FAKE_SIZE); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_PAUSE+n; fakes[n][FK_ABORT]=uint8(next()%2); }
                    for(int n=0;n<NEV;++n){ std::memset(events[n].data(),0,EV_SIZE); At<void*>(events[n].data(),0)=eventVtbl.data(); }
                    std::memset(clump.data(),0,clump.size()); clump[0]=2;
                    std::memset(object.data(),0xCD,object.size());

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
                    auto setSub=[&](){ At<CTask*>(o,8)=Fake(0); };
                    auto typeOf=[&](void* p){ return reinterpret_cast<CTask*>(p)->GetTaskType(); };
                    auto callTask=[&](CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    auto callClone=[&]{ return reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); };
                    auto callDtor=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); };
                    auto callSubTask=[&](int32 type){ return reinterpret_cast<CTask*(__thiscall*)(void*,int32)>(target)(o,type); };
                    auto callAbort=[&]{
                        static const int32 prios[]{0,1,2,1,2,3,-1};
                        const int32 prio=prios[next()%std::size(prios)]; const CEvent* ev=next()%2 ? reinterpret_cast<const CEvent*>(events[0].data()) : nullptr;
                        return reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,p0,prio,ev);
                    };
                    auto inVeh=[&](CPed* p,bool on){ auto& fl=At<uint32>(p,P_FLAGS); fl=on ? (fl|0x100u) : (fl&~0x100u); };
                    auto ctorDone=[&](size_t size){ for(auto* h:hooks)h->State(false); out["object"]=Region(o,size,true); out["type"]=typeOf(o); };
                    auto anyEntity=[&]()->CEntity*{ const uint32 m=next()%5; return m==0 ? nullptr : m<3 ? (CEntity*)Ped(1+m%2) : (CEntity*)Veh(m%2); };
                    auto anyVeh=[&]()->CVehicle*{ const uint32 m=next()%4; return m==0 ? nullptr : Veh(m%2); };
                    // comun: Clone, ControlSubTask, CreateFirstSubTask y CreateNextSubTask con la subtarea falsa 0
                    auto common=[&](size_t size,auto&& set,auto&& prep){
                        if(nm=="Destructor"){ set(); At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; callDtor(); }
                        else if(nm=="MakeAbortable"){ set(); At<CTask*>(o,8)=Fake(0); prep(); ret=callAbort(); }
                        else if(nm=="CreateSubTask"){
                            set(); static const int32 t[]{TASK_SIMPLE_CAR_SET_PED_OUT,TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_GO_TO_POINT,TASK_COMPLEX_SMART_FLEE_POINT,
                                                          TASK_FINISHED,TASK_NONE,TASK_SIMPLE_PAUSE,-1,0x7FFFFFFF};
                            prep(); auto* r=callSubTask(t[next()%std::size(t)]); collectFresh(r); ret=Name(r);
                        }
                        else { set(); setSub(); prep();
                            auto* r=nm=="Clone" ? callClone() : callTask(p0); collectFresh(r); ret=Name(r); }
                        out["object"]=Region(o,size,true);
                    };
                    auto none=[]{};

                    if(nm=="GetTaskType") {
                        setTask(fn.vt,0x40);
                        ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        out["object"]=Region(o,0x40,true);
                    }
                    else if(cls=="CTaskComplexGetOnBoatSeat") {
                        // +0xC vehiculo (referencia)
                        if(nm=="Constructor"){
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*)>(target)(o,anyVeh()));
                            ctorDone(0x10);
                        } else {
                            auto set=[&]{ setTask(VT_BOAT,0x10); At<CVehicle*>(o,0xC)=nm=="CreateFirstSubTask" ? Veh(next()%2) : anyVeh(); Reg(At<CVehicle*>(o,0xC),o+0xC); };
                            common(0x10,set,none);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexHitByGunResponse") {
                        // +0xC lado del golpe (dword completo)
                        static const int32 sides[]{0,1,2,3,0,1,2,3,4,-1,0x100,0x102,0x7FFFFFFF};
                        if(nm=="Constructor"){
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32)>(target)(o,sides[next()%std::size(sides)]));
                            ctorDone(0x10);
                        } else {
                            auto set=[&]{ setTask(VT_HBG,0x10); At<int32>(o,0xC)=sides[next()%std::size(sides)]; };
                            common(0x10,set,none);
                        }
                    }
                    else if(cls=="CTaskComplexInvestigateDisturbance") {
                        // +0xC punto, +0x18 entidad (referencia)
                        const CVector pp=p0->GetPosition();
                        auto point=[&]{ const uint32 m=next()%12; return m==0 ? pp : m==1 ? CVector{qnan,pp.y,pp.z} : CVector{pp.x+uni(-30.f,30.f),pp.y+uni(-30.f,30.f),pp.z+uni(-3.f,3.f)}; };
                        if(nm=="Constructor"){
                            argVec=point(); CEntity* e=anyEntity();
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,const CVector*,CEntity*)>(target)(o,&argVec,e));
                            ctorDone(0x1C);
                        } else {
                            auto set=[&]{ setTask(VT_INV,0x1C); At<CVector>(o,0xC)=point(); At<CEntity*>(o,0x18)=anyEntity(); Reg(At<CEntity*>(o,0x18),o+0x18); };
                            auto prep=[&]{
                                if(nm=="MakeAbortable"&&next()%4==0)At<CTask*>(o,8)=nullptr;
                                if(nm=="CreateFirstSubTask"){
                                    // eventos de la historia (temporal y no temporal) con tipos 62/63 u otros
                                    static const int32 types[]{62,63,62,63,61,64,0,-1};
                                    for(auto& e:events)At<int32>(e.data(),EV_TYPE)=types[next()%std::size(types)];
                                    const uint32 m=next()%4; At<void*>(intels[0].data(),I_HIST_TEMP)=m&1 ? events[0].data() : nullptr; At<void*>(intels[0].data(),I_HIST_NONTEMP)=m&2 ? events[1].data() : nullptr;
                                    // mitad de los casos con el punto delante del peaton (umbral 0.707 y empates cerca de el)
                                    if(next()%2){ const CVector fw=At<CVector>(pedMats[0].data(),0x10); const float s=uni(-20.f,20.f); At<CVector>(o,0xC)=CVector{pp.x+fw.x*s+uni(-2.f,2.f),pp.y+fw.y*s+uni(-2.f,2.f),pp.z+fw.z*s}; }
                                } else if(nm=="CreateNextSubTask"){
                                    static const int32 t[]{TASK_SIMPLE_ANIM,421,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_COMPLEX_SEEK_ENTITY,TASK_COMPLEX_TURN_TO_FACE_ENTITY,
                                                           TASK_COMPLEX_TURN_TO_FACE_ENTITY,TASK_SIMPLE_GO_TO_POINT,TASK_FINISHED,904};
                                    At<int32>(fakes[0].data(),FK_TYPE)=t[next()%std::size(t)];
                                }
                            };
                            common(0x1C,set,prep);
                            out["intel"]=Region(intels[0].data(),0x48);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexLeaveBoat") {
                        // +0xC vehiculo (referencia), +0x10 retardo
                        if(nm=="Constructor"){
                            CVehicle* v=anyVeh(); const uint32 d=next()%3 ? next()%5000 : next();
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,uint32)>(target)(o,v,d));
                            ctorDone(0x14);
                        } else {
                            auto set=[&]{ setTask(VT_LB,0x14); At<CVehicle*>(o,0xC)=anyVeh(); At<uint32>(o,0x10)=next()%5000; Reg(At<CVehicle*>(o,0xC),o+0xC); };
                            common(0x14,set,none);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexLeaveCarAndFlee") {
                        // +0xC vehiculo (referencia), +0x10 puerta, +0x14 punto de huida, +0x20 retardo, +0x24 esperar a que frene (byte)
                        if(nm=="Constructor"){
                            CVehicle* v=anyVeh(); argVec=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                            const uint32 door=next()%12; const int32 delay=int32(next()%5000)-100; const uint32 wait=next()%2;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,const CVector*,uint32,int32,uint32)>(target)(o,v,&argVec,door,delay,wait));
                            ctorDone(0x28);
                        } else {
                            // con espera el EXE lee la posicion del vehiculo sin comprobar nulo: siempre hay vehiculo en ese caso
                            auto set=[&]{ setTask(VT_LCF,0x28); const uint8 wait=uint8(next()%2); At<CVehicle*>(o,0xC)=wait ? Veh(next()%2) : anyVeh(); At<uint32>(o,0x10)=next()%12;
                                At<CVector>(o,0x14)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)}; At<int32>(o,0x20)=int32(next()%5000); o[0x24]=wait; Reg(At<CVehicle*>(o,0xC),o+0xC); };
                            auto prep=[&]{
                                inVeh(p0,next()%2); At<CVehicle*>(p0,P_VEHICLE)=anyVeh();
                                if(nm=="CreateNextSubTask"){
                                    static const int32 t[]{TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_GO_TO_POINT,TASK_COMPLEX_SMART_FLEE_POINT,
                                                           TASK_COMPLEX_SMART_FLEE_POINT,TASK_SIMPLE_PAUSE,TASK_FINISHED};
                                    At<int32>(fakes[0].data(),FK_TYPE)=t[next()%std::size(t)];
                                    // tarea primaria previa (SetTask la borra)
                                    if(next()%3==0)At<CTask*>(intels[0].data(),I_PRIMARY_DEFAULT)=Fake(1);
                                }
                            };
                            common(0x28,set,prep);
                            out["intel"]=Region(intels[0].data(),0x30);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexLeaveCarAndWander") {
                        // +0xC vehiculo (referencia), +0x10 puerta, +0x14 retardo, +0x18 salida sensata (byte)
                        if(nm=="Constructor"){
                            CVehicle* v=anyVeh(); const uint32 door=next()%12; const int32 delay=int32(next()%5000)-100; const uint32 sens=next()%2;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,uint32,int32,uint32)>(target)(o,v,door,delay,sens));
                            ctorDone(0x1C);
                        } else {
                            auto set=[&]{ setTask(VT_LCW,0x1C); At<CVehicle*>(o,0xC)=anyVeh(); At<uint32>(o,0x10)=next()%12; At<int32>(o,0x14)=int32(next()%5000); o[0x18]=uint8(next()%2);
                                Reg(At<CVehicle*>(o,0xC),o+0xC); };
                            auto prep=[&]{
                                inVeh(p0,next()%2);
                                if(nm=="CreateNextSubTask"){
                                    static const int32 t[]{TASK_NONE,TASK_COMPLEX_LEAVE_CAR,TASK_NONE,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_WANDER,TASK_SIMPLE_PAUSE,TASK_FINISHED};
                                    At<int32>(fakes[0].data(),FK_TYPE)=t[next()%std::size(t)];
                                    // tarea primaria por defecto: nula, de deambular u otra
                                    const uint32 m=next()%3; At<CTask*>(intels[0].data(),I_PRIMARY_DEFAULT)=m==0 ? nullptr : Fake(1);
                                    At<int32>(fakes[1].data(),FK_TYPE)=m==1 ? TASK_COMPLEX_WANDER : TASK_SIMPLE_PAUSE+int32(next()%3);
                                }
                            };
                            common(0x1C,set,prep);
                            out["intel"]=Region(intels[0].data(),0x30);
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
                    restoreGlobals();
                    out["ret"]=ret; out["trace"]=trace; out["delete_trace"]=delTrace; out["rng"]=rngCalls;
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
    restoreGlobals();
    ResetPool(TaskHdr(),TASK_SLOT,lf0,statesStart,occupancyStart,Occupancy());
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&Last(TaskHdr())==lf0&&globalsIntact();
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
