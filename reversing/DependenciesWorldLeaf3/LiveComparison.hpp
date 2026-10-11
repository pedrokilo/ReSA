#pragma once
#include <fstream>
#include "Pad.h"
#include "FxFrustumInfo.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesWorldLeaf3Live {
struct Fn {const char* cls;const char* name;uintptr_t address,vtable;int slot,cases;};
inline constexpr Fn fns[]{
{"CPad","GetCurrentKeyPressed",0x541490,0,-1,4096},
{"FxFrustumInfo_c","IsCollision",0x4AA030,0,-1,4096}
};
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
struct Result {uint32 eax,ecx,edx;bool abi;};
__declspec(noinline) inline Result Call(uintptr_t entry,void* object,void* arg,bool method){
    uint32 resultA{},resultC{},resultD{},before{},after{},b{},s{},d{},frameBefore{},frameAfter{};
    __asm {
        push ebx
        push esi
        push edi
        mov before,esp
        mov frameBefore,ebp
        mov ebx,13579BDFh
        mov esi,2468ACE0h
        mov edi,10293847h
        mov ecx,object
        mov edx,31415926h
        push arg
        call entry
        cmp method,0
        jne cleaned
        add esp,4
    cleaned:
        mov resultA,eax
        mov resultC,ecx
        mov resultD,edx
        mov after,esp
        mov frameAfter,ebp
        mov b,ebx
        mov s,esi
        mov d,edi
        pop edi
        pop esi
        pop ebx
    }
    return {resultA,resultC,resultD,before==after&&frameBefore==frameAfter&&b==0x13579BDF&&s==0x2468ACE0&&d==0x10293847};
}
inline uint16 Top(){uint16 v{};__asm {fnstsw v}return v&0x3800;}
inline void SaveFP(void* p){__asm {mov eax,p} __asm {fnsave [eax]} __asm {frstor [eax]}}
inline void RestoreFP(void* p){__asm {mov eax,p} __asm {frstor [eax]}}
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciesworldleaf3-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependenciesworldleaf3-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesworldleaf3");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    const auto keyboardSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB72F20),0x4E0);
    const auto controlSaved=_controlfp(0,0);const auto topSaved=Top();
    alignas(16) std::array<uint8,108> fpSaved{};SaveFP(fpSaved.data());
    alignas(16) std::array<uint32,29> geometry{}; // guards + 0x54 frustum + 0x14 sphere + guards
    uint32 keyGuard[]{0x13579BDF,0xFFFFFFFF,0x2468ACE0};
    auto* frustum=reinterpret_cast<FxFrustumInfo_c*>(&geometry[1]);
    auto* sphere=reinterpret_cast<FxSphere_c*>(&geometry[22]);
    const uintptr_t collisionBody=std::bit_cast<uintptr_t>(&FxFrustumInfo_c::IsCollision);
    OriginalDependencies499::HookRoute keys("CPad",fns[0].name,fns[0].address,&GetCurrentKeyPressed);
    OriginalDependencies499::HookRoute collision("FxFrustumInfo_c",fns[1].name,fns[1].address,reinterpret_cast<void*>(collisionBody));
    const bool registered=keys.Registered()&&collision.Registered();bool passed=registered;int pair=0;
    Write({{"event","begin"},{"registered_hook",registered},{"layout",sizeof(FxFrustumInfo_c)==0x54&&sizeof(FxSphere_c)==0x14}});
    for(int index=0;index<2;++index){auto& route=index?collision:keys;const auto& fn=fns[index];int failures=0;
        for(int scenario=0;scenario<fn.cases;++scenario){
            auto invoke=[&](bool candidate){
                memcpy(reinterpret_cast<void*>(0xB72F20),keyboardSaved.data(),keyboardSaved.size());
                RestoreFP(fpSaved.data());_controlfp(_PC_24,_MCW_PC);_clearfp();const auto topBefore=Top();
                keyGuard[0]=0x13579BDF;keyGuard[1]=0xFFFFFFFF;keyGuard[2]=0x2468ACE0;
                if(!index){
                    auto* old=reinterpret_cast<int16*>(0xB72F20);auto* current=reinterpret_cast<int16*>(0xB73190);
                    memset(old,0,0x4E0);
                    if(scenario<1248){const int k=scenario%312,mode=scenario/312;current[k]=mode==2?0:mode==3?-1:1;old[k]=mode==0?0:1;}
                    else {uint32 rng=uint32(scenario)*1664525u+1013904223u;for(int k=0;k<312;++k){rng=rng*1664525u+1013904223u;current[k]=(rng&7)==0?-1:0;old[k]=(rng&24)==0?1:0;}}
                }
                geometry.fill(0);geometry.front()=0xBAD0C0DE;geometry[27]=0xA153EC41;geometry[28]=0xFEEDBABE;
                if(index){
                    // Raw valid-layout fixture: planes need no allocation, vtable, or engine collaborator.
                    auto set=[&](int k,float f){geometry[k]=std::bit_cast<uint32>(f);};
                    set(1,0);set(2,0);set(3,0);set(4,8);geometry[5]=scenario;
                    for(int p=0;p<4;++p){set(6+p*4,float(p==0)-float(p==2));set(7+p*4,float(p==1)-float(p==3));set(8+p*4,0);set(9+p*4,float((scenario>>3)%7)-3.f);}
                    const float values[]{-16.f,-8.f,-2.f,-1.f,-0.f,0.f,1.f,2.f,8.f,16.f,std::bit_cast<float>(0x7FC00001u),std::bit_cast<float>(0x7F800000u)};
                    set(22,values[scenario%12]);set(23,values[(scenario/12)%12]);set(24,values[(scenario/144)%12]);set(25,values[(scenario/1728)%12]);
                    geometry[26]=scenario<2048?uint32(scenario%4):uint32(scenario)*2654435761u;
                    if(scenario>=3072){set(22,0);set(23,0);set(24,0);set(25,1);set(9+(scenario%4)*4,std::bit_cast<float>(0x7FC00001u));}
                }
                const auto fixtureBefore=geometry;
                const auto keyboardBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB72F20),0x4E0);
                bool routeOk=false;const auto result=route.Invoke(candidate,[&]{routeOk=ReversingProbe::JumpsToDll(fn.address)==candidate;return Call(fn.address,frustum,index?static_cast<void*>(sphere):static_cast<void*>(&keyGuard[1]),index!=0);});
                const bool fp=Top()==topBefore&&(_controlfp(0,0)&_MCW_PC)==_PC_24;
                json value={{"return_bits",result.eax},{"ecx",result.ecx},{"edx",result.edx},{"stack_restored",result.abi},{"registers_preserved",result.abi},{"route_ok",routeOk},{"fp_preserved",fp}};
                if(index){value["fixture"]=geometry;value["guards_ok"]=geometry.front()==0xBAD0C0DE&&geometry[27]==0xA153EC41&&geometry[28]==0xFEEDBABE;value["readonly_ok"]=std::equal(geometry.begin(),geometry.begin()+26,fixtureBefore.begin());value["semantic_return_ok"]=result.eax<=1;}
                else {value["output"]=keyGuard;value["guards_ok"]=keyGuard[0]==0x13579BDF&&keyGuard[2]==0x2468ACE0;value["keyboard"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB72F20),0x4E0);value["readonly_ok"]=keyboardBefore==value["keyboard"].get<std::vector<uint8>>();value["semantic_return_ok"]=result.eax==uintptr_t(&keyGuard[1]);}
                memcpy(reinterpret_cast<void*>(0xB72F20),keyboardSaved.data(),keyboardSaved.size());RestoreFP(fpSaved.data());return value;
            };
            const auto original=invoke(false),candidate=invoke(true);const bool equal=registered&&original==candidate&&original["route_ok"]==true&&original["stack_restored"]==true&&original["fp_preserved"]==true&&original["guards_ok"]==true&&original["readonly_ok"]==true&&original["semantic_return_ok"]==true;
            passed&=equal;failures+=!equal;Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pair++},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
        }
        Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures}});
    }
    memcpy(reinterpret_cast<void*>(0xB72F20),keyboardSaved.data(),keyboardSaved.size());RestoreFP(fpSaved.data());
    const bool restored=keys.Restore()&&collision.Restore()&&keyboardSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB72F20),0x4E0)&&Top()==topSaved&&(_controlfp(0,0)&_MCW_PC)==(controlSaved&_MCW_PC);
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
