#pragma once
#include <fstream>
#include "PathFind.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"


extern void PathOrientation14Normalise();

namespace DependenciesPathOrientation14Live {
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
    {"CPathFind","FindNodeOrientationForCarPlacement",0x450320,0,-1,2048}
};
inline uintptr_t Resolve(uintptr_t a){for(unsigned n=0;n<16&&*reinterpret_cast<uint8*>(a)==0xE9;++n)a+=5+*reinterpret_cast<int32*>(a+1);return a;}
inline uintptr_t ModuleOf(uintptr_t a){MEMORY_BASIC_INFORMATION i{};return VirtualQuery(reinterpret_cast<void*>(a),&i,sizeof(i))==sizeof(i)?uintptr_t(i.AllocationBase):0;}
inline void ModuleMarker(){}

struct Result {uint32 eax,ecx,edx,arg0,arg1,floatBits;std::array<uint8,10> extended;bool stack,nonvolatile;};
inline Result Gate(uintptr_t entry,uintptr_t instance,uint32 argument0){
    uint32 before{},after{},a{},c{},d{},x{},y{},b0{},s0{},i0{},p0{},b1{},s1{},i1{},p1{};float returned{};std::array<uint8,10> extended{};auto* extendedOutput=extended.data();
    __asm {
        mov before,esp
        mov b0,ebx
        mov s0,esi
        mov i0,edi
        mov p0,ebp
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
        mov eax,[esp-4]
        mov x,eax
        xor eax,eax
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

struct HelperResult {uint32 eax,ecx,edx;bool stack,nonvolatile;};
inline HelperResult HelperGate(uintptr_t entry,uintptr_t instance){
 uint32 before{},after{},a{},c{},d{},b0{},s0{},i0{},p0{},b1{},s1{},i1{},p1{};
 __asm {mov before,esp} __asm {mov b0,ebx} __asm {mov s0,esi} __asm {mov i0,edi} __asm {mov p0,ebp}
 __asm {mov eax,0A5123456h} __asm {mov edx,0C0DE5921h} __asm {mov ecx,instance} __asm {call entry}
 __asm {mov a,eax} __asm {mov c,ecx} __asm {mov d,edx} __asm {mov b1,ebx} __asm {mov s1,esi} __asm {mov i1,edi} __asm {mov p1,ebp} __asm {mov after,esp}
 return {a,c,d,before==after,b0==b1&&s0==s1&&i0==i1&&p0==p1};
}
inline json FPObserve(){alignas(16)std::array<uint8,108> state{};SaveFP(state.data());const uint32 ip=ReadInt(uintptr_t(state.data()+12));memset(state.data()+12,0,4);return {{"numeric_env",Hex(uintptr_t(state.data()),state.size())},{"instruction_pointer",ip},{"mxcsr",ReadMXCSR()}};}
inline void ResetTestFP(){__asm {fninit} _controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);float sentinel=3.25f;__asm {fld sentinel}}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependenciespathorientation14-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependenciespathorientation14-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciespathorientation14");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 static_assert(sizeof(CPathFind)==0x3C80&&sizeof(CPathNode)==0x1C&&sizeof(CCarPathLink)==0xE);
 const auto member=&CPathFind::FindNodeOrientationForCarPlacement;const uintptr_t raw=uintptr_t(FunctionToVoidPtr(member)),helperRaw=uintptr_t(&PathOrientation14Normalise),helper=Resolve(helperRaw);
 std::vector<std::pair<uintptr_t,std::vector<uint8>>> initialCode;for(auto at:{uintptr_t(0x450320),raw,Resolve(raw),uintptr_t(0x59C910),helperRaw,helper})initialCode.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
 OriginalDependencies499::HookRoute route("CPathFind","FindNodeOrientationForCarPlacement",0x450320,FunctionToVoidPtr(member));const auto offCode=ReversingProbe::Bytes(reinterpret_cast<void*>(0x450320),16);
 alignas(16)std::array<uint8,0x3CA0> path{};alignas(16)std::array<uint8,3*0x1C+32> sources{};alignas(16)std::array<uint8,16*0x1C+32> targets{};
 alignas(16)std::array<uint8,0x1C+32> marker{};alignas(16)std::array<uint8,15*0xE+32> navi{};alignas(16)std::array<uint8,45*4+32> links{};alignas(16)std::array<uint8,45*2+32> naviLinks{};
 const uintptr_t instance=uintptr_t(path.data()+16),source=uintptr_t(sources.data()+16),target=uintptr_t(targets.data()+16),mark=uintptr_t(marker.data()+16),nav=uintptr_t(navi.data()+16),nodeLinks=uintptr_t(links.data()+16),navLinks=uintptr_t(naviLinks.data()+16);
 std::vector<Block> blocks;blocks.emplace_back(uintptr_t(path.data()),path.size());blocks.emplace_back(uintptr_t(sources.data()),sources.size());blocks.emplace_back(uintptr_t(targets.data()),targets.size());blocks.emplace_back(uintptr_t(marker.data()),marker.size());blocks.emplace_back(uintptr_t(navi.data()),navi.size());blocks.emplace_back(uintptr_t(links.data()),links.size());blocks.emplace_back(uintptr_t(naviLinks.data()),naviLinks.size());
 const uintptr_t actual=uintptr_t(&ThePaths);const auto actualBefore=Hex(actual,sizeof(CPathFind));auto actualUnchanged=[&]{return Hex(actual,sizeof(CPathFind))==actualBefore;};
 SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;auto restore=[&]{for(const auto& b:blocks)b.Restore();RestoreFloatingState();};auto restored=[&]{return std::ranges::all_of(blocks,[](const auto& b){return b.Restored();})&&FloatingStateRestored();};
 auto helperRouteOK=[&]{return ModuleOf(helper)==ModuleOf(uintptr_t(&ModuleMarker))&&ModuleOf(0x59C910)==uintptr_t(GetModuleHandleA(nullptr))&&!ReversingProbe::JumpsToDll(0x59C910)&&*reinterpret_cast<uint8*>(helper)!=0xE9;};
 bool ready=route.Registered()&&ModuleOf(route.Body())==ModuleOf(uintptr_t(&ModuleMarker))&&helperRouteOK(),passed=ready;
 json descriptors=json::array();for(const auto& b:blocks)descriptors.push_back({{"address",b.address},{"bytes",b.saved.size()}});
 Write({{"event","actual_path_initial"},{"bytes",actualBefore}});
 Write({{"event","begin"},{"registered_hook",ready},{"functions",1},{"pairs",2048},{"helper_equivalence_cases",2048},{"helper_entry",0x59C910},{"helper_body",helper},{"helper_route_ok",helperRouteOK()},{"collaborators_stubbed",false},{"snapshot_layout",descriptors},{"floating_state_restored",FloatingStateRestored()}});
 alignas(16)std::array<uint8,44> vector{};Block vectorSaved(uintptr_t(vector.data()),vector.size());
 int helperFailures=0;
 const float components[]{0.f,1.f,-1.f,3.f,4.f,.125f,-.125f,10000000000.f,.0000000001f};
 for(unsigned scenario=0;scenario<2048&&ready;++scenario){const unsigned p=scenario%32,v=scenario/32;const float x=p==0?0.f:components[(p+v)%9],y=p==0?0.f:components[(2*p+v/3)%9],z=p==0?0.f:components[(3*p+v/7)%9];
  uint32 originalIP{},candidateIP{};auto invoke=[&](bool candidate){RestoreFloatingState();vectorSaved.Restore();memset(vector.data(),0xAB,vector.size());PutInt(uintptr_t(vector.data()+16),std::bit_cast<uint32>(x));PutInt(uintptr_t(vector.data()+20),std::bit_cast<uint32>(y));PutInt(uintptr_t(vector.data()+24),std::bit_cast<uint32>(z));ResetTestFP();const auto r=HelperGate(candidate?helper:0x59C910,uintptr_t(vector.data()+16));auto fp=FPObserve();const uint32 ip=fp["instruction_pointer"];(candidate?candidateIP:originalIP)=ip;const bool executedBody=candidate?(ip>=helper&&ip<helper+256):(ip>=0x59C910&&ip<=0x59C960);fp.erase("instruction_pointer");json out={{"input",json::array({std::bit_cast<uint32>(x),std::bit_cast<uint32>(y),std::bit_cast<uint32>(z)})},{"state",vectorSaved.Observe()},{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"stack_restored",r.stack},{"nonvolatile_preserved",r.nonvolatile},{"route_ok",helperRouteOK()&&executedBody},{"floating_observation",fp}};vectorSaved.Restore();RestoreFloatingState();out["globals_restored"]=vectorSaved.Restored();out["floating_state_restored"]=FloatingStateRestored();return out;};
  const auto a=invoke(false),b=invoke(true);bool ok=a==b;for(const char* k:{"stack_restored","nonvolatile_preserved","route_ok","globals_restored","floating_state_restored"})ok&=a[k]==true&&b[k]==true;passed&=ok;helperFailures+=!ok;Write({{"event","helper_equivalence"},{"case",scenario},{"original_fp_ip_raw",originalIP},{"candidate_fp_ip_raw",candidateIP},{"original",a},{"candidate",b},{"equal",ok},{"accepted",ok}});
 }
 Write({{"event","helper_summary"},{"cases",ready?2048:0},{"failures",helperFailures},{"state_restored",vectorSaved.Restored()},{"floating_state_restored",FloatingStateRestored()}});
 int pairs=0,failures=0;
 for(unsigned scenario=0;scenario<2048&&ready;++scenario){const unsigned profile=scenario%64,value=scenario/64,mode=profile%16;
  auto invoke=[&](bool candidate){restore();ResetTestFP();
   const uint8 canaries[]{0xA5,0x5A,0xC3,0xD4,0xE5,0xB6,0xB7};for(unsigned i=0;i<blocks.size();++i){memset(reinterpret_cast<void*>(blocks[i].address),canaries[i],blocks[i].saved.size());memset(reinterpret_cast<void*>(blocks[i].address+16),0,blocks[i].saved.size()-32);}
   const unsigned area=profile,targetArea=mode==12?64+profile%8:(profile+2)%64,navArea=(profile+3)%64,missingNav=(profile+5)%64,nodeIndex=value%3;
   const bool sourceLoaded=mode!=0||(value%2),targetLoaded=mode!=4,navLoaded=mode!=5;const uint32 argument=area|(nodeIndex<<16);const unsigned number=mode;
   PutInt(instance+0x804+4*area,sourceLoaded?uint32(source):0);PutInt(instance+0x804+4*targetArea,targetLoaded?uint32(target):0);PutInt(instance+0x804+4*navArea,navLoaded?uint32(mark):0);PutInt(instance+0x924+4*navArea,uint32(nav));PutInt(instance+0xA44+4*area,uint32(nodeLinks));PutInt(instance+0xDA4+4*area,uint32(navLinks));
   int16 cx=mode==12?32767:int16((int(value%5)-2)*128),cy=mode==12?-32768:int16((int(value/5%5)-2)*128);
   json sourceData=json::array(),targetData=json::array(),naviData=json::array(),edgeData=json::array();
   for(unsigned n=0;n<3;++n){const int16 x=int16(cx+n*8),y=int16(cy+n*8),z=int16(int(value%9)-4);const uint8 flag=uint8(number|((profile%16)<<4));const uintptr_t a=source+n*0x1C;PutWord(a+8,x);PutWord(a+0xA,y);PutWord(a+0xC,z);PutWord(a+0x10,int16(n*15));*reinterpret_cast<uint8*>(a+0x18)=flag;sourceData.push_back({{"x",x},{"y",y},{"z",z},{"links",number},{"base",n*15},{"flags",flag}});}
   for(unsigned n=0;n<16;++n){const int16 x=mode==11?int16(cx+nodeIndex*8):int16(cx+(int(n%5)-2)*8),y=mode==11?int16(cy+nodeIndex*8):int16(cy+(int(n/5)-1)*8),z=int16(int(n)-8);PutWord(target+n*0x1C+8,x);PutWord(target+n*0x1C+0xA,y);PutWord(target+n*0x1C+0xC,z);targetData.push_back({{"x",x},{"y",y},{"z",z}});}
   for(unsigned n=0;n<15;++n){unsigned opposite=(n+value/16)%8,same=(2*n+profile/16)%8;if(mode==6)opposite=same=0;if(mode==7)opposite=same=7;const uint8 flag=uint8(opposite|(same<<3)|((profile%4)<<6));const unsigned ownerArea=value%2?area:targetArea,ownerNode=value%3?nodeIndex:(nodeIndex+1)%3;const uintptr_t a=nav+n*0xE;PutWord(a+4,int16(ownerArea));PutWord(a+6,int16(ownerNode));*reinterpret_cast<uint8*>(a+0xB)=flag;naviData.push_back({{"flags",flag},{"area",ownerArea},{"node",ownerNode}});}
   for(unsigned n=0;n<3;++n)for(unsigned e=0;e<15;++e){const unsigned linkArea=mode==14?area:targetArea,node=mode==14?(n+1)%3:(e+3*n)%15,linkNavArea=mode==10&&(e%2)?missingNav:navArea,navIndex=(e+n)%15;const unsigned i=n*15+e;PutInt(nodeLinks+4*i,linkArea|(node<<16));PutWord(navLinks+2*i,int16((linkNavArea<<10)|navIndex));edgeData.push_back({{"area",linkArea},{"node",node},{"navi_area",linkNavArea},{"navi",navIndex}});}
   json input={{"profile",profile},{"value",value},{"mode",mode},{"source_area",area},{"target_area",targetArea},{"navi_area",navArea},{"source_loaded",sourceLoaded},{"target_loaded",targetLoaded},{"navi_loaded",navLoaded},{"argument",argument},{"sources",sourceData},{"targets",targetData},{"navi",naviData},{"edges",edgeData}};
   std::vector<std::string> prepared;for(const auto& b:blocks)prepared.push_back(b.Observe());bool routeOK=false;
   const auto result=route.Invoke(candidate,[&]{routeOK=ReversingProbe::JumpsToDll(0x450320)==candidate&&helperRouteOK();if(candidate)routeOK&=Resolve(0x450320)==route.Body()&&ModuleOf(route.Body())==ModuleOf(uintptr_t(&ModuleMarker));return Gate(0x450320,instance,argument);});
   json state=json::array();bool readonly=true;for(unsigned n=0;n<blocks.size();++n){const auto seen=blocks[n].Observe();readonly&=seen==prepared[n];state.push_back(seen);}
   json out={{"input",input},{"eax",result.eax},{"ecx",result.ecx},{"edx",result.edx},{"argument_after",result.arg0},{"float_bits",result.floatBits},{"extended_return",Hex(uintptr_t(result.extended.data()),10)},{"state",state},{"stack_restored",result.stack},{"nonvolatile_preserved",result.nonvolatile},{"route_ok",routeOK},{"fixture_readonly",readonly},{"actual_path_unchanged",actualUnchanged()}};
   restore();out["globals_restored"]=restored();out["floating_state_restored"]=FloatingStateRestored();return out;
  };
  const auto a=invoke(false),b=invoke(true);bool ok=a==b;for(const char* field:{"stack_restored","nonvolatile_preserved","route_ok","fixture_readonly","actual_path_unchanged","globals_restored","floating_state_restored"})ok&=a[field]==true&&b[field]==true;passed&=ok;failures+=!ok;Write({{"event","comparison"},{"function","CPathFind::FindNodeOrientationForCarPlacement"},{"case",scenario},{"pair",pairs++},{"original",a},{"candidate",b},{"equal",ok},{"accepted",ok}});
 }
 Write({{"event","function_summary"},{"function","CPathFind::FindNodeOrientationForCarPlacement"},{"cases",pairs},{"failures",failures}});
 restore();bool stateRestored=restored()&&actualUnchanged()&&vectorSaved.Restored()&&offCode==ReversingProbe::Bytes(reinterpret_cast<void*>(0x450320),16);stateRestored&=route.Restore();for(const auto& [at,bytes]:initialCode)stateRestored&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
 Write({{"event","actual_path_final"},{"bytes",Hex(actual,sizeof(CPathFind))}});Write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pairs},{"helper_equivalence_cases",ready?2048:0},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"actual_path_unchanged",actualUnchanged()},{"helper_route_ok",helperRouteOK()}});RestoreFloatingState();savedFPActive=false;
}
}
