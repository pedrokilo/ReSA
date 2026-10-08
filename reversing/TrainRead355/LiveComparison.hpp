#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Train.h"
#include "TrainNode.h"
#include "TheZones.h"
#include "Streaming.h"
#include "GameLogic.h"
#include "FileMgr.h"
#include "../ProbeRuntime.hpp"
namespace TrainRead355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{};
alignas(16) inline std::array<uint8,80> createdNodes{},existingNodes{};
alignas(16) inline std::array<uint8,0xB530> fileBuffer{};
inline std::array<char,1024> text{};
inline std::array<std::array<uint32,4>,8> calls{};inline size_t callCount{};inline int countFixture{};
inline void Record(uint32 op,uint32 a=0,uint32 b=0,uint32 c=0){if(callCount<calls.size())calls[callCount++]={op,a,b,c};}
inline void* __cdecl NewArray(size_t size){Record(1,uint32(size));return size==0xB530?fileBuffer.data():createdNodes.data();}
inline void __cdecl DeleteArray(void* memory){Record(3,memory==fileBuffer.data()?1u:memory==createdNodes.data()?2u:3u);}
inline size_t __cdecl Load(const char* filename,uint8* buffer,size_t size,const char* mode){Record(2,strcmp(filename,"fixture.dat")==0,uint32(size),strcmp(mode,"rb")==0);const auto length=strlen(text.data());memcpy(buffer,text.data(),length+1);return length;}
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CTrain","ReadAndInterpretTrackFile",0x6F55D0,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"train-read355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"train-read355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("train-read355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"train-read355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 std::array<uint8,352> savedString;std::array<uint8,72> savedStations;memcpy(savedString.data(),reinterpret_cast<void*>(0xB71670),savedString.size());memcpy(savedStations.data(),reinterpret_cast<void*>(0x8D48F8),savedStations.size());uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x821195,NewArray),b(0x8213AE,DeleteArray);
 EntryPatch c(0x538890,Load),cd(bit_cast<uintptr_t>(&CFileMgr::LoadFile),Load);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){scenario=i;callCount=0;trace=json::array();memset(createdNodes.data(),0xCD,createdNodes.size());memset(existingNodes.data(),0xCD,existingNodes.size());memset(fileBuffer.data(),0xCD,fileBuffer.size());memset(reinterpret_cast<void*>(0xB71670),0xCD,savedString.size());memset(reinterpret_cast<void*>(0x8D48F8),0xCD,savedStations.size());
    countFixture=(i/2)%9;const auto processed=(i/18)%2!=0;size_t length=size_t(sprintf(text.data(),processed?"processed\n%d\n":"%d\n",countFixture));
    for(int j=0;j<countFixture;++j){const auto x=(i%101-50)*.125f+j*.25f,y=(i%73-36)*.25f-j*.125f,z=(i%31-15)*.5f;length+=size_t(sprintf(text.data()+length,"%.3f %.3f %.3f %d\n",double(x),double(y),double(z),j%3==0));auto* node=reinterpret_cast<CTrainNode*>(existingNodes.data())+j;node->x=int16(x*8.f);node->y=int16(y*8.f);node->z=int16(z*8.f);}
    CTrainNode* nodes=i%2?reinterpret_cast<CTrainNode*>(existingNodes.data()):nullptr;int32 count=countFixture;float total=-123.f;
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__cdecl*)(const char*,CTrainNode**,int32*,float*,int32)>(fn.address)("fixture.dat",&nodes,&count,&total,(i/36)%4);
    for(size_t call=0;call<callCount;++call)trace.push_back(calls[call]);if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"count",count},{"total",std::bit_cast<uint32>(total)},{"node_identity",nodes==reinterpret_cast<CTrainNode*>(createdNodes.data())?1:nodes==reinterpret_cast<CTrainNode*>(existingNodes.data())?2:3},{"created",Hash(createdNodes.data(),createdNodes.size())},{"existing",Hash(existingNodes.data(),existingNodes.size())},{"buffer",Hash(fileBuffer.data(),fileBuffer.size())},{"string",Hash(reinterpret_cast<void*>(0xB71670),savedString.size())},{"stations",Hash(reinterpret_cast<void*>(0x8D48F8),savedStations.size())},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 memcpy(reinterpret_cast<void*>(0xB71670),savedString.data(),savedString.size());memcpy(reinterpret_cast<void*>(0x8D48F8),savedStations.data(),savedStations.size());bool restored=true;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}

