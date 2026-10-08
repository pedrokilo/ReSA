from pathlib import Path
p=Path('reversing/RadioQueue355');p.mkdir(exist_ok=True)
s=Path('reversing/RadioSelection355/LiveComparison.hpp').read_text().replace('RadioSelection355Live','RadioQueue355Live').replace('radio-selection355','radio-queue355').replace('#include "AEAudioUtility.h"','#include "AEAudioUtility.h"\n#include "AEUserRadioTrackManager.h"')
a=s.index('inline ReversibleHooks');s=s[:a]+'''struct Access:CAERadioTrackManager{using CAERadioTrackManager::ChooseIdentIndex;using CAERadioTrackManager::ChooseAdvertIndex;using CAERadioTrackManager::ChooseDJBanterIndex;using CAERadioTrackManager::ChooseMusicTrackIndex;};
inline int32 __fastcall Ident(void*,void*,eRadioID id){const auto r=scenario%4==0?-1:100+scenario%9;trace.push_back({"ident",int(id),r});return r;}
inline int32 __fastcall Advert(void*,void*,eRadioID id){const auto r=200+scenario%9;trace.push_back({"advert",int(id),r});return r;}
inline int32 __fastcall DJ(void*,void*,eRadioID id){const auto r=scenario%3==0?-1:300+scenario%9;trace.push_back({"dj",int(id),r});return r;}
inline int8 __fastcall Music(void*,void*,eRadioID id){const auto r=int8(scenario%31);trace.push_back({"music",int(id),int(r)});return r;}
inline int32 __fastcall User(void*,void*){const auto r=scenario%4==0?-1:scenario%32;trace.push_back({"user",r});return r;}
'''+s[a:]
a=s.index('inline const std::array<Fn,6>');b=s.index('inline void Update()',a);s=s[:a]+'inline const std::array<Fn,1> fns{{{"CAERadioTrackManager","QueueUpTracksForStation",0x4EA670,0,-1,2048}}};\n'+s[b:]
s=s.replace('Base*,6>','Base*,1>').replace('bool,6>','bool,1>').replace('n<6','n<1')
a=s.index(' std::array<uint8,0xEBC>');b=s.index(' for(size_t n=',a)
s=s[:a]+''' uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch r(0x4D9C10,Random),rd(bit_cast<uintptr_t>(static_cast<int32(*)(int32,int32)>(&CAEAudioUtility::GetRandomNumberInRange<int32>)),Random);
 EntryPatch a(0x4E94C0,Ident),ad(bit_cast<uintptr_t>(&Access::ChooseIdentIndex),Ident);
 EntryPatch b(0x4E9570,Advert),bd(bit_cast<uintptr_t>(&Access::ChooseAdvertIndex),Advert);
 EntryPatch c(0x4EA2D0,DJ),cd(bit_cast<uintptr_t>(&Access::ChooseDJBanterIndex),DJ);
 EntryPatch d(0x4EA270,Music),dd(bit_cast<uintptr_t>(&Access::ChooseMusicTrackIndex),Music);
 EntryPatch u(0x4F3250,User),ud(bit_cast<uintptr_t>(&CAEUserRadioTrackManager::SelectUserTrackIndex),User);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CAERadioTrackManager)> object{};tRadioSettings settings;int8 count{};
   auto run=[&](bool candidate){scenario=i;draw=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(&settings,0xCD,sizeof(settings));count=int8(i%3);
    auto* self=reinterpret_cast<CAERadioTrackManager*>(object.data());hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    const auto ret=reinterpret_cast<bool(__thiscall*)(void*,eRadioID,int8*,int8,tRadioSettings&)>(fn.address)(self,static_cast<eRadioID>((i/3)%13),&count,int8((i/39)%9-1),settings);
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"return",ret},{"count",count},{"object",Hash(object.data(),object.size())},{"settings",Hash(&settings,sizeof(settings))},{"trace",trace}};
'''+s[b:]
a=s.index(' memcpy(reinterpret_cast<void*>(0xB61D78)');b=s.index(' for(size_t n=',a);s=s[:a]+' bool restored=true;\n'+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/RadioSelection355',fn).read_text().replace('RadioSelection355Live','RadioQueue355Live').replace('radio-selection355','radio-queue355')
 if fn=='build_probe.py':s=s.replace("'Audio/Managers/AERadioTrackManager.h'","'Audio/Managers/AERadioTrackManager.h','Audio/Managers/AEUserRadioTrackManager.cpp','Audio/Managers/AEUserRadioTrackManager.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('QueueUpTracksForStation 0x4EA670: 2.048 pares preparados; pendiente aplicar borrador fuente y ejecutar. Cuenta 0-2, estados -1..7, estaciones 0-12; selección positiva/negativa y user tracks. Comparación de todo settings, contador y traza de selección/RNG con tablas originales de intros/tracks/outros. Dependencias simétricas. Evidencia combo ../VehicleAudio355Evidence/Radio_4EA670.txt.\n')
