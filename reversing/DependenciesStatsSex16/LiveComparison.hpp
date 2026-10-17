#pragma once
#include <fstream>
#include "Stats.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "Pad.h"
#include "PlayerInfo.h"
#include "Entity/Vehicle/Vehicle.h"
#include "References.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "HelperRoutes.hpp"
namespace DependenciesStatsSex16Live {
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
// 128 perfiles x 32 valores (16 del banco original + 16 de estres: NaN con carga/senalizante, +-inf, +-0, subnormal, +-FLT_MAX, +-1e30, 1e-30); ver fixture_oracle.py.
inline constexpr unsigned kCases=4096;
// true: la igualdad EXE/DLL exige los 108 bytes del FNSAVE (salvo FIP) incluido el contenido crudo de los registros ST marcados como vacios en FTW.
// false: esos registros vacios (residuo que ninguna instruccion puede leer sin cargarlo antes) se ponen a cero en ambos lados antes de comparar; FCW, FSW, FTW, FOP/FCS, FDP/FDS,
// los registros con datos y MXCSR siguen comparandose enteros. fp_difference de cada fila dice siempre que campos difieren, con el valor true.
inline constexpr bool kCompareDeadX87Registers=true;
struct Fn { const char* cls;const char* name;uintptr_t address;int reserved,unused,count; };
inline constexpr Fn fns[]{
    {"CStats","UpdateSexAppealStat",0x55BF20,0,-1,int(kCases)}
};

struct Result {uint32 eax,ecx,edx;bool stack,nonvolatile;};
inline Result Gate(uintptr_t entry){uint32 before{},after{},a{},c{},d{},b0{},s0{},i0{},p0{},b1{},s1{},i1{},p1{};
 __asm {mov before,esp} __asm {mov b0,ebx} __asm {mov s0,esi} __asm {mov i0,edi} __asm {mov p0,ebp}
 __asm {mov eax,0A5123456h} __asm {mov ecx,0B1234567h} __asm {mov edx,0C0DE5921h} __asm {call entry}
 __asm {mov a,eax} __asm {mov c,ecx} __asm {mov d,edx} __asm {mov after,esp} __asm {mov b1,ebx} __asm {mov s1,esi} __asm {mov i1,edi} __asm {mov p1,ebp}
 return {a,c,d,before==after,b0==b1&&s0==s1&&i0==i1&&p0==p1};}
inline std::string Hex(uintptr_t a,size_t n){constexpr char digits[]="0123456789abcdef";const auto* b=reinterpret_cast<const uint8*>(a);std::string out;out.reserve(n*2);for(size_t i=0;i<n;++i){out+=digits[b[i]>>4];out+=digits[b[i]&15];}return out;}
struct Block {uintptr_t address;std::vector<uint8> saved;Block(uintptr_t a,size_t n):address(a),saved(n){memcpy(saved.data(),reinterpret_cast<void*>(a),n);}void Restore()const{memcpy(reinterpret_cast<void*>(address),saved.data(),saved.size());}bool Restored()const{return !memcmp(reinterpret_cast<void*>(address),saved.data(),saved.size());}std::string Observe()const{return Hex(address,saved.size());}};

inline uint32 ReadInt(uintptr_t a){uint32 v{};memcpy(&v,reinterpret_cast<void*>(a),4);return v;}
inline void PutInt(uintptr_t a,uint32 v){memcpy(reinterpret_cast<void*>(a),&v,4);}
inline void PutFloat(uintptr_t a,float v){memcpy(reinterpret_cast<void*>(a),&v,4);}
inline uint32 FloatBits(float v){uint32 x{};memcpy(&x,&v,4);return x;}
inline void PutWord(uintptr_t a,int16 v){memcpy(reinterpret_cast<void*>(a),&v,2);}
inline json Fixture(unsigned scenario){const unsigned p=scenario%128,v=scenario/128;constexpr int classes[]{-128,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,127};constexpr uint32 health[]{0xC1200000,0x00000000,0x3F800000,0x43F9FFFF,0x43FA0000,0x43FA0001,0x443B8000,0x4479FFFF,0x447A0000,0x447A0001,0x44BB8000,0x44FA0000,0xC47A0000,0x437A0000,0x44160000,0x44480000,0x80000000,0x00000001,0x00800000,0x7FC01234,0x7F800001,0xFFC00001,0x7F800000,0xFF800000,0x7F7FFFFF,0xFF7FFFFF,0x7149F2CA,0xF149F2CA,0x0DA24260,0x3F800000,0x42C80000,0x44800000};constexpr uint32 tattoo[]{0xC47A0000,0xBF800000,0x00000000,0x3F800000,0x43FA0000,0x447A0000,0x44FA0000,0x457A0000,0x80000000,0x00000001,0x7FC0ABCD,0xFF800001,0x7F800000,0xFF800000,0x7F7FFFFF,0x0DA24260};constexpr uint32 distance[]{0x00000000,0x3F800000,0x420BFFFF,0x420C0000,0x420C0001,0x42100000,0xC20C0000,0x42C80000,0x7FC00001,0x7F800000,0xFF800000,0x7149F2CA,0x7F7FFFFF,0x00000001,0xC20BFFFD,0x80000000};return {{"profile",p},{"value",v},{"player_mode",p%4},{"cache_mode",p/4%3},{"reference_mode",p/32%3},{"vehicle_matrix",bool(p&16)},{"ped_matrix",bool(p&64)},{"cheat",bool(v&1)},{"vehicle_class",classes[p%16]},{"health_bits",health[v]},{"tattoo_bits",tattoo[(p+3*v)%16]},{"distance_bits",distance[p/4%16]}};}
inline bool RefNodeValid(uintptr_t at){return at>=0xB9B9A8&&at<0xBA1768&&(at-0xB9B9A8)%8==0;}
// FNSAVE en modo protegido de 32 bits (108 bytes): +0 FCW, +4 FSW, +8 FTW (2 bits por registro fisico, 3 = vacio), +12 FIP, +16 FCS|FOP<<16, +20 FDP, +24 FDS, +28 ST0..ST7 (10 bytes cada uno, orden de pila).
inline uint32 FpWord(const std::array<uint8,108>& fp,size_t at){uint32 v{};memcpy(&v,fp.data()+at,4);return v;}
inline bool FpRegisterIsDead(const std::array<uint8,108>& fp,unsigned st){const uint32 ftw=FpWord(fp,8)&0xFFFF,top=(FpWord(fp,4)>>11)&7;return ((ftw>>(2*((top+st)&7)))&3)==3;}
inline std::array<uint8,108> MaskDeadRegisters(std::array<uint8,108> fp){for(unsigned st=0;st<8;++st)if(FpRegisterIsDead(fp,st))memset(fp.data()+28+10*st,0,10);return fp;}
// Campos del FNSAVE que difieren (FIP ya viene a cero de ambos lados: se valida aparte). stN(dead) = registro vacio en FTW del lado original.
inline json FpDifference(const std::array<uint8,108>& a,const std::array<uint8,108>& b){
 json out=json::array();
 const struct {const char* name;size_t at,size;} fields[]{{"fcw",0,2},{"fsw",4,2},{"ftw",8,2},{"fcs_fop",16,4},{"fdp",20,4},{"fds",24,4}};
 for(const auto& f:fields)if(memcmp(a.data()+f.at,b.data()+f.at,f.size))out.push_back(f.name);
 for(unsigned st=0;st<8;++st)if(memcmp(a.data()+28+10*st,b.data()+28+10*st,10))out.push_back("st"+std::to_string(st)+(FpRegisterIsDead(a,st)?"(dead)":"(valid)"));
 return out;
}
// Ultima instruccion x87 de cada lado: `fstp dword [0xB793E4]` (D9 1D E4 93 B7 00). Se comprueba por los bytes en `ip` (FIP del FNSAVE), no por FDP: esta CPU deja FDP = 0
// (y FCS/FOP = 0) tras un fstp a memoria, tambien en el EXE (medido con un fstp seguido de fnsave en un programa aparte), asi que FDP no identifica la instruccion.
// FDP, FCS|FOP y FDS siguen comparandose enteros entre EXE y DLL en el FNSAVE (solo los cuatro bytes de FIP se normalizan).
inline bool FpIpIsFinalStore(uint32 ip,bool candidate){
 if(!ip)return false;
 if(candidate?ModuleOf(ip)!=ModuleOf(uintptr_t(&ModuleMarker)):ip!=0x55C093)return false;
 const auto* code=reinterpret_cast<const uint8*>(ip);uint32 address{};memcpy(&address,code+2,4);
 return code[0]==0xD9&&code[1]==0x1D&&address==0xB793E4;
}
// Registro compacto: los bloques de mas de 16 KB (la tabla de referencias) se reducen a longitud + FNV-1a de 64 bits cuando el par es igual; la comparacion en memoria es siempre byte a byte.
inline json Slim(json row){
 for(auto& block:row["state"])if(block.is_string()&&block.get_ref<const std::string&>().size()>16384){const std::string text=block.get<std::string>();uint64_t h=1469598103934665603ull;for(unsigned char c:text){h^=c;h*=1099511628211ull;}block=json{{"bytes",text.size()/2},{"fnv1a64",h}};}
 return row;
}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependenciesstatssex16-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependenciesstatssex16-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesstatssex16");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 alignas(16)std::array<uint8,0x7A4+32> pedBytes{};
 alignas(16)std::array<uint8,0x5A0+32> currentBytes{},previousBytes{};
 alignas(16)std::array<uint8,0x48+32> pedMatrixBytes{},currentMatrixBytes{},previousMatrixBytes{};
 alignas(16)std::array<uint8,0x308+32> modelBytes{};uint32 dummy{};
 const uintptr_t ped=uintptr_t(pedBytes.data())+16,current=uintptr_t(currentBytes.data())+16,previous=uintptr_t(previousBytes.data())+16;
 const uintptr_t pedMatrix=uintptr_t(pedMatrixBytes.data())+16,currentMatrix=uintptr_t(currentMatrixBytes.data())+16,previousMatrix=uintptr_t(previousMatrixBytes.data())+16,model=uintptr_t(modelBytes.data())+16;
 int slot=-1;for(int id=19999;id>=19000;--id)if(!ReadInt(0xA9B0C8+4*id)){slot=id;break;}
 const uintptr_t first=ReadInt(0xBA1768),second=RefNodeValid(first)?ReadInt(first):0,third=RefNodeValid(second)?ReadInt(second):0;
 bool refsValid=RefNodeValid(first)&&RefNodeValid(second)&&RefNodeValid(third)&&first!=second&&first!=third&&second!=third;
 HelperRoute targetSnapshot(0x55BF20);
 const json targetInitial=targetSnapshot.Metadata();   // antes de HookRoute: su constructor deja el hook apagado y el estado inicial (habilitado) ya no coincidiria
 OriginalDependencies499::HookRoute route("CStats","UpdateSexAppealStat",0x55BF20,&CStats::UpdateSexAppealStat);
 const uintptr_t raw=uintptr_t(&CStats::UpdateSexAppealStat);
 std::array<HelperRoute,7> helpers{HelperRoute(0x56E0D0),HelperRoute(0x56E010),HelperRoute(0x571B70),HelperRoute(0x559760),HelperRoute(0x53FB70),HelperRoute(0x56E120),HelperRoute(0x56E210)};
 auto helpersOK=[&]{return std::ranges::all_of(helpers,[](const auto& h){return h.Check();});};
 bool ready=slot>=0&&refsValid&&route.Registered()&&ModuleOf(route.Body())==ModuleOf(uintptr_t(&ModuleMarker))&&helpersOK();
 for(const auto& h:helpers){auto m=h.Metadata();const auto at=m["entry"].get<uintptr_t>();if(at==0x56E0D0||at==0x56E010||at==0x571B70||at==0x559760)ready&=m["registered"]==true&&m["reconstructed"]==true&&m["enabled"]==true;}
 std::vector<Block> blocks;
 blocks.emplace_back(0xB78200,0x1338);blocks.emplace_back(0xB7CD98,0x320);blocks.emplace_back(0xB7CD74,1);blocks.emplace_back(0x969180,1);blocks.emplace_back(0xB9B9A8,24004);
 if(slot>=0)blocks.emplace_back(0xA9B0C8+4*slot,4);
 blocks.emplace_back(uintptr_t(&CPad::GetPad(0)->JustOutOfFrontEnd),1);
 const std::array<std::pair<uintptr_t,size_t>,8> buffers{{{uintptr_t(pedBytes.data()),pedBytes.size()},{uintptr_t(currentBytes.data()),currentBytes.size()},{uintptr_t(previousBytes.data()),previousBytes.size()},{uintptr_t(pedMatrixBytes.data()),pedMatrixBytes.size()},{uintptr_t(currentMatrixBytes.data()),currentMatrixBytes.size()},{uintptr_t(previousMatrixBytes.data()),previousMatrixBytes.size()},{uintptr_t(modelBytes.data()),modelBytes.size()},{uintptr_t(&dummy),4}}};
 for(const auto& b:buffers)blocks.emplace_back(b.first,b.second);
 SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
 auto restore=[&]{for(const auto& b:blocks)b.Restore();RestoreFloatingState();};
 auto restored=[&]{return std::ranges::all_of(blocks,[](const auto& b){return b.Restored();})&&FloatingStateRestored();};
 json layout=json::array(),initial=json::array(),metadata=json::array();for(const auto& b:blocks){layout.push_back({{"address",b.address},{"bytes",b.saved.size()}});initial.push_back(b.Observe());}for(const auto& h:helpers)metadata.push_back(h.Metadata());
 Write({{"event","begin"},{"registered_hook",ready},{"functions",1},{"pairs",kCases},{"cases",kCases},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"collaborators_stubbed",false},{"helper_routes",metadata},{"target_initial",targetInitial},{"snapshot_layout",layout},{"initial_state",initial},{"initial_fp",Hex(uintptr_t(savedFP.data()),108)},{"initial_mxcsr",savedMXCSR},{"unused_model_slot",slot},{"refs_head",first},{"refs_second",second},{"refs_third",third},{"refs_pool_valid",refsValid},{"body",route.Body()},{"raw",raw},{"asi_module",ModuleOf(uintptr_t(&ModuleMarker))},{"original_module",uintptr_t(GetModuleHandleA(nullptr))}});
 int pairs{},failures{};bool passed=ready;std::array<std::array<uint8,108>,2> fpNormalized{};
 for(unsigned scenario=0;scenario<kCases&&ready;++scenario){const auto input=Fixture(scenario);std::array<std::string,2> observedFP;std::array<uint32,2> observedIP{};
  auto invoke=[&](bool candidate){restore();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
   auto init=[&](uintptr_t base,size_t size){memset(reinterpret_cast<void*>(base-16),0xA5,size+32);memset(reinterpret_cast<void*>(base),0,size);};
   init(ped,0x7A4);init(current,0x5A0);init(previous,0x5A0);init(pedMatrix,0x48);init(currentMatrix,0x48);init(previousMatrix,0x48);init(model,0x308);
   PutInt(0xA9B0C8+4*slot,uint32(model));*reinterpret_cast<int8*>(model+0x4D)=input["vehicle_class"].get<int8>();
   const auto mode=input["player_mode"].get<unsigned>(),cache=input["cache_mode"].get<unsigned>(),reference=input["reference_mode"].get<unsigned>();
   *reinterpret_cast<uint8*>(0xB7CD74)=0;PutInt(0xB7CD98,mode?uint32(ped):0);CPad::GetPad(0)->JustOutOfFrontEnd=1;
   PutInt(ped+0x46C,mode>=2?0x100:0);PutInt(ped+0x58C,mode==3?uint32(current):0);
   const bool pm=input["ped_matrix"].get<bool>(),vm=input["vehicle_matrix"].get<bool>();
   PutInt(ped+0x14,pm?uint32(pedMatrix):0);PutInt(current+0x14,vm?uint32(currentMatrix):0);PutInt(previous+0x14,vm?uint32(previousMatrix):0);
   const uint32 distance=input["distance_bits"].get<uint32>(),health=input["health_bits"].get<uint32>();
   PutInt(ped+4,distance);PutInt(pedMatrix+0x30,distance);PutFloat(ped+12,13.f);PutFloat(pedMatrix+0x38,13.f);
   for(auto at:{current,previous}){PutFloat(at+12,42.f);PutWord(at+0x22,int16(slot));*reinterpret_cast<uint8*>(at+0x36)=2;PutInt(at+0x4C0,health);}
   PutFloat(currentMatrix+0x38,42.f);PutFloat(previousMatrix+0x38,42.f);
   PutInt(0xB79530,cache==1?uint32(previous):cache==2?uint32(current):0);PutInt(0xB794C0,input["tattoo_bits"].get<uint32>());PutInt(0xB793E4,0xC1234567);*reinterpret_cast<uint8*>(0x969180)=input["cheat"].get<bool>()?1:0;dummy=uint32(current);
   if(reference){PutInt(first,0);PutInt(first+4,reference==1?0xB79530:uint32(uintptr_t(&dummy)));PutInt(current+0x24,uint32(first));PutInt(0xBA1768,uint32(second));}
   alignas(16)std::array<uint8,108> fp{};uint32 mx{};bool routeOK{};uint32 ip{};
   const auto result=route.Invoke(candidate,[&]{routeOK=candidate?ReversingProbe::JumpsToDll(0x55BF20)&&ModuleOf(Resolve(0x55BF20))==ModuleOf(uintptr_t(&ModuleMarker)):!ReversingProbe::JumpsToDll(0x55BF20);const auto value=Gate(0x55BF20);SaveFP(fp.data());mx=ReadMXCSR();return value;});
   memcpy(&ip,fp.data()+12,4);observedIP[candidate?1:0]=ip;observedFP[candidate?1:0]=Hex(uintptr_t(fp.data()),108);
   memset(fp.data()+12,0,4);fpNormalized[candidate?1:0]=fp;const auto logged=kCompareDeadX87Registers?fp:MaskDeadRegisters(fp);const bool ipOK=FpIpIsFinalStore(ip,candidate);
   json state=json::array();for(const auto& b:blocks)state.push_back(b.Observe());
   json out={{"input",input},{"state",state},{"eax",result.eax},{"ecx",result.ecx},{"edx",result.edx},{"post_floating_state",Hex(uintptr_t(logged.data()),108)},{"post_mxcsr",mx},{"fp_ip_route_ok",ipOK},{"stack_restored",result.stack},{"nonvolatile_preserved",result.nonvolatile},{"route_ok",routeOK},{"helper_routes_ok",helpersOK()}};
   restore();out["globals_restored"]=restored();out["floating_state_restored"]=FloatingStateRestored();return out;
  };
  const auto a=invoke(false),b=invoke(true);bool ok=a==b;const auto fpDifference=FpDifference(fpNormalized[0],fpNormalized[1]);for(const char* k:{"fp_ip_route_ok","stack_restored","nonvolatile_preserved","route_ok","helper_routes_ok","globals_restored","floating_state_restored"})ok&=a[k]==true&&b[k]==true;
  passed&=ok;failures+=!ok;Write({{"event","comparison"},{"function","CStats::UpdateSexAppealStat"},{"case",scenario},{"pair",pairs++},{"original",ok?Slim(a):a},{"candidate",ok?Slim(b):b},{"fp_difference",fpDifference},{"original_fp_raw",observedFP[0]},{"candidate_fp_raw",observedFP[1]},{"original_fp_ip_raw",observedIP[0]},{"candidate_fp_ip_raw",observedIP[1]},{"equal",ok},{"accepted",ok}});
 }
 restore();bool stateRestored=restored();stateRestored&=route.Restore();stateRestored&=helpersOK();stateRestored&=targetSnapshot.Check();
 json final=json::array();for(const auto& b:blocks)final.push_back(b.Observe());alignas(16)std::array<uint8,108> finalFP{};SaveFP(finalFP.data());
 Write({{"event","function_summary"},{"function","CStats::UpdateSexAppealStat"},{"cases",pairs},{"failures",failures},{"state_restored",stateRestored}});
 Write({{"event","global_state_final"},{"state",final},{"floating_state",Hex(uintptr_t(finalFP.data()),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"target_restored",targetSnapshot.Check()}});
 Write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"helper_routes_ok",helpersOK()},{"target_restored",targetSnapshot.Check()}});restore();savedFPActive=false;
}
}
