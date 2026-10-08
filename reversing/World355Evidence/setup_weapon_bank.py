from pathlib import Path
p=Path('reversing/WeaponDrive355');p.mkdir(exist_ok=True)
for name in ['build_probe.py','run_local.py']:
 s=Path('reversing/ProjectileCore355',name).read_text(encoding='utf-8-sig').replace('ProjectileCore355','WeaponDrive355').replace('projectile-core355','weapon-drive355')
 if name=='build_probe.py':s=s.replace("'source/game_sa/ProjectileInfo.cpp', 'source/game_sa/ProjectileInfo.h', 'source/game_sa/Entity/Object/Projectile.h'","'source/game_sa/Weapon.cpp', 'source/game_sa/Weapon.h', 'source/game_sa/Entity/Vehicle/Vehicle.h', 'source/game_sa/Collision/Collision.h'")
 (p/name).write_text(s,encoding='utf8')
s=Path('reversing/DecisionPending/LiveComparison.hpp').read_text().replace('DecisionPendingLive','WeaponDrive355Live').replace('decision-pending','weapon-drive355').replace('#include "DecisionMakers/Decision.h"','#include "Weapon.h"\n#include "Collision/Collision.h"')
start=s.index('inline const std::array<Fn,2>');end=s.index('inline ReversibleHooks',start)
s=s[:start]+'inline const std::array<Fn,1> fns{{{"CWeapon","DoDriveByAutoAiming",0x73D720,0,-1,1024}}};\n'+s[end:]
s=s.replace('inline bool done{};', '''inline json trace;inline uint32 scenario{};inline std::array<CEntity*,4> entities{};
inline void __cdecl Find(const CVector& p,float radius,bool twoD,int16* count,int16 maximum,CEntity** out,bool buildings,bool vehicles,bool peds,bool objects,bool dummies){trace.push_back({"find",std::bit_cast<uint32>(radius),twoD,maximum,buildings,vehicles,peds,objects,dummies});*count=int16((scenario/2)%4);for(int16 k=0;k<*count;++k)out[k]=entities[k];}
inline float __cdecl Dist(const CVector& a,const CVector& b,const CVector& p){trace.push_back({"dist",std::bit_cast<uint32>(p.x),std::bit_cast<uint32>(p.y),std::bit_cast<uint32>(p.z)});return float((scenario+uint32(int(p.x)))%7)*.125f;}
inline float __fastcall Angle(CVehicle*,void*){trace.push_back({"angle"});constexpr std::array<float,5> values{0.f,.5f,1.f,30.f,60.f};return values[(scenario/8)%5];}
inline bool done{};''').replace('inline bool done{}; inline std::ofstream log; inline json trace;','inline bool done{}; inline std::ofstream log;')
s=s.replace(' bool passed=true;uint32 pairCount=0;',''' bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x564A20,Find),ad(bit_cast<uintptr_t>(&CWorld::FindObjectsInRange),Find);
 EntryPatch b(0x417610,Dist),bd(bit_cast<uintptr_t>(&CCollision::DistToLine),Dist);
 EntryPatch c(0x6D3E00,Angle),cd(bit_cast<uintptr_t>(&CVehicle::GetPlaneGunsAutoAimAngle),Angle);''')
start=s.index('    alignas(CDecision)');end=s.index('\n   };',start)
s=s[:start]+'''    alignas(CVehicle) std::array<uint8,sizeof(CVehicle)> vehicle{};auto* veh=reinterpret_cast<CVehicle*>(vehicle.data());veh->m_nVehicleSubType=eVehicleType((i/32)%5);veh->GetPosition()=CVector{0.f,0.f,0.f};
    alignas(CPed) std::array<std::array<uint8,sizeof(CPed)>,4> raw{};
    for(size_t k=0;k<4;++k){auto* ped=reinterpret_cast<CPed*>(raw[k].data());raw[k][0x36]=uint8((i+k)%2?3:2);ped->m_nPedState=(i+k)%5==0?PEDSTATE_DEAD:(i+k)%5==1?PEDSTATE_DIE:PEDSTATE_IDLE;ped->m_pVehicle=(i+k)%7==0?veh:nullptr;ped->GetPosition()=CVector{float(k+1)*float(i%5+1),float(int(k)-2)*.125f,float(i%3)*.0625f};entities[k]=ped;}
    auto* owner=entities[3];CVector start{0.f,0.f,0.f},end{float(i%2?20:-20),float(i%7)*.125f,float(i%11)*.0625f};
    scenario=uint32(i);trace=json::array();const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    reinterpret_cast<void(__cdecl*)(CEntity*,CVehicle*,CVector*,CVector*,bool)>(fn.address)(i%13==0?nullptr:owner,veh,&start,&end,i%2!=0);
    return json{{"start",ReversingProbe::Bytes(&start,sizeof(start))},{"end",ReversingProbe::Bytes(&end,sizeof(end))},{"vehicle",ReversingProbe::Bytes(vehicle.data(),vehicle.size())},{"entities",ReversingProbe::Bytes(raw.data(),sizeof(raw))},{"trace",trace},{"switches",switched}};''' +s[end:]
(p/'LiveComparison.hpp').write_text(s,encoding='utf8')
