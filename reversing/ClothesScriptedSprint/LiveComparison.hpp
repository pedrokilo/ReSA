#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Clothes.h"
#include "Scripted2dEffects.h"
#include "TaskComplexUseClosestFreeScriptedAttractorSprint.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace QuickLive {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r){ ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline int Report(int t,char* msg,int*){if(t!=_CRT_ASSERT)return FALSE;Write({{"event","assertion"},{"message",msg?msg:""}});ExitProcess(91);}
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"clothes-scripted-sprint-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"clothes-scripted-sprint-runtime.jsonl");Write({{"event","waiting_for_player"}});}
    auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;auto report=_CrtSetReportHook(Report);
    using Hook=ReversibleHooks::ReversibleHook::Base;
    std::array<Hook*,4> hooks{};Hook* baseCtor{};
    const std::array names{"GetTextureDependency","Init","Clone","GetTaskType"};
    const std::array classes{"CClothes","CScripted2dEffects","CTaskComplexUseClosestFreeScriptedAttractorSprint","CTaskComplexUseClosestFreeScriptedAttractorSprint"};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){
        for(auto& h:c.Items()){
            for(int n=0;n<4;++n)if(c.Name()==classes[n]&&h->Name()==names[n])hooks[n]=h.get();
            if(c.Name()=="CTaskComplexUseClosestFreeScriptedAttractor"&&h->Name()=="Constructor")baseCtor=h.get();
        }
    });
    if(!baseCtor||std::ranges::any_of(hooks,[](auto* h){return !h;})){Write({{"event","error"},{"reason","missing hooks"}});_CrtSetReportHook(report);return;}
    auto playerBefore=ReversingProbe::Bytes(player,sizeof(CPlayerPed));
    auto* globals=reinterpret_cast<uint8*>(0xC39ED0);constexpr size_t length=0xC3E010-0xC39ED0;
    const auto saved=ReversingProbe::Bytes(globals,length);
    const auto baseEnabled=baseCtor->Hooked(),baseLocked=baseCtor->Locked();
    baseCtor->LockState(false);baseCtor->State(false);
    const auto poolBefore=GetTaskPool()->GetNoOfUsedSpaces();
    bool passed=true;int pair=0;
    for(int fn=0;fn<4&&passed;++fn){
        auto* h=hooks[fn];const auto enabled=h->Hooked(),locked=h->Locked();h->LockState(false);
        Write({{"event","hook_state"},{"function",fn},{"enabled",enabled},{"locked",locked},{"startup_calls",QuickProbe::calls[fn]}});
        const int total=fn==0?4096:fn==1?256:2048;
        for(int i=0;i<total&&passed;++i){
            alignas(16) std::array<uint8,48> source{};
            for(size_t j=0;j<source.size();++j)source[j]=uint8(i*73+j*19);
            auto* task=reinterpret_cast<CTaskComplexUseClosestFreeScriptedAttractorSprint*>(source.data()+16);
            // Source is read-only for Clone/GetTaskType, including arbitrary parent/subtask/move words.
            const auto sourceBefore=source;
            std::vector<uint8> seed(length);
            for(size_t j=0;j<length;++j)seed[j]=uint8(i==0?0:i==1?255:(uint32(i)*2654435761u+uint32(j)*2246822519u)>>(j%4*8));
            const auto run=[&]{
                // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot
                { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0; }
                json out;
                if(fn==0){
                    const int32 input=i<2048?i-1024:std::bit_cast<int32>(uint32(i)*2654435761u);
                    auto value=reinterpret_cast<int32(__cdecl*)(int32)>(0x5A7EA0)(input);
                    out={{"input",input},{"result",value}};
                }else if(fn==1){
                    std::memcpy(globals,seed.data(),length);
                    reinterpret_cast<void(__cdecl*)()>(0x6FA6F0)();
                    out={{"bytes",ReversingProbe::Bytes(globals,length)}};
                    // Restore synchronously before the rest of the engine can see fixtures.
                    std::memcpy(globals,saved.data(),length);
                }else if(fn==2){
                    const auto used=GetTaskPool()->GetNoOfUsedSpaces();
                    auto* clone=reinterpret_cast<CTask*(__thiscall*)(const CTask*)>(0x4B8870)(task);
                    if(!clone){out={{"allocated",false}};return out;}
                    const auto bytes=ReversingProbe::Bytes(reinterpret_cast<uint8*>(clone)+4,12);
                    const auto type=clone->GetTaskType();
                    const auto allocation=GetTaskPool()->GetNoOfUsedSpaces()==used+1;
                    delete clone;
                    out={{"allocated",true},{"object_without_vtable",bytes},{"type",int(type)},{"one_allocation",allocation},{"pool_restored",GetTaskPool()->GetNoOfUsedSpaces()==used}};
                }else{
                    out={{"result",int(reinterpret_cast<eTaskType(__thiscall*)(const CTask*)>(0x4B88D0)(task))}};
                }
                out["source_unchanged"]=sourceBefore==source;return out;
            };
            h->State(false);auto a=QuickProbe::calls;auto original=run();auto b=QuickProbe::calls;
            h->State(true);auto candidate=run();auto c=QuickProbe::calls;
            passed=original==candidate&&a[fn]==b[fn]&&c[fn]==b[fn]+1&&original.value("source_unchanged",false);
            if(fn==2)passed=passed&&original.value("allocated",false)&&original.value("one_allocation",false)&&original.value("pool_restored",false)&&original["type"]==262&&original["object_without_vtable"]==json(std::array<uint8,12>{0,0,0,0,0,0,0,0,7,0,0,0});
            if(fn==3)passed=passed&&original["result"]==262;
            Write({{"event","comparison"},{"pair",pair++},{"function",fn},{"case",i},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b[fn]-a[fn]},{"candidate_calls",c[fn]-b[fn]}});
        }
        h->State(enabled);h->LockState(locked);
    }
    baseCtor->State(baseEnabled);baseCtor->LockState(baseLocked);
    const bool restored=saved==ReversingProbe::Bytes(globals,length)&&poolBefore==GetTaskPool()->GetNoOfUsedSpaces()&&playerBefore==ReversingProbe::Bytes(player,sizeof(CPlayerPed));
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});
    _CrtSetReportHook(report);
}
}
