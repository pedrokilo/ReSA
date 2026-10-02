#pragma once
// Banco dentro del motor: CInformFriendsEventQueue, CInformGroupEvent y CInformGroupEventQueue.
// Cada par ejecuta el original (todos los hooks del lote desactivados: codigo del EXE) y el
// candidato (hooks activados) sobre el mismo estado y compara exactamente: retorno, colas globales
// (punteros traducidos a nombres), lista de referencias de cada peaton, efectos sobre los eventos
// (borrados, clones, escrituras) y la traza de dependencias controladas.
#include <fstream>
#include <map>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "InformFriendsEventQueue.h"
#include "InformGroupEvent.h"
#include "InformGroupEventQueue.h"
#include "PedGroups.h"
#include "Reference.h"
#include "Events/EventEditableResponse.h"
#include "../ProbeRuntime.hpp"

namespace InformLive {
inline bool done{}; inline std::ofstream log;
inline json trace; inline int seed{}; inline int rngCalls{}; inline bool trapped{};
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
inline int Report(int type,char* msg,int*) { if(type!=_CRT_ASSERT)return FALSE; Write({{"event","assertion"},{"message",msg ? msg:""}}); ExitProcess(91); }

// ---- Eventos falsos: vtable propia que registra efectos -------------------------------------
struct FakeEvent { void* vptr; uint8 body[0x14]; }; // disposicion de CEventAcquaintancePed (0x18)
static_assert(sizeof(FakeEvent)==0x18);
constexpr int EVENTS=96;
alignas(8) inline std::array<FakeEvent,EVENTS> events{};
inline std::array<int,EVENTS> types{};
inline int nextEvent{};
inline std::array<void*,24> vtbl{};
inline int EventId(const void* p) { const auto d=reinterpret_cast<const uint8*>(p)-reinterpret_cast<const uint8*>(events.data()); return d>=0&&d<int(sizeof(events))&&d%sizeof(FakeEvent)==0 ? int(d/sizeof(FakeEvent)):-1; }
inline FakeEvent* NewEvent(int type) { auto* e=&events[nextEvent]; std::memset(e,0,sizeof(*e)); e->vptr=vtbl.data(); types[nextEvent++]=type; return e; }

// ---- Fixtures --------------------------------------------------------------------------------
constexpr int PEDS=10;
alignas(CPed) inline std::array<std::array<uint8,sizeof(CPed)>,PEDS> peds{};
alignas(CPedIntelligence) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,PEDS> intel{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
alignas(CInformGroupEvent) inline std::array<uint8,sizeof(CInformGroupEvent)> object{};
inline auto& friends=CInformFriendsEventQueue::ms_informFriendsEvents;
inline auto& groupQueue=CInformGroupEventQueue::ms_informGroupEvents;

inline json Name(const void* v) {
    const auto a=uintptr_t(v);
    if(!a)return nullptr;
    if(const int e=EventId(v);e>=0)return "ev"+std::to_string(e);
    for(int n=0;n<PEDS;++n) {
        if(v==Ped(n))return "ped"+std::to_string(n);
        const auto base=uintptr_t(intel[n].data());
        if(a>=base&&a<base+intel[n].size())return "ped"+std::to_string(n)+".intel+"+std::to_string(a-base);
    }
    for(int g=0;g<8;++g) {
        const auto base=uintptr_t(&CPedGroups::ms_groups[g]);
        if(a==base)return "group"+std::to_string(g);
        if(a>base&&a<base+sizeof(CPedGroup))return "group"+std::to_string(g)+"+"+std::to_string(a-base);
    }
    for(int k=0;k<8;++k) {
        if(v==&friends[k].m_Ped)return "friends["+std::to_string(k)+"]";
        if(v==&groupQueue[k].m_Ped)return "groupq["+std::to_string(k)+"]";
    }
    if(v==object.data())return "object";
    if(v==FindPlayerPed())return "player";
    return "unknown";
}

// ---- Vtable falsa ------------------------------------------------------------------------------
inline void* __fastcall Dtor(FakeEvent* self,void*,unsigned flags) { trace.push_back({"delete",Name(self),flags}); return self; }
inline int __fastcall Type(FakeEvent* self,void*) { const int id=EventId(self); return id>=0 ? types[id]:-1; }
inline FakeEvent* __fastcall Clone(FakeEvent* self,void*) {
    auto* c=NewEvent(Type(self,nullptr)); std::memcpy(c->body,self->body,sizeof(c->body));
    trace.push_back({"clone",Name(self),Name(c)}); return c;
}
template<int N> void* __fastcall Trap(FakeEvent* self,void*) { trapped=true; trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((vtbl[N]=reinterpret_cast<void*>(&Trap<N>)),...); }

// ---- Dependencias controladas ------------------------------------------------------------------
inline std::array<int16,EVENTS> responseTask{};
inline CEvent* __fastcall EventGroupAdd(CEventGroup* g,void*,CEvent* e,bool valid) { trace.push_back({"event_group_add",Name(g),Name(e),valid}); return nullptr; }
inline bool __fastcall GroupAddEvent(CPedGroupIntelligence* gi,void*,CEvent* e) {
    const auto* raw=reinterpret_cast<const uint8*>(e);
    trace.push_back({"group_add_event",Name(gi),e->GetEventType(),Name(*reinterpret_cast<CPed* const*>(raw+0xC)),Name(*reinterpret_cast<CEvent* const*>(raw+0x10))});
    return true;
}
inline void __fastcall ComputeResponse(CEventEditableResponse* e,void*,CPedGroup* g) {
    trace.push_back({"compute_response",Name(e),Name(g)});
    if(const int id=EventId(e);id>=0)*reinterpret_cast<int16*>(reinterpret_cast<uint8*>(e)+0xE)=responseTask[id];
}
inline std::array<int,PEDS> pedsGroup{};
inline CPedGroup* __cdecl GetPedsGroup(const CPed* p) {
    trace.push_back({"get_peds_group",Name(p)});
    for(int n=0;n<PEDS;++n)if(p==Ped(n))return pedsGroup[n]<0 ? nullptr:&CPedGroups::ms_groups[pedsGroup[n]];
    return nullptr;
}
inline int __cdecl FakeRand() { return int((uint32(seed)*2654435761u+uint32(rngCalls++)*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

// ---- Instantanea ------------------------------------------------------------------------------
inline json Refs() {
    json r=json::array();
    for(int n=0;n<PEDS;++n){json l=json::array();for(auto* x=Ped(n)->m_pReferences;x;x=x->m_pNext)l.push_back(Name(x->m_ppEntity));r.push_back(l);}
    return r;
}
inline void ClearRefs() { for(int n=0;n<PEDS;++n)while(auto* x=Ped(n)->m_pReferences)Ped(n)->CleanUpOldReference(x->m_ppEntity); }
inline json Friends() { json r=json::array(); for(auto& e:friends)r.push_back({Name(e.m_Ped),Name(e.m_Event),e.m_Time}); return r; }
inline json GroupQ() { json r=json::array(); for(auto& e:groupQueue)r.push_back({Name(e.m_Ped),Name(e.m_PedGroup),Name(e.m_Event),e.m_Time}); return r; }
inline json Object() { const auto& o=*reinterpret_cast<CInformGroupEvent*>(object.data()); return {Name(o.m_Ped),Name(o.m_PedGroup),Name(o.m_Event),o.m_Time}; }
inline json Events() { json r=json::array(); for(int k=0;k<nextEvent;++k)r.push_back({types[k],ReversingProbe::Bytes(events[k].body,sizeof(events[k].body)),events[k].vptr==vtbl.data()}); return r; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; int cases; };
inline const std::array<Fn,11> fns{{
    {"CInformFriendsEventQueue","Init",0x4B2990,256},{"CInformFriendsEventQueue","Flush",0x4AC2A0,256},
    {"CInformFriendsEventQueue","Add",0x4AC1E0,1024},{"CInformFriendsEventQueue","Process",0x4AC2E0,1024},
    {"CInformGroupEventQueue","Init",0x4B2AD0,256},{"CInformGroupEventQueue","Flush",0x4AC410,256},
    {"CInformGroupEventQueue","Add",0x4B7CD0,1024},{"CInformGroupEventQueue","Process",0x4B2AE0,1024},
    {"CInformGroupEvent","Constructor",0x4AC350,64},{"CInformGroupEvent","Destructor",0x4B29A0,256},
    {"CInformGroupEvent","Process",0x4B29E0,1024},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"inform-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"inform-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("inform");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; auto report=_CrtSetReportHook(Report);
    std::array<ReversibleHooks::ReversibleHook::Base*,11> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",fns[f].name}});_CrtSetReportHook(report);return;}
    FillTraps(std::make_integer_sequence<int,24>{}); vtbl[0]=reinterpret_cast<void*>(&Dtor); vtbl[1]=reinterpret_cast<void*>(&Type); vtbl[4]=reinterpret_cast<void*>(&Clone);

    // Estado global que se toca: se guarda y se restaura byte a byte.
    const auto playerGroupId=player->GetPlayerData()->m_nPlayerGroup; auto& playerGroup=player->GetPlayerGroup(); auto* playerMembers=reinterpret_cast<CPed**>(reinterpret_cast<uint8*>(&playerGroup)+0xC);
    const auto savedFriends=ReversingProbe::Bytes(&friends,sizeof(friends)), savedGroupQ=ReversingProbe::Bytes(&groupQueue,sizeof(groupQueue));
    const auto savedActive=CPedGroups::ms_activeGroups; const auto savedTime=CTimer::m_snTimeInMilliseconds;
    const auto savedMembers=ReversingProbe::Bytes(playerMembers,8*sizeof(CPed*)); const auto savedPlayer=ReversingProbe::Bytes(player,sizeof(CPlayerPed));
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    std::array<bool,11> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        ReversingProbe::EntryPatch addDll(std::bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),EventGroupAdd), addExe(0x4AB420,EventGroupAdd);
        ReversingProbe::EntryPatch giDll(std::bit_cast<uintptr_t>(static_cast<bool(CPedGroupIntelligence::*)(CEvent*)>(&CPedGroupIntelligence::AddEvent)),GroupAddEvent), giExe(0x5F7470,GroupAddEvent);
        ReversingProbe::EntryPatch crDll(std::bit_cast<uintptr_t>(static_cast<void(CEventEditableResponse::*)(CPedGroup*)>(&CEventEditableResponse::ComputeResponseTaskType)),ComputeResponse), crExe(0x4B57A0,ComputeResponse);
        ReversingProbe::EntryPatch pgExe(0x5F7E80,GetPedsGroup);
        ReversingProbe::EntryPatch rngDll(std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), rngExe(0x821B1E,FakeRand);
        const std::array<int,4> eventTypes{EVENT_ACQUAINTANCE_PED_HATE,EVENT_ACQUAINTANCE_PED_DISLIKE,EVENT_DAMAGE,EVENT_SHOT_FIRED};
        for(size_t f=0;f<fns.size()&&passed;++f) {
            const auto& fn=fns[f];
            for(int i=0;i<fn.cases&&passed;++i) {
                const auto mix=[&](int n){return (uint32(i)*2654435761u+uint32(f)*0x9E3779B9u)>>(n%23)^uint32(n*0x45D9F3B);};
                // Construye el estado (identico para ambos lados) y devuelve lo necesario para la llamada.
                struct Args { CPed* ped{}; CPedGroup* group{}; FakeEvent* event{}; } args;
                auto setup=[&]() {
                    ClearRefs(); nextEvent=0; rngCalls=0; seed=i*31+int(f); trapped=false;
                    for(auto& p:peds)std::memset(p.data(),0,p.size());
                    for(int n=0;n<PEDS;++n) {
                        auto* p=Ped(n); p->SetType(ENTITY_TYPE_PED);
                        *reinterpret_cast<CPedIntelligence**>(peds[n].data()+0x47C)=reinterpret_cast<CPedIntelligence*>(intel[n].data());
                        p->SetCreatedBy((mix(n+40)%3)==0 ? PED_MISSION:PED_GAME);
                        p->bInVehicle=(mix(n+50)&1)!=0;
                        pedsGroup[n]=(mix(n+60)%4)==0 ? -1:int(mix(n+70)%8);
                    }
                    for(int g=0;g<8;++g)CPedGroups::ms_activeGroups[g]=char((mix(g+80)%3)!=0);
                    CPedGroups::ms_activeGroups[playerGroupId]=1; // el grupo del jugador siempre existe (GetPlayerGroup lo exige)
                    CTimer::m_snTimeInMilliseconds=200000+(i%7)*13;
                    const std::array<int32,8> deltas{-1000,-1,0,1,1000,0,0,0};
                    auto time=[&](int n)->int32 { const auto m=mix(n)%9; return m==5 ? -1 : m==6 ? int32(0x80000000) : m==7 ? 0 : int32(CTimer::m_snTimeInMilliseconds)+deltas[m%5]; };
                    auto entry=[&](int k,CPed*& ped,CEvent*& ev,int32& t) {
                        const auto mode=mix(k+10)%6; ped=nullptr; ev=nullptr; t=-1;
                        if(mode>=2&&mode<=4){ped=Ped(mix(k+20)%PEDS);ev=reinterpret_cast<CEvent*>(NewEvent(eventTypes[mix(k+30)%4]));t=time(k);}
                        else if(mode==5){ev=reinterpret_cast<CEvent*>(NewEvent(eventTypes[mix(k+30)%4]));t=time(k);}
                    };
                    for(int k=0;k<8;++k) {
                        entry(k,friends[k].m_Ped,friends[k].m_Event,friends[k].m_Time);
                        entry(k+8,groupQueue[k].m_Ped,groupQueue[k].m_Event,groupQueue[k].m_Time);
                        groupQueue[k].m_PedGroup=(mix(k+90)%5)==0 ? nullptr:&CPedGroups::ms_groups[mix(k+95)%8];
                        if(friends[k].m_Ped)friends[k].m_Ped->RegisterReference(reinterpret_cast<CEntity**>(&friends[k].m_Ped));
                        if(groupQueue[k].m_Ped)groupQueue[k].m_Ped->RegisterReference(reinterpret_cast<CEntity**>(&groupQueue[k].m_Ped));
                    }
                    auto& o=*reinterpret_cast<CInformGroupEvent*>(object.data());
                    if(fn.address==0x4AC350){for(size_t b=0;b<object.size();++b)object[b]=uint8(b*29+i);}
                    else {
                        CPed* p; CEvent* e; int32 t; entry(i%13+20,p,e,t); o.m_Ped=p; o.m_Event=e; o.m_Time=t;
                        o.m_PedGroup=(i&1) ? &CPedGroups::ms_groups[i%8]:nullptr;
                        if(o.m_Ped)o.m_Ped->RegisterReference(reinterpret_cast<CEntity**>(&o.m_Ped));
                    }
                    args.ped=(i%11)==10 ? nullptr:Ped(mix(100)%PEDS); if(!args.ped)args.ped=Ped(0);
                    args.group=(mix(101)%4)==0 ? nullptr:&CPedGroups::ms_groups[mix(102)%8];
                    args.event=NewEvent(eventTypes[mix(103)%4]);
                    *reinterpret_cast<CPed**>(args.event->body+0x10)=Ped(mix(104)%PEDS); // m_AcquaintancePed (0x14)
                    for(int k=0;k<EVENTS;++k)responseTask[k]=(mix(105)&1) ? int16(TASK_NONE):int16(TASK_COMPLEX_KILL_PED_ON_FOOT);
                    for(int k=0;k<8;++k)playerMembers[k]=reinterpret_cast<CPed* const*>(savedMembers.data())[k];
                    if(mix(106)%3==0)playerMembers[3]=args.ped; // ped del grupo del jugador
                };
                auto run=[&](bool candidate)->json {
                    setup(); trace=json::array();
                    for(auto* h:hooks)h->State(candidate);
                    bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    json ret=nullptr;
                    switch(f) {
                    case 0: case 1: case 3: case 4: case 5: case 7: reinterpret_cast<void(__cdecl*)()>(fn.address)(); break;
                    case 2: ret=reinterpret_cast<bool(__cdecl*)(CPed*,CEvent*)>(fn.address)(args.ped,reinterpret_cast<CEvent*>(args.event)); break;
                    case 6: ret=reinterpret_cast<bool(__cdecl*)(CPed*,CPedGroup*,CEventEditableResponse*)>(fn.address)(args.ped,args.group,reinterpret_cast<CEventEditableResponse*>(args.event)); break;
                    case 8: ret=Name(reinterpret_cast<void*(__thiscall*)(void*)>(fn.address)(object.data())); break;
                    case 9: reinterpret_cast<void*(__thiscall*)(void*)>(fn.address)(object.data()); break;
                    case 10: reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(object.data()); break;
                    }
                    json snap={{"return",ret},{"friends",Friends()},{"group_queue",GroupQ()},{"refs",Refs()},{"trace",trace},{"rng_calls",rngCalls},{"events",Events()},
                               {"time",CTimer::m_snTimeInMilliseconds},{"switches",switches},{"trapped",trapped}};
                    snap["object"]=f==8 ? json(ReversingProbe::Bytes(object.data(),object.size())) : Object();
                    ClearRefs();
                    return snap;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["trapped"]==false;
                passed=ok;
                Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"original",original},{"candidate",candidate},{"equal",original==candidate},{"accepted",ok}});
            }
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    ClearRefs();
    std::memcpy(&friends,savedFriends.data(),savedFriends.size()); std::memcpy(&groupQueue,savedGroupQ.data(),savedGroupQ.size());
    CPedGroups::ms_activeGroups=savedActive; CTimer::m_snTimeInMilliseconds=savedTime; std::memcpy(playerMembers,savedMembers.data(),savedMembers.size());
    const bool restored=ReversingProbe::Bytes(player,sizeof(CPlayerPed))==savedPlayer&&freeRefs()==savedFree;
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});_CrtSetReportHook(report);
}
}
