from pathlib import Path
p=Path('reversing/ScriptLoaded355');p.mkdir(exist_ok=True)
s=Path('reversing/ScriptLinks355/LiveComparison.hpp').read_text().replace('ScriptLinks355Live','ScriptLoaded355Live').replace('script-links355','script-loaded355')
a=s.index('inline ReversibleHooks');s=s[:a]+'''inline std::array<int8,65536> volumes;alignas(16) inline std::array<uint8,sizeof(CAESound)> requested;inline void* activeSelf{};
inline void __fastcall Init(CAESound* sound,void*,eSoundBankSlot bank,eSoundID id,CAEAudioEntity* owner,CVector pos,float volume,float rolloff,float speed,float doppler,uint8 delay,uint16 flags,float variance,int16 time){trace.push_back({"initialise",int(bank),int(id),owner==activeSelf,std::bit_cast<uint32>(pos.x),std::bit_cast<uint32>(pos.y),std::bit_cast<uint32>(pos.z),std::bit_cast<uint32>(volume),std::bit_cast<uint32>(rolloff),std::bit_cast<uint32>(speed),std::bit_cast<uint32>(doppler),delay,flags,std::bit_cast<uint32>(variance),time});memset(sound,0,sizeof(*sound));sound->m_BankSlot=bank;sound->m_SoundID=id;sound->m_AudioEntity=owner;sound->m_CurrPos=pos;sound->m_Volume=volume;}
inline void __fastcall Flags(CAESound* sound,void*,uint16 flag,uint16 enabled){trace.push_back({"flags",flag,enabled,sound->m_Flags});if(enabled)sound->m_Flags|=flag;else sound->m_Flags&=uint16(~flag);}
inline CAESound* __fastcall Request(void*,void*,CAESound* sound){trace.push_back({"request",int(sound->m_BankSlot),int(sound->m_SoundID),std::bit_cast<uint32>(sound->m_Volume),std::bit_cast<uint32>(sound->m_CurrPos.x),std::bit_cast<uint32>(sound->m_CurrPos.y),std::bit_cast<uint32>(sound->m_CurrPos.z),sound->m_Flags});return reinterpret_cast<CAESound*>(requested.data());}
inline void __fastcall Destructor(void*,void*){trace.push_back({"destructor"});}
'''+s[a:]
a=s.index('inline const std::array<Fn,6>');b=s.index('inline void Update()',a);s=s[:a]+'inline const std::array<Fn,1> fns{{{"CAEScriptAudioEntity","PlayLoadedMissionAudio",0x4EC270,0,-1,1024}}};\n'+s[b:]
s=s.replace('Base*,6>','Base*,1>').replace('bool,6>','bool,1>').replace('n<6','n<1')
s=s.replace(' uint32 pairs=0,totalFailures=0;', ' const auto oldVolumes=StaticRef<int8*>(0xBD00F8);uint32 pairs=0,totalFailures=0;')
a=s.index(' EntryPatch a(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch d(0x4D8F00,Sound),dd(bit_cast<uintptr_t>(&CAEAudioHardware::GetSoundLoadingStatus),Sound);
 EntryPatch i(0x4EFE50,Init),id(bit_cast<uintptr_t>(&CAESound::Initialise),Init);
 EntryPatch f(0x4EF2B0,Flags),fd(bit_cast<uintptr_t>(&CAESound::SetFlags),Flags);
 EntryPatch r(0x4EFB10,Request),rd(bit_cast<uintptr_t>(&CAESoundManager::RequestNewSound),Request);
 EntryPatch q(0x4EF660,Destructor),qu(0x4EF1A0,Destructor),qd(bit_cast<uintptr_t>(&CAESound::UnregisterWithPhysicalEntity),Destructor);
'''+s[b:]
s=s.replace('auto* self=reinterpret_cast<CAEScriptAudioEntity*>(object.data());','auto* self=reinterpret_cast<CAEScriptAudioEntity*>(object.data());activeSelf=self;StaticRef<int8*>(0xBD00F8)=volumes.data();volumes.fill(int8(i%7==0?-128:i%9-5));')
s=s.replace('link.m_nAudioEvent=400+i%9;', 'link.m_nAudioEvent=i%11==0?65535:400+i%9;')
a=s.index('    switch(n)');b=s.index('    if(!candidate',a)
s=s[:a]+'''    reinterpret_cast<void(__thiscall*)(void*,uint8)>(fn.address)(self,slot);
'''+s[b:]
s=s.replace('if((n==1||n==2||n==4)&&!positiveCases)','if(!positiveCases)')
s=s.replace(' bool restored=true;', ' StaticRef<int8*>(0xBD00F8)=oldVolumes;bool restored=StaticRef<int8*>(0xBD00F8)==oldVolumes;')
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/ScriptLinks355',fn).read_text().replace('ScriptLinks355Live','ScriptLoaded355Live').replace('script-links355','script-loaded355')
 if fn=='build_probe.py':s=s.replace("'Audio/Hardware/AEAudioHardware.h'","'Audio/Hardware/AEAudioHardware.h','Audio/AESound.cpp','Audio/AESound.h','Audio/Entities/AEAudioEntity.cpp','Audio/Entities/AEAudioEntity.h','Core/Vector.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('PlayLoadedMissionAudio 0x4EC270: 1.024 pares preparados, pendiente ejecución. Ranuras válidas/inválidas; bank/sound inválidos y loading 0/1; evento 65535, volumen -128 y ordinary, entidad con/sin matriz, sentinela/cero/posición. Initialise, SetFlags, Request y destructor semánticos simétricos; flags iniciales 14 y protección 0x100/0x200/0x800. Compara enlaces completos y traza argumentos/posición/volumen/flags, sin observar padding indeterminado del CAESound temporal original. Restaura pointertabla de volumen y hooks. Evidencia ../VehicleAudio355Evidence/Script_4EC270.txt.\n')
