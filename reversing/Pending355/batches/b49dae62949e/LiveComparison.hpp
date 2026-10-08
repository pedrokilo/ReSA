#pragma once
#include "../../../CameraDerived355/LiveComparison.hpp"
#include "../../../CameraDuck355/LiveComparison.hpp"
#include "../../../CameraFrustum355/LiveComparison.hpp"
#include "../../../CameraImprove355/LiveComparison.hpp"
#include "../../../CameraLinear355/LiveComparison.hpp"
#include "../../../CameraNear355/LiveComparison.hpp"
#include "../../../CameraNewMode355/LiveComparison.hpp"
#include "../../../CameraPath355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    CameraDerived355Live::Update();
    CameraDuck355Live::Update();
    CameraFrustum355Live::Update();
    CameraImprove355Live::Update();
    CameraLinear355Live::Update();
    CameraNear355Live::Update();
    CameraNewMode355Live::Update();
    CameraPath355Live::Update();
} }
