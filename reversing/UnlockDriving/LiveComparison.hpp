#pragma once
#include <fstream>
#include <crtdbg.h>
#include <rtcapi.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "VehicleAnimGroupData.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace UDLive {
inline bool done{};inline std::ofstream log;inline int currentCase=-1;
inline void Write(const json& r){log<<r.dump()<<'\n';}
inline int __cdecl Error(int,const wchar_t*,int,const wchar_t*,const wchar_t*,...){Write({{"event","error"},{"reason","runtime check"},{"case",currentCase}});log.flush();ExitProcess(92);}
inline void __cdecl Assertion(const wchar_t* expression,const wchar_t* file,unsigned line){char expr[1024]{},path[1024]{};WideCharToMultiByte(CP_UTF8,0,expression,-1,expr,1024,nullptr,nullptr);WideCharToMultiByte(CP_UTF8,0,file,-1,path,1024,nullptr,nullptr);Write({{"event","assertion"},{"expression",expr},{"file",path},{"line",line},{"case",currentCase}});log.flush();ExitProcess(93);}
template<class T>struct Raw{alignas(T) std::array<uint8,sizeof(T)> b{};T* get(){return reinterpret_cast<T*>(b.data());}};
inline std::array<Raw<CAnimBlendAssociation>,512> assocs;
inline std::array<bool,512> present{},touched{};inline json trace;inline float skill;inline int kind,appearance;inline RpClump* expectedClump;
inline CAnimBlendAssociation* Query(RpClump* clump,uint32 id){assert(clump==expectedClump&&id<512);trace.push_back(json::array({"query",id}));touched[id]=true;return present[id]?assocs[id].get():nullptr;}
inline CAnimBlendAssociation* Blend(RpClump* clump,AssocGroupId group,AnimationId id,float value){assert(clump==expectedClump&&id>=0&&id<512);trace.push_back(json::array({"blend",int(group),int(id),std::bit_cast<uint32>(value)}));touched[id]=true;if(id==ANIM_ID_CAR_TUNE_RADIO&&currentCase%13)return nullptr;present[id]=true;return assocs[id].get();}
inline float Stat(eStats s){trace.push_back(json::array({"stat",int(s)}));return skill;}
inline bool Kart(AssocGroupId){trace.push_back("kart");return kind==2;}
inline bool Truck(AssocGroupId){trace.push_back("truck");return kind==3;}
inline eVehicleAppearance __fastcall Appearance(const CVehicle*,void*){trace.push_back("appearance");return static_cast<eVehicleAppearance>(appearance);}
inline std::array<uint32,3> AppearanceState(CVehicle* vehicle){
 uint32 result{},thisAfter{},dataAfter{};
 __asm {
  mov ecx, vehicle
  mov edx, 13572468h
  mov eax, 6D1080h
  call eax
  mov result, eax
  mov thisAfter, ecx
  mov dataAfter, edx
 }
 return {result,uint32(thisAfter==reinterpret_cast<uintptr_t>(vehicle)),dataAfter};
}
inline void Update(){
 if(done)return;_set_error_mode(_OUT_TO_STDERR);_set_abort_behavior(0,_WRITE_ABORT_MSG|_CALL_REPORTFAULT);
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"unlock-driving-runtime.start")){done=true;return;}
 if(!log.is_open()){_RTC_SetErrorFuncW(Error);_CrtSetReportMode(_CRT_ASSERT,_CRTDBG_MODE_FILE);_CrtSetReportMode(_CRT_ERROR,_CRTDBG_MODE_FILE);log.open(dir/"unlock-driving-runtime.jsonl");Write({{"event","waiting_for_player"}});log.flush();}
 auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;
 ReversingProbe::EntryPatch ap(reinterpret_cast<uintptr_t>(&_wassert),Assertion);
 using Hook=ReversibleHooks::ReversibleHook::Base;Hook* hook{};Hook* helper{};
 ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CVehicle")for(auto& h:c.Items()){if(h->Name()=="ProcessDrivingAnims")hook=h.get();if(h->Name()=="GetVehicleAppearance")helper=h.get();}});
 if(!hook||!helper){Write({{"event","error"},{"reason","missing hook"}});log.flush();return;}
 const auto playerBefore=ReversingProbe::Bytes(player,sizeof(CPlayerPed)),cameraBefore=ReversingProbe::Bytes(&TheCamera,sizeof(TheCamera));
 const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);Write({{"event","hook_state"},{"function",0},{"enabled",enabled},{"locked",locked},{"startup_calls",UDProbe::calls[0]}});
 auto& flags=CVehicleAnimGroupData::m_vehicleAnimGroups[0].m_specialFlags;const auto flagsBefore=flags.intValue;
 Raw<CVehicle> vehicle;Raw<CPed> ped;Raw<tHandlingData> handling;Raw<RpClump> clump;clump.get()->object.type=rpCLUMP;expectedClump=clump.get();bool passed=true;int pairs=0;
 {
 const bool he=helper->Hooked(),hl=helper->Locked();helper->LockState(false);Write({{"event","hook_state"},{"function",1},{"enabled",he},{"locked",hl},{"startup_calls",UDProbe::calls[1]}});
 vehicle.get()->m_pHandlingData=handling.get();
 for(int i=0;i<256&&passed;++i){currentCase=i;handling.get()->m_nModelFlags=static_cast<eVehicleHandlingModelFlags>((uint32(i%16)<<24)|uint32(i/16));
  helper->State(false);auto a=UDProbe::calls[1];auto original=AppearanceState(vehicle.get());auto b=UDProbe::calls[1];helper->State(true);auto candidate=AppearanceState(vehicle.get());auto c=UDProbe::calls[1];passed=original==candidate&&a==b&&c==b+1;
  Write({{"event","comparison"},{"function",1},{"case",i},{"pair",pairs++},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b}});
 }
 helper->State(he);helper->LockState(hl);log.flush();
 using Q=CAnimBlendAssociation*(*)(RpClump*,uint32);using B=CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float);using S=float(*)(eStats);
 ReversingProbe::EntryPatch qn(0x4D68B0,Query),ql(reinterpret_cast<uintptr_t>(static_cast<Q>(&RpAnimBlendClumpGetAssociation)),Query);
 ReversingProbe::EntryPatch bn(0x4D4610,Blend),bl(reinterpret_cast<uintptr_t>(static_cast<B>(&CAnimManager::BlendAnimation)),Blend);
 ReversingProbe::EntryPatch sn(0x558E40,Stat),sl(reinterpret_cast<uintptr_t>(static_cast<S>(&CStats::GetStatValue)),Stat);
 
 

 for(int i=0;i<16384&&passed;++i){currentCase=i;
  const auto run=[&]{
   vehicle.b.fill(0);ped.b.fill(0);handling.b.fill(0);trace=json::array();touched.fill(false);
   auto* v=vehicle.get();auto* p=ped.get();v->m_pHandlingData=handling.get();p->m_nPedType=i%97?PED_TYPE_PLAYER1:PED_TYPE_CIVMALE;*reinterpret_cast<RpClump**>(ped.b.data()+0x18)=clump.get();
   v->m_bOffscreen=i%101==0;kind=i%10;flags.intValue=kind==2?4:kind==3?8:0;v->vehicleFlags.bLowVehicle=kind==0;v->m_nVehicleType=kind==1?VEHICLE_TYPE_BOAT:VEHICLE_TYPE_AUTOMOBILE;handling.get()->m_bSitInBoat=(i/10)%2;handling.get()->m_nAnimGroup=0;
   skill=std::array{0.f,49.999996f,50.f,99.99999f,100.f,1000.f}[(i/3)%6];
   v->m_vecMoveSpeed=CVector{std::array{0.f,.4f,std::nextafter(.4f,0.f),std::nextafter(.4f,1.f),.3f,.2f}[(i/7)%6],(i/42)%2?.2f:0.f,(i/84)%2?.1f:0.f};
   v->m_GasPedal=i%3-1.f;v->m_fSteerAngle=std::array{0.f,-0.f,.61f,-.61f,.2f,-.2f,.6100001f,-.6100001f,1.f,-1.f,.001f,-.001f}[(i/11)%12];handling.get()->m_nModelFlags=std::array{VEHICLE_HANDLING_MODEL_NONE,VEHICLE_HANDLING_MODEL_IS_HELI,VEHICLE_HANDLING_MODEL_IS_PLANE}[(i/13)%3];
   TheCamera.m_nActiveCam=0;CCamera::GetActiveCamera().m_nMode=i%2?eCamMode::MODE_1STPERSON:eCamMode::MODE_FOLLOWPED;CCamera::GetActiveCamera().m_nDirectionWasLooking=static_cast<eLookingDirection>((i/2)%4);
   for(int id=0;id<512;++id){assocs[id].b.fill(0);present[id]=((uint32(i)*1664525u+uint32(id)*1013904223u)>>(id%17))%4!=0;auto* a=assocs[id].get();a->m_BlendAmount=std::array{0.f,.5f,1.f,1.1f}[(i/5+id)%4];a->m_BlendDelta=std::array{-4.f,0.f,2.f}[(i/17+id)%3];}
   present[ANIM_ID_CAR_TUNE_RADIO]=i%127==0;
   for(auto id:{ANIM_ID_DRIVEBY_L,ANIM_ID_DRIVEBY_R,ANIM_ID_DRIVEBYL_L,ANIM_ID_DRIVEBYL_R})present[id]=i%7==0&&((i/7+int(id))%5==0);
   reinterpret_cast<void(__thiscall*)(CVehicle*,CPed*,bool)>(0x6DF4A0)(v,p,i%113==0);
   json states=json::array();for(int id=0;id<512;++id)if(touched[id])states.push_back(json::array({id,present[id],ReversingProbe::Bytes(assocs[id].get(),sizeof(CAnimBlendAssociation))}));
   return json{{"trace",trace},{"states",states}};
  };
  hook->State(false);auto a=UDProbe::calls[0];auto original=run();auto b=UDProbe::calls[0];hook->State(true);auto candidate=run();auto c=UDProbe::calls[0];passed=original==candidate&&a==b&&c==b+1;
  Write({{"event","comparison"},{"function",0},{"case",i},{"pair",pairs++},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b}});if(i%64==0)log.flush();
 }
 }
 flags.intValue=flagsBefore;std::memcpy(&TheCamera,cameraBefore.data(),sizeof(TheCamera));hook->State(enabled);hook->LockState(locked);
 bool restored=playerBefore==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&cameraBefore==ReversingProbe::Bytes(&TheCamera,sizeof(TheCamera));Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});log.flush();
}
}
