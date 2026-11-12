/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
#include "TaskTimer.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleStandStill : public CTaskSimple {
public:
    int32      m_nTime;
    CTaskTimer m_timer;
    bool       m_bLooped;
    bool       m_bUseAnimIdleStance;
    float      m_fBlendData;

public:
    static constexpr auto Type = TASK_SIMPLE_STAND_STILL;

    CTaskSimpleStandStill(int32 nTime = 0, bool Looped = false, bool bUseAnimIdleStance = false, float fBlendData = 8.0f);
    ~CTaskSimpleStandStill() override = default;

public:
    eTaskType GetTaskType() const override; // 0x62F350 (lote 171A, banco Fn171A)
    CTask* Clone() const override { return new CTaskSimpleStandStill(m_nTime, m_bLooped, m_bUseAnimIdleStance, m_fBlendData); } // 0x635CF0
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    friend class CTaskSimpleOnEscalator;      // su Clone llama al Constructor enganchado (0x62F310), como el EXE
    friend class CTaskComplexWalkRoundObject; // su CreateSubTask (0x655290) tambien
    static void InjectHooks();

    __declspec(noinline) CTaskSimpleStandStill* Constructor(int32 nTime, bool Looped, bool bUseAnimIdleStance, float fBlendData) { this->CTaskSimpleStandStill::CTaskSimpleStandStill(nTime, Looped, bUseAnimIdleStance, fBlendData); return this; }
};
VALIDATE_SIZE(CTaskSimpleStandStill, 0x20);

namespace CTaskSimpleStandStillBody {
// 0x62F360: cuerpo no virtual de ~CTaskSimpleStandStill del EXE (lo usan tambien los destructores de sus subclases, p. ej. CTaskSimpleOnEscalator)
void __fastcall StandStillDestructorBody(CTaskSimpleStandStill* self, void* edx);
} // namespace CTaskSimpleStandStillBody
