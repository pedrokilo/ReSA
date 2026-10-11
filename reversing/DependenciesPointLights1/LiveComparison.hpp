#pragma once
// Banco independiente: entrada original intacta vs parche temporal hacia el cuerpo DLL.
// No instala RH ni necesita fila en docs/hooks.csv. Todos los estados se restauran antes de Write.
#include <fstream>
#include <float.h>
#include "PointLights.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"

namespace DependenciesPointLights1Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn, 1> fns{{
    {"CPointLights","AddLight",0x7000E0,0,-1,2560},
}};
inline uintptr_t Body(uintptr_t at) {
    for (unsigned n = 0; n != 16 && *reinterpret_cast<uint8*>(at) == 0xE9; ++n)
        at += 5 + *reinterpret_cast<int32*>(at + 1);
    return at;
}
// Guardas de lista, contador, NextCachedValue y relleno; cámara: placement + matrix pointer.
inline const std::array<std::pair<uintptr_t, size_t>, 2> globals{{
    {0xC3F0C0, 0x20 + 32 * 0x30 + 16},
    {0xB6F02C, 20},
}};
alignas(16) inline std::array<uint8, 0x80> matrixBytes{};
alignas(16) inline std::array<uint8, 32> entityToken{};
inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "dependenciespointlights1-runtime.start")) return;
    if (!log.is_open()) {
        log.open(dir / "dependenciespointlights1-runtime.jsonl");
        ReversingProbe::InstallFailureCapture("dependenciespointlights1");
        Write({{"event", "waiting_for_player"}});
    }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done = true;
    constexpr uintptr_t entry = 0x7000E0;
    const uintptr_t symbol = reinterpret_cast<uintptr_t>(&CPointLights::AddLight);
    OriginalDependencies499::HookRoute route("CPointLights","AddLight",entry,&CPointLights::AddLight);
    const uintptr_t body = route.Body(); // Antes de desactivar el hook y crear parches.
    const auto entryBefore = ReversingProbe::Bytes(reinterpret_cast<void*>(entry), 16);
    const auto symbolBefore = ReversingProbe::Bytes(reinterpret_cast<void*>(symbol), 16);
    const auto bodyBefore = ReversingProbe::Bytes(reinterpret_cast<void*>(body), 16);
    if (entryBefore[0] != 0x8B || entryBefore[1] != 0x0D || body < 0x1000000) {
        Write({{"event", "error"}, {"reason", "Entrada original modificada o cuerpo DLL fuera de dominio"},
               {"entry", entryBefore}, {"symbol", symbol}, {"body", body}});
        return;
    }
    std::array<std::vector<uint8>, 2> saved;
    for (size_t j = 0; j < globals.size(); ++j)
        saved[j] = ReversingProbe::Bytes(reinterpret_cast<void*>(globals[j].first), globals[j].second);
    const unsigned savedFP = _controlfp(0, 0);
    auto restore = [&]() {
        for (size_t j = 0; j < globals.size(); ++j)
            std::memcpy(reinterpret_cast<void*>(globals[j].first), saved[j].data(), saved[j].size());
        _controlfp(savedFP, _MCW_PC | _MCW_RC);
    };
    auto restored = [&]() {
        bool ok = _controlfp(0, 0) == savedFP;
        for (size_t j = 0; j < globals.size(); ++j)
            ok &= saved[j] == ReversingProbe::Bytes(reinterpret_cast<void*>(globals[j].first), globals[j].second);
        return ok && entryBefore == ReversingProbe::Bytes(reinterpret_cast<void*>(entry), 16)
            && symbolBefore == ReversingProbe::Bytes(reinterpret_cast<void*>(symbol), 16)
            && bodyBefore == ReversingProbe::Bytes(reinterpret_cast<void*>(body), 16);
    };
    Write({{"event", "begin"}, {"function", "CPointLights::AddLight"}, {"cases", 2560},
           {"original_entry", entry}, {"candidate_body", body}, {"registered_hook",route.Registered()}});
    int failures = 0;
    bool allRestored = true;
    const std::array<uint32, 5> counts{0, 1, 31, 32, 33};
    const std::array<float, 4> radii{0.0f, 1.0f, 15.0f, 100.0f};
    for (unsigned pair = 0; pair < 2560; ++pair) {
        const unsigned placement = pair % 2, countIndex = (pair / 2) % 5;
        const unsigned positionIndex = (pair / 10) % 16, radiusIndex = (pair / 160) % 4, variant = pair / 640;
        const float radius = radii[radiusIndex], limit = radius + 15.0f;
        const CVector camera{variant == 0 ? 0.0f : 123.5f, variant == 0 ? 0.0f : -78.25f, 10.0f};
        const float below = std::nextafter(limit, 0.0f), above = std::nextafter(limit, INFINITY);
        const float fade = limit * 0.75f;
        const std::array<CVector, 16> offsets{{
            {0,0,0}, {limit,0,0}, {-limit,0,0}, {0,limit,0}, {0,-limit,0},
            {0,0,limit}, {0,0,-limit}, {below,0,0}, {above,0,0},
            {fade,0,0}, {std::nextafter(fade,0.0f),0,0}, {std::nextafter(fade,INFINITY),0,0},
            {limit*0.5f,limit*0.5f,0}, {limit*0.5f,limit*0.5f,limit*0.5f},
            {std::bit_cast<float>(0x7FC00000u),0,0}, {0,0,std::bit_cast<float>(0x7FC00000u)},
        }};
        const CVector point{camera.x + offsets[positionIndex].x, camera.y + offsets[positionIndex].y,
                            camera.z + offsets[positionIndex].z};
        const CVector direction{float(variant)-1.5f, -0.0f, float(radiusIndex)*0.125f};
        const float red = variant == 0 ? 0.0f : variant == 1 ? 1.0f : variant == 2 ? -0.5f : 2.0f;
        const float green = variant == 3 ? std::bit_cast<float>(0x7FC01234u) : 0.375f;
        const float blue = variant == 0 ? -0.0f : 0.875f;
        const uint8 kind = uint8((positionIndex + variant) % 6), fog = uint8(variant);
        CEntity* token = variant & 1 ? reinterpret_cast<CEntity*>(entityToken.data()) : nullptr;
        auto run = [&](bool candidate) {
            restore();
            auto* region = reinterpret_cast<uint8*>(globals[0].first);
            for (size_t k = 0; k < globals[0].second; ++k) region[k] = uint8(k * 17 + pair * 13);
            CPointLights::NumLights = counts[countIndex];
            matrixBytes.fill(0xA7);
            std::memcpy(matrixBytes.data() + 0x30, &camera, sizeof(camera));
            std::memcpy(reinterpret_cast<void*>(0xB6F02C), &camera, sizeof(camera));
            *reinterpret_cast<uintptr_t*>(0xB6F03C) = placement ? uintptr_t(matrixBytes.data()) : 0;
            const auto cameraBefore = ReversingProbe::Bytes(reinterpret_cast<void*>(0xB6F02C), 20);
            const auto matrixBefore = matrixBytes;
            _controlfp(_PC_24 | _RC_NEAR, _MCW_PC | _MCW_RC);
            bool routeOK = false;
            route.Invoke(candidate,[&] {
                routeOK = ReversingProbe::JumpsToDll(entry) == candidate;
                using Call = void(__cdecl*)(uint8,CVector,CVector,float,float,float,float,uint8,bool,CEntity*);
                reinterpret_cast<Call>(entry)(kind, point, direction, radius, red, green, blue, fog, (variant&2)!=0, token);
                return true;
            });
            json out{{"route_ok", routeOK}, {"num_lights", CPointLights::NumLights},
                     {"list_and_guards", ReversingProbe::Bytes(region, globals[0].second)},
                     {"camera_unchanged", cameraBefore == ReversingProbe::Bytes(reinterpret_cast<void*>(0xB6F02C),20)},
                     {"matrix_unchanged", matrixBefore == matrixBytes},
                     {"entry_restored", entryBefore == ReversingProbe::Bytes(reinterpret_cast<void*>(entry),16)}};
            restore();
            return out;
        };
        const auto original = run(false), candidate = run(true);
        const bool stateOK = restored();
        allRestored &= stateOK;
        const bool equal = original == candidate;
        const bool accepted = equal && stateOK && original["route_ok"] == true
            && original["camera_unchanged"] == true && original["matrix_unchanged"] == true
            && original["entry_restored"] == true;
        failures += !accepted;
        Write({{"event","comparison"},{"function","CPointLights::AddLight"},{"pair",pair},
               {"case",pair},
               {"equal",equal},{"accepted",accepted},{"state_restored",stateOK},
               {"original",original},{"candidate",candidate}});
    }
    restore();
    allRestored &= restored();
    allRestored &= route.Restore();
    Write({{"event","function_summary"},{"function","CPointLights::AddLight"},{"cases",2560},{"failures",failures}});
    Write({{"event",failures == 0 && allRestored ? "complete" : "error"},
           {"pairs",2560},{"state_restored",allRestored}});
}
}
