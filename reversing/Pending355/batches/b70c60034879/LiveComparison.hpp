#pragma once
#include "../../../CamRotate355/LiveComparison.hpp"
#include "../../../CamShooting355/LiveComparison.hpp"
#include "../../../CamSpotter355/LiveComparison.hpp"
#include "../../../CamSyphon355/LiveComparison.hpp"
#include "../../../CamTwoCars355/LiveComparison.hpp"
#include "../../../CamTwoPlayer355/LiveComparison.hpp"
#include "../../../CamWheel355/LiveComparison.hpp"
#include "../../../CarDoorPosition355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    CamRotate355Live::Update();
    CamShooting355Live::Update();
    CamSpotter355Live::Update();
    CamSyphon355Live::Update();
    CamTwoCars355Live::Update();
    CamTwoPlayer355Live::Update();
    CamWheel355Live::Update();
    CarDoorPosition355Live::Update();
} }
