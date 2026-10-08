#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Bike.h"
#include "Collision/ColModel.h"
#include "Collision/CollisionData.h"
#include "Collision/ColLine.h"
#include "Coronas.h"
#include "PointLights.h"
#include "Shadows.h"
#include "Models/VehicleModelInfo.h"
#include "tHandlingData.h"
#include "../ProbeRuntime.hpp"
namespace BikePreRender355Live {
template<class T> T& At(void* p,size_t n){return *reinterpret_cast<T*>(static_cast<uint8*>(p)+n);}
inline uint32 F(float v){return std::bit_cast<uint32>(v);}
inline json Vector(const CVector& v){return json::array({F(v.x),F(v.y),F(v.z)});}
inline json Bytes(const void* p,size_t n){json a=json::array();auto*b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i)a.push_back(b[i]);return a;}
inline uint32 scenario; inline json trace; inline CColModel* col;
inline void __fastcall Base(CVehicle*,void*){trace.push_back({"base"});}
inline CColModel* __fastcall Col(const CEntity*,void*){trace.push_back({"col"});return col;}
inline void __fastcall Suspension(CBike*,void*){trace.push_back({"suspension"});}
inline void __fastcall Lean(CBike* bike,void*){trace.push_back({"lean"});bike->m_bLeanMatrixCalculated=true;std::memcpy(&bike->m_mLeanMatrix,bike->m_matrix,64);At<uintptr_t>(&bike->m_mLeanMatrix,64)=0;At<bool>(&bike->m_mLeanMatrix,68)=false;}
inline bool __fastcall Particles(CVehicle*,void*,tWheelState state,uint32 status,float ratio,float speed,CColPoint* contact,CVector* position,float sign,int32 wheel,uint32 surface,bool* blood,uint32 flags){trace.push_back({"particles",int(state),status,F(ratio),F(speed),Vector(contact->m_vecPoint),Vector(*position),F(sign),wheel,surface,*blood,flags});*blood=(scenario&4)!=0;return true;}
inline void __fastcall Exhaust(CVehicle*,void*){trace.push_back({"exhaust"});}
inline void __fastcall Damage(CVehicle*,void*){trace.push_back({"damage"});}
inline void __fastcall Lights(CVehicle*,void*,CMatrix& m,eVehicleLightsFlags flags){trace.push_back({"lights",Bytes(&m,64),uint32(flags)});}
inline void __cdecl Shadow(CVehicle*,VEH_SHD_TYPE type){trace.push_back({"shadow",int(type)});}
inline float __fastcall Wheel(CVehicle*,void*,tWheelState state,const CVector& direction,const CVector& point,float radius){trace.push_back({"wheel",int(state),Vector(direction),Vector(point),F(radius)});return float(int(scenario%7)-3)*.0625f;}
inline void __cdecl Corona(uint32 id,CEntity* vehicle,uint8 red,uint8 green,uint8 blue,uint8 intensity,const CVector& point,float size,float range,eCoronaType texture,eCoronaFlareType flare,eCoronaReflType reflection,eCoronaLOSCheck los,eCoronaTrail trail,float angle,bool neon,float pull,bool bright,float fade,bool below,bool core){trace.push_back({"corona",id-uint32(uintptr_t(vehicle)),red,green,blue,intensity,Vector(point),F(size),F(range),int(texture),int(flare),int(reflection),int(los),int(trail),F(angle),neon,F(pull),bright,F(fade),below,core});}
inline void __cdecl Light(uint8 type,CVector point,CVector direction,float radius,float red,float green,float blue,uint8 fog,bool shadows,CEntity* entity){trace.push_back({"light",type,Vector(point),Vector(direction),F(radius),F(red),F(green),F(blue),fog,shadows,entity==nullptr});}
inline uintptr_t Body(const char* cls,const char* name){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& x:c.Items())if(x->Name()==name)h=x.get();});return h?uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress()):0;}
inline auto* Hook(){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CBike")for(auto& x:c.Items())if(x->Name()=="PreRender")h=x.get();});return h;}
inline bool done;inline std::ofstream log;
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{{"CBike","PreRender",0,0x871360,17,1024}};
inline void Update(){
 if(done)return;char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);auto dir=std::filesystem::path(executable).parent_path();
 if(!std::filesystem::exists(dir/"bike-pre-render355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"bike-pre-render355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("bike-pre-render355");}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch b0(0x6D6480,Base),b1(Body("CVehicle","PreRender"),Base),c0(0x535300,Col),c1(bit_cast<uintptr_t>(&CEntity::GetColModel),Col),l0(0x6B7150,Lean),l1(bit_cast<uintptr_t>(&CBike::CalculateLeanMatrix),Lean);
 EntryPatch p0(0x6DE880,Particles),p1(bit_cast<uintptr_t>(&CVehicle::AddSingleWheelParticles),Particles),e0(0x6DE240,Exhaust),e1(bit_cast<uintptr_t>(&CVehicle::AddExhaustParticles),Exhaust),d0(0x6D2A80,Damage),d1(bit_cast<uintptr_t>(&CVehicle::AddDamagedVehicleParticles),Damage);
 EntryPatch light0(0x6E1A60,Lights),light1(bit_cast<uintptr_t>(&CVehicle::DoVehicleLights),Lights),sh0(0x70BDA0,Shadow),sh1(bit_cast<uintptr_t>(&CShadows::StoreShadowForVehicle),Shadow),w0(0x6D1230,Wheel),w1(bit_cast<uintptr_t>(&CVehicle::ProcessWheelRotation),Wheel);
 using CoronaFn=void(*)(uint32,CEntity*,uint8,uint8,uint8,uint8,const CVector&,float,float,eCoronaType,eCoronaFlareType,eCoronaReflType,eCoronaLOSCheck,eCoronaTrail,float,bool,float,bool,float,bool,bool);
 EntryPatch cor0(0x6FC580,Corona),cor1(bit_cast<uintptr_t>(static_cast<CoronaFn>(&CCoronas::RegisterCorona)),Corona),point0(0x7000E0,Light),point1(bit_cast<uintptr_t>(&CPointLights::AddLight),Light);
 struct Region{uintptr_t address;size_t size;std::vector<uint8> saved;};std::array<Region,4> globals{{{0xB7CB58,8},{0xB7CB84,4},{0xB7C4E4,4},{0xC1C81C,4}}};for(auto&r:globals){r.saved.resize(r.size);std::memcpy(r.saved.data(),reinterpret_cast<void*>(r.address),r.size);}
 auto* hook=Hook();if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}const bool old=hook->Hooked(),locked=hook->Locked();hook->LockState(false);int failures=0;json coverage=json::object();
 for(int test=0;test<1024;++test){auto run=[&](bool candidate){
  scenario=uint32(test);trace=json::array();for(auto&r:globals)std::memcpy(reinterpret_cast<void*>(r.address),r.saved.data(),r.size);
  alignas(CBike) std::array<uint8,sizeof(CBike)> bikeBytes{};auto* bike=reinterpret_cast<CBike*>(bikeBytes.data());CMatrixLink matrix{};matrix.SetUnity();matrix.GetPosition()=CVector{float(test%5),float(test%7),2.f};bike->m_matrix=&matrix;
  std::array<uintptr_t,80> vt{};vt[52]=uintptr_t(&Suspension);At<uintptr_t>(bike,0)=uintptr_t(vt.data());
  const uint8 status=uint8((test/64)%8);At<uint8>(bike,0x36)=uint8(status<<3);At<uint8>(bike,0x428)=test&2?0x10:0;At<uint8>(bike,0x42B)=uint8((test&1?1:0)|(test&128?0x40:0));At<uint8>(bike,0x42D)=test&4?0x80:0;At<uint32>(bike,0x40)=test&256?0x20000000:0;bike->m_nModelIndex=test%3?522:523;
  alignas(float) std::array<uint8,sizeof(tHandlingData)> handling{};bike->m_pHandlingData=reinterpret_cast<tHandlingData*>(handling.data());At<uint32>(handling.data(),0xCC)=test&32?0x1000:0;
  alignas(CColModel) std::array<uint8,sizeof(CColModel)> colBytes{};alignas(CCollisionData) std::array<uint8,sizeof(CCollisionData)> dataBytes{};auto& modelCol=*reinterpret_cast<CColModel*>(colBytes.data());auto& data=*reinterpret_cast<CCollisionData*>(dataBytes.data());std::array<CColLine,4> lines{};modelCol.m_pColData=&data;data.m_pLines=lines.data();modelCol.m_boundBox.m_vecMin=CVector{-1.f,-2.f,-.75f};modelCol.m_boundBox.m_vecMax=CVector{1.f,2.f,1.f};col=&modelCol;
  alignas(CVehicleModelInfo) std::array<uint8,sizeof(CVehicleModelInfo)> modelBytes{};auto* model=reinterpret_cast<CVehicleModelInfo*>(modelBytes.data());model->m_fWheelSizeFront=.75f;model->m_fWheelSizeRear=1.f;model->m_fBikeSteerAngle=15.f+float(test%4)*5.f;auto*& modelSlot=StaticRef<CBaseModelInfo*>(0xA9B0C8+uint32(bike->m_nModelIndex)*4);auto* previousModel=modelSlot;modelSlot=model;
  std::array<RwFrame,BIKE_NUM_NODES> frames{};for(size_t i=0;i<frames.size();++i){frames[i].modelling.right.x=1.f;frames[i].modelling.up.y=1.f;frames[i].modelling.at.z=1.f;frames[i].modelling.pos=RwV3d{float(i)*.125f,float(i)*.25f,float(i)*.0625f};frames[i].modelling.flags=0x10000;bike->m_aBikeNodes[i]=&frames[i];}if(test&16)bike->m_aBikeNodes[BIKE_FORKS_FRONT]=nullptr;if(test&32)bike->m_aBikeNodes[BIKE_FORKS_REAR]=nullptr;if(test&64)bike->m_aBikeNodes[BIKE_HANDLEBARS]=nullptr;if(test&128)bike->m_aBikeNodes[BIKE_CHASSIS]=nullptr;
  bike->m_vecMoveSpeed=CVector{.125f,float(test%5)*.125f,0.f};bike->m_fSteerAngle=float(int(test%7)-3)*.125f;bike->m_RideAnimData.LeanAngle=float(int(test%9)-4)*.125f;bike->m_RideAnimData.BarSteerAngle=float(int(test%5)-2)*.125f;
  for(int i=0;i<4;++i){lines[i].m_vecStart=CVector{float(i)*.125f,float(i)-1.5f,1.f};lines[i].m_vecEnd=CVector{float(i)*.125f,float(i)-1.5f,-.5f};bike->m_fSuspensionLength[i]=.5f;bike->m_fLineLength[i]=1.f;bike->m_aWheelRatios[i]=float((test>>i)%5)*.25f;bike->m_aRatioHistory[i]=test&(1<<(i+1))?.25f:0.f;bike->m_WheelCounts[i]=test&(1<<(i+4))?.5f:0.f;bike->m_aWheelColPoints[i].m_vecPoint=CVector{float(i),float(i)*.5f,0.f};}
  for(int i=0;i<2;++i){bike->m_WheelStates[i]=static_cast<tWheelState>((test+i)%3);bike->m_nWheelStatus[i]=uint8((test+i)%3);bike->m_bWheelBloody[i]=test&(1<<(i+2));bike->m_bMoreSkidMarks[i]=test&(1<<(i+3));bike->m_aWheelSkidmarkType[i]=static_cast<eSkidmarkType>((test+i)%3);bike->m_aWheelSuspensionHeights[i]=.5f;bike->m_aWheelOrigHeights[i]=.5f;bike->m_aWheelPitchAngles[i]=float(test%7)*.125f;}
  bike->m_fSwingArmLength=1.f;bike->m_fForkYOffset=.125f;bike->m_fForkZOffset=.25f;bike->m_fSteerAngleTan=.25f;StaticRef<float>(0xC1C81C)=.01f;StaticRef<float>(0xB7C4E4)=.5f;CTimer::ms_fTimeStep=float(test%4)*.5f;CTimer::m_snTimeInMilliseconds=uint32(test*17);
  hook->State(candidate);const uintptr_t entry=StaticRef<uintptr_t>(0x871360+17*4);const bool switched=(entry!=0x6BD090)==candidate;reinterpret_cast<void(__thiscall*)(CBike*)>(entry)(bike);
  if(candidate)for(auto&e:trace){const auto n=e[0].get<std::string>();coverage[n]=coverage.value(n,0)+1;}
  const auto identity=[&](const void* value)->json{if(!value)return nullptr;const auto pointer=uintptr_t(value);if(value==bike)return "bike";if(value==&matrix)return "matrix";if(value==handling.data())return "handling";for(size_t i=0;i<frames.size();++i)if(value==&frames[i])return json::array({"frame",i});return pointer;};
  json relationships{{"matrix",identity(bike->m_matrix)},{"handling",identity(bike->m_pHandlingData)},{"nodes",json::array()}};for(auto* node:bike->m_aBikeNodes)relationships["nodes"].push_back(identity(node));
  const uintptr_t actualVptr=At<uintptr_t>(bike,0);const uintptr_t dllVptr=uintptr_t(ReversibleHooks::detail::GetVTableAddress("CBike"));relationships["vtable"]=actualVptr==uintptr_t(vt.data())?json("fixture_vtable"):(actualVptr==0x871360||actualVptr==dllVptr)?json("CBike"):json(actualVptr);
  At<uintptr_t>(bike,0)=0;At<uintptr_t>(bike,0x14)=0;At<uintptr_t>(bike,0x384)=0;for(auto*& frame:bike->m_aBikeNodes)frame=nullptr;
  json result{{"relationships",relationships},{"bike",Bytes(bike,bikeBytes.size())},{"frames",Bytes(frames.data(),sizeof(frames))},{"matrix",Bytes(&matrix,64)},{"trace",trace},{"switches",switched}};json gs=json::array();for(auto&r:globals)gs.push_back(Bytes(reinterpret_cast<void*>(r.address),r.size));result["globals"]=gs;modelSlot=previousModel;return result;
 };auto a=run(false),b=run(true);const bool equal=a==b&&a["switches"]==true;failures+=!equal;Write({{"event","comparison"},{"function","CBike::PreRender"},{"case",test},{"pair",test},{"equal",equal},{"accepted",equal},{"original",a},{"candidate",b}});}
 hook->State(old);hook->LockState(locked);for(auto&r:globals)std::memcpy(reinterpret_cast<void*>(r.address),r.saved.data(),r.size);bool pass=failures==0;for(auto*n:{"base","suspension","particles","exhaust","damage","corona","light","lights","shadow","wheel"})if(!coverage.value(n,0)){pass=false;Write({{"event","error"},{"reason","missing coverage"},{"branch",n}});}Write({{"event","function_summary"},{"function","CBike::PreRender"},{"cases",1024},{"failures",failures},{"coverage",coverage}});Write({{"event",pass?"complete":"error"},{"pairs",1024},{"state_restored",true}});
}
}
