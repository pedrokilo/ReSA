#pragma once
#include <fstream>
#include "DecisionMakers/DecisionMakerTypes.h"
#include "PlayerRelationshipRecorder.h"
#include "Pools/Pools.h"
#include "Pools/PedPool.h"
#include "CopPed.h"
#include "PedIntelligence.h"
#include "PedGroups.h"
#include "FileMgr.h"
#include "References.h"
#include "Tasks/TaskTypes/TaskSimpleFightingControl.h"
#include "Tasks/TaskTypes/TaskSimpleGunControl.h"
#include "Tasks/TaskTypes/TaskSimpleThrowControl.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesDecisionRelationship6Live {
struct Fn {const char* cls;const char* name;uintptr_t address;int vt,slot,cases;};
inline const Fn fns[]{
 {"CDecisionMakerTypes","RemoveDecisionMaker",0x6043A0,0,-1,2048},
 {"CDecisionMakerTypes","AddDecisionMaker",0x607050,0,-1,2048},
 {"CDecisionMakerTypes","LoadEventIndices",0x600840,0,-1,128},
 {"CPlayerRelationshipRecorder","RecordRelationshipWithPlayer",0x61A1D0,0,-1,2048}
};
inline bool done{};inline std::ofstream log;
inline void Write(const json& row){ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();}
inline std::vector<uint8> FileBytes(const std::filesystem::path& path){std::ifstream in(path,std::ios::binary);return {std::istreambuf_iterator<char>(in),std::istreambuf_iterator<char>()};}
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
inline void Update(){
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependenciesdecisionrelationship6-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependenciesdecisionrelationship6-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesdecisionrelationship6");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 const auto filePath=dir/"data"/"decision"/"PedEvent.txt";const auto fileBefore=FileBytes(filePath);
 if(fileBefore.empty()){Write({{"event","error"},{"reason","Missing or empty real data/decision/PedEvent.txt"},{"state_restored",true}});return;}
 OriginalDependencies499::HookRoute remove("CDecisionMakerTypes","RemoveDecisionMaker",0x6043A0,FunctionToVoidPtr(&CDecisionMakerTypes::RemoveDecisionMaker));
 OriginalDependencies499::HookRoute add("CDecisionMakerTypes","AddDecisionMaker",0x607050,FunctionToVoidPtr(&CDecisionMakerTypes::AddDecisionMaker));
 OriginalDependencies499::HookRoute load("CDecisionMakerTypes","LoadEventIndices",0x600840,FunctionToVoidPtr(&CDecisionMakerTypes::LoadEventIndices));
 OriginalDependencies499::HookRoute record("CPlayerRelationshipRecorder","RecordRelationshipWithPlayer",0x61A1D0,FunctionToVoidPtr(&CPlayerRelationshipRecorder::RecordRelationshipWithPlayer));
 OriginalDependencies499::HookRoute* routes[]{&remove,&add,&load,&record};
 std::array<std::vector<uint8>,4> codes;for(int i=0;i<4;++i)codes[i]=ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),5);
 json helpers=json::array();for(uintptr_t a:{0x600B50u,0x5F7340u,0x6006B0u,0x5BB9F0u,0x681720u,0x6985E0u,0x698610u})helpers.push_back({{"address",a},{"jumps_to_dll",ReversingProbe::JumpsToDll(a)},{"bytes",ReversingProbe::Bytes(reinterpret_cast<void*>(a),5)}});
 Write({{"event","begin"},{"registered_hook",remove.Registered()&&add.Registered()&&load.Registered()&&record.Registered()},{"collaborators_stubbed",false},{"helper_routes",helpers},{"data_file","data/decision/PedEvent.txt"},{"file_bytes",fileBefore},{"relationship3_and_7_unreachable_with_original_classifiers",true}});
 const auto savedActive=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC0B01C),20);
 const auto savedGroup=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC0AFE0),20);
 const auto savedGroups=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC09920),8*0x2D4);
 const auto savedGroupActive=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC098E0),8);
 const auto savedDir=ReversingProbe::Bytes(CFileMgr::ms_dirName,128),savedRootDir=ReversingProbe::Bytes(CFileMgr::ms_rootDirName,128);
 wchar_t savedWindowsDir[32768]{};GetCurrentDirectoryW(32768,savedWindowsDir);
 auto*& globalPool=*reinterpret_cast<CPedPool**>(0xB74490);auto* savedPool=globalPool;const auto savedTime=CTimer::GetTimeInMS();auto* savedRefs=CReferences::pEmptyList;
 auto restore=[&]{globalPool=savedPool;memcpy(reinterpret_cast<void*>(0xC0B01C),savedActive.data(),20);memcpy(reinterpret_cast<void*>(0xC0AFE0),savedGroup.data(),20);memcpy(reinterpret_cast<void*>(0xC09920),savedGroups.data(),savedGroups.size());memcpy(reinterpret_cast<void*>(0xC098E0),savedGroupActive.data(),8);memcpy(CFileMgr::ms_dirName,savedDir.data(),128);memcpy(CFileMgr::ms_rootDirName,savedRootDir.data(),128);SetCurrentDirectoryW(savedWindowsDir);*reinterpret_cast<uint32*>(0xB7CB84)=savedTime;CReferences::pEmptyList=savedRefs;};
 static_assert(sizeof(CCopPed)==0x7C4);
 alignas(4) std::array<uint8,7*0x7C4> storage;std::array<uint8,7> states;
 CPedPool ownPool(7,storage.data(),states.data());
 CPedPool emptyPool(0,storage.data(),states.data());
 alignas(4) std::array<uint8,sizeof(CDecisionMakerTypes)> maker;
 alignas(4) std::array<uint8,0x99C> decision;
 alignas(4) std::array<uint8,7*sizeof(CPedIntelligence)> infos;
 alignas(4) std::array<uint8,7*0x40> stats;
 alignas(4) std::array<uint8,sizeof(CPed)> ped;
 alignas(4) std::array<uint8,sizeof(CPedIntelligence)> intel;
 alignas(4) std::array<uint8,0x80> relationships;
 alignas(16) std::array<uint8,0x100> taskStorage;
 auto put32=[](void* p,uint32 v){memcpy(p,&v,4);};auto get32=[](const void* p){uint32 v;memcpy(&v,p,4);return v;};
 auto active=reinterpret_cast<uint8*>(0xC0B01C);auto group=reinterpret_cast<uint8*>(0xC0AFE0);auto groupFlags=reinterpret_cast<uint8*>(0xC098E0);auto groupObjects=reinterpret_cast<uint8*>(0xC09920);
 bool passed=true;int pair=0;
 for(int index=0;index<4;++index){int failures=0;const auto& fn=fns[index];
  for(int scenario=0;scenario<fn.cases;++scenario){
   auto invoke=[&](bool candidate){restore();uint32 seed=uint32(scenario)*2654435761u+0x12893211u;
    auto next=[&]{seed=seed*1664525u+1013904223u;return seed;};
    auto fill=[&](auto& a){auto* p=reinterpret_cast<uint8*>(a.data());for(size_t n=0;n<sizeof(a);++n)p[n]=uint8(next()>>24);};
    fill(maker);fill(decision);for(int d=0;d<41;++d)for(int f=48;f<60;++f)decision[d*60+f]&=1;for(int d=0;d<20*41;++d)for(int f=48;f<60;++f)maker[4+d*60+f]&=1;fill(storage);fill(infos);fill(stats);fill(relationships);memset(ped.data(),0,ped.size());memset(intel.data(),0,intel.size());taskStorage.fill(0xD7);
    memset(active,0,20);memset(group,0,20);uint32 thisPtr=uint32(reinterpret_cast<uintptr_t>(maker.data())),argA=0,argB=0,argC=0;int argc=0;bool contract=true,routeOk=false;Abi abi{};
    auto native=[&]{return routes[index]->Invoke(candidate,[&]{routeOk=ReversingProbe::JumpsToDll(fn.address)==candidate;return Call(fn.address,thisPtr,argA,argB,argC,0,argc,0xED123411u^uint32(scenario));});};
    if(index==0){
     argA=uint32((scenario/8)%20);argc=1;const bool enabled=scenario&1,isGroup=scenario&2;active[argA]=uint8(enabled);group[argA]=uint8(isGroup);
     put32(maker.data(),uint32(1+(scenario%20)));auto expectedMaker=maker;if(enabled)put32(expectedMaker.data(),get32(maker.data())-1);
     auto expectedInfos=infos;std::array<uint8,8*0x2D4> expectedGroups;memcpy(expectedGroups.data(),groupObjects,expectedGroups.size());
     for(int n=0;n<7;++n){states[n]=uint8((scenario&(1u<<(n+3)))?0x80:n);auto* person=storage.data()+n*0x7C4;auto* info=infos.data()+n*sizeof(CPedIntelligence);auto* stat=stats.data()+n*0x40;
      put32(person+0x47C,uint32(reinterpret_cast<uintptr_t>(info)));put32(person+0x59C,uint32(reinterpret_cast<uintptr_t>(stat)));const uint32 id=(n%2==(scenario>>10)%2)?argA:((argA+1)%20);put32(info+0xB4,id);const int8 values[]{0,7,-1,4};stat[0x32]=uint8(values[(scenario+n)%4]);
     }
     expectedInfos=infos;
     for(int n=0;n<7;++n){auto* e=expectedInfos.data()+n*sizeof(CPedIntelligence);if(enabled&&!isGroup&&!(scenario&1024)&&!(states[n]&0x80)&&get32(e+0xB4)==argA){const uint32 id=get32(e+0xB4),newId=uint32(int32(int8(stats[n*0x40+0x32])));if(id==0)put32(e+0xB8,newId);else if(newId==0){put32(e+0xB8,id);put32(e+0xB4,0);}else put32(e+0xB4,newId);if(get32(e+0xB4)==7){put32(e+0xC8,0x40A00000);put32(e+0xC4,15);}}}
     for(int n=0;n<8;++n){groupFlags[n]=uint8((scenario>>(n%6))&1);put32(groupObjects+n*0x2D4+0x2C8,(n%2)?argA:(argA+1)%20);}
     memcpy(expectedGroups.data(),groupObjects,expectedGroups.size());for(int n=0;n<8;++n)if(enabled&&isGroup&&groupFlags[n]&&get32(expectedGroups.data()+n*0x2D4+0x2C8)==argA)put32(expectedGroups.data()+n*0x2D4+0x2C8,0xFFFFFFFF);
     globalPool=(scenario&1024)?&emptyPool:&ownPool;const auto storageBefore=storage;const auto statesBefore=states;const auto statsBefore=stats;abi=native();contract=maker==expectedMaker&&infos==expectedInfos&&memcmp(expectedGroups.data(),groupObjects,expectedGroups.size())==0&&storage==storageBefore&&states==statesBefore&&stats==statsBefore&&active[argA]==0&&group[argA]==uint8(enabled?false:isGroup);
    }else if(index==1){
     argA=uint32(reinterpret_cast<uintptr_t>(decision.data()));argB=uint8((scenario>>1)&1);argC=uint8(scenario&1);argc=3;const int first=argC?15:0,last=argC?20:15;int chosen=-1;
     for(int n=0;n<20;++n){active[n]=uint8((scenario>>2)%5==0||n!=first+int((scenario/20)%uint32(last-first)));group[n]=uint8(next());}for(int n=first;n<last;++n)if(!active[n]){chosen=n;break;}
     const auto decisionBefore=decision;auto expected=maker;put32(maker.data(),uint32(scenario%20));expected=maker;
     if(chosen>=0){put32(expected.data(),get32(maker.data())+1);if(scenario&512)argA=uint32(reinterpret_cast<uintptr_t>(maker.data()+4+chosen*0x99C));else memcpy(expected.data()+4+chosen*0x99C,decision.data(),decision.size());}
     std::array<uint8,20> expectedActive,expectedGroup;memcpy(expectedActive.data(),active,20);memcpy(expectedGroup.data(),group,20);if(chosen>=0){expectedActive[chosen]=1;expectedGroup[chosen]=uint8(argB);}
     abi=native();contract=abi.eax==uint32(chosen)&&maker==expected&&decision==decisionBefore&&memcmp(active,expectedActive.data(),20)==0&&memcmp(group,expectedGroup.data(),20)==0;
    }else if(index==2){
     const auto before=maker;DWORD handlesBefore=0,handlesAfter=0;GetProcessHandleCount(GetCurrentProcess(),&handlesBefore);abi=native();GetProcessHandleCount(GetCurrentProcess(),&handlesAfter);
     contract=handlesBefore==handlesAfter;for(size_t n=0;n<maker.size();++n)if(n<0xC034||n>=0xC1B4)contract&=maker[n]==before[n];contract&=FileBytes(filePath)==fileBefore;
    }else{
     thisPtr=uint32(reinterpret_cast<uintptr_t>(relationships.data()));argA=uint32(reinterpret_cast<uintptr_t>(ped.data()));argc=1;put32(ped.data()+0x47C,uint32(reinterpret_cast<uintptr_t>(intel.data())));
     const int taskKind=scenario%5,slot=(scenario/5)%5;CTask* task=nullptr;
     switch(taskKind){case 1:task=::new(taskStorage.data()) CTaskSimpleFightingControl(nullptr,0.f,1.f);break;case 2:task=::new(taskStorage.data()) CTaskSimpleGunControl(nullptr,CVector{},CVector{},eGunCommand::AIM,1,0);break;case 3:task=::new(taskStorage.data()) CTaskSimpleThrowControl(nullptr,nullptr);break;case 4:task=::new(taskStorage.data()) CTaskSimpleStandStill(0,false,false,8.f);break;}
     if(task)put32(intel.data()+4+slot*4,uint32(reinterpret_cast<uintptr_t>(task)));
     for(int n=0;n<16;++n)put32(relationships.data()+n*8,uint32((n==((scenario/25)%17)||((scenario&512)&&n==15))?argA:0x12340000u+n*4));if(scenario&256)put32(relationships.data(),0);
     auto expected=relationships;for(int n=0;n<16;++n)if(get32(expected.data()+n*8)==argA){put32(expected.data()+n*8,0);expected[n*8+4]=0;break;}
     const auto pedBefore=ped;const auto intelBefore=intel;const auto taskBefore=taskStorage;abi=native();contract=relationships==expected&&ped==pedBefore&&intel==intelBefore&&taskStorage==taskBefore;
     if(task)put32(intel.data()+4+slot*4,0);switch(taskKind){case 1:reinterpret_cast<CTaskSimpleFightingControl*>(task)->~CTaskSimpleFightingControl();break;case 2:reinterpret_cast<CTaskSimpleGunControl*>(task)->~CTaskSimpleGunControl();break;case 3:reinterpret_cast<CTaskSimpleThrowControl*>(task)->~CTaskSimpleThrowControl();break;case 4:reinterpret_cast<CTaskSimpleStandStill*>(task)->~CTaskSimpleStandStill();break;}
    }
    json value={{"eax",abi.eax},{"ecx",abi.ecx},{"edx",abi.edx},{"stack_restored",abi.stack},{"nonvolatile_preserved",abi.nonvolatile},{"seh_restored",abi.seh},{"nonvolatile_delta",abi.nonvolatileDelta},{"route_ok",routeOk},{"contract_ok",contract}};
    if(index<=2){value["count"]=get32(maker.data());value["active"]=ReversingProbe::Bytes(active,20);value["group"]=ReversingProbe::Bytes(group,20);}if(index==0){json ii=json::array(),gg=json::array();for(int n=0;n<7;++n)ii.push_back(ReversingProbe::Bytes(infos.data()+n*sizeof(CPedIntelligence)+0xB4,0x18));for(int n=0;n<8;++n)gg.push_back(get32(groupObjects+n*0x2D4+0x2C8));value["info_fields"]=ii;value["group_ids"]=gg;}
    if(index==1){json slots=json::array();for(int n=0;n<20;++n)if(active[n]&&memcmp(maker.data()+4+n*0x99C,decision.data(),decision.size())==0)slots.push_back({{"slot",n},{"decision",decision}});value["copied_slots"]=slots;}
    if(index==2)value["event_indices"]=ReversingProbe::Bytes(maker.data()+0xC034,0x180);if(index==3)value["relationships"]=relationships;
    restore();wchar_t currentDir[32768]{};GetCurrentDirectoryW(32768,currentDir);
    value["globals_restored"]=globalPool==savedPool&&CTimer::GetTimeInMS()==savedTime&&CReferences::pEmptyList==savedRefs&&wcscmp(savedWindowsDir,currentDir)==0&&savedDir==ReversingProbe::Bytes(CFileMgr::ms_dirName,128)&&savedRootDir==ReversingProbe::Bytes(CFileMgr::ms_rootDirName,128)&&savedActive==ReversingProbe::Bytes(active,20)&&savedGroup==ReversingProbe::Bytes(group,20)&&savedGroups==ReversingProbe::Bytes(groupObjects,8*0x2D4)&&savedGroupActive==ReversingProbe::Bytes(groupFlags,8);return value;
   };
   const auto original=invoke(false),candidate=invoke(true);const bool equal=original==candidate&&original["route_ok"]==true&&original["stack_restored"]==true&&original["nonvolatile_preserved"]==true&&original["seh_restored"]==true&&original["contract_ok"]==true&&original["globals_restored"]==true;
   passed&=equal;failures+=!equal;Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pair++},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
  }
  Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures}});
 }
 restore();wchar_t finalDir[32768]{};GetCurrentDirectoryW(32768,finalDir);
 bool restored=globalPool==savedPool&&CTimer::GetTimeInMS()==savedTime&&CReferences::pEmptyList==savedRefs&&wcscmp(savedWindowsDir,finalDir)==0&&FileBytes(filePath)==fileBefore&&savedDir==ReversingProbe::Bytes(CFileMgr::ms_dirName,128)&&savedRootDir==ReversingProbe::Bytes(CFileMgr::ms_rootDirName,128)&&savedActive==ReversingProbe::Bytes(active,20)&&savedGroup==ReversingProbe::Bytes(group,20)&&savedGroups==ReversingProbe::Bytes(groupObjects,8*0x2D4)&&savedGroupActive==ReversingProbe::Bytes(groupFlags,8);
 for(int i=0;i<4;++i)restored&=codes[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),5);for(auto* route:routes){const bool ok=route->Restore();restored&=ok;}
 Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
