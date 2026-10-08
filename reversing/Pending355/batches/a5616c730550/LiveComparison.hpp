#pragma once
#include "../../../TrainCtor355/LiveComparison.hpp"
#include "../../../TrainHit355/LiveComparison.hpp"
#include "../../../TrainPosition355/LiveComparison.hpp"
#include "../../../TrainPassenger355/LiveComparison.hpp"
#include "../../../TrainGeneration355/LiveComparison.hpp"
#include "../../../TrainCreate355/LiveComparison.hpp"
#include "../../../TrainAddPassenger355/LiveComparison.hpp"
#include "../../../RadioQueue355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    TrainCtor355Live::Update();
    TrainHit355Live::Update();
    TrainPosition355Live::Update();
    TrainPassenger355Live::Update();
    TrainGeneration355Live::Update();
    TrainCreate355Live::Update();
    TrainAddPassenger355Live::Update();
    RadioQueue355Live::Update();
} }
