#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "CarEnterExit.h"
#include "VehicleAnimGroupData.h"
#include "VehicleModelInfo.h"
#include "Bike.h"
#include "Events/EventPedEnteredMyVehicle.h"
#include "Events/EventGroup.h"
#include "../ProbeRuntime.hpp"
namespace CarNearest355Live {
inline json trace;inline uint32 scenario{};
inline CVector* __cdecl Door(CVector* out,const CVehicle*,int32 door){trace.push_back({"door",door});*out=CVector{float(door-8)*.5f,float(int32((scenario/8)%7)-3),float((scenario/56)%5)*.125f};return out;}
inline bool __cdecl Room(const CVehicle*,int32 door,const CVector* offset){trace.push_back({"room",door,uintptr_t(offset)});return (scenario+uint32(door))%5!=0;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCarEnterExit","GetNearestCarPassengerDoor",0x650BB0,0,-1,4096}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"car-nearest355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"car-nearest355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("car-nearest355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedModel=StaticRef<uintptr_t>(0xA9B0C8+2000*4);
 EntryPatch a(0x64E740,Door),ad(bit_cast<uintptr_t>(&CCarEnterExit::GetPositionToOpenCarDoor),Door);
 EntryPatch b(0x6504C0,Room),bd(bit_cast<uintptr_t>(&CCarEnterExit::IsRoomForPedToLeaveCar),Room);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();alignas(CVehicle) std::array<uint8,0xA00> vehicle{};auto* veh=reinterpret_cast<CVehicle*>(vehicle.data());alignas(CVehicleModelInfo) std::array<uint8,sizeof(CVehicleModelInfo)> model{};alignas(tHandlingData) std::array<uint8,sizeof(tHandlingData)> handling{};alignas(CPed) std::array<uint8,sizeof(CPed)> pedRaw{};auto* ped=reinterpret_cast<CPed*>(pedRaw.data());
    At<int16>(vehicle.data(),0x22)=2000;veh->m_pHandlingData=reinterpret_cast<tHandlingData*>(handling.data());veh->m_nVehicleType=i%4==0?VEHICLE_TYPE_BIKE:i%4==1?VEHICLE_TYPE_BOAT:VEHICLE_TYPE_AUTOMOBILE;auto* h=veh->m_pHandlingData;h->m_bTandemSeats=i%3==0;h->m_nAnimGroup=i%9==0?15:i%9==1?16:0;At<uint32>(handling.data(),0xCC)|=i%5==0?0x8000u:0;At<int8>(model.data(),0x4C)=int8((i/16)%5);StaticRef<uintptr_t>(0xA9B0C8+2000*4)=uintptr_t(model.data());At<uint8>(vehicle.data(),0x486)=uint8((i/64)%16);veh->m_apPassengers[0]=i%7==0?reinterpret_cast<CPed*>(0x1234):nullptr;veh->m_apPassengers[1]=i%11==0?reinterpret_cast<CPed*>(0x1234):nullptr;veh->m_apPassengers[2]=i%13==0?reinterpret_cast<CPed*>(0x1234):nullptr;At<uintptr_t>(vehicle.data(),0x674)=i%3==0?0x1234:0;At<uintptr_t>(vehicle.data(),0x66C)=i%3==1?0x1234:0;ped->GetPosition()=CVector{float(int32((i/4)%7)-3),float((i/28)%5),0};
    CVector output{-999,888,777};int32 door=-999;const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;const bool result=reinterpret_cast<bool(__cdecl*)(const CPed*,const CVehicle*,CVector*,int32*,bool,bool,bool)>(fn.address)(ped,veh,&output,&door,i%2!=0,(i/2)%2!=0,(i/4)%2!=0);
    return json{{"vehicle",Digest(vehicle.data(),vehicle.size())},{"ped",Digest(pedRaw.data(),pedRaw.size())},{"result",result},{"output",result?json{F(output.x),F(output.y),F(output.z)}:json(nullptr)},{"door",result?json(door):json(nullptr)},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<uintptr_t>(0xA9B0C8+2000*4)=savedModel;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
