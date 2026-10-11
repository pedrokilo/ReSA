#pragma once
#include <fstream>
#include "Pad.h"
#include "Cam.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesPadCamera7Live {
struct Fn{const char* cls;const char* name;uintptr_t address,vtable;int slot,cases;};
inline constexpr Fn fns[]{
{"CPad","GetCameraCycleUp",0x5404A0,0,-1,4096},
{"CPad","GetMouseCameraStickHorizontal",0x540E80,0,-1,8192},
{"CPad","GetMouseCameraStickVertical",0x540F80,0,-1,8192}
};
inline bool done{};inline std::ofstream log;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
struct Result{uint32 value;bool abi;};
__declspec(noinline) inline Result Call(uintptr_t entry,uintptr_t pad,bool cycle){
    uint32 value{},before{},after{},rb{},rs{},rd{},frameBefore{},frameAfter{};
    __asm {
        push ebx
        push esi
        push edi
        mov before,esp
        mov frameBefore,ebp
        mov ebx,13579BDFh
        mov esi,2468ACE0h
        mov edi,10293847h
        mov ecx,pad
        call entry
        mov value,eax
        mov after,esp
        mov frameAfter,ebp
        mov rb,ebx
        mov rs,esi
        mov rd,edi
        pop edi
        pop esi
        pop ebx
    }
    return {value&(cycle?255u:65535u),before==after&&frameBefore==frameAfter&&rb==0x13579BDF&&rs==0x2468ACE0&&rd==0x10293847};
}
inline uint16 Top(){uint16 v{};__asm{fnstsw v}return v&0x3800;}
inline void SaveFP(void* p){__asm{mov eax,p}__asm{fnsave [eax]}__asm{frstor [eax]}}
inline void RestoreFP(void* p){__asm{mov eax,p}__asm{frstor [eax]}}
inline uint32 ReadMXCSR(){uint32 v{};__asm{stmxcsr v}return v;}
inline void WriteMXCSR(uint32 v){__asm{ldmxcsr v}}
inline uintptr_t Resolve(uintptr_t e){for(int n=0;n<16&&*reinterpret_cast<const uint8*>(e)==0xE9;++n)e+=5+*reinterpret_cast<const int32*>(e+1);return e;}
inline bool ModuleIsProbe(uintptr_t e){HMODULE owner{},probe{};const DWORD flags=GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;return GetModuleHandleExA(flags,reinterpret_cast<LPCSTR>(e),&owner)&&GetModuleHandleExA(flags,reinterpret_cast<LPCSTR>(&Call),&probe)&&owner==probe;}
inline bool ActualCRT(){HMODULE owner{};const DWORD flags=GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;return GetModuleHandleExA(flags,reinterpret_cast<LPCSTR>(Resolve(0x821B40)),&owner)&&owner==GetModuleHandleA(nullptr);}
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciespadcamera7-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependenciespadcamera7-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciespadcamera7");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    const auto cameraSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB6F19C),sizeof(CCam));
    const auto mouseSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB73400),0x60);
    const uint8 mouseFlagSaved=*reinterpret_cast<uint8*>(0xB6EC2E);
    alignas(16)std::array<uint8,108> fpSaved{};SaveFP(fpSaved.data());const auto controlSaved=_controlfp(0,0);const auto topSaved=Top();const auto mxcsrSaved=ReadMXCSR();
    auto restore=[&]{std::memcpy(reinterpret_cast<void*>(0xB6F19C),cameraSaved.data(),cameraSaved.size());std::memcpy(reinterpret_cast<void*>(0xB73400),mouseSaved.data(),mouseSaved.size());*reinterpret_cast<uint8*>(0xB6EC2E)=mouseFlagSaved;RestoreFP(fpSaved.data());WriteMXCSR(mxcsrSaved);};
    std::array<std::unique_ptr<OriginalDependencies499::HookRoute>,3> routes;
    routes[0]=std::make_unique<OriginalDependencies499::HookRoute>(fns[0].cls,fns[0].name,fns[0].address,FunctionToVoidPtr(&CPad::GetCameraCycleUp));
    routes[1]=std::make_unique<OriginalDependencies499::HookRoute>(fns[1].cls,fns[1].name,fns[1].address,FunctionToVoidPtr(&CPad::GetMouseCameraStickHorizontal));
    routes[2]=std::make_unique<OriginalDependencies499::HookRoute>(fns[2].cls,fns[2].name,fns[2].address,FunctionToVoidPtr(&CPad::GetMouseCameraStickVertical));
    const uintptr_t helperPointer=std::bit_cast<uintptr_t>(&CCam::Using3rdPersonMouseCam);
    OriginalDependencies499::HookRoute helper("CCam","Using3rdPersonMouseCam",0x50A850,FunctionToVoidPtr(&CCam::Using3rdPersonMouseCam));
    Write({{"event","begin"},{"registered_hook",routes[0]->Registered()&&routes[1]->Registered()&&routes[2]->Registered()},{"collaborators_stubbed",false},{"helper_registered",helper.Registered()},{"library_route_ok",ActualCRT()}});
    bool passed=true;uint32 pair=0;
    for(int index=0;index<3;++index){const auto& fn=fns[index];uint32 failures=0;std::array<uint32,3> branches{};
        for(int scenario=0;scenario<fn.cases;++scenario){
            auto run=[&](bool candidate){restore();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);const auto topBefore=Top();
                alignas(4)std::array<uint8,sizeof(CPad)+8> pad{};pad.fill(0xA7);uint32 guardA=0x13579BDF,guardB=0x2468ACE0;std::memcpy(pad.data(),&guardA,4);std::memcpy(pad.data()+pad.size()-4,&guardB,4);auto* body=pad.data()+4;
                auto word=[&](size_t off,int16 v){std::memcpy(body+off,&v,2);};uint32 expected{},branch{};
                if(index==0){const uint16 modes[]{0,1,2,3,4,7,255,65535};const uint16 mode=modes[scenario%8];word(0x10A,int16(mode));const int values[]{0,1,-1,32767,-32768,2,-2,255};
                    const int a=values[(scenario/8)%8],b=values[(scenario/64)%8],c=values[(scenario/512)%8],d=values[(scenario/16)%8];word(0x1A,int16(a));word(0x4A,int16(b));word(0x10,int16(c));word(0x40,int16(d));
                    expected=mode<=3&&((mode==1?c:a)!=0)&&((mode==1?d:b)==0);branch=mode>3?2:expected?1:0;
                }else{
                    const int sample=scenario%128;const int16 edges[]{-32768,-32767,-301,-257,-201,-200,-151,-150,-102,-101,-100,-99,-52,-51,-50,-49,-2,-1,0,1,2,49,50,51,52,99,100,101,102,150,151,200,201,257,301,32766,32767};
                    const int16 input=sample<int(std::size(edges))?edges[sample]:int16(uint16(uint32(sample)*40503u+7919u));
                    const bool inversion=(scenario/128)%2!=0,enabled=(scenario/256)%2!=0;const uint16 modes[]{0,4,7,65535};const uint16 mode=modes[(scenario/512)%4];const bool disable=(scenario/2048)%2!=0,button=(scenario/4096)%2!=0;
                    *reinterpret_cast<int16*>(0xB7345C)=input;*reinterpret_cast<int16*>(0xB7345E)=input;*reinterpret_cast<uint8*>(0xB73402)=uint8(inversion);*reinterpret_cast<uint8*>(0xB6EC2E)=uint8(enabled);*reinterpret_cast<uint16*>(0xB6F1A8)=mode;word(0x10E,int16(disable));word(0x26,int16(button));
                    const int i=index==2&&inversion?int(int16(-int(input))):int(input);int result=0;
                    if(std::abs(i)>100&&(disable||!button)){result=i>0?i-50:i+50;branch=1;}
                    else if(enabled&&mode==4&&std::abs(i)>50){result=(i>0?i-50:i+50)/2;branch=2;}
                    expected=uint16(int16(result));
                }
                const auto padBefore=pad;const auto cameraBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB6F19C),sizeof(CCam));const auto mouseBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB73400),0x60);const auto flagBefore=*reinterpret_cast<uint8*>(0xB6EC2E);
                bool routeOK=false,helperOK=true;Result result{};
                auto call=[&]{return routes[index]->Invoke(candidate,[&]{routeOK=ReversingProbe::JumpsToDll(fn.address)==candidate;return Call(fn.address,uintptr_t(body),index==0);});};
                if(index==0)result=call();else result=helper.Invoke(candidate,[&]{helperOK=helper.Registered()&&ReversingProbe::JumpsToDll(0x50A850)==candidate;if(candidate)helperOK&=Resolve(helperPointer)==helper.Body()&&ModuleIsProbe(Resolve(helperPointer));return call();});
                json value={{"return_bits",result.value},{"expected",expected},{"branch",branch},{"route_ok",routeOK},{"helper_route_ok",helperOK},{"library_route_ok",ActualCRT()},{"stack_restored",result.abi},{"registers_preserved",result.abi},{"fp_preserved",Top()==topBefore&&(_controlfp(0,0)&(_MCW_PC|_MCW_RC))==(_PC_24|_RC_NEAR)},{"contract_ok",result.value==expected},{"readonly_ok",pad==padBefore&&cameraBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB6F19C),sizeof(CCam))&&mouseBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB73400),0x60)&&flagBefore==*reinterpret_cast<uint8*>(0xB6EC2E)},{"guards_ok",pad==padBefore}};
                restore();return value;
            };
            const auto original=run(false),candidate=run(true);bool equal=routes[index]->Registered()&&original==candidate;for(auto key:{"route_ok","helper_route_ok","library_route_ok","stack_restored","registers_preserved","fp_preserved","contract_ok","readonly_ok","guards_ok"})equal&=original[key]==true;
            ++branches[original["branch"].get<uint32>()];passed&=equal;failures+=!equal;Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
        }
        const bool covered=branches[0]>0&&branches[1]>0&&branches[2]>0;passed&=covered;Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"complete_paths",covered},{"branch_counts",branches}});
    }
    restore();bool restored=helper.Restore();for(auto& route:routes)restored&=route->Restore();restored&=cameraSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB6F19C),sizeof(CCam))&&mouseSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB73400),0x60)&&mouseFlagSaved==*reinterpret_cast<uint8*>(0xB6EC2E)&&Top()==topSaved&&ReadMXCSR()==mxcsrSaved&&(_controlfp(0,0)&(_MCW_PC|_MCW_RC))==(controlSaved&(_MCW_PC|_MCW_RC));
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
