#pragma once
#include <reversiblehooks/ReversibleHook/Simple.h>

namespace DependenciesCollisionAdapters1Live {
inline void BankModuleMarker() {}
inline uintptr_t ModuleOf(uintptr_t address) {
    MEMORY_BASIC_INFORMATION info{};
    return VirtualQuery(reinterpret_cast<void*>(address),&info,sizeof(info))==sizeof(info)
        ? reinterpret_cast<uintptr_t>(info.AllocationBase):0;
}
inline uintptr_t Resolve(uintptr_t address) {
    for(unsigned i=0;i<16&&*reinterpret_cast<const uint8*>(address)==0xE9;++i)
        address+=5+*reinterpret_cast<const int32*>(address+1);
    return address;
}
// Unlike the four target routes, helpers stay ON throughout the bank.
// Capture the source body using stored original ILT bytes before enabling RH.
class HelperHookRoute {
    ReversibleHooks::ReversibleHook::Simple* hook{};
    uintptr_t entry{},raw{},body{},module{};
    bool enabled{},locked{},restored{};
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> initial,on;
public:
    HelperHookRoute(const char* cls,const char* name,uintptr_t original,void* source) : entry(original),raw(reinterpret_cast<uintptr_t>(source)) {
        module=ModuleOf(reinterpret_cast<uintptr_t>(&BankModuleMarker));
        ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category) {
            if(category.Name()!=cls)return;
            for(auto& candidate:category.Items())
                if(candidate->Name()==name&&candidate->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple)
                    hook=static_cast<ReversibleHooks::ReversibleHook::Simple*>(candidate.get());
        });
        if(!hook)return;
        enabled=hook->Hooked();locked=hook->Locked();
        const auto library=reinterpret_cast<uintptr_t>(hook->GetHookOurAddress());
        body=library;
        for(unsigned i=0;i<16;++i) {
            const auto* bytes=body==library ? hook->m_LibOriginalFunctionContent:reinterpret_cast<const uint8*>(body);
            if(bytes[0]!=0xE9)break;
            int32 delta;std::memcpy(&delta,bytes+1,4);body+=5+delta;
        }
        for(auto at:{entry,raw,library,body})initial.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
        hook->LockState(false);hook->State(true);hook->LockState(true);
        for(auto& [at,bytes]:initial)on.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    }
    bool Registered() const { return hook!=nullptr; }
    bool Check() const {
        bool ok=hook&&hook->Hooked()&&hook->Locked()&&module!=0&&body!=0
            &&ReversingProbe::JumpsToDll(entry)&&Resolve(entry)==body&&Resolve(raw)==body
            &&ModuleOf(raw)==module&&ModuleOf(body)==module;
        for(auto& [at,bytes]:on)ok&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
        return ok;
    }
    json Metadata(const char* cls,const char* name) const {
        return {{"class",cls},{"function",name},{"entry",entry},{"raw_symbol",raw},{"body",body},
            {"asi_module",module},{"helper_route_ok",Check()},{"shared_reconstructed_collaborator",true}};
    }
    bool Restore() {
        if(!restored&&hook) {hook->LockState(false);hook->State(enabled);hook->LockState(locked);}
        restored=true;
        bool ok=hook&&hook->Hooked()==enabled&&hook->Locked()==locked;
        for(auto& [at,bytes]:initial)ok&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
        return ok;
    }
    ~HelperHookRoute() { Restore(); }
};
}
