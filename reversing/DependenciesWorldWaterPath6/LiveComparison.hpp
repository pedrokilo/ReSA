#pragma once
#include <fstream>
#include "Cam.h"
#include "WaterLevel.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesWorldWaterPath6Live {
struct Fn{const char* cls;const char* name;uintptr_t address,vtable;int slot,cases;};
inline constexpr Fn fns[]{
{"CCam","CameraPathInterpolateVector",0x5B2090,0,-1,4096},
{"CCam","CameraPathInterpolateScalar",0x5B2330,0,-1,4096},
{"CWaterLevel","GetWaterLevelNoWavesForQuad",0x6E5BB0,0,-1,8192},
{"CWaterLevel","GetWaterLevelNoWavesForTriangle",0x6E5E90,0,-1,8192}
};
inline bool done{};inline std::ofstream log;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
// Estres de las entradas x, y, z (cada ranura se sustituye en 1 de cada 73 escenarios): NaN con distinta carga/signo, +-inf, +-0, subnormales, +-1e30, +-1e-30, FLT_MAX.
inline float Stress(float value,int scenario,int slot){static constexpr uint32 bits[]{0x7FC00000u,0xFFC00001u,0x7F800000u,0xFF800000u,0x00000000u,0x80000000u,0x00000001u,0x80000003u,0x7F7FFFFFu,0xFF7FFFFFu,0x7149F2CAu,0xF149F2CAu,0x0DA24260u,0x8DA24260u};return scenario%73!=slot*24+3?value:std::bit_cast<float>(bits[size_t(scenario/73)%std::size(bits)]);}
struct Result{uint32 value;bool abi;};
__declspec(noinline) inline Result Call(uintptr_t entry,const std::array<uintptr_t,7>& args,bool water){
    const uintptr_t a=args[0],b=args[1],c=args[2],d=args[3],e=args[4],f=args[5],g=args[6];
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
        cmp water,0
        je fourArgs
        push g
        push f
        push e
    fourArgs:
        push d
        push c
        push b
        push a
        call entry
        mov value,eax
        add esp,16
        cmp water,0
        je cleaned
        add esp,12
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
    return {water?value&255:0,before==after&&frameBefore==frameAfter&&rb==0x13579BDF&&rs==0x2468ACE0&&rd==0x10293847};
}
inline uint16 Top(){uint16 v{};__asm{fnstsw v}return v&0x3800;}
inline void SaveFP(void* p){__asm{mov eax,p}__asm{fnsave [eax]}__asm{frstor [eax]}}
inline void RestoreFP(void* p){__asm{mov eax,p}__asm{frstor [eax]}}
inline uint32 ReadMXCSR(){uint32 v{};__asm{stmxcsr v}return v;}
inline void WriteMXCSR(uint32 v){__asm{ldmxcsr v}}
inline uintptr_t Resolve(uintptr_t e){for(int n=0;n<16&&*reinterpret_cast<const uint8*>(e)==0xE9;++n)e+=5+*reinterpret_cast<const int32*>(e+1);return e;}
inline bool ActualCRT(){HMODULE owner{};const DWORD flags=GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;return GetModuleHandleExA(flags,reinterpret_cast<LPCSTR>(Resolve(0x821B40)),&owner)&&owner==GetModuleHandleA(nullptr);}
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciesworldwaterpath6-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependenciesworldwaterpath6-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesworldwaterpath6");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    static_assert(sizeof(CWaterVertex)==20&&sizeof(CWaterQuad)==10&&sizeof(CWaterTriangle)==8);
    const auto verticesSaved=ReversingProbe::Bytes(&CWaterLevel::m_aVertices,sizeof(CWaterLevel::m_aVertices));
    const auto quadsSaved=ReversingProbe::Bytes(&CWaterLevel::WaterQuads,sizeof(CWaterLevel::WaterQuads));
    const auto trianglesSaved=ReversingProbe::Bytes(&CWaterLevel::WaterTriangles,sizeof(CWaterLevel::WaterTriangles));
    const auto countsSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC22884),12);
    const auto translationSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB6F8AC),12);
    alignas(16)std::array<uint8,108> fpSaved{};SaveFP(fpSaved.data());const auto controlSaved=_controlfp(0,0);const auto topSaved=Top();const auto mxcsrSaved=ReadMXCSR();
    auto restore=[&]{std::memcpy(&CWaterLevel::m_aVertices,verticesSaved.data(),verticesSaved.size());std::memcpy(&CWaterLevel::WaterQuads,quadsSaved.data(),quadsSaved.size());std::memcpy(&CWaterLevel::WaterTriangles,trianglesSaved.data(),trianglesSaved.size());std::memcpy(reinterpret_cast<void*>(0xC22884),countsSaved.data(),countsSaved.size());std::memcpy(reinterpret_cast<void*>(0xB6F8AC),translationSaved.data(),translationSaved.size());RestoreFP(fpSaved.data());WriteMXCSR(mxcsrSaved);};
    auto atlasReadonly=[&]{return verticesSaved==ReversingProbe::Bytes(&CWaterLevel::m_aVertices,sizeof(CWaterLevel::m_aVertices))&&quadsSaved==ReversingProbe::Bytes(&CWaterLevel::WaterQuads,sizeof(CWaterLevel::WaterQuads))&&trianglesSaved==ReversingProbe::Bytes(&CWaterLevel::WaterTriangles,sizeof(CWaterLevel::WaterTriangles))&&countsSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC22884),12);};
    std::vector<uint32> validQuads,validTriangles,zeroQuads,zeroTriangles;
    const uint32 nv=CWaterLevel::NumWaterVertices,nq=CWaterLevel::NumWaterQuads,nt=CWaterLevel::NumWaterTriangles;
    bool atlasValid=nv>0&&nv<=1021&&nq>0&&nq<=301&&nt>0&&nt<=6;
    auto valid=[&](const auto& p){for(auto id:p.verts)if(id>=nv)return false;const auto& a=CWaterLevel::m_aVertices[p.verts[0]];const auto& b=CWaterLevel::m_aVertices[p.verts[1]];const auto& c=CWaterLevel::m_aVertices[p.verts[2]];return a.x<b.x&&a.y!=c.y;};
    if(atlasValid){for(uint32 n=0;n<nq;++n)if(valid(CWaterLevel::WaterQuads[n]))validQuads.push_back(n);for(uint32 n=0;n<nt;++n)if(valid(CWaterLevel::WaterTriangles[n]))validTriangles.push_back(n);}
    auto zeroHeight=[&](const auto& p){for(auto id:p.verts)if(CWaterLevel::m_aVertices[id].rp.z!=0.f)return false;return true;};
    for(auto n:validQuads)if(zeroHeight(CWaterLevel::WaterQuads[n]))zeroQuads.push_back(n);
    for(auto n:validTriangles)if(zeroHeight(CWaterLevel::WaterTriangles[n]))zeroTriangles.push_back(n);
    bool orientations[2]{};for(auto n:validTriangles){const auto& p=CWaterLevel::WaterTriangles[n];orientations[CWaterLevel::m_aVertices[p.verts[0]].x==CWaterLevel::m_aVertices[p.verts[2]].x?0:1]=true;}
    atlasValid&=!validQuads.empty()&&!validTriangles.empty()&&!zeroQuads.empty()&&!zeroTriangles.empty()&&orientations[0]&&orientations[1];
    if(!atlasValid){restore();Write({{"event","error"},{"reason","loaded real water atlas lacks required nondegenerate quads/two triangle orientations"},{"vertices",nv},{"quads",nq},{"triangles",nt},{"state_restored",atlasReadonly()}});return;}
    std::array<std::unique_ptr<OriginalDependencies499::HookRoute>,4> routes;
    routes[0]=std::make_unique<OriginalDependencies499::HookRoute>(fns[0].cls,fns[0].name,fns[0].address,&CameraPathInterpolateVector);
    routes[1]=std::make_unique<OriginalDependencies499::HookRoute>(fns[1].cls,fns[1].name,fns[1].address,&CameraPathInterpolateScalar);
    routes[2]=std::make_unique<OriginalDependencies499::HookRoute>(fns[2].cls,fns[2].name,fns[2].address,&CWaterLevel::GetWaterLevelNoWavesForQuad);
    routes[3]=std::make_unique<OriginalDependencies499::HookRoute>(fns[3].cls,fns[3].name,fns[3].address,&CWaterLevel::GetWaterLevelNoWavesForTriangle);
    bool allRegistered=true;for(auto& route:routes)allRegistered&=route->Registered();
    Write({{"event","begin"},{"registered_hook",allRegistered},{"loaded_vertices",nv},{"loaded_quads",nq},{"loaded_triangles",nt},{"both_triangle_orientations",orientations[0]&&orientations[1]},{"zero_height_quads",zeroQuads.size()},{"zero_height_triangles",zeroTriangles.size()},{"actual_crt_library",ActualCRT()}});
    bool passed=true;uint32 pair=0;
    for(int index=0;index<4;++index){const auto& fn=fns[index];const bool registered=routes[index]->Registered();bool sawFalse=false,sawTrue=false,sawChanged=false,sawAdvance=false,sawLowEqual=false,sawLowReject=false,sawHighEqual=false,sawHighReject=false,sawNullDepth=false;uint32 failures=0;std::array<uint32,8> acceptedKinds{};std::array<uint32,3> durationClasses{};
        for(int scenario=0;scenario<fn.cases;++scenario){
            auto run=[&](bool candidate){restore();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);const auto topBefore=Top();
                std::array<uint32,174> path{};path.fill(0xA7A7A7A7);path.front()=0x13579BDF;path.back()=0x2468ACE0;
                std::array<uint32,6> output{0x13579BDF,0x47A5A5A5,0x47A5A5A5,0x47A5A5A5,0xA7A7A7A7,0x2468ACE0};
                alignas(4)std::array<uint8,16> polygon{};polygon.fill(0xA7);polygon[0]=0x13;polygon[1]=0x57;polygon[14]=0x24;polygon[15]=0x68;
                std::array<int32,3> key{int32(0x13579BDF),0,int32(0x2468ACE0)};std::array<uintptr_t,7> args{};uint32 fixtureKind=0;bool fixtureValid=true;int pathStep=0,pathCount=0,pathFloats=0,keyBefore=0,keyExpected=0,minRead=0,maxRead=0,durationClass=-1,zVariant=-1,optional=-1;float pathDuration=0;bool advanceExpected=false,limited=false;
                if(index<2){
                    const int step=index==0?10:4,count=2+(scenario/256)%15,segment=1+(scenario/64)%(count-1),mode=(scenario/8)%(index==0?8:2);
                    auto* data=reinterpret_cast<float*>(&path[1]);data[0]=float(count);pathStep=step;pathCount=count;pathFloats=1+count*step;
                    // Exact LoadPathSplines storage: count+N*(time,endpoint,incoming,outgoing).
                    const float gaps[]{.016f,.032f,.033f,.75f};float clock=0;
                    for(int n=0;n<count;++n){const int base=1+n*step;data[base]=clock;const int dims=index==0?3:1;for(int k=0;k<dims;++k){const float value=float(n*n*(k+1)-3*n+2*k)*0.125f;data[base+1+k]=value;data[base+1+dims+k]=value+float(k+1)*0.25f;data[base+1+2*dims+k]=(mode&(1<<k))?value+float(k+1)*0.375f:value;}if(n+1<count)clock+=(n+1==count-1)?.75f:gaps[(scenario/512)%4];}
                    const float start=data[1+(segment-1)*step]*1000.f,end=data[1+segment*step]*1000.f,last=data[1+(count-1)*step]*1000.f;
                    const float times[]{start-.25f,start,start+(end-start)*.25f,start+(end-start)*.5f,std::nextafter(end,-std::numeric_limits<float>::infinity()),end,std::nextafter(end,std::numeric_limits<float>::infinity()),last+1000.f};const float time=times[scenario%8];
                    // Native Process_FlyBy starts at11/5, advances while next time<=elapsed,
                    // and selects the final node after finish. Final gap750ms prevents a short terminal lookahead.
                    key[1]=1+step;if(time>=last)key[1]=1+(count-1)*step;else while(key[1]<1+(count-1)*step&&data[key[1]]*1000.f<=time)key[1]+=step;
                    keyBefore=key[1];pathDuration=(data[keyBefore]-data[keyBefore-step])*1000.f;const float threshold=*reinterpret_cast<const float*>(0x8D0F80);
                    durationClass=pathDuration<threshold?0:pathDuration==threshold?1:2;advanceExpected=time<last&&pathDuration<=threshold;keyExpected=keyBefore+(advanceExpected?step:0);
                    minRead=std::min(0,std::min(keyBefore-step,keyExpected-(index==0?9:3)));maxRead=std::max(1+(count-1)*step,std::max(keyBefore,keyExpected+(index==0?6:2)));
                    fixtureValid=keyBefore>=step+1&&(keyBefore-1)%step==0&&keyExpected<=1+(count-1)*step&&minRead>=0&&maxRead<pathFloats;
                    path[1+pathFloats]=0xDEADBEEF; // immediate post-allocation canary; no node beyond header count exists.
                    auto* translation=reinterpret_cast<float*>(0xB6F8AC);translation[0]=(scenario&1024)?12.25f:0;translation[1]=(scenario&1024)?-3.5f:0;translation[2]=(scenario&1024)?.75f:0;
                    args={uintptr_t(&output[1]),uintptr_t(data),std::bit_cast<uint32>(time),uintptr_t(&key[1]),0,0,0};fixtureKind=uint32(mode);
                }else{
                    const bool triangle=index==3;zVariant=(scenario/64)%18;const bool boundary=(zVariant>=2&&zVariant<=4)||(zVariant>=10&&zVariant<=12);const auto& rows=boundary?(triangle?zeroTriangles:zeroQuads):(triangle?validTriangles:validQuads);const uint32 row=rows[uint32(scenario/288)%rows.size()];const size_t size=triangle?8:10;
                    const void* realPolygon=triangle?static_cast<const void*>(&CWaterLevel::WaterTriangles[row]):static_cast<const void*>(&CWaterLevel::WaterQuads[row]);std::memcpy(&polygon[2],realPolygon,size);
                    // Only documented limited-depth flag changes on private copy of a REAL atlas polygon.
                    limited=(scenario&4096)!=0;polygon[2+(triangle?6:8)]=uint8((polygon[2+(triangle?6:8)]&~4u)|(limited?4u:0));
                    const auto* ids=reinterpret_cast<const uint16*>(&polygon[2]);const auto& a=CWaterLevel::m_aVertices[ids[0]];const auto& b=CWaterLevel::m_aVertices[ids[1]];const auto& c=CWaterLevel::m_aVertices[ids[2]];
                    const float fractions[]{-.001f,0,.125f,.375f,.625f,.875f,1,1.001f};const float u=fractions[scenario%8],v=fractions[(scenario/8)%8];
                    const float x=float(a.x)+float(int(b.x)-int(a.x))*u;const float y=float(std::min(a.y,c.y))+float(std::abs(int(c.y)-int(a.y)))*v;
                    const float zModes[]{-2000,-100,std::nextafter(-6.f,-std::numeric_limits<float>::infinity()),-6,std::nextafter(-6.f,std::numeric_limits<float>::infinity()),-4,-3,0,.5f,1,std::nextafter(20.f,-std::numeric_limits<float>::infinity()),20,std::nextafter(20.f,std::numeric_limits<float>::infinity()),100,2000,-1,6,19};const float z=a.rp.z+zModes[zVariant];optional=(scenario/1152)%3;
                    args={uintptr_t(&polygon[2]),std::bit_cast<uint32>(Stress(x,scenario,0)),std::bit_cast<uint32>(Stress(y,scenario,1)),std::bit_cast<uint32>(Stress(z,scenario,2)),uintptr_t(&output[1]),optional==0?uintptr_t(&output[2]):0,optional==1?0:uintptr_t(&output[3])};
                    // Native big!=NULL requires small!=NULL; big==NULL permits either small state.
                    fixtureKind=triangle?(a.x==c.x?0:1):(u+v<=1?0:1);fixtureValid=ids[0]<nv&&ids[1]<nv&&ids[2]<nv&&(args[5]==0||args[6]!=0);
                }
                const auto pathBefore=path;const auto polygonBefore=polygon;const auto translationBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB6F8AC),12);
                bool routeOk=false;Result result{};if(fixtureValid)result=routes[index]->Invoke(candidate,[&]{routeOk=ReversingProbe::JumpsToDll(fn.address)==candidate;return Call(fn.address,args,index>=2);});
                const bool guard=path.front()==0x13579BDF&&path.back()==0x2468ACE0&&output.front()==0x13579BDF&&output[4]==0xA7A7A7A7&&output.back()==0x2468ACE0&&key.front()==int32(0x13579BDF)&&key.back()==int32(0x2468ACE0)&&polygon==polygonBefore;
                const bool rangeOK=index>=2||(key[1]==keyExpected&&key[1]>=pathStep+1&&(key[1]-1)%pathStep==0&&key[1]<=1+(pathCount-1)*pathStep&&minRead>=0&&maxRead<pathFloats);
                const bool canaries=index>=2||path[1+pathFloats]==0xDEADBEEF;
                const bool optionalOK=index<2||optional==0||(output[2]==0x47A5A5A5&&output[3]==0x47A5A5A5);
                json value={{"return_bits",result.value},{"output",output},{"key",key[1]},{"key_before",keyBefore},{"expected_key",keyExpected},{"path_float_count",pathFloats},{"min_read_index",minRead},{"max_read_index",maxRead},{"duration_bits",std::bit_cast<uint32>(pathDuration)},{"duration_class",durationClass},{"advance_expected",advanceExpected},{"z_variant",zVariant},{"optional_mode",optional},{"limited_depth",limited},{"read_range_ok",rangeOK},{"path_canaries_ok",canaries},{"optional_outputs_ok",optionalOK},{"fixture_kind",fixtureKind},{"fixture_valid",fixtureValid},{"route_ok",routeOk},{"library_route_ok",ActualCRT()},{"stack_restored",result.abi},{"registers_preserved",result.abi},{"fp_preserved",Top()==topBefore&&(_controlfp(0,0)&(_MCW_PC|_MCW_RC))==(_PC_24|_RC_NEAR)},{"readonly_ok",path==pathBefore&&atlasReadonly()&&translationBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB6F8AC),12)},{"guards_ok",guard},{"return_domain_ok",result.value<=1}};
                restore();return value;
            };
            const auto original=run(false),candidate=run(true);bool equal=registered&&original==candidate;for(auto keyName:{"fixture_valid","read_range_ok","path_canaries_ok","optional_outputs_ok","route_ok","library_route_ok","stack_restored","registers_preserved","fp_preserved","readonly_ok","guards_ok","return_domain_ok"})equal&=original[keyName]==true;
            const auto ret=original["return_bits"].get<uint32>();sawFalse|=ret==0;sawTrue|=ret==1;sawChanged|=original["output"][1]!=0x47A5A5A5;const auto kind=original["fixture_kind"].get<uint32>();if((index<2||ret==1)&&kind<acceptedKinds.size())++acceptedKinds[kind];
            if(index<2){++durationClasses[original["duration_class"].get<int>()];sawAdvance|=original["advance_expected"]==true&&original["key"]==original["expected_key"]&&original["key"]!=original["key_before"];}
            else if((original["output"][1].get<uint32>()&0x7FFFFFFF)==0){const int zv=original["z_variant"].get<int>(),op=original["optional_mode"].get<int>();sawLowEqual|=zv==3&&op==0&&ret==1;sawLowReject|=zv==2&&op==0&&original["limited_depth"]==true&&ret==0;sawHighEqual|=zv==11&&op==0&&ret==1;sawHighReject|=zv==12&&op==0&&ret==0;sawNullDepth|=zv==12&&op!=0&&ret==uint32(index==2);}
            passed&=equal;failures+=!equal;Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
        }
        bool covered=sawChanged&&acceptedKinds[0]>0&&acceptedKinds[1]>0&&(index<2||(sawFalse&&sawTrue));if(index==0)for(auto n:acceptedKinds)covered&=n>0;
        if(index<2)covered&=sawAdvance&&durationClasses[0]>0&&durationClasses[1]>0&&durationClasses[2]>0;else covered&=sawLowEqual&&sawLowReject&&sawHighEqual&&sawHighReject&&sawNullDepth;passed&=covered;
        Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"complete_paths",covered},{"accepted_fixture_kinds",acceptedKinds},{"duration_classes",durationClasses},{"saw_index_advance",sawAdvance},{"depth_low_equal",sawLowEqual},{"depth_low_reject",sawLowReject},{"depth_high_equal",sawHighEqual},{"depth_high_reject",sawHighReject},{"null_depth_branch",sawNullDepth},{"saw_false",sawFalse},{"saw_true",sawTrue}});
    }
    restore();bool restored=atlasReadonly()&&translationSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB6F8AC),12)&&Top()==topSaved&&ReadMXCSR()==mxcsrSaved&&(_controlfp(0,0)&(_MCW_PC|_MCW_RC))==(controlSaved&(_MCW_PC|_MCW_RC));for(auto& route:routes)restored&=route->Restore();
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
