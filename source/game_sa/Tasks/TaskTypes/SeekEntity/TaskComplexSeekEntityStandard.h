#pragma once

#include "PosCalculators/EntitySeekPosCalculatorStandard.h"
#include "TaskComplexSeekEntity.h"

class NOTSA_EXPORT_VTABLE CTaskComplexSeekEntityStandard : public CTaskComplexSeekEntity<CEntitySeekPosCalculatorStandard> {
public:
    static void InjectHooks();

    using CTaskComplexSeekEntity::CTaskComplexSeekEntity;

private:
    CTaskComplexSeekEntityStandard* Constructor(CEntity* entity, int32 seekInterval, int32 scanInterval, float maxDist, float moveRadius, float heightChange, bool tired, bool face) {
        this->CTaskComplexSeekEntityStandard::CTaskComplexSeekEntityStandard(entity, seekInterval, scanInterval, maxDist, moveRadius, heightChange, tired, face);
        return this;
    }
};
VALIDATE_SIZE(CTaskComplexSeekEntityStandard, 0x4C);
