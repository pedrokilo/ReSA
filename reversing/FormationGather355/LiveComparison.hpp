#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Formation.h"
#include "PointList.h"
#include "PedList.h"
#include "../ProbeRuntime.hpp"
namespace FormationGather355Live {
inline bool done{};inline std::ofstream log;inline std::array<uint32,32> pedTokens{},targetTokens{};
alignas(16) inline std::array<uint8,sizeof(CPed)> anchor{};
inline CMatrixLink matrix{};
alignas(16) inline std::array<uint8,0xA18*8> vehicles{};
alignas(16) inline std::array<uint8,0x19C*8> objects{};
inline std::array<CMatrixLink,16> matrices{};inline std::array<uint8,8> vehicleFlags{},objectFlags{};
inline std::array<uintptr_t,6> vehiclePool{},objectPool{};
alignas(16) inline std::array<uint8,0x40> modelInfo{};
inline CColModel col{};inline json trace;inline int scenario{};
inline uint32 Bits(float v){return std::bit_cast<uint32>(v);}
inline CColModel* __fastcall GetCol(CEntity*,void*){return &col;}
inline bool __fastcall CanCover(CObject* object,void*){const int index=int((reinterpret_cast<uint8*>(object)-objects.data())/0x19C);const bool answer=(scenario+index)%3!=0;trace.push_back({"can_cover",index,answer});return answer;}
inline void __cdecl Cover(CPointList* out,CVector pos,CMatrix* matrix,const CVector& center,const CVector& low,const CVector& high,float cutoff){trace.push_back({"cover_box",Bits(pos.x),Bits(pos.y),Bits(pos.z),int((reinterpret_cast<const uint8*>(matrix)-reinterpret_cast<const uint8*>(matrices.data()))/sizeof(CMatrixLink)),Bits(center.x),Bits(low.x),Bits(high.z),Bits(cutoff)});out->m_Count=uint32(scenario%5);for(uint32 k=0;k<out->m_Count;++k)out->m_Points[k]=CVector{float(k),float(scenario%7),float((reinterpret_cast<const uint8*>(matrix)-reinterpret_cast<const uint8*>(matrices.data()))/sizeof(CMatrixLink))};}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;for(size_t i=0;i<n;++i){h^=static_cast<const uint8*>(p)[i];h*=1099511628211ull;}return h;}
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
inline json Ptr(const void* p){if(!p)return nullptr;auto a=uintptr_t(p);if(a>=uintptr_t(targetTokens.data())&&a<uintptr_t(targetTokens.data())+sizeof(targetTokens))return json::array({"target",(a-uintptr_t(targetTokens.data()))/4});return a;}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{
{"CFormation","GenerateGatherDestinations_AroundCar",0x69A770,0,-1,1024},
{"CFormation","FindCoverPoints",0x69B860,0,-1,1024}};
inline auto* FindHook(const char* n){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CFormation")for(auto& x:c.Items())if(x->Name()==n)h=x.get();});return h;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"formation-gather355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"formation-gather355-runtime.jsonl");ReversingProbe::InstallFailureCapture("formation-gather355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
auto* globals=reinterpret_cast<uint8*>(0xC1A2C0);std::array<uint8,0x294> saved{};std::memcpy(saved.data(),globals,saved.size());auto& vehicleSlot=StaticRef<CVehiclePool*>(0xB74494);auto& objectSlot=StaticRef<CObjectPool*>(0xB7449C);auto* oldVehicles=vehicleSlot;auto* oldObjects=objectSlot;auto*& modelSlot=CModelInfo::ms_modelInfoPtrs[100];auto* oldModel=modelSlot;
int pairs=0;bool pass=true;
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch ce(0x535300,GetCol),cd(bit_cast<uintptr_t>(&CEntity::GetColModel),GetCol),oe(0x5A1B60,CanCover),od(bit_cast<uintptr_t>(&CObject::CanBeUsedToTakeCoverBehind),CanCover),be(0x699FF0,Cover),bd(bit_cast<uintptr_t>(&CFormation::FindCoverPointsBehindBox),Cover);
for(const auto& fn:fns){auto* h=FindHook(fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}auto old=h->Hooked(),lock=h->Locked();h->LockState(false);int failures=0,yes=0,no=0;
for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){scenario=i;trace=json::array();std::memset(globals,0xCD,saved.size());vehicles.fill(0);objects.fill(0);modelInfo.fill(0);*reinterpret_cast<CColModel**>(modelInfo.data()+0x14)=&col;modelSlot=reinterpret_cast<CBaseModelInfo*>(modelInfo.data());
col.m_boundBox.m_vecMin=CVector{-1.f,-2.f,-.5f};col.m_boundBox.m_vecMax=CVector{float(i%7)*.25f,2.f,float(i%5)*.5f};col.m_boundSphere.m_vecCenter=CVector{.125f,.25f,.5f};
vehiclePool={uintptr_t(vehicles.data()),uintptr_t(vehicleFlags.data()),uintptr_t(i%9),0,0,0};objectPool={uintptr_t(objects.data()),uintptr_t(objectFlags.data()),uintptr_t((i/9)%9),0,0,0};vehicleSlot=reinterpret_cast<CVehiclePool*>(vehiclePool.data());objectSlot=reinterpret_cast<CObjectPool*>(objectPool.data());
for(int k=0;k<8;++k){matrices[k].SetUnity();matrices[k+8].SetUnity();matrices[k].GetPosition()=CVector{float(k)-3.f,float(i%7),1.f};matrices[k+8].GetPosition()=CVector{float(k)*.5f,float(k%3),1.f};matrices[k+8].GetUp().z=(i+k)%3?.96f:.95f;auto* v=reinterpret_cast<CVehicle*>(vehicles.data()+k*0xA18);auto* o=reinterpret_cast<CObject*>(objects.data()+k*0x19C);v->m_matrix=&matrices[k];o->m_matrix=&matrices[k+8];v->m_nModelIndex=100;v->m_nVehicleType=static_cast<eVehicleType>((i+k)%4);v->m_pDriver=(i+k)%4?nullptr:reinterpret_cast<CPed*>(anchor.data());v->m_vecMoveSpeed=CVector{(i+k)%3?.001f:.005f,0.f,0.f};vehicleFlags[k]=uint8((i+k)%5?0:128);objectFlags[k]=uint8((i+k)%4?0:128);}
CFormation::m_Destinations.m_Count=23;std::memset(CFormation::m_Destinations.m_PointHasBeenClaimed,0xCD,sizeof(CFormation::m_Destinations.m_PointHasBeenClaimed));CPedList list{};list.m_count=uint32(i%31);CVector center{float(i%9-4),float(i%5),1.f};
h->State(candidate);bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
if(fn.address==0x69A770)reinterpret_cast<void(__cdecl*)(CPedList&,CVehicle*)>(fn.address)(list,reinterpret_cast<CVehicle*>(vehicles.data()));
else reinterpret_cast<void(__cdecl*)(CVector,float)>(fn.address)(center,float(i%7));
json points=json::array();for(uint32 k=0;k<CFormation::m_Destinations.m_Count;++k){const auto& point=CFormation::m_Destinations.m_Points[k];points.push_back({Bits(point.x),Bits(point.y),Bits(point.z)});}return json{{"points",points},{"globals",Hash(globals,saved.size())},{"vehicles",Hash(vehicles.data(),vehicles.size())},{"objects",Hash(objects.data(),objects.size())},{"matrices",Hash(matrices.data(),sizeof(matrices))},{"trace",trace},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;const bool changed=fn.address==0x69A770?i%31!=0:!a["trace"].empty();yes+=changed;no+=!changed;Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(lock);pass&=yes>0&&no>0;Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"found",yes},{"missing",no}});}
vehicleSlot=oldVehicles;objectSlot=oldObjects;modelSlot=oldModel;std::memcpy(globals,saved.data(),saved.size());Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
