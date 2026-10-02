#pragma once
// Banco dentro del motor: eventos y tareas sueltos (lista de 56, 23 funciones de esta tanda).
// Constructores de eventos (AcquaintancePedHate, DontJoinPlayerGroup, LeaderEntryExit, NewGangMember,
// PedCollisionWithPlayer, PlayerCollisionWithPed, PlayerCommandToGroupGather, ScriptedAttractor, SeenCop),
// AffectsPed de CEventEscalator y CEventOnFire, constructores de tareas (FollowPedFootsteps, KillPedOnFoot,
// Partner, PartnerChat, PartnerDeal, PartnerShove, WalkRoundObject, Uninterruptable, CScriptedBrainTaskEntry),
// CTaskSimpleAnim::MakeAbortable y ProcessPed de CTaskSimpleDrown/CTaskSimpleDrownInCar.
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos: los constructores se llaman en
// la direccion del EXE con `this` = buffer propio rellenado con 0xCD (se compara el tipo en vez del vptr); los
// metodos virtuales se llaman por la entrada de la vtable del EXE. Peatones, entidad, inteligencia, vehiculo,
// animacion y tareas/evento falsos (vtable trazada) en buffers propios. Se simulan y trazan: CPed::SetPedState,
// FindPlayerPed, CStats::IncrementStat, CTaskSimpleDie::ProcessPed, CEventGroup::Add,
// CAnimBlendAssociation::SetFinishCallback y CAnimManager::BlendAnimation. El pool de CPointRoute (cabecera,
// estados y almacenamiento), el reloj y los MI_ESCALATORSTEP se guardan y se restauran en cada ejecucion.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Events/EventAcquaintancePedHate.h"
#include "Events/EventDontJoinPlayerGroup.h"
#include "Events/EventLeaderEntryExit.h"
#include "Events/EventNewGangMember.h"
#include "Events/EventPedCollisionWithPlayer.h"
#include "Events/EventPlayerCollisionWithPed.h"
#include "Events/EventPlayerCommandToGroupGather.h"
#include "Events/EventScriptedAttractor.h"
#include "Events/EventSeenCop.h"
#include "Events/EventEscalator.h"
#include "Events/EventOnFire.h"
#include "Events/EventScriptCommand.h"
#include "Events/EventGroup.h"
#include "Tasks/TaskTypes/TaskComplexFollowPedFootsteps.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFoot.h"
#include "Tasks/TaskTypes/TaskComplexPartner.h"
#include "Tasks/TaskTypes/TaskComplexPartnerChat.h"
#include "Tasks/TaskTypes/TaskComplexPartnerDeal.h"
#include "Tasks/TaskTypes/TaskComplexPartnerShove.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundObject.h"
#include "Tasks/TaskTypes/TaskSimpleUninterruptable.h"
#include "Tasks/TaskTypes/TaskSimpleAnim.h"
#include "Tasks/TaskTypes/TaskSimpleDie.h"
#include "Tasks/TaskTypes/TaskSimpleDrown.h"
#include "Tasks/TaskTypes/TaskSimpleDrownInCar.h"
#include "Tasks/ScriptedBrainTaskStore.h"
#include "PedIntelligence.h"
#include "PedGeometryAnalyser.h"
#include "Pools/Pools.h"
#include "Pools/PointRoutePool.h"
#include "Pools/TaskPool.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Stats.h"
#include "ModelIndices.h"
#include "../ProbeRuntime.hpp"

namespace EvTask56Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
constexpr int FAKES=8;                 // 0-4 tareas primarias, 5 tarea secundaria, 6 tarea del comando de script, 7 libre
constexpr size_t FK_TYPE=0x38;         // tipo devuelto por GetTaskType, por objeto
// desplazamientos crudos del EXE (miembros protegidos/privados o para escribir sin depender de la DLL)
constexpr size_t P_CLUMP=0x18, P_PHYS_FLAGS=0x40, P_PED_FLAGS2=0x46C, P_INTEL=0x47C, P_ANIM_GROUP=0x4D4, P_STATE=0x530,
                 P_CONTACT=0x584, P_VEHICLE=0x58C, P_TYPE=0x598, P_FIRE=0x730;
