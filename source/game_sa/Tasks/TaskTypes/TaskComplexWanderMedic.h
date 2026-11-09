#pragma once

#include "TaskComplexWander.h"
#include "eMoveState.h"

class NOTSA_EXPORT_VTABLE CTaskComplexWanderMedic : public CTaskComplexWander {
public:
    static void InjectHooks();

    CTaskComplexWanderMedic(eMoveState MoveState, uint8 Dir, bool bWanderSensibly = true);

    eWanderType GetWanderType() override { return WANDER_TYPE_MEDIC; } // 0x658810
    CTask* Clone() const override { return new CTaskComplexWanderMedic(m_nMoveState, m_nDir, m_bWanderSensibly); } // 0x6587A0
    void ScanForStuff(CPed* ped) override {} // 0x658820

private: // Wrappers for hooks
    // 0x658770
    CTaskComplexWanderMedic* Constructor(eMoveState MoveState, uint8 Dir, bool bWanderSensibly) {
        this->CTaskComplexWanderMedic::CTaskComplexWanderMedic(MoveState, Dir, bWanderSensibly);
        return this;
    }
    CTaskComplexWanderMedic* ScalarDeletingDestructor(uint8 flags); // 0x658830 (lote 142A, banco Fn142A): destructor con borrado (ranura 0 de la vtable 0x86F48C), `ret 4`
};
