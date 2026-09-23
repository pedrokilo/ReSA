#pragma once
#include <fstream>
#include <crtdbg.h>
#include "CarGenerator.h"
#include "TaskSimpleSwim.h"
#include "TaskSimpleJetPack.h"
#include "TaskComplexUseSwatRope.h"
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Probe.hpp"
VALIDATE_OFFSET(CVehicle, m_nVehicleType, 0x590);
namespace UpdatePositionLive {
inline bool done{};inline std::ofstream log;
inline void Write(const json& r){log<<r.dump()<<'\n';log.flush();}
inline int Report(int type,char* message,int*){if(type!=_CRT_ASSERT)return FALSE;Write({{"event","assertion"},{"message",message?message:""}});ExitProcess(91);}
inline bool Select(bool enabled){bool found{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category){if(category.Name()!="CPed")return;for(auto& h:category.Items())if(h->Name()=="UpdatePosition"){category.SetItemEnabled(h,enabled);found=h->Hooked()==enabled;}});return found;}
struct State {
    CPed* p; CMatrixLink* matrixPtr; CMatrix matrix; CSimpleTransform placement;
    CVector move,turn,point,normal;CVector2D shift;float rotation,aim,step;ePedType type;ePedState pedState;eSurfaceType surface;CPhysical* attached;CEntity* standingOn;bool standing,fallen;eReplayMode replay;
    std::array<CTask*,TASK_PRIMARY_MAX> tasks;
    State(CPed* ped):p(ped),matrixPtr(p->m_matrix),matrix(p->GetMatrix()),placement(p->m_placement),move(p->m_vecMoveSpeed),turn(p->m_vecTurnSpeed),point(p->field_56C),normal(p->field_578),shift(p->m_vecAnimMovingShift),rotation(p->m_fCurrentRotation),aim(p->m_fAimingRotation),step(CTimer::GetTimeStep()),type(p->m_nPedType),pedState(p->m_nPedState),surface(p->m_nContactSurface),attached(p->m_pAttachedTo),standingOn(p->m_standingOnEntity),standing(p->bIsStanding),fallen(p->bFallenDown),replay(CReplay::Mode),tasks(p->GetTaskManager().m_aPrimaryTasks){}
    void Restore()const{p->m_matrix=matrixPtr;if(matrixPtr)*static_cast<CMatrix*>(matrixPtr)=matrix;p->m_placement=placement;p->m_vecMoveSpeed=move;p->m_vecTurnSpeed=turn;p->field_56C=point;p->field_578=normal;p->m_vecAnimMovingShift=shift;p->m_fCurrentRotation=rotation;p->m_fAimingRotation=aim;p->m_nPedType=type;p->m_nPedState=pedState;p->m_nContactSurface=surface;p->m_pAttachedTo=attached;p->m_standingOnEntity=standingOn;p->bIsStanding=standing;p->bFallenDown=fallen;CTimer::SetTimeStep(step);CReplay::Mode=replay;p->GetTaskManager().m_aPrimaryTasks=tasks;}
};
inline json Snapshot(CPed* p){
    std::vector<float> values;auto add=[&](CVector v){values.insert(values.end(),{v.x,v.y,v.z});};
    if(p->m_matrix){add(p->m_matrix->GetRight());add(p->m_matrix->GetForward());add(p->m_matrix->GetUp());add(p->m_matrix->GetPosition());}else{add(p->m_placement.m_vPosn);values.push_back(p->m_placement.m_fHeading);}
    add(p->m_vecMoveSpeed);add(p->m_vecTurnSpeed);values.push_back(p->m_fCurrentRotation);values.push_back(p->m_fAimingRotation);
    return {{"numbers",values},{"matrix",p->m_matrix!=nullptr},{"standing",(bool)p->bIsStanding},{"fallen",(bool)p->bFallenDown},{"support",(uintptr_t)p->m_standingOnEntity},{"attached",(uintptr_t)p->m_pAttachedTo}};
}
inline bool Compare(CPed* p,int pair,json context){
    const auto input=Snapshot(p);const auto move=p->m_vecMoveSpeed;const auto rot=p->m_fCurrentRotation,aim=p->m_fAimingRotation;const auto placement=p->m_placement;
    CMatrix matrix;if(p->m_matrix)matrix=*p->m_matrix;
    const auto before=UpdatePositionProbe::calls;
    if(!Select(false)){Write({{"event","error"},{"reason","original hook"}});return false;}
    auto entry=reinterpret_cast<void(__thiscall*)(CPed*)>(0x5E1B10);entry(p);const auto original=Snapshot(p);const auto middle=UpdatePositionProbe::calls;
    p->m_vecMoveSpeed=move;p->m_fCurrentRotation=rot;p->m_fAimingRotation=aim;p->m_placement=placement;if(p->m_matrix)*static_cast<CMatrix*>(p->m_matrix)=matrix;
    if(Snapshot(p)!=input){Write({{"event","error"},{"reason","input restore"}});return false;}
    if(!Select(true)){Write({{"event","error"},{"reason","candidate hook"}});return false;}
    entry(p);const auto candidate=Snapshot(p);const auto after=UpdatePositionProbe::calls;
    bool equal=original["numbers"].size()==candidate["numbers"].size();float maxError{};
    if(equal)for(size_t i=0;i<original["numbers"].size();++i){auto a=original["numbers"][i].get<float>(),b=candidate["numbers"][i].get<float>();equal&=std::isfinite(a)&&std::isfinite(b);maxError=std::max(maxError,std::abs(a-b));}
    for(const auto key:{"matrix","standing","fallen","support","attached"})equal&=original[key]==candidate[key];
    equal&=maxError<=0.00001f&&middle==before&&after==middle+1;
    context.update({{"event","comparison"},{"pair",pair},{"equal",equal},{"max_error",maxError},{"original",original},{"candidate",candidate},{"original_candidate_calls",middle-before},{"candidate_calls",after-middle}});Write(context);return equal;
}
inline void Update(){
    if(done)return;char path[MAX_PATH]{};GetModuleFileNameA(nullptr,path,MAX_PATH);const auto dir=std::filesystem::path(path).parent_path();
    if(!std::filesystem::exists(dir/"updateposition-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"updateposition-runtime.jsonl",std::ios::trunc);Write({{"event","waiting_for_player"}});_CrtSetReportHook(Report);}
    auto* p=FindPlayerPed();if(!p||CCutsceneMgr::IsRunning())return;done=true;
    p->GetMatrix();const State saved(p);Write({{"event","player_ready"}});
    CTaskSimpleSwim swim(nullptr,nullptr);CTaskSimpleJetPack jet;CTaskComplexUseSwatRope rope(0);
    int flat=-1,steep=-1;for(int i=0;i<179;++i){if(g_surfaceInfos.IsSteepSlope((eSurfaceType)i))steep=i;else flat=i;}
    if(flat<0||steep<0){Write({{"event","error"},{"reason","surface coverage"}});return;}
    std::array<CVehicle*,4> parents{};const int models[]={560,473,481,522};bool passed=true;int pair{};
    for(int i=0;i<4;++i){CStreaming::RequestModel(models[i],STREAMING_GAME_REQUIRED);CStreaming::LoadAllRequestedModels(false);if(!CStreaming::IsModelLoaded(models[i])){passed=false;break;}parents[i]=CCarGenerator::CreateVehicle((eModelID)models[i],MISSION_VEHICLE);if(!parents[i]){passed=false;break;}parents[i]->GetMatrix().SetRotateZ(0.4f);parents[i]->GetMatrix().GetPosition()=CVector{20.f,30.f,5.f};}
    for(int hasMatrix=0;hasMatrix<2&&passed;++hasMatrix){
        const auto reset=[&]{saved.Restore();CReplay::Mode=(eReplayMode)0;p->GetTaskManager().m_aPrimaryTasks.fill(nullptr);p->m_matrix=hasMatrix?saved.matrixPtr:nullptr;p->m_placement.m_vPosn=CVector{21.f,31.f,7.f};if(p->m_matrix)p->m_matrix->GetPosition()=p->m_placement.m_vPosn;p->m_nPedType=PED_TYPE_PLAYER1;p->m_nPedState=PEDSTATE_IDLE;p->bIsStanding=true;p->bFallenDown=false;p->m_pAttachedTo=nullptr;p->m_standingOnEntity=nullptr;p->m_nContactSurface=(eSurfaceType)flat;p->m_vecMoveSpeed=CVector{0.1f,-0.2f,0.3f};p->m_vecAnimMovingShift=CVector2D{0.7f,-0.6f};p->field_56C=CVector{1.2f,0.3f,-0.1f};p->field_578=CVector{};p->m_fCurrentRotation=0.7f;p->m_fAimingRotation=-0.4f;CTimer::SetTimeStep(1.f);};
        for(int c=0;c<6&&passed;++c){reset();p->bIsStanding=false;if(c==1)p->GetTaskManager().m_aPrimaryTasks[TASK_PRIMARY_PRIMARY]=&swim;if(c==2)p->GetTaskManager().m_aPrimaryTasks[TASK_PRIMARY_PRIMARY]=&jet;if(c==3)p->GetTaskManager().m_aPrimaryTasks[TASK_PRIMARY_PRIMARY]=&rope;if(c==4){p->bIsStanding=true;p->m_pAttachedTo=parents[0];}if(c==5){p->bIsStanding=true;CReplay::Mode=MODE_PLAYBACK;}passed=Compare(p,pair++,{{"group","early"},{"case",c},{"matrix_case",hasMatrix}});}
        for(int surface=0;surface<2&&passed;++surface)for(int normal=0;normal<3&&passed;++normal)for(int fallen=0;fallen<2&&passed;++fallen)for(int shift=0;shift<3&&passed;++shift)for(int step=0;step<3&&passed;++step){reset();p->m_nContactSurface=(eSurfaceType)(surface?steep:flat);p->field_578=normal==0?CVector{}:normal==1?CVector{0.6f,0.8f,0.f}:CVector{-0.8f,0.6f,0.f};p->bFallenDown=fallen;p->m_vecAnimMovingShift*=shift?1.f:-1.f;if(shift==2)p->m_vecAnimMovingShift=CVector2D{0.1f,-0.2f};CTimer::SetTimeStep(step==0?0.f:step==1?0.5f:2.f);passed=Compare(p,pair++,{{"group","ground"},{"surface",surface},{"normal",normal},{"fallen",fallen},{"shift",shift},{"step",step},{"matrix_case",hasMatrix}});}
        for(int model=0;model<4&&passed;++model)for(int playerType=0;playerType<2&&passed;++playerType)for(int dying=0;dying<2&&passed;++dying)for(int flags=0;flags<3&&passed;++flags)for(int attached=0;attached<2&&passed;++attached)for(int fast=0;fast<4&&passed;++fast)for(int step=0;step<3&&passed;++step){reset();auto* parent=parents[model];p->m_standingOnEntity=parent;p->m_nPedType=playerType?PED_TYPE_PLAYER1:PED_TYPE_CIVMALE;p->m_nPedState=dying?PEDSTATE_DIE:PEDSTATE_IDLE;parent->physicalFlags.bDisableCollisionForce=flags!=0;parent->physicalFlags.bCollidable=flags==2;parent->m_pAttachedTo=attached?parents[(model+1)%4]:nullptr;parent->m_vecMoveSpeed=fast==0?CVector{0.01f,0.f,0.f}:fast==1?CVector{0.2f,0.f,0.f}:fast==2?CVector{std::nextafter(0.2f,1.f),0.f,0.f}:CVector{0.4f,-0.3f,0.2f};parent->m_vecTurnSpeed=CVector{0.02f,-0.03f,0.04f};parent->m_vecFrictionMoveSpeed=CVector{0.01f,0.02f,0.03f};parent->m_vecFrictionTurnSpeed=CVector{0.02f,0.01f,0.f};CTimer::SetTimeStep(step==0?0.f:step==1?0.5f:2.f);passed=Compare(p,pair++,{{"group","support"},{"model",models[model]},{"player_type",playerType},{"dying",dying},{"flags",flags},{"parent_attached",attached},{"fast",fast},{"step",step},{"matrix_case",hasMatrix}});parent->m_pAttachedTo=nullptr;}
        if(passed){reset();auto* parent=parents[0];parent->SetTypeObject();parent->physicalFlags.bDisableCollisionForce=false;p->m_standingOnEntity=parent;passed=Compare(p,pair++,{{"group","object_type"},{"matrix_case",hasMatrix}});parent->SetTypeVehicle();}
    }
    saved.Restore();Select(true);for(auto* parent:parents)if(parent){parent->m_pAttachedTo=nullptr;delete parent;}
    Write(passed?json{{"event","complete"},{"pairs",pair},{"state_restored",true}}:json{{"event","error"},{"reason","comparison or model load failed"}});
}
}
