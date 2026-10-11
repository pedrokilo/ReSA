#pragma once
#include <fstream>
#include <set>
#include "PathFind.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesWorld13Live {
struct Fn{const char* cls;const char* name;uintptr_t address,vtable;int slot,cases;};
inline constexpr Fn fns[]{
{"CPathFind","RemoveInteriorLinks",0x44DF60,0,-1,2048}
};
inline bool done{};inline std::ofstream log;
inline void Write(const json& r);
struct Result{uint32 eax,ecx,edx;bool abi;};
__declspec(noinline) inline Result Call(uintptr_t entry,uint32 node,uint32 seed){
 uint32 before{},after{},rb{},rs{},rd{},fb{},fa{},eaxCaptured{},ecxCaptured{},edxCaptured{};
 __asm {
  push ebx
  push esi
  push edi
  mov before,esp
  mov fb,ebp
  mov ebx,13579BDFh
  mov esi,2468ACE0h
  mov edi,10293847h
  push node
  mov eax,seed
  mov ecx,96F050h
  call entry
  mov eaxCaptured,eax
  mov ecxCaptured,ecx
  mov edxCaptured,edx
  mov after,esp
  mov fa,ebp
  mov rb,ebx
  mov rs,esi
  mov rd,edi
  pop edi
  pop esi
  pop ebx
 }
 return {eaxCaptured,ecxCaptured,edxCaptured,before==after&&fb==fa&&rb==0x13579BDF&&rs==0x2468ACE0&&rd==0x10293847};
}
inline uint16 Top(){uint16 v{};__asm{fnstsw v}return v&0x3800;}
inline void SaveFP(void* p){__asm{mov eax,p}__asm{fnsave [eax]}__asm{frstor [eax]}}
inline void RestoreFP(void* p){__asm{mov eax,p}__asm{frstor [eax]}}
inline uint32 ReadMXCSR(){uint32 v{};__asm{stmxcsr v}return v;}
inline void WriteMXCSR(uint32 v){__asm{ldmxcsr v}}
alignas(16) inline std::array<uint8,108> canonicalFP{};inline uint32 canonicalMXCSR{};inline bool canonicalActive{};
inline void RestoreCanonical(){if(canonicalActive){RestoreFP(canonicalFP.data());WriteMXCSR(canonicalMXCSR);}}
inline bool CanonicalRestored(){alignas(16)std::array<uint8,108> current{};SaveFP(current.data());return current==canonicalFP&&ReadMXCSR()==canonicalMXCSR;}
inline void Write(const json& r){alignas(16)std::array<uint8,108> current{};const auto mx=ReadMXCSR();if(!canonicalActive)SaveFP(current.data());RestoreCanonical();ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();if(canonicalActive)RestoreCanonical();else{RestoreFP(current.data());WriteMXCSR(mx);}}
inline uintptr_t Resolve(uintptr_t e){for(int n=0;n<16&&*reinterpret_cast<const uint8*>(e)==0xE9;++n)e+=5+*reinterpret_cast<const int32*>(e+1);return e;}
inline bool ModuleIs(uintptr_t e,HMODULE expected){HMODULE owner{};const DWORD flags=GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;return GetModuleHandleExA(flags,reinterpret_cast<LPCSTR>(e),&owner)&&owner==expected;}
inline HMODULE ProbeModule(){HMODULE p{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&Call),&p);return p;}
using Table=std::array<std::array<int8,6>,64>;
inline Table Graph(uint32 count,uint32 mode){Table rows{};for(auto& r:rows)r.fill(-1);auto edge=[&](uint32 a,uint32 b){if(a==b||a>=count||b>=count)return;auto put=[&](uint32 from,uint32 to){auto& row=rows[from];if(std::find(row.begin(),row.end(),int8(to))==row.end()){auto free=std::find(row.begin(),row.end(),int8(-1));if(free!=row.end())*free=int8(to);}};put(a,b);put(b,a);};
 switch(mode){case 0:break;case 1:case 7:for(uint32 i=1;i<count;++i)edge(i-1,i);break;case 2:for(uint32 i=0;i<count;++i)edge(i,(i+1)%count);break;case 3:for(uint32 i=0;i<count;++i)for(uint32 d=1;d<=3;++d)edge(i,(i+d)%count);break;case 4:for(uint32 i=1;i<count;i+=2)edge(i-1,i);break;case 5:for(uint32 i=1;i<std::min(count,7u);++i)edge(0,i);break;case 6:for(uint32 i=0;i+2<count;i+=3){edge(i,i+1);edge(i+1,i+2);edge(i+2,i);}break;}
 // Rotation moves holes without changing the legal undirected adjacency graph.
 if(mode==7)for(uint32 i=0;i<count;++i)std::rotate(rows[i].begin(),rows[i].begin()+int(i%6),rows[i].end());return rows;}
