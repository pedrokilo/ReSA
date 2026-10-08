#pragma once
#include "../../../CameraProcess355/LiveComparison.hpp"
#include "../../../CamFollowCar355/LiveComparison.hpp"
#include "../../../WeaponOccupant355/LiveComparison.hpp"
#include "../../../WeaponInstant355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    CameraProcess355Live::Update();
    CamFollowCar355Live::Update();
    WeaponOccupant355Live::Update();
    WeaponInstant355Live::Update();
} }
