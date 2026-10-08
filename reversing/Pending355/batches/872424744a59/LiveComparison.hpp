#pragma once
#include "../../../BikeBurst355/LiveComparison.hpp"
#include "../../../BikeCore355/LiveComparison.hpp"
#include "../../../BikeHands355/LiveComparison.hpp"
#include "../../../BikeSetup355/LiveComparison.hpp"
#include "../../../BikeWheel355/LiveComparison.hpp"
#include "../../../BikePreRender355/LiveComparison.hpp"
#include "../../../PlaneBlow355/LiveComparison.hpp"
#include "../../../CamHeliChase355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    BikeBurst355Live::Update();
    BikeCore355Live::Update();
    BikeHands355Live::Update();
    BikeSetup355Live::Update();
    BikeWheel355Live::Update();
    BikePreRender355Live::Update();
    PlaneBlow355Live::Update();
    CamHeliChase355Live::Update();
} }
