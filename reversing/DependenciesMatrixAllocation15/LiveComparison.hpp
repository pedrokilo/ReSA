#pragma once
#include <fstream>
#include <set>
#include <memory>
#include "Pools/Pools.h"
#include "Pools/VehiclePool.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Core/MatrixLinkList.h"
#include "Core/MatrixLink.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "../DependenciesWorld15/MatrixPoolProof.hpp"
#include "../DependenciesWorld14/NativeProof.hpp"
namespace DependenciesMatrixAllocation15Live {
struct Fn{const char* cls;const char* name;uintptr_t address,vtable;int slot,cases;};
inline constexpr Fn fns[]{
{"CMatrixLinkList","AddToList1",0x54E9D0,0,-1,2048},
{"CPlaceable","AllocateMatrix",0x54F560,0,-1,2048},
{"CMatrixLinkList","AddToList2",0x54EA10,0,-1,2048}
};
inline bool done{};inline std::ofstream log;inline uint64 waitSince{},lastDiagnostic{};
inline void Write(const json&);
struct Result{uint32 eax;bool abi;};
__declspec(noinline) inline Result Call(uintptr_t entry,void* object){
 uint32 before{},after{},fb{},fa{},rb{},rs{},rd{},eaxCaptured{};
 __asm {
  push ebx
  push esi
  push edi
  mov before,esp
  mov fb,ebp
  mov ebx,13579BDFh
  mov esi,2468ACE0h
  mov edi,10293847h
  mov ecx,object
  call entry
  mov eaxCaptured,eax
  mov after,esp
  mov fa,ebp
  mov rb,ebx
  mov rs,esi
  mov rd,edi
  pop edi
  pop esi
  pop ebx
 }
 return {eaxCaptured,before==after&&fb==fa&&rb==0x13579BDF&&rs==0x2468ACE0&&rd==0x10293847};
}
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

static_assert(sizeof(CMatrixLink)==0x54);
static_assert(offsetof(CMatrixLinkList,m_freeListHead)==0x150);
static_assert(offsetof(CMatrixLinkList,m_freeListTail)==0x1A4);
static_assert(offsetof(CMatrixLink,m_pNext)==0x50);
static_assert(offsetof(CMatrixLink,m_pPrev)==0x4C);
using MatrixSnapshot=DependenciesWorld15Matrix::Snapshot;
struct OwnerSnapshot {
 std::vector<std::pair<CPlaceable*,std::vector<uint8>>> owners;
 bool legal{true};
 explicit OwnerSnapshot(const MatrixSnapshot& pool,CVehicle* target){
  std::set<CPlaceable*> unique;for(const auto&[m,bytes]:pool.nodes){auto* owner=m->m_pOwner;if(owner){if(owner->m_matrix!=m||!unique.insert(owner).second)legal=false;}}
  unique.insert(target);for(auto* o:unique)owners.push_back({o,ReversingProbe::Bytes(o,sizeof(CPlaceable))});
 }
 void Restore()const{for(const auto&[at,bytes]:owners)std::memcpy(at,bytes.data(),bytes.size());}
 bool Equal()const{for(const auto&[at,bytes]:owners)if(bytes!=ReversingProbe::Bytes(at,bytes.size()))return false;return true;}
 json Current(CPlaceable* target)const{json out=json::array();for(const auto&[at,bytes]:owners){auto current=ReversingProbe::Bytes(at,bytes.size());if(at==target||current!=bytes)out.push_back({{"owner",uintptr_t(at)},{"bytes",current}});}return out;}
};
struct RouteSpec{const char* cls;const char* name;uintptr_t entry;void* candidate;};
inline bool RemoveProof(){return Resolve(0x54F3B0)==0x54F3B0&&ModuleIs(0x54F3B0,GetModuleHandleA(nullptr))&&std::equal(DependenciesWorld14Live::nativeRemoveMatrix.begin(),DependenciesWorld14Live::nativeRemoveMatrix.end(),reinterpret_cast<const uint8*>(0x54F3B0));}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependenciesmatrixallocation15-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependenciesmatrixallocation15-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesmatrixallocation15");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;SaveFP(canonicalFP.data());canonicalMXCSR=ReadMXCSR();canonicalActive=true;
 MatrixSnapshot baseline;CVehicle* vehicle=nullptr;uint32 freeCount{};if(baseline.legal)for(auto* m=gMatrixList.m_freeListHead.m_pNext;m!=&gMatrixList.m_freeListTail;m=m->m_pNext)++freeCount;
 if(auto* p=GetVehiclePool())for(uint32 i=0;i<p->GetSize();++i){auto* v=p->GetAt(i);if(v&&v->m_matrix&&v->m_matrix->m_pOwner==v&&gMatrixList.m_tail.m_pPrev->m_pOwner&&gMatrixList.m_tail.m_pPrev->m_pOwner!=v){vehicle=v;break;}}
 if(!baseline.legal||freeCount<2||!vehicle||FindPlayerPed()->GetAreaCode()!=AREA_CODE_NORMAL_WORLD){const auto now=GetTickCount64();if(!waitSince)waitSince=now;const bool expired=now-waitSince>=60000;RestoreCanonical();if(expired||!lastDiagnostic||now-lastDiagnostic>=5000){lastDiagnostic=now;Write({{"event",expired?"error":"waiting_for_real_matrix_resources"},{"real_matrices",baseline.Count()},{"pool_legal",baseline.legal},{"free_count",freeCount},{"real_vehicle_ready",vehicle!=nullptr},{"elapsed_ms",now-waitSince}});}RestoreCanonical();canonicalActive=false;if(expired)done=true;return;}
 done=true;OwnerSnapshot owners(baseline,vehicle);const auto vehicleBefore=ReversingProbe::Bytes(vehicle,sizeof(CVehicle));
 auto restore=[&]{baseline.Restore();owners.Restore();std::memcpy(vehicle,vehicleBefore.data(),vehicleBefore.size());};
 auto restored=[&]{return baseline.Equal()&&owners.Equal()&&vehicleBefore==ReversingProbe::Bytes(vehicle,sizeof(CVehicle));};
 const RouteSpec specs[]{
 {"CMatrixLinkList","AddToList1",0x54E9D0,FunctionToVoidPtr(&CMatrixLinkList::AddToList1)},
 {"CPlaceable","AllocateMatrix",0x54F560,FunctionToVoidPtr(&CPlaceable::AllocateMatrix)},
 {"CMatrixLinkList","MoveToList1",0x54EA60,FunctionToVoidPtr(&CMatrixLinkList::MoveToList1)},
 {"CMatrixLinkList","MoveToFreeList",0x54EAD0,FunctionToVoidPtr(&CMatrixLinkList::MoveToFreeList)},
 {"CPlaceable","RemoveMatrix",0x54F3B0,FunctionToVoidPtr(&CPlaceable::RemoveMatrix)},
 {"CMatrixLink","Insert",0x54E8F0,FunctionToVoidPtr(&CMatrixLink::Insert)},
 {"CMatrixLink","Remove",0x54E910,FunctionToVoidPtr(&CMatrixLink::Remove)},
 {"CMatrixLinkList","AddToList2",0x54EA10,FunctionToVoidPtr(&CMatrixLinkList::AddToList2)},
 {"CMatrixLinkList","MoveToList2",0x54EA90,FunctionToVoidPtr(&CMatrixLinkList::MoveToList2)}
 };
 std::vector<std::unique_ptr<OriginalDependencies499::HookRoute>> routes;std::vector<std::vector<uint8>> off,on;bool ready=owners.legal;
 for(const auto& s:specs){routes.push_back(std::make_unique<OriginalDependencies499::HookRoute>(s.cls,s.name,s.entry,s.candidate));off.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(s.entry),32));on.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(routes.back()->Body()),32));ready&=routes.back()->Registered();}
 auto switchAll=[&](bool candidate,auto&& fn){auto recurse=[&](auto&& self,size_t i)->void{if(i==routes.size()){fn();return;}routes[i]->Invoke(candidate,[&]{self(self,i+1);return 0;});};recurse(recurse,0);};
 auto routeProof=[&](bool candidate){bool ok=true;for(size_t i=0;i<routes.size();++i){const auto&s=specs[i];auto&r=*routes[i];ok&=r.Registered()&&ReversingProbe::JumpsToDll(s.entry)==candidate;if(candidate)ok&=Resolve(s.entry)==r.Body()&&Resolve(uintptr_t(s.candidate))==r.Body()&&ModuleIs(r.Body(),ProbeModule())&&on[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(r.Body()),32);else ok&=off[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(s.entry),32);}return ok;};
 switchAll(true,[&]{ready&=routeProof(true);});switchAll(false,[&]{ready&=routeProof(false)&&RemoveProof();});
 auto prepare=[&](int fn,uint32 kind){restore();bool ok=true;switchAll(false,[&]{ok&=routeProof(false)&&RemoveProof();if(fn==1&&kind>0)Call(0x54F3B0,vehicle);if((fn!=1&&kind==1)||(fn==1&&kind==2)){size_t n=0;while(gMatrixList.m_freeListHead.m_pNext!=&gMatrixList.m_freeListTail&&n++<=baseline.Count()){const auto a=Call(0x54E9D0,&gMatrixList);ok&=a.abi&&a.eax!=0;}ok&=gMatrixList.IsFull();}ok&=routeProof(false)&&RemoveProof();});return ok;};
 for(int fn=0;fn<3;++fn)for(uint32 kind=0;kind<uint32(fn!=1?2:3);++kind){ready&=prepare(fn,kind);if(fn==1&&kind==2)ready&=gMatrixList.m_tail.m_pPrev->m_pOwner&&gMatrixList.m_tail.m_pPrev->m_pOwner!=vehicle;restore();}
 json routeMetadata=json::array();for(size_t i=0;i<routes.size();++i)routeMetadata.push_back({{"class",specs[i].cls},{"name",specs[i].name},{"entry",specs[i].entry},{"body",routes[i]->Body()},{"registered",routes[i]->Registered()},{"off_bytes",off[i]},{"on_bytes",on[i]}});
 RestoreCanonical();Write({{"event","begin"},{"route_proofs",routeMetadata},{"free_head_next_address",uintptr_t(&gMatrixList.m_freeListHead.m_pNext)},{"free_tail_prev_address",uintptr_t(&gMatrixList.m_freeListTail.m_pPrev)},{"registered_hook",ready},{"all_routes_registered",ready},{"collaborators_stubbed",false},{"real_pool_count",baseline.Count()},{"initial_free_count",freeCount},{"full_pool_fixture_checked",ready},{"owned_actors_created",0},{"fixture_configuration","FastLoader.SaveGameToLoad=0"},{"player_area",int(FindPlayerPed()->GetAreaCode())},{"fp_state_restored",CanonicalRestored()}});
 if(!ready){bool rh=true;for(auto& r:routes){const bool x=r->Restore();rh&=x;}restore();RestoreCanonical();Write({{"event","error"},{"reason","Real pool/routing preflight rejected"},{"pairs",0},{"state_restored",rh&&restored()&&CanonicalRestored()}});return;}
 bool passed=true;int pair=0;
 for(int fn=0;fn<3;++fn){uint32 failures=0;std::array<uint32,3> coverage{};for(int scenario=0;scenario<2048;++scenario){const uint32 kind=uint32(scenario)%(fn!=1?2:3);
  auto run=[&](bool candidate){const bool prepared=prepare(fn,kind);MatrixSnapshot fixture;const auto expected=fn!=1?(kind==1?uintptr_t(0):uintptr_t(gMatrixList.m_freeListHead.m_pNext)):(kind==0?uintptr_t(vehicle->m_matrix):kind==1?uintptr_t(gMatrixList.m_freeListHead.m_pNext):uintptr_t(gMatrixList.m_tail.m_pPrev));CPlaceable* evicted=fn==1&&kind==2?gMatrixList.m_tail.m_pPrev->m_pOwner:nullptr;Result result{};bool routeOK=false;RestoreCanonical();switchAll(candidate,[&]{routeOK=routeProof(candidate);result=Call(specs[fn==2?7:fn].entry,fn!=1?static_cast<void*>(&gMatrixList):static_cast<void*>(vehicle));routeOK&=routeProof(candidate);});const auto actual=fn!=1?uintptr_t(result.eax):uintptr_t(vehicle->m_matrix);const bool contract=actual==expected&&(fn!=1||(vehicle->m_matrix&&vehicle->m_matrix->m_pOwner==vehicle))&&(!evicted||evicted->m_matrix==nullptr);
  json changes=json::array();for(const auto&[at,bytes]:fixture.nodes){auto current=ReversingProbe::Bytes(at,bytes.size());if(current!=bytes)changes.push_back({{"node",uintptr_t(at)},{"bytes",current}});}
  const auto currentOwners=owners.Current(vehicle);const auto currentHeader=ReversingProbe::Bytes(&gMatrixList,sizeof(gMatrixList));const bool fullGraphLegal=MatrixSnapshot{}.legal;bool vehicleRestReadOnly=true;auto vehicleAfter=ReversingProbe::Bytes(vehicle,sizeof(CVehicle));for(size_t i=sizeof(CPlaceable);i<vehicleBefore.size();++i)vehicleRestReadOnly&=vehicleAfter[i]==vehicleBefore[i];restore();RestoreCanonical();json value={{"fixture_kind",kind},{"result_pointer",actual},{"expected_pointer",expected},{"changed_nodes",changes},{"owners",currentOwners},{"pool_header",currentHeader},{"route_ok",routeOK},{"fixture_legal",prepared&&fixture.legal},{"graph_legal",fullGraphLegal},{"contract_ok",contract},{"readonly_entity_tail_ok",vehicleRestReadOnly},{"stack_restored",result.abi},{"registers_preserved",result.abi},{"state_restored",restored()},{"fp_state_restored",CanonicalRestored()}};RestoreCanonical();return value;};
  const auto original=run(false),candidate=run(true);bool equal=original==candidate;for(auto k:{"route_ok","fixture_legal","graph_legal","contract_ok","readonly_entity_tail_ok","stack_restored","registers_preserved","state_restored","fp_state_restored"})equal&=original[k]==true&&candidate[k]==true;passed&=equal;failures+=!equal;++coverage[kind];Write({{"event","comparison"},{"pair",pair++},{"case",scenario},{"function",fn==0?"CMatrixLinkList::AddToList1":fn==1?"CPlaceable::AllocateMatrix":"CMatrixLinkList::AddToList2"},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
 }
 const bool covered=fn!=1?coverage[0]==1024&&coverage[1]==1024:coverage[0]==683&&coverage[1]==683&&coverage[2]==682;passed&=covered;Write({{"event","function_summary"},{"function",fn==0?"CMatrixLinkList::AddToList1":fn==1?"CPlaceable::AllocateMatrix":"CMatrixLinkList::AddToList2"},{"cases",2048},{"failures",failures},{"fixture_counts",coverage},{"complete_paths",covered}});
 }
 bool rh=true;for(auto& r:routes){const bool x=r->Restore();rh&=x;}restore();RestoreCanonical();const bool state=rh&&restored()&&CanonicalRestored();Write({{"event",passed&&state?"complete":"error"},{"pairs",6144},{"state_restored",state},{"resources_restored",restored()},{"fp_state_restored",CanonicalRestored()}});
}
}
