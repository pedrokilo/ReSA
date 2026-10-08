#pragma once
#include "../../../CamFollowPed355/LiveComparison.hpp"
#include "../../../WeaponInstant355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    CamFollowPed355Live::Update();
    WeaponInstant355Live::Update();
} }
