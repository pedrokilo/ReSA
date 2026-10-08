#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "GenericGameStorage.h"
#include "C_PcSave.h"
#include "FileMgr.h"
#include "../ProbeRuntime.hpp"
namespace StorageCore355Live {
alignas(16) inline std::array<uint8,51201> buffer{};
inline uint32 scenario{},newCalls{},deleteCalls{},requested{};
inline json trace;
inline FILE* __cdecl Open(const char* path,const char* mode){trace.push_back({"open",path,mode});return scenario%3==0?nullptr:reinterpret_cast<FILE*>(0x123400);}
inline int32 __cdecl Close(FILE* file){trace.push_back({"close",uintptr_t(file)});return int32(scenario%3)-1;}
inline int32 __cdecl Total(FILE* file){trace.push_back({"size",uintptr_t(file)});return 100+int32(scenario)*13;}
inline void* __cdecl NewArray(size_t n){++newCalls;requested=uint32(n);return buffer.data();}
inline void __cdecl DeleteArray(void*){++deleteCalls;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,8> fns{{
{"CGenericGameStorage","GetCurrentVersionNumber",0x5D0F50,0,-1,512},
{"CGenericGameStorage","MakeValidSaveName",0x5D0E90,0,-1,512},
{"CGenericGameStorage","CloseFile",0x5D0E30,0,-1,512},
{"CGenericGameStorage","OpenFileForWriting",0x5D0DD0,0,-1,512},
{"CGenericGameStorage","OpenFileForReading",0x5D0D20,0,-1,512},
{"CGenericGameStorage","GetSavedGameDateAndTime",0x618D00,0,-1,512},
{"CGenericGameStorage","InitRadioStationPositionList",0x618E70,0,-1,512},
{"CGenericGameStorage","RestoreForStartLoad",0x619000,0,-1,512},
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"storage-core355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"storage-core355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("storage-core355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 std::array<uint8,0xC17038-0xC15F60> saved{};memcpy(saved.data(),reinterpret_cast<void*>(0xC15F60),saved.size());const auto savedWorkSize=CGenericGameStorage::ms_WorkBufferSize;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x538900,Open),ad(bit_cast<uintptr_t>(&CFileMgr::OpenFile),Open);
 EntryPatch b(0x5389D0,Close),bd(bit_cast<uintptr_t>(&CFileMgr::CloseFile),Close);
 EntryPatch c(0x5389E0,Total),cd(bit_cast<uintptr_t>(&CFileMgr::GetTotalSize),Total);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);
    scenario=uint32(i);newCalls=deleteCalls=requested=0;trace=json::array();
    memcpy(reinterpret_cast<void*>(0xC15F60),saved.data(),saved.size());CGenericGameStorage::ms_WorkBufferSize=savedWorkSize;
    memset(CGenericGameStorage::ms_LoadFileName,0xCC,sizeof(CGenericGameStorage::ms_LoadFileName));memset(CGenericGameStorage::ms_LoadFileNameWithPath,0xCC,sizeof(CGenericGameStorage::ms_LoadFileNameWithPath));memset(CGenericGameStorage::ms_SaveFileName,0xCC,sizeof(CGenericGameStorage::ms_SaveFileName));
    std::strcpy(C_PcSave::DefaultPCSaveFileName,i%4==0?"fixture?\n?after":"fixture?path");std::strcpy(CGenericGameStorage::ms_SaveFileName,"fixture_save");std::strcpy(CGenericGameStorage::ms_LoadFileName,"fixture_load");
    CGenericGameStorage::ms_WorkBuffer=i%2?buffer.data():nullptr;CGenericGameStorage::ms_FileHandle=reinterpret_cast<FILE*>(0x765400);CGenericGameStorage::ms_WorkBufferPos=17;CGenericGameStorage::ms_FilePos=23;CGenericGameStorage::ms_FileSize=51;CGenericGameStorage::ms_bFailed=i%2!=0;s_PcSaveHelper.error=C_PcSave::eErrorCode::NONE;
    uintptr_t result=0;const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    {
      EntryPatch alloc(0x821195,NewArray),free(0x8213AE,DeleteArray);
      if(fn.address==0x5D0F50)result=reinterpret_cast<uint32(__cdecl*)()>(fn.address)();
      else if(fn.address==0x5D0E90)reinterpret_cast<void(__cdecl*)(int32)>(fn.address)(i%8);
      else if(fn.address==0x5D0E30||fn.address==0x5D0DD0||fn.address==0x619000)result=reinterpret_cast<bool(__cdecl*)()>(fn.address)();
      else if(fn.address==0x5D0D20)result=reinterpret_cast<bool(__cdecl*)(const char*,int32)>(fn.address)(i%3==0?nullptr:"input_file",i%8);
      else if(fn.address==0x618D00)result=uintptr_t(reinterpret_cast<const GxtChar*(__cdecl*)(int32)>(fn.address)(i%8));
      else reinterpret_cast<void(__cdecl*)()>(fn.address)();
    }
    return json{{"load",ReversingProbe::Bytes(CGenericGameStorage::ms_LoadFileName,sizeof(CGenericGameStorage::ms_LoadFileName))},{"loadpath",ReversingProbe::Bytes(CGenericGameStorage::ms_LoadFileNameWithPath,sizeof(CGenericGameStorage::ms_LoadFileNameWithPath))},{"save",ReversingProbe::Bytes(CGenericGameStorage::ms_SaveFileName,sizeof(CGenericGameStorage::ms_SaveFileName))},{"globals",ReversingProbe::Bytes(reinterpret_cast<void*>(0xC16EE8),0x16)},{"size",CGenericGameStorage::ms_WorkBufferSize},{"error",int(s_PcSaveHelper.error)},{"alloc",newCalls},{"free",deleteCalls},{"requested",requested},{"result",result},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 memcpy(reinterpret_cast<void*>(0xC15F60),saved.data(),saved.size());CGenericGameStorage::ms_WorkBufferSize=savedWorkSize;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
