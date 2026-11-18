#pragma once

#include "TaskSimpleNone.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleFinishBrain : public CTaskSimpleNone {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_FINISH_BRAIN;

    static void InjectHooks();

    CTaskSimpleFinishBrain() = default;
    CTaskSimpleFinishBrain(const CTaskSimpleFinishBrain&);
    ~CTaskSimpleFinishBrain() override = default;

    /*!
     * @addr 0x463660
     */
    CTask*    Clone() const override { return new CTaskSimpleFinishBrain{ *this }; }
    /*!
     * @addr 0x4636B0
     */
    eTaskType GetTaskType() const override { return Type; }

private:
    CTaskSimpleFinishBrain* ScalarDeletingDestructor(uint8 flags); // 0x4636C0 (lote 142A, banco Fn142A): destructor con borrado (ranura 0 de la vtable 0x859D20), `ret 4`
};