constexpr size_t I_SECONDARY=0x18;    // CPedIntelligence: m_TaskMgr en +4 (primarias en +4, secundarias en +0x18); grupo de eventos en +0x68
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84, MI_STEP=0x8CD674, MI_STEP8=0x8CD678, EXE_DEFAULT_ANIM_CB=0x4D1390;
alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, pedB{};
alignas(16) inline std::array<uint8,0x200> ent{};                    // entidad (contacto / atractor / objeto a rodear)
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel{};
alignas(16) inline std::array<uint8,0x80> veh{}, fire{}, clump{}, effect{};
alignas(16) inline std::array<uint8,sizeof(CAnimBlendAssociation)> anim{};
alignas(16) inline std::array<uint8,0x80> object{};                  // objeto probado
alignas(16) inline std::array<std::array<uint8,0x40>,FAKES> fakes{};
alignas(16) inline std::array<uint8,0x40> event{};                   // evento falso (MakeAbortable)
alignas(16) inline std::array<char,40> nameBuf{};
alignas(16) inline std::array<uint8,sizeof(CTaskSimpleDie)> dieProbe{};
inline CPed* PedA() { return reinterpret_cast<CPed*>(pedA.data()); }
inline CPed* PedB() { return reinterpret_cast<CPed*>(pedB.data()); }
inline CEntity* Ent() { return reinterpret_cast<CEntity*>(ent.data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// pool de CPointRoute (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot, banderas)
inline uint8* RouteHdr() { return reinterpret_cast<uint8*>(GetPointRoutePool()); }
inline uint8* RouteStorage() { return *reinterpret_cast<uint8**>(RouteHdr()); }
inline uint8* RouteStates() { return *reinterpret_cast<uint8**>(RouteHdr()+4); }
inline size_t RouteCap() { return *reinterpret_cast<uint32*>(RouteHdr()+8); }
constexpr size_t ROUTE_SZ=sizeof(CPointRoute);

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==EXE_DEFAULT_ANIM_CB||v==std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB))return "DefaultAnimCB";
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(pedA.data(),pedA.size(),"pedA"))return *s; if(auto s=in(pedB.data(),pedB.size(),"pedB"))return *s;
    if(auto s=in(ent.data(),ent.size(),"ent"))return *s; if(auto s=in(intel.data(),intel.size(),"intel"))return *s;
    if(auto s=in(veh.data(),veh.size(),"veh"))return *s; if(auto s=in(fire.data(),fire.size(),"fire"))return *s;
    if(auto s=in(clump.data(),clump.size(),"clump"))return *s; if(auto s=in(effect.data(),effect.size(),"effect"))return *s;
    if(auto s=in(anim.data(),anim.size(),"anim"))return *s; if(auto s=in(object.data(),object.size(),"object"))return *s;
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    if(auto s=in(event.data(),event.size(),"event"))return *s; if(auto s=in(nameBuf.data(),nameBuf.size(),"name"))return *s;
    { const auto rs=uintptr_t(RouteStorage()); if(v>=rs&&v<rs+RouteCap()*ROUTE_SZ)return "route+"+std::to_string((v-rs)%ROUTE_SZ); }
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
inline int eventType{}; inline uintptr_t playerRet{}; inline bool dieRet{};

