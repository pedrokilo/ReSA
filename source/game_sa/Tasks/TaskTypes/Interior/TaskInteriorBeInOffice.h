#pragma once

#include "TaskComplex.h"

class InteriorGroup_c;
class Interior_c;
struct InteriorInfo_t;
class  CPed;

class NOTSA_EXPORT_VTABLE CTaskInteriorBeInOffice : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_INTERIOR_BE_IN_OFFICE;

    static void InjectHooks();

    CTaskInteriorBeInOffice(InteriorGroup_c* ig);
    CTaskInteriorBeInOffice(const CTaskInteriorBeInOffice&);
    ~CTaskInteriorBeInOffice() override = default;

    CTask*    Clone() const override { return new CTaskInteriorBeInOffice{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

private:
    void   GetInfoForPedToUse(int32* outDur); // Como el original: __thiscall con un solo argumento (ret 4)

private: // Wrappers for hooks
    // 0x675220
    CTaskInteriorBeInOffice* Constructor(InteriorGroup_c* ig) {
        this->CTaskInteriorBeInOffice::CTaskInteriorBeInOffice(ig);
        return this;
    }

    // 0x675260
    __declspec(noinline) CTaskInteriorBeInOffice* Destructor() {
        this->CTaskInteriorBeInOffice::~CTaskInteriorBeInOffice();
        return this;
    }
    CTaskInteriorBeInOffice* ScalarDeletingDestructor(uint8 flags); // 0x676330 (lote 127A, banco Fn127A): destructor con borrado (ranura 0 de la vtable 0x870218), `ret 4`

private:
    InteriorGroup_c* m_IntGrp{};
    Interior_c*      m_CurrInt{};
    InteriorInfo_t*  m_CurrIntInfo{};
};
