#pragma once
#include <fstream>
#include "TheZones.h"
#include "Core/MatrixLink.h"
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "NativeProof.hpp"
namespace DependenciesZones13Live {
inline bool done{};inline std::ofstream log;
struct Fn {const char* cls;const char* name;uintptr_t address;int reserved,slot,count;};
inline constexpr Fn fns[]{{"CTheZones","SetCurrentZoneAsUnlocked",0x572800,0,-1,2048}};
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


inline constexpr float coordinates[]{-2700,-2100,-1500,-900,-300,300,900,1500,2100,2700,-3000,-2400,-1800,-1200,0,2999};
inline constexpr unsigned cells[]{0,1,2,3,4,5,6,7,8,9,0,1,2,3,5,9};
inline uint32 ExpectedLevel(const CVector& v,unsigned count){auto* zones=CTheZones::MapZoneArray.data();for(unsigned i=1;i<count;++i){const auto& z=zones[i];if(v.x>=z.m_fX1&&v.x<=z.m_fX2&&v.y>=z.m_fY1&&v.y<=z.m_fY2&&v.z>=z.m_fZ1&&v.z<=z.m_fZ2)return uint8(z.m_nLevel);}return uint8(zones[0].m_nLevel);}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependencieszones13-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependencieszones13-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependencieszones13");}
 auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;const auto fp=CaptureFP();
 const auto selected=*reinterpret_cast<uint8*>(0xB7CD74);const auto mapCount=uint16(CTheZones::TotalNumberOfMapZones);
 if(selected>=2||*reinterpret_cast<CPlayerPed**>(0xB7CD98+selected*0x190)!=player||(reinterpret_cast<const uint8*>(player)[0x46D]&1)||!mapCount||mapCount>39){RestoreFP(fp);Write({{"event","error"},{"reason","Real selected player on foot and map catalog required"},{"pairs",0},{"state_restored",true}});RestoreFP(fp);return;}
 auto* matrix=*reinterpret_cast<CMatrixLink**>(reinterpret_cast<uint8*>(player)+0x14);auto* position=matrix?reinterpret_cast<CVector*>(reinterpret_cast<uint8*>(matrix)+0x30):reinterpret_cast<CVector*>(reinterpret_cast<uint8*>(player)+4);
 const auto savedPosition=ReversingProbe::Bytes(position,12);const auto savedPlayer=ReversingProbe::Bytes(player,sizeof(CPlayerPed));const auto savedMatrix=matrix?ReversingProbe::Bytes(matrix,sizeof(CMatrixLink)):std::vector<uint8>{};
 const auto savedGrid=ReversingProbe::Bytes(reinterpret_cast<void*>(0xBA3730),100);const auto savedCounter=*reinterpret_cast<uint32*>(0xBA372C);const auto savedLevel=*reinterpret_cast<uint32*>(0xBA6718);const auto savedOffMap=*reinterpret_cast<uint8*>(0xA444A4);const auto savedArea=*reinterpret_cast<uint32*>(0xB72914);
 const auto savedMap=ReversingProbe::Bytes(CTheZones::MapZoneArray.data(),sizeof(CTheZones::MapZoneArray));
 const auto gridBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xBA371C),16);const auto gridAfter=ReversingProbe::Bytes(reinterpret_cast<void*>(0xBA3794),16);const auto levelBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xBA6714),4);const auto levelAfter=ReversingProbe::Bytes(reinterpret_cast<void*>(0xBA671C),4);
 OriginalDependencies499::HookRoute route("CTheZones","SetCurrentZoneAsUnlocked",0x572800,&CTheZones::SetCurrentZoneAsUnlocked);
 std::vector<Helper> helpers;for(auto at:{0x56E010u,0x572300u,0x572270u,0x572130u,0x821B40u})helpers.emplace_back(at);
 auto helpersOK=[&]{for(auto& h:helpers)if(!h.Check())return false;return true;};
 const bool nativePlayerCoords=helpers[0].body==0x56E010;
 auto nativeOK=[&]{return std::equal(canonical_572130.begin(),canonical_572130.end(),reinterpret_cast<const uint8*>(0x572130))&&std::equal(canonical_821B40.begin(),canonical_821B40.end(),reinterpret_cast<const uint8*>(0x821B40))&&(!nativePlayerCoords||std::equal(canonical_56E010.begin(),canonical_56E010.end(),reinterpret_cast<const uint8*>(0x56E010)));};
 auto mapIntact=[&]{return mapCount==uint16(CTheZones::TotalNumberOfMapZones)&&savedMap==ReversingProbe::Bytes(CTheZones::MapZoneArray.data(),savedMap.size());};
 auto actorIdentity=[&]{return *reinterpret_cast<uint8*>(0xB7CD74)==selected&&*reinterpret_cast<CPlayerPed**>(0xB7CD98+selected*0x190)==player&&FindPlayerPed()==player&&*reinterpret_cast<CMatrixLink**>(reinterpret_cast<uint8*>(player)+0x14)==matrix;};
 auto guards=[&]{return gridBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0xBA371C),16)&&gridAfter==ReversingProbe::Bytes(reinterpret_cast<void*>(0xBA3794),16)&&levelBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0xBA6714),4)&&levelAfter==ReversingProbe::Bytes(reinterpret_cast<void*>(0xBA671C),4);};
 auto restore=[&]{std::memcpy(position,savedPosition.data(),12);std::memcpy(reinterpret_cast<void*>(0xBA3730),savedGrid.data(),100);*reinterpret_cast<uint32*>(0xBA372C)=savedCounter;*reinterpret_cast<uint32*>(0xBA6718)=savedLevel;*reinterpret_cast<uint8*>(0xA444A4)=savedOffMap;*reinterpret_cast<uint32*>(0xB72914)=savedArea;RestoreFP(fp);};
 auto restored=[&]{return actorIdentity()&&savedPlayer==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&(!matrix||savedMatrix==ReversingProbe::Bytes(matrix,sizeof(CMatrixLink)))&&savedPosition==ReversingProbe::Bytes(position,12)&&savedGrid==ReversingProbe::Bytes(reinterpret_cast<void*>(0xBA3730),100)&&*reinterpret_cast<uint32*>(0xBA372C)==savedCounter&&*reinterpret_cast<uint32*>(0xBA6718)==savedLevel&&*reinterpret_cast<uint8*>(0xA444A4)==savedOffMap&&*reinterpret_cast<uint32*>(0xB72914)==savedArea&&mapIntact()&&guards();};
 json hs=json::array();for(auto& h:helpers)hs.push_back(h.Metadata());RestoreFP(fp);
 Write({{"event","begin"},{"registered_hook",route.Registered()},{"actor_real_selected_player",actorIdentity()},{"on_foot",true},{"helper_routes",hs},{"helper_routes_ok",helpersOK()},{"native_complete_bodies_ok",nativeOK()},{"player_coords_native_code_checked",nativePlayerCoords},{"player_matrix_present",matrix!=nullptr},{"player_snapshot_bytes",sizeof(CPlayerPed)},{"matrix_snapshot_bytes",savedMatrix.size()},
 {"map_zone_count",mapCount},{"map_zone_bytes",savedMap},{"coordinates",coordinates},{"cells",cells},{"native_visited_bytes",canonical_572130},{"native_ftol_bytes",canonical_821B40}});RestoreFP(fp);
 const bool ready=route.Registered()&&helpersOK()&&nativeOK()&&restored();
 if(!ready){restore();const bool memoryRestored=restored();const bool routeRestored=route.Restore();RestoreFP(fp);Write({{"event","error"},{"reason","Initial registered route/helper/native/restoration gate failed; cases not executed"},{"pairs",0},{"state_restored",memoryRestored&&routeRestored},{"helper_route_ok",helpersOK()},{"native_complete_bodies_ok",nativeOK()},{"floating_state_restored",CaptureFP()==fp}});RestoreFP(fp);return;}
 bool passed=true;int failures=0;
 for(unsigned scenario=0;scenario<2048;++scenario){auto invoke=[&](bool candidate){restore();const auto xi=scenario%16,yi=(scenario/16)%16;const bool offMap=(scenario&256)!=0,visited=(scenario&512)!=0,interior=(scenario&1024)!=0;const auto cell=10*cells[xi]+9-cells[yi];const CVector input{coordinates[xi],coordinates[yi],0};const auto expectedLevel=ExpectedLevel(input,mapCount);
   std::array<uint8,100> inputGrid{};uint32 inputCount=0;for(unsigned i=0;i<100;++i)inputGrid[i]=uint8((i+scenario*7)%11==0);inputGrid[cell]=uint8(visited);for(auto b:inputGrid)inputCount+=b;
   std::memcpy(position,&input,12);std::memcpy(reinterpret_cast<void*>(0xBA3730),inputGrid.data(),100);*reinterpret_cast<uint32*>(0xBA372C)=inputCount;*reinterpret_cast<uint32*>(0xBA6718)=scenario&3;*reinterpret_cast<uint8*>(0xA444A4)=uint8(offMap);*reinterpret_cast<uint32*>(0xB72914)=interior?1:0;
   const auto inputPlayer=ReversingProbe::Bytes(player,sizeof(CPlayerPed));const auto inputMatrix=matrix?ReversingProbe::Bytes(matrix,sizeof(CMatrixLink)):std::vector<uint8>{};CVector actualCoords{};
   // Invoke the exact current native collaborator ABI, including its hidden return pointer.
   plugin::Call<0x56E010,CVector*,int32>(&actualCoords,-1);bool actorOK=actorIdentity()&&std::memcmp(&actualCoords,&input,12)==0&&inputPlayer==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&(!matrix||inputMatrix==ReversingProbe::Bytes(matrix,sizeof(CMatrixLink)));
   bool helperOK=helpersOK()&&nativeOK(),routeOK=false;RestoreFP(fp);const auto abi=route.Invoke(candidate,[&]{routeOK=ReversingProbe::JumpsToDll(0x572800)==candidate;return Gate(0x572800,0xA5000000|scenario);});helperOK&=helpersOK()&&nativeOK();
   const auto actualGrid=ReversingProbe::Bytes(reinterpret_cast<void*>(0xBA3730),100);const auto actualCount=*reinterpret_cast<uint32*>(0xBA372C),actualLevel=*reinterpret_cast<uint32*>(0xBA6718);auto expectedGrid=inputGrid;const bool changed=!offMap&&!visited&&!interior;if(changed)expectedGrid[cell]=1;
   actorOK&=actorIdentity()&&inputPlayer==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&(!matrix||inputMatrix==ReversingProbe::Bytes(matrix,sizeof(CMatrixLink)));
   const bool contract=std::equal(expectedGrid.begin(),expectedGrid.end(),actualGrid.begin())&&actualCount==inputCount+uint32(changed)&&actualLevel==expectedLevel&&*reinterpret_cast<uint8*>(0xA444A4)==uint8(offMap)&&*reinterpret_cast<uint32*>(0xB72914)==uint32(interior)&&mapIntact();const bool guardOK=guards();const auto coordBytes=ReversingProbe::Bytes(&actualCoords,12);restore();
   return json{{"x_index",xi},{"y_index",yi},{"off_map",offMap},{"visited",visited},{"interior",interior},{"cell",cell},{"input_count",inputCount},{"expected_level",expectedLevel},{"level",actualLevel},{"count",actualCount},{"grid",actualGrid},{"coordinates_bytes",coordBytes},{"cell_changed",changed},{"stack_restored",abi.stack},{"nonvolatile_preserved",abi.nv},{"seh_restored",abi.seh},{"route_ok",routeOK},{"helper_route_ok",helperOK},{"fixture_contract",contract},{"actor_intact",actorOK},{"map_intact",mapIntact()},{"guards_preserved",guardOK},{"state_restored",restored()},{"floating_state_restored",CaptureFP()==fp}};
  };
  const auto original=invoke(false);const auto candidate=invoke(true);bool ok=original==candidate;for(auto k:{"stack_restored","nonvolatile_preserved","seh_restored","route_ok","helper_route_ok","fixture_contract","actor_intact","map_intact","guards_preserved","state_restored","floating_state_restored"})ok&=original[k]==true&&candidate[k]==true;passed&=ok;failures+=!ok;Write({{"event","comparison"},{"function","CTheZones::SetCurrentZoneAsUnlocked"},{"pair",scenario},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",ok},{"accepted",ok}});RestoreFP(fp);
 }
 Write({{"event","function_summary"},{"function","CTheZones::SetCurrentZoneAsUnlocked"},{"cases",2048},{"failures",failures}});restore();const bool finalMemory=restored(),finalHelpers=helpersOK()&&nativeOK();const bool finalRoute=route.Restore();const bool final=finalMemory&&finalHelpers&&finalRoute;RestoreFP(fp);
 Write({{"event",passed&&final?"complete":"error"},{"pairs",2048},{"state_restored",final},{"helper_route_ok",helpersOK()},{"native_complete_bodies_ok",nativeOK()},{"floating_state_restored",CaptureFP()==fp}});RestoreFP(fp);
}
}
