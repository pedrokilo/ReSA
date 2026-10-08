#pragma once
#include "../../../CamMan355/LiveComparison.hpp"
#include "../../../CamAttached355/LiveComparison.hpp"
#include "../../../CamCineExit355/LiveComparison.hpp"
#include "../../../CamDead355/LiveComparison.hpp"
#include "../../../CamFlyBy355/LiveComparison.hpp"
#include "../../../CamLamp355/LiveComparison.hpp"
#include "../../../CamRotate355/LiveComparison.hpp"
#include "../../../CamSpotter355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    CamMan355Live::Update();
    CamAttached355Live::Update();
    CamCineExit355Live::Update();
    CamDead355Live::Update();
    CamFlyBy355Live::Update();
    CamLamp355Live::Update();
    CamRotate355Live::Update();
    CamSpotter355Live::Update();
} }
