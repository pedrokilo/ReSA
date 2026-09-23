#pragma once

#include "TaskComplexGoToPointAiming.h"

class CEntity;

class NOTSA_EXPORT_VTABLE CTaskComplexGoToPointShooting : public CTaskComplexGoToPointAiming {
public:
    static constexpr auto Type = TASK_COMPLEX_GO_TO_POINT_SHOOTING;

    CTaskComplexGoToPointShooting(
        eMoveState moveState,
        const CVector& movePos,
        CEntity* aimAtEntity,
        CVector aimPos,
        const float targetRadius,
        const float slowDownDist
    );
    CTaskComplexGoToPointShooting(const CTaskComplexGoToPointShooting&);
    ~CTaskComplexGoToPointShooting() override = default;

    eTaskType GetTaskType() const override;
    CTask*    Clone() const override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    // 0x668C70
    CTaskComplexGoToPointShooting* Constructor(eMoveState moveState, CVector const& movePos, CEntity* aimAtEntity, CVector aimPos, float targetRadius, float slowDownDist) {
        this->CTaskComplexGoToPointShooting::CTaskComplexGoToPointShooting(moveState, movePos, aimAtEntity, aimPos, targetRadius, slowDownDist);
        return this;
    }
};
