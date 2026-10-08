from pathlib import Path
p=Path('reversing/ScriptBank355');p.mkdir(exist_ok=True)
s=Path('reversing/ScriptLoaded355/LiveComparison.hpp').read_text().replace('ScriptLoaded355Live','ScriptBank355Live').replace('script-loaded355','script-bank355')
s=s.replace('inline void __fastcall Destructor(void*,void*){trace.push_back({"destructor"});}', '''inline bool __fastcall Loaded(void*,void*,eSoundBank bank,eSoundBankSlot slot){const auto r=(scenario/8)%3!=0;trace.push_back({"loaded",int(bank),int(slot),r});return r;}
inline int16 __fastcall EventPlaying(void*,void*,int16 event,CAEAudioEntity* owner){const auto r=int16(scenario%4==0?-1:0);trace.push_back({"eventplaying",event,owner==activeSelf,r});return r;}
inline void __fastcall Register(CAESound* sound,void*,CEntity* entity){trace.push_back({"register",entity==reinterpret_cast<CEntity*>(physical.data())});*reinterpret_cast<uintptr_t*>(&sound->m_PhysicalEntity)=uintptr_t(entity);}''')
s=s.replace('sound->m_Flags});return reinterpret_cast<CAESound*>', 'sound->m_Flags,sound->m_Event});return reinterpret_cast<CAESound*>')
s=s.replace('{"CAEScriptAudioEntity","PlayLoadedMissionAudio",0x4EC270,0,-1,1024}', '{"CAEScriptAudioEntity","PlayMissionBankSound",0x4EC6D0,0,-1,1024}')
s=s.replace(' EntryPatch d(0x4D8F00,Sound),dd(bit_cast<uintptr_t>(&CAEAudioHardware::GetSoundLoadingStatus),Sound);', ' EntryPatch d(0x4D88C0,Loaded),dd(bit_cast<uintptr_t>(&CAEAudioHardware::IsSoundBankLoaded),Loaded);\n EntryPatch e(0x4EF570,EventPlaying),ed(bit_cast<uintptr_t>(&CAESoundManager::AreSoundsOfThisEventPlayingForThisEntity),EventPlaying);')
s=s.replace(' EntryPatch q(0x4EF660,Destructor),qu(0x4EF1A0,Destructor),qd(bit_cast<uintptr_t>(&CAESound::UnregisterWithPhysicalEntity),Destructor);', ' EntryPatch q(0x4EF820,Register),qd(bit_cast<uintptr_t>(&CAESound::RegisterWithPhysicalEntity),Register);')
s=s.replace('CVector position{float(i%11-5),float(i%7-3),float(i%5)};', 'CVector position=std::array<CVector,6>{CVector{-1000.f,-1000.f,-1000.f},CVector{0.f,-0.f,0.f},CVector{-1.f,0.f,0.f},CVector{1.f,0.f,0.f},CVector{2.f,3.f,4.f},CVector{std::numeric_limits<float>::quiet_NaN(),0.f,0.f}}[(i/6)%6];')
s=s.replace('reinterpret_cast<void(__thiscall*)(void*,uint8)>(fn.address)(self,slot);', '''const auto event=static_cast<eAudioEvents>(std::array<int32,4>{0,400+i%9,0x10000+403,65535}[(i/36)%4]);
    reinterpret_cast<void(__thiscall*)(void*,eAudioEvents,CVector&,CPhysical*,int16,uint8,uint8,float,float,float)>(fn.address)(self,event,position,(i/9)%2?reinterpret_cast<CPhysical*>(physical.data()):nullptr,int16(i%7-3),uint8(i%6),uint8((i/16)%3),float(i%9-4)*.25f,1.f+float(i%5)*.25f,.5f+float(i%7)*.125f);''')
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/ScriptLoaded355',fn).read_text().replace('ScriptLoaded355Live','ScriptBank355Live').replace('script-loaded355','script-bank355')
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('PlayMissionBankSound 0x4EC6D0: 1.024 pares preparados, pendiente ejecución. ABI corregida primer argumento event eAudioEvents (32 bits), nueve parámetros y ret36. Ranuras 0-5 con2/3 válidas, playing negativo/cero, loaded, eventoalto truncado a16 para volumen/Event; físico y posición sentinela/cero/+1/-1/ordinary/NaN. Initialise/flags/register/request simétricos. m_tempSound embebido, flags6, Eventlow16; retornoRequest ignorado conforme EXE. Restaura tabla volumen y hooks. Evidencia combo ../VehicleAudio355Evidence/Script_4EC6D0.txt.\n')
