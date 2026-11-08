#pragma once

#include "TaskSimple.h"
#include "TaskTimer.h"

class CTaskInteriorSitAtDesk;
class CAnimBlendAssociation;
class CEvent;
class CPed;

struct InteriorInfo_t;

class NOTSA_EXPORT_VTABLE CTaskInteriorSitAtDesk : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_INTERIOR_SIT_AT_DESK;

    static void InjectHooks();

    CTaskInteriorSitAtDesk(int32 duration, InteriorInfo_t* interiorInfo, bool bDoInstantly);
    CTaskInteriorSitAtDesk(const CTaskInteriorSitAtDesk&);
    ~CTaskInteriorSitAtDesk();
    static void FinishAnimCB(CAnimBlendAssociation*, void*);

    void StartRandomLoopAnim(CPed* ped, float blendDelta);
    void StartRandomOneOffAnim(CPed* ped);

    CTask*    Clone() const override { return new CTaskInteriorSitAtDesk{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

private:
    void StartAnim(CPed* ped, AnimationId animId, float blendDelta);

private: // Wrappers for hooks
    // 0x676010
    CTaskInteriorSitAtDesk* Constructor(int32 actionAnimTime, InteriorInfo_t* interiorInfo, uint8 a4) {
        this->CTaskInteriorSitAtDesk::CTaskInteriorSitAtDesk(actionAnimTime, interiorInfo, a4);
        return this;
    }

    // 0x676080
    __declspec(noinline) CTaskInteriorSitAtDesk* Destructor() {
        this->CTaskInteriorSitAtDesk::~CTaskInteriorSitAtDesk();
        return this;
    }
    CTaskInteriorSitAtDesk* ScalarDeletingDestructor(uint8 flags); // 0x677760 (lote 127A, banco Fn127A): destructor con borrado (ranura 0 de la vtable 0x87035C), `ret 4`

private:
    int32                  m_Duration;
    InteriorInfo_t*        m_InteriorInfo;
    bool                   m_bDoInstantly;
    CAnimBlendAssociation* m_Anim{};
    AnimationId            m_PrevAnimId{ANIM_ID_UNDEFINED};
    CTaskTimer             m_TaskTimer;
    CTaskTimer             m_AnimTimer;
    bool                   m_bUpdatePedPos{};
    bool                   m_bTaskFinished{};
    bool                   m_bTaskAborting{};
};
