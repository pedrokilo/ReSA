#pragma once
#include "FixtureContracts.hpp"
namespace RequestedGeometryWrappersFiveV3Live {
inline bool done{};
inline unsigned randCalls{};
inline uint16 __cdecl FixtureRand() { ++randCalls; return 55; }
inline uintptr_t imageBase{},imageEnd{};
inline bool Within(uintptr_t p,size_t n=1){return p>=imageBase&&p+n>=p&&p+n<=imageEnd;}
inline uintptr_t Resolve(uintptr_t entry) {
    for(int i=0;i<8;++i){if(!Within(entry,5))return 0;if(*reinterpret_cast<uint8*>(entry)!=0xE9)return entry;entry=entry+5+*reinterpret_cast<int32*>(entry+1);}
    return 0;
}
inline uintptr_t Call(uintptr_t call){if(!Within(call,5)||*reinterpret_cast<uint8*>(call)!=0xE8)return 0;const auto destination=call+5+*reinterpret_cast<int32*>(call+1);return Within(destination,5)?destination:0;}
__declspec(noinline) inline CEntity* ConstructFixture(void* storage) { return ::new(storage) CEntity{}; }
inline void Update() {
    if(done)return;done=true;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"requested-geometry-wrappers-five-v3-runtime.start"))return;
    std::ofstream log(dir/"requested-geometry-wrappers-five-v3-runtime.jsonl");auto write=[&](const json&r){log<<r.dump()<<'\n';log.flush();};
    using Simple=ReversibleHooks::ReversibleHook::Simple;
    constexpr uintptr_t targets[]{0x5F3650,0x5F3B40,0x5F2B80,0x5F36A0,0x5F2BC0};
    constexpr const char* names[]{"ComputeEntityBoundingBoxCorners","ComputeEntityBoundingBoxCentreUncachedAll","ComputeEntityBoundingBoxPlanesUncachedAll","ComputeEntityBoundingBoxSegmentPlanes","ComputeEntityBoundingBoxSegmentPlanesUncachedAll"};
    const auto flags=SmallPendingSupport::SnapshotHooks();std::array<Simple*,5> hooks{};
    for(const auto&f:flags)if(f.hook->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple){auto*h=static_cast<Simple*>(f.hook);for(size_t k=0;k<5;++k)if(h->GetHookGTAAddress()==reinterpret_cast<void*>(targets[k])&&h->Name()==names[k])hooks[k]=h;}
    if(std::any_of(hooks.begin(),hooks.end(),[](auto*p){return !p;})){write({{"event","error"},{"reason","exact wrapper identities missing"}});return;}
    HMODULE module{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(hooks[0]->GetHookOurAddress()),&module);
    imageBase=reinterpret_cast<uintptr_t>(module);
    if(!module){write({{"event","error"},{"reason","own ASI module lookup failed"}});return;}
    auto* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(module);auto* nt=reinterpret_cast<IMAGE_NT_HEADERS*>(imageBase+dos->e_lfanew);imageEnd=imageBase+nt->OptionalHeader.SizeOfImage;
    const auto wrapperBody=Resolve(std::bit_cast<uintptr_t>(&ConstructFixture));
    constexpr uint8 ctorCallPrefix[]{0x8B,0x8D,0x38,0xFF,0xFF,0xFF};
    const auto ctorEntry=wrapperBody?Call(wrapperBody+0x35):0;
    const auto ctorBody=Resolve(ctorEntry);
    const auto generalEntry=std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),generalBody=Resolve(generalEntry);
    const auto ctorGeneralEntry=ctorBody?Call(ctorBody+0xDE):0;
    constexpr uint8 seedStore[]{0x8B,0x4D,0xEC,0x66,0x89,0x41,0x20};
    const auto randEntry=generalBody?Call(generalBody+0x17):0,randBody=Resolve(randEntry);
    const bool callProof=Within(wrapperBody+0x2F,6)&&std::memcmp(reinterpret_cast<void*>(wrapperBody+0x2F),ctorCallPrefix,6)==0&&ctorBody&&Within(ctorBody+0xE3,7)&&std::memcmp(reinterpret_cast<void*>(ctorBody+0xE3),seedStore,7)==0&&ctorGeneralEntry&&Resolve(ctorGeneralEntry)==generalBody&&generalBody&&randBody&&Within(generalEntry,52)&&Within(generalBody,52)&&Within(ctorGeneralEntry,52)&&Within(randBody,52);
    if(!callProof){write({{"event","error"},{"reason","current ConstructFixture/ctor/General CALL layout does not match exact verified prefixes and source symbol path; no constructor run"}});return;}
    std::vector<uintptr_t> generalAddresses;for(auto address:{generalEntry,generalBody,ctorGeneralEntry})if(std::find(generalAddresses.begin(),generalAddresses.end(),address)==generalAddresses.end())generalAddresses.push_back(address);
    std::vector<std::vector<uint8>> generalBefore;for(auto address:generalAddresses)generalBefore.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(address),52));
    const auto crtBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(randBody),52);
    alignas(CEntity)std::array<uint8,sizeof(CEntity)+32> entityArena{};entityArena.fill(0xCD);
    bool privateGeneralGates=true;
    {
        std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
        for(auto address:generalAddresses)patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(address,FixtureRand));
        for(const auto& patch:patches)privateGeneralGates &= std::memcmp(reinterpret_cast<void*>(patch->address),patch->jump.data(),5)==0;
        entity=ConstructFixture(entityArena.data()+16);
    }
    bool generalRestored=true;for(size_t k=0;k<generalAddresses.size();++k)generalRestored &= generalBefore[k]==ReversingProbe::Bytes(reinterpret_cast<void*>(generalAddresses[k]),52);
    const bool ctorControlled=callProof&&privateGeneralGates&&generalRestored&&randCalls==1&&entity->m_nRandomSeed==55&&crtBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(randBody),52);
    const auto entityBefore=entityArena;
    write({{"event","fixture"},{"rand_calls",randCalls},{"rand_controlled",ctorControlled},{"callsite_proven",callProof},{"general_entry",generalEntry},{"general_body",generalBody},{"ctor_entry",ctorEntry},{"ctor_body",ctorBody},{"ctor_general_call",ctorGeneralEntry},{"rand_body_readonly",randBody},{"general_private_gates",privateGeneralGates},{"general_restored",generalRestored},{"entity_payload",ReversingProbe::Bytes(entity,sizeof(CEntity))},{"scope","typed inert entity, isolated primitive helpers; constructor/destructor/helper integration not credited"}});
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
    generalRestored=true;for(size_t k=0;k<generalAddresses.size();++k)generalRestored &= generalBefore[k]==ReversingProbe::Bytes(reinterpret_cast<void*>(generalAddresses[k]),52);
    bool restored=SmallPendingSupport::HooksUnchanged(flags)&&entityArena==entityBefore&&generalRestored&&crtBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(randBody),52);
    for(size_t k=0;k<8;++k)restored&=helperBefore[k]==ReversingProbe::Bytes(reinterpret_cast<void*>(helpers[k]),52);
    for(size_t k=0;k<5;++k)restored&=targetBefore[k]==ReversingProbe::Bytes(hooks[k]->GetHookGTAAddress(),52)&&dllBefore[k]==ReversingProbe::Bytes(hooks[k]->GetHookOurAddress(),52);
    // Placement fixture has null resources; storage ends here without invoking infrastructure callbacks.
    write({{"event",passed&&restored?"complete":"error"},{"pairs",5120},{"failures",failures},{"passed",passed&&restored},{"state_restored",restored},{"rand_controlled",ctorControlled}});
}
}
