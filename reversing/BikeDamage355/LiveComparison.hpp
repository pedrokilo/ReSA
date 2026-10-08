#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Bike.h"
#include "SurfaceInfos_c.h"
#include "AEVehicleAudioEntity.h"
#include "CarCtrl.h"
#include "World.h"
#include "Camera.h"
#include "Explosion.h"
#include "Darkel.h"
#include "VisibilityPlugins.h"
#include "Stats.h"
#include "PlayerPed.h"
#include "../ProbeRuntime.hpp"
namespace BikeDamage355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline CBike* current{};inline CVehicle* other{};inline CPed* driver{};inline CPlayerPed* player{};inline int scenario{},vehicleCalls{};
inline float __cdecl Progress(){trace.push_back({"progress"});current->m_fDamageIntensity+=.5f;return float(scenario%3)*50.f;}
inline bool __cdecl Knock(CVehicle* vehicle,float intensity,uint16 piece,CEntity* entity,const CVector& position,const CVector& velocity){trace.push_back({"knock",vehicle==current,std::bit_cast<uint32>(intensity),piece,entity==other,std::bit_cast<uint32>(position.x),std::bit_cast<uint32>(velocity.z)});return false;}
inline CVehicle* __cdecl PlayerVehicle(int32 id,bool remote){trace.push_back({"player_vehicle",id,remote,vehicleCalls++});current->m_fHealth+=.125f;return scenario%4==0?nullptr:scenario%4==1?current:other;}
inline CPlayerPed* __cdecl Player(int32 id){trace.push_back({"player",id});return player;}
inline void __fastcall Reference(CEntity* entity,void*,CEntity** ref){trace.push_back({"reference",entity==other,ref==&current->m_pLastDamageEntity?0:ref==&current->m_Damager?1:2,*ref==entity});}
inline void __fastcall Wanted(void*,void*,eWantedLevel level){trace.push_back({"wanted",int(level)});}
inline int16 __fastcall Say(void* ped,void*,eGlobalSpeechContext context,uint32 delay,float probability,bool silence,bool audible,bool frontend){trace.push_back({"say",ped==driver,int(context),delay,std::bit_cast<uint32>(probability),silence,audible,frontend});current->m_fHealth+=1.f;return 0;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CBike","VehicleDamage",0,0x871360,56,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"bike-damage355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"bike-damage355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("bike-damage355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"bike-damage355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch pr(0x5591E0,Progress),prd(bit_cast<uintptr_t>(&CStats::GetPercentageProgress),Progress);
 EntryPatch ko(0x6B5A10,Knock),kod(bit_cast<uintptr_t>(&CBike::DamageKnockOffRider),Knock);
 EntryPatch pv(0x56E0D0,PlayerVehicle),pvd(bit_cast<uintptr_t>(&FindPlayerVehicle),PlayerVehicle);
 EntryPatch pp(0x56E210,Player),ppd(bit_cast<uintptr_t>(&FindPlayerPed),Player);
 EntryPatch re(0x571B70,Reference),red(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Reference);
 EntryPatch wa(0x609F30,Wanted),wad(bit_cast<uintptr_t>(&CPlayerPed::SetWantedLevelNoDrop),Wanted);
 EntryPatch sa(0x5EFFE0,Say),sad(bit_cast<uintptr_t>(&CPed::Say),Say);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{},offender{};alignas(16) std::array<uint8,sizeof(CPed)> ped{};alignas(16) std::array<uint8,sizeof(CPlayerPed)> playerPed{};alignas(16) std::array<uint8,0x80> matrix{};alignas(16) std::array<uint8,sizeof(tHandlingData)> handling{};
   auto run=[&](bool candidate){scenario=i;vehicleCalls=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(offender.data(),0,offender.size());memset(ped.data(),0,ped.size());memset(playerPed.data(),0,playerPed.size());memset(matrix.data(),0,matrix.size());memset(handling.data(),0,handling.size());current=reinterpret_cast<CBike*>(object.data());other=reinterpret_cast<CVehicle*>(offender.data());driver=reinterpret_cast<CPed*>(ped.data());player=reinterpret_cast<CPlayerPed*>(playerPed.data());*reinterpret_cast<uintptr_t*>(current)=0x871360;current->m_matrix=reinterpret_cast<CMatrixLink*>(matrix.data());*reinterpret_cast<CVector*>(matrix.data()+0x20)=CVector{0.f,0.f,1.f};current->m_pHandlingData=reinterpret_cast<tHandlingData*>(handling.data());current->m_pHandlingData->m_fCollisionDamageMultiplier=float(i%5)*.25f;current->m_pDriver=i%3==0?nullptr:driver;other->m_pDriver=i%7==0?nullptr:driver;offender[0x36]=uint8((i/5)%3+1);offender[0x4A4]=uint8((i/13)%3);
    object[0x42A]=i%19==0?0:0x20;object[0x428]=uint8(i%2);object[0x429]=i%5==0?0x80:0;object[0x614]=uint8((i%3==0?0x40:0)|(i%4==0?0x10:0));current->SetStatus(static_cast<eEntityStatus>((i/11)%6));current->m_nPhysicalFlags=i%17==0?0x100000u:0;static constexpr float intensity[]{0.f,.5f,1.f,20.f,25.f,40.f,100.f,300.f};current->m_fDamageIntensity=intensity[(i/3)%8];current->m_fHealth=float(i%9)*75.f;current->m_nPieceType=uint16(i%17);current->m_pDamageEntity=i%5==0?nullptr:other;current->m_vecLastCollisionImpactVelocity=CVector{.25f,.5f,float(i%5)*.25f};current->m_vecLastCollisionPosn=CVector{1.f,2.f,3.f};current->m_vecMoveSpeed=CVector{float(i%3)*.25f,0.f,0.f};other->m_vecMoveSpeed=CVector{.5f,0.f,0.f};current->m_pLastDamageEntity=nullptr;current->m_Damager=nullptr;
    hooks[n]->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((address!=0x6B8EC0)!=candidate)return json{{"switch",false}};
    const float request=i%23==0?1.f:i%29==0?-1.f:0.f;reinterpret_cast<void(__thiscall*)(CBike*,float,eVehicleCollisionComponent,CEntity*,CVector*,CVector*,eWeaponType)>(address)(current,request,static_cast<eVehicleCollisionComponent>(13),other,nullptr,nullptr,static_cast<eWeaponType>(0));
    const auto pointerID=[&](const void* value)->json {const auto address=reinterpret_cast<uintptr_t>(value);if(!address)return json::array({"null"});
      const auto region=[&](const void* base,size_t bytes,const char* label)->json {const auto start=reinterpret_cast<uintptr_t>(base);return address>=start && address-start<bytes?json::array({label,address-start}):json();};
      for(auto result:{region(object.data(),object.size(),"bike"),region(offender.data(),offender.size(),"other_vehicle"),region(ped.data(),ped.size(),"driver_fixture"),region(playerPed.data(),playerPed.size(),"player_fixture"),region(matrix.data(),matrix.size(),"matrix_fixture"),region(handling.data(),handling.size(),"handling_fixture")})if(!result.is_null())return result;
      return json::array({"unknown_exact",address});};
    const json pointerState={{"matrix",pointerID(current->m_matrix)},{"handling",pointerID(current->m_pHandlingData)},{"physical_damager",pointerID(current->m_pDamageEntity)},{"driver",pointerID(current->m_pDriver)},{"last_damage_entity",pointerID(current->m_pLastDamageEntity)},{"vehicle_damager",pointerID(current->m_Damager)}};
    current->m_matrix=nullptr;current->m_pHandlingData=nullptr;current->m_pDamageEntity=nullptr;current->m_pDriver=nullptr;current->m_pLastDamageEntity=nullptr;current->m_Damager=nullptr;
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"pointer_state",pointerState},{"object",Hash(object.data()+4,object.size()-4)},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 bool restored=true;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}




