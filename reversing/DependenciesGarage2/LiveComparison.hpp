#pragma once
#include <fstream>
#include "Garage.h"
#include "Garages.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesGarage2Live {
inline bool done{};inline std::ofstream log;
struct Fn {const char* cls;const char* name;uintptr_t address,vtable;int slot,cases;};
inline constexpr Fn fns[]{
{"CGarage","IsPointInsideGarage-point",0x448740,0,-1,1024},
{"CGarage","IsPointInsideGarage-radius",0x4487D0,0,-1,1280},
{"CGarages","FindSafeHouseIndexForGarageType",0x4489F0,0,-1,256}
};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
__declspec(noinline) inline uint32 GeoCall(uintptr_t entry,CGarage* garage,float x,float y,float z,float radius,bool expanded,bool& abi,uint32& ecxAfter,uint32& edxAfter){
    uint32 result{},before{},after{},a{},b{},c{},d{},g{};
    __asm {
        push ebx
        push esi
        push edi
        mov before,esp
        mov ebx,13579BDFh
        mov esi,2468ACE0h
        mov edi,10293847h
        mov ecx,garage
        mov edx,31415926h
        cmp expanded,0
        je pointArgs
        push radius
    pointArgs:
        push z
        push y
        push x
        call entry
        mov result,eax
        mov after,esp
        mov a,ebx
        mov b,esi
        mov c,edi
        mov d,edx
        mov g,ecx
        pop edi
        pop esi
        pop ebx
    }
    abi=before==after&&a==0x13579BDF&&b==0x2468ACE0&&c==0x10293847&&d==0x31415926&&(!expanded||g==uintptr_t(garage));
    ecxAfter=g;edxAfter=d;
    return result&255;
}
__declspec(noinline) inline uint32 CdeclCall(uintptr_t entry,uintptr_t arg,bool& abi){
    uint32 callResult{},before{},after{},a{},b{},c{};
    __asm {
        push ebx
        push esi
        push edi
        mov before,esp
        mov ebx,13579BDFh
        mov esi,2468ACE0h
        mov edi,10293847h
        push arg
        call entry
        add esp,4
        mov callResult,eax
        mov after,esp
        mov a,ebx
        mov b,esi
        mov c,edi
        pop edi
        pop esi
        pop ebx
    }
    abi=before==after&&a==0x13579BDF&&b==0x2468ACE0&&c==0x10293847;
    return callResult;
}
inline uint16 X87Status(){uint16 s{};__asm { fnstsw s } return s;}
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"depsgarage2-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"depsgarage2-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("depsgarage2");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    const auto savedGroups=ReversingProbe::Bytes(&CGarages::aGarages,sizeof(CGarages::aGarages));const auto savedNum=CGarages::NumGarages;const auto savedFP=_controlfp(0,0);
    const uintptr_t pointBody=std::bit_cast<uintptr_t>(static_cast<bool(CGarage::*)(CVector)>(&CGarage::IsPointInsideGarage));
    const uintptr_t radiusBody=std::bit_cast<uintptr_t>(static_cast<bool(CGarage::*)(CVector,float)>(&CGarage::IsPointInsideGarage));
    std::array<std::unique_ptr<OriginalDependencies499::HookRoute>,3> routes;
    routes[0]=std::make_unique<OriginalDependencies499::HookRoute>("CGarage",fns[0].name,fns[0].address,reinterpret_cast<void*>(pointBody));
    routes[1]=std::make_unique<OriginalDependencies499::HookRoute>("CGarage",fns[1].name,fns[1].address,reinterpret_cast<void*>(radiusBody));
    routes[2]=std::make_unique<OriginalDependencies499::HookRoute>("CGarages",fns[2].name,fns[2].address,&CGarages::FindSafeHouseIndexForGarageType);
    bool passed=true,registered=true;for(auto& r:routes)registered&=r->Registered();passed&=registered;
    Write({{"event","begin"},{"known",passed},{"registered_hook",registered},{"layout",sizeof(CGarage)==0xD8},{"scope","two_geometry_methods_and_safehouse;FindGarage_original_only_composed_observer"}});
    alignas(16)std::array<uint8,sizeof(CGarage)> garageBytes{};alignas(16)std::array<uint8,0x200> objectBytes{};alignas(16)std::array<uint8,0x80> matrixBytes{};
    auto* garage=reinterpret_cast<CGarage*>(garageBytes.data());unsigned pair=0;
    for(unsigned fn=0;fn<3;++fn){unsigned failures=0;
        for(unsigned i=0;i<unsigned(fns[fn].cases);++i){
            const bool composed=fn==1&&i>=1024;
            auto run=[&](bool candidate){
                _controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);garageBytes.fill(0xA7);objectBytes.fill(0);matrixBytes.fill(0);
                garage->m_vPosn=CVector{-2.0f,3.0f,-1.0f};garage->m_vDirectionA=CVector2D{1.0f,0.0f};garage->m_vDirectionB=CVector2D{0.0f,1.0f};garage->m_fTopZ=6.0f;garage->m_fWidth=8.0f;garage->m_fHeight=5.0f;
                if(i&64){garage->m_vDirectionA=CVector2D{0.6f,0.8f};garage->m_vDirectionB=CVector2D{-0.8f,0.6f};}
                const std::array<float,8> px{-20,-2,-1.9999998f,0,5.9999995f,6,6.0000005f,20};
                const std::array<float,8> py{-20,3,3.0000002f,4,7.9999995f,8,8.000001f,20};
                const std::array<float,4> pz{-1.0000001f,-1,6,6.0000005f};
                float x=px[i&7],y=py[(i>>3)&7],z=pz[(i>>6)&3];
                const std::array<float,4> radios{0,0.5f,7,-0.5f};float radius=radios[(i>>8)&3];
                if(i&512){
                    // Estres x87: NaN (carga y signo distintos, senalizante), +-inf, +-FLT_MAX y subnormal en una de las cuatro entradas; ademas direcciones del garaje enormes o diminutas (productos con exponente extendido).
                    static const std::array<uint32,8> stress{0x7FC12345u,0xFFC00001u,0x7FA00001u,0x7F800000u,0xFF800000u,0x7F7FFFFFu,0xFF7FFFFFu,0x00000001u};const float sv=std::bit_cast<float>(stress[(i>>2)&7]);
                    switch(i&3){case 0:x=sv;break;case 1:y=sv;break;case 2:z=sv;break;case 3:radius=sv;break;}
                    if(i&16){const float big=(i&32)?3.0e38f:1.0e-38f;garage->m_vDirectionA=CVector2D{big,-big};garage->m_vDirectionB=CVector2D{-big,big};}
                }
                if(composed){
                    std::memset(&CGarages::aGarages,0xA7,sizeof(CGarages::aGarages));CGarages::NumGarages=(i&4)?0:3;
                    for(unsigned n=0;n<3;++n){auto& g=CGarages::aGarages[n];std::memcpy(&g,garageBytes.data(),garageBytes.size());g.m_vPosn=CVector{float(n*4),0,-2};g.m_vDirectionA=CVector2D{1,0};g.m_vDirectionB=CVector2D{0,1};g.m_fWidth=8;g.m_fHeight=8;g.m_fTopZ=6;}
                    CVector pos{float((i-1024)%32)-8,float((i>>3)&7),1};std::memcpy(objectBytes.data()+4,&pos,12);
                    if(i&1){std::memcpy(matrixBytes.data()+0x30,&pos,12);const uintptr_t m=uintptr_t(matrixBytes.data());std::memcpy(objectBytes.data()+0x14,&m,4);}
                }
                const auto beforeGarage=garageBytes;const auto beforeObject=objectBytes;const auto beforeMatrix=matrixBytes;const auto beforeGlobals=ReversingProbe::Bytes(&CGarages::aGarages,sizeof(CGarages::aGarages));const int beforeNum=CGarages::NumGarages;
                bool abi=true,route{};uint32 ecxAfter{},edxAfter{};const uint16 fpBefore=X87Status();
                int32 ret=routes[fn]->Invoke(candidate,[&]()->int32{
                    route=ReversingProbe::JumpsToDll(fns[fn].address)==candidate;
                    if(fn==2)return int32(CdeclCall(fns[fn].address,i,abi));
                    if(composed)return int16(CdeclCall(0x44A240,uintptr_t(objectBytes.data()),abi));
                    return GeoCall(fns[fn].address,garage,x,y,z,radius,fn==1,abi,ecxAfter,edxAfter);
                });
                const uint16 fpAfter=X87Status();
                const bool x87Balanced=(fpBefore&0x3800)==(fpAfter&0x3800);
                // Restore the global fixture before Write pumps real-world messages.
                const bool globalsUnchanged=beforeGlobals==ReversingProbe::Bytes(&CGarages::aGarages,sizeof(CGarages::aGarages))&&beforeNum==CGarages::NumGarages;
                std::memcpy(&CGarages::aGarages,savedGroups.data(),savedGroups.size());CGarages::NumGarages=savedNum;
                return json{{"return",ret},{"ecx_after",(!composed&&fn<2)?json(ecxAfter):json(nullptr)},{"edx_after",(!composed&&fn<2)?json(edxAfter):json(nullptr)},{"route_ok",route},{"stack_restored",abi},{"registers_preserved",abi},{"x87_stack_balanced",x87Balanced},{"fixture_unchanged",beforeGarage==garageBytes&&beforeObject==objectBytes&&beforeMatrix==matrixBytes&&globalsUnchanged},{"composed_original_FindGarage",composed}};
            };
            const auto original=run(false),candidate=run(true);bool equal=registered&&original==candidate;for(auto key:{"route_ok","stack_restored","registers_preserved","x87_stack_balanced","fixture_unchanged"})equal&=original[key]==true;
            failures+=!equal;Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fns[fn].cls)+"::"+fns[fn].name},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
        }
        passed&=!failures;Write({{"event","function_summary"},{"function",std::string(fns[fn].cls)+"::"+fns[fn].name},{"cases",fns[fn].cases},{"failures",failures}});
    }
    _controlfp(savedFP,_MCW_PC|_MCW_RC);std::memcpy(&CGarages::aGarages,savedGroups.data(),savedGroups.size());CGarages::NumGarages=savedNum;
    bool restored=savedGroups==ReversingProbe::Bytes(&CGarages::aGarages,savedGroups.size())&&savedNum==CGarages::NumGarages&&savedFP==_controlfp(0,0);for(auto& r:routes)restored&=r->Restore();
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
