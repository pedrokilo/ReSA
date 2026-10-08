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
namespace TrainSkip355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{};
inline int stationCalls{};inline CTrain* activeEngine{};
inline void __cdecl Next(bool clockwise,float distance,float* outDistance,int32* station){*station=(scenario+stationCalls++)%6;*outDistance=distance+(clockwise?100.f:-100.f);trace.push_back({"next",clockwise,std::bit_cast<uint32>(distance),std::bit_cast<uint32>(*outDistance),*station});}
inline uint32 __cdecl Zone(const CVector& p){const auto level=stationCalls<=scenario%4?3u:0u;trace.push_back({"zone",std::bit_cast<uint32>(p.x),std::bit_cast<uint32>(p.y),std::bit_cast<uint32>(p.z),level});return level;}
inline float __cdecl Stat(eStats stat){trace.push_back({"stat",int(stat)});return 1.f;}
inline void __cdecl Scene(const CVector& p){trace.push_back({"scene",std::bit_cast<uint32>(p.x),std::bit_cast<uint32>(p.y),std::bit_cast<uint32>(p.z)});activeEngine->GetPosition().x+=3.f;}
inline void __cdecl Load(bool priority){trace.push_back({"load",priority});activeEngine->GetPosition().y-=4.f;}
inline void __cdecl Time(uint32 minutes){trace.push_back({"time",minutes});}
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CTrain","SkipToNextAllowedStation",0x6F72F0,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"train-skip355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"train-skip355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("train-skip355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"train-skip355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch z(0x572300,Zone),zd(bit_cast<uintptr_t>(&CTheZones::GetLevelFromPosition),Zone);
 EntryPatch t(0x558E40,Stat),td(bit_cast<uintptr_t>(static_cast<float(*)(eStats)>(&CStats::GetStatValue)),Stat);
 EntryPatch a(0x6F5F00,Next),ad(bit_cast<uintptr_t>(&CTrain::FindNextStationPositionInDirection),Next);
 EntryPatch b(0x40EB70,Scene),bd(bit_cast<uintptr_t>(&CStreaming::LoadScene),Scene);
 EntryPatch c(0x40EA10,Load),cd(bit_cast<uintptr_t>(&CStreaming::LoadAllRequestedModels),Load);
 EntryPatch d(0x4414C0,Time),dd(bit_cast<uintptr_t>(&CGameLogic::PassTime),Time);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<std::array<uint8,sizeof(CTrain)>,8> objects{};alignas(16) std::array<uint8,0x80> matrix{};
   auto run=[&](bool candidate){scenario=i;stationCalls=0;trace=json::array();memset(objects.data(),0xCD,sizeof(objects));memset(matrix.data(),0,matrix.size());const auto length=1+i%8;
    for(int t=0;t<8;++t){auto* train=reinterpret_cast<CTrain*>(objects[t].data());train->m_pNextCarriage=t+1<length?reinterpret_cast<CTrain*>(objects[t+1].data()):nullptr;train->m_pPrevCarriage=t>0?reinterpret_cast<CTrain*>(objects[t-1].data()):nullptr;train->trainFlags.bClockwiseDirection=i%2;train->m_fCurrentRailDistance=float(i%101-5)*61.f;*reinterpret_cast<uintptr_t*>(objects[t].data()+0x14)=0;*reinterpret_cast<CVector*>(objects[t].data()+4)=CVector{float(i%331-165),float(i%241-120),float(i%19)};}
    activeEngine=reinterpret_cast<CTrain*>(objects[0].data());if((i/8)%2){*reinterpret_cast<uintptr_t*>(objects[0].data()+0x14)=uintptr_t(matrix.data());*reinterpret_cast<CVector*>(matrix.data()+0x30)=CVector{float(i%331-165),float(i%241-120),float(i%19)};}
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__cdecl*)(CTrain*)>(fn.address)(reinterpret_cast<CTrain*>(objects[(i/16)%length].data()));
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"objects",Hash(objects.data(),sizeof(objects))},{"matrix",Hash(matrix.data(),matrix.size())},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 bool restored=true;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}

