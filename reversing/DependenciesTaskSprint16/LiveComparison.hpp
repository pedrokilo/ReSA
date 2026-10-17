#pragma once
#include <fstream>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Entity/Ped/PlayerPed.h"
#include "PlayerPedData.h"
#include "PlayerInfo.h"
#include "Pad.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesTaskSprint16Live {
inline bool done{};inline std::ofstream log;
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
static_assert(sizeof(CPlayerPedData)==0xAC&&sizeof(CPad)==0x134&&sizeof(CPlayerInfo)==0x190);
inline constexpr Fn fns[]{{"CPlayerPed","ControlButtonSprint",0x60A610,0,-1,3848}};
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
inline std::function<bool()> pumpStateCheck;inline bool pumpStateOK=true;inline bool writeFPActive{};inline Fpu writeFP{};
struct PumpObserverGuard{~PumpObserverGuard(){pumpStateCheck={};writeFPActive=false;}};
inline void Write(const json& r){const auto enteringFP=CaptureFP();const auto& baseline=writeFPActive?writeFP:enteringFP;RestoreFP(baseline);
 if(pumpStateCheck)pumpStateOK&=pumpStateCheck();RestoreFP(baseline);ReversingProbe::PumpMessages();RestoreFP(baseline);if(pumpStateCheck)pumpStateOK&=pumpStateCheck();RestoreFP(baseline);
 auto record=r;if(record.value("event","")=="complete"&&!pumpStateOK){record["event"]="error";record["state_restored"]=false;}if(pumpStateCheck)record["pump_state_intact"]=pumpStateOK;log<<record.dump()<<'\n';log.flush();RestoreFP(baseline);
}
struct Result {std::array<uint8,10> extended;uint32 bits;bool stack,nv,seh,top;};
inline Result Call(uintptr_t entry,CPlayerPed* player,uint32 sprintType){Result result{};auto* extended=result.extended.data();uint32 bits{},sp0{},sp1{},b0{},b1{},s0{},s1{},d0{},d1{},p0{},p1{},h0{},h1{};uint16 t0{},t1{};
 __asm{
  fnstsw t0
  mov sp0,esp
  mov b0,ebx
  mov s0,esi
  mov d0,edi
  mov p0,ebp
  mov eax,fs:[0]
  mov h0,eax
  push sprintType
  mov ecx,player
  mov eax,0A5123456h
  mov edx,0C0DE5921h
  call entry
  mov sp1,esp
  mov b1,ebx
  mov s1,esi
  mov d1,edi
  mov p1,ebp
  mov eax,fs:[0]
  mov h1,eax
  mov eax,extended
  fstp tbyte ptr[eax]
  fld tbyte ptr[eax]
  fstp bits
  fnstsw t1
 }
 result.bits=bits;result.stack=sp0==sp1;result.nv=b0==b1&&s0==s1&&d0==d1&&p0==p1;result.seh=h0==h1;result.top=(t0&0x3800)==(t1&0x3800);return result;
}
inline void Put32(void* p,uint32 value){memcpy(p,&value,4);}
inline void Put16(void* p,uint16 value){memcpy(p,&value,2);}
inline uint32 Get32(const std::vector<uint8>& b,unsigned offset){uint32 value;memcpy(&value,b.data()+offset,4);return value;}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"dependenciestasksprint16-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependenciestasksprint16-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciestasksprint16");}
 auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning()||!player->GetPlayerData())return;done=true;
 auto* originalData=player->GetPlayerData();auto* pad0=CPad::GetPad(0);auto* pad1=CPad::GetPad(1);const uint8 focus=*reinterpret_cast<uint8*>(0xB7CD74);
 if(!pad0||!pad1||pad0==pad1||focus>1){Write({{"event","error"},{"reason","invalid real pads/focus"},{"pairs",0},{"state_restored",true}});return;}
 const auto fp=CaptureFP();writeFP=fp;writeFPActive=true;pumpStateOK=true;PumpObserverGuard pumpGuard;
 const auto playerSaved=ReversingProbe::Bytes(player,sizeof(CPlayerPed));const auto dataSaved=ReversingProbe::Bytes(originalData,sizeof(CPlayerPedData));const auto pad0Saved=ReversingProbe::Bytes(pad0,sizeof(CPad));const auto pad1Saved=ReversingProbe::Bytes(pad1,sizeof(CPad));const auto infoSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CD98),0x320);const auto tableSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D2460),128);const auto timeSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB5C),4);
 alignas(16) std::array<uint8,sizeof(CPlayerPedData)+32> arena{};arena.fill(0xA5);auto* shadow=arena.data()+16;const auto arenaInitial=arena;
 OriginalDependencies499::HookRoute route("CPlayerPed","ControlButtonSprint",0x60A610,FunctionToVoidPtr(&CPlayerPed::ControlButtonSprint));const auto mainOff=ReversingProbe::Bytes(reinterpret_cast<void*>(0x60A610),64);
 std::vector<Helper> helpers;for(auto at:{0x53FB70u,0x5407F0u,0x5407A0u,0x60A550u,0x420800u})helpers.emplace_back(at);
 auto helpersOK=[&]{return std::ranges::all_of(helpers,[](const auto& h){return h.Check();});};
 auto readonly=[&]{return dataSaved==ReversingProbe::Bytes(originalData,sizeof(CPlayerPedData))&&tableSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D2460),128)&&*reinterpret_cast<uint8*>(0xB7CD74)==focus;};
 auto restore=[&]{Put32(reinterpret_cast<uint8*>(player)+0x480,uint32(uintptr_t(originalData)));Put32(reinterpret_cast<uint8*>(player)+0x598,Get32(playerSaved,0x598));memcpy(pad0,pad0Saved.data(),pad0Saved.size());memcpy(pad1,pad1Saved.data(),pad1Saved.size());memcpy(reinterpret_cast<void*>(0xB7CD98),infoSaved.data(),infoSaved.size());memcpy(reinterpret_cast<void*>(0xB7CB5C),timeSaved.data(),4);arena=arenaInitial;RestoreFP(fp);};
 auto restored=[&]{return playerSaved==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&pad0Saved==ReversingProbe::Bytes(pad0,sizeof(CPad))&&pad1Saved==ReversingProbe::Bytes(pad1,sizeof(CPad))&&infoSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CD98),infoSaved.size())&&timeSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB5C),4)&&arena==arenaInitial&&readonly()&&CaptureFP()==fp;};
 auto routeProof=[&](bool candidate){return ReversingProbe::JumpsToDll(0x60A610)==candidate&&(candidate?(Resolve(0x60A610)==route.Body()&&ModuleOf(route.Body())==ModuleOf(uintptr_t(&ModuleMarker))):(Resolve(0x60A610)==0x60A610&&ModuleOf(0x60A610)==GetModuleHandleA(nullptr)&&mainOff==ReversingProbe::Bytes(reinterpret_cast<void*>(0x60A610),64)));};
 const bool routesReady=route.Invoke(true,[&]{return routeProof(true);})&&routeProof(false);const bool ready=route.Registered()&&routesReady&&helpersOK()&&restored();pumpStateCheck=[&]{return restored()&&helpersOK();};json hs=json::array();for(const auto& h:helpers)hs.push_back(h.Metadata());
 RestoreFP(fp);Write({{"event","begin"},{"registered_hook",route.Registered()},{"ready",ready},{"target_routes_ready",routesReady},{"helper_routes_ok",helpersOK()},{"helper_routes",hs},{"player_bytes",playerSaved.size()},{"player_data_bytes",dataSaved.size()},{"pad_bytes",pad0Saved.size()},{"player_addresses",json::array({uintptr_t(player),uintptr_t(originalData)})},{"pad_addresses",json::array({uintptr_t(pad0),uintptr_t(pad1)})},{"sprint_table_bytes",tableSaved},{"main_off_bytes",mainOff},{"main_body",route.Body()},{"candidate_module",uintptr_t(ModuleOf(route.Body()))},{"probe_module",uintptr_t(ModuleOf(uintptr_t(&ModuleMarker)))},{"scope","real player/pads; complete private data view; shared real helpers; finite legal types/indexes"}});
 if(!ready){restore();const bool state=restored();const bool hook=route.Restore();Write({{"event","error"},{"reason","fixture/routes preflight failed"},{"pairs",0},{"state_restored",state&&hook}});return;}
 // Matriz mixta: 3840 casos = 16 velocidades x 240 contextos (4 tipos x 2 pads x 6 entradas x 5 configuraciones de pad) + 8 con PlayerData NULL (scenario >= 3840).
 // Velocidades (+0x1C): -1, -0, +0, subnormal minimo, 0.001, 1, 4.9999, 5 (umbral exacto), 5.0001, 10, 1e30, FLT_MAX, +inf, -inf, NaN no senalizante, NaN negativo con carga.
 constexpr uint32 speeds[]{0xBF800000u,0x80000000u,0u,1u,0x3A83126Fu,0x3F800000u,0x409FF7CFu,0x40A00000u,0x40A00831u,0x41200000u,0x7149F2CAu,0x7F7FFFFFu,0x7F800000u,0xFF800000u,0x7FC00000u,0xFFC00001u};
 // Energia (+0x18): bajo/en/sobre el limite -150, 0, 1, 30 (normal), 30 con adrenalina (flag +0x34 bit 1), 30 con inmunidad al cansancio (PlayerInfo), NaN, -0.
 constexpr uint32 energies[]{0xC3170000u,0xC3160000u,0xC3150000u,0u,0x3F800000u,0x41F00000u,0x41F00000u,0x41F00000u,0x7FC00000u,0x80000000u};
 // Paso de tiempo (0xB7CB5C): 0.25, 2, NaN, subnormal minimo.
 constexpr uint32 times[]{0x3E800000u,0x40000000u,0x7FC00000u,1u};
 bool passed=true;unsigned failures{};
 for(unsigned scenario=0;scenario<3848;++scenario){const bool nullData=scenario>=3840;const unsigned speedIndex=nullData?0u:scenario%16,ctx=nullData?scenario-3840:scenario/16;
 const unsigned type=ctx%4,padIndex=ctx/4%2,inputClass=ctx/8%6,padCfg=ctx/48%5;const unsigned button=inputClass%3,sprintOff=inputClass/3;const unsigned energyIndex=(ctx/2+3*speedIndex)%10,timeMode=(ctx/3+speedIndex)%4;
 auto invoke=[&](bool candidate){restore();memcpy(shadow,dataSaved.data(),dataSaved.size());Put32(shadow+0x18,energies[energyIndex]);Put32(shadow+0x1C,speeds[speedIndex]);shadow[0x84]=uint8(sprintOff);shadow[0x34]=uint8((shadow[0x34]&~2u)|((energyIndex==6)?2u:0u));Put32(reinterpret_cast<uint8*>(player)+0x480,nullData?0u:uint32(uintptr_t(shadow)));Put32(reinterpret_cast<uint8*>(player)+0x598,padIndex);
 auto* pad=reinterpret_cast<uint8*>(padIndex?pad1:pad0);const unsigned padMode=padCfg<4?padCfg:0u;Put16(pad+0x10A,uint16(padMode));Put16(pad+0x10E,uint16(padCfg==4));Put16(pad+0x20,0);Put16(pad+0x22,0);Put16(pad+0x50,0);Put16(pad+0x52,0);const unsigned offset=padMode==2?0x22:0x20;Put16(pad+offset,button!=0?255:0);Put16(pad+offset+0x30,button==1?255:0);
 reinterpret_cast<uint8*>(0xB7CEE4)[0x190*focus]=uint8(energyIndex==7);Put32(reinterpret_cast<void*>(0xB7CB5C),times[timeMode]);
 const auto inputPlayer=ReversingProbe::Bytes(player,sizeof(CPlayerPed));const auto input=ReversingProbe::Bytes(shadow,sizeof(CPlayerPedData));const auto inputPads0=ReversingProbe::Bytes(pad0,sizeof(CPad));const auto inputPads1=ReversingProbe::Bytes(pad1,sizeof(CPad));const auto inputInfo=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CD98),0x320);const auto inputTime=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB5C),4);
 bool routes=false;RestoreFP(fp);const auto result=route.Invoke(candidate,[&]{routes=routeProof(candidate);RestoreFP(fp);const auto value=Call(0x60A610,player,type);routes&=routeProof(candidate);return value;});const auto actual=ReversingProbe::Bytes(shadow,sizeof(CPlayerPedData));bool fields=true;for(unsigned i=0;i<actual.size();++i)if(i<0x18||i>=0x20)fields&=actual[i]==input[i];if(nullData)fields&=actual==input&&result.bits==0;const bool actor=ReversingProbe::Bytes(player,sizeof(CPlayerPed))==inputPlayer;const bool read=readonly()&&ReversingProbe::Bytes(pad0,sizeof(CPad))==inputPads0&&ReversingProbe::Bytes(pad1,sizeof(CPad))==inputPads1&&ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CD98),0x320)==inputInfo&&ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB5C),4)==inputTime;const bool guards=std::equal(arena.begin(),arena.begin()+16,arenaInitial.begin())&&std::equal(arena.end()-16,arena.end(),arenaInitial.end()-16);const bool helper=helpersOK();restore();const bool memory=restored();RestoreFP(fp);
 return json{{"controls_disabled",padCfg==4},{"type",type},{"pad_index",padIndex},{"speed_index",speedIndex},{"input_class",inputClass},{"pad_cfg",padCfg},{"energy_index",energyIndex},{"time_mode",timeMode},{"pad_mode",padMode},{"null_data",nullData},{"input_data",input},{"player_data",actual},{"return_extended",result.extended},{"return_bits",result.bits},{"route_ok",routes},{"helper_route_ok",helper},{"stack_restored",result.stack},{"nonvolatile_preserved",result.nv},{"seh_restored",result.seh},{"fpu_top_balanced",result.top},{"fixture_contract",fields&&actor},{"readonly_ok",read},{"guards_preserved",guards},{"state_restored",memory},{"floating_state_restored",CaptureFP()==fp}};
 };const auto original=invoke(false);const auto candidate=invoke(true);bool equal=original==candidate;for(auto k:{"route_ok","helper_route_ok","stack_restored","nonvolatile_preserved","seh_restored","fpu_top_balanced","fixture_contract","readonly_ok","guards_preserved","state_restored","floating_state_restored"})equal&=original[k]==true&&candidate[k]==true;passed&=equal;failures+=!equal;Write({{"event","comparison"},{"function","CPlayerPed::ControlButtonSprint"},{"pair",scenario},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
 }
 Write({{"event","function_summary"},{"function","CPlayerPed::ControlButtonSprint"},{"cases",3848},{"failures",failures}});restore();const bool memory=restored()&&helpersOK();const bool hook=route.Restore();RestoreFP(fp);Write({{"event",passed&&memory&&hook?"complete":"error"},{"pairs",3848},{"state_restored",memory&&hook},{"helper_route_ok",helpersOK()},{"floating_state_restored",CaptureFP()==fp}});RestoreFP(fp);
}
}
