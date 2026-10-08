from pathlib import Path
p=Path('reversing/PlaneBlow355');p.mkdir(exist_ok=True)
s=Path('reversing/BikeBlow355/LiveComparison.hpp').read_text().replace('BikeBlow355Live','PlaneBlow355Live').replace('bike-blow355','plane-blow355').replace('#include "Bike.h"','#include "Plane.h"').replace('#include "VisibilityPlugins.h"','#include "VisibilityPlugins.h"\n#include "Stats.h"\n#include "FireManager.h"\n#include "DamageManager.h"')
a=s.index('inline CBike* current');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''inline CPlane* current{};inline CEntity* damager{};inline CPed* playerPed{};inline int scenario{};inline RpAtomic atomic{};
inline CPlayerPed* __cdecl Player(int32 id){trace.push_back({"player",id});CWorld::PlayerInFocus=uint8(scenario%2);return reinterpret_cast<CPlayerPed*>(playerPed);}
inline CVehicle* __cdecl PlayerVehicle(int32 id,bool remote){trace.push_back({"player_vehicle",id,remote});CWorld::PlayerInFocus^=1;return current;}
inline int32 __cdecl Random(){const int32 result=scenario%19==0?-12345:(scenario*7919)&0x7FFF;trace.push_back({"random",result});return result;}
inline void __cdecl Stat(eStats stat,float value){trace.push_back({"stat",int(stat),std::bit_cast<uint32>(value)});if(scenario%9==0)current->SetStatus(STATUS_PLAYER);if(scenario%11==0)current->m_nModelIndex=464;CTimer::SetTimeInMS(CTimer::GetTimeInMS()+19);}
inline void __cdecl Clump(RpClump*,int32 flags){trace.push_back({"clump",flags});}
inline void __fastcall Completely(void*,void*,bool detach){trace.push_back({"complete",detach});reinterpret_cast<uint8*>(current)[0x5A0]=0x17;}
inline void __fastcall Bumper(void*,void*,ePanels panel,bool effect){trace.push_back({"bumper",int(panel),effect});}
inline void __fastcall Door(void*,void*,eDoors door,bool effect){trace.push_back({"door",int(door),effect});}
inline CObject* __fastcall Flying(void*,void*,eCarNodes node,uint32 collision){trace.push_back({"flying",int(node),collision});return nullptr;}
inline RwFrame* __cdecl Objects(RwFrame* frame,RwObjectCallBack cb,void* data){trace.push_back({"objects",cb==GetCurrentAtomicObjectCB});*static_cast<RpAtomic**>(data)=scenario%3==0?nullptr:&atomic;return frame;}
inline void __fastcall Shake(void*,void*,float strength,CVector position){trace.push_back({"shake",std::bit_cast<uint32>(strength),std::bit_cast<uint32>(position.x),std::bit_cast<uint32>(position.y),std::bit_cast<uint32>(position.z)});current->GetPosition().z+=.5f;}
inline void __fastcall Kill(void*,void*){trace.push_back({"kill"});if(scenario%7==0)reinterpret_cast<uint8*>(current)[0x428]=0xFF;reinterpret_cast<uint8*>(current)[0x4A8]=0xFF;}
inline void __fastcall Law(void*,void*,bool state){trace.push_back({"law",state});current->GetPosition().x+=.25f;}
inline CFire* __fastcall Fire(void*,void*,CEntity* target,CEntity* creator,float size,uint8 flag,uint32 time,int8 generations){trace.push_back({"fire",target==current,creator==damager,std::bit_cast<uint32>(size),flag,time,generations});current->GetPosition().y+=.125f;return nullptr;}
inline void __cdecl Explosion(CEntity* victim,CEntity* creator,eExplosionType type,CVector position,uint32 lifetime,uint8 sound,float shake,uint8 visible){trace.push_back({"explosion",victim==current,creator==damager,int(type),std::bit_cast<uint32>(position.x),std::bit_cast<uint32>(position.y),std::bit_cast<uint32>(position.z),lifetime,sound,std::bit_cast<uint32>(shake),visible});}
inline void __cdecl Darkel(CVehicle& vehicle,int32 player){trace.push_back({"darkel",&vehicle==current,player});}
'''+s[b:]
s=s.replace('{"CBike","BlowUpCar",0,0x871360,41,1024}','{"CPlane","BlowUpCar",0,0x871948,41,1024}')
s=s.replace('uint32 pairs=0,totalFailures=0;', '''std::array<std::array<uint8,12>,2> savedPlayerStats{};for(size_t j=0;j<2;++j)memcpy(savedPlayerStats[j].data(),reinterpret_cast<void*>(0xB7CED8+j*0x190),12);std::array<uint8,8> savedCounts{};memcpy(savedCounts.data(),reinterpret_cast<void*>(0x9690A8),8);const auto savedFocus=CWorld::PlayerInFocus;const auto savedTime=CTimer::GetTimeInMS();uint32 pairs=0,totalFailures=0;''')
a=s.index(' EntryPatch cl(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch pp(0x56E210,Player),ppd(bit_cast<uintptr_t>(&FindPlayerPed),Player);
 EntryPatch pv(0x56E0D0,PlayerVehicle),pvd(bit_cast<uintptr_t>(&FindPlayerVehicle),PlayerVehicle);
 EntryPatch rn(0x821B1E,Random),rnd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
 EntryPatch st(0x55C180,Stat),stdp(bit_cast<uintptr_t>(&CStats::IncrementStat),Stat);
 EntryPatch cl(0x7322F0,Clump),cld(bit_cast<uintptr_t>(&CVisibilityPlugins::SetClumpForAllAtomicsFlag),Clump);
 EntryPatch co(0x6C25D0,Completely),cod(bit_cast<uintptr_t>(&CDamageManager::FuckCarCompletely),Completely);
 EntryPatch bu(0x6B1350,Bumper),bud(bit_cast<uintptr_t>(&CAutomobile::SetBumperDamage),Bumper);
 EntryPatch doo(0x6B1600,Door),dod(bit_cast<uintptr_t>(&CAutomobile::SetDoorDamage),Door);
 EntryPatch fl(0x6A8580,Flying),fldp(bit_cast<uintptr_t>(&CAutomobile::SpawnFlyingComponent),Flying);
 EntryPatch ob(0x7F1200,Objects),obd(bit_cast<uintptr_t>(&RwFrameForAllObjects),Objects);
 EntryPatch sh(0x50A9F0,Shake),shd(bit_cast<uintptr_t>(&CCamera::CamShake),Shake);
 EntryPatch ki(0x6D1C80,Kill),kid(bit_cast<uintptr_t>(&CVehicle::KillPedsInVehicle),Kill);
 EntryPatch la(0x6D2330,Law),lad(bit_cast<uintptr_t>(&CVehicle::ChangeLawEnforcerState),Law);
 EntryPatch fi(0x53A050,Fire),fid(bit_cast<uintptr_t>(static_cast<CFire*(CFireManager::*)(CEntity*,CEntity*,float,uint8,uint32,int8)>(&CFireManager::StartFire)),Fire);
 EntryPatch ex(0x736A50,Explosion),exd(bit_cast<uintptr_t>(&CExplosion::AddExplosion),Explosion);
 EntryPatch da(0x43DF20,Darkel),dad(bit_cast<uintptr_t>(&CDarkel::RegisterCarBlownUpByPlayer),Darkel);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original=',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CPlane)> object{};alignas(16) std::array<uint8,0x80> matrix{};alignas(16) std::array<uint8,sizeof(CEntity)> offender{};std::array<std::array<uint8,sizeof(CPed)>,9> peds{};RwFrame frame{};
   auto run=[&](bool candidate){scenario=i;trace=json::array();memset(object.data(),0xCD,object.size());memset(matrix.data(),0,matrix.size());memset(offender.data(),0,offender.size());memset(peds.data(),0,sizeof(peds));memset(&frame,0,sizeof(frame));memset(&atomic,0,sizeof(atomic));reinterpret_cast<uint8*>(&atomic)[2]=0xFF;current=reinterpret_cast<CPlane*>(object.data());playerPed=reinterpret_cast<CPed*>(peds[0].data());damager=i%4==0?nullptr:i%4==1?static_cast<CEntity*>(playerPed):i%4==2?static_cast<CEntity*>(current):reinterpret_cast<CEntity*>(offender.data());*reinterpret_cast<uintptr_t*>(current)=0x871948;current->m_matrix=i%2?reinterpret_cast<CMatrixLink*>(matrix.data()):nullptr;current->GetPosition()={float(i%7),float(i%11),float(i%13)*.25f};current->m_vecMoveSpeed={.25f,-.5f,.125f};current->m_vecTurnSpeed={.125f,.25f,-.5f};current->m_pDriver=i%3==0?nullptr:playerPed;for(size_t j=0;j<8;++j)current->m_apPassengers[j]=(i+int(j))%3==0?nullptr:reinterpret_cast<CPed*>(peds[j+1].data());current->SetStatus(static_cast<eEntityStatus>((i/3)%6));object[0x42A]=i%17==0?0:0x20;object[0x428]=uint8(i%8);object[0x429]=0xFF;object[0x42D]=0xFF;object[0x4A8]=0xFF;object[0x868]=0xFF;current->m_nPhysicalFlags=uint32(i*13);current->m_autoPilot.m_nCarMission=static_cast<eCarMission>(i%5==0?57:0);current->m_nModelIndex=i%7==0?464:476;current->m_fHealth=100.f;current->m_wBombTimer=17;std::ranges::fill(current->m_aCarNodes,nullptr);current->m_aCarNodes[5]=i%2?&frame:nullptr;*reinterpret_cast<void**>(object.data()+0x18)=nullptr;for(size_t j=0;j<2;++j){memset(reinterpret_cast<void*>(0xB7CED8+j*0x190),0,12);*reinterpret_cast<uint32*>(0xB7CED8+j*0x190)=uint32(i+j);*reinterpret_cast<float*>(0xB7CEE0+j*0x190)=float(i%9)*.5f;}CWorld::PlayerInFocus=uint8(i%2);CCarCtrl::NumAmbulancesOnDuty=i;CCarCtrl::NumFireTrucksOnDuty=i+3;CTimer::SetTimeInMS(uint32(i*1234));
    hooks[n]->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((address!=0x6CCCF0)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(CPlane*,CEntity*,bool)>(address)(current,damager,i%3!=0);current->m_matrix=nullptr;current->m_pDriver=nullptr;std::ranges::fill(current->m_apPassengers,nullptr);std::ranges::fill(current->m_aCarNodes,nullptr);
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"matrix",Hash(matrix.data(),matrix.size())},{"peds",Hash(peds.data(),sizeof(peds))},{"atomic",Hash(&atomic,sizeof(atomic))},{"stats0",Hash(reinterpret_cast<void*>(0xB7CED8),12)},{"stats1",Hash(reinterpret_cast<void*>(0xB7CED8+0x190),12)},{"focus",CWorld::PlayerInFocus},{"counts",Hash(reinterpret_cast<void*>(0x9690A8),8)},{"time",CTimer::GetTimeInMS()},{"trace",trace}};
'''+s[b:]
s=s.replace(' bool restored=true;', ' for(size_t j=0;j<2;++j)memcpy(reinterpret_cast<void*>(0xB7CED8+j*0x190),savedPlayerStats[j].data(),12);memcpy(reinterpret_cast<void*>(0x9690A8),savedCounts.data(),8);CWorld::PlayerInFocus=savedFocus;CTimer::SetTimeInMS(savedTime);\n bool restored=true;')
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/BikeBlow355',fn).read_text().replace('bike-blow355','plane-blow355').replace('BikeBlow355Live','PlaneBlow355Live').replace('Entity/Vehicle/Bike.cpp','Entity/Vehicle/Plane.cpp').replace('Entity/Vehicle/Bike.h','Entity/Vehicle/Plane.h')
 if fn=='build_probe.py':s=s.replace("'Entity/Vehicle/Vehicle.h'","'Entity/Vehicle/Vehicle.h','Entity/Vehicle/Automobile.cpp','Entity/Vehicle/Automobile.h','DamageManager.cpp','DamageManager.h','FireManager.cpp','FireManager.h','Stats.cpp','Stats.h','PlayerInfo.h','CarCtrl.cpp','CarCtrl.h','RenderWare/rw/rwcore.cpp','RenderWare/rw/rwcore.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('BlowUpCar 0x6CCCF0 VMT871948 slot41:1024 pares pendientes. ASM integro Plane_BlowFull. Gate damage, NPC crash mission immediate/deferred, player credit with focused havoc/chase globals and signed RNGmod6000, current-status reread after Stat mutation, occupants invisible/velocity reset, destruction/body parts/atomic hide, original damager preserved, on-duty counters/fire/explosion (hideExplosion ignored EXE).16 paired deps traced; camera/law/fire mutate position; stat mutates status/model/clock. Full plane/peds/atomic/globals hashes; player stats2x12,counts8,focus/time restored. Scoped hooksfalse until native validation.\n')
