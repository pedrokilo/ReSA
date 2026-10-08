#pragma once
#include "../../../GeometryCore355/LiveComparison.hpp"
#include "../../../GeometryQueries355/LiveComparison.hpp"
#include "../../../GeometryRoutes355/LiveComparison.hpp"
#include "../../../ScriptBank355/LiveComparison.hpp"
#include "../../../ScriptUpdate355/LiveComparison.hpp"
#include "../../../BikeControl355/LiveComparison.hpp"
#include "../../../BikeRider355/LiveComparison.hpp"
#include "../../../BikePreRender355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    GeometryCore355Live::Update();
    GeometryQueries355Live::Update();
    GeometryRoutes355Live::Update();
    ScriptBank355Live::Update();
    ScriptUpdate355Live::Update();
    BikeControl355Live::Update();
    BikeRider355Live::Update();
    BikePreRender355Live::Update();
} }
