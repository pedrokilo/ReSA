#pragma once
#include <fstream>
#include <set>
#include "CarCtrl.h"
#include "PathFind.h"
#include "Core/MatrixLinkList.h"
#include "Pools/Pools.h"
#include "Pools/VehiclePool.h"
#include "NativeProof.hpp"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesWorld14Live {
struct Fn{const char* cls;const char* name;uintptr_t address,vtable;int slot,cases;};
inline constexpr Fn fns[]{
{"CCarCtrl","FindGhostRoadHeight",0x422370,0,-1,2048}
};
inline bool done{};inline std::ofstream log;inline uint64 waitSince{},lastWaitDiagnostic{};
inline void Write(const json& r);
struct Result{std::array<uint8,10> extended;uint32 bits;bool abi,topBalanced;};
__declspec(noinline) inline Result Call(uintptr_t entry,CVehicle* vehicle){
 uint32 before{},after{},rb{},rs{},rd{},fb{},fa{};uint16 swBefore{},swAfter{};float value{};std::array<uint8,10> extended{};void* destination=extended.data();
 __asm {
  fnstsw swBefore
  push ebx
  push esi
  push edi
  mov before,esp
  mov fb,ebp
  mov ebx,13579BDFh
  mov esi,2468ACE0h
  mov edi,10293847h
  push vehicle
  call entry
  add esp,4
  mov eax,destination
  fstp tbyte ptr [eax]
  fnstsw swAfter
  fld tbyte ptr [eax]
  fstp value
  mov after,esp
  mov fa,ebp
  mov rb,ebx
  mov rs,esi
  mov rd,edi
  pop edi
  pop esi
  pop ebx
 }
 return {extended,std::bit_cast<uint32>(value),before==after&&fb==fa&&rb==0x13579BDF&&rs==0x2468ACE0&&rd==0x10293847,(swBefore&0x3800)==(swAfter&0x3800)};
}
inline void NativeRemove(uintptr_t entry,CVehicle* vehicle){__asm{mov ecx,vehicle}__asm{call entry}}
inline uint16 Top(){uint16 v{};__asm{fnstsw v}return v&0x3800;}
inline void SaveFP(void* p){__asm{mov eax,p}__asm{fnsave [eax]}__asm{frstor [eax]}}
inline void RestoreFP(void* p){__asm{mov eax,p}__asm{frstor [eax]}}
inline uint32 ReadMXCSR(){uint32 v{};__asm{stmxcsr v}return v;}
inline void WriteMXCSR(uint32 v){__asm{ldmxcsr v}}
alignas(16) inline std::array<uint8,108> canonicalFP{};inline uint32 canonicalMXCSR{};inline bool canonicalActive{};
inline void RestoreCanonical(){if(canonicalActive){RestoreFP(canonicalFP.data());WriteMXCSR(canonicalMXCSR);}}
inline bool CanonicalRestored(){alignas(16)std::array<uint8,108> current{};SaveFP(current.data());return current==canonicalFP&&ReadMXCSR()==canonicalMXCSR;}
inline void Write(const json& r){alignas(16)std::array<uint8,108> current{};const auto mx=ReadMXCSR();if(!canonicalActive)SaveFP(current.data());RestoreCanonical();ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();if(canonicalActive)RestoreCanonical();else{RestoreFP(current.data());WriteMXCSR(mx);}}
inline uintptr_t Resolve(uintptr_t e){for(int n=0;n<16&&*reinterpret_cast<const uint8*>(e)==0xE9;++n)e+=5+*reinterpret_cast<const int32*>(e+1);return e;}
inline bool ModuleIs(uintptr_t e,HMODULE expected){HMODULE owner{};const DWORD flags=GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;return GetModuleHandleExA(flags,reinterpret_cast<LPCSTR>(e),&owner)&&owner==expected;}
inline HMODULE ProbeModule(){HMODULE p{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&Call),&p);return p;}
inline bool RemoveProof(){return Resolve(0x54F3B0)==0x54F3B0&&ModuleIs(0x54F3B0,GetModuleHandleA(nullptr))&&std::equal(nativeRemoveMatrix.begin(),nativeRemoveMatrix.end(),reinterpret_cast<const uint8*>(0x54F3B0));}
struct Edge{CNodeAddress a,b;CPathNode* first;CPathNode* second;CNodeAddress* link;CVector p,q;};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"dependenciesworld14-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependenciesworld14-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesworld14");}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;SaveFP(canonicalFP.data());canonicalMXCSR=ReadMXCSR();canonicalActive=true;
 std::vector<Edge> edges;int32 unloaded=-1;for(uint32 area=0;area<64;++area){auto* nodes=ThePaths.m_pPathNodes[area];if(!nodes){if(unloaded<0)unloaded=int32(area);continue;}auto* links=ThePaths.m_pNodeLinks[area];if(!links)continue;for(uint32 i=0;i<ThePaths.m_anNumVehicleNodes[area]&&edges.size()<4096;++i){auto& n=nodes[i];if(n.m_wBaseLinkId<0)continue;for(uint32 k=0;k<n.m_nNumLinks;++k){const uint32 index=uint32(n.m_wBaseLinkId)+k;if(index>=ThePaths.m_anNumAddresses[area])continue;auto* link=&links[index];const auto to=*link;if(to.m_wAreaId>=64||!ThePaths.m_pPathNodes[to.m_wAreaId]||to.m_wNodeId>=ThePaths.m_anNumVehicleNodes[to.m_wAreaId])continue;auto* target=&ThePaths.m_pPathNodes[to.m_wAreaId][to.m_wNodeId];const CVector a=n.GetPosition(),b=target->GetPosition();if(a.x==b.x&&a.y==b.y)continue;edges.push_back({{uint16(area),uint16(i)},to,&n,target,link,a,b});}}}
 CVehicle* vehicle=nullptr;if(auto* pool=GetVehiclePool())for(uint32 i=0;i<pool->GetSize();++i){auto* v=pool->GetAt(i);if(v&&v->m_matrix&&v->m_matrix->m_pOwner==v){vehicle=v;break;}}
 if(!vehicle||edges.empty()||unloaded<0||FindPlayerPed()->GetAreaCode()!=AREA_CODE_NORMAL_WORLD){const uint64 now=GetTickCount64();if(!waitSince)waitSince=now;const bool expired=now-waitSince>=60000;if(expired||!lastWaitDiagnostic||now-lastWaitDiagnostic>=5000){lastWaitDiagnostic=now;RestoreCanonical();Write({{"event",expired?"error":"waiting_for_real_road_resources"},{"reason","Waiting for exterior real vehicle, linked loaded road nodes and actual unloaded area"},{"edge_count",edges.size()},{"vehicle_available",vehicle!=nullptr},{"unloaded_area",unloaded},{"player_area",int(FindPlayerPed()->GetAreaCode())},{"elapsed_ms",now-waitSince},{"fp_state_restored",CanonicalRestored()}});}RestoreCanonical();canonicalActive=false;done=expired;return;}done=true;
 auto* matrix=vehicle->m_matrix;const auto vehicleSaved=ReversingProbe::Bytes(vehicle,sizeof(CVehicle)),listSaved=ReversingProbe::Bytes(&gMatrixList,sizeof(gMatrixList)),pathsSaved=ReversingProbe::Bytes(&ThePaths,sizeof(ThePaths));std::set<CMatrixLink*> borrowed{matrix,matrix->m_pPrev,matrix->m_pNext,gMatrixList.m_freeListHead.m_pNext};std::vector<std::pair<CMatrixLink*,std::vector<uint8>>> linksSaved;for(auto* m:borrowed){if(!m){RestoreCanonical();Write({{"event","error"},{"reason","Matrix pool link invariant missing"}});return;}linksSaved.push_back({m,ReversingProbe::Bytes(m,sizeof(CMatrixLink))});}
 auto restore=[&]{std::memcpy(&gMatrixList,listSaved.data(),listSaved.size());for(const auto&[m,bytes]:linksSaved)std::memcpy(m,bytes.data(),bytes.size());std::memcpy(vehicle,vehicleSaved.data(),vehicleSaved.size());};auto restoredState=[&]{if(vehicleSaved!=ReversingProbe::Bytes(vehicle,vehicleSaved.size())||listSaved!=ReversingProbe::Bytes(&gMatrixList,listSaved.size())||pathsSaved!=ReversingProbe::Bytes(&ThePaths,pathsSaved.size()))return false;for(const auto&[m,bytes]:linksSaved)if(bytes!=ReversingProbe::Bytes(m,bytes.size()))return false;return true;};
 const auto positionPointer=std::bit_cast<uintptr_t>(&CPathNode::GetPosition);OriginalDependencies499::HookRoute route("CCarCtrl","FindGhostRoadHeight",0x422370,&CCarCtrl::FindGhostRoadHeight),helper("CPathNode","GetPosition",0x420A10,FunctionToVoidPtr(&CPathNode::GetPosition)),remove("CPlaceable","RemoveMatrix",0x54F3B0,FunctionToVoidPtr(&CPlaceable::RemoveMatrix));const auto helperOff=ReversingProbe::Bytes(reinterpret_cast<void*>(0x420A10),32);std::vector<uint8> helperOn;const bool helperReady=helper.Invoke(true,[&]{helperOn=ReversingProbe::Bytes(reinterpret_cast<void*>(helper.Body()),32);return helper.Registered()&&Resolve(positionPointer)==helper.Body()&&ModuleIs(helper.Body(),ProbeModule());});const bool removeReady=remove.Invoke(false,[&]{return remove.Registered()&&RemoveProof();});RestoreCanonical();Write({{"event","begin"},{"registered_hook",route.Registered()},{"helper_registered",helper.Registered()},{"helper_route_ok",helperReady},{"helper_entry",0x420A10},{"helper_body",helper.Body()},{"helper_off_bytes",helperOff},{"helper_on_bytes",helperOn},{"remove_registered",remove.Registered()},{"remove_route_ok",removeReady},{"remove_entry",0x54F3B0},{"remove_bytes",nativeRemoveMatrix},{"real_edges",edges.size()},{"actual_unloaded_area",unloaded},{"player_area",int(FindPlayerPed()->GetAreaCode())},{"collaborators_stubbed",false},{"fp_state_restored",CanonicalRestored()}});
 if(!route.Registered()||!helperReady||!removeReady){
  const bool mainRestored=route.Restore();const bool helperRestored=helper.Restore();const bool removeRestored=remove.Restore();
  restore();RestoreCanonical();Write({{"event","error"},{"reason","Registered main/helper/native-remove preflight rejected"},{"pairs",0},{"state_restored",mainRestored&&helperRestored&&removeRestored&&restoredState()&&CanonicalRestored()},{"helper_route_restored",helperRestored},{"remove_route_restored",removeRestored},{"fp_state_restored",CanonicalRestored()}});return;
 }
 bool passed=true;uint32 failures=0;std::array<uint32,8> modes{};std::array<uint32,2> representations{};
 for(int scenario=0;scenario<2048;++scenario){const auto& edge=edges[uint32(scenario/8)%edges.size()];const uint32 mode=scenario%8;const bool matrixPresent=((scenario/8)&1)!=0;CVector position=mode==5?edge.p:mode==6?edge.q:CVector{(edge.p.x+edge.q.x)*0.5f,(edge.p.y+edge.q.y)*0.5f,(edge.p.z+edge.q.z)*0.5f};const auto nodeA=ReversingProbe::Bytes(edge.first,sizeof(CPathNode)),nodeB=ReversingProbe::Bytes(edge.second,sizeof(CPathNode)),pathLink=ReversingProbe::Bytes(edge.link,sizeof(CNodeAddress));
 auto run=[&](bool candidate){restore();vehicle->m_autoPilot.m_currentAddress=mode==0||mode==1?CNodeAddress{}:mode==3?CNodeAddress{uint16(unloaded),0}:edge.a;vehicle->m_autoPilot.m_startingRouteNode=mode==0||mode==2?CNodeAddress{}:mode==4?CNodeAddress{uint16(unloaded),0}:edge.b;matrix->GetPosition()=position;bool removeOK=remove.Invoke(false,[&]{const bool before=RemoveProof();if(!matrixPresent)NativeRemove(0x54F3B0,vehicle);return before&&RemoveProof();});const bool representationOK=matrixPresent?vehicle->m_matrix==matrix&&matrix->m_pOwner==vehicle:vehicle->m_matrix==nullptr&&matrix->m_pOwner==nullptr&&gMatrixList.m_freeListHead.m_pNext==matrix;const auto fixtureVehicle=ReversingProbe::Bytes(vehicle,sizeof(CVehicle)),fixtureList=ReversingProbe::Bytes(&gMatrixList,sizeof(gMatrixList));std::vector<std::pair<CMatrixLink*,std::vector<uint8>>> fixtureLinks;for(auto* m:borrowed)fixtureLinks.push_back({m,ReversingProbe::Bytes(m,sizeof(CMatrixLink))});RestoreCanonical();bool helperOK=false,routeOK=false;const auto result=helper.Invoke(candidate,[&]{auto check=[&]{return helper.Registered()&&ReversingProbe::JumpsToDll(0x420A10)==candidate&&(candidate?Resolve(positionPointer)==helper.Body()&&ModuleIs(helper.Body(),ProbeModule())&&helperOn==ReversingProbe::Bytes(reinterpret_cast<void*>(helper.Body()),32):helperOff==ReversingProbe::Bytes(reinterpret_cast<void*>(0x420A10),32));};helperOK=check();const auto r=route.Invoke(candidate,[&]{routeOK=ReversingProbe::JumpsToDll(0x422370)==candidate;if(candidate)routeOK&=Resolve(0x422370)==route.Body()&&ModuleIs(route.Body(),ProbeModule());return Call(0x422370,vehicle);});helperOK&=check();return r;});bool ro=fixtureVehicle==ReversingProbe::Bytes(vehicle,sizeof(CVehicle))&&fixtureList==ReversingProbe::Bytes(&gMatrixList,sizeof(gMatrixList))&&pathsSaved==ReversingProbe::Bytes(&ThePaths,sizeof(ThePaths))&&nodeA==ReversingProbe::Bytes(edge.first,sizeof(CPathNode))&&nodeB==ReversingProbe::Bytes(edge.second,sizeof(CPathNode))&&pathLink==ReversingProbe::Bytes(edge.link,sizeof(CNodeAddress));for(const auto&[m,bytes]:fixtureLinks)ro&=bytes==ReversingProbe::Bytes(m,bytes.size());const float value=std::bit_cast<float>(result.bits);const bool numeric=mode<5?result.bits==0:std::isfinite(value)&&value>=std::min(edge.p.z,edge.q.z)-0.001f&&value<=std::max(edge.p.z,edge.q.z)+0.001f;restore();RestoreCanonical();json record={{"return_bits",result.bits},{"return_extended",result.extended},{"fixture_mode",mode},{"matrix_present",matrixPresent},{"source_area",edge.a.m_wAreaId},{"source_node",edge.a.m_wNodeId},{"target_area",edge.b.m_wAreaId},{"target_node",edge.b.m_wNodeId},{"route_ok",routeOK},{"helper_route_ok",helperOK},{"remove_route_ok",removeOK},{"representation_legal",representationOK},{"stack_restored",result.abi},{"registers_preserved",result.abi},{"fpu_top_balanced",result.topBalanced},{"contract_ok",numeric},{"readonly_ok",ro},{"state_restored",restoredState()},{"fp_state_restored",CanonicalRestored()}};RestoreCanonical();return record;};
 const auto original=run(false),candidate=run(true);bool equal=route.Registered()&&original==candidate;for(auto k:{"route_ok","helper_route_ok","remove_route_ok","representation_legal","stack_restored","registers_preserved","fpu_top_balanced","contract_ok","readonly_ok","state_restored","fp_state_restored"})equal&=original[k]==true;passed&=equal;failures+=!equal;++modes[mode];++representations[matrixPresent];Write({{"event","comparison"},{"pair",scenario},{"case",scenario},{"function","CCarCtrl::FindGhostRoadHeight"},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});}
 bool coverage=true;for(auto n:modes)coverage&=n==256;for(auto n:representations)coverage&=n==1024;passed&=coverage;Write({{"event","function_summary"},{"function","CCarCtrl::FindGhostRoadHeight"},{"cases",2048},{"failures",failures},{"complete_paths",coverage},{"mode_cases",modes},{"representation_cases",representations}});const bool mainRestore=route.Restore(),helperRestore=helper.Restore(),removeRestore=remove.Restore();restore();RestoreCanonical();const bool fp=CanonicalRestored(),restored=mainRestore&&helperRestore&&removeRestore&&restoredState()&&fp;Write({{"event",passed&&restored?"complete":"error"},{"pairs",2048},{"state_restored",restored},{"fp_state_restored",fp},{"helper_route_ok",helperRestore},{"remove_route_ok",removeRestore}});
}
}
