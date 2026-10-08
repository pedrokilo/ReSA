#pragma once
#include "../../../WaterFog355/LiveComparison.hpp"
#include "../../../FormationAssign355/LiveComparison.hpp"
#include "../../../FormationGather355/LiveComparison.hpp"
#include "../../../Sunbathe355/LiveComparison.hpp"
#include "../../../FormationFollow355/LiveComparison.hpp"
#include "../../../HassleVehicle355/LiveComparison.hpp"
#include "../../../GangFollower355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    WaterFog355Live::Update();
    FormationAssign355Live::Update();
    FormationGather355Live::Update();
    Sunbathe355Live::Update();
    FormationFollow355Live::Update();
    HassleVehicle355Live::Update();
    GangFollower355Live::Update();
} }
