#pragma once

#include "TaskSimple.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleTogglePedThreatScanner : public CTaskSimple {
public:
    bool m_bScanAllowedScriptPed;
    bool m_bScanAllowedInVehicle;
    bool m_bScanAllowedScriptedTask;

public:
    static void InjectHooks();

    static constexpr auto Type = TASK_SIMPLE_TOGGLE_PED_THREAT_SCANNER;

    CTaskSimpleTogglePedThreatScanner(bool bScanAllowedScriptPed, bool bScanAllowedInVehicle, bool bScanAllowedScriptedTask);
    ~CTaskSimpleTogglePedThreatScanner() override = default;

    eTaskType GetTaskType() const override { return Type; } // 0x492DD0
    CTask* Clone() const override { return new CTaskSimpleTogglePedThreatScanner(m_bScanAllowedScriptPed, m_bScanAllowedInVehicle, m_bScanAllowedScriptedTask); } // 0x492D50
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return false; } // 0x492DE0
    bool ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x492D10
    CTaskSimpleTogglePedThreatScanner* Constructor(bool bScanAllowedScriptPed, bool bScanAllowedInVehicle, bool bScanAllowedScriptedTask) {
        this->CTaskSimpleTogglePedThreatScanner::CTaskSimpleTogglePedThreatScanner(bScanAllowedScriptPed, bScanAllowedInVehicle, bScanAllowedScriptedTask);
        return this;
    }
    CTaskSimpleTogglePedThreatScanner* ScalarDeletingDestructor(uint8 flags); // 0x492DF0 (lote 142A, banco Fn142A): destructor con borrado (ranura 0 de la vtable 0x85A334), `ret 4`
};
VALIDATE_SIZE(CTaskSimpleTogglePedThreatScanner, 0xC);
