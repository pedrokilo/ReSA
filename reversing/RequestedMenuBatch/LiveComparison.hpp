#pragma once
#include "../RequestedParallelSmall/LiveComparison.hpp"
#include "../RequestedParallelSmall/InterestingVisibility/LiveComparison.hpp"
#include "../RequestedParallelSmall/FormationCover/LiveComparison.hpp"
#include "../RequestedParallelSmall/FormationCoverExpanded/LiveComparison.hpp"
#include "../RequestedParallelReplay/WanderType/LiveComparison.hpp"
namespace RequestedMenuLive {
inline void Update() {
    RequestedParallelSmallLive::Update();
    InterestingVisibilityPendingLive::Update();
    FormationCoverPendingLive::Update();
    FormationCoverExpandedLive::Update();
    RequestedWanderTypeLive::Update();
}
}
