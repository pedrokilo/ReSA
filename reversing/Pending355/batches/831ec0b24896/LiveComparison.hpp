#pragma once
#include "../../../CamFollowPed355/LiveComparison.hpp"
#include "../../../CamFollowCar355/LiveComparison.hpp"
#include "../../../CamBirdy355/LiveComparison.hpp"
#include "../../../CamHeliChase355/LiveComparison.hpp"
#include "../../../CamWheel355/LiveComparison.hpp"
#include "../../../CamLookBehind355/LiveComparison.hpp"
#include "../../../CamLookSide355/LiveComparison.hpp"
#include "../../../CamArrest355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    CamFollowPed355Live::Update();
    CamFollowCar355Live::Update();
    CamBirdy355Live::Update();
    CamHeliChase355Live::Update();
    CamWheel355Live::Update();
    CamLookBehind355Live::Update();
    CamLookSide355Live::Update();
    CamArrest355Live::Update();
} }
