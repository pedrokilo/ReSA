#pragma once
// Banco dentro del motor: CTaskComplexCarSlowBeDraggedOutAndStandUp (8 funciones sin validar):
// Constructor (0x648620), Destructor (0x648690), CreateSubTask (0x648710) y las virtuales Clone, MakeAbortable,
// CreateNextSubTask, CreateFirstSubTask y ControlSubTask (vtable 0x86EF80, huecos 1, 6, 8, 9 y 10).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 3 peatones (salud, estado, tipo y vehiculo variados), 2 vehiculos (vtable falsa con trampas, handling propio con el
// byte de grupo de animacion 0xDE y conductor 0x460), 3 tareas falsas (GetTaskType, MakeAbortable y destructor trazados).
// Se simulan y trazan: RNG, FindPlayerPed, CPed::SetRadioStation, CAEVehicleAudioEntity::PlayerAboutToExitVehicleAsDriver,
// CAEPedAudioEntity::AddAudioEvent y CEventGroup::Add (el evento de muerte de la pila se serializa por campos).
// Las subtareas que crea CreateSubTask (CTaskComplexLeaveCar, CTaskSimpleCarSetPedOut, CTaskComplexGetUpAndStandStill,
// CTaskComplexCarSlowBeDraggedOut) salen del pool real con el constructor que toque (EXE o DLL) y se serializan enteras.
// Pool de tareas: m_LastFreeSlot y estados restaurados y slots libres con 0xCD en cada ejecucion; lo nuevo se libera
// sin destructor. Se guarda y restaura el reloj. Constructores sobre buffer con 0xCD comparado entero (tipo en vez de vptr).
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Tasks/Task.h"
#include "Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.h"
#include "Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCar.h"
#include "Tasks/TaskTypes/TaskComplexGetUpAndStandStill.h"
#include "Tasks/TaskTypes/TaskSimpleCarSetPedOut.h"
#include "Audio/Entities/AEPedAudioEntity.h"
#include "Audio/Entities/AEVehicleAudioEntity.h"
#include "Events/EventGroup.h"
#include "Events/EventDeath.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace DragOut60Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }
template<class M> uintptr_t MemberAddress(M m){ static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }
inline uintptr_t Body(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

constexpr int NPEDS=3, NVEH=2, FAKES=3;
// tareas falsas: campos de control por encima de 0x20
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_TYPE=0x68, FK_CNT=0x70;
// desplazamientos crudos del EXE
constexpr size_t E_TYPE=0x36;                                                                                    // CEntity
constexpr size_t P_INTEL=0x47C, P_STATE=0x530, P_HEALTH=0x540, P_VEH=0x58C, P_PEDTYPE=0x598, P_AUDIO=0x138;      // CPed
constexpr size_t V_AUDIO=0x138, V_HANDLING=0x384, V_DRIVER=0x460;                                                // CVehicle
constexpr size_t H_ANIMGROUP=0xDE, H_SIZE=0xE0;                                                                  // tHandlingData
constexpr size_t EV_TYPE=0x30;
constexpr uintptr_t TIMER=0xB7CB84, VT_CLASS=0x86EF80;
constexpr size_t OBJ_SIZE=0x14;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,H_SIZE>,NVEH> handlings{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<uint8,0x100> object{};
alignas(16) inline std::array<uint8,0x40> event{};
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
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(vehVtbl.data()))return "veh_vt"; if(v==uintptr_t(eventVtbl.data()))return "event_vt";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n);
                              if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(handlings[n].data(),handlings[n].size(),"hnd"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(event.data(),event.size(),"event"))return *s;
    if(SlotIndex(TaskHdr(),TASK_SLOT,v)>=0)return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    // locales de la pila (evento de muerte): no comparables
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
// evento de muerte (de la pila): tipo por la vtable y solo los campos (sin relleno)
inline json SerDeath(CEvent* e) {
    if(!e)return nullptr;
    return json::array({e->GetEventType(),At<int32>(e,4),At<uint8>(e,8),At<uint8>(e,0xC),At<uint32>(e,0x10)});
}

