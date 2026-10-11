#pragma once
#include <fstream>
#include "DecisionMakers/DecisionMakerTypes.h"
#include "PedIntelligence.h"
#include "PedGroup.h"
#include "Pools/PedPool.h"
#include "Pools/TaskPool.h"
#include "Events/EventScriptCommand.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "References.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesDecisionPed7Live {
struct Fn {const char* cls;const char* name;uintptr_t address;int vt,slot,cases;};
inline const Fn fns[]{
 {"CDecisionMakerTypes","MakeDecision-Ped",0x606E70,0,-1,2048},
 {"CDecisionMakerTypes","MakeDecision-Group",0x606F80,0,-1,2048},
 {"CPedPool","GetRef",0x4442D0,0,-1,2048},
 {"CEventScriptCommand","Destructor",0x4B0A50,0,-1,2048}
};
inline bool done{};inline std::ofstream log;
inline void Write(const json& row){ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();}
inline uintptr_t ResolveBody(uintptr_t address){for(int n=0;n<16&&*reinterpret_cast<const uint8*>(address)==0xE9;++n)address+=5+*reinterpret_cast<const int32*>(address+1);return address;}
struct HelperRoute {
 uintptr_t entry,body;std::vector<uint8> entryBytes,bodyBytes;
 explicit HelperRoute(uintptr_t address):entry(address),body(ResolveBody(address)),entryBytes(ReversingProbe::Bytes(reinterpret_cast<void*>(address),16)),bodyBytes(ReversingProbe::Bytes(reinterpret_cast<void*>(body),16)){}
 bool Unchanged()const{return body==ResolveBody(entry)&&entryBytes==ReversingProbe::Bytes(reinterpret_cast<void*>(entry),16)&&bodyBytes==ReversingProbe::Bytes(reinterpret_cast<void*>(body),16);}
 json Observe()const{return {{"entry",entry},{"body",body},{"entry_bytes",entryBytes},{"body_bytes",bodyBytes}};}
};
struct FpuState {std::array<uint8,108> x87;uint32 mxcsr;bool operator==(const FpuState&)const=default;};
inline FpuState CaptureFpu(){FpuState result;auto* p=result.x87.data();auto* m=&result.mxcsr;__asm {
 mov eax,p
 fnsave [eax]
 frstor [eax]
 mov eax,m
 stmxcsr [eax]
 }return result;}
inline void RestoreFpu(const FpuState& state){const auto* p=state.x87.data();const auto* m=&state.mxcsr;__asm {
 mov eax,p
 frstor [eax]
 mov eax,m
 ldmxcsr [eax]
 }}
