#pragma once
#include "../../../StorageIO355/LiveComparison.hpp"
#include "../../../StorageGame355/LiveComparison.hpp"
#include "../../../InteriorClosest355/LiveComparison.hpp"
#include "../../../InteriorExit355/LiveComparison.hpp"
#include "../../../InteriorPathsLoaded355/LiveComparison.hpp"
#include "../../../InteriorOffice355/LiveComparison.hpp"
#include "../../../InteriorSpawn355/LiveComparison.hpp"
#include "../../../InteriorSetupPaths355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    StorageIO355Live::Update();
    StorageGame355Live::Update();
    InteriorClosest355Live::Update();
    InteriorExit355Live::Update();
    InteriorPathsLoaded355Live::Update();
    InteriorOffice355Live::Update();
    InteriorSpawn355Live::Update();
    InteriorSetupPaths355Live::Update();
} }
