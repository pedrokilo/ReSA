#pragma once

#include "TaskSimple.h"

class CAnimBlendAssociation;
class CPed;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskSimpleTurn180 : public CTaskSimple {

public:
    bool m_animHasFinished = {};        // 0x8
    CAnimBlendAssociation* m_anim = {}; // 0xC

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_SIMPLE_TURN_180;

    CTaskSimpleTurn180() = default;
    CTaskSimpleTurn180(const CTaskSimpleTurn180&);
    ~CTaskSimpleTurn180();

    static void FinishAnimTurn180CB(CAnimBlendAssociation* anim, void* data);

    // 0x636570
    CTask*    Clone() const override { return new CTaskSimpleTurn180{ *this }; }
    // 0x631CE0
    eTaskType GetTaskType() const override { return Type; }
    bool      ProcessPed(CPed* ped) override;
    // 0x631CF0
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return false; }

private: // Wrappers for hooks
    // 0x631CC0
    CTaskSimpleTurn180* Constructor() {
        this->CTaskSimpleTurn180::CTaskSimpleTurn180();
        return this;
    }
    // 0x631D00
    CTaskSimpleTurn180* Destructor() {
        this->CTaskSimpleTurn180::~CTaskSimpleTurn180();
        return this;
    }
};
