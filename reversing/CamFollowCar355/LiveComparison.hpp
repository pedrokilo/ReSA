#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cam.h"
#include "Camera.h"
#include "Pad.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Ped/Ped.h"
#include "PedIntelligence.h"
#include "Tasks/TaskManager.h"
#include "Collision/ColModel.h"
#include "../ProbeRuntime.hpp"
namespace CamFollowCar355Live {
template<class T> T& At(void* p,size_t n){return *reinterpret_cast<T*>(static_cast<uint8*>(p)+n);}
inline json Bytes(const void* p,size_t n){json v=json::array();auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i)v.push_back(b[i]);return v;}
inline uint32 F(float v){return std::bit_cast<uint32>(v);}
inline json trace;inline uint32 scenario;inline CVehicle* fixtureVehicle;inline CColModel* fixtureCol;inline CTask* fixtureTask;
inline CPad* __cdecl GetPad(int32 n){return &CPad::Pads[n];}
inline void __fastcall Tweaks(CCamera*,void*,CVehicle*){trace.push_back({"tweaks"});StaticRef<float>(0xB6FD70)=.5f+float(scenario%3)*.125f;StaticRef<float>(0xB6FD74)=1.f;StaticRef<float>(0xB6FD78)=float(scenario%5)*.03125f;}
inline eVehicleAppearance __fastcall Appearance(const CVehicle*,void*){trace.push_back({"appearance"});return static_cast<eVehicleAppearance>(1+scenario%5);}
inline bool __fastcall ArrayPosition(CCamera*,void*,eVehicleType t,int32& out){trace.push_back({"array",int(t)});out=scenario%5;return true;}
inline CColModel* __fastcall Col(const CEntity*,void*){trace.push_back({"col"});return fixtureCol;}
inline float __fastcall Height(CVehicle*,void*){trace.push_back({"height"});return .5f+float(scenario%5)*.125f;}
inline float __fastcall Heading(const CPlaceable*,void*){trace.push_back({"heading"});return float(int32(scenario%7)-3)*.375f;}
inline int16 __fastcall AimX(const CPad* pad,void*,CPed*){trace.push_back({"aimX",pad==&CPad::Pads[1]});return int16(int32((scenario/4)%9)*32-128);}
inline int16 __fastcall AimY(const CPad* pad,void*,CPed*){trace.push_back({"aimY",pad==&CPad::Pads[1]});return int16(int32((scenario/36)%9)*32-128);}
inline int16 __fastcall Steering(const CPad*,void*){trace.push_back({"steering"});return int16(scenario%3==0?0:scenario%3==1?-128:128);}
inline CTask* __fastcall Active(CTaskManager* manager,void*){trace.push_back({"active",At<uint32>(manager,0x10)});return scenario%7==0?nullptr:fixtureTask;}
inline eTaskType __fastcall Type(CTask*,void*){trace.push_back({"taskType"});return static_cast<eTaskType>(scenario%3==0?704:scenario%3==1?1309:100);}
inline void __fastcall Clip(CCam* c,void*){trace.push_back({"clip",F(c->m_fHorizontalAngle)});while(c->m_fHorizontalAngle>PI)c->m_fHorizontalAngle-=TWO_PI;while(c->m_fHorizontalAngle< -PI)c->m_fHorizontalAngle+=TWO_PI;}
inline void __fastcall Vectors(CCam* c,void*){trace.push_back({"vectors",F(c->m_vecFront.x),F(c->m_vecFront.y),F(c->m_vecFront.z)});c->m_vecUp=CVector{0,0,1};}
inline void __cdecl ColVars(eVehicleType state,int32 zoom){trace.push_back({"colVars",int(state),zoom});}
inline void __fastcall Special(CCamera*,void*,CVehicle*){trace.push_back({"special"});}
inline bool __fastcall React(CCamera*,void*,CVector* source,CVector* target){trace.push_back({"react",Bytes(source,sizeof(*source)),Bytes(target,sizeof(*target))});source->x+=.125f;return true;}
inline void __fastcall Improve(CCamera*,void*,CVehicle*,CPed*,CVector* source,CVector* target){trace.push_back({"improve",Bytes(source,sizeof(*source)),Bytes(target,sizeof(*target))});source->z+=.0625f;}
inline bool done;inline std::ofstream log;
inline void Write(const json& value){ReversingProbe::PumpMessages();log<<value.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,1> fns{{{"CCam","Process_FollowCar_SA",0x5245B0,0,-1,1024}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(){ReversibleHooks::ReversibleHook::Base* found{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CCam")for(auto& h:c.Items())if(h->Name()=="Process_FollowCar_SA")found=h.get();});return found;}
inline void Update(){
 if(done)return;char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);auto directory=std::filesystem::path(executable).parent_path();
 if(!std::filesystem::exists(directory/"cam-follow-car355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(directory/"cam-follow-car355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-follow-car355");}
 if(!FindPlayerPed() || CCutsceneMgr::IsRunning())return;done=true;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch pad(0x53FB70,GetPad),tw0(0x50A480,Tweaks),tw1(bit_cast<uintptr_t>(&CCamera::ApplyVehicleCameraTweaks),Tweaks);
 EntryPatch ap0(0x6D1080,Appearance),ap1(bit_cast<uintptr_t>(&CVehicle::GetVehicleAppearance),Appearance),ar0(0x50AF00,ArrayPosition),ar1(bit_cast<uintptr_t>(&CCamera::GetArrPosForVehicleType),ArrayPosition);
 EntryPatch col0(0x535300,Col),col1(bit_cast<uintptr_t>(&CEntity::GetColModel),Col),h0(0x441DB0,Heading),h1(bit_cast<uintptr_t>(&CPlaceable::GetHeading),Heading);
 EntryPatch x0(0x541040,AimX),x1(bit_cast<uintptr_t>(&CPad::AimWeaponLeftRight),AimX),y0(0x5410C0,AimY),y1(bit_cast<uintptr_t>(&CPad::AimWeaponUpDown),AimY);
 EntryPatch steer0(0x53FBD0,Steering),steer1(bit_cast<uintptr_t>(&CPad::GetSteeringUpDown),Steering),act0(0x681720,Active),act1(bit_cast<uintptr_t>(&CTaskManager::GetActiveTask),Active);
 EntryPatch clip0(0x509C50,Clip),clip1(bit_cast<uintptr_t>(&CCam::ClipBeta),Clip),vec0(0x509CE0,Vectors),vec1(bit_cast<uintptr_t>(&CCam::GetVectorsReadyForRW),Vectors);
 EntryPatch cv0(0x50CCA0,ColVars),cv1(bit_cast<uintptr_t>(&CCamera::SetColVarsVehicle),ColVars),sp0(0x50CDE0,Special),sp1(bit_cast<uintptr_t>(&CCamera::CameraVehicleModeSpecialCases),Special);
 EntryPatch re0(0x520190,React),re1(bit_cast<uintptr_t>(&CCamera::CameraColDetAndReact),React),im0(0x516B20,Improve),im1(bit_cast<uintptr_t>(&CCamera::ImproveNearClip),Improve);
 struct Region{uintptr_t address;size_t count;std::vector<uint8> saved;};
 std::array<Region,12> regions{{{0xB6F028,sizeof(CCamera)},{0xB6F018,12},{0xB70114,12},{0xB73420,12},{0xB6EC10,128},{0xB7CB5C,4},{0xB7CB84,4},{0x8CCEA4,80},{0x8CC384,8},{0xB7CD68,4},{0x9655E5,1},{0xC1CC02,2}}};
 for(auto& r:regions){r.saved.resize(r.count);std::memcpy(r.saved.data(),reinterpret_cast<void*>(r.address),r.count);}
 std::array<uint8,sizeof(CPad)*2> padSave{};std::memcpy(padSave.data(),CPad::Pads,padSave.size());
 auto* hook=FindHook();if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);int failures=0;
 for(int test=0;test<1024;++test){auto run=[&](bool candidate){
 hook->State(candidate);scenario=test;trace=json::array();for(auto& r:regions)std::memcpy(reinterpret_cast<void*>(r.address),r.saved.data(),r.count);std::memset(CPad::Pads,0,sizeof(CPad)*2);
 alignas(CCam) std::array<uint8,sizeof(CCam)> camBytes{};auto* c=reinterpret_cast<CCam*>(camBytes.data());
 alignas(CAutomobile) std::array<uint8,sizeof(CAutomobile)> vehicleBytes{},trailerBytes{};auto* vehicle=reinterpret_cast<CVehicle*>(vehicleBytes.data());fixtureVehicle=vehicle;auto* trailer=reinterpret_cast<CVehicle*>(trailerBytes.data());
 std::array<uintptr_t,64> vehicleVT{};vehicleVT[53]=bit_cast<uintptr_t>(&Height);At<uintptr_t>(vehicle,0)=uintptr_t(vehicleVT.data());
 CMatrixLink matrix{},trailerMatrix{};std::memset(&matrix,0,sizeof(matrix));std::memset(&trailerMatrix,0,sizeof(trailerMatrix));At<CMatrixLink*>(vehicle,0x14)=&matrix;At<CMatrixLink*>(trailer,0x14)=&trailerMatrix;
 matrix.GetRight()=CVector{1,0,float(int32(test%5)-2)*.125f};matrix.GetForward()=CVector{.125f,1,float(int32(test%7)-3)*.0625f};matrix.GetUp()=CVector{0,0,1};matrix.GetPosition()=CVector{float(test%5),float(test%7),1};trailerMatrix.GetPosition()=CVector{3,5,2};
 At<uint8>(vehicle,0x36)=test%16==0?3:uint8(2|((test&32)?0x40:0));constexpr int16 models[]{400,522,487,520,539,441,465,443,564,531,592,406};At<int16>(vehicle,0x22)=models[(test/2)%12];At<int32>(vehicle,0x590)=test%5==0?9:0;constexpr int32 subclasses[]{0,2,3,4,5,9};At<int32>(vehicle,0x594)=subclasses[(test/3)%6];At<uint16>(vehicle,0x86C)=uint16((test&64)?60000:0);At<uint8>(vehicle,0x960)=uint8(test%4);At<uint8>(vehicle,0x804)=uint8(test%5);At<uint32>(vehicle,0x38C)=(test&128)?0x20000:0;At<uint8>(vehicle,0x429)=(test&64)?4:0;
 vehicle->m_vecMoveSpeed=CVector{float(test%5)*.125f,float((test/5)%5)*.125f,.03125f};vehicle->m_vecTurnSpeed=CVector{.0125f,.025f,.0375f};vehicle->m_pVehicleBeingTowed=(test&256)?trailer:nullptr;
 alignas(CColModel) std::array<uint8,sizeof(CColModel)> colBytes{};fixtureCol=reinterpret_cast<CColModel*>(colBytes.data());fixtureCol->m_boundBox.m_vecMin=CVector{-1,-2,-.5f};fixtureCol->m_boundBox.m_vecMax=CVector{1.25f,2.5f,1.5f+float(test%4)*.25f};
 alignas(CPed) std::array<std::array<uint8,sizeof(CPed)>,2> pedBytes{};alignas(CPedIntelligence) std::array<std::array<uint8,sizeof(CPedIntelligence)>,2> intelligenceBytes{};
 for(size_t n=0;n<2;++n){auto* p=reinterpret_cast<CPed*>(pedBytes[n].data());At<CPedIntelligence*>(p,0x47C)=reinterpret_cast<CPedIntelligence*>(intelligenceBytes[n].data());p->m_nPedType=(test&8)?PED_TYPE_PLAYER2:PED_TYPE_PLAYER1;At<uint32>(&p->GetIntelligence()->m_TaskMgr,0x10)=uint32(n);}
 vehicle->m_pDriver=(test&4)?reinterpret_cast<CPed*>(pedBytes[0].data()):nullptr;vehicle->m_apPassengers[0]=(test&16)?reinterpret_cast<CPed*>(pedBytes[1].data()):nullptr;
 alignas(CTask) std::array<uint8,64> taskBytes{};fixtureTask=reinterpret_cast<CTask*>(taskBytes.data());std::array<uintptr_t,9> taskVT{};taskVT[4]=bit_cast<uintptr_t>(&Type);At<uintptr_t>(fixtureTask,0)=uintptr_t(taskVT.data());At<uint8>(fixtureTask,0x2D)=(test&32)?1:0;
 c->m_pCamTargetEntity=vehicle;c->m_bResetStatics=(test&1)!=0;c->m_fFOV=70.f+float(test%35);c->m_fHorizontalAngle=float(int32(test%13)-6)*.375f;c->m_fVerticalAngle=float(int32(test%9)-4)*.125f;c->m_fAlphaSpeed=.0125f;c->m_fBetaSpeed=-.025f;c->m_vecSource=CVector{2,4,6};c->m_vecFront=CVector{0,1,0};c->m_avecTargetHistoryPos[0]=CVector{-3,-4,1};c->m_avecTargetHistoryPos[1]=CVector{-1,-2,1};
 StaticRef<float>(0xB7CB5C)=float(1+test%5)*.25f;StaticRef<uint32>(0xB7CB84)=1000+test;StaticRef<int32>(0xB6F0DC)=1+test%3;StaticRef<float>(0xB6F0E8)=2.f+float(test%4)*.25f;StaticRef<bool>(0xB6F042)=(test&64)!=0;StaticRef<bool>(0xB6F043)=(test&128)!=0;StaticRef<bool>(0xB6F050)=(test&512)!=0;StaticRef<float>(0xB7011C)=float(test%5)*.25f;StaticRef<float>(0xB70118)=(test&32)?10.f:0.f;StaticRef<bool>(0xB70114)=(test&64)!=0;StaticRef<bool>(0xB6EC2E)=(test&128)!=0;StaticRef<float>(0xB6EC1C)=.004f;StaticRef<float>(0xB73424)=float(int32(test%5)-2)*.5f;StaticRef<float>(0xB73428)=float(int32((test/5)%5)-2)*.5f;StaticRef<int32>(0x8CC384)=test%3==0?3:0;StaticRef<int32>(0x8CC388)=test%4==0?18:0;StaticRef<bool>(0xC1CC02)=(test&256)!=0;StaticRef<bool>(0xC1CC03)=(test&512)!=0;At<uint16>(&CPad::Pads[0],0x10E)=(test&256)?1:0;At<uint16>(&CPad::Pads[1],0x10E)=(test&256)?1:0;
 const bool switched=ReversingProbe::JumpsToDll(0x5245B0)==candidate;const CVector target{2.5f,3.75f,2.f};reinterpret_cast<void(__thiscall*)(CCam*,const CVector&,float,float,float,bool)>(0x5245B0)(c,target,0.f,0.f,0.f,(test/2)%3==0);
 json globals=json::array();for(auto& r:regions)globals.push_back(Bytes(reinterpret_cast<void*>(r.address),r.count));return json{{"cam",Bytes(camBytes.data(),camBytes.size())},{"vehicle",Bytes(vehicleBytes.data(),vehicleBytes.size())},{"trailer",Bytes(trailerBytes.data(),trailerBytes.size())},{"matrix",Bytes(&matrix,sizeof(matrix))},{"trailerMatrix",Bytes(&trailerMatrix,sizeof(trailerMatrix))},{"peds",Bytes(pedBytes.data(),sizeof(pedBytes))},{"intelligence",Bytes(intelligenceBytes.data(),sizeof(intelligenceBytes))},{"task",Bytes(taskBytes.data(),taskBytes.size())},{"col",Bytes(colBytes.data(),colBytes.size())},{"pads",Bytes(CPad::Pads,sizeof(CPad)*2)},{"globals",globals},{"trace",trace},{"switches",switched}};
 };auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original["switches"]==true;failures+=!equal;Write({{"event","comparison"},{"pair",test},{"function","CCam::Process_FollowCar_SA"},{"case",test},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});}
 hook->State(enabled);hook->LockState(locked);for(auto& r:regions)std::memcpy(reinterpret_cast<void*>(r.address),r.saved.data(),r.count);std::memcpy(CPad::Pads,padSave.data(),padSave.size());
 Write({{"event","function_summary"},{"function","CCam::Process_FollowCar_SA"},{"cases",1024},{"failures",failures}});Write({{"event",failures?"error":"complete"},{"pairs",1024},{"state_restored",true}});
}
}

