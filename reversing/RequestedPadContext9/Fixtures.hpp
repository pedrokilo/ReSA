#pragma once
#include <Pad.h>
#include "ActorLayout.hpp"
namespace RequestedPadContext9 {
struct Fixture {CPad pad;ActorState actor;GunState gun;uint8_t flags;};
inline CPad Blank(){auto pad=CPad::Pads[0];std::memset(&pad,0,sizeof(pad));return pad;}
inline std::vector<Fixture> SpatialFixtures(){
    constexpr int16_t values[]{-128,-36,-35,-16,-15,-6,-5,-1,0,1,5,6,15,16,35,36,127,128};
    std::vector<std::pair<int16_t,int16_t>> pairs;
    for(int i=0;i<18;++i)pairs.emplace_back(values[i],values[(i+5)%18]);
    for(int16_t value:{-128,-36,-35,-1,0,1,35,36,128})pairs.emplace_back(value,-value);
    std::vector<Fixture> out;
    for(int actor=0;actor<7;++actor)for(int flags=0;flags<16;++flags)for(bool disabled:{false,true})for(size_t pair=0;pair<pairs.size();++pair){
        auto pad=Blank();pad.Mode=pair%4;pad.DisablePlayerControls=disabled?0x100:0;
        pad.NewState.LeftStickX=pad.NewState.LeftStickY=pairs[pair].first;
        pad.NewState.RightStickX=pad.NewState.RightStickY=pairs[pair].second;
        out.push_back({pad,ActorState(actor),GunState::NoTasks,uint8_t(flags)});
    }
    return out;
}
inline std::vector<Fixture> GunFixtures(){
    std::vector<Fixture> out;
    for(int mode=0;mode<4;++mode)for(int actor=0;actor<3;++actor)for(int gun=0;gun<5;++gun)for(int gates=0;gates<8;++gates)for(bool disabled:{false,true})for(int16_t pressure:{0,1,128,255})for(int transition=0;transition<4;++transition){
        auto pad=Blank();pad.Mode=mode;pad.DisablePlayerControls=disabled?1:0;
        pad.bDisablePlayerFireWeapon=(gates&1)!=0;pad.bDisablePlayerFireWeaponWithL1=(gates&2)!=0;pad.bDisablePlayerDisplayVitalStats=(gates&4)!=0;
        const int16_t now=(transition&1)?pressure:0,old=(transition&2)?pressure:0;
        pad.NewState.LeftShoulder1=pad.NewState.ButtonCircle=pad.NewState.ButtonCross=pad.NewState.RightShoulder1=now;
        pad.OldState.LeftShoulder1=pad.OldState.ButtonCircle=pad.OldState.ButtonCross=pad.OldState.RightShoulder1=old;
        out.push_back({pad,ActorState(actor),GunState(gun),0});
    }
    // Isolate shoulder fire fallback instead of masking it with Circle priority.
    for(int mode=0;mode<4;++mode)for(int actor=0;actor<3;++actor)for(int gun=0;gun<5;++gun)for(bool l1Disabled:{false,true}){
        auto pad=Blank();pad.Mode=mode;pad.NewState.LeftShoulder1=255;pad.NewState.RightShoulder1=128;pad.NewState.ButtonCross=1;pad.bDisablePlayerFireWeaponWithL1=l1Disabled;
        out.push_back({pad,ActorState(actor),GunState(gun),0});
    }
    return out;
}
}
