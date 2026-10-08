#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "CarEnterExit.h"
#include "PlayerPed.h"
#include "Pad.h"
#include "Events/EventPedEnteredMyVehicle.h"
#include "Events/EventGroup.h"
#include "../ProbeRuntime.hpp"
namespace CarQuit355Live {
inline json trace;inline uint32 scenario{};inline CPad pad;
inline CPad* __fastcall Pad(const CPlayerPed*,void*){trace.push_back({"pad"});return &pad;}
inline uint8 __fastcall Melee(const CPad*,void*,bool only){trace.push_back({"melee",only});return uint8(scenario%5==0);}
inline int16 __fastcall UpDown(const CPad*,void*){trace.push_back({"up"});return int16((int32(scenario%5)-2)*64);}
inline int16 __fastcall LeftRight(const CPad*,void*){trace.push_back({"left"});return int16((int32((scenario/5)%5)-2)*64);}
inline eTaskType __fastcall Type(CTask*,void*){constexpr int32 types[]{0,0xCB,0xD0,0x2C0,0x321,0x322,0x323,0x324,0x325,0x326,0x327,0x328,0x32C,0x330,0x331,0x334,0x33D};trace.push_back({"type",types[scenario%17]});return eTaskType(types[scenario%17]);}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCarEnterExit","IsPlayerToQuitCarEnter",0x64F240,0,-1,4096}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"car-quit355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"car-quit355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("car-quit355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedTime=CTimer::GetTimeInMS();const auto savedOrientation=StaticRef<float>(0xB6F178);
 EntryPatch a(0x609560,Pad),ad(bit_cast<uintptr_t>(&CPlayerPed::GetPadFromPlayer),Pad);
 EntryPatch b(0x540390,Melee),bd(bit_cast<uintptr_t>(&CPad::MeleeAttackJustDown),Melee);
 EntryPatch c(0x53FD30,UpDown),cd(bit_cast<uintptr_t>(static_cast<int16(CPad::*)()const>(&CPad::GetPedWalkUpDown)),UpDown);
 EntryPatch d(0x53FC90,LeftRight),dd(bit_cast<uintptr_t>(static_cast<int16(CPad::*)()const>(&CPad::GetPedWalkLeftRight)),LeftRight);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();std::memset(&pad,0,sizeof(pad));pad.DisablePlayerControls=i%7==0;
    alignas(CPlayerPed) std::array<uint8,sizeof(CPlayerPed)> pedRaw{};alignas(CVehicle) std::array<uint8,sizeof(CVehicle)> vehicle{};alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrix{};alignas(CTask) std::array<uint8,32> taskRaw{};std::array<uintptr_t,9> vtable{};vtable[4]=bit_cast<uintptr_t>(&Type);At<uintptr_t>(taskRaw.data(),0)=uintptr_t(vtable.data());auto* ped=reinterpret_cast<CPlayerPed*>(pedRaw.data());auto* veh=reinterpret_cast<CVehicle*>(vehicle.data());At<uintptr_t>(vehicle.data(),0x14)=uintptr_t(matrix.data());At<CVector>(matrix.data(),0)=CVector{1,0,0};At<CVector>(matrix.data(),16)=CVector{.125f,1,0};At<CVector>(matrix.data(),32)=CVector{0,0,i%3==0?-1.f:1.f};At<CVector>(matrix.data(),48)=CVector{0,0,0};ped->GetPosition()=CVector{float(int32((i/17)%7)-3),float((i/119)%5),1};ped->m_fCurrentRotation=float(int32((i/595)%5)-2);At<uintptr_t>(vehicle.data(),0x490)=i%11==0?0x1234:0;
    StaticRef<uint32>(0xB7CB84)=uint32(i%5==0?0xFFFFFFFFu:2000);StaticRef<float>(0xB6F178)=float(i%9)*.5f;const int32 startTime=i%4==0?1999:i%4==1?0:1000;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;const bool result=reinterpret_cast<bool(__cdecl*)(const CPed*,const CVehicle*,int32,CTask*)>(fn.address)(ped,veh,startTime,i%8==0?nullptr:reinterpret_cast<CTask*>(taskRaw.data()));
    return json{{"ped",Digest(pedRaw.data(),pedRaw.size())},{"vehicle",Digest(vehicle.data(),vehicle.size())},{"pad",Digest(&pad,sizeof(pad))},{"task",Digest(taskRaw.data(),taskRaw.size())},{"result",result},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<uint32>(0xB7CB84)=savedTime;StaticRef<float>(0xB6F178)=savedOrientation;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
