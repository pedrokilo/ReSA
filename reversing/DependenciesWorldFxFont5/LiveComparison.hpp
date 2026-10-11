#pragma once
#include <fstream>
#include "Camera.h"
#include "Radar.h"
#include "Font.h"
#include "Fx/Fx.h"
#include "Scripts/TheScripts.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesWorldFxFont5Live {
struct Fn {const char* cls;const char* name;uintptr_t address,vtable;int slot,cases;};
inline constexpr Fn fns[]{
{"Fx_c","CreateMatFromVec",0x49E950,0,-1,1024},
{"CFont","GetScriptLetterSize",0x719670,0,-1,6144},
{"CCamera","IsSphereVisible-matrix",0x420C40,0,-1,4096},
{"CRadar","ClipRadarPoly",0x585040,0,-1,4096}
};
inline bool done{};inline std::ofstream log;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
struct Result {uint32 value;bool abi;};
__declspec(noinline) inline Result Call(uintptr_t entry,uintptr_t object,uintptr_t arg1,uintptr_t arg2,uintptr_t arg3,int kind){
    uint32 rawResult{},before{},after{},b{},s{},d{},frameBefore{},frameAfter{};float floatResult{};
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
        cmp kind,1
        je oneArg
        cmp kind,3
        je twoArgs
        push arg3
    twoArgs:
        push arg2
    oneArg:
        push arg1
        call entry
        mov rawResult,eax
        cmp kind,1
        jne notFloat
        fstp floatResult
        add esp,4
        jmp cleaned
    notFloat:
        cmp kind,3
        jne cleaned
        add esp,8
    cleaned:
        mov after,esp
        mov frameAfter,ebp
        mov b,ebx
        mov s,esi
        mov d,edi
        pop edi
        pop esi
        pop ebx
    }
    const uint32 semantic=kind==0?0:kind==1?std::bit_cast<uint32>(floatResult):kind==2?(rawResult&255):rawResult;
    return {semantic,before==after&&frameBefore==frameAfter&&b==0x13579BDF&&s==0x2468ACE0&&d==0x10293847};
}
inline uint16 Top(){uint16 v{};__asm {fnstsw v}return v&0x3800;}
inline void SaveFP(void* p){__asm {mov eax,p} __asm {fnsave [eax]} __asm {frstor [eax]}}
inline void RestoreFP(void* p){__asm {mov eax,p} __asm {frstor [eax]}}
inline uint32 ReadMXCSR(){uint32 v{};__asm {stmxcsr v}return v;}
inline void WriteMXCSR(uint32 v){__asm {ldmxcsr v}}
inline uintptr_t Resolve(uintptr_t entry){for(int i=0;i<16&&*reinterpret_cast<const uint8*>(entry)==0xE9;++i)entry+=5+*reinterpret_cast<const int32*>(entry+1);return entry;}
inline bool SameProbeModule(uintptr_t body){HMODULE owner{},probe{};const DWORD flags=GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;return GetModuleHandleExA(flags,reinterpret_cast<LPCSTR>(body),&owner)&&GetModuleHandleExA(flags,reinterpret_cast<LPCSTR>(&Call),&probe)&&owner==probe;}
inline bool ActualLibrary(uintptr_t entry){HMODULE owner{};const DWORD flags=GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;return GetModuleHandleExA(flags,reinterpret_cast<LPCSTR>(Resolve(entry)),&owner)&&owner==GetModuleHandleA(nullptr);}
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciesworldfxfont5-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependenciesworldfxfont5-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesworldfxfont5");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    const auto cameraSaved=ReversingProbe::Bytes(&TheCamera,sizeof(TheCamera));const auto clipSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC3EF9C),8);
    const auto introSaved=ReversingProbe::Bytes(&CTheScripts::IntroTextLines,sizeof(CTheScripts::IntroTextLines));const auto introCountSaved=CTheScripts::NumberOfIntroTextLinesThisFrame;
    // Never fabricate font metrics; read real loaded font bytes, including native unchecked letter indices.
    const auto fontSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC718B0),512);const auto fxSaved=ReversingProbe::Bytes(&g_fx,sizeof(g_fx));
    alignas(16)std::array<uint8,108> fpSaved{};SaveFP(fpSaved.data());const auto controlSaved=_controlfp(0,0);const auto topSaved=Top();const uint32 mxcsrSaved=ReadMXCSR();
    alignas(16)std::array<uint32,20> matrix{};std::array<uint32,5> origin{},direction{};std::array<uint32,34> polyIn{},polyOut{};
    auto* rw=reinterpret_cast<RwMatrix*>(&matrix[1]);auto* pos=reinterpret_cast<CVector*>(&origin[1]);auto* vec=reinterpret_cast<CVector*>(&direction[1]);
    std::array<std::unique_ptr<OriginalDependencies499::HookRoute>,4> routes;
    routes[0]=std::make_unique<OriginalDependencies499::HookRoute>(fns[0].cls,fns[0].name,fns[0].address,reinterpret_cast<void*>(std::bit_cast<uintptr_t>(&Fx_c::CreateMatFromVec)));
    routes[1]=std::make_unique<OriginalDependencies499::HookRoute>(fns[1].cls,fns[1].name,fns[1].address,&GetScriptLetterSize);
    const uintptr_t cameraBody=std::bit_cast<uintptr_t>(static_cast<bool(CCamera::*)(const CVector&,float,RwMatrix*)>(&CCamera::IsSphereVisible));
    routes[2]=std::make_unique<OriginalDependencies499::HookRoute>(fns[2].cls,fns[2].name,fns[2].address,reinterpret_cast<void*>(cameraBody));
    routes[3]=std::make_unique<OriginalDependencies499::HookRoute>(fns[3].cls,fns[3].name,fns[3].address,&CRadar::ClipRadarPoly);
    const uintptr_t fontHelperPtr=reinterpret_cast<uintptr_t>(&CFont::FindSubFontCharacter),radarHelperPtr=reinterpret_cast<uintptr_t>(&LineRadarBoxCollision);
    OriginalDependencies499::HookRoute fontHelper("CFont","FindSubFontCharacter",0x7192C0,&CFont::FindSubFontCharacter);
    OriginalDependencies499::HookRoute radarHelper("CRadar","LineRadarBoxCollision",0x584E00,&LineRadarBoxCollision);
    bool registered=fontHelper.Registered()&&radarHelper.Registered();for(auto& r:routes)registered&=r->Registered();bool passed=registered;int pair=0;
    const bool libraryOk=ActualLibrary(0x7ED9B0)&&ActualLibrary(0x7F18A0)&&ActualLibrary(0x7EDD90);passed&=libraryOk;
    Write({{"event","begin"},{"registered_hook",registered},{"actual_RenderWare_libraries",libraryOk},{"game_collaborator_modes","original_OFF_candidate_ON"},{"camera_layout",sizeof(CCamera)==0xD78}});
    auto restore=[&]{memcpy(&TheCamera,cameraSaved.data(),cameraSaved.size());memcpy(reinterpret_cast<void*>(0xC3EF9C),clipSaved.data(),clipSaved.size());memcpy(&CTheScripts::IntroTextLines,introSaved.data(),introSaved.size());CTheScripts::NumberOfIntroTextLinesThisFrame=introCountSaved;RestoreFP(fpSaved.data());WriteMXCSR(mxcsrSaved);};
    for(int index=0;index<4;++index){const auto& fn=fns[index];int failures=0;bool sawZero=false,sawPositive=false;
        for(int scenario=0;scenario<fn.cases;++scenario){
            auto run=[&](bool candidate){
                restore();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);_clearfp();const auto topBefore=Top();
                matrix.fill(0xA7A7A7A7);matrix.front()=0x13579BDF;matrix.back()=0x2468ACE0;origin.fill(0);direction.fill(0);origin.front()=direction.front()=0xFEEDFACE;origin.back()=direction.back()=0xBAD0C0DE;
                uintptr_t object=0,arg1=0,arg2=0,arg3=0;
                if(index==0){
                    const CVector values[]{CVector{1,0,0},CVector{0,1,0},CVector{0,0,1},CVector{1,1,1},CVector{-0.6f,0.8f,0},CVector{0.001f,-0.002f,0.003f},CVector{10,-20,40},CVector{-1,-1,-1}};
                    *pos=CVector{float(scenario%17)-8,float((scenario/17)%13)-6,float((scenario/221)%5)-2};*vec=values[scenario%8];
                    rw->flags=uint32(scenario)*2654435761u;object=uintptr_t(&g_fx);arg1=uintptr_t(rw);
                    arg2=uintptr_t((scenario&64)?reinterpret_cast<CVector*>(&rw->pos):pos);arg3=uintptr_t((scenario&128)?reinterpret_cast<CVector*>(&rw->up):vec);
                }else if(index==1){
                    const int letter=scenario%256,style=(scenario/256)%4,proportional=(scenario/1024)%2,variation=scenario/2048;
                    CTheScripts::NumberOfIntroTextLinesThisFrame=uint16(scenario%96);auto& text=CTheScripts::IntroTextLines[scenario%96];
                    text.FontStyle=static_cast<eFontStyle>(style);text.IsProportional=proportional!=0;text.TextEdge=variation==0?int8(-128):variation==1?int8(0):int8(127);text.Scale.x=variation==0?0.f:variation==1?1.25f:-0.5f;
                    arg1=uintptr_t(letter);
                }else if(index==2){
                    RwMatrixSetIdentity(rw);if(scenario&128){rw->right={0.6f,0.8f,0};rw->up={-0.8f,0.6f,0};}if(scenario&256)rw->pos={-3.f,2.f,1.f};RwMatrixUpdate(rw);
                    const float coordinate[]{-80,-40,-2,-1,-0.f,0.f,1,2,40,80,std::bit_cast<float>(0x7FC00001u),std::bit_cast<float>(0x7F800000u)};
                    *pos=CVector{coordinate[scenario%12],coordinate[(scenario/12)%12],coordinate[(scenario/144)%12]};
                    auto* cam=reinterpret_cast<uint8*>(&TheCamera);const float side[]={0.70710677f,-0.70710677f};
                    const float plane[12]{side[0],side[1],0,side[1],side[1],0,0,side[1],side[0],0,side[1],side[1]};memcpy(cam+0xB24,plane,sizeof(plane));
                    // The EXE checks y+radius against near=C3EFA0, y-radius against far=C3EF9C.
                    *reinterpret_cast<float*>(0xC3EF9C)=40.f;*reinterpret_cast<float*>(0xC3EFA0)=0.5f;
                    const float radii[]{0,0.5f,2,7,-0.5f,std::bit_cast<float>(0x7FC00001u)};const float radius=radii[(scenario/64)%6];
                    object=uintptr_t(&TheCamera);arg1=uintptr_t(pos);arg2=std::bit_cast<uint32>(radius);arg3=uintptr_t(rw);
                }else{
                    polyIn.fill(0xA7A7A7A7);polyOut.fill(0xA7A7A7A7);polyIn.front()=polyOut.front()=0x13579BDF;polyIn.back()=polyOut.back()=0x2468ACE0;
                    auto* input=reinterpret_cast<CVector2D*>(&polyIn[1]);const float centres[]{-4,-1.000001f,-1,-0.9999999f,0,0.9999999f,1,1.000001f,4};
                    const float cx=centres[scenario%9],cy=centres[(scenario/9)%9];const float extents[]{0.0001f,0.25f,1,3};const float dx=extents[(scenario/81)%4],dy=extents[(scenario/324)%4];
                    const float cornersX[]{dx,dx,-dx,-dx},cornersY[]{-dy,dy,dy,-dy};const bool rotated=(scenario&1024)!=0;
                    for(int k=0;k<4;++k){const int n=(scenario&2048)?3-k:k;const float x=cornersX[n],y=cornersY[n];input[k]=CVector2D{cx+(rotated?x*0.6f-y*0.8f:x),cy+(rotated?x*0.8f+y*0.6f:y)};}
                    arg1=uintptr_t(&polyOut[1]);arg2=uintptr_t(input);
                }
                const auto matrixBefore=matrix;const auto originBefore=origin;const auto directionBefore=direction;
                const auto inputBefore=polyIn;const auto cameraBefore=ReversingProbe::Bytes(&TheCamera,sizeof(TheCamera));const auto textBefore=ReversingProbe::Bytes(&CTheScripts::IntroTextLines,sizeof(CTheScripts::IntroTextLines));const auto textCountBefore=CTheScripts::NumberOfIntroTextLinesThisFrame;
                bool routeOk=false,helperOk=true;
                auto call=[&]{return routes[index]->Invoke(candidate,[&]{routeOk=ReversingProbe::JumpsToDll(fn.address)==candidate;return Call(fn.address,object,arg1,arg2,arg3,index);});};
                Result result{};
                if(index==1){result=fontHelper.Invoke(candidate,[&]{helperOk=ReversingProbe::JumpsToDll(0x7192C0)==candidate;if(candidate)helperOk&=Resolve(fontHelperPtr)==fontHelper.Body()&&SameProbeModule(Resolve(fontHelperPtr));return call();});}
                else if(index==3){result=radarHelper.Invoke(candidate,[&]{helperOk=ReversingProbe::JumpsToDll(0x584E00)==candidate;if(candidate)helperOk&=Resolve(radarHelperPtr)==radarHelper.Body()&&SameProbeModule(Resolve(radarHelperPtr));return call();});}
                else result=call();
                json value={{"return_bits",result.value},{"route_ok",routeOk},{"helper_route_ok",helperOk},{"library_route_ok",ActualLibrary(0x7ED9B0)&&ActualLibrary(0x7F18A0)&&ActualLibrary(0x7EDD90)},{"stack_restored",result.abi},{"registers_preserved",result.abi},{"fp_preserved",Top()==topBefore&&(_controlfp(0,0)&(_MCW_PC|_MCW_RC))==(_PC_24|_RC_NEAR)}};
                value["readonly_ok"]=originBefore==origin&&directionBefore==direction&&cameraBefore==ReversingProbe::Bytes(&TheCamera,sizeof(TheCamera))&&textBefore==ReversingProbe::Bytes(&CTheScripts::IntroTextLines,sizeof(CTheScripts::IntroTextLines))&&textCountBefore==CTheScripts::NumberOfIntroTextLinesThisFrame&&fontSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC718B0),512)&&fxSaved==ReversingProbe::Bytes(&g_fx,sizeof(g_fx));
                if(index==0){value["matrix"]=matrix;value["guards_ok"]=matrix.front()==0x13579BDF&&matrix[17]==0xA7A7A7A7&&matrix[18]==0xA7A7A7A7&&matrix.back()==0x2468ACE0;value["return_domain_ok"]=true;}
                else if(index==1){value["guards_ok"]=true;value["return_domain_ok"]=std::isfinite(std::bit_cast<float>(result.value));}
                else if(index==2){value["guards_ok"]=matrixBefore==matrix;value["return_domain_ok"]=result.value<=1;}
                else {value["output"]=polyOut;value["guards_ok"]=polyOut.front()==0x13579BDF&&polyOut.back()==0x2468ACE0&&inputBefore==polyIn;value["return_domain_ok"]=int32(result.value)>=0&&int32(result.value)<=16;}
                restore();return value;
            };
            const auto original=run(false),candidate=run(true);bool equal=registered&&original==candidate;
            if(index==2||index==3){const uint32 ret=original["return_bits"].get<uint32>();sawZero|=ret==0;sawPositive|=ret>0;}
            for(auto key:{"route_ok","helper_route_ok","library_route_ok","stack_restored","registers_preserved","fp_preserved","readonly_ok","guards_ok","return_domain_ok"})equal&=original[key]==true;
            passed&=equal;failures+=!equal;Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
        }
        const bool completePaths=(index!=2&&index!=3)||(sawZero&&sawPositive);passed&=completePaths;
        Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"both_empty_or_rejected_and_positive_paths",completePaths}});
    }
    restore();bool restored=fontHelper.Restore()&&radarHelper.Restore();for(auto& r:routes)restored&=r->Restore();
    restored&=cameraSaved==ReversingProbe::Bytes(&TheCamera,sizeof(TheCamera))&&clipSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC3EF9C),8)&&introSaved==ReversingProbe::Bytes(&CTheScripts::IntroTextLines,sizeof(CTheScripts::IntroTextLines))&&introCountSaved==CTheScripts::NumberOfIntroTextLinesThisFrame&&fontSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC718B0),512)&&fxSaved==ReversingProbe::Bytes(&g_fx,sizeof(g_fx))&&Top()==topSaved&&ReadMXCSR()==mxcsrSaved&&(_controlfp(0,0)&(_MCW_PC|_MCW_RC))==(controlSaved&(_MCW_PC|_MCW_RC));
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