inline bool Legal(const Table& rows,uint32 count){if(count>64)return false;for(uint32 i=0;i<64;++i)for(auto v:rows[i]){if(v==-1)continue;if(i>=count||v<0||uint32(v)>=count||uint32(v)==i||std::find(rows[uint32(v)].begin(),rows[uint32(v)].end(),int8(i))==rows[uint32(v)].end())return false;}return true;}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"dependenciesworld13-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependenciesworld13-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesworld13");}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;SaveFP(canonicalFP.data());canonicalMXCSR=ReadMXCSR();canonicalActive=true;
 static_assert(sizeof(Table)==384);const auto globalsSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0x96EAB8),0x4D4),pathsSaved=ReversingProbe::Bytes(&ThePaths,sizeof(ThePaths));const auto guardLeft=ReversingProbe::Bytes(reinterpret_cast<void*>(0x96EAB8),8),guardRight=ReversingProbe::Bytes(reinterpret_cast<void*>(0x96EC40),8),countGuards=ReversingProbe::Bytes(reinterpret_cast<void*>(0x96EF7C),16);
 OriginalDependencies499::HookRoute route("CPathFind","RemoveInteriorLinks",0x44DF60,FunctionToVoidPtr(&CPathFind::RemoveInteriorLinks));RestoreCanonical();Write({{"event","begin"},{"registered_hook",route.Registered()},{"real_global_table",0x96EAC0},{"real_count_global",0x96EF80},{"table_capacity",64},{"slots_per_row",6},{"collaborators_stubbed",false},{"fp_state_restored",CanonicalRestored()}});
 bool passed=true;uint32 failures=0;std::array<uint32,65> counts{};std::array<uint32,8> modes{};const std::array<uint32,8> seeds{0,0xFF,0x13579BDF,0x2468ACE0,0xFFFFFFFF,0x80000000,0x01020304,0x7FFFFFFF};
 for(int scenario=0;scenario<2048;++scenario){const uint32 count=uint32(scenario)%65,mode=uint32(scenario/65)%8,node=count?uint32(scenario/8)%count:0,seed=seeds[scenario%8];const auto input=Graph(count,mode);auto expected=input;for(uint32 r=0;r<count;++r)for(auto& link:expected[r])if(r==node||link==int32(node))link=-1;
 auto run=[&](bool candidate){RestoreCanonical();std::memcpy(reinterpret_cast<void*>(0x96EAC0),&input,sizeof(input));*reinterpret_cast<uint32*>(0x96EF80)=count;bool routeOK=false;const auto result=route.Invoke(candidate,[&]{routeOK=ReversingProbe::JumpsToDll(0x44DF60)==candidate;if(candidate)routeOK&=Resolve(0x44DF60)==route.Body()&&ModuleIs(route.Body(),ProbeModule());return Call(0x44DF60,node,seed);});const auto output=ReversingProbe::Bytes(reinterpret_cast<void*>(0x96EAC0),sizeof(input));const bool guards=guardLeft==ReversingProbe::Bytes(reinterpret_cast<void*>(0x96EAB8),8)&&guardRight==ReversingProbe::Bytes(reinterpret_cast<void*>(0x96EC40),8)&&std::equal(countGuards.begin(),countGuards.begin()+4,reinterpret_cast<const uint8*>(0x96EF7C))&&std::equal(countGuards.begin()+8,countGuards.end(),reinterpret_cast<const uint8*>(0x96EF84));const bool tableOK=output==ReversingProbe::Bytes(&expected,sizeof(expected))&&*reinterpret_cast<uint32*>(0x96EF80)==count;const bool rawOK=result.edx==count&&result.eax==(count?6u:seed)&&result.ecx==(count?0x96EAC0+6*count:0x96F050);const bool objectOK=pathsSaved==ReversingProbe::Bytes(&ThePaths,sizeof(ThePaths));std::memcpy(reinterpret_cast<void*>(0x96EAB8),globalsSaved.data(),globalsSaved.size());RestoreCanonical();json value={{"return_bits",result.eax},{"ecx_after",result.ecx},{"edx_after",result.edx},{"count",count},{"target_node",node},{"graph_mode",mode},{"eax_seed",seed},{"table",output},{"route_ok",routeOK},{"stack_restored",result.abi},{"registers_preserved",result.abi},{"contract_ok",Legal(input,count)&&tableOK&&rawOK},{"readonly_object_ok",objectOK},{"guards_ok",guards},{"state_restored",globalsSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0x96EAB8),globalsSaved.size())},{"fp_state_restored",CanonicalRestored()}};RestoreCanonical();return value;};
 const auto original=run(false),candidate=run(true);bool equal=route.Registered()&&original==candidate;for(auto k:{"route_ok","stack_restored","registers_preserved","contract_ok","readonly_object_ok","guards_ok","state_restored","fp_state_restored"})equal&=original[k]==true;passed&=equal;failures+=!equal;++counts[count];++modes[mode];Write({{"event","comparison"},{"pair",scenario},{"case",scenario},{"function","CPathFind::RemoveInteriorLinks"},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});}
 bool coverage=true;for(auto n:counts)coverage&=n>0;for(auto n:modes)coverage&=n>0;passed&=coverage;Write({{"event","function_summary"},{"function","CPathFind::RemoveInteriorLinks"},{"cases",2048},{"failures",failures},{"complete_paths",coverage},{"count_cases",counts},{"mode_cases",modes}});const bool mainRestore=route.Restore();std::memcpy(reinterpret_cast<void*>(0x96EAB8),globalsSaved.data(),globalsSaved.size());RestoreCanonical();const bool fp=CanonicalRestored(),restored=mainRestore&&globalsSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0x96EAB8),globalsSaved.size())&&pathsSaved==ReversingProbe::Bytes(&ThePaths,sizeof(ThePaths))&&fp;Write({{"event",passed&&restored?"complete":"error"},{"pairs",2048},{"state_restored",restored},{"fp_state_restored",fp}});
}
}
