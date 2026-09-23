#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Ragdoll/IKChain.h"
#include "Ragdoll/BoneNodeManager.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace WarningsLive {
inline bool done{};
inline std::ofstream log;
inline void Write(json row) { log << row.dump() << '\n'; log.flush(); }
using Hook = ReversibleHooks::ReversibleHook::Base;
inline Hook* Find(const char* category, const char* name) {
    Hook* result{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        if(c.Name()==category) for(auto& h:c.Items()) if(h->Name()==name) result=h.get();
    });
    return result;
}
struct Toggle {
    Hook* h; bool state, locked;
    Toggle(Hook* hook):h(hook),state(h->Hooked()),locked(h->Locked()){h->LockState(false);}
    ~Toggle(){h->State(state);h->LockState(locked);}
};
inline json Snapshot(IKChain_c& chain) {
    json nodes=json::array();
    for(auto* n:chain.GetBones()) {
        const auto q=n->GetOrientation();
        json limits=json::array();
        for(int i=0;i<3;++i){float a{},b{};n->GetLimits(static_cast<eRotationAxis>(i),a,b);limits.push_back({a,b});}
        nodes.push_back({{"tag",int(n->GetBoneTag())},{"parent",n->GetParent()?int(n->GetParent()->GetBoneTag()):-1},
            {"orientation",{q.imag.x,q.imag.y,q.imag.z,q.real}},{"limits",limits},{"speed",n->GetSpeed()}});
    }
    return nodes;
}
inline void Update() {
    if(done)return;
    if(!std::filesystem::exists("ik-data-runtime.start")){done=true;return;}
    auto* ped=FindPlayerPed(); if(!ped || !ped->GetRpClump())return;
    auto* frames=ped->GetAnimBlendData().GetFrames(); if(!frames || !frames[0].KeyFrame)return;
    done=true;log.open("ik-data-runtime.jsonl");
    auto* setup=Find("IKChain_c","SetupBones");auto* exit=Find("IKChain_c","Exit");
    auto* timecycle=Find("CTimeCycle","Initialise");auto* groups=Find("CPopulation","LoadCarGroups");
    if(!setup||!exit||!timecycle||!groups){Write({{"event","error"},{"reason","missing hooks"}});return;}
    const auto playerBefore=ReversingProbe::Bytes(ped,sizeof(CPlayerPed));
    const auto managerBefore=ReversingProbe::Bytes(&g_boneNodeMan,sizeof(g_boneNodeMan));
    const auto framesBefore=ReversingProbe::Bytes(frames,sizeof(*frames)*ped->GetAnimBlendData().m_NumFrameData);
    std::vector<std::vector<uint8>> keyframesBefore;
    for(unsigned i=0;i<ped->GetAnimBlendData().m_NumFrameData;++i)
        keyframesBefore.push_back(ReversingProbe::Bytes(frames[i].KeyFrame,sizeof(*frames[i].KeyFrame)));
    const auto freeBefore=g_boneNodeMan.GetNumBoneNodesLeft();
    unsigned pairs{},differences{};bool counters=true,returned=true;
    {
        Toggle s{setup},e{exit};
        const auto* infos=reinterpret_cast<const BoneInfo_t*>(0x8D26D0);
        for(unsigned tagIndex=0;tagIndex<MAX_BONE_NUM;++tagIndex) {
            auto effector=infos[tagIndex].BoneTag;
            if(effector<=BONE_ROOT || RpHAnimIDGetIndex(&ped->GetAnimHierarchy(),int(effector))<0)continue;
            auto pivot=infos[tagIndex].ParentBoneTag;
            for(unsigned depth=0;depth<MAX_BONE_NUM && pivot>BONE_ROOT;++depth) {
                if(RpHAnimIDGetIndex(&ped->GetAnimHierarchy(),int(pivot))<0)break;
                if(freeBefore<=depth+1){Write({{"event","error"},{"reason","insufficient free bone nodes"}});return;}
                auto run=[&](bool candidate) {
                    setup->State(candidate);exit->State(candidate);
                    IKChain_c chain{};
                    // Borrow the real skeleton read-only; no entity reference is registered.
                    std::memcpy(reinterpret_cast<uint8*>(&chain)+8,&ped,sizeof(ped));
                    using Setup=void(__thiscall*)(IKChain_c*,eBoneTag32,CVector,eBoneTag32,AnimBlendFrameData*);
                    reinterpret_cast<Setup>(0x617CA0)(&chain,effector,CVector{},pivot,frames);
                    auto result=Snapshot(chain);
                    using Exit=void(__thiscall*)(IKChain_c*);
                    reinterpret_cast<Exit>(0x617870)(&chain); // Matching allocator for each implementation.
                    std::memset(reinterpret_cast<uint8*>(&chain)+8,0,sizeof(ped));
                    returned &= g_boneNodeMan.GetNumBoneNodesLeft()==freeBefore;
                    std::memcpy(&g_boneNodeMan,managerBefore.data(),managerBefore.size());
                    return result;
                };
                const auto n0=WarningsProbe::setups;auto a=run(false);const auto n1=WarningsProbe::setups;
                auto b=run(true);const auto n2=WarningsProbe::setups;
                counters &= n0==n1 && n2==n1+1;
                bool equal=a==b; ++pairs;if(!equal)++differences;
                Write({{"event","chain"},{"effector",int(effector)},{"pivot",int(pivot)},{"original",a},{"candidate",b},
                    {"equal",equal},{"original_candidate_calls",n1-n0},{"candidate_calls",n2-n1}});
                auto* info=BoneNodeManager_c::GetBoneInfoFromTag(pivot);pivot=info->ParentBoneTag;
            }
        }
    }
    // Compare all loaded colour tables on the repaired data, restoring runtime state afterwards.
    auto* state=reinterpret_cast<uint8*>(0xB79E38);constexpr size_t stateSize=0xB7CB24-0xB79E38;
    const auto before=ReversingProbe::Bytes(state,stateSize);
    unsigned tableDifferences{};
    {
        Toggle t{timecycle};
        timecycle->State(false);reinterpret_cast<void(__cdecl*)(bool)>(0x5BBAC0)(false);
        const auto native=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB79FD8),0xB7C480-0xB79FD8);
        std::memcpy(state,before.data(),stateSize);
        timecycle->State(true);reinterpret_cast<void(__cdecl*)(bool)>(0x5BBAC0)(false);
        const auto candidate=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB79FD8),native.size());
        for(size_t i=0;i<native.size();++i)if(native[i]!=candidate[i])++tableDifferences;
        Write({{"event","timecycle"},{"bytes",native.size()},{"different_bytes",tableDifferences}});
        std::memcpy(state,before.data(),stateSize);
    }
    const auto carGroups=CPopulation::m_CarGroups;const auto carCounts=CPopulation::m_nNumCarsInGroup;
    bool groupsEqual{};
    {
        Toggle g{groups};groups->State(false);reinterpret_cast<void(__cdecl*)()>(0x5BD1A0)();
        const auto original=CPopulation::m_CarGroups;const auto counts=CPopulation::m_nNumCarsInGroup;
        groups->State(true);reinterpret_cast<void(__cdecl*)()>(0x5BD1A0)();
        // Compare active entries; unused native slots are unspecified.
        groupsEqual=counts==CPopulation::m_nNumCarsInGroup;
        for(size_t i=0;i<counts.size();++i)for(size_t j=0;j<counts[i];++j)
            groupsEqual &= original[i][j]==CPopulation::m_CarGroups[i][j];
        CPopulation::m_CarGroups=carGroups;CPopulation::m_nNumCarsInGroup=carCounts;
    }
    bool framesRestored=framesBefore==ReversingProbe::Bytes(frames,framesBefore.size());
    for(size_t i=0;i<keyframesBefore.size();++i)
        framesRestored &= keyframesBefore[i]==ReversingProbe::Bytes(frames[i].KeyFrame,sizeof(*frames[i].KeyFrame));
    auto* skimmer=static_cast<CVehicleModelInfo*>(CModelInfo::GetModelInfo(460));
    bool skimmerValid=skimmer && skimmer->m_fWheelSizeFront==.7f && skimmer->m_fWheelSizeRear==.7f && skimmer->m_nWheelModelIndex==-1;
    Write({{"event","complete"},{"chain_pairs",pairs},{"chain_differences",differences},{"counters_valid",counters},
        {"skeleton_unchanged",framesRestored},{"skimmer_defaults_explicit",skimmerValid},
        {"bone_nodes_returned",returned},{"timecycle_different_bytes",tableDifferences},{"groups_equal",groupsEqual},
        {"state_restored",playerBefore==ReversingProbe::Bytes(ped,sizeof(CPlayerPed)) && managerBefore==ReversingProbe::Bytes(&g_boneNodeMan,sizeof(g_boneNodeMan)) && before==ReversingProbe::Bytes(state,stateSize)}});
}
}
