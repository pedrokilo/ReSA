#pragma once
#include <fstream>
#include <ctime>
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
#include "Camera.h"
#include "WaterLevel.h"
#include "PedIntelligence.h"
#include "Tasks/TaskTypes/TaskSimpleUseGun.h"
#include "Events/EventGlobalGroup.h"
#include "Events/EventGunShot.h"
#include "Events/EventGunShotWhizzedBy.h"
#include "InterestingEvents.h"
#include "PointLights.h"
#include "Birds.h"
#include "Shadows.h"
#include "Fx/Fx.h"
#include "AudioEngine.h"
#include "PedModelInfo.h"
#include "../ProbeRuntime.hpp"
namespace WeaponInstant355Live {
inline json trace;inline uint32 scenario{},randomCalls{},lineCalls{};inline RwMatrix head;inline CWeaponInfo* info{};inline CPlayerPed* player{};inline CEntity* targetObject{};inline CTaskSimpleUseGun* gun{};inline CColModel* col{};inline CEventGlobalGroup* group{};
inline uint32 Bits(float v){return std::bit_cast<uint32>(v);}
inline json V(const CVector& v){return json::array({Bits(v.x),Bits(v.y),Bits(v.z)});}
inline RpHAnimHierarchy* __cdecl Hierarchy(RpClump*){trace.push_back({"hierarchy"});return reinterpret_cast<RpHAnimHierarchy*>(0x11223344);}
inline int32 __cdecl Index(RpHAnimHierarchy*,int32 id){trace.push_back({"index",id});return 0;}
inline RwMatrix* __cdecl Matrices(RpHAnimHierarchy*){trace.push_back({"matrices"});return &head;}
inline CWeaponInfo* __cdecl Info(eWeaponType type,eWeaponSkill skill){trace.push_back({"info",int(type),int(skill)});return info;}
inline eWeaponSkill __fastcall Skill(CPed*,void*,eWeaponType type){trace.push_back({"skill",int(type)});return eWeaponSkill::STD;}
inline CTaskSimpleUseGun* __fastcall Gun(CPedIntelligence*,void*){trace.push_back({"gun"});return scenario%5==0?gun:nullptr;}
inline bool __fastcall IsPlayer(CPed*,void*){trace.push_back({"is_player"});return scenario%3!=0;}
inline CPlayerPed* __cdecl Player(int32 id){trace.push_back({"player",id});return player;}
inline int __cdecl Random(){const int value=int((scenario*7919+randomCalls++*14033)%32768);trace.push_back({"random",value});return value;}
inline float __cdecl RandomRange(float a,float b){trace.push_back({"range",Bits(a),Bits(b)});return a*.25f+b*.75f;}
inline clock_t __cdecl Clock(){trace.push_back({"clock"});return scenario%2?25:75;}
inline void __cdecl Aim(CEntity* owner,CVehicle*,CVector* origin,CVector* target,bool vehicles){trace.push_back({"aim",uintptr_t(owner),V(*origin),V(*target),vehicles});origin->x+=.125f;target->y-=.25f;}
inline void __cdecl Doom(CEntity*,CVector* a,CVector* b){trace.push_back({"doom",V(*a),V(*b)});b->z+=.125f;}
inline float __cdecl Range(CEntity* target,CEntity*){trace.push_back({"weapon_range",uintptr_t(target)});return .75f+float(scenario%5)*.25f;}
inline void __fastcall Bone(CPed*,void*,CVector* out,eBoneTag bone,bool update){trace.push_back({"bone",int(bone),update});*out=CVector{2,3,4};}
inline void __fastcall Transformed(CPed*,void*,RwV3d& point,eBoneTagU32 bone,bool update){trace.push_back({"transformed",V(CVector{point}),bone.get_underlying(),update});point.x+=.25f;point.y+=.5f;point.z+=.75f;}
inline void __fastcall CameraRay(CCamera*,void*,float range,CVector muzzle,CVector& from,CVector& to){trace.push_back({"camera_ray",Bits(range),V(muzzle)});from=CVector{1,2,3};to=CVector{4,5,6};}
inline void __fastcall TwoAim(CCam*,void*,CVector& out){trace.push_back({"two_aim"});out=CVector{.25f,.5f,.75f};}
inline void __cdecl Birds(const CVector* a,const CVector* b){trace.push_back({"birds",V(*a),V(*b)});}
inline void __cdecl Oil(const CVector& a,const CVector& b){trace.push_back({"oil",V(a),V(b)});}
inline bool __cdecl Line(const CVector& a,const CVector& b,CColPoint& point,CEntity*& entity,bool b0,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7){trace.push_back({"line",V(a),V(b),b0,b1,b2,b3,b4,b5,b6,b7,uintptr_t(CWorld::pIgnoreEntity),CWorld::bIncludeDeadPeds,CWorld::bIncludeCarTyres,CWorld::bIncludeBikers});std::memset(&point,0,sizeof(point));point.m_vecPoint=CVector{2,3,4};point.m_nPieceTypeB=uint8(scenario%19);entity=scenario%3!=0&&lineCalls++==0?targetObject:nullptr;return entity!=nullptr;}
inline void __cdecl Occupant(CEntity** entity,CColPoint*,eWeaponType type,const CVector& a,const CVector& b){trace.push_back({"occupant",uintptr_t(*entity),int(type),V(a),V(b)});if(scenario%7==0)*entity=nullptr;}
inline CEventGlobalGroup* __cdecl Group(){trace.push_back({"group"});return group;}
inline void __fastcall Reference(CEntity*,void*,CEntity**){trace.push_back({"reference"});}
inline void __fastcall Cleanup(CEntity*,void*,CEntity**){trace.push_back({"cleanup"});}
inline CEvent* __fastcall EventAdd(CEventGroup*,void*,CEvent* event,bool valid){auto* shot=static_cast<CEventGunShot*>(event);trace.push_back({"event",int(event->GetEventType()),V(shot->m_startPoint),V(shot->m_endPoint),shot->m_bHasNoSound,valid});return nullptr;}
inline void __fastcall Interesting(CInterestingEvents*,void*,CInterestingEvents::EType type,CEntity*){trace.push_back({"interesting",int(type)});}
inline void __cdecl Light(uint8 type,CVector a,CVector b,float radius,float red,float green,float blue,uint8 fog,bool shadows,CEntity*){trace.push_back({"light",type,V(a),V(b),Bits(radius),Bits(red),Bits(green),Bits(blue),fog,shadows});}
inline void __fastcall FxShot(Fx_c*,void*,CEntity*,const CVector& a,const CVector& b,bool flash){trace.push_back({"fx_shot",V(a),V(b),flash});}
inline void __fastcall Shell(CWeapon*,void*,CEntity*,CVector& point,const CVector2D& d,float size){trace.push_back({"shell",V(point),Bits(d.x),Bits(d.y),Bits(size)});}
inline bool __cdecl Water(CVector a,CVector b,CVector* point){trace.push_back({"water",V(a),V(b)});*point=CVector{1,2,3};return scenario%4!=0;}
inline void __fastcall Splash(Fx_c*,void*,const CVector& point){trace.push_back({"splash",V(point)});}
inline void __fastcall Audio(CAudioEngine*,void*,CEntity*,eSurfaceType surface,const CVector& point,float angle){trace.push_back({"audio",int(surface),V(point),Bits(angle)});}
inline void __fastcall Impact(CWeapon*,void*,CEntity*,CEntity* victim,const CVector& a,const CVector& b,const CColPoint& cp,int32 flag){trace.push_back({"impact",uintptr_t(victim),V(a),V(b),V(cp.m_vecPoint),cp.m_nPieceTypeB,flag});}
inline void __fastcall Pellets(CWeapon*,void*,int32 count,CEntity*,CEntity*,CVector& start,CColPoint& hit,CMatrix& matrix){trace.push_back({"pellets",count,V(start),V(hit.m_vecPoint)});std::memset(&matrix,0,sizeof(matrix));matrix.GetRight()=CVector{1,0,0};matrix.GetForward()=CVector{0,1,0};matrix.GetUp()=CVector{0,0,1};}
inline CColModel* __fastcall Col(CEntity*,void*){trace.push_back({"col"});return col;}
inline CColModel* __fastcall PedCol(CPedModelInfo*,void*,RpClump*){trace.push_back({"ped_col"});return col;}
inline int32 __cdecl Collisions(const CMatrix&,CColModel&,const CMatrix&,CColModel&,std::array<CColPoint,32>&,CColPoint*,float* d,bool flag){trace.push_back({"collisions",flag});for(int n=0;n<15;++n)d[n]=n<int(scenario%16)?.5f:1.f;return 0;}
inline void __cdecl Reset(){trace.push_back({"reset"});CWorld::bIncludeDeadPeds=false;CWorld::bIncludeCarTyres=false;CWorld::bIncludeBikers=false;CWorld::pIgnoreEntity=nullptr;CWorld::fWeaponSpreadRate=0;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CWeapon","FireInstantHit",0x73FB10,0,-1,20480}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"weapon-instant355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"weapon-instant355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("weapon-instant355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch h(0x734A40,Hierarchy),hd(bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump),Hierarchy),idx(0x7C51A0,Index),idxd(bit_cast<uintptr_t>(&RpHAnimIDGetIndex),Index),ma(0x7C5120,Matrices),mad(bit_cast<uintptr_t>(&RpHAnimHierarchyGetMatrixArray),Matrices);
 EntryPatch wi(0x743C60,Info),wid(bit_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)),Info);
 EntryPatch sk(0x5E3B60,Skill),skd(bit_cast<uintptr_t>(static_cast<eWeaponSkill(CPed::*)(eWeaponType)>(&CPed::GetWeaponSkill)),Skill);
 EntryPatch gu(0x600F70,Gun),gud(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskUseGun),Gun),isp(0x5DF8F0,IsPlayer),ispd(bit_cast<uintptr_t>(&CPed::IsPlayer),IsPlayer);
 EntryPatch pl(0x56E210,Player),pld(bit_cast<uintptr_t>(&FindPlayerPed),Player),rn(0x821B1E,Random),rnd(bit_cast<uintptr_t>(&std::rand),Random);
 EntryPatch rr(0x41BD90,RandomRange),rrd(bit_cast<uintptr_t>(static_cast<float(*)(float,float)>(&CGeneral::GetRandomNumberInRange<float>)),RandomRange);
 EntryPatch ai(0x73D720,Aim),aid(bit_cast<uintptr_t>(&CWeapon::DoDriveByAutoAiming),Aim),doom(0x73CDC0,Doom),doomd(bit_cast<uintptr_t>(&CWeapon::DoDoomAiming),Doom);
 EntryPatch range(0x73B380,Range),ranged(bit_cast<uintptr_t>(&CWeapon::TargetWeaponRangeMultiplier),Range);
 EntryPatch bone(0x5E4280,Bone),boned(bit_cast<uintptr_t>(static_cast<void(CPed::*)(CVector*,eBoneTag,bool)>(&CPed::GetBonePosition)),Bone),trans(0x5E01C0,Transformed),transd(bit_cast<uintptr_t>(&CPed::GetTransformedBonePosition),Transformed);
 EntryPatch cam(0x514970,CameraRay),camd(bit_cast<uintptr_t>(&CCamera::Find3rdPersonCamTargetVector),CameraRay),two(0x513E40,TwoAim),twod(bit_cast<uintptr_t>(&CCam::Get_TwoPlayer_AimVector),TwoAim);
 EntryPatch birds(0x712E40,Birds),birdsd(bit_cast<uintptr_t>(&CBirds::HandleGunShot),Birds),oil(0x707550,Oil),oild(bit_cast<uintptr_t>(&CShadows::GunShotSetsOilOnFire),Oil);
 EntryPatch line(0x56BA00,Line),lined(bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),Line),occupant(0x73F480,Occupant),occupantd(bit_cast<uintptr_t>(&CWeapon::CheckForShootingVehicleOccupant),Occupant);
 EntryPatch gr(0x4ABA50,Group),grd(bit_cast<uintptr_t>(&GetEventGlobalGroup),Group),add(0x4AB420,EventAdd),addd(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),EventAdd);
 EntryPatch ref(0x571B70,Reference),refd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Reference),clean(0x571A00,Cleanup),cleand(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),Cleanup);
 EntryPatch interesting(0x602590,Interesting),interestingd(bit_cast<uintptr_t>(&CInterestingEvents::Add),Interesting),light(0x7000E0,Light),lightd(bit_cast<uintptr_t>(&CPointLights::AddLight),Light);
 EntryPatch fxshot(0x4A0DE0,FxShot),fxshotd(bit_cast<uintptr_t>(&Fx_c::TriggerGunshot),FxShot),shell(0x73A3E0,Shell),shelld(bit_cast<uintptr_t>(&CWeapon::AddGunshell),Shell);
 EntryPatch water(0x6E61B0,Water),waterd(bit_cast<uintptr_t>(&CWaterLevel::TestLineAgainstWater),Water),splash(0x4A10E0,Splash),splashd(bit_cast<uintptr_t>(&Fx_c::TriggerBulletSplash),Splash),audio(0x506EC0,Audio),audiod(bit_cast<uintptr_t>(&CAudioEngine::ReportBulletHit),Audio);
 EntryPatch impact(0x73B550,Impact),impactd(bit_cast<uintptr_t>(&CWeapon::DoBulletImpact),Impact),pellets(0x73C710,Pellets),pelletsd(bit_cast<uintptr_t>(&CWeapon::SetUpPelletCol),Pellets);
 EntryPatch colmodel(0x535300,Col),colmodeld(bit_cast<uintptr_t>(&CEntity::GetColModel),Col),pedcol(0x4C6F70,PedCol),pedcold(bit_cast<uintptr_t>(&CPedModelInfo::AnimatePedColModelSkinned),PedCol),collisions(0x4185C0,Collisions),collisionsd(bit_cast<uintptr_t>(&CCollision::ProcessColModels),Collisions);
 EntryPatch reset(0x5631C0,Reset),resetd(bit_cast<uintptr_t>(&CWorld::ResetLineTestOptions),Reset);
 const auto savedModel=StaticRef<uintptr_t>(0xA9B0C8+2000*4);const auto savedMS=CTimer::GetTimeInMS();std::array<uint8,24> savedWorld;std::memcpy(savedWorld.data(),reinterpret_cast<void*>(0xB7CD60),savedWorld.size());const auto savedCounter=StaticRef<int32>(0xC8A80C);const auto savedActive=TheCamera.m_nActiveCam;std::array<uint8,0x470> savedCams;std::memcpy(savedCams.data(),TheCamera.m_aCams.data(),savedCams.size());
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    randomCalls=0;lineCalls=0;
    alignas(CWeapon) std::array<uint8,sizeof(CWeapon)> weapon{};weapon.fill(uint8(i*37));auto* self=reinterpret_cast<CWeapon*>(weapon.data());constexpr std::array<int,10> types{22,23,24,25,26,27,28,30,31,38};self->m_Type=static_cast<eWeaponType>(types[size_t(i%10)]);const uint32 variant=uint32(i/10);scenario=variant;
    alignas(16) std::array<uint8,4096> ownerBytes{},targetBytes{};alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrix[2]{};alignas(CWeaponInfo) std::array<uint8,sizeof(CWeaponInfo)> infoBytes{};alignas(16) std::array<uint8,128> model{},clump{},task{},intel{},eventGroup{},playerData{};alignas(4) std::array<uint8,32> boneData{};alignas(CColModel) std::array<uint8,sizeof(CColModel)> colBytes{};
    auto* owner=reinterpret_cast<CEntity*>(ownerBytes.data());targetObject=reinterpret_cast<CEntity*>(targetBytes.data());col=reinterpret_cast<CColModel*>(colBytes.data());info=reinterpret_cast<CWeaponInfo*>(infoBytes.data());gun=reinterpret_cast<CTaskSimpleUseGun*>(task.data());group=reinterpret_cast<CEventGlobalGroup*>(eventGroup.data());player=reinterpret_cast<CPlayerPed*>(owner);
    ownerBytes[0x36]=uint8(variant%3==0?2:variant%3==1?3:4);ownerBytes[0x36]|=uint8(variant%11==0?8<<3:0);targetBytes[0x36]=uint8(variant%3==0?2:variant%3==1?3:4);At<int16>(ownerBytes.data(),0x22)=int16(variant%4==0?447:variant%4==1?469:variant%4==2?564:2000);At<int16>(targetBytes.data(),0x22)=2000;At<uintptr_t>(ownerBytes.data(),0x14)=uintptr_t(matrix[0].data());At<uintptr_t>(targetBytes.data(),0x14)=uintptr_t(matrix[1].data());
    for(auto& m:matrix){At<CVector>(m.data(),0)=CVector{.8f,.6f,0};At<CVector>(m.data(),0x10)=CVector{-.6f,.8f,0};At<CVector>(m.data(),0x20)=CVector{0,0,1};At<CVector>(m.data(),0x30)=CVector{1,2,3};}
    At<uintptr_t>(ownerBytes.data(),0x130)=uintptr_t(intel.data());At<uintptr_t>(ownerBytes.data(),0x480)=variant%6==1||variant%6==4?uintptr_t(playerData.data()):0;At<float>(playerData.data(),0x2C)=.25f+float(variant%7)*.125f;At<uint32>(playerData.data(),0x70)=4;At<CVector>(playerData.data(),0x74)=CVector{1,2,3};clump[0]=2;At<uintptr_t>(ownerBytes.data(),0x18)=uintptr_t(clump.data());At<uintptr_t>(targetBytes.data(),0x18)=uintptr_t(clump.data());At<uintptr_t>(ownerBytes.data(),0x4A0)=uintptr_t(boneData.data());At<int32>(boneData.data(),0x14)=3;
    ownerBytes[0x71A]=uint8(variant%101);ownerBytes[0x528]=uint8(variant%2);At<uint32>(ownerBytes.data(),0x46C)=variant%7==0?0x100u:variant%7==1?0x4000000u:0u;At<uintptr_t>(ownerBytes.data(),0x58C)=variant%7==0?uintptr_t(targetBytes.data()):0;At<uintptr_t>(ownerBytes.data(),0xFC)=variant%7==2?uintptr_t(targetBytes.data()):0;At<uint32>(ownerBytes.data(),0x470)=variant%13==0?0x2000000u:0u;At<uintptr_t>(ownerBytes.data(),0x460)=uintptr_t(targetBytes.data());At<int32>(ownerBytes.data(),0x594)=int32(variant%5);At<uintptr_t>(ownerBytes.data(),0x4F8)=variant%3==0?uintptr_t(task.data()):0;At<uint32>(targetBytes.data(),0x40)=variant%2?0x100u:0u;targetBytes[0x42D]=uint8(variant%5==0?4:0);At<CVector>(targetBytes.data(),0x44)=CVector{.1f,.2f,.3f};task[0xE]=uint8(variant%2);
    info->m_fAnimLoopStart=.1f;info->m_fAnimLoopEnd=.1f+float(variant%7)*.0125f;info->m_fWeaponRange=10.f+float(variant%11);info->m_fAccuracy=5.f+float(variant%7);info->m_nDamage=uint16(variant%50);info->m_nFlags=variant%2?1:0;At<uintptr_t>(model.data(),0x14)=uintptr_t(colBytes.data());StaticRef<uintptr_t>(0xA9B0C8+2000*4)=uintptr_t(model.data());
    std::memset(&head,0,sizeof(head));head.right=RwV3d{1,0,0};head.up=RwV3d{0,1,0};head.at=RwV3d{0,0,1};head.pos=RwV3d{1,2,3};TheCamera.m_nActiveCam=uint8(variant%2);TheCamera.m_aCams[0].m_nMode=static_cast<eCamMode>(variant%5==0?49:variant%5==1?53:variant%5==2?55:variant%5==3?65:4);TheCamera.m_aCams[1].m_nMode=TheCamera.m_aCams[0].m_nMode;TheCamera.m_aCams[0].m_vecFront=CVector{0,1,.25f};TheCamera.m_aCams[0].m_vecUp=CVector{0,0,1};StaticRef<uint32>(0xB7CB84)=variant%2?1000:0xF0001234;StaticRef<int32>(0xC8A80C)=int32(variant%3);std::memset(reinterpret_cast<void*>(0xB7CD60),0,24);
    CVector origin{4,5,6},muzzle{4.5f,5.5f,6.5f},target{7,8,9},drive{1,2,3};CEntity* explicitTarget=variant%4==1?targetObject:nullptr;CVector* explicitPoint=variant%4==2?&target:nullptr;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;const bool result=reinterpret_cast<bool(__thiscall*)(CWeapon*,CEntity*,CVector*,CVector*,CEntity*,CVector*,CVector*,bool,bool)>(fn.address)(self,owner,&origin,&muzzle,explicitTarget,explicitPoint,variant%3==0?&drive:nullptr,explicitTarget||explicitPoint?true:variant%2!=0,variant%2!=0);
    return json{{"weapon",Digest(weapon.data(),weapon.size())},{"owner",Digest(ownerBytes.data(),ownerBytes.size())},{"target",Digest(targetBytes.data(),targetBytes.size())},{"player_data",Digest(playerData.data(),playerData.size())},{"matrix",Digest(matrix,sizeof(matrix))},{"world",Digest(reinterpret_cast<void*>(0xB7CD60),24)},{"counter",StaticRef<int32>(0xC8A80C)},{"result",result},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<uintptr_t>(0xA9B0C8+2000*4)=savedModel;StaticRef<uint32>(0xB7CB84)=savedMS;std::memcpy(reinterpret_cast<void*>(0xB7CD60),savedWorld.data(),savedWorld.size());StaticRef<int32>(0xC8A80C)=savedCounter;TheCamera.m_nActiveCam=savedActive;std::memcpy(TheCamera.m_aCams.data(),savedCams.data(),savedCams.size());
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
