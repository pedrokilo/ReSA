#pragma once
#include "../../../CamLookBehind355/LiveComparison.hpp"
#include "../../../CamLookSide355/LiveComparison.hpp"
#include "../../../CamM16355/LiveComparison.hpp"
#include "../../../CamMan355/LiveComparison.hpp"
#include "../../../CamMouse355/LiveComparison.hpp"
#include "../../../CamPlane355/LiveComparison.hpp"
#include "../../../CamPlaneOrbit355/LiveComparison.hpp"
#include "../../../CamProcess355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    CamLookBehind355Live::Update();
    CamLookSide355Live::Update();
    CamM16355Live::Update();
    CamMan355Live::Update();
    CamMouse355Live::Update();
    CamPlane355Live::Update();
    CamPlaneOrbit355Live::Update();
    CamProcess355Live::Update();
} }
