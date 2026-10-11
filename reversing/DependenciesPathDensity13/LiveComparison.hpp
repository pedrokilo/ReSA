#pragma once
#include <fstream>
#include "PathFind.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"


namespace DependenciesPathDensity13Live {
inline bool done{};
inline std::ofstream log;
inline void SaveFP(void* p) {__asm {mov eax,p} __asm {fnsave [eax]} __asm {frstor [eax]}}
inline void RestoreFP(void* p) {__asm {mov eax,p} __asm {frstor [eax]}}
inline uint32 ReadMXCSR() {uint32 v{};__asm {stmxcsr v}return v;}
inline void SetMXCSR(uint32 v) {__asm {ldmxcsr v}}
alignas(16) inline std::array<uint8,108> savedFP{};
inline uint32 savedMXCSR{};inline bool savedFPActive{};
inline void RestoreFloatingState() {if(savedFPActive) {RestoreFP(savedFP.data());SetMXCSR(savedMXCSR);}}
inline bool FloatingStateRestored() {alignas(16)std::array<uint8,108> current{};SaveFP(current.data());return current==savedFP&&ReadMXCSR()==savedMXCSR;}
inline void Write(const json& row) {RestoreFloatingState();ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();RestoreFloatingState();}
struct Fn { const char* cls;const char* name;uintptr_t address;int reserved,unused,count; };
inline constexpr Fn fns[]{
    {"CPathFind","CalcRoadDensity",0x44EFC0,0,-1,2048}
};
inline uintptr_t Resolve(uintptr_t a){for(unsigned n=0;n<16&&*reinterpret_cast<uint8*>(a)==0xE9;++n)a+=5+*reinterpret_cast<int32*>(a+1);return a;}
inline uintptr_t ModuleOf(uintptr_t a){MEMORY_BASIC_INFORMATION i{};return VirtualQuery(reinterpret_cast<void*>(a),&i,sizeof(i))==sizeof(i)?uintptr_t(i.AllocationBase):0;}
inline void ModuleMarker(){}

struct Result {uint32 eax,ecx,edx,arg0,arg1,floatBits;std::array<uint8,10> extended;bool stack,nonvolatile;};
inline Result Gate(uintptr_t entry,uintptr_t instance,uint32 argument0,uint32 argument1){
    uint32 before{},after{},a{},c{},d{},x{},y{},b0{},s0{},i0{},p0{},b1{},s1{},i1{},p1{};float returned{};std::array<uint8,10> extended{};auto* extendedOutput=extended.data();
    __asm {
        mov before,esp
        mov b0,ebx
        mov s0,esi
        mov i0,edi
        mov p0,ebp
        push argument1
        push argument0
        mov eax,0A5123456h
        mov edx,0C0DE5921h
        mov ecx,instance
        call entry
        mov a,eax
        mov c,ecx
        mov d,edx
        mov b1,ebx
        mov s1,esi
        mov i1,edi
        mov p1,ebp
        mov eax,[esp-8]
        mov x,eax
        mov eax,[esp-4]
        mov y,eax
        fld st(0)
        fstp returned
        mov eax,extendedOutput
        fstp tbyte ptr [eax]
        mov after,esp
    }
    return {a,c,d,x,y,std::bit_cast<uint32>(returned),extended,before==after,b0==b1&&s0==s1&&i0==i1&&p0==p1};
}
inline uint32 ReadInt(uintptr_t a){uint32 v;memcpy(&v,reinterpret_cast<void*>(a),4);return v;}
inline void PutInt(uintptr_t a,uint32 v){memcpy(reinterpret_cast<void*>(a),&v,4);}
inline void PutWord(uintptr_t a,int16 v){memcpy(reinterpret_cast<void*>(a),&v,2);}
inline std::string Hex(uintptr_t a,size_t n){constexpr char digits[]="0123456789abcdef";const auto* b=reinterpret_cast<const uint8*>(a);std::string out;out.reserve(n*2);for(size_t i=0;i<n;++i){out+=digits[b[i]>>4];out+=digits[b[i]&15];}return out;}
struct Block {uintptr_t address;std::vector<uint8> saved;Block(uintptr_t a,size_t n):address(a),saved(n){memcpy(saved.data(),reinterpret_cast<void*>(a),n);}void Restore()const{memcpy(reinterpret_cast<void*>(address),saved.data(),saved.size());}bool Restored()const{return !memcmp(reinterpret_cast<void*>(address),saved.data(),saved.size());}std::string Observe()const{return Hex(address,saved.size());}};
inline void Update(){
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciespathdensity13-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependenciespathdensity13-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciespathdensity13");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    static_assert(sizeof(CPathFind)==0x3C80&&sizeof(CPathNode)==0x1C&&sizeof(CCarPathLink)==0xE);
    const auto member=&CPathFind::CalcRoadDensity;const auto raw=uintptr_t(FunctionToVoidPtr(member));
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> initialCode;for(auto at:{uintptr_t(0x44EFC0),raw,Resolve(raw)})initialCode.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    OriginalDependencies499::HookRoute route("CPathFind","CalcRoadDensity",0x44EFC0,FunctionToVoidPtr(member));const auto offCode=ReversingProbe::Bytes(reinterpret_cast<void*>(0x44EFC0),16);
    alignas(16)std::array<uint8,0x3CA0> path{};alignas(16)std::array<uint8,3*0x1C+32> sources{};alignas(16)std::array<uint8,16*0x1C+32> targets{};
    alignas(16)std::array<uint8,0x1C+32> marker{};alignas(16)std::array<uint8,15*0xE+32> navi{};alignas(16)std::array<uint8,45*4+32> links{};alignas(16)std::array<uint8,45*2+32> naviLinks{};
    const uintptr_t instance=uintptr_t(path.data()+16),source=uintptr_t(sources.data()+16),target=uintptr_t(targets.data()+16),mark=uintptr_t(marker.data()+16),nav=uintptr_t(navi.data()+16),nodeLinks=uintptr_t(links.data()+16),navLinks=uintptr_t(naviLinks.data()+16);
    std::vector<Block> blocks;blocks.emplace_back(uintptr_t(path.data()),path.size());blocks.emplace_back(uintptr_t(sources.data()),sources.size());blocks.emplace_back(uintptr_t(targets.data()),targets.size());blocks.emplace_back(uintptr_t(marker.data()),marker.size());blocks.emplace_back(uintptr_t(navi.data()),navi.size());blocks.emplace_back(uintptr_t(links.data()),links.size());blocks.emplace_back(uintptr_t(naviLinks.data()),naviLinks.size());
    const uintptr_t actual=uintptr_t(&ThePaths);const auto actualBefore=Hex(actual,sizeof(CPathFind));auto actualUnchanged=[&]{return Hex(actual,sizeof(CPathFind))==actualBefore;};
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;auto restore=[&]{for(const auto& b:blocks)b.Restore();RestoreFloatingState();};auto restored=[&]{return std::ranges::all_of(blocks,[](const auto& b){return b.Restored();})&&FloatingStateRestored();};
    bool ready=route.Registered()&&ModuleOf(route.Body())==ModuleOf(uintptr_t(&ModuleMarker));
    json descriptors=json::array();for(const auto& b:blocks)descriptors.push_back({{"address",b.address},{"bytes",b.saved.size()}});
    Write({{"event","actual_path_initial"},{"bytes",actualBefore}});
    Write({{"event","begin"},{"registered_hook",ready},{"functions",1},{"pairs",2048},{"collaborators_stubbed",false},{"collaborators",json::array()},{"snapshot_layout",descriptors},{"constant_bits",json::array({ReadInt(0x858C48),ReadInt(0x858A14),ReadInt(0x858FC0)})},{"floating_state_restored",FloatingStateRestored()}});
    bool passed=ready;int pairs=0,failures=0;
    for(int scenario=0;scenario<2048&&ready;++scenario){const unsigned profile=scenario%64,value=scenario/64,mode=profile%16;
        auto invoke=[&](bool candidate){
            restore();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
            const uint8 canaries[]{0xA5,0x5A,0xC3,0xD4,0xE5,0xB6,0xB7};for(unsigned i=0;i<blocks.size();++i){memset(reinterpret_cast<void*>(blocks[i].address),canaries[i],blocks[i].saved.size());memset(reinterpret_cast<void*>(blocks[i].address+16),0,blocks[i].saved.size()-32);}
            const unsigned area=profile,targetArea=mode==12?64+profile%8:(profile+2)%64,navArea=(profile+3)%64,missingTarget=(profile+4)%64,missingNav=(profile+5)%64;
            const uint32 count=mode==1?0:mode==2?0xFFFFFFFF:mode>=9&&mode!=14?3:1;
            const bool sourceLoaded=mode!=0,targetLoaded=mode!=4,navLoaded=mode!=5;
            PutInt(instance+0x804+4*area,sourceLoaded?uint32(source):0);PutInt(instance+0x10C4+4*area,count);
            PutInt(instance+0x804+4*targetArea,targetLoaded?uint32(target):0);PutInt(instance+0x10C4+4*targetArea,16);
            PutInt(instance+0x804+4*navArea,navLoaded?uint32(mark):0);PutInt(instance+0x924+4*navArea,uint32(nav));
            PutInt(instance+0xA44+4*area,uint32(nodeLinks));PutInt(instance+0xDA4+4*area,uint32(navLinks));
            int16 centerX=mode==12?32767:int16((int(value%5)-2)*128),centerY=mode==12?-32768:int16((int(value/5%5)-2)*128);
            json sourceData=json::array(),targetData=json::array(),naviData=json::array(),edgeData=json::array();
            for(unsigned n=0;n<3;++n){const int16 x=int16(centerX+n*8),y=int16(centerY+n*8);unsigned number=mode==3?0:mode==8||mode==10?15:mode==9?n+1:mode==15?(n==0?1:n==1?15:2):1;const uint8 flag=uint8(number|((profile%16)<<4));const uintptr_t a=source+n*0x1C;PutWord(a+8,x);PutWord(a+0xA,y);PutWord(a+0x10,int16(n*15));*reinterpret_cast<uint8*>(a+0x18)=flag;sourceData.push_back({{"x",x},{"y",y},{"links",number},{"base",n*15},{"flags",flag}});}
            for(unsigned n=0;n<16;++n){const int16 x=mode==11?centerX:int16(centerX+(int(n%5)-2)*8),y=mode==11?centerY:int16(centerY+(int(n/5)-1)*8);PutWord(target+n*0x1C+8,x);PutWord(target+n*0x1C+0xA,y);targetData.push_back({{"x",x},{"y",y}});}
            for(unsigned n=0;n<15;++n){unsigned opposite=(n+value/16)%8,same=(2*n+profile/16)%8;if(mode==6)opposite=same=0;if(mode==7)opposite=same=7;if(mode==15){opposite=0;same=n%2?7:0;}const uint8 flag=uint8(opposite|(same<<3)|((profile%4)<<6));*reinterpret_cast<uint8*>(nav+n*0xE+0xB)=flag;naviData.push_back(flag);}
            for(unsigned n=0;n<3;++n)for(unsigned e=0;e<15;++e){const unsigned linkArea=mode==14?area:mode==9&&(e%2)?missingTarget:targetArea,node=mode==14?0:(e+3*n)%15,linkNavArea=mode==10&&(e%2)?missingNav:navArea,navIndex=(e+n)%15;const unsigned i=n*15+e;PutInt(nodeLinks+4*i,linkArea|(node<<16));PutWord(navLinks+2*i,int16((linkNavArea<<10)|navIndex));edgeData.push_back({{"area",linkArea},{"node",node},{"navi_area",linkNavArea},{"navi",navIndex}});}
            float qx=float(centerX)*0.125f,qy=float(centerY)*0.125f;const unsigned query=value%16;const float below=std::bit_cast<float>(uint32(0x429FFFFF)),equal=80.f,above=std::bit_cast<float>(uint32(0x42A00001));
            if(query>=1&&query<=3)qx+=query==1?below:query==2?equal:above;
            if(query>=4&&query<=6)qy+=query==4?below:query==5?equal:above;
            if(query>=7&&query<=9)qx-=query==7?below:query==8?equal:above;
            if(query>=10&&query<=12)qy-=query==10?below:query==11?equal:above;
            if(query==13)qx=std::bit_cast<float>(uint32(0x7FC01234));if(query==14)qy=std::bit_cast<float>(uint32(0x7F800000));if(query==15){qx+=79.f;qy-=79.f;}
            const uint32 xbits=std::bit_cast<uint32>(qx),ybits=std::bit_cast<uint32>(qy);
            json input={{"profile",profile},{"value",value},{"mode",mode},{"source_area",area},{"target_area",targetArea},{"navi_area",navArea},{"source_count",count},{"source_loaded",sourceLoaded},{"target_loaded",targetLoaded},{"navi_loaded",navLoaded},{"x_bits",xbits},{"y_bits",ybits},{"sources",sourceData},{"targets",targetData},{"navi",naviData},{"edges",edgeData}};
            std::vector<std::string> prepared;for(const auto& b:blocks)prepared.push_back(b.Observe());bool routeOK=false;
            const auto result=route.Invoke(candidate,[&]{routeOK=ReversingProbe::JumpsToDll(0x44EFC0)==candidate;if(candidate)routeOK&=Resolve(0x44EFC0)==route.Body()&&ModuleOf(route.Body())==ModuleOf(uintptr_t(&ModuleMarker));return Gate(0x44EFC0,instance,xbits,ybits);});
            json state=json::array();bool readonly=true;for(unsigned n=0;n<blocks.size();++n){const auto seen=blocks[n].Observe();readonly&=seen==prepared[n];state.push_back(seen);}
            json out={{"input",input},{"eax",result.eax},{"ecx",result.ecx},{"edx",result.edx},{"argument0_after",result.arg0},{"argument1_after",result.arg1},{"float_bits",result.floatBits},{"extended_return",Hex(uintptr_t(result.extended.data()),10)},{"state",state},{"stack_restored",result.stack},{"nonvolatile_preserved",result.nonvolatile},{"route_ok",routeOK},{"arguments_unchanged",result.arg0==xbits&&result.arg1==ybits},{"fixture_readonly",readonly},{"actual_path_unchanged",actualUnchanged()}};
            restore();out["globals_restored"]=restored();out["floating_state_restored"]=FloatingStateRestored();return out;
        };
        const auto a=invoke(false),b=invoke(true);bool ok=a==b;for(const char* field:{"stack_restored","nonvolatile_preserved","route_ok","arguments_unchanged","fixture_readonly","actual_path_unchanged","globals_restored","floating_state_restored"})ok&=a[field]==true&&b[field]==true;passed&=ok;failures+=!ok;
        Write({{"event","comparison"},{"function","CPathFind::CalcRoadDensity"},{"case",scenario},{"pair",pairs++},{"original",a},{"candidate",b},{"equal",ok},{"accepted",ok}});
    }
    Write({{"event","function_summary"},{"function","CPathFind::CalcRoadDensity"},{"cases",pairs},{"failures",failures}});
    restore();bool stateRestored=restored()&&actualUnchanged()&&offCode==ReversingProbe::Bytes(reinterpret_cast<void*>(0x44EFC0),16);stateRestored&=route.Restore();for(const auto& [at,bytes]:initialCode)stateRestored&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
    Write({{"event","actual_path_final"},{"bytes",Hex(actual,sizeof(CPathFind))}});
    Write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"actual_path_unchanged",actualUnchanged()}});RestoreFloatingState();savedFPActive=false;
}
}
