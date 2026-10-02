/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleTogglePedThreatScanner
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleTogglePedThreatScanner.h"

void CTaskSimpleTogglePedThreatScanner::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleTogglePedThreatScanner, 0x85A334, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x492D10);

    RH_ScopedVMTInstall(Clone, 0x492D50);
    RH_ScopedVMTInstall(GetTaskType, 0x492DD0);
    RH_ScopedVMTInstall(MakeAbortable, 0x492DE0);
    RH_ScopedVMTInstall(ProcessPed, 0x6337A0);
}

// 0x492D10
CTaskSimpleTogglePedThreatScanner::CTaskSimpleTogglePedThreatScanner(bool bScanAllowedScriptPed, bool bScanAllowedInVehicle, bool bScanAllowedScriptedTask)
    : CTaskSimple()
{
    m_bScanAllowedScriptPed = bScanAllowedScriptPed;
    m_bScanAllowedInVehicle = bScanAllowedInVehicle;
    m_bScanAllowedScriptedTask = bScanAllowedScriptedTask;
}

// 0x6337A0
bool CTaskSimpleTogglePedThreatScanner::ProcessPed(CPed* ped) {
    auto& scanner = ped->GetIntelligence()->GetEventScanner().m_pedAcquaintanceScanner;
    scanner.m_bScanAllowedInVehicle    = m_bScanAllowedInVehicle;
    scanner.m_bScanAllowedScriptPed    = m_bScanAllowedScriptPed;
    scanner.m_bScanAllowedScriptedTask = m_bScanAllowedScriptedTask;
    return true;
}
