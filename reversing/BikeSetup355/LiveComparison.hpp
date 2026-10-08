#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Bike.h"
#include "SurfaceInfos_c.h"
#include "AEVehicleAudioEntity.h"
#include "CarCtrl.h"
#include "World.h"
#include "RwHelper.h"
#include "../ProbeRuntime.hpp"
namespace BikeSetup355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline RwMatrix* __cdecl Transform(RwMatrix* matrix,const RwMatrix* parent,RwOpCombineType type){trace.push_back({"transform",int(type),std::bit_cast<uint32>(parent->pos.x),std::bit_cast<uint32>(parent->pos.y),std::bit_cast<uint32>(parent->pos.z)});matrix->pos.x+=parent->pos.x;matrix->pos.y+=parent->pos.y;matrix->pos.z+=parent->pos.z;return matrix;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CBike","SetupSuspensionLines",0,0x871360,48,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"bike-setup355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"bike-setup355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("bike-setup355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"bike-setup355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 auto& modelSlot=CModelInfo::ms_modelInfoPtrs[400];const auto savedModel=modelSlot;uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch t(0x7F25A0,Transform),td(bit_cast<uintptr_t>(&RwMatrixTransform),Transform);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{};alignas(16) std::array<uint8,sizeof(CVehicleModelInfo)> model{};alignas(16) std::array<uint8,sizeof(CColModel)> col{};alignas(16) std::array<uint8,sizeof(CCollisionData)> data{};alignas(16) std::array<uint8,sizeof(tHandlingData)> handling{};std::array<CColLine,4> lines{};std::array<CColSphere,3> spheres{};std::array<RwFrame,6> frames{};RpClump clump{};
   auto run=[&](bool candidate){trace=json::array();memset(object.data(),0xCD,object.size());memset(model.data(),0,model.size());memset(col.data(),0,col.size());memset(data.data(),0,data.size());memset(handling.data(),0,handling.size());memset(lines.data(),0,sizeof(lines));memset(spheres.data(),0,sizeof(spheres));memset(frames.data(),0,sizeof(frames));memset(&clump,0,sizeof(clump));
    auto* self=reinterpret_cast<CBike*>(object.data());*reinterpret_cast<uintptr_t*>(self)=0x871360;self->m_nModelIndex=400;self->m_pHandlingData=reinterpret_cast<tHandlingData*>(handling.data());auto* mi=reinterpret_cast<CVehicleModelInfo*>(model.data());auto* cm=reinterpret_cast<CColModel*>(col.data());auto* dc=reinterpret_cast<CCollisionData*>(data.data());modelSlot=mi;*reinterpret_cast<void**>(model.data()+0x14)=cm;cm->m_pColData=dc;dc->m_pLines=lines.data();dc->m_nNumSpheres=uint16(i%4);dc->m_pSpheres=spheres.data();
    const bool authored=i%2!=0;for(size_t j=0;j<4;++j){lines[j].m_vecStart=CVector{float(j)*.25f,float(j)*.5f,authored?float(j)*.125f:std::bit_cast<float>(0x47C34FFFu)};lines[j].m_vecEnd=CVector{0.f,0.f,0.f};}if(i%3==0)lines[1].m_vecStart.z=std::bit_cast<float>(0x47C34FFFu);
    mi->m_fWheelSizeFront=float(i%7+1)*.25f;mi->m_fWheelSizeRear=float(i%5+1)*.25f;self->m_aWheelOrigHeights={-.25f,-.5f};self->m_pHandlingData->m_fSuspensionUpperLimit=.5f;self->m_pHandlingData->m_fSuspensionLowerLimit=-float(i%4+1)*.25f;self->m_pHandlingData->m_fSuspensionForceLevel=float(i%5+1)*.5f;self->m_pHandlingData->m_nModelFlags=static_cast<eVehicleHandlingModelFlags>((i/4)%2?0x40000000u:0u);self->m_pHandlingData->m_nHandlingFlags=static_cast<eVehicleHandlingFlags>(i%2?0x40000000u:0u);
    cm->m_boundBox.m_vecMin=CVector{-1.f,-2.f,-.125f};cm->m_boundBox.m_vecMax=CVector{6.f,8.f,0.f};cm->m_boundSphere.m_fRadius=i%3==0?20.f:1.f;
    for(size_t j=0;j<3;++j){spheres[j].m_vecCenter=CVector{0.f,0.f,float(int(j)-1)*.25f};spheres[j].m_fRadius=float(j+1)*.25f;}
    for(size_t j=0;j<6;++j){frames[j].modelling.right={1.f,0.f,0.f};frames[j].modelling.up={0.f,1.f,0.f};frames[j].modelling.at={0.f,0.f,1.f};frames[j].modelling.pos={float(j)*.25f,float(j)*.5f,float(j)*.25f};}
    clump.object.type=rpCLUMP;clump.object.parent=&frames[5];*reinterpret_cast<void**>(object.data()+0x18)=&clump;std::ranges::fill(self->m_aBikeNodes,nullptr);self->m_aBikeNodes[BIKE_WHEEL_FRONT]=&frames[0];self->m_aBikeNodes[BIKE_WHEEL_REAR]=&frames[1];self->m_aBikeNodes[BIKE_FORKS_FRONT]=&frames[2];self->m_aBikeNodes[BIKE_FORKS_REAR]=i%5==0?nullptr:&frames[3];frames[0].object.parent=i%3==0?nullptr:&frames[4];frames[1].object.parent=i%4==0?nullptr:&frames[4];frames[4].object.parent=&frames[5];
    hooks[n]->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((address!=0x6B89B0)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(CBike*)>(address)(self);self->m_pHandlingData=nullptr;*reinterpret_cast<void**>(object.data()+0x18)=nullptr;std::ranges::fill(self->m_aBikeNodes,nullptr);
    if(!candidate)++positiveCases;
    json sphereBytes=json::array();const auto* sphereRaw=reinterpret_cast<const uint8*>(spheres.data());for(size_t byte=0;byte<sizeof(spheres);++byte)sphereBytes.push_back(uint32(sphereRaw[byte]));
    json sphereFields=json::array();for(const auto& sphere:spheres)sphereFields.push_back({{"x",std::bit_cast<uint32>(sphere.m_vecCenter.x)},{"y",std::bit_cast<uint32>(sphere.m_vecCenter.y)},{"z",std::bit_cast<uint32>(sphere.m_vecCenter.z)},{"radius",std::bit_cast<uint32>(sphere.m_fRadius)}});
    return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"lines",Hash(lines.data(),sizeof(lines))},{"spheres",Hash(spheres.data(),sizeof(spheres))},{"sphere_bytes",sphereBytes},{"sphere_fields",sphereFields},{"sphere_count",uint32(dc->m_nNumSpheres)},{"model_flags",uint32(reinterpret_cast<tHandlingData*>(handling.data())->m_nModelFlags)},{"handling_flags",uint32(reinterpret_cast<tHandlingData*>(handling.data())->m_nHandlingFlags)},{"height_bits",std::bit_cast<uint32>(self->m_fHeightAboveRoad)},{"box",Hash(&cm->m_boundBox,sizeof(cm->m_boundBox))},{"radius",std::bit_cast<uint32>(cm->m_boundSphere.m_fRadius)},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 modelSlot=savedModel;
 bool restored=true;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}




