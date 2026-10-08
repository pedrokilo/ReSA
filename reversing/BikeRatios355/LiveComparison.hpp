#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Bike.h"
#include "SurfaceInfos_c.h"
#include "../ProbeRuntime.hpp"
namespace BikeRatios355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline int scenario{},draw{};
inline int32 __fastcall Adhesion(void*,void*,SurfaceId id){const auto group=int(id)%7==4?4:2;trace.push_back({"adhesion",int(id),group});return group;}
inline int32 __cdecl Random(){const auto r=(scenario*43+draw++*7919)&0x7FFF;trace.push_back({"random",r});return r;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CBike","DoBurstAndSoftGroundRatios",0,0x871360,52,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"bike-ratios355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"bike-ratios355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("bike-ratios355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"bike-ratios355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 auto& modelSlot=StaticRef<CBaseModelInfo*>(0xA9B0C8+400*4);const auto oldModel=modelSlot;auto& tankSlot=StaticRef<CBaseModelInfo*>(0xA9B0C8+432*4);const auto oldTank=tankSlot;const auto oldStep=CTimer::GetTimeStep();const auto oldWet=StaticRef<float>(0xC81308);uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x55E5C0,Adhesion),ad(bit_cast<uintptr_t>(&SurfaceInfos_c::GetAdhesionGroup),Adhesion);
 EntryPatch b(0x821B1E,Random),bd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{};alignas(16) std::array<uint8,sizeof(CVehicleModelInfo)> mi{};alignas(16) std::array<uint8,0x80> matrix{};
   auto run=[&](bool candidate){scenario=i;draw=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(mi.data(),0,mi.size());memset(matrix.data(),0,matrix.size());
    auto* self=reinterpret_cast<CBike*>(object.data());*reinterpret_cast<uintptr_t*>(self)=0x871360;self->m_nModelIndex=i%17==0?432:400;*reinterpret_cast<uintptr_t*>(object.data()+0x14)=uintptr_t(matrix.data());
    auto* model=reinterpret_cast<CVehicleModelInfo*>(mi.data());modelSlot=model;tankSlot=model;model->m_fWheelSizeFront=float(i%15+1)*.125f;model->m_fWheelSizeRear=float(i%11+1)*.125f;
    *reinterpret_cast<CVector*>(matrix.data()+0x10)=CVector{float(i%9-4)*.25f,1.f,float(i%7-3)*.125f};self->m_vecMoveSpeed=CVector{float(i%13-6)*.05f,float(i%11-5)*.05f,float(i%7-3)*.05f};
    self->m_nHandlingFlagsIntValue=static_cast<eVehicleHandlingFlags>(std::array<uint32,4>{0,0x100000,0x200000,0x300000}[(i/27)%4]);
    static constexpr float ratios[]{-.25f,0.f,.2f,.5f,.9f,1.f,1.25f,std::bit_cast<float>(0x7FC00000u)};
    for(size_t w=0;w<4;++w){self->m_aWheelRatios[w]=ratios[(i/3+int(w))%8];self->m_aWheelColPoints[w].m_nSurfaceTypeB=static_cast<eSurfaceType>(i%5==0?178:(i+int(w))%21);self->m_fLineLength[w]=float((i+int(w))%9+1)*.25f;self->m_fSuspensionLength[w]=self->m_fLineLength[w]*float((i/9+int(w))%5)*.25f;}
    for(size_t w=0;w<2;++w){self->m_nWheelStatus[w]=uint8((i/9+int(w))%4);self->m_aWheelPitchAngles[w]=float((i+int(w))%41-20)*.17f;self->m_aWheelAngularVelocity[w]=float((i/7+int(w))%7-3)*.2f;}
    CTimer::UpdateTimeStep(float(i%5)*.5f);StaticRef<float>(0xC81308)=float((i/13)%5)*.25f;
    hooks[n]->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((address!=0x6B6950)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(void*)>(address)(self);
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"matrix",Hash(matrix.data(),matrix.size())},{"model",Hash(mi.data(),mi.size())},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 modelSlot=oldModel;tankSlot=oldTank;CTimer::UpdateTimeStep(oldStep);StaticRef<float>(0xC81308)=oldWet;
 }
 bool restored=modelSlot==oldModel&&tankSlot==oldTank;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}




