#pragma once
#include "../../../WeaponCarShot355/LiveComparison.hpp"
#include "../../../WeaponInstant355/LiveComparison.hpp"
#include "../../../ProjectileAdd355/LiveComparison.hpp"
#include "../../../ProjectileUpdate355/LiveComparison.hpp"
#include "../../../CarQuit355/LiveComparison.hpp"
#include "../../../CarRoom355/LiveComparison.hpp"
#include "../../../CarNearest355/LiveComparison.hpp"
#include "../../../ScriptAnim355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    WeaponCarShot355Live::Update();
    WeaponInstant355Live::Update();
    ProjectileAdd355Live::Update();
    ProjectileUpdate355Live::Update();
    CarQuit355Live::Update();
    CarRoom355Live::Update();
    CarNearest355Live::Update();
    ScriptAnim355Live::Update();
} }
