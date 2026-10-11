#pragma once
#include <fstream>
#include "Cam.h"
#include "CarCtrl.h"
#include "cHandlingDataMgr.h"
#include "Pools/Pools.h"
#include "Models/VehicleModelInfo.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesWorld10Live {
struct Fn{const char* cls;const char* name;uintptr_t address,vtable;int slot,cases;};
inline constexpr Fn fns[]{
{"CCam","GetBoatHeightForFirstPerson",0x509CA0,0,-1,2048},
{"CCarCtrl","Native_00424100",0x424100,0,-1,2048}
};
inline bool done{};inline std::ofstream log;inline uint64 resourcesWaitingSince{},resourcesLastDiagnostic{};
inline void Write(const json& r);
struct Result{uint32 al;bool abi;};
__declspec(noinline) inline Result Call(uintptr_t entry,uintptr_t camera,uintptr_t output){
 uint32 before{},after{},rb{},rs{},rd{},fb{},fa{},returned{};
 __asm {
  push ebx
  push esi
  push edi
  mov before,esp
  mov fb,ebp
  mov ebx,13579BDFh
  mov esi,2468ACE0h
  mov edi,10293847h
  push output
  mov ecx,camera
  call entry
  mov returned,eax
  mov after,esp
  mov fa,ebp
  mov rb,ebx
  mov rs,esi
  mov rd,edi
  pop edi
  pop esi
  pop ebx
 }
 return {returned&255,before==after&&fb==fa&&rb==0x13579BDF&&rs==0x2468ACE0&&rd==0x10293847};
}
struct ConstantResult{uint32 eax,xAfter,yAfter;bool abi;};
__declspec(noinline) inline ConstantResult CallConstant(uintptr_t entry,uint32 x,uint32 y,uint32 seed){
 uint32 before{},after{},rb{},rs{},rd{},fb{},fa{},eaxCaptured{},xAfter{},yAfter{};
 __asm {
  push ebx
  push esi
  push edi
  mov before,esp
  mov fb,ebp
  mov ebx,13579BDFh
  mov esi,2468ACE0h
  mov edi,10293847h
  push y
  push x
  mov eax,seed
  call entry
  mov eaxCaptured,eax
  mov edx,dword ptr [esp]
  mov xAfter,edx
  mov edx,dword ptr [esp+4]
  mov yAfter,edx
  add esp,8
  mov after,esp
  mov fa,ebp
  mov rb,ebx
  mov rs,esi
  mov rd,edi
  pop edi
  pop esi
  pop ebx
 }
 return {eaxCaptured,xAfter,yAfter,before==after&&fb==fa&&rb==0x13579BDF&&rs==0x2468ACE0&&rd==0x10293847};
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
inline bool ProbeModule(uintptr_t e){HMODULE owner{},probe{};const DWORD flags=GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;return GetModuleHandleExA(flags,reinterpret_cast<LPCSTR>(e),&owner)&&GetModuleHandleExA(flags,reinterpret_cast<LPCSTR>(&Call),&probe)&&owner==probe;}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependenciesworld10-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependenciesworld10-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesworld10");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;SaveFP(canonicalFP.data());canonicalMXCSR=ReadMXCSR();canonicalActive=true;
 static_assert(sizeof(CCam)==0x238);static_assert(sizeof(CVehicleModelInfo)==0x308);static_assert(sizeof(cHandlingDataMgr)==0xC624);
 const auto cameraSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB6F19C),sizeof(CCam));const auto handlingSaved=ReversingProbe::Bytes(&gHandlingDataMgr,sizeof(gHandlingDataMgr));
 std::vector<CVehicle*> vehicles;uint32 used{},missingInfo{},invalidModels{};std::array<uint32,32> modelTypes{};auto* pool=GetVehiclePool();if(pool)for(uint32 i=0;i<pool->GetSize();++i){auto* v=pool->GetAt(i);if(!v)continue;++used;if(v->m_nModelIndex<0||v->m_nModelIndex>=20000){++invalidModels;continue;}auto* info=CModelInfo::GetModelInfo(v->m_nModelIndex);if(!info){++missingInfo;continue;}const auto type=uint32(info->GetModelType());if(type<modelTypes.size())++modelTypes[type];if(type==uint32(MODEL_INFO_VEHICLE))vehicles.push_back(v);}
 if(vehicles.empty()||FindPlayerPed()->GetAreaCode()!=AREA_CODE_NORMAL_WORLD){const uint64 now=GetTickCount64();if(!resourcesWaitingSince)resourcesWaitingSince=now;const bool expired=now-resourcesWaitingSince>=60000;if(expired||!resourcesLastDiagnostic||now-resourcesLastDiagnostic>=5000){resourcesLastDiagnostic=now;const auto& position=FindPlayerPed()->GetPosition();json diagnostic={{"event",expired?"error":"waiting_for_real_vehicles"},{"reason","Waiting for naturally loaded real vehicles/model metadata"},{"elapsed_ms",now-resourcesWaitingSince},{"pool_capacity",pool?pool->GetSize():0},{"pool_used",used},{"missing_model_info",missingInfo},{"invalid_model_indices",invalidModels},{"model_type_counts",modelTypes},{"player_position",{position.x,position.y,position.z}},{"player_area",int(FindPlayerPed()->GetAreaCode())}};RestoreCanonical();diagnostic["fp_state_restored"]=CanonicalRestored();diagnostic["state_restored"]=CanonicalRestored();Write(diagnostic);}RestoreCanonical();canonicalActive=false;done=expired;return;}
 done=true;
 const uintptr_t helperPointer=std::bit_cast<uintptr_t>(&cHandlingDataMgr::GetBoatPointer);
 OriginalDependencies499::HookRoute route("CCam","GetBoatHeightForFirstPerson",0x509CA0,FunctionToVoidPtr(&CCam::GetBoatHeightForFirstPerson));
 OriginalDependencies499::HookRoute constant("CCarCtrl","Native_00424100",0x424100,&Native_00424100);
 OriginalDependencies499::HookRoute helper("cHandlingDataMgr","GetBoatPointer",0x6F5300,FunctionToVoidPtr(&cHandlingDataMgr::GetBoatPointer));
 const auto helperOff=ReversingProbe::Bytes(reinterpret_cast<void*>(0x6F5300),32);std::vector<uint8>helperOn;
 const bool helperReady=helper.Invoke(true,[&]{helperOn=ReversingProbe::Bytes(reinterpret_cast<void*>(helper.Body()),32);return helper.Registered()&&Resolve(helperPointer)==helper.Body()&&ProbeModule(helper.Body())&&ReversingProbe::JumpsToDll(0x6F5300);});
 auto readonly=[&]{return cameraSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB6F19C),sizeof(CCam))&&handlingSaved==ReversingProbe::Bytes(&gHandlingDataMgr,sizeof(gHandlingDataMgr));};
 RestoreCanonical();Write({{"event","begin"},{"registered_hook",route.Registered()&&constant.Registered()},{"helper_registered",helper.Registered()},{"helper_route_ok",helperReady},{"helper_entry",0x6F5300},{"helper_body",helper.Body()},{"helper_on_bytes",helperOn},{"helper_off_bytes",helperOff},{"real_vehicle_count",vehicles.size()},{"player_area",int(FindPlayerPed()->GetAreaCode())},{"fixture_configuration","FastLoader.SaveGameToLoad=0"},{"collaborators_stubbed",false},{"fp_state_restored",CanonicalRestored()}});
 bool passed=helperReady;uint32 failures=0,nonnull=0,null=0;
 for(int scenario=0;scenario<2048;++scenario){
  auto* vehicle=vehicles[uint32(scenario/2)%vehicles.size()];auto* model=static_cast<CVehicleModelInfo*>(CModelInfo::GetModelInfo(vehicle->m_nModelIndex));const auto entityBefore=ReversingProbe::Bytes(vehicle,sizeof(CVehicle));const auto modelBefore=ReversingProbe::Bytes(model,sizeof(CVehicleModelInfo));const bool targetPresent=(scenario&1)!=0;
  auto run=[&](bool candidate){RestoreCanonical();alignas(4)std::array<uint8,sizeof(CCam)+8> camera{};camera.fill(0xA7);std::memcpy(camera.data()+4,cameraSaved.data(),sizeof(CCam));auto* cam=reinterpret_cast<CCam*>(camera.data()+4);cam->m_pCamTargetEntity=targetPresent?vehicle:nullptr;
   std::array<uint32,3> output{0x13579BDF,0x7FC12345,0x2468ACE0};const auto cameraBefore=camera;const uint8 id=uint8(model->m_nHandlingId);const auto* row=&gHandlingDataMgr.m_aBoatHandling[(id>=175&&id<=186)?id-175:0];uint32 expected=output[1];if(targetPresent)std::memcpy(&expected,reinterpret_cast<const uint8*>(row)+0x20,4);
   bool helperOK=false,routeOK=false;const auto result=helper.Invoke(candidate,[&]{auto check=[&]{return helper.Registered()&&ReversingProbe::JumpsToDll(0x6F5300)==candidate&&(candidate?(Resolve(helperPointer)==helper.Body()&&ProbeModule(helper.Body())&&helperOn==ReversingProbe::Bytes(reinterpret_cast<void*>(helper.Body()),32)):helperOff==ReversingProbe::Bytes(reinterpret_cast<void*>(0x6F5300),32));};helperOK=check();const auto r=route.Invoke(candidate,[&]{routeOK=ReversingProbe::JumpsToDll(0x509CA0)==candidate;if(candidate)routeOK&=Resolve(0x509CA0)==route.Body()&&ProbeModule(route.Body());return Call(0x509CA0,uintptr_t(cam),uintptr_t(&output[1]));});helperOK&=check();return r;});
   const bool contract=result.al==uint32(targetPresent)&&output[1]==expected;const bool ro=readonly()&&entityBefore==ReversingProbe::Bytes(vehicle,sizeof(CVehicle))&&modelBefore==ReversingProbe::Bytes(model,sizeof(CVehicleModelInfo));const bool guard=camera==cameraBefore&&output.front()==0x13579BDF&&output.back()==0x2468ACE0;
   RestoreCanonical();json value={{"return_bits",result.al},{"height_bits",output[1]},{"expected_height_bits",expected},{"target_present",targetPresent},{"model_id",vehicle->m_nModelIndex},{"handling_id",model->m_nHandlingId},{"route_ok",routeOK},{"helper_route_ok",helperOK},{"stack_restored",result.abi},{"registers_preserved",result.abi},{"contract_ok",contract},{"readonly_ok",ro},{"guards_ok",guard},{"fp_state_restored",CanonicalRestored()}};RestoreCanonical();return value;
  };
  const auto original=run(false),candidate=run(true);bool equal=route.Registered()&&original==candidate;for(auto k:{"route_ok","helper_route_ok","stack_restored","registers_preserved","contract_ok","readonly_ok","guards_ok","fp_state_restored"})equal&=original[k]==true;
  failures+=!equal;passed&=equal;null+=!targetPresent;nonnull+=targetPresent;Write({{"event","comparison"},{"pair",scenario},{"case",scenario},{"function","CCam::GetBoatHeightForFirstPerson"},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
 }
 const bool covered=null&&nonnull;passed&=covered;Write({{"event","function_summary"},{"function","CCam::GetBoatHeightForFirstPerson"},{"cases",2048},{"failures",failures},{"complete_paths",covered},{"null_targets",null},{"real_targets",nonnull}});
 bool allSeeds=true;uint32 constantFailures=0;
 const uint32 seeds[]{0,0xFF,0x13579BDF,0x2468ACE0,0xFFFFFFFF,0x80000000,0x01020304,0x7FFFFFFF};
 for(int scenario=0;scenario<2048;++scenario){auto* v=vehicles[uint32(scenario/8)%vehicles.size()];const auto entityBefore=ReversingProbe::Bytes(v,sizeof(CVehicle));const auto& pos=v->GetPosition();const uint32 x=std::bit_cast<uint32>(pos.x),y=std::bit_cast<uint32>(pos.y),seed=seeds[scenario%8];
  auto run=[&](bool candidate){RestoreCanonical();bool routeOK=false;const auto r=constant.Invoke(candidate,[&]{routeOK=ReversingProbe::JumpsToDll(0x424100)==candidate;if(candidate)routeOK&=Resolve(0x424100)==constant.Body()&&ProbeModule(constant.Body());return CallConstant(0x424100,x,y,seed);});const bool contract=r.eax==(seed&0xFFFFFF00u)&&r.xAfter==x&&r.yAfter==y;const bool ro=readonly()&&entityBefore==ReversingProbe::Bytes(v,sizeof(CVehicle));RestoreCanonical();json value={{"return_bits",r.eax},{"al",r.eax&255},{"eax_seed",seed},{"x_bits",x},{"y_bits",y},{"x_after",r.xAfter},{"y_after",r.yAfter},{"route_ok",routeOK},{"helper_route_ok",true},{"stack_restored",r.abi},{"registers_preserved",r.abi},{"contract_ok",contract},{"readonly_ok",ro},{"guards_ok",r.xAfter==x&&r.yAfter==y},{"fp_state_restored",CanonicalRestored()}};RestoreCanonical();return value;};
  const auto original=run(false),candidate=run(true);bool equal=constant.Registered()&&original==candidate;for(auto k:{"route_ok","helper_route_ok","stack_restored","registers_preserved","contract_ok","readonly_ok","guards_ok","fp_state_restored"})equal&=original[k]==true;passed&=equal;constantFailures+=!equal;allSeeds&=original["contract_ok"]==true;Write({{"event","comparison"},{"pair",2048+scenario},{"case",scenario},{"function","CCarCtrl::Native_00424100"},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
 }
 Write({{"event","function_summary"},{"function","CCarCtrl::Native_00424100"},{"cases",2048},{"failures",constantFailures},{"complete_paths",allSeeds}});
 const bool mainRestored=route.Restore(),helperRestored=helper.Restore(),constantRestored=constant.Restore();const bool hooks=mainRestored&&helperRestored&&constantRestored;RestoreCanonical();const bool fp=CanonicalRestored(),restored=readonly()&&hooks&&fp;Write({{"event",passed&&restored?"complete":"error"},{"pairs",4096},{"state_restored",restored},{"fp_state_restored",fp},{"helper_route_ok",hooks}});
}
}
