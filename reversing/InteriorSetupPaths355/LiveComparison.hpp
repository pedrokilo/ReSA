#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Interior/InteriorGroup_c.h"
#include "Interior/Interior_c.h"
#include "PathFind.h"
#include "BaseModelInfo.h"
#include "ColModel.h"
#include "../ProbeRuntime.hpp"
namespace InteriorSetupPaths355Live {
inline json trace; inline int testCase{}, nextNode{};
inline int8 __fastcall Loaded(InteriorGroup_c*,void*) { trace.push_back({"loaded"}); return int8(testCase%11!=0); }
inline void __fastcall Start(CPathFind*,void*,int32 id) {trace.push_back({"start",id});nextNode=0;}
inline CNodeAddress* __fastcall Add(CPathFind*,void*,CNodeAddress* out,float x,float y,float z,bool dont,int8 c0,int8 c1,int8 c2,int8 c3,int8 c4,int8 c5){trace.push_back({"add",std::bit_cast<uint32>(x),std::bit_cast<uint32>(y),std::bit_cast<uint32>(z),dont,c0,c1,c2,c3,c4,c5});*out=CNodeAddress{64,uint16(nextNode++)};return out;}
inline void __fastcall Link(CPathFind*,void*,int32 a,int32 b){trace.push_back({"link",a,b});}
inline CNodeAddress* __fastcall Find(CPathFind*,void*,CNodeAddress* out,int32 node){trace.push_back({"find",node});*out=(node+testCase)%3?CNodeAddress{0,uint16(node%4)}:CNodeAddress{};return out;}
inline void __fastcall External(CPathFind*,void*,int32 node,CNodeAddress address){trace.push_back({"external",node,address.m_wAreaId,address.m_wNodeId});}
inline void __fastcall Remove(CPathFind*,void*,int32 node){trace.push_back({"remove",node});}
inline void __fastcall Complete(CPathFind*,void*,CNodeAddress* out){trace.push_back({"complete",uintptr_t(out)});}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"InteriorGroup_c","SetupPaths",0x595590,0,-1,4096}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"interior-setup-paths355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"interior-setup-paths355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("interior-setup-paths355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedNodes=StaticRef<uintptr_t>(0x96F854);
 EntryPatch l(0x595380,Loaded),ld(bit_cast<uintptr_t>(&InteriorGroup_c::ArePathsLoaded),Loaded);
 EntryPatch st(0x44DE80,Start),stdll(bit_cast<uintptr_t>(&CPathFind::StartNewInterior),Start);
 EntryPatch ad(0x450E90,Add),addll(bit_cast<uintptr_t>(&CPathFind::AddNodeToNewInterior),Add);
 EntryPatch li(0x44DED0,Link),lidll(bit_cast<uintptr_t>(&CPathFind::AddInteriorLink),Link);
 EntryPatch fi(0x450F30,Find),fidll(bit_cast<uintptr_t>(&CPathFind::FindNearestExteriorNodeToInteriorNode),Find);
 EntryPatch ex(0x44DF30,External),exdll(bit_cast<uintptr_t>(&CPathFind::AddInteriorLinkToExternalNode),External);
 EntryPatch re(0x44DF60,Remove),co(0x452270,Complete);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);
    alignas(InteriorGroup_c) std::array<uint8,sizeof(InteriorGroup_c)> raw{};auto* self=reinterpret_cast<InteriorGroup_c*>(raw.data());
    alignas(Interior_c) std::array<uint8,sizeof(Interior_c)> interiors[2]{};
    std::array<uint8,32> effects[2]{}; alignas(4) std::array<uint8,4*0x1C> nodes{};
    self->m_id=uint8(i%256);self->m_groupType=uint8((i/7)%3);self->m_pathSetupComplete=int8(i%3);
    StaticRef<uintptr_t>(0x96F854)=uintptr_t(nodes.data());
    for(int n=0;n<4;++n){At<int16>(nodes.data(),n*0x1C+8)=int16((i%9+n)*8);At<int16>(nodes.data(),n*0x1C+10)=int16((i%7-n)*8);At<int16>(nodes.data(),n*0x1C+12)=int16(n*8);}
    constexpr size_t enable[]{5,6,10,8};
    for(int n=0;n<2;++n){auto* d=interiors[n].data();self->m_interiors[n*3]=reinterpret_cast<Interior_c*>(d);At<uintptr_t>(d,0x14)=uintptr_t(effects[n].data());
      const int count=(i/17+n)%7;At<int8>(d,0x40C)=int8(count==1||count==2?0:count);
      for(int g=0;g<6;++g)At<CVector>(d,0x414+g*16)=CVector{float(g+n),float(g-n),float(i%5)};
      for(int k=0;k<4;++k){At<int8>(effects[n].data(),enable[k])=(i&(1<<(k+n)))?int8(k):int8(-1);const size_t base=0x510+k*0x20;
       At<int8>(d,base+2)=int8((i+k)%4-2);At<int8>(d,base+3)=int8((i+n)%4-2);
       At<CVector>(d,base+4)=CVector{float(k+n),float(i%7),float(n)};
       At<CVector>(d,base+0x14)=CVector{float(k+n)+float(i%3)*.5f,float(i%7)+float(n)*.5f,float(n)};
      }
    }
    if(i%13==0)self->m_interiors[3]=nullptr;
    testCase=i;nextNode=0;trace=json::array();const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    reinterpret_cast<void(__thiscall*)(InteriorGroup_c*)>(fn.address)(self);
    return json{{"group",Digest(raw.data(),raw.size())},{"interior0",Digest(interiors[0].data(),interiors[0].size())},{"interior1",Digest(interiors[1].data(),interiors[1].size())},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<uintptr_t>(0x96F854)=savedNodes;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
