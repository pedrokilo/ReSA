#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "ProjectileInfo.h"
#include "Entity/Object/Projectile.h"
#include "Fx/FxSystem.h"
#include "Fx/FxPrtMult.h"
#include "Weapon.h"
#include "AudioEngine.h"
#include "World.h"
#include "General.h"
#include "../ProbeRuntime.hpp"
namespace ProjectileUpdate355Live {
inline json trace;inline uint32 scenario{},randomCalls{};inline CPed* player{};inline CVehicle* playerVehicle{};
inline uint32 F(float f){return std::bit_cast<uint32>(f);}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline void __fastcall Kill(FxSystem_c* fx,void*){trace.push_back({"kill",uintptr_t(fx)});}
inline bool __fastcall Valid(CPed* ped,void*){trace.push_back({"valid",uintptr_t(ped)});return scenario%4!=0;}
inline bool __fastcall IsPlayer(CPed* ped,void*){trace.push_back({"is_player",uintptr_t(ped)});return scenario%3!=0;}
inline int32 __fastcall Slot(CPed*,void*,eWeaponType type){trace.push_back({"slot",int(type)});return 0;}
inline int32 __cdecl Range(int32 low,int32 high){trace.push_back({"range",low,high});return int32(scenario%12);}
inline int __cdecl Random(){const int value=int((scenario*7919+randomCalls++*14033)%32768);trace.push_back({"random",value});return value;}
inline void __cdecl Fire(float x,float y,float z,float radius,CEntity* creator){trace.push_back({"fire",F(x),F(y),F(z),F(radius),uintptr_t(creator)});}
inline void __fastcall Particle(FxSystem_c* fx,void*,const CVector& position,const CVector& velocity,float time,const FxPrtMult_c& mult,float rot,float light,float limit,bool local){trace.push_back({"particle",uintptr_t(fx),F(position.x),F(position.y),F(position.z),F(velocity.x),F(velocity.y),F(velocity.z),F(time),Digest(&mult,sizeof(mult)),F(rot),F(light),F(limit),local});}
inline void __cdecl Remove(CProjectileInfo* info,CProjectile* projectile){trace.push_back({"remove",uintptr_t(info),uintptr_t(projectile)});info->m_bActive=false;}
inline bool __cdecl Clear(const CVector& a,const CVector& b,bool bu,bool ve,bool pe,bool ob,bool du,bool se,bool ca){trace.push_back({"clear",F(a.x),F(a.y),F(a.z),F(b.x),F(b.y),F(b.z),uintptr_t(CWorld::pIgnoreEntity),bu,ve,pe,ob,du,se,ca});return scenario%3!=0;}
inline CVehicle* __cdecl PlayerVehicle(int32 id,bool remote){trace.push_back({"player_vehicle",id,remote});return playerVehicle;}
inline CPed* __cdecl Player(int32 id){trace.push_back({"player",id});return player;}
inline float __cdecl Evaluate(CEntity* entity,const CVector& origin,const CVector& aim,float tolerance,bool planes,CEntity* preferred){trace.push_back({"evaluate",uintptr_t(entity),F(origin.x),F(origin.y),F(origin.z),F(aim.x),F(aim.y),F(aim.z),F(tolerance),planes,uintptr_t(preferred)});return entity==playerVehicle?.75f:scenario%2?1.f:.25f;}
inline void __fastcall Audio(CAudioEngine*,void*,eAudioEvents event,float volume,float speed){trace.push_back({"audio",int(event),F(volume),F(speed)});}
inline void __fastcall Attach(CPhysical* entity,void*,CPhysical* parent,CVector* offset,CQuaternion* rotation){trace.push_back({"attach",uintptr_t(entity),uintptr_t(parent),uintptr_t(offset),uintptr_t(rotation)});}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }

struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CProjectileInfo","Update",0x738B20,0,-1,8192}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"projectile-update355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"projectile-update355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("projectile-update355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedMS=CTimer::GetTimeInMS();const auto savedStep=CTimer::GetTimeStep();const auto savedIgnore=CWorld::pIgnoreEntity;const auto savedPlayback=StaticRef<uint8>(0xA43088);const auto savedSmoke=StaticRef<FxSystem_c*>(0xA9AE20);
 const auto savedInfos=gaProjectileInfo;const auto savedProjectiles=CProjectileInfo::ms_apProjectile;
 EntryPatch ki(0x4AA3F0,Kill),kid(bit_cast<uintptr_t>(&FxSystem_c::Kill),Kill);
 EntryPatch va(0x5E4220,Valid),vad(bit_cast<uintptr_t>(&CPed::IsPointerValid),Valid);
 EntryPatch ip(0x5DF8F0,IsPlayer),ipd(bit_cast<uintptr_t>(&CPed::IsPlayer),IsPlayer);
 EntryPatch sl(0x5DF200,Slot),sld(bit_cast<uintptr_t>(&CPed::GetWeaponSlot),Slot);
 EntryPatch ra(0x407180,Range),rad(bit_cast<uintptr_t>(static_cast<int32(*)(int32,int32)>(&CGeneral::GetRandomIntInRange)),Range);
 EntryPatch rn(0x821B1E,Random),rnd(bit_cast<uintptr_t>(&std::rand),Random);
 EntryPatch fi(0x565800,Fire),fid(bit_cast<uintptr_t>(&CWorld::SetPedsOnFire),Fire);
 EntryPatch pa(0x4AA440,Particle),pad(bit_cast<uintptr_t>(static_cast<void(FxSystem_c::*)(const CVector&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool)>(&FxSystem_c::AddParticle)),Particle);
 EntryPatch re(0x7388F0,Remove),red(bit_cast<uintptr_t>(&CProjectileInfo::RemoveProjectile),Remove);
 EntryPatch cl(0x56A490,Clear),cld(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),Clear);
 EntryPatch pv(0x56E0D0,PlayerVehicle),pvd(bit_cast<uintptr_t>(&FindPlayerVehicle),PlayerVehicle);
 EntryPatch pl(0x56E210,Player),pld(bit_cast<uintptr_t>(&FindPlayerPed),Player);
 EntryPatch ev(0x73E560,Evaluate),evd(bit_cast<uintptr_t>(&CWeapon::EvaluateTargetForHeatSeekingMissile),Evaluate);
 EntryPatch au(0x506EA0,Audio),aud(bit_cast<uintptr_t>(&CAudioEngine::ReportFrontendAudioEvent),Audio);
 EntryPatch at(0x54D690,Attach),atd(bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(CPhysical*,CVector*,CQuaternion*)>(&CPhysical::AttachEntityToEntity)),Attach);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();randomCalls=0;
    alignas(CProjectile) std::array<uint8,sizeof(CProjectile)> objects[32]{};alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrices[34]{};alignas(16) std::array<uint8,4096> creator{},victim{};alignas(16) std::array<uint8,64> fx{};
    const auto setMatrix=[&](void* entity,int k,CVector position){At<uintptr_t>(entity,0x14)=uintptr_t(matrices[k].data());At<CVector>(matrices[k].data(),0)=CVector{1,0,0};At<CVector>(matrices[k].data(),0x10)=CVector{0,1,0};At<CVector>(matrices[k].data(),0x20)=CVector{0,0,1};At<CVector>(matrices[k].data(),0x30)=position;};
    gaProjectileInfo.fill({});CProjectileInfo::ms_apProjectile.fill(nullptr);
    for(int k=0;k<32;++k){CProjectileInfo::ms_apProjectile[k]=reinterpret_cast<CProjectile*>(objects[k].data());setMatrix(objects[k].data(),k,CVector{float(k),2,3});objects[k][0x36]=4;}
    constexpr std::array<uint32,9> types{16,17,18,19,20,21,39,58,22};const uint32 type=types[size_t(i%9)];const size_t index=size_t((i/9)%32),decoy=(index+1)%32;
    creator[0x36]=uint8(type==39?2:i%2?3:2);victim[0x36]=2;setMatrix(creator.data(),32,CVector{float(i%5),2,3});setMatrix(victim.data(),33,CVector{float(i%101)-50,5,6});At<int32>(creator.data(),0x5A0)=i%3?40:1;At<int32>(creator.data(),0x5AC)=i%4?2:0;At<int32>(victim.data(),0x594)=i%4?4:1;
    At<CVector>(victim.data(),0x44)=CVector{float(i%9)*.25f,float(i%5)*.125f,0};player=i%3==0?reinterpret_cast<CPed*>(creator.data()):reinterpret_cast<CPed*>(victim.data());playerVehicle=i%3==1?reinterpret_cast<CVehicle*>(creator.data()):reinterpret_cast<CVehicle*>(victim.data());
    auto& info=gaProjectileInfo[index];info.m_bActive=true;info.m_nWeaponType=type;info.m_pCreator=i%5==0&&type!=39?nullptr:reinterpret_cast<CEntity*>(creator.data());info.m_pVictim=i%4?reinterpret_cast<CEntity*>(victim.data()):nullptr;info.m_nDestroyTime=i%3==0?900:i%3==1?1100:0;info.m_vecLastPosn=CVector{float(i%5),float(i%7),float(i%3)};info.m_pFxSystem=i%3?reinterpret_cast<FxSystem_c*>(fx.data()):nullptr;
    auto* projectile=objects[index].data();At<uint32>(projectile,0x40)=uint32(i%4==0?0x100:i%4==1?0x200:0);At<uint32>(projectile,0x1C)=0xABCD0101;At<CVector>(projectile,0x44)=CVector{float(i%21)*.625f,float(i%13)*.25f,float(i%7)*.125f};At<float>(projectile,0x9C)=i%2?.2f:.05f;At<uint8>(projectile,0xB9)=uint8(i%3);At<CEntity*>(projectile,0xBC)=i%4==0?nullptr:i%4==1?reinterpret_cast<CEntity*>(creator.data()):reinterpret_cast<CEntity*>(victim.data());At<int16>(victim.data(),0x22)=int16(i%2?345:411);At<float>(projectile,0xD8)=i%2?1.f:0.f;At<CPhysical*>(projectile,0xDC)=reinterpret_cast<CPhysical*>(creator.data());At<CPhysical*>(projectile,0xFC)=i%3==0?reinterpret_cast<CPhysical*>(victim.data()):nullptr;
    if(type==20&&i%2){auto& distractor=gaProjectileInfo[decoy];distractor.m_bActive=true;distractor.m_nWeaponType=58;distractor.m_nDestroyTime=1100;distractor.m_vecLastPosn=CVector{1,2,3};}
    StaticRef<uint8>(0xA43088)=i%17==0?1:0;StaticRef<uint32>(0xB7CB84)=1000;StaticRef<float>(0xB7CB5C)=.5f+float(i%5)*.25f;StaticRef<FxSystem_c*>(0xA9AE20)=reinterpret_cast<FxSystem_c*>(fx.data());CWorld::pIgnoreEntity=reinterpret_cast<CEntity*>(victim.data());
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__cdecl*)()>(fn.address)();
    json velocities=json::array(),forwards=json::array();for(size_t k=0;k<32;++k){const auto& v=At<CVector>(objects[k].data(),0x44);const auto& f=At<CVector>(matrices[k].data(),0x10);velocities.push_back({F(v.x),F(v.y),F(v.z)});forwards.push_back({F(f.x),F(f.y),F(f.z)});}
    return json{{"velocities",velocities},{"forwards",forwards},{"objects",Digest(objects,sizeof(objects))},{"matrices",Digest(matrices,sizeof(matrices))},{"creator",Digest(creator.data(),creator.size())},{"victim",Digest(victim.data(),victim.size())},{"infos",Digest(gaProjectileInfo.data(),sizeof(gaProjectileInfo))},{"ignore",uintptr_t(CWorld::pIgnoreEntity)},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 gaProjectileInfo=savedInfos;CProjectileInfo::ms_apProjectile=savedProjectiles;StaticRef<uint32>(0xB7CB84)=savedMS;StaticRef<float>(0xB7CB5C)=savedStep;CWorld::pIgnoreEntity=savedIgnore;StaticRef<uint8>(0xA43088)=savedPlayback;StaticRef<FxSystem_c*>(0xA9AE20)=savedSmoke;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
