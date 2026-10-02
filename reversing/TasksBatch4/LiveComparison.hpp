#pragma once
// Banco dentro del motor: CTaskComplexUseScriptedAttractor (7), CTaskComplexUseScriptedBrain (8),
// CTaskComplexUseSequence (6), CTaskComplexUseSwatRope (7) y CTaskComplexUseWaterCannon (5).
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos: peaton, helicoptero,
// vehiculo e incendio en buffers propios, tareas falsas con vtable trazada (Clone/GetTaskType/MakeAbortable
// configurables por objeto), evento falso, entradas de CTaskSequences::ms_taskSequence rellenadas por caso
// (instantanea del bloque restaurada tras cada ejecucion). Se simulan y trazan: CScriptedBrainTaskStore
// (GetTask y Clear(CTask*)), CPedPlacement::FindZCoorForPed, CRopes::FindCoorsAlongRope, CPed::Say,
// CAutomobile::FireTruckControl y el RNG. Se compara la memoria de los fixtures (punteros traducidos),
// las tareas creadas, las referencias, las entradas de secuencias cambiadas, el retorno y la traza.
#include <fstream>
#include <unordered_map>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexUseScriptedAttractor.h"
#include "TaskComplexUseScriptedBrain.h"
#include "Tasks/TaskComplexUseSequence.h"
#include "Tasks/TaskSequences.h"
#include "TaskComplexUseSwatRope.h"
#include "TaskComplexUseWaterCannon.h"
#include "Attractors/PedAttractor.h"
#include "Scripts/Scripted2dEffects.h"
#include "Tasks/ScriptedBrainTaskStore.h"
#include "Events/EventScriptCommand.h"
#include "Entity/Vehicle/Heli.h"
#include "Fire.h"
#include "PedPlacement.h"
#include "Ropes.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace Tasks4Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
constexpr int FAKES=8;                 // 0 subtarea, 1-2 tareas del cerebro/evento, 3 resultado de Clone, 4-7 tareas de secuencia
constexpr size_t FK_CLONE=0x34, FK_TYPE=0x38, FK_ABORT=0x3C;   // configuracion por objeto dentro de cada tarea falsa
constexpr bool MISALIGNED_FX=true;     // m_Fx apuntando dentro (no al inicio) de un ms_effects[k]: el EXE compara igualdad exacta
alignas(16) inline std::array<uint8,sizeof(CPed)> ped{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> mat{};
alignas(16) inline std::array<uint8,sizeof(CHeli)> heli{};
alignas(16) inline std::array<uint8,sizeof(CAutomobile)> veh{};
alignas(16) inline std::array<uint8,sizeof(CFire)> fire{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<std::array<uint8,0x40>,FAKES> fakes{};
alignas(16) inline std::array<uint8,0x60> event{};
alignas(16) inline std::array<uint8,sizeof(CPedAttractor)> attractor{};
alignas(16) inline std::array<uint8,0x40> fxFake{};
alignas(16) inline std::array<char,16> nameBuf{};
inline CPed* Ped() { return reinterpret_cast<CPed*>(ped.data()); }
inline CHeli* Heli() { return reinterpret_cast<CHeli*>(heli.data()); }
inline CAutomobile* Veh() { return reinterpret_cast<CAutomobile*>(veh.data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline uint8* const seqs=reinterpret_cast<uint8*>(&CTaskSequences::ms_taskSequence[0]);
constexpr size_t SEQ_BYTES=64*0x40;
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(ped.data(),ped.size(),"ped"))return *s; if(auto s=in(mat.data(),mat.size(),"mat"))return *s;
    if(auto s=in(heli.data(),heli.size(),"heli"))return *s; if(auto s=in(veh.data(),veh.size(),"veh"))return *s;
    if(auto s=in(fire.data(),fire.size(),"fire"))return *s; if(auto s=in(object.data(),object.size(),"object"))return *s;
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    if(auto s=in(event.data(),event.size(),"event"))return *s; if(auto s=in(attractor.data(),attractor.size(),"attractor"))return *s;
    if(auto s=in(fxFake.data(),fxFake.size(),"fxfake"))return *s; if(auto s=in(nameBuf.data(),nameBuf.size(),"name"))return *s;
    if(v>=uintptr_t(seqs)&&v<uintptr_t(seqs)+SEQ_BYTES)return "seq#"+std::to_string((v-uintptr_t(seqs))/0x40)+"+"+std::to_string((v-uintptr_t(seqs))%0x40);
    { const auto fx=uintptr_t(&CScripted2dEffects::ms_effects[0]); if(v>=fx&&v<fx+sizeof(CScripted2dEffects::ms_effects))return "fx#"+std::to_string((v-fx)/sizeof(C2dEffect))+"+"+std::to_string((v-fx)%sizeof(C2dEffect)); }
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
template<size_t N> std::string EntHash(const std::array<uint8,N>& buf) {
    auto copy=buf; const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(buf.data())->m_pReferences)-uintptr_t(buf.data());
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}

// configuracion por caso de las dependencias simuladas
inline int eventType{}; inline bool findZRet{}, ropeRet{}; inline float groundZ{}; inline CVector ropePos{};
inline uintptr_t brainRet{}; inline uint32 seed{}, rngCalls{};

// tareas falsas: cada objeto lleva su resultado de Clone, su tipo y su respuesta a MakeAbortable
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(uintptr_t(t)),f}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(uintptr_t(t))}); return *reinterpret_cast<CTask**>(uintptr_t(t)+FK_CLONE); }
inline CTask* __fastcall FSub(CTask*,void*) { return nullptr; }
inline bool __fastcall FSimple(CTask*,void*) { return false; }
inline int __fastcall FType(CTask* t,void*) { trace.push_back({"get_task_type",Name(uintptr_t(t))}); return *reinterpret_cast<int32*>(uintptr_t(t)+FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int prio,const CEvent* e) { trace.push_back({"make_abortable",Name(uintptr_t(t)),Name(uintptr_t(p)),prio,Name(uintptr_t(e))}); return *reinterpret_cast<uint8*>(uintptr_t(t)+FK_ABORT)!=0; }
template<int N> void* __fastcall FTrap(CTask*,void*) { trace.push_back({"unexpected_slot",N}); return nullptr; }
inline std::array<void*,12> fakeVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// evento falso (GetEventType en el hueco 1)
inline int __fastcall EType(void* e,void*) { trace.push_back({"event_get_type",Name(uintptr_t(e))}); return eventType; }
template<int N> void* __fastcall ETrap(void*,void*) { trace.push_back({"event_unexpected_slot",N}); return nullptr; }
inline std::array<void*,16> eventVtbl{};
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }

