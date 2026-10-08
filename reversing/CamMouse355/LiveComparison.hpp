#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cam.h"
#include "Camera.h"
#include "Pad.h"
#include "RwHelper.h"
#include "Entity/Ped/Ped.h"
#include "World.h"
#include "Entity/Vehicle/Vehicle.h"
#include "../ProbeRuntime.hpp"
namespace CamMouse355Live {
inline json trace; inline uint32 scenario; inline uint32 sightQueries,sphereQueries; inline CVehicle* fakeVehicle; inline CEntity* fakeHit; inline RpHAnimHierarchy hierarchy; inline RwMatrix bones;
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
inline void __cdecl ClearMouse(){trace.push_back({"clearMouse"});}
inline CVehicle* __cdecl Vehicle(int32 player,bool remote){trace.push_back({"playerVehicle",player,remote});return scenario%4==0?nullptr:fakeVehicle;}
inline bool __fastcall MouseMode(const CCam*,void*){trace.push_back({"mouseMode"});return scenario%3!=0;}
inline bool __cdecl Sight(const CVector& from,const CVector& to,CColPoint& point,CEntity*& entity,bool a,bool b,bool c,bool d,bool e,bool f,bool g,bool h){trace.push_back({"sight",F(from.x),F(from.y),F(from.z),F(to.x),F(to.y),F(to.z),a,b,c,d,e,f,g,h,uintptr_t(CWorld::pIgnoreEntity)});const auto q=sightQueries++;const CVector origin=from,destination=to;point.m_vecPoint=origin+(destination-origin)*(float(1+(scenario/7)%9)*.1f);entity=fakeHit;return q==0?scenario%3!=0:scenario%4<2;}
inline CEntity* __cdecl Sphere(CVector p,float radius,CEntity* ignore,bool a,bool b,bool c,bool d,bool e,bool f){trace.push_back({"sphere",F(p.x),F(p.y),F(p.z),F(radius),uintptr_t(ignore),a,b,c,d,e,f});StaticRef<CVector>(0xB9B250)=CVector{p.x+float(scenario%5)*.03125f,p.y+.0625f,p.z+.125f};return sphereQueries++<scenario%7?fakeHit:nullptr;}
inline bool done; inline std::ofstream log;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,1> fns{{{"CCam","Process_FollowPedWithMouse",0x50F970,0,-1,1024}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name){ReversibleHooks::ReversibleHook::Base* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)result=h.get();});return result;}
inline void Update(){
 if(done)return;char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);auto dir=std::filesystem::path(executable).parent_path();
 if(!std::filesystem::exists(dir/"cam-mouse355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-mouse355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-mouse355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch clear0(0x541BD0,ClearMouse),clear1(bit_cast<uintptr_t>(&CPad::ClearMouseHistory),ClearMouse);
 EntryPatch vehicle0(0x56E0D0,Vehicle),vehicle1(bit_cast<uintptr_t>(&FindPlayerVehicle),Vehicle);
 EntryPatch mode0(0x50A850,MouseMode),mode1(bit_cast<uintptr_t>(&CCam::Using3rdPersonMouseCam),MouseMode);
 EntryPatch sight0(0x56BA00,Sight),sight1(bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),Sight),sphere0(0x569E20,Sphere),sphere1(bit_cast<uintptr_t>(&CWorld::TestSphereAgainstWorld),Sphere);
 EntryPatch cb0(0x509C50,Clip),cb1(bit_cast<uintptr_t>(&CCam::ClipBeta),Clip),vectors0(0x509CE0,Vectors),vectors1(bit_cast<uintptr_t>(&CCam::GetVectorsReadyForRW),Vectors);
 EntryPatch n0(0x7EE1D0,Near),n1(bit_cast<uintptr_t>(&RwCameraSetNearClipPlane),Near),pad(0x53FB70,GetPad);
 struct Region{uintptr_t address;std::vector<uint8> saved;};
 std::array<Region,13> globals{{{0xB6F028,std::vector<uint8>(sizeof(CCamera))},{0xB6FFC4,std::vector<uint8>(32)},{0xB73424,std::vector<uint8>(8)},{0xB6EC10,std::vector<uint8>(32)},{0xB7CB5C,std::vector<uint8>(4)},{0x8CCC54,std::vector<uint8>(12)},{0xB73458,std::vector<uint8>(sizeof(CPad)*2)},{0xB6F174,std::vector<uint8>(4)},{0xC1703C,std::vector<uint8>(4)},{0xC3EFAB,std::vector<uint8>(1)},{0xB73402,std::vector<uint8>(1)},{0xB9B250,std::vector<uint8>(12)},{0xB7CD68,std::vector<uint8>(4)}}};
 for(auto& g:globals)std::memcpy(g.saved.data(),reinterpret_cast<void*>(g.address),g.saved.size());
 bool passed=true;uint32 pairs=0;auto* hook=FindHook("CCam","Process_FollowPedWithMouse");if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);int failures=0;
 for(int test=0;test<1024;++test){
  auto run=[&](bool candidate){
   for(auto& g:globals)std::memcpy(reinterpret_cast<void*>(g.address),g.saved.data(),g.saved.size());
   scenario=test;sightQueries=0;sphereQueries=0;trace=json::array();hook->State(candidate);
   alignas(CCam) std::array<uint8,sizeof(CCam)> camBytes{};auto* c=reinterpret_cast<CCam*>(camBytes.data());
   alignas(CPed) std::array<uint8,sizeof(CPed)> pedBytes{};auto* ped=reinterpret_cast<CPed*>(pedBytes.data());
   alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrixBytes{};auto* matrix=reinterpret_cast<CMatrix*>(matrixBytes.data());
   alignas(CEntity) std::array<uint8,sizeof(CEntity)> attachedBytes{};auto* attached=reinterpret_cast<CEntity*>(attachedBytes.data());
   alignas(CVehicle) std::array<uint8,sizeof(CVehicle)> vehicleBytes{};fakeVehicle=reinterpret_cast<CVehicle*>(vehicleBytes.data());At<uint32>(fakeVehicle,0x590)=uint32((test/4)%8);
   alignas(CEntity) std::array<uint8,sizeof(CEntity)> hitBytes{};fakeHit=reinterpret_cast<CEntity*>(hitBytes.data());At<uint8>(fakeHit,0x36)=test%2==0?3:4;
   RpClump clump{};RwFrame frame{};RwCamera camera{};camera.nearPlane=.1f+float(test%7)*.125f;clump.object.type=2;clump.object.parent=&frame;
   At<uintptr_t>(pedBytes.data(),0x14)=uintptr_t(matrix);At<uintptr_t>(pedBytes.data(),0x18)=test%16==0?0:uintptr_t(&clump);At<uint8>(pedBytes.data(),0x36)=test%16==1?2:3;
   matrix->GetRight()=CVector{1,0,0};matrix->GetForward()=CVector{.125f,1,0};matrix->GetUp()=CVector{0,0,1};matrix->GetPosition()=CVector{float(test%7),float(test%11),float(test%5)};
   ped->m_fCurrentRotation=float(int32(test%9)-4)*.375f;ped->m_pAttachedTo=test%4==2?reinterpret_cast<CPhysical*>(attached):nullptr;ped->m_fTurretAngleA=uint16((test/4)%5);ped->m_fTurretAngleB=float(test%7)*.125f;
   c->m_pCamTargetEntity=ped;c->m_bResetStatics=test%2;c->m_nMode=test%3==0?MODE_SNIPER_RUNABOUT:MODE_1STPERSON;c->m_fFOV=70.f;c->m_fDistance=float((test/3)%13)*.25f;At<float>(c,0x9C)=float(test%7)*.25f;c->m_vecSource=CVector{float(test%7),float(test%11),float(test%5)};c->m_fVerticalAngle=float(int32(test%13)-6)*.25f;c->m_fHorizontalAngle=float(int32(test%17)-8)*.5f;c->m_vecBufferedPlayerBodyOffset=CVector{.5f,.25f,.125f};
   TheCamera.m_pRwCamera=&camera;Scene.m_pRwCamera=&camera;TheCamera.m_bHeadBob=((test/2)%2)!=0;StaticRef<CPed*>(0xB6F980)=ped;
   StaticRef<CVector>(0xB6FFC4)=CVector{.0625f,.125f,.25f};StaticRef<uint32>(0xB6FFE0)=0x12340000;StaticRef<bool>(0xB6FFDC)=true;StaticRef<float>(0xB6FC48)=float(test%5)*.25f;
   StaticRef<float>(0xB73424)=test%4?float(int32(test%5)-2)*.5f:0.f;StaticRef<float>(0xB73428)=test%4?float(int32((test/5)%5)-2)*.5f:0.f;StaticRef<float>(0xB6EC18)=.003f;StaticRef<float>(0xB6EC1C)=.004f;StaticRef<float>(0xB7CB5C)=float(1+test%5)*.25f;
   StaticRef<CVector>(0x8CCC54)=CVector{.1f,.2f,.3f};std::memset(&bones,0,sizeof(bones));bones.right={1,0,0};bones.up={0,1,0};bones.at={0,0,1};hierarchy={};hierarchy.pMatrixArray=&bones;
   StaticRef<uint8>(0xB6F042)=test%5==0;StaticRef<uint8>(0xB6F043)=test%7==0;StaticRef<uint8>(0xB6F05F)=test%9==0;StaticRef<float>(0xB6F148)=.375f;StaticRef<uint8>(0xB6F079)=test%3!=0;StaticRef<uint16>(0xB6FC58)=uint16((test/3)%3);StaticRef<uint8>(0xC3EFAB)=uint8(test%256);StaticRef<uint8>(0xB73402)=uint8((test/8)%2);
   std::memset(CPad::Pads,0,sizeof(CPad::Pads));At<int16>(&CPad::Pads[0],4)=test%16==0?int16(-32768):test%16==8?int16(32767):int16(int32((test/4)%9)*32-128);At<int16>(&CPad::Pads[0],6)=test%8==0?int16(-32768):int16(int32((test/36)%9)*32-128);At<int16>(&CPad::Pads[0],0x10E)=int16(test%7==0?32:(test%11==0?1:0));At<int16>(&CPad::Pads[0],0x26)=int16(test%5==0);
   const bool switched=ReversingProbe::JumpsToDll(0x50F970)==candidate;const CVector target{1,2,3};reinterpret_cast<void(__thiscall*)(CCam*,const CVector&,float,float,float)>(0x50F970)(c,target,.375f,.5f,.75f);
   json globalState=json::array();for(auto& g:globals)globalState.push_back(Bytes(reinterpret_cast<void*>(g.address),g.saved.size()));
   return json{{"sightQueries",sightQueries},{"sphereQueries",sphereQueries},{"vehicle",Bytes(vehicleBytes.data(),vehicleBytes.size())},{"hit",Bytes(hitBytes.data(),hitBytes.size())},{"cam",Bytes(camBytes.data(),camBytes.size())},{"ped",Bytes(pedBytes.data(),pedBytes.size())},{"matrix",Bytes(matrixBytes.data(),matrixBytes.size())},{"frame",Bytes(&frame,sizeof(frame))},{"bones",Bytes(&bones,sizeof(bones))},{"camera",Bytes(&camera,sizeof(camera))},{"globals",globalState},{"trace",trace},{"switches",switched}};
  };
  auto original=run(false),candidate=run(true);bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
  Write({{"event","comparison"},{"pair",pairs++},{"function","CCam::Process_FollowPedWithMouse"},{"case",test},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
 }
 hook->State(enabled);hook->LockState(locked);for(auto& g:globals)std::memcpy(reinterpret_cast<void*>(g.address),g.saved.data(),g.saved.size());
 Write({{"event","function_summary"},{"function","CCam::Process_FollowPedWithMouse"},{"failures",failures},{"cases",1024}});Write({{"event",passed?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
