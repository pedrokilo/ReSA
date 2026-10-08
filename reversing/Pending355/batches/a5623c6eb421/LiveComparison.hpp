#pragma once
#include "../../../TrainLinks355/LiveComparison.hpp"
#include "../../../TrainCoordinates355/LiveComparison.hpp"
#include "../../../TrainQueries355/LiveComparison.hpp"
#include "../../../TrainRemove355/LiveComparison.hpp"
#include "../../../TrainSkip355/LiveComparison.hpp"
#include "../../../TrainInit355/LiveComparison.hpp"
#include "../../../TrainRead355/LiveComparison.hpp"
#include "../../../TrainNearest355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    TrainLinks355Live::Update();
    TrainCoordinates355Live::Update();
    TrainQueries355Live::Update();
    TrainRemove355Live::Update();
    TrainSkip355Live::Update();
    TrainInit355Live::Update();
    TrainRead355Live::Update();
    TrainNearest355Live::Update();
} }
