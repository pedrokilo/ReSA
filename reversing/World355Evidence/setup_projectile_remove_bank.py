from pathlib import Path
p=Path('reversing/ProjectileRemove355');p.mkdir(exist_ok=True)
for name in ['build_probe.py','run_local.py']:
 s=Path('reversing/ProjectileCore355',name).read_text(encoding='utf-8-sig').replace('ProjectileCore355','ProjectileRemove355').replace('projectile-core355','projectile-remove355');(p/name).write_text(s,encoding='utf8')
s=Path('reversing/ProjectileCore355/LiveComparison.hpp').read_text(encoding='utf-8-sig').replace('ProjectileCore355Live','ProjectileRemove355Live').replace('projectile-core355','projectile-remove355')
s=s.replace('#include "Explosion.h"','#include "Explosion.h"\n#include "Radar.h"\n#include "Pools/ObjectPool.h"\n#include "AEAudioEntity.h"\n#include "AudioEngine.h"')
a=s.index('inline const std::array<Fn,6>');b=s.index('inline void Update()',a)
s=s[:a]+'''inline const std::array<Fn,3> fns{{
{"CProjectileInfo","RemoveProjectile",0x7388F0,0,-1,512},
{"CProjectileInfo","RemoveAllProjectiles",0x7399B0,0,-1,512},
{"CProjectileInfo","RemoveIfThisIsAProjectile",0x739A40,0,-1,512}
}};
inline uintptr_t objectBase{};inline CPed* player{};
inline size_t Index(CEntity* e){return (uintptr_t(e)-objectBase)/0x19C;}
inline void __cdecl Blip(eBlipType t,int32 handle){trace.push_back({"blip",int(t),handle});}
inline void __cdecl Remove(CEntity* e){trace.push_back({"world",Index(e)});}
inline void* __fastcall Delete(CEntity* e,void*,uint32 flags){trace.push_back({"delete",Index(e),flags});return e;}
inline void __fastcall Audio(void*,void*,CEntity* e){trace.push_back({"audio",Index(e)});}
inline CPed* __cdecl Player(int32 index){trace.push_back({"player",index});return player;}
''' +s[b:]
s=s.replace('std::array<ReversibleHooks::ReversibleHook::Base*,6>','std::array<ReversibleHooks::ReversibleHook::Base*,3>').replace('std::array<bool,6>','std::array<bool,3>').replace('n<6','n<3')
s=s.replace(' uint32 pairs=0,failuresTotal=0;', ''' auto& poolPointer=*reinterpret_cast<CObjectPool**>(0xB7449C);auto* savedPool=poolPointer;
 uint32 pairs=0,failuresTotal=0;''')
s=s.replace(' for(size_t n=0;n<3;++n){const auto& fn=', ''' EntryPatch d(0x587C60,Blip),dd(bit_cast<uintptr_t>(static_cast<void(*)(eBlipType,int32)>(&CRadar::ClearBlipForEntity)),Blip);
 EntryPatch e(0x563280,Remove),ed(bit_cast<uintptr_t>(&CWorld::Remove),Remove);
 EntryPatch f(0x506ED0,Audio),fd(bit_cast<uintptr_t>(&CAudioEngine::ReportObjectDestruction),Audio);
 EntryPatch g(0x56E210,Player),gd(bit_cast<uintptr_t>(&FindPlayerPed),Player);
 for(size_t n=0;n<3;++n){const auto& fn=''')
s=s.replace('std::array<std::array<uint8,sizeof(CProjectile)>,32> objects{};', 'std::array<std::array<uint8,0x19C>,32> objects{};')
s=s.replace('  for(int i=0;i<fn.cases;++i){', '''  alignas(CVehicle) std::array<uint8,sizeof(CVehicle)> creatorVehicle{};alignas(CPed) std::array<uint8,sizeof(CPed)> creatorPed{};
  alignas(16) std::array<uint8,sizeof(CObjectPool)> pool{};std::array<uint8,32> states{};std::array<uintptr_t,20> vt{};vt.fill(uintptr_t(&Delete));
  for(int i=0;i<fn.cases;++i){''')
a=s.index('    memset(gaProjectileInfo.data()');b=s.index('    for(auto* h:hooks)',a)
s=s[:a]+'''    memset(gaProjectileInfo.data(),uint8(i*17),sizeof(gaProjectileInfo));creatorVehicle.fill(0);creatorVehicle[0x36]=2;creatorPed.fill(0);creatorPed[0x36]=3;
    auto* vehicle=reinterpret_cast<CVehicle*>(creatorVehicle.data());player=reinterpret_cast<CPed*>(creatorPed.data());vehicle->m_pDriver=player;
    objectBase=uintptr_t(objects.data());*reinterpret_cast<void**>(pool.data())=objects.data();*reinterpret_cast<void**>(pool.data()+4)=states.data();*reinterpret_cast<int32*>(pool.data()+8)=32;poolPointer=reinterpret_cast<CObjectPool*>(pool.data());
    constexpr std::array<int32,9> weapons{16,18,19,20,21,39,0,17,40};
    for(size_t k=0;k<32;++k){auto& info=gaProjectileInfo[k];objects[k].fill(0);*reinterpret_cast<uintptr_t*>(objects[k].data())=uintptr_t(vt.data());auto* obj=reinterpret_cast<CProjectile*>(objects[k].data());obj->GetPosition()=CVector{float(i-k),float(k)*.25f,float(i%7)};
     states[k]=uint8((i+k)&0x7F);info.m_nWeaponType=uint32(weapons[(i+k)%weapons.size()]);info.m_bActive=(k+i)%4!=0;info.m_pCreator=(i+k)%3==0?nullptr:(i+k)%3==1?static_cast<CEntity*>(vehicle):static_cast<CEntity*>(player);info.m_pFxSystem=(k+i)%3==1?nullptr:reinterpret_cast<FxSystem_c*>(fx[k].data());CProjectileInfo::ms_apProjectile[k]=obj;}
''' +s[b:]
a=s.index('    int32 result=-999;');b=s.index('    auto snapshot=',a)
s=s[:a]+'''    bool result=false;
    if(fn.address==0x7399B0)reinterpret_cast<void(__cdecl*)()>(fn.address)();
    else if(fn.address==0x7388F0)reinterpret_cast<void(__cdecl*)(CProjectileInfo*,CProjectile*)>(fn.address)(&gaProjectileInfo[i%32],CProjectileInfo::ms_apProjectile[i%32]);
    else result=reinterpret_cast<bool(__cdecl*)(CObject*)>(fn.address)(i%5==0?reinterpret_cast<CObject*>(creatorVehicle.data()):CProjectileInfo::ms_apProjectile[i%32]);
''' +s[b:]
s=s.replace('const bool needsTrace=n>=2;', 'const bool needsTrace=n<2;')
s=s.replace(' memcpy(gaProjectileInfo.data(),savedInfo.data(),sizeof(gaProjectileInfo));', ' poolPointer=savedPool;memcpy(gaProjectileInfo.data(),savedInfo.data(),sizeof(gaProjectileInfo));')
(p/'LiveComparison.hpp').write_text(s,encoding='utf8')
