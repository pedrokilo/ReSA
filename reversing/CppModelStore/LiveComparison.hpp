#pragma once
#include <fstream>
#include "ModelInfo.h"
#include "MemoryMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace CppModelStoreLive {
struct Fn { const char* cls; const char* name; uintptr_t address; int vt,slot,cases; };
inline const Fn fns[]{
    {"CModelInfo","Get2dEffectStore",0x4C5A60,0,-1,1024},
    {"CMemoryMgr","InitScratchPad",0x72F480,0,-1,1024}
};
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row){ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();}
struct Abi { uint32 eax,ecx,edx; bool stack; };
inline Abi Call(uintptr_t entry,uint32 seedC,uint32 seedD) {
    uint32 resultA,resultC,resultD,before,after;
    __asm {
        mov before,esp
        mov ecx,seedC
        mov edx,seedD
        call entry
        mov resultA,eax
        mov resultC,ecx
        mov resultD,edx
        mov after,esp
    }
    return {resultA,resultC,resultD,before==after};
}
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cppmodelstore-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cppmodelstore-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cppmodelstore");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;
    OriginalDependencies499::HookRoute model("CModelInfo","Get2dEffectStore",0x4C5A60,&CModelInfo::Get2dEffectStore);
    OriginalDependencies499::HookRoute scratch("CMemoryMgr","InitScratchPad",0x72F480,&CMemoryMgr::InitScratchPad);
    Write({{"event","begin"},{"registered_hook",model.Registered()&&scratch.Registered()}});
    const auto heapSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC87BD0),0x48);
    const auto memSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC8E0C8),0x4000);
    const auto modelCode=ReversingProbe::Bytes(reinterpret_cast<void*>(0x4C5A60),5);
    const auto scratchCode=ReversingProbe::Bytes(reinterpret_cast<void*>(0x72F480),5);
    auto restore=[&]{memcpy(reinterpret_cast<void*>(0xC87BD0),heapSaved.data(),heapSaved.size());memcpy(reinterpret_cast<void*>(0xC8E0C8),memSaved.data(),memSaved.size());};
    bool passed=true;int pair=0;
    for(int index=0;index<2;++index){
        auto& route=index? scratch:model;
        const auto& fn=fns[index];int failures=0;
        for(int scenario=0;scenario<fn.cases;++scenario){
            auto invoke=[&](bool candidate){
                restore();
                uint32 random=uint32(scenario)*2654435761u+0xA153EC41u;
                if(index){
                    // Estado privado temporal coherente: lista circular, sin asignaciones pendientes.
                    auto* heap=reinterpret_cast<uint8*>(0xC87BD0);
                    auto* mem=reinterpret_cast<uint8*>(0xC8E0C8);
                    for(size_t i=0;i<0x48;++i){random=random*1664525u+1013904223u;heap[i]=uint8(random>>24);}
                    for(size_t i=0;i<0x4000;++i){random=random*1664525u+1013904223u;mem[i]=uint8(random>>24);}
                    *reinterpret_cast<uint32*>(heap+0x18)=0xC87BF0;
                    *reinterpret_cast<uint32*>(heap+0x1C)=0xC87BF0;
                    *reinterpret_cast<uint32*>(heap+0x30)=0xC87BD8;
                    *reinterpret_cast<uint32*>(heap+0x34)=0xC87BD8;
                }
                const uint32 seedC=0xE012AA55u^uint32(scenario)*1664525u;
                const uint32 seedD=0x59137FFFu^uint32(scenario)*1013904223u;
                bool routeOk=false;
                const auto result=route.Invoke(candidate,[&]{routeOk=ReversingProbe::JumpsToDll(fn.address)==candidate;return Call(fn.address,seedC,seedD);});
                const bool regs=index?(result.eax==0xC87BD8&&result.ecx==0xC8E0C8&&result.edx==0xC87BF0):(result.ecx==seedC&&result.edx==seedD);
                json value={{"return_bits",result.eax},{"ecx",result.ecx},{"edx",result.edx},{"registers_preserved",regs},{"stack_restored",result.stack},{"route_ok",routeOk}};
                if(index){value["heap_bytes"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC87BD0),0x48);value["scratch_bytes"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC8E0C8),0x4000);}
                else value["store_address_ok"]=result.eax==0xB4C2D8;
                restore();return value;
            };
            const auto original=invoke(false),candidate=invoke(true);
            const bool equal=original==candidate&&original["route_ok"]==true&&original["stack_restored"]==true&&original["registers_preserved"]==true&&(!index?original["store_address_ok"]==true:true);
            passed&=equal;failures+=!equal;
            Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pair++},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
        }
        Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures}});
    }
    restore();
    const bool dataRestored=heapSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC87BD0),0x48)&&memSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC8E0C8),0x4000)&&modelCode==ReversingProbe::Bytes(reinterpret_cast<void*>(0x4C5A60),5)&&scratchCode==ReversingProbe::Bytes(reinterpret_cast<void*>(0x72F480),5);
    const bool modelRestored=model.Restore(),scratchRestored=scratch.Restore();
    Write({{"event",passed&&dataRestored&&modelRestored&&scratchRestored?"complete":"error"},{"pairs",pair},{"state_restored",dataRestored&&modelRestored&&scratchRestored}});
}
}
