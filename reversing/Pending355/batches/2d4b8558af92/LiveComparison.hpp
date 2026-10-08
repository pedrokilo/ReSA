#pragma once
#include "../../../InteriorSpatial/LiveComparison.hpp"
#include "../../../InteriorLifecycleCore/LiveComparison.hpp"
#include "../../../InteriorPlacement/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    InteriorSpatialLive::Update();
    InteriorLifecycleCoreLive::Update();
    InteriorPlacementLive::Update();
} }
