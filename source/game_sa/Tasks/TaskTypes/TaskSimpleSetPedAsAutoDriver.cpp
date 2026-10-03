/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleSetPedAsAutoDriver
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
#include "TaskSimpleSetPedAsAutoDriver.h"

void CTaskSimpleSetPedAsAutoDriver::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSetPedAsAutoDriver, 0x86eff4, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x648F40);
    RH_ScopedInstall(Destructor, 0x648FC0);

    RH_ScopedVMTInstall(Clone, 0x64A2F0);
    RH_ScopedVMTInstall(GetTaskType, 0x648FA0);
    RH_ScopedVMTInstall(MakeAbortable, 0x648FB0);
    RH_ScopedVMTInstall(ProcessPed, 0x649020);
}

// 0x648F40
CTaskSimpleSetPedAsAutoDriver::CTaskSimpleSetPedAsAutoDriver(CVehicle* veh) :
    m_veh{veh}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskSimpleSetPedAsAutoDriver::CTaskSimpleSetPedAsAutoDriver(const CTaskSimpleSetPedAsAutoDriver& o) :
    CTaskSimpleSetPedAsAutoDriver{o.m_veh}
{
}

// 0x648FC0
CTaskSimpleSetPedAsAutoDriver::~CTaskSimpleSetPedAsAutoDriver() {
    CEntity::SafeCleanUpRef(m_veh);
}

// 0x649020
bool CTaskSimpleSetPedAsAutoDriver::ProcessPed(CPed* ped) {
    CCarCtrl::JoinCarWithRoadSystem(m_veh);
    m_veh->SetStatus(STATUS_PHYSICS);
    m_veh->m_autoPilot.SetCarMission(MISSION_CRUISE);
    m_veh->m_autoPilot.SetCruiseSpeed(10);
    // Como el original: el motor queda encendido salvo que esté averiado
    m_veh->vehicleFlags.bEngineOn = !m_veh->vehicleFlags.bEngineBroken;
    return true;
}
