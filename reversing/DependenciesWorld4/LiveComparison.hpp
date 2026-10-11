#pragma once
#include <fstream>
#include "Garages.h"
#include "RenderWare/D3DResourceSystem.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesWorld4Live {
struct Fn {const char* cls;const char* name;uintptr_t address,vtable;int slot,cases;};
inline constexpr Fn fns[]{
{"D3DResourceSystem","SetUseD3DResourceBuffering",0x730AC0,0,-1,512},
{"CGarages","FindGarageForObject",0x44A240,0,-1,4096}
};
inline bool done{};inline std::ofstream log;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
struct Result {uint32 eax,ecx,edx;bool abi;};
__declspec(noinline) inline Result Call(uintptr_t entry,uintptr_t arg,uint32 seed){
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
        mov ecx,41315926h
        mov edx,81928374h
        push arg
        mov eax,seed
        call entry
        add esp,4
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
inline uintptr_t Resolve(uintptr_t entry){for(int i=0;i<16&&*reinterpret_cast<const uint8*>(entry)==0xE9;++i)entry+=5+*reinterpret_cast<const int32*>(entry+1);return entry;}
inline bool SameProbeModule(uintptr_t body){HMODULE owner{},probe{};const DWORD flags=GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;return GetModuleHandleExA(flags,reinterpret_cast<LPCSTR>(body),&owner)&&GetModuleHandleExA(flags,reinterpret_cast<LPCSTR>(&Call),&probe)&&owner==probe;}
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciesworld4-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependenciesworld4-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesworld4");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    const auto savedGarages=ReversingProbe::Bytes(&CGarages::aGarages,sizeof(CGarages::aGarages));const int savedCount=CGarages::NumGarages;
    const auto savedBuffering=ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D607C),17);
    alignas(16)std::array<uint8,108> fpSaved{};SaveFP(fpSaved.data());const auto savedControl=_controlfp(0,0);const auto savedTop=Top();
    alignas(16)std::array<uint32,130> object{};alignas(16)std::array<uint32,34> matrix{};
    auto* obj=reinterpret_cast<CObject*>(&object[1]);auto* mat=reinterpret_cast<uint8*>(&matrix[1]);
    OriginalDependencies499::HookRoute setter("D3DResourceSystem",fns[0].name,fns[0].address,&D3DResourceSystem::SetUseD3DResourceBuffering);
    OriginalDependencies499::HookRoute finder("CGarages",fns[1].name,fns[1].address,&CGarages::FindGarageForObject);
    const uintptr_t radiusBody=std::bit_cast<uintptr_t>(static_cast<bool(CGarage::*)(CVector,float)>(&CGarage::IsPointInsideGarage));
    OriginalDependencies499::HookRoute radius("CGarage","IsPointInsideGarage-radius",0x4487D0,reinterpret_cast<void*>(radiusBody));
    // RH also patches the DLL ILThunk when OFF. Candidate must enable radius and prove its body.
    const bool registered=setter.Registered()&&finder.Registered()&&radius.Registered();bool passed=registered;int pair=0;
    Write({{"event","begin"},{"registered_hook",registered},{"layout",sizeof(CGarage)==0xD8},{"geometry_collaborator_modes","original_OFF_candidate_ON"},{"radius_resolved_body",radius.Body()},{"radius_probe_module",SameProbeModule(radius.Body())}});
    auto restore=[&]{memcpy(&CGarages::aGarages,savedGarages.data(),savedGarages.size());CGarages::NumGarages=savedCount;memcpy(reinterpret_cast<void*>(0x8D607C),savedBuffering.data(),savedBuffering.size());RestoreFP(fpSaved.data());};
    for(int index=0;index<2;++index){auto& route=index?finder:setter;const auto& fn=fns[index];int failures=0;
        for(int scenario=0;scenario<fn.cases;++scenario){
            auto run=[&](bool candidate){
                restore();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);_clearfp();const auto topBefore=Top();
                object.fill(0);matrix.fill(0);object.front()=0xFEEDFACE;object.back()=0xBAD0C0DE;matrix.front()=0x13579BDF;matrix.back()=0x2468ACE0;
                if(!index){*reinterpret_cast<uint8*>(0x8D6084)=uint8(scenario>>1);}
                else{
                    memset(&CGarages::aGarages,0xA7,sizeof(CGarages::aGarages));
                    CGarages::NumGarages=scenario%51;const int mode=(scenario/51)%8;
                    for(int n=0;n<50;++n){auto& g=CGarages::aGarages[n];
                        g.m_vPosn=CVector{float(n*3)-40.f,float(n%5)*2.f-4.f,-2.f};
                        g.m_vDirectionA=CVector2D{1.f,0.f};g.m_vDirectionB=CVector2D{0.f,1.f};
                        if((scenario/408+n)&1){g.m_vDirectionA=CVector2D{0.6f,0.8f};g.m_vDirectionB=CVector2D{-0.8f,0.6f};}
                        g.m_fWidth=float(4+n%9);g.m_fHeight=float(4+n%7);g.m_fTopZ=8.f;
                        if(mode==2){g.m_vPosn=CVector{-4,-4,-2};g.m_vDirectionA=CVector2D{1,0};g.m_vDirectionB=CVector2D{0,1};g.m_fWidth=8;g.m_fHeight=8;}
                    }
                    CVector pos{0,0,-2};
                    if(mode==0)pos=CVector{20000,-20000,100};
                    else if(CGarages::NumGarages>0){const int which=(scenario/51)%CGarages::NumGarages;const auto& g=CGarages::aGarages[which];
                        pos=CVector{g.m_vPosn.x+g.m_vDirectionA.x*g.m_fWidth*0.5f+g.m_vDirectionB.x*g.m_fHeight*0.5f,g.m_vPosn.y+g.m_vDirectionA.y*g.m_fWidth*0.5f+g.m_vDirectionB.y*g.m_fHeight*0.5f,g.m_vPosn.z};
                        if(mode==4)pos.z=g.m_fTopZ+7.000001f;
                        if(mode==5){pos.x=g.m_vPosn.x+g.m_vDirectionA.x*(g.m_fWidth+7.f);pos.y=g.m_vPosn.y+g.m_vDirectionA.y*(g.m_fWidth+7.f);}
                        if(mode==6){pos.x-=6.999999f;pos.y+=6.999999f;pos.z-=7.f;}
                        if(mode==7){pos.x+=0.000001f;pos.y-=0.000001f;pos.z=8.f;}
                    }
                    memcpy(reinterpret_cast<uint8*>(obj)+4,&pos,12);
                    if(scenario&1){memcpy(mat+0x30,&pos,12);const uintptr_t ptr=uintptr_t(mat);memcpy(reinterpret_cast<uint8*>(obj)+0x14,&ptr,4);}
                }
                const auto globalsBefore=ReversingProbe::Bytes(&CGarages::aGarages,sizeof(CGarages::aGarages));const int countBefore=CGarages::NumGarages;
                const auto objectBefore=object;const auto matrixBefore=matrix;const auto toggleBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D607C),17);
                const uint32 seed=0xA153EC41u^uint32(scenario)*2654435761u;bool routeOk=false,helperRouteOk=false;
                const auto result=radius.Invoke(index&&candidate,[&]{
                    helperRouteOk=ReversingProbe::JumpsToDll(0x4487D0)==bool(index&&candidate);
                    if(index&&candidate)helperRouteOk&=Resolve(radiusBody)==radius.Body()&&SameProbeModule(Resolve(radiusBody));
                    return route.Invoke(candidate,[&]{routeOk=ReversingProbe::JumpsToDll(fn.address)==candidate;return Call(fn.address,index?uintptr_t(obj):uintptr_t(scenario&1),seed);});
                });
                json value={{"eax",result.eax},{"ecx",result.ecx},{"edx",result.edx},{"route_ok",routeOk},{"stack_restored",result.abi},{"registers_preserved",result.abi},{"fp_preserved",Top()==topBefore&&(_controlfp(0,0)&(_MCW_PC|_MCW_RC))==(_PC_24|_RC_NEAR)},{"helper_route_ok",helperRouteOk}};
                value["fixture_unchanged"]=globalsBefore==ReversingProbe::Bytes(&CGarages::aGarages,sizeof(CGarages::aGarages))&&countBefore==CGarages::NumGarages&&objectBefore==object&&matrixBefore==matrix;
                if(index){value["return_index"]=int16(result.eax);value["return_range_ok"]=int16(result.eax)>=-1&&int16(result.eax)<countBefore;value["counter"]=countBefore;value["matrix_position"]=bool(scenario&1);value["guards_ok"]=object.front()==0xFEEDFACE&&object.back()==0xBAD0C0DE&&matrix.front()==0x13579BDF&&matrix.back()==0x2468ACE0;}
                else{auto expected=toggleBefore;expected[8]=uint8(scenario&1);value["buffering_bytes"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D607C),17);value["setter_exact_ok"]=expected==value["buffering_bytes"].get<std::vector<uint8>>()&&result.eax==((seed&0xFFFFFF00u)|uint32(scenario&1))&&result.ecx==0x41315926&&result.edx==0x81928374;value["guards_ok"]=expected==value["buffering_bytes"].get<std::vector<uint8>>();}
                restore();return value;
            };
            const auto original=run(false),candidate=run(true);bool equal=registered&&original==candidate;
            for(auto key:{"route_ok","stack_restored","registers_preserved","fp_preserved","helper_route_ok","fixture_unchanged","guards_ok"})equal&=original[key]==true;
            equal&=original[index?"return_range_ok":"setter_exact_ok"]==true;passed&=equal;failures+=!equal;
            Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
        }
        Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures}});
    }
    restore();const bool restored=setter.Restore()&&finder.Restore()&&radius.Restore()&&savedGarages==ReversingProbe::Bytes(&CGarages::aGarages,savedGarages.size())&&CGarages::NumGarages==savedCount&&savedBuffering==ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D607C),17)&&Top()==savedTop&&(_controlfp(0,0)&(_MCW_PC|_MCW_RC))==(savedControl&(_MCW_PC|_MCW_RC));
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
