#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Train.h"
#include "../ProbeRuntime.hpp"
namespace TrainLinks355Live {
inline bool done{};inline std::ofstream log;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,5> fns{{{"CTrain","FindCaboose",0x6F5E70,0,-1,1024},{"CTrain","FindEngine",0x6F5E90,0,-1,1024},{"CTrain","FindCarriage",0x6F5EB0,0,-1,1024},{"CTrain","FindNextStationPositionInDirection",0x6F5F00,0,-1,1024},{"CTrain","FindMaximumSpeedToStopAtStations",0x6F5BA0,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"train-links355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"train-links355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("train-links355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"train-links355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,5> hooks{};std::array<bool,5> enabled{},locked{};
 for(size_t n=0;n<5;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 const auto oldStations=StaticRef<std::array<float,6>>(0xC38034);const auto oldTotal=StaticRef<float>(0xC37FEC);uint32 pairs=0,totalFailures=0;
 for(size_t n=0;n<5;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<std::array<uint8,sizeof(CTrain)>,8> objects{};
   auto run=[&](bool candidate){memset(objects.data(),0xCD,sizeof(objects));const int length=1+i%8;
    for(int t=0;t<8;++t){auto* train=reinterpret_cast<CTrain*>(objects[t].data());train->m_pNextCarriage=t+1<length?reinterpret_cast<CTrain*>(objects[t+1].data()):nullptr;train->m_pPrevCarriage=t>0?reinterpret_cast<CTrain*>(objects[t-1].data()):nullptr;train->m_nTrackId=int8((i/11)%4);train->trainFlags.bClockwiseDirection=(i/3)%2;train->m_fCurrentRailDistance=float(i%101-10)*63.f;}
    const auto originalStations=std::array<float,6>{200.f,800.f,1400.f,2400.f,3600.f,4800.f};StaticRef<std::array<float,6>>(0xC38034)=originalStations;StaticRef<float>(0xC37FEC)=6000.f;
    const int index=(i/8)%length;auto* self=reinterpret_cast<CTrain*>(objects[index].data());float distance=-12.5f;int32 station=-12;intptr_t ret=0;
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    if(n<2)ret=intptr_t(reinterpret_cast<CTrain*(__cdecl*)(CTrain*)>(fn.address)(self));
    else if(n==2)ret=intptr_t(reinterpret_cast<CTrain*(__cdecl*)(CTrain*,uint8)>(fn.address)(self,uint8(i%12)));
    else if(n==3){const float position=std::array<float,12>{-100.f,0.f,100.f,200.f,300.f,799.f,800.f,900.f,4800.f,4900.f,6000.f,std::numeric_limits<float>::quiet_NaN()}[(i/2)%12];reinterpret_cast<void(__cdecl*)(bool,float,float*,int32*)>(fn.address)(i%2,position,&distance,&station);}
    else ret=reinterpret_cast<bool(__thiscall*)(void*,float*)>(fn.address)(self,&distance);
    if(n<3&&ret)ret=(ret-intptr_t(objects.data()))/sizeof(CTrain)+1;
    return json{{"switch",true},{"return",ret},{"distance",std::bit_cast<uint32>(distance)},{"station",station},{"objects",Hash(objects.data(),sizeof(objects))},{"stations",Hash(reinterpret_cast<void*>(0xC38034),24)},{"total",std::bit_cast<uint32>(StaticRef<float>(0xC37FEC))}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures}});
 }
 StaticRef<std::array<float,6>>(0xC38034)=oldStations;StaticRef<float>(0xC37FEC)=oldTotal;bool restored=StaticRef<std::array<float,6>>(0xC38034)==oldStations&&StaticRef<float>(0xC37FEC)==oldTotal;
 for(size_t n=0;n<5;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}

