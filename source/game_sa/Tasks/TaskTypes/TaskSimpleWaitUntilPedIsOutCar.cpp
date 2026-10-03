/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleWaitUntilPedIsOutCar
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleWaitUntilPedIsOutCar.h"

void CTaskSimpleWaitUntilPedIsOutCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleWaitUntilPedIsOutCar, 0x86F094, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x649450);
    RH_ScopedInstall(Destructor, 0x6494F0);
    RH_ScopedVMTInstall(Clone, 0x64A4B0);
    RH_ScopedVMTInstall(GetTaskType, 0x6494D0);
    RH_ScopedVMTInstall(MakeAbortable, 0x6494E0);
    RH_ScopedVMTInstall(ProcessPed, 0x649550);
}

// 0x649450
CTaskSimpleWaitUntilPedIsOutCar::CTaskSimpleWaitUntilPedIsOutCar(CPed* ped, CVector const& pos) : CTaskSimple(),
    m_PedToWaitFor{ ped },
    m_Pos{ pos },
    m_bIsNotGoingTowardsPos{ true }
{
    CEntity::SafeRegisterRef(m_PedToWaitFor);
}

// 0x6494F0
CTaskSimpleWaitUntilPedIsOutCar::~CTaskSimpleWaitUntilPedIsOutCar() {
    CEntity::SafeCleanUpRef(m_PedToWaitFor);
}

// 0x649550

// 0x0
bool CTaskSimpleWaitUntilPedIsOutCar::ProcessPed(CPed* ped) {
    if (!m_PedToWaitFor) {
        return true;
    }

    if (m_bIsNotGoingTowardsPos) {
        m_bIsNotGoingTowardsPos = false;

        CTaskSimpleStandStill standStillTask;
        standStillTask.ProcessPed(ped);

        // Como el original: ángulo en radianes (0x53CBE0), no en grados
        ped->m_fAimingRotation = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(m_Pos.x, m_Pos.y, 0.f, 0.f));
    }

    return !m_PedToWaitFor->bInVehicle;
}

