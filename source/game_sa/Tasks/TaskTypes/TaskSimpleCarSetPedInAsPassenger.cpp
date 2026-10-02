/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarSetPedInAsPassenger
 * Reconstrucción: INCOMPLETA — 4/5 funciones reconstruidas.
 * Pendientes de reconstruir: 1.
 * Funciones bloqueadas: 1.
 * Validación registrada: 0/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   ProcessPed | 0x0064B5D0 | V
 * BLOQUEADAS (nombre | dirección | tipo):
 *   ProcessPed | 0x0064B5D0 | V
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleCarSetPedInAsPassenger.h"
#include "TaskUtilityLineUpPedWithCar.h"

void CTaskSimpleCarSetPedInAsPassenger::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarSetPedInAsPassenger, 0x86EE04, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    // El constructor original (0x646FE0) no se instala: la firma actual añade un argumento NOTSA.
    RH_ScopedInstall(Destructor, 0x647080);

    RH_ScopedVMTInstall(Clone, 0x649D90);
    RH_ScopedVMTInstall(GetTaskType, 0x647060);
    RH_ScopedVMTInstall(MakeAbortable, 0x647070);
    // Stub hacia el original: activarlo provocaría recursión infinita.
    RH_ScopedVMTInstall(ProcessPed, 0x64B5D0, { .reversed = false });
}

// OG constructor was at 0x646FE0
CTaskSimpleCarSetPedInAsPassenger::CTaskSimpleCarSetPedInAsPassenger(CVehicle* targetVehicle, eTargetDoor nTargetDoor, bool warpingInToCar, CTaskUtilityLineUpPedWithCar* utility) :
    m_nTargetDoor{ nTargetDoor },
    m_pTargetVehicle{ targetVehicle },
    m_pUtility{ utility },
    m_bWarpingInToCar{warpingInToCar}
{
    CEntity::SafeRegisterRef(m_pTargetVehicle);
}

// For 0x649D90
CTaskSimpleCarSetPedInAsPassenger::CTaskSimpleCarSetPedInAsPassenger(const CTaskSimpleCarSetPedInAsPassenger& o) :
    CTaskSimpleCarSetPedInAsPassenger{
        o.m_pTargetVehicle,
        o.m_nTargetDoor,
        o.m_bWarpingInToCar,
        o.m_pUtility
    }
{
    m_nNumGettingInToClear = o.m_nNumGettingInToClear;
}

// 0x647080
CTaskSimpleCarSetPedInAsPassenger::~CTaskSimpleCarSetPedInAsPassenger() {
    CEntity::SafeCleanUpRef(m_pTargetVehicle);
}

// 0x64B5D0
bool CTaskSimpleCarSetPedInAsPassenger::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x64B5D0, CTask*, CPed*>(this, ped);
}
