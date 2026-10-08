from pathlib import Path
p=Path('reversing/RadioDJ355');p.mkdir(exist_ok=True)
s=Path('reversing/RadioQueue355/LiveComparison.hpp').read_text().replace('RadioQueue355Live','RadioDJ355Live').replace('radio-queue355','radio-dj355')
a=s.index('struct Access:');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''struct Access:CAERadioTrackManager{using CAERadioTrackManager::ChooseDJBanterIndexFromList;};
inline bool __cdecl Probability(float probability){const auto r=(scenario/7+draw++)%4!=0;trace.push_back({"probability",std::bit_cast<uint32>(probability),r});return r;}
inline bool __cdecl Forecast(eWeatherType weather,int32 steps){const auto r=(scenario/11)%19==int(weather);trace.push_back({"forecast",int(weather),steps,r});return r;}
inline bool __cdecl Riot(){const auto r=scenario%3==0;trace.push_back({"riots",r});return r;}
inline int32 __fastcall List(void*,void*,eRadioID id,int32** list){const auto r=(scenario+draw++)%3==0?-1:600+scenario%17;trace.push_back({"list",int(id),uintptr_t(list),r});return r;}
'''+s[b:]
s=s.replace('{"CAERadioTrackManager","QueueUpTracksForStation",0x4EA670,0,-1,2048}','{"CAERadioTrackManager","ChooseDJBanterIndex",0x4EA2D0,0,-1,2048}')
a=s.index(' uint32 pairs=');b=s.index(' for(size_t n=',a)
s=s[:a]+''' const auto oldSpecial=StaticRef<uint16>(0xB62C72);const auto oldHour=StaticRef<uint8>(0xB70153);const auto oldChase=StaticRef<int32>(0xB72914);std::array<uint8,15*4*13> oldHistory{};memcpy(oldHistory.data(),reinterpret_cast<void*>(0xB61D78),oldHistory.size());uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch p(0x4D9C80,Probability),pd(bit_cast<uintptr_t>(&CAEAudioUtility::ResolveProbability),Probability);
 EntryPatch f(0x72A590,Forecast),fd(bit_cast<uintptr_t>(&CWeather::ForecastWeather),Forecast);
 EntryPatch r(0x441C10,Riot),rd(bit_cast<uintptr_t>(&CGameLogic::LaRiotsActiveHere),Riot);
 EntryPatch l(0x4E95E0,List),ld(bit_cast<uintptr_t>(&Access::ChooseDJBanterIndexFromList),List);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CAERadioTrackManager)> object{};
   auto run=[&](bool candidate){scenario=i;draw=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(reinterpret_cast<void*>(0xB61D78),0xFF,oldHistory.size());
    const auto station=(i/5)%12;StaticRef<int8>(0xB62C73)=int8(i%5-1);StaticRef<int8>(0xB62C72)=int8((i/60)%22);StaticRef<uint8>(0xB70153)=uint8(i%24);StaticRef<int32>(0xB72914)=i%9==0?1:0;
    if(i%2)(&StaticRef<int32>(0xB61D78))[station*15]=(&StaticRef<int32>(0x8C8BF0))[station*2];
    auto* self=reinterpret_cast<CAERadioTrackManager*>(object.data());hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    const auto ret=reinterpret_cast<int32(__thiscall*)(void*,eRadioID)>(fn.address)(self,static_cast<eRadioID>(station));
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"return",ret},{"object",Hash(object.data(),object.size())},{"history",Hash(reinterpret_cast<void*>(0xB61D78),oldHistory.size())},{"trace",trace}};
'''+s[b:]
s=s.replace(' bool restored=true;', ' StaticRef<uint16>(0xB62C72)=oldSpecial;StaticRef<uint8>(0xB70153)=oldHour;StaticRef<int32>(0xB72914)=oldChase;memcpy(reinterpret_cast<void*>(0xB61D78),oldHistory.data(),oldHistory.size());bool restored=StaticRef<uint16>(0xB62C72)==oldSpecial&&StaticRef<uint8>(0xB70153)==oldHour&&StaticRef<int32>(0xB72914)==oldChase&&memcmp(reinterpret_cast<void*>(0xB61D78),oldHistory.data(),oldHistory.size())==0;')
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/RadioQueue355',fn).read_text().replace('RadioQueue355Live','RadioDJ355Live').replace('radio-queue355','radio-dj355')
 if fn=='build_probe.py':s=s.replace("'Stats.h'","'Stats.h','Weather.cpp','Weather.h','GameLogic.cpp','GameLogic.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('ChooseDJBanterIndex 0x4EA2D0: 2.048 pares preparados; pendiente aplicar fuente y ejecutar. DJ especial 0/1/2/otros, índices 0-21, historiales repetidos, horas 0-23, forecast, probabilidad, persecución y disturbios. Incluye secuencia exacta de llamadas forecast aun para rama original imposible [8,6). Stubs simétricos de dependencias; compara return/historia y trazas. Restaura globales. Evidencia combo ../VehicleAudio355Evidence/Radio_4EA2D0.txt.\n')
