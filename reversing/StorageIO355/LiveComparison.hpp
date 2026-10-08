#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "GenericGameStorage.h"
#include "C_PcSave.h"
#include "FileMgr.h"
#include "Streaming.h"
#include "../ProbeRuntime.hpp"
namespace StorageIO355Live {
alignas(16) inline std::array<uint8,52000> buffer{};
inline uint32 scenario{},newCalls{},deleteCalls{},requested{},loads{};
inline json trace;
inline uint64_t Digest(const void* ptr,size_t n){uint64_t h=14695981039346656037ull;for(size_t i=0;i<n;++i)h=(h^static_cast<const uint8*>(ptr)[i])*1099511628211ull;return h;}
inline int32 __cdecl Close(FILE* file){trace.push_back({"close",uintptr_t(file)});return scenario%7==6?-1:0;}
inline void __cdecl DeleteArray(void*){++deleteCalls;}
inline size_t __cdecl Read(FILE* file,void* out,size_t n){trace.push_back({"read",uintptr_t(file),n});for(size_t j=0;j<n;++j)static_cast<uint8*>(out)[j]=uint8(j+scenario);return scenario%7==4?n-1:n;}
inline size_t __cdecl WriteFile(FILE* file,const void* src,size_t n){trace.push_back({"write",uintptr_t(file),n,Digest(src,n)});return scenario%7==4?n-1:n;}
inline int32 __cdecl Error(FILE* file){trace.push_back({"error",uintptr_t(file)});return scenario%7==5?32:0;}
inline bool __cdecl OpenRead(const char* path,int32 slot){trace.push_back({"open",path?path:"<null>",slot});return scenario%5!=0;}
inline bool __cdecl Load(){trace.push_back({"load",loads});if(loads++>=scenario%3+1)return false;return true;}
inline bool __cdecl Check(int32 slot,const char* path){trace.push_back({"check",slot,path});return scenario%2!=0;}
inline void __fastcall Generate(C_PcSave*,void*,int32 slot,char* out){std::sprintf(out,"fixture%d.b",slot);trace.push_back({"generate",slot,out});}
inline void __cdecl DeleteRw(){trace.push_back({"delete_rw"});}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,4> fns{{
{"CGenericGameStorage","LoadWorkBuffer",0x5D10B0,0,-1,1024},
{"CGenericGameStorage","SaveWorkBuffer",0x5D0F80,0,-1,1024},
{"CGenericGameStorage","CheckSlotDataValid",0x5D1380,0,-1,1024},
{"CGenericGameStorage","CheckDataNotCorrupt",0x5D1170,0,-1,1024},
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"storage-io355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"storage-io355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("storage-io355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 std::array<uint8,0xC17038-0xC15F60> saved{};memcpy(saved.data(),reinterpret_cast<void*>(0xC15F60),saved.size());const auto savedWorkSize=CGenericGameStorage::ms_WorkBufferSize;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x538950,Read),ad(bit_cast<uintptr_t>(&CFileMgr::Read),Read);
 EntryPatch b(0x5389D0,Close),bd(bit_cast<uintptr_t>(&CFileMgr::CloseFile),Close);
 EntryPatch c(0x538970,WriteFile),cd(bit_cast<uintptr_t>(&CFileMgr::Write),WriteFile);
 EntryPatch e(0x538A50,Error),ed(bit_cast<uintptr_t>(&CFileMgr::GetErrorReadWrite),Error);
 EntryPatch g(0x6190A0,Generate),gd(bit_cast<uintptr_t>(&C_PcSave::GenerateGameFilename),Generate);
 EntryPatch rw(0x4090A0,DeleteRw),rwd(bit_cast<uintptr_t>(&CStreaming::DeleteAllRwObjects),DeleteRw);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);newCalls=deleteCalls=requested=loads=0;trace=json::array();
    memcpy(reinterpret_cast<void*>(0xC15F60),saved.data(),saved.size());CGenericGameStorage::ms_WorkBufferSize=savedWorkSize;
    for(size_t j=0;j<buffer.size();++j)buffer[j]=uint8(j*3+i);
    memset(CGenericGameStorage::ms_SaveFileNameJustSaved,0xCC,260);std::strcpy(CGenericGameStorage::ms_SaveFileName,"fixture_save.b");
    CGenericGameStorage::ms_WorkBuffer=buffer.data();CGenericGameStorage::ms_FileHandle=reinterpret_cast<FILE*>(0x765400);
    CGenericGameStorage::ms_WorkBufferSize=uint32(i%4==0?0:i%4==1?3:i%4==2?16:256);
    CGenericGameStorage::ms_WorkBufferPos=uint32(i%7==0?0:i%7<=3&&i%8==1?51200:16+i%17);
    CGenericGameStorage::ms_FilePos=uint32(i*4);CGenericGameStorage::ms_FileSize=CGenericGameStorage::ms_FilePos+std::array<uint32,6>{0,3,4,16,51200,51204}[i%6];
    CGenericGameStorage::ms_CheckSum=0xFFFFFF00u+uint32(i);CGenericGameStorage::ms_bFailed=i%13==0;s_PcSaveHelper.error=i%11==0?C_PcSave::eErrorCode::FAILED_TO_READ:C_PcSave::eErrorCode::NONE;
    if(fn.address==0x5D1170&&CGenericGameStorage::ms_WorkBufferSize>=4){uint32 sum=0;for(uint32 j=0;j<CGenericGameStorage::ms_WorkBufferSize-4;++j)sum+=buffer[j];sum*=i%3+1;if(i%2==0)++sum;memcpy(buffer.data()+CGenericGameStorage::ms_WorkBufferSize-4,&sum,4);}
    uintptr_t result=0;const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    {
      EntryPatch free(0x8213AE,DeleteArray);
      std::unique_ptr<EntryPatch> oa,ob,la,lb,ca,cb;
      if(fn.address==0x5D1170){oa=std::make_unique<EntryPatch>(0x5D0D20,OpenRead);ob=std::make_unique<EntryPatch>(bit_cast<uintptr_t>(&CGenericGameStorage::OpenFileForReading),OpenRead);la=std::make_unique<EntryPatch>(0x5D10B0,Load);lb=std::make_unique<EntryPatch>(bit_cast<uintptr_t>(&CGenericGameStorage::LoadWorkBuffer),Load);}
      if(fn.address==0x5D1380){ca=std::make_unique<EntryPatch>(0x5D1170,Check);cb=std::make_unique<EntryPatch>(bit_cast<uintptr_t>(&CGenericGameStorage::CheckDataNotCorrupt),Check);}
      if(fn.address==0x5D10B0)result=reinterpret_cast<bool(__cdecl*)()>(fn.address)();
      else if(fn.address==0x5D0F80)result=reinterpret_cast<bool(__cdecl*)(bool)>(fn.address)(i%2!=0);
      else if(fn.address==0x5D1380)result=reinterpret_cast<bool(__cdecl*)(int32)>(fn.address)(i%8);
      else result=reinterpret_cast<bool(__cdecl*)(int32,const char*)>(fn.address)(i%8,"fixture.b");
    }
    return json{{"save",ReversingProbe::Bytes(CGenericGameStorage::ms_SaveFileNameJustSaved,260)},{"globals",ReversingProbe::Bytes(reinterpret_cast<void*>(0xC16EE8),0x16)},{"size",CGenericGameStorage::ms_WorkBufferSize},{"checksum",CGenericGameStorage::ms_CheckSum},{"buffer",Digest(buffer.data(),buffer.size())},{"error",int(s_PcSaveHelper.error)},{"free",deleteCalls},{"result",result},{"trace",trace},{"switches",switched}};
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
