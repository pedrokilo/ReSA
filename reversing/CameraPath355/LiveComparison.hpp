#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Camera.h"
#include "FileLoader.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace CameraPath355Live {
inline json trace;inline uint32 scenario{};
inline std::array<std::array<uint8,256>,4> allocations{};inline std::array<uint32,4> allocatedSizes{};inline uint32 allocationCount{},lineIndex{},lineCount{};
inline std::array<std::array<char,192>,64> lines{};inline std::array<uint32,64> calls{};inline uint32 callCount{};
inline void __fastcall Delete(CCamera* cam,void*){calls[callCount++]=0xD;for(auto& path:cam->m_aPathArray)path.m_pArrPathData=nullptr;}
inline char* __cdecl Line(FILE* file){calls[callCount++]=0x100+lineIndex;return lineIndex<lineCount?lines[lineIndex++].data():nullptr;}
inline void* __cdecl New(size_t n){calls[callCount++]=uint32(0x200+n);allocatedSizes[allocationCount]=uint32(n);return allocations[allocationCount++].data();}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCamera","LoadPathSplines",0x5B24D0,0,-1,1024}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"camera-path355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"camera-path355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("camera-path355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x5B24A0,Delete),ad(bit_cast<uintptr_t>(&CCamera::DeleteCutSceneCamDataMemory),Delete);
 EntryPatch b(0x536F80,Line),bd(bit_cast<uintptr_t>(static_cast<char*(*)(FILE*)>(&CFileLoader::LoadLine<FILE*>)),Line);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CCamera) std::array<uint8,sizeof(CCamera)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCamera*>(raw.data());
    for(auto& data:allocations)data.fill(0xCD);allocatedSizes.fill(0);allocationCount=lineIndex=lineCount=callCount=0;calls.fill(0);
    auto add=[&](const char* text){std::strcpy(lines[lineCount++].data(),text);};
    const int blocks=i%6;
    add("# fixture");add("");
    for(int k=0;k<blocks;++k){
      const int count=(i/6+k)%4;char countText[16];std::sprintf(countText,"%d",count);add(countText);
      const int components=k<2?4:10;
      for(int point=0;point<count;++point){char text[192]{};size_t used=0;for(int j=0;j<components;++j)used+=std::sprintf(text+used,"%s%.6f",j?(j%3==0?",":" "):"",float(int32(i%17)-8)*.125f+float(j+point));add(text);}
      if((i/24)%4!=0)add("ignored until semicolon");add(";");add("# separator");
    }
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    {EntryPatch allocator(0x82119A,New);reinterpret_cast<void(__thiscall*)(CCamera*,FILE*)>(fn.address)(self,reinterpret_cast<FILE*>(0x1234));}
    json pathPointers=json::array();for(const auto& path:self->m_aPathArray){pathPointers.push_back(path.m_pArrPathData?int((uintptr_t(path.m_pArrPathData)-uintptr_t(allocations.data()))/256):-1);}
    trace=json::array();for(uint32 j=0;j<callCount;++j)trace.push_back(calls[j]);
    return json{{"camera",Digest(raw.data(),raw.size())},{"allocations",Digest(allocations.data(),sizeof(allocations))},{"sizes",allocatedSizes},{"pointers",pathPointers},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
