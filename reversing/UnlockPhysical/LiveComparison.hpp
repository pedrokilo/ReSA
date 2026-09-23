#pragma once
#include <fstream>
#include <crtdbg.h>
#include <rtcapi.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Pools/EntryInfoNodePool.h"
#include "Pools/PtrNodeDoubleLinkPool.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace UPLive {
inline bool done{};inline std::ofstream log;inline int currentFunction=-1,currentCase=-1;
inline void Write(const json& r){log<<r.dump()<<'\n';}
inline int __cdecl Error(int,const wchar_t*,int,const wchar_t*,const wchar_t*,...){Write({{"event","error"},{"reason","runtime check"},{"function",currentFunction},{"case",currentCase}});log.flush();ExitProcess(92);}
inline void __cdecl Assertion(const wchar_t* expression,const wchar_t* file,unsigned line){
    char expr[1024]{},path[1024]{};WideCharToMultiByte(CP_UTF8,0,expression,-1,expr,1024,nullptr,nullptr);WideCharToMultiByte(CP_UTF8,0,file,-1,path,1024,nullptr,nullptr);
    Write({{"event","assertion"},{"expression",expr},{"file",path},{"line",line},{"function",currentFunction},{"case",currentCase}});log.flush();ExitProcess(93);
}
inline CRect bounds;inline json trace;
struct Physical:CPhysical { CRect GetBoundRect() const override {return bounds;} };
inline void __fastcall EntityAdd(CEntity*,void*){trace.push_back("entity_add");}
inline void __fastcall EntityRemove(CEntity*,void*){trace.push_back("entity_remove");}
inline void Update(){
    if(done)return;_set_error_mode(_OUT_TO_STDERR);_set_abort_behavior(0,_WRITE_ABORT_MSG|_CALL_REPORTFAULT);
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"unlock-physical-runtime.start")){done=true;return;}
    if(!log.is_open()){_RTC_SetErrorFuncW(Error);_CrtSetReportMode(_CRT_ASSERT,_CRTDBG_MODE_FILE);_CrtSetReportMode(_CRT_ERROR,_CRTDBG_MODE_FILE);log.open(dir/"unlock-physical-runtime.jsonl");Write({{"event","waiting_for_player"}});log.flush();}
    auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;
    ReversingProbe::EntryPatch assertPatch(reinterpret_cast<uintptr_t>(&_wassert),Assertion);
    using Hook=ReversibleHooks::ReversibleHook::Base;std::array<Hook*,2> hooks{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CPhysical")for(auto& h:c.Items()){if(h->Name()=="Add")hooks[0]=h.get();if(h->Name()=="RemoveAndAdd")hooks[1]=h.get();}});
    if(!hooks[0]||!hooks[1]){Write({{"event","error"},{"reason","missing hook"}});log.flush();return;}
    const auto playerBefore=ReversingProbe::Bytes(player,sizeof(CPlayerPed));
    auto& sectors=CWorld::ms_aRepeatSectors;const auto sectorsBefore=ReversingProbe::Bytes(&sectors,sizeof(sectors));
    auto& entryGlobal=*reinterpret_cast<CEntryInfoNodePool**>(0xB7448C);auto* entryBefore=entryGlobal;
    auto& ptrGlobal=*reinterpret_cast<CPtrNodeDoubleLinkPool**>(0xB74488);auto* ptrBefore=ptrGlobal;
    alignas(4) std::array<uint8,64*20> entries;std::array<uint8,64> entryFlags;
    alignas(4) std::array<uint8,128*12> pointers;std::array<uint8,128> ptrFlags;
    CEntryInfoNodePool entryPool(64,entries.data(),entryFlags.data());CPtrNodeDoubleLinkPool ptrPool(128,pointers.data(),ptrFlags.data());
    const auto entryImage=ReversingProbe::Bytes(&entryPool,sizeof(entryPool)),ptrImage=ReversingProbe::Bytes(&ptrPool,sizeof(ptrPool));
    entryGlobal=&entryPool;ptrGlobal=&ptrPool;
    bool passed=true;int pairs=0;
    {
    HMODULE module{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&Update),&module);
    const auto local=[&](const char* name){auto p=reinterpret_cast<uintptr_t>(GetProcAddress(module,name));if(!p){Write({{"event","error"},{"reason",name}});log.flush();ExitProcess(94);}if(*reinterpret_cast<uint8*>(p)==0xE9)p+=5+*reinterpret_cast<int32*>(p+1);return p;};
    ReversingProbe::EntryPatch an(0x533020,EntityAdd),al(local("?Add@CEntity@@UAEXXZ"),EntityAdd);
    ReversingProbe::EntryPatch rn(0x534AE0,EntityRemove),rl(local("?Remove@CEntity@@UAEXXZ"),EntityRemove);
    Physical entity;const auto entityImage=ReversingProbe::Bytes(&entity,sizeof(entity));
    for(int fn=0;fn<2&&passed;++fn){
        auto* h=hooks[fn];const bool enabled=h->Hooked(),locked=h->Locked();h->LockState(false);
        Write({{"event","hook_state"},{"function",fn},{"enabled",enabled},{"locked",locked},{"startup_calls",UPProbe::calls[fn]}});
        for(int i=0;i<(fn?2048:1024)&&passed;++i){
            const auto run=[&]{
                std::memcpy(&entity,entityImage.data(),sizeof(entity));std::memset(&sectors,0,sizeof(sectors));
                std::memcpy(&entryPool,entryImage.data(),sizeof(entryPool));std::memcpy(&ptrPool,ptrImage.data(),sizeof(ptrPool));
                entries.fill(0xAA);pointers.fill(0xAA);entryFlags.fill(0x80);ptrFlags.fill(0x80);trace=json::array();
                entity.SetType(eEntityType(ENTITY_TYPE_VEHICLE+i%3));entity.m_bIsBIGBuilding=i%31==0;
                // Background nodes exercise nonempty heads and preservation of neighbours.
                for(int j=0;j<12;++j){auto* list=reinterpret_cast<CPtrListDoubleLink<CPhysical*>*>(&sectors[(i+j)%16][(i/16+j)%16])+j%3;list->AddItem(&entity);}
                if(fn&&!entity.m_bIsBIGBuilding){
                    for(int j=0;j<i%17;++j){
                        auto* sector=&sectors[(i+j)%16][(i/16+j)%16];auto* list=reinterpret_cast<CPtrListDoubleLink<CPhysical*>*>(sector)+i%3;
                        auto* n=new CEntryInfoNode;n->m_doubleLink=list->AddItem(&entity);n->m_doubleLinkList=list;n->m_repeatSector=sector;n->AddToList(entity.m_pCollisionList.m_node);entity.m_pCollisionList.m_node=n;
                    }
                }
                const float x=float(i%161-80)*50.f,y=float(i%137-68)*50.f;
                bounds.left=i%3==0?std::nextafter(x,-INFINITY):i%3==1?x:std::nextafter(x,INFINITY);
                bounds.bottom=y;bounds.right=bounds.left+float((i/3)%4)*50.f;bounds.top=y+float((i/12)%4)*50.f;
                if(i%37==0)bounds.right=bounds.left-100.f; // Original skips empty ranges.
                reinterpret_cast<void(__thiscall*)(CPhysical*)>(fn?0x542560:0x544A30)(&entity);
                json nodes=json::array();for(auto* n=entity.m_pCollisionList.m_node;n;n=n->m_next){
                    const auto* p=reinterpret_cast<uint8*>(n);
                    nodes.push_back({entryPool.GetIndex(n),ptrPool.GetIndex(reinterpret_cast<CPtrNodeDoubleLink<void*>*>(n->m_doubleLink)),reinterpret_cast<uintptr_t>(n->m_repeatSector)-reinterpret_cast<uintptr_t>(&sectors),reinterpret_cast<uintptr_t>(n->m_doubleLinkList)-reinterpret_cast<uintptr_t>(&sectors),n->m_previous?entryPool.GetIndex(n->m_previous):-1,n->m_next?entryPool.GetIndex(n->m_next):-1});
                    if(nodes.size()>64){Write({{"event","error"},{"reason","cycle"}});log.flush();ExitProcess(93);}
                }
                json livePtrs=json::array();for(int j=0;j<128;++j)if(!(ptrFlags[j]&0x80))livePtrs.push_back({j,ReversingProbe::Bytes(pointers.data()+j*12,12)});
                return json{{"nodes",nodes},{"pointers",livePtrs},{"entry_flags",entryFlags},{"ptr_flags",ptrFlags},{"entry_pool",ReversingProbe::Bytes(&entryPool,sizeof(entryPool))},{"ptr_pool",ReversingProbe::Bytes(&ptrPool,sizeof(ptrPool))},{"sectors",ReversingProbe::Bytes(&sectors,sizeof(sectors))},{"trace",trace}};
            };
            currentFunction=fn;currentCase=i;h->State(false);const auto a=UPProbe::calls[fn];const auto original=run();const auto b=UPProbe::calls[fn];
            h->State(true);const auto candidate=run();const auto c=UPProbe::calls[fn];
            passed=original==candidate&&a==b&&c==b+1;
            Write({{"event","comparison"},{"function",fn},{"case",i},{"pair",pairs++},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b}});
        }
        h->State(enabled);h->LockState(locked);log.flush();
    }
    entity.m_pCollisionList.m_node=nullptr;entity.m_bIsBIGBuilding=false;
    }
    entryGlobal=entryBefore;ptrGlobal=ptrBefore;std::memcpy(&sectors,sectorsBefore.data(),sizeof(sectors));
    const bool restored=playerBefore==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&sectorsBefore==ReversingProbe::Bytes(&sectors,sizeof(sectors))&&entryGlobal==entryBefore&&ptrGlobal==ptrBefore;
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});log.flush();
}
}
