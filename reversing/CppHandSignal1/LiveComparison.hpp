#pragma once
#include <fstream>
#include "TaskComplexPlayHandSignalAnim.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace CppHandSignal1Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }
struct Result { int32 value; uint32 edx; bool stack, nonvolatile; };
__declspec(noinline) inline Result Call(uintptr_t entry, CPed* ped, uint32 seed) {
    uint32 before{}, after{}, b{}, s{}, d{}, x{}; int32 returned{};
    __asm {
        push ebx
        push esi
        push edi
        mov before, esp
        mov ebx, 13579BDFh
        mov esi, 2468ACE0h
        mov edi, 10293847h
        mov edx, seed
        push ped
        call entry
        mov returned, eax
        mov x, edx
        mov after, esp
        mov b, ebx
        mov s, esi
        mov d, edi
        pop edi
        pop esi
        pop ebx
    }
    return {returned, x, before == after, b == 0x13579BDF && s == 0x2468ACE0 && d == 0x10293847};
}
inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "cpphandsignal1-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "cpphandsignal1-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("cpphandsignal1"); }
    auto* ped = FindPlayerPed(); if (!ped || CCutsceneMgr::IsRunning()) return;
    done = true;
    OriginalDependencies499::HookRoute route("CTaskComplexPlayHandSignalAnim", "GetAnimIdForPed", 0x61B460, &CTaskComplexPlayHandSignalAnim::GetAnimIdForPed);
    const auto saved = ReversingProbe::Bytes(ped, sizeof(CPed));
    const auto typeSaved = ped->m_nPedType;
    Write({{"event", "begin"}, {"registered_hook", route.Registered()}});
    unsigned failures = 0;
    for (unsigned i = 0; i < 2048; ++i) {
        const unsigned type = i % 32;
        const int32 expected = type == 7 ? 320 : type == 8 ? 321 : type == 9 ? 322 : type == 11 ? 324 : type == 14 ? 323 : -1;
        const uint32 seed = i * 2654435761u;
        auto run = [&](bool candidate) {
            ped->m_nPedType = static_cast<ePedType>(type);
            const auto before = ReversingProbe::Bytes(ped, sizeof(CPed));
            Result result{}; bool routed{};
            route.Invoke(candidate, [&] { routed = ReversingProbe::JumpsToDll(0x61B460) == candidate; result = Call(0x61B460, ped, seed); return true; });
            const bool unchanged = before == ReversingProbe::Bytes(ped, sizeof(CPed));
            ped->m_nPedType = typeSaved;
            return json{{"return", result.value}, {"edx", result.edx}, {"edx_preserved", result.edx == seed}, {"stack_restored", result.stack}, {"nonvolatile_preserved", result.nonvolatile}, {"ped_unchanged", unchanged}, {"contract_ok", result.value == expected}, {"route_ok", routed}};
        };
        const auto original = run(false), candidate = run(true);
        bool equal = original == candidate;
        for (const auto* gate : {"edx_preserved", "stack_restored", "nonvolatile_preserved", "ped_unchanged", "contract_ok", "route_ok"}) equal &= original[gate] == true && candidate[gate] == true;
        failures += !equal;
        Write({{"event", "comparison"}, {"function", "CTaskComplexPlayHandSignalAnim::GetAnimIdForPed"}, {"pair", i}, {"case", i}, {"ped_type", type}, {"original", original}, {"candidate", candidate}, {"equal", equal}, {"accepted", equal}});
    }
    ped->m_nPedType = typeSaved;
    const bool unchanged = saved == ReversingProbe::Bytes(ped, sizeof(CPed));
    const bool restored = route.Restore() && unchanged;
    Write({{"event", "function_summary"}, {"function", "CTaskComplexPlayHandSignalAnim::GetAnimIdForPed"}, {"cases", 2048}, {"failures", failures}});
    Write({{"event", !failures && restored ? "complete" : "error"}, {"pairs", 2048}, {"state_restored", restored}});
}
}
