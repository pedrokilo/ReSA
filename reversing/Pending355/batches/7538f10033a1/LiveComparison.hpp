#pragma once
#include "../../../GeometryRoutes355/LiveComparison.hpp"
#include "../../../BikeRider355/LiveComparison.hpp"
#include "../../../BikePreRender355/LiveComparison.hpp"
#include "../../../TrainHit355/LiveComparison.hpp"
#include "../../../TrainPosition355/LiveComparison.hpp"
#include "../../../TrainGeneration355/LiveComparison.hpp"
#include "../../../TrainCreate355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    GeometryRoutes355Live::Update();
    BikeRider355Live::Update();
    BikePreRender355Live::Update();
    TrainHit355Live::Update();
    TrainPosition355Live::Update();
    TrainGeneration355Live::Update();
    TrainCreate355Live::Update();
} }
