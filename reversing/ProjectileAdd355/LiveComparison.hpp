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
#include "WeaponInfo.h"
#include "ColModel.h"
#include "CollisionData.h"
#include "Camera.h"
#include "MemoryMgr.h"
#include "Streaming.h"
#include "Radar.h"
#include "Fx/FxManager.h"
#include "../ProbeRuntime.hpp"
namespace ProjectileAdd355Live {
inline json trace;inline uint32 scenario{};inline CProjectile* object{};inline CMatrix* objectMatrix{};inline CWeaponInfo* weaponInfo{};inline CPlayerPed* player{};inline CVehicle* playerVehicle{};inline CCollisionData* collisionData{};inline CColSphere* collisionSphere{};inline FxSystem_c* fx{};
inline uint32 F(float v){return std::bit_cast<uint32>(v);}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline void* __cdecl Allocate(size_t size){trace.push_back({"allocate",size});return scenario%13==0?nullptr:object;}
inline CProjectile* __fastcall Construct(CProjectile* target,void*,int32 model){trace.push_back({"construct",model});std::memset(target,0xCD,sizeof(CProjectile));reinterpret_cast<uint8*>(target)[0x36]=4;target->m_nModelIndex=int16(model);target->m_matrix=reinterpret_cast<CMatrixLink*>(objectMatrix);target->m_nPhysicalFlags=0xABCD4321;return target;}
inline CWeaponInfo* __cdecl Info(eWeaponType type,eWeaponSkill skill){trace.push_back({"info",int(type),int(skill)});return weaponInfo;}
inline void __fastcall Reference(CEntity* entity,void*,CEntity** at){trace.push_back({"ref",uintptr_t(entity),uintptr_t(at)});}
inline void __fastcall Matrix(CPlaceable*,void*,CMatrix& matrix){trace.push_back({"matrix",F(matrix.GetRight().x),F(matrix.GetRight().y),F(matrix.GetRight().z),F(matrix.GetForward().x),F(matrix.GetForward().y),F(matrix.GetForward().z),F(matrix.GetUp().x),F(matrix.GetUp().y),F(matrix.GetUp().z),F(matrix.GetPosition().x),F(matrix.GetPosition().y),F(matrix.GetPosition().z)});objectMatrix->GetRight()=matrix.GetRight();objectMatrix->GetForward()=matrix.GetForward();objectMatrix->GetUp()=matrix.GetUp();objectMatrix->GetPosition()=matrix.GetPosition();}
inline void __cdecl WorldAdd(CEntity* entity){trace.push_back({"world_add",uintptr_t(entity)});}
inline void __fastcall CollisionAllocate(CColModel* model,void*,int32 spheres,int32 boxes,int32 lines,int32 verts,int32 triangles,bool disks){trace.push_back({"collision_allocate",spheres,boxes,lines,verts,triangles,disks});model->m_pColData=collisionData;collisionData->m_nNumSpheres=uint16(spheres);collisionData->m_pSpheres=collisionSphere;}
inline void* __cdecl Malloc(uint32 size,uint32 hint){trace.push_back({"malloc",size,hint});return collisionSphere;}
inline RwMatrix* __fastcall Modelling(CEntity*,void*){trace.push_back({"modelling"});return scenario%3==0?nullptr:reinterpret_cast<RwMatrix*>(objectMatrix);}
inline FxSystem_c* __fastcall Create(FxManager_c*,void*,const char* name,const CVector& point,RwMatrix* matrix,bool ignore){trace.push_back({"fx_create",name,F(point.x),F(point.y),F(point.z),uintptr_t(matrix),ignore});return scenario%4==0?nullptr:fx;}
inline void __fastcall Play(FxSystem_c*,void*){trace.push_back({"play"});}
inline void __cdecl Request(int32 model,int32 flags){trace.push_back({"request",model,flags});}
inline bool __fastcall IsPlayer(CPed*,void*){trace.push_back({"is_player"});return scenario%3!=0;}
inline CPlayerPed* __cdecl Player(int32 id){trace.push_back({"player",id});return player;}
inline CVehicle* __cdecl PlayerVehicle(int32 id,bool remote){trace.push_back({"player_vehicle",id,remote});return playerVehicle;}
inline tBlipHandle __cdecl Blip(eBlipType type,int32 handle,uint32 color,eBlipDisplay display){trace.push_back({"blip",int(type),handle,color,int(display)});return 77;}
inline void __cdecl Scale(tBlipHandle handle,int32 scale){trace.push_back({"scale",handle,scale});}
inline void __cdecl Colour(tBlipHandle handle,eBlipColour color){trace.push_back({"colour",handle,uint32(color)});}
inline void __fastcall Audio(CAudioEngine*,void*,int32 event,eWeaponType type,CPhysical* entity){trace.push_back({"audio",event,int(type),uintptr_t(entity)});}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }

struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CProjectileInfo","AddProjectile",0x737C80,0,-1,16384}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"projectile-add355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"projectile-add355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("projectile-add355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedMS=CTimer::GetTimeInMS();const auto savedModel=StaticRef<uintptr_t>(0xA9B0C8+2000*4),savedPool=StaticRef<uintptr_t>(0xB7449C);const auto savedActive=TheCamera.m_nActiveCam;const auto savedFront=TheCamera.m_aCams[0].m_vecFront,savedUp=TheCamera.m_aCams[0].m_vecUp;const auto savedFlare=StaticRef<int32>(0x8CD75C);
 const auto savedInfos=gaProjectileInfo;const auto savedProjectiles=CProjectileInfo::ms_apProjectile;
 EntryPatch al(0x5A1EE0,Allocate),ald(bit_cast<uintptr_t>(static_cast<void*(*)(size_t)>(&CObject::operator new)),Allocate);
 EntryPatch ct(0x5A4030,Construct);
 EntryPatch wi(0x743C60,Info),wid(bit_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)),Info);
 EntryPatch re(0x571B70,Reference),red(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Reference);
 EntryPatch mt(0x54F610,Matrix),mtd(bit_cast<uintptr_t>(&CPlaceable::SetMatrix),Matrix);
 EntryPatch wa(0x563220,WorldAdd),wad(bit_cast<uintptr_t>(&CWorld::Add),WorldAdd);
 EntryPatch ca(0x40F870,CollisionAllocate),cad(bit_cast<uintptr_t>(static_cast<void(CColModel::*)(int32,int32,int32,int32,int32,bool)>(&CColModel::AllocateData)),CollisionAllocate);
 EntryPatch ma(0x72F420,Malloc),mad(bit_cast<uintptr_t>(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc)),Malloc);
 EntryPatch mo(0x46A2D0,Modelling),mod(bit_cast<uintptr_t>(&CEntity::GetModellingMatrix),Modelling);
 EntryPatch cr(0x4A9BE0,Create),crd(bit_cast<uintptr_t>(static_cast<FxSystem_c*(FxManager_c::*)(const char*,const CVector&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem)),Create);
 EntryPatch py(0x4AA2F0,Play),pyd(bit_cast<uintptr_t>(&FxSystem_c::Play),Play);
 EntryPatch rq(0x4087E0,Request),rqd(bit_cast<uintptr_t>(&CStreaming::RequestModel),Request);
 EntryPatch ip(0x5DF8F0,IsPlayer),ipd(bit_cast<uintptr_t>(&CPed::IsPlayer),IsPlayer);
 EntryPatch pl(0x56E210,Player),pld(bit_cast<uintptr_t>(&FindPlayerPed),Player);
 EntryPatch pv(0x56E0D0,PlayerVehicle),pvd(bit_cast<uintptr_t>(&FindPlayerVehicle),PlayerVehicle);
 EntryPatch bl(0x5839A0,Blip),bld(bit_cast<uintptr_t>(&CRadar::SetEntityBlip),Blip);
 EntryPatch sc(0x583CC0,Scale),scd(bit_cast<uintptr_t>(&CRadar::ChangeBlipScale),Scale);
 EntryPatch cl(0x583AB0,Colour),cld(bit_cast<uintptr_t>(&CRadar::ChangeBlipColour),Colour);
 EntryPatch au(0x506F40,Audio),aud(bit_cast<uintptr_t>(&CAudioEngine::ReportWeaponEvent),Audio);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CProjectile) std::array<uint8,0x19C> objectBytes{};objectBytes.fill(0xCD);object=reinterpret_cast<CProjectile*>(objectBytes.data());alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrices[2]{};objectMatrix=reinterpret_cast<CMatrix*>(matrices[0].data());alignas(16) std::array<uint8,4096> creator{},victim{};alignas(CWeaponInfo) std::array<uint8,sizeof(CWeaponInfo)> infoBytes{};weaponInfo=reinterpret_cast<CWeaponInfo*>(infoBytes.data());alignas(CColModel) std::array<uint8,sizeof(CColModel)> modelCol{};alignas(CCollisionData) std::array<uint8,sizeof(CCollisionData)> data{};collisionData=reinterpret_cast<CCollisionData*>(data.data());alignas(CColSphere) std::array<uint8,sizeof(CColSphere)> sphere{};sphere.fill(0xCD);collisionSphere=reinterpret_cast<CColSphere*>(sphere.data());alignas(16) std::array<uint8,128> model{};alignas(4) std::array<uint8,20> pool{};std::array<uint8,1> poolFlags{0x81};alignas(16) std::array<uint8,64> fxBytes{};fx=reinterpret_cast<FxSystem_c*>(fxBytes.data());
    constexpr std::array<uint32,8> types{16,17,18,19,20,21,39,58};const auto type=static_cast<eWeaponType>(types[size_t(i%8)]);const uint32 variant=uint32(i/8);scenario=variant;
    creator[0x36]=uint8(variant%4==0?2:variant%4==1?3:variant%4==2?4:1);At<int16>(creator.data(),0x22)=int16(variant%2?405:411);At<uintptr_t>(creator.data(),0x14)=variant%5==0&&int(type)<19?0:uintptr_t(matrices[1].data());At<float>(creator.data(),0x10)=float(int32(variant%9)-4)*.25f;At<CVector>(creator.data(),0x44)=CVector{float(variant%7)*.125f,float(variant%5)*.25f,float(variant%3)*.125f};
    At<CVector>(matrices[1].data(),0)=CVector{.8f,.6f,0};At<CVector>(matrices[1].data(),0x10)=CVector{-.6f,.8f,0};At<CVector>(matrices[1].data(),0x20)=CVector{0,0,1};At<CVector>(matrices[1].data(),0x30)=CVector{1,2,3};weaponInfo->m_nModelId1=2000;
    At<uintptr_t>(model.data(),0x14)=uintptr_t(modelCol.data());At<CVector>(modelCol.data(),0x18)=CVector{1,2,3};At<float>(modelCol.data(),0x24)=2;auto* collision=reinterpret_cast<CColModel*>(modelCol.data());collision->m_pColData=variant%3==0?nullptr:collisionData;collisionData->m_nNumSpheres=uint16(variant%3==2?1:0);collisionData->m_pSpheres=variant%3==2?collisionSphere:nullptr;
    At<uintptr_t>(pool.data(),0)=uintptr_t(objectBytes.data());At<uintptr_t>(pool.data(),4)=uintptr_t(poolFlags.data());At<int32>(pool.data(),8)=1;StaticRef<uintptr_t>(0xB7449C)=uintptr_t(pool.data());StaticRef<uintptr_t>(0xA9B0C8+2000*4)=uintptr_t(model.data());StaticRef<int32>(0x8CD75C)=2000;StaticRef<uint32>(0xB7CB84)=1000;TheCamera.m_nActiveCam=0;TheCamera.m_aCams[0].m_vecFront=CVector{0,1,.25f};TheCamera.m_aCams[0].m_vecUp=CVector{0,0,1};
    gaProjectileInfo.fill({});CProjectileInfo::ms_apProjectile.fill(nullptr);const size_t busy=variant%37;for(size_t n=0;n<std::min(size_t(32),busy);++n)gaProjectileInfo[n].m_bActive=true;
    auto* source=reinterpret_cast<CEntity*>(creator.data());auto* target=variant%3==0?nullptr:reinterpret_cast<CEntity*>(victim.data());player=variant%2?reinterpret_cast<CPlayerPed*>(source):reinterpret_cast<CPlayerPed*>(victim.data());playerVehicle=variant%2?reinterpret_cast<CVehicle*>(victim.data()):reinterpret_cast<CVehicle*>(source);const CVector direction{.25f,1,.5f},origin{4,5,6};const float force=variant%5==0?0.f:float(int32(variant%11)-2)*.25f;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;const bool result=reinterpret_cast<bool(__cdecl*)(CEntity*,eWeaponType,CVector,float,const CVector*,CEntity*)>(fn.address)(source,type,origin,force,variant%3==0?nullptr:&direction,target);
    return json{{"objects",Digest(objectBytes.data(),objectBytes.size())},{"matrices",Digest(matrices,sizeof(matrices))},{"creator",Digest(creator.data(),creator.size())},{"victim",Digest(victim.data(),victim.size())},{"infos",Digest(gaProjectileInfo.data(),sizeof(gaProjectileInfo))},{"projectiles",Digest(CProjectileInfo::ms_apProjectile.data(),sizeof(CProjectileInfo::ms_apProjectile))},{"col",Digest(modelCol.data(),modelCol.size())},{"data",Digest(data.data(),data.size())},{"sphere",Digest(sphere.data(),sphere.size())},{"result",result},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 gaProjectileInfo=savedInfos;CProjectileInfo::ms_apProjectile=savedProjectiles;StaticRef<uint32>(0xB7CB84)=savedMS;StaticRef<uintptr_t>(0xA9B0C8+2000*4)=savedModel;StaticRef<uintptr_t>(0xB7449C)=savedPool;StaticRef<int32>(0x8CD75C)=savedFlare;TheCamera.m_nActiveCam=savedActive;TheCamera.m_aCams[0].m_vecFront=savedFront;TheCamera.m_aCams[0].m_vecUp=savedUp;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
