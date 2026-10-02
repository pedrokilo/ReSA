#pragma once
// Banco dentro del motor: asignadores de tareas por defecto de grupo (Random, SitInLeaderCar) y
// CTaskComplexGangFollower::SetCanEnterLeadersCar. Cada par ejecuta el original (hook desactivado,
// codigo del EXE) y el candidato (hook activado) sobre los mismos fixtures y compara exactamente:
// pares peaton/tarea resultantes, todas las tareas creadas en el pool (bytes, con punteros
// traducidos a identificadores), consultas de puerta controladas, fugas y mutacion de entradas.
#include <fstream>
#include <unordered_map>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "PedGroup.h"
#include "CarEnterExit.h"
#include "Pools/TaskPool.h"
#include "TaskComplexGangFollower.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"

namespace GroupAllocLive {
inline bool done{}; inline std::ofstream log;
inline json doorTrace; inline int seed{};
inline std::unordered_map<uintptr_t,std::string> names; // direcciones de fixtures -> nombre estable

inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
inline int Report(int type,char* msg,int*) { if(type!=_CRT_ASSERT)return FALSE; Write({{"event","assertion"},{"message",msg ? msg:""}}); ExitProcess(91); }

// Dependencia controlada: puerta objetivo en funcion de (vehiculo, asiento, semilla).
inline int32 __cdecl Door(const CVehicle* veh,int32 seat) {
    const auto it=names.find(uintptr_t(veh));
    doorTrace.push_back({it!=names.end() ? it->second : "?",seat});
    return (seat*7+seed)%11-1;
}

inline std::string Name(uintptr_t v) { const auto it=names.find(v); return it!=names.end() ? it->second : std::string{}; }

inline size_t TaskSize(eTaskType t) {
    switch(t) {
    case TASK_COMPLEX_GANG_FOLLOWER: return 0x4C;
    case TASK_COMPLEX_GANG_LEADER: return 0x38;
    case TASK_COMPLEX_SEQUENCE: return 0x40;
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER: case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: return 0x50;
    case TASK_SIMPLE_CAR_DRIVE: return 0x60;
    default: return 0;
    }
}

inline std::vector<bool> Occupancy() {
    auto* pool=GetTaskPool(); std::vector<bool> r(pool->GetSize());
    for(size_t k=0;k<r.size();++k)r[k]=!pool->IsFreeSlotAtIndex(k);
    return r;
}

// Serializa el resultado: pares (peaton, tarea) y la tabla de tareas nuevas alcanzables.
// Devuelve tambien las raices nuevas para liberarlas y el numero de tareas nuevas no alcanzadas.
struct Snapshot { json value; std::vector<CTask*> roots; int leaked{}; bool unknown{}; };
inline Snapshot Capture(CPedTaskPair* pairs,const std::vector<bool>& before) {
    auto* pool=GetTaskPool(); std::unordered_map<uintptr_t,int> fresh, ids;
    for(size_t k=0;k<before.size();++k)if(!before[k]&&!pool->IsFreeSlotAtIndex(k))fresh[uintptr_t(pool->GetAt(k))]=-1;
    Snapshot s; std::vector<CTask*> order; json pj=json::array();
    auto ref=[&](uintptr_t v)->json {
        if(!v)return nullptr;
        if(fresh.contains(v)) {
            if(!ids.contains(v)){ids[v]=int(order.size());order.push_back(reinterpret_cast<CTask*>(v));}
            return "T"+std::to_string(ids[v]);
        }
        if(auto n=Name(v);!n.empty())return n;
        return v;
    };
    for(int n=0;n<TOTAL_PED_GROUP_MEMBERS;++n) {
        const auto t=uintptr_t(pairs[n].Task);
        if(fresh.contains(t)&&!ids.contains(t))s.roots.push_back(pairs[n].Task);
        pj.push_back({ref(uintptr_t(pairs[n].Ped)),ref(t)});
    }
    json tj=json::array();
    for(size_t k=0;k<order.size();++k) {
        auto* task=order[k]; const auto type=task->GetTaskType(); const auto size=TaskSize(type);
        if(!size){s.unknown=true;tj.push_back({{"type",int(type)}});continue;}
        json words=json::array(); const auto* w=reinterpret_cast<const uint32*>(task);
        for(size_t o=1;o<size/4;++o) { // w[0] = vtable (EXE o DLL), se compara el tipo
            auto v=w[o];
            if(type==TASK_COMPLEX_SEQUENCE&&o*4==0x38)v&=0xFF; // m_bFlushTasks; 0x39..0x3B son relleno sin valor definido
            words.push_back(ref(v));
        }
        tj.push_back({{"type",int(type)},{"words",words}});
    }
    s.leaked=int(fresh.size()-ids.size());
    s.value={{"pairs",pj},{"tasks",tj},{"new_tasks",int(fresh.size())},{"doors",doorTrace}};
    return s;
}

inline auto* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"group-alloc-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"group-alloc-runtime.jsonl");Write({{"event","waiting_for_player"}});}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; auto report=_CrtSetReportHook(Report);
    const std::array fnNames{"Random::AllocateDefaultTasks","SitInLeaderCar::AllocateDefaultTasks","GangFollower::SetCanEnterLeadersCar"};
    std::array hooks{FindHook("CPedGroupDefaultTaskAllocatorRandom","AllocateDefaultTasks"),
                     FindHook("CPedGroupDefaultTaskAllocatorSitInLeaderCar","AllocateDefaultTasks"),
                     FindHook("CTaskComplexGangFollower","SetCanEnterLeadersCar")};
    if(std::ranges::any_of(hooks,[](auto* h){return !h;})){Write({{"event","error"},{"reason","hook not found"}});_CrtSetReportHook(report);return;}
    const std::array<uintptr_t,2> instances{0xC09918,0xC0991C}; // ms_RandomAllocator, ms_SitInLeaderCarAllocator
    const std::array<uintptr_t,2> originals{0x5F6E90,0x5F6FC0};
    const auto savedPlayer=ReversingProbe::Bytes(player,sizeof(CPlayerPed));
    const auto occupancyStart=Occupancy();
    std::array<bool,3> enabled{},locked{};
    for(int f=0;f<3;++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0; bool switching=true;
    {
        ReversingProbe::EntryPatch doorDll(std::bit_cast<uintptr_t>(&CCarEnterExit::ComputeTargetDoorToEnterAsPassenger),Door), doorExe(0x64F190,Door);
        constexpr int PEDS=10; // 0..7 miembros, 8 lider alternativo, 9 extrano fuera del grupo
        alignas(CPed) static std::array<std::array<uint8,sizeof(CPed)>,PEDS> pedStorage{};
        alignas(CVehicle) static std::array<uint8,sizeof(CVehicle)> vehStorage{};
        alignas(CPedGroup) static std::array<uint8,sizeof(CPedGroup)> groupStorage{};
        auto* group=reinterpret_cast<CPedGroup*>(groupStorage.data());
        auto* veh=reinterpret_cast<CVehicle*>(vehStorage.data());
        auto ped=[&](int n){return reinterpret_cast<CPed*>(pedStorage[n].data());};
        names.clear(); names[uintptr_t(group)]="group"; names[uintptr_t(veh)]="vehicle"; names[uintptr_t(player)]="player";
        if(player->m_pVehicle)names[uintptr_t(player->m_pVehicle)]="player_vehicle";
        for(int n=0;n<PEDS;++n)names[uintptr_t(ped(n))]="ped"+std::to_string(n);
        auto* members=reinterpret_cast<CPed**>(groupStorage.data()+0xC);          // CPedGroupMembership::m_members
        auto* pairs=reinterpret_cast<CPedTaskPair*>(groupStorage.data()+0x21C);   // m_DefaultPedTaskPairs
        for(int f=0;f<2&&passed;++f) {
            const auto vt=*reinterpret_cast<uintptr_t**>(instances[f]);
            for(int i=0;i<2048&&passed;++i) {
                seed=i; const auto mix=[&](int n){return (uint32(i)*2654435761u>>(n*3+5))^uint32(n*0x9E37);};
                for(auto& p:pedStorage)std::memset(p.data(),0,p.size());
                std::memset(vehStorage.data(),0,vehStorage.size()); std::memset(groupStorage.data(),0,groupStorage.size());
                for(int n=0;n<PEDS;++n){ped(n)->SetType(ENTITY_TYPE_PED);ped(n)->SetPosn(float(n*3+i%7),float(i%13)-6.f,float(n));}
                veh->SetType(ENTITY_TYPE_VEHICLE); veh->m_nMaxPassengers=uint8(i%10);
                *reinterpret_cast<CPedGroup**>(groupStorage.data()+0x8)=group;  // membership -> grupo
                *reinterpret_cast<CPedGroup**>(groupStorage.data()+0x30)=group; // inteligencia -> grupo
                group->m_bMembersEnterLeadersVehicle=((i>>10)&1)!=0;
                for(int n=0;n<7;++n)members[n]=(i>>n)&1 ? ped(n):nullptr;
                // Lider: ausente, ped7, ped8 o el jugador real (cubre la comparacion con FindPlayerPed).
                const int leaderMode=(i>>7)&3;
                CPed* leader=leaderMode==0 ? nullptr : leaderMode==1 ? ped(7) : leaderMode==2 ? ped(8) : (i&0x400 ? player:ped(7));
                members[7]=leader;
                if(f==1&&leader&&leader!=player)leader->m_pVehicle=(i>>9)&1 ? veh:nullptr;
                for(int n=0;n<8;++n) {
                    const auto m=mix(n)%8;
                    pairs[n].Ped=m<5 ? members[n] : m<6 ? nullptr : m<7 ? ped(9) : ped((n+1)%8);
                    pairs[n].Task=(mix(n+8)%5==0) ? reinterpret_cast<CTask*>(0xF00D0000u+n):nullptr; // centinela: nunca se libera
                    names[0xF00D0000u+n]="sentinel"+std::to_string(n);
                }
                const int pedMode=(i>>8)&3;
                CPed* argPed=pedMode==0 ? nullptr : pedMode==1 ? pairs[i%8].Ped : pedMode==2 ? leader : ped(9);
                const auto pedsBefore=pedStorage; const auto vehBefore=vehStorage; const auto groupBefore=groupStorage;
                const auto occupancy=Occupancy();
                auto run=[&](bool candidate)->json {
                    hooks[f]->State(candidate); hooks[2]->State(candidate); // original: tambien 0x65EE90 del EXE
                    if((vt[0]==originals[f])==candidate)switching=false;
                    doorTrace=json::array();
                    reinterpret_cast<void(__thiscall*)(uintptr_t,CPedGroup*,CPed*)>(vt[0])(instances[f],group,argPed);
                    auto snap=Capture(pairs,occupancy);
                    for(auto* t:snap.roots)delete t;
                    for(int n=0;n<8;++n)pairs[n].Task=reinterpret_cast<const CPedTaskPair*>(groupBefore.data()+0x21C)[n].Task;
                    snap.value["leaked"]=snap.leaked; snap.value["unknown_type"]=snap.unknown;
                    snap.value["pool_restored"]=Occupancy()==occupancy;
                    return snap.value;
                };
                auto start=GroupAllocProbe::calls; auto original=run(false); auto middle=GroupAllocProbe::calls;
                auto candidate=run(true); auto end=GroupAllocProbe::calls;
                const bool intact=pedsBefore==pedStorage&&vehBefore==vehStorage&&groupBefore==groupStorage;
                const bool clean=original["leaked"]==0&&original["pool_restored"]==true&&original["unknown_type"]==false&&candidate["pool_restored"]==true;
                passed=original==candidate&&intact&&clean&&switching&&middle==start&&end[f]==middle[f]+1;
                Write({{"event","comparison"},{"pair",pair++},{"function",fnNames[f]},{"case",i},{"original",original},{"candidate",candidate},
                       {"equal",original==candidate},{"inputs_restored",intact},{"clean",clean},{"hook_switches",switching},
                       {"original_candidate_calls",int(middle[f]-start[f])},{"candidate_calls",int(end[f]-middle[f])}});
            }
        }
        // SetCanEnterLeadersCar: 256 valores de flags x 2 argumentos, objeto completo comparado.
        for(int i=0;i<512&&passed;++i) {
            alignas(CTaskComplexGangFollower) std::array<uint8,sizeof(CTaskComplexGangFollower)> base{};
            for(size_t b=0;b<base.size();++b)base[b]=uint8(b*37+i);
            base[0x3D]=uint8(i&0xFF); const bool arg=(i>>8)&1;
            auto run=[&](bool candidate){
                hooks[2]->State(candidate);
                if((*reinterpret_cast<uint8*>(0x65EE90)==0xE9)!=candidate)switching=false;
                auto obj=base; reinterpret_cast<void(__thiscall*)(void*,bool)>(0x65EE90)(obj.data(),arg);
                return ReversingProbe::Bytes(obj.data(),obj.size());
            };
            auto start=GroupAllocProbe::calls; auto original=run(false); auto middle=GroupAllocProbe::calls;
            auto candidate=run(true); auto end=GroupAllocProbe::calls;
            passed=original==candidate&&switching&&middle==start&&end[2]==middle[2]+1;
            Write({{"event","comparison"},{"pair",pair++},{"function",fnNames[2]},{"case",i},{"original",original},{"candidate",candidate},
                   {"equal",original==candidate},{"inputs_restored",true},{"clean",true},{"hook_switches",switching},
                   {"original_candidate_calls",int(middle[2]-start[2])},{"candidate_calls",int(end[2]-middle[2])}});
        }
    }
    for(int f=0;f<3;++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=ReversingProbe::Bytes(player,sizeof(CPlayerPed))==savedPlayer&&Occupancy()==occupancyStart;
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});_CrtSetReportHook(report);
}
}
