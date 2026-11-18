#pragma once

#include "TaskComplex.h"

class CPed;
class CTaskComplexInWater;

class NOTSA_EXPORT_VTABLE CTaskComplexInWater : public CTaskComplex {

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_IN_WATER;

    CTaskComplexInWater() = default;
    CTaskComplexInWater(const CTaskComplexInWater&);
    ~CTaskComplexInWater() = default;

    // 0x6370B0
    CTask*    Clone() const override { return new CTaskComplexInWater{ *this }; }
    // 0x6350F0
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    // 0x635420
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

private: // Wrappers for hooks
    friend CTaskComplexInWater* __fastcall InWaterScalarDeletingDestructor(CTaskComplexInWater* self, void* edx, uint8 flags); // lote 207A: destructor con borrado (0x6396A0), llama al cuerpo enganchado Destructor
    // 0x6350D0
    CTaskComplexInWater* Constructor() {
        this->CTaskComplexInWater::CTaskComplexInWater();
        return this;
    }

    // 0x635100
    CTaskComplexInWater* Destructor() {
        this->CTaskComplexInWater::~CTaskComplexInWater();
        return this;
    }
};
