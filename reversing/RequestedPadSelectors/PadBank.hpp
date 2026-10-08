#pragma once
#include <Pad.h>
#include <Timer.h>
#include <fstream>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedPadSelectorsLive {
static_assert(sizeof(CPad)==0x134 && sizeof(CControllerState)==0x30);
static_assert(std::is_trivially_copyable_v<CPad>);
using Simple=ReversibleHooks::ReversibleHook::Simple;
struct Range {void* ptr;std::vector<uint8_t> data;};
struct Fixture {CPad pad;uint32_t clock;int16_t history;bool melee;};
inline void SetField(CControllerState& state,int field,int16_t value){std::memcpy(reinterpret_cast<uint8_t*>(&state)+field*sizeof(int16_t),&value,sizeof(value));}
inline std::vector<Fixture> Fixtures() {
    std::vector<Fixture> out;
    auto add=[&](CPad pad,uint32_t clock=0u,int16_t history=0,bool melee=false){out.push_back({pad,clock,history,melee});};
    auto blank=[](){auto pad=CPad::Pads[0];std::memset(&pad,0,sizeof(pad));return pad;};
    constexpr int16_t axes[]{-128,-36,-35,-16,-15,-6,-5,-1,0,1,5,6,15,16,35,36,127,128};
    // Full pressure range, all four valid modes and both declared melee arguments.
    for(int mode=0;mode<4;++mode)for(int pressure=0;pressure<256;++pressure)for(bool melee:{false,true}) {
        auto pad=blank();pad.Mode=mode;
        for(int field=4;field<24;++field){SetField(pad.NewState,field,pressure);SetField(pad.OldState,field,(pressure+127)%256);}
        pad.NewState.LeftStickX=axes[pressure%18];pad.NewState.LeftStickY=axes[(pressure+4)%18];
        pad.NewState.RightStickX=axes[(pressure+9)%18];pad.NewState.RightStickY=axes[(pressure+13)%18];
        pad.OldState.RightStickY=axes[(pressure+2)%18];pad.LastTimeTouched=0xfffffff0u;
        for(int i=0;i<10;++i)pad.SteeringLeftRightBuffer[i]=axes[(pressure+i)%18];pad.DrunkDrivingBufferUsed=pressure%10;
        add(pad,uint32_t(pressure),axes[(pressure+5)%18],melee);
    }
    // One-hot fields and all four press/release/held/idle transitions; prevents mutually masked buttons.
    for(int mode=0;mode<4;++mode)for(int field=4;field<24;++field)for(int transition=0;transition<4;++transition)for(bool disabled:{false,true}) {
        auto pad=blank();pad.Mode=mode;pad.DisablePlayerControls=disabled?0x100:0;
        SetField(pad.NewState,field,(transition&1)?255:0);
        SetField(pad.OldState,field,(transition&2)?255:0);
        add(pad,0u,0,false);add(pad,0u,0,true);
    }
    // Individual disable flags against all buttons pressed, and every control-mask bit.
    for(int mode=0;mode<4;++mode)for(int bit=0;bit<24;++bit){
        auto pad=blank();pad.Mode=mode;
        for(int field=4;field<24;++field)SetField(pad.NewState,field,255);
        if(bit<16)pad.DisablePlayerControls=uint16_t(1u<<bit);
        else switch(bit){case 16:pad.bDisablePlayerEnterCar=true;break;case 17:pad.bDisablePlayerDuck=true;break;case 18:pad.bDisablePlayerFireWeapon=true;break;case 19:pad.bDisablePlayerFireWeaponWithL1=true;break;case 20:pad.bDisablePlayerCycleWeapon=true;break;case 21:pad.bDisablePlayerJump=true;break;case 22:pad.bDisablePlayerDisplayVitalStats=true;break;case 23:pad.bApplyBrakes=true;break;}
        add(pad,0xffffffffu,16,true);
    }
    // Cartesian threshold/current-history domain for analogue directions, independent of buttons.
    for(int16_t now:axes)for(int16_t previous:axes){auto pad=blank();pad.NewState.LeftStickX=pad.NewState.LeftStickY=now;add(pad,0,previous);}
    // ABI stress outside ordinary +/-128 input, only integer selectors in this phase.
    constexpr int16_t wide[]{-32768,-32767,-129,129,255,256,32766,32767};
    for(int mode=0;mode<4;++mode)for(int axis=0;axis<4;++axis)for(int16_t value:wide){auto pad=blank();pad.Mode=mode;SetField(pad.NewState,axis,value);SetField(pad.OldState,axis,value);add(pad,0xfffffff0u,value);}
    return out;
}
inline json Globals() {
    return {{"pads",ReversingProbe::Bytes(&CPad::Pads,sizeof(CPad::Pads))},{"histories",ReversingProbe::Bytes(reinterpret_cast<void*>(0xB736F0),14)},{"clock",CTimer::m_snTimeInMilliseconds}};
}
template<class R> inline uint32_t ReturnBits(R value) {
    if constexpr(std::is_same_v<R,bool>)return value?1u:0u;
    else if constexpr(sizeof(R)==1)return uint8_t(value);
    else if constexpr(sizeof(R)==2)return uint16_t(value);
    else return uint32_t(value);
}
template<class R,class... Args> inline R Call(void* address,CPad* pad,Args...args){return reinterpret_cast<R(__thiscall*)(CPad*,Args...)>(address)(pad,args...);}
inline Simple* Find(uintptr_t address){Simple* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){for(auto& h:cat.Items())if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple){auto* s=static_cast<Simple*>(h.get());if(uintptr_t(s->GetHookGTAAddress())==address)result=s;}});return result;}
template<class R> inline void Compare(std::ofstream& log,const char* name,uintptr_t address,bool isStatic,bool hasMelee,const std::vector<Fixture>& fixtures){
    auto* hook=Find(address);if(!hook){log<<json({{"event","error"},{"function",name},{"reason","missing exact hook"}}).dump()<<'\n';return;}
    const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
    for(size_t seed=0;seed<fixtures.size();++seed){json outcomes=json::array();bool switched=true;
        for(bool candidate:{false,true}){
            auto pad=fixtures[seed].pad;CPad::Pads[0]=pad;CTimer::m_snTimeInMilliseconds=fixtures[seed].clock;
            for(uintptr_t p:{0xB736F0u,0xB736F4u,0xB736F8u,0xB736FCu})*reinterpret_cast<int16_t*>(p)=fixtures[seed].history;
            hook->State(candidate);switched&=SmallPendingSupport::ExactSwitch(*hook,candidate);
            void* target=candidate?hook->GetHookOurAddress():hook->GetHookGTAAddress();R value{};
            if(isStatic)value=reinterpret_cast<R(__cdecl*)()>(target)();
            else if(hasMelee)value=Call<R,bool>(target,&pad,fixtures[seed].melee);
            else value=Call<R>(target,&pad);
            outcomes.push_back({{"return_bits",ReturnBits(value)},{"pad",ReversingProbe::Bytes(&pad,sizeof(pad))},{"globals",Globals()}});
        }
        json row={{"event","comparison"},{"function",name},{"address",address},{"seed",seed},{"switch_exact",switched},{"equal",outcomes[0]==outcomes[1]},{"original_bits",outcomes[0]["return_bits"]},{"candidate_bits",outcomes[1]["return_bits"]},{"pad_equal",outcomes[0]["pad"]==outcomes[1]["pad"]},{"globals_equal",outcomes[0]["globals"]==outcomes[1]["globals"]}};
        if(outcomes[0]!=outcomes[1]){row["original"]=outcomes[0];row["candidate"]=outcomes[1];}
        log<<row.dump()<<'\n';
        if(seed%128==127)log.flush();
    }
    hook->State(enabled);hook->LockState(locked);
}
inline void Update();
}
