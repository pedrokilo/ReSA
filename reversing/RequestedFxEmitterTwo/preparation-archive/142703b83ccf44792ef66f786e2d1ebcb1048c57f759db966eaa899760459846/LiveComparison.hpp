#pragma once
#include "FxEmitterBP.h"
#include "../ProbeRuntime.hpp"
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
namespace RequestedFxEmitterTwo {
using namespace ReversibleHooks::ReversibleHook;
alignas(16) inline std::array<uint8,96> storage;
alignas(16) inline std::array<uint8,160> nameStorage;
alignas(16) inline std::array<RwTexture,8> textures;
inline json trace;
inline unsigned successMask{},fallbackMask{},ordinal{};
inline uint8* Object(){return storage.data()+16;}
inline FxName32_t* Names(){return reinterpret_cast<FxName32_t*>(nameStorage.data()+16);}
template<class T> inline void Put(void* p,size_t offset,T value){std::memcpy(static_cast<uint8*>(p)+offset,&value,sizeof(value));}
inline RwTexture* __cdecl Read(const char* name,const char* mask) {
    // Find the exact private slot address; string equality alone cannot distinguish duplicate names.
    int slot=-1;for(int j=0;j<4;++j)if(name==Names()[j])slot=j;
    const bool bounded=slot>=0&&std::memchr(name,0,32)!=nullptr;
    const std::string copied=bounded?std::string(name):std::string("INVALID");
    const bool maskBounded=!mask||std::memchr(mask,0,64)!=nullptr;
    const bool good=bounded&&maskBounded&&((mask?successMask:fallbackMask)&(1u<<unsigned(slot>=0?slot:0)));
    auto* result=good?&textures[size_t(slot)+4]:nullptr;
    trace.push_back({{"ordinal",ordinal++},{"slot",slot},{"name",copied},{"mask",mask?(maskBounded?json(std::string(mask)):json("INVALID")):json(nullptr)},{"bounded",bounded&&maskBounded},{"result",good?json::array({"textures",slot+4}):json(nullptr)}});
    return result;
}
inline Base* Find(const char* name){Base* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="FxEmitterBP_c")for(auto& h:c.Items())if(h->Name()==name)result=h.get();});return result;}
inline std::pair<uintptr_t,uintptr_t> Entries(Base* h){if(h->Type()==Base::HookType::Simple){auto* p=static_cast<Simple*>(h);return {uintptr_t(p->GetHookGTAAddress()),uintptr_t(p->GetHookOurAddress())};}auto* p=static_cast<Virtual*>(h);return {uintptr_t(p->GetHookGTAAddress()),uintptr_t(p->GetHookOurAddress())};}
inline void Update(){
    char path[MAX_PATH]{};GetModuleFileNameA(nullptr,path,MAX_PATH);const auto dir=std::filesystem::path(path).parent_path();
    std::ofstream log(dir/"fx-emitter-two.jsonl");auto write=[&](const json& row){log<<row.dump()<<'\n';log.flush();};
    auto* ctor=Find("Constructor");auto* loader=Find("LoadTextures");
    if(!ctor||!loader){write({{"event","error"},{"reason","missing target hooks"}});write({{"event","complete"},{"comparisons",0},{"all_equal",false},{"state_restored",true}});return;}
    const auto ctorEntries=Entries(ctor),loadEntries=Entries(loader);
    if(ctorEntries.first!=0x4A18D0||loadEntries.first!=0x5C0A30){write({{"event","error"},{"reason","wrong target identities"}});write({{"event","complete"},{"comparisons",0},{"all_equal",false},{"state_restored",true}});return;}
    const uintptr_t dllVT=uintptr_t(ReversibleHooks::detail::GetVTableAddress("FxEmitterBP_c"));
    const auto savedStorage=storage;const auto savedNames=nameStorage;const auto savedTextures=ReversingProbe::Bytes(textures.data(),sizeof(textures));const auto savedTrace=trace;const auto savedSuccess=successMask,savedFallback=fallbackMask,savedOrdinal=ordinal;const unsigned savedFP=_controlfp(0,0);
    const std::array<bool,4> hookFlags{ctor->Hooked(),ctor->Locked(),loader->Hooked(),loader->Locked()};ctor->LockState(false);loader->LockState(false);ctor->State(false);loader->State(false);
    std::vector<uintptr_t> readEntries{0x7F3AC0,uintptr_t(&RwTextureRead)};uintptr_t resolved=uintptr_t(&RwTextureRead);json jumpChain=json::array();
    for(unsigned j=0;j<16&&*reinterpret_cast<const uint8*>(resolved)==0xE9;++j){const uintptr_t next=resolved+5+*reinterpret_cast<const int32*>(resolved+1);jumpChain.push_back({resolved,next});resolved=next;}
    if(std::find(readEntries.begin(),readEntries.end(),resolved)==readEntries.end())readEntries.push_back(resolved);
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> readBefore;std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> readGates;
    for(auto p:readEntries)readBefore.push_back({p,ReversingProbe::Bytes(reinterpret_cast<void*>(p),52)});
    write({{"event","begin"},{"native_vtable",0x85A788},{"candidate_vtable",dllVT},{"native_table",ReversingProbe::Bytes(reinterpret_cast<void*>(0x85A788),28)},{"candidate_table",ReversingProbe::Bytes(reinterpret_cast<void*>(dllVT),28)},{"rw_native",0x7F3AC0},{"rw_candidate_entry",uintptr_t(&RwTextureRead)},{"rw_candidate_body",resolved},{"rw_jump_chain",jumpChain},{"rw_body_before",ReversingProbe::Bytes(reinterpret_cast<void*>(resolved),128)},{"private_texture_addresses",json::array()}});
    for(auto p:readEntries)readGates.push_back(std::make_unique<ReversingProbe::EntryPatch>(p,Read));
    const char* names[8][4]{{"NULL","NULL","NULL","NULL"},{"NULLX","NULLX","NULLX","NULLX"},{"","","",""},{"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa","aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa","aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa","aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"},{"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb","bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb","bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb","bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"},{"smoke","NULL","","NULLX"},{"NULLX","bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb","NULL","aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"},{"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa","NULLX","bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb","NULL"}};
    constexpr int32 versions[]{100,101,102,-1,INT_MAX};constexpr unsigned fallbacks[]{0,15,5,10};
    size_t pair=0,differences=0;bool all=true;
    for(int function=0;function<2;++function){const int count=function?5120:512;auto* hook=function?loader:ctor;const auto entry=function?loadEntries:ctorEntries;
        for(int i=0;i<count;++i){const int profile=i%8,versionIndex=(i/8)%5;const unsigned first=(i/40)%16,fallbackIndex=(i/640)%4;const bool seeded=(i/2560)%2;const uint8 poison=function?uint8((profile%4)*85):uint8(i%256);const bool patterned=!function&&(i/256)%2;
            json fixture{{"index",i},{"poison",poison},{"patterned",patterned},{"profile",profile},{"version",versions[versionIndex]},{"success_mask",first},{"fallback_mask",fallbacks[fallbackIndex]},{"seeded",seeded}};json legs[2];
            for(int leg=0;leg<2;++leg){storage.fill(0xCD);nameStorage.fill(0xD3);std::memset(textures.data(),0xA7,sizeof(textures));
                for(int b=0;b<64;++b)Object()[b]=patterned?uint8(poison+b*37):poison;
                const uintptr_t vt=leg?dllVT:uintptr_t(0x85A788);Put(Object(),0,vt);
                for(int j=0;j<4;++j){std::memset(Names()[j],poison,32);std::memcpy(Names()[j],names[profile][j],std::strlen(names[profile][j])+1);}
                if(function){
                    // Establish a real complete CPU constructor before the load fixture, then seed private opaque handles.
                    ctor->State(leg!=0);reinterpret_cast<void*(__thiscall*)(void*)>(ctorEntries.first)(Object());
                    for(int j=0;j<4;++j)Put(Object(),12+j*4,seeded?uintptr_t(&textures[j]):uintptr_t(0));
                }
                const auto before=storage;const auto beforeNames=nameStorage;const auto beforeTextures=ReversingProbe::Bytes(textures.data(),sizeof(textures));
                trace=json::array();successMask=first;fallbackMask=fallbacks[fallbackIndex];ordinal=0;hook->State(leg!=0);_controlfp(savedFP,_MCW_PC|_MCW_RC);
                uintptr_t returned=function?uintptr_t(reinterpret_cast<bool(__thiscall*)(void*,FxName32_t*,int32)>(entry.first)(Object(),Names(),versions[versionIndex])):uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(entry.first)(Object()));
                bool gates=true;for(size_t g=0;g<readEntries.size();++g)gates&=ReversingProbe::Bytes(reinterpret_cast<void*>(readEntries[g]),5)==std::vector<uint8>(readGates[g]->jump.begin(),readGates[g]->jump.end());
                const uintptr_t rawVT=*reinterpret_cast<uintptr_t*>(Object());const bool role=rawVT==vt;const bool targetGate=hook->Hooked()==bool(leg)&&ReversingProbe::JumpsToDll(entry.first)==bool(leg);
                json pointers=json::array();for(int j=0;j<8;++j)pointers.push_back(uintptr_t(&textures[j]));
                legs[leg]={{"payload",{{"object",std::vector<uint8>(Object()+4,Object()+64)},{"guards_before",std::vector<uint8>(storage.begin(),storage.begin()+16)},{"guards_after",std::vector<uint8>(storage.begin()+80,storage.end())},{"names",nameStorage},{"textures",ReversingProbe::Bytes(textures.data(),sizeof(textures))},{"trace",trace},{"returned",function?json(returned):json::array({"storage",16})}}},{"raw_vptr",rawVT},{"expected_vptr",vt},{"vptr_role_exact",role},{"target_gate",targetGate},{"helper_gates",gates},{"names_immutable",beforeNames==nameStorage},{"textures_immutable",beforeTextures==ReversingProbe::Bytes(textures.data(),sizeof(textures))},{"before",before},{"after",storage},{"texture_addresses",pointers},{"entry_native",ReversingProbe::Bytes(reinterpret_cast<void*>(entry.first),52)},{"entry_candidate",ReversingProbe::Bytes(reinterpret_cast<void*>(entry.second),52)}};
            }
            bool equal=legs[0]["payload"]==legs[1]["payload"];for(const auto& result:legs)for(const auto* flag:{"vptr_role_exact","target_gate","helper_gates","names_immutable","textures_immutable"})equal&=result[flag]==true;
            all&=equal;differences+=!equal;write({{"event","comparison"},{"function",function?"FxEmitterBP_c::LoadTextures":"FxEmitterBP_c::Constructor"},{"fixture",fixture},{"pair",pair++},{"equal",equal},{"original",legs[0]},{"candidate",legs[1]}});
        }
    }
    ctor->State(hookFlags[0]);ctor->LockState(hookFlags[1]);loader->State(hookFlags[2]);loader->LockState(hookFlags[3]);
    readGates.clear();bool restored=true;for(const auto& [p,bytes]:readBefore)restored&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(p),52);
    storage=savedStorage;nameStorage=savedNames;std::memcpy(textures.data(),savedTextures.data(),sizeof(textures));trace=savedTrace;successMask=savedSuccess;fallbackMask=savedFallback;ordinal=savedOrdinal;_controlfp(savedFP,_MCW_PC|_MCW_RC);
    restored&=storage==savedStorage&&nameStorage==savedNames&&savedTextures==ReversingProbe::Bytes(textures.data(),sizeof(textures));
    write({{"event","complete"},{"comparisons",pair},{"differences",differences},{"all_equal",all},{"state_restored",restored},{"fp_restored",unsigned(_controlfp(0,0))==savedFP}});
}
}
