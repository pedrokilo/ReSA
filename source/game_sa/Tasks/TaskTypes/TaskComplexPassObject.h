#pragma once

#include "TaskTimer.h"
#include "TaskComplex.h"
#include "Vector.h"

class CEvent;
class CPed;
class CEntity;
class CTaskComplexPassObject;

class NOTSA_EXPORT_VTABLE CTaskComplexPassObject : public CTaskComplex {

public:
    CPed*      m_ped{};          // 0xC
    bool       m_isPasser{};     // 0x10
    CVector    m_targetPos;      // 0x14 - Como el original: el constructor no lo escribe
    CTaskTimer m_timer;          // 0x20 - Como el original: sin {} (no se escribe el relleno 0x2A..0x2B)
    CEntity*   m_entityToPass;   // 0x2C - Como el original: el constructor no lo escribe

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_PASS_OBJECT;

    CTaskComplexPassObject(CPed* ped, bool isPasser);
    CTaskComplexPassObject(const CTaskComplexPassObject&);
    ~CTaskComplexPassObject();

    void AbortIK(CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexPassObject{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x6611B0
    CTaskComplexPassObject* Constructor(CPed* ped, uint8 isPasser) {
        this->CTaskComplexPassObject::CTaskComplexPassObject(ped, isPasser);
        *reinterpret_cast<uint8*>(&m_isPasser) = isPasser; // Como el original (0x6611B0): el byte se guarda crudo, sin normalizar a bool
        return this;
    }
    // 0x661230
    CTaskComplexPassObject* Destructor() {
        this->CTaskComplexPassObject::~CTaskComplexPassObject();
        return this;
    }
};
VALIDATE_SIZE(CTaskComplexPassObject, 0x30);
