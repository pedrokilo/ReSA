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
namespace BikeHands355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline int scenario{};
inline std::array<RwMatrix,13> bones{};inline RpHAnimHierarchy hierarchy{};
inline RpHAnimHierarchy* __cdecl Hierarchy(RpClump*){trace.push_back({"hierarchy"});return &hierarchy;}
inline int32 __cdecl Index(RpHAnimHierarchy*,int32 id){static constexpr int32 ids[]{0x18,0x19,0x1A,0x17,0x16,0x12D,0x22,0x23,0x24,0x21,0x20,0x12E};const auto index=int32(std::find(std::begin(ids),std::end(ids),id)-std::begin(ids));trace.push_back({"index",id,index});return index;}
inline RwMatrix* __cdecl Matrices(RpHAnimHierarchy*){trace.push_back({"array"});return bones.data();}
inline bool __fastcall IsPlayer(void*,void*){trace.push_back({"player"});return scenario%2!=0;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CBike","FixHandsToBars",0x6B7F90,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"bike-hands355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"bike-hands355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("bike-hands355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"bike-hands355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 std::array<CBaseModelInfo*,3> savedModels{};static constexpr int32 models[]{400,509,510};for(size_t j=0;j<3;++j)savedModels[j]=CModelInfo::ms_modelInfoPtrs[models[j]];uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch h(0x734A40,Hierarchy),hd(bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump),Hierarchy);
 EntryPatch ix(0x7C51A0,Index),ixd(bit_cast<uintptr_t>(&RpHAnimIDGetIndex),Index);
 EntryPatch ar(0x7C5120,Matrices),ard(bit_cast<uintptr_t>(&RpHAnimHierarchyGetMatrixArray),Matrices);
 EntryPatch pl(0x5DF8F0,IsPlayer),pld(bit_cast<uintptr_t>(&CPed::IsPlayer),IsPlayer);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{};alignas(16) std::array<uint8,sizeof(CPed)> ped{};alignas(16) std::array<uint8,0x80> matrix{};alignas(16) std::array<uint8,sizeof(CVehicleModelInfo)> model{};alignas(16) std::array<uint8,0x400> vehicleStruct{};RwFrame frame{};
   auto run=[&](bool candidate){scenario=i;trace=json::array();memset(object.data(),0xCD,object.size());memset(ped.data(),0,ped.size());memset(matrix.data(),0,matrix.size());memset(model.data(),0,model.size());memset(vehicleStruct.data(),0,vehicleStruct.size());memset(&frame,0,sizeof(frame));memset(bones.data(),0,sizeof(bones));
    auto* self=reinterpret_cast<CBike*>(object.data());*reinterpret_cast<uintptr_t*>(self)=0x871360;self->m_matrix=reinterpret_cast<CMatrixLink*>(matrix.data());self->m_nModelIndex=int16(models[(i/7)%3]);self->m_nFixLeftHand=uint8((i/3)%2);self->m_nFixRightHand=uint8((i/5)%2);std::ranges::fill(self->m_aBikeNodes,nullptr);self->m_aBikeNodes[BIKE_CHASSIS]=i%11==0?nullptr:&frame;
    auto* mi=reinterpret_cast<CVehicleModelInfo*>(model.data());*reinterpret_cast<void**>(model.data()+0x5C)=vehicleStruct.data();for(auto id:models)CModelInfo::ms_modelInfoPtrs[id]=mi;
    *reinterpret_cast<CVector*>(vehicleStruct.data()+0x78)=i%4==0?CVector{0.f,0.f,0.f}:CVector{float(i%5)*.125f,.5f,.25f};
    *reinterpret_cast<CVector*>(matrix.data())=CVector{1.f,0.f,0.f};*reinterpret_cast<CVector*>(matrix.data()+0x10)=CVector{0.f,1.f,0.f};*reinterpret_cast<CVector*>(matrix.data()+0x20)=CVector{0.f,0.f,1.f};*reinterpret_cast<CVector*>(matrix.data()+0x30)=CVector{float(i%7)*.25f,float(i%11)*.125f,float(i%13)*.25f};
    frame.modelling.right={1.f,0.f,0.f};frame.modelling.up={0.f,1.f,0.f};frame.modelling.at={0.f,0.f,1.f};frame.modelling.pos={.25f,.5f,.125f};
    for(size_t j=0;j<bones.size();++j){bones[j].right={1.f,0.f,0.f};bones[j].up={0.f,1.f,0.f};bones[j].at={0.f,0.f,1.f};bones[j].pos={float(j)*.125f,float(j)*.25f,float(j)*.125f};}
    hooks[n]->State(candidate);const auto* bytes=reinterpret_cast<const uint8*>(fn.address);if((bytes[0]==0xE9)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(CBike*,CPed*)>(fn.address)(self,reinterpret_cast<CPed*>(ped.data()));std::ranges::fill(self->m_aBikeNodes,nullptr);self->m_matrix=nullptr;
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"matrix",Hash(matrix.data(),matrix.size())},{"bones",Hash(bones.data(),sizeof(bones))},{"frame",Hash(&frame,sizeof(frame))},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 for(size_t j=0;j<3;++j)CModelInfo::ms_modelInfoPtrs[models[j]]=savedModels[j];
 bool restored=true;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}




