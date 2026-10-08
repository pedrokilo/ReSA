#pragma once
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "../ProbeSupport.hpp"
namespace SmallPendingSupport {
struct HookSnapshot {
    ReversibleHooks::ReversibleHook::Base* hook;
    bool enabled, locked;
};
inline auto SnapshotHooks() {
    std::vector<HookSnapshot> result;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category) {
        for (auto& item : category.Items()) result.push_back({item.get(), item->Hooked(), item->Locked()});
    });
    return result;
}
inline bool HooksUnchanged(const std::vector<HookSnapshot>& saved) {
    return std::all_of(saved.begin(), saved.end(), [](const auto& item) { return item.hook->Hooked() == item.enabled && item.hook->Locked() == item.locked; });
}
inline bool ExactSwitch(ReversibleHooks::ReversibleHook::Simple& hook, bool candidate) {
    const auto* gta = candidate ? reinterpret_cast<const uint8*>(&hook.m_HookContent) : hook.m_OriginalFunctionContent;
    const auto* ours = candidate ? hook.m_LibOriginalFunctionContent : reinterpret_cast<const uint8*>(&hook.m_LibHookContent);
    return hook.Hooked() == candidate && std::memcmp(hook.GetHookGTAAddress(), gta, hook.m_iHookedBytes) == 0 && std::memcmp(hook.GetHookOurAddress(), ours, hook.m_iLibHookedBytes) == 0;
}
}
