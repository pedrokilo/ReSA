#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "MonsterTruck.h"
#include "../ProbeRuntime.hpp"
namespace MonsterSetup355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline CVector wheels[4];
inline std::array<CColDisk,4> allocated;
inline void __fastcall Wheel(void*,void*,int32 i,CVector& out,bool local){out=wheels[i];trace.push_back({"wheel",i,local});}
inline void* __cdecl Allocate(uint32 bytes,uint32 hint){trace.push_back({"malloc",bytes,hint});return allocated.data();}
inline void __cdecl Free(void*){trace.push_back({"free"});}
inline void __fastcall BaseControl(void*,void*){trace.push_back({"base_control"});}
inline void __fastcall BaseReset(void*,void*){trace.push_back({"base_reset"});}
inline void __fastcall BaseCheck(void*,void*,bool speed){trace.push_back({"base_check",speed});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CMonsterTruck","SetupSuspensionLines",0,0x8717D8,48,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"monster-setup355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"monster-setup355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("monster-setup355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"monster-setup355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 const auto oldStep=CTimer::GetTimeStep(),oldRate=fWheelExtensionRate;const auto oldClamp=StaticRef<uint8>(0x8D33B0);uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch w(0x4C7D20,Wheel),wd(bit_cast<uintptr_t>(&CVehicleModelInfo::GetWheelPosn),Wheel);
 EntryPatch a(0x72F420,Allocate),ad(bit_cast<uintptr_t>(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc)),Allocate);
 EntryPatch f(0x72F430,Free),fd(bit_cast<uintptr_t>(&CMemoryMgr::Free),Free);
 auto& modelSlot=StaticRef<CBaseModelInfo*>(0xA9B0C8+400*4);const auto oldModel=modelSlot;
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CMonsterTruck)> object{};
   auto run=[&](bool candidate){trace=json::array();memset(object.data(),0xCD,object.size());auto* self=reinterpret_cast<CMonsterTruck*>(object.data());*reinterpret_cast<uintptr_t*>(object.data())=0x8717D8;
    alignas(16) std::array<uint8,sizeof(CVehicleModelInfo)> mi{};auto* model=reinterpret_cast<CVehicleModelInfo*>(mi.data());
    alignas(16) std::array<uint8,sizeof(CColModel)> cm{};auto* col=reinterpret_cast<CColModel*>(cm.data());
    alignas(16) std::array<uint8,sizeof(CCollisionData)> dc{};auto* data=reinterpret_cast<CCollisionData*>(dc.data());
    tHandlingData handling{};std::array<CColDisk,4> disks;memset(disks.data(),0xCD,sizeof(disks));memset(allocated.data(),0xCD,sizeof(allocated));
    modelSlot=model;model->m_pColModel=col;model->m_fWheelSizeFront=float(i%12+1)*.125f;col->m_pColData=data;
    data->m_pDisks=i%3==0?nullptr:disks.data();data->bUsesDisks=i%3==2;data->m_nNumLines=7;
    self->m_nModelIndex=400;self->m_pHandlingData=&handling;
    handling.m_fSuspensionForceLevel=float(i%9+1)*.25f;handling.m_fSuspensionUpperLimit=.75f;handling.m_fSuspensionLowerLimit=-.5f;
    col->m_boundBox.m_vecMin=CVector{-1.f,-2.f,float(i%8-5)*.25f};col->m_boundBox.m_vecMax=CVector{1.f,2.f,1.f};col->m_boundSphere.m_fRadius=float(i%5);
    for(int j=0;j<4;++j)wheels[j]=CVector{float(j-2)*.25f,float(j%2)*.5f,float((i+j)%11)*.125f};
    hooks[n]->State(candidate);const auto address=fn.address?fn.address:reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];const bool switched=fn.address?ReversingProbe::JumpsToDll(address)==candidate:(address!=std::array<uintptr_t,4>{0x6C7FB0,0,0,0}[n])==candidate;
    if(!switched)return json{{"switch",false}};
    if(n==3)reinterpret_cast<void(__thiscall*)(void*,bool)>(address)(self,i%2);else reinterpret_cast<void(__thiscall*)(void*)>(address)(self);
    self->m_pHandlingData=nullptr;return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"disks",Hash(data->m_pDisks,4*sizeof(CColDisk))},{"flags",bool(data->bUsesDisks)},{"lines",data->m_nNumLines},{"box",Hash(&col->m_boundBox,sizeof(col->m_boundBox))},{"sphere",Hash(&col->m_boundSphere,sizeof(col->m_boundSphere))},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures}});
 }
 modelSlot=oldModel;
 }
 CTimer::ms_fTimeStep=oldStep;fWheelExtensionRate=oldRate;StaticRef<uint8>(0x8D33B0)=oldClamp;bool restored=CTimer::GetTimeStep()==oldStep&&fWheelExtensionRate==oldRate&&StaticRef<uint8>(0x8D33B0)==oldClamp;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}



