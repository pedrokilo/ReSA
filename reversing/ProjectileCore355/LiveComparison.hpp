#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "ProjectileInfo.h"
#include "Entity/Object/Projectile.h"
#include "Fx/FxManager.h"
#include "Fx/FxSystem.h"
#include "Explosion.h"
#include "../ProbeRuntime.hpp"
namespace ProjectileCore355Live {
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{};inline std::ofstream log;inline json trace;inline uintptr_t base{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint32 F(float f){return std::bit_cast<uint32>(f);}
inline void __fastcall Destroy(void*,void*,FxSystem_c* fx){trace.push_back({"destroy",uintptr_t(fx)-base});}
inline void __fastcall Kill(FxSystem_c* fx,void*){trace.push_back({"kill",uintptr_t(fx)-base});}
inline void __cdecl Explosion(CEntity* victim,CEntity* creator,eExplosionType type,CVector pos,uint32 lifetime,uint8 sound,float shake,uint8 visible){trace.push_back({"explosion",uintptr_t(victim),uintptr_t(creator),int(type),F(pos.x),F(pos.y),F(pos.z),lifetime,sound,F(shake),visible});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* name){ReversibleHooks::ReversibleHook::Base* out{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CProjectileInfo")for(auto& h:c.Items())if(h->Name()==name)out=h.get();});return out;}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,6> fns{{
{"CProjectileInfo","Initialise",0x737B40,0,-1,512},
{"CProjectileInfo","GetProjectileInfo",0x737BF0,0,-1,512},
{"CProjectileInfo","RemoveFXSystem",0x737B80,0,-1,512},
{"CProjectileInfo","Shutdown",0x737BC0,0,-1,512},
{"CProjectileInfo","RemoveNotAdd",0x737C00,0,-1,512},
{"CProjectileInfo","RemoveDetonatorProjectiles",0x738860,0,-1,512}
}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"projectile-core355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"projectile-core355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("projectile-core355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 const auto savedInfo=gaProjectileInfo;const auto savedObjects=CProjectileInfo::ms_apProjectile;
 std::array<ReversibleHooks::ReversibleHook::Base*,6> hooks{};std::array<bool,6> enabled{},locked{};
 for(size_t n=0;n<6;++n){hooks[n]=FindHook(fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 uint32 pairs=0,failuresTotal=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x4A9810,Destroy),ad(bit_cast<uintptr_t>(&FxManager_c::DestroyFxSystem),Destroy);
 EntryPatch b(0x4AA3F0,Kill),bd(bit_cast<uintptr_t>(&FxSystem_c::Kill),Kill);
 EntryPatch c(0x736A50,Explosion),cd(bit_cast<uintptr_t>(&CExplosion::AddExplosion),Explosion);
 for(size_t n=0;n<6;++n){const auto& fn=fns[n];uint32 failures=0,positive=0;
  alignas(16) std::array<std::array<uint8,sizeof(CProjectile)>,32> objects{};
  alignas(16) std::array<std::array<uint8,sizeof(FxSystem_c)>,32> fx{};
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){trace=json::array();base=uintptr_t(fx.data());
    memset(gaProjectileInfo.data(),uint8(i*17),sizeof(gaProjectileInfo));
    for(size_t k=0;k<32;++k){auto& info=gaProjectileInfo[k];objects[k].fill(0);auto* obj=reinterpret_cast<CProjectile*>(objects[k].data());obj->GetPosition()=CVector{float(i-k),float(k)*.25f,float(i%7)};
     info.m_nWeaponType=(k+i)%3==0?WEAPON_REMOTE_SATCHEL_CHARGE:WEAPON_GRENADE;info.m_bActive=(k+i)%4!=0;info.m_pCreator=reinterpret_cast<CEntity*>(0x100000+uint32(k)*256);info.m_pFxSystem=(k+i)%3==1?nullptr:reinterpret_cast<FxSystem_c*>(fx[k].data());CProjectileInfo::ms_apProjectile[k]=obj;}
    for(auto* h:hooks)h->State(candidate);const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    int32 result=-999;
    if(fn.address==0x737B40||fn.address==0x737BC0||fn.address==0x738860)reinterpret_cast<void(__cdecl*)()>(fn.address)();
    else if(fn.address==0x737BF0){const int32 index=(i%36)-2;result=int32((uintptr_t(reinterpret_cast<CProjectileInfo*(__cdecl*)(int32)>(fn.address)(index))-uintptr_t(gaProjectileInfo.data()))/sizeof(CProjectileInfo));}
    else if(fn.address==0x737B80)reinterpret_cast<void(__thiscall*)(CProjectileInfo*,bool)>(fn.address)(&gaProjectileInfo[i%32],(i/32)%2!=0);
    else {constexpr std::array<int,10> weapons{16,39,18,19,20,0,17,21,40,-1};reinterpret_cast<void(__cdecl*)(CEntity*,eWeaponType,CVector)>(fn.address)(reinterpret_cast<CEntity*>(0x123450),eWeaponType(weapons[i%10]),CVector{float(i)*.125f,-float(i),float(i%13)});}
    auto snapshot=json{{"pointers",Digest(CProjectileInfo::ms_apProjectile.data(),sizeof(CProjectileInfo::ms_apProjectile))},{"info",Digest(gaProjectileInfo.data(),sizeof(gaProjectileInfo))},{"objects",Digest(objects.data(),sizeof(objects))},{"trace",trace},{"result",result},{"switches",switched}};return snapshot;
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;failures+=!ok;if(!original["trace"].empty())++positive;
   Write({{"event","comparison"},{"pair",pairs++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  const bool needsTrace=n>=2;if(needsTrace&&!positive)++failures;failuresTotal+=failures;
  Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"positive_trace",positive}});
 }
 }
 memcpy(gaProjectileInfo.data(),savedInfo.data(),sizeof(gaProjectileInfo));CProjectileInfo::ms_apProjectile=savedObjects;
 for(size_t n=0;n<6;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);}
 Write({{"event",failuresTotal?"error":"complete"},{"pairs",pairs},{"state_restored",memcmp(gaProjectileInfo.data(),savedInfo.data(),sizeof(gaProjectileInfo))==0&&CProjectileInfo::ms_apProjectile==savedObjects}});
}
}
