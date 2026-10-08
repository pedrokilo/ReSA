#pragma once
#include "../../../InteriorBase/LiveComparison.hpp"
#include "../../../InteriorCommerce/LiveComparison.hpp"
#include "../../../InteriorHome/LiveComparison.hpp"
#include "../../../InteriorSpatial/LiveComparison.hpp"
#include "../../../InteriorLifecycleCore/LiveComparison.hpp"
#include "../../../InteriorLifecycleRuntime/LiveComparison.hpp"
#include "../../../InteriorPlacement/LiveComparison.hpp"
#include "../../../InteriorAbi/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    InteriorBaseLive::Update();
    InteriorCommerceLive::Update();
    InteriorHomeLive::Update();
    InteriorSpatialLive::Update();
    InteriorLifecycleCoreLive::Update();
    InteriorLifecycleRuntimeLive::Update();
    InteriorPlacementLive::Update();
    InteriorAbiLive::Update();
} }
