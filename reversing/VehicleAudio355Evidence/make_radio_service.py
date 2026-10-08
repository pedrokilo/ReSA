from pathlib import Path
p=Path('reversing/RadioService355');p.mkdir(exist_ok=True)
s=Path('reversing/RadioRetune355/LiveComparison.hpp').read_text().replace('RadioRetune355Live','RadioService355Live').replace('radio-retune355','radio-service355')
a=s.index('struct Access:');b=s.index('inline int modeCalls',a)
s=s[:a]+'''struct Access:CAERadioTrackManager{using CAERadioTrackManager::ChooseAdvertIndex;using CAERadioTrackManager::CheckForMissionStatsChanges;using CAERadioTrackManager::CheckForStationRetune;using CAERadioTrackManager::CheckForStationRetuneDuringPause;using CAERadioTrackManager::CheckForPause;using CAERadioTrackManager::CheckForTrackConcatenation;using CAERadioTrackManager::AddMusicTrackIndexToHistory;using CAERadioTrackManager::AddIdentIndexToHistory;using CAERadioTrackManager::AddAdvertIndexToHistory;using CAERadioTrackManager::AddDJBanterIndexToHistory;};
'''+s[b:]
a=s.index('inline ReversibleHooks');s=s[:a]+'''inline void __cdecl Mission(){trace.push_back({"mission_stats"});}
inline void __fastcall Retune(void*,void*){trace.push_back({"retune"});}
inline void __fastcall PauseRetune(void*,void*){trace.push_back({"pause_retune"});}
inline void __fastcall Pause(void*,void*){trace.push_back({"pause"});}
inline void __fastcall Volume(void*,void*){trace.push_back({"volume"});}
inline void __fastcall Concat(void*,void*){trace.push_back({"concat"});}
inline int32 __fastcall Length(void*,void*){trace.push_back({"length"});return 70000;}
inline int32 __fastcall Playing(void*,void*){const auto r=scenario%3==0?manager->m_ActiveSettings.TrackQueue[0]:scenario%3==1?manager->m_ActiveSettings.PrevTrackID:900;trace.push_back({"playing",r});return r;}
inline bool __fastcall Ambience(void*,void*){const auto r=scenario%5==0;trace.push_back({"ambience",r});return r;}
inline bool __cdecl VehicleActive(){const auto r=scenario%3!=0;trace.push_back({"vehicle_active",r});return r;}
inline void __fastcall Bass(void*,void*,eBassSetting setting,float gain){trace.push_back({"bass",int(setting),std::bit_cast<uint32>(gain)});}
inline void __fastcall Playback(void*,void*){trace.push_back({"playback"});}
inline void __fastcall StopTrack(void*,void*){trace.push_back({"stop_track"});}
inline void __fastcall HardwareService(void*,void*){trace.push_back({"hardware_service"});++StaticRef<uint32>(0xB7CB84);}
inline int32 __fastcall Time(void*,void*){trace.push_back({"time",-6});return -6;}
inline void __fastcall MusicHistory(void*,void*,eRadioID station,int8 index){trace.push_back({"music_history",int(station),int(index)});}
inline void __fastcall IdentHistory(void*,void*,eRadioID station,int32 index){trace.push_back({"ident_history",int(station),index});}
inline void __fastcall AdvertHistory(void*,void*,eRadioID station,int32 index){trace.push_back({"advert_history",int(station),index});}
inline void __fastcall DJHistory(void*,void*,eRadioID station,int32 index){trace.push_back({"dj_history",int(station),index});}
'''+s[a:]
s=s.replace('self->m_nMode=eRadioTrackMode::RADIO_STOPPING;self->m_bInitialised=false;','self->m_nMode=scenario%5==0?eRadioTrackMode::RADIO_STOPPING:eRadioTrackMode::RADIO_STOPPED;self->m_bInitialised=false;self->m_nStationsListed=self->m_nStationsListDown=0;')
s=s.replace('"CheckForStationRetune",0x4EB660','"Service",0x4EB9A0')
a=s.index(' const auto savedTimer=');b=s.index(' uint32 pairs=',a)
s=s[:a]+''' std::array<uint8,0x40> timer{};memcpy(timer.data(),reinterpret_cast<void*>(0xB7CB48),timer.size());const auto oldPrevious=StaticRef<int32>(0x8CBA68);std::array<uint8,sizeof(CPad)> pad{};memcpy(pad.data(),CPad::GetPad(0),pad.size());
'''+s[b:]
a=s.index(' EntryPatch c(');b=s.index(' for(size_t n=',a)
dependencies=[('l','0x4D8F70','Length','&CAEAudioHardware::GetTrackLengthMs'),('p','0x4D8F90','Playing','&CAEAudioHardware::GetPlayingTrackID'),('m','0x4E8410','Mission','&Access::CheckForMissionStatsChanges'),('r','0x4EB660','Retune','&Access::CheckForStationRetune'),('pr','0x4EB890','PauseRetune','&Access::CheckForStationRetuneDuringPause'),('o','0x4F4ED0','VehicleOptions','&CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio'),('a','0x507210','Ambience','&CAudioEngine::IsAmbienceTrackActive'),('va','0x4E9800','VehicleActive','&CAERadioTrackManager::IsVehicleRadioActive'),('b','0x4D94A0','Bass','&CAEAudioHardware::SetBassSetting'),('pl','0x4D8F10','Play','&CAEAudioHardware::PlayTrack'),('f','0x506EA0','Report','&CAudioEngine::ReportFrontendAudioEvent'),('pb','0x4EA640','Playback','&CAERadioTrackManager::StartTrackPlayback'),('v','0x4E9570','Advert','&Access::ChooseAdvertIndex'),('u','0x4F3250','User','&CAEUserRadioTrackManager::SelectUserTrackIndex'),('ac','0x4D8F80','Active','&CAEAudioHardware::GetActiveTrackID'),('mo','0x4F3330','Mode','&CAEUserRadioTrackManager::GetUserTrackPlayMode'),('mh','0x4E96C0','MusicHistory','&Access::AddMusicTrackIndexToHistory'),('ih','0x4E9720','IdentHistory','&Access::AddIdentIndexToHistory'),('ah','0x4E9760','AdvertHistory','&Access::AddAdvertIndexToHistory'),('dh','0x4E97B0','DJHistory','&Access::AddDJBanterIndexToHistory'),('st','0x4E9820','Stop','&CAERadioTrackManager::StopRadio'),('tm','0x4D8F60','Time','&CAEAudioHardware::GetTrackPlayTime'),('hs','0x4D9870','HardwareService','&CAEAudioHardware::Service'),('pa','0x4EA590','Pause','&Access::CheckForPause'),('vol','0x4EA010','Volume','&CAERadioTrackManager::UpdateRadioVolumes'),('cc','0x4EA930','Concat','&Access::CheckForTrackConcatenation'),('sp','0x4D8F50','StopTrack','&CAEAudioHardware::StopTrack')]
patches=''.join(f' EntryPatch {name}({address},{stub}),{name}d(bit_cast<uintptr_t>({target}),{stub});\n' for name,address,stub,target in dependencies)
patches+=' EntryPatch sr(0x4EB3C0,Start),srd(bit_cast<uintptr_t>(static_cast<void(CAERadioTrackManager::*)(eRadioID,eBassSetting,float,bool)>(&CAERadioTrackManager::StartRadio)),Start);\n'
s=s[:a]+patches+s[b:]
a=s.index('    optionCalls=');b=s.index('    hooks[n]->State',a)
s=s[:a]+'''    optionCalls=0;auto* self=reinterpret_cast<CAERadioTrackManager*>(object.data());manager=self;self->m_nMode=static_cast<eRadioTrackMode>(i%8);self->m_bInitialised=(i/8)%3==0;self->m_bBassEnhance=i%2;self->m_nStationsListed=self->m_nStationsListDown=0;auto& settings=self->m_ActiveSettings;settings=tRadioSettings(static_cast<eRadioID>((i/7)%14));self->m_RequestedSettings=tRadioSettings(static_cast<eRadioID>((i/11)%14));self->m_RequestedSettings.PlayTime=i%3==0?-1:123;self->m_RequestedSettings.BassGain=.75f;self->m_RequestedSettings.BassSetting=static_cast<eBassSetting>(i%3);
    for(size_t k=0;k<5;++k){settings.TrackQueue[k]=100+int32(k);settings.TrackTypes[k]=int8((i/13+k)%8);settings.TrackIndices[k]=int8(k+10);self->m_RequestedSettings.TrackQueue[k]=200+int32(k);self->m_RequestedSettings.TrackTypes[k]=int8((i/19+k)%8);self->m_RequestedSettings.TrackIndices[k]=int8(k+20);}
    if(i%5==0)settings.TrackQueue[1]=-1;settings.PrevTrackID=555;settings.PrevTrackType=int8((i/17)%8);settings.PrevTrackIdx=33;settings.PlayTime=-123;settings.BassGain=.5f;settings.BassSetting=static_cast<eBassSetting>(i%3);active=i%2?settings.TrackQueue[0]:900;
    static constexpr int32 times[]{-8,-7,-6,-2,-1,0,100};const auto input=times[(i/8)%std::size(times)];memset(&options,0xCD,sizeof(options));auto* gamepad=CPad::GetPad(0);memset(gamepad,0,sizeof(CPad));gamepad->NewState.m_bRadioTrackSkip=i%3==0?128:0;
    StaticRef<uint8>(0xB7CB48)=uint8((i/23)%2);StaticRef<uint8>(0xB7CB49)=uint8((i/47)%2);StaticRef<uint32>(0xB7CB84)=10000+uint32(i);StaticRef<uint32>(0xB7CB7C)=20000+uint32(i);StaticRef<int32>(0x8CBA68)=-999;
'''+s[b:]
s=s.replace('reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(self);','reinterpret_cast<void(__thiscall*)(void*,int32)>(fn.address)(self,input);')
s=s.replace('{"timer",StaticRef<uint32>(0xB7CB84)}','{"timer",Hash(reinterpret_cast<void*>(0xB7CB48),0x40)},{"previous",StaticRef<int32>(0x8CBA68)}')
a=s.index(' StaticRef<uint32>(0xB7CB84)=savedTimer;');b=s.index(' bool restored=true;',a)
s=s[:a]+''' memcpy(reinterpret_cast<void*>(0xB7CB48),timer.data(),timer.size());StaticRef<int32>(0x8CBA68)=oldPrevious;memcpy(CPad::GetPad(0),pad.data(),pad.size());
'''+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for name in ['build_probe.py','run_local.py']:
 t=Path('reversing/RadioRetune355',name).read_text().replace('RadioRetune355Live','RadioService355Live').replace('radio-retune355','radio-service355');p.joinpath(name).write_text(t)
p.joinpath('README.md').write_text('Service 0x4EB9A0: 1024 isolated EXE/DLL pairs prepared, runtime pending. Eight modes, hardware sentinel play times, all stations/types, initialized pending settings, history selection, mutable dependencies and skip with zero/two recursive service iterations. Full object/options/pad/timer hash and exact dependency trace. Timer span, previous-play-time global and pad restored. Dependencies isolated symmetrically; hardware/audio services are traced fixtures, not full integration certification. Evidence ../VehicleAudio355Evidence/Radio_4EB9A0.txt. Hook remains quarantined.\n')
source=Path('source/game_sa/Audio/Managers/AERadioTrackManager.cpp');t=source.read_text();a=t.index('void CAERadioTrackManager::Service(');b=t.index('// 0x5D40E0',a);t=t[:a]+Path('reversing/VehicleAudio355Evidence/Radio_ServiceNative.txt').read_text()+'\n\n'+t[b:];source.write_text(t)
