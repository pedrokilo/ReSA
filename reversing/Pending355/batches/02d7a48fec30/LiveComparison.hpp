#pragma once
#include "../../../CameraDerived355/LiveComparison.hpp"
#include "../../../CameraFrustum355/LiveComparison.hpp"
#include "../../../CameraImprove355/LiveComparison.hpp"
#include "../../../CameraPath355/LiveComparison.hpp"
#include "../../../CameraRwCopy355/LiveComparison.hpp"
#include "../../../CamPlane355/LiveComparison.hpp"
#include "../../../ScriptLoaded355/LiveComparison.hpp"
#include "../../../WeaponDrive355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    CameraDerived355Live::Update();
    CameraFrustum355Live::Update();
    CameraImprove355Live::Update();
    CameraPath355Live::Update();
    CameraRwCopy355Live::Update();
    CamPlane355Live::Update();
    ScriptLoaded355Live::Update();
    WeaponDrive355Live::Update();
} }
