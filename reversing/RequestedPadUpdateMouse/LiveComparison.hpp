#pragma once
#include "Pad.h"
#include "MouseControllerState.h"
#include <xmmintrin.h>
#include <float.h>
#include "../ProbeRuntime.hpp"
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
namespace RequestedPadUpdateMouse {
inline constexpr unsigned fpMask=_MCW_PC|_MCW_RC|_MCW_EM|_MCW_IC|_MCW_DN;
using Simple=ReversibleHooks::ReversibleHook::Simple;
static_assert(sizeof(CPad)==308 && sizeof(CMouseControllerState)==20 && sizeof(DIMOUSESTATE2)==20);
static_assert(offsetof(CPad,LastTimeTouched)==0x120 && std::is_trivially_copyable_v<CPad>);
alignas(16) inline std::array<std::array<uint8,340>,2> pads;
inline std::array<uint8,48> opaque;
inline json trace;
inline unsigned route{},profile{},flags{},receiver{};
inline bool valid=true;
inline CPad* Pad(unsigned index){return reinterpret_cast<CPad*>(pads[index].data()+16);}
inline uintptr_t Mouse(){return uintptr_t(opaque.data()+16);}
inline uintptr_t& MouseGlobal(){return *reinterpret_cast<uintptr_t*>(0xC8CFA4);}
inline bool __cdecl Foreground(){const bool result=route!=0;trace.push_back({"foreground",result});return result;}
inline bool __cdecl Exclusive(){const bool result=profile%2;trace.push_back({"exclusive",result});return result;}
inline void __cdecl Init(bool exclusive){trace.push_back({"init",exclusive});MouseGlobal()=route==1?0:Mouse();}
inline const std::array<DIMOUSESTATE2,12> states{{
 {0,0,0,{0,0,0,0,0,0,0,0}},{1,-1,0,{0,0,0,0,0,0,0,0}},{-1,1,0,{0,0,0,0,0,0,0,0}},
 {0,0,1,{0,0,0,0,0,0,0,0}},{0,0,-1,{0,0,0,0,0,0,0,0}},
 {0,0,0,{128,0,0,0,0,1,127,255}},{0,0,0,{0,255,0,0,0,128,0,0}},{0,0,0,{0,0,129,0,0,0,0,0}},{0,0,0,{0,0,0,128,255,0,0,0}},
 {INT_MAX,-INT_MAX,0,{1,127,128,129,255,0,0,0}}, {16777217,-16777217,INT_MAX,{255,255,255,255,255,255,255,255}}, {-16777219,16777219,INT_MIN,{127,127,127,127,127,127,127,127}}
}};
inline HRESULT Read(DIMOUSESTATE2* state){const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb());const uintptr_t low=uintptr_t(tib->StackLimit),high=uintptr_t(tib->StackBase);const uintptr_t pointer=uintptr_t(state);const bool inStack=pointer>=low&&pointer<=high-20;valid&=state&&inStack&&MouseGlobal()==Mouse();if(state&&inStack)std::memcpy(state,&states[profile],20);const HRESULT status=route==2?HRESULT(0x80004005L):route==5?HRESULT(0x80070005L):route==4?1:0;trace.push_back({"read",int32(status),ReversingProbe::Bytes(&states[profile],20),valid});return status;}
inline HRESULT __cdecl ReadExe(DIMOUSESTATE2* state){return Read(state);}
inline HRESULT __fastcall ReadDll(CPad* self,void*,DIMOUSESTATE2* state){valid&=self==Pad(receiver);return Read(state);}
inline Simple* Find(const char* owner,const char* name){Simple* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){if(cat.Name()==owner)for(auto& h:cat.Items())if(h->Name()==name&&h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple)result=static_cast<Simple*>(h.get());});return result;}
inline uintptr_t Body(uintptr_t address){for(int j=0;j<16&&*reinterpret_cast<uint8*>(address)==0xE9;++j)address+=5+*reinterpret_cast<int32*>(address+1);return address;}
struct Gate{std::vector<uintptr_t> entries;std::vector<std::pair<uintptr_t,std::vector<uint8>>> before;std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;bool good=true;
 template<class A,class B>Gate(uintptr_t native,Simple* hook,A original,B candidate){good=hook&&uintptr_t(hook->GetHookGTAAddress())==native;if(!good)return;const auto own=uintptr_t(hook->GetHookOurAddress());entries={native,own};const auto body=Body(own);if(std::find(entries.begin(),entries.end(),body)==entries.end())entries.push_back(body);for(auto p:entries)before.push_back({p,ReversingProbe::Bytes(reinterpret_cast<void*>(p),52)});for(auto p:entries){if(p==native)patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(p,original));else patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(p,candidate));}}
 bool Active()const{if(!good)return false;for(size_t j=0;j<entries.size();++j)if(ReversingProbe::Bytes(reinterpret_cast<void*>(entries[j]),5)!=std::vector<uint8>(patches[j]->jump.begin(),patches[j]->jump.end()))return false;return true;}
 bool Restore(){patches.clear();for(auto& [p,bytes]:before)good&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(p),52);return good;}
};
inline void Update(){
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);std::ofstream log(std::filesystem::path(exe).parent_path()/"pad-update-mouse-runtime.jsonl");auto write=[&](const json& row){log<<row.dump()<<'\n';log.flush();};
 auto* target=Find("CPad","UpdateMouse");auto* check=Find("CMouseControllerState","CheckForInput");if(!target||!check||uintptr_t(target->GetHookGTAAddress())!=0x53F3C0||uintptr_t(check->GetHookGTAAddress())!=0x53F270){write({{"event","error"},{"reason","missing target/pure dependency"}});return;}
 const auto savedPads=pads;const auto savedOpaque=opaque;const auto savedTrace=trace;const auto savedRoute=route,savedProfile=profile,savedFlags=flags,savedReceiver=receiver;const bool savedValid=valid;const unsigned savedFP=_controlfp(0,0),savedSSE=_mm_getcsr();
 const std::array<std::pair<uintptr_t,size_t>,6> globals{{{0xB733F4,100},{0xC8CFA4,4},{0xBA67A4,1},{0xBA6744,2},{0xB7CB7C,12},{0xB73458,616}}};std::vector<std::vector<uint8>> saved;for(auto [p,n]:globals)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(p),n));
 const std::array<bool,4> hookFlags{target->Hooked(),target->Locked(),check->Hooked(),check->Locked()};target->LockState(false);target->State(false);check->LockState(false);check->State(false);
 auto* foreground=Find("WinPlatform","IsForegroundApp");auto* exclusive=Find("VideoMode","IsVideoModeExclusive");auto* init=Find("WinInput","diMouseInit");auto* read=Find("CPad","GetMouseState");
 // Owner names are resolved uniquely by function identity if platform categories differ.
 auto byNative=[&](uintptr_t native){Simple* found{};unsigned hits=0;ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){for(auto& h:c.Items())if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple){auto* p=static_cast<Simple*>(h.get());if(uintptr_t(p->GetHookGTAAddress())==native){found=p;++hits;}}});return hits==1?found:nullptr;};
 foreground=byNative(0x746060);exclusive=byNative(0x745CA0);init=byNative(0x7469A0);read=byNative(0x746ED0);
 std::vector<std::unique_ptr<Gate>> gates;gates.push_back(std::make_unique<Gate>(0x746060,foreground,Foreground,Foreground));gates.push_back(std::make_unique<Gate>(0x745CA0,exclusive,Exclusive,Exclusive));gates.push_back(std::make_unique<Gate>(0x7469A0,init,Init,Init));gates.push_back(std::make_unique<Gate>(0x746ED0,read,ReadExe,ReadDll));
 const auto checkOwn=uintptr_t(check->GetHookOurAddress()),checkBody=Body(checkOwn);std::vector<uintptr_t> checkEntries{checkOwn};if(checkBody!=checkOwn)checkEntries.push_back(checkBody);std::vector<std::pair<uintptr_t,std::vector<uint8>>> checkBefore;std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> checkPatches;const auto checkNativeBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0x53F270),128);
 const bool pureCandidateKnown=checkBody>=0x1000000&&checkBody!=0x53F270;
 if(pureCandidateKnown)for(auto p:checkEntries){checkBefore.push_back({p,ReversingProbe::Bytes(reinterpret_cast<void*>(p),52)});checkPatches.push_back(std::make_unique<ReversingProbe::EntryPatch>(p,reinterpret_cast<void*>(0x53F270)));}
 bool known=(savedFP&_MCW_RC)==_RC_NEAR&&(savedSSE&0x6000)==0&&pureCandidateKnown&&std::ranges::all_of(gates,[](const auto& g){return g->good;});write({{"event","begin"},{"fp_control_before",savedFP},{"mxcsr_before",savedSSE},{"helpers_known",known},{"mouse_address",Mouse()},{"mouse_guard_before",std::vector<uint8>(saved[0].begin(),saved[0].begin()+16)},{"mouse_guard_after",std::vector<uint8>(saved[0].begin()+76,saved[0].end())},{"pad_addresses",json::array({uintptr_t(Pad(0)),uintptr_t(Pad(1))})},{"check_native",0x53F270},{"check_candidate",checkOwn},{"check_body",checkBody},{"getmouse_native_ABI","cdecl(buffer)"},{"getmouse_candidate_ABI","thiscall(self,buffer)"}});
 size_t pairs=0,differences=0;bool all=known;
 if(known)for(unsigned i=0;i<9216;++i){profile=i%12;route=(i/12)%6;flags=(i/72)%8;const uint8 poison=uint8(((i/576)%4)*85);receiver=(i/2304)%2;const uint32 clock=(i/4608)%2?UINT_MAX:0;json fixture{{"index",i},{"profile",profile},{"route",route},{"flags",flags},{"poison",poison},{"receiver",receiver},{"clock",clock}};json legs[2];
  for(int leg=0;leg<2;++leg){for(auto& p:pads)p.fill(0xCD);opaque.fill(0xA7);std::array<uint8,308> objectSeed;objectSeed.fill(poison);for(unsigned p=0;p<2;++p)std::memcpy(Pad(p),objectSeed.data(),objectSeed.size());
   uint8 seeds[60];for(unsigned j=0;j<60;++j)seeds[j]=uint8(poison+j*17);for(unsigned s=0;s<3;++s){seeds[s*20+3]=(poison/85)%2;seeds[s*20+4]=(poison/85+1)%2;const float initial=poison?float(poison):0.f;for(unsigned off:{8u,12u,16u})std::memcpy(seeds+s*20+off,&initial,4);}std::memcpy(reinterpret_cast<void*>(0xB73404),seeds,60);
   MouseGlobal()=(route==2||route==3||(route==0&&profile%2))?Mouse():0;*reinterpret_cast<uint8*>(0xBA67A4)=flags&1;*reinterpret_cast<uint8*>(0xBA6744)=(flags>>1)&1;*reinterpret_cast<uint8*>(0xBA6745)=(flags>>2)&1;std::memcpy(reinterpret_cast<void*>(0xB7CB84),&clock,4);
   const auto mouseGuardBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB733F4),16),mouseGuardAfter=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB73440),24);const auto timerLegBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB7C),12);const auto globalPadsBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB73458),616);trace=json::array();valid=true;target->State(leg!=0);_controlfp(savedFP,fpMask);_mm_setcsr(savedSSE);
   reinterpret_cast<void(__thiscall*)(CPad*)>(0x53F3C0)(Pad(receiver));
   bool active=std::ranges::all_of(gates,[](const auto& g){return g->Active();});for(size_t j=0;j<checkEntries.size();++j)active&=ReversingProbe::Bytes(reinterpret_cast<void*>(checkEntries[j]),5)==std::vector<uint8>(checkPatches[j]->jump.begin(),checkPatches[j]->jump.end());
   const bool controlsUnchanged=*reinterpret_cast<uint8*>(0xBA67A4)==(flags&1)&&*reinterpret_cast<uint8*>(0xBA6744)==((flags>>1)&1)&&*reinterpret_cast<uint8*>(0xBA6745)==((flags>>2)&1);const bool checkUnchanged=checkNativeBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0x53F270),128);const bool targetGate=target->Hooked()==bool(leg)&&ReversingProbe::JumpsToDll(0x53F3C0)==bool(leg);const bool fp=_controlfp(0,0)==savedFP&&_mm_getcsr()==savedSSE;
   legs[leg]={{"payload",{{"mouse",ReversingProbe::Bytes(reinterpret_cast<void*>(0xB73404),60)},{"mouse_guard_before",ReversingProbe::Bytes(reinterpret_cast<void*>(0xB733F4),16)},{"mouse_guard_after",ReversingProbe::Bytes(reinterpret_cast<void*>(0xB73440),24)},{"pads",pads},{"opaque",opaque},{"device",MouseGlobal()},{"trace",trace}}},{"helper_gates",active},{"target_gate",targetGate},{"abi_valid",valid},{"controls_immutable",controlsUnchanged},{"mouse_guards_immutable",mouseGuardBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB733F4),16)&&mouseGuardAfter==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB73440),24)},{"native_check_immutable",checkUnchanged},{"timer_immutable",timerLegBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB7C),12)},{"fp_control_raw",_controlfp(0,0)},{"mxcsr_raw",_mm_getcsr()},{"global_pads_immutable",globalPadsBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB73458),616)},{"fp_exact",fp}};
  }
  bool equal=legs[0]["payload"]==legs[1]["payload"];for(const auto& leg:legs)for(const char* k:{"helper_gates","target_gate","abi_valid","controls_immutable","native_check_immutable","global_pads_immutable","timer_immutable","mouse_guards_immutable","fp_exact"})equal&=leg[k]==true;all&=equal;differences+=!equal;write({{"event","comparison"},{"function","CPad::UpdateMouse"},{"fixture",fixture},{"pair",pairs++},{"equal",equal},{"original",legs[0]},{"candidate",legs[1]}});
 }
 target->State(hookFlags[0]);target->LockState(hookFlags[1]);check->State(hookFlags[2]);check->LockState(hookFlags[3]);checkPatches.clear();bool restored=true;for(auto& [p,b]:checkBefore)restored&=b==ReversingProbe::Bytes(reinterpret_cast<void*>(p),52);for(auto& g:gates)restored&=g->Restore();
 for(size_t j=0;j<globals.size();++j)std::memcpy(reinterpret_cast<void*>(globals[j].first),saved[j].data(),saved[j].size());pads=savedPads;opaque=savedOpaque;trace=savedTrace;route=savedRoute;profile=savedProfile;flags=savedFlags;receiver=savedReceiver;valid=savedValid;_controlfp(savedFP,fpMask);_mm_setcsr(savedSSE);
 for(size_t j=0;j<globals.size();++j)restored&=saved[j]==ReversingProbe::Bytes(reinterpret_cast<void*>(globals[j].first),globals[j].second);restored&=pads==savedPads&&opaque==savedOpaque&&checkNativeBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0x53F270),128);
 write({{"event","complete"},{"comparisons",pairs},{"differences",differences},{"all_equal",all},{"state_restored",restored},{"fp_restored",_controlfp(0,0)==savedFP&&_mm_getcsr()==savedSSE}});
}
}
