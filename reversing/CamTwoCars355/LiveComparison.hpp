#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cam.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Collision/ColModel.h"
#include "../ProbeRuntime.hpp"
namespace CamTwoCars355Live {
template<class T> T& At(void* b,size_t o){return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o);}
inline json Bytes(const void* p,size_t n){json r=json::array();auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i)r.push_back(b[i]);return r;}
inline bool done;inline std::ofstream log;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,1> fns{{{"CCam","Process_Cam_TwoPlayer_Separate_Cars",0x513510,0,-1,1024}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(){ReversibleHooks::ReversibleHook::Base* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CCam")for(auto& h:c.Items())if(h->Name()=="Process_Cam_TwoPlayer_Separate_Cars")result=h.get();});return result;}
inline void Update(){
 if(done)return;char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);auto dir=std::filesystem::path(executable).parent_path();
 if(!std::filesystem::exists(dir/"cam-two-cars355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-two-cars355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-two-cars355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 std::array<uint8,800> savedPlayers{};std::memcpy(savedPlayers.data(),reinterpret_cast<void*>(0xB7CD98),800);
 std::array<uint8,8> savedModels{};std::memcpy(savedModels.data(),reinterpret_cast<void*>(0xA9B0CC),8);const auto savedStep=StaticRef<float>(0xB7CB5C);
 auto* hook=FindHook();if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);bool passed=true;int failures=0;
 for(int test=0;test<1024;++test){
  auto run=[&](bool candidate){
   hook->State(candidate);std::memcpy(reinterpret_cast<void*>(0xB7CD98),savedPlayers.data(),800);
   alignas(CCam) std::array<uint8,sizeof(CCam)> camBytes{};camBytes.fill(0x5A);auto* c=reinterpret_cast<CCam*>(camBytes.data());At<uint32>(c,0x110)=uint32(test%2);At<float>(c,0x114)=float((test/2)%17)*.0625f;
   alignas(CPed) std::array<std::array<uint8,sizeof(CPed)>,2> pedBytes{};
   alignas(CVehicle) std::array<std::array<uint8,sizeof(CVehicle)>,2> vehicleBytes{};
   alignas(CMatrix) std::array<std::array<uint8,sizeof(CMatrix)>,2> matrixBytes{};
   alignas(CBaseModelInfo) std::array<std::array<uint8,sizeof(CBaseModelInfo)>,2> modelBytes{};
   alignas(CColModel) std::array<std::array<uint8,sizeof(CColModel)>,2> colBytes{};
   for(int j=0;j<2;++j){
    auto* ped=reinterpret_cast<CPed*>(pedBytes[j].data());auto* v=reinterpret_cast<CVehicle*>(vehicleBytes[j].data());auto* matrix=reinterpret_cast<CMatrix*>(matrixBytes[j].data());
    ped->m_pVehicle=v;StaticRef<CPed*>(0xB7CD98+j*400)=ped;At<uintptr_t>(v,0x14)=((test/34+j)%3)==0?0:uintptr_t(matrix);At<int16>(v,0x22)=int16(j+1);
    const CVector pos{float(int32((test/4+j*3)%13)-6)*2.f,float(int32((test/52+j*5)%11)-5)*2.f,float((test/572+j)%3)};
    At<CVector>(v,4)=pos;matrix->GetPosition()=pos;matrix->GetRight()=CVector{1,0,0};matrix->GetForward()=CVector{0,1,0};matrix->GetUp()=CVector{0,0,1};
    v->m_vecMoveSpeed=CVector{float(int32((test/8+j)%7)-3)*.125f,float(int32((test/56+j*2)%7)-3)*.125f,float(j)*.0625f};
    At<uintptr_t>(modelBytes[j].data(),0x14)=uintptr_t(colBytes[j].data());At<float>(colBytes[j].data(),0x14)=.5f+float((test/7+j)%9)*.125f;
    StaticRef<uintptr_t>(0xA9B0CC+j*4)=uintptr_t(modelBytes[j].data());
   }
   if(test%32==0){auto* v=reinterpret_cast<CVehicle*>(vehicleBytes[1].data());At<CVector>(v,4)=At<CVector>(vehicleBytes[0].data(),4);reinterpret_cast<CMatrix*>(matrixBytes[1].data())->GetPosition()=At<CVector>(vehicleBytes[0].data(),4);}
   StaticRef<float>(0xB7CB5C)=float(test%7)*.25f;
   const bool switched=ReversingProbe::JumpsToDll(0x513510)==candidate;reinterpret_cast<void(__thiscall*)(CCam*)>(0x513510)(c);
   return json{{"cam",Bytes(camBytes.data(),camBytes.size())},{"peds",Bytes(pedBytes.data(),sizeof(pedBytes))},{"vehicles",Bytes(vehicleBytes.data(),sizeof(vehicleBytes))},{"matrices",Bytes(matrixBytes.data(),sizeof(matrixBytes))},{"models",Bytes(modelBytes.data(),sizeof(modelBytes))},{"colModels",Bytes(colBytes.data(),sizeof(colBytes))},{"players",Bytes(reinterpret_cast<void*>(0xB7CD98),800)},{"modelPointers",Bytes(reinterpret_cast<void*>(0xA9B0CC),8)},{"step",std::bit_cast<uint32>(StaticRef<float>(0xB7CB5C))},{"switches",switched}};
  };
  const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;Write({{"event","comparison"},{"pair",test},{"function","CCam::Process_Cam_TwoPlayer_Separate_Cars"},{"case",test},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
 }
 hook->State(enabled);hook->LockState(locked);std::memcpy(reinterpret_cast<void*>(0xB7CD98),savedPlayers.data(),800);std::memcpy(reinterpret_cast<void*>(0xA9B0CC),savedModels.data(),8);StaticRef<float>(0xB7CB5C)=savedStep;
 Write({{"event","function_summary"},{"function","CCam::Process_Cam_TwoPlayer_Separate_Cars"},{"cases",1024},{"failures",failures}});Write({{"event",passed?"complete":"error"},{"pairs",1024},{"state_restored",true}});
}
}
