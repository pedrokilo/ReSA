#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexWander.h"
#include "TaskSimpleScratchHead.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleCarDriveTimed.h"
#include "TaskComplexCrossRoadLookAndAchieveHeading.h"
#include "TaskComplexObserveTrafficLightsAndAchieveHeading.h"
#include "Probe.hpp"
namespace WanderLive {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& r) { log << r.dump() << '\n'; log.flush(); }
inline int Report(int type, char* msg, int*) {
    if (type != _CRT_ASSERT) return FALSE;
    Write({{"event","assertion"},{"message",msg ? msg : ""}}); ExitProcess(91);
}
inline json Node(const CNodeAddress& n) { return {n.m_wAreaId,n.m_wNodeId}; }
struct InputTask : CTaskSimpleScratchHead {
    eTaskType type;
    explicit InputTask(eTaskType t) : type(t) {}
    eTaskType GetTaskType() const override { return type; }
};
struct Wander : CTaskComplexWander {
    int mode;
    Wander(int i, eTaskType type) : CTaskComplexWander(eMoveState(4+i%4), uint8(i%2 ? 255 : i%8), true, float(i%5)*.25f), mode(i%8) {
        m_nFlags=uint8((i/8)%32); m_LastNode={1,10}; m_NextNode={1,20};
        m_pSubTask=new InputTask(type);
    }
    CTask* Clone() const override { return nullptr; }
    eWanderType GetWanderType() override { return WANDER_TYPE_STANDARD; }
    void ScanForStuff(CPed*) override {}
    CTask* CreateFirstSubTask(CPed*) override { WanderProbe::events.push_back({"first"}); return new CTaskSimpleScratchHead(); }
    void UpdateDir(CPed*) override {
        WanderProbe::events.push_back({"dir",m_nDir});
        if (mode==7) { m_LastNode={2,30}; m_nDir=3; }
        ++m_nLastUpdateDirFrameCount;
    }
    void UpdatePathNodes(const CPed*, uint8 dir, CNodeAddress& last, CNodeAddress& next, uint8& out) override {
        WanderProbe::events.push_back({"nodes",dir,Node(last),Node(next)});
        const auto previous=last; last=next;
        next=mode==0 || mode==7 ? previous : mode==1 ? last : CNodeAddress{2,40};
        if (mode==2) next.m_wAreaId=65535;
        if (mode==3) last.m_wAreaId=65535;
        if (mode==4) { last={65535,40}; next=previous; }
        out=uint8(dir+1);
    }
    json State() const { return {m_nMoveState,m_nDir,m_fTargetRadius,Node(m_LastNode),Node(m_NextNode),m_nLastUpdateDirFrameCount,m_nFlags}; }
};
inline json TaskState(CTask* t) {
    if (!t) return nullptr;
    json r={{"type",int(t->GetTaskType())},{"parent_null",t->GetParent()==nullptr}};
    switch(t->GetTaskType()) {
    case TASK_COMPLEX_SEQUENCE: {
        auto* s=static_cast<CTaskComplexSequence*>(t); json children=json::array();
        for(auto* c:s->m_Tasks) children.push_back(TaskState(c));
        r["children"]=children; r["state"]={s->m_CurrTaskIdx,int(s->m_RepeatMode),s->m_RepeatedCnt,s->m_bFlushTasks,s->m_RefCnt}; break;
    }
    case TASK_SIMPLE_GO_TO_POINT: {
        auto* s=static_cast<CTaskSimpleGoToPoint*>(t);
        r["state"]={s->m_moveState,s->m_vecTargetPoint.x,s->m_vecTargetPoint.y,s->m_vecTargetPoint.z,s->m_fRadius,int(s->m_GoToPointFlags&31),int(s->m_GoToFlags&63)}; break;
    }
    case TASK_SIMPLE_STAND_STILL: {
        auto* s=static_cast<CTaskSimpleStandStill*>(t); r["state"]={s->m_nTime,s->m_bLooped,s->m_bUseAnimIdleStance,s->m_fBlendData}; break;
    }
    case TASK_SIMPLE_NAMED_ANIM: {
        auto* s=static_cast<CTaskSimpleRunAnim*>(t); r["state"]={s->m_nAnimGroup,s->m_nAnimId,s->m_fBlendDelta,s->m_nTaskType}; break;
    }
    case TASK_SIMPLE_CAR_DRIVE_TIMED: {
        auto* s=static_cast<CTaskSimpleCarDriveTimed*>(t); r["state"]={s->m_nTime,s->m_pVehicle==nullptr}; break;
    }
    case TASK_COMPLEX_CROSS_ROAD_LOOK_AND_ACHIEVE_HEADING: {
        auto* s=static_cast<CTaskComplexCrossRoadLookAndAchieveHeading*>(t); r["state"]={s->m_nDuration,s->m_fTargetHeading}; break;
    }
    case TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS_AND_ACHIEVE_HEADING: {
        auto* s=static_cast<CTaskComplexObserveTrafficLightsAndAchieveHeading*>(t); r["state"]={s->m_nDurationInMs,s->m_fTargetHeading}; break;
    }
    }
    return r;
}
inline void Update() {
    if(done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"wander-runtime.start")) { done=true; return; }
    if(!log.is_open()) { log.open(dir/"wander-runtime.jsonl"); Write({{"event","waiting_for_player"}}); }
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning()) return;
    done=true; auto previousReport=_CrtSetReportHook(Report);
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        if(c.Name()=="CTaskComplexWander") for(auto& h:c.Items()) if(h->Name()=="CreateNextSubTask") hook=h.get();
    });
    if(!hook) { Write({{"event","error"},{"reason","missing hook"}}); _CrtSetReportHook(previousReport); return; }
    bool enabled=hook->Hooked(),locked=hook->Locked(); hook->LockState(false);
    alignas(CPlayerPed) std::array<uint8,sizeof(CPlayerPed)> a{},b{},saved{};
    std::memcpy(saved.data(),player,saved.size());
    Write({{"event","player_ready"},{"independent_inputs",true}});
    bool passed=true; int pair=0;
    const std::array types{TASK_SIMPLE_SCRATCH_HEAD,TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS_AND_ACHIEVE_HEADING,TASK_COMPLEX_CROSS_ROAD_LOOK_AND_ACHIEVE_HEADING,TASK_SIMPLE_CAR_DRIVE_TIMED,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_SEQUENCE,TASK_SIMPLE_GO_TO_POINT,TASK_COMPLEX_IN_AIR_AND_LAND,TASK_COMPLEX_SIT_DOWN_THEN_IDLE_THEN_STAND_UP,TASK_NONE};
    auto entry=reinterpret_cast<CTask*(__thiscall*)(CTaskComplexWander*,CPed*)>(0x674140);
    WanderProbe::recording=true;
    for(auto type:types) for(int i=0;i<1024 && passed;++i) {
        std::memcpy(a.data(),saved.data(),a.size()); auto* pa=reinterpret_cast<CPlayerPed*>(a.data());
        pa->bInVehicle=(i&32)!=0; pa->m_pVehicle=nullptr;
        b=a; auto* pb=reinterpret_cast<CPlayerPed*>(b.data()); const auto initial=a;
        Wander wa(i,type),wb(i,type); WanderProbe::light=(i&256)!=0; WanderProbe::road=(i&512)!=0;
        hook->State(false); WanderProbe::events=json::array(); auto start=WanderProbe::calls;
        auto* ra=entry(&wa,pa); const auto original=json{{"task",TaskState(ra)},{"state",wa.State()},{"events",WanderProbe::events}};
        const auto middle=WanderProbe::calls;
        hook->State(true); WanderProbe::events=json::array(); auto* rb=entry(&wb,pb);
        const auto candidate=json{{"task",TaskState(rb)},{"state",wb.State()},{"events",WanderProbe::events}};
        auto end=WanderProbe::calls;
        const bool unchanged=a==initial && b==initial;
        passed=original==candidate && unchanged && middle==start && end==middle+1 && (!ra || ra!=rb);
        Write({{"event","comparison"},{"pair",pair++},{"input_type",int(type)},{"input",i},{"original",original},{"candidate",candidate},{"original_candidate_calls",middle-start},{"candidate_calls",end-middle},{"inputs_unchanged",unchanged},{"equal",passed}});
        delete ra; delete rb;
    }
    WanderProbe::recording=false; hook->State(enabled); hook->LockState(locked);
    const bool restored=std::memcmp(player,saved.data(),saved.size())==0 && hook->Hooked()==enabled && hook->Locked()==locked;
    Write({{"event",passed && restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
    _CrtSetReportHook(previousReport);
}
}
