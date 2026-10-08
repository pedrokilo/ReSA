#pragma once
#include <fstream>
#include "InterestingEvents.h"
#include "../HookSnapshots.hpp"
namespace InterestingVisibilityPendingLive {
inline bool done{};
inline json trace;
inline int scenario{};
inline void __fastcall Cleanup(CEntity*, void*, CEntity** slot) { trace.push_back({"cleanup", uintptr_t(slot)}); }
inline bool __cdecl Los(const CVector& a, const CVector& b, bool c, bool d, bool e, bool f, bool g, bool h, bool i) {
    const bool clear = scenario % 3 != 0;
    trace.push_back({"los", {a.x,a.y,a.z}, {b.x,b.y,b.z}, c,d,e,f,g,h,i,clear}); return clear;
}
inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); auto dir=std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir/"interesting-visibility-pending-runtime.start")) {done=true;return;}
    done=true; std::ofstream log(dir/"interesting-visibility-pending-runtime.jsonl");
    auto write=[&](const json& row){log<<row.dump()<<'\n';log.flush();};
    ReversibleHooks::ReversibleHook::Simple* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){if(cat.Name()=="CInterestingEvents")for(auto& h:cat.Items())if(h->Name()=="InvalidateNonVisibleEvents")hook=static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get());});
    if(!hook){write({{"event","error"},{"reason","missing hook"}});return;}
    const auto allHooks=SmallPendingSupport::SnapshotHooks();
    const auto gtaBefore=ReversingProbe::Bytes(hook->GetHookGTAAddress(),52),dllBefore=ReversingProbe::Bytes(hook->GetHookOurAddress(),52);
    const bool enabled=hook->Hooked(),locked=hook->Locked(); hook->LockState(false);
    const auto cameraBefore=ReversingProbe::Bytes(&TheCamera,sizeof(TheCamera));
    const uintptr_t cleanupDll=std::bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference));
    const uintptr_t losDll=std::bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear);
    const std::array<uintptr_t,4> patchAddresses{0x571A00,cleanupDll,0x56A490,losDll};
    std::array<std::vector<uint8>,4> patchBefore; for(size_t k=0;k<4;++k)patchBefore[k]=ReversingProbe::Bytes(reinterpret_cast<void*>(patchAddresses[k]),5);
    alignas(CInterestingEvents) std::array<uint8,sizeof(CInterestingEvents)> events{};
    alignas(CEntity) std::array<uint8,sizeof(CEntity)> entity{}; CMatrixLink matrix{};
    bool passed=true;int failures{};
    {
        ReversingProbe::EntryPatch c0(0x571A00,Cleanup),c1(cleanupDll,Cleanup),l0(0x56A490,Los),l1(losDll,Los);
        for(int n=0;n<384;++n){json results[2];for(int side=0;side<2;++side){
            scenario=n;trace=json::array();events.fill(0xCD);entity.fill(0);auto* g=reinterpret_cast<CInterestingEvents*>(events.data());auto* e=reinterpret_cast<CEntity*>(entity.data());
            matrix.SetUnity();matrix.GetPosition()=CVector{2.f,3.f,4.f};e->m_matrix=n%2?&matrix:nullptr;e->m_placement.m_vPosn=CVector{5.f,6.f,7.f};
            g->m_nInterestingEvent=int8(n%10)-1;for(int k=0;k<8;++k){g->m_Events[k].type=k;g->m_Events[k].time=1000+k;g->m_Events[k].entity=(n&(1<<(k%6)))?e:nullptr;}
            TheCamera.m_nActiveCam=uint8(n%3);TheCamera.m_aCams[n%3].m_vecSource=CVector{float(n%7),2.f,10.f};
            hook->State(side!=0);bool switched=SmallPendingSupport::ExactSwitch(*hook,side!=0);
            reinterpret_cast<void(__thiscall*)(CInterestingEvents*)>(0x6029C0)(g);
            results[side]={{"state",ReversingProbe::Bytes(events.data(),events.size())},{"entity",ReversingProbe::Bytes(entity.data(),entity.size())},{"trace",trace},{"switched",switched}};
        }const bool equal=results[0]==results[1]&&results[0]["switched"]==true;passed&=equal;failures+=!equal;write({{"event","comparison"},{"function","CInterestingEvents::InvalidateNonVisibleEvents"},{"pair",n},{"case",n},{"equal",equal},{"accepted",equal},{"original",results[0]},{"candidate",results[1]}});}
    }
    std::memcpy(&TheCamera,cameraBefore.data(),cameraBefore.size());hook->State(enabled);hook->LockState(locked);
    bool restored=SmallPendingSupport::HooksUnchanged(allHooks)&&cameraBefore==ReversingProbe::Bytes(&TheCamera,sizeof(TheCamera))&&gtaBefore==ReversingProbe::Bytes(hook->GetHookGTAAddress(),52)&&dllBefore==ReversingProbe::Bytes(hook->GetHookOurAddress(),52);
    for(size_t k=0;k<4;++k)restored&=patchBefore[k]==ReversingProbe::Bytes(reinterpret_cast<void*>(patchAddresses[k]),5);
    write({{"event","function_summary"},{"function","CInterestingEvents::InvalidateNonVisibleEvents"},{"cases",384},{"failures",failures}});
    write({{"event",passed&&restored?"complete":"error"},{"pairs",384},{"state_restored",restored}});
}
}
