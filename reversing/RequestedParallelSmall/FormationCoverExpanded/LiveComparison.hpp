#pragma once
#include <fstream>
#include "Formation.h"
#include "PointList.h"
#include "../HookSnapshots.hpp"
namespace FormationCoverExpandedLive {
inline bool done{};
inline float Distance(const CVector& a, const CVector& b) {
    const float firstX=a.x,firstY=a.y,secondX=b.x,secondY=b.y;
    float result;
    __asm {
        fld firstX
        fsub secondX
        fld firstY
        fsub secondY
        fld st(0)
        fmul st,st(1)
        fld st(2)
        fmul st,st(3)
        faddp st(1),st
        fsqrt
        fstp st(2)
        fstp st(0)
        fstp result
    }
    return result;
}
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"formation-cover-expanded-runtime.start")){done=true;return;}
    done=true;
    std::ofstream log(dir/"formation-cover-expanded-runtime.jsonl");
    auto write=[&](const json& row){log<<row.dump()<<'\n';log.flush();};
    ReversibleHooks::ReversibleHook::Simple* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto&cat){
        if(cat.Name()=="CFormation")for(auto&h:cat.Items())if(h->Name()=="FindCoverPointsBehindBox")hook=static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get());
    });
    if(!hook){write({{"event","error"},{"reason","missing hook"}});return;}
    const auto allHooks=SmallPendingSupport::SnapshotHooks();
    const auto gtaBefore=ReversingProbe::Bytes(hook->GetHookGTAAddress(),52),dllBefore=ReversingProbe::Bytes(hook->GetHookOurAddress(),52);
    const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
    bool passed=true;int failures{},heightInside{},heightOutside{},added{},unchanged{};
    CMatrix matrix;
    alignas(CPointList)std::array<uint8,sizeof(CPointList)> points{};
    for(int n=0;n<4096;++n){
        // Valid rigid transforms, nondegenerate boxes, and output capacities 0..24.
        matrix.SetRotate(float((n/64)%3-1)*.3f,float((n/192)%3-1)*.2f,float((n/4)%8)*(PI/4.f));
        matrix.GetPosition()=CVector{float(n%3)*.125f,float(n%5)*.25f,0.f};
        const CVector min{-float(1+n%4),-float(1+(n/16)%3),-.5f};
        const CVector max{float(1+(n/32)%3),float(1+(n/48)%4),.75f},center{.125f,.25f,99.f};
        CVector target{float((n/128)%4-2)*2.5f,float((n/512)%4-2)*2.75f,0.f};
        constexpr uint32 heights[]{0x00000000,0x40DFFFFF,0x40E00000,0x40E00001,0x80000000,0xC0DFFFFF,0xC0E00000,0xC0E00001};
        target.z=std::bit_cast<float>(heights[(n/8)%8]);
        const int cornerIndex=(n/256)%4;
        const CVector local{cornerIndex<2?min.x:max.x,(cornerIndex==0||cornerIndex==3)?min.y:max.y,0.f};
        CVector boundaryCorner;
        reinterpret_cast<CVector*(__cdecl*)(CVector*,const CMatrix*,const CVector*)>(0x59C890)(&boundaryCorner,&matrix,&local);
        const float distance=Distance(boundaryCorner,target);
        const int cutoffMode=(n/1024)%4;
        const float cutoff=cutoffMode==0?distance:cutoffMode==1?std::nextafter(distance,0.f):cutoffMode==2?std::nextafter(distance,std::numeric_limits<float>::infinity()):100.f;
        const uint32 count=n%4==0?24u:n%4==1?23u:n%4==2?0u:22u;
        json results[2];
        for(int side=0;side<2;++side){
            points.fill(0xCD);auto*out=reinterpret_cast<CPointList*>(points.data());out->m_Count=count;
            hook->State(side!=0);const bool switched=SmallPendingSupport::ExactSwitch(*hook,side!=0);
            write({{"event","case_start"},{"case",n},{"candidate",side!=0}});
            reinterpret_cast<void(__cdecl*)(CPointList*,CVector,CMatrix*,const CVector&,const CVector&,const CVector&,float)>(0x699FF0)(out,target,&matrix,center,min,max,cutoff);
            results[side]={{"points",ReversingProbe::Bytes(points.data(),points.size())},{"count",out->m_Count},{"switched",switched}};
        }
        const bool equal=results[0]==results[1]&&results[0]["switched"]==true;
        passed&=equal;failures+=!equal;
        heightInside+=std::abs(target.z)<7.f;heightOutside+=!(std::abs(target.z)<7.f);
        added+=results[0]["count"].get<uint32>()>count;unchanged+=results[0]["count"].get<uint32>()==count;
        write({{"event","comparison"},{"function","CFormation::FindCoverPointsBehindBox"},{"pair",n},{"case",n},{"height_bits",heights[(n/8)%8]},{"cutoff_mode",cutoffMode},{"corner",cornerIndex},{"input_count",count},{"equal",equal},{"accepted",equal},{"original",results[0]},{"candidate",results[1]}});
    }
    hook->State(enabled);hook->LockState(locked);
    const bool restored=SmallPendingSupport::HooksUnchanged(allHooks)&&gtaBefore==ReversingProbe::Bytes(hook->GetHookGTAAddress(),52)&&dllBefore==ReversingProbe::Bytes(hook->GetHookOurAddress(),52);
    const bool coverage=heightInside>0&&heightOutside>0&&added>0&&unchanged>0;
    write({{"event","function_summary"},{"function","CFormation::FindCoverPointsBehindBox"},{"cases",4096},{"failures",failures},{"height_inside",heightInside},{"height_outside",heightOutside},{"added",added},{"unchanged",unchanged},{"coverage",coverage}});
    write({{"event",passed&&restored&&coverage?"complete":"error"},{"pairs",4096},{"state_restored",restored}});
}
}
