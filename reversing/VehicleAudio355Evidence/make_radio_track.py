from pathlib import Path
p=Path('reversing/RadioTrack355');p.mkdir(exist_ok=True)
s=Path('reversing/RadioConcat355/LiveComparison.hpp').read_text().replace('RadioConcat355Live','RadioTrack355Live').replace('radio-concat355','radio-track355')
s=s.replace('using CAERadioTrackManager::ChooseAdvertIndex;','using CAERadioTrackManager::ChooseAdvertIndex;using CAERadioTrackManager::AddMusicTrackIndexToHistory;')
a=s.index('inline ReversibleHooks');s=s[:a]+'''inline void __fastcall History(void*,void*,eRadioID station,int8 index){trace.push_back({"history",int(station),int(index)});}
'''+s[a:]
s=s.replace('"CheckForTrackConcatenation",0x4EA930','"TrackRadioStation",0x4EAC30')
s=s.replace(' const auto savedModeByte=StaticRef<uint8>(0xBA67F8);',' const auto savedModeByte=StaticRef<uint8>(0xBA67F8);const auto savedPause=StaticRef<uint32>(0xB7CB84);std::array<uint8,3> clock{};memcpy(clock.data(),reinterpret_cast<void*>(0xB70153),3);')
a=s.index(' EntryPatch m(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch u(0x4F3250,User),ud(bit_cast<uintptr_t>(&CAEUserRadioTrackManager::SelectUserTrackIndex),User);
 EntryPatch r(0x4D9C80,Probability),rd(bit_cast<uintptr_t>(&CAEAudioUtility::ResolveProbability),Probability);
 EntryPatch q(0x4EA670,Queue),qd(bit_cast<uintptr_t>(&Access::QueueUpTracksForStation),Queue);
 EntryPatch h(0x4E96C0,History),hd(bit_cast<uintptr_t>(&Access::AddMusicTrackIndexToHistory),History);
'''+s[b:]
a=s.index('    auto* self=');b=s.index('    hooks[n]->State',a)
s=s[:a]+'''    auto* self=reinterpret_cast<CAERadioTrackManager*>(object.data());const auto station=static_cast<eRadioID>(i%13);auto& state=self->m_aRadioState[size_t(int8(station))];
    state.m_nGameClockDays=int8((i/13)%6==0?-1:10);state.m_nGameClockHours=int8((i/29)%6==0?-1:12);StaticRef<uint8>(0xB70154)=uint8(i%7==0?9:10);StaticRef<uint8>(0xB70153)=uint8((i/23)%24);StaticRef<uint8>(0xB70155)=uint8((i/17)%12);StaticRef<uint8>(0xBA67F8)=uint8(i%2);
    state.m_iTimeInPauseModeInMs=10000;static constexpr int32 elapsed[]{-1,0,6999,7000,7001,14999,15000,29999,30000,44999,45000,50000,51999,52000,200000,200001,204999,205000,205001};StaticRef<uint32>(0xB7CB84)=uint32(10000+elapsed[(i/3)%std::size(elapsed)]);
    state.m_iTrackPlayTime=(i/31)%2?200:-1;for(size_t k=0;k<3;++k){state.m_aElapsed[k]=15000;state.m_aTrackQueue[k]=int32(100+k);state.m_aTrackTypes[k]=int8((i/7+k)%8);}if(i%5==0)state.m_aTrackQueue[1]=-1;
'''+s[b:]
s=s.replace('reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(self);','const auto ret=reinterpret_cast<bool(__thiscall*)(void*,eRadioID,bool)>(fn.address)(self,station,i%2!=0);')
s=s.replace('return json{{"switch",true},{"object"','return json{{"switch",true},{"return",ret},{"object"')
s=s.replace(' StaticRef<uint8>(0xBA67F8)=savedModeByte;',' StaticRef<uint8>(0xBA67F8)=savedModeByte;StaticRef<uint32>(0xB7CB84)=savedPause;memcpy(reinterpret_cast<void*>(0xB70153),clock.data(),3);')
p.joinpath('LiveComparison.hpp').write_text(s)
for name in ['build_probe.py','run_local.py']:
 t=Path('reversing/RadioConcat355',name).read_text().replace('RadioConcat355Live','RadioTrack355Live').replace('radio-concat355','radio-track355');p.joinpath(name).write_text(t)
p.joinpath('README.md').write_text('TrackRadioStation 0x4EAC30: 1024 EXE/DLL pairs prepared, runtime pending. All stations, saved/invalid game dates, previous-month wrap, stale state, pause elapsed boundaries, skipping, all saved track types, missing user track, queue failures and probability order. Exact return, full object hash and dependency trace; frontend mode/pause clock/game date restored. Raw adjacent saved-state copies preserve EXE behavior. Evidence ../VehicleAudio355Evidence/Radio_4EAC30.txt.\n')
source=Path('source/game_sa/Audio/Managers/AERadioTrackManager.cpp');t=source.read_text();a=t.index('bool CAERadioTrackManager::TrackRadioStation(');b=t.index('// 0x4EA670',a);t=t[:a]+Path('reversing/VehicleAudio355Evidence/Radio_TrackNative.txt').read_text()+'\n\n'+t[b:];source.write_text(t)
