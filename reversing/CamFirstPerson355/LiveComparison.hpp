#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cam.h"
#include "Camera.h"
#include "Pad.h"
#include "RwHelper.h"
#include "Entity/Ped/Ped.h"
#include "../ProbeRuntime.hpp"
namespace CamFirstPerson355Live {
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
inline int16 __fastcall Left(CPad*,void*){trace.push_back({"left"});return int16(int32((scenario/4)%9)*24-96);}
inline int16 __fastcall Up(CPad*,void*){trace.push_back({"up"});return int16(int32((scenario/36)%9)*24-96);}
inline bool __fastcall ZoomIn(const CPad*,void*){trace.push_back({"zoomIn"});return scenario%4==0;}
inline bool __fastcall ZoomOut(const CPad*,void*){trace.push_back({"zoomOut"});return scenario%4==1;}
inline bool __fastcall Player(const CPed*,void*){trace.push_back({"isPlayer"});return scenario%3!=0;}
inline float __fastcall Heading(const CPlaceable*,void*){trace.push_back({"attachedHeading"});return float(int32((scenario/8)%7)-3)*.5f;}
inline void __fastcall Clip(CCam* c,void*){trace.push_back({"clip",F(c->m_fHorizontalAngle)});while(c->m_fHorizontalAngle>3.1415927f)c->m_fHorizontalAngle-=6.2831855f;while(c->m_fHorizontalAngle< -3.1415927f)c->m_fHorizontalAngle+=6.2831855f;}
inline void __fastcall Vectors(CCam* c,void*){trace.push_back({"vectors",F(c->m_vecFront.x),F(c->m_vecFront.y),F(c->m_vecFront.z)});c->m_vecUp=CVector{0,0,1};}
inline RwCamera* __cdecl Near(RwCamera* camera,float plane){trace.push_back({"near",F(plane)});camera->nearPlane=plane;return camera;}
inline CPad* __cdecl GetPad(int32 n){return &CPad::Pads[n];}
inline bool done; inline std::ofstream log;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,1> fns{{{"CCam","Process_1rstPersonPedOnPC",0x50EB70,0,-1,1024}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name){ReversibleHooks::ReversibleHook::Base* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)result=h.get();});return result;}
inline void Update(){
 if(done)return;char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);auto dir=std::filesystem::path(executable).parent_path();
 if(!std::filesystem::exists(dir/"cam-first-person355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-first-person355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-first-person355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch h0(0x734A40,Hierarchy),h1(bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump),Hierarchy);
 EntryPatch t0(0x4D58A0,BoneTag),t1(bit_cast<uintptr_t>(&ConvertPedNode2BoneTag),BoneTag);
 EntryPatch i0(0x7C51A0,BoneIndex),i1(bit_cast<uintptr_t>(&RpHAnimIDGetIndex),BoneIndex);
 EntryPatch a0(0x7C5120,BoneArray),a1(bit_cast<uintptr_t>(&RpHAnimHierarchyGetMatrixArray),BoneArray);
 EntryPatch x0(0x7EDD90,Transform),x1(bit_cast<uintptr_t>(&RwV3dTransformPoints),Transform);
 EntryPatch s0(0x7F22C0,Scale),s1(bit_cast<uintptr_t>(&RwMatrixScale),Scale);
 EntryPatch b0(0x5E4280,BonePosition),b1(bit_cast<uintptr_t>(static_cast<void(CPed::*)(CVector*,eBoneTag,bool)>(&CPed::GetBonePosition)),BonePosition);
 EntryPatch l0(0x540E80,Left);
 EntryPatch u0(0x540F80,Up);
 EntryPatch zi0(0x540B30,ZoomIn),zi1(bit_cast<uintptr_t>(&CPad::SniperZoomIn),ZoomIn);
 EntryPatch zo0(0x540B80,ZoomOut),zo1(bit_cast<uintptr_t>(&CPad::SniperZoomOut),ZoomOut);
 EntryPatch p0(0x5DF8F0,Player),p1(bit_cast<uintptr_t>(&CPed::IsPlayer),Player);
 EntryPatch gh0(0x441DB0,Heading),gh1(bit_cast<uintptr_t>(&CPlaceable::GetHeading),Heading);
 EntryPatch cb0(0x509C50,Clip),cb1(bit_cast<uintptr_t>(&CCam::ClipBeta),Clip);
 EntryPatch v0(0x509CE0,Vectors),v1(bit_cast<uintptr_t>(&CCam::GetVectorsReadyForRW),Vectors);
 EntryPatch n0(0x7EE1D0,Near),n1(bit_cast<uintptr_t>(&RwCameraSetNearClipPlane),Near),pad(0x53FB70,GetPad);
 struct Region{uintptr_t address;std::vector<uint8> saved;};
 std::array<Region,9> globals{{{0xB6F028,std::vector<uint8>(sizeof(CCamera))},{0xB6FFC4,std::vector<uint8>(32)},{0xB73424,std::vector<uint8>(8)},{0xB6EC10,std::vector<uint8>(32)},{0xB7CB5C,std::vector<uint8>(4)},{0x8CCC54,std::vector<uint8>(12)},{0xB73458,std::vector<uint8>(sizeof(CPad)*2)},{0xB6F174,std::vector<uint8>(4)},{0xC1703C,std::vector<uint8>(4)}}};
 for(auto& g:globals)std::memcpy(g.saved.data(),reinterpret_cast<void*>(g.address),g.saved.size());
 bool passed=true;uint32 pairs=0;auto* hook=FindHook("CCam","Process_1rstPersonPedOnPC");if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);int failures=0;
 for(int test=0;test<1024;++test){
  auto run=[&](bool candidate){
   for(auto& g:globals)std::memcpy(reinterpret_cast<void*>(g.address),g.saved.data(),g.saved.size());
   scenario=test;trace=json::array();hook->State(candidate);
   alignas(CCam) std::array<uint8,sizeof(CCam)> camBytes{};auto* c=reinterpret_cast<CCam*>(camBytes.data());
   alignas(CPed) std::array<uint8,sizeof(CPed)> pedBytes{};auto* ped=reinterpret_cast<CPed*>(pedBytes.data());
   alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrixBytes{};auto* matrix=reinterpret_cast<CMatrix*>(matrixBytes.data());
   alignas(CEntity) std::array<uint8,sizeof(CEntity)> attachedBytes{};auto* attached=reinterpret_cast<CEntity*>(attachedBytes.data());
   RpClump clump{};RwFrame frame{};RwCamera camera{};clump.object.type=2;clump.object.parent=&frame;
   At<uintptr_t>(pedBytes.data(),0x14)=uintptr_t(matrix);At<uintptr_t>(pedBytes.data(),0x18)=test%16==0?0:uintptr_t(&clump);At<uint8>(pedBytes.data(),0x36)=test%16==1?2:3;
   matrix->GetRight()=CVector{1,0,0};matrix->GetForward()=CVector{.125f,1,0};matrix->GetUp()=CVector{0,0,1};matrix->GetPosition()=CVector{float(test%7),float(test%11),float(test%5)};
   ped->m_fCurrentRotation=float(int32(test%9)-4)*.375f;ped->m_pAttachedTo=test%4==2?reinterpret_cast<CPhysical*>(attached):nullptr;ped->m_fTurretAngleA=uint16((test/4)%5);ped->m_fTurretAngleB=float(test%7)*.125f;
   c->m_pCamTargetEntity=ped;c->m_bResetStatics=test%2;c->m_nMode=test%3==0?MODE_SNIPER_RUNABOUT:MODE_1STPERSON;c->m_fFOV=15.f+float(test%56);c->m_fVerticalAngle=float(int32(test%13)-6)*.25f;c->m_fHorizontalAngle=float(int32(test%17)-8)*.5f;c->m_vecBufferedPlayerBodyOffset=CVector{.5f,.25f,.125f};
   TheCamera.m_pRwCamera=&camera;Scene.m_pRwCamera=&camera;TheCamera.m_bHeadBob=((test/2)%2)!=0;StaticRef<CPed*>(0xB6F980)=ped;
   StaticRef<CVector>(0xB6FFC4)=CVector{.0625f,.125f,.25f};StaticRef<uint32>(0xB6FFE0)=0x12340000;StaticRef<bool>(0xB6FFDC)=true;StaticRef<float>(0xB6FC48)=float(test%5)*.25f;
   StaticRef<float>(0xB73424)=test%4?float(int32(test%5)-2)*.5f:0.f;StaticRef<float>(0xB73428)=test%4?float(int32((test/5)%5)-2)*.5f:0.f;StaticRef<float>(0xB6EC18)=.003f;StaticRef<float>(0xB6EC1C)=.004f;StaticRef<float>(0xB7CB5C)=float(1+test%5)*.25f;
   StaticRef<CVector>(0x8CCC54)=CVector{.1f,.2f,.3f};std::memset(&bones,0,sizeof(bones));bones.right={1,0,0};bones.up={0,1,0};bones.at={0,0,1};hierarchy={};hierarchy.pMatrixArray=&bones;
   const bool switched=ReversingProbe::JumpsToDll(0x50EB70)==candidate;const CVector target{1,2,3};reinterpret_cast<void(__thiscall*)(CCam*,const CVector&,float,float,float)>(0x50EB70)(c,target,.375f,.5f,.75f);
   json globalState=json::array();for(auto& g:globals)globalState.push_back(Bytes(reinterpret_cast<void*>(g.address),g.saved.size()));
   return json{{"cam",Bytes(camBytes.data(),camBytes.size())},{"ped",Bytes(pedBytes.data(),pedBytes.size())},{"matrix",Bytes(matrixBytes.data(),matrixBytes.size())},{"frame",Bytes(&frame,sizeof(frame))},{"bones",Bytes(&bones,sizeof(bones))},{"camera",Bytes(&camera,sizeof(camera))},{"globals",globalState},{"trace",trace},{"switches",switched}};
  };
  auto original=run(false),candidate=run(true);bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
  Write({{"event","comparison"},{"pair",pairs++},{"function","CCam::Process_1rstPersonPedOnPC"},{"case",test},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
 }
 hook->State(enabled);hook->LockState(locked);for(auto& g:globals)std::memcpy(reinterpret_cast<void*>(g.address),g.saved.data(),g.saved.size());
 Write({{"event","function_summary"},{"function","CCam::Process_1rstPersonPedOnPC"},{"failures",failures},{"cases",1024}});Write({{"event",passed?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
