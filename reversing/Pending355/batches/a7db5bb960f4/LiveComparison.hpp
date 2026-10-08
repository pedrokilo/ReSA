#pragma once
#include "../../../HudSubtitles355/LiveComparison.hpp"
#include "../../../HudWanted355/LiveComparison.hpp"
#include "../../../HudTimers355/LiveComparison.hpp"
#include "../../../HudVitals355/LiveComparison.hpp"
#include "../../../HudCrosshairs355/LiveComparison.hpp"
#include "../../../HudHelp355/LiveComparison.hpp"
#include "../../../HudPlayerInfo355/LiveComparison.hpp"
#include "../../../HudSuccess355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    HudSubtitles355Live::Update();
    HudWanted355Live::Update();
    HudTimers355Live::Update();
    HudVitals355Live::Update();
    HudCrosshairs355Live::Update();
    HudHelp355Live::Update();
    HudPlayerInfo355Live::Update();
    HudSuccess355Live::Update();
} }
