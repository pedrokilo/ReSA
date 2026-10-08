#pragma once
#include <fstream>
#include "PedGroupPlacer.h"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedGroupPlacerCorrectedLive {
inline bool done{};
inline json trace;
inline int fixture{};
inline CPedGroupPlacer placer{};
alignas(256) inline std::array<uint8,512> origins{};
inline bool Dispatch(const char* route, CPedGroupPlacer* self, ePedType type, uint32 count, const CVector& origin, ePedGroupDefaultTaskAllocatorType allocator) {
    trace.push_back({{"route",route},{"self_matches",self==&placer},{"type",int(type)},{"count",count},{"origin_offset",uintptr_t(&origin)-uintptr_t(origins.data())},{"origin_bits",{std::bit_cast<uint32>(origin.x),std::bit_cast<uint32>(origin.y),std::bit_cast<uint32>(origin.z)}},{"allocator",int(allocator)}});
    return (fixture&1)!=0;
}
inline bool __fastcall Formation(CPedGroupPlacer* self,void*,ePedType type,uint32 count,const CVector& origin,ePedGroupDefaultTaskAllocatorType allocator){return Dispatch("formation",self,type,count,origin,allocator);}
inline bool __fastcall Chat(CPedGroupPlacer* self,void*,ePedType type,uint32 count,const CVector& origin,ePedGroupDefaultTaskAllocatorType allocator){return Dispatch("chat",self,type,count,origin,allocator);}
inline bool __fastcall Random(CPedGroupPlacer* self,void*,ePedType type,uint32 count,const CVector& origin,ePedGroupDefaultTaskAllocatorType allocator){return Dispatch("random",self,type,count,origin,allocator);}
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"requested-group-placer-corrected-runtime.start")){done=true;return;}
    done=true;std::ofstream log(dir/"requested-group-placer-corrected-runtime.jsonl");auto write=[&](const json&r){log<<r.dump()<<'\n';log.flush();};
    ReversibleHooks::ReversibleHook::Simple* hook{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto&cat){if(cat.Name()=="CPedGroupPlacer")for(auto&item:cat.Items())if(item->Name()=="PlaceGroup")hook=static_cast<ReversibleHooks::ReversibleHook::Simple*>(item.get());});
    if(!hook){write({{"event","error"},{"reason","missing PlaceGroup hook"}});return;}
    const auto allHooks=SmallPendingSupport::SnapshotHooks();const auto gtaBefore=ReversingProbe::Bytes(hook->GetHookGTAAddress(),52),dllBefore=ReversingProbe::Bytes(hook->GetHookOurAddress(),52);const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
    const std::array<uintptr_t,6> addresses{0x5FC9B0,std::bit_cast<uintptr_t>(&CPedGroupPlacer::PlaceFormationGroup),0x5FCE80,std::bit_cast<uintptr_t>(&CPedGroupPlacer::PlaceChatGroup),0x5FD330,std::bit_cast<uintptr_t>(&CPedGroupPlacer::PlaceRandomGroup)};
    std::array<std::vector<uint8>,6> bytesBefore;for(size_t k=0;k<6;++k)bytesBefore[k]=ReversingProbe::Bytes(reinterpret_cast<void*>(addresses[k]),52);
    bool subsetPassed=true;int failures{},pair{};
    {
        ReversingProbe::EntryPatch a0(addresses[0],Formation),a1(addresses[1],Formation),b0(addresses[2],Chat),b1(addresses[3],Chat),c0(addresses[4],Random),c1(addresses[5],Random);
        constexpr int allocators[]{0,1,2,3,4,5};
        std::array<std::vector<uint8>,6> helperActive;for(size_t k=0;k<6;++k)helperActive[k]=ReversingProbe::Bytes(reinterpret_cast<void*>(addresses[k]),52);
        for(const int allocator:allocators)for(int n=0;n<256;++n){json result[2];for(int side=0;side<2;++side){fixture=n;trace=json::array();origins.fill(0xCD);auto*origin=::new(origins.data()+4*(n%64))CVector{float(n%17)-8.f,float(n%11)*.25f,3.f};hook->State(side!=0);const bool switched=SmallPendingSupport::ExactSwitch(*hook,side!=0);
            const auto placerBefore=ReversingProbe::Bytes(&placer,sizeof(placer));const auto originBefore=origins;bool helperGates=true;for(size_t k=0;k<6;++k)helperGates&=helperActive[k]==ReversingProbe::Bytes(reinterpret_cast<void*>(addresses[k]),52);
            const auto returned=reinterpret_cast<uint8(__thiscall*)(CPedGroupPlacer*,ePedType,uint32,const CVector&,ePedGroupDefaultTaskAllocatorType)>(0x5FD810)(&placer,static_cast<ePedType>(7+n%10),1+n%5,*origin,static_cast<ePedGroupDefaultTaskAllocatorType>(allocator));
            const char* expectedRoute=allocator<2?"formation":allocator<4?"chat":"random";
            const bool routeOK=allocator==4 ? trace.empty() : trace.size()==1&&trace[0]["route"]==expectedRoute&&trace[0]["self_matches"]==true&&trace[0]["type"]==7+n%10&&trace[0]["count"]==1+n%5&&trace[0]["origin_offset"]==4*(n%64)&&trace[0]["allocator"]==allocator;
            const bool alOK=allocator==4 ? returned==(uintptr_t(origin)&255) : returned==(n&1);
            result[side]={{"al",returned},{"trace",trace},{"origin_bytes",ReversingProbe::Bytes(origins.data(),origins.size())},{"placer_bytes",ReversingProbe::Bytes(&placer,sizeof(placer))},{"payload_unchanged",originBefore==origins&&placerBefore==ReversingProbe::Bytes(&placer,sizeof(placer))},{"helper_gates",helperGates},{"route_contract",routeOK},{"raw_al_contract",alOK},{"switched",switched}};
        }const bool equal=result[0]==result[1]&&result[0]["switched"]==true&&result[0]["payload_unchanged"]==true&&result[0]["helper_gates"]==true&&result[0]["route_contract"]==true&&result[0]["raw_al_contract"]==true;subsetPassed&=equal;failures+=!equal;write({{"event","comparison"},{"function","CPedGroupPlacer::PlaceGroup"},{"pair",pair++},{"case",n},{"allocator",allocator},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});}

    }
    hook->State(enabled);hook->LockState(locked);bool restored=SmallPendingSupport::HooksUnchanged(allHooks)&&gtaBefore==ReversingProbe::Bytes(hook->GetHookGTAAddress(),52)&&dllBefore==ReversingProbe::Bytes(hook->GetHookOurAddress(),52);for(size_t k=0;k<6;++k)restored&=bytesBefore[k]==ReversingProbe::Bytes(reinterpret_cast<void*>(addresses[k]),52);
    write({{"event","function_summary"},{"function","CPedGroupPlacer::PlaceGroup"},{"cases",1536},{"failures",failures},{"subset_passed",subsetPassed},{"allocator4_candidate_replayed",true}});
    write({{"event",subsetPassed&&restored?"done":"error"},{"pairs",1536},{"passed",subsetPassed&&restored},{"state_restored",restored}});
}
}
