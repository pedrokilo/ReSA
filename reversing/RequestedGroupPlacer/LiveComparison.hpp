#pragma once
#include <fstream>
#include "PedGroupPlacer.h"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedGroupPlacerLive {
inline bool done{};
inline json trace;
inline int fixture{};
inline CPedGroupPlacer placer{};
alignas(256) inline std::array<uint8,512> origins{};
inline bool Dispatch(const char* route, CPedGroupPlacer* self, ePedType type, uint32 count, const CVector& origin, ePedGroupDefaultTaskAllocatorType allocator) {
    trace.push_back({"route",route,"self_matches",self==&placer,"type",int(type),"count",count,"origin_offset",uintptr_t(&origin)-uintptr_t(origins.data()),"origin_bits",{std::bit_cast<uint32>(origin.x),std::bit_cast<uint32>(origin.y),std::bit_cast<uint32>(origin.z)},"allocator",int(allocator)});
    return (fixture&1)!=0;
}
inline bool __fastcall Formation(CPedGroupPlacer* self,void*,ePedType type,uint32 count,const CVector& origin,ePedGroupDefaultTaskAllocatorType allocator){return Dispatch("formation",self,type,count,origin,allocator);}
inline bool __fastcall Chat(CPedGroupPlacer* self,void*,ePedType type,uint32 count,const CVector& origin,ePedGroupDefaultTaskAllocatorType allocator){return Dispatch("chat",self,type,count,origin,allocator);}
inline bool __fastcall Random(CPedGroupPlacer* self,void*,ePedType type,uint32 count,const CVector& origin,ePedGroupDefaultTaskAllocatorType allocator){return Dispatch("random",self,type,count,origin,allocator);}
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"group-placer-pending-runtime.start")){done=true;return;}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;std::ofstream log(dir/"group-placer-pending-runtime.jsonl");auto write=[&](const json&r){log<<r.dump()<<'\n';log.flush();};
    ReversibleHooks::ReversibleHook::Simple* hook{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto&cat){if(cat.Name()=="CPedGroupPlacer")for(auto&item:cat.Items())if(item->Name()=="PlaceGroup")hook=static_cast<ReversibleHooks::ReversibleHook::Simple*>(item.get());});
    if(!hook){write({{"event","error"},{"reason","missing PlaceGroup hook"}});return;}
    const auto allHooks=SmallPendingSupport::SnapshotHooks();const auto gtaBefore=ReversingProbe::Bytes(hook->GetHookGTAAddress(),52),dllBefore=ReversingProbe::Bytes(hook->GetHookOurAddress(),52);const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
    const std::array<uintptr_t,6> addresses{0x5FC9B0,std::bit_cast<uintptr_t>(&CPedGroupPlacer::PlaceFormationGroup),0x5FCE80,std::bit_cast<uintptr_t>(&CPedGroupPlacer::PlaceChatGroup),0x5FD330,std::bit_cast<uintptr_t>(&CPedGroupPlacer::PlaceRandomGroup)};
    std::array<std::vector<uint8>,6> bytesBefore;for(size_t k=0;k<6;++k)bytesBefore[k]=ReversingProbe::Bytes(reinterpret_cast<void*>(addresses[k]),5);
    bool subsetPassed=true;int failures{},pair{};
    {
        ReversingProbe::EntryPatch a0(addresses[0],Formation),a1(addresses[1],Formation),b0(addresses[2],Chat),b1(addresses[3],Chat),c0(addresses[4],Random),c1(addresses[5],Random);
        constexpr int allocators[]{0,1,2,3,5};
        for(const int allocator:allocators)for(int n=0;n<256;++n){json result[2];for(int side=0;side<2;++side){fixture=n;trace=json::array();origins.fill(0xCD);auto*origin=::new(origins.data()+4*(n%64))CVector{float(n%17)-8.f,float(n%11)*.25f,3.f};hook->State(side!=0);const bool switched=SmallPendingSupport::ExactSwitch(*hook,side!=0);
            const auto returned=reinterpret_cast<uint8(__thiscall*)(CPedGroupPlacer*,ePedType,uint32,const CVector&,ePedGroupDefaultTaskAllocatorType)>(0x5FD810)(&placer,static_cast<ePedType>(7+n%10),1+n%5,*origin,static_cast<ePedGroupDefaultTaskAllocatorType>(allocator));
            result[side]={{"al",returned},{"trace",trace},{"origin_bytes",ReversingProbe::Bytes(origins.data(),origins.size())},{"switched",switched}};
        }const bool equal=result[0]==result[1]&&result[0]["switched"]==true;subsetPassed&=equal;failures+=!equal;write({{"event","comparison"},{"function","CPedGroupPlacer::PlaceGroup"},{"pair",pair++},{"case",n},{"allocator",allocator},{"equal",equal},{"accepted",equal},{"original",result[0]},{"candidate",result[1]}});}
        // Original-only characterization: enum4 is valid, but its raw AL is a pointer byte.
        // Calling the current candidate would enter a modal unreachable dialog and exit3.
        for(const int offset:{0,4,128,252}){
            trace=json::array();origins.fill(0xCD);auto*origin=::new(origins.data()+offset)CVector{1.f,2.f,3.f};hook->State(false);
            const bool switched=SmallPendingSupport::ExactSwitch(*hook,false);
            const auto returned=reinterpret_cast<uint8(__thiscall*)(CPedGroupPlacer*,ePedType,uint32,const CVector&,ePedGroupDefaultTaskAllocatorType)>(0x5FD810)(&placer,static_cast<ePedType>(7),1,*origin,ePedGroupDefaultTaskAllocatorType::SIT_IN_LEADER_CAR);
            write({{"event","characterization"},{"function","CPedGroupPlacer::PlaceGroup"},{"allocator",4},{"origin_low_byte",uintptr_t(origin)&255},{"returned_al",returned},{"trace",trace},{"switched",switched},{"expected_raw_al",returned==(uintptr_t(origin)&255)}});
        }
    }
    hook->State(enabled);hook->LockState(locked);bool restored=SmallPendingSupport::HooksUnchanged(allHooks)&&gtaBefore==ReversingProbe::Bytes(hook->GetHookGTAAddress(),52)&&dllBefore==ReversingProbe::Bytes(hook->GetHookOurAddress(),52);for(size_t k=0;k<6;++k)restored&=bytesBefore[k]==ReversingProbe::Bytes(reinterpret_cast<void*>(addresses[k]),5);
    write({{"event","function_summary"},{"function","CPedGroupPlacer::PlaceGroup"},{"cases",1280},{"failures",failures},{"subset_passed",subsetPassed},{"allocator4_candidate_replayed",false}});
    write({{"event","error"},{"reason","valid allocator4 lacks a candidate returning the original raw AL; full function remains pending"},{"pairs",1280},{"subset_passed",subsetPassed},{"state_restored",restored}});
}
}
