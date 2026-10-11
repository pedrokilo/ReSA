#pragma once
#include <fstream>
#include "TaskSimpleFight.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesFightLeaf2Live {
struct Fn { const char* cls; const char* name; uintptr_t address; int vt,slot,cases; };
inline const Fn fns[]{
    {"CTaskSimpleFight","GetComboAnimGroupID",0x4ABDA0,0,-1,2048},
    {"CTaskSimpleFight","IsComboSet",0x4ABDC0,0,-1,2048},
    {"CTaskSimpleFight","IsHitComboSet",0x4ABDF0,0,-1,2048}
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
    if(!std::filesystem::exists(dir/"dependenciesfightleaf2-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependenciesfightleaf2-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesfightleaf2");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;
    OriginalDependencies499::HookRoute group("CTaskSimpleFight","GetComboAnimGroupID",0x4ABDA0,FunctionToVoidPtr(&CTaskSimpleFight::GetComboAnimGroupID));
    OriginalDependencies499::HookRoute combo("CTaskSimpleFight","IsComboSet",0x4ABDC0,FunctionToVoidPtr(&CTaskSimpleFight::IsComboSet));
    OriginalDependencies499::HookRoute hit("CTaskSimpleFight","IsHitComboSet",0x4ABDF0,FunctionToVoidPtr(&CTaskSimpleFight::IsHitComboSet));
    std::array<OriginalDependencies499::HookRoute*,3> routes{&group,&combo,&hit};
    Write({{"event","begin"},{"registered_hook",group.Registered()&&combo.Registered()&&hit.Registered()}});
    const auto tableSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC170D0),12*0x88);
    std::array<std::vector<uint8>,3> code;
    for(int i=0;i<3;++i)code[i]=ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),5);
    auto restore=[&]{memcpy(reinterpret_cast<void*>(0xC170D0),tableSaved.data(),tableSaved.size());};
    bool passed=true;int pair=0;
    alignas(4) std::array<uint8,0x28> object;
    for(int index=0;index<3;++index){
        const auto& fn=fns[index];int failures=0;
        for(int scenario=0;scenario<fn.cases;++scenario){
            auto invoke=[&](bool candidate){
                restore();
                uint32 random=uint32(scenario)*2654435761u+0xA153EC41u;
                auto next=[&]{random=random*1664525u+1013904223u;return random;};
                for(auto& byte:object)byte=uint8(next()>>24);
                auto* table=reinterpret_cast<uint8*>(0xC170D0);
                for(size_t i=0;i<12*0x88;++i)table[i]=uint8(next()>>24);
                const int8 selected=int8((scenario%144)-128);
                const uint8 move=uint8(scenario*73+scenario/144);
                object[0x24]=uint8(selected);object[0x25]=move;
                const uint16 patterns[]{0,0xFFFF,1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,0x5555,0xAAAA};
                for(int i=0;i<12;++i)*reinterpret_cast<uint16*>(table+i*0x88+0x84)=patterns[(scenario+i)%20];
                const int tableIndex=std::max(int(selected)-4,0);
                const uint16 flags=*reinterpret_cast<uint16*>(table+tableIndex*0x88+0x84);
                const uint32 groupValue=*reinterpret_cast<uint32*>(table+tableIndex*0x88);
                const auto before=object;
                const auto tableBefore=ReversingProbe::Bytes(table,12*0x88);
                const uint32 thisPointer=uint32(reinterpret_cast<uintptr_t>(object.data()));
                const uint32 seedD=0x59137FFFu^uint32(scenario)*1013904223u;
                bool routeOk=false;
                const auto result=routes[index]->Invoke(candidate,[&]{routeOk=ReversingProbe::JumpsToDll(fn.address)==candidate;return Call(fn.address,thisPointer,seedD);});
                const uint32 shift=move&31u;
                const uint32 expectedC=index==0?(selected<4?0u:0xFFFFFFFFu):((thisPointer&0xFFFFFF00u)|move);
                const uint32 expectedD=index==0?seedD:index==1?(0x10u<<shift):(int8(move)>2?seedD:(0x1000u<<shift));
                const uint32 expectedA=index==0?groupValue:index==1?((uint32(flags)&0xFFFFFF00u)|uint32((flags&(0x10u<<shift))!=0)):uint32(int8(move)<=2&&(flags&(0x1000u<<shift))!=0);
                const bool abi=result.eax==expectedA&&result.ecx==expectedC&&result.edx==expectedD;
                const bool unchanged=before==object&&tableBefore==ReversingProbe::Bytes(table,12*0x88);
                json value={{"return_bits",index==1?(result.eax&0xFFu):result.eax},{"eax",result.eax},{"ecx",result.ecx},{"edx",result.edx},{"registers_preserved",abi},{"stack_restored",result.stack},{"route_ok",routeOk},{"state_unchanged",unchanged}};
                restore();return value;
            };
            const auto original=invoke(false),candidate=invoke(true);
            const bool equal=original==candidate&&original["route_ok"]==true&&original["stack_restored"]==true&&original["registers_preserved"]==true&&original["state_unchanged"]==true;
            passed&=equal;failures+=!equal;
            Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pair++},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
        }
        Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures}});
    }
    restore();
    bool restored=tableSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC170D0),12*0x88);
    for(int i=0;i<3;++i)restored=(code[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),5))&&restored;
    for(auto* route:routes){const bool hookRestored=route->Restore();restored=hookRestored&&restored;}
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
