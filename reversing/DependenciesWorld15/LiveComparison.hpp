#pragma once
#include <fstream>
#include <set>
#include <memory>
#include "Garages.h"
#include "Garage.h"
#include "Models/ModelInfo.h"
#include "Models/BaseModelInfo.h"
#include "Collision/ColModel.h"
#include "Collision/CollisionData.h"
#include "Pools/Pools.h"
#include "Pools/VehiclePool.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Core/MatrixLinkList.h"
#include "SimpleTransform.h"
#include "NativeProof.hpp"
#include "MatrixPoolProof.hpp"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
// Garage.cpp: el hook del EXE 0x4487D0 es esta envoltura __fastcall (conserva ECX y EDX como el original); la sobrecarga CGarage::IsPointInsideGarage es la version para C++.
uint32 __fastcall GarageIsPointInsideRadiusRegs(CGarage* self, uint32 entryEdx, CVector point, float radius);
namespace DependenciesWorld15Live {
struct Fn{const char* cls;const char* name;uintptr_t address,vtable;int slot,cases;};
inline constexpr Fn fns[]{{"CGarage","IsEntityTouching3D",0x448EE0,0,-1,2048}};
inline bool done{};inline std::ofstream log;inline uint64 waitSince{},lastDiagnostic{};
inline void Write(const json&);
struct Result{uint32 eax;bool abi;};
__declspec(noinline) inline Result Call(uintptr_t entry,void* object,void* entity){
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
  push entity
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
inline uintptr_t CallThis(uintptr_t entry,void* object){uintptr_t result{};__asm{mov ecx,object}__asm{call entry}__asm{mov result,eax}return result;}
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

struct OwnerSnapshot {
 std::vector<std::pair<CPlaceable*,std::vector<uint8>>> owners;
 bool legal{true};
 explicit OwnerSnapshot(const DependenciesWorld15Matrix::Snapshot& pool,CVehicle* target){
  std::set<CPlaceable*> unique;for(const auto&[m,bytes]:pool.nodes){auto* owner=m->m_pOwner;if(owner){if(owner->m_matrix!=m||!unique.insert(owner).second)legal=false;}}
  unique.insert(target);for(auto* o:unique)owners.push_back({o,ReversingProbe::Bytes(o,sizeof(CPlaceable))});
 }
 void Restore()const{for(const auto&[at,bytes]:owners)std::memcpy(at,bytes.data(),bytes.size());}
 bool Equal()const{for(const auto&[at,bytes]:owners)if(bytes!=ReversingProbe::Bytes(at,bytes.size()))return false;return true;}
 json Current(CPlaceable* target)const{json out=json::array();for(const auto&[at,bytes]:owners){auto current=ReversingProbe::Bytes(at,bytes.size());if(at==target||current!=bytes)out.push_back({{"owner",uintptr_t(at)},{"bytes",current}});}return out;}
};

using MatrixSnapshot=DependenciesWorld15Matrix::Snapshot;
struct RouteSpec{const char* cls;const char* name;uintptr_t entry;void* candidate;};
template<size_t N> inline bool Native(uintptr_t entry,const std::array<uint8,N>& bytes){return Resolve(entry)==entry&&ModuleIs(entry,GetModuleHandleA(nullptr))&&std::equal(bytes.begin(),bytes.end(),reinterpret_cast<const uint8*>(entry));}
inline bool Libraries(){return Native(0x59C890,nativePoint)&&Native(0x54F1B0,nativeUpdateMatrix)&&Native(0x59AF40,nativeSetTranslate)&&Native(0x59AF80,nativeSetTranslateOnly)&&Native(0x59AEA0,nativeResetOrientation)&&Native(0x59B020,nativeSetRotateZOnly);}
inline CVector WorldPoint(const CMatrix& m,const CVector& p){CVector out;reinterpret_cast<CVector*(__cdecl*)(CVector*,const CMatrix*,const CVector*)>(0x59C890)(&out,&m,&p);return out;}
inline bool SphereHit(CGarage& g,CVector p,float radius){return reinterpret_cast<bool(__thiscall*)(CGarage*,CVector,float)>(0x4487D0)(&g,p,radius);}
inline CGarage Box(const CGarage& seed,CVector center,float halfX,float halfY,float halfZ,uint32 angle){CGarage g=seed;const float ca=angle==0?1.0f:angle==1?0.0f:angle==2?0.70710677f:0.8660254f,sa=angle==0?0.0f:angle==1?1.0f:angle==2?0.70710677f:-0.5f;
 g.m_vDirectionA=CVector2D{ca,sa};g.m_vDirectionB=CVector2D{-sa,ca};g.m_fWidth=2.0f*halfX;g.m_fHeight=2.0f*halfY;g.m_vPosn=CVector{center.x-ca*halfX+sa*halfY,center.y-sa*halfX-ca*halfY,center.z-halfZ};g.m_fTopZ=center.z+halfZ;
 const float ex=std::abs(ca)*halfX+std::abs(sa)*halfY,ey=std::abs(sa)*halfX+std::abs(ca)*halfY;g.m_fLeftCoord=center.x-ex;g.m_fRightCoord=center.x+ex;g.m_fFrontCoord=center.y-ey;g.m_fBackCoord=center.y+ey;return g;}
// Extended double subtraction of float operands is exact; strict profiles avoid rounded boundary ambiguity.
inline uint32 Broad(const CGarage& g,const CVector& p,float r){if(double(g.m_fLeftCoord)-r>p.x)return 0;if(double(g.m_fRightCoord)+r<p.x)return 1;if(double(g.m_fFrontCoord)-r>p.y)return 2;if(double(g.m_fBackCoord)+r<p.y)return 3;if(double(g.m_vPosn.z)-r>p.z)return 4;if(double(g.m_fTopZ)+r<p.z)return 5;return 6;}
struct Fixture{CGarage garage;uint32 kind,angle;int32 hit;};
inline int32 FirstSphere(CGarage& g,const std::vector<CVector>& centers,CCollisionData* data){for(uint32 i=0;i<centers.size();++i)if(SphereHit(g,centers[i],data->m_pSpheres[i].m_fRadius))return int32(i);return -1;}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"dependenciesworld15-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependenciesworld15-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesworld15");}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;SaveFP(canonicalFP.data());canonicalMXCSR=ReadMXCSR();canonicalActive=true;
 auto wait=[&](size_t candidates,size_t nodes,const char* reason){const auto now=GetTickCount64();if(!waitSince)waitSince=now;const bool expired=now-waitSince>=60000;RestoreCanonical();if(expired||!lastDiagnostic||now-lastDiagnostic>=5000){lastDiagnostic=now;Write({{"event",expired?"error":"waiting_for_real_collision_resources"},{"reason",reason},{"actual_candidates",candidates},{"real_matrices",nodes},{"elapsed_ms",now-waitSince}});}RestoreCanonical();canonicalActive=false;if(expired)done=true;};
 MatrixSnapshot baseline;std::vector<CVehicle*> candidates;if(auto* p=GetVehiclePool())for(uint32 i=0;i<p->GetSize();++i){auto* v=p->GetAt(i);if(v&&v->m_matrix&&v->m_matrix->m_pOwner==v&&v->GetModelInfo()&&v->GetModelInfo()->GetColModel())candidates.push_back(v);}
 if(!baseline.legal||candidates.empty()||CGarages::NumGarages<=0||FindPlayerPed()->GetAreaCode()!=AREA_CODE_NORMAL_WORLD){wait(candidates.size(),baseline.Count(),"Real vehicle/matrix/garage unavailable");return;}
 const RouteSpec specs[]{
 {"CGarage","IsEntityTouching3D",0x448EE0,FunctionToVoidPtr(&CGarage::IsEntityTouching3D)},
 {"CEntity","GetColModel",0x535300,FunctionToVoidPtr(&CEntity::GetColModel)},
 {"CPlaceable","GetMatrix",0x411990,FunctionToVoidPtr(&CPlaceable::GetMatrix)},
 {"CGarage","IsPointInsideGarage-radius",0x4487D0,reinterpret_cast<void*>(&GarageIsPointInsideRadiusRegs)},
 {"CPlaceable","AllocateMatrix",0x54F560,FunctionToVoidPtr(&CPlaceable::AllocateMatrix)},
 {"CPlaceable","RemoveMatrix",0x54F3B0,FunctionToVoidPtr(&CPlaceable::RemoveMatrix)},
 {"CMatrixLinkList","AddToList1",0x54E9D0,FunctionToVoidPtr(&CMatrixLinkList::AddToList1)},
 {"CMatrixLinkList","MoveToList1",0x54EA60,FunctionToVoidPtr(&CMatrixLinkList::MoveToList1)},
 {"CMatrixLinkList","MoveToFreeList",0x54EAD0,FunctionToVoidPtr(&CMatrixLinkList::MoveToFreeList)},
 {"CMatrixLink","Insert",0x54E8F0,FunctionToVoidPtr(&CMatrixLink::Insert)},
 {"CMatrixLink","Remove",0x54E910,FunctionToVoidPtr(&CMatrixLink::Remove)}
 };
 std::vector<std::unique_ptr<OriginalDependencies499::HookRoute>> routes;std::vector<std::vector<uint8>> off,on;bool ready=true;
 for(const auto&s:specs){routes.push_back(std::make_unique<OriginalDependencies499::HookRoute>(s.cls,s.name,s.entry,s.candidate));off.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(s.entry),32));on.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(routes.back()->Body()),32));ready&=routes.back()->Registered();}
 auto switchAll=[&](bool candidate,auto&& fn){auto recurse=[&](auto&& self,size_t i)->void{if(i==routes.size()){fn();return;}routes[i]->Invoke(candidate,[&]{self(self,i+1);return 0;});};recurse(recurse,0);};
 auto routeProof=[&](bool candidate){bool ok=true;for(size_t i=0;i<routes.size();++i){auto&r=*routes[i];const auto&s=specs[i];ok&=r.Registered()&&ReversingProbe::JumpsToDll(s.entry)==candidate;if(candidate)ok&=Resolve(s.entry)==r.Body()&&Resolve(uintptr_t(s.candidate))==r.Body()&&ModuleIs(r.Body(),ProbeModule())&&on[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(r.Body()),32);else ok&=off[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(s.entry),32);}return ok;};
 auto restoreRoutes=[&]{bool ok=true;for(auto&r:routes){const bool x=r->Restore();ok&=x;}return ok;};switchAll(true,[&]{ready&=routeProof(true);});switchAll(false,[&]{ready&=routeProof(false)&&Libraries()&&Native(0x54F3B0,nativeRemoveMatrix);});
 const uintptr_t simpleBody=Resolve(uintptr_t(FunctionToVoidPtr(&CSimpleTransform::UpdateMatrix)));const auto simpleBytes=ReversingProbe::Bytes(reinterpret_cast<void*>(simpleBody),32);ready&=ModuleIs(simpleBody,ProbeModule());const void* directPointers[]{FunctionToVoidPtr(&CMatrix::SetTranslate),FunctionToVoidPtr(&CMatrix::SetTranslateOnly),FunctionToVoidPtr(&CMatrix::ResetOrientation),FunctionToVoidPtr(&CMatrix::SetRotateZOnly)};std::vector<uintptr_t> directBodies;std::vector<std::vector<uint8>> directBytes;for(auto* ptr:directPointers){const auto body=Resolve(uintptr_t(ptr));directBodies.push_back(body);directBytes.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(body),32));ready&=ModuleIs(body,ProbeModule());}auto directProof=[&]{bool ok=Libraries();for(size_t i=0;i<directBodies.size();++i)ok&=Resolve(uintptr_t(directPointers[i]))==directBodies[i]&&ModuleIs(directBodies[i],ProbeModule())&&directBytes[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(directBodies[i]),32);return ok;};
 if(!ready){const bool rh=restoreRoutes();baseline.Restore();RestoreCanonical();Write({{"event","error"},{"reason","Registered main/collaborator preflight rejected"},{"pairs",0},{"state_restored",rh&&baseline.Equal()&&CanonicalRestored()}});done=true;return;}
 CVehicle* vehicle=nullptr;CColModel* model=nullptr;CColModel* standard=nullptr;CCollisionData* data=nullptr;float boundRadius{};std::array<std::array<std::vector<Fixture>,10>,2> fixtures;
 for(auto* v:candidates){OwnerSnapshot ownerBase(baseline,v);if(!ownerBase.legal)continue;const auto saved=ReversingProbe::Bytes(v,sizeof(CVehicle));auto reset=[&]{baseline.Restore();ownerBase.Restore();std::memcpy(v,saved.data(),saved.size());};bool valid=true;std::array<std::array<std::vector<Fixture>,10>,2> possible;
  switchAll(false,[&]{standard=v->GetModelInfo()->GetColModel();model=reinterpret_cast<CColModel*>(CallThis(0x535300,v));data=model?model->m_pColData:nullptr;boundRadius=standard?standard->m_boundSphere.m_fRadius:0.0f;if(!data||data->m_nNumSpheres<2||data->m_nNumSpheres>128||!data->m_pSpheres||!std::isfinite(boundRadius)||boundRadius<=0){valid=false;return;}
   for(uint32 representation=0;representation<2;++representation){reset();if(representation==0)CallThis(0x54F3B0,v);auto* m=reinterpret_cast<CMatrix*>(CallThis(0x411990,v));if(!m||!v->m_matrix||v->m_matrix->m_pOwner!=v){valid=false;break;}const CVector pos=m->GetPosition();std::vector<CVector> centers;for(uint32 i=0;i<data->m_nNumSpheres;++i){auto&s=data->m_pSpheres[i];if(!std::isfinite(s.m_fRadius)||s.m_fRadius<=0){valid=false;break;}const auto p=WorldPoint(*m,s.m_vecCenter);if(!std::isfinite(p.x)||!std::isfinite(p.y)||!std::isfinite(p.z)){valid=false;break;}centers.push_back(p);}if(!valid)break;
    for(uint32 kind=0;kind<6;++kind){CVector center=pos;const float distance=5.0f*boundRadius+4.0f;if(kind<2)center.x+=kind==0?distance:-distance;else if(kind<4)center.y+=kind==2?distance:-distance;else center.z+=kind==4?distance:-distance;auto g=Box(CGarages::aGarages[0],center,2*boundRadius+1,2*boundRadius+1,2*boundRadius+1,0);if(Broad(g,pos,boundRadius)!=kind){valid=false;break;}possible[representation][kind].push_back({g,kind,0,-1});}if(!valid)break;
    for(uint32 angle=0;angle<4;++angle){auto early=Box(CGarages::aGarages[0],centers[0],0.01f,0.01f,2*boundRadius+1,angle);if(Broad(early,pos,boundRadius)==6&&FirstSphere(early,centers,data)==0)possible[representation][6].push_back({early,6,angle,0});
     bool gotLate=false,gotMiss=false;for(int zi=-2;zi<=2&&(!gotLate||!gotMiss);++zi)for(int yi=-5;yi<=5&&(!gotLate||!gotMiss);++yi)for(int xi=-5;xi<=5&&(!gotLate||!gotMiss);++xi){CVector center{pos.x+float(xi)*boundRadius/5.0f,pos.y+float(yi)*boundRadius/5.0f,pos.z+float(zi)*boundRadius/2.0f};auto g=Box(CGarages::aGarages[0],center,0.01f,0.01f,0.01f,angle);if(Broad(g,pos,boundRadius)!=6)continue;const auto hit=FirstSphere(g,centers,data);if(hit>0&&!gotLate){possible[representation][7].push_back({g,7,angle,hit});gotLate=true;}if(hit<0&&!gotMiss){possible[representation][8].push_back({g,8,angle,-1});gotMiss=true;}}
    }
    auto empty=Box(CGarages::aGarages[0],pos,2*boundRadius+1,2*boundRadius+1,2*boundRadius+1,0);possible[representation][9].push_back({empty,9,0,-1});for(const auto& xs:possible[representation])valid&=!xs.empty();
   }
  });reset();RestoreCanonical();if(valid){vehicle=v;fixtures=std::move(possible);break;}
 }
 if(!vehicle){const bool rh=restoreRoutes();baseline.Restore();RestoreCanonical();if(!rh){Write({{"event","error"},{"reason","Waiting route restore failed"}});done=true;return;}wait(candidates.size(),baseline.Count(),"Required nonempty real sphere early/late/miss branches unavailable");return;}
 done=true;OwnerSnapshot ownerBase(baseline,vehicle);const auto vehicleBefore=ReversingProbe::Bytes(vehicle,sizeof(CVehicle));const auto standardBefore=ReversingProbe::Bytes(standard,sizeof(CColModel)),modelBefore=ReversingProbe::Bytes(model,sizeof(CColModel)),dataBefore=ReversingProbe::Bytes(data,sizeof(CCollisionData)),spheresBefore=ReversingProbe::Bytes(data->m_pSpheres,data->m_nNumSpheres*sizeof(CColSphere));const auto garageGlobals=ReversingProbe::Bytes(CGarages::aGarages,sizeof(CGarages::aGarages));const auto garageCount=CGarages::NumGarages;auto* modelInfo=vehicle->GetModelInfo();const auto modelInfoBefore=ReversingProbe::Bytes(modelInfo,sizeof(CBaseModelInfo));const uintptr_t modelSlot=0xA9B0C8+4*uint32(vehicle->m_nModelIndex);const auto modelSlotBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(modelSlot),4);
 auto reset=[&]{baseline.Restore();ownerBase.Restore();std::memcpy(vehicle,vehicleBefore.data(),vehicleBefore.size());std::memcpy(data,dataBefore.data(),dataBefore.size());};auto stateRestored=[&]{return baseline.Equal()&&ownerBase.Equal()&&vehicleBefore==ReversingProbe::Bytes(vehicle,sizeof(CVehicle))&&dataBefore==ReversingProbe::Bytes(data,sizeof(CCollisionData));};
 json meta=json::array();for(size_t i=0;i<routes.size();++i)meta.push_back({{"class",specs[i].cls},{"name",specs[i].name},{"entry",specs[i].entry},{"body",routes[i]->Body()},{"registered",routes[i]->Registered()},{"off_bytes",off[i]},{"on_bytes",on[i]}});RestoreCanonical();Write({{"event","begin"},{"registered_hook",ready},{"all_routes_registered",ready},{"route_proofs",meta},{"real_pool_count",baseline.Count()},{"real_spheres",data->m_nNumSpheres},{"native_matrix_entries_shared",true},{"native_settranslate_bytes",nativeSetTranslate},{"native_settranslateonly_bytes",nativeSetTranslateOnly},{"native_resetorientation_bytes",nativeResetOrientation},{"native_setrotatez_bytes",nativeSetRotateZOnly},{"direct_matrix_bodies",directBodies},{"direct_matrix_bytes",directBytes},{"standard_model",uintptr_t(standard)},{"actual_model",uintptr_t(model)},{"native_point_bytes",nativePoint},{"native_update_bytes",nativeUpdateMatrix},{"native_remove_bytes",nativeRemoveMatrix},{"simple_transform_body",simpleBody},{"simple_transform_bytes",simpleBytes},{"player_area",int(FindPlayerPed()->GetAreaCode())},{"fixture_configuration","FastLoader.SaveGameToLoad=0"},{"collaborators_stubbed",false},{"owned_actors_created",0},{"fp_state_restored",CanonicalRestored()}});
 bool passed=true;uint32 failures{};std::array<uint32,10> kinds{};std::array<uint32,2> representations{};std::array<uint32,4> angleCounts{};bool lateSeen=false,missSeen=false,lazySeen=false,zeroSeen=false,zeroNullSeen=false;
 for(int scenario=0;scenario<2048;++scenario){const uint32 kind=uint32(scenario)%10,representation=(uint32(scenario)/10)%2;const auto& options=fixtures[representation][kind];const auto& fixture=options[(uint32(scenario)/20)%options.size()];
  auto run=[&](bool candidate){reset();bool prepOK=false;switchAll(false,[&]{prepOK=routeProof(false)&&Native(0x54F3B0,nativeRemoveMatrix)&&Libraries();if(representation==0)CallThis(0x54F3B0,vehicle);prepOK&=routeProof(false)&&Native(0x54F3B0,nativeRemoveMatrix);});if(kind==9)data->m_nNumSpheres=0;const auto fixtureDataBytes=ReversingProbe::Bytes(data,sizeof(CCollisionData));const auto descriptorCount=data->m_nNumSpheres;const CVector positionBefore=vehicle->GetPosition();MatrixSnapshot fixturePool;OwnerSnapshot fixtureOwners(fixturePool,vehicle);const auto fixtureVehicle=ReversingProbe::Bytes(vehicle,sizeof(CVehicle));const auto expectedMatrix=representation!=0?uintptr_t(vehicle->m_matrix):(kind<6||kind==9)?uintptr_t(0):uintptr_t(gMatrixList.m_freeListHead.m_pNext);struct Guarded{std::array<uint8,16> left;CGarage garage;std::array<uint8,16> right;};Guarded box;box.left.fill(0xA5);box.right.fill(0x5A);box.garage=fixture.garage;const auto boxBefore=ReversingProbe::Bytes(&box,sizeof(box));Result result{};bool routeOK=false;RestoreCanonical();switchAll(candidate,[&]{routeOK=routeProof(candidate)&&directProof();result=Call(0x448EE0,&box.garage,vehicle);routeOK&=routeProof(candidate)&&directProof()&&simpleBytes==ReversingProbe::Bytes(reinterpret_cast<void*>(simpleBody),32)&&ModuleIs(simpleBody,ProbeModule());});
  const bool returned=(result.eax&0xFF)!=0;const bool contract=returned==(kind==6||kind==7)&&uintptr_t(vehicle->m_matrix)==expectedMatrix&&(!vehicle->m_matrix||vehicle->m_matrix->m_pOwner==vehicle);json changes=json::array();for(const auto&[at,bytes]:fixturePool.nodes){const auto current=ReversingProbe::Bytes(at,bytes.size());if(current!=bytes)changes.push_back({{"node",uintptr_t(at)},{"bytes",current}});}const auto header=ReversingProbe::Bytes(&gMatrixList,sizeof(gMatrixList));const auto currentOwners=ownerBase.Current(vehicle);const auto matrixObserved=uintptr_t(vehicle->m_matrix);const auto matrixBytes=vehicle->m_matrix?ReversingProbe::Bytes(vehicle->m_matrix,sizeof(CMatrix)):std::vector<uint8>{};const auto garageBytes=ReversingProbe::Bytes(&box.garage,sizeof(CGarage));const auto positionBits=std::array<uint32,3>{std::bit_cast<uint32>(positionBefore.x),std::bit_cast<uint32>(positionBefore.y),std::bit_cast<uint32>(positionBefore.z)};
  bool ro=modelInfoBefore==ReversingProbe::Bytes(modelInfo,sizeof(CBaseModelInfo))&&modelSlotBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(modelSlot),4)&&garageCount==CGarages::NumGarages&&boxBefore==ReversingProbe::Bytes(&box,sizeof(box))&&standardBefore==ReversingProbe::Bytes(standard,sizeof(CColModel))&&modelBefore==ReversingProbe::Bytes(model,sizeof(CColModel))&&fixtureDataBytes==ReversingProbe::Bytes(data,sizeof(CCollisionData))&&spheresBefore==ReversingProbe::Bytes(data->m_pSpheres,spheresBefore.size())&&garageGlobals==ReversingProbe::Bytes(CGarages::aGarages,sizeof(CGarages::aGarages));auto afterVehicle=ReversingProbe::Bytes(vehicle,sizeof(CVehicle));for(size_t i=0;i<afterVehicle.size();++i)if(i<0x14||i>=0x18)ro&=afterVehicle[i]==fixtureVehicle[i];for(const auto&[at,bytes]:fixtureOwners.owners)if(at!=vehicle)ro&=bytes==ReversingProbe::Bytes(at,bytes.size());const bool graph=MatrixSnapshot{}.legal;reset();RestoreCanonical();json value={{"return_bool",returned},{"fixture_kind",kind},{"matrix_present",representation!=0},{"first_sphere_hit",fixture.hit},{"sphere_count",uint16(dataBefore[0]|uint16(dataBefore[1])<<8)},{"descriptor_count",descriptorCount},{"descriptor_int16",int16(descriptorCount)},{"descriptor_bytes",fixtureDataBytes},{"descriptor_before_bytes",dataBefore},{"descriptor_guards_ok",std::equal(dataBefore.begin()+2,dataBefore.end(),fixtureDataBytes.begin()+2)},{"sphere_bytes",spheresBefore},{"matrix_bytes",matrixBytes},{"garage_bytes",garageBytes},{"entity_position_bits",positionBits},{"standard_radius_bits",std::bit_cast<uint32>(boundRadius)},{"x87_control",uint16(canonicalFP[0]|uint16(canonicalFP[1])<<8)},{"garage_angle",fixture.angle},{"actual_matrix",matrixObserved},{"expected_matrix",expectedMatrix},{"pool_header",header},{"changed_nodes",changes},{"owners",currentOwners},{"route_ok",routeOK},{"fixture_legal",prepOK&&fixturePool.legal&&fixtureOwners.legal},{"graph_legal",graph},{"contract_ok",contract},{"readonly_ok",ro},{"stack_restored",result.abi},{"registers_preserved",result.abi},{"state_restored",stateRestored()},{"fp_state_restored",CanonicalRestored()}};RestoreCanonical();return value;};
  const auto original=run(false),candidate=run(true);bool equal=original==candidate;for(auto k:{"route_ok","fixture_legal","graph_legal","descriptor_guards_ok","contract_ok","readonly_ok","stack_restored","registers_preserved","state_restored","fp_state_restored"})equal&=original[k]==true&&candidate[k]==true;passed&=equal;failures+=!equal;++kinds[kind];++representations[representation];++angleCounts[fixture.angle];lateSeen|=kind==7&&fixture.hit>0;missSeen|=kind==8;lazySeen|=representation==0&&kind>=6&&kind<9;zeroSeen|=kind==9;zeroNullSeen|=kind==9&&representation==0;Write({{"event","comparison"},{"pair",scenario},{"case",scenario},{"function","CGarage::IsEntityTouching3D"},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
 }
 bool coverage=lateSeen&&missSeen&&lazySeen&&zeroSeen&&zeroNullSeen&&representations[0]==1028&&representations[1]==1020;for(uint32 i=0;i<10;++i)coverage&=kinds[i]==(i<8?205:204);coverage&=angleCounts[1]+angleCounts[2]+angleCounts[3]>0;passed&=coverage;Write({{"event","function_summary"},{"function","CGarage::IsEntityTouching3D"},{"cases",2048},{"failures",failures},{"complete_paths",coverage},{"fixture_counts",kinds},{"representation_counts",representations},{"angle_counts",angleCounts},{"late_sphere_seen",lateSeen},{"all_spheres_miss_seen",missSeen},{"lazy_matrix_seen",lazySeen},{"zero_spheres_seen",zeroSeen},{"zero_spheres_null_no_lazy",zeroNullSeen}});
 const bool rh=restoreRoutes();reset();RestoreCanonical();const bool restored=rh&&stateRestored()&&CanonicalRestored();Write({{"event",passed&&restored?"complete":"error"},{"pairs",2048},{"state_restored",restored},{"resources_restored",stateRestored()},{"fp_state_restored",CanonicalRestored()}});
}
}
