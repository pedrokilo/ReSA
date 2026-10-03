#pragma once

#include "TaskComplex.h"
#include "Enums/eDirection.h"
#include <extensions/WEnum.hpp>

class CTaskComplexHitResponse;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexHitResponse final : public CTaskComplex {
public:
    notsa::WEnumS32<eDirection> m_HitSide{};

public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_HIT_RESPONSE;

    static void InjectHooks();

    CTaskComplexHitResponse(eDirection hs);
    CTaskComplexHitResponse(const CTaskComplexHitResponse&);
    ~CTaskComplexHitResponse() override = default;

    CTask*    Clone() const override { return new CTaskComplexHitResponse{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override { return nullptr; }
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

private: // Wrappers for hooks
    // 0x631D70
    CTaskComplexHitResponse* Constructor(int32 hs) {
        this->CTaskComplexHitResponse::CTaskComplexHitResponse((eDirection)hs);
        m_HitSide.get_underlying_ref() = hs; // Como el original: se guarda el dword completo del argumento
        return this;
    }

    // 0x631DA0
    CTaskComplexHitResponse* Destructor() {
        this->CTaskComplexHitResponse::~CTaskComplexHitResponse();
        return this;
    }
};
