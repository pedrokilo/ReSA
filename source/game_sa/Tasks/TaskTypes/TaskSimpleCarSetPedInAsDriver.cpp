/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarSetPedInAsDriver
 * Reconstrucción: INCOMPLETA — 4/5 funciones reconstruidas.
 * Pendientes de reconstruir: 1.
 * Funciones bloqueadas: 1.
 * Validación registrada: 0/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   ProcessPed | 0x0064B950 | V
 * BLOQUEADAS (nombre | dirección | tipo):
 *   ProcessPed | 0x0064B950 | V
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleCarSetPedInAsDriver.h"

void CTaskSimpleCarSetPedInAsDriver::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarSetPedInAsDriver, 0x86EE28, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6470E0);

    RH_ScopedVMTInstall(Clone, 0x649E00);
    RH_ScopedVMTInstall(GetTaskType, 0x647150);
    RH_ScopedVMTInstall(MakeAbortable, 0x647160);
    // Stub hacia el original: activarlo provocaría recursión infinita.
    RH_ScopedVMTInstall(ProcessPed, 0x64B950, { .reversed = false });
}

// 0x6470E0
CTaskSimpleCarSetPedInAsDriver::CTaskSimpleCarSetPedInAsDriver(CVehicle* targetVehicle, CTaskUtilityLineUpPedWithCar* utility) : CTaskSimple() {
    m_bIsFinished = 0;
    m_pAnim = 0;
    m_pTargetVehicle = targetVehicle;
    m_pUtility = utility;
    m_bWarpingInToCar = 0;
    m_nDoorFlagsToClear = 0;
    m_nNumGettingInToClear = 0;
    CEntity::SafeRegisterRef(m_pTargetVehicle);
}

CTaskSimpleCarSetPedInAsDriver::CTaskSimpleCarSetPedInAsDriver(CVehicle* targetVehicle, bool warpingInToCar, CTaskUtilityLineUpPedWithCar* utility) : // NOTSA
    CTaskSimpleCarSetPedInAsDriver{ targetVehicle, utility }
{
    m_bWarpingInToCar = warpingInToCar;
}

CTaskSimpleCarSetPedInAsDriver::~CTaskSimpleCarSetPedInAsDriver() {
    CEntity::SafeCleanUpRef(m_pTargetVehicle);
}

// 0x649E00
CTask* CTaskSimpleCarSetPedInAsDriver::Clone() const {
    auto task = new CTaskSimpleCarSetPedInAsDriver(m_pTargetVehicle, m_pUtility);
    task->m_bWarpingInToCar = m_bWarpingInToCar;
    task->m_nDoorFlagsToClear = m_nDoorFlagsToClear;
    task->m_nNumGettingInToClear = m_nNumGettingInToClear;
    return task;
}

// 0x64B950
bool CTaskSimpleCarSetPedInAsDriver::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x64B950, CTask*, CPed*>(this, ped);
}
