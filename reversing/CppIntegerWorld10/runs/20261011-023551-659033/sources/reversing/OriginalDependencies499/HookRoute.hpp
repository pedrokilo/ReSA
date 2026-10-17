#pragma once
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "../ProbeSupport.hpp"

// Banco antes y después de registrar: siempre invoca la misma entrada original.
// El hook RH, si existe, queda apagado entre llamadas y se restaura al finalizar.
namespace OriginalDependencies499 {
class HookRoute {
    ReversibleHooks::ReversibleHook::Base* m_hook{};
    uintptr_t m_entry{}, m_body{};
    bool m_enabled{}, m_locked{}, m_restored{};
    std::vector<uint8> m_initial;
public:
    template<class F>
    HookRoute(const char* cls, const char* fn, uintptr_t entry, F candidate) : m_entry(entry) {
        m_initial = ReversingProbe::Bytes(reinterpret_cast<void*>(entry), 5);
        ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category) {
            if (category.Name() == cls)
                for (auto& hook : category.Items())
                    if (hook->Name() == fn) m_hook = hook.get();
        });
        if (m_hook) {
            m_enabled = m_hook->Hooked(); m_locked = m_hook->Locked();
            m_hook->LockState(false); m_hook->State(true);
        }
        m_body = reinterpret_cast<uintptr_t>(candidate);
        for (int i=0; i<16 && *reinterpret_cast<uint8*>(m_body)==0xE9; ++i)
            m_body += 5 + *reinterpret_cast<int32*>(m_body+1);
        if (m_hook) m_hook->State(false);
    }
    uintptr_t Body() const { return m_body; }
    bool Registered() const { return m_hook != nullptr; }
    template<class F> auto Invoke(bool candidate, F call) {
        if (m_hook) {
            m_hook->State(candidate);
            const auto value = call();
            m_hook->State(false);
            return value;
        }
        if (candidate) {
            ReversingProbe::EntryPatch patch(m_entry, reinterpret_cast<void*>(m_body));
            return call();
        }
        return call();
    }
    bool Restore() {
        if (!m_restored && m_hook) {
            m_hook->State(m_enabled); m_hook->LockState(m_locked);
        }
        m_restored = true;
        return m_initial == ReversingProbe::Bytes(reinterpret_cast<void*>(m_entry), 5)
            && (!m_hook || (m_hook->Hooked()==m_enabled && m_hook->Locked()==m_locked));
    }
    ~HookRoute() { Restore(); }
};
}
