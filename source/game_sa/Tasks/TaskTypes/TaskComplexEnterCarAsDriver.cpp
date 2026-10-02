/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexEnterCarAsDriver
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexEnterCarAsDriver.h"

void CTaskComplexEnterCarAsDriver::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEnterCarAsDriver, 0x86EAAC, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6402F0);

    RH_ScopedVMTInstall(Clone, 0x643780);
    RH_ScopedVMTInstall(GetTaskType, 0x640320);
}

// 0x6402F0
CTaskComplexEnterCarAsDriver::CTaskComplexEnterCarAsDriver(CVehicle* targetVehicle) :
    CTaskComplexEnterCar(targetVehicle, true, false, false, false)
{
}

CTaskComplexEnterCarAsDriver::CTaskComplexEnterCarAsDriver(CVehicle* targetVehicle, eMoveState moveState) : // NOTSA
    CTaskComplexEnterCarAsDriver{ targetVehicle }
{
    m_MoveState = moveState;
}

// For 0x643780
CTaskComplexEnterCarAsDriver::CTaskComplexEnterCarAsDriver(const CTaskComplexEnterCarAsDriver& o) :
    CTaskComplexEnterCarAsDriver{ o.m_Car }
{
    m_MoveState = o.m_MoveState;
}
