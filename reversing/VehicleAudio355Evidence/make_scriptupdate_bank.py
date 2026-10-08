from pathlib import Path
p=Path('reversing/ScriptUpdate355');p.mkdir(exist_ok=True)
s=Path('reversing/ScriptLinks355/LiveComparison.hpp').read_text().replace('ScriptLinks355Live','ScriptUpdate355Live').replace('script-links355','script-update355').replace('#include "AEAudioHardware.h"','#include "AEAudioHardware.h"\n#include "AEAudioUtility.h"')
a=s.index('inline void __fastcall Cancel');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''inline std::array<int8,65536> volumes;
alignas(16) inline std::array<uint8,sizeof(CAESound)> soundBuffer;
inline void* activeSelf{};
inline void __fastcall Stop(CAESound* sound,void*){trace.push_back({"stop",sound==reinterpret_cast<CAESound*>(soundBuffer.data())});sound->m_Speed=-.25f;}
inline void __fastcall Position(CAESound* sound,void*,CVector position){trace.push_back({"position",std::bit_cast<uint32>(position.x),std::bit_cast<uint32>(position.y),std::bit_cast<uint32>(position.z)});sound->m_CurrPos=position;}
inline void __fastcall Resident(void* owner,void*,eSoundBankSlot slot,eSoundBank bank,eSoundID id,eAudioEvents event,CVector& position,CPhysical* physicalEntity,float volume,float speed,int16 time,float distance){trace.push_back({"resident",owner==activeSelf,int(slot),int(bank),int(id),int(event),physicalEntity==reinterpret_cast<CPhysical*>(physical.data()),std::bit_cast<uint32>(position.x),std::bit_cast<uint32>(position.y),std::bit_cast<uint32>(position.z),std::bit_cast<uint32>(volume),std::bit_cast<uint32>(speed),time,std::bit_cast<uint32>(distance)});}
inline void __fastcall BankSound(void* owner,void*,eAudioEvents event,CVector& position,CPhysical* physicalEntity,int16 id,uint8 slot,uint8 only,float volume,float distance,float speed){trace.push_back({"banksound",owner==activeSelf,int(event),std::bit_cast<uint32>(position.x),std::bit_cast<uint32>(position.y),std::bit_cast<uint32>(position.z),physicalEntity==nullptr,id,slot,only,std::bit_cast<uint32>(volume),std::bit_cast<uint32>(distance),std::bit_cast<uint32>(speed)});}
inline int32 __cdecl Random(int32 lo,int32 hi){const auto r=lo+scenario%(hi-lo+1);trace.push_back({"rng",lo,hi,r});return r;}
'''+s[b:]
a=s.index('inline const std::array<Fn,6>');b=s.index('inline void Update()',a);s=s[:a]+'inline const std::array<Fn,1> fns{{{"CAEScriptAudioEntity","UpdateParameters",0,0x862E58,0,1024}}};\n'+s[b:]
s=s.replace('Base*,6>','Base*,1>').replace('bool,6>','bool,1>').replace('n<6','n<1')
s=s.replace('uint32 pairs=0,totalFailures=0;', 'uint32 pairs=0,totalFailures=0;const auto oldVolumes=StaticRef<int8*>(0xBD00F8);const auto oldTime=StaticRef<uint32>(0xB7CB84);')
a=s.index(' EntryPatch a(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch a(0x4EF850,Stop),ad(bit_cast<uintptr_t>(&CAESound::StopSoundAndForget),Stop);
 EntryPatch b(0x4EF880,Position),bd(bit_cast<uintptr_t>(&CAESound::SetPosition),Position);
 EntryPatch c(0x4EC550,Resident),cd(bit_cast<uintptr_t>(&CAEScriptAudioEntity::PlayResidentSoundEvent),Resident);
 EntryPatch d(0x4EC6D0,BankSound),dd(bit_cast<uintptr_t>(&CAEScriptAudioEntity::PlayMissionBankSound),BankSound);
 EntryPatch e(0x4D9C10,Random),ed(bit_cast<uintptr_t>(&CAEAudioUtility::GetRandomNumberInRange<int32>),Random);
'''+s[b:]
a=s.index('    for(size_t w=0;');b=s.index('    if(!candidate',a)
s=s[:a]+'''    activeSelf=self;memset(soundBuffer.data(),0xCD,soundBuffer.size());auto* sound=reinterpret_cast<CAESound*>(soundBuffer.data());
    static constexpr int events[]{104,1086,1113,1118,1042,1167,1177,1147,900};sound->m_Event=events[i%9];sound->m_AudioEntity=reinterpret_cast<CAEAudioEntity*>(physical.data());sound->m_CurrPos=CVector{3.f,4.f,5.f};
    static constexpr float levels[]{-50.f,-40.f,-20.f,-18.f,-.05f,0.f,.05f,1.f,std::bit_cast<float>(0x7FC00000u)};sound->m_Volume=levels[(i/9)%9];sound->m_Speed=levels[(i/81)%9];
    for(size_t w=0;w<4;++w){auto& link=self->wavLinks[w];link.m_pEntity=(i/13)%2?reinterpret_cast<CPhysical*>(physical.data()):nullptr;link.m_Sound=(i/4)%6==int(w)?sound:nullptr;}
    self->m_nLastTimeHornPlayed=(i/3)%2?1000u:0xFFFFFF00u;self->field_7C=uint8((i/7)%2);self->field_8C=std::array<float,4>{0.f,1.f,2.f,std::bit_cast<float>(0x7FC00000u)}[(i/11)%4];self->m_Volume=-7.f;self->m_Speed=.75f;
    for(size_t v=0;v<volumes.size();++v)volumes[v]=int8(int(v%41)-20);StaticRef<int8*>(0xBD00F8)=volumes.data();
    static constexpr uint32 offsets[]{0,299,300,301,499,500,501,4500,4501,4800,4801,5000};StaticRef<uint32>(0xB7CB84)=self->m_nLastTimeHornPlayed+offsets[(i/5)%12];
    const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];hooks[n]->State(candidate);const auto entry=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((entry!=0x4EC970)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(void*,CAESound*,int16)>(entry)(self,(i/17)%8==0?nullptr:sound,(i/19)%3==0?-1:2);
'''+s[b:]
s=s.replace('const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];hooks','hooks')
s=s.replace('{"return",ret},','').replace('{"object",Hash(object.data(),object.size())}', '{"object",Hash(object.data(),object.size())},{"sound",Hash(soundBuffer.data(),soundBuffer.size())},{"volume_table",Hash(volumes.data(),volumes.size())},{"time",StaticRef<uint32>(0xB7CB84)}')
s=s.replace('if((n==1||n==2||n==4)&&!positiveCases)', 'if(!positiveCases)')
s=s.replace(' bool restored=true;', ' StaticRef<int8*>(0xBD00F8)=oldVolumes;StaticRef<uint32>(0xB7CB84)=oldTime;bool restored=true;')
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/ScriptLoaded355',fn).read_text().replace('script-loaded355','script-update355').replace('ScriptLoaded355Live','ScriptUpdate355Live')
 if fn=='build_probe.py':s=s.replace("'Core/Vector.h'", "'Core/Vector.h','Audio/AEAudioUtility.cpp','Audio/AEAudioUtility.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('UpdateParameters 0x4EC970 VMT862E58 slot0: 1024 pares preparados pendientes ejecución. Links cero/uno coincidente, fin -1, entidades matrix/placeable, eventos104/1086/1113/1118/1042/1167/1177/1147/other, reloj wrap y límites300/500/4500/4800, volumen/velocidad NaN, field8C bits0/1/2/NaN. El switch se ejecuta por CADA link no coincidente: llamadas repetidas preservadas. Stubs Stop con efecto en velocidad, SetPosition, Resident/BankSound, RNG simétricos; tabla volumen y reloj restaurados. Evidencia combo Script_4EC970.txt, cuerpo draft Script_UpdateNative.txt. Source todavía no aplicado durante cola; requiere aplicación antes del build.\n')
print('ScriptUpdate355 prepared')
