#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cover.h"
#include "Curves.h"
#include "EntryExitManager.h"
#include "EntryExit.h"
#include "DecisionMakers/DecisionMakerTypesFileLoader.h"
#include "Pools/EntryInfoNodePool.h"
#include "VehicleRecording.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"

namespace UCLive {
inline std::ofstream log;
inline bool done{};
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; }
inline int Report(int type, char* msg, int*) {
    if (type != _CRT_ASSERT) return FALSE;
    Write({{"event","assertion"},{"message",msg ? msg : ""}});
    log.flush(); ExitProcess(91);
}
template<class T> struct Raw {
    alignas(T) std::array<uint8,sizeof(T)> bytes{};
    T* get() { return reinterpret_cast<T*>(bytes.data()); }
};
inline CDecisionMakerTypes* decisionManager{};
inline CDecisionMakerTypes* GetDecisionManager() { return decisionManager; }
inline constexpr std::array counts{4096,256,1024,1024,512,512,16,16,512};

inline void Update() {
    if (done) return;
    _set_error_mode(_OUT_TO_STDERR);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    char exe[MAX_PATH]; GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir/"unlock-core-runtime.start")) { done=true; return; }
    if (!log.is_open()) { log.open(dir/"unlock-core-runtime.jsonl"); Write({{"event","waiting_for_player"}}); log.flush(); }
    auto* player=FindPlayerPed();
    if (!player || CCutsceneMgr::IsRunning()) return;
    done=true;
    const auto report=_CrtSetReportHook(Report);
    const auto playerBefore=ReversingProbe::Bytes(player,sizeof(CPlayerPed));
    std::array<ReversibleHooks::ReversibleHook::Base*,9> hooks{};
    ReversibleHooks::ReversibleHook::Base* entryHelper{};
    constexpr std::array classes{"CCover","CCover","CEntryExitManager","CDecisionMakerTypesFileLoader","CEntryInfoNode","CEntryInfoNode","CCurves","CVehicleRecording","CEntryInfoNodePool"};
    constexpr std::array names{"FindDirFromVector","FindVectorFromDir","GetPositionRelativeToOutsideWorld","UnloadDecisionMaker","operator new","operator delete","TestCurves","Render","New"};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category) {
        for (auto& h : category.Items()) {
            for (int fn=0;fn<9;++fn) if(category.Name()==classes[fn]&&h->Name()==names[fn]) hooks[fn]=h.get();
            if(category.Name()=="CEntryExit"&&h->Name()=="GetPositionRelativeToOutsideWorld")entryHelper=h.get();
        }
    });
    if (!entryHelper || std::ranges::any_of(hooks,[](auto* h){return !h;})) {
        Write({{"event","error"},{"reason","missing hook"}});log.flush();_CrtSetReportHook(report);return;
    }
    std::array<bool,9> enabled{},locked{};
    for(int fn=0;fn<9;++fn) {
        enabled[fn]=hooks[fn]->Hooked();locked[fn]=hooks[fn]->Locked();hooks[fn]->LockState(false);
        Write({{"event","hook_state"},{"function",fn},{"enabled",enabled[fn]},{"locked",locked[fn]},{"startup_calls",UCProbe::calls[fn]}});
    }
    const bool helperEnabled=entryHelper->Hooked(),helperLocked=entryHelper->Locked();
    entryHelper->LockState(false);entryHelper->State(false);
    auto& stack=CEntryExitManager::ms_entryExitStack;
    const auto stackBefore=ReversingProbe::Bytes(stack,sizeof(stack));
    const auto stackCount=CEntryExitManager::ms_entryExitStackPosn;
    std::array<Raw<CEntryExit>,8> exits;
    auto& active=CDecisionMakerTypes::m_IsActive;
    const auto activeBefore=active;
    auto* isGroup=reinterpret_cast<uint8*>(0xC0AFE0);
    const auto groupBefore=ReversingProbe::Bytes(isGroup,20);
    auto* groupsUsed=reinterpret_cast<uint8*>(0xC098E0);
    const auto groupsBefore=ReversingProbe::Bytes(groupsUsed,8);
    auto& pedPool=*reinterpret_cast<void**>(0xB74490);auto* pedPoolBefore=pedPool;
    std::array<uint32,5> emptyPool{};pedPool=emptyPool.data();
    auto manager=std::make_unique<Raw<CDecisionMakerTypes>>();decisionManager=manager->get();
    ReversingProbe::EntryPatch managerPatch(0x4684F0,GetDecisionManager);
    auto& entryPool=*reinterpret_cast<CEntryInfoNodePool**>(0xB7448C);auto* poolBefore=entryPool;
    alignas(4) std::array<uint8,16*20+32> storage{};
    std::array<uint8,16+32> states{};
    CEntryInfoNodePool pool(16,storage.data()+16,states.data()+16);
    entryPool=&pool;const auto poolImage=ReversingProbe::Bytes(&pool,sizeof(pool));
    bool passed=true;int pairs=0;
    for(int fn=0;fn<9&&passed;++fn) {
        for(int i=0;i<counts[fn]&&passed;++i) {
            const auto run=[&] {
                json out;
                if(fn==0) {
                    CVector v{float(i%127-63)*.125f,float((i/127)%31-15)*.25f,float(i%7)};
                    if(i<4)v=CVector{i==0?0.f:i==1?-0.f:1.f,i==2?0.f:i==3?-0.f:1.f,0.f};
                    const auto initial=v;
                    const auto result=reinterpret_cast<uint8(__cdecl*)(CVector)>(0x698D40)(v);
                    out={{"dir",result},{"input_unchanged",std::memcmp(&initial,&v,sizeof(v))==0}};
                } else if(fn==1) {
                    const auto v=reinterpret_cast<CVector(__cdecl*)(uint8)>(0x698D60)(uint8(i));
                    out={{"bits",ReversingProbe::Bytes(&v,sizeof(v))}};
                } else if(fn==2) {
                    for(int j=0;j<8;++j) {
                        exits[j].bytes.fill(0);auto* e=exits[j].get();
                        e->m_recEntrance=CRect{float(j+i%9),float(j+2),float(j+i%9+4),float(j+7)};
                        e->m_fEntranceZ=float(i%13);e->m_vecExitPos=CVector{float(j*3),float(i%17),float(j-i%5)};
                        e->m_nArea=static_cast<eAreaCodes>((i>>j)&1);
                        e->m_pLink=j<4&&(i&(1<<j))?exits[j+4].get():nullptr;
                        if(j<4)stack[j]=e;
                    }
                    CEntryExitManager::ms_entryExitStackPosn=i%5;
                    CVector v{float(i%19)*.5f,float(i%7),-float(i%11)};
                    reinterpret_cast<void(__cdecl*)(CVector&)>(0x43F150)(v);
                    out={{"bits",ReversingProbe::Bytes(&v,sizeof(v))},{"stack_count",CEntryExitManager::ms_entryExitStackPosn}};
                } else if(fn==3) {
                    active.fill(false);std::memset(isGroup,0,20);std::memset(groupsUsed,0,8);
                    const auto id=i%20;active[id]=i%3!=0;isGroup[id]=(i/20)%2;
                    decisionManager->m_NoOfDecisionMakers=20;
                    reinterpret_cast<void(__cdecl*)(eDecisionTypes)>(0x607A70)(eDecisionTypes(id));
                    out={{"active",active},{"group",ReversingProbe::Bytes(isGroup,20)},{"count",decisionManager->m_NoOfDecisionMakers}};
                } else if(fn==4||fn==5||fn==8) {
                    std::memcpy(&pool,poolImage.data(),poolImage.size());
                    storage.fill(uint8(i));states.fill(0xA5);
                    const int32 cursor=i%17-1;std::memcpy(reinterpret_cast<uint8*>(&pool)+12,&cursor,4);
                    for(int j=0;j<16;++j)states[j+16]=uint8((i+j)%128)|uint8(((i>> (j%9))&1)?0x80:0);
                    CEntryInfoNode* result{};
                    if(fn==5) {
                        const int slot=i%16;states[slot+16]&=0x7F;
                        result=reinterpret_cast<CEntryInfoNode*>(storage.data()+16+slot*20);
                        reinterpret_cast<void(__cdecl*)(void*,size_t)>(0x536DD0)(result,sizeof(CEntryInfoNode));
                    } else if(fn==4)result=reinterpret_cast<CEntryInfoNode*(__cdecl*)(size_t)>(0x536DC0)(sizeof(CEntryInfoNode));
                    else result=reinterpret_cast<CEntryInfoNode*(__thiscall*)(CEntryInfoNodePool*)>(0x536D10)(&pool);
                    out={{"slot",result?(reinterpret_cast<uint8*>(result)-storage.data()-16)/20:-1},
                         {"pool",ReversingProbe::Bytes(&pool,sizeof(pool))},{"states",states},{"storage",storage}};
                } else {
                    const auto frame=CTimer::GetFrameCounter();
                    reinterpret_cast<void(__cdecl*)()>(fn==6?0x43C600:0x459F70)();
                    out={{"frame_unchanged",frame==CTimer::GetFrameCounter()}};
                }
                return out;
            };
            hooks[fn]->State(false);
            if(fn==4)hooks[8]->State(false);
            auto a=UCProbe::calls[fn];const auto original=run();auto b=UCProbe::calls[fn];
            hooks[fn]->State(true);
            if(fn==4)hooks[8]->State(true);
            const auto candidate=run();auto c=UCProbe::calls[fn];
            passed=original==candidate&&a==b&&c==b+1;
            Write({{"event","comparison"},{"pair",pairs++},{"function",fn},{"case",i},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b}});
        }
        log.flush();
        hooks[fn]->State(enabled[fn]);
    }
    entryPool=poolBefore;pedPool=pedPoolBefore;active=activeBefore;
    std::memcpy(isGroup,groupBefore.data(),20);std::memcpy(groupsUsed,groupsBefore.data(),8);
    std::memcpy(stack,stackBefore.data(),sizeof(stack));CEntryExitManager::ms_entryExitStackPosn=stackCount;
    entryHelper->State(helperEnabled);entryHelper->LockState(helperLocked);
    for(int fn=0;fn<9;++fn){hooks[fn]->State(enabled[fn]);hooks[fn]->LockState(locked[fn]);}
    const bool restored=playerBefore==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&entryPool==poolBefore&&pedPool==pedPoolBefore&&active==activeBefore&&stackBefore==ReversingProbe::Bytes(stack,sizeof(stack));
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});
    log.flush();_CrtSetReportHook(report);
}
}
