#pragma once

namespace DependenciesCollisionAdapters1Live {
enum class Phase { Setup, Execute, Cleanup };
struct AllocationRecord {
    void* address{};
    uint32 size{};
    unsigned id{};
    Phase allocatedDuring{},freedDuring{};
    bool live{};
    std::vector<uint8> initial,final;
};
inline std::array<AllocationRecord,16> allocationRecords;
inline unsigned allocationCount{};
inline Phase allocationPhase{};
inline bool allocatorRouteOK{};
inline std::vector<unsigned> executionFrees;
inline std::unique_ptr<ReversingProbe::EntryPatch> mallocTrace,freeTrace;
inline std::vector<uint8> mallocOriginal,freeOriginal;
inline void* __cdecl TraceMalloc(uint32 size) {
    mallocTrace->Enable(false);
    allocatorRouteOK&=mallocOriginal==ReversingProbe::Bytes(reinterpret_cast<void*>(0x72F420),16);
    void* result=reinterpret_cast<void*(__cdecl*)(uint32)>(0x72F420)(size);
    mallocTrace->Enable(true);
    allocatorRouteOK&=ReversingProbe::JumpsToDll(0x72F420);
    if(!result||allocationCount>=allocationRecords.size()||size<10||size>50||size%10)std::abort();
    auto& record=allocationRecords[allocationCount];
    record.address=result;record.size=size;record.id=++allocationCount;
    record.allocatedDuring=allocationPhase;record.live=true;
    record.initial=ReversingProbe::Bytes(result,size);record.final.clear();
    return result;
}
inline void __cdecl TraceFree(void* address) {
    auto* owned=static_cast<AllocationRecord*>(nullptr);
    for(unsigned i=0;i<allocationCount;++i)
        if(allocationRecords[i].live&&allocationRecords[i].address==address)owned=&allocationRecords[i];
    // Fail rather than free any resource belonging to the real game's cache.
    if(!owned)std::abort();
    owned->final=ReversingProbe::Bytes(address,owned->size);
    owned->freedDuring=allocationPhase;owned->live=false;
    if(allocationPhase==Phase::Execute)executionFrees.push_back(owned->id);
    freeTrace->Enable(false);
    allocatorRouteOK&=freeOriginal==ReversingProbe::Bytes(reinterpret_cast<void*>(0x72F430),16);
    reinterpret_cast<void(__cdecl*)(void*)>(0x72F430)(address);
    freeTrace->Enable(true);
    allocatorRouteOK&=ReversingProbe::JumpsToDll(0x72F430);
}
inline void BeginAllocationTrace() {
    allocationCount=0;executionFrees.clear();allocationPhase=Phase::Setup;
    mallocOriginal=ReversingProbe::Bytes(reinterpret_cast<void*>(0x72F420),16);
    freeOriginal=ReversingProbe::Bytes(reinterpret_cast<void*>(0x72F430),16);
    int32 mallocCall{},freeJump{};
    std::memcpy(&mallocCall,mallocOriginal.data()+6,4);std::memcpy(&freeJump,freeOriginal.data()+1,4);
    allocatorRouteOK=mallocOriginal[0]==0x8B&&mallocOriginal[1]==0x44&&mallocOriginal[2]==0x24&&mallocOriginal[3]==4
        &&mallocOriginal[4]==0x50&&mallocOriginal[5]==0xE8&&uintptr_t(0x72F42A+mallocCall)==0x824257
        &&mallocOriginal[10]==0x83&&mallocOriginal[11]==0xC4&&mallocOriginal[12]==4&&mallocOriginal[13]==0xC3
        &&freeOriginal[0]==0xE9&&uintptr_t(0x72F435+freeJump)==0x82413F;
    if(!allocatorRouteOK)std::abort();
    mallocTrace=std::make_unique<ReversingProbe::EntryPatch>(0x72F420,&TraceMalloc);
    freeTrace=std::make_unique<ReversingProbe::EntryPatch>(0x72F430,&TraceFree);
    allocatorRouteOK&=Resolve(0x72F420)==Resolve(reinterpret_cast<uintptr_t>(&TraceMalloc))
        &&Resolve(0x72F430)==Resolve(reinterpret_cast<uintptr_t>(&TraceFree));
}
inline unsigned AllocationId(void* address) {
    if(!address)return 0;
    for(unsigned i=0;i<allocationCount;++i)
        if(allocationRecords[i].live&&allocationRecords[i].address==address)return allocationRecords[i].id;
    return 0xFFFFFFFFu;
}
inline bool AllocationPaddingPreserved(const AllocationRecord& record) {
    if(record.live||record.final.size()!=record.initial.size())return false;
    const unsigned triangles=record.size/10-1;
    const unsigned link=(triangles*10+3)&~3u;
    for(unsigned offset=0;offset<record.size;++offset) {
        const bool planeField=offset<triangles*10&&offset%10<9;
        const bool linkField=offset>=link&&offset<link+4;
        if(!planeField&&!linkField&&record.final[offset]!=record.initial[offset])return false;
    }
    return true;
}
inline json AllocationObservations() {
    json observations=json::array();
    for(unsigned i=0;i<allocationCount;++i) {
        const auto& record=allocationRecords[i];
        observations.push_back({{"id",record.id},{"actual_address",reinterpret_cast<uintptr_t>(record.address)},
            {"size",record.size},{"initial_raw",record.initial},{"final_raw",record.final},
            {"allocated_phase",int(record.allocatedDuring)},{"freed_phase",int(record.freedDuring)},
            {"padding_preserved",AllocationPaddingPreserved(record)}});
    }
    return observations;
}
inline bool EndAllocationTrace() {
    bool balanced=true;
    for(unsigned i=0;i<allocationCount;++i)balanced&=!allocationRecords[i].live&&AllocationPaddingPreserved(allocationRecords[i]);
    mallocTrace.reset();freeTrace.reset();
    return balanced&&allocatorRouteOK&&mallocOriginal==ReversingProbe::Bytes(reinterpret_cast<void*>(0x72F420),16)
        &&freeOriginal==ReversingProbe::Bytes(reinterpret_cast<void*>(0x72F430),16);
}
}
