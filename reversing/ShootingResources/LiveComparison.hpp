#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexGoToPointShooting.h"
#include "ScriptResourceManager.h"
#include "References.h"
#include "Reference.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace SRLive {
inline bool done{};inline std::ofstream log;
inline void Write(const json& r){log<<r.dump()<<'\n';}
inline int Report(int t,char* msg,int*){if(t!=_CRT_ASSERT)return FALSE;Write({{"event","assertion"},{"message",msg?msg:""}});log.flush();ExitProcess(91);}
template<class T>struct Raw{alignas(T) std::array<uint8,sizeof(T)> b{};T* get(){return reinterpret_cast<T*>(b.data());}};
inline std::array<uint8,0x38> allocationImage{};
inline void* Alloc(size_t size){auto* p=GetTaskPool()->New();if(p)std::memcpy(p,allocationImage.data(),std::min(size,allocationImage.size()));return p;}
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"shooting-resources-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"shooting-resources-runtime.jsonl");Write({{"event","waiting_for_player"}});log.flush();}
    auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;auto report=_CrtSetReportHook(Report);
    using Hook=ReversibleHooks::ReversibleHook::Base;
    std::array<Hook*,7> hooks{};Hook* baseCtor{};
    const std::array names{"Clone","GetTaskType","Initialise","AddToResourceManager","RemoveFromResourceManager","HasResourceBeenRequested","Constructor"};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){for(auto& h:c.Items()){
        for(int n=0;n<7;++n)if(c.Name()==(n<2?"CTaskComplexGoToPointShooting":n==6?"CTaskComplexGoToPointAiming":"CScriptResourceManager")&&h->Name()==names[n])hooks[n]=h.get();
        if(c.Name()=="CTaskComplexGoToPointAiming"&&h->Name()=="Constructor")baseCtor=h.get();
    }});
    if(!baseCtor||std::ranges::any_of(hooks,[](auto* h){return !h;})){Write({{"event","error"},{"reason","missing hook"}});log.flush();_CrtSetReportHook(report);return;}
    auto before=ReversingProbe::Bytes(player,sizeof(CPlayerPed));
    const auto savedRefs=ReversingProbe::Bytes(CReferences::aRefs,sizeof(CReferences::aRefs));
    auto* empty=CReferences::pEmptyList;std::array<CReference,16> refs{};Raw<CEntity> entity;
    const auto poolUsed=GetTaskPool()->GetNoOfUsedSpaces();
    const auto be=baseCtor->Hooked(),bl=baseCtor->Locked();baseCtor->LockState(false);baseCtor->State(false);
    ReversingProbe::EntryPatch allocNative(0x61A5A0,Alloc);
    ReversingProbe::EntryPatch allocLocal(reinterpret_cast<uintptr_t>(&CTask::operator new),Alloc);
    const auto startup=SRProbe::calls;
    bool passed=true;int pairs=0;
    for(int fn=0;fn<7&&passed;++fn){
        auto* h=hooks[fn];const bool enabled=h->Hooked(),locked=h->Locked();h->LockState(false);
        Write({{"event","hook_state"},{"function",fn},{"enabled",fn==6?be:enabled},{"locked",fn==6?bl:locked},{"startup_calls",startup[fn]}});
        int total=fn<2?1024:(fn==2||fn==6)?2048:4096;
        for(int i=0;i<total&&passed;++i){
            Raw<CTaskComplexGoToPointShooting> source;alignas(4) std::array<uint8,0x384+32> table{};
            const auto run=[&]{
                for(int j=0;j<16;++j)refs[j]={j==15?nullptr:&refs[j+1],nullptr};
                CReferences::pEmptyList=refs.data();entity.b.fill(0);entity.get()->SetTypeObject();json out;
                if(fn<2||fn==6){
                    source.b.fill(uint8(i));auto* s=source.get();
                    s->m_moveState=eMoveState(i%8);s->m_aimAtEntity=i%2?entity.get():nullptr;
                    s->m_aimPos=CVector{float(i%31)-15.f,float(i%47)*.125f,-float(i%13)};
                    s->m_movePos=CVector{float(i)-500.f,float(i%7),float(i%101)*.25f};
                    s->m_moveTargetRadius=float(i%17)*.125f;s->m_slowDownDistance=float(i%67)*.5f;
                    CEntity::SafeRegisterRef(s->m_aimAtEntity);auto sourceBefore=source.b;
                    allocationImage=source.b;
                    if(i%3==0)*reinterpret_cast<float*>(allocationImage.data()+0x20)+=1.f;
                    if(i%3==1)*reinterpret_cast<float*>(allocationImage.data()+0x2C)+=1.f;
                    allocationImage[0x34]=uint8((i/3)&1);
                    Raw<CTaskComplexGoToPointAiming> constructed;constructed.b=allocationImage;
                    if(fn==1)out={{"type",int(reinterpret_cast<eTaskType(__thiscall*)(const CTask*)>(0x668CC0)(s))}};
                    else{
                        CTask* clone{};
                        if(fn==6){
                            clone=reinterpret_cast<CTask*(__thiscall*)(CTask*,eMoveState,const CVector&,CEntity*,CVector,float,float)>(0x668790)(constructed.get(),s->m_moveState,s->m_movePos,s->m_aimAtEntity,s->m_aimPos,s->m_moveTargetRadius,s->m_slowDownDistance);
                        }else clone=reinterpret_cast<CTask*(__thiscall*)(const CTask*)>(0x66CE10)(s);
                        if(!clone)return json{{"allocated",false}};
                        auto* c=static_cast<CTaskComplexGoToPointAiming*>(clone);
                        int count=0;for(auto* ref=entity.get()->m_pReferences;ref;ref=ref->m_pNext)++count;
                        out={{"allocated",true},{"type",int(c->GetTaskType())},{"move",int(c->m_moveState)},{"aim_entity",c->m_aimAtEntity==s->m_aimAtEntity},{"aim",std::array{c->m_aimPos.x,c->m_aimPos.y,c->m_aimPos.z}},{"pos",std::array{c->m_movePos.x,c->m_movePos.y,c->m_movePos.z}},{"radius",c->m_moveTargetRadius},{"slow",c->m_slowDownDistance},{"new_target",bool(c->m_newTargetSet)},{"parent_null",c->GetParent()==nullptr},{"subtask_null",c->GetSubTask()==nullptr},{"refs",count},{"one_allocation",GetTaskPool()->GetNoOfUsedSpaces()==poolUsed+(fn==6?0:1)}};
                        if(fn==6)static_cast<CTaskComplexGoToPointAiming*>(clone)->~CTaskComplexGoToPointAiming();else delete clone;out["pool_restored"]=GetTaskPool()->GetNoOfUsedSpaces()==poolUsed;
                    }
                    out["source_unchanged"]=sourceBefore==source.b;
                    CEntity::SafeCleanUpRef(s->m_aimAtEntity);out["refs_clean"]=entity.get()->m_pReferences==nullptr;
                }else{
                    for(size_t j=0;j<table.size();++j)table[j]=uint8(i*29+j*17);
                    auto* manager=reinterpret_cast<CScriptResourceManager*>(table.data()+16);
                    constexpr std::array<int32,8> types{0,1,2,3,65535,65536,65537,-1};
                    const auto type=eScriptResourceType(types[(i/7)%types.size()]);
                    const int id=i%11-2;
                    auto* script=reinterpret_cast<CRunningScript*>(uintptr_t((i%5)*0x1000));
                    for(int j=0;j<75;++j){
                        auto& r=manager->m_aScriptResources[j];
                        r.m_nModelId=(j+i)%11-2;r.m_pThread=reinterpret_cast<void*>(uintptr_t(((j+i/5)%5)*0x1000));r.m_nType=uint16((j+i/13)%4);
                        if(i%8==0)r.m_nType=1;
                        if(i%8==1)r.m_nType=0;
                        if(i%8>=2&&j%(i%9+1)==0){r.m_nModelId=id;r.m_nType=uint16(type);r.m_pThread=j%2?script:reinterpret_cast<void*>(0x9000);}
                    }
                    auto initial=table;int value=-1;
                    if(fn==2)reinterpret_cast<void(__thiscall*)(CScriptResourceManager*)>(0x470480)(manager);
                    if(fn==3)reinterpret_cast<void(__thiscall*)(CScriptResourceManager*,int32,eScriptResourceType,CRunningScript*)>(0x4704B0)(manager,id,type,script);
                    if(fn==4)value=reinterpret_cast<bool(__thiscall*)(CScriptResourceManager*,int32,eScriptResourceType,CRunningScript*)>(0x470510)(manager,id,type,script);
                    if(fn==5)value=reinterpret_cast<bool(__thiscall*)(CScriptResourceManager*,int32,eScriptResourceType)>(0x470620)(manager,id,type);
                    bool guards=std::equal(table.begin(),table.begin()+16,initial.begin())&&std::equal(table.end()-16,table.end(),initial.end()-16);
                    out={{"result",value},{"bytes",table},{"guards",guards},{"query_unchanged",fn!=5||table==initial}};
                }
                return out;
            };
            h->State(false);auto a=SRProbe::calls[fn];auto original=run();auto b=SRProbe::calls[fn];
            h->State(true);auto candidate=run();auto c=SRProbe::calls[fn];
            passed=original==candidate&&a==b&&c==b+1;
            if(fn<2||fn==6)passed=passed&&original["type"]==int(fn==6?CTaskComplexGoToPointAiming::Type:CTaskComplexGoToPointShooting::Type)&&original["source_unchanged"]==true&&original["refs_clean"]==true;
            if(fn==0)passed=passed&&original["one_allocation"]==true&&original["pool_restored"]==true&&original["parent_null"]==true&&original["subtask_null"]==true;
            if(fn>=2&&fn!=6)passed=passed&&original["guards"]==true&&original["query_unchanged"]==true;
            Write({{"event","comparison"},{"pair",pairs++},{"function",fn},{"case",i},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b}});
        }
        h->State(enabled);h->LockState(locked);
    }
    CReferences::pEmptyList=empty;baseCtor->State(be);baseCtor->LockState(bl);
    bool restored=before==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&savedRefs==ReversingProbe::Bytes(CReferences::aRefs,sizeof(CReferences::aRefs))&&GetTaskPool()->GetNoOfUsedSpaces()==poolUsed;
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});log.flush();_CrtSetReportHook(report);
}
}

