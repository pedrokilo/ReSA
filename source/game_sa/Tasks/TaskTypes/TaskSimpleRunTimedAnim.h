#pragma once

#include "TaskSimpleAnim.h"
#include "TaskTimer.h"
#include "AnimationEnums.h"

class CTaskSimpleRunTimedAnim;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleRunTimedAnim : public CTaskSimpleAnim {
public:
    // Como el original: los constructores no escriben el relleno de m_timer (+0x2E) ni el que sigue a m_taskId (+0x32)
    AssocGroupId m_animGrpId;
    AnimationId m_animId;
    float m_blendDelta;
    float m_unused1;
    uint32 m_durationMs;
    CTaskTimer m_timer;
    uint16 m_taskId; // Como el original: 16 bits (mov word ptr [esi+30h] / movzx eax,word ptr [ecx+30h])

public:
    static void InjectHooks();

    CTaskSimpleRunTimedAnim(AssocGroupId groupId, AnimationId animId, float blendDelta, float unused1, uint32 durationMs, bool holdLastFrame);
    CTaskSimpleRunTimedAnim(AssocGroupId groupId, AnimationId animId, float blendDelta, float unused1, uint32 durationMs, eTaskType taskType, const char* taskName, bool holdLastFrame);
    CTaskSimpleRunTimedAnim(const CTaskSimpleRunTimedAnim& o) : CTaskSimpleRunTimedAnim{o.m_animGrpId, o.m_animId, o.m_blendDelta, o.m_unused1, o.m_durationMs, o.m_bHoldLastFrame} {} // NOTSA
    ~CTaskSimpleRunTimedAnim() = default;

    void StartAnim(CPed* ped);

    CTask*    Clone() const override { return new CTaskSimpleRunTimedAnim{ *this }; }
    eTaskType GetTaskType() const override { return static_cast<eTaskType>(m_taskId); }
    bool      ProcessPed(CPed* ped) override;
private: // Wrappers for hooks
    friend class CTaskSimpleChat; // lote 151A (banco Fn151A): su constructor del EXE (0x5F65A0) llama al cuerpo enganchado 0x61ABE0
    // 0x61AB70
    CTaskSimpleRunTimedAnim* Constructor(AssocGroupId groupId, AnimationId animId, float blendDelta, float unused1, uint32 durationMs, bool holdLastFrame) {
        this->CTaskSimpleRunTimedAnim::CTaskSimpleRunTimedAnim(groupId, animId, blendDelta, unused1, durationMs, holdLastFrame);
        return this;
    }

    // 0x61ABE0
    CTaskSimpleRunTimedAnim* Constructor(AssocGroupId groupId, AnimationId animId, float blendDelta, float unused1, uint32 durationMs, eTaskType taskType, const char* unused2, bool holdLastFrame) {
        this->CTaskSimpleRunTimedAnim::CTaskSimpleRunTimedAnim(groupId, animId, blendDelta, unused1, durationMs, taskType, unused2, holdLastFrame);
        return this;
    }

    // 0x48E090
    __declspec(noinline) CTaskSimpleRunTimedAnim* Destructor() {
        this->CTaskSimpleRunTimedAnim::~CTaskSimpleRunTimedAnim();
        return this;
    }

    // 0x61BBC0 (lote 166A, banco Fn166A): destructor con borrado (ranura 0 de la vtable 0x86D570), `ret 4`
    CTaskSimpleRunTimedAnim* ScalarDeletingDestructor(uint8 flags);
};
