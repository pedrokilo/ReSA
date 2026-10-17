#pragma once
#include <reversiblehooks/ReversibleHook/Simple.h>
namespace DependenciesStatsSex16Live {
inline void ModuleMarker() {}
inline uintptr_t ModuleOf(uintptr_t address) {MEMORY_BASIC_INFORMATION i{};return VirtualQuery(reinterpret_cast<void*>(address),&i,sizeof(i))==sizeof(i)?uintptr_t(i.AllocationBase):0;}
inline uintptr_t Resolve(uintptr_t address) {for(unsigned i=0;i<16&&*reinterpret_cast<uint8*>(address)==0xE9;++i)address+=5+*reinterpret_cast<int32*>(address+1);return address;}
// Read-only proof: collaborators retain the exact initial shared mode.
class HelperRoute {
    ReversibleHooks::ReversibleHook::Simple* hook{};
    uintptr_t entry{},library{},body{},module{};bool enabled{},locked{},reversed{};
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> bytes;
public:
    explicit HelperRoute(uintptr_t address):entry(address),module(ModuleOf(uintptr_t(&ModuleMarker))) {
        ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category) {
            for(auto& item:category.Items())if(item->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple) {
                auto* simple=static_cast<ReversibleHooks::ReversibleHook::Simple*>(item.get());
                if(uintptr_t(simple->GetHookGTAAddress())==entry)hook=simple;
            }
        });
        if(hook) {
            enabled=hook->Hooked();locked=hook->Locked();reversed=hook->Reversed();library=uintptr_t(hook->GetHookOurAddress());body=library;
            const auto* stored=hook->m_LibOriginalFunctionContent;
            if(stored[0]==0xE9) {int32 delta{};std::memcpy(&delta,stored+1,4);body=Resolve(library+5+delta);}
            for(auto at:{entry,library,body})bytes.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
        } else {body=Resolve(entry);for(auto at:{entry,body})bytes.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});}
    }
    bool Check() const {
        bool ok=module!=0;
        for(auto& [at,saved]:bytes)ok&=saved==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
        if(!hook)return ok&&ModuleOf(Resolve(entry))==uintptr_t(GetModuleHandleA(nullptr));
        ok&=hook->Hooked()==enabled&&hook->Locked()==locked&&hook->Reversed()==reversed;
        // A wrapper not reconstructed must never run through its DLL wrapper (recursive original call).
        ok&=reversed||!enabled;
        const auto* expectedGame=enabled?reinterpret_cast<const uint8*>(&hook->m_HookContent):hook->m_OriginalFunctionContent;
        const auto* expectedLib=enabled?hook->m_LibOriginalFunctionContent:reinterpret_cast<const uint8*>(&hook->m_LibHookContent);
        ok&=std::memcmp(reinterpret_cast<void*>(entry),expectedGame,hook->m_iHookedBytes)==0
            &&std::memcmp(reinterpret_cast<void*>(library),expectedLib,hook->m_iLibHookedBytes)==0;
        if(enabled)ok&=Resolve(entry)==body&&ModuleOf(body)==module&&ModuleOf(library)==module;
        else ok&=ModuleOf(Resolve(entry))==uintptr_t(GetModuleHandleA(nullptr))&&Resolve(library)==Resolve(entry);
        return ok;
    }
    json Metadata() const {return {{"entry",entry},{"registered",hook!=nullptr},{"enabled",enabled},{"locked",locked},
        {"reconstructed",reversed},{"library",library},{"body",body},{"asi_module",module},{"route_ok",Check()}};}
};
}
