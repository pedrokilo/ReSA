#pragma once
#include <fstream>
#include "TaskSimpleFight.h"
#include "PedSaveStructure.h"
#include "References.h"
#include "Reference.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesTaskLeaf3Live {
struct Fn {const char* cls;const char* name;uintptr_t address;int vt,slot,cases;};
inline const Fn fns[]{
    {"CTaskSimpleFight","ControlFight",0x61C5E0,0,-1,2048},
    {"CPedSaveStructure","Construct",0x5D43D0,0,-1,2048}
};
inline bool done{};inline std::ofstream log;
inline void Write(const json& row){ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();}
struct Abi {uint32 eax,ecx,edx;bool stack,nonvolatile;std::array<uint32,4> nonvolatileDelta;};
inline Abi Call(uintptr_t entry,uintptr_t object,uint32 argA,uint32 argB,int argc,uint32 seedD){
    uint32 resultA,resultC,resultD,before,after;
    uint32 beforeB,beforeS,beforeI,beforeP,afterB,afterS,afterI,afterP;
    __asm {
        mov beforeB,ebx
        mov beforeS,esi
        mov beforeI,edi
        mov beforeP,ebp
        mov before,esp
        cmp argc,2
        jne taskLeafOneArg
        push argB
    taskLeafOneArg:
        push argA
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
    }
    return {resultA,resultC,resultD,before==after,
        beforeB==afterB&&beforeS==afterS&&beforeI==afterI&&beforeP==afterP,
        {beforeB^afterB,beforeS^afterS,beforeI^afterI,beforeP^afterP}};
}
inline void Update(){
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciestaskleaf3-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependenciestaskleaf3-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciestaskleaf3");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    OriginalDependencies499::HookRoute control("CTaskSimpleFight","ControlFight",0x61C5E0,FunctionToVoidPtr(&CTaskSimpleFight::ControlFight));
    OriginalDependencies499::HookRoute construct("CPedSaveStructure","Construct",0x5D43D0,FunctionToVoidPtr(&CPedSaveStructure::Construct));
    Write({{"event","begin"},{"registered_hook",control.Registered()&&construct.Registered()},{"collaborators_stubbed",false},
        {"reference_helpers",json::array({
            {{"address",0x571A00},{"jumps_to_dll",ReversingProbe::JumpsToDll(0x571A00)},{"entry_bytes",ReversingProbe::Bytes(reinterpret_cast<void*>(0x571A00),5)}},
            {{"address",0x571B70},{"jumps_to_dll",ReversingProbe::JumpsToDll(0x571B70)},{"entry_bytes",ReversingProbe::Bytes(reinterpret_cast<void*>(0x571B70),5)}}
        })}});
    const auto savedReferences=CReferences::pEmptyList;
    auto& exitPool=*reinterpret_cast<uintptr_t*>(0x96A7D8);const auto savedPool=exitPool;
    const auto controlCode=ReversingProbe::Bytes(reinterpret_cast<void*>(0x61C5E0),5),constructCode=ReversingProbe::Bytes(reinterpret_cast<void*>(0x5D43D0),5);
    auto restore=[&]{CReferences::pEmptyList=savedReferences;exitPool=savedPool;};
    alignas(4) std::array<uint8,0x28> task;
    alignas(4) std::array<std::array<uint8,0x38>,2> entities;
    alignas(4) std::array<CReference,8> references;
    alignas(4) std::array<uint8,0x800> ped;
    alignas(4) std::array<uint8,0x18C> save;
    alignas(4) std::array<uint8,0x48> matrix;
    alignas(4) std::array<std::array<uint8,0x3C>,4> exits;
    alignas(4) std::array<uint32,4> pool;
    auto put32=[](void* p,uint32 value){memcpy(p,&value,4);};
    bool passed=true;int pair=0;
    for(int index=0;index<2;++index){int failures=0;auto& route=index?construct:control;const auto& fn=fns[index];
        for(int scenario=0;scenario<fn.cases;++scenario){
            auto invoke=[&](bool candidate){restore();uint32 seed=uint32(scenario)*2654435761u+0x1387FF21u;
                auto next=[&]{seed=seed*1664525u+1013904223u;return seed;};
                auto fill=[&](auto& array){auto* data=reinterpret_cast<uint8*>(array.data());for(size_t i=0;i<sizeof(array);++i)data[i]=uint8(next()>>24);};
                uint32 thisPointer,argA,argB=0;int argc;std::array<uint8,0x28> taskBefore;std::array<uint8,0x800> pedBefore;std::array<uint8,0x48> matrixBefore;uint8 expectedCommand=0;
                if(!index){fill(task);fill(entities);memset(references.data(),0,sizeof(references));
                    for(int i=0;i<2;++i){put32(entities[i].data()+0x24,0);put32(entities[i].data()+0x1C,0);entities[i][0x2E]=0;entities[i][0x36]=uint8((scenario&8)?1:4);}
                    const int oldIndex=scenario%3,newIndex=(scenario/3)%3;
                    thisPointer=uint32(reinterpret_cast<uintptr_t>(task.data()));
                    const uint32 oldPointer=oldIndex==2?0:uint32(reinterpret_cast<uintptr_t>(entities[oldIndex].data()));
                    argA=newIndex==2?0:uint32(reinterpret_cast<uintptr_t>(entities[newIndex].data()));
                    argB=uint8(scenario*73);argc=2;put32(task.data()+0x18,oldPointer);
                    for(int i=1;i<7;++i)references[i].m_pNext=&references[i+1];references[7].m_pNext=nullptr;
                    CReferences::pEmptyList=&references[1];
                    if(oldPointer){references[0].m_ppEntity=reinterpret_cast<CEntity**>(task.data()+0x18);put32(entities[oldIndex].data()+0x24,uint32(reinterpret_cast<uintptr_t>(&references[0])));}
                    else {references[0].m_pNext=&references[1];CReferences::pEmptyList=&references[0];}
                    expectedCommand=int8(uint8(argB))>int8(task[0x26])?uint8(argB):task[0x26];taskBefore=task;
                }else{fill(ped);fill(save);fill(matrix);memset(exits.data(),0,sizeof(exits));memset(pool.data(),0,sizeof(pool));
                    // Construct copia posiciones por bits y no usa x87.
                    put32(ped.data()+0x14,(scenario&1)?uint32(reinterpret_cast<uintptr_t>(matrix.data())):0);
                    put32(pool.data(),uint32(reinterpret_cast<uintptr_t>(exits.data())));exitPool=reinterpret_cast<uintptr_t>(pool.data());
                    const int selected=(scenario/2)%4;exits[selected][0x32]=uint8((scenario>>3)&1);
                    if(scenario&16){put32(exits[selected].data()+0x38,uint32(reinterpret_cast<uintptr_t>(exits[(selected+1)%4].data())));exits[(selected+1)%4][0x32]=uint8((scenario>>5)&1);}
                    put32(ped.data()+0x78C,(scenario&64)?0:uint32(reinterpret_cast<uintptr_t>(exits[selected].data())));
                    thisPointer=uint32(reinterpret_cast<uintptr_t>(save.data()));argA=uint32(reinterpret_cast<uintptr_t>(ped.data()));argc=1;pedBefore=ped;matrixBefore=matrix;
                }
                bool routeOk=false;const auto result=route.Invoke(candidate,[&]{routeOk=ReversingProbe::JumpsToDll(fn.address)==candidate;return Call(fn.address,thisPointer,argA,argB,argc,0xED12AA55u^uint32(scenario));});
                bool contract=true;
                if(!index){uint32 target;memcpy(&target,task.data()+0x18,4);contract=target==argA&&task[9]==1&&task[0x26]==expectedCommand&&(result.eax&0xFFu)==1;
                    auto masked=task;masked[9]=taskBefore[9];masked[0x26]=taskBefore[0x26];memcpy(masked.data()+0x18,taskBefore.data()+0x18,4);contract=contract&&masked==taskBefore;
                }else contract=ped==pedBefore&&matrix==matrixBefore;
                json value={{"eax",result.eax},{"ecx",result.ecx},{"edx",result.edx},{"return_bits",index?0u:(result.eax&0xFFu)},{"stack_restored",result.stack},{"nonvolatile_preserved",result.nonvolatile},{"nonvolatile_delta",result.nonvolatileDelta},{"route_ok",routeOk},{"contract_ok",contract}};
                if(!index){value["task"]=task;value["entities"]=entities;value["references"]=ReversingProbe::Bytes(references.data(),sizeof(references));value["free_head"]=uint32(reinterpret_cast<uintptr_t>(CReferences::pEmptyList));}
                else{value["saved_ped"]=save;value["ped_unchanged"]=ped==pedBefore;}
                restore();return value;
            };
            const auto original=invoke(false),candidate=invoke(true);const bool equal=original==candidate&&original["route_ok"]==true&&original["stack_restored"]==true&&original["nonvolatile_preserved"]==true&&original["contract_ok"]==true;
            passed&=equal;failures+=!equal;Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pair++},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
        }
        Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures}});
    }
    restore();bool restored=CReferences::pEmptyList==savedReferences&&exitPool==savedPool&&controlCode==ReversingProbe::Bytes(reinterpret_cast<void*>(0x61C5E0),5)&&constructCode==ReversingProbe::Bytes(reinterpret_cast<void*>(0x5D43D0),5);
    const bool controlRestored=control.Restore(),constructRestored=construct.Restore();restored=restored&&controlRestored&&constructRestored;
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
