#pragma once
#include <fstream>
#include "AttractorScanner.h"
#include "PedIntelligence.h"
#include "MentalHealth.h"
#include "TaskSimpleFight.h"
#include "EventSource.h"
#include "EventDamage.h"
#include "References.h"
#include "Reference.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesTaskPed5Live {
struct Fn {const char* cls;const char* name;uintptr_t address;int vt,slot,cases;};
inline const Fn fns[]{
 {"CAttractorScanner","Clear",0x5FFF90,0,-1,2048},
 {"CPedIntelligence","IncrementAngerAtPlayer",0x421050,0,-1,2048},
 {"CTaskSimpleFight","Constructor",0x61C470,0,-1,2048},
 {"CEventSource","ComputeEventSourceType",0x4ABAC0,0,-1,2048}
};
inline bool done{};inline std::ofstream log;
inline void Write(const json& row){ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();}
struct Abi {uint32 eax,ecx,edx;bool stack,nonvolatile,seh;std::array<uint32,4> nonvolatileDelta;};
inline Abi Call(uintptr_t entry,uintptr_t object,uint32 argA,uint32 argB,uint32 argC,uint32 argD,int argc,uint32 seedD,bool cdeclCall=false){
    uint32 resultA,resultC,resultD,before,after,beforeH,afterH;
    uint32 beforeB,beforeS,beforeI,beforeP,afterB,afterS,afterI,afterP;
    __asm {
        mov eax,dword ptr fs:[0]
        mov beforeH,eax
        mov beforeB,ebx
        mov beforeS,esi
        mov beforeI,edi
        mov beforeP,ebp
        mov before,esp
        cmp argc,3
        jl taskPedNoC
        push argC
    taskPedNoC:
        cmp argc,2
        jl taskPedNoB
        push argB
    taskPedNoB:
        cmp argc,1
        jl taskPedNoA
        push argA
    taskPedNoA:
        mov ecx,object
        mov edx,seedD
        mov eax,0A5123456h
        call entry
        mov resultA,eax
        mov resultC,ecx
        mov resultD,edx
        mov afterB,ebx
        mov afterS,esi
        mov afterI,edi
        mov afterP,ebp
        cmp cdeclCall,0
        je taskPedStackDone
        add esp,8
    taskPedStackDone:
        mov after,esp
        mov eax,dword ptr fs:[0]
        mov afterH,eax
    }
    return {resultA,resultC,resultD,before==after,
        beforeB==afterB&&beforeS==afterS&&beforeI==afterI&&beforeP==afterP,beforeH==afterH,
        {beforeB^afterB,beforeS^afterS,beforeI^afterI,beforeP^afterP}};
}
inline float ClearSquaredConstant(){
 float squared;
 __asm {
  fld dword ptr ds:[0086C52Ch]
  fmul dword ptr ds:[0086C52Ch]
  fstp squared
 }
 return squared;
}
inline void Update(){
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependenciestaskped5-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependenciestaskped5-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciestaskped5");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 OriginalDependencies499::HookRoute clear("CAttractorScanner","Clear",0x5FFF90,FunctionToVoidPtr(&CAttractorScanner::Clear));
 OriginalDependencies499::HookRoute anger("CPedIntelligence","IncrementAngerAtPlayer",0x421050,FunctionToVoidPtr(&IncrementAngerAtPlayerRegs));
 OriginalDependencies499::HookRoute fight("CTaskSimpleFight","Constructor",0x61C470,FunctionToVoidPtr(&CTaskSimpleFight::Constructor));
 OriginalDependencies499::HookRoute event("CEventSource","ComputeEventSourceType",0x4ABAC0,&CEventSource::ComputeEventSourceType);
 OriginalDependencies499::HookRoute* routes[]{&clear,&anger,&fight,&event};
 std::array<std::vector<uint8>,4> codes;for(int i=0;i<4;++i)codes[i]=ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),5);
 json helpers=json::array();for(uintptr_t a:{0x571B70u,0x571A00u,0x601C30u,0x601BC0u,0x5DF8F0u})helpers.push_back({{"address",a},{"jumps_to_dll",ReversingProbe::JumpsToDll(a)},{"bytes",ReversingProbe::Bytes(reinterpret_cast<void*>(a),5)}});
 Write({{"event","begin"},{"registered_hook",clear.Registered()&&anger.Registered()&&fight.Registered()&&event.Registered()},{"collaborators_stubbed",false},{"helper_routes",helpers},{"exception_unwind_tested",false}});
 auto savedRefs=CReferences::pEmptyList;const auto savedTime=CTimer::GetTimeInMS();const auto savedFp=_controlfp(0,0);
 auto restore=[&]{CReferences::pEmptyList=savedRefs;*reinterpret_cast<uint32*>(0xB7CB84)=savedTime;_controlfp(savedFp,_MCW_PC);};
 alignas(4) std::array<uint8,0x300> object;
 alignas(4) std::array<uint8,sizeof(CPed)> source,victim;
 alignas(4) std::array<uint8,sizeof(CPedIntelligence)> intelligence;
 alignas(4) std::array<CReference,8> refs;
 alignas(4) std::array<uint8,0x1C> alias;
 CEntity* unrelated=nullptr;
 auto put32=[](void* p,uint32 v){memcpy(p,&v,4);};auto put16=[](void* p,uint16 v){memcpy(p,&v,2);};
 bool passed=true;int pair=0;
 for(int index=0;index<4;++index){int failures=0;const auto& fn=fns[index];
  for(int scenario=0;scenario<fn.cases;++scenario){
   auto invoke=[&](bool candidate){restore();_controlfp(_PC_24,_MCW_PC);uint32 seed=uint32(scenario)*2654435761u+0x12578F21u;
    auto next=[&]{seed=seed*1664525u+1013904223u;return seed;};
    auto fill=[&](auto& a){auto* p=reinterpret_cast<uint8*>(a.data());for(size_t n=0;n<sizeof(a);++n)p[n]=uint8(next()>>24);};
    fill(object);memset(source.data(),0,source.size());memset(victim.data(),0,victim.size());memset(intelligence.data(),0,intelligence.size());memset(refs.data(),0,sizeof(refs));
    for(int r=0;r<7;++r)refs[r].m_pNext=&refs[r+1];CReferences::pEmptyList=refs.data();
    uint32 thisPtr=uint32(reinterpret_cast<uintptr_t>(object.data())),argA=0,argB=0,argC=0;int argc=0;auto expected=object;
    bool contract=true,aliasEqual=true;int expectedEvent=0;Abi abi{};bool routeOk=false;
    auto native=[&]{return routes[index]->Invoke(candidate,[&]{routeOk=ReversingProbe::JumpsToDll(fn.address)==candidate;return Call(fn.address,thisPtr,argA,argB,argC,0,argc,0xED12AA55u^uint32(scenario),index==3);});};
    if(index==0){
     const float squared=ClearSquaredConstant();
     for(int n=0;n<10;++n){put32(expected.data()+0x18+4*n,0);put32(expected.data()+0x40+4*n,0);put32(expected.data()+0x68+4*n,std::bit_cast<uint32>((n==4||n==7)?squared:25.f));}
     abi=native();contract=object==expected;
    }else if(index==1){
     const uint32 times[]{0,1,2999,3000,3001,0x7FFFFFFFu,0xFFFFFFFEu,0xFFFFFFFFu};
     const uint32 intervals[]{0,1,3000,0xFFFFFFFFu,0x7FFFFFFFu,0x80000000u};
     const uint32 now=times[(scenario/4)%8],start=(scenario&32)?next():times[(scenario/16)%8],interval=intervals[(scenario/64)%6];
     *reinterpret_cast<uint32*>(0xB7CB84)=now;object[0x174]=uint8(scenario>>3);put32(object.data()+0x178,start);put32(object.data()+0x17C,interval);object[0x180]=uint8(scenario&1);object[0x181]=uint8((scenario>>1)&1);
     expected=object;argA=uint8(scenario);argc=1;
     if(expected[0x180]){if(expected[0x181]){put32(expected.data()+0x178,now);expected[0x181]=0;}
      uint32 st;memcpy(&st,expected.data()+0x178,4);if(now>=uint32(st+interval)){put32(expected.data()+0x178,now);put32(expected.data()+0x17C,3000);expected[0x180]=1;expected[0x174]=uint8(expected[0x174]+argA);}}
     alias.fill(0xD7);memcpy(alias.data()+4,object.data()+0x174,0x14);abi=native();
     reinterpret_cast<CMentalState*>(alias.data()+4)->IncrementAnger(int32(argA));aliasEqual=memcmp(alias.data()+4,object.data()+0x174,0x14)==0;
     for(int n:{0,1,2,3,24,25,26,27})aliasEqual&=alias[n]==0xD7;contract=object==expected&&aliasEqual;
    }else if(index==2){
     argA=(scenario&1)?uint32(reinterpret_cast<uintptr_t>(source.data())):0;argB=next();const uint32 idles[]{0,1,59999,60000,60001,65535,65536,0xFFFFFFFFu};argC=idles[(scenario/2)%8];argc=3;
     source[0x36]=(scenario&16)?1:4;source[0x2E]=0;put32(source.data()+0x1C,(scenario&32)?0x10000u:0);
     if(argA&&(scenario&64)){
      CReferences::pEmptyList=&refs[1];unrelated=reinterpret_cast<CEntity*>(source.data());
      refs[0].m_ppEntity=(scenario&128)?reinterpret_cast<CEntity**>(object.data()+24):&unrelated;
      refs[0].m_pNext=nullptr;put32(source.data()+0x24,uint32(reinterpret_cast<uintptr_t>(&refs[0])));
     }
     put32(expected.data(),0x86D684);put32(expected.data()+4,0);expected[8]=0;expected[9]=1;expected[10]=0;put32(expected.data()+12,0x21);put16(expected.data()+16,uint16(std::min(argC,60000u)));put16(expected.data()+18,0);expected[20]=0;expected[21]=0;
     put32(expected.data()+24,argA);put32(expected.data()+28,0);put32(expected.data()+32,0);expected[36]=0xFF;expected[37]=0xFF;expected[38]=uint8(argB);expected[39]=0;
     abi=native();contract=object==expected&&abi.eax==thisPtr;
    }else{
     auto* src=reinterpret_cast<CPed*>(source.data());auto* dst=reinterpret_cast<CPed*>(victim.data());auto* intel=reinterpret_cast<CPedIntelligence*>(intelligence.data());
     const int mode=scenario%8;src->m_nPedType=static_cast<ePedType>((scenario/8)%32);dst->m_nPedType=PED_TYPE_CIVMALE;put32(victim.data()+0x47C,uint32(reinterpret_cast<uintptr_t>(intel)));put32(intelligence.data(),uint32(reinterpret_cast<uintptr_t>(dst)));
     source[0x36]=mode==1?4:3;victim[0x36]=3;const uint32 flag=1u<<uint32(src->m_nPedType);
     if(mode==2||mode==6)dst->m_acquaintance.m_nHate=flag;
     if(mode==3||mode==6)dst->m_acquaintance.m_nLike=flag;
     if(mode==4)src->m_nPedType=PED_TYPE_PLAYER1;
     if(mode==5)src->m_nPedType=dst->m_nPedType;
     const auto victimBefore=victim;const auto intelligenceBefore=intelligence;
     {CEventDamage damage(mode==0?nullptr:reinterpret_cast<CEntity*>(src),0,WEAPON_UNARMED,static_cast<ePedPieceTypes>(0),0,false,false);
      const auto damageBefore=ReversingProbe::Bytes(&damage,sizeof(damage));argA=uint32(reinterpret_cast<uintptr_t>(&damage));argB=uint32(reinterpret_cast<uintptr_t>(dst));argc=2;thisPtr=0xEC12AA55u;
      if(mode!=0&&mode!=1){if(mode==2||mode==6)expectedEvent=3;else if(mode==3||src->m_nPedType==dst->m_nPedType)expectedEvent=2;else if(uint32(src->m_nPedType)<2)expectedEvent=1;}
      abi=native();contract=abi.eax==uint32(expectedEvent)&&victim==victimBefore&&intelligence==intelligenceBefore&&damageBefore==ReversingProbe::Bytes(&damage,sizeof(damage));}
     contract&=*reinterpret_cast<uint32*>(source.data()+0x24)==0;
    }
    json value={{"eax",abi.eax},{"ecx",abi.ecx},{"edx",abi.edx},{"stack_restored",abi.stack},{"nonvolatile_preserved",abi.nonvolatile},{"seh_restored",abi.seh},{"nonvolatile_delta",abi.nonvolatileDelta},{"route_ok",routeOk},{"contract_ok",contract},{"alias_equal",aliasEqual},{"object",object},{"source",source},{"victim",victim},{"intelligence",intelligence},{"references",ReversingProbe::Bytes(refs.data(),sizeof(refs))},{"free_head",uint32(reinterpret_cast<uintptr_t>(CReferences::pEmptyList))}};
    restore();return value;
   };
   const auto original=invoke(false),candidate=invoke(true);const bool equal=original==candidate&&original["route_ok"]==true&&original["stack_restored"]==true&&original["nonvolatile_preserved"]==true&&original["seh_restored"]==true&&original["contract_ok"]==true;
   passed&=equal;failures+=!equal;Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pair++},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
  }
  Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures}});
 }
 restore();bool restored=CReferences::pEmptyList==savedRefs&&CTimer::GetTimeInMS()==savedTime&&(_controlfp(0,0)&_MCW_PC)==(savedFp&_MCW_PC);
 for(int i=0;i<4;++i)restored&=codes[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),5);
 for(auto* route:routes){const bool ok=route->Restore();restored&=ok;}
 Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
