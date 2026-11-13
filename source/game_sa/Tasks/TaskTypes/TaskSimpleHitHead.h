#pragma once

#include "TaskSimple.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleHitHead : public CTaskSimple {
public:
    bool m_bIsFinished;
    CAnimBlendAssociation* m_pAnim;

public:
    static constexpr auto Type = TASK_SIMPLE_HIT_HEAD;

    CTaskSimpleHitHead();
    ~CTaskSimpleHitHead() override;

    eTaskType GetTaskType() const override; // 0x653080 (lote 150A, banco Fn150A: definido en el .cpp)
    CTask* Clone() const override; // 0x6557A0 (lote 180A, banco Fn180A)
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;
    static void FinishAnimCB(CAnimBlendAssociation* anim, void* data);

    static void InjectHooks();
    CTaskSimpleHitHead* Constructor();
};
VALIDATE_SIZE(CTaskSimpleHitHead, 0x10);
