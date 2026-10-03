#pragma once

#include "Base.h"
#include "TaskComplexSeekEntityAnyMeans.h"
#include "PosCalculators/EntitySeekPosCalculatorXYOffset.h"

class NOTSA_EXPORT_VTABLE CTaskComplexSeekEntityAnyMeansXYOffset : public CTaskComplexSeekEntityAnyMeans<CEntitySeekPosCalculatorXYOffset> {
public:
    static void InjectHooks();

    CTaskComplexSeekEntityAnyMeansXYOffset(CEntity* entity, const CVector& offsetXY, int32 period = 1000, float radius = 8.f) :
        CTaskComplexSeekEntityAnyMeans{ entity, period, radius, { offsetXY } }
    {
    }

    // 0x696030 - Como el original: el clon es un CTaskComplexSeekEntityAnyMeansXYOffset (vtable 0x870CF0) con periodo 1000, radio 8 y el mismo desplazamiento
    CTask* Clone() const override { return new CTaskComplexSeekEntityAnyMeansXYOffset{ *this }; }
};
