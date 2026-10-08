#pragma once
#include "../../../InteriorPlacement/LiveComparison.hpp"
#include "../../../InteriorSpatial/LiveComparison.hpp"
#include "../../../InteriorLifecycleCore/LiveComparison.hpp"
#include "../../../InteriorLifecycleRuntime/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    InteriorPlacementLive::Update();
    InteriorSpatialLive::Update();
    InteriorLifecycleCoreLive::Update();
    InteriorLifecycleRuntimeLive::Update();
} }
