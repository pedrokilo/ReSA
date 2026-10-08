#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Train.h"
#include "TrainNode.h"
#include "../ProbeRuntime.hpp"
namespace TrainCoordinates355Live {
inline bool done{};inline std::ofstream log;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CTrain","FindCoorsFromPositionOnTrack",0x6F59E0,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"train-coordinates355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"train-coordinates355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("train-coordinates355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"train-coordinates355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 const auto oldCounts=StaticRef<std::array<int32,4>>(0xC38014);const auto oldNodes=StaticRef<std::array<CTrainNode*,4>>(0xC38024);uint32 pairs=0,totalFailures=0;
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0;
  for(int i=0;i<fn.cases;++i){std::array<CTrainNode,8> nodes;CVector output;
   auto run=[&](bool candidate){memset(nodes.data(),0xCD,sizeof(nodes));output=CVector{111.f,222.f,333.f};
    const auto count=i%9;const auto track=(i/9)%4;StaticRef<std::array<int32,4>>(0xC38014).fill(count);StaticRef<std::array<CTrainNode*,4>>(0xC38024).fill(nodes.data());
    for(int j=0;j<8;++j){nodes[j].x=int16((i+j)%129-64);nodes[j].y=int16((i-j)%93);nodes[j].z=int16((i+j)%31-15);nodes[j].m_nDistanceFromStart=uint16(j*30+(i/36)%7);}
    float distance=float((i/4)%81-5);if(i%17==0)distance=std::numeric_limits<float>::quiet_NaN();
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__cdecl*)(float,int32,CVector*)>(fn.address)(distance,track,&output);
    return json{{"switch",true},{"output",Hash(&output,sizeof(output))},{"nodes",Hash(nodes.data(),sizeof(nodes))},{"counts",StaticRef<std::array<int32,4>>(0xC38014)}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures}});
 }
 StaticRef<std::array<int32,4>>(0xC38014)=oldCounts;StaticRef<std::array<CTrainNode*,4>>(0xC38024)=oldNodes;bool restored=StaticRef<std::array<int32,4>>(0xC38014)==oldCounts&&StaticRef<std::array<CTrainNode*,4>>(0xC38024)==oldNodes;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}


