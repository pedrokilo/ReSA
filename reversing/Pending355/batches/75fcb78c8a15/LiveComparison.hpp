#pragma once
#include "../../../BikeSetup355/LiveComparison.hpp"
#include "../../../BikeWheel355/LiveComparison.hpp"
#include "../../../BikePreRender355/LiveComparison.hpp"
#include "../../../TrainPosition355/LiveComparison.hpp"
#include "../../../PlaneAmbient355/LiveComparison.hpp"
#include "../../../PlaneBlow355/LiveComparison.hpp"
#include "../../../PlaneControl355/LiveComparison.hpp"
#include "../../../PlaneCreation355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    BikeSetup355Live::Update();
    BikeWheel355Live::Update();
    BikePreRender355Live::Update();
    TrainPosition355Live::Update();
    PlaneAmbient355Live::Update();
    PlaneBlow355Live::Update();
    PlaneControl355Live::Update();
    PlaneCreation355Live::Update();
} }
