from pathlib import Path
p=Path('reversing/TrainNearest355');p.mkdir(exist_ok=True)
s=Path('reversing/PlaneAmbient355/LiveComparison.hpp').read_text().replace('PlaneAmbient355Live','TrainNearest355Live').replace('plane-ambient355','train-nearest355').replace('#include "Plane.h"','#include "Train.h"')
a=s.index('inline uint8* storage');b=s.index('inline ReversibleHooks',a);s=s[:a]+s[b:]
s=s.replace('{"CPlane","SwitchAmbientPlanes",0x6CCC50,0,-1,1024}','{"CTrain","FindNearestTrain",0x6F7090,0,-1,1024}')
a=s.index(' const auto oldActive=');b=s.index(' for(size_t n=',a);s=s[:a]+' auto& poolSlot=StaticRef<CVehiclePool*>(0xB74494);const auto oldPool=poolSlot;uint32 pairs=0,totalFailures=0;\n'+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,0xA18*8> objects{};std::array<uint8,8> flags{};std::array<uintptr_t,6> pool{};alignas(16) std::array<std::array<uint8,0x80>,8> matrices{};
   auto run=[&](bool candidate){memset(objects.data(),0xCD,objects.size());memset(matrices.data(),0,sizeof(matrices));const auto count=i%9;auto* storage=objects.data();
    pool={std::bit_cast<uintptr_t>(storage),std::bit_cast<uintptr_t>(flags.data()),uintptr_t(count),0,0,0};poolSlot=reinterpret_cast<CVehiclePool*>(pool.data());
    for(int j=0;j<8;++j){auto* train=reinterpret_cast<CTrain*>(storage+j*0xA18);train->m_nVehicleType=static_cast<eVehicleType>((i+j)%7);train->trainFlags.bIsFrontCarriage=(i/3+j)%2;flags[j]=uint8((i+j)%3?0:0x80);const CVector position{i%11==0?1.f:float((i+j*3)%17-8),i%11==0?2.f:float(j%5-2),float(i+j)};*reinterpret_cast<CVector*>(storage+j*0xA18+4)=position;*reinterpret_cast<CVector*>(matrices[j].data()+0x30)=position;*reinterpret_cast<uintptr_t*>(storage+j*0xA18+0x14)=(i+j)%2?uintptr_t(matrices[j].data()):0;}
    if(i%17==0&&count)*reinterpret_cast<float*>(matrices[count-1].data()+0x30)=std::numeric_limits<float>::quiet_NaN();
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    const auto* ret=reinterpret_cast<CTrain*(__cdecl*)(CVector,bool)>(fn.address)(CVector{float(i%13-6),float(i%7-3),float(i%19)},(i/9)%2);
    const auto index=ret?int32((reinterpret_cast<const uint8*>(ret)-storage)/0xA18)+1:0;
    return json{{"switch",true},{"return",index},{"objects",Hash(objects.data(),sizeof(objects))},{"matrices",Hash(matrices.data(),sizeof(matrices))},{"flags",Hash(flags.data(),flags.size())}};
'''+s[b:]
s=s.replace(' }\n }\n CPlane::GenPlane_Active=oldActive;poolSlot=oldPool;bool restored=CPlane::GenPlane_Active==oldActive&&poolSlot==oldPool;', ' }\n poolSlot=oldPool;bool restored=poolSlot==oldPool;')
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/PlaneAmbient355',fn).read_text().replace('PlaneAmbient355Live','TrainNearest355Live').replace('plane-ambient355','train-nearest355')
 if fn=='build_probe.py':s=s.replace('Entity/Vehicle/Plane.cpp','Entity/Vehicle/Train.cpp').replace('Entity/Vehicle/Plane.h','Entity/Vehicle/Train.h')
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('FindNearestTrain 0x6F7090: 1.024 pares preparados, pendiente ejecución. Pool aislado 0-8 slots libres/usados, tipos 0-6, filtros front carriage, posición placeable/matriz, empates y NaN. Distancia planar en orden original y recorrido inverso; retorna identidad normalizada y compara bytes conservados. Restaura pool. Evidencia combo ../VehicleAudio355Evidence/Train_6F7090.txt.\n')
