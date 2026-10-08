from pathlib import Path
p=Path('reversing/PlaneAmbient355');p.mkdir(exist_ok=True)
s=Path('reversing/MonsterControl355/LiveComparison.hpp').read_text().replace('MonsterControl355Live','PlaneAmbient355Live').replace('monster-control355','plane-ambient355').replace('#include "MonsterTruck.h"','#include "Plane.h"\n#include "VehiclePool.h"')
a=s.index('inline void __fastcall BaseControl');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''inline uint8* storage{};
inline int Index(void* p){return int((static_cast<uint8*>(p)-storage)/0xA18);}
inline void __cdecl Remove(CEntity* p){trace.push_back({"remove",Index(p)});}
inline void __fastcall Delete(void* p,void*,uint32 flags){trace.push_back({"delete",Index(p),flags});}
'''+s[b:]
a=s.index('inline const std::array<Fn,4>');b=s.index('inline void Update()',a)
s=s[:a]+'inline const std::array<Fn,1> fns{{{"CPlane","SwitchAmbientPlanes",0x6CCC50,0,-1,1024}}};\n'+s[b:]
s=s.replace('Base*,4>','Base*,1>').replace('bool,4>','bool,1>').replace('n<4','n<1')
a=s.index(' const auto oldStep=');b=s.index(' for(size_t n=',a)
s=s[:a]+''' const auto oldActive=CPlane::GenPlane_Active;auto& poolSlot=StaticRef<CVehiclePool*>(0xB74494);const auto oldPool=poolSlot;uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch r(0x563280,Remove),rd(bit_cast<uintptr_t>(&CWorld::Remove),Remove);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,0xA18*8> objects{};std::array<uint8,8> flags{};std::array<uintptr_t,6> pool{};std::array<uintptr_t,71> vtable{};vtable[0]=bit_cast<uintptr_t>(&Delete);storage=objects.data();
   auto run=[&](bool candidate){trace=json::array();memset(objects.data(),0xCD,objects.size());const auto count=i%9;
    pool={bit_cast<uintptr_t>(storage),bit_cast<uintptr_t>(flags.data()),uintptr_t(count),0,0,0};poolSlot=reinterpret_cast<CVehiclePool*>(pool.data());
    CPlane::GenPlane_Active=(i/9)%2;
    for(int j=0;j<8;++j){auto* v=reinterpret_cast<CVehicle*>(storage+j*0xA18);*reinterpret_cast<uintptr_t*>(v)=bit_cast<uintptr_t>(vtable.data());v->m_nVehicleSubType=static_cast<eVehicleType>((i+j)%6);v->m_nCreatedBy=static_cast<eVehicleCreatedBy>((i/3+j)%3);flags[j]=uint8((i+j)%3?0:0x80);}
    hooks[n]->State(candidate);const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;if(!switched)return json{{"switch",false}};
    reinterpret_cast<void(__cdecl*)(bool)>(fn.address)((i/18)%2);
    return json{{"switch",true},{"active",CPlane::GenPlane_Active},{"trace",trace},{"flags",Hash(flags.data(),flags.size())}};
'''+s[b:]
a=s.index(' CTimer::ms_fTimeStep=');b=s.index(' for(size_t n=',a)
s=s[:a]+' CPlane::GenPlane_Active=oldActive;poolSlot=oldPool;bool restored=CPlane::GenPlane_Active==oldActive&&poolSlot==oldPool;\n'+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/MonsterControl355',fn).read_text().replace('MonsterControl355Live','PlaneAmbient355Live').replace('monster-control355','plane-ambient355').replace('Monster control 355','Plane ambient 355')
 if fn=='build_probe.py':
  a=s.index('SOURCES=');b=s.index("if __name__",a);s=s[:a]+"SOURCES=[G+p for p in ('Entity/Vehicle/Plane.cpp','Entity/Vehicle/Plane.h','Entity/Vehicle/Vehicle.h','Core/Pool.h','Pools/VehiclePool.h','World.cpp','World.h')]\n"+s[b:]
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('SwitchAmbientPlanes 0x6CCC50: 1.024 pares preparados, pendientes de ejecución. Pool aislado de 0-8 slots con subtipos 0-5, createdBy 0-2, slots libres/usados y activación previa/nueva. Remove y destructor virtual registran traza simétrica, orden inverso e indicador delete=1. Restaura pool y GenPlane_Active. Evidencia combo ../VehicleAudio355Evidence/Plane_6CCC50.txt.\n')
