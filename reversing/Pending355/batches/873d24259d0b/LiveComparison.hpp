#pragma once
#include "../../../CameraProcess355/LiveComparison.hpp"
#include "../../../WeaponOccupant355/LiveComparison.hpp"
#include "../../../WeaponInstant355/LiveComparison.hpp"
#include "../../../CamMouse355/LiveComparison.hpp"
#include "../../../CamAim355/LiveComparison.hpp"
#include "../../../CamFollowPed355/LiveComparison.hpp"
#include "../../../BikeSetup355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    CameraProcess355Live::Update();
    WeaponOccupant355Live::Update();
    WeaponInstant355Live::Update();
    CamMouse355Live::Update();
    CamAim355Live::Update();
    CamFollowPed355Live::Update();
    BikeSetup355Live::Update();
} }
