#pragma once
#include <fstream>
#include "DecisionMakers/DecisionMakerTypes.h"
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "NativeProof.hpp"
// Genuine C++ global function proposed in TaskComplexSunbathe.cpp, no fabricated class.
int32 CanSunbathe();
namespace CppSunbatheProposalLive {
inline bool done{};inline std::ofstream log;
struct Fn {const char* cls;const char* name;uintptr_t address;int reserved,slot,count;};
inline constexpr Fn fns[]{
 {"CDecisionMakerTypes","GetInstance",0x4684F0,0,-1,2048},
 {"CTaskComplexSunbathe","CanSunbathe",0x632140,0,-1,2048}
};
inline uintptr_t Resolve(uintptr_t p){for(int i=0;i<16&&*reinterpret_cast<uint8*>(p)==0xE9;++i)p+=5+*reinterpret_cast<int32*>(p+1);return p;}
inline HMODULE ModuleOf(uintptr_t p){HMODULE m{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(p),&m);return m;}
inline void ModuleMarker(){}
struct Helper {
 uintptr_t entry,body;HMODULE module;std::vector<uint8> entryBytes,bodyBytes;
 ReversibleHooks::ReversibleHook::Base* hook{};bool enabled{},locked{};
 explicit Helper(uintptr_t at):entry(at),body(Resolve(at)),module(ModuleOf(body)),entryBytes(ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)),bodyBytes(ReversingProbe::Bytes(reinterpret_cast<void*>(body),64)){
  ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){for(auto& h:c.Items())if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple&&reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress())==at){hook=h.get();enabled=hook->Hooked();locked=hook->Locked();}});
 }
 bool Check()const{return body==Resolve(entry)&&ModuleOf(body)==module&&(module==GetModuleHandleA(nullptr)||module==ModuleOf(uintptr_t(&ModuleMarker)))
  &&entryBytes==ReversingProbe::Bytes(reinterpret_cast<void*>(entry),16)&&bodyBytes==ReversingProbe::Bytes(reinterpret_cast<void*>(body),64)
  &&(!hook||(hook->Hooked()==enabled&&hook->Locked()==locked));}
 json Metadata()const{return {{"entry",entry},{"body",body},{"module",uintptr_t(module)},{"entry_bytes",entryBytes},{"body_bytes",bodyBytes},{"registered",hook!=nullptr},{"enabled",enabled},{"locked",locked},{"route_ok",Check()}};}
};
struct Fpu {std::array<uint8,108> state;uint32 mxcsr;bool operator==(const Fpu&)const=default;};
inline Fpu CaptureFP(){Fpu f{};auto* p=f.state.data();auto* m=&f.mxcsr;__asm {mov eax,p} __asm {fnsave [eax]} __asm {frstor [eax]} __asm {mov eax,m} __asm {stmxcsr [eax]}return f;}
inline void RestoreFP(const Fpu& f){const auto* p=f.state.data();const auto m=f.mxcsr;__asm {mov eax,p} __asm {frstor [eax]} __asm {ldmxcsr m}}
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
struct ABI {uint32 eax,ecx,edx;bool stack,nv,seh;};
inline ABI Gate(uintptr_t entry,uint32 seed){uint32 a{},c{},d{},sp0{},sp1{},b0{},b1{},s0{},s1{},d0{},d1{},p0{},p1{},h0{},h1{};
 __asm {
  mov sp0,esp
  mov b0,ebx
  mov s0,esi
  mov d0,edi
  mov p0,ebp
  mov eax,fs:[0]
  mov h0,eax
  mov eax,seed
  mov ecx,137AFF01h
  mov edx,0C0DE5921h
  call entry
  mov a,eax
  mov c,ecx
  mov d,edx
  mov sp1,esp
  mov b1,ebx
  mov s1,esi
  mov d1,edi
  mov p1,ebp
  mov eax,fs:[0]
  mov h1,eax
 }
 return {a,c,d,sp0==sp1,b0==b1&&s0==s1&&d0==d1&&p0==p1,h0==h1};}

