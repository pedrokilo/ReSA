#pragma once
#include <fstream>
#include <crtdbg.h>
#include "CarGenerator.h"
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Probe.hpp"
namespace AttachedLive {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r){ { static DWORD pumpLast=0; const DWORD pumpNow=GetTickCount(); if(pumpNow-pumpLast>=250){ pumpLast=pumpNow; MSG pumpMsg; PeekMessageA(&pumpMsg,nullptr,0,0,PM_NOREMOVE); } }log<<r.dump()<<'\n';log.flush();}
inline int Report(int type,char* message,int*){if(type!=_CRT_ASSERT)return FALSE;Write({{"event","assertion"},{"message",message?message:""}});ExitProcess(91);}
inline bool Select(bool enabled){bool found{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category){if(category.Name()!="CPed")return;for(auto& h:category.Items())if(h->Name()=="PositionAttachedPed"){category.SetItemEnabled(h,enabled);found=h->Hooked()==enabled;}});return found;}
struct State {
    CMatrix matrix; CVector move,turn,offset;float rotation,limit;uint16 mode;ePedType type;CPhysical* parent;CEntity* contact;CEntity* standingOn;bool standing;
    State(CPed* p):matrix(p->GetMatrix()),move(p->m_vecMoveSpeed),turn(p->m_vecTurnSpeed),offset(p->m_vecTurretOffset),rotation(p->m_fCurrentRotation),limit(p->m_fTurretAngleB),mode(p->m_fTurretAngleA),type(p->m_nPedType),parent(p->m_pAttachedTo),contact(p->m_pContactEntity),standingOn(p->m_standingOnEntity),standing(p->bIsStanding){}
    void Restore(CPed* p)const{p->GetMatrix()=matrix;p->m_vecMoveSpeed=move;p->m_vecTurnSpeed=turn;p->m_vecTurretOffset=offset;p->m_fCurrentRotation=rotation;p->m_fTurretAngleB=limit;p->m_fTurretAngleA=mode;p->m_nPedType=type;p->m_pAttachedTo=parent;p->m_pContactEntity=contact;p->m_standingOnEntity=standingOn;p->bIsStanding=standing;}
};
inline json Snapshot(CPed* p){
    std::vector<float> numbers;auto add=[&](CVector v){numbers.insert(numbers.end(),{v.x,v.y,v.z});};
    auto& m=p->GetMatrix();add(m.GetRight());add(m.GetForward());add(m.GetUp());add(m.GetPosition());add(p->m_vecMoveSpeed);add(p->m_vecTurnSpeed);numbers.push_back(p->m_fCurrentRotation);
    return {{"numbers",numbers},{"standing",(bool)p->bIsStanding},{"contact",(uintptr_t)p->m_pContactEntity},{"standing_on",(uintptr_t)p->m_standingOnEntity}};
}
inline bool Compare(CPed* player,int pair,json context){
    const State initial(player);
    auto entry=reinterpret_cast<void(__thiscall*)(CPed*)>(0x5DFDF0);
    const auto before=AttachedProbe::calls;
    if(!Select(false)){Write({{"event","error"},{"reason","original hook selection"}});return false;}
    entry(player);const auto original=Snapshot(player);const auto middle=AttachedProbe::calls;
    initial.Restore(player);
    if(!Select(true)){Write({{"event","error"},{"reason","candidate hook selection"}});return false;}
    entry(player);const auto candidate=Snapshot(player);const auto after=AttachedProbe::calls;
    float maxError{};bool finite=true;
    for(size_t i=0;i<original["numbers"].size();++i){const auto a=original["numbers"][i].get<float>(),b=candidate["numbers"][i].get<float>();finite&=std::isfinite(a)&&std::isfinite(b);maxError=std::max(maxError,std::abs(a-b));}
    const bool equal=finite&&maxError<=0.00001f&&original["standing"]==candidate["standing"]&&original["contact"]==candidate["contact"]&&original["standing_on"]==candidate["standing_on"]&&middle==before&&after==middle+1;
    context.update({{"event","comparison"},{"pair",pair},{"equal",equal},{"max_error",maxError},{"original",original},{"candidate",candidate},{"original_candidate_calls",middle-before},{"candidate_calls",after-middle}});Write(context);
    initial.Restore(player);return equal;
}
inline void Update(){
    if(done)return;char path[MAX_PATH]{};GetModuleFileNameA(nullptr,path,MAX_PATH);const auto dir=std::filesystem::path(path).parent_path();
    if(!std::filesystem::exists(dir/"attached-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"attached-runtime.jsonl",std::ios::trunc);Write({{"event","waiting_for_player"}});_CrtSetReportHook(Report);}
    auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;
    Write({{"event","player_ready"},{"startup_candidate_calls",AttachedProbe::calls}});
    const State saved(player);int pair{};bool passed=true;int ladderCases{};
    player->m_pAttachedTo=nullptr;passed=Compare(player,pair++,{{"model",-1}});
    for(const auto model:{560,544}){
        if(!passed)break;
        CStreaming::RequestModel(model,STREAMING_GAME_REQUIRED);CStreaming::LoadAllRequestedModels(false);
        if(!CStreaming::IsModelLoaded(model)){Write({{"event","error"},{"reason","model missing"}});saved.Restore(player);return;}
        auto* parent=CCarGenerator::CreateVehicle(static_cast<eModelID>(model),MISSION_VEHICLE);
        if(!parent){Write({{"event","error"},{"reason","vehicle creation"}});saved.Restore(player);return;}
        parent->GetMatrix().SetRotateZ(0.7f);parent->GetMatrix().GetPosition()=CVector{25.f,-30.f,12.f};parent->m_vecMoveSpeed=CVector{0.2f,-0.3f,0.4f};parent->m_vecTurnSpeed=CVector{-0.01f,0.03f,0.02f};
        for(int entityType=0;entityType<3&&passed;++entityType)for(int mode=0;mode<5&&passed;++mode)for(int angle=0;angle<3&&passed;++angle)for(int offset=0;offset<2&&passed;++offset)for(int playerType=0;playerType<2&&passed;++playerType){
            saved.Restore(player);parent->SetType(entityType==0?ENTITY_TYPE_VEHICLE:entityType==1?ENTITY_TYPE_OBJECT:ENTITY_TYPE_PED);
            player->m_pAttachedTo=parent;player->m_standingOnEntity=parent;player->m_pContactEntity=parent;player->m_nPedType=playerType?PED_TYPE_PLAYER1:PED_TYPE_CIVMALE;player->m_fTurretAngleA=(uint16)mode;player->m_fTurretAngleB=0.4f;player->m_fCurrentRotation=angle==0?-6.f:angle==1?0.8f:6.f;player->m_vecTurretOffset=CVector{1.25f,-0.5f,offset?-999.f:2.f};player->bIsStanding=false;
            const bool ladder=model==544&&offset&&parent->AsAutomobile()->m_aCarNodes[CAR_MISC_B];if(ladder)++ladderCases;
            passed=Compare(player,pair++,{{"model",model},{"entity_type",entityType},{"mode",mode},{"angle",angle},{"offset",offset},{"player_type",playerType},{"ladder",ladder}});
        }
        saved.Restore(player);parent->SetTypeVehicle();delete parent;
    }
    saved.Restore(player);Select(true);
    Write(passed?json{{"event","complete"},{"pairs",pair},{"ladder_cases",ladderCases},{"state_restored",true}}:json{{"event","error"},{"reason","comparison mismatch"}});
}
}
