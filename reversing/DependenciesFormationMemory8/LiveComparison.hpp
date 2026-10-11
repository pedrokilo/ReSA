#pragma once
#include <fstream>
#include "PointList.h"
#include "Formation.h"
#include "app/platform/platform.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesFormationMemory8Live {
struct Fn{const char* cls;const char* name;uintptr_t address,vtable;int slot,cases;};
inline constexpr Fn fns[]{
{"CFormation","FindNearestFreeDestination",0x69B1B0,0,-1,8192},
{"Rs","psGetMemoryFunctions",0x745510,0,-1,64}
};
inline bool done{};inline std::ofstream log;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
struct Result{uint32 value;bool abi;};
__declspec(noinline) inline Result Call(uintptr_t entry,uint32 x,uint32 y,uint32 z,uintptr_t total,bool memory){
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
        cmp memory,0
        jne invoke
        push total
        push z
        push y
        push x
    invoke:
        call entry
        mov value,eax
        cmp memory,0
        jne cleaned
        add esp,16
    cleaned:
        mov after,esp
        mov frameAfter,ebp
        mov rb,ebx
        mov rs,esi
        mov rd,edi
        pop edi
        pop esi
        pop ebx
    }
    return {value,before==after&&frameBefore==frameAfter&&rb==0x13579BDF&&rs==0x2468ACE0&&rd==0x10293847};
}
inline uint16 Top(){uint16 v{};__asm{fnstsw v}return v&0x3800;}
inline void SaveFP(void* p){__asm{mov eax,p}__asm{fnsave [eax]}__asm{frstor [eax]}}
inline void RestoreFP(void* p){__asm{mov eax,p}__asm{frstor [eax]}}
inline uint32 ReadMXCSR(){uint32 v{};__asm{stmxcsr v}return v;}
inline void WriteMXCSR(uint32 v){__asm{ldmxcsr v}}
inline bool CodePointer(uintptr_t p){MEMORY_BASIC_INFORMATION m{};if(VirtualQuery(reinterpret_cast<void*>(p),&m,sizeof(m))!=sizeof(m))return false;const auto protection=m.Protect&255;return m.State==MEM_COMMIT&&(protection==PAGE_EXECUTE||protection==PAGE_EXECUTE_READ||protection==PAGE_EXECUTE_READWRITE||protection==PAGE_EXECUTE_WRITECOPY);}
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciesformationmemory8-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependenciesformationmemory8-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesformationmemory8");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    static_assert(sizeof(CPointList)==0x13C);
    const auto pointsSaved=ReversingProbe::Bytes(&CFormation::m_Destinations,sizeof(CFormation::m_Destinations));const auto memorySaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D6228),sizeof(RwMemoryFunctions));
    alignas(16)std::array<uint8,108> fpSaved{};SaveFP(fpSaved.data());const auto controlSaved=_controlfp(0,0);const auto topSaved=Top();const auto mxcsrSaved=ReadMXCSR();
    auto restore=[&]{std::memcpy(&CFormation::m_Destinations,pointsSaved.data(),pointsSaved.size());RestoreFP(fpSaved.data());WriteMXCSR(mxcsrSaved);};
    std::array<std::unique_ptr<OriginalDependencies499::HookRoute>,2> routes;
    routes[0]=std::make_unique<OriginalDependencies499::HookRoute>(fns[0].cls,fns[0].name,fns[0].address,&CFormation::FindNearestFreeDestination);
    routes[1]=std::make_unique<OriginalDependencies499::HookRoute>(fns[1].cls,fns[1].name,fns[1].address,&psGetMemoryFunctions);
    Write({{"event","begin"},{"registered_hook",routes[0]->Registered()&&routes[1]->Registered()},{"collaborators_stubbed",false},{"game_collaborators",0}});
    bool passed=true;uint32 pair=0;
    for(int index=0;index<2;++index){const auto& fn=fns[index];uint32 failures=0;bool sawNegative=false,sawSelected=false,sawZeroCount=false,sawFullCount=false,sawTie=false;
        for(int scenario=0;scenario<fn.cases;++scenario){
            auto run=[&](bool candidate){restore();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);const auto topBefore=Top();
                std::array<uint32,3> total{0x13579BDF,0,0x2468ACE0};const float seeds[]{0,-1,17.25f,100000,0.125f,-999};total[1]=std::bit_cast<uint32>(seeds[(scenario/200)%6]);
                const uint32 count=uint32(scenario%25),mode=uint32((scenario/25)%8);CVector origin{float(scenario%11)*.25f-1,float(scenario%7)*-.5f,float(scenario%3)*.125f};
                if(index==0){auto& list=CFormation::m_Destinations;list.m_Count=count;for(int n=0;n<24;++n){const float delta=float(n+1)*.375f;list.m_Points[n]=CVector{origin.x+delta,origin.y-delta*.5f,origin.z+delta*.25f};list.m_PointHasBeenClaimed[n]=mode==1||(mode==2&&n%2!=0)||(mode==3&&n!=int(count)-1)||(mode==4&&n!=0);}
                    if(mode==5){for(int n=0;n<24;++n)list.m_Points[n]=CVector{origin.x+1,origin.y,origin.z};}
                    if(mode==6&&count>0){list.m_Points[count-1]=origin;}
                    if(mode==7){for(int n=0;n<24;++n)list.m_Points[n]=CVector{10000000.f+float(n)*1000,10000000.f,10000000.f};}
                    if((scenario/200)%8==7&&count>2)list.m_Points[1].x=std::bit_cast<float>(0x7FC12345u);
                }
                const auto pointsBefore=ReversingProbe::Bytes(&CFormation::m_Destinations,sizeof(CFormation::m_Destinations));bool routeOK=false;
                const auto result=routes[index]->Invoke(candidate,[&]{routeOK=ReversingProbe::JumpsToDll(fn.address)==candidate;return Call(fn.address,std::bit_cast<uint32>(origin.x),std::bit_cast<uint32>(origin.y),std::bit_cast<uint32>(origin.z),uintptr_t(&total[1]),index==1);});
                bool domain=index==1?result.value==0x8D6228:int32(result.value)>=-1&&int32(result.value)<int32(count);bool contract=true;
                if(index==0&&int32(result.value)>=0&&domain)contract=!CFormation::m_Destinations.m_PointHasBeenClaimed[result.value];
                if(index==0&&mode==5&&count>0)contract&=result.value==0;
                bool callbacks=true;const auto* table=reinterpret_cast<const uintptr_t*>(0x8D6228);for(size_t n=0;n<sizeof(RwMemoryFunctions)/sizeof(uintptr_t);++n)callbacks&=CodePointer(table[n]);
                json value={{"return_bits",result.value},{"cost_bits",total[1]},{"count",count},{"mode",mode},{"route_ok",routeOK},{"stack_restored",result.abi},{"registers_preserved",result.abi},{"fp_preserved",Top()==topBefore&&(_controlfp(0,0)&(_MCW_PC|_MCW_RC))==(_PC_24|_RC_NEAR)},{"return_domain_ok",domain},{"contract_ok",contract},{"callbacks_real",callbacks},{"readonly_ok",pointsBefore==ReversingProbe::Bytes(&CFormation::m_Destinations,sizeof(CFormation::m_Destinations))&&memorySaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D6228),sizeof(RwMemoryFunctions))},{"guards_ok",total.front()==0x13579BDF&&total.back()==0x2468ACE0},{"table",ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D6228),sizeof(RwMemoryFunctions))}};
                restore();return value;
            };
            const auto original=run(false),candidate=run(true);bool equal=routes[index]->Registered()&&original==candidate;for(auto key:{"route_ok","stack_restored","registers_preserved","fp_preserved","return_domain_ok","contract_ok","callbacks_real","readonly_ok","guards_ok"})equal&=original[key]==true;
            if(index==0){const auto value=int32(original["return_bits"].get<uint32>());sawNegative|=value==-1;sawSelected|=value>=0;sawZeroCount|=original["count"]==0;sawFullCount|=original["count"]==24;sawTie|=original["mode"]==5&&value==0;}
            passed&=equal;failures+=!equal;Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
        }
        const bool covered=index==1||(sawNegative&&sawSelected&&sawZeroCount&&sawFullCount&&sawTie);passed&=covered;Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"complete_paths",covered}});
    }
    restore();bool restored=pointsSaved==ReversingProbe::Bytes(&CFormation::m_Destinations,sizeof(CFormation::m_Destinations))&&memorySaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D6228),sizeof(RwMemoryFunctions))&&Top()==topSaved&&ReadMXCSR()==mxcsrSaved&&(_controlfp(0,0)&(_MCW_PC|_MCW_RC))==(controlSaved&(_MCW_PC|_MCW_RC));for(auto& route:routes)restored&=route->Restore();Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
