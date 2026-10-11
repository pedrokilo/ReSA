#pragma once
#include <fstream>
#include "Interior/TaskInteriorSitInChair.h"
#include "TaskComplexWaitForPed.h"
#include "References.h"
#include "Reference.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesTaskCtor4Live {
struct Fn {const char* cls;const char* name;uintptr_t address;int vt,slot,cases;};
inline const Fn fns[]{
    {"CTaskInteriorSitInChair","Constructor",0x675C30,0,-1,2048},
    {"CTaskComplexWaitForPed","Constructor",0x683340,0,-1,2048}
};
inline bool done{};inline std::ofstream log;
inline void Write(const json& row){ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();}
struct Abi {uint32 eax,ecx,edx;bool stack,nonvolatile,seh;std::array<uint32,4> nonvolatileDelta;};
inline Abi Call(uintptr_t entry,uintptr_t object,uint32 argA,uint32 argB,uint32 argC,uint32 argD,int argc,uint32 seedD){
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
        cmp argc,4
        jne taskCtorThreeArgs
        push argD
    taskCtorThreeArgs:
        push argC
        push argB
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
    if(!std::filesystem::exists(dir/"dependenciestaskctor4-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependenciestaskctor4-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciestaskctor4");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    OriginalDependencies499::HookRoute sit("CTaskInteriorSitInChair","Constructor",0x675C30,FunctionToVoidPtr(&CTaskInteriorSitInChair::Constructor));
    OriginalDependencies499::HookRoute wait("CTaskComplexWaitForPed","Constructor",0x683340,FunctionToVoidPtr(&CTaskComplexWaitForPed::Constructor));
    Write({{"event","begin"},{"registered_hook",sit.Registered()&&wait.Registered()},{"collaborators_stubbed",false},
        {"reference_helper",{{"address",0x571B70},{"jumps_to_dll",ReversingProbe::JumpsToDll(0x571B70)},{"entry_bytes",ReversingProbe::Bytes(reinterpret_cast<void*>(0x571B70),5)}}}});
    const auto savedReferences=CReferences::pEmptyList;
    const auto sitCode=ReversingProbe::Bytes(reinterpret_cast<void*>(0x675C30),5),waitCode=ReversingProbe::Bytes(reinterpret_cast<void*>(0x683340),5);
    auto restore=[&]{CReferences::pEmptyList=savedReferences;};
    alignas(4) std::array<uint8,0x2C> task;
    alignas(4) std::array<uint8,0x800> ped;
    alignas(4) std::array<uint8,0x40> info;
    alignas(4) std::array<CReference,8> references;
    CEntity* unrelated=nullptr;
    auto put32=[](void* p,uint32 value){memcpy(p,&value,4);};
    bool passed=true;int pair=0;
    for(int index=0;index<2;++index){int failures=0;auto& route=index?wait:sit;const auto& fn=fns[index];
        for(int scenario=0;scenario<fn.cases;++scenario){
            auto invoke=[&](bool candidate){restore();uint32 seed=uint32(scenario)*2654435761u+0x1387FF21u;
                auto next=[&]{seed=seed*1664525u+1013904223u;return seed;};
                auto fill=[&](auto& array){auto* data=reinterpret_cast<uint8*>(array.data());for(size_t i=0;i<sizeof(array);++i)data[i]=uint8(next()>>24);};
                fill(task);fill(ped);fill(info);memset(references.data(),0,sizeof(references));
                const uint32 thisPointer=uint32(reinterpret_cast<uintptr_t>(task.data()));
                const uint32 pedPointer=uint32(reinterpret_cast<uintptr_t>(ped.data()));
                const uint32 infoPointer=uint32(reinterpret_cast<uintptr_t>(info.data()));
                uint32 argA,argB,argC,argD=0;auto expected=task;
                put32(expected.data(),index?0x8706F8:0x870314);put32(expected.data()+4,0);
                if(!index){argA=(scenario&1)?infoPointer:0;argB=next();argC=uint8(scenario);
                    put32(expected.data()+8,argA);put32(expected.data()+0xC,argB);expected[0x10]=uint8(argC);
                    put32(expected.data()+0x14,0);put32(expected.data()+0x18,0xFFFFFFFFu);
                    put32(expected.data()+0x1C,0);put32(expected.data()+0x20,0);
                    for(int off:{0x24,0x25,0x28,0x29,0x2A})expected[off]=0;
                }else{argA=pedPointer;const float radii[]{0.f,-0.f,0.25f,1.f,15.f,100.f};argB=std::bit_cast<uint32>(radii[scenario%6]);argC=next();argD=uint8(scenario&1);
                    put32(expected.data()+8,0);put32(expected.data()+0xC,argA);put32(expected.data()+0x10,argB);put32(expected.data()+0x14,argC);expected[0x18]=uint8(argD);
                    put32(expected.data()+0x1C,0);put32(expected.data()+0x20,0);expected[0x24]=0;expected[0x25]=0;
                    // Ped no nulo con lista propia y libres suficientes; nunca poda pools del juego.
                    ped[0x36]=3;put32(ped.data()+0x1C,0);ped[0x2E]=0;put32(ped.data()+0x24,0);
                    unrelated=reinterpret_cast<CEntity*>(ped.data());
                    for(int i=1;i<7;++i)references[i].m_pNext=&references[i+1];references[7].m_pNext=nullptr;
                    CReferences::pEmptyList=&references[1];
                    if(scenario&2){if(scenario&4)put32(task.data()+0xC,pedPointer);references[0].m_ppEntity=(scenario&4)?reinterpret_cast<CEntity**>(task.data()+0xC):&unrelated;put32(ped.data()+0x24,uint32(reinterpret_cast<uintptr_t>(&references[0])));}
                    else{references[0].m_pNext=&references[1];CReferences::pEmptyList=&references[0];}
                }
                const auto infoBefore=info;
                bool routeOk=false;const auto result=route.Invoke(candidate,[&]{routeOk=ReversingProbe::JumpsToDll(fn.address)==candidate;return Call(fn.address,thisPointer,argA,argB,argC,argD,index?4:3,0xED12AA55u^uint32(scenario));});
                const bool contract=task==expected&&result.eax==thisPointer&&info==infoBefore;
                json value={{"eax",result.eax},{"ecx",result.ecx},{"edx",result.edx},{"return_bits",result.eax},{"stack_restored",result.stack},{"nonvolatile_preserved",result.nonvolatile},{"seh_restored",result.seh},{"nonvolatile_delta",result.nonvolatileDelta},{"route_ok",routeOk},{"contract_ok",contract},{"object",task}};
                if(index){value["ped"]=ped;value["references"]=ReversingProbe::Bytes(references.data(),sizeof(references));value["free_head"]=uint32(reinterpret_cast<uintptr_t>(CReferences::pEmptyList));}
                restore();return value;
            };
            const auto original=invoke(false),candidate=invoke(true);const bool equal=original==candidate&&original["route_ok"]==true&&original["stack_restored"]==true&&original["nonvolatile_preserved"]==true&&original["seh_restored"]==true&&original["contract_ok"]==true;
            passed&=equal;failures+=!equal;Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pair++},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
        }
        Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures}});
    }
    restore();bool restored=CReferences::pEmptyList==savedReferences&&sitCode==ReversingProbe::Bytes(reinterpret_cast<void*>(0x675C30),5)&&waitCode==ReversingProbe::Bytes(reinterpret_cast<void*>(0x683340),5);
    const bool sitRestored=sit.Restore(),waitRestored=wait.Restore();restored=restored&&sitRestored&&waitRestored;
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