// tareas falsas: GetTaskType devuelve el tipo guardado en cada objeto
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(uintptr_t(t)),f}); return t; }
inline int __fastcall FType(CTask* t,void*) { trace.push_back({"get_task_type",Name(uintptr_t(t))}); return *reinterpret_cast<int32*>(uintptr_t(t)+FK_TYPE); }
template<int N> void* __fastcall FTrap(CTask*,void*) { trace.push_back({"unexpected_slot",N}); return nullptr; }
inline std::array<void*,12> fakeVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// evento falso (GetEventType en el hueco 1)
inline int __fastcall EType(void* e,void*) { trace.push_back({"event_get_type",Name(uintptr_t(e))}); return eventType; }
template<int N> void* __fastcall ETrap(void*,void*) { trace.push_back({"event_unexpected_slot",N}); return nullptr; }
inline std::array<void*,16> eventVtbl{};
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }

// dependencias simuladas
inline void __fastcall SetPedState(CPed* p,void*,int32 s) { trace.push_back({"set_ped_state",Name(uintptr_t(p)),s}); }
inline CPlayerPed* __cdecl FindPlayer(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(playerRet); }
inline void __cdecl IncStat(uint16 s,float v) { trace.push_back({"increment_stat",s,F(v)}); }
inline bool __fastcall DieProcess(CTask* t,void*,CPed* p) { trace.push_back({"die_process_ped",Name(uintptr_t(t)),Name(uintptr_t(p))}); return dieRet; }
inline CEvent* __fastcall GroupAdd(void* g,void*,CEvent* e,bool valid) {
    auto* b=reinterpret_cast<uint8*>(e); // bytes 9-0xB del evento en pila sin inicializar: no se trazan
    trace.push_back({"event_group_add",Name(uintptr_t(g)),e->GetEventType(),At<int32>(b,4),b[8],b[0xC],At(b,0x10),valid});
    return nullptr;
}
inline void __fastcall SetFinishCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { trace.push_back({"set_finish_callback",Name(uintptr_t(a)),Name(uintptr_t(cb)),Name(uintptr_t(data))}); }
inline CAnimBlendAssociation* __cdecl Blend(RpClump* c,int32 group,int32 id,float delta) { trace.push_back({"blend_animation",Name(uintptr_t(c)),group,id,F(delta)}); return nullptr; }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
enum Kind { EV_PED1, EV_COLL, EV_ATTR, EV_ESC, EV_FIRE, TK_FOLLOW, TK_KILL, TK_PARTNER, TK_CHAT, TK_DEAL, TK_SHOVE, TK_WALK, TK_UNINT, TK_ENTRY, TK_ANIM, TK_DROWN, TK_DROWNCAR };
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; Kind kind; };
constexpr size_t NFNS=23;
inline const std::array<Fn,NFNS> fns{{
    {"CEventAcquaintancePedHate","Constructor2",0x420E70,0x858E68,-1,64,EV_PED1},
    {"CEventDontJoinPlayerGroup","Constructor",0x6090E0,0x86D0E0,-1,64,EV_PED1},
    {"CEventLeaderEntryExit","Constructor",0x43E1C0,0x859540,-1,64,EV_PED1},
    {"CEventNewGangMember","Constructor",0x608F70,0x86D0A0,-1,64,EV_PED1},
    {"CEventPedCollisionWithPlayer","Constructor",0x5FED40,0x86C970,-1,128,EV_COLL},
    {"CEventPlayerCollisionWithPed","Constructor",0x5FEE40,0x86C9B0,-1,128,EV_COLL},
    {"CEventPlayerCommandToGroupGather","Constructor",0x609250,0x86D120,-1,64,EV_PED1},
    {"CEventScriptedAttractor","Constructor",0x5FEF40,0x86C9F0,-1,128,EV_ATTR},
    {"CEventSeenCop","Constructor1",0x5FF380,0x86CB58,-1,64,EV_PED1},
    {"CEventEscalator","AffectsPed",0,0x86CD00,5,512,EV_ESC},
    {"CEventOnFire","AffectsPed",0,0x86CCC0,5,512,EV_FIRE},
    {"CTaskComplexFollowPedFootsteps","Constructor",0x694E20,0x870CC0,-1,128,TK_FOLLOW},
    {"CTaskComplexKillPedOnFoot","Constructor",0x620E30,0x86D894,-1,128,TK_KILL},
    {"CTaskComplexPartner","Constructor",0x681E70,0x870664,-1,128,TK_PARTNER},
    {"CTaskComplexPartnerChat","Constructor",0x684290,0x8707C4,-1,128,TK_CHAT},
    {"CTaskComplexPartnerDeal","Constructor",0x684190,0x870754,-1,128,TK_DEAL},
    {"CTaskComplexPartnerShove","Constructor",0x6846F0,0x870800,-1,128,TK_SHOVE},
    {"CTaskComplexWalkRoundObject","Constructor",0x655020,0x86F364,-1,128,TK_WALK},
    {"CTaskSimpleUninterruptable","Constructor",0x43E2E0,0x859580,-1,64,TK_UNINT},
    {"CTaskSimpleAnim","MakeAbortable",0,0x86D504,6,1024,TK_ANIM},
    {"CTaskSimpleDrown","ProcessPed",0,0x86DE30,7,256,TK_DROWN},
    {"CTaskSimpleDrownInCar","ProcessPed",0,0x86DE54,7,256,TK_DROWNCAR},
    {"CScriptedBrainTaskEntry","Constructor",0x62EC40,0,-1,64,TK_ENTRY},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(PedA()); clean(PedB()); clean(Ent());
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"evtask56-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"evtask56-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("evtask56");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"evtask56-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(std::make_integer_sequence<int,12>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[4]=reinterpret_cast<void*>(&FType);
    FillETraps(std::make_integer_sequence<int,16>{}); eventVtbl[1]=reinterpret_cast<void*>(&EType);
    // CTaskSimpleDie::ProcessPed de la DLL: la llamada calificada de CTaskSimpleDrown::ProcessPed va a la misma entrada
    // que el hueco 7 de la vtable propia de la DLL (se construye una tarea en un buffer para leerla)
    const uintptr_t dllDieProcess=(*reinterpret_cast<uintptr_t**>(::new (dieProbe.data()) CTaskSimpleDie(ANIM_GROUP_DEFAULT,ANIM_ID_DROWN,4.f,0.f)))[7];
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado por las funciones: se guarda y se restaura en cada ejecucion
    const std::vector<uint8> savedRouteHdr(RouteHdr(),RouteHdr()+0x14), savedRouteStates(RouteStates(),RouteStates()+RouteCap()),
                             savedRouteStorage(RouteStorage(),RouteStorage()+RouteCap()*ROUTE_SZ);
    auto restoreRoutes=[&]{ std::memcpy(RouteStates(),savedRouteStates.data(),savedRouteStates.size()); std::memcpy(RouteStorage(),savedRouteStorage.data(),savedRouteStorage.size());
                            std::memcpy(RouteHdr(),savedRouteHdr.data(),savedRouteHdr.size()); };
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    auto& miStep=*reinterpret_cast<uint16*>(MI_STEP); auto& miStep8=*reinterpret_cast<uint16*>(MI_STEP8); const uint16 savedMi=miStep, savedMi8=miStep8;
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch s1(bit_cast<uintptr_t>(&CPed::SetPedState),SetPedState), s1e(0x5E4500,SetPedState);
        EntryPatch s2(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FindPlayer), s2e(0x56E210,FindPlayer);
        EntryPatch s3(bit_cast<uintptr_t>(static_cast<void(*)(eStats,float)>(&CStats::IncrementStat)),IncStat), s3e(0x55C180,IncStat);
        EntryPatch s4(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),GroupAdd), s4e(0x4AB420,GroupAdd);
        EntryPatch s5(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetFinishCallback),SetFinishCb), s5e(0x4CEBE0,SetFinishCb);
        EntryPatch s6(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),Blend), s6e(0x4D4610,Blend);
        EntryPatch s7(0x6397E0,DieProcess);
        std::optional<EntryPatch> s7d; if(dllDieProcess!=0x6397E0)s7d.emplace(dllDieProcess,DieProcess);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto vec=[&]{return CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(-10.f,40.f)};};
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot
                    { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0; }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5656u; if(!st)st=1; trace=json::array();
                    CleanRefs(); restoreRoutes();
                    // pool de rutas: los huecos libres se rellenan con 0xCD (como CPool::New en depuracion)
                    { auto* states=RouteStates(); for(size_t k=0;k<RouteCap();++k)if(states[k]&0x80)std::memset(RouteStorage()+k*ROUTE_SZ,0xCD,ROUTE_SZ); }
                    const std::vector<uint8> preStates(RouteStates(),RouteStates()+RouteCap());
                    // peatones y entidad
                    std::memset(pedA.data(),0,pedA.size()); std::memset(pedB.data(),0,pedB.size()); std::memset(ent.data(),0,ent.size());
                    PedA()->SetType(ENTITY_TYPE_PED); PedB()->SetType(ENTITY_TYPE_PED); Ent()->SetType(ENTITY_TYPE_OBJECT);
                    for(auto& b:intel)b=0; for(auto& b:veh)b=uint8(next()); for(auto& b:fire)b=uint8(next()); for(auto& b:clump)b=uint8(next()); clump[0]=2; // rpCLUMP (GetRpClump lo comprueba en la DLL)
                    for(auto& b:effect)b=uint8(next()); for(auto& b:anim)b=uint8(next());
                    { const int len=int(next()%32); for(int k=0;k<int(nameBuf.size());++k)nameBuf[k]=char(k<len ? 'a'+next()%26 : k==len ? 0 : next()); }
                    timeMs=next();
                    // tareas falsas y evento falso
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,fakes[n].size()); *reinterpret_cast<void**>(fakes[n].data())=fakeVtbl.data();
                        static const int types[]{TASK_COMPLEX_ON_FIRE,TASK_SIMPLE_PLAYER_ON_FIRE,TASK_SIMPLE_NAMED_ANIM,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_PAUSE};
                        At<int32>(fakes[n].data(),FK_TYPE)=types[next()%5]; }
                    for(auto& b:event)b=uint8(next()); *reinterpret_cast<void**>(event.data())=eventVtbl.data();
                    At<CTask*>(event.data(),0x10)=next()%4 ? Fake(6) : nullptr;               // CEventScriptCommand::m_task
                    eventType=next()%3 ? EVENT_SCRIPT_COMMAND : (next()%2 ? EVENT_DAMAGE : EVENT_ON_FIRE);
                    playerRet=next()%3==0 ? 0 : next()%2 ? uintptr_t(PedA()) : uintptr_t(PedB()); dieRet=next()%2;
                    std::memset(object.data(),0xCD,object.size());
                    void* const o=object.data(); CPed* pedArg=next()%5 ? PedA() : (next()%2 ? PedB() : nullptr);
                    // fixtures propios de cada tipo de funcion
                    bool argB1=next()%2, argB2=next()%2, argB3=next()%2; const float argF=next()%6 ? uni(-5.f,30.f) : 0.f;
                    const int32 argI1=int32(next()), argI2=next()%3 ? int32(next()%20000) : -1, argI3=int32(next()%100), argI4=int32(next()%100);
                    const int8 argC=int8(next()); const int16 piece=int16(next()); const int32 ms1=int32(next()%8), ms2=next()%6 ? int32(next()%8) : int32(next()%0x20000);
                    const CVector v1=vec(), v2=vec(); CEntity* argEnt=next()%5==0 ? nullptr : next()%2 ? Ent() : static_cast<CEntity*>(PedB());
                    void* argEffect=next()%5 ? effect.data() : nullptr; const int prio=int(next()%3);
                    const void* ev=next()%5 ? event.data() : nullptr;
                    auto* p=PedA();
                    switch(fn.kind){
                    case EV_ESC: {
                        static const uint32 states[]{0x36,0x37,1,0,0x29,7}; At(p,P_STATE)=states[next()%6]; At(p,P_TYPE)=next()%3 ? next()%8 : 0;
                        static const uint16 mis[]{1424,1425,0xFFFF,0x8001,0x8002,0x0010};
                        miStep=mis[next()%6]; miStep8=next()%3 ? uint16(miStep+1) : mis[next()%6];
                        const uint32 m=next()%6; At<uint16>(ent.data(),0x22)=m==0 ? miStep : m==1 ? miStep8 : m==2 ? 0xFFFF : m==3 ? uint16(0x8000|next()) : uint16(next()%20000);
                        At<CEntity*>(p,P_CONTACT)=next()%5 ? Ent() : nullptr;
                        break; }
                    case EV_FIRE: {
                        static const uint32 states[]{0x36,0x37,1,0}; At(p,P_STATE)=states[next()%4];
                        At<void*>(p,P_FIRE)=next()%5 ? fire.data() : nullptr; At(p,P_PHYS_FLAGS)=next()|(next()%3 ? 0u : 0x80000u); if(next()%2)At(p,P_PHYS_FLAGS)&=~0x80000u;
                        At<void*>(p,P_INTEL)=intel.data();
                        const int first=int(next()%6); for(int k=0;k<5;++k)At<CTask*>(intel.data(),4+4*k)=k>=first&&next()%2 ? Fake(k) : nullptr;
                        if(first<5)At<CTask*>(intel.data(),4+4*first)=Fake(first);
                        At<CTask*>(intel.data(),I_SECONDARY+4*4)=next()%3 ? Fake(5) : nullptr;
                        break; }
                    case TK_ANIM: {
                        At<uintptr_t>(o,0)=fn.vt; At<void*>(o,4)=nullptr; At<void*>(o,8)=next()%5 ? anim.data() : nullptr; At<uint8>(o,0xC)=uint8(next());
                        { static const float ds[]{-4.f,1.f,0.f,-1000.f,8.f}; At<float>(anim.data(),0x1C)=ds[next()%5]; }
                        At<void*>(p,P_CLUMP)=clump.data(); At<int32>(p,P_ANIM_GROUP)=int32(next()%118);
                        break; }
                    case TK_DROWN:
                        At<uintptr_t>(o,0)=fn.vt; At<void*>(o,4)=nullptr; for(size_t k=8;k<sizeof(CTaskSimpleDrown);++k)At<uint8>(o,k)=uint8(next());
                        At<uint8>(o,0x28)=next()%3 ? uint8(next()%2) : uint8(next());
                        At(p,P_PED_FLAGS2)=next(); At(p,P_STATE)=next()%60;
                        break;
                    case TK_DROWNCAR:
                        At<uintptr_t>(o,0)=fn.vt; At<void*>(o,4)=nullptr;
                        At(p,P_PED_FLAGS2)=next(); At(p,P_STATE)=next()%60; At<void*>(p,P_VEHICLE)=veh.data(); At<void*>(p,P_INTEL)=intel.data();
                        break;
                    default: break;
                    }
                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    // VMT: la entrada de la vtable del EXE cambia con el hook; estatico: la entrada del EXE salta a la DLL
                    const uintptr_t call=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((call>=0x400000&&call<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    uintptr_t rawRet=0; json ret=nullptr;
                    switch(fn.kind){
                    case EV_PED1: case TK_FOLLOW: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CPed*)>(call)(o,pedArg)); break;
                    case EV_COLL: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int16,float,CPed*,const CVector&,const CVector&,int32,int32)>(call)(o,piece,argF,pedArg,v1,v2,ms1,ms2)); break;
                    case EV_ATTR: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,CEntity*)>(call)(o,argEffect,argEnt)); break;
                    case EV_ESC: case EV_FIRE: ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(call)(o,p); break;
                    case TK_KILL: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CPed*,int32,int32,int32,int32,int8)>(call)(o,pedArg,argI2,argI1,argI3,argI4,argC)); break;
                    case TK_PARTNER: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const char*,CPed*,bool,float,bool,int8,CVector)>(call)(o,nameBuf.data(),pedArg,argB1,argF,argB2,argC,v1)); break;
                    case TK_CHAT: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const char*,CPed*,bool,float,int8,bool,bool,CVector)>(call)(o,nameBuf.data(),pedArg,argB1,argF,argC,argB2,argB3,v1)); break;
                    case TK_DEAL: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const char*,CPed*,bool,float,CVector)>(call)(o,nameBuf.data(),pedArg,argB1,argF,v1)); break;
                    case TK_SHOVE: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const char*,CPed*,bool,float,int8,CVector)>(call)(o,nameBuf.data(),pedArg,argB1,argF,argC,v1)); break;
                    case TK_WALK: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,const CVector&,CEntity*)>(call)(o,int32(next()%8),v1,argEnt)); break;
                    case TK_UNINT: case TK_ENTRY: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(call)(o)); break;
                    case TK_ANIM: ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const void*)>(call)(o,p,prio,ev); break;
                    case TK_DROWN: case TK_DROWNCAR: ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(call)(o,p); break;
                    }
                    for(auto* h:hooks)h->State(false);
                    if(ret.is_null())ret=Name(rawRet);
                    const bool isCtor=nm.starts_with("Constructor");
                    json objectWords=Region(o,object.size(),isCtor&&fn.kind!=TK_ENTRY);
                    if(isCtor&&fn.kind<=EV_ATTR)objectWords.push_back(int(reinterpret_cast<CEvent*>(o)->GetEventType()));   // vptr EXE o DLL: se compara el tipo
                    else if(isCtor&&fn.kind!=TK_ENTRY)objectWords.push_back(int(reinterpret_cast<CTask*>(o)->GetTaskType()));
                    // rutas nuevas del pool (contenido completo)
                    json routes=json::array(); { auto* states=RouteStates(); for(size_t k=0;k<RouteCap();++k)if((preStates[k]&0x80)&&!(states[k]&0x80))routes.push_back(Region(RouteStorage()+k*ROUTE_SZ,ROUTE_SZ)); }
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    refs.push_back(refList(PedA())); refs.push_back(refList(PedB())); refs.push_back(refList(Ent()));
                    json fakeWords=json::array(); for(int n=0;n<FAKES;++n)fakeWords.push_back(Region(fakes[n].data(),fakes[n].size(),true));
                    json r={{"ret",ret},{"object",objectWords},{"routes",routes},{"refs",refs},{"pedA",EntHash(pedA)},{"pedB",EntHash(pedB)},{"ent",EntHash(ent)},
                            {"ped_fields",{At(p,P_PED_FLAGS2),At(p,P_STATE),At(p,P_PHYS_FLAGS)}},{"intel",Region(intel.data(),intel.size())},
                            {"veh",ReversingProbe::Bytes(veh.data(),veh.size())},{"anim",Region(anim.data(),anim.size())},{"fakes",fakeWords},
                            {"event",Region(event.data(),event.size(),true)},{"name",Hash(nameBuf.data(),nameBuf.size())},
                            {"time",timeMs},{"mi",{miStep,miStep8}}};
                    r["trace"]=trace; r["clean"]=Occupancy()==occ; r["switches"]=switches;
                    CleanRefs(); restoreRoutes(); miStep=savedMi; miStep8=savedMi8;
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
    CleanRefs(); restoreRoutes();
    timeMs=savedTime; miStep=savedMi; miStep8=savedMi8;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree
        &&std::memcmp(RouteHdr(),savedRouteHdr.data(),savedRouteHdr.size())==0&&timeMs==savedTime}});
}
}
