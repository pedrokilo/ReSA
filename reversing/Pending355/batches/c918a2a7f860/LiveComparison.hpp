#pragma once
#include "../../../HudSubtitles355/LiveComparison.hpp"
#include "../../../HudWanted355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    HudSubtitles355Live::Update();
    HudWanted355Live::Update();
} }
