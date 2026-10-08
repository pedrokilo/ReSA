#pragma once
#include "../../../RadioConcat355/LiveComparison.hpp"
#include "../../../RadioTrack355/LiveComparison.hpp"
#include "../../../RadioStop355/LiveComparison.hpp"
#include "../../../RadioRetune355/LiveComparison.hpp"
#include "../../../RadioService355/LiveComparison.hpp"
#include "../../../WeaponScreen355/LiveComparison.hpp"
#include "../../../WeaponTank355/LiveComparison.hpp"
#include "../../../WeaponOccupant355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    RadioConcat355Live::Update();
    RadioTrack355Live::Update();
    RadioStop355Live::Update();
    RadioRetune355Live::Update();
    RadioService355Live::Update();
    WeaponScreen355Live::Update();
    WeaponTank355Live::Update();
    WeaponOccupant355Live::Update();
} }
