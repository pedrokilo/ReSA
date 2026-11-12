#pragma once

#include "TaskComplexPartner.h"

class NOTSA_EXPORT_VTABLE CTaskComplexPartnerShove : public CTaskComplexPartner {
public:
    int32 field_70;

public:
    static constexpr auto Type = TASK_COMPLEX_PARTNER_SHOVE;

    CTaskComplexPartnerShove(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, int8 updateDirectionCount, CVector point);
    ~CTaskComplexPartnerShove() override = default;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override; // 0x684ED0 (lote 185A, sin validar)
    virtual CTaskComplexSequence* GetPartnerSequence();

    static void InjectHooks();
    CTaskComplexPartnerShove* Constructor(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, int8 updateDirectionCount, CVector point);
};
VALIDATE_SIZE(CTaskComplexPartnerShove, 0x74);
