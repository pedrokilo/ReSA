#pragma once

#include "TaskSimpleAnim.h"
class CHandObject;

class NOTSA_EXPORT_VTABLE CTaskSimplePlayHandSignalAnim : public CTaskSimpleAnim {
public:
    static constexpr auto Type = TASK_SIMPLE_HANDSIGNAL_ANIM;

    CTaskSimplePlayHandSignalAnim(AnimationId animationId, float fBlendFactor, bool bFatHands, bool bHoldLastFrame);
    ~CTaskSimplePlayHandSignalAnim() override;

    eTaskType GetTaskType() const override; // 0x61AEA0 (lote 202A, sin validar)
    CTask* Clone() const override;          // 0x61B980 (lote 202A, sin validar)
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override; // 0x61AF50 (lote 202A, sin validar)
    bool ProcessPed(CPed* ped) override;

    void StartAnim(CPed* ped);

    //! 0x61AEB0 (lote 202A, sin validar): cuerpo del destructor del EXE (vtable 0x86D5B8, libera las manos y sigue con 0x61A6F0); funcion propia sin expansion en linea
    static void __fastcall DestructorBodyExe(CTaskSimplePlayHandSignalAnim* self, void* edx);

    static void InjectHooks();

private:
    AnimationId  m_PlayerIdlesAnimBlockId{};
    float        m_AnimBlenDelta{};
    bool         m_DoUseFatHands{};
    CHandObject* m_LeftHand{};
    CHandObject* m_RightHand{};
};
VALIDATE_SIZE(CTaskSimplePlayHandSignalAnim, 0x24);
