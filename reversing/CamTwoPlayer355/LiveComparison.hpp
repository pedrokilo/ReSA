#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cam.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Collision/ColModel.h"
#include "World.h"
#include "Hud.h"
#include "Text/Text.h"
#include "Pad.h"
#include "../ProbeRuntime.hpp"
namespace CamTwoPlayer355Live {
template<class T> T& At(void* b,size_t o){return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o);}
inline json Bytes(const void* p,size_t n){json r=json::array();auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i)r.push_back(b[i]);return r;}
inline json trace;inline uint32 scenario,queries;
inline CPad* __cdecl GetPad(int32 n){return &CPad::Pads[n];}
inline bool __cdecl Sight(const CVector& from,const CVector& to,CColPoint& point,CEntity*& entity,bool a,bool b,bool c,bool d,bool e,bool f,bool g,bool h){trace.push_back({"sight",std::bit_cast<uint32>(from.x),std::bit_cast<uint32>(from.y),std::bit_cast<uint32>(from.z),std::bit_cast<uint32>(to.x),std::bit_cast<uint32>(to.y),std::bit_cast<uint32>(to.z),a,b,c,d,e,f,g,h});point.m_vecPoint=CVector{1.25f,2.5f,3.75f};entity=nullptr;const auto q=queries++;return scenario%8==0||q<uint32((scenario/8)%23);}
inline void __fastcall FollowCar(CCam* c,void*,const CVector& p,float a,float b,float d,bool e){trace.push_back({"followCar",std::bit_cast<uint32>(p.x),std::bit_cast<uint32>(p.y),std::bit_cast<uint32>(p.z),std::bit_cast<uint32>(a),std::bit_cast<uint32>(b),std::bit_cast<uint32>(d),e,uintptr_t(c->m_pCamTargetEntity)});c->m_vecSource=p;c->m_fFOV=55.f;}
inline void __fastcall FollowPed(CCam* c,void*,const CVector& p,float a,float b,float d,bool e){trace.push_back({"followPed",std::bit_cast<uint32>(p.x),std::bit_cast<uint32>(p.y),std::bit_cast<uint32>(p.z),std::bit_cast<uint32>(a),std::bit_cast<uint32>(b),std::bit_cast<uint32>(d),e,uintptr_t(c->m_pCamTargetEntity)});c->m_vecSource=p;c->m_fFOV=60.f;}
inline int16 __fastcall Aim(const CPad* pad,void*,CPed*){const int id=pad==&CPad::Pads[1]?1:0;trace.push_back({"aim",id});return int16(int32((scenario/4+id)%9)*32-128);}
inline const GxtChar* __fastcall Text(CText*,void*,const char* key){trace.push_back({"text",key});static const GxtChar value[]{'W','A','R','N',0};return value;}
inline void __cdecl Help(const GxtChar* text,bool a,bool b,bool c){trace.push_back({"help",std::string(reinterpret_cast<const char*>(text)),a,b,c});}
inline bool done;inline std::ofstream log;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,1> fns{{{"CCam","Process_Cam_TwoPlayer",0x525E50,0,-1,1024}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(){ReversibleHooks::ReversibleHook::Base* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CCam")for(auto& h:c.Items())if(h->Name()=="Process_Cam_TwoPlayer")result=h.get();});return result;}
inline void Update(){
 if(done)return;char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);auto dir=std::filesystem::path(executable).parent_path();
 if(!std::filesystem::exists(dir/"cam-two-player355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-two-player355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-two-player355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch sight0(0x56BA00,Sight),sight1(bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),Sight),pad(0x53FB70,GetPad);
 EntryPatch car0(0x5245B0,FollowCar),car1(bit_cast<uintptr_t>(&CCam::Process_FollowCar_SA),FollowCar),ped0(0x522D40,FollowPed),ped1(bit_cast<uintptr_t>(&CCam::Process_FollowPed_SA),FollowPed);
 EntryPatch aim0(0x541040,Aim),aim1(bit_cast<uintptr_t>(&CPad::AimWeaponLeftRight),Aim),text0(0x6A0050,Text),text1(bit_cast<uintptr_t>(&CText::Get),Text),help0(0x588BE0,Help),help1(bit_cast<uintptr_t>(&CHud::SetHelpMessage),Help);
 struct Region{uintptr_t address;std::vector<uint8> bytes;};std::array<Region,6> regions{{{0x8A5E50,std::vector<uint8>(4)},{0xB6EC24,std::vector<uint8>(8)},{0x96A8B4,std::vector<uint8>(8)},{0x8CCB80,std::vector<uint8>(1)},{0xB73458,std::vector<uint8>(sizeof(CPad)*2)},{0xB7CB84,std::vector<uint8>(4)}}};for(auto& r:regions)std::memcpy(r.bytes.data(),reinterpret_cast<void*>(r.address),r.bytes.size());
 std::array<uint8,800> savedPlayers{};std::memcpy(savedPlayers.data(),reinterpret_cast<void*>(0xB7CD98),800);
 std::array<uint8,8> savedModels{};std::memcpy(savedModels.data(),reinterpret_cast<void*>(0xA9B0CC),8);const auto savedStep=StaticRef<float>(0xB7CB5C);
 auto* hook=FindHook();if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);bool passed=true;int failures=0;
 for(int test=0;test<1024;++test){
  auto run=[&](bool candidate){
   hook->State(candidate);scenario=test;queries=0;trace=json::array();for(auto& r:regions)std::memcpy(reinterpret_cast<void*>(r.address),r.bytes.data(),r.bytes.size());std::memcpy(reinterpret_cast<void*>(0xB7CD98),savedPlayers.data(),800);
   alignas(CCam) std::array<uint8,sizeof(CCam)> camBytes{};camBytes.fill(0x5A);auto* c=reinterpret_cast<CCam*>(camBytes.data());c->m_fFOV=15.f+float(test%56);c->m_fHorizontalAngle=float(int32(test%17)-8)*.25f;c->m_fBetaSpeed=float(int32((test/17)%5)-2)*.01f;c->m_bResetStatics=test%2;
   StaticRef<uint32>(0x8A5E50)=uint32((test/2)%3);StaticRef<uint32>(0xB7CB84)=5000+test*17;StaticRef<uint32>(0xB6EC24)=test%3==0?StaticRef<uint32>(0xB7CB84)-300:100;StaticRef<uint32>(0xB6EC28)=test%4==0?StaticRef<uint32>(0xB7CB84)-500:100;StaticRef<uint32>(0x96A8B4)=test%5==0?100000:100;StaticRef<uint32>(0x96A8B8)=uint32((test/5)%8);
   std::memset(CPad::Pads,0,sizeof(CPad::Pads));for(int j=0;j<2;++j){auto* padBytes=reinterpret_cast<uint8*>(&CPad::Pads[j]);At<uint16>(padBytes,0x10A)=uint16((test/11+j)%5);At<int16>(padBytes,0x10)=test%16==j?1:0;At<int16>(padBytes,0x1A)=test%16==j?1:0;At<int16>(padBytes,0x40)=test%32==j?1:0;At<int16>(padBytes,0x4A)=test%32==j?1:0;}
   alignas(CPed) std::array<std::array<uint8,sizeof(CPed)>,2> pedBytes{};
   alignas(CVehicle) std::array<std::array<uint8,sizeof(CVehicle)>,2> vehicleBytes{};
   alignas(CMatrix) std::array<std::array<uint8,sizeof(CMatrix)>,2> matrixBytes{};
   alignas(CBaseModelInfo) std::array<std::array<uint8,sizeof(CBaseModelInfo)>,2> modelBytes{};
   alignas(CColModel) std::array<std::array<uint8,sizeof(CColModel)>,2> colBytes{};
   for(int j=0;j<2;++j){
    auto* ped=reinterpret_cast<CPed*>(pedBytes[j].data());auto* v=reinterpret_cast<CVehicle*>(vehicleBytes[j].data());auto* matrix=reinterpret_cast<CMatrix*>(matrixBytes[j].data());
    ped->m_pVehicle=test%7==0?nullptr:v;ped->bInVehicle=(test/7+j)%2;StaticRef<CPed*>(0xB7CD98+j*400)=ped;At<uintptr_t>(v,0x14)=((test/34+j)%3)==0?0:uintptr_t(matrix);At<int16>(v,0x22)=int16(j+1);
    const CVector pos{float(int32((test/4+j*3)%13)-6)*2.f,float(int32((test/52+j*5)%11)-5)*2.f,float((test/572+j)%3)};
    At<CVector>(v,4)=pos;matrix->GetPosition()=pos;matrix->GetRight()=CVector{1,0,0};matrix->GetForward()=CVector{0,1,0};matrix->GetUp()=CVector{0,0,1};
    v->m_vecMoveSpeed=CVector{float(int32((test/8+j)%7)-3)*.125f,float(int32((test/56+j*2)%7)-3)*.125f,float(j)*.0625f};
    At<uintptr_t>(modelBytes[j].data(),0x14)=uintptr_t(colBytes[j].data());At<float>(colBytes[j].data(),0x14)=.5f+float((test/7+j)%9)*.125f;
    StaticRef<uintptr_t>(0xA9B0CC+j*4)=uintptr_t(modelBytes[j].data());
   }
   if(test%32==0){auto* v=reinterpret_cast<CVehicle*>(vehicleBytes[1].data());At<CVector>(v,4)=At<CVector>(vehicleBytes[0].data(),4);reinterpret_cast<CMatrix*>(matrixBytes[1].data())->GetPosition()=At<CVector>(vehicleBytes[0].data(),4);}
   c->m_pCamTargetEntity=reinterpret_cast<CPed*>(pedBytes[0].data());StaticRef<float>(0xB7CB5C)=float(test%7)*.25f;
   const bool switched=ReversingProbe::JumpsToDll(0x525E50)==candidate;reinterpret_cast<void(__thiscall*)(CCam*)>(0x525E50)(c);
   json regionState=json::array();for(auto& r:regions)regionState.push_back(Bytes(reinterpret_cast<void*>(r.address),r.bytes.size()));
   return json{{"regions",regionState},{"trace",trace},{"cam",Bytes(camBytes.data(),camBytes.size())},{"peds",Bytes(pedBytes.data(),sizeof(pedBytes))},{"vehicles",Bytes(vehicleBytes.data(),sizeof(vehicleBytes))},{"matrices",Bytes(matrixBytes.data(),sizeof(matrixBytes))},{"models",Bytes(modelBytes.data(),sizeof(modelBytes))},{"colModels",Bytes(colBytes.data(),sizeof(colBytes))},{"players",Bytes(reinterpret_cast<void*>(0xB7CD98),800)},{"modelPointers",Bytes(reinterpret_cast<void*>(0xA9B0CC),8)},{"step",std::bit_cast<uint32>(StaticRef<float>(0xB7CB5C))},{"switches",switched}};
  };
  const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;Write({{"event","comparison"},{"pair",test},{"function","CCam::Process_Cam_TwoPlayer"},{"case",test},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
 }
 hook->State(enabled);hook->LockState(locked);for(auto& r:regions)std::memcpy(reinterpret_cast<void*>(r.address),r.bytes.data(),r.bytes.size());std::memcpy(reinterpret_cast<void*>(0xB7CD98),savedPlayers.data(),800);std::memcpy(reinterpret_cast<void*>(0xA9B0CC),savedModels.data(),8);StaticRef<float>(0xB7CB5C)=savedStep;
 Write({{"event","function_summary"},{"function","CCam::Process_Cam_TwoPlayer"},{"cases",1024},{"failures",failures}});Write({{"event",passed?"complete":"error"},{"pairs",1024},{"state_restored",true}});
}
}
