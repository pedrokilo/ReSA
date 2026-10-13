#pragma once

#include "TaskSimple.h"

class CPed;
class CEvent;

// Constructor reconstruido; las virtuales restantes conservan el cuerpo del EXE.
class NOTSA_EXPORT_VTABLE CTaskInteriorSitInChair : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_INTERIOR_SIT_ON_CHAIR;

    static void InjectHooks();

    // 0x675C30: devuelve EDX:EAX como el EXE (EAX = this)
    uint32 Constructor(void* info, uint32 duration, uint8 mode);
    std::array<uint8, 0x24> m_OriginalFields; // Campos nativos y padding sin inicializar.

    eTaskType GetTaskType() const override { return Type; } // 0x675C80
    // CTask* Clone() const override;
    // bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    // bool ProcessPed(CPed* ped) override;
};

VALIDATE_SIZE(CTaskInteriorSitInChair, 0x2C);