// buffers de fixtures cuyos cambios se comparan (palabra a palabra, traducidas)
struct Buf { std::string name; uint8* p; size_t n; };
inline std::vector<Buf> Buffers() {
    std::vector<Buf> b;
    for(int n=0;n<NPEDS;++n){ b.push_back({"ped#"+std::to_string(n),peds[n].data(),peds[n].size()}); b.push_back({"intel#"+std::to_string(n),intels[n].data(),intels[n].size()}); }
    for(int n=0;n<NVEH;++n){ b.push_back({"veh#"+std::to_string(n),vehs[n].data(),vehs[n].size()}); b.push_back({"hnd#"+std::to_string(n),handlings[n].data(),handlings[n].size()}); }
    for(int n=0;n<FAKES;++n)b.push_back({"fk#"+std::to_string(n),fakes[n].data(),fakes[n].size()});
    b.push_back({"event",event.data(),event.size()});
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
inline uintptr_t playerRet{};

// tareas falsas: GetTaskType trazado; MakeAbortable segun una mascara por llamada (contador en el propio buffer)
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline int __fastcall FType(CTask* t,void*) { trace.push_back({"type",Name(t)}); return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) {
    const uint32 k=At<uint32>(t,FK_CNT)++; const bool r=(At<uint32>(t,FK_ABORT)>>(k&31))&1;
    trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e),r}); return r;
}
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<size_t S,int... N> void FillTraps(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }

