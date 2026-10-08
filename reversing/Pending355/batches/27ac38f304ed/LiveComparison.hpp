#pragma once
#include "../../../WaterFog355/LiveComparison.hpp"
#include "../../../FormationAssign355/LiveComparison.hpp"
#include "../../../FormationGather355/LiveComparison.hpp"
#include "../../../FormationFollow355/LiveComparison.hpp"
#include "../../../BikeControl355/LiveComparison.hpp"
#include "../../../BikeAI355/LiveComparison.hpp"
#include "../../../BikeRider355/LiveComparison.hpp"
#include "../../../BikePreRender355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    WaterFog355Live::Update();
    FormationAssign355Live::Update();
    FormationGather355Live::Update();
    FormationFollow355Live::Update();
    BikeControl355Live::Update();
    BikeAI355Live::Update();
    BikeRider355Live::Update();
    BikePreRender355Live::Update();
} }
