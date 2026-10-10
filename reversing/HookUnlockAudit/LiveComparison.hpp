#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "EntryExit.h"
#include "EntryExitManager.h"
#include "../ProbeRuntime.hpp"
#include "Targets.hpp"
#include "PostEffectsChecks.hpp"

namespace HookUnlockAuditLive {
using Hook = ReversibleHooks::ReversibleHook::Base;
inline std::ofstream log;
inline bool done{}, started{}, passed{true};
inline int frames{}, pairs{};
inline Hook* entry{};
inline Hook* manager{};
inline Hook* postHook{};
inline bool postEnabled{},postLocked{},postPassed{};
inline uintptr_t postDll{};
inline std::vector<uint8> postExeBefore,postDllBefore;
inline bool entryEnabled{}, entryLocked{}, managerEnabled{}, managerLocked{};
inline uintptr_t entryDll{}, managerDll{};
inline std::vector<uint8> entryExeBefore,entryDllBefore,managerExeBefore,managerDllBefore;
inline int32 lastEspDelta{};
inline std::filesystem::path dir;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
inline Hook* Find(const char* cls,const char* name) {
    Hook* out{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        if(c.Name()==cls) for(auto& h:c.Items()) if(h->Name()==name) out=h.get();
    });
    return out;
}
inline uint32 CallEdx(uintptr_t target,void* self,void* arg) {
    uint32 result,stackBefore,stackAfter;
    __asm {
        mov stackBefore,esp
        mov ecx,self
        push arg
        mov edx,5A17C0DEh
        call target
        mov result,edx
        mov stackAfter,esp
    }
    lastEspDelta=int32(stackAfter-stackBefore);
    return result;
}
template<class T> struct Raw {
    alignas(T) std::array<uint8,sizeof(T)> bytes{};
    T* Get() { return reinterpret_cast<T*>(bytes.data()); }
};
inline json Invoke(int scenario,bool viaManager) {
    std::array<Raw<CEntryExit>,8> raw;
    auto& stack=CEntryExitManager::ms_entryExitStack;
    const auto oldStack=ReversingProbe::Bytes(stack,sizeof(stack));
    const auto oldCount=CEntryExitManager::ms_entryExitStackPosn;
    for(int j=0;j<8;++j) {
        auto* e=raw[j].Get();
        e->m_recEntrance=CRect{float(j+scenario%13),float(j+2),float(j+scenario%13+4),float(j+7)};
        e->m_fEntranceZ=float(scenario%19);
        e->m_vecExitPos=CVector{float(j*3),float(scenario%17),float(j-scenario%5)};
        e->m_nArea=static_cast<eAreaCodes>((scenario>>j)&1);
        e->m_pLink=j<4&&(scenario&(1<<j))?raw[j+4].Get():nullptr;
        if(j<4) stack[j]=e;
    }
    CEntryExitManager::ms_entryExitStackPosn=uint32(scenario%5);
    const auto objectBefore=ReversingProbe::Bytes(raw.data(),sizeof(raw));
    const auto fixtureStack=ReversingProbe::Bytes(stack,sizeof(stack));
    CVector pos{float(scenario%31)*.5f,float(scenario%11),-float(scenario%23)};
    uint32 edx=0x5A17C0DE;
    if(viaManager) reinterpret_cast<void(__cdecl*)(CVector&)>(0x43F150)(pos);
    else edx=CallEdx((scenario&1)?entryDll:uintptr_t(0x43EA00),raw[scenario%4].Get(),&pos);
    json result={{"position",ReversingProbe::Bytes(&pos,sizeof(pos))},{"edx",viaManager?json(nullptr):json(edx)},
        {"esp_delta",viaManager?json(nullptr):json(lastEspDelta)},
        {"stack_unchanged",fixtureStack==ReversingProbe::Bytes(stack,sizeof(stack))},
        {"objects_unchanged",objectBefore==ReversingProbe::Bytes(raw.data(),sizeof(raw))},
        {"count_unchanged",CEntryExitManager::ms_entryExitStackPosn==uint32(scenario%5)}};
    std::memcpy(stack,oldStack.data(),sizeof(stack));
    CEntryExitManager::ms_entryExitStackPosn=oldCount;
    result["globals_restored"]=oldStack==ReversingProbe::Bytes(stack,sizeof(stack))&&CEntryExitManager::ms_entryExitStackPosn==oldCount;
    return result;
}
inline bool Route(bool active) {
    entry->State(active);
    FlushInstructionCache(GetCurrentProcess(),nullptr,0);
    return entry->Hooked()==active&&ReversingProbe::JumpsToDll(0x43EA00)==active;
}
inline void Compare(int scenario,bool viaManager,bool managerActive) {
    manager->State(managerActive);
    const bool managerRoute=manager->Hooked()==managerActive&&ReversingProbe::JumpsToDll(0x43F150)==managerActive;
    const bool originalRoute=Route(false);
    const auto original=Invoke(scenario,viaManager);
    const bool candidateRoute=Route(true);
    const auto candidate=Invoke(scenario,viaManager);
    const bool good=managerRoute&&originalRoute&&candidateRoute&&original==candidate&&
        (viaManager||(original["edx"]==0x5A17C0DE&&original["esp_delta"]==0))&&original["stack_unchanged"]==true&&
        original["objects_unchanged"]==true&&original["count_unchanged"]==true&&original["globals_restored"]==true;
    passed&=good;
    Write({{"event","comparison"},{"pair",pairs++},{"function","CEntryExit::GetPositionRelativeToOutsideWorld"},
        {"case",scenario},{"via_manager",viaManager},{"manager_active",managerActive},
        {"original",original},{"candidate",candidate},{"original_route",originalRoute},{"candidate_route",candidateRoute},
        {"equal",good},{"accepted",good}});
}
inline void Restore() {
    entry->State(entryEnabled);entry->LockState(entryLocked);
    manager->State(managerEnabled);manager->LockState(managerLocked);
    if(postHook){postHook->LockState(false);postHook->State(postEnabled);postHook->LockState(postLocked);}
}
inline void Update() {
    if(done)return;
    if(dir.empty()) { char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);dir=std::filesystem::path(exe).parent_path(); }
    if(!std::filesystem::exists(dir/"hookunlock-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"hookunlock-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("hookunlock");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    if(!started) {
        started=true;
        ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){for(auto& h:c.Items()){
            Write({{"event","runtime_hook"},{"class",c.Name()},{"fn_name",h->Name()},
                {"locked",h->Locked()},{"hooked",h->Hooked()},{"reversed",h->Reversed()},{"type",h->Symbol()}});
        }});
        for(const auto& target:targets) {
            auto* h=Find(target.cls,target.name);
            if(!h){passed=false;Write({{"event","error"},{"reason","catalogued hook missing"},{"class",target.cls},{"fn_name",target.name}});done=true;return;}
            const bool before=h->Hooked();
            bool refusal=true;
            if(h->Locked()) refusal=!h->State(!before)&&h->Hooked()==before;
            passed&=refusal;
            Write({{"event","catalog_lock_review"},{"class",target.cls},{"fn_name",target.name},
                {"runtime_locked",h->Locked()},{"runtime_hooked",before},{"lock_refusal_ok",refusal}});
        }
        entry=Find("CEntryExit","GetPositionRelativeToOutsideWorld");
        manager=Find("CEntryExitManager","GetPositionRelativeToOutsideWorld");
        entryEnabled=entry->Hooked();entryLocked=entry->Locked();managerEnabled=manager->Hooked();managerLocked=manager->Locked();
        entryDll=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(entry)->GetHookOurAddress());
        managerDll=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(manager)->GetHookOurAddress());
        entryExeBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0x43EA00),5);
        entryDllBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(entryDll),5);
        managerExeBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0x43F150),5);
        managerDllBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(managerDll),5);
        postHook=Find("CPostEffects","ImmediateModeRenderStatesStore");
        postEnabled=postHook->Hooked();postLocked=postHook->Locked();
        postDll=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(postHook)->GetHookOurAddress());
        postExeBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0x700CC0),5);
        postDllBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(postDll),5);
        const auto post=RunPostEffectsChecks(postHook);
        postPassed=post.value("passed",false);
        Write(post);
        entry->LockState(false);manager->LockState(false);
        for(int i=0;i<1024&&passed;++i) Compare(i,false,true);
        for(int i=0;i<1024&&passed;++i) Compare(i,true,(i&1)!=0);
        Restore();
        if(!passed){done=true;Write({{"event","error"},{"reason","pair mismatch"},{"state_restored",true}});return;}
    }
    // Entre fotogramas sólo se conmuta este ayudante puro, sin sustituir colaboradores ni el audio.
    entry->LockState(false);manager->LockState(false);
    manager->State((frames&1)!=0);
    const bool active=(frames%4)>=2;
    const bool routed=Route(active);
    const auto result=Invoke(2048+frames,true);
    passed&=routed&&result["objects_unchanged"]==true&&result["globals_restored"]==true;
    manager->State(managerEnabled);manager->LockState(managerLocked);
    Write({{"event","live_frame"},{"frame",frames},{"entry_active",active},{"route_ok",routed},{"result",result}});
    if(postPassed) {
        postHook->LockState(false);postHook->State(active);
        const auto post=RunPostEffectsChecks(postHook);
        postPassed&=post.value("passed",false);
        Write({{"event","posteffects_live_frame"},{"frame",frames},{"active",active},{"checks",post}});
    }
    if(++frames>=240||!passed) {
        Restore();done=true;
        const bool restored=entry->Hooked()==entryEnabled&&entry->Locked()==entryLocked&&manager->Hooked()==managerEnabled&&manager->Locked()==managerLocked&&
            entryExeBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0x43EA00),5)&&entryDllBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(entryDll),5)&&
            managerExeBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0x43F150),5)&&managerDllBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(managerDll),5);
        const bool postRestored=postHook->Hooked()==postEnabled&&postHook->Locked()==postLocked&&postExeBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0x700CC0),5)&&postDllBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(postDll),5);
        Write({{"event",passed&&restored&&postRestored?"complete":"error"},{"pairs",pairs},{"frames",frames},{"state_restored",restored&&postRestored},{"posteffects_passed",postPassed}});
    }
}
}
