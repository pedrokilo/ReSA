#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Train.h"
#include "TrainNode.h"
#include "TheZones.h"
#include "Streaming.h"
#include "GameLogic.h"
#include "../ProbeRuntime.hpp"
namespace TrainInit355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{};
inline std::array<std::array<CTrainNode,8>,4> nodes;
inline int closestCalls{};
inline void __cdecl Read(const char* filename,CTrainNode** out,int32* count,float* total,int32 track){trace.push_back({"read",filename,track,uintptr_t(out)==uintptr_t(0xC38024+track*4),uintptr_t(count)==uintptr_t(0xC38014+track*4),uintptr_t(total)==uintptr_t(0xC37FEC+track*4)});*out=nodes[track].data();*count=8;*total=float(1000+track*100+scenario);}
inline int32 __cdecl Closest(CVector position,int32* track){const auto index=(scenario+closestCalls++)%8;*track=(scenario+closestCalls)%4;nodes[0][index].m_nDistanceFromStart=uint16(nodes[0][index].m_nDistanceFromStart+3);trace.push_back({"closest",std::bit_cast<uint32>(position.x),std::bit_cast<uint32>(position.y),std::bit_cast<uint32>(position.z),index,*track});return index;}
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CTrain","InitTrains",0x6F7440,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"train-init355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"train-init355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("train-init355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"train-init355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 std::array<uint8,0x60> saved;memcpy(saved.data(),reinterpret_cast<void*>(0xC37FEC),saved.size());uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x6F55D0,Read),ad(bit_cast<uintptr_t>(&CTrain::ReadAndInterpretTrackFile),Read);
 EntryPatch b(0x6F6BD0,Closest),bd(bit_cast<uintptr_t>(&CTrain::FindClosestTrackNode),Closest);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){scenario=i;closestCalls=0;trace=json::array();memset(nodes.data(),0xCD,sizeof(nodes));memset(reinterpret_cast<void*>(0xC37FEC),0xCD,saved.size());
    for(size_t track=0;track<4;++track){StaticRef<std::array<CTrainNode*,4>>(0xC38024)[track]=(i&(1<<track))?nodes[track].data():nullptr;StaticRef<std::array<int32,4>>(0xC38014)[track]=int32(track)+1;StaticRef<std::array<float,4>>(0xC37FEC)[track]=float(track)*123.f;for(size_t j=0;j<8;++j)nodes[track][j].m_nDistanceFromStart=uint16(scenario*17+int(track)*137+int(j)*31);}
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__cdecl*)()>(fn.address)();
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"globals",Hash(reinterpret_cast<void*>(0xC37FEC),saved.size())},{"nodes",Hash(nodes.data(),sizeof(nodes))},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 memcpy(reinterpret_cast<void*>(0xC37FEC),saved.data(),saved.size());bool restored=memcmp(reinterpret_cast<void*>(0xC37FEC),saved.data(),saved.size())==0;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}

