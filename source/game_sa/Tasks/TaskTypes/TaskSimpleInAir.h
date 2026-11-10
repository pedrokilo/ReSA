/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
#include "TaskTimer.h"
#include "Vector.h"
class CAnimBlendAssociation;
class CEntity;

class NOTSA_EXPORT_VTABLE CTaskSimpleInAir : public CTaskSimple {
public:
    CVector                m_vecPosn;
    float                  m_fAngle;
    eSurfaceType           m_nSurfaceType;
    CAnimBlendAssociation* m_pAnim;
    float                  m_fMinZSpeed;
    union {
        uint8 m_nFlags;
        struct
        {
            uint8 m_bUsingJumpGlide : 1;
            uint8 m_bUsingFallGlide : 1;
            uint8 m_bUsingClimbJump : 1;
        };
    };
    uint32     m_nProcessCounter;
    CTaskTimer m_timer;
    CEntity*   m_pClimbEntity;

    static inline auto& ms_fSlowFallThreshold = StaticRef<float>(0x8D2EFC); // -0.05f
    static inline auto& ms_nMaxSlowFallFrames = StaticRef<uint32>(0x8D2EF8); // 10

public:
    static constexpr auto Type = TASK_SIMPLE_IN_AIR;

    CTaskSimpleInAir(bool bUsingJumpGlide, bool bUsingFallGlide, bool bUsingClimbJump);
    ~CTaskSimpleInAir() override;

    eTaskType GetTaskType() const override; // 0x678D40 (lote 152A, banco Fn152A): `mov eax,0F1h; ret` (ranura 4 de la vtable 0x8704D8)
    CTask* Clone() const override { return new CTaskSimpleInAir(m_bUsingJumpGlide, m_bUsingFallGlide, m_bUsingClimbJump); }
    bool ProcessPed(CPed* ped) override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;


    static void DeleteAnimCB(CAnimBlendAssociation* anim, void* data);
    void ApplyRollAndPitch(CPed* ped); // 0x678E80: vacia en el EXE

    static void InjectHooks();
    CTaskSimpleInAir* Constructor(bool bUsingJumpGlide, bool bUsingFallGlide, bool bUsingClimbJump);
};
VALIDATE_SIZE(CTaskSimpleInAir, 0x3C);
