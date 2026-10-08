#pragma once
#include "../../../ScriptLocate355/LiveComparison.hpp"
#include "../../../ScriptLocateChar355/LiveComparison.hpp"
#include "../../../ScriptArea355/LiveComparison.hpp"
#include "../../../ScriptPickup355/LiveComparison.hpp"
#include "../../../ScriptAngled355/LiveComparison.hpp"
#include "../../../MonsterControl355/LiveComparison.hpp"
#include "../../../MonsterSetup355/LiveComparison.hpp"
#include "../../../MonsterSuspension355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    ScriptLocate355Live::Update();
    ScriptLocateChar355Live::Update();
    ScriptArea355Live::Update();
    ScriptPickup355Live::Update();
    ScriptAngled355Live::Update();
    MonsterControl355Live::Update();
    MonsterSetup355Live::Update();
    MonsterSuspension355Live::Update();
} }
