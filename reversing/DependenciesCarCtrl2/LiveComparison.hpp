#pragma once
#include <fstream>
#include "CarCtrl.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesCarCtrl2Live {
inline bool done{};
inline std::ofstream log;
struct Fn { const char* cls; const char* name; uintptr_t address,vtable; int slot,cases; };
inline constexpr Fn fns[]{
{"CCarCtrl","ClearInterestingVehicleList",0x423F00,0,-1,256},
{"CCarCtrl","RegisterVehicleOfInterest",0x423DE0,0,-1,2048},
{"CCarCtrl","FindSequenceElement",0x421770,0,-1,2048}};
inline void Write(const json& row) { ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush(); }
__declspec(noinline) inline int32 Call(uintptr_t entry,uint32 argument,bool hasArgument,bool preserveVolatile,bool& abi) {
 uint32 before{},after{},b{},s{},d{},c{},x{};int32 result{};
 __asm {
  push ebx
  push esi
  push edi
  mov before,esp
  mov ebx,13579BDFh
  mov esi,2468ACE0h
  mov edi,10293847h
  mov ecx,13572468h
  mov edx,24681357h
  cmp hasArgument,0
  je noArgument
  push argument
 noArgument:
  call entry
  mov result,eax
  mov c,ecx
  mov x,edx
  cmp hasArgument,0
  je cleanDone
  add esp,4
 cleanDone:
  mov after,esp
  mov b,ebx
  mov s,esi
  mov d,edi
  pop edi
  pop esi
  pop ebx
 }
 abi=before==after&&b==0x13579BDF&&s==0x2468ACE0&&d==0x10293847&&(!preserveVolatile||(c==0x13572468&&x==0x24681357));
 return result;
}
inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"depscarctrl2-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"depscarctrl2-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("depscarctrl2");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
 done=true;
 using OriginalDependencies499::HookRoute;
 HookRoute clear("CCarCtrl","ClearInterestingVehicleList",0x423F00,&CCarCtrl::ClearInterestingVehicleList);
 HookRoute interest("CCarCtrl","RegisterVehicleOfInterest",0x423DE0,&CCarCtrl::RegisterVehicleOfInterest);
 HookRoute sequence("CCarCtrl","FindSequenceElement",0x421770,&CCarCtrl::FindSequenceElement);
 auto* region=reinterpret_cast<uint8*>(0x969070);
 const auto saved=ReversingProbe::Bytes(region,28);
 auto& time=*reinterpret_cast<uint32*>(0xB7CB84);const auto savedTime=time;
 Write({{"event","begin"},{"registered_hook",clear.Registered()&&interest.Registered()&&sequence.Registered()}});
 unsigned pair=0,failures=0;
 for(unsigned f=0;f<3;++f){unsigned failed=0;auto& route=f==0?clear:f==1?interest:sequence;
 for(unsigned i=0;i<unsigned(fns[f].cases);++i){
 auto run=[&](bool candidate){
  for(unsigned j=0;j<28;++j)region[j]=uint8(0xA7u+j*13u+i);
  time=0x9E3779B9u*i;
  int32 arg{};uint32 needle=0x12345000u;
  if(f==1){
   const unsigned mask=i&3,choice=(i>>2)&3,t=(i>>4)&7;
   const uint32 values[]{0u,1u,2u,0x7FFFFFFFu,0x80000000u,0xFFFFFFFEu,0xFFFFFFFFu,100u};
   auto* slots=reinterpret_cast<uint32*>(region+20);auto* stamps=reinterpret_cast<uint32*>(region+12);
   slots[0]=(mask&1)?0x12345000u:0;slots[1]=(mask&2)?((i&256)?0x12345000u:0x12346000u):0;
   stamps[0]=values[t];stamps[1]=values[(i>>7)&7];
   needle=choice==0?0u:choice==1?0x12345000u:choice==2?0x12346000u:0x12347000u;
  }else if(f==2){
   const int32 lengths[]{1,2,3,4,7,8,31,256};const unsigned mode=(i>>8)&7;
   const int32 length=lengths[(i>>2)&7];const int32 offset=int32((i>>5)%unsigned(length));
   region[0]=(i&3)==0?0:(i&3)==1?1:(i&3)==2?0x80:0xFF;
   std::memcpy(region+4,&offset,4);std::memcpy(region+8,&length,4);
   const int32 args[]{-1,0,1,length-1,length,length+1,INT32_MIN+512,INT32_MAX-512};arg=args[mode];
  }
  const auto before=ReversingProbe::Bytes(region,28);const auto timerBefore=time;
  int32 ret{};bool routeOk{},abi{};
  route.Invoke(candidate,[&]{routeOk=ReversingProbe::JumpsToDll(fns[f].address)==candidate;
   const auto value=Call(fns[f].address,f==1?needle:uint32(arg),f!=0,f==0,abi);
   if(f==2)ret=value;
   return true;
  });
  const auto after=ReversingProbe::Bytes(region,28);
  const bool guards=f==0?std::equal(before.begin(),before.begin()+20,after.begin()):f==1?std::equal(before.begin(),before.begin()+12,after.begin()):before==after;
  return json{{"return",ret},{"world",after},{"route_ok",routeOk},{"guards_ok",guards},{"timer_unchanged",time==timerBefore},{"abi_ok",abi}};
 };
 const auto original=run(false),candidate=run(true);
 const bool equal=original==candidate&&original["route_ok"]==true&&original["guards_ok"]==true&&original["timer_unchanged"]==true&&original["abi_ok"]==true;
 failed+=!equal;Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fns[f].cls)+"::"+fns[f].name},{"case",i},{"accepted",equal},{"equal",equal},{"original",original},{"candidate",candidate}});
 }
 failures+=failed;Write({{"event","function_summary"},{"function",std::string(fns[f].cls)+"::"+fns[f].name},{"cases",fns[f].cases},{"failures",failed}});
 }
 std::memcpy(region,saved.data(),saved.size());time=savedTime;
 const bool state=saved==ReversingProbe::Bytes(region,28)&&time==savedTime;
 const bool c=clear.Restore(),v=interest.Restore(),s=sequence.Restore();const bool restored=state&&c&&v&&s;
 Write({{"event",!failures&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
