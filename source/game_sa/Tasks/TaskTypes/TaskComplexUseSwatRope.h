#pragma once

#include "TaskComplex.h"
#include "Heli.h"

class NOTSA_EXPORT_VTABLE CTaskComplexUseSwatRope : public CTaskComplex {
public:
    uint32 m_nRopeId;
    float  m_fCoorAlongRope;
    uint8  m_bIsOnHeli : 1; // Como el original: bit 0 del byte +0x14 (or/and/test byte ptr [+14h],1); el resto del byte no se toca
    CHeli* m_pHeli;

public:
    static constexpr auto Type = TASK_COMPLEX_USE_SWAT_ROPE;

    CTaskComplexUseSwatRope(uint32 ropeId, CHeli* heli);
    explicit CTaskComplexUseSwatRope(uint32 ropeId);
    ~CTaskComplexUseSwatRope() override;

    CTask* Clone() const override;
    eTaskType GetTaskType() const override { return Type; }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;
    virtual bool IsInterruptable(CPed* ped) { return false; }
    virtual bool AbortUrgently(CPed* ped) { return false; }

    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexUseSwatRope* Constructor(uint32 ropeId, CHeli* heli);

};

VALIDATE_SIZE(CTaskComplexUseSwatRope, 0x1C);
