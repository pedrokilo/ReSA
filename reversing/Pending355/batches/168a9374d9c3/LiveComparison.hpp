#pragma once
#include "../../../CamArrest355/LiveComparison.hpp"
#include "../../../CamAttached355/LiveComparison.hpp"
#include "../../../CamBirdy355/LiveComparison.hpp"
#include "../../../CamCineExit355/LiveComparison.hpp"
#include "../../../CamControl355/LiveComparison.hpp"
#include "../../../CamCore355/LiveComparison.hpp"
#include "../../../CamDead355/LiveComparison.hpp"
#include "../../../CameraCollision355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    CamArrest355Live::Update();
    CamAttached355Live::Update();
    CamBirdy355Live::Update();
    CamCineExit355Live::Update();
    CamControl355Live::Update();
    CamCore355Live::Update();
    CamDead355Live::Update();
    CameraCollision355Live::Update();
} }
