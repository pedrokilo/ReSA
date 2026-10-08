#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Pipelines/CustomBuilding/CustomBuildingRenderer.h"
#include "Pipelines/CustomBuilding/CustomBuildingDNPipeline.h"
#include "Pipelines/CustomBuilding/CustomBuildingPipeline.h"
#include "../ProbeRuntime.hpp"
namespace RendererPendingLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,4> fns{{
 {"CCustomBuildingRenderer","AtomicSetup",0x5D7F00,0,-1,1024},
 {"CCustomBuildingRenderer","IsCBPCPipelineAttached",0x5D7F40,0,-1,1024},
 {"CCustomBuildingRenderer","UpdateDayNightBalanceParam",0x5D7F80,0,-1,2048},
 {"CCustomBuildingRenderer","Update",0x5D8050,0,-1,2048},
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}


inline uint32 pipeID{};
inline uint32 __cdecl PipelineID(RpAtomic*) {trace.push_back({"id",pipeID});return pipeID;}
inline RpAtomic* __cdecl Basic(RpAtomic* a) {trace.push_back("basic");At<uint8>(a,2)^=1;return a;}
inline RpAtomic* __cdecl DayNight(RpAtomic* a) {trace.push_back("daynight");At<uint8>(a,2)^=2;return a;}
inline void Update() {
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"renderer-pending-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"renderer-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("renderer-pending");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 auto& clock=*reinterpret_cast<uint32*>(0xB70150);auto& balance=CCustomBuildingDNPipeline::m_fDNBalanceParam;auto& index=*reinterpret_cast<uint32*>(0xC02C14);auto& magic=*reinterpret_cast<uint32*>(0xC02C18);auto& offset=CCustomBuildingDNPipeline::ms_extraVertColourPluginOffset;
 const auto savedClock=clock,savedIndex=index,savedMagic=magic;const auto savedBalance=balance;const auto savedOffset=offset;
 bool passed=true;uint32 pairCount=0;int basicCount=0,dnCount=0;
 {
 using ReversingProbe::EntryPatch;
 EntryPatch e1(0x72FC40,PipelineID),d1(std::bit_cast<uintptr_t>(&GetPipelineID),PipelineID),e2(0x5D7E50,Basic),d2(std::bit_cast<uintptr_t>(&CCustomBuildingPipeline::CustomPipeAtomicSetup),Basic),e3(0x5D71C0,DayNight),d3(std::bit_cast<uintptr_t>(&CCustomBuildingDNPipeline::CustomPipeAtomicSetup),DayNight);
 for(const auto& fn:fns){auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
 const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
 for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){hook->State(candidate);trace=json::array();
 alignas(16) std::array<uint8,sizeof(RpGeometry)+sizeof(ExtraVertColour)> geometry{};RpAtomic atomic{};atomic.geometry=reinterpret_cast<RpGeometry*>(geometry.data());At<uint8>(&atomic,2)=uint8(i);
 offset=sizeof(RpGeometry);auto* extra=reinterpret_cast<ExtraVertColour*>(geometry.data()+offset);extra->NightColors=i%4<2?reinterpret_cast<RwRGBA*>(uintptr_t(0x123400)):nullptr;atomic.geometry->preLitLum=i%2?reinterpret_cast<RwRGBA*>(uintptr_t(0x567800)):nullptr;
 pipeID=i%5==0?0x53F2009C:i%5==1?0x53F20098:0x11223344u+uint32(i);
 constexpr std::array<int,12> hours{0,5,6,6,7,19,20,20,21,23,24,255};clock=uint32(uint16(i%3==0?i*31:i%60))|(uint32(i%60)<<16)|(uint32(hours[(i/60)%hours.size()])<<24);
 balance=std::bit_cast<float>(uint32(0x3F001234));index=i%3==0?0xffffffffu:uint32(i*17);magic=0x12345678;
 const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;bool result=false;
 if(fn.address==0x5D7F00)reinterpret_cast<void(__cdecl*)(RpAtomic*)>(fn.address)(&atomic);
 else if(fn.address==0x5D7F40)result=reinterpret_cast<bool(__cdecl*)(RpAtomic*)>(fn.address)(&atomic);
 else reinterpret_cast<void(__cdecl*)()>(fn.address)();
 return json{{"atomic",ReversingProbe::Bytes(&atomic,sizeof(atomic))},{"geometry",ReversingProbe::Bytes(geometry.data(),geometry.size())},{"clock",clock},{"balance",F(balance)},{"index",index},{"magic",magic},{"result",result},{"trace",trace},{"switches",switched}};
 };
 const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
 if(fn.address==0x5D7F00){basicCount+=original["trace"]==json::array({"basic"});dnCount+=original["trace"]==json::array({"daynight"});}
 Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
 }
 hook->State(enabled);hook->LockState(locked);Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 }
 clock=savedClock;balance=savedBalance;index=savedIndex;magic=savedMagic;offset=savedOffset;
 const bool coverage=basicCount>0&&dnCount>0;Write({{"event","coverage"},{"basic",basicCount},{"daynight",dnCount},{"accepted",coverage}});
 Write({{"event",passed&&coverage?"complete":"error"},{"pairs",pairCount},{"state_restored",clock==savedClock&&balance==savedBalance&&index==savedIndex&&magic==savedMagic&&offset==savedOffset}});
}
}
