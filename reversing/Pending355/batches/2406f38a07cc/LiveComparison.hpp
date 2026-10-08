#pragma once
#include "../../../GangFollower355/LiveComparison.hpp"
#include "../../../GangJoin355/LiveComparison.hpp"
#include "../../../GeometryCore355/LiveComparison.hpp"
#include "../../../GeometryQueries355/LiveComparison.hpp"
#include "../../../GeometryRoutes355/LiveComparison.hpp"
#include "../../../HandSignal355/LiveComparison.hpp"
#include "../../../HassleVehicle355/LiveComparison.hpp"
#include "../../../HitCar355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    GangFollower355Live::Update();
    GangJoin355Live::Update();
    GeometryCore355Live::Update();
    GeometryQueries355Live::Update();
    GeometryRoutes355Live::Update();
    HandSignal355Live::Update();
    HassleVehicle355Live::Update();
    HitCar355Live::Update();
} }
