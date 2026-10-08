from pathlib import Path
p=Path('reversing/TrainRemove355');p.mkdir(exist_ok=True)
s=Path('reversing/PlaneAmbient355/LiveComparison.hpp').read_text().replace('PlaneAmbient355Live','TrainRemove355Live').replace('plane-ambient355','train-remove355').replace('#include "Plane.h"','#include "Train.h"')
s=s.replace('inline uint8* storage{};', 'inline uint8* storage{};inline uint8* slotFlags{};inline CVehicle* player{};')
s=s.replace('inline void __fastcall Delete(void* p,void*,uint32 flags){trace.push_back({"delete",Index(p),flags});}', '''inline void __fastcall Delete(void* p,void*,uint32 flags){const auto index=Index(p);trace.push_back({"delete",index,flags});slotFlags[index]|=0x80;auto* train=static_cast<CTrain*>(p);if(train->m_pPrevCarriage)train->m_pPrevCarriage->m_pNextCarriage=train->m_pNextCarriage;if(train->m_pNextCarriage)train->m_pNextCarriage->m_pPrevCarriage=train->m_pPrevCarriage;train->m_pPrevCarriage=nullptr;train->m_pNextCarriage=nullptr;}
inline CVehicle* __cdecl Player(int32 id,bool remote){trace.push_back({"player",id,remote,player?Index(player):-1});return player;}''')
s=s.replace('{"CPlane","SwitchAmbientPlanes",0x6CCC50,0,-1,1024}','{"CTrain","RemoveAllTrains",0x6F6AA0,0,-1,1024}')
s=s.replace('const auto oldActive=CPlane::GenPlane_Active;', '')
s=s.replace(' EntryPatch r(0x563280,Remove),rd(bit_cast<uintptr_t>(&CWorld::Remove),Remove);', ' EntryPatch r(0x563280,Remove),rd(bit_cast<uintptr_t>(&CWorld::Remove),Remove);\n EntryPatch p(0x56E0D0,Player),pd(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),Player);')
a=s.index('    CPlane::GenPlane_Active=');b=s.index('    hooks[n]->State',a)
s=s[:a]+'''    slotFlags=flags.data();
    for(int j=0;j<8;++j){auto* t=reinterpret_cast<CTrain*>(storage+j*0xA18);*reinterpret_cast<uintptr_t*>(t)=bit_cast<uintptr_t>(vtable.data());t->m_nVehicleType=(i+j)%7==0?VEHICLE_TYPE_AUTOMOBILE:VEHICLE_TYPE_TRAIN;flags[j]=uint8((i+j)%5?0:0x80);t->m_pPrevCarriage=nullptr;t->m_pNextCarriage=nullptr;}
    for(int j=0;j<count;++j){auto* t=reinterpret_cast<CTrain*>(storage+j*0xA18);if(flags[j]&0x80||!t->IsTrain())continue;if(j>0&&!(flags[j-1]&0x80)&&reinterpret_cast<CTrain*>(storage+(j-1)*0xA18)->IsTrain())t->m_pPrevCarriage=reinterpret_cast<CTrain*>(storage+(j-1)*0xA18);if(j+1<count&&!(flags[j+1]&0x80)&&reinterpret_cast<CTrain*>(storage+(j+1)*0xA18)->IsTrain())t->m_pNextCarriage=reinterpret_cast<CTrain*>(storage+(j+1)*0xA18);}
    const auto pi=(i/9)%10;player=pi<count&&!(flags[pi]&0x80)?reinterpret_cast<CVehicle*>(storage+pi*0xA18):nullptr;
'''+s[b:]
s=s.replace('reinterpret_cast<void(__cdecl*)(bool)>(fn.address)((i/18)%2);', 'reinterpret_cast<void(__cdecl*)()>(fn.address)();')
s=s.replace('{"active",CPlane::GenPlane_Active},', '{"objects",Hash(objects.data(),sizeof(objects))},')
s=s.replace(' CPlane::GenPlane_Active=oldActive;poolSlot=oldPool;bool restored=CPlane::GenPlane_Active==oldActive&&poolSlot==oldPool;', ' poolSlot=oldPool;bool restored=poolSlot==oldPool;')
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/PlaneAmbient355',fn).read_text().replace('PlaneAmbient355Live','TrainRemove355Live').replace('plane-ambient355','train-remove355')
 if fn=='build_probe.py':s=s.replace('Entity/Vehicle/Plane.cpp','Entity/Vehicle/Train.cpp').replace('Entity/Vehicle/Plane.h','Entity/Vehicle/Train.h')
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('RemoveAllTrains 0x6F6AA0: 1.024 pares preparados, pendiente ejecución. Pool 0-8 slots, bloques de vagones enlazados en ambas direcciones, huecos/tipos y jugador nulo/presente. FindPlayerVehicle, Remove y destructor simétricos, destructor fixture desvincula vecinos y marca slot libre. Comprueba recorrido inverso, llamadas repetidas a player (sin cortocircuito), protección de todo consist y conservación de otras entradas. Restaura pool; EXE canónico y combo ../VehicleAudio355Evidence/Train_6F6AA0.txt.\n')
