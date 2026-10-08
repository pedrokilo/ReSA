#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cam.h"
#include "Camera.h"
#include "Pad.h"
#include "RwHelper.h"
#include "ControllerConfigManager.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Ped/Ped.h"
#include "PedIntelligence.h"
#include "Tasks/TaskTypes/TaskSimpleUseGun.h"
#include "PlayerPedData.h"
#include "PedClothesDesc.h"
#include "WeaponInfo.h"
#include "Weapon.h"
#include "../ProbeRuntime.hpp"
namespace CamAim355Live {
inline json trace; inline uint32 scenario; inline RpHAnimHierarchy hierarchy; inline RwMatrix bones;
inline json Bytes(const void* p,size_t n){json r=json::array();auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i)r.push_back(b[i]);return r;}
inline uint32 F(float x){return std::bit_cast<uint32>(x);}
template<class T> T& At(void* b,size_t o){return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o);}
inline RpHAnimHierarchy* __cdecl Hierarchy(RpClump*){trace.push_back({"hierarchy"});return &hierarchy;}
inline int32 __cdecl BoneTag(int32 n){trace.push_back({"boneTag",n});return 3;}
inline int32 __cdecl BoneIndex(RpHAnimHierarchy*,int32 n){trace.push_back({"boneIndex",n});return 0;}
inline RwMatrix* __cdecl BoneArray(RpHAnimHierarchy*){trace.push_back({"boneArray"});return &bones;}
inline RwV3d* __cdecl Transform(RwV3d* out,const RwV3d* in,int32 n,const RwMatrix* m){trace.push_back({"transform",F(in->x),F(in->y),F(in->z),n,Bytes(m,sizeof(*m))});*out={float(int32(scenario%9)-4)*.0625f,.125f,float((scenario/9)%5)*.125f};return out;}
inline RwMatrix* __cdecl Scale(RwMatrix* m,const RwV3d* v,RwOpCombineType type){trace.push_back({"scale",F(v->x),F(v->y),F(v->z),int32(type)});m->right={0,0,0};m->up={0,0,0};m->at={0,0,0};return m;}
inline void __fastcall BonePosition(CPed*,void*,RwV3d& p,uint32 n,bool update){trace.push_back({"bonePosition",n,update});p={1,2,3};}
inline int16 __fastcall Left(CPad*,void*,CPed*){trace.push_back({"left"});return int16(int32((scenario/4)%9)*24-96);}
inline int16 __fastcall Up(CPad*,void*,CPed*){trace.push_back({"up"});return int16(int32((scenario/36)%9)*24-96);}
inline bool __fastcall ZoomIn(const CPad*,void*){trace.push_back({"zoomIn"});return scenario%4==0;}
inline bool __fastcall ZoomOut(const CPad*,void*){trace.push_back({"zoomOut"});return scenario%4==1;}
inline bool __fastcall Player(const CPed*,void*){trace.push_back({"isPlayer"});return scenario%3!=0;}
inline float __fastcall Heading(const CPlaceable*,void*){trace.push_back({"attachedHeading"});return float(int32((scenario/8)%7)-3)*.5f;}
inline void __fastcall Clip(CCam* c,void*){trace.push_back({"clip",F(c->m_fHorizontalAngle)});while(c->m_fHorizontalAngle>3.1415927f)c->m_fHorizontalAngle-=6.2831855f;while(c->m_fHorizontalAngle< -3.1415927f)c->m_fHorizontalAngle+=6.2831855f;}
inline void __fastcall Vectors(CCam* c,void*){trace.push_back({"vectors",F(c->m_vecFront.x),F(c->m_vecFront.y),F(c->m_vecFront.z)});c->m_vecUp=CVector{0,0,1};}
inline RwCamera* __cdecl Near(RwCamera* camera,float plane){trace.push_back({"near",F(plane)});camera->nearPlane=plane;return camera;}
inline CPad* __cdecl GetPad(int32 n){return &CPad::Pads[n];}
inline CTaskSimpleUseGun* gunTask;inline CWeaponInfo* weaponInfo;
inline CTaskSimpleUseGun* __fastcall GunTask(CPedIntelligence*,void*){trace.push_back({"gunTask"});return scenario%4==0?nullptr:gunTask;}
inline void* __fastcall FightTask(CPedIntelligence*,void*){trace.push_back({"fightTask"});return scenario%5==0?nullptr:reinterpret_cast<void*>(1);}
inline void* __fastcall JetTask(CPedIntelligence*,void*){trace.push_back({"jetTask"});return scenario%7==0?reinterpret_cast<void*>(1):nullptr;}
inline CWeaponInfo* __cdecl Info(eWeaponType type,eWeaponSkill skill){trace.push_back({"weaponInfo",int(type),int(skill)});return weaponInfo;}
inline bool __fastcall Melee(CWeapon*,void*){trace.push_back({"melee"});return scenario%4==0;}
inline bool __fastcall Hair(CPedClothesDesc*,void*,int arg){trace.push_back({"hair",arg});return scenario%5==0;}
inline int16 __fastcall AimX(const CPad*,void*,CPed*){trace.push_back({"aimX"});return int16(int32((scenario/4)%9)*32-128);}
inline int16 __fastcall AimY(const CPad*,void*,CPed*){trace.push_back({"aimY"});return int16(int32((scenario/36)%9)*32-128);}
inline int32 __fastcall Fire(const CPad*,void*,CPed*){trace.push_back({"fire"});return int32(scenario%4==0);}
inline bool __fastcall Enter(const CPad*,void*){trace.push_back({"enter"});return scenario%3==0;}
inline void __fastcall DuckReset(CCamera*,void*,CPed*){trace.push_back({"duckReset"});}
inline void __fastcall DuckMotion(CCamera*,void*,CPed*,CVector* source,CVector* target,bool fifth){trace.push_back({"duckMotion",Bytes(source,sizeof(*source)),Bytes(target,sizeof(*target)),fifth});source->z-=.125f;target->z-=.125f;}
inline void __cdecl ColVars(int state){trace.push_back({"colVars",state});}
inline void __fastcall Generic(CCamera*,void*,CPed*){trace.push_back({"generic"});}
inline void __fastcall PedCases(CCamera*,void*,CPed*){trace.push_back({"pedCases"});}
inline bool __fastcall ColReact(CCamera*,void*,CVector* source,CVector* target){trace.push_back({"colReact",Bytes(source,sizeof(*source)),Bytes(target,sizeof(*target))});source->x+=.125f;return true;}
inline void __fastcall Improve(CCamera*,void*,CVehicle* vehicle,CPed*,CVector* source,CVector* target){trace.push_back({"improve",vehicle==nullptr,Bytes(source,sizeof(*source)),Bytes(target,sizeof(*target))});}
inline float __fastcall QuickPitch(const CCamera*,void*){trace.push_back({"quickPitch"});return .125f;}
inline uint32 sightCalls;
inline KeyCode __fastcall MouseButton(CControllerConfigManager*,void*,eControllerAction action){trace.push_back({"mouseButton",int(action)});return static_cast<KeyCode>(4+(scenario/16+int(action))%2);}
inline void __fastcall Frame(CEntity*,void*){trace.push_back({"frame"});}
inline void __fastcall Animation(CEntity*,void*){trace.push_back({"animation"});}
inline void __fastcall AttachedPosition(CPed*,void*){trace.push_back({"attachedPosition"});}
inline void __fastcall Heli(CAutomobile* v,void*,float heading){trace.push_back({"heli",F(heading)});At<float>(v,0x954)=heading;}
inline bool __cdecl Sight(const CVector& a,const CVector& b,bool c,bool d,bool e,bool f,bool g,bool h,bool i){trace.push_back({"sight",F(a.x),F(a.y),F(a.z),F(b.x),F(b.y),F(b.z),c,d,e,f,g,h,i});return sightCalls++!=scenario%4;}
inline bool done; inline std::ofstream log;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,1> fns{{{"CCam","Process_AimWeapon",0x521500,0,-1,1024}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name){ReversibleHooks::ReversibleHook::Base* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)result=h.get();});return result;}
inline void Update(){
 if(done)return;char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);auto dir=std::filesystem::path(executable).parent_path();
 if(!std::filesystem::exists(dir/"cam-aim355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-aim355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-aim355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch h0(0x734A40,Hierarchy),h1(bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump),Hierarchy);
 EntryPatch t0(0x4D58A0,BoneTag),t1(bit_cast<uintptr_t>(&ConvertPedNode2BoneTag),BoneTag);
 EntryPatch i0(0x7C51A0,BoneIndex),i1(bit_cast<uintptr_t>(&RpHAnimIDGetIndex),BoneIndex);
 EntryPatch a0(0x7C5120,BoneArray),a1(bit_cast<uintptr_t>(&RpHAnimHierarchyGetMatrixArray),BoneArray);
 EntryPatch x0(0x7EDD90,Transform),x1(bit_cast<uintptr_t>(&RwV3dTransformPoints),Transform);
 EntryPatch s0(0x7F22C0,Scale),s1(bit_cast<uintptr_t>(&RwMatrixScale),Scale);
 EntryPatch b0(0x5E4280,BonePosition),b1(bit_cast<uintptr_t>(static_cast<void(CPed::*)(CVector*,eBoneTag,bool)>(&CPed::GetBonePosition)),BonePosition);
 EntryPatch l0(0x540BD0,Left),l1(bit_cast<uintptr_t>(&CPad::LookAroundLeftRight),Left);
 EntryPatch u0(0x540CC0,Up),u1(bit_cast<uintptr_t>(&CPad::LookAroundUpDown),Up);
 EntryPatch zi0(0x540B30,ZoomIn),zi1(bit_cast<uintptr_t>(&CPad::SniperZoomIn),ZoomIn);
 EntryPatch zo0(0x540B80,ZoomOut),zo1(bit_cast<uintptr_t>(&CPad::SniperZoomOut),ZoomOut);
 EntryPatch p0(0x5DF8F0,Player),p1(bit_cast<uintptr_t>(&CPed::IsPlayer),Player);
 EntryPatch gh0(0x441DB0,Heading),gh1(bit_cast<uintptr_t>(&CPlaceable::GetHeading),Heading);
 EntryPatch cb0(0x509C50,Clip),cb1(bit_cast<uintptr_t>(&CCam::ClipBeta),Clip);
 EntryPatch v0(0x509CE0,Vectors),v1(bit_cast<uintptr_t>(&CCam::GetVectorsReadyForRW),Vectors);
 EntryPatch n0(0x7EE1D0,Near),n1(bit_cast<uintptr_t>(&RwCameraSetNearClipPlane),Near),pad(0x53FB70,GetPad);
 EntryPatch mb0(0x52F580,MouseButton),mb1(bit_cast<uintptr_t>(&CControllerConfigManager::GetMouseButtonAssociatedWithAction),MouseButton);
 EntryPatch fr0(0x532B00,Frame),fr1(bit_cast<uintptr_t>(&CEntity::UpdateRwFrame),Frame),an0(0x532B20,Animation),an1(bit_cast<uintptr_t>(&CEntity::UpdateRpHAnim),Animation);
 EntryPatch at0(0x5DFDF0,AttachedPosition),at1(bit_cast<uintptr_t>(&CPed::PositionAttachedPed),AttachedPosition),he0(0x6A2450,Heli),he1(bit_cast<uintptr_t>(&CAutomobile::SetHeliOrientation),Heli);
 EntryPatch los0(0x56A490,Sight),los1(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),Sight);
 EntryPatch gu0(0x600F70,GunTask),gu1(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskUseGun),GunTask),fi0(0x600F30,FightTask),fi1(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskFighting),FightTask),je0(0x601110,JetTask),je1(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskJetPack),JetTask);
 EntryPatch wi0(0x743C60,Info),wi1(bit_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)),Info),me0(0x73B1C0,Melee),me1(bit_cast<uintptr_t>(&CWeapon::IsTypeMelee),Melee),ha0(0x5A7970,Hair),ha1(bit_cast<uintptr_t>(&CPedClothesDesc::HasVisibleNewHairCut),Hair);
 EntryPatch ax0(0x541040,AimX),ax1(bit_cast<uintptr_t>(&CPad::AimWeaponLeftRight),AimX),ay0(0x5410C0,AimY),ay1(bit_cast<uintptr_t>(&CPad::AimWeaponUpDown),AimY),fw0(0x540180,Fire),fw1(bit_cast<uintptr_t>(&CPad::GetWeapon),Fire),en0(0x5406B0,Enter),en1(bit_cast<uintptr_t>(&CPad::GetEnterTargeting),Enter);
 EntryPatch dr0(0x50CEF0,DuckReset),dr1(bit_cast<uintptr_t>(&CCamera::ResetDuckingSystem),DuckReset),dm0(0x50D090,DuckMotion),dm1(bit_cast<uintptr_t>(&CCamera::HandleCameraMotionForDuckingDuringAim),DuckMotion);
 EntryPatch cv0(0x50CBF0,ColVars),cv1(bit_cast<uintptr_t>(&CCamera::SetColVarsAimWeapon),ColVars),gc0(0x50CD30,Generic),gc1(bit_cast<uintptr_t>(&CCamera::CameraGenericModeSpecialCases),Generic),pc0(0x50CDA0,PedCases),pc1(bit_cast<uintptr_t>(&CCamera::CameraPedAimModeSpecialCases),PedCases);
 EntryPatch cr0(0x520190,ColReact),cr1(bit_cast<uintptr_t>(&CCamera::CameraColDetAndReact),ColReact),im0(0x516B20,Improve),im1(bit_cast<uintptr_t>(&CCamera::ImproveNearClip),Improve),qp0(0x50AD40,QuickPitch),qp1(bit_cast<uintptr_t>(&CCamera::Find3rdPersonQuickAimPitch),QuickPitch);
 struct Region{uintptr_t address;std::vector<uint8> saved;};
 std::array<Region,15> globals{{{0xB6F028,std::vector<uint8>(sizeof(CCamera))},{0xB6FFC4,std::vector<uint8>(64)},{0xB73420,std::vector<uint8>(12)},{0xB6EC10,std::vector<uint8>(80)},{0xB7CB5C,std::vector<uint8>(4)},{0x8CCC54,std::vector<uint8>(12)},{0xB73458,std::vector<uint8>(sizeof(CPad)*2)},{0xB6F174,std::vector<uint8>(4)},{0xC1703C,std::vector<uint8>(4)},{0xA44498,std::vector<uint8>(8)},{0xB7CB84,std::vector<uint8>(4)},{0xB700F4,std::vector<uint8>(32)},{0x8CC530,std::vector<uint8>(4)},{0x8CC384,std::vector<uint8>(4)},{0x8CCE64,std::vector<uint8>(1)}}};
 for(auto& g:globals)std::memcpy(g.saved.data(),reinterpret_cast<void*>(g.address),g.saved.size());
 bool passed=true;uint32 pairs=0;auto* hook=FindHook("CCam","Process_AimWeapon");if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);int failures=0;
 for(int test=0;test<1024;++test){
  auto run=[&](bool candidate){
   for(auto& g:globals)std::memcpy(reinterpret_cast<void*>(g.address),g.saved.data(),g.saved.size());
   sightCalls=0;scenario=test;trace=json::array();hook->State(candidate);
   alignas(CCam) std::array<uint8,sizeof(CCam)> camBytes{};auto* c=reinterpret_cast<CCam*>(camBytes.data());
   alignas(CPed) std::array<uint8,sizeof(CPed)> pedBytes{};auto* ped=reinterpret_cast<CPed*>(pedBytes.data());
   alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrixBytes{};auto* matrix=reinterpret_cast<CMatrix*>(matrixBytes.data());
   alignas(CAutomobile) std::array<uint8,sizeof(CAutomobile)> attachedBytes{};auto* attached=reinterpret_cast<CAutomobile*>(attachedBytes.data());
   alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> attachedMatrixBytes{};auto* attachedMatrix=reinterpret_cast<CMatrix*>(attachedMatrixBytes.data());
   At<uintptr_t>(attached,0x14)=uintptr_t(attachedMatrix);At<uint8>(attached,0x36)=2;At<uint32>(attached,0x590)=test%3==0?9:0;At<uint32>(attached,0x594)=test%5==0?3:0;At<float>(attached,0x954)=1.f;
   attachedMatrix->GetRight()=CVector{1,0,float(int32(test%7)-3)*.125f};attachedMatrix->GetForward()=CVector{0,1,float(int32(test%11)-5)*.125f};attachedMatrix->GetUp()=CVector{0,0,1};
   alignas(CPedIntelligence) std::array<uint8,sizeof(CPedIntelligence)> intelligenceBytes{};At<CPedIntelligence*>(ped,0x47C)=reinterpret_cast<CPedIntelligence*>(intelligenceBytes.data());
   CPlayerPedData playerData{};CPedClothesDesc clothes;std::memset(&clothes,0,sizeof(clothes));playerData.m_pPedClothesDesc=&clothes;At<CPlayerPedData*>(ped,0x480)=&playerData;
   alignas(CTaskSimpleUseGun) std::array<uint8,sizeof(CTaskSimpleUseGun)> gunBytes{};gunTask=reinterpret_cast<CTaskSimpleUseGun*>(gunBytes.data());CWeaponInfo info{};weaponInfo=&info;gunTask->m_WeaponInfo=&info;info.m_nWeaponFire=static_cast<eWeaponFire>(test%4==0?0:1);info.m_nFlags=(test&32)?2:0;
   alignas(CEntity) std::array<uint8,sizeof(CEntity)> targetBytes{};auto* aim=reinterpret_cast<CEntity*>(targetBytes.data());CMatrixLink targetMatrix;std::memset(&targetMatrix,0,sizeof(targetMatrix));targetMatrix.GetPosition()=CVector{3,4,5};At<CMatrixLink*>(aim,0x14)=&targetMatrix;At<uint8>(aim,0x36)=test%7==0?3:4;ped->m_pTargetedObject=(test&4)?aim:nullptr;ped->m_nMoveState=static_cast<eMoveState>(test%6);
   ped->m_nActiveWeaponSlot=0;constexpr eWeaponType weapons[]{WEAPON_UNARMED,WEAPON_AK47,WEAPON_M4,WEAPON_COUNTRYRIFLE,WEAPON_SPRAYCAN,WEAPON_EXTINGUISHER};ped->m_aWeapons[0].m_Type=weapons[(test/4)%6];ped->field_578=CVector{.125f,.25f,.375f};
   ped->m_pVehicle=attached;attached->m_pDriver=(test&8)?ped:nullptr;
   RpClump clump{};RwFrame frame{};RwCamera camera{};clump.object.type=2;clump.object.parent=&frame;
   At<uintptr_t>(pedBytes.data(),0x14)=uintptr_t(matrix);At<uintptr_t>(pedBytes.data(),0x18)=test%16==0?0:uintptr_t(&clump);At<uint8>(pedBytes.data(),0x36)=test%16==1?2:3;
   matrix->GetRight()=CVector{1,0,0};matrix->GetForward()=CVector{.125f,1,0};matrix->GetUp()=CVector{0,0,1};matrix->GetPosition()=CVector{float(test%7),float(test%11),float(test%5)};
   ped->m_fCurrentRotation=float(int32(test%9)-4)*.375f;ped->m_pAttachedTo=test%4==2?reinterpret_cast<CPhysical*>(attached):nullptr;ped->m_fTurretAngleA=uint16((test/4)%5);ped->m_fTurretAngleB=float(test%7)*.125f;
   c->m_pCamTargetEntity=ped;c->m_bResetStatics=test%2;c->m_nMode=static_cast<eCamMode>(test%3==0?7:test%3==1?46:4);c->m_bCollisionChecksOn=(test&8)!=0;c->m_nCamBumpedTime=(test&32)?100:0;c->m_fCamBumpedHorz=.0625f;c->m_fCamBumpedVert=-.125f;c->m_fFOV=15.f+float(test%56);c->m_fVerticalAngle=float(int32(test%13)-6)*.25f;c->m_fHorizontalAngle=float(int32(test%17)-8)*.5f;c->m_vecBufferedPlayerBodyOffset=CVector{.5f,.25f,.125f};
   At<uint32>(ped,0x46C)=uint32((test&16)?0x4000000:0)|uint32((test&64)?0x100:0)|uint32((test&128)?1:0);StaticRef<uint32>(0xB70110)=test%8;StaticRef<CVector>(0xB70104)=CVector{2,3,4};StaticRef<float>(0xB70100)=3.f;StaticRef<float>(0xB700FC)=20.f;StaticRef<float>(0xB700F4)=.25f;StaticRef<float>(0xB700F8)=float(int32(test%9)-4)*25.f;StaticRef<float>(0x8CC530)=test%5==0?-1001.f:.5f;StaticRef<bool>(0x8CCE64)=(test&256)!=0;StaticRef<int32>(0x8CC384)=test%3==0?3:0;StaticRef<bool>(0xB6EC44)=(test&128)!=0;StaticRef<int32>(0xB6EC48)=test%3==0?60000:0;ped->m_nTurretPosnMode=.5f;StaticRef<float>(0xA44498)=(test&64)?.05f:0.f;StaticRef<float>(0xA4449C)=.5f;StaticRef<float>(0xB6FFE8)=float(test%90);StaticRef<bool>(0xB6FFF4)=(test&256)!=0;StaticRef<bool>(0xB6EC2E)=(test&128)!=0;StaticRef<uint32>(0xB7CB84)=200+test%300;StaticRef<float>(0xB73420)=float(int32((test/16)%3)-1)*120.f;At<uint16>(&CPad::Pads[0],0x10E)=(test&512)?1:0;At<uint8>(&CPad::Pads[0],0x117)=test%17==0;TheCamera.m_pRwCamera=&camera;Scene.m_pRwCamera=&camera;TheCamera.m_bHeadBob=((test/2)%2)!=0;StaticRef<CPed*>(0xB6F980)=ped;
   StaticRef<CVector>(0xB6FFC4)=CVector{.0625f,.125f,.25f};StaticRef<uint32>(0xB6FFE0)=0x12340000;StaticRef<bool>(0xB6FFDC)=true;StaticRef<float>(0xB6FC48)=float(test%5)*.25f;
   StaticRef<float>(0xB73424)=test%4?float(int32(test%5)-2)*.5f:0.f;StaticRef<float>(0xB73428)=test%4?float(int32((test/5)%5)-2)*.5f:0.f;StaticRef<float>(0xB6EC18)=.003f;StaticRef<float>(0xB6EC1C)=.004f;StaticRef<float>(0xB7CB5C)=float(1+test%5)*.25f;
   StaticRef<CVector>(0x8CCC54)=CVector{.1f,.2f,.3f};std::memset(&bones,0,sizeof(bones));bones.right={1,0,0};bones.up={0,1,0};bones.at={0,0,1};hierarchy={};hierarchy.pMatrixArray=&bones;
   const bool switched=ReversingProbe::JumpsToDll(0x521500)==candidate;const CVector target{1,2,3};reinterpret_cast<void(__thiscall*)(CCam*,const CVector&,float,float,float)>(0x521500)(c,target,.375f,.5f,.75f);
   json globalState=json::array();for(auto& g:globals)globalState.push_back(Bytes(reinterpret_cast<void*>(g.address),g.saved.size()));
   return json{{"cam",Bytes(camBytes.data(),camBytes.size())},{"ped",Bytes(pedBytes.data(),pedBytes.size())},{"intelligence",Bytes(intelligenceBytes.data(),intelligenceBytes.size())},{"playerData",Bytes(&playerData,sizeof(playerData))},{"clothes",Bytes(&clothes,sizeof(clothes))},{"gun",Bytes(gunBytes.data(),gunBytes.size())},{"weaponInfo",Bytes(&info,sizeof(info))},{"aimEntity",Bytes(targetBytes.data(),targetBytes.size())},{"aimMatrix",Bytes(&targetMatrix,sizeof(targetMatrix))},{"attached",Bytes(attachedBytes.data(),attachedBytes.size())},{"attachedMatrix",Bytes(attachedMatrixBytes.data(),attachedMatrixBytes.size())},{"matrix",Bytes(matrixBytes.data(),matrixBytes.size())},{"frame",Bytes(&frame,sizeof(frame))},{"bones",Bytes(&bones,sizeof(bones))},{"camera",Bytes(&camera,sizeof(camera))},{"globals",globalState},{"trace",trace},{"switches",switched}};
  };
  auto original=run(false),candidate=run(true);bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
  Write({{"event","comparison"},{"pair",pairs++},{"function","CCam::Process_AimWeapon"},{"case",test},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
 }
 hook->State(enabled);hook->LockState(locked);for(auto& g:globals)std::memcpy(reinterpret_cast<void*>(g.address),g.saved.data(),g.saved.size());
 Write({{"event","function_summary"},{"function","CCam::Process_AimWeapon"},{"failures",failures},{"cases",1024}});Write({{"event",passed?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
