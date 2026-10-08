#pragma once
#include "../../../InteriorBase/LiveComparison.hpp"
#include "../../../InteriorCommerce/LiveComparison.hpp"
#include "../../../InteriorHome/LiveComparison.hpp"
#include "../../../InteriorPlacement/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    InteriorBaseLive::Update();
    InteriorCommerceLive::Update();
    InteriorHomeLive::Update();
    InteriorPlacementLive::Update();
} }
