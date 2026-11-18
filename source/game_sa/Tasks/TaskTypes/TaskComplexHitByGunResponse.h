#pragma once

#include "TaskComplex.h"
#include "Enums/eDirection.h"
#include <extensions/WEnum.hpp>

class CPed;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskComplexHitByGunResponse : public CTaskComplex {
public:
    notsa::WEnumS32<eDirection> m_HitSide{};

public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_HIT_BY_GUN_RESPONSE;

    static void InjectHooks();

    CTaskComplexHitByGunResponse(eDirection hitSide);
    CTaskComplexHitByGunResponse(const CTaskComplexHitByGunResponse&);
    ~CTaskComplexHitByGunResponse() override = default;

    // 0x636640
    CTask*    Clone() const override { return new CTaskComplexHitByGunResponse{ *this }; }
    // 0x631DF0
    eTaskType GetTaskType() const override { return Type; }
    // 0x631E40
    CTask*    CreateNextSubTask(CPed* ped) override { return nullptr; }
    CTask*    CreateFirstSubTask(CPed* ped) override;
    // 0x631F70
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;

private: // Wrappers for hooks
    // 0x631DD0
    // Como el original: guarda el dword completo del argumento
    CTaskComplexHitByGunResponse* Constructor(int32 hs) {
        this->CTaskComplexHitByGunResponse::CTaskComplexHitByGunResponse(eDirection::FORWARD);
        m_HitSide.get_underlying_ref() = hs;
        return this;
    }

    // 0x631E30
    CTaskComplexHitByGunResponse* Destructor() {
        this->CTaskComplexHitByGunResponse::~CTaskComplexHitByGunResponse();
        return this;
    }
};
