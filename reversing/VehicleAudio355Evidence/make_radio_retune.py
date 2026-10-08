from pathlib import Path
p=Path('reversing/RadioRetune355');p.mkdir(exist_ok=True)
s=Path('reversing/RadioStop355/LiveComparison.hpp').read_text().replace('RadioStop355Live','RadioRetune355Live').replace('radio-stop355','radio-retune355')
a=s.index('inline ReversibleHooks');s=s[:a]+'''inline int optionCalls{};
inline int8 __fastcall Cutscene(void*,void*){const auto value=int8(scenario%9==0?1:0);trace.push_back({"cutscene",int(value)});return value;}
inline tVehicleAudioSettings* __cdecl VehicleOptions(){const auto call=optionCalls++;options.RadioType=static_cast<eAERadioType>(call==0?scenario%4:(scenario%6==0?1:0));trace.push_back({"options",call,int(options.RadioType)});return call==0&&scenario%17==0?nullptr:&options;}
inline void __fastcall Stop(void* raw,void*,tVehicleAudioSettings* settings,bool pause){trace.push_back({"stop",settings!=nullptr,pause});auto* self=static_cast<CAERadioTrackManager*>(raw);self->m_nMode=eRadioTrackMode::RADIO_STOPPING;self->m_bInitialised=false;}
inline void __fastcall Start(void* raw,void*,eRadioID station,eBassSetting bass,float gain,bool skip){trace.push_back({"start",int(station),int(bass),std::bit_cast<uint32>(gain),skip});static_cast<CAERadioTrackManager*>(raw)->m_ActiveSettings.StationID=station;}
'''+s[a:]
s=s.replace('"StopRadio",0x4E9820','"CheckForStationRetune",0x4EB660')
a=s.index(' std::array<uint8,0x40> timer{};');b=s.index(' uint32 pairs=',a)
s=s[:a]+''' const auto savedTimer=StaticRef<uint32>(0xB7CB84);const auto savedReplay=StaticRef<uint8>(0xA43088);const auto savedUsers=StaticRef<uint16>(0xB6B976);const auto savedCamera=StaticRef<float>(0xB6F14C);std::array<uint8,sizeof(CPad)> pad{};memcpy(pad.data(),CPad::GetPad(0),pad.size());
'''+s[b:]
a=s.index(' EntryPatch r(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch c(0x507160,Cutscene),cd(bit_cast<uintptr_t>(&CAudioEngine::GetCutsceneTrackStatus),Cutscene);
 EntryPatch o(0x4F4ED0,VehicleOptions),od(bit_cast<uintptr_t>(&CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio),VehicleOptions);
 EntryPatch f(0x506EA0,Report),fd(bit_cast<uintptr_t>(&CAudioEngine::ReportFrontendAudioEvent),Report);
 EntryPatch t(0x4E9820,Stop),td(bit_cast<uintptr_t>(&CAERadioTrackManager::StopRadio),Stop);
 EntryPatch b(0x4EB3C0,Start),bd(bit_cast<uintptr_t>(static_cast<void(CAERadioTrackManager::*)(eRadioID,eBassSetting,float,bool)>(&CAERadioTrackManager::StartRadio)),Start);
'''+s[b:]
a=s.index('    auto* self=');b=s.index('    hooks[n]->State',a)
s=s[:a]+'''    optionCalls=0;auto* self=reinterpret_cast<CAERadioTrackManager*>(object.data());manager=self;self->m_nMode=static_cast<eRadioTrackMode>((i/14)%8);self->m_ActiveSettings.StationID=static_cast<eRadioID>(i%14);self->m_RequestedSettings.StationID=static_cast<eRadioID>((i/17)%14);self->m_nStationsListed=(i/7)%5-2;self->m_nStationsListDown=(i/11)%3-1;self->m_bInitialised=i%3==0;self->m_iRadioStationScriptRequest=i%4==0?(i/19)%14:-1;self->m_nTimeRadioStationRetuned=1000;self->field_60=2000;self->m_ActiveSettings.BassSetting=static_cast<eBassSetting>(i%3);self->m_ActiveSettings.BassGain=.5f;
    memset(&options,0xCD,sizeof(options));auto* gamepad=CPad::GetPad(0);memset(gamepad,0,sizeof(CPad));gamepad->DisablePlayerControls=i%13==0;gamepad->OldState.DPadUp=i%4==0?128:0;gamepad->OldState.DPadDown=i%4==1?128:0;gamepad->NewState.DPadUp=i%8==0?128:0;
    static constexpr uint32 times[]{0,2499,2500,2501,3999,4000,4001,5999,6000,6001,0xFFFFFFFFu};StaticRef<uint32>(0xB7CB84)=times[(i/3)%std::size(times)];StaticRef<uint8>(0xA43088)=uint8(i%23==0?1:0);StaticRef<uint16>(0xB6B976)=uint16(i%3==0?0:3);StaticRef<float>(0xB6F14C)=i%5==0?std::bit_cast<float>(0x7FC00000u):float(i%4)*.3f;
'''+s[b:]
s=s.replace('reinterpret_cast<void(__thiscall*)(void*,tVehicleAudioSettings*,bool)>(fn.address)(self,i%3==0?nullptr:&options,i%2!=0);','reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(self);')
s=s.replace('{"timer",Hash(reinterpret_cast<void*>(0xB7CB48),0x40)}','{"timer",StaticRef<uint32>(0xB7CB84)},{"pad",Hash(CPad::GetPad(0),sizeof(CPad))}')
a=s.index(' memcpy(reinterpret_cast<void*>(0xB7CB48),timer.data()');b=s.index(' bool restored=true;',a)
s=s[:a]+''' StaticRef<uint32>(0xB7CB84)=savedTimer;StaticRef<uint8>(0xA43088)=savedReplay;StaticRef<uint16>(0xB6B976)=savedUsers;StaticRef<float>(0xB6F14C)=savedCamera;memcpy(CPad::GetPad(0),pad.data(),pad.size());
'''+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for name in ['build_probe.py','run_local.py']:
 t=Path('reversing/RadioStop355',name).read_text().replace('RadioStop355Live','RadioRetune355Live').replace('radio-stop355','radio-retune355')
 if name=='build_probe.py':t=t.replace("'Stats.cpp'","'Pad.cpp','Pad.h','ControllerState.h','Stats.cpp'")
 p.joinpath(name).write_text(t)
p.joinpath('README.md').write_text('CheckForStationRetune 0x4EB660: 1024 EXE/DLL pairs prepared, runtime pending. All modes/stations, mutable first/second vehicle options, cutscene/replay, script and pad requests, missing user tracks, pause-clock thresholds/wrap and camera NaN. Full object/options/pad hash and exact stop/start/frontend trace. Real pad snapshot restored; timer/replay/user count/camera restored. Evidence ../VehicleAudio355Evidence/Radio_4EB660.txt. Hook remains quarantined.\n')
source=Path('source/game_sa/Audio/Managers/AERadioTrackManager.cpp');t=source.read_text();a=t.index('void CAERadioTrackManager::CheckForStationRetune()');b=t.index('// 0x4EB890',a);t=t[:a]+Path('reversing/VehicleAudio355Evidence/Radio_RetuneNative.txt').read_text()+'\n\n'+t[b:];source.write_text(t)
