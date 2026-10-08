#pragma once
#include "../../../ScriptEvents355/LiveComparison.hpp"
#include "../../../ShadowPermanent355/LiveComparison.hpp"
#include "../../../WaterNearest355/LiveComparison.hpp"
#include "../../../WaterFog355/LiveComparison.hpp"
#include "../../../WaterQuery355/LiveComparison.hpp"
#include "../../../WaterRender355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    ScriptEvents355Live::Update();
    ShadowPermanent355Live::Update();
    WaterNearest355Live::Update();
    WaterFog355Live::Update();
    WaterQuery355Live::Update();
    WaterRender355Live::Update();
} }
