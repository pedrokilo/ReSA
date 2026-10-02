#pragma once
// Call counter injected directly into CEntryExit::TransitionFinished. Only
// incremented when the *candidate* C++ body actually executes; calling the
// real original machine code at the same address (hook disabled) never
// touches this counter, mirroring reversing/CTaskSimpleUseGun/Probe.hpp.
namespace EnexProbe {
inline unsigned calls{};
inline bool recording{};
inline json events = json::array();
inline void Record(const char* name) {
    if (recording) events.push_back(name);
}
}
