#pragma once
#include "FixtureContracts.hpp"
namespace RequestedGeometryWrappersFiveV2Live {
inline bool done{};
inline unsigned randCalls{};
inline int __cdecl FixtureRand() { ++randCalls; return 55; }
inline ULONG_PTR* NamedRandSlot(HMODULE module) {
    if(!module)return nullptr;
    auto* b=reinterpret_cast<uint8*>(module);auto* d=reinterpret_cast<IMAGE_DOS_HEADER*>(b);
    if(d->e_magic!=IMAGE_DOS_SIGNATURE)return nullptr;
    auto* n=reinterpret_cast<IMAGE_NT_HEADERS*>(b+d->e_lfanew);
    if(n->Signature!=IMAGE_NT_SIGNATURE)return nullptr;
    const auto size=n->OptionalHeader.SizeOfImage;
    const auto rva=n->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if(!rva||rva>=size)return nullptr;
    auto* desc=reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(b+rva);
    for(;reinterpret_cast<uint8*>(desc)+sizeof(*desc)<=b+size&&desc->Name;++desc){
        if(!desc->OriginalFirstThunk||desc->OriginalFirstThunk>=size||desc->FirstThunk>=size)continue;
        auto* names=reinterpret_cast<IMAGE_THUNK_DATA*>(b+desc->OriginalFirstThunk);auto* slots=reinterpret_cast<IMAGE_THUNK_DATA*>(b+desc->FirstThunk);
        for(;reinterpret_cast<uint8*>(names)+sizeof(*names)<=b+size&&reinterpret_cast<uint8*>(slots)+sizeof(*slots)<=b+size&&names->u1.AddressOfData;++names,++slots){
            if(IMAGE_SNAP_BY_ORDINAL(names->u1.Ordinal)||names->u1.AddressOfData>=size)continue;
            auto* name=reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(b+names->u1.AddressOfData);
            if(!std::strcmp(reinterpret_cast<const char*>(name->Name),"rand"))return &slots->u1.Function;
        }
    }
    return nullptr;
}
inline void SetSlot(ULONG_PTR*slot,ULONG_PTR value){DWORD before{},ignored{};if(!VirtualProtect(slot,sizeof(value),PAGE_READWRITE,&before))std::abort();*slot=value;VirtualProtect(slot,sizeof(value),before,&ignored);}
__declspec(noinline) inline CEntity* ConstructFixture(void* storage) { return ::new(storage) CEntity{}; }
inline void Update() {
    if(done)return;done=true;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"requested-geometry-wrappers-five-v2-runtime.start"))return;
    std::ofstream log(dir/"requested-geometry-wrappers-five-v2-runtime.jsonl");auto write=[&](const json&r){log<<r.dump()<<'\n';log.flush();};
    using Simple=ReversibleHooks::ReversibleHook::Simple;
    constexpr uintptr_t targets[]{0x5F3650,0x5F3B40,0x5F2B80,0x5F36A0,0x5F2BC0};
    constexpr const char* names[]{"ComputeEntityBoundingBoxCorners","ComputeEntityBoundingBoxCentreUncachedAll","ComputeEntityBoundingBoxPlanesUncachedAll","ComputeEntityBoundingBoxSegmentPlanes","ComputeEntityBoundingBoxSegmentPlanesUncachedAll"};
    const auto flags=SmallPendingSupport::SnapshotHooks();std::array<Simple*,5> hooks{};
    for(const auto&f:flags)if(f.hook->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple){auto*h=static_cast<Simple*>(f.hook);for(size_t k=0;k<5;++k)if(h->GetHookGTAAddress()==reinterpret_cast<void*>(targets[k])&&h->Name()==names[k])hooks[k]=h;}
    if(std::any_of(hooks.begin(),hooks.end(),[](auto*p){return !p;})){write({{"event","error"},{"reason","exact wrapper identities missing"}});return;}
    HMODULE module{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(hooks[0]->GetHookOurAddress()),&module);
    auto* slot=NamedRandSlot(module);if(!slot||!*slot){write({{"event","error"},{"reason","named current ASI rand import missing; fixture not constructed"}});return;}
    const ULONG_PTR savedRand=*slot;const auto crtBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(savedRand),52);
    const auto iatBefore=ReversingProbe::Bytes(reinterpret_cast<uint8*>(slot)-24,52);
    alignas(CEntity)std::array<uint8,sizeof(CEntity)+32> entityArena{};entityArena.fill(0xCD);
    SetSlot(slot,reinterpret_cast<ULONG_PTR>(&FixtureRand));entity=ConstructFixture(entityArena.data()+16);SetSlot(slot,savedRand);
    const bool ctorControlled=randCalls==1&&entity->m_nRandomSeed==55&&crtBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(savedRand),52)&&iatBefore==ReversingProbe::Bytes(reinterpret_cast<uint8*>(slot)-24,52);
    const auto entityBefore=entityArena;
    write({{"event","fixture"},{"rand_calls",randCalls},{"rand_controlled",ctorControlled},{"named_iat",reinterpret_cast<uintptr_t>(slot)},{"rand_target",savedRand},{"entity_payload",ReversingProbe::Bytes(entity,sizeof(CEntity))},{"scope","typed inert entity, isolated primitive helpers; constructor/destructor/helper integration not credited"}});
    if(!ctorControlled){write({{"event","error"},{"reason","constructor rand path not exactly controlled"},{"state_restored",SmallPendingSupport::HooksUnchanged(flags)}});return;}
    const std::array<uintptr_t,8> helpers{0x5F1FA0,std::bit_cast<uintptr_t>(&CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached),0x5F1600,std::bit_cast<uintptr_t>(&CPedGeometryAnalyser::ComputeEntityBoundingBoxCentreUncached),0x5F1670,std::bit_cast<uintptr_t>(&CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached),0x5F1750,std::bit_cast<uintptr_t>(&CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanesUncached)};
    std::array<std::vector<uint8>,8> helperBefore;for(size_t k=0;k<8;++k)helperBefore[k]=ReversingProbe::Bytes(reinterpret_cast<void*>(helpers[k]),52);
    std::array<std::vector<uint8>,5> targetBefore,dllBefore;for(size_t k=0;k<5;++k){targetBefore[k]=ReversingProbe::Bytes(hooks[k]->GetHookGTAAddress(),52);dllBefore[k]=ReversingProbe::Bytes(hooks[k]->GetHookOurAddress(),52);}
    bool passed=true;int pair{},failures{};
    {
        ReversingProbe::EntryPatch c0(helpers[0],Corners),c1(helpers[1],Corners),m0(helpers[2],Centre),m1(helpers[3],Centre),p0(helpers[4],Planes),p1(helpers[5],Planes),s0(helpers[6],Segment),s1(helpers[7],Segment);
        std::array<std::vector<uint8>,8> helperActive;for(size_t k=0;k<8;++k)helperActive[k]=ReversingProbe::Bytes(reinterpret_cast<void*>(helpers[k]),52);
        for(function=0;function<5;++function){auto&h=*hooks[function];h.LockState(false);int fnFailures{};
            for(seed=0;seed<1024;++seed){json results[2];json raw[2];
                for(int side=0;side<2;++side){alignas(CVector)std::array<uint8,160> arena{};arena.fill(0xCD);output=reinterpret_cast<CVector*>(arena.data()+16);dots=reinterpret_cast<float*>(arena.data()+80);for(int i=0;i<4;++i)::new(output+i)CVector{};
                    producedCorners=producedCentre=nullptr;contract=true;trace=json::array();rawPointers=json::array();h.State(side!=0);bool gates=SmallPendingSupport::ExactSwitch(h,side!=0);for(size_t k=0;k<8;++k)gates&=helperActive[k]==ReversingProbe::Bytes(reinterpret_cast<void*>(helpers[k]),52);
                    const float z=seed%127==0?-0.f:float(seed%101-50)*.25f;uintptr_t returned{};
                    if(function<2)reinterpret_cast<void(__cdecl*)(float,CEntity&,CVector*)>(targets[function])(z,*entity,output);
                    else if(function<4)reinterpret_cast<void(__cdecl*)(float,CEntity&,CVector*,float*)>(targets[function])(z,*entity,output,dots);
                    else returned=reinterpret_cast<uintptr_t(__cdecl*)(float,CEntity&,CVector*,float*)>(targets[function])(z,*entity,output,dots);
                    const bool pointerRole=function!=4||returned==reinterpret_cast<uintptr_t>(producedCorners);
                    raw[side]={{"pointers",rawPointers},{"returned",returned}};
                    results[side]={{"trace",trace},{"arena",arena},{"entity_unchanged",entityArena==entityBefore},{"gates",gates},{"contracts",contract},{"return_role",pointerRole}};
                }
                const bool accepted=results[0]==results[1]&&results[0]["entity_unchanged"]==true&&results[0]["gates"]==true&&results[0]["contracts"]==true&&results[0]["return_role"]==true;passed&=accepted;failures+=!accepted;fnFailures+=!accepted;
                write({{"event","comparison"},{"function",std::string("CPedGeometryAnalyser::")+names[function]},{"pair",pair++},{"case",seed},{"wrapper",function},{"equal",results[0]==results[1]},{"accepted",accepted},{"original",results[0]},{"candidate",results[1]},{"raw_pointer_audit",raw}});
            }
            write({{"event","function_summary"},{"function",std::string("CPedGeometryAnalyser::")+names[function]},{"cases",1024},{"failures",fnFailures}});
        }
    }
    for(size_t k=0;k<5;++k){const auto it=std::find_if(flags.begin(),flags.end(),[&](const auto&f){return f.hook==hooks[k];});hooks[k]->LockState(false);hooks[k]->State(it->enabled);hooks[k]->LockState(it->locked);}
    bool restored=SmallPendingSupport::HooksUnchanged(flags)&&entityArena==entityBefore&&*slot==savedRand&&crtBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(savedRand),52)&&iatBefore==ReversingProbe::Bytes(reinterpret_cast<uint8*>(slot)-24,52);
    for(size_t k=0;k<8;++k)restored&=helperBefore[k]==ReversingProbe::Bytes(reinterpret_cast<void*>(helpers[k]),52);
    for(size_t k=0;k<5;++k)restored&=targetBefore[k]==ReversingProbe::Bytes(hooks[k]->GetHookGTAAddress(),52)&&dllBefore[k]==ReversingProbe::Bytes(hooks[k]->GetHookOurAddress(),52);
    // Placement fixture has null resources; storage ends here without invoking infrastructure callbacks.
    write({{"event",passed&&restored?"complete":"error"},{"pairs",5120},{"failures",failures},{"passed",passed&&restored},{"state_restored",restored},{"rand_controlled",ctorControlled}});
}
}