// dependencias simuladas
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { trace.push_back({"find_player_ped",id,Name(playerRet)}); return reinterpret_cast<CPlayerPed*>(playerRet); }
inline void __fastcall FakeSetRadio(CPed* p,void*) { trace.push_back({"set_radio_station",Name(p)}); }
inline void __fastcall FakeAboutToExit(void* a,void*) { trace.push_back({"player_about_to_exit",Name(a)}); }
inline void __fastcall FakeAddAudio(void* a,void*,int32 ev,float vol,float spd,void* ped,int32 surf,int32 a7,uint32 maxVol) {
    trace.push_back({"add_audio_event",Name(a),ev,F(vol),F(spd),Name(ped),surf,a7,maxVol});
}
inline CEvent* __fastcall FakeEventAdd(void* g,void*,CEvent* e,uint32 valid) { trace.push_back({"event_group_add",Name(g),SerDeath(e),valid&0xFF}); return e; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=8;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexCarSlowBeDraggedOutAndStandUp","Clone",0,0x86EF80,1,256},
    {"CTaskComplexCarSlowBeDraggedOutAndStandUp","Constructor",0x648620,0,-1,256},
    {"CTaskComplexCarSlowBeDraggedOutAndStandUp","ControlSubTask",0,0x86EF80,10,256},
    {"CTaskComplexCarSlowBeDraggedOutAndStandUp","CreateFirstSubTask",0,0x86EF80,9,1024},
    {"CTaskComplexCarSlowBeDraggedOutAndStandUp","CreateNextSubTask",0,0x86EF80,8,1024},
    {"CTaskComplexCarSlowBeDraggedOutAndStandUp","CreateSubTask",0x648710,0,-1,1024},
    {"CTaskComplexCarSlowBeDraggedOutAndStandUp","Destructor",0x648690,0,-1,256},
    {"CTaskComplexCarSlowBeDraggedOutAndStandUp","MakeAbortable",0,0x86EF80,6,512},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dragout60-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dragout60-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dragout60");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"dragout60-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(fakeVtbl,std::make_integer_sequence<int,16>{}); FillTraps(vehVtbl,std::make_integer_sequence<int,96>{}); FillTraps(eventVtbl,std::make_integer_sequence<int,20>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    const auto occTaskStart=Occ(TaskHdr());
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    const int32 lt0=Last(TaskHdr());
    const std::vector<uint8> stTask(States(TaskHdr()),States(TaskHdr())+Cap(TaskHdr()));
    auto resetPools=[&]{ ResetPool(TaskHdr(),TASK_SLOT,lt0,stTask,occTaskStart); };
    // estado global tocado: solo el reloj; se restaura en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(Body(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber)),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch p1(Body(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed))),FakeFindPlayerPed), p1e(0x56E210,FakeFindPlayerPed);
        EntryPatch s1(Body(MemberAddress(&CPed::SetRadioStation)),FakeSetRadio), s1e(0x5DFD90,FakeSetRadio);
        EntryPatch a1(Body(MemberAddress(&CAEVehicleAudioEntity::PlayerAboutToExitVehicleAsDriver)),FakeAboutToExit), a1e(0x4F5BA0,FakeAboutToExit);
        EntryPatch a2(Body(MemberAddress(&CAEPedAudioEntity::AddAudioEvent)),FakeAddAudio), a2e(0x4E2BB0,FakeAddAudio);
        EntryPatch q1(Body(MemberAddress(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add))),FakeEventAdd), q1e(0x4AB420,FakeEventAdd);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                const float qnan=std::numeric_limits<float>::quiet_NaN();
                auto run=[&](bool candidate)->json {
                    resetPools();
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x6060u; if(!st)st=1; trace=json::array(); seed=uint32(i);
                    rngCalls=0; freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    // grupos de animacion: coach (0xF), tanque (0x18), bus (0x10), los de la tabla (0, 1, 13, 14, 17, 18, 19, 22, 23, 28) y otros
                    auto pickGroup=[&]()->uint8{ static const uint8 g[]{0xF,0x18,0x10,0,1,13,14,17,18,19,22,23,28,2,3,12,16,20,24,29,40,0xFF}; return next()%4==0 ? uint8(next()) : g[next()%std::size(g)]; };
                    // vehiculos: vtable con trampas, handling propio, conductor
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%8)<<3);
                        for(auto& b:handlings[n])b=uint8(next()); handlings[n][H_ANIMGROUP]=pickGroup();
                        At<void*>(v,V_HANDLING)=handlings[n].data();
                        { const uint32 m=next()%3; At<CPed*>(v,V_DRIVER)=m==0 ? nullptr : Ped(int(next()%NPEDS)); }
                    }
                    // peatones: inteligencia, tipo, estado, salud, vehiculo
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                        p[E_TYPE]=3; At<void*>(p,P_INTEL)=intels[n].data();
                        { static const int32 types[]{0,1,2,4,7,8,9,10,6}; At<int32>(p,P_PEDTYPE)=types[next()%std::size(types)]; }
                        { static const int32 states[]{0,1,50,54,55,56,0x31}; At<int32>(p,P_STATE)=next()%4==0 ? int32(next()%60) : states[next()%std::size(states)]; }
                        { const uint32 m=next()%7; At<float>(p,P_HEALTH)=m==0 ? 0.f : m==1 ? -5.f : m==2 ? qnan : m==3 ? 1e-30f : float(1+next()%100); }
                        { const uint32 m=next()%6; At<CVehicle*>(p,P_VEH)=m==0 ? nullptr : Veh(int(next()%NVEH)); }
                    }
                    // CreateFirstSubTask: el EXE desreferencia el vehiculo del peaton sin guarda: siempre hay vehiculo
                    if(nm=="CreateFirstSubTask")At<CVehicle*>(peds[0].data(),P_VEH)=Veh(int(next()%NVEH));
                    // jugador devuelto por FindPlayerPed: el peaton 0, otro, o nadie
                    { const uint32 m=next()%5; playerRet=m<2 ? uintptr_t(Ped(0)) : m<4 ? uintptr_t(Ped(1)) : 0; }
                    // tareas falsas
                    for(int n=0;n<FAKES;++n){ auto* k=fakes[n].data(); for(size_t b=0;b<FAKE_SIZE;++b)k[b]=uint8(next());
                        At<void*>(k,0)=fakeVtbl.data(); At<uint32>(k,4)=0; At<uint32>(k,8)=0;
                        At<int32>(k,FK_TYPE)=TASK_SIMPLE_PAUSE; At<uint32>(k,FK_ABORT)=next(); At<uint32>(k,FK_CNT)=0; }
                    for(auto& b:event)b=uint8(next()); At<void*>(event.data(),0)=eventVtbl.data();
                    std::memset(object.data(),0xCD,object.size());

                    auto* o=object.data(); auto* p0=Ped(0);
                    auto setObj=[&](){ for(size_t b=0;b<OBJ_SIZE;++b)o[b]=uint8(next()); At<uintptr_t>(o,0)=VT_CLASS; At<uint32>(o,4)=0; At<uint32>(o,8)=0; };
                    auto occT=Occ(TaskHdr());

                    // argumentos preparados antes de la instantanea
                    int32 a0{}, a1{}; CVehicle* qv{}; const CEvent* ev{};
                    static const int32 d10[]{8,10,8,10,0,1,5,0x7FFFFFFF,-1};
                    if(nm=="Constructor"){ a0=0; qv=next()%5==0 ? nullptr : Veh(int(next()%NVEH)); a1=d10[next()%std::size(d10)]; }
                    else {
                        setObj();
                        { const uint32 m=next()%6; At<CVehicle*>(o,0xC)=m==0 ? nullptr : Veh(int(next()%NVEH)); }
                        At<int32>(o,0x10)=d10[next()%std::size(d10)];
                        At<CTask*>(o,8)=next()%8==0 ? nullptr : Fake(0);
                        // CreateFirstSubTask puede llegar a CreateSubTask(LEAVE_CAR) (0x648AA5), que lee [m_Vehicle+0x384] sin guarda de nulo (0x64875B):
                        // el vehiculo de la tarea nunca es nulo. Su handling nunca es nulo (siempre se asigna arriba)
                        if(nm=="CreateFirstSubTask"&&!At<CVehicle*>(o,0xC))At<CVehicle*>(o,0xC)=Veh(int(next()%NVEH));
                        if(nm=="CreateSubTask"){
                            static const int32 ty[]{TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_GET_UP_AND_STAND_STILL,
                                                    TASK_SIMPLE_CAR_SET_PED_OUT,TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT,TASK_FINISHED,TASK_NONE,1000,TASK_SIMPLE_PAUSE};
                            a0=ty[next()%std::size(ty)];
                            // el EXE desreferencia m_Vehicle (this+0xC) al buscar el grupo de animacion de LEAVE_CAR
                            if(a0==TASK_COMPLEX_LEAVE_CAR&&!At<CVehicle*>(o,0xC))At<CVehicle*>(o,0xC)=Veh(int(next()%NVEH));
                        }
                        if(nm=="CreateNextSubTask"){
                            static const int32 ty[]{TASK_SIMPLE_CAR_SET_PED_OUT,TASK_COMPLEX_GET_UP_AND_STAND_STILL,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT,
                                                    TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT,TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT,TASK_SIMPLE_PAUSE,TASK_FINISHED,TASK_NONE};
                            At<CTask*>(o,8)=Fake(0); At<int32>(fakes[0].data(),FK_TYPE)=ty[next()%std::size(ty)];
                        }
                        if(nm=="MakeAbortable"){ At<CTask*>(o,8)=Fake(0); a0=int32(next()%4); ev=next()%2 ? reinterpret_cast<const CEvent*>(event.data()) : nullptr; }
                    }
                    const auto before=Snapshot();

                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches}}; json ret=nullptr;
                    // retornos puntero: se traducen despues de nombrar las tareas nuevas
                    uintptr_t retPtr{}; bool isPtr=false; auto Ptr=[&](const void* p)->json{ retPtr=uintptr_t(p); isPtr=true; return nullptr; };
                    auto callTask=[&](CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    if(nm=="Clone")ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                    else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,p0,a0,ev);
                    else if(nm=="ControlSubTask"||nm=="CreateFirstSubTask"||nm=="CreateNextSubTask")ret=Ptr(callTask(p0));
                    else if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32)>(target)(o,qv,a1));
                    else if(nm=="Destructor")reinterpret_cast<void(__thiscall*)(void*)>(target)(o);
                    else if(nm=="CreateSubTask")ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,a0,p0));
                    for(auto* h:hooks)h->State(false);

                    // tareas nuevas (se nombran antes de serializar el objeto)
                    { auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                      for(size_t k=0;k<occT.size();++k)if(!occT[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                      json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t)); out["tasks"]=tasks; }
                    if(isPtr)ret=Name(retPtr);
                    out["object"]=Region(o,OBJ_SIZE,true);
                    if(nm=="Constructor")out["type"]=reinterpret_cast<CTask*>(o)->GetTaskType();
                    out["changes"]=Changes(before); out["refs"]=AllRefs();
                    out["ret"]=ret; out["trace"]=trace; out["rng"]=rngCalls;
                    // limpieza: referencias de los fixtures y slots nuevos (sin destructor); el pool debe quedar como antes
                    CleanRefs();
                    FreeFresh(TaskHdr(),occT);
                    out["clean"]=Occ(TaskHdr())==occT;
                    restoreGlobals();
                    return out;
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
