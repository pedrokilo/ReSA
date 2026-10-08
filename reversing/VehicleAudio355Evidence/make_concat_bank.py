from pathlib import Path
p=Path('reversing/RadioConcat355');p.mkdir(exist_ok=True)
s=Path('reversing/RadioQueue355/LiveComparison.hpp').read_text().replace('RadioQueue355Live','RadioConcat355Live').replace('radio-queue355','radio-concat355').replace('#include "AEVehicleAudioEntity.h"','#include "AEVehicleAudioEntity.h"\n#include "AEAudioHardware.h"')
a=s.index('struct Access:');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''struct Access:CAERadioTrackManager{using CAERadioTrackManager::ChooseAdvertIndex;using CAERadioTrackManager::QueueUpTracksForStation;};
inline int modeCalls{}, userCalls{}, active{};
inline uint8 __fastcall Mode(void*,void*){const auto r=uint8((scenario/3+modeCalls++)%3);trace.push_back({"mode",r});return r;}
inline void __fastcall SetIndex(void*,void*,int32 id){trace.push_back({"set_index",id});}
inline int32 __fastcall User(void*,void*){const auto r=600+(scenario+userCalls++)%101;trace.push_back({"user",r});return r;}
inline int32 __fastcall Active(void*,void*){trace.push_back({"active",active});return active;}
inline void __fastcall Play(void*,void*,uint32 id,int32 next,uint32 offset,uint8 flags,bool user,bool nextUser){trace.push_back({"play",id,next,offset,flags,user,nextUser});}
inline bool __cdecl Probability(float chance){const auto r=(scenario/7+draw++)%3!=0;trace.push_back({"probability",std::bit_cast<uint32>(chance),r});return r;}
inline int32 __fastcall Advert(void*,void*,eRadioID station){const auto r=200+scenario%13;trace.push_back({"advert",int(station),r});return r;}
inline bool __fastcall Queue(void*,void*,eRadioID station,int8* count,int8 state,tRadioSettings& settings){const auto r=state!=2||scenario%3!=0;trace.push_back({"queue",int(station),int(*count),int(state),r});if(r&&*count>=0&&*count<5){settings.TrackQueue[size_t(*count)]=300+state;settings.TrackTypes[size_t(*count)]=state;settings.TrackIndices[size_t(*count)]=int8(40+state);++*count;}return r;}
'''+s[b:]
s=s.replace('"QueueUpTracksForStation",0x4EA670','"CheckForTrackConcatenation",0x4EA930')
a=s.index(' EntryPatch r(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch m(0x4F3330,Mode),md(bit_cast<uintptr_t>(&CAEUserRadioTrackManager::GetUserTrackPlayMode),Mode);
 EntryPatch i(0x4F3340,SetIndex),id(bit_cast<uintptr_t>(&CAEUserRadioTrackManager::SetUserTrackIndex),SetIndex);
 EntryPatch u(0x4F3250,User),ud(bit_cast<uintptr_t>(&CAEUserRadioTrackManager::SelectUserTrackIndex),User);
 EntryPatch a(0x4D8F80,Active),ad(bit_cast<uintptr_t>(&CAEAudioHardware::GetActiveTrackID),Active);
 EntryPatch p(0x4D8F10,Play),pd(bit_cast<uintptr_t>(&CAEAudioHardware::PlayTrack),Play);
 EntryPatch r(0x4D9C80,Probability),rd(bit_cast<uintptr_t>(&CAEAudioUtility::ResolveProbability),Probability);
 EntryPatch v(0x4E9570,Advert),vd(bit_cast<uintptr_t>(&Access::ChooseAdvertIndex),Advert);
 EntryPatch q(0x4EA670,Queue),qd(bit_cast<uintptr_t>(&Access::QueueUpTracksForStation),Queue);
 const auto savedModeByte=StaticRef<uint8>(0xBA67F8);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CAERadioTrackManager)> object{};
   auto run=[&](bool candidate){scenario=i;draw=modeCalls=userCalls=0;trace=json::array();memset(object.data(),0xCD,object.size());
    auto* self=reinterpret_cast<CAERadioTrackManager*>(object.data());auto& settings=self->m_ActiveSettings;settings=tRadioSettings(static_cast<eRadioID>(i%13));settings.PlayTime=i%4==0?-4:25;settings.TrackFlags=int8(i);self->m_nUserTrackPlayMode=uint8((i/5)%3);self->m_nTracksInARow.fill(uint8((i/13)%4==3?255:(i/13)%4));
    for(size_t k=0;k<5;++k){settings.TrackQueue[k]=100+int32(k);settings.TrackTypes[k]=int8((i/17+k)%8);settings.TrackIndices[k]=int8(k+20);}
    if(i%3!=0)settings.TrackQueue[2]=-1;if(i%11==0)settings.TrackQueue[1]=-1;active=i%5==0?900:settings.TrackQueue[1];StaticRef<uint8>(0xBA67F8)=uint8((i/23)%2);
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(self);
    if(!candidate&&trace.size()>1)++positiveCases;
    return json{{"switch",true},{"object",Hash(object.data(),object.size())},{"mode_global",StaticRef<uint8>(0xBA67F8)},{"trace",trace}};
'''+s[b:]
s=s.replace(' bool restored=true;',' StaticRef<uint8>(0xBA67F8)=savedModeByte;\n bool restored=true;')
# savedModeByte must live outside the EntryPatch block for restoration.
s=s.replace(' uint32 pairs=0,totalFailures=0;',' const auto savedModeByte=StaticRef<uint8>(0xBA67F8);\n uint32 pairs=0,totalFailures=0;').replace(' const auto savedModeByte=StaticRef<uint8>(0xBA67F8);\n for(size_t n=',' for(size_t n=')
p.joinpath('LiveComparison.hpp').write_text(s)
for name in ['build_probe.py','run_local.py']:
 t=Path('reversing/RadioQueue355',name).read_text().replace('RadioQueue355Live','RadioConcat355Live').replace('radio-queue355','radio-concat355')
 if name=='build_probe.py':t=t.replace("'Stats.cpp'","'Audio/Hardware/AEAudioHardware.cpp','Audio/Hardware/AEAudioHardware.h','Stats.cpp'")
 p.joinpath(name).write_text(t)
p.joinpath('README.md').write_text('CheckForTrackConcatenation 0x4EA930: 1024 EXE/DLL pairs prepared, runtime pending. Stations 0-12, changing user mode on successive queries, playback -4, active/mismatched/negative next track, missing/present queue tail, signed tracks-in-row, queue success/failure and probability sequences. Full object hash and exact dependency trace. Symmetric isolated dependencies; frontend mode byte restored. Evidence ../VehicleAudio355Evidence/Radio_4EA930.txt. Native hook remains quarantined.\n')
source=Path('source/game_sa/Audio/Managers/AERadioTrackManager.cpp')
t=source.read_text();a=t.index('void CAERadioTrackManager::CheckForTrackConcatenation()');b=t.index('// 0x4EB660',a)
t=t[:a]+Path('reversing/VehicleAudio355Evidence/Radio_ConcatNative.txt').read_text()+'\n\n'+t[b:];source.write_text(t)
