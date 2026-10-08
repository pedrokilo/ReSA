#pragma once
#include "../../../PlaneFlying355/LiveComparison.hpp"
#include "../../../PlaneGeneration355/LiveComparison.hpp"
#include "../../../PlaneDamage355/LiveComparison.hpp"
#include "../../../PlaneInputs355/LiveComparison.hpp"
#include "../../../PlanePreRender355/LiveComparison.hpp"
#include "../../../CamControl355/LiveComparison.hpp"
#include "../../../CameraProcess355/LiveComparison.hpp"
#include "../../../CameraNewMode355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    PlaneFlying355Live::Update();
    PlaneGeneration355Live::Update();
    PlaneDamage355Live::Update();
    PlaneInputs355Live::Update();
    PlanePreRender355Live::Update();
    CamControl355Live::Update();
    CameraProcess355Live::Update();
    CameraNewMode355Live::Update();
} }
