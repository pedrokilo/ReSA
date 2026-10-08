#pragma once
#include "../../../RadioTrack355/LiveComparison.hpp"
#include "../../../RadioStop355/LiveComparison.hpp"
#include "../../../RadioRetune355/LiveComparison.hpp"
#include "../../../RadioService355/LiveComparison.hpp"
#include "../../../WeaponOccupant355/LiveComparison.hpp"
#include "../../../WeaponCarShot355/LiveComparison.hpp"
#include "../../../WeaponInstant355/LiveComparison.hpp"
#include "../../../ProjectileUpdate355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    RadioTrack355Live::Update();
    RadioStop355Live::Update();
    RadioRetune355Live::Update();
    RadioService355Live::Update();
    WeaponOccupant355Live::Update();
    WeaponCarShot355Live::Update();
    WeaponInstant355Live::Update();
    ProjectileUpdate355Live::Update();
} }
