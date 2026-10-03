/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexLeaveAnyCar
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexLeaveAnyCar.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleNone.h"

void CTaskComplexLeaveAnyCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexLeaveAnyCar, 0x858EB0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x421150);
    RH_ScopedInstall(Destructor, 0x421270);
    RH_ScopedVMTInstall(Clone, 0x4211C0);
    RH_ScopedVMTInstall(GetTaskType, 0x421240);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x63BCD0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x63BCE0);
    RH_ScopedVMTInstall(ControlSubTask, 0x63BDC0);
}

// 0x421150
// Como el original: sin inicializar por valor la base; los bytes 0x12..0x13 quedan sin escribir
CTaskComplexLeaveAnyCar::CTaskComplexLeaveAnyCar(int32 delayTime, bool sensibleLeaveCar, bool forceGetOut) :
    m_sensibleLeaveCar{ sensibleLeaveCar },
    m_nDelayTime{ delayTime },
    m_forceGetOut{ forceGetOut }
{
}

// 0x63BCE0
CTask* CTaskComplexLeaveAnyCar::CreateFirstSubTask(CPed* ped) {
    if (ped->IsInVehicle()) {
        return new CTaskComplexLeaveCar(ped->m_pVehicle, TARGET_DOOR_FRONT_LEFT, m_nDelayTime, m_sensibleLeaveCar, m_forceGetOut);
    }
    return new CTaskSimpleNone();
}
