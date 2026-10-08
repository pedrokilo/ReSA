#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Train.h"
#include "TrainNode.h"
#include "TheZones.h"
#include "../ProbeRuntime.hpp"
namespace TrainQueries355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{};
inline uint32 __cdecl Zone(const CVector& p){trace.push_back({"zone",std::bit_cast<uint32>(p.x),std::bit_cast<uint32>(p.y),std::bit_cast<uint32>(p.z)});return uint32(scenario%5);}
inline float __cdecl Stat(eStats stat){const auto v=scenario%17==0?std::numeric_limits<float>::quiet_NaN():float((scenario/5)%7-1);trace.push_back({"stat",int(stat),std::bit_cast<uint32>(v)});return v;}
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,2> fns{{{"CTrain","FindClosestTrackNode",0x6F6BD0,0,-1,1024},{"CTrain","IsNextStationAllowed",0x6F7260,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"train-queries355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"train-queries355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("train-queries355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"train-queries355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,2> hooks{};std::array<bool,2> enabled{},locked{};
 for(size_t n=0;n<2;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 const auto oldStations=StaticRef<std::array<float,6>>(0xC38034);const auto oldTotal=StaticRef<float>(0xC37FEC);const auto oldCounts=StaticRef<std::array<int32,4>>(0xC38014);const auto oldNodes=StaticRef<std::array<CTrainNode*,4>>(0xC38024);uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch z(0x572300,Zone),zd(bit_cast<uintptr_t>(&CTheZones::GetLevelFromPosition),Zone);
 EntryPatch t(0x558E40,Stat),td(bit_cast<uintptr_t>(static_cast<float(*)(eStats)>(&CStats::GetStatValue)),Stat);
 for(size_t n=0;n<2;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<std::array<uint8,sizeof(CTrain)>,8> objects{};std::array<std::array<CTrainNode,8>,4> nodes;
   auto run=[&](bool candidate){scenario=i;trace=json::array();memset(objects.data(),0xCD,sizeof(objects));memset(nodes.data(),0xCD,sizeof(nodes));const auto length=1+i%8;
    for(int t=0;t<8;++t){auto* train=reinterpret_cast<CTrain*>(objects[t].data());train->m_pNextCarriage=t+1<length?reinterpret_cast<CTrain*>(objects[t+1].data()):nullptr;train->m_pPrevCarriage=t>0?reinterpret_cast<CTrain*>(objects[t-1].data()):nullptr;train->trainFlags.bClockwiseDirection=i%2;train->m_fCurrentRailDistance=float(i%101-5)*61.f;}
    for(int t=0;t<4;++t){StaticRef<std::array<int32,4>>(0xC38014)[t]=1+(i+t)%8;StaticRef<std::array<CTrainNode*,4>>(0xC38024)[t]=nodes[t].data();for(int j=0;j<8;++j){nodes[t][j].x=int16(i%129-64+j*8+t*32);nodes[t][j].y=int16(i%73-36+j*4-t*24);nodes[t][j].z=int16(j*3+t*8);}}
    StaticRef<std::array<float,6>>(0xC38034)={200.f,800.f,1400.f,2400.f,3600.f,4800.f};StaticRef<float>(0xC37FEC)=6000.f;
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};int32 outTrack=-123,ret;
    if(n==0)ret=reinterpret_cast<int32(__cdecl*)(CVector,int32*)>(fn.address)(CVector{float(i%31-15),float(i%23-11),float(i%19-9)},&outTrack);
    else ret=reinterpret_cast<bool(__cdecl*)(CTrain*)>(fn.address)(reinterpret_cast<CTrain*>(objects[(i/8)%length].data()));
    return json{{"switch",true},{"return",ret},{"outtrack",outTrack},{"objects",Hash(objects.data(),sizeof(objects))},{"nodes",Hash(nodes.data(),sizeof(nodes))},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures}});
 }
 }
 StaticRef<std::array<int32,4>>(0xC38014)=oldCounts;StaticRef<std::array<CTrainNode*,4>>(0xC38024)=oldNodes;
 StaticRef<std::array<float,6>>(0xC38034)=oldStations;StaticRef<float>(0xC37FEC)=oldTotal;bool restored=StaticRef<std::array<float,6>>(0xC38034)==oldStations&&StaticRef<float>(0xC37FEC)==oldTotal;
 for(size_t n=0;n<2;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}

