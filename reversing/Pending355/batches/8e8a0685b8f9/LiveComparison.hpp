#pragma once
#include "../../../CamBirdy355/LiveComparison.hpp"
#include "../../../ScriptLocateChar355/LiveComparison.hpp"
#include "../../../CameraProcess355/LiveComparison.hpp"
#include "../../../WeaponOccupant355/LiveComparison.hpp"
#include "../../../WeaponInstant355/LiveComparison.hpp"
#include "../../../ProjectileUpdate355/LiveComparison.hpp"
#include "../../../CamProcess355/LiveComparison.hpp"
#include "../../../CamM16355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    CamBirdy355Live::Update();
    ScriptLocateChar355Live::Update();
    CameraProcess355Live::Update();
    WeaponOccupant355Live::Update();
    WeaponInstant355Live::Update();
    ProjectileUpdate355Live::Update();
    CamProcess355Live::Update();
    CamM16355Live::Update();
} }
