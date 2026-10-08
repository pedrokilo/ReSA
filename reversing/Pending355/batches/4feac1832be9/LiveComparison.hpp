#pragma once
#include "../../../WaterFog355/LiveComparison.hpp"
#include "../../../WaterRender355/LiveComparison.hpp"
#include "../../../FormationLookup355/LiveComparison.hpp"
#include "../../../FormationAssign355/LiveComparison.hpp"
#include "../../../FormationGather355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    WaterFog355Live::Update();
    WaterRender355Live::Update();
    FormationLookup355Live::Update();
    FormationAssign355Live::Update();
    FormationGather355Live::Update();
} }
