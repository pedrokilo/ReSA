#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Bike.h"
#include "../ProbeRuntime.hpp"
namespace BikeWheel355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
alignas(16) inline std::array<uint8,sizeof(CColModel)> originalCol{};
inline CColModel* __fastcall Col(void*,void*){trace.push_back({"col"});return reinterpret_cast<CColModel*>(originalCol.data());}
inline RwMatrix* __cdecl Transform(RwMatrix* matrix,const RwMatrix* parent,RwOpCombineType type){trace.push_back({"transform",int(type),std::bit_cast<uint32>(parent->pos.x),std::bit_cast<uint32>(parent->pos.y),std::bit_cast<uint32>(parent->pos.z)});matrix->pos.x+=parent->pos.x;matrix->pos.y+=parent->pos.y;matrix->pos.z+=parent->pos.z;return matrix;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CBike","SetUpWheelColModel",0,0x871360,43,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"bike-wheel355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"bike-wheel355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("bike-wheel355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"bike-wheel355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 auto& modelSlot=StaticRef<CBaseModelInfo*>(0xA9B0C8+400*4);const auto oldModel=modelSlot;uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch c(0x535300,Col),cd(bit_cast<uintptr_t>(&CEntity::GetColModel),Col);
 EntryPatch t(0x7F25A0,Transform),td(bit_cast<uintptr_t>(&RwMatrixTransform),Transform);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{};alignas(16) std::array<uint8,sizeof(CVehicleModelInfo)> mi{};alignas(16) std::array<uint8,sizeof(CColModel)> cm{};alignas(16) std::array<uint8,sizeof(CCollisionData)> dc{};std::array<CColSphere,2> spheres;std::array<RwFrame,5> frames{};
   auto run=[&](bool candidate){trace=json::array();memset(object.data(),0xCD,object.size());memset(mi.data(),0,mi.size());memset(cm.data(),0xCD,cm.size());memset(dc.data(),0xCD,dc.size());memset(spheres.data(),0xCD,sizeof(spheres));memset(originalCol.data(),0xCD,originalCol.size());memset(frames.data(),0,sizeof(frames));
    auto* self=reinterpret_cast<CBike*>(object.data());*reinterpret_cast<uintptr_t*>(self)=0x871360;self->m_nModelIndex=400;
    auto* model=reinterpret_cast<CVehicleModelInfo*>(mi.data());modelSlot=model;model->m_fWheelSizeFront=float(i%15+1)*.125f;model->m_fWheelSizeRear=float(i%11+1)*.125f;
    auto* col=reinterpret_cast<CColModel*>(cm.data());auto* data=reinterpret_cast<CCollisionData*>(dc.data());col->m_pColData=data;data->m_pSpheres=spheres.data();
    auto* source=reinterpret_cast<CColModel*>(originalCol.data());source->m_boundBox.m_vecMin=CVector{-1.f,-2.f,float(i%9-4)};source->m_boundBox.m_vecMax=CVector{1.f,2.f,float(i%11)};source->m_boundSphere.m_vecCenter=CVector{3.f,4.f,5.f};source->m_boundSphere.m_fRadius=float(i%7);
    for(int j=0;j<5;++j){frames[j].modelling.right={1.f,0.f,0.f};frames[j].modelling.up={0.f,1.f,0.f};frames[j].modelling.at={0.f,0.f,1.f};frames[j].modelling.pos={float(i%7+j),float(j-2),float(i%11-j)};}
    self->m_aBikeNodes[BIKE_CHASSIS]=&frames[0];self->m_aBikeNodes[BIKE_WHEEL_FRONT]=&frames[1];self->m_aBikeNodes[BIKE_WHEEL_REAR]=&frames[2];
    frames[1].object.parent=i%3==0?nullptr:i%3==1?&frames[0]:&frames[3];frames[2].object.parent=i%4==0?nullptr:i%4==1?&frames[0]:&frames[4];frames[3].object.parent=&frames[0];frames[4].object.parent=i%4==3?&frames[3]:&frames[0];
    hooks[n]->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((address!=0x6B67E0)!=candidate)return json{{"switch",false}};
    const auto ret=reinterpret_cast<bool(__thiscall*)(void*,CColModel*)>(address)(self,col);std::ranges::fill(self->m_aBikeNodes,nullptr);
    return json{{"switch",true},{"return",ret},{"object",Hash(object.data()+4,object.size()-4)},{"spheres",Hash(spheres.data(),sizeof(spheres))},{"count",data->m_nNumSpheres},{"box",Hash(&col->m_boundBox,sizeof(col->m_boundBox))},{"sphere",Hash(&col->m_boundSphere,sizeof(col->m_boundSphere))},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures}});
 }
 modelSlot=oldModel;
 }
 bool restored=modelSlot==oldModel;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}




