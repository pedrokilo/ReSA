#pragma once
#include "../../../CarNearest355/LiveComparison.hpp"
#include "../../../CarPassenger355/LiveComparison.hpp"
#include "../../../CarQuit355/LiveComparison.hpp"
#include "../../../CarRoom355/LiveComparison.hpp"
#include "../../../ClothesArray355/LiveComparison.hpp"
#include "../../../ClothesGeometry355/LiveComparison.hpp"
#include "../../../ClothesRules355/LiveComparison.hpp"
#include "../../../ClothesSkin355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    CarNearest355Live::Update();
    CarPassenger355Live::Update();
    CarQuit355Live::Update();
    CarRoom355Live::Update();
    ClothesArray355Live::Update();
    ClothesGeometry355Live::Update();
    ClothesRules355Live::Update();
    ClothesSkin355Live::Update();
} }
