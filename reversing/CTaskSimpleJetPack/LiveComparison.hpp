#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskSimpleJetPack.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace JetLive {
using ReversingProbe::Bytes;
inline bool done{},realGeneration{};
inline uint32 randomState{};
inline int32 generated=-1;
inline json events=json::array();
inline std::ofstream log;
inline ReversingProbe::EntryPatch *generatePatch{},*coordsPatch{};
inline void Write(const json& r) { log << r.dump() << '\n'; log.flush(); }
inline int Report(int type,char* msg,int*) {
    if(type!=_CRT_ASSERT) return FALSE;
    Write({{"event","assertion"},{"message",msg ? msg : ""}}); ExitProcess(91);
}
inline int __cdecl Random() { randomState=randomState*214013u+2531011u; return int((randomState>>16)&32767); }
inline void __cdecl Coords(float x,float y,float z,float& ox,float& oy,float& oz) {
    coordsPatch->Enable(false);
    reinterpret_cast<void(__cdecl*)(float,float,float,float&,float&,float&)>(0x458A80)(x,y,z,ox,oy,oz);
    coordsPatch->Enable(true); events.push_back({"coords",x,y,z,ox,oy,oz});
}
inline int32 __cdecl Generate(CVector pos,uint32 model,ePickupType type,uint32 ammo,uint32 money,bool empty,char* message) {
    int32 result=-1;
    if(realGeneration) {
        generatePatch->Enable(false);
        result=reinterpret_cast<int32(__cdecl*)(CVector,uint32,ePickupType,uint32,uint32,bool,char*)>(0x456F20)(pos,model,type,ammo,money,empty,message);
        generatePatch->Enable(true);
    }
    generated=result; events.push_back({"generate",pos.x,pos.y,pos.z,model,int(type),ammo,money,empty,message==nullptr,result}); return result;
}
struct Jet : CTaskSimpleJetPack {
    bool abortResult;
    explicit Jet(int i) : abortResult((i&2)!=0) {
        auto* p=reinterpret_cast<uint8*>(this); p[8]=uint8(i&1); p[15]=0; p[13]=1; p[14]=uint8(i%3-1);
        float strafe=float(i%9-4)*.5f; std::memcpy(p+16,&strafe,4);
    }
    bool MakeAbortable(CPed*,eAbortPriority priority,const CEvent* event) override {
        events.push_back({"abort",int(priority),event==nullptr,abortResult});
        if(abortResult) reinterpret_cast<uint8*>(this)[8]=1;
        return abortResult;
    }
};
inline json PickupState() {
    json list=json::array();
    for(size_t i=0;i<CPickups::aPickUps.size();++i) {
        auto& p=CPickups::aPickUps[i];
        if(p.m_nPickupType==PICKUP_NONE) continue;
        auto position=p.GetPosn();
        list.push_back({i,p.m_nModelIndex,int(p.m_nPickupType),p.m_nAmmo,p.m_nRegenerationTime,p.m_nReferenceIndex,position.x,position.y,position.z,p.m_pObject!=nullptr});
    }
    return list;
}
inline void RestorePickups(const std::array<CPickup,MAX_NUM_PICKUPS>& saved) {
    for(size_t i=0;i<saved.size();++i) {
        auto& p=CPickups::aPickUps[i];
        if(p.m_pObject && p.m_pObject!=saved[i].m_pObject) p.GetRidOfObjects();
    }
    std::memcpy(CPickups::aPickUps.data(),saved.data(),sizeof(saved));
}
inline void Update() {
    if(done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"jetpack-runtime.start")) { done=true; return; }
    if(!log.is_open()) { log.open(dir/"jetpack-runtime.jsonl"); Write({{"event","waiting_for_player"}}); }
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning()) return;
    done=true; auto previousReport=_CrtSetReportHook(Report);
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        if(c.Name()=="CTaskSimpleJetPack") for(auto& h:c.Items()) if(h->Name()=="DropJetPack") hook=h.get();
    });
    if(!hook) { Write({{"event","error"},{"reason","missing hook"}}); _CrtSetReportHook(previousReport); return; }
    const auto savedPickups=CPickups::aPickUps; const auto savedPlayer=Bytes(player,sizeof(CPlayerPed));
    bool enabled=hook->Hooked(),locked=hook->Locked(); hook->LockState(false);
    bool passed=true; int pairs=0,created=0;
    {
        ReversingProbe::EntryPatch gen(0x456F20,Generate),coords(0x458A80,Coords),random(0x821B1E,Random);
        generatePatch=&gen; coordsPatch=&coords;
        auto entry=reinterpret_cast<void(__thiscall*)(CTaskSimpleJetPack*,CPed*)>(0x67B660);
        for(int i=0;i<128 && passed;++i) {
            realGeneration=i<64;
            alignas(CPlayerPed) std::array<uint8,sizeof(CPlayerPed)> a{},b{};
            std::memcpy(a.data(),player,a.size()); auto* pa=reinterpret_cast<CPlayerPed*>(a.data());
            CMatrixLink matrix=*player->m_matrix; pa->m_matrix=&matrix;
            if(i&4) { pa->m_matrix=nullptr; pa->SetPosn(player->GetPosition()); }
            b=a; auto* pb=reinterpret_cast<CPlayerPed*>(b.data()); CMatrixLink matrixB=matrix;
            if(pb->m_matrix) pb->m_matrix=&matrixB;
            const auto beforeA=a,beforeB=b;
            Jet ja(i),jb(i); const auto start=JetProbe::calls;
            hook->State(false); randomState=uint32(i+1); events=json::array(); generated=-1; entry(&ja,pa);
            const auto original=json{{"task",Bytes(reinterpret_cast<uint8*>(&ja)+8,0x68)},{"events",events},{"pickups",PickupState()},{"rng",randomState}};
            const auto middle=JetProbe::calls; if(generated!=-1) ++created;
            RestorePickups(savedPickups);
            hook->State(true); randomState=uint32(i+1); events=json::array(); generated=-1; entry(&jb,pb);
            const auto candidate=json{{"task",Bytes(reinterpret_cast<uint8*>(&jb)+8,0x68)},{"events",events},{"pickups",PickupState()},{"rng",randomState}};
            const auto end=JetProbe::calls; RestorePickups(savedPickups);
            const bool unchanged=a==beforeA && b==beforeB;
            passed=original==candidate && unchanged && middle==start && end==middle+1;
            Write({{"event","comparison"},{"pair",pairs++},{"real_generation",realGeneration},{"original",original},{"candidate",candidate},{"equal",passed},{"inputs_unchanged",unchanged},{"original_candidate_calls",middle-start},{"candidate_calls",end-middle}});
        }
    }
    hook->State(enabled); hook->LockState(locked);
    const bool restored=Bytes(player,sizeof(CPlayerPed))==savedPlayer && std::memcmp(CPickups::aPickUps.data(),savedPickups.data(),sizeof(savedPickups))==0 && hook->Hooked()==enabled && hook->Locked()==locked;
    Write({{"event",passed && restored ? "complete":"error"},{"pairs",pairs},{"created",created},{"state_restored",restored}});
    _CrtSetReportHook(previousReport);
}
}
