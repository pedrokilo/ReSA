#pragma once
#include "../../../BikeBlow355/LiveComparison.hpp"
#include "../../../BikeBurst355/LiveComparison.hpp"
#include "../../../BikeCore355/LiveComparison.hpp"
#include "../../../BikeDamage355/LiveComparison.hpp"
#include "../../../BikeHands355/LiveComparison.hpp"
#include "../../../BikeInputs355/LiveComparison.hpp"
#include "../../../BikeRatios355/LiveComparison.hpp"
#include "../../../BikeRoad355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    BikeBlow355Live::Update();
    BikeBurst355Live::Update();
    BikeCore355Live::Update();
    BikeDamage355Live::Update();
    BikeHands355Live::Update();
    BikeInputs355Live::Update();
    BikeRatios355Live::Update();
    BikeRoad355Live::Update();
} }
