#pragma once
#include "../../../CamShooting355/LiveComparison.hpp"
#include "../../../CamMouse355/LiveComparison.hpp"
#include "../../../CamAim355/LiveComparison.hpp"
#include "../../../CamFollowPed355/LiveComparison.hpp"
#include "../../../CamFollowCar355/LiveComparison.hpp"
#include "../../../BikeSetup355/LiveComparison.hpp"
#include "../../../BikePreRender355/LiveComparison.hpp"
#include "../../../CamHeliChase355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    CamShooting355Live::Update();
    CamMouse355Live::Update();
    CamAim355Live::Update();
    CamFollowPed355Live::Update();
    CamFollowCar355Live::Update();
    BikeSetup355Live::Update();
    BikePreRender355Live::Update();
    CamHeliChase355Live::Update();
} }
