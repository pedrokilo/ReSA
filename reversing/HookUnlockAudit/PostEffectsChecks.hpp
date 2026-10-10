#pragma once
#include "PostEffects.h"

namespace HookUnlockAuditLive {
inline nlohmann::json CallPostEffectsStore(uintptr_t target) {
    uint32_t before{}, after{}, returnedEax{};
    __asm {
        mov before, esp
        mov eax, 13579BDFh
        call target
        mov returnedEax, eax
        mov after, esp
    }
    std::array<uint32_t, 10> result{};
    memcpy(result.data(), reinterpret_cast<void*>(0xC40280), sizeof(result));
    return nlohmann::json{{"states", result}, {"esp_delta", int64_t(after) - before}, {"eax", returnedEax}};
}
// Ejecutar en el hilo principal del banco, con un dispositivo RW vivo.
inline nlohmann::json RunPostEffectsChecks(ReversibleHooks::ReversibleHook::Base* hook) {
    using json = nlohmann::json;
    constexpr std::array<RwRenderState, 10> states{
        rwRENDERSTATESRCBLEND, rwRENDERSTATEDESTBLEND, rwRENDERSTATEFOGENABLE,
        rwRENDERSTATECULLMODE, rwRENDERSTATEZTESTENABLE, rwRENDERSTATEZWRITEENABLE,
        rwRENDERSTATESHADEMODE, rwRENDERSTATEVERTEXALPHAENABLE,
        rwRENDERSTATETEXTUREADDRESS, rwRENDERSTATETEXTUREFILTER
    };
    if (!hook || !*reinterpret_cast<uintptr_t*>(0xC97B24)) {
        return json{{"event", "posteffects_unavailable"}, {"passed", false}};
    }
    const bool savedEnabled = hook->Hooked(), savedLocked = hook->Locked();
    std::array<uint8_t, sizeof(CPostEffects::imf)> savedImf{};
    memcpy(savedImf.data(), &CPostEffects::ms_imf, savedImf.size());
    std::array<uint32_t, 10> liveStates{};
    bool queried = true;
    for (size_t i = 0; i < states.size(); ++i)
        queried &= RwRenderStateGet(states[i], &liveStates[i]) != 0;
    if (!queried) return json{{"event", "posteffects_unavailable"}, {"passed", false}, {"reason", "driver_get_failed"}};

    const uintptr_t dll = reinterpret_cast<uintptr_t>(&CPostEffects::ImmediateModeRenderStatesStore);
    hook->LockState(false);
    json comparisons = json::array();
    bool passed = true;
    // Los dos sentidos y una vuelta al estado original. No presupone preservación de EAX.
    for (int i = 0; i < 6; ++i) {
        const bool candidate = (i % 3) != 1;
        hook->State(candidate);
        FlushInstructionCache(GetCurrentProcess(),nullptr,0);
        memcpy(&CPostEffects::ms_imf, savedImf.data(), savedImf.size());
        const auto actual = CallPostEffectsStore(i < 3 ? uintptr_t(0x700CC0) : dll);
        const bool route = hook->Hooked()==candidate&&ReversingProbe::JumpsToDll(0x700CC0)==candidate;
        const bool equal = actual["states"] == json(liveStates) && actual["esp_delta"] == 0 && route;
        passed &= equal;
        comparisons.push_back({{"candidate_enabled", candidate}, {"entry", i < 3 ? "exe" : "dll"}, {"original", json{{"states", liveStates}, {"esp_delta", 0}}}, {"candidate", actual}, {"route_ok",route}, {"passed", equal}});
    }
    hook->State(savedEnabled);
    hook->LockState(savedLocked);
    memcpy(&CPostEffects::ms_imf, savedImf.data(), savedImf.size());
    bool restored = memcmp(&CPostEffects::ms_imf, savedImf.data(), savedImf.size()) == 0 && hook->Hooked() == savedEnabled && hook->Locked() == savedLocked;
    std::array<uint32_t, 10> afterStates{};
    for (size_t i = 0; i < states.size(); ++i) {
        restored &= RwRenderStateGet(states[i], &afterStates[i]) != 0;
        restored &= afterStates[i] == liveStates[i];
    }
    return json{{"event", "posteffects_live_checks"}, {"comparisons", comparisons}, {"passed", passed && restored}, {"state_restored", restored}, {"eax_contract", "caller_saved_observed_only"}};
}
}
