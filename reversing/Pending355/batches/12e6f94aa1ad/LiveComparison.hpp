#pragma once
#include "../../../Wander355/LiveComparison.hpp"
#include "../../../Sunbathe355/LiveComparison.hpp"
#include "../../../HandSignal355/LiveComparison.hpp"
#include "../../../VehicleLean355/LiveComparison.hpp"
#include "../../../FormationFollow355/LiveComparison.hpp"
#include "../../../HassleVehicle355/LiveComparison.hpp"
#include "../../../GangFollower355/LiveComparison.hpp"
#include "../../../SmartFlee355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    Wander355Live::Update();
    Sunbathe355Live::Update();
    HandSignal355Live::Update();
    VehicleLean355Live::Update();
    FormationFollow355Live::Update();
    HassleVehicle355Live::Update();
    GangFollower355Live::Update();
    SmartFlee355Live::Update();
} }
