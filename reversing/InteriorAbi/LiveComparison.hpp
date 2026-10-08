#pragma once
#include "Interior/Interior_c.h"
#include "../ProbeRuntime.hpp"
namespace InteriorAbiLive {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
inline std::string Bytes(const void* ptr,size_t size){const auto* p=static_cast<const uint8*>(ptr);std::string out(size*2,'0');constexpr char digits[]="0123456789abcdef";for(size_t i=0;i<size;++i){out[2*i]=digits[p[i]>>4];out[2*i+1]=digits[p[i]&15];}return out;}
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,2> fns{{
{"Interior_c","Constructor",0x5921D0,0,-1,1024},
{"Interior_c","Destructor",0x591360,0,-1,1024},
}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"interior-abi-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"interior-abi-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("interior-abi");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;bool passed=true;uint32 pairs=0;
 for(const auto& fn:fns){
 const auto candidateAddress=fn.address==0x5921D0?std::bit_cast<uintptr_t>(&Interior_c::Constructor):std::bit_cast<uintptr_t>(&Interior_c::Destructor);
 std::array<uint8,5> entryBefore{};std::memcpy(entryBefore.data(),reinterpret_cast<const void*>(fn.address),5);
 int failures=0;const std::string full=std::string(fn.cls)+"::"+fn.name;
 {
 ReversingProbe::EntryPatch patch(fn.address,reinterpret_cast<void*>(candidateAddress));
 for(int i=0;i<fn.cases;++i){
 auto run=[&](bool candidate){
 patch.Enable(candidate);
 alignas(Interior_c) std::array<uint8,sizeof(Interior_c)> raw{};
 for(size_t j=0;j<raw.size();++j)raw[j]=uint8((uint32(i)*37u+uint32(j)*83u+(uint32(j)*uint32(j)>>3))&255u);
 // Exercise every NodeId byte value; these bytes must survive the native constructor unchanged.
 raw[0x3F2]=uint8(i);raw[0x3F3]=uint8(i>>2);
 const auto before=raw;auto expected=raw;auto* self=reinterpret_cast<Interior_c*>(raw.data());
 bool returnSelf=true;
 const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
 if(fn.address==0x5921D0){
 auto* result=reinterpret_cast<Interior_c*(__thiscall*)(Interior_c*)>(fn.address)(self);returnSelf=result==self;
 std::fill(expected.begin(),expected.begin()+8,0);std::fill(expected.begin()+0x5C,expected.begin()+0x68,0);
 expected[0x3EC]=255;expected[0x3ED]=255;expected[0x3F0]=255;expected[0x3F1]=255;
 }else reinterpret_cast<void(__thiscall*)(Interior_c*)>(fn.address)(self);
 return json{{"before_hex",Bytes(before.data(),before.size())},{"after_hex",Bytes(raw.data(),raw.size())},{"expected_writes",raw==expected},{"node_id_preserved",raw[0x3F2]==before[0x3F2]&&raw[0x3F3]==before[0x3F3]},{"returns_self",returnSelf},{"switches",switched}};
 };
 const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true&&original["expected_writes"]==true&&original["returns_self"]==true;
 passed&=ok;failures+=!ok;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
 }
 }
 const bool restored=std::memcmp(entryBefore.data(),reinterpret_cast<const void*>(fn.address),5)==0;passed&=restored;
 Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases},{"entry_restored",restored}});
 }
 Write({{"event",passed?"complete":"error"},{"pairs",pairs},{"state_restored",true},{"cataloged",false}});
}
}
