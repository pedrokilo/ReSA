#pragma once
#include "Replay.h"
#include "Pickups.h"
#include "SpecialFX.h"
#include "UserDisplay.h"
#include "OnscreenTimer.h"
#include "Darkel.h"
#include "Garages.h"

namespace HookUnlockRenderPairs {
using namespace HookUnlockGamePairs;
alignas(16) inline std::array<uint8,sizeof(CPlayerPed)> fakePed{};
inline CPlayerPed* __cdecl Player(int32){return reinterpret_cast<CPlayerPed*>(fakePed.data());}
inline void __cdecl DrawRect(const CRect* rect,const CRGBA* color) {
    // Registrar floats y color por contenido, no la dirección local de pila.
    auto put=[&](uint32 value){if(nrec<recs.size()){Rec r{};r.stub=0xFFFE;r.n=1;r.v[0]=value;recs[nrec++]=r;}else overflow=true;};
    const auto* p=reinterpret_cast<const uint32*>(rect);for(int i=0;i<4;++i)put(p[i]);
    put(*reinterpret_cast<const uint32*>(color));
}
inline json RenderTrace() {
    json t=json::array();
    for(size_t i=0;i<nrec;++i){const auto& r=recs[i];json e=json::array();
        e.push_back(r.stub==0xFFFE?"DrawRect.value":Stubs()[r.stub].name);
        for(int k=0;k<r.n;++k)e.push_back(r.v[k]);t.push_back(e);}
    return t;
}
inline bool Run() {
    auto* hook=FindHook("Game","Render2dStuff");if(!hook)return false;
    const bool active=hook->Hooked(),lock=hook->Locked();hook->LockState(false);hook->State(true);
    auto backup=Stubs();auto* priorThunks=thunks;
    Stubs()={
        {0x45C210,A(&CReplay::Display),"Replay::Display",'c',"",'0'},
        {0x455000,A(&CPickups::RenderPickUpText),"Pickups::RenderPickUpText",'c',"",'0'},
        {0x514860,A(&CCamera::DrawBordersForWideScreen),"Camera::DrawBordersForWideScreen",'t',"",'0'},
        {0x507030,A(&CAudioEngine::DisplayRadioStationName),"Audio::DisplayRadioStationName",'t',"",'0'},
        {0x58FAE0,A(&CHud::Draw),"Hud::Draw",'c',"",'0'},
        {0x721660,A(&CSpecialFX::Render2DFXs),"SpecialFX::Render2DFXs",'c',"",'0'},
        {0x44CC20,A(&COnscreenTimer::ProcessForDisplay),"Timer::ProcessForDisplay",'t',"",'0'},
        {0x69EFC0,A(&CMessages::Display),"Messages::Display",'c',"b",'0'},
        {0x43CEC0,A(&CDarkel::DrawMessages),"Darkel::DrawMessages",'c',"",'0'},
        {0x447790,A(&CGarages::PrintMessages),"Garages::PrintMessages",'c',"",'0'},
        {0x71A210,A(&CFont::DrawFonts),"Font::DrawFonts",'c',"",'0'},
        {0,0,"RwRenderStateSet",'c',"ii",'1'},
    };
    bool passed=BuildThunks()&&ResolveStubs().empty();
    if(!passed){Stubs()=backup;thunks=priorThunks;hook->State(active);hook->LockState(lock);return false;}
    const auto drawBody=Body(A(static_cast<void(*)(const CRect&,const CRGBA&)>(&CSprite2d::DrawRect)));
    const auto playerBody=Body(A(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)));
    const bool wide=TheCamera.m_bWideScreenOn,menuWide=FrontEndMenuManager.m_bWidescreenOn;
    const auto mode=CCamera::GetActiveCamera().m_nMode;const int32 width=RsGlobal.maximumWidth,height=RsGlobal.maximumHeight;
    auto& fp=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet;const auto oldFp=fp;
    auto restore=[&]{TheCamera.m_bWideScreenOn=wide;FrontEndMenuManager.m_bWidescreenOn=menuWide;
        CCamera::GetActiveCamera().m_nMode=mode;RsGlobal.maximumWidth=width;RsGlobal.maximumHeight=height;fp=oldFp;};
    for(int scenario=0;scenario<512;++scenario){
        auto invoke=[&](bool candidate){restore();
            TheCamera.m_bWideScreenOn=(scenario&1)!=0;FrontEndMenuManager.m_bWidescreenOn=(scenario&2)!=0;
            static const eCamMode modes[]{MODE_SNIPER,MODE_SNIPER_RUNABOUT,MODE_ROCKETLAUNCHER,MODE_ROCKETLAUNCHER_RUNABOUT,MODE_CAMERA,MODE_HELICANNON_1STPERSON,MODE_FOLLOWPED};
            CCamera::GetActiveCamera().m_nMode=modes[(scenario/4)%7];
            RsGlobal.maximumWidth=640+(scenario%31)*17;RsGlobal.maximumHeight=480+(scenario%23)*13;
            std::memset(fakePed.data(),0xCD,fakePed.size());const int slot=scenario%13;fakePed[0x718]=uint8(slot);
            const int weapons[]{19,34,0,22};*reinterpret_cast<int32*>(fakePed.data()+0x5A0+slot*0x1C)=weapons[(scenario/7)%4];
            hook->State(candidate);FlushInstructionCache(GetCurrentProcess(),nullptr,0);
            const bool route=ReversingProbe::JumpsToDll(0x53E230)==candidate;
            nrec=0;overflow=false;
            {std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
                for(size_t i=0;i<Stubs().size();++i){const auto& s=Stubs()[i];if(s.exe)patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(s.exe,Thunk(i)));if(s.dll)patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(s.dll,Thunk(i)));}
                patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(0x727B60,&DrawRect));
                patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(drawBody,&DrawRect));
                patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(0x56E210,&Player));
                patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(playerBody,&Player));
                fp=reinterpret_cast<RwRenderStateSetFunction>(Thunk(Stubs().size()-1));
                reinterpret_cast<void(__cdecl*)()>(0x53E230)();
                fp=oldFp;while(!patches.empty())patches.pop_back();}
            const json out={{"trace",RenderTrace()},{"route_ok",route},{"overflow",overflow}};restore();return out;};
        const auto original=invoke(false),candidate=invoke(true);
        const bool ok=original==candidate&&original["route_ok"]==true&&!overflow;passed&=ok;
        Write({{"event","comparison"},{"function","Game::Render2dStuff"},{"case",scenario},{"pair",2048+scenario},
            {"original",original},{"candidate",candidate},{"equal",ok},{"accepted",ok}});
    }
    restore();hook->State(active);hook->LockState(lock);
    Write({{"event","function_summary"},{"function","Game::Render2dStuff"},{"cases",512},{"failures",passed?0:1}});
    VirtualFree(thunks,0,MEM_RELEASE);Stubs()=backup;thunks=priorThunks;
    return passed;
}
}
