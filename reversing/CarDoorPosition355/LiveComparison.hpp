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
namespace CarDoorPosition355Live {
inline json trace;inline uint32 scenario{};
inline CVector* __fastcall Offset(CVehicleAnimGroup* self,void*,CVector* out,eVehAnimDoorOffset kind){trace.push_back({"offset",uintptr_t(self),int(kind)});*out=CVector{float(int(kind)+1)*.125f,.25f,.5f};return out;}
inline void __fastcall Bike(CBike*,void*,CVector& out,CVector offset,CVector seat){trace.push_back({"bike",std::bit_cast<uint32>(offset.x),std::bit_cast<uint32>(seat.x)});out=seat-offset;}
inline float __fastcall Height(CVehicle*,void*){trace.push_back({"height"});return float(scenario%5)*.125f;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCarEnterExit","GetPositionToOpenCarDoor",0x64E740,0,-1,4096}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"car-door-position355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"car-door-position355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("car-door-position355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedModel=StaticRef<uintptr_t>(0xA9B0C8+2000*4);
 EntryPatch a(0x6E3D10,Offset),ad(bit_cast<uintptr_t>(&CVehicleAnimGroup::ComputeAnimDoorOffsets),Offset);
 EntryPatch b(0x6BF230,Bike),bd(bit_cast<uintptr_t>(&CBike::GetCorrectedWorldDoorPosition),Bike);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CBike) std::array<uint8,sizeof(CBike)> raw{};auto* veh=reinterpret_cast<CVehicle*>(raw.data());alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrix{};alignas(CVehicleModelInfo) std::array<uint8,sizeof(CVehicleModelInfo)> model{};alignas(16) std::array<uint8,256> structure{};alignas(tHandlingData) std::array<uint8,sizeof(tHandlingData)> handling{};std::array<uintptr_t,80> vtable{};vtable[0xD4/4]=bit_cast<uintptr_t>(&Height);
    At<uintptr_t>(raw.data(),0)=uintptr_t(vtable.data());At<uintptr_t>(raw.data(),0x14)=uintptr_t(matrix.data());At<int16>(raw.data(),0x22)=2000;veh->m_pHandlingData=reinterpret_cast<tHandlingData*>(handling.data());veh->m_nVehicleType=i%4==0?VEHICLE_TYPE_BIKE:VEHICLE_TYPE_AUTOMOBILE;veh->m_nVehicleSubType=i%5==0?VEHICLE_TYPE_MTRUCK:i%5==1?VEHICLE_TYPE_PLANE:VEHICLE_TYPE_AUTOMOBILE;
    auto* h=veh->m_pHandlingData;h->m_bTandemSeats=i%3==0;h->m_bIsBig=i%2!=0;h->m_fSeatOffsetDistance=float((i/20)%5)*.125f;h->m_nAnimGroup=uint8(i%7);At<uintptr_t>(model.data(),0x5C)=uintptr_t(structure.data());At<int32>(model.data(),0x3C)=i%6==0?5:0;for(size_t k=0;k<6;++k)At<CVector>(structure.data(),k*12)=CVector{float(k+1),float(k)*.5f,.25f};StaticRef<uintptr_t>(0xA9B0C8+2000*4)=uintptr_t(model.data());
    At<CVector>(matrix.data(),0)=CVector{1,.125f,0};At<CVector>(matrix.data(),16)=CVector{-.125f,1,0};At<CVector>(matrix.data(),32)=CVector{0,0,1};At<CVector>(matrix.data(),48)=CVector{float(i%5),-2,3};
    constexpr int doors[]{8,9,10,11,18};const int32 door=doors[i%5];CVector result;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<CVector*(__cdecl*)(CVector*,const CVehicle*,int32)>(fn.address)(&result,veh,door);
    return json{{"vehicle",Digest(raw.data(),raw.size())},{"matrix",Digest(matrix.data(),matrix.size())},{"handling",Digest(handling.data(),handling.size())},{"result",{F(result.x),F(result.y),F(result.z)}},{"trace",trace},{"switches",switched}};
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
