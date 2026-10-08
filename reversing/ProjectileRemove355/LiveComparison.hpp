#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "ProjectileInfo.h"
#include "Entity/Object/Projectile.h"
#include "Fx/FxManager.h"
#include "Fx/FxSystem.h"
#include "Explosion.h"
#include "Radar.h"
#include "Pools/ObjectPool.h"
#include "AudioEngine.h"
#include "../ProbeRuntime.hpp"
namespace ProjectileRemove355Live {
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{};inline std::ofstream log;inline json trace;inline uintptr_t base{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint32 F(float f){return std::bit_cast<uint32>(f);}
inline void __fastcall Destroy(void*,void*,FxSystem_c* fx){trace.push_back({"destroy",uintptr_t(fx)-base});}
inline void __fastcall Kill(FxSystem_c* fx,void*){trace.push_back({"kill",uintptr_t(fx)-base});}
inline void __cdecl Explosion(CEntity* victim,CEntity* creator,eExplosionType type,CVector pos,uint32 lifetime,uint8 sound,float shake,uint8 visible){trace.push_back({"explosion",uintptr_t(victim),uintptr_t(creator),int(type),F(pos.x),F(pos.y),F(pos.z),lifetime,sound,F(shake),visible});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* name){ReversibleHooks::ReversibleHook::Base* out{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CProjectileInfo")for(auto& h:c.Items())if(h->Name()==name)out=h.get();});return out;}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,3> fns{{
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
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"projectile-remove355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"projectile-remove355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("projectile-remove355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 const auto savedInfo=gaProjectileInfo;const auto savedObjects=CProjectileInfo::ms_apProjectile;
 std::array<ReversibleHooks::ReversibleHook::Base*,3> hooks{};std::array<bool,3> enabled{},locked{};
 for(size_t n=0;n<3;++n){hooks[n]=FindHook(fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 auto& poolPointer=*reinterpret_cast<CObjectPool**>(0xB7449C);auto* savedPool=poolPointer;
 uint32 pairs=0,failuresTotal=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x4A9810,Destroy),ad(bit_cast<uintptr_t>(&FxManager_c::DestroyFxSystem),Destroy);
 EntryPatch b(0x4AA3F0,Kill),bd(bit_cast<uintptr_t>(&FxSystem_c::Kill),Kill);
 EntryPatch c(0x736A50,Explosion),cd(bit_cast<uintptr_t>(&CExplosion::AddExplosion),Explosion);
 EntryPatch d(0x587C60,Blip),dd(bit_cast<uintptr_t>(static_cast<void(*)(eBlipType,int32)>(&CRadar::ClearBlipForEntity)),Blip);
 EntryPatch e(0x563280,Remove),ed(bit_cast<uintptr_t>(&CWorld::Remove),Remove);
 EntryPatch f(0x506ED0,Audio),fd(bit_cast<uintptr_t>(&CAudioEngine::ReportObjectDestruction),Audio);
 EntryPatch g(0x56E210,Player),gd(bit_cast<uintptr_t>(&FindPlayerPed),Player);
 for(size_t n=0;n<3;++n){const auto& fn=fns[n];uint32 failures=0,positive=0;
  alignas(16) std::array<std::array<uint8,0x19C>,32> objects{};
  alignas(16) std::array<std::array<uint8,sizeof(FxSystem_c)>,32> fx{};
  alignas(CVehicle) std::array<uint8,sizeof(CVehicle)> creatorVehicle{};alignas(CPed) std::array<uint8,sizeof(CPed)> creatorPed{};
  alignas(16) std::array<uint8,sizeof(CObjectPool)> pool{};std::array<uint8,32> states{};std::array<uintptr_t,20> vt{};vt.fill(uintptr_t(&Delete));
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){trace=json::array();base=uintptr_t(fx.data());
    memset(gaProjectileInfo.data(),uint8(i*17),sizeof(gaProjectileInfo));creatorVehicle.fill(0);creatorVehicle[0x36]=2;creatorPed.fill(0);creatorPed[0x36]=3;
    auto* vehicle=reinterpret_cast<CVehicle*>(creatorVehicle.data());player=reinterpret_cast<CPed*>(creatorPed.data());vehicle->m_pDriver=player;
    objectBase=uintptr_t(objects.data());*reinterpret_cast<void**>(pool.data())=objects.data();*reinterpret_cast<void**>(pool.data()+4)=states.data();*reinterpret_cast<int32*>(pool.data()+8)=32;poolPointer=reinterpret_cast<CObjectPool*>(pool.data());
    constexpr std::array<int32,9> weapons{16,18,19,20,21,39,0,17,40};
    for(size_t k=0;k<32;++k){auto& info=gaProjectileInfo[k];objects[k].fill(0);*reinterpret_cast<uintptr_t*>(objects[k].data())=uintptr_t(vt.data());auto* obj=reinterpret_cast<CProjectile*>(objects[k].data());obj->GetPosition()=CVector{float(i-k),float(k)*.25f,float(i%7)};
     states[k]=uint8((i+k)&0x7F);info.m_nWeaponType=uint32(weapons[(i+k)%weapons.size()]);info.m_bActive=(k+i)%4!=0;info.m_pCreator=(i+k)%3==0?nullptr:(i+k)%3==1?static_cast<CEntity*>(vehicle):static_cast<CEntity*>(player);info.m_pFxSystem=(k+i)%3==1?nullptr:reinterpret_cast<FxSystem_c*>(fx[k].data());CProjectileInfo::ms_apProjectile[k]=obj;}
    for(auto* h:hooks)h->State(candidate);const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    bool result=false;
    if(fn.address==0x7399B0)reinterpret_cast<void(__cdecl*)()>(fn.address)();
    else if(fn.address==0x7388F0)reinterpret_cast<void(__cdecl*)(CProjectileInfo*,CProjectile*)>(fn.address)(&gaProjectileInfo[i%32],CProjectileInfo::ms_apProjectile[i%32]);
    else result=reinterpret_cast<bool(__cdecl*)(CObject*)>(fn.address)(i%5==0?reinterpret_cast<CObject*>(creatorVehicle.data()):CProjectileInfo::ms_apProjectile[i%32]);
    auto snapshot=json{{"pointers",Digest(CProjectileInfo::ms_apProjectile.data(),sizeof(CProjectileInfo::ms_apProjectile))},{"info",Digest(gaProjectileInfo.data(),sizeof(gaProjectileInfo))},{"objects",Digest(objects.data(),sizeof(objects))},{"trace",trace},{"result",result},{"switches",switched}};return snapshot;
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;failures+=!ok;if(!original["trace"].empty())++positive;
   Write({{"event","comparison"},{"pair",pairs++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  const bool needsTrace=n<2;if(needsTrace&&!positive)++failures;failuresTotal+=failures;
  Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"positive_trace",positive}});
 }
 }
 poolPointer=savedPool;memcpy(gaProjectileInfo.data(),savedInfo.data(),sizeof(gaProjectileInfo));CProjectileInfo::ms_apProjectile=savedObjects;
 for(size_t n=0;n<3;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);}
 Write({{"event",failuresTotal?"error":"complete"},{"pairs",pairs},{"state_restored",memcmp(gaProjectileInfo.data(),savedInfo.data(),sizeof(gaProjectileInfo))==0&&CProjectileInfo::ms_apProjectile==savedObjects}});
}
}
