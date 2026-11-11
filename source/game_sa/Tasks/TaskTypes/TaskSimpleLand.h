#pragma once

#include "TaskSimple.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleLand : public CTaskSimple {
public:
    CAnimBlendAssociation* m_pAnim;
    AnimationId            m_nAnimId;
    union {
        struct
        {
            uint8 bIsFinished : 1;
            uint8 bNoAnimation : 1;
            uint8 bPedNotUpdated : 1;
        };
        uint8 m_nFlags;
    };

public:
    static constexpr auto Type = TASK_SIMPLE_LAND;

    explicit CTaskSimpleLand(AnimationId nAnimId);
    ~CTaskSimpleLand() override;

    eTaskType GetTaskType() const override; // 0x678ED0 (lote 159A, banco Fn159A)
    CTask* Clone() const override { return new CTaskSimpleLand(m_nAnimId); }
    bool ProcessPed(CPed* ped) override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;

    bool LeftFootLanded();
    bool RightFootLanded();

    static void FinishAnimCB(CAnimBlendAssociation* anim, void* data);

    static void InjectHooks();
    CTaskSimpleLand* Constructor(AnimationId nAnimId);
};
VALIDATE_SIZE(CTaskSimpleLand, 0x14);
