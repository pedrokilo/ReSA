#pragma once
#include <fstream>
#include <float.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "CustomBuildingRenderer.h"
#include "CustomBuildingDNPipeline.h"
#include "CustomBuildingPipeline.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace BuildingLive {
inline bool done{};
inline std::ofstream log;
inline void Write(json j){log<<j.dump()<<'\n';}
using Hook=ReversibleHooks::ReversibleHook::Base;
inline Hook* Find(const char* cls,const char* name){Hook* p{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)p=h.get();});return p;}
struct SavedHook {Hook* h;bool enabled,locked;SavedHook(Hook* p):h(p),enabled(p->Hooked()),locked(p->Locked()){h->LockState(false);}~SavedHook(){h->State(enabled);h->LockState(locked);}};
inline uint32 Bits(float f){uint32 n;std::memcpy(&n,&f,4);return n;}
struct Fixture {
    RpAtomic* a{};RpGeometry* g{};RwRGBA night[3]{};
    Fixture(unsigned mode,uint32 id,unsigned seed){
        g=RpGeometryCreate(3,0,rpGEOMETRYPOSITIONS|((mode&1)?rpGEOMETRYPRELIT:0));
        a=RpAtomicCreate();if(!g||!a)std::abort();RpAtomicSetGeometry(a,g,0);
        auto* extra=CCustomBuildingDNPipeline::GetExtraVertColourPtr(g);
        extra->NightColors=(mode&2)?night:nullptr;extra->DNBalance=float(seed)*.125f;
        for(unsigned i=0;i<3;++i){night[i]={uint8(seed+i),uint8(seed*3+i),uint8(255-i),255};if(g->preLitLum)g->preLitLum[i]=night[i];}
        SetPipelineID(a,id);
    }
    ~Fixture(){CCustomBuildingDNPipeline::GetExtraVertColourPtr(g)->NightColors=nullptr;RpAtomicDestroy(a);RpGeometryDestroy(g);}
    json Snapshot(){
        auto* ex=CCustomBuildingDNPipeline::GetExtraVertColourPtr(g);
        json colors=json::array();for(int i=0;i<3;++i)if(g->preLitLum)colors.push_back({g->preLitLum[i].red,g->preLitLum[i].green,g->preLitLum[i].blue,g->preLitLum[i].alpha});
        return {{"id",GetPipelineID(a)},{"pipe",reinterpret_cast<uintptr_t>(a->pipeline)},{"usage",int(RpD3D9GeometryGetUsageFlags(g))},{"colors",colors},{"night",ex->NightColors==night},{"balance",Bits(ex->DNBalance)},{"geometry_same",a->geometry==g},{"flags",g->flags},{"materials",g->matList.numMaterials}};
    }
};
inline void Update(){
    if(done)return;if(!std::filesystem::exists("custom-building-runtime.start")){done=true;return;}
    if(!FindPlayerPed()||!CCustomBuildingPipeline::ObjPipeline||!CCustomBuildingDNPipeline::ObjPipeline)return;
    done=true;log.open("custom-building-runtime.jsonl");
    std::array<Hook*,5> hs{Find("CCustomBuildingRenderer","AtomicSetup"),Find("CCustomBuildingRenderer","IsCBPCPipelineAttached"),Find("CCustomBuildingRenderer","UpdateDayNightBalanceParam"),Find("CCustomBuildingRenderer","Update"),Find("CCustomBuildingDNPipeline","CustomPipeAtomicSetup")};
    for(auto* h:hs)if(!h){Write({{"event","error"},{"reason","missing hook"}});log.flush();return;}
    auto* normal=Find("CCustomBuildingPipeline","CustomPipeAtomicSetup");if(!normal)std::abort();
    const auto h=CClock::ms_nGameClockHours,m=CClock::ms_nGameClockMinutes;const auto s=CClock::ms_nGameClockSeconds;
    const auto bal=CCustomBuildingDNPipeline::m_fDNBalanceParam;
    auto& ix=StaticRef<uint32>(0xC02C14);auto& flag=StaticRef<uint32>(0xC02C18);const auto oldIx=ix,oldFlag=flag;
    const auto player=ReversingProbe::Bytes(FindPlayerPed(),sizeof(CPlayerPed));
    std::array<bool,5> initialEnabled{},initialLocked{};
    for(unsigned i=0;i<5;++i){initialEnabled[i]=hs[i]->Hooked();initialLocked[i]=hs[i]->Locked();Write({{"event","hook"},{"function",i},{"enabled",initialEnabled[i]},{"locked",initialLocked[i]},{"calls_before_bank",BuildingProbe::calls[i]}});}
    unsigned counts[5]{},diffs[5]{};bool counters=true,unchanged=true;
    {
        SavedHook a(hs[0]),b(hs[1]),c(hs[2]),d(hs[3]),e(hs[4]),f(normal);normal->State(false);
        auto select=[&](unsigned fn,bool candidate){hs[fn]->State(candidate);if(fn==0)hs[4]->State(candidate);if(fn==3)hs[2]->State(candidate);};
        for(unsigned fn=2;fn<=3;++fn)for(unsigned i=0;i<(fn==2?90496u:4096u);++i){
            unsigned hour,minute,second;
            if(i<86400&&fn==2){hour=i/3600;minute=i/60%60;second=i%60;}
            else {hour=(i*17)%256;minute=(i*31)%256;second=(i*101)%65536;}
            const uint32 initial=i%4==0?0xFFFFFFFFu:i%4==1?0xFFFFFFF0u:i;
            auto run=[&](bool cand){select(fn,cand);CClock::ms_nGameClockHours=uint8(hour);CClock::ms_nGameClockMinutes=uint8(minute);CClock::ms_nGameClockSeconds=uint16(second);ix=initial;flag=0x12340000u+i;CCustomBuildingDNPipeline::m_fDNBalanceParam=-99.f;
                reinterpret_cast<void(__cdecl*)()>(fn==2?0x5D7F80:0x5D8050)();
                unchanged &= CClock::ms_nGameClockHours==hour&&CClock::ms_nGameClockMinutes==minute&&CClock::ms_nGameClockSeconds==second;
                return std::array<uint32,3>{Bits(CCustomBuildingDNPipeline::m_fDNBalanceParam),ix,flag};};
            auto n0=BuildingProbe::calls[fn];auto x=run(false);auto n1=BuildingProbe::calls[fn];auto y=run(true);auto n2=BuildingProbe::calls[fn];
            counters &= n1==n0&&n2==n1+1;bool equal=x==y;++counts[fn];if(!equal)++diffs[fn];
            Write({{"event","pair"},{"function",fn},{"case",i},{"original",x},{"candidate",y},{"equal",equal},{"original_candidate_calls",n1-n0},{"candidate_calls",n2-n1}});
        }
        const uint32 ids[]{0,0x53F20098,0x53F2009C,0x53F2009A,0xFFFFFFFF,1,0x53F20097,0x53F2009D};
        for(unsigned fn: {0u,1u,4u})for(unsigned i=0;i<(fn==4?64u:256u);++i){
            const unsigned mode=i%4;Fixture x(mode,ids[i/4%8],i/32),y(mode,ids[i/4%8],i/32);
            auto beforeX=x.Snapshot(),beforeY=y.Snapshot();bool returnedX{},returnedY{};
            auto run=[&](Fixture& fix,bool cand){select(fn,cand);if(fn==0)reinterpret_cast<void(__cdecl*)(RpAtomic*)>(0x5D7F00)(fix.a);
                else if(fn==1)return reinterpret_cast<bool(__cdecl*)(RpAtomic*)>(0x5D7F40)(fix.a);
                else return reinterpret_cast<RpAtomic*(__cdecl*)(RpAtomic*)>(0x5D71C0)(fix.a)==fix.a;return true;};
            auto n0=BuildingProbe::calls[fn];returnedX=run(x,false);auto n1=BuildingProbe::calls[fn];returnedY=run(y,true);auto n2=BuildingProbe::calls[fn];
            auto sx=x.Snapshot(),sy=y.Snapshot();counters &= n0==n1&&n2==n1+1;
            if(fn==1)unchanged &= sx==beforeX&&sy==beforeY;
            const bool equal=sx==sy&&returnedX==returnedY;++counts[fn];if(!equal)++diffs[fn];
            Write({{"event","pair"},{"function",fn},{"case",i},{"mode",mode},{"input_id",ids[i/4%8]},{"original",sx},{"candidate",sy},{"returned_original",returnedX},{"returned_candidate",returnedY},{"equal",equal},{"original_candidate_calls",n1-n0},{"candidate_calls",n2-n1}});
        }
    }
    CClock::ms_nGameClockHours=h;CClock::ms_nGameClockMinutes=m;CClock::ms_nGameClockSeconds=s;CCustomBuildingDNPipeline::m_fDNBalanceParam=bal;ix=oldIx;flag=oldFlag;
    bool hooksRestored=true;for(unsigned i=0;i<5;++i)hooksRestored &= hs[i]->Hooked()==initialEnabled[i]&&hs[i]->Locked()==initialLocked[i];
    unsigned fpControl{};_controlfp_s(&fpControl,0,0);Write({{"event","environment"},{"fp_control",fpControl},{"hooks_restored",hooksRestored}});
    Write({{"event","complete"},{"counts",counts},{"differences",diffs},{"counters_valid",counters},{"inputs_unchanged",unchanged},{"player_unchanged",player==ReversingProbe::Bytes(FindPlayerPed(),sizeof(CPlayerPed))},{"state_restored",CClock::ms_nGameClockHours==h&&CClock::ms_nGameClockMinutes==m&&CClock::ms_nGameClockSeconds==s&&Bits(CCustomBuildingDNPipeline::m_fDNBalanceParam)==Bits(bal)&&ix==oldIx&&flag==oldFlag}});log.flush();
}
}
