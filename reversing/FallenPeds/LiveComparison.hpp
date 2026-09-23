#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskSimpleRunNamedAnim.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace NamedLive {
inline bool done{};
inline std::ofstream log;
inline void Write(json r){log<<r.dump()<<'\n';log.flush();}
template<class T>struct Raw{alignas(T) std::array<uint8,sizeof(T)> bytes;T* get(){return reinterpret_cast<T*>(bytes.data());}};
inline json Vec(const CVector& v){return json::array({v.x,v.y,v.z});}
inline json Fields(CTaskSimpleRunNamedAnim* t){
    return {{"name",t->m_animName},{"group",t->m_animGroupName},{"blend",t->m_fBlendDelta},{"hierarchy",reinterpret_cast<uintptr_t>(t->m_pAnimHierarchy)},
        {"time",t->m_Time},{"timer",{t->m_Timer.m_nStartTime,t->m_Timer.m_nInterval,t->m_Timer.m_bStarted,t->m_Timer.m_bStopped}},
        {"offset",Vec(t->m_vecOffsetAtEnd)},{"flags",t->m_nFlags},{"anim_flags",t->m_animFlags},{"anim",reinterpret_cast<uintptr_t>(t->m_pAnim)}};
}
inline void Update(){
    if(done)return;
    if(!std::filesystem::exists("namedanim-init-runtime.start")){done=true;return;}
    auto* player=FindPlayerPed();if(!player||!CAnimManager::GetAnimationBlock("ped"))return;
    done=true;log.open("namedanim-init-runtime.jsonl");
    using Hook=ReversibleHooks::ReversibleHook::Base;Hook* ctor{};Hook* offset{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CTaskSimpleRunNamedAnim")for(auto& h:c.Items()){
        Write({{"event","hook"},{"name",h->Name()}});
        if(h->Name().find("Constructor")!=std::string::npos&&h->Name().find("Anim")!=std::string::npos)ctor=h.get();
        if(h->Name()=="OffsetPedPosition")offset=h.get();
    }});
    if(!ctor||!offset){Write({{"event","error"},{"reason","missing hook"}});return;}
    const auto savedPlayer=ReversingProbe::Bytes(player,sizeof(CPlayerPed));
    const bool ce=ctor->Hooked(),cl=ctor->Locked(),oe=offset->Hooked(),ol=offset->Locked();
    ctor->LockState(false);offset->LockState(false);
    unsigned pairs{},differences{};bool counters=true;
    Raw<CTaskSimpleRunNamedAnim> a,b;
    using Ctor=CTaskSimpleRunNamedAnim*(__thiscall*)(CTaskSimpleRunNamedAnim*,const char*,const char*,uint32,float,uint32,bool,bool,bool,bool);
    for(unsigned fill=0;fill<256;++fill)for(unsigned mask=0;mask<16;++mask)for(unsigned block=0;block<2;++block){
        a.bytes.fill(uint8(fill));b.bytes.fill(uint8(fill));
        auto run=[&](auto* t){return reinterpret_cast<Ctor>(0x61A990)(t,"IDLE_stance",block?"missing_probe":"ped",mask*17,mask&1?4.f:-2.f,mask&2?0xFFFFFFFFu:750u,mask&1,mask&2,mask&4,mask&8);};
        ctor->State(false);auto n0=NamedProbe::constructors;run(a.get());auto n1=NamedProbe::constructors;
        ctor->State(true);run(b.get());auto n2=NamedProbe::constructors;
        const auto x=Fields(a.get()),y=Fields(b.get());bool equal=x==y;
        counters &= n0==n1&&n2==n1+1;
        if(!equal)++differences;++pairs;
        Write({{"event","constructor"},{"fill",fill},{"mask",mask},{"missing_block",block},{"original",x},{"candidate",y},{"equal",equal},{"original_candidate_calls",n1-n0},{"candidate_calls",n2-n1}});
        a.get()->~CTaskSimpleRunNamedAnim();b.get()->~CTaskSimpleRunNamedAnim();
    }
    // Real, private RenderWare clump with no atomics: UpdateRpHAnim has no bones to mutate.
    auto* clump=RpClumpCreate();Raw<CPed> ped;CMatrixLink matrix;
    unsigned processPairs{},processDifferences{};
    for(unsigned i=0;i<128;++i){
        auto run=[&](bool candidate){
            ped.bytes.fill(0);std::memcpy(ped.bytes.data()+0x18,&clump,sizeof(clump));ped.get()->m_matrix=&matrix;
            matrix.SetRotateZ(float(i%32)*.1f);matrix.GetPosition()=CVector{2300.f,-1400.f,24.f};
            a.bytes.fill(std::array<uint8,4>{0,0xCD,0xA5,0xFF}[i/32]);
            ctor->State(candidate);offset->State(candidate);
            reinterpret_cast<Ctor>(0x61A990)(a.get(),"IDLE_stance","ped",0,4.f,750,false,false,true,false);
            // A completed task with offset-at-end enabled: native ProcessPed returns after consuming a pending offset.
            a.get()->m_bIsFinished=true;
            using Process=bool(__thiscall*)(CTaskSimpleRunNamedAnim*,CPed*);
            const auto returned=reinterpret_cast<Process>(0x61BF20)(a.get(),ped.get());
            auto result=json{{"pos",Vec(ped.get()->GetPosition())},{"returned",returned},{"available",bool(a.get()->m_bOffsetAvailable)}};
            a.get()->~CTaskSimpleRunNamedAnim();return result;
        };
        const auto x=run(false),y=run(true);const bool equal=x==y;
        if(!equal)++processDifferences;++processPairs;
        Write({{"event","process"},{"case",i},{"original",x},{"candidate",y},{"equal",equal}});
    }
    unsigned offsetPairs{},offsetDifferences{};float maxError{};
    for(unsigned i=0;i<512;++i){
        auto run=[&]{
            ped.bytes.fill(0);std::memcpy(ped.bytes.data()+0x18,&clump,sizeof(clump));ped.get()->m_matrix=&matrix;
            matrix.SetRotateZ(float(i%29)*.1f);matrix.GetPosition()=CVector{float(i%17),-float(i%31),float(i%7)};
            // Constructor leaves a zero displacement; nonzero cases also verify real offsets remain supported.
            a.bytes.fill(0);auto* t=::new(a.bytes.data()) CTaskSimpleRunNamedAnim("IDLE_stance","ped",0,4.f,750,false,false,true,false);
            t->m_bOffsetAvailable=true;t->m_vecOffsetAtEnd=i%2?CVector{float(i%11)*.125f,-float(i%13)*.25f,float(i%19)*.0625f}:CVector{};
            using Offset=void(__thiscall*)(CTaskSimpleRunNamedAnim*,CPed*);reinterpret_cast<Offset>(0x61AB00)(t,ped.get());
            const auto result=json{{"pos",Vec(ped.get()->GetPosition())},{"available",bool(t->m_bOffsetAvailable)},{"hierarchy_flag",bool(ped.get()->m_bDontUpdateHierarchy)}};
            t->~CTaskSimpleRunNamedAnim();return result;
        };
        offset->State(false);auto n0=NamedProbe::offsets;auto x=run();auto n1=NamedProbe::offsets;
        offset->State(true);auto y=run();auto n2=NamedProbe::offsets;
        float err{};for(int j=0;j<3;++j)err=std::max(err,std::abs(x["pos"][j].get<float>()-y["pos"][j].get<float>()));
        bool equal=err<=1e-5f&&x["available"]==y["available"]&&x["hierarchy_flag"]==y["hierarchy_flag"];
        counters &= n0==n1&&n2==n1+1;maxError=std::max(maxError,err);if(!equal)++offsetDifferences;++offsetPairs;
        Write({{"event","offset"},{"case",i},{"original",x},{"candidate",y},{"equal",equal},{"error",err},{"original_candidate_calls",n1-n0},{"candidate_calls",n2-n1}});
    }
    RpClumpDestroy(clump);
    ctor->State(ce);ctor->LockState(cl);offset->State(oe);offset->LockState(ol);
    bool restored=savedPlayer==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&ctor->Hooked()==ce&&ctor->Locked()==cl&&offset->Hooked()==oe&&offset->Locked()==ol;
    Write({{"event","complete"},{"constructor_pairs",pairs},{"constructor_differences",differences},{"process_pairs",processPairs},{"process_differences",processDifferences},{"offset_pairs",offsetPairs},{"offset_differences",offsetDifferences},{"max_error",maxError},{"counters_valid",counters},{"state_restored",restored}});
}
}
