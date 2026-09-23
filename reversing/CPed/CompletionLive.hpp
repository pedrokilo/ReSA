#pragma once
#include <fstream>
#include "Shadows.h"
#include "PedStats.h"
#include "TaskSimpleLand.h"
#include "EventSoundQuiet.h"
#include "TaskComplexDestroyCarMelee.h"
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "CompletionProbe.hpp"
#include "../ProbeSupport.hpp"
namespace PedCompletion {
using ReversingProbe::Bytes;
inline bool done{};
inline std::ofstream log;
inline void Write(const json& r) { log << r.dump() << '\n'; log.flush(); }
inline int Report(int type,char* msg,int*) {
    if(type!=_CRT_ASSERT) return FALSE;
    Write({{"event","assertion"},{"message",msg ? msg : ""}}); ExitProcess(91);
}
template<class T> struct Storage {
    alignas(T) std::array<uint8,sizeof(T)> bytes{};
    T* get() { return reinterpret_cast<T*>(bytes.data()); }
};
struct PedInput {
    Storage<CPlayerPed> ped;
    Storage<CPedIntelligence> intelligence;
    CMatrixLink matrix;
    PedInput(CPlayerPed* source,int i) {
        std::memcpy(ped.get(),source,sizeof(CPlayerPed));
        matrix.SetRotate(float(i%5-2)*.3f,float(i%7-3)*.2f,float(i%11-5)*.3f);
        auto* p=ped.get(); p->m_matrix=&matrix;
        auto* pi=intelligence.get(); std::memcpy(ped.bytes.data()+0x47C,&pi,4);
        pi->m_pPed=p; pi->m_TaskMgr.m_pPed=p;
        p->m_nPedType=i%3==0 ? PED_TYPE_PLAYER1 : i%3==1 ? PED_TYPE_PLAYER2 : PED_TYPE_CIVMALE;
        p->m_nPedState=std::array{PEDSTATE_IDLE,PEDSTATE_DIE,PEDSTATE_DEAD,PEDSTATE_ARRESTED}[i%4];
        p->bIsInTheAir=(i%19)==0; p->bIsLanding=(i%23)==0;
        p->m_nMoveState=eMoveState((i/4)%8);
        p->m_fCurrentRotation=std::array{-7.f,-3.14f,-.1f,0.f,.1f,3.14f,7.f}[(i/3)%7];
        p->m_fAimingRotation=std::array{-7.f,-3.14f,-.1f,0.f,.1f,3.14f,7.f}[(i/21)%7];
        p->m_fHeadingChangeRate=std::array{1.f,15.f,90.f,180.f}[(i/7)%4];
        p->m_fMoveAnim=std::array{-1.f,-.1f,0.f,.1f,1.f}[(i/5)%5];
        p->m_pedIK.bSlopePitch=(i/2)%2;
        p->m_pedIK.m_fSlopePitch=std::array{0.f,.009f,.01f,.011f,.5f,-.2f}[(i/6)%6];
        p->m_pedIK.m_fSlopeRoll=std::array{0.f,.019f,.02f,.021f,.5f,-.2f}[(i/36)%6];
        p->m_pedIK.m_fBodyRoll=.75f;
        p->bFallenDown=(i%7)==0; p->bPedHitWallLastFrame=(i%11)==0;
        p->field_578=CVector{float(i%5-2)*.5f,float((i/5)%5-2)*.5f,float((i/25)%5-2)*.5f};
        p->m_vecAnimMovingShiftLocal=CVector2D{float(i%7-3)*.02f,float(i%9-4)*.03f};
        p->m_vecAnimMovingShift=CVector2D{99.f,98.f};
    }
    json State() {
        auto* p=ped.get();
        return {{"exact",{int(p->m_nMoveState)}},{"numbers",{p->m_fCurrentRotation,p->m_fMoveAnim,p->m_pedIK.m_fBodyRoll,p->m_pedIK.m_fSlopePitch,p->m_pedIK.m_fSlopeRoll,p->m_vecAnimMovingShift.x,p->m_vecAnimMovingShift.y}}};
    }
    bool Bounded(const std::array<uint8,sizeof(CPlayerPed)>& before) const {
        for(size_t n=0;n<before.size();++n) {
            const bool allowed=(n>=0x518 && n<0x51C)||(n>=0x520 && n<0x528)||(n>=0x534 && n<0x538)||(n>=0x550 && n<0x55C)||(n>=0x564 && n<0x568);
            if(!allowed && before[n]!=ped.bytes[n]) return false;
        }
        return true;
    }
};
inline bool Equal(const json& a,const json& b,double& maximum) {
    if(a["exact"]!=b["exact"] || a["numbers"].size()!=b["numbers"].size()) return false;
    for(size_t i=0;i<a["numbers"].size();++i) {
        if(!a["numbers"][i].is_number() || !b["numbers"][i].is_number()) return false;
        const double error=std::abs(a["numbers"][i].get<double>()-b["numbers"][i].get<double>());
        maximum=std::max(maximum,error); if(!std::isfinite(error) || error>1e-5) return false;
    }
    return true;
}
#include "FootLandedComparison.hpp"
#include "FootStepsComparison.hpp"
#include "TyresComparison.hpp"
#include "PositionComparison.hpp"
#include "KillComparison.hpp"
inline void Update() {
    if(done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cped-completion-runtime.start")) { done=true; return; }
    if(!log.is_open()) { log.open(dir/"cped-completion-runtime.jsonl"); Write({{"event","waiting_for_player"}}); }
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning()) return;
    done=true; auto previousReport=_CrtSetReportHook(Report);
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        if(c.Name()=="CPed") for(auto& h:c.Items()) if(h->Name()=="CalculateNewVelocity") hook=h.get();
    });
    if(!hook) { Write({{"event","error"},{"reason","missing hook"}}); _CrtSetReportHook(previousReport); return; }
    const auto savedPlayer=Bytes(player,sizeof(CPlayerPed)); const float savedStep=CTimer::GetTimeStep(); const bool savedSlow=CTimer::bSlowMotionActive;
    const bool enabled=hook->Hooked(),locked=hook->Locked(); hook->LockState(false);
    bool passed=true; int pair=0; double maximum=0;
    int flat=-1,stairs=-1;
    for(int i=0;i<179;++i) { if(g_surfaceInfos.IsStairs(eSurfaceType(i))) stairs=i; else flat=i; }
    auto entry=reinterpret_cast<void(__thiscall*)(CPed*)>(0x5E4C50);
    for(int i=0;i<4096 && passed;++i) {
        PedInput a(player,i),b(player,i);
        a.ped.get()->m_nContactSurface=b.ped.get()->m_nContactSurface=eSurfaceType(i%2 ? stairs:flat);
        const auto beforeA=a.ped.bytes,beforeB=b.ped.bytes;
        CTimer::SetTimeStep(std::array{.001f,.01f,.5f,1.f,3.f}[(i/13)%5]); CTimer::bSlowMotionActive=(i/17)%2;
        hook->State(false); const auto start=PedCompletionProbe::calls[0]; entry(a.ped.get()); const auto middle=PedCompletionProbe::calls[0];
        hook->State(true); entry(b.ped.get()); const auto end=PedCompletionProbe::calls[0];
        const auto original=a.State(),candidate=b.State(); const bool bounded=a.Bounded(beforeA)&&b.Bounded(beforeB);
        passed=Equal(original,candidate,maximum)&&bounded&&middle==start&&end==middle+1;
        Write({{"event","comparison"},{"function","CalculateNewVelocity"},{"pair",pair++},{"input",i},{"original",original},{"candidate",candidate},{"equal",passed},{"bounded",bounded},{"original_candidate_calls",middle-start},{"candidate_calls",end-middle}});
    }
    if(passed) passed=Foot::Run(player,pair,maximum);
    if(passed) passed=Steps::Run(player,pair,maximum);
    if(passed) passed=Tyres::Run(player,pair,maximum);
    if(passed) passed=Position::Run(player,pair,maximum);
    if(passed) passed=Kill::Run(player,pair,maximum);
    CTimer::SetTimeStep(savedStep); CTimer::bSlowMotionActive=savedSlow; hook->State(enabled); hook->LockState(locked);
    const bool restored=Bytes(player,sizeof(CPlayerPed))==savedPlayer && CTimer::GetTimeStep()==savedStep && CTimer::bSlowMotionActive==savedSlow && hook->Hooked()==enabled && hook->Locked()==locked;
    Write({{"event",passed && restored ? "complete":"error"},{"pairs",pair},{"max_error",maximum},{"state_restored",restored}});
    _CrtSetReportHook(previousReport);
}
}
