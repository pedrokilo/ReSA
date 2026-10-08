#pragma once
#include <fstream>
#include "Formation.h"
#include "PointList.h"
#include "../HookSnapshots.hpp"
namespace FormationCoverPendingLive {
inline bool done{};
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"formation-cover-pending-runtime.start")){done=true;return;}
    done=true;std::ofstream log(dir/"formation-cover-pending-runtime.jsonl");auto write=[&](const json&r){log<<r.dump()<<'\n';log.flush();};
    ReversibleHooks::ReversibleHook::Simple* hook{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto&cat){if(cat.Name()=="CFormation")for(auto&h:cat.Items())if(h->Name()=="FindCoverPointsBehindBox")hook=static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get());});
    if(!hook){write({{"event","error"},{"reason","missing hook"}});return;}
    const auto allHooks=SmallPendingSupport::SnapshotHooks();const auto gtaBefore=ReversingProbe::Bytes(hook->GetHookGTAAddress(),52),dllBefore=ReversingProbe::Bytes(hook->GetHookOurAddress(),52);const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
    bool passed=true;int failures{};CMatrix matrix;alignas(CPointList)std::array<uint8,sizeof(CPointList)> points{};
    for(int n=0;n<256;++n){json results[2];for(int side=0;side<2;++side){points.fill(0xCD);auto*out=reinterpret_cast<CPointList*>(points.data());out->m_Count=n%4==0?24:n%4==1?23:0;matrix.SetUnity();matrix.GetPosition()=CVector{float(n%3),float(n%5),0.f};
        const CVector min{-2.f,-1.f,0.f},max{2.f,float(1+n%3),1.f},center{0.f,0.f,0.f};
        const CVector target{float((n%4)-2)*4.f,float(((n/4)%4)-2)*4.f,n%8==0?7.f:0.f};const float cutoff=n%2?100.f:0.f;
        hook->State(side!=0);const bool switched=SmallPendingSupport::ExactSwitch(*hook,side!=0);
        write({{"event","case_start"},{"case",n},{"candidate",side!=0}});
        reinterpret_cast<void(__cdecl*)(CPointList*,CVector,CMatrix*,const CVector&,const CVector&,const CVector&,float)>(0x699FF0)(out,target,&matrix,center,min,max,cutoff);
        results[side]={{"points",ReversingProbe::Bytes(points.data(),points.size())},{"switched",switched}};
    }const bool equal=results[0]==results[1]&&results[0]["switched"]==true;passed&=equal;failures+=!equal;write({{"event","comparison"},{"function","CFormation::FindCoverPointsBehindBox"},{"pair",n},{"case",n},{"equal",equal},{"accepted",equal},{"original",results[0]},{"candidate",results[1]}});}
    hook->State(enabled);hook->LockState(locked);bool restored=SmallPendingSupport::HooksUnchanged(allHooks)&&gtaBefore==ReversingProbe::Bytes(hook->GetHookGTAAddress(),52)&&dllBefore==ReversingProbe::Bytes(hook->GetHookOurAddress(),52);
    write({{"event","function_summary"},{"function","CFormation::FindCoverPointsBehindBox"},{"cases",256},{"failures",failures}});write({{"event",passed&&restored?"complete":"error"},{"pairs",256},{"state_restored",restored}});
}
}
