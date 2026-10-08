from pathlib import Path
p=Path('reversing/RadioVolumes355');p.mkdir(exist_ok=True)
s=Path('reversing/ScriptUpdate355/LiveComparison.hpp').read_text().replace('ScriptUpdate355Live','RadioVolumes355Live').replace('script-update355','radio-volumes355')
a=s.index('#include "AEScript');b=s.index('#include "../ProbeRuntime',a);s=s[:a]+'''#include "AERadioTrackManager.h"
#include "AEAudioHardware.h"
#include "AEAudioEnvironment.h"
#include "AudioEngine.h"
'''+s[b:]
a=s.index('alignas(16) inline');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''inline CVector position;
inline int factorCalls{};
inline float Factor(bool effects){static constexpr float values[]{0.f,.1f,.5f,1.f,2.f,10.f,std::bit_cast<float>(0x7FC00000u)};const auto r=values[(scenario/5+(effects?0:1)+(factorCalls++/2)%3)%7];trace.push_back({effects?"effects":"music",std::bit_cast<uint32>(r)});return r;}
inline float __fastcall Effects(void*,void*){return Factor(true);}
inline float __fastcall Music(void*,void*){return Factor(false);}
inline bool __cdecl Cutscene(){const auto r=scenario%13==0;trace.push_back({"cutscene",r});return r;}
inline bool __fastcall Finished(void*,void*,uint8 slot){const auto r=(scenario+slot)%4==0;trace.push_back({"finished",slot,r});return r;}
inline int32 __fastcall Event(void*,void*,uint8 slot){const auto r=(scenario+slot)%5==0?65535:104;trace.push_back({"event",slot,r});return r;}
inline CVector* __fastcall Position(void*,void*,uint8 slot){const auto present=(scenario+slot)%3!=0;trace.push_back({"position",slot,present});return present?&position:nullptr;}
inline void __cdecl Relative(CVector* out,const CVector* source){static constexpr float values[]{0.f,14.9f,15.f,15.1f,30.f,std::bit_cast<float>(0x7FC00000u)};*out=CVector{values[(scenario/9)%6],0.f,0.f};trace.push_back({"relative",source==&position,std::bit_cast<uint32>(out->x)});}
inline bool __cdecl Ambience(){const auto r=scenario%3==0;trace.push_back({"ambience",r});return r;}
inline void __fastcall Channel(void*,void*,int16 channel,uint16 id,float volume,uint8 unused){trace.push_back({"channel",channel,id,std::bit_cast<uint32>(volume),unused});}
inline constexpr std::array<uintptr_t,4> globalAddresses{0xB7CB49,0xB7CB48,0xB6F065,0xB613E4};
'''+s[b:]
s=s.replace('{"CAEScriptAudioEntity","UpdateParameters",0,0x862E58,0,1024}', '{"CAERadioTrackManager","UpdateRadioVolumes",0x4EA010,0,-1,1024}')
s=s.replace('const auto oldVolumes=StaticRef<int8*>(0xBD00F8);const auto oldTime=StaticRef<uint32>(0xB7CB84);', 'std::array<uint8,4> saved{};for(size_t g=0;g<4;++g)saved[g]=StaticRef<uint8>(globalAddresses[g]);')
a=s.index(' EntryPatch a(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch a(0x4D9570,Effects),ad(bit_cast<uintptr_t>(&CAEAudioHardware::GetEffectsMasterScalingFactor),Effects);
 EntryPatch b(0x4D9580,Music),bd(bit_cast<uintptr_t>(&CAEAudioHardware::GetMusicMasterScalingFactor),Music);
 EntryPatch c(0x561AD0,Cutscene),cd(bit_cast<uintptr_t>(&CCutsceneMgr::IsRunning),Cutscene);
 EntryPatch d(0x5072C0,Finished),dd(bit_cast<uintptr_t>(&CAudioEngine::IsMissionAudioSampleFinished),Finished);
 EntryPatch e(0x5072D0,Event),ed(bit_cast<uintptr_t>(&CAudioEngine::GetMissionAudioEvent),Event);
 EntryPatch f(0x5072E0,Position),fd(bit_cast<uintptr_t>(&CAudioEngine::GetMissionAudioPosition),Position);
 EntryPatch g(0x4D80B0,Relative),gd(bit_cast<uintptr_t>(&CAEAudioEnvironment::GetPositionRelativeToCamera_Vector),Relative);
 EntryPatch h(0x507280,Ambience),hd(bit_cast<uintptr_t>(&CAudioEngine::IsAmbienceRadioActive),Ambience);
 EntryPatch j(0x4D8870,Channel),jd(bit_cast<uintptr_t>(&CAEAudioHardware::SetChannelVolume),Channel);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original=',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CAERadioTrackManager)> object{};
   auto run=[&](bool candidate){scenario=i;factorCalls=0;trace=json::array();memset(object.data(),0xCD,object.size());auto* self=reinterpret_cast<CAERadioTrackManager*>(object.data());
    for(size_t g=0;g<4;++g)StaticRef<uint8>(globalAddresses[g])=uint8((i/(1<<g))%2);
    self->m_bEnabledInPauseMode=(i/16)%2!=0;self->m_bBassEnhance=(i/32)%2!=0;self->m_ActiveSettings.BassSetting=static_cast<eBassSetting>((i/64)%5-1);self->m_HwClientHandle=i%17-8;
    static constexpr float levels[]{-50.f,-4.f,-.05f,0.f,1.f,std::bit_cast<float>(0x7FC00000u)};self->m_f80=levels[(i/3)%6];self->m_f84=levels[(i/18)%6];position=CVector{3.f,4.f,5.f};
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(self);
    if(!candidate&&!trace.empty())++positiveCases;
    std::array<uint8,4> globals{};for(size_t g=0;g<4;++g)globals[g]=StaticRef<uint8>(globalAddresses[g]);
    return json{{"switch",true},{"object",Hash(object.data(),object.size())},{"globals",globals},{"trace",trace}};
'''+s[b:]
s=s.replace('StaticRef<int8*>(0xBD00F8)=oldVolumes;StaticRef<uint32>(0xB7CB84)=oldTime;', 'for(size_t g=0;g<4;++g)StaticRef<uint8>(globalAddresses[g])=saved[g];')
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/RadioQueue355',fn).read_text().replace('radio-queue355','radio-volumes355').replace('RadioQueue355Live','RadioVolumes355Live')
 if fn=='build_probe.py':s=s.replace("'Stats.h'", "'Stats.h','Audio/Hardware/AEAudioHardware.cpp','Audio/Hardware/AEAudioHardware.h','Audio/AEAudioEnvironment.cpp','Audio/AEAudioEnvironment.h','Audio/AudioEngine.cpp','Audio/AudioEngine.h','Core/Vector.cpp','Core/Vector.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('UpdateRadioVolumes 0x4EA010: 1024 pares pendientes. Cutscene/pausa/camera/speech globals aislados; hardware master getters secuencia mutable y trazada para exigir orden y repetición; eventos/posición/distancia15 exacta/NaN; fade m_f80/84 negativo/cero/NaN; bass-1..3; channel16 truncation. Getters/engine/relative/channel stubs simétricos, vector Magnitude real pura. ASM log10 fldlg2/fyl2x en draft Radio_VolumesNative.txt; nueva fuente todavía no aplicada. Cuatro globalsbyte restaurados.\n')
print('RadioVolumes355 prepared')
