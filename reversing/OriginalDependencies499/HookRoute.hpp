#pragma once
#include <bit>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "../ProbeSupport.hpp"

// Banco antes y después de registrar: siempre invoca la misma entrada original.
// El hook RH, si existe, queda apagado entre llamadas y se restaura al finalizar.
namespace OriginalDependencies499 {
class HookRoute {
    ReversibleHooks::ReversibleHook::Base* m_hook{};
    uintptr_t m_entry{}, m_body{};
    bool m_enabled{}, m_locked{}, m_restored{};
    std::vector<uint8> m_initial;
    void Attach(const char* cls, const char* fn, uintptr_t entry) {
        m_entry = entry;
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
    }
    void Finish() {
        for (int i=0; m_body && i<16 && *reinterpret_cast<uint8*>(m_body)==0xE9; ++i)
            m_body += 5 + *reinterpret_cast<int32*>(m_body+1);
        if (m_hook) m_hook->State(false);
    }
public:
    // Marca explicita: el candidato es el destino vivo del hook registrado (hook "Simple"), p. ej. una envoltura Exe con enlace interno.
    struct RegisteredTarget {};
    template<class F>
    HookRoute(const char* cls, const char* fn, uintptr_t entry, F candidate) {
        Attach(cls, fn, entry);
        m_body = std::bit_cast<uintptr_t>(candidate);   // puntero a funcion o a miembro no virtual (4 bytes en x86)
        Finish();
    }
    HookRoute(const char* cls, const char* fn, uintptr_t entry, RegisteredTarget) {
        Attach(cls, fn, entry);
        if (m_hook && m_hook->Symbol()[0] == 'S')
            m_body = reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(m_hook)->GetHookOurAddress());
        Finish();   // si no hay hook Simple, m_body queda en 0: Registered()/ModuleOf lo rechazan antes de ejecutar
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
