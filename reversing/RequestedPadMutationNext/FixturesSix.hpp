#pragma once
#include <Pad.h>
#include <KeyboardState.h>
#include <MouseControllerState.h>
namespace RequestedPadMutationSix {
static_assert(sizeof(CPad)==0x134 && sizeof(CKeyboardState)==0x270 && sizeof(CMouseControllerState)==0x14);
static_assert(offsetof(CPad,LastTimeTouched)==0x120 && offsetof(CPad,Phase)==0x108 && offsetof(CPad,Mode)==0x10A);
static_assert(offsetof(CKeyboardState,enter)==0x23C);
static_assert(offsetof(CMouseControllerState,m_fWheelMoved)==8 && offsetof(CMouseControllerState,m_AmountMoved)==12);
inline constexpr uint32_t clocks[]{0,1,127,65535,0x7FFFFFFF,0x80000000,0xFFFFFFFE,0xFFFFFFFF};
inline constexpr int16_t stopArguments[]{-32768,-128,-1,0,1,127,255,32767};
struct GlobalRange {uintptr_t address;size_t size;const char* name;};
inline constexpr GlobalRange globalRanges[]{
 {0xB72CA0,16,"before-keyboard-guard"},{0xB72CB0,0x750,"three-keyboards"},
 {0xB73400,4,"pad-number-and-look-bytes"},{0xB73404,0x3C,"three-mice"},
 {0xB73440,24,"mouse-pad-gap"},{0xB73458,0x268,"two-global-pads"},
 {0xB736C0,16,"after-pads-guard"},{0xB7CB7C,16,"timer-and-clock-neighbours"}
};
inline std::vector<uint8_t> Pattern(size_t size,uint32_t seed,size_t role){
 std::vector<uint8_t> result(size);
 for(size_t i=0;i<size;++i)result[i]=seed==0?0:seed==1?0xFF:seed==2?0xA5:seed==3?0x5A:uint8_t(seed*73+i*29+(i>>3)+role*47);
 return result;
}
inline void CanonicalPadBools(std::vector<uint8_t>& bytes,size_t start=0){for(size_t offset=0x118;offset<=0x11F;++offset)bytes[start+offset]&=1;}
struct Fixture {
 std::vector<uint8_t> pad;
 std::vector<std::vector<uint8_t>> globals;
 uint32_t clock{};int16_t stopArgument{};bool clearDisabled{},resetPhase{};
 Fixture(uint32_t poison,uint32_t clockIndex=0,uint32_t argumentIndex=0):pad(Pattern(sizeof(CPad),poison,0)),clock(clocks[clockIndex]),stopArgument(stopArguments[argumentIndex]){
  CanonicalPadBools(pad);
  for(size_t role=0;role<std::size(globalRanges);++role){auto bytes=Pattern(globalRanges[role].size,poison,role+1);
   if(role==3)for(size_t mouse=0;mouse<3;++mouse){bytes[mouse*0x14+3]&=1;bytes[mouse*0x14+4]&=1;}
   if(role==5){CanonicalPadBools(bytes);CanonicalPadBools(bytes,0x134);}
   if(role==7)std::memcpy(bytes.data()+8,&clock,sizeof(clock));
   globals.push_back(std::move(bytes));
  }
 }
};
inline constexpr size_t Counts[]{2048,2048,256,256,1024,256};
inline Fixture Case(size_t target,size_t seed){
 assert(target<6&&seed<Counts[target]);
 if(target==0)return Fixture(uint32_t(seed/8),0,uint32_t(seed%8));
 if(target==1)return Fixture(uint32_t(seed/8),uint32_t(seed%8));
 if(target==4){Fixture fixture(uint32_t(seed/4));fixture.clearDisabled=(seed&1)!=0;fixture.resetPhase=(seed&2)!=0;return fixture;}
 return Fixture(uint32_t(seed));
}
}
