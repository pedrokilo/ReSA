#pragma once
#include "../../../CamProcess355/LiveComparison.hpp"
#include "../../../CamM16355/LiveComparison.hpp"
#include "../../../CamTwoCars355/LiveComparison.hpp"
#include "../../../CamShooting355/LiveComparison.hpp"
#include "../../../CamMouse355/LiveComparison.hpp"
#include "../../../CamAim355/LiveComparison.hpp"
#include "../../../CamFollowPed355/LiveComparison.hpp"
#include "../../../CamFollowCar355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    CamProcess355Live::Update();
    CamM16355Live::Update();
    CamTwoCars355Live::Update();
    CamShooting355Live::Update();
    CamMouse355Live::Update();
    CamAim355Live::Update();
    CamFollowPed355Live::Update();
    CamFollowCar355Live::Update();
} }