// dependencias
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline CTask* __cdecl StoreGet(CPed* p) { trace.push_back({"brain_store_get",Name(uintptr_t(p))}); return reinterpret_cast<CTask*>(brainRet); }
inline void __cdecl StoreClearTask(CTask* t) { trace.push_back({"brain_store_clear_task",Name(uintptr_t(t))}); }
// EXE 0x616920: bool FindZCoorForPed(CVector* pos) modifica pos en el sitio (lea ecx,[esp+0Ch]; push ecx)
inline bool __cdecl FindZExe(CVector* pos) { trace.push_back({"find_z",F(pos->x),F(pos->y),F(pos->z)}); if(findZRet)pos->z=groundZ; return findZRet; }
// DLL: std::pair<CVector,bool> FindZCoorForPed(CVector pos), mismo comportamiento simulado
inline std::pair<CVector,bool> __cdecl FindZDll(CVector pos) { trace.push_back({"find_z",F(pos.x),F(pos.y),F(pos.z)}); if(findZRet)pos.z=groundZ; return {pos,findZRet}; }
inline bool __cdecl FindRope(uint32 id,float dist,CVector* out,CVector* speed) {
    trace.push_back({"find_coors_along_rope",id,F(dist),out!=nullptr,speed!=nullptr}); if(ropeRet&&out)*out=ropePos; return ropeRet;
}
inline int16 __fastcall Say(CPed* p,void*,int ctx,uint32 delay,float prob,bool overrideSilence,bool forceAudible,bool frontEnd) {
    trace.push_back({"say",Name(uintptr_t(p)),ctx,delay,F(prob),overrideSilence,forceAudible,frontEnd}); return 0;
}
inline void __fastcall FireTruck(CAutomobile* a,void*,CFire* f) { trace.push_back({"fire_truck_control",Name(uintptr_t(a)),Name(uintptr_t(f))}); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
enum Kind { ATR, BRN, SEQ, SWT, WAT };
struct Fn { const char* cls; const char* name; uintptr_t address; int slot; int cases; Kind kind; };
constexpr uintptr_t VT[]{0x86E2DC,0x86E568,0x86E518,0x86F59C,0x86F5D0};
constexpr size_t SZ[]{sizeof(CTaskComplexUseScriptedAttractor),sizeof(CTaskComplexUseScriptedBrain),sizeof(CTaskComplexUseSequence),sizeof(CTaskComplexUseSwatRope),sizeof(CTaskComplexUseWaterCannon)};
constexpr size_t NFNS=33;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexUseScriptedAttractor","Constructor",0x633330,-1,64,ATR},{"CTaskComplexUseScriptedAttractor","Destructor",0x633360,-1,128,ATR},
    {"CTaskComplexUseScriptedAttractor","Clone",0,1,128,ATR},{"CTaskComplexUseScriptedAttractor","GetTaskType",0,4,32,ATR},
    {"CTaskComplexUseScriptedAttractor","CreateNextSubTask",0,8,64,ATR},{"CTaskComplexUseScriptedAttractor","CreateFirstSubTask",0,9,1024,ATR},
    {"CTaskComplexUseScriptedAttractor","ControlSubTask",0,10,128,ATR},
    {"CTaskComplexUseScriptedBrain","Constructor",0x6358B0,-1,256,BRN},{"CTaskComplexUseScriptedBrain","Destructor",0x635900,-1,256,BRN},
    {"CTaskComplexUseScriptedBrain","Clone",0,1,256,BRN},{"CTaskComplexUseScriptedBrain","GetTaskType",0,4,32,BRN},
    {"CTaskComplexUseScriptedBrain","MakeAbortable",0,6,1024,BRN},{"CTaskComplexUseScriptedBrain","CreateNextSubTask",0,8,512,BRN},
    {"CTaskComplexUseScriptedBrain","CreateFirstSubTask",0,9,512,BRN},{"CTaskComplexUseScriptedBrain","ControlSubTask",0,10,1024,BRN},
    {"CTaskComplexUseSequence","Constructor",0x635450,-1,256,SEQ},{"CTaskComplexUseSequence","Clone",0x637100,1,256,SEQ},
    {"CTaskComplexUseSequence","ControlSubTask",0x635530,10,128,SEQ},{"CTaskComplexUseSequence","CreateFirstSubTask",0x6354D0,9,1024,SEQ},
    {"CTaskComplexUseSequence","CreateNextSubTask",0x6354A0,8,1024,SEQ},{"CTaskComplexUseSequence","MakeAbortable",0x639730,6,2048,SEQ},
    {"CTaskComplexUseSwatRope","Constructor",0x659470,-1,128,SWT},{"CTaskComplexUseSwatRope","CreateSubTask",0x659620,-1,128,SWT},
    {"CTaskComplexUseSwatRope","Clone",0,1,128,SWT},{"CTaskComplexUseSwatRope","ControlSubTask",0,10,2048,SWT},
    {"CTaskComplexUseSwatRope","CreateFirstSubTask",0,9,64,SWT},{"CTaskComplexUseSwatRope","CreateNextSubTask",0,8,256,SWT},
    {"CTaskComplexUseSwatRope","MakeAbortable",0,6,2048,SWT},
    {"CTaskComplexUseWaterCannon","Constructor",0x659780,-1,64,WAT},{"CTaskComplexUseWaterCannon","Clone",0,1,64,WAT},
    {"CTaskComplexUseWaterCannon","ControlSubTask",0,10,512,WAT},{"CTaskComplexUseWaterCannon","CreateFirstSubTask",0,9,128,WAT},
    {"CTaskComplexUseWaterCannon","CreateNextSubTask",0,8,128,WAT},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(Ped()); clean(Heli()); clean(Veh());
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"tasks4-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"tasks4-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("tasks4");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"tasks4-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(std::make_integer_sequence<int,12>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    FillETraps(std::make_integer_sequence<int,16>{}); eventVtbl[1]=reinterpret_cast<void*>(&EType);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado por las funciones: se guarda y se restaura en cada ejecucion
    const std::vector<uint8> savedSeqs(seqs,seqs+SEQ_BYTES);
    auto* brainStore=reinterpret_cast<uint8*>(&CScriptedBrainTaskStore::ms_entries); const std::vector<uint8> savedBrainStore(brainStore,brainStore+sizeof(CScriptedBrainTaskStore::ms_entries));
    const auto savedSeqIds=CScripted2dEffects::ms_effectSequenceTaskIDs;
    auto& timeStep=*reinterpret_cast<float*>(0xB7CB5C); const float savedTimeStep=timeStep;
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        // GetTask va en línea en el EXE: se rellena el almacén real (0xC188F0) en vez de simularlo
        EntryPatch d2(bit_cast<uintptr_t>(static_cast<void(*)(CTask*)>(&CScriptedBrainTaskStore::Clear)),StoreClearTask), d2e(0x6357F0,StoreClearTask);
        EntryPatch d3(bit_cast<uintptr_t>(&CPedPlacement::FindZCoorForPed),FindZDll), d3e(0x616920,FindZExe);
        EntryPatch d4(bit_cast<uintptr_t>(&CRopes::FindCoorsAlongRope),FindRope), d4e(0x555E40,FindRope);
        EntryPatch d5(bit_cast<uintptr_t>(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say)),Say), d5e(0x5EFFE0,Say);
        EntryPatch d6(bit_cast<uintptr_t>(&CAutomobile::FireTruckControl),FireTruck), d6e(0x729B60,FireTruck);
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x4444u; if(!st)st=1; trace=json::array(); seed=uint32(i); rngCalls=0;
                    CleanRefs();
                    std::memcpy(seqs,savedSeqs.data(),SEQ_BYTES);
                    // peaton
                    std::memset(ped.data(),0,ped.size()); std::memset(mat.data(),0,mat.size());
                    auto* p=Ped(); p->SetType(ENTITY_TYPE_PED);
                    if(next()%4){ p->m_matrix=reinterpret_cast<CMatrixLink*>(mat.data()); CMatrix m{}; m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)}; std::memcpy(static_cast<CMatrix*>(p->m_matrix),&m,sizeof(CMatrix)); }
                    else p->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    p->m_bUsesCollision=next()%2; p->bIsStanding=next()%2;
                    p->m_vecMoveSpeed=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                    p->m_fCurrentRotation=uni(-3.2f,3.2f); p->m_fAimingRotation=uni(-3.2f,3.2f);
                    { static const float hs[]{19.99f,20.f,20.01f,5.f,100.f,0.f,-1.f,std::numeric_limits<float>::quiet_NaN()}; p->m_fHealth=next()%3 ? hs[next()%8] : uni(0.f,200.f); }
                    p->m_pVehicle=reinterpret_cast<CVehicle*>(veh.data());
                    // helicoptero, vehiculo, incendio
                    std::memset(heli.data(),0,heli.size()); Heli()->SetType(ENTITY_TYPE_VEHICLE);
                    Heli()->m_autoPilot.m_nCarMission=next()%3==0 ? MISSION_HELI_CRASH_AND_BURN : eCarMission(next()%58);
                    { static const float hs[]{1000.f,0.5f,0.f,-0.f,-10.f,std::numeric_limits<float>::quiet_NaN()}; Heli()->m_fHealth=hs[next()%6]; }
                    std::memset(veh.data(),0,veh.size()); Veh()->SetType(ENTITY_TYPE_VEHICLE);
                    Veh()->SetStatus(next()%2 ? STATUS_PHYSICS : eEntityStatus(next()%10));
                    for(auto& b:fire)b=uint8(next());
                    // tareas falsas
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,fakes[n].size()); *reinterpret_cast<void**>(fakes[n].data())=fakeVtbl.data();
                        *reinterpret_cast<CTask**>(fakes[n].data()+FK_CLONE)=next()%3 ? Fake(3) : nullptr; *reinterpret_cast<int32*>(fakes[n].data()+FK_TYPE)=TASK_SIMPLE_STAND_STILL;
                        fakes[n][FK_ABORT]=uint8(next()%4!=0); }
                    *reinterpret_cast<CTask**>(fakes[3].data()+FK_CLONE)=nullptr;
                    *reinterpret_cast<int32*>(fakes[3].data()+FK_TYPE)=next()%2 ? TASK_COMPLEX_USE_SEQUENCE : TASK_SIMPLE_PAUSE;
                    for(int n=4;n<FAKES;++n)*reinterpret_cast<int32*>(fakes[n].data()+FK_TYPE)=TASK_SIMPLE_PAUSE+int32(n);
                    // atractor y efecto falso
                    for(auto& b:attractor)b=uint8(next()); *reinterpret_cast<void**>(attractor.data())=nullptr;
                    for(auto& b:fxFake)b=uint8(next());
                    { const uint32 m=next()%8; auto* base=reinterpret_cast<uint8*>(&CScripted2dEffects::ms_effects[next()%NUM_SCRIPTED_2D_EFFECTS]);
                      void* fx=m<5 ? (void*)base : m==5 ? (void*)fxFake.data() : m==6 ? (MISALIGNED_FX ? (void*)(base+4*(1+next()%15)) : (void*)base) : nullptr;
                      *reinterpret_cast<void**>(attractor.data()+4)=fx; }
                    for(auto& id:CScripted2dEffects::ms_effectSequenceTaskIDs)id=int32(next()%64);
                    // evento
                    for(auto& b:event)b=uint8(next()%4==0 ? next() : 0); *reinterpret_cast<void**>(event.data())=eventVtbl.data();
                    { static const int types[]{EVENT_DEATH,EVENT_HEALTH_REALLY_LOW,EVENT_ON_FIRE,EVENT_IN_WATER,EVENT_WATER_CANNON,EVENT_VEHICLE_DAMAGE_COLLISION,EVENT_VEHICLE_DAMAGE_WEAPON,EVENT_DAMAGE,EVENT_SCRIPT_COMMAND,0,1,2};
                      eventType=types[next()%12]; }
                    { auto* d=reinterpret_cast<CEventDamage*>(event.data()); d->m_damageResponse.m_bHealthZero=next()%2; d->m_bAddToEventGroup=next()%2; }
                    *reinterpret_cast<CTask**>(event.data()+0x10)=next()%4 ? Fake(1) : nullptr; // CEventScriptCommand::m_task
                    *reinterpret_cast<int32*>(fakes[1].data()+FK_TYPE)=next()%2 ? TASK_COMPLEX_USE_EFFECT : TASK_SIMPLE_STAND_STILL;
                    // otras dependencias
                    findZRet=next()%5!=0; ropeRet=next()%4!=0; ropePos=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)};
                    { const float z=p->GetPosition().z; const uint32 m=next()%8; groundZ=m==0 ? z-2.f : m==1 ? std::numeric_limits<float>::quiet_NaN() : z-uni(0.f,4.f); }
                    timeStep=uni(0.2f,3.f);
                    { const uint32 m=next()%3; brainRet=m==0 ? 0 : m==1 ? uintptr_t(Fake(1)) : uintptr_t(Fake(2));
                      auto* st=reinterpret_cast<uint8*>(&CScriptedBrainTaskStore::ms_entries); std::memset(st,0,sizeof(CScriptedBrainTaskStore::ms_entries));
                      if(brainRet){ auto* en=st+(next()%48)*0x10; *reinterpret_cast<void**>(en)=Ped(); *reinterpret_cast<uintptr_t*>(en+0xC)=brainRet; } }
                    const int prio=int(next()%3); const void* ev=next()%5 ? event.data() : nullptr;
                    // tarea probada
                    std::memset(object.data(),0xCD,object.size()); std::memset(object.data(),0,SZ[fn.kind]);
                    *reinterpret_cast<uintptr_t*>(object.data())=VT[fn.kind]; *reinterpret_cast<void**>(object.data()+8)=fakes[0].data();
                    auto* o=object.data(); auto W=[&](size_t off)->uint32&{return *reinterpret_cast<uint32*>(o+off);};
                    auto& subType=*reinterpret_cast<int32*>(fakes[0].data()+FK_TYPE);
                    int32 seqIndex=int32(next()%64), ctorInt=0; CTask* subTask=Fake(0);
                    uintptr_t ctorPtr=0; int createType=TASK_FINISHED;
                    switch(fn.kind){
                    case ATR: W(0xC)=uint32(uintptr_t(attractor.data())); if(nm=="Destructor"&&next()%4==0)subTask=nullptr; break;
                    case BRN: {
                        o[0xC]=uint8(next()%2);
                        const int len=int(next()%8); for(int k=0;k<8;++k)o[0xD+k]=uint8(k<len ? 'A'+next()%26 : k==len ? 0 : next());
                        { const uint32 m=next()%3; W(0x18)=m==0 ? 0 : m==1 ? uint32(uintptr_t(Fake(1))) : uint32(uintptr_t(Fake(2))); }
                        { static const int types[]{TASK_SIMPLE_FINISH_BRAIN,TASK_SIMPLE_PAUSE,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_USE_EFFECT}; subType=types[next()%4]; }
                        { const int nl=int(next()%8); for(int k=0;k<16;++k)nameBuf[k]=char(k<nl ? 'a'+next()%26 : k==nl ? 0 : next()); }
                        if(nm=="Destructor"&&next()%4==0)subTask=nullptr;
                        if(next()%2)eventType=EVENT_SCRIPT_COMMAND;
                        break; }
                    case SEQ: {
                        // entrada de secuencia elegida: tareas falsas, contadores y banderas (el resto del bloque queda como en la partida)
                        if((nm=="CreateFirstSubTask"||nm=="CreateNextSubTask"||nm=="ControlSubTask")&&next()%8==0)seqIndex=-1;
                        const int32 entry=seqIndex>=0 ? seqIndex : int32(next()%64);
                        auto* e=seqs+entry*0x40; auto EW=[&](size_t off)->uint32&{return *reinterpret_cast<uint32*>(e+off);};
                        const bool forever=next()%3==0; const int len=forever ? 1+int(next()%8) : int(next()%9);
                        EW(4)=0; EW(8)=0; EW(0xC)=next()%8;
                        for(int k=0;k<8;++k)EW(0x10+4*k)=k<len&&(k==0||next()%8) ? uint32(uintptr_t(Fake(4+k%4))) : 0;
                        EW(0x30)=forever ? 1 : 0; EW(0x34)=next()%4; EW(0x38)=next()%2; EW(0x3C)=next()%3==0 ? 1 : next()%4;
                        W(0xC)=uint32(seqIndex); W(0x10)=next()%8; W(0x14)=next()%2 ? uint32(-1) : next()%8; W(0x18)=next()%5;
                        ctorInt=entry;
                        if(nm=="ControlSubTask"){ if(next()%6==0)subTask=nullptr; } // MakeAbortable del EXE no comprueba la subtarea
                        if(nm=="MakeAbortable"){ eventType=next()%5<3 ? EVENT_DAMAGE : eventType; W(0xC)=uint32(entry); }
                        break; }
                    case SWT: {
                        W(0xC)=next()%1000; *reinterpret_cast<float*>(o+0x10)=next()%4 ? uni(0.f,30.f) : 0.f; o[0x14]=uint8(next());
                        const bool onHeli=o[0x14]&1; const bool needHeli=nm=="Clone"&&onHeli;
                        CHeli* h=(needHeli||next()%5) ? Heli() : nullptr; W(0x18)=uint32(uintptr_t(h));
                        if(h&&onHeli&&nm!="Constructor")h->RegisterReference(reinterpret_cast<CEntity**>(o+0x18));
                        { static const int types[]{TASK_SIMPLE_PAUSE,TASK_SIMPLE_ABSEIL,TASK_NONE,TASK_SIMPLE_STAND_STILL}; subType=types[next()%4]; }
                        { static const int types[]{TASK_SIMPLE_ABSEIL,TASK_SIMPLE_PAUSE,TASK_NONE,TASK_FINISHED,TASK_SIMPLE_STAND_STILL}; createType=types[next()%5]; }
                        ctorInt=int32(next()%1000);
                        break; }
                    case WAT: {
                        subType=next()%5<3 ? TASK_SIMPLE_CAR_DRIVE : TASK_SIMPLE_STAND_STILL;
                        const bool needFire=nm=="ControlSubTask"&&subType==TASK_SIMPLE_CAR_DRIVE&&Veh()->GetStatus()==STATUS_PHYSICS;
                        W(0xC)=(needFire||next()%4) ? uint32(uintptr_t(fire.data())) : 0;
                        ctorPtr=next()%4 ? uintptr_t(fire.data()) : 0;
                        break; }
                    }
                    // eventos permitidos: SwatRope solo admite evento nulo con prioridad IMMEDIATE
                    const void* evArg=ev;
                    if(fn.kind==SWT&&prio!=2&&!evArg)evArg=event.data();
                    *reinterpret_cast<CTask**>(o+8)=subTask;
                    const std::vector<uint8> preSeqs(seqs,seqs+SEQ_BYTES);
                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(VT[fn.kind]); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    if(fn.slot>=0&&fn.address)switches=switches&&(ReversingProbe::JumpsToDll(fn.address)==candidate);
                    void* self=o; CPed* pd=Ped(); uintptr_t rawRet=0; json ret=nullptr;
                    if(nm=="Constructor"){ std::memset(o,0xCD,object.size());
                        switch(fn.kind){
                        case ATR: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const void*)>(target)(self,attractor.data())); break;
                        case BRN: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const char*)>(target)(self,nameBuf.data())); break;
                        case SEQ: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32)>(target)(self,ctorInt)); break;
                        case SWT: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,uint32,CHeli*)>(target)(self,uint32(ctorInt),Heli())); break;
                        case WAT: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CFire*)>(target)(self,reinterpret_cast<CFire*>(ctorPtr))); break;
                        } }
                    else if(nm=="Destructor")reinterpret_cast<void*(__thiscall*)(void*)>(target)(self);
                    else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(self));
                    else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(self);
                    else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const void*)>(target)(self,pd,prio,evArg);
                    else if(nm=="CreateSubTask")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,int,CPed*)>(target)(self,createType,pd));
                    else rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(self,pd));
                    for(auto* h:hooks)h->State(false);
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    if(ret.is_null()&&nm!="Destructor")ret=Name(rawRet);
                    json tasks=json::array(); for(auto* t:fresh){ auto w=Region(t,0x70,true); if(t->GetTaskType()==TASK_SIMPLE_PAUSE&&w[3].is_number()){ const uint32 m=w[3].get<uint32>()&0xFFFF; w[3]=m ? json(m) : json(nullptr); } tasks.push_back({{"type",t->GetTaskType()},{"words",w}}); } // relleno del temporizador sin escribir en el EXE
                    json objectWords=Region(o,SZ[fn.kind],true);
                    if(nm=="Constructor")objectWords.push_back(reinterpret_cast<CTask*>(self)->GetTaskType()); // vptr EXE o DLL: se compara el tipo
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    refs.push_back(refList(Ped())); refs.push_back(refList(Heli())); refs.push_back(refList(Veh()));
                    const auto& pos=p->GetPosition();
                    json pedInfo={EntHash(ped),Hash(mat.data(),mat.size()),F(pos.x),F(pos.y),F(pos.z),F(p->m_vecMoveSpeed.x),F(p->m_vecMoveSpeed.y),F(p->m_vecMoveSpeed.z),
                                  F(p->m_fAimingRotation),bool(p->bIsStanding),bool(p->m_bUsesCollision)};
                    json fakeWords=json::array(); for(int n=0;n<FAKES;++n)fakeWords.push_back(Region(fakes[n].data(),fakes[n].size(),true));
                    json seqDiff=json::array();
                    for(int k=0;k<64;++k)if(std::memcmp(seqs+k*0x40,preSeqs.data()+k*0x40,0x40))seqDiff.push_back({k,Region(seqs+k*0x40,0x40,true)});
                    const auto seqsNow=Hash(seqs,SEQ_BYTES);
                    const auto seqIds=Hash(CScripted2dEffects::ms_effectSequenceTaskIDs.data(),sizeof(CScripted2dEffects::ms_effectSequenceTaskIDs));
                    json r={{"ret",ret},{"object",objectWords},{"tasks",tasks},{"fakes",fakeWords},{"refs",refs},{"ped",pedInfo},
                            {"heli",EntHash(heli)},{"veh",EntHash(veh)},{"fire",ReversingProbe::Bytes(fire.data(),fire.size())},
                            {"attractor",Region(attractor.data(),attractor.size())},{"event",Region(event.data(),event.size(),true)},
                            {"seq_diff",seqDiff},{"seqs",seqsNow},{"seq_ids",seqIds},{"time_step",F(timeStep)}};
                    // borrado de las tareas creadas: sin vaciados de secuencias (se limpian las banderas antes)
                    for(auto& s:CTaskSequences::ms_taskSequence)s.m_bFlushTasks=false;
                    const size_t traceBefore=trace.size();
                    for(auto* t:fresh)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    const bool clean=Occupancy()==occ;
                    r["seqs_after"]=Hash(seqs,SEQ_BYTES); r["refs_after"]=json::array({refList(Ped()),refList(Heli()),refList(Veh())});
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    r["trace"]=trace; r["delete_trace"]=delTrace; r["rng"]=rngCalls; r["clean"]=clean; r["switches"]=switches;
                    std::memcpy(seqs,savedSeqs.data(),SEQ_BYTES);
                    CleanRefs();
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    CleanRefs();
    std::memcpy(brainStore,savedBrainStore.data(),savedBrainStore.size());
    std::memcpy(seqs,savedSeqs.data(),SEQ_BYTES); CScripted2dEffects::ms_effectSequenceTaskIDs=savedSeqIds; timeStep=savedTimeStep;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree}});
}
}
