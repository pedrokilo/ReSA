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
    RH_ScopedInstall(ScalarDeletingDestructor, 0x64CD60); // lote 180A (sin validar)

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

// 0x64CD60 (lote 180A, sin validar; el listado decia CTaskSimpleCarFallOut): destructor con borrado de CTaskSimpleSetPedAsAutoDriver del EXE (ranura 0 de su vtable 0x86EFF4; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x648FC0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x648FC0, con hook), libera `this` con
// CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleSetPedAsAutoDriver* CTaskSimpleSetPedAsAutoDriver::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
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