inline bool NativeHelpers(){
 const std::array<uint8,16> d{0x83,0xEC,0x60,0x53,0x55,0x56,0x8B,0xD9,0x33,0xF6,0x57,0x89,0x5C,0x24,0x10,0x33};
 const std::array<uint8,16> m{0x8B,0x54,0x24,0x20,0x8B,0xC1,0x8B,0x4C,0x24,0x24,0x51,0x8B,0x4C,0x24,0x20,0x52};
 const std::array<uint8,16> r{0xE8,0x1A,0x60,0x00,0x00,0x8B,0x48,0x14,0x69,0xC9,0xFD,0x43,0x03,0x00,0x81,0xC1};
 const std::array<uint8,16> p{0x53,0x56,0xFF,0x15,0x94,0x80,0x85,0x00,0xFF,0x35,0x10,0x31,0x8E,0x00,0x8B,0xD8};
 return !ReversingProbe::JumpsToDll(0x6040D0)&&!ReversingProbe::JumpsToDll(0x606060)&&
 memcmp(d.data(),reinterpret_cast<void*>(0x6040D0),16)==0&&memcmp(m.data(),reinterpret_cast<void*>(0x606060),16)==0&&
 memcmp(r.data(),reinterpret_cast<void*>(0x821B1E),16)==0&&memcmp(p.data(),reinterpret_cast<void*>(0x827B3D),16)==0&&
 (0x821B23+*reinterpret_cast<const int32*>(0x821B1F))==0x827B3D;
}
struct Abi {uint32 eax,ecx,edx;bool stack,nonvolatile,seh;std::array<uint32,4> nonvolatileDelta;};
inline Abi Call(uintptr_t entry,uintptr_t object,const uint32* arguments,int argc,uint32 seedD){
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
        mov ecx,argc
        mov edx,arguments
        dec ecx
        js decisionPed7NoArgs
    decisionPed7PushArg:
        push dword ptr [edx+ecx*4]
        dec ecx
        jns decisionPed7PushArg
    decisionPed7NoArgs:
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
        mov after,esp
        mov eax,dword ptr fs:[0]
        mov afterH,eax
    }
    return {resultA,resultC,resultD,before==after,
        beforeB==afterB&&beforeS==afterS&&beforeI==afterI&&beforeP==afterP,beforeH==afterH,
        {beforeB^afterB,beforeS^afterS,beforeI^afterI,beforeP^afterP}};
}
inline void Update(){
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependenciesdecisionped7-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependenciesdecisionped7-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesdecisionped7");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 // The empty unhooked C++ MakeDecision wrapper must never mask the native collaborator.
 if(!NativeHelpers()){Write({{"event","error"},{"reason","native decision/RNG helper route changed"},{"state_restored",true}});return;}
 using PedDecision=void(CDecisionMakerTypes::*)(CPed*,eEventType,int32,bool,eTaskType,eTaskType,eTaskType,eTaskType,bool,int16&,int16&);
 using GroupDecision=eTaskType(CDecisionMakerTypes::*)(CPedGroup*,eEventType,int32,bool,eTaskType,eTaskType,eTaskType,eTaskType);
 OriginalDependencies499::HookRoute one("CDecisionMakerTypes","MakeDecision-Ped",0x606E70,FunctionToVoidPtr(static_cast<PedDecision>(&CDecisionMakerTypes::MakeDecision)));
 OriginalDependencies499::HookRoute two("CDecisionMakerTypes","MakeDecision-Group",0x606F80,FunctionToVoidPtr(static_cast<GroupDecision>(&CDecisionMakerTypes::MakeDecision)));
 OriginalDependencies499::HookRoute ref("CPedPool","GetRef",0x4442D0,FunctionToVoidPtr(&CPedPool::GetRef));
 OriginalDependencies499::HookRoute destroy("CEventScriptCommand","Destructor",0x4B0A50,FunctionToVoidPtr(&CEventScriptCommand::Destructor));
 OriginalDependencies499::HookRoute* routes[]{&one,&two,&ref,&destroy};
 std::array<std::vector<uint8>,4> codes;for(int n=0;n<4;++n)codes[n]=ReversingProbe::Bytes(reinterpret_cast<void*>(fns[n].address),5);
 std::vector<HelperRoute> helperRoutes;
 for(uintptr_t a:{0x6040D0u,0x606060u,0x6007A0u,0x821B1Eu,0x827B3Du,0x61A5A0u,0x61A5B0u,0x61A500u,0x4B0A00u,0x62F310u})helperRoutes.emplace_back(a);
 // C++ placement construction/new may call DLL bodies directly; record those actual routes too.
 helperRoutes.emplace_back(reinterpret_cast<uintptr_t>(FunctionToVoidPtr(&CTask::operator new)));
 helperRoutes.emplace_back(reinterpret_cast<uintptr_t>(FunctionToVoidPtr(&CTask::operator delete)));
 auto helpersUnchanged=[&]{if(!NativeHelpers())return false;for(const auto& h:helperRoutes)if(!h.Unchanged())return false;return true;};
 json helpers=json::array();for(const auto& h:helperRoutes)helpers.push_back(h.Observe());
 Write({{"event","begin"},{"registered_hook",one.Registered()&&two.Registered()&&ref.Registered()&&destroy.Registered()},{"collaborators_stubbed",false},{"helper_routes",helpers},{"seh_exception_unwind_tested",false}});
 const auto getPtd=reinterpret_cast<uint8*(__cdecl*)()>(0x827B3D);auto* randomState=reinterpret_cast<uint32*>(getPtd()+0x14);const auto savedRandom=*randomState;
 auto*& globalTaskPool=*reinterpret_cast<CTaskPool**>(0xB744A8);auto* savedTaskPool=globalTaskPool;auto* savedRefs=CReferences::pEmptyList;const auto savedTime=CTimer::GetTimeInMS();const auto savedFp=CaptureFpu();
 auto restore=[&]{globalTaskPool=savedTaskPool;*randomState=savedRandom;*reinterpret_cast<uint32*>(0xB7CB84)=savedTime;CReferences::pEmptyList=savedRefs;RestoreFpu(savedFp);};
 alignas(4) std::array<uint8,sizeof(CDecisionMakerTypes)> maker;
 alignas(4) std::array<uint8,sizeof(CPed)> ped;
 alignas(4) std::array<uint8,sizeof(CPedIntelligence)> intel;
 alignas(4) std::array<uint8,sizeof(CPedGroup)> group;
 alignas(4) std::array<uint8,7*0x7C4> pedStorage;std::array<uint8,7> pedStates;CPedPool pedPool(7,pedStorage.data(),pedStates.data());
 alignas(4) std::array<uint8,4*0x80> taskStorage;std::array<uint8,4> taskStates;CTaskPool taskPool(4,taskStorage.data(),taskStates.data());
 const auto taskPoolInitial=ReversingProbe::Bytes(&taskPool,sizeof(taskPool));
 alignas(4) std::array<uint8,sizeof(CEventScriptCommand)> eventStorage;
 std::array<int16,4> outputs;
 auto put32=[](void* p,uint32 v){memcpy(p,&v,4);};auto get32=[](const void* p){uint32 v;memcpy(&v,p,4);return v;};
 bool passed=one.Registered()&&two.Registered()&&ref.Registered()&&destroy.Registered();int pair=0;
 for(int index=0;index<4;++index){int failures=0;const auto& fn=fns[index];
  for(int scenario=0;scenario<fn.cases;++scenario){
   auto invoke=[&](bool candidate){restore();_controlfp(_PC_24,_MCW_PC);uint32 seed=uint32(scenario)*2654435761u+0x12893211u;
    auto next=[&]{seed=seed*1664525u+1013904223u;return seed;};auto fill=[&](auto& a){auto* p=reinterpret_cast<uint8*>(a.data());for(size_t n=0;n<sizeof(a);++n)p[n]=uint8(next()>>24);};
    fill(maker);fill(pedStorage);fill(eventStorage);fill(taskStorage);memset(ped.data(),0,ped.size());memset(intel.data(),0,intel.size());memset(group.data(),0,group.size());
    outputs={int16(0x5A51),int16(0x5A52),int16(0x5A53),int16(0x5A54)};uint32 thisPtr=uint32(reinterpret_cast<uintptr_t>(maker.data()));std::array<uint32,11> arguments{};int argc=0;bool routeOk=false,contract=true,taskPresent=false;Abi abi{};
    uint32 selectedOffset=0,randomBefore=next();bool helperRouteOK=helpersUnchanged();json virtualDispatch=json::object();
    if(index<2){
     const int dmIds[]{-2,-1,0,1,7,19};uint32 dm=uint32(dmIds[(scenario/8)%6]);const uint32 otherDm=uint32(dmIds[((scenario/8)+1)%6]);const uint32 eventId=uint32((scenario/8)%96),eventIndex=uint32((scenario/4)%41);put32(maker.data()+0xC034+eventId*4,eventIndex);
     const uint32 sourceType=uint32(scenario%4),inVehicle=uint32((scenario/4)%2);const uint32 avoid1=1001,avoid2=(scenario&64)?1002:0xFFFFFFFFu,avoid3=(scenario&128)?1003:0xFFFFFFFFu,seek=(scenario&256)?1004:0xFFFFFFFFu;
     if(index==0){put32(ped.data()+0x47C,uint32(reinterpret_cast<uintptr_t>(intel.data())));put32(intel.data()+0xB4,dm);put32(intel.data()+0xB8,otherDm);ped[0x484]=uint8((scenario&16)?2:1);const bool useGroup=scenario&32;if(useGroup)dm=otherDm;
      selectedOffset=dm==0xFFFFFFFEu?0xD4EC+eventIndex*60:dm==0xFFFFFFFFu?((ped[0x484]==2)?0xCB50:0xC1B4)+eventIndex*60:4+(dm*41+eventIndex)*60;
      arguments={uint32(reinterpret_cast<uintptr_t>(ped.data())),eventId,sourceType,inVehicle,avoid1,avoid2,avoid3,seek,uint32(useGroup),uint32(reinterpret_cast<uintptr_t>(&outputs[1])),uint32(reinterpret_cast<uintptr_t>(&outputs[2]))};argc=11;
     }else{const uint32 id=dm==0xFFFFFFFEu?0xFFFFFFFFu:dm;put32(group.data()+0x2C8,id);group[0x2D0]=uint8((scenario&16)?1:0);
      selectedOffset=id==0xFFFFFFFFu?((group[0x2D0]==1)?0xE824:0xDE88)+eventIndex*60:4+(id*41+eventIndex)*60;
      arguments={uint32(reinterpret_cast<uintptr_t>(group.data())),eventId,sourceType,inVehicle,avoid1,avoid2,avoid3,seek,0,0,0};argc=8;
     }
     // Define all six choices, including empty/filter/weighted paths; real rand state is snapshotted.
     auto* choice=maker.data()+selectedOffset;
     for(int n=0;n<6;++n){put32(choice+n*4,(scenario&512)?0xFFFFFFFFu:uint32(1000+n));for(int r=0;r<4;++r)choice[24+n*4+r]=uint8((scenario&1024)?0:1+(n+r+scenario)%99);choice[48+n*2]=uint8((scenario+n)&1);choice[49+n*2]=uint8(((scenario/2)+n)&1);}
     const auto makerBefore=maker;const auto pedBefore=ped;const auto intelBefore=intel;const auto groupBefore=group;*randomState=randomBefore;
     abi=routes[index]->Invoke(candidate,[&]{routeOk=routes[index]->Registered()&&ReversingProbe::JumpsToDll(fn.address)==candidate;return Call(fn.address,thisPtr,arguments.data(),argc,0xED123411u^uint32(scenario));});
     contract=maker==makerBefore&&ped==pedBefore&&intel==intelBefore&&group==groupBefore&&outputs[0]==int16(0x5A51)&&outputs[3]==int16(0x5A54);
     if(index==1)contract&=outputs[1]==int16(0x5A52)&&outputs[2]==int16(0x5A53);
    }else if(index==2){
     const uint32 slot=uint32(scenario%7);for(int n=0;n<7;++n)pedStates[n]=uint8((scenario+n*13)&255);thisPtr=uint32(reinterpret_cast<uintptr_t>(&pedPool));arguments[0]=uint32(reinterpret_cast<uintptr_t>(pedStorage.data()+slot*0x7C4));argc=1;
     const auto poolBefore=ReversingProbe::Bytes(&pedPool,sizeof(pedPool));const auto storageBefore=pedStorage;const auto statesBefore=pedStates;*randomState=randomBefore;
     abi=routes[index]->Invoke(candidate,[&]{routeOk=routes[index]->Registered()&&ReversingProbe::JumpsToDll(fn.address)==candidate;return Call(fn.address,thisPtr,arguments.data(),argc,0xED123411u^uint32(scenario));});
     contract=abi.eax==((slot<<8)+pedStates[slot])&&poolBefore==ReversingProbe::Bytes(&pedPool,sizeof(pedPool))&&pedStorage==storageBefore&&pedStates==statesBefore&&*randomState==randomBefore;
    }else{
     memcpy(&taskPool,taskPoolInitial.data(),taskPoolInitial.size());taskStates.fill(0x80);globalTaskPool=&taskPool;CTask* task=nullptr;
     if(scenario&1)task=new CTaskSimpleStandStill(100,false,false,8.f);taskPresent=task!=nullptr;
     auto* event=::new(eventStorage.data()) CEventScriptCommand(scenario%5,task,bool(scenario&2));thisPtr=uint32(reinterpret_cast<uintptr_t>(event));auto expected=eventStorage;put32(expected.data(),0x85AB38);
     const auto beforeStates=taskStates;const auto storageBefore=taskStorage;const auto poolBefore=ReversingProbe::Bytes(&taskPool,sizeof(taskPool));*randomState=randomBefore;
     std::vector<HelperRoute> virtualRoutes;
     if(task){const auto vtable=*reinterpret_cast<uintptr_t*>(task);virtualRoutes.emplace_back(*reinterpret_cast<uintptr_t*>(vtable));virtualDispatch=virtualRoutes[0].Observe();}
     abi=routes[index]->Invoke(candidate,[&]{routeOk=routes[index]->Registered()&&ReversingProbe::JumpsToDll(fn.address)==candidate;return Call(fn.address,thisPtr,arguments.data(),0,0xED123411u^uint32(scenario));});
     contract=eventStorage==expected&&*randomState==randomBefore&&taskPresent==bool(scenario&1);
     for(const auto& v:virtualRoutes)helperRouteOK&=v.Unchanged();
     if(task){const auto slot=(reinterpret_cast<uintptr_t>(task)-reinterpret_cast<uintptr_t>(taskStorage.data()))/0x80;contract&=slot<4&&taskStates[slot]==uint8(beforeStates[slot]|0x80)&&poolBefore==ReversingProbe::Bytes(&taskPool,sizeof(taskPool));for(int n=0;n<4;++n)if(size_t(n)!=slot)contract&=taskStates[n]==beforeStates[n];if(slot<4)for(size_t n=0;n<taskStorage.size();++n)if(n<slot*0x80||n>=slot*0x80+4)contract&=taskStorage[n]==storageBefore[n];}
     else contract&=taskStates==beforeStates&&poolBefore==ReversingProbe::Bytes(&taskPool,sizeof(taskPool));
    }
    helperRouteOK&=helpersUnchanged();
    json value={{"eax",abi.eax},{"ecx",abi.ecx},{"edx",abi.edx},{"stack_restored",abi.stack},{"nonvolatile_preserved",abi.nonvolatile},{"seh_restored",abi.seh},{"nonvolatile_delta",abi.nonvolatileDelta},{"route_ok",routeOk},{"helper_route_ok",helperRouteOK},{"contract_ok",contract},{"random_after",*randomState},{"outputs",outputs}};
    if(index<2)value["decision"]=ReversingProbe::Bytes(maker.data()+selectedOffset,60);if(index==2)value["ped_states"]=pedStates;
    if(index==3){value["task_present"]=taskPresent;value["event_object"]=eventStorage;value["task_storage"]=taskStorage;value["task_states"]=taskStates;value["task_pool"]=ReversingProbe::Bytes(&taskPool,sizeof(taskPool));value["virtual_task_dispatch"]=virtualDispatch;}
    restore();value["fpu_restored"]=CaptureFpu()==savedFp;value["globals_restored"]=globalTaskPool==savedTaskPool&&*randomState==savedRandom&&CReferences::pEmptyList==savedRefs&&CTimer::GetTimeInMS()==savedTime&&CaptureFpu()==savedFp;return value;
   };
   const auto original=invoke(false),candidate=invoke(true);const bool equal=original==candidate&&original["route_ok"]==true&&original["helper_route_ok"]==true&&original["stack_restored"]==true&&original["nonvolatile_preserved"]==true&&original["seh_restored"]==true&&original["contract_ok"]==true&&original["globals_restored"]==true&&original["fpu_restored"]==true;
   passed&=equal;failures+=!equal;Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pair++},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
  }
  Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures}});
 }
 restore();bool restored=globalTaskPool==savedTaskPool&&*randomState==savedRandom&&CReferences::pEmptyList==savedRefs&&CTimer::GetTimeInMS()==savedTime&&CaptureFpu()==savedFp&&helpersUnchanged();
 for(int i=0;i<4;++i)restored&=codes[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),5);for(auto* route:routes){const bool ok=route->Restore();restored&=ok;}
 Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
