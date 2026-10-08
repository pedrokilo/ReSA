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
#include "../ProbeRuntime.hpp"
namespace BikeRoad355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline int scenario{},colCalls{},lineCalls{};inline CBike* current{};
alignas(16) inline std::array<uint8,sizeof(CColModel)> col{};
alignas(16) inline std::array<uint8,sizeof(CEntity)> hitEntity{};
inline CColModel* __fastcall Col(void*,void*){trace.push_back({"col",colCalls++});return reinterpret_cast<CColModel*>(col.data());}
inline bool __cdecl Vertical(const CVector& origin,float end,CColPoint& point,CEntity*& entity,bool buildings,bool vehicles,bool peds,bool objects,bool dummies,bool see,CStoredCollPoly* poly){
 const auto call=lineCalls++;const auto hit=(scenario>>call)&1;
 trace.push_back({"vertical",call,std::bit_cast<uint32>(origin.x),std::bit_cast<uint32>(origin.y),std::bit_cast<uint32>(origin.z),std::bit_cast<uint32>(end),buildings,vehicles,peds,objects,dummies,see,poly!=nullptr,hit});
 current->GetPosition().z+=.25f;point.m_vecPoint=CVector{origin.x,origin.y,float(scenario%17-8)*.25f+float(call)*.5f};point.m_nLightingB.value=uint8(scenario*17+call);entity=reinterpret_cast<CEntity*>(hitEntity.data());return hit!=0;
}
inline float __fastcall Height(void*,void*){trace.push_back({"height"});if(scenario%7==0)current->m_matrix=nullptr;return float(scenario%11)*.125f;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CBike","PlaceOnRoadProperly",0x6BEEB0,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"bike-road355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"bike-road355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("bike-road355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"bike-road355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch c(0x535300,Col),cd(bit_cast<uintptr_t>(&CEntity::GetColModel),Col);
 EntryPatch v(0x5674E0,Vertical),vd(bit_cast<uintptr_t>(&CWorld::ProcessVerticalLine),Vertical);
 EntryPatch h(0x6B58B0,Height);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{};alignas(16) std::array<uint8,0x80> matrix{};
   auto run=[&](bool candidate){scenario=i;colCalls=lineCalls=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(matrix.data(),0,matrix.size());memset(col.data(),0,col.size());memset(hitEntity.data(),0,hitEntity.size());
    current=reinterpret_cast<CBike*>(object.data());*reinterpret_cast<uintptr_t*>(current)=0x871360;current->m_matrix=reinterpret_cast<CMatrixLink*>(matrix.data());
    auto* model=reinterpret_cast<CColModel*>(col.data());model->m_boundBox.m_vecMax.y=float(i%7+1)*.5f;model->m_boundBox.m_vecMin.y=-float((i/7)%7+1)*.5f;
    *reinterpret_cast<CVector*>(matrix.data()+0x10)=CVector{float(i%9-4)*.25f,1.f,float(i%7-3)*.125f};*reinterpret_cast<CVector*>(matrix.data()+0x30)=CVector{float(i%13-6),float(i%17-8),float(i%19-9)*.5f};
    *reinterpret_cast<uint32*>(hitEntity.data()+0x1C)=uint32(i%4)<<30;current->m_pEntityWeAreOn=nullptr;
    hooks[n]->State(candidate);const auto* bytes=reinterpret_cast<const uint8*>(fn.address);if((bytes[0]==0xE9)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(CBike*)>(fn.address)(current);const bool placement=current->m_matrix==nullptr;current->m_matrix=nullptr;current->m_pEntityWeAreOn=nullptr;
    if(!candidate&&lineCalls==2)++positiveCases;
    return json{{"switch",true},{"placement",placement},{"object",Hash(object.data()+4,object.size()-4)},{"matrix",Hash(matrix.data(),matrix.size())},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 bool restored=true;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}




