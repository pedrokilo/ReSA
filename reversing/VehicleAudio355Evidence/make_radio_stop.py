from pathlib import Path
p=Path('reversing/RadioStop355');p.mkdir(exist_ok=True)
s=Path('reversing/RadioTrack355/LiveComparison.hpp').read_text().replace('RadioTrack355Live','RadioStop355Live').replace('radio-track355','radio-stop355').replace('#include "AEAudioHardware.h"','#include "AEAudioHardware.h"\n#include "AudioEngine.h"')
a=s.index('inline ReversibleHooks');s=s[:a]+'''inline CAERadioTrackManager* manager{};
inline void __fastcall Report(void*,void*,eAudioEvents event,float volume,float speed){trace.push_back({"frontend",int(event),std::bit_cast<uint32>(volume),std::bit_cast<uint32>(speed)});StaticRef<uint32>(0xB7CB84)+=7;if(scenario%7==0)manager->m_bInitialised=true;if(scenario%11==0)manager->m_ActiveSettings.StationID=static_cast<eRadioID>(0);}
'''+s[a:]
s=s.replace('"TrackRadioStation",0x4EAC30','"StopRadio",0x4E9820')
a=s.index(' const auto savedModeByte=');b=s.index(' uint32 pairs=',a)
s=s[:a]+''' std::array<uint8,0x40> timer{};memcpy(timer.data(),reinterpret_cast<void*>(0xB7CB48),timer.size());std::array<uint8,3> clock{};memcpy(clock.data(),reinterpret_cast<void*>(0xB70153),3);
'''+s[b:]
a=s.index(' EntryPatch u(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch r(0x4D9C10,Random),rd(bit_cast<uintptr_t>(static_cast<int32(*)(int32,int32)>(&CAEAudioUtility::GetRandomNumberInRange<int32>)),Random);
 EntryPatch f(0x506EA0,Report),fd(bit_cast<uintptr_t>(&CAudioEngine::ReportFrontendAudioEvent),Report);
'''+s[b:]
a=s.index('    auto* self=');b=s.index('    hooks[n]->State',a)
s=s[:a]+'''    auto* self=reinterpret_cast<CAERadioTrackManager*>(object.data());manager=self;auto& settings=self->m_ActiveSettings;self->m_nMode=static_cast<eRadioTrackMode>((i/14)%8);settings.StationID=static_cast<eRadioID>(i%14);if(i%29==0&&int(self->m_nMode)>=3)settings.StationID=static_cast<eRadioID>(-1);
    settings.BassSetting=static_cast<eBassSetting>((i/11)%3);settings.PlayTime=int32((i/3)%50000);settings.TrackLengthMs=int32((i/5)%70000);settings.CurrTrackType=int8((i/7)%9);settings.CurrTrackID=i%2?100:400;
    for(size_t k=0;k<5;++k){settings.TrackQueue[k]=int32(100+k);settings.TrackTypes[k]=int8((i/17+k)%8);settings.TrackIndices[k]=int8(k);}
    for(size_t k=0;k<14;++k){self->m_aListenTimes[k]=int32(0x7FFFFFF0u+k);self->m_aRadioState[k].m_iTimeInMs=i%5==0?-1:20000;}
    memset(&options,0xCD,sizeof(options));StaticRef<uint8>(0xB7CB48)=uint8((i/23)%2);StaticRef<uint8>(0xB7CB49)=uint8((i/47)%2);StaticRef<uint32>(0xB7CB84)=uint32(i*17);StaticRef<uint32>(0xB7CB7C)=uint32(i*31);StaticRef<uint8>(0xB70154)=uint8(i%31);StaticRef<uint8>(0xB70153)=uint8(i%24);
'''+s[b:]
s=s.replace('const auto ret=reinterpret_cast<bool(__thiscall*)(void*,eRadioID,bool)>(fn.address)(self,station,i%2!=0);','reinterpret_cast<void(__thiscall*)(void*,tVehicleAudioSettings*,bool)>(fn.address)(self,i%3==0?nullptr:&options,i%2!=0);')
s=s.replace('{"return",ret},','').replace('{"mode_global",StaticRef<uint8>(0xBA67F8)}','{"options",Hash(&options,sizeof(options))},{"timer",Hash(reinterpret_cast<void*>(0xB7CB48),0x40)}')
s=s.replace('if(!candidate&&trace.size()>1)++positiveCases;','if(!candidate&&(int(self->m_nMode)==3||!trace.empty()))++positiveCases;')
a=s.index(' StaticRef<uint8>(0xBA67F8)=savedModeByte;');b=s.index(' bool restored=true;',a)
s=s[:a]+''' memcpy(reinterpret_cast<void*>(0xB7CB48),timer.data(),timer.size());memcpy(reinterpret_cast<void*>(0xB70153),clock.data(),3);
'''+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for name in ['build_probe.py','run_local.py']:
 t=Path('reversing/RadioTrack355',name).read_text().replace('RadioTrack355Live','RadioStop355Live').replace('radio-track355','radio-stop355')
 if name=='build_probe.py':t=t.replace("'Stats.cpp'","'Audio/AudioEngine.cpp','Audio/AudioEngine.h','Stats.cpp'")
 p.joinpath(name).write_text(t)
p.joinpath('README.md').write_text('StopRadio 0x4E9820: 1024 isolated EXE/DLL pairs prepared, runtime pending. All modes/stations/types, pause combinations, null/options, signed invalid station in inactive modes, listen-time wrap, current/queued identity and frontend dependency mutations before saved-state decisions. Full object/options/timer hash and exact frontend/RNG trace; timer span and game clock restored. Evidence ../VehicleAudio355Evidence/Radio_4E9820.txt. Native hook remains quarantined.\n')
source=Path('source/game_sa/Audio/Managers/AERadioTrackManager.cpp');t=source.read_text();a=t.index('void CAERadioTrackManager::StopRadio(');b=t.index('// 0x4E94C0',a);t=t[:a]+Path('reversing/VehicleAudio355Evidence/Radio_StopNative.txt').read_text()+'\n\n'+t[b:];source.write_text(t)
