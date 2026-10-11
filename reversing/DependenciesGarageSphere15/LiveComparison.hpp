#pragma once
#include <fstream>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Garage.h"
#include "Collision/CollisionData.h"
#include "Collision/ColModel.h"
#include "Pools/Pools.h"
#include "Pools/VehiclePool.h"
#include "../DependenciesWorld15/MatrixPoolProof.hpp"
#include "NativeProof.hpp"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesGarageSphere15Live {
inline bool done{};inline uint64 waitingSince{};inline std::ofstream log;
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
static_assert(offsetof(CGarage,m_nTimeToOpen)==0x3C);
static_assert(offsetof(CMatrixLinkList,m_freeListHead)==0x150&&offsetof(CMatrixLink,m_pNext)==0x50&&offsetof(CMatrixLinkList,m_freeListTail)==0x1A4&&offsetof(CMatrixLink,m_pPrev)==0x4C);
inline constexpr Fn fns[]{{"CGarage","EntityHasASphereWayOutsideGarage",0x449050,0,-1,2048}};
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
struct ABI {uint32 eax;bool stack,nv,seh;};
inline void Put(void* p,uintptr_t v){memcpy(p,&v,4);}
struct CallResult{uint32 eax;bool stack,nv,seh,top;};
inline CallResult Call(uintptr_t entry,CGarage* garage,CEntity* entity,float radius){uint32 a{},sp0{},sp1{},b0{},b1{},s0{},s1{},d0{},d1{},p0{},p1{},h0{},h1{};uint16 t0{},t1{};
 __asm{
  fnstsw t0
  mov sp0,esp
  mov b0,ebx
  mov s0,esi
  mov d0,edi
  mov p0,ebp
  mov eax,fs:[0]
  mov h0,eax
  push radius
  push entity
  mov ecx,garage
  mov eax,0A5123456h
  mov edx,0C0DE5921h
  call entry
  mov a,eax
  mov sp1,esp
  mov b1,ebx
  mov s1,esi
  mov d1,edi
  mov p1,ebp
  mov eax,fs:[0]
  mov h1,eax
  fnstsw t1
 }
 return {a,sp0==sp1,b0==b1&&s0==s1&&d0==d1&&p0==p1,h0==h1,(t0&0x3800)==(t1&0x3800)};
}
inline void RemoveReal(CEntity* entity){uintptr_t at=0x54F3B0;__asm{mov ecx,entity}__asm{call at}}
inline float RoundedAdd(float a,float b){volatile float r=a+b;return r;}
inline float RoundedMul(float a,float b){volatile float r=a*b;return r;}
inline CVector TransformOracle(const CMatrix* m,const CVector& p){const auto* a=reinterpret_cast<const float*>(m);
 return {RoundedAdd(RoundedAdd(RoundedAdd(RoundedMul(a[8],p.z),RoundedMul(a[4],p.y)),RoundedMul(a[0],p.x)),a[12]),
 RoundedAdd(RoundedAdd(RoundedAdd(RoundedMul(a[9],p.z),RoundedMul(a[1],p.x)),RoundedMul(a[5],p.y)),a[13]),
 RoundedAdd(RoundedAdd(RoundedAdd(RoundedMul(a[10],p.z),RoundedMul(a[2],p.x)),RoundedMul(a[6],p.y)),a[14])};}
inline bool InsideOracle(const CGarage* g,const CVector& p,float r){const auto* a=reinterpret_cast<const float*>(g);
 const auto dx=RoundedAdd(p.x,-a[0]),dy=RoundedAdd(p.y,-a[1]);
 const auto dotA=RoundedAdd(RoundedMul(dx,a[3]),RoundedMul(dy,a[4]));
 const auto dotB=RoundedAdd(RoundedMul(dx,a[5]),RoundedMul(dy,a[6]));
 return p.z>=RoundedAdd(a[2],-r)&&p.z<=RoundedAdd(a[7],r)&&dotA>=-r&&dotA<=RoundedAdd(r,a[8])&&dotB>=-r&&dotB<=RoundedAdd(r,a[9]);}

inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"dependenciesgaragesphere15-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependenciesgaragesphere15-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesgaragesphere15");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
 CVehicle* vehicle{};CColModel* model{};CCollisionData* nativeData{};
 if(auto* pool=GetVehiclePool())for(unsigned i=0;i<unsigned(pool->GetSize());++i){auto* v=pool->GetAt(i);if(!v||!v->m_matrix||v->m_matrix->m_pOwner!=v)continue;auto* m=v->GetColModel();if(m&&m->m_pColData&&m->m_pColData->m_nNumSpheres>0&&m->m_pColData->m_nNumSpheres<=128&&m->m_pColData->m_pSpheres){vehicle=v;model=m;nativeData=m->m_pColData;break;}}
 if(!vehicle||FindPlayerPed()->GetAreaCode()!=AREA_CODE_NORMAL_WORLD){if(!waitingSince)waitingSince=GetTickCount64();if(GetTickCount64()-waitingSince>60000){done=true;Write({{"event","error"},{"reason","no exterior real vehicle with loaded sphere geometry"},{"pairs",0},{"state_restored",true}});}return;}done=true;
 const auto fp=CaptureFP();const uint16 controlWord=uint16(fp.state[0]|uint16(fp.state[1])<<8);writeFP=fp;writeFPActive=true;pumpStateOK=true;PumpObserverGuard pumpGuard;
 DependenciesWorld15Matrix::Snapshot poolSaved;auto* originalMatrix=vehicle->m_matrix;const auto vehicleSaved=ReversingProbe::Bytes(vehicle,sizeof(CVehicle));const auto modelSaved=ReversingProbe::Bytes(model,sizeof(CColModel));const auto dataSaved=ReversingProbe::Bytes(nativeData,sizeof(CCollisionData));const auto spheresSaved=ReversingProbe::Bytes(nativeData->m_pSpheres,nativeData->m_nNumSpheres*sizeof(CColSphere));
 auto restore=[&]{memcpy(model,modelSaved.data(),modelSaved.size());poolSaved.Restore();memcpy(vehicle,vehicleSaved.data(),vehicleSaved.size());RestoreFP(fp);};
 auto nativeGeometryIntact=[&]{return dataSaved==ReversingProbe::Bytes(nativeData,sizeof(CCollisionData))&&spheresSaved==ReversingProbe::Bytes(nativeData->m_pSpheres,spheresSaved.size());};
 auto restored=[&]{return poolSaved.Equal()&&vehicleSaved==ReversingProbe::Bytes(vehicle,sizeof(CVehicle))&&modelSaved==ReversingProbe::Bytes(model,sizeof(CColModel))&&nativeGeometryIntact()&&CaptureFP()==fp;};
 OriginalDependencies499::HookRoute route("CGarage","EntityHasASphereWayOutsideGarage",0x449050,FunctionToVoidPtr(&CGarage::EntityHasASphereWayOutsideGarage)),allocate("CPlaceable","AllocateMatrix",0x54F560,FunctionToVoidPtr(&CPlaceable::AllocateMatrix)),remove("CPlaceable","RemoveMatrix",0x54F3B0,FunctionToVoidPtr(&CPlaceable::RemoveMatrix));
 const auto mainOff=ReversingProbe::Bytes(reinterpret_cast<void*>(0x449050),64);
 std::vector<uint8> allocateOn;allocate.Invoke(true,[&]{allocateOn=ReversingProbe::Bytes(reinterpret_cast<void*>(allocate.Body()),64);return 0;});const auto allocateOff=ReversingProbe::Bytes(reinterpret_cast<void*>(0x54F560),64);
 std::vector<Helper> helpers;for(auto at:{0x535300u,0x54F1B0u,0x54E9D0u,0x59C890u,0x4487D0u,0x59AF40u,0x59B020u,0x40F030u,0x4470E0u,0x447110u,0x4EF660u})helpers.emplace_back(at);
 auto helpersOK=[&]{return std::ranges::all_of(helpers,[](const auto& h){return h.Check();});};
 auto nativeOK=[&]{return Resolve(0x59C890)==0x59C890&&ModuleOf(0x59C890)==GetModuleHandleA(nullptr)&&std::equal(nativeTransform.begin(),nativeTransform.end(),reinterpret_cast<const uint8*>(0x59C890))
 &&Resolve(0x54F1B0)==0x54F1B0&&ModuleOf(0x54F1B0)==GetModuleHandleA(nullptr)&&std::equal(nativeUpdateSimple.begin(),nativeUpdateSimple.end(),reinterpret_cast<const uint8*>(0x54F1B0));};
 auto removeOK=[&]{return Resolve(0x54F3B0)==0x54F3B0&&ModuleOf(0x54F3B0)==GetModuleHandleA(nullptr)&&std::equal(nativeRemove.begin(),nativeRemove.end(),reinterpret_cast<const uint8*>(0x54F3B0));};
 auto delta=[&]{json nodes=json::array();for(unsigned i=0;i<poolSaved.nodes.size();++i){const auto& n=poolSaved.nodes[i];const auto actual=ReversingProbe::Bytes(n.first,sizeof(CMatrixLink));if(actual!=n.second)nodes.push_back({{"index",i},{"bytes",actual}});}return json{{"header",ReversingProbe::Bytes(&gMatrixList,sizeof(gMatrixList))},{"changed_nodes",nodes}};};
 auto routeProof=[&](bool candidate){return ReversingProbe::JumpsToDll(0x449050)==candidate&&(candidate?(Resolve(0x449050)==route.Body()&&ModuleOf(route.Body())==ModuleOf(uintptr_t(&ModuleMarker))):(Resolve(0x449050)==0x449050&&ModuleOf(0x449050)==GetModuleHandleA(nullptr)&&mainOff==ReversingProbe::Bytes(reinterpret_cast<void*>(0x449050),64)));};
 auto allocateProof=[&](bool candidate){return ReversingProbe::JumpsToDll(0x54F560)==candidate&&(candidate?(Resolve(0x54F560)==allocate.Body()&&ModuleOf(allocate.Body())==ModuleOf(uintptr_t(&ModuleMarker))&&allocateOn==ReversingProbe::Bytes(reinterpret_cast<void*>(allocate.Body()),64)):(Resolve(0x54F560)==0x54F560&&ModuleOf(0x54F560)==GetModuleHandleA(nullptr)&&allocateOff==ReversingProbe::Bytes(reinterpret_cast<void*>(0x54F560),64)));};
 const bool targetRoutesReady=route.Invoke(true,[&]{return routeProof(true);})&&routeProof(false)&&allocate.Invoke(true,[&]{return allocateProof(true);})&&allocateProof(false);
 bool ready=(controlWord&0x0F00)==0&&(fp.mxcsr&0x6000)==0&&targetRoutesReady&&poolSaved.legal&&poolSaved.Count()>1&&!gMatrixList.IsFull()&&route.Registered()&&allocate.Registered()&&remove.Registered()&&helpersOK()&&nativeOK()&&nativeGeometryIntact();
 const bool removeReady=remove.Invoke(false,[&]{return removeOK();});ready&=removeReady;
 pumpStateCheck=[&]{return restored()&&helpersOK()&&nativeOK();};json helperRows=json::array();for(const auto& h:helpers)helperRows.push_back(h.Metadata());json baselineNodes=json::array();for(const auto& n:poolSaved.nodes)baselineNodes.push_back(n.second);
 RestoreFP(fp);Write({{"event","begin"},{"registered_hook",route.Registered()},{"allocate_registered",allocate.Registered()},{"remove_registered",remove.Registered()},{"ready",ready},{"helper_routes",helperRows},{"helper_routes_ok",helpersOK()},{"native_helpers_ok",nativeOK()},{"remove_route_ok",removeReady},{"matrix_pool_legal",poolSaved.legal},{"matrix_pool_nodes",poolSaved.Count()},{"matrix_pool_object_base",uintptr_t(gMatrixList.m_pObjects)},{"matrix_pool_baseline_header",poolSaved.header},{"matrix_pool_baseline_nodes",baselineNodes},{"native_transform_bytes",nativeTransform},{"native_update_simple_bytes",nativeUpdateSimple},{"remove_off_bytes",nativeRemove},{"native_sphere_count",nativeData->m_nNumSpheres},{"native_spheres",spheresSaved},{"target_routes_ready",targetRoutesReady},{"main_body",route.Body()},{"main_off_bytes",mainOff},{"main_candidate_module",uintptr_t(ModuleOf(route.Body()))},{"probe_module",uintptr_t(ModuleOf(uintptr_t(&ModuleMarker)))},{"allocate_candidate_module",uintptr_t(ModuleOf(allocate.Body()))},{"allocate_body",allocate.Body()},{"allocate_on_bytes",allocateOn},{"allocate_off_bytes",allocateOff},{"scope","real vehicle/native geometry and private valid geometry; allocation helper OFF/ON auxiliary, no extra cohort credit"},{"fixture_configuration","FastLoader.SaveGameToLoad=0"}});
 if(!ready){restore();const bool memory=restored();const bool r=route.Restore();const bool a=allocate.Restore();const bool d=remove.Restore();Write({{"event","error"},{"reason","invalid fixture/native resources"},{"pairs",0},{"state_restored",memory&&r&&a&&d}});return;}
 alignas(16) std::array<uint8,16+sizeof(CGarage)+16> garageArena{};alignas(16) std::array<uint8,16+sizeof(CCollisionData)+16> dataArena{};alignas(16) std::array<uint8,16+8*sizeof(CColSphere)+16> sphereArena{};
 garageArena.fill(0x93);dataArena.fill(0xA5);sphereArena.fill(0xB7);const auto garageClean=garageArena;const auto dataClean=dataArena;const auto sphereClean=sphereArena;
 auto* garage=reinterpret_cast<CGarage*>(garageArena.data()+16);auto* ownData=reinterpret_cast<CCollisionData*>(dataArena.data()+16);auto* ownSpheres=reinterpret_cast<CColSphere*>(sphereArena.data()+16);
 auto guards=[&]{auto edge=[](const auto& a,const auto& b){return std::equal(a.begin(),a.begin()+16,b.begin())&&std::equal(a.end()-16,a.end(),b.end()-16);};return edge(garageArena,garageClean)&&edge(dataArena,dataClean)&&edge(sphereArena,sphereClean);};
 bool passed=true;unsigned failures=0;std::set<unsigned> profiles;std::array<unsigned,2> domains{},representations{};
 for(unsigned scenario=0;scenario<2048;++scenario){const bool native=scenario>=1536;const unsigned n=native?scenario-1536:scenario;const unsigned geometry=n%4,mode=(n/8)%8,radIndex=(n/64)%4;const bool matrixPresent=((n/4)&1)!=0;const float extra=std::array<float,4>{0.f,.25f,1.f,4.f}[radIndex];
  auto invoke=[&](bool candidate){restore();garageArena=garageClean;dataArena=dataClean;sphereArena=sphereClean;memset(garage,0,sizeof(CGarage));reinterpret_cast<void(__thiscall*)(void*)>(0x4470E0)(garage);garage->m_nTimeToOpen=scenario;memset(ownData,0,sizeof(CCollisionData));reinterpret_cast<void(__thiscall*)(void*)>(0x40F030)(ownData);
   const auto pos=originalMatrix->GetPosition();originalMatrix->SetUnity();originalMatrix->GetPosition()=pos;auto* g=reinterpret_cast<float*>(garage);g[0]=pos.x-5.f;g[1]=pos.y-5.f;g[2]=pos.z-5.f;g[3]=1.f;g[4]=0.f;g[5]=0.f;g[6]=1.f;g[7]=pos.z+5.f;g[8]=10.f;g[9]=10.f;
   const float width=native?std::array<float,4>{.125f,1.f,8.f,128.f}[geometry]:5.f;if(native){g[0]=pos.x-width;g[1]=pos.y-width;g[2]=pos.z-width;g[7]=pos.z+width;g[8]=width*2;g[9]=width*2;if(mode&1)g[0]+=128.f;if(mode&2)g[1]-=128.f;if(mode&4){g[2]+=128.f;g[7]+=128.f;}}
   if(!native){ownData->m_nNumSpheres=std::array<uint16,4>{0,1,4,8}[geometry];ownData->m_pSpheres=ownData->m_nNumSpheres?ownSpheres:nullptr;for(unsigned i=0;i<ownData->m_nNumSpheres;++i){memset(&ownSpheres[i],0,sizeof(CColSphere));ownSpheres[i].m_fRadius=.5f;ownSpheres[i].m_vecCenter=CVector{float(i%3)-1.f,0.f,0.f};
     const bool select=mode==1?i==0:mode==2?i+1==ownData->m_nNumSpheres:mode==3?i==ownData->m_nNumSpheres/2:mode==4||mode>=5?i==0:false;if(select){if(mode==3)ownSpheres[i].m_vecCenter.y=-32.f;else if(mode==4)ownSpheres[i].m_vecCenter.z=32.f;else if(mode>=5)ownSpheres[i].m_vecCenter.x=RoundedAdd(5.f,RoundedAdd(extra,.5f))+(mode==5?0.f:mode==6?-.125f:.125f);else ownSpheres[i].m_vecCenter.x=32.f;}}
    model->m_pColData=ownData;
   }
   const auto inputGarage=ReversingProbe::Bytes(garage,sizeof(CGarage));const auto inputModel=ReversingProbe::Bytes(model,sizeof(CColModel));const auto inputData=ReversingProbe::Bytes(model->m_pColData,sizeof(CCollisionData));const auto inputSpheres=model->m_pColData->m_nNumSpheres?ReversingProbe::Bytes(model->m_pColData->m_pSpheres,model->m_pColData->m_nNumSpheres*sizeof(CColSphere)):std::vector<uint8>{};
   bool rm=remove.Invoke(false,[&]{const bool before=removeOK();if(!matrixPresent)RemoveReal(vehicle);return before&&removeOK();});auto* expectedAllocated=gMatrixList.m_freeListHead.m_pNext;const auto fixtureVehicle=ReversingProbe::Bytes(vehicle,sizeof(CVehicle));DependenciesWorld15Matrix::Snapshot fixturePool;
   bool routeOK=false,allocationRouteOK=false;const auto result=allocate.Invoke(candidate,[&]{auto proof=[&]{return allocateProof(candidate);};allocationRouteOK=proof();const auto r=route.Invoke(candidate,[&]{routeOK=routeProof(candidate);RestoreFP(fp);const auto value=Call(0x449050,garage,vehicle,extra);routeOK&=routeProof(candidate);return value;});allocationRouteOK&=proof();return r;});
   const unsigned count=model->m_pColData->m_nNumSpheres;const bool allocated=!matrixPresent&&count>0;const bool representation=count==0&&!matrixPresent?vehicle->m_matrix==nullptr:vehicle->m_matrix!=nullptr&&vehicle->m_matrix->m_pOwner==vehicle&&vehicle->m_matrix==(allocated?expectedAllocated:originalMatrix);
   bool expected=false;unsigned firstOutside=count;json transformed=json::array();if(count&&vehicle->m_matrix)for(unsigned i=0;i<count;++i){const auto& sphere=model->m_pColData->m_pSpheres[i];const auto point=TransformOracle(vehicle->m_matrix,sphere.m_vecCenter);const auto r=RoundedAdd(extra,sphere.m_fRadius);const bool inside=InsideOracle(garage,point,r);transformed.push_back({{"point_bits",{std::bit_cast<uint32>(point.x),std::bit_cast<uint32>(point.y),std::bit_cast<uint32>(point.z)}},{"radius_bits",std::bit_cast<uint32>(r)},{"inside",inside}});if(!inside&&firstOutside==count){firstOutside=i;expected=true;}}
   auto expectedVehicle=fixtureVehicle;if(allocated)Put(expectedVehicle.data()+0x14,uintptr_t(expectedAllocated));const bool contract=uint8(result.eax)==uint8(expected)&&expectedVehicle==ReversingProbe::Bytes(vehicle,sizeof(CVehicle))&&((allocated)||fixturePool.Equal());const bool readonly=inputGarage==ReversingProbe::Bytes(garage,sizeof(CGarage))&&inputModel==ReversingProbe::Bytes(model,sizeof(CColModel))&&inputData==ReversingProbe::Bytes(model->m_pColData,sizeof(CCollisionData))&&inputSpheres==(count?ReversingProbe::Bytes(model->m_pColData->m_pSpheres,inputSpheres.size()):std::vector<uint8>{})&&nativeGeometryIntact();
   DependenciesWorld15Matrix::Snapshot finalPool;const auto poolState=delta();const int matrixIndex=vehicle->m_matrix?int((uintptr_t(vehicle->m_matrix)-uintptr_t(gMatrixList.m_pObjects))/sizeof(CMatrixLink)):-1;const auto matrixPayload=vehicle->m_matrix?ReversingProbe::Bytes(vehicle->m_matrix,sizeof(CMatrixLink)):std::vector<uint8>{};const auto vehicleState=ReversingProbe::Bytes(vehicle,sizeof(CVehicle));const bool helper=helpersOK()&&nativeOK();const bool guard=guards();reinterpret_cast<void(__thiscall*)(void*)>(0x447110)(garage);const bool cleanupHelper=helpersOK();
   restore();RestoreFP(fp);return json{{"x87_control",controlWord},{"native_geometry",native},{"geometry_index",geometry},{"mode",mode},{"matrix_present",matrixPresent},{"extra_radius_bits",std::bit_cast<uint32>(extra)},{"input_garage",inputGarage},{"input_data",inputData},{"input_spheres",inputSpheres},{"sphere_count",count},{"first_outside",firstOutside},{"transformed",transformed},{"return_al",uint8(result.eax)},{"matrix_index",matrixIndex},{"matrix_payload",matrixPayload},{"vehicle_state",vehicleState},{"matrix_pool_delta",poolState},{"route_ok",routeOK},{"allocate_route_ok",allocationRouteOK},{"remove_route_ok",rm},{"helper_route_ok",helper&&cleanupHelper},{"representation_legal",representation&&finalPool.legal},{"stack_restored",result.stack},{"nonvolatile_preserved",result.nv},{"seh_restored",result.seh},{"fpu_top_balanced",result.top},{"fixture_contract",contract},{"readonly_ok",readonly},{"guards_preserved",guard},{"state_restored",restored()},{"floating_state_restored",CaptureFP()==fp}};
  };
  const auto original=invoke(false);const auto candidate=invoke(true);bool equal=original==candidate;for(auto k:{"route_ok","allocate_route_ok","remove_route_ok","helper_route_ok","representation_legal","stack_restored","nonvolatile_preserved","seh_restored","fpu_top_balanced","fixture_contract","readonly_ok","guards_preserved","state_restored","floating_state_restored"})equal&=original[k]==true&&candidate[k]==true;passed&=equal;failures+=!equal;++domains[native];++representations[matrixPresent];profiles.insert((unsigned(native)<<8)|(geometry<<5)|(mode<<2)|(unsigned(matrixPresent)<<1));Write({{"event","comparison"},{"function","CGarage::EntityHasASphereWayOutsideGarage"},{"pair",scenario},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
 }
 Write({{"event","function_summary"},{"function","CGarage::EntityHasASphereWayOutsideGarage"},{"cases",2048},{"failures",failures},{"domain_cases",domains},{"representation_cases",representations}});
 restore();const bool memory=restored()&&helpersOK()&&nativeOK();const bool r=route.Restore();const bool a=allocate.Restore();const bool d=remove.Restore();const bool final=memory&&r&&a&&d&&pumpStateOK;Write({{"event",passed&&final?"complete":"error"},{"pairs",2048},{"state_restored",final},{"floating_state_restored",CaptureFP()==fp},{"helper_route_ok",helpersOK()}});
}
}
