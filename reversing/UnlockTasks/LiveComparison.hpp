#pragma once
#include <fstream>
#include <crtdbg.h>
#include <rtcapi.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexDestroyCarArmed.h"
#include "TaskSimpleThrowControl.h"
#include "TaskSimpleThrowProjectile.h"
#include "TaskSimpleGunControl.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "References.h"
#include "Reference.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace UTLive {
inline bool done{}; inline std::ofstream log;
inline int currentFunction=-1,currentCase=-1;inline bool candidatePhase{};
inline void Write(const json& r){log<<r.dump()<<'\n';}
inline int __cdecl RuntimeError(int type,const wchar_t*,int line,const wchar_t*,const wchar_t*,...) {
    void* frames[32]{};const auto n=CaptureStackBackTrace(0,32,frames,nullptr);json stack=json::array();
    HMODULE module{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&RuntimeError),&module);
    for(int j=0;j<n;++j)stack.push_back(reinterpret_cast<uintptr_t>(frames[j]));
    Write({{"event","error"},{"reason","runtime check"},{"type",type},{"line",line},{"function",currentFunction},{"case",currentCase},{"candidate_phase",candidatePhase},{"module",reinterpret_cast<uintptr_t>(module)},{"stack",stack}});log.flush();ExitProcess(92);
}
template<class T>struct Raw{alignas(T) std::array<uint8,sizeof(T)> b{};T* get(){return reinterpret_cast<T*>(b.data());}};
inline CWeaponInfo info{};
inline CWeaponInfo* Info(eWeaponType,eWeaponSkill){return &info;}
inline int RefCount(CEntity* e){int n=0;for(auto* r=e->m_pReferences;r;r=r->m_pNext)++n;return n;}
struct FakeTask:CTaskSimple {
    int aborted{};
    CTask* Clone()const override{return nullptr;} eTaskType GetTaskType()const override{return TASK_NONE;}
    bool MakeAbortable(CPed*,eAbortPriority p,const CEvent* e)override{aborted=1+int(p)+(e?100:0);return true;}
    bool ProcessPed(CPed*)override{return false;}
};
inline json Vec(const CVector& v){return ReversingProbe::Bytes(&v,sizeof(v));}
inline json State(CTask* t,CVehicle* car){
    if(!t)return nullptr;
    const auto* p=reinterpret_cast<uint8*>(t);
    json r={{"type",int(t->GetTaskType())},{"parent_null",!t->GetParent()}};
    switch(t->GetTaskType()){
    case TASK_SIMPLE_PAUSE:r["duration"]=*reinterpret_cast<const int32*>(p+0x14);break;
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:{auto* v=static_cast<CTaskComplexGoToPointAndStandStill*>(t);r["fields"]={int(v->m_moveState),Vec(v->m_vecTargetPoint),v->m_fRadius,v->m_fMoveStateRadius,bool(v->m_bMustOverShootTarget),bool(v->m_bStopExactly)};break;}
    case TASK_COMPLEX_SEEK_ENTITY:r["entity"]=*reinterpret_cast<CEntity*const*>(p+0xC)==car;r["fields"]=ReversingProbe::Bytes(p+0x10,24);r["move"]=*reinterpret_cast<const int32*>(p+0x44);r["flags"]=p[0x48]&15;break;
    case TASK_SIMPLE_THROW_CTRL:{auto* v=static_cast<CTaskSimpleThrowControl*>(t);r["fields"]={v->m_entity==car,Vec(v->m_pos),v->byte8,v->m_isAttacking};break;}
    case TASK_SIMPLE_GUN_CTRL:{auto* v=static_cast<CTaskSimpleGunControl*>(t);r["fields"]={v->m_targetEntity==car,Vec(v->m_targetPos),Vec(v->m_moveTarget),int(v->m_firingTask),v->m_burstAmmoCnt,v->m_leisureDurMs};break;}
    case TASK_SIMPLE_THROW_PROJECTILE:{auto* v=static_cast<CTaskSimpleThrowProjectile*>(t);r["fields"]={v->m_TargetEntity==car,Vec(v->m_TargetPos),v->m_bIsFinished,v->m_bStartThrowFinished,v->m_bButtonReleased,v->m_ButtonCounter};break;}
    default:break;
    }return r;
}
inline void Update(){
    if(done)return;_set_error_mode(_OUT_TO_STDERR);_set_abort_behavior(0,_WRITE_ABORT_MSG|_CALL_REPORTFAULT);
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"unlock-tasks-runtime.start")){done=true;return;}
    if(!log.is_open()){_RTC_SetErrorFuncW(RuntimeError);_CrtSetReportMode(_CRT_ASSERT,_CRTDBG_MODE_FILE);_CrtSetReportMode(_CRT_ERROR,_CRTDBG_MODE_FILE);log.open(dir/"unlock-tasks-runtime.jsonl");Write({{"event","waiting_for_player"}});log.flush();}
    auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;
    using Hook=ReversibleHooks::ReversibleHook::Base;
    const std::array names{"CalculateSearchPositionAndRanges","CreateSubTask","ProcessPed"};
    const std::array<uintptr_t,3> addresses{0x628C80,0x628DA0,0x61F9F0};
    std::array<Hook*,3> hooks{};std::vector<std::tuple<Hook*,bool,bool>> deps;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){for(auto& h:c.Items()){
        for(int n=0;n<3;++n)if(c.Name()==(n<2?"CTaskComplexDestroyCarArmed":"CTaskSimpleThrowControl")&&h->Name()==names[n])hooks[n]=h.get();
        if(h->Name()=="Constructor"&&(c.Name()=="CTaskSimplePause"||c.Name()=="CTaskSimpleThrowControl"||c.Name()=="CTaskSimpleThrowProjectile"||c.Name()=="CTaskSimpleGunControl"||c.Name()=="CTaskComplexSeekEntityStandard"||c.Name()=="CTaskComplexGoToPointAndStandStill"))deps.emplace_back(h.get(),h->Hooked(),h->Locked());
    }});
    if(std::ranges::any_of(hooks,[](auto* h){return !h;})){Write({{"event","error"},{"reason","missing hook"}});log.flush();return;}
    const auto playerBefore=ReversingProbe::Bytes(player,sizeof(CPlayerPed));
    const auto refsBefore=ReversingProbe::Bytes(CReferences::aRefs,sizeof(CReferences::aRefs));auto* emptyBefore=CReferences::pEmptyList;
    const auto usedBefore=GetTaskPool()->GetNoOfUsedSpaces();std::array<CReference,64> refs{};
    for(auto [h,e,l]:deps){h->LockState(false);h->State(false);}
    bool passed=true;int pairs=0;
    {
    ReversingProbe::EntryPatch in(0x743C60,Info),il(reinterpret_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)),Info);
    Raw<CVehicle> car;Raw<CPed> ped;Raw<CPedIntelligence> intel;Raw<CMatrixLink> carMat,pedMat;
    Raw<CTaskComplexDestroyCarArmed> armed;Raw<CTaskSimpleThrowControl> control;Raw<CTaskSimpleThrowProjectile> projectile;FakeTask wrong;
    for(int fn=0;fn<3&&passed;++fn){
        auto* h=hooks[fn];const bool enabled=h->Hooked(),locked=h->Locked();h->LockState(false);
        Write({{"event","hook_state"},{"function",fn},{"enabled",enabled},{"locked",locked},{"startup_calls",UTProbe::calls[fn]}});
        const int count=fn==0?4096:fn==1?1024:2048;
        for(int i=0;i<count&&passed;++i){
            if(fn==1&&i==512)for(auto[h,e,l]:deps)h->State(true);
            const auto run=[&]{
                // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot
                { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0; }
                for(int j=0;j<64;++j)refs[j]={j==63?nullptr:&refs[j+1],nullptr};CReferences::pEmptyList=refs.data();
                car.b.fill(0);ped.b.fill(0);intel.b.fill(0);armed.b.fill(0);control.b.fill(0);projectile.b.fill(0);carMat.b.fill(0);pedMat.b.fill(0);wrong.aborted=0;
                car.get()->SetTypeVehicle();ped.get()->SetTypePed();ped.get()->m_nPedType=PED_TYPE_CIVMALE;
                *reinterpret_cast<CPedIntelligence**>(ped.b.data()+0x47C)=intel.get();intel.get()->m_TaskMgr.m_pPed=ped.get();
                auto* a=armed.get();a->m_VehToDestroy=car.get();a->m_VehiclePos=CVector{float(i)*.03125f,-float(i%11),float(i%7)};
                ped.get()->SetPosn(CVector{float(i%97)*.125f,-float(i%53)*.25f,float(i%11)*.5f});
                car.get()->SetPosn(CVector{float(i%193)*.375f,float(i%29)*.25f,-float(i%7)*.5f});
                if(i&1){carMat.get()->GetPosition()=car.get()->GetPosition();*reinterpret_cast<CMatrixLink**>(car.b.data()+0x14)=carMat.get();}
                if(i&2){pedMat.get()->GetPosition()=ped.get()->GetPosition();*reinterpret_cast<CMatrixLink**>(ped.b.data()+0x14)=pedMat.get();}
                ped.get()->GetActiveWeapon().m_Type=WEAPON_PISTOL;info.m_fWeaponRange=float(i%257)*.0625f;info.flags.bThrow=(i/8)%2;
                json out;
                if(fn==0){
                    reinterpret_cast<void(__thiscall*)(void*,CPed*)>(addresses[fn])(a,ped.get());
                    out={{"task",armed.b}};
                }else if(fn==1){
                    const auto type=std::array{TASK_SIMPLE_GUN_CTRL,TASK_COMPLEX_SEEK_ENTITY,TASK_SIMPLE_PAUSE,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_NONE,TASK_FINISHED,TASK_SIMPLE_GUN_CTRL,TASK_SIMPLE_GUN_CTRL}[i%8];
                    auto* task=reinterpret_cast<CTask*(__thiscall*)(void*,eTaskType,CPed*)>(addresses[fn])(a,type,ped.get());
                    out={{"task",State(task,car.get())},{"refs",RefCount(car.get())},{"pool_delta",GetTaskPool()->GetNoOfUsedSpaces()-usedBefore}};
                    delete task;
                }else{
                    auto* c=control.get();*reinterpret_cast<uintptr_t*>(c)=0x86D7B4;c->byte8=i%13==0;c->m_isAttacking=(i/6)%2;
                    c->m_entity=i%3?car.get():nullptr;c->m_pos=i%4?CVector{float(i%17),-float(i%19),float(i%7)}:CVector{};
                    auto* t=projectile.get();*reinterpret_cast<uintptr_t*>(t)=0x86D790;t->m_bIsFinished=(i/12)%2;t->m_TargetPos=CVector{-1.f,-2.f,-3.f};
                    auto*& slot=intel.get()->m_TaskMgr.m_aSecondaryTasks[TASK_SECONDARY_ATTACK];
                    slot=i%6<2?nullptr:i%6<4?static_cast<CTask*>(t):&wrong;
                    const bool result=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(addresses[fn])(c,ped.get());
                    out={{"result",result},{"control",control.b},{"heading",ReversingProbe::Bytes(&ped.get()->m_fAimingRotation,4)},{"move",int(ped.get()->m_nMoveState)},{"secondary",State(slot,car.get())},{"abort",wrong.aborted},{"refs",RefCount(car.get())},{"pool_delta",GetTaskPool()->GetNoOfUsedSpaces()-usedBefore}};
                    if(slot&&slot!=t&&slot!=&wrong)delete slot;slot=nullptr;
                }
                out["refs_clean"]=car.get()->m_pReferences==nullptr;out["pool_restored"]=GetTaskPool()->GetNoOfUsedSpaces()==usedBefore;
                return out;
            };
            currentFunction=fn;currentCase=i;candidatePhase=false;
            h->State(false);const auto ca=UTProbe::calls[fn];const auto original=run();const auto cb=UTProbe::calls[fn];
            candidatePhase=true;h->State(true);const auto candidate=run();const auto cc=UTProbe::calls[fn];
            passed=original==candidate&&ca==cb&&cc==cb+1&&original["refs_clean"]==true&&original["pool_restored"]==true;
            Write({{"event","comparison"},{"function",fn},{"case",i},{"pair",pairs++},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",cb-ca},{"candidate_calls",cc-cb}});
        }
        h->State(enabled);h->LockState(locked);log.flush();
    }
    }
    CReferences::pEmptyList=emptyBefore;for(auto[h,e,l]:deps){h->State(e);h->LockState(l);}
    const bool restored=playerBefore==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&refsBefore==ReversingProbe::Bytes(CReferences::aRefs,sizeof(CReferences::aRefs))&&GetTaskPool()->GetNoOfUsedSpaces()==usedBefore;
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});log.flush();
}
}
