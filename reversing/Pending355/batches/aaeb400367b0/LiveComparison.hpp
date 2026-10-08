#pragma once
#include "../../../CameraProcess355/LiveComparison.hpp"
#include "../../../CameraRwCopy355/LiveComparison.hpp"
#include "../../../CamFirstPerson355/LiveComparison.hpp"
#include "../../../CamFlyBy355/LiveComparison.hpp"
#include "../../../CamFollowCar355/LiveComparison.hpp"
#include "../../../CamFollowPed355/LiveComparison.hpp"
#include "../../../CamHeliChase355/LiveComparison.hpp"
#include "../../../CamLamp355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    CameraProcess355Live::Update();
    CameraRwCopy355Live::Update();
    CamFirstPerson355Live::Update();
    CamFlyBy355Live::Update();
    CamFollowCar355Live::Update();
    CamFollowPed355Live::Update();
    CamHeliChase355Live::Update();
    CamLamp355Live::Update();
} }
