#pragma once
#include "../../../BikePreRender355/LiveComparison.hpp"
#include "../../../BikeRatios355/LiveComparison.hpp"
#include "../../../BikeRider355/LiveComparison.hpp"
#include "../../../BikeRoad355/LiveComparison.hpp"
#include "../../../BikeSetup355/LiveComparison.hpp"
#include "../../../BikeWheel355/LiveComparison.hpp"
#include "../../../Boat355/LiveComparison.hpp"
#include "../../../CamAim355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    BikePreRender355Live::Update();
    BikeRatios355Live::Update();
    BikeRider355Live::Update();
    BikeRoad355Live::Update();
    BikeSetup355Live::Update();
    BikeWheel355Live::Update();
    Boat355Live::Update();
    CamAim355Live::Update();
} }
