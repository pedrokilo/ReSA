#pragma once
#include <Pad.h>
#include <MenuManager.h>
#include <CutsceneMgr.h>
#include <Timer.h>
namespace RequestedPadRemaining3Hex {
static_assert(offsetof(CMenuManager,m_PrefsUseVibration)==0x20);
static_assert(sizeof(CPad)==0x134&&sizeof(CControllerState)==0x30);
static_assert(offsetof(CPad,NoShakeBeforeThis)==0x12C&&offsetof(CPad,NoShakeFreq)==0x130);
inline constexpr size_t counts[]{2892,2560,89600};
inline constexpr uint32_t clocks[]{0,1,127,65535,0x7FFFFFFF,0x80000000,0xFFFFFFFE,0xFFFFFFFF};
inline constexpr int16_t axes[]{-32768,-128,-1,0,1,128,32767};
inline constexpr int16_t pressures[]{0,1,128,255};
inline constexpr uint8_t frequencies[]{0,1,127,128,255};
inline constexpr uint32_t delays[]{0,1,100,0xFFFFFFFF};
inline constexpr std::pair<int16_t,int16_t> durations[]{{0,100},{100,100},{200,100},{-1,100},{100,-1},{-32768,32767},{32767,-32768}};
inline CPad Blank(size_t seed){auto pad=CPad::Pads[0];auto* bytes=reinterpret_cast<uint8_t*>(&pad);for(size_t i=0;i<sizeof(pad);++i)bytes[i]=uint8_t(seed*73+i*29+(i>>3));for(size_t i=0x118;i<=0x11F;++i)bytes[i]&=1;return pad;}
inline void Field(CControllerState& state,size_t field,int16_t value){assert(field<24);std::memcpy(reinterpret_cast<uint8_t*>(&state)+field*2,&value,2);}
inline CControllerState Controller(size_t seed){CControllerState state{};constexpr int16_t normalAxes[]{-128,-1,0,1,128};for(size_t i=0;i<24;++i)Field(state,i,i<4?normalAxes[(seed+i)%5]:pressures[(seed*3+i)%4]);return state;}
struct Fixture {
 CPad pad;CControllerState a{},b{},out{};size_t alias{};uint32_t clock{},deadline{},delay{};int16_t duration{};uint8_t frequency{},storedFrequency{},gates{};int32_t padId{};const char* domain="normal";
 explicit Fixture(size_t seed):pad(Blank(seed)),clock(clocks[seed%8]){out=Controller(seed+7);}
};
inline Fixture Reconcile(size_t seed){assert(seed<2892);Fixture f(seed);f.alias=seed%3;size_t n=seed/3;
 if(n<196){const size_t field=n/49,pair=n%49;Field(f.a,field,axes[pair/7]);Field(f.b,field,axes[pair%7]);if(pair/7==0||pair/7==6||pair%7==0||pair%7==6)f.domain="signed-short-stress";}
 else if((n-=196)<320){const size_t field=4+n/16,pair=n%16;Field(f.a,field,pressures[pair/4]);Field(f.b,field,pressures[pair%4]);}
 else if((n-=320)<192){constexpr int16_t conflictAxes[]{-128,-36,-1,1,36,128};const size_t field=n/96,value=(n/16)%6,mask=n%16;Field(f.a,field,conflictAxes[value]);for(size_t i=0;i<4;++i)Field(f.a,8+i,(mask&(1<<i))?255:0);}
 else{n-=192;assert(n<256);f.a=Controller(n);f.b=Controller(n*7+3);}
 return f;
}
inline Fixture UpdateCase(size_t seed){assert(seed<2560);Fixture f(seed);const size_t input=seed%16,just=(seed/16)%4,horn=(seed/64)%5,disabled=(seed/320)%2,mode=seed/640;
 f.pad.NewState=Controller(seed+1);f.pad.OldState=Controller(seed+2);f.pad.PCTempKeyState={};f.pad.PCTempJoyState={};f.pad.PCTempMouseState={};
 if(input){auto fill=[&](CControllerState& state,size_t role){Field(state,(input*3+role*7)%24,input<8?int16_t(128):int16_t(255));if(input>=12)Field(state,role%4,role==1?-128:128);};fill(f.pad.PCTempKeyState,0);fill(f.pad.PCTempJoyState,1);fill(f.pad.PCTempMouseState,2);}
 f.pad.Mode=int16_t(mode);f.pad.DisablePlayerControls=disabled?0x100:0;f.pad.iCurrHornHistory=char(horn);constexpr uint8_t justValues[]{0,1,127,255};f.pad.JustOutOfFrontEnd=char(justValues[just]);if(just==3)f.domain="byte-counter-stress";
 f.pad.DrunkDrivingBufferUsed=int32_t(seed%10);for(size_t i=0;i<10;++i)f.pad.SteeringLeftRightBuffer[i]=int16_t(i*101+seed);f.padId=int32_t(seed%2);return f;
}
inline Fixture Shake(size_t seed){assert(seed<89600);Fixture f(seed);size_t n=seed;f.delay=delays[n%4];n/=4;const size_t deadlineCase=n%4;n/=4;f.clock=clocks[n%8];n/=8;f.storedFrequency=frequencies[n%5];n/=5;f.frequency=frequencies[n%5];n/=5;const size_t durationCase=n%7;n/=7;f.gates=uint8_t(n);assert(n<4);
 const auto [oldDuration,requested]=durations[durationCase];f.pad.ShakeDur=oldDuration;f.duration=requested;f.deadline=deadlineCase==0?f.clock-1:deadlineCase==1?f.clock:deadlineCase==2?f.clock+1:0xFFFFFFFF;
 auto* bytes=reinterpret_cast<uint8_t*>(&f.pad);std::memcpy(bytes+0x12C,&f.deadline,4);bytes[0x130]=f.storedFrequency;if(durationCase>=3)f.domain="signed-duration-stress";return f;
}
inline Fixture Case(size_t bank,size_t seed){assert(bank<3&&seed<counts[bank]);return bank==0?Reconcile(seed):bank==1?UpdateCase(seed):Shake(seed);}
}
