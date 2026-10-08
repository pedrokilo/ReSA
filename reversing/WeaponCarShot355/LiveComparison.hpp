#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Weapon.h"
#include "WeaponInfo.h"
#include "Entity/Ped/PlayerPed.h"
#include "ColModel.h"
#include "BaseModelInfo.h"
#include "CollisionData.h"
#include "Automobile.h"
#include "SurfaceInfos_c.h"
#include "Collision/ColLine.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace WeaponCarShot355Live {
inline json trace;inline uint32 scenario{},randomCalls{};inline RwMatrix head;inline CWeaponInfo* info{};inline CPlayerPed* player{};
inline RpHAnimHierarchy* __cdecl Hierarchy(RpClump* clump){trace.push_back({"hierarchy",uintptr_t(clump)});return reinterpret_cast<RpHAnimHierarchy*>(0x11223344);}
inline int32 __cdecl Index(RpHAnimHierarchy*,int32 id){trace.push_back({"index",id});return 0;}
inline RwMatrix* __cdecl Matrices(RpHAnimHierarchy*){trace.push_back({"matrices"});return &head;}
inline CWeaponInfo* __cdecl Info(eWeaponType type,eWeaponSkill skill){trace.push_back({"info",int(type),int(skill)});return info;}
inline CPlayerPed* __cdecl Player(int32 id){trace.push_back({"player",id});return player;}
inline int __cdecl Random(){const int value=int((scenario*7919+randomCalls++*14033)%32768);trace.push_back({"random",value});return value;}
inline void __cdecl Aim(CEntity* owner,CVehicle* vehicle,CVector* origin,CVector* target,bool vehicles){trace.push_back({"aim",uintptr_t(owner),uintptr_t(vehicle),std::bit_cast<uint32>(origin->x),std::bit_cast<uint32>(origin->y),std::bit_cast<uint32>(origin->z),std::bit_cast<uint32>(target->x),std::bit_cast<uint32>(target->y),std::bit_cast<uint32>(target->z),vehicles});origin->x+=.125f;target->y-=.25f;}
inline void __fastcall Fire(CWeapon*,void*,CVector origin,CVector target,CVehicle* vehicle,CEntity* owner){trace.push_back({"fire",std::bit_cast<uint32>(origin.x),std::bit_cast<uint32>(origin.y),std::bit_cast<uint32>(origin.z),std::bit_cast<uint32>(target.x),std::bit_cast<uint32>(target.y),std::bit_cast<uint32>(target.z),uintptr_t(vehicle),uintptr_t(owner)});}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CWeapon","FireInstantHitFromCar",0x73EC40,0,-1,8192}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"weapon-car-shot355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"weapon-car-shot355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("weapon-car-shot355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch h(0x734A40,Hierarchy),hd(bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump),Hierarchy),idx(0x7C51A0,Index),idxd(bit_cast<uintptr_t>(&RpHAnimIDGetIndex),Index),ma(0x7C5120,Matrices),mad(bit_cast<uintptr_t>(&RpHAnimHierarchyGetMatrixArray),Matrices);
 EntryPatch wi(0x743C60,Info),wid(bit_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)),Info);
 EntryPatch pl(0x56E210,Player),pld(bit_cast<uintptr_t>(&FindPlayerPed),Player);
 EntryPatch rn(0x821B1E,Random),rnd(bit_cast<uintptr_t>(&std::rand),Random);
 EntryPatch ai(0x73D720,Aim),aid(bit_cast<uintptr_t>(&CWeapon::DoDriveByAutoAiming),Aim);
 EntryPatch fi(0x73CBA0,Fire),fid(bit_cast<uintptr_t>(&CWeapon::FireInstantHitFromCar2),Fire);
 const auto savedModel=StaticRef<uintptr_t>(0xA9B0C8+2000*4);const auto savedStep=CTimer::GetTimeStep();
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    randomCalls=0;alignas(CWeapon) std::array<uint8,sizeof(CWeapon)> weapon{};weapon.fill(uint8(i*37));auto* self=reinterpret_cast<CWeapon*>(weapon.data());self->m_Type=static_cast<eWeaponType>(i%3==0?28:i%3==1?29:32);
    alignas(16) std::array<uint8,4096> car{},driver{};alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrix{};alignas(CWeaponInfo) std::array<uint8,sizeof(CWeaponInfo)> weaponInfo{};alignas(16) std::array<uint8,128> model{},vehicleData{},clump{};alignas(4) std::array<uint8,32> bone{};alignas(CColModel) std::array<uint8,sizeof(CColModel)> col{};
    auto* vehicle=reinterpret_cast<CVehicle*>(car.data());car[0x36]=2;vehicle->m_vehicleSpecialColIndex=-1;driver[0x36]=3;auto* owner=reinterpret_cast<CPed*>(driver.data());player=reinterpret_cast<CPlayerPed*>(owner);info=reinterpret_cast<CWeaponInfo*>(weaponInfo.data());info->m_fWeaponRange=10.f+float(i%7);info->m_vecFireOffset=CVector{.25f,.5f,.75f};
    At<uintptr_t>(car.data(),0x14)=uintptr_t(matrix.data());At<int16>(car.data(),0x22)=2000;At<int32>(car.data(),0x590)=i%3==0?9:0;At<uintptr_t>(car.data(),0x460)=i%3==0&&i%2==0?0:uintptr_t(owner);At<CVector>(car.data(),0x44)=CVector{float(i%5)*.125f,float(i%7)*.25f,float(i%3)*.125f};
    At<CVector>(matrix.data(),0)=CVector{.8f,.6f,0};At<CVector>(matrix.data(),0x10)=CVector{-.6f,.8f,0};At<CVector>(matrix.data(),0x20)=CVector{0,0,1};At<CVector>(matrix.data(),0x30)=CVector{float(i%11),float(i%7),float(i%5)};
    At<uintptr_t>(driver.data(),0x18)=uintptr_t(clump.data());At<uintptr_t>(driver.data(),0x4A0)=uintptr_t(bone.data());clump[0]=2;At<int32>(bone.data(),0x14)=0;
    At<uintptr_t>(model.data(),0x14)=uintptr_t(col.data());At<int32>(model.data(),0x3C)=i%2?5:0;At<uintptr_t>(model.data(),0x5C)=uintptr_t(vehicleData.data());At<CVector>(vehicleData.data(),0)=CVector{1,2,3};At<CVector>(vehicleData.data(),0x30)=CVector{4,5,6};At<CVector>(col.data(),0)=CVector{-1,-2,-3};At<CVector>(col.data(),12)=CVector{1,2,3};
    std::memset(&head,0,sizeof(head));head.right=RwV3d{1,0,0};head.up=RwV3d{0,1,0};head.at=RwV3d{0,0,1};head.pos=RwV3d{float(i%9),float(i%13),float(i%7)};StaticRef<uintptr_t>(0xA9B0C8+2000*4)=uintptr_t(model.data());StaticRef<float>(0xB7CB5C)=.5f+float(i%5)*.25f;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;const bool result=reinterpret_cast<bool(__thiscall*)(CWeapon*,CVehicle*,bool,bool)>(fn.address)(self,vehicle,(i/3)%2!=0,(i/6)%2!=0);
    return json{{"weapon",Digest(weapon.data(),weapon.size())},{"car",Digest(car.data(),car.size())},{"driver",Digest(driver.data(),driver.size())},{"matrix",Digest(matrix.data(),matrix.size())},{"result",result},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<uintptr_t>(0xA9B0C8+2000*4)=savedModel;StaticRef<float>(0xB7CB5C)=savedStep;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
