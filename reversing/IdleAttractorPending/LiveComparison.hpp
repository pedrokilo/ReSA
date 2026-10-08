#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "IdleCam.h"
#include "Attractors/PedAttractor.h"
#include "../ProbeRuntime.hpp"

namespace IdleAttractorPendingLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,2> fns{{
    {"CIdleCam","ProcessFOVZoom",0x517BF0,0,-1,1024},
    {"CPedAttractor","AbortPedTasks",0x5EAF60,0,-1,1024},
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}
alignas(16) inline std::array<uint8,sizeof(CPed)> ped{};
alignas(16) inline std::array<uint8,sizeof(CCam)> cam{};
alignas(16) inline std::array<uint8,sizeof(CIdleCam)> idle{};
alignas(16) inline std::array<uint8,sizeof(CPedAttractor)> attractor{};
inline std::array<CPedTaskPair,16> pairs{};
alignas(16) inline std::array<std::array<uint8,16>,16> tasks{};
inline std::array<uintptr_t,9> taskVt{};
inline CVector lookPos; inline bool losResult{};
inline void __fastcall FakeLook(CIdleCam*,void*,const CEntity* target,CVector& out) {
    trace.push_back({"look",target==reinterpret_cast<CEntity*>(ped.data())}); out=lookPos;
}
inline bool __cdecl FakeLOS(const CVector& origin,const CVector& target,bool buildings,bool vehicles,bool peds,bool objects,bool dummies,bool seeThrough,bool cameraIgnore) {
    trace.push_back({"los",F(origin.x),F(origin.y),F(origin.z),F(target.x),F(target.y),F(target.z),buildings,vehicles,peds,objects,dummies,seeThrough,cameraIgnore,CWorld::pIgnoreEntity==reinterpret_cast<CEntity*>(ped.data())}); return losResult;
}
inline void* __fastcall FakeDelete(void* task,void*,uint32 flags) {
    const int index=int((uintptr_t(task)-uintptr_t(tasks.data()))/sizeof(tasks[0]));
    const auto& p=pairs[index];
    trace.push_back({"delete",index,flags,p.Ped==nullptr,p.Task==task,int(p.Slot),p.MatchID,p.UsedTask});
    At<uint32>(task,4)=0xDEAD0000u+index; return task;
}
inline json Bytes(const void* p,size_t n) { return ReversingProbe::Bytes(p,n); }
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"idle-attractor-pending-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"idle-attractor-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("idle-attractor-pending");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return; done=true;
    std::string only;{std::ifstream f(dir/"idle-attractor-pending-only.txt");if(f)std::getline(f,only);}
    std::array<ReversibleHooks::ReversibleHook::Base*,2> hooks{};
    std::array<bool,2> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){hooks[f]=FindHook(fns[f].cls,fns[f].name);if(!hooks[f]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[f].name}});return;}}
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto& clock=*reinterpret_cast<uint32*>(0xB7CB84);
    const auto savedClock=clock; const auto savedIgnore=CWorld::pIgnoreEntity;
    uint32 pairCount{}; bool passed=true;
    {
        using ReversingProbe::EntryPatch;
        EntryPatch lookExe(0x50EAE0,FakeLook),lookDll(std::bit_cast<uintptr_t>(&CIdleCam::GetLookAtPositionOnTarget),FakeLook);
        EntryPatch losExe(0x56A490,FakeLOS),losDll(std::bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),FakeLOS);
        taskVt.fill(uintptr_t(&FakeDelete));
        for(size_t f=0;f<fns.size();++f){const auto& fn=fns[f];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full&&only!=fn.cls)continue;int failures=0;
            for(int i=0;i<fn.cases;++i){
                auto run=[&](bool candidate){
                    hooks[f]->State(candidate);trace=json::array();
                    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    clock=uint32(i%4==0?0xF0000000u:20000u+uint32(i)*37u);
                    CWorld::pIgnoreEntity=reinterpret_cast<CEntity*>(uintptr_t(0x1234));
                    json out;
                    if(f==0){
                        idle.fill(0xCD);cam.fill(0xCD);ped.fill(0xCD);
                        auto* self=reinterpret_cast<CIdleCam*>(idle.data());
                        self->m_Target=i%7 ? reinterpret_cast<CEntity*>(ped.data()):nullptr;
                        self->m_Cam=reinterpret_cast<CCam*>(cam.data());
                        At<uint8>(ped.data(),0x36)=i%3==0 ? 3:2;
                        At<int32>(ped.data(),0x598)=i%4==0 ? 22:i%4==1 ? 5:4;
                        lookPos=CVector{float(i%13),float(i%11),float(i%7)};
                        self->m_Cam->m_vecSource=lookPos+CVector{float(i%23),float(i%5),float(i%9)};
                        self->m_ZoomState=static_cast<eIdleCamZoomState>(i%5);
                        self->m_ZoomNearest=15.0f+float(i%7);self->m_ZoomFarthest=70.0f;
                        self->m_CurFOV=i%8==0 ? self->m_ZoomNearest:i%8==1 ? 69.5f:float(20+i%50);
                        self->m_ZoomFrom=20.0f+float(i%13);self->m_ZoomTo=10.0f+float(i%17);
                        self->m_TimeZoomStarted=float(clock)-float(i%2000);
                        self->m_DurationFOVZoom=1000.0f+float(i%5)*125.0f;
                        self->m_TimeLastZoomIn=float(clock)-float((i%4)*12000);
                        self->m_TimeBeforeNewZoomIn=12000.0f;
                        self->m_DistStartFOVZoom=float(i%5)*7.5f;
                        self->m_TargetLOSCounter=i%16;self->m_nForceAZoomOut=i%2;self->m_bHasZoomedIn=(i/2)%2;losResult=(i/4)%2;
                        const float time=i%3==0?0.5f:i%3==1?1.0f:2.0f;
                        reinterpret_cast<void(__thiscall*)(CIdleCam*,float)>(fn.address)(self,time);
                        out["idle"]=Bytes(idle.data(),idle.size());out["camera"]=Bytes(cam.data(),cam.size());out["ped"]=Bytes(ped.data(),ped.size());
                    }else{
                        attractor.fill(0xCD);auto* self=reinterpret_cast<CPedAttractor*>(attractor.data());
                        const int count=i%17;self->m_PedTaskPairs._First=count?pairs.data():nullptr;
                        self->m_PedTaskPairs._Last=count?pairs.data()+count:nullptr;
                        self->m_PedTaskPairs._End=count?pairs.data()+pairs.size():nullptr;
                        for(int k=0;k<16;++k){tasks[k].fill(0xCD);At<uintptr_t>(tasks[k].data(),0)=uintptr_t(taskVt.data());
                            pairs[k].Ped=reinterpret_cast<CPed*>(uintptr_t(0x1000+k*4));pairs[k].Task=(i>>(k%9))&1 ? reinterpret_cast<CTask*>(tasks[k].data()):nullptr;
                            pairs[k].Slot=static_cast<eSecondaryTask>((i+k)%6);pairs[k].MatchID=i*17+k;pairs[k].UsedTask=(i+k)%2;}
                        reinterpret_cast<void(__thiscall*)(CPedAttractor*)>(fn.address)(self);
                        out["attractor"]=Bytes(attractor.data(),attractor.size());out["pairs"]=Bytes(pairs.data(),sizeof(pairs));out["tasks"]=Bytes(tasks.data(),sizeof(tasks));
                    }
                    out["trace"]=trace;out["switches"]=switched;out["ignore_restored"]=uintptr_t(CWorld::pIgnoreEntity)==0x1234;
                    clock=savedClock;CWorld::pIgnoreEntity=savedIgnore;return out;
                };
                const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok;failures+=!ok;Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    clock=savedClock;CWorld::pIgnoreEntity=savedIgnore;
    Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",clock==savedClock&&CWorld::pIgnoreEntity==savedIgnore}});
}
}
