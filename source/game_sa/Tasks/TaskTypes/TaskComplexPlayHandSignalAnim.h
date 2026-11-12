/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once


#include "TaskComplex.h"

//typedef int32 AnimationId;

class NOTSA_EXPORT_VTABLE CTaskComplexPlayHandSignalAnim : public CTaskComplex {
public:
    static inline auto& ms_animBlock = StaticRef<uint32>(0x8D2E38);

public:
    AnimationId m_animationId{};
    float       m_AnimBlenDelta{};
    bool        m_bLeftHandLoaded : 1;
    bool        m_bRightHandLoaded : 1;
    bool        m_bAnimationLoaded : 1;
    bool        m_DoUseFatHands : 1; // Como el original: el constructor (0x61B2B0) solo limpia los bits 0-2 (and 0F8h)

public:
    static constexpr auto Type = TASK_COMPLEX_HANDSIGNAL_ANIM;

    static void InjectHooks();

    explicit CTaskComplexPlayHandSignalAnim(AnimationId animationId = ANIM_ID_UNDEFINED, float blendFactor = 4.f);
    CTaskComplexPlayHandSignalAnim(const CTaskComplexPlayHandSignalAnim&);
    ~CTaskComplexPlayHandSignalAnim();

    static AnimationId __stdcall GetAnimIdForPed(CPed* ped);

    CTask* CreateSubTask(eTaskType taskType);

    CTask*    Clone() const override;
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;
private:
    // 0x61B2B0
    CTaskComplexPlayHandSignalAnim* Constructor(AnimationId animationId, float blendFactor) {
        this->CTaskComplexPlayHandSignalAnim::CTaskComplexPlayHandSignalAnim(animationId, blendFactor);
        return this;
    }

    // 0x61BDF0
    __declspec(noinline) CTaskComplexPlayHandSignalAnim* Destructor() {
        this->CTaskComplexPlayHandSignalAnim::~CTaskComplexPlayHandSignalAnim();
        return this;
    }

    // 0x61C0D0 (lote 170A, banco Fn170A): destructor con borrado (ranura 0 de la vtable 0x86D5DC), thiscall, un argumento de pila, `ret 4`
    CTaskComplexPlayHandSignalAnim* ScalarDeletingDestructor(uint8 flags);
};

VALIDATE_SIZE(CTaskComplexPlayHandSignalAnim, 0x18);
