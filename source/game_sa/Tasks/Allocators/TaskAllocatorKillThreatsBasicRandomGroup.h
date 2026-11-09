#pragma once

#include "TaskAllocatorKillThreatsBasic.h"

class NOTSA_EXPORT_VTABLE CTaskAllocatorKillThreatsBasicRandomGroup final : public CTaskAllocatorKillThreatsBasic {
public:
    constexpr static inline auto Type = eTaskAllocatorType::KILL_THREATS_BASIC_RANDOM_GROUP;

    static void InjectHooks();

    using CTaskAllocatorKillThreatsBasic::CTaskAllocatorKillThreatsBasic;
    ~CTaskAllocatorKillThreatsBasicRandomGroup() override = default;

    eTaskAllocatorType GetType() override { return eTaskAllocatorType::KILL_THREATS_BASIC_RANDOM_GROUP; }; // 0x5F68F0
    void AllocateTasks(CPedGroupIntelligence* intel) override;

    CTaskAllocatorKillThreatsBasicRandomGroup* ScalarDeletingDestructor(uint8 flags); // 0x5F6900 (lote 141A, banco Fn141A): destructor con borrado (ranura 0 de la vtable 0x86C80C), `ret 4`
};
