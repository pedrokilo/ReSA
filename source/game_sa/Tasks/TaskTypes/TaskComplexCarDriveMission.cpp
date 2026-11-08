/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexCarDriveMission
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CTaskComplexCarDriveMissionFleeScene
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CTaskComplexCarDriveMissionKillPed
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexCarDriveMission.h"
#include "CarCtrl.h"

void CTaskComplexCarDriveMission::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCarDriveMission, 0x86E9A4, 14);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63CC30);
    RH_ScopedInstall(Destructor, 0x63CCC0);
    RH_ScopedVMTInstall(Clone, 0x63DD70);
    RH_ScopedVMTInstall(GetTaskType, 0x63CCB0);
    RH_ScopedVMTInstall(SetUpCar, 0x63CD20);
}

void CTaskComplexCarDriveMissionFleeScene::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCarDriveMissionFleeScene, 0x85B95C, 14);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x4B8920);
    RH_ScopedInstall(Destructor, 0x4B89F0);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x4B89D0);
    RH_ScopedVMTInstall(Clone, 0x4B8950);
    RH_ScopedVMTInstall(GetTaskType, 0x4B89C0);
}

void CTaskComplexCarDriveMissionKillPed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCarDriveMissionKillPed, 0x85B994, 14);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x4B8A00);
    RH_ScopedInstall(Destructor, 0x4B8AD0);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x4B8AB0);
    RH_ScopedVMTInstall(Clone, 0x4B8A30);
    RH_ScopedVMTInstall(GetTaskType, 0x4B8AA0);
}

// 0x63CC30
CTaskComplexCarDriveMission::CTaskComplexCarDriveMission(CVehicle* vehicle, CEntity* targetVehicle, eCarMission carDriveMission, eCarDrivingStyle carDrivingStyle, float fSpeed) :
    CTaskComplexCarDrive{ vehicle, fSpeed, MODEL_INVALID, carDrivingStyle }
{
    // assert(targetVehicle->GetIsTypeVehicle());
    m_pTargetVehicle = static_cast<CVehicle*>(targetVehicle);
    m_nCarMission    = carDriveMission;
    CEntity::SafeRegisterRef(m_pTargetVehicle);
}

// 0x63CCC0
CTaskComplexCarDriveMission::~CTaskComplexCarDriveMission() {
    CEntity::SafeCleanUpRef(m_pTargetVehicle);
}

// 0x63CD20
void CTaskComplexCarDriveMission::SetUpCar() {
    CTaskComplexCarDrive::SetUpCar();

    CCarCtrl::JoinCarWithRoadSystem(m_Veh);
    m_Veh->SetStatus(STATUS_PHYSICS);

    auto& autopilot = m_Veh->m_autoPilot;
    autopilot.m_nCruiseSpeed        = (uint8)(int64)m_CruiseSpeed; // Como el original: _ftol2 (truncado a 64 bits), byte bajo
    autopilot.m_speed               = (float)autopilot.m_nCruiseSpeed;
    autopilot.m_nCarDrivingStyle    = (eCarDrivingStyle)m_CarDrivingStyle;
    autopilot.m_TargetEntity          = m_pTargetVehicle;
    autopilot.SetCarMission(m_nCarMission, 0);
    CEntity::SafeRegisterRef(autopilot.m_TargetEntity);

    m_Veh->vehicleFlags.bEngineOn = m_Veh->vehicleFlags.bEngineBroken ? false : true;
}

// 0x4B89D0 (lote 125A, banco Fn125A) : destructor con borrado de CTaskComplexCarDriveMissionFleeScene del EXE (ranura 0 de su vtable 0x85B95C; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x4B89F0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x4B89F0; aqui su envoltura Destructor()), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. 
CTaskComplexCarDriveMissionFleeScene* CTaskComplexCarDriveMissionFleeScene::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}

// 0x4B8AB0 (lote 125A, banco Fn125A) : destructor con borrado de CTaskComplexCarDriveMissionKillPed del EXE (ranura 0 de su vtable 0x85B994; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x4B8AD0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x4B8AD0; aqui su envoltura Destructor()), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. 
CTaskComplexCarDriveMissionKillPed* CTaskComplexCarDriveMissionKillPed::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
