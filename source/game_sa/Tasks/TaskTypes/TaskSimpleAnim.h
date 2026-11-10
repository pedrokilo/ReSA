/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimple.h"
class CAnimBlendAssociation;

class NOTSA_EXPORT_VTABLE CTaskSimpleAnim : public CTaskSimple {
public:
    CAnimBlendAssociation *m_pAnim;
    union {
        struct {
            bool m_bIsFinished : 1;
            bool m_bDontInterrupt : 1;
            bool m_bHoldLastFrame : 1;

            // These flags are used in CTaskSimpleRunAnim only
            bool m_bDontBlendOut : 1;

            // These flags are used in 	CTaskSimpleRunNamedAnim only
            bool m_bRunInSequence : 1;
            bool m_bOffsetAtEnd : 1;
            bool m_bOffsetAvailable : 1;
        };
        uint8 m_nFlags;
    };

public:
    explicit CTaskSimpleAnim(bool bHoldLastFrame);
    ~CTaskSimpleAnim() override;

    //! 0x61A6F0 (lote 150A, banco Fn150A; usado desde el lote 151A por el cuerpo de ~CTaskSimpleLookAbout): cuerpo del destructor del EXE, funcion propia sin expansion en linea
    static void __fastcall DestructorBodyExe(CTaskSimpleAnim* self, void* edx);

    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    static void FinishRunAnimCB(CAnimBlendAssociation* blendAssoc, void* data); //data is CTaskSimpleAnim

    static void InjectHooks();
};
VALIDATE_SIZE(CTaskSimpleAnim, 0x10);
