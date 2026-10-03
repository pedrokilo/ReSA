#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AESound.h"
#include "Reference.h"
#include "References.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace SoundLive {
inline bool done{};inline std::ofstream log;
inline void Write(const json& r){ ReversingProbe::PumpMessages();log<<r.dump()<<'\n';}
inline int Report(int t,char* msg,int*){if(t!=_CRT_ASSERT)return FALSE;Write({{"event","assertion"},{"message",msg?msg:""}});log.flush();ExitProcess(91);}
template<class T>struct Raw{alignas(T) std::array<uint8,sizeof(T)> b{};T* get(){return reinterpret_cast<T*>(b.data());}};
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"sound-copy-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"sound-copy-runtime.jsonl");Write({{"event","waiting_for_player"}});log.flush();}
    auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;auto report=_CrtSetReportHook(Report);
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CAESound")for(auto& h:c.Items())if(h->Name()=="operator=")hook=h.get();});
    if(!hook){Write({{"event","error"},{"reason","missing hook"}});log.flush();_CrtSetReportHook(report);return;}
    const auto enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
    auto before=ReversingProbe::Bytes(player,sizeof(CPlayerPed));
    const auto savedRefs=ReversingProbe::Bytes(CReferences::aRefs,sizeof(CReferences::aRefs));
    auto* empty=CReferences::pEmptyList;std::array<CReference,16> refs{};
    Raw<CEntity> entities[2];Raw<CAESound> src,dst;
    Write({{"event","hook_state"},{"enabled",enabled},{"locked",locked},{"calls_before_bank",SoundProbe::calls}});
    bool passed=true;int pairs=0;
    for(int i=1;i<=1024&&passed;++i){
        auto* source=src.get();auto* dest=i%8==7?source:dst.get();
        const auto run=[&]{
            for(int j=0;j<16;++j)refs[j]={j==15?nullptr:&refs[j+1],nullptr};
            CReferences::pEmptyList=refs.data();
            for(auto& e:entities){e.b.fill(0);e.get()->SetTypeObject();}
            for(size_t j=0;j<src.b.size();++j){src.b[j]=uint8(i*71+j*29);dst.b[j]=uint8(i*31+j*17+1);}
            *reinterpret_cast<CEntity**>(src.b.data()+8)=nullptr;*reinterpret_cast<CEntity**>(dst.b.data()+8)=nullptr;
            if(i%4)source->RegisterWithPhysicalEntity(entities[0].get());
            if(i%3)dst.get()->RegisterWithPhysicalEntity(entities[i%2].get());
            const auto initialSource=src.b;
            auto* result=reinterpret_cast<CAESound*(__thiscall*)(CAESound*,const CAESound&)>(0x4EF680)(dest,*source);
            const auto refCount=[&](int e){int count=0;for(auto* r=entities[e].get()->m_pReferences;r;r=r->m_pNext){if(++count>16)break;}return count;};
            auto bytes=ReversingProbe::Bytes(dest,sizeof(CAESound));std::fill(bytes.begin()+8,bytes.begin()+12,0);
            int refId=dest->m_PhysicalEntity.Get()==entities[0].get()?0:dest->m_PhysicalEntity.Get()==entities[1].get()?1:-1;
            json out={{"returned_this",result==dest},{"bytes_without_entity_address",bytes},{"entity",refId},{"refs0",refCount(0)},{"refs1",refCount(1)},{"source_unchanged",src.b==initialSource}};
            entities[0].get()->ResolveReferences();entities[1].get()->ResolveReferences();
            out["source_cleared"]=source->m_PhysicalEntity.Get()==nullptr;out["dest_cleared"]=dest->m_PhysicalEntity.Get()==nullptr;
            // Do not dereference the dangling reference in the unfixed diagnostic.
            if(dest->m_PhysicalEntity.Get()==nullptr){
                dest->m_Flags=SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY;dest->m_HasRequestedStopped=false;
                dest->UpdateParameters(10);out["stops_after_entity_removed"]=bool(dest->m_HasRequestedStopped);
            }else out["stops_after_entity_removed"]=false;
            // Raw fixtures have no destructors. All reference nodes are private to this run.
            return out;
        };
        hook->State(false);auto a=SoundProbe::calls;auto original=run();auto b=SoundProbe::calls;
        hook->State(true);auto candidate=run();auto c=SoundProbe::calls;
        passed=original==candidate&&original["returned_this"]==true&&original["source_unchanged"]==true&&original["dest_cleared"]==true;
        Write({{"event","comparison"},{"case",i-1},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b}});
        ++pairs;
    }
    CReferences::pEmptyList=empty;hook->State(enabled);hook->LockState(locked);
    bool restored=before==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&savedRefs==ReversingProbe::Bytes(CReferences::aRefs,sizeof(CReferences::aRefs))&&CReferences::pEmptyList==empty;
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});log.flush();_CrtSetReportHook(report);
}
}
