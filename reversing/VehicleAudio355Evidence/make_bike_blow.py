from pathlib import Path
p=Path('reversing/BikeBlow355');p.mkdir(exist_ok=True)
s=Path('reversing/BikeRoad355/LiveComparison.hpp').read_text().replace('BikeRoad355Live','BikeBlow355Live').replace('bike-road355','bike-blow355').replace('#include "World.h"','#include "World.h"\n#include "Camera.h"\n#include "Explosion.h"\n#include "Darkel.h"\n#include "VisibilityPlugins.h"')
a=s.index('inline int scenario');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''inline CBike* current{};inline CEntity* damager{};
inline void __cdecl Clump(RpClump*,int32 flags){trace.push_back({"clump",flags});}
inline void __fastcall Shake(void*,void*,float strength,CVector position){trace.push_back({"shake",std::bit_cast<uint32>(strength),std::bit_cast<uint32>(position.x),std::bit_cast<uint32>(position.y),std::bit_cast<uint32>(position.z)});current->GetPosition().z+=.5f;}
inline void __fastcall Kill(void*,void*){trace.push_back({"kill"});reinterpret_cast<uint8*>(current)[0x428]=0xFF;reinterpret_cast<uint8*>(current)[0x4A8]=0xFF;}
inline void __fastcall Law(void*,void*,bool state){trace.push_back({"law",state});current->GetPosition().x+=.25f;}
inline void __cdecl Explosion(CEntity* victim,CEntity* creator,eExplosionType type,CVector position,uint32 lifetime,uint8 sound,float shake,uint8 visible){trace.push_back({"explosion",victim==current,creator==damager,int(type),std::bit_cast<uint32>(position.x),std::bit_cast<uint32>(position.y),std::bit_cast<uint32>(position.z),lifetime,sound,std::bit_cast<uint32>(shake),visible});}
inline void __cdecl Darkel(CVehicle& vehicle,int32 player){trace.push_back({"darkel",&vehicle==current,player});}
'''+s[b:]
s=s.replace('{"CBike","PlaceOnRoadProperly",0x6BEEB0,0,-1,1024}','{"CBike","BlowUpCar",0,0x871360,41,1024}')
a=s.index(' EntryPatch c(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch cl(0x7322F0,Clump),cld(bit_cast<uintptr_t>(&CVisibilityPlugins::SetClumpForAllAtomicsFlag),Clump);
 EntryPatch sh(0x50A9F0,Shake),shd(bit_cast<uintptr_t>(&CCamera::CamShake),Shake);
 EntryPatch ki(0x6D1C80,Kill),kid(bit_cast<uintptr_t>(&CVehicle::KillPedsInVehicle),Kill);
 EntryPatch la(0x6D2330,Law),lad(bit_cast<uintptr_t>(&CVehicle::ChangeLawEnforcerState),Law);
 EntryPatch ex(0x736A50,Explosion),exd(bit_cast<uintptr_t>(&CExplosion::AddExplosion),Explosion);
 EntryPatch da(0x43DF20,Darkel),dad(bit_cast<uintptr_t>(&CDarkel::RegisterCarBlownUpByPlayer),Darkel);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original=',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{};alignas(16) std::array<uint8,0x80> matrix{};alignas(16) std::array<uint8,sizeof(CEntity)> offender{};
   auto run=[&](bool candidate){trace=json::array();memset(object.data(),0xCD,object.size());memset(matrix.data(),0,matrix.size());memset(offender.data(),0,offender.size());current=reinterpret_cast<CBike*>(object.data());damager=i%3==0?nullptr:reinterpret_cast<CEntity*>(offender.data());*reinterpret_cast<uintptr_t*>(current)=0x871360;current->m_matrix=i%2?reinterpret_cast<CMatrixLink*>(matrix.data()):nullptr;*reinterpret_cast<CVector*>(matrix.data()+0x30)={float(i%7),float(i%11),float(i%13)*.25f};current->GetPosition()={float(i%7),float(i%11),float(i%13)*.25f};current->m_vecMoveSpeed={.25f,-.5f,float(i%17-8)*.125f};object[0x42A]=uint8((i%5==0?0:0x20)|uint8(i%8));object[0x36]=uint8(i%64);current->m_nPhysicalFlags=uint32(i*123);current->m_fHealth=float(i%19+1);current->m_wBombTimer=uint16(i+1);*reinterpret_cast<void**>(object.data()+0x18)=nullptr;
    hooks[n]->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((address!=0x6BEA10)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(CBike*,CEntity*,bool)>(address)(current,damager,i%7!=0);current->m_matrix=nullptr;
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"matrix",Hash(matrix.data(),matrix.size())},{"trace",trace}};
'''+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/BikeRoad355',fn).read_text().replace('bike-road355','bike-blow355').replace('BikeRoad355Live','BikeBlow355Live')
 if fn=='build_probe.py':s=s.replace("'Core/Vector.h'","'Core/Vector.h','Camera.cpp','Camera.h','Explosion.cpp','Explosion.h','Darkel.cpp','Darkel.h','VisibilityPlugins.cpp','VisibilityPlugins.h','Entity/Vehicle/Vehicle.cpp','Entity/Vehicle/Vehicle.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('BlowUpCar 0x6BEA10 VMT871360 slot41:1024 pares pendientes. ASM integro Bike_BlowFull. canBeDamaged off/on, status bits/physical flags, matrix or placement, damager null/live, hideExplosion. Paired deps clump/CameraShake/Kill/Law/Explosion/Darkel; shake/law mutate position to check subsequent rereads; Kill changes flags before masks. Full object/matrix hash and call parameters/order. No globals mutated. Hook false until EXE approval.\n')