inline void NativeFree(void* block){reinterpret_cast<void(__cdecl*)(void*)>(0x8214BD)(block);}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"cppsunbatheproposal-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cppsunbatheproposal-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cppsunbatheproposal");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 static_assert(sizeof(CDecision)==0x3C&&sizeof(CDecisionMaker)==0x99C&&sizeof(CDecisionMakerTypes)==0xF1C0);
 static_assert(offsetof(CDecisionMakerTypes,m_DecisionMakers)==4&&offsetof(CDecisionMakerTypes,m_EventIndices)==0xC034&&offsetof(CDecisionMakerTypes,m_DefaultRandomPedDecisionMaker)==0xC1B4);
 const auto fp=CaptureFP();auto*& singleton=*reinterpret_cast<CDecisionMakerTypes**>(0xC0B030);auto* oldSingleton=singleton;
 if(!oldSingleton){Write({{"event","error"},{"reason","Existing real singleton required"},{"pairs",0},{"state_restored",true}});RestoreFP(fp);return;}
 const auto oldObject=ReversingProbe::Bytes(oldSingleton,0xF1C0);const auto oldGlobals=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC0AFE0),80);
 const auto oldHour=*reinterpret_cast<uint8*>(0xB70153);const auto oldWeather=*reinterpret_cast<int16*>(0xC8131C);
 std::array<std::unique_ptr<OriginalDependencies499::HookRoute>,2> routes;
 routes[0]=std::make_unique<OriginalDependencies499::HookRoute>("CDecisionMakerTypes","GetInstance",0x4684F0,&CDecisionMakerTypes::GetInstance);
 routes[1]=std::make_unique<OriginalDependencies499::HookRoute>("CTaskComplexSunbathe","CanSunbathe",0x632140,&CanSunbathe);
 std::vector<Helper> helpers;for(auto at:{0x4650F0u,0x4650A0u,0x6040C0u,0x600530u,0x821D30u,0x82119Au,0x8214BDu,0x824257u,0x82413Fu,0x62F2D0u})helpers.emplace_back(at);
 auto helpersOK=[&]{for(const auto& h:helpers)if(!h.Check())return false;return true;};
 auto nativeOK=[&]{return std::equal(canonical_4650F0.begin(),canonical_4650F0.end(),reinterpret_cast<const uint8*>(0x4650F0))&&std::equal(canonical_62F2D0.begin(),canonical_62F2D0.end(),reinterpret_cast<const uint8*>(0x62F2D0));};
 auto originalIntact=[&]{return oldObject==ReversingProbe::Bytes(oldSingleton,0xF1C0);};
 auto restore=[&]{singleton=oldSingleton;std::memcpy(reinterpret_cast<void*>(0xC0AFE0),oldGlobals.data(),80);*reinterpret_cast<uint8*>(0xB70153)=oldHour;*reinterpret_cast<int16*>(0xC8131C)=oldWeather;RestoreFP(fp);};
 auto restored=[&]{return singleton==oldSingleton&&originalIntact()&&oldGlobals==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC0AFE0),80)&&*reinterpret_cast<uint8*>(0xB70153)==oldHour&&*reinterpret_cast<int16*>(0xC8131C)==oldWeather;};
 bool registered=routes[0]->Registered()&&routes[1]->Registered();json helperMetadata=json::array();for(auto& h:helpers)helperMetadata.push_back(h.Metadata());
 RestoreFP(fp);Write({{"event","begin"},{"functions",2},{"expected_pairs",4096},{"registered_hook",registered},{"helper_routes",helperMetadata},{"helper_routes_ok",helpersOK()},{"native_complete_bodies_ok",nativeOK()},
 {"constructor_bytes",canonical_4650F0},{"weather_helper_bytes",canonical_62F2D0},{"singleton_bytes",0xF1C0},{"defined_constructor_bytes",61504},{"undefined_event_index_bytes",384},{"original_singleton_intact",originalIntact()}});RestoreFP(fp);
 bool passed=registered&&helpersOK()&&nativeOK();int pair=0;
 constexpr uint8 hours[]{0,1,8,9,10,11,12,13,14,15,16,17,18,19,23,255};
 constexpr int16 weathers[]{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,-1,-2,-32768,32767,128,256,257,513,-255};
 constexpr uint32 seeds[]{0xA5123456,0x00003456,0xFFFF3456,0x137A3456};
 for(int fn=0;fn<2;++fn){int failures=0;for(int scenario=0;scenario<2048;++scenario){
  auto invoke=[&](bool candidate){restore();bool helperOK=helpersOK()&&nativeOK(),routeOK=false,oldOK=originalIntact();
   const bool nullBranch=(scenario&1)!=0;const unsigned mode=(scenario/2)%4;
   auto inputGlobals=oldGlobals;
   // Byte fixtures remain confined to constructor-owned flags/indices, with no callbacks or message pumping while installed.
   if(fn==0){for(unsigned i=0;i<80;++i)inputGlobals[i]=uint8((i+scenario*13)&255);std::memcpy(reinterpret_cast<void*>(0xC0AFE0),inputGlobals.data(),80);singleton=nullBranch?nullptr:oldSingleton;}
   const unsigned hi=scenario%16,wi=(scenario/16)%32,si=scenario/512;
   if(fn==1){*reinterpret_cast<uint8*>(0xB70153)=hours[hi];*reinterpret_cast<int16*>(0xC8131C)=weathers[wi];}
   const auto beforeGlobals=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC0AFE0),80);
   const auto abi=routes[fn]->Invoke(candidate,[&]{routeOK=ReversingProbe::JumpsToDll(fns[fn].address)==candidate;return Gate(fns[fn].address,seeds[fn==0?mode:si]);});
   helperOK&=helpersOK()&&nativeOK();oldOK&=originalIntact();
   json observation;bool contract=true,resources=true;unsigned allocations=0,freed=0;
   if(fn==0){auto* returned=reinterpret_cast<CDecisionMakerTypes*>(abi.eax);contract=returned!=nullptr&&returned==singleton;
    const auto actualGlobals=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC0AFE0),80);
    json matrices=json::array();bool allDefault=true;int noDM=0;
    if(nullBranch){++allocations;contract&=returned!=oldSingleton;
     if(returned&&returned!=oldSingleton){noDM=*reinterpret_cast<int32*>(returned);const auto* object=reinterpret_cast<const uint8*>(returned);
      constexpr unsigned bases[]{4,0xC1B4,0xCB50,0xD4EC,0xDE88,0xE824};constexpr unsigned counts[]{20,1,1,1,1,1};
      for(unsigned a=0;a<6;++a)for(unsigned m=0;m<counts[a];++m){bool matrix=true;for(unsigned d=0;d<41;++d)for(unsigned byte=0;byte<60;++byte)matrix&=object[bases[a]+m*0x99C+d*60+byte]==(byte<24?255:0);matrices.push_back(matrix);allDefault&=matrix;}
      std::vector<uint8> expectedGlobals(80,0);for(unsigned i=20;i<60;i+=2)expectedGlobals[i]=1;
      contract&=noDM==0&&allDefault&&actualGlobals==expectedGlobals;
      NativeFree(returned);++freed;
     }else contract=false;
    }else contract&=returned==oldSingleton&&actualGlobals==inputGlobals;
    resources=allocations==freed;
    observation={{"null_branch",nullBranch},{"seed_mode",mode},{"return_nonnull",returned!=nullptr},{"return_is_global",returned==singleton},{"return_is_original",returned==oldSingleton},{"no_decision_makers",noDM},{"matrices_default",matrices},
     {"defined_bytes_checked",nullBranch?61504:0},{"global_bytes",actualGlobals},{"allocations",allocations},{"frees",freed}};
   }else{const auto w=weathers[wi];bool sunny=false;for(auto good:{0,1,2,3,5,6,10,11,13,14,17,18})sunny|=w==good;const bool expected=hours[hi]>=10&&hours[hi]<18&&sunny;
    contract=abi.eax==uint32(expected)&&abi.ecx==0x137AFF01&&abi.edx==0xC0DE5921&&singleton==oldSingleton&&beforeGlobals==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC0AFE0),80)&&*reinterpret_cast<uint8*>(0xB70153)==hours[hi]&&*reinterpret_cast<int16*>(0xC8131C)==w;
    observation={{"hour",hours[hi]},{"weather",w},{"seed_index",si},{"return",abi.eax},{"ecx",abi.ecx},{"edx",abi.edx}};
   }
   helperOK&=helpersOK()&&nativeOK();oldOK&=originalIntact();restore();
   observation["stack_restored"]=abi.stack;observation["nonvolatile_preserved"]=abi.nv;observation["seh_restored"]=abi.seh;observation["route_ok"]=routeOK;observation["helper_route_ok"]=helperOK;
   observation["fixture_contract"]=contract;observation["resources_balanced"]=resources;observation["original_singleton_intact"]=oldOK&&originalIntact();observation["state_restored"]=restored();observation["floating_state_restored"]=CaptureFP()==fp;
   return observation;
  };
  const auto original=invoke(false);const auto candidate=invoke(true);bool ok=original==candidate;
  for(auto key:{"stack_restored","nonvolatile_preserved","seh_restored","route_ok","helper_route_ok","fixture_contract","resources_balanced","original_singleton_intact","state_restored","floating_state_restored"})ok&=original[key]==true&&candidate[key]==true;
  passed&=ok;failures+=!ok;Write({{"event","comparison"},{"function",std::string(fns[fn].cls)+"::"+fns[fn].name},{"pair",pair++},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",ok},{"accepted",ok}});RestoreFP(fp);
 }
 Write({{"event","function_summary"},{"function",std::string(fns[fn].cls)+"::"+fns[fn].name},{"cases",2048},{"failures",failures}});RestoreFP(fp);}
 restore();bool final=restored()&&helpersOK()&&nativeOK();for(auto& route:routes){const bool one=route->Restore();final&=one;}RestoreFP(fp);
 Write({{"event",passed&&final?"complete":"error"},{"pairs",pair},{"state_restored",final},{"original_singleton_intact",originalIntact()},{"helper_route_ok",helpersOK()},{"native_complete_bodies_ok",nativeOK()},{"floating_state_restored",CaptureFP()==fp}});RestoreFP(fp);
}
}
