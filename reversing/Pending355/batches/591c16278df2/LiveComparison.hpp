#pragma once
#include "../../../CameraFrustum355/LiveComparison.hpp"
#include "../../../WeaponOccupant355/LiveComparison.hpp"
#include "../../../WeaponInstant355/LiveComparison.hpp"
#include "../../../ProjectileUpdate355/LiveComparison.hpp"
#include "../../../ClothesGeometry355/LiveComparison.hpp"
#include "../../../ClothesSkin355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    CameraFrustum355Live::Update();
    WeaponOccupant355Live::Update();
    WeaponInstant355Live::Update();
    ProjectileUpdate355Live::Update();
    ClothesGeometry355Live::Update();
    ClothesSkin355Live::Update();
} }
