#pragma once
#include "../../../ClothesTextures355/LiveComparison.hpp"
#include "../../../FormationAssign355/LiveComparison.hpp"
#include "../../../FormationFollow355/LiveComparison.hpp"
#include "../../../FormationGather355/LiveComparison.hpp"
#include "../../../FormationLookup355/LiveComparison.hpp"
#include "../../../FxEmitterCore355/LiveComparison.hpp"
#include "../../../FxEmitterParticle355/LiveComparison.hpp"
#include "../../../FxEmitterRender355/LiveComparison.hpp"
namespace Pending355BatchLive { inline void Update() {
    ClothesTextures355Live::Update();
    FormationAssign355Live::Update();
    FormationFollow355Live::Update();
    FormationGather355Live::Update();
    FormationLookup355Live::Update();
    FxEmitterCore355Live::Update();
    FxEmitterParticle355Live::Update();
    FxEmitterRender355Live::Update();
} }
