/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarSetPedOut
 * Reconstrucción: INCOMPLETA — 2/5 funciones reconstruidas.
 * Pendientes de reconstruir: 3.
 * Funciones bloqueadas: 3.
 * Validación registrada: 0/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   Clone | 0x00649F50 | V
 *   PositionPedOutOfCollision | 0x006479B0 | S
 *   ProcessPed | 0x00647D10 | V
 * BLOQUEADAS (nombre | dirección | tipo):
 *   Clone | 0x00649F50 | V
 *   PositionPedOutOfCollision | 0x006479B0 | S
 *   ProcessPed | 0x00647D10 | V
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleCarSetPedOut.h"

void CTaskSimpleCarSetPedOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarSetPedOut, 0x86EEB8, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    // Stubs hacia el original: activarlos provocaría recursión infinita.
    RH_ScopedInstall(PositionPedOutOfCollision, 0x6479B0, { .reversed = false });

    RH_ScopedVMTInstall(Clone, 0x649F50, { .reversed = false });
    RH_ScopedVMTInstall(GetTaskType, 0x647930);
    RH_ScopedVMTInstall(MakeAbortable, 0x647940);
    RH_ScopedVMTInstall(ProcessPed, 0x647D10, { .reversed = false });
}

CTaskSimpleCarSetPedOut::CTaskSimpleCarSetPedOut(CVehicle* targetVehicle, eTargetDoor nTargetDoor, bool bSwitchOffEngine, bool warpingOutOfCar) :
    m_nTargetDoor{ nTargetDoor },
    m_pTargetVehicle{ targetVehicle },
    m_bSwitchOffEngine{ bSwitchOffEngine },
    m_bWarpingOutOfCar{ warpingOutOfCar }
{
    CEntity::SafeRegisterRef(m_pTargetVehicle);
}

CTaskSimpleCarSetPedOut::~CTaskSimpleCarSetPedOut() {
    CEntity::SafeCleanUpRef(m_pTargetVehicle);
}

// 0x6479B0
void CTaskSimpleCarSetPedOut::PositionPedOutOfCollision(CPed* ped, CVehicle* veh, int32 door) {
    return plugin::Call<0x6479B0>(ped, veh, door);
}

CTask* CTaskSimpleCarSetPedOut::Clone() const {
    return plugin::CallMethodAndReturn<CTask*, 0x649F50, const CTask*>(this);
}

bool CTaskSimpleCarSetPedOut::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x647D10, CTask*, CPed*>(this, ped);
}
