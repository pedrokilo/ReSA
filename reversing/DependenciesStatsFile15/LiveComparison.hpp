#pragma once
#include <fstream>
#include "Stats.h"
#include "CutsceneMgr.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include <cerrno>
#include <direct.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "HelperRoutes.hpp"


namespace DependenciesStatsFile15Live {
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
inline int protectedErrno{};inline DWORD protectedWinError{};inline bool protectedErrorsActive{};
inline void RestoreErrors(){if(protectedErrorsActive){errno=protectedErrno;SetLastError(protectedWinError);}}
inline void Write(const json& row) {RestoreFloatingState();RestoreErrors();ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();RestoreFloatingState();RestoreErrors();}
struct Fn { const char* cls;const char* name;uintptr_t address;int reserved,unused,count; };
inline constexpr Fn fns[]{
    {"CStats","LoadStatUpdateConditions",0x559860,0,-1,2048}
};

struct Result {uint32 eax,ecx,edx;bool stack,nonvolatile;};
inline Result Gate(uintptr_t entry){uint32 before{},after{},a{},c{},d{},b0{},s0{},i0{},p0{},b1{},s1{},i1{},p1{};
 __asm {mov before,esp} __asm {mov b0,ebx} __asm {mov s0,esi} __asm {mov i0,edi} __asm {mov p0,ebp}
 __asm {mov eax,0A5123456h} __asm {mov ecx,0B1234567h} __asm {mov edx,0C0DE5921h} __asm {call entry}
 __asm {mov a,eax} __asm {mov c,ecx} __asm {mov d,edx} __asm {mov after,esp} __asm {mov b1,ebx} __asm {mov s1,esi} __asm {mov i1,edi} __asm {mov p1,ebp}
 return {a,c,d,before==after,b0==b1&&s0==s1&&i0==i1&&p0==p1};}
inline std::string Hex(uintptr_t a,size_t n){constexpr char digits[]="0123456789abcdef";const auto* b=reinterpret_cast<const uint8*>(a);std::string out;out.reserve(n*2);for(size_t i=0;i<n;++i){out+=digits[b[i]>>4];out+=digits[b[i]&15];}return out;}
struct Block {uintptr_t address;std::vector<uint8> saved;Block(uintptr_t a,size_t n):address(a),saved(n){memcpy(saved.data(),reinterpret_cast<void*>(a),n);}void Restore()const{memcpy(reinterpret_cast<void*>(address),saved.data(),saved.size());}bool Restored()const{return !memcmp(reinterpret_cast<void*>(address),saved.data(),saved.size());}std::string Observe()const{return Hex(address,saved.size());}};
inline std::string ReadFile(const std::filesystem::path& file){std::ifstream in(file,std::ios::binary);return {std::istreambuf_iterator<char>(in),std::istreambuf_iterator<char>()};}
inline bool WriteFile(const std::filesystem::path& file,const std::string& bytes){std::ofstream out(file,std::ios::binary|std::ios::trunc);out.write(bytes.data(),bytes.size());out.close();return bool(out);}
inline bool ExclusiveOpenClosed(const std::filesystem::path& file){const auto h=CreateFileW(file.c_str(),GENERIC_READ,0,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);if(h==INVALID_HANDLE_VALUE)return false;return CloseHandle(h)!=0;}
inline std::wstring WinCwd(){std::array<wchar_t,32768> b{};const auto n=GetCurrentDirectoryW(b.size(),b.data());return n&&n<b.size()?std::wstring(b.data(),n):std::wstring();}
inline std::string CRSCwd(){std::array<char,32768> b{};return _getcwd(b.data(),b.size())?std::string(b.data()):std::string();}
inline json Fixture(unsigned scenario){const unsigned profile=scenario%32,value=scenario/32,mode=profile%8;const unsigned counts[]{0,1,2,7,8,16,31,64,127,128};const char* ids[]{"-32768","-1","0","32767","65535","2147483647","-2147483648"};const char* floats[]{"0","-0","1.25","-2.5","1e-10","1e10","3.4028234e38"};const std::string end=mode&2?"\r\n":"\n",delimiter=mode&4?",\t":" ";std::string file=value%2?"# Header"+end:std::string();json rows=json::array();
 for(unsigned n=0;n<counts[profile%10];++n){const char* id=ids[(n+value)%7],*number=floats[(2*n+value)%7],*condition=(n+profile)%3==0?"lessthan":(n+profile)%3==1?"morethan":"equals";char key[32]{};snprintf(key,sizeof(key),"K%06u",(value*131+n)%1000000);if((mode&1)&&n%3==0)file+="# Comment"+end;file+=id+delimiter+"STAT"+delimiter+condition+delimiter+number+delimiter+key+end;if(mode&4)file+="  \t"+end;rows.push_back({{"id",id},{"number",number},{"condition",condition},{"key",key}});}
 if(mode&1)file+="# Tail"+end;return {{"profile",profile},{"value",value},{"mode",mode},{"count",counts[profile%10]},{"initial_pattern",0xA0+value%16},{"line_pattern",0xB0+value%16},{"errno_seed",value%4==0?0:value%4==1?2:value%4==2?22:34},{"file",file},{"rows",rows}};}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependenciesstatsfile15-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependenciesstatsfile15-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesstatsfile15");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 static_assert(sizeof(tStatMessage)==16);
 const auto winBefore=WinCwd();const auto crtBefore=CRSCwd();const int errnoBefore=errno;const DWORD errorBefore=GetLastError();protectedErrno=errnoBefore;protectedWinError=errorBefore;protectedErrorsActive=true;
 SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
 std::vector<Block> blocks;blocks.emplace_back(0xB781F0,128*16+32);blocks.emplace_back(0xB794D0,4);blocks.emplace_back(0xB71848,512);blocks.emplace_back(0xB71A60,128);blocks.emplace_back(0xB71AE0,128);
 const auto root=dir/("dependenciesstatsfile15-fixture-"+std::to_string(GetCurrentProcessId())),data=root/"DATA",file=data/"STATDISP.DAT";bool owned=false;
 std::error_code scopeError;const bool inClone=std::filesystem::equivalent(dir,std::filesystem::path("E:/JUEGOS/GTA San Andreas - CVehicle Test"),scopeError)&&!scopeError;
 if(inClone&&root.parent_path()==dir&&!std::filesystem::exists(root)){owned=std::filesystem::create_directories(data);}
 const auto fixtureRoot=root.string()+"\\";
 auto restore=[&]{for(const auto& b:blocks)b.Restore();const bool cr=_chdir(crtBefore.c_str())==0;const bool wr=SetCurrentDirectoryW(winBefore.c_str())!=0;errno=errnoBefore;SetLastError(errorBefore);RestoreFloatingState();return cr&&wr;};
 auto restored=[&]{const bool errors=errno==errnoBefore&&GetLastError()==errorBefore;const bool states=std::ranges::all_of(blocks,[](const auto& b){return b.Restored();})&&CRSCwd()==crtBefore&&WinCwd()==winBefore&&FloatingStateRestored();RestoreErrors();return errors&&states;};
 const uintptr_t raw=uintptr_t(&CStats::LoadStatUpdateConditions);std::vector<std::pair<uintptr_t,std::vector<uint8>>> initialCode;for(auto at:{uintptr_t(0x559860),raw,Resolve(raw),uintptr_t(0x8220AD),Resolve(0x8220AD)})initialCode.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
 OriginalDependencies499::HookRoute route("CStats","LoadStatUpdateConditions",0x559860,&CStats::LoadStatUpdateConditions);const auto offCode=ReversingProbe::Bytes(reinterpret_cast<void*>(0x559860),16);
 const std::array<uintptr_t,5> helperAddresses{0x5387D0,0x538900,0x536F80,0x5389D0,0x5389B0};std::vector<HelperRoute> helpers;for(auto address:helperAddresses)helpers.emplace_back(address);
 auto helpersOK=[&]{return std::ranges::all_of(helpers,[](const auto& h){const auto m=h.Metadata();return h.Check()&&m["registered"]==true&&m["enabled"]==true&&m["reconstructed"]==true;})&&!ReversingProbe::JumpsToDll(0x8220AD)&&ModuleOf(Resolve(0x8220AD))==uintptr_t(GetModuleHandleA(nullptr));};
 bool ready=inClone&&owned&&fixtureRoot.size()<128&&route.Registered()&&helpersOK()&&ModuleOf(route.Body())==ModuleOf(uintptr_t(&ModuleMarker)),passed=ready;
 json descriptors=json::array(),initial=json::array(),metadata=json::array();for(const auto& b:blocks){descriptors.push_back({{"address",b.address},{"bytes",b.saved.size()}});initial.push_back(b.Observe());}for(const auto& h:helpers)metadata.push_back(h.Metadata());
 Write({{"event","begin"},{"registered_hook",ready},{"functions",1},{"pairs",2048},{"collaborators_stubbed",false},{"helpers",metadata},{"crt_sscanf_entry",0x8220AD},{"open_mode_bytes",Hex(0x859C6C,3)},{"snapshot_layout",descriptors},{"initial_state",initial},{"fixture_root",fixtureRoot},{"initial_cwd_crt",crtBefore},{"initial_cwd_win",std::filesystem::path(winBefore).string()},{"initial_errno",errnoBefore},{"initial_winerror",errorBefore},{"initial_fp",Hex(uintptr_t(savedFP.data()),108)},{"initial_mxcsr",savedMXCSR},{"file_scope_ok",inClone&&owned},{"floating_state_restored",FloatingStateRestored()}});
 int pairs=0,failures=0;
 for(unsigned scenario=0;scenario<2048&&ready;++scenario){const auto input=Fixture(scenario);const std::string contents=input["file"];const bool fileWritten=WriteFile(file,contents);restore();
  auto invoke=[&](bool candidate){const bool beforeRestored=restore();memset(reinterpret_cast<void*>(0xB78200),unsigned(input["initial_pattern"]),2048);memset(reinterpret_cast<void*>(0xB71848),unsigned(input["line_pattern"]),512);memset(reinterpret_cast<void*>(0xB71A60),0xCC,128);memset(reinterpret_cast<void*>(0xB71AE0),0xDD,128);memcpy(reinterpret_cast<void*>(0xB71AE0),fixtureRoot.c_str(),fixtureRoot.size()+1);*reinterpret_cast<uint32*>(0xB794D0)=0x89ABCDEF;errno=int(input["errno_seed"]);SetLastError(0x514F);_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
   bool routeOK=false;const auto modeBefore=Hex(0x859C6C,3);int errnoAfter{};DWORD winErrorAfter{};alignas(16)std::array<uint8,108> postFP{};uint32 postMXCSR{};const auto result=route.Invoke(candidate,[&]{routeOK=helpersOK()&&ReversingProbe::JumpsToDll(0x559860)==candidate;if(candidate)routeOK&=Resolve(0x559860)==route.Body();errno=int(input["errno_seed"]);SetLastError(0x514F);const auto r=Gate(0x559860);errnoAfter=errno;winErrorAfter=GetLastError();SaveFP(postFP.data());postMXCSR=ReadMXCSR();return r;});
   json state=json::array();for(const auto& b:blocks)state.push_back(b.Observe());const auto cwdCRT=CRSCwd(),cwdWin=std::filesystem::path(WinCwd()).string();const bool handles=ExclusiveOpenClosed(file),unchanged=ReadFile(file)==contents;
   json out={{"input",input},{"state",state},{"open_mode_before",modeBefore},{"open_mode_after",Hex(0x859C6C,3)},{"mode_bytes_ok",modeBefore=="726200"&&Hex(0x859C6C,3)=="726200"},{"eax",result.eax},{"ecx",result.ecx},{"edx",result.edx},{"errno_after",errnoAfter},{"winerror_after",winErrorAfter},{"post_floating_state",Hex(uintptr_t(postFP.data()),108)},{"post_mxcsr",postMXCSR},{"cwd_crt",cwdCRT},{"cwd_win",cwdWin},{"stack_restored",result.stack},{"nonvolatile_preserved",result.nonvolatile},{"route_ok",routeOK},{"helpers_route_ok",helpersOK()},{"file_written",fileWritten},{"file_readonly",unchanged},{"filehandles_balanced",handles}};
   const bool restoredCall=restore();out["globals_restored"]=beforeRestored&&restoredCall&&restored();out["floating_state_restored"]=FloatingStateRestored();return out;
  };
  const auto a=invoke(false),b=invoke(true);bool ok=a==b;for(const char* k:{"stack_restored","nonvolatile_preserved","route_ok","helpers_route_ok","mode_bytes_ok","file_written","file_readonly","filehandles_balanced","globals_restored","floating_state_restored"})ok&=a[k]==true&&b[k]==true;passed&=ok;failures+=!ok;Write({{"event","comparison"},{"function","CStats::LoadStatUpdateConditions"},{"case",scenario},{"pair",pairs++},{"original",a},{"candidate",b},{"equal",ok},{"accepted",ok}});
 }
 Write({{"event","function_summary"},{"function","CStats::LoadStatUpdateConditions"},{"cases",pairs},{"failures",failures}});
 bool fileRemoved=!owned;if(owned){fileRemoved=(!std::filesystem::exists(file)||std::filesystem::remove(file))&&std::filesystem::remove(data)&&std::filesystem::remove(root);}
 bool stateRestored=restore()&&restored()&&helpersOK()&&offCode==ReversingProbe::Bytes(reinterpret_cast<void*>(0x559860),16);stateRestored&=route.Restore();for(const auto& [at,bytes]:initialCode)stateRestored&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
 const bool finalRestore=restore();const int finalErrno=errno;const DWORD finalWinError=GetLastError();const auto finalCRT=CRSCwd(),finalWin=std::filesystem::path(WinCwd()).string();json finalState=json::array();for(const auto& b:blocks)finalState.push_back(b.Observe());alignas(16)std::array<uint8,108> finalFP{};SaveFP(finalFP.data());const auto finalMXCSR=ReadMXCSR();restore();stateRestored&=finalRestore;
 Write({{"event","global_state_final"},{"state",finalState},{"cwd_crt",finalCRT},{"cwd_win",finalWin},{"errno",finalErrno},{"winerror",finalWinError},{"floating_state",Hex(uintptr_t(finalFP.data()),108)},{"mxcsr",finalMXCSR}});
 Write({{"event",passed&&stateRestored&&fileRemoved?"complete":"error"},{"pairs",pairs},{"open_mode_bytes",Hex(0x859C6C,3)},{"mode_bytes_ok",Hex(0x859C6C,3)=="726200"},{"state_restored",stateRestored},{"file_scope_ok",inClone&&owned},{"fixture_files_removed",fileRemoved},{"floating_state_restored",FloatingStateRestored()},{"helpers_route_ok",helpersOK()}});restore();savedFPActive=false;protectedErrorsActive=false;
}
}
