from pathlib import Path
p=Path('reversing/BikeDamage355');p.mkdir(exist_ok=True)
s=Path('reversing/BikeBlow355/LiveComparison.hpp').read_text().replace('BikeBlow355Live','BikeDamage355Live').replace('bike-blow355','bike-damage355').replace('#include "VisibilityPlugins.h"','#include "VisibilityPlugins.h"\n#include "Stats.h"\n#include "PlayerPed.h"')
a=s.index('inline CBike* current');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''inline CBike* current{};inline CVehicle* other{};inline CPed* driver{};inline CPlayerPed* player{};inline int scenario{},vehicleCalls{};
inline float __cdecl Progress(){trace.push_back({"progress"});current->m_fDamageIntensity+=.5f;return float(scenario%3)*50.f;}
inline bool __cdecl Knock(CVehicle* vehicle,float intensity,uint16 piece,CEntity* entity,const CVector& position,const CVector& velocity){trace.push_back({"knock",vehicle==current,std::bit_cast<uint32>(intensity),piece,entity==other,std::bit_cast<uint32>(position.x),std::bit_cast<uint32>(velocity.z)});return false;}
inline CVehicle* __cdecl PlayerVehicle(int32 id,bool remote){trace.push_back({"player_vehicle",id,remote,vehicleCalls++});current->m_fHealth+=.125f;return scenario%4==0?nullptr:scenario%4==1?current:other;}
inline CPlayerPed* __cdecl Player(int32 id){trace.push_back({"player",id});return player;}
inline void __fastcall Reference(CEntity* entity,void*,CEntity** ref){trace.push_back({"reference",entity==other,ref==&current->m_pLastDamageEntity?0:ref==&current->m_Damager?1:2,*ref==entity});}
inline void __fastcall Wanted(void*,void*,eWantedLevel level){trace.push_back({"wanted",int(level)});}
inline int16 __fastcall Say(void* ped,void*,eGlobalSpeechContext context,uint32 delay,float probability,bool silence,bool audible,bool frontend){trace.push_back({"say",ped==driver,int(context),delay,std::bit_cast<uint32>(probability),silence,audible,frontend});current->m_fHealth+=1.f;return 0;}
'''+s[b:]
s=s.replace('{"CBike","BlowUpCar",0,0x871360,41,1024}','{"CBike","VehicleDamage",0,0x871360,56,1024}')
a=s.index(' EntryPatch cl(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch pr(0x5591E0,Progress),prd(bit_cast<uintptr_t>(&CStats::GetPercentageProgress),Progress);
 EntryPatch ko(0x6B5A10,Knock),kod(bit_cast<uintptr_t>(&CBike::DamageKnockOffRider),Knock);
 EntryPatch pv(0x56E0D0,PlayerVehicle),pvd(bit_cast<uintptr_t>(&FindPlayerVehicle),PlayerVehicle);
 EntryPatch pp(0x56E210,Player),ppd(bit_cast<uintptr_t>(&FindPlayerPed),Player);
 EntryPatch re(0x571B70,Reference),red(bit_cast<uintptr_t>(&CEntity::RegisterReference),Reference);
 EntryPatch wa(0x609F30,Wanted),wad(bit_cast<uintptr_t>(&CPlayerPed::SetWantedLevelNoDrop),Wanted);
 EntryPatch sa(0x5EFFE0,Say),sad(bit_cast<uintptr_t>(&CPed::Say),Say);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original=',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{},offender{};alignas(16) std::array<uint8,sizeof(CPed)> ped{};alignas(16) std::array<uint8,sizeof(CPlayerPed)> playerPed{};alignas(16) std::array<uint8,0x80> matrix{};alignas(16) std::array<uint8,sizeof(tHandlingData)> handling{};
   auto run=[&](bool candidate){scenario=i;vehicleCalls=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(offender.data(),0,offender.size());memset(ped.data(),0,ped.size());memset(playerPed.data(),0,playerPed.size());memset(matrix.data(),0,matrix.size());memset(handling.data(),0,handling.size());current=reinterpret_cast<CBike*>(object.data());other=reinterpret_cast<CVehicle*>(offender.data());driver=reinterpret_cast<CPed*>(ped.data());player=reinterpret_cast<CPlayerPed*>(playerPed.data());*reinterpret_cast<uintptr_t*>(current)=0x871360;current->m_matrix=reinterpret_cast<CMatrixLink*>(matrix.data());*reinterpret_cast<CVector*>(matrix.data()+0x20)={0.f,0.f,1.f};current->m_pHandlingData=reinterpret_cast<tHandlingData*>(handling.data());current->m_pHandlingData->m_fCollisionDamageMultiplier=float(i%5)*.25f;current->m_pDriver=i%3==0?nullptr:driver;other->m_pDriver=i%7==0?nullptr:driver;offender[0x36]=uint8((i/5)%3+1);offender[0x4A4]=uint8((i/13)%3);
    object[0x42A]=i%19==0?0:0x20;object[0x428]=uint8(i%2);object[0x429]=i%5==0?0x80:0;object[0x614]=uint8((i%3==0?0x40:0)|(i%4==0?0x10:0));current->SetStatus(static_cast<eEntityStatus>((i/11)%6));current->m_nPhysicalFlags=i%17==0?0x100000u:0;static constexpr float intensity[]{0.f,.5f,1.f,20.f,25.f,40.f,100.f,300.f};current->m_fDamageIntensity=intensity[(i/3)%8];current->m_fHealth=float(i%9)*75.f;current->m_nPieceType=uint16(i%17);current->m_pDamageEntity=i%5==0?nullptr:other;current->m_vecLastCollisionImpactVelocity={.25f,.5f,float(i%5)*.25f};current->m_vecLastCollisionPosn={1.f,2.f,3.f};current->m_vecMoveSpeed={float(i%3)*.25f,0.f,0.f};other->m_vecMoveSpeed={.5f,0.f,0.f};current->m_pLastDamageEntity=nullptr;current->m_Damager=nullptr;
    hooks[n]->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((address!=0x6B8EC0)!=candidate)return json{{"switch",false}};
    const float request=i%23==0?1.f:i%29==0?-1.f:0.f;reinterpret_cast<void(__thiscall*)(CBike*,float,eVehicleCollisionComponent,CEntity*,CVector*,CVector*,eWeaponType)>(address)(current,request,static_cast<eVehicleCollisionComponent>(13),other,nullptr,nullptr,static_cast<eWeaponType>(0));
    current->m_matrix=nullptr;current->m_pHandlingData=nullptr;current->m_pDamageEntity=nullptr;current->m_pDriver=nullptr;current->m_pLastDamageEntity=nullptr;current->m_Damager=nullptr;
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"trace",trace}};
'''+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/BikeBlow355',fn).read_text().replace('bike-blow355','bike-damage355').replace('BikeBlow355Live','BikeDamage355Live')
 if fn=='build_probe.py':s=s.replace("'Entity/Vehicle/Vehicle.h'","'Entity/Vehicle/Vehicle.h','Stats.cpp','Stats.h','Entity/Ped/PlayerPed.cpp','Entity/Ped/PlayerPed.h','Entity/Ped/Ped.cpp','Entity/Ped/Ped.h','tHandlingData.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('VehicleDamage 0x6B8EC0 VMT871360 slot56:1024 pares pendientes. ASM completo Bike_DamageFull. Input positive/0/negative, stored collision thresholds, progress0/50/100, local damage before mutable progress, gate damage/invulnerability, building/vehicle/ped and directional impact, statuses, driver speech, repeated player queries, protected health factors/clamp, fire setup/references. Paired Knock/Progress/PlayerVehicle/PlayerPed/Reference/Wanted/Say, player query/speech mutate health to check timing. Whole bike hash and dependency trace. Finite moderate damage/speed fixtures; NaN/overflow not certified. No globals changed, hooksfalse until validation.\n')
