/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexCarDriveWander
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

#include "TaskComplexDriveWander.h"
#include "CarCtrl.h"

namespace {
// 0x63CB50 (lote 140A, banco Fn140A; con hook desde el lote 148A, banco Fn148A; thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],0x86E96C; jmp 0x63CA40 (~CTaskComplexCarDrive, con hook, envoltura Destructor())`):
// cuerpo no virtual de ~CTaskComplexCarDriveWander del EXE. Deja la vtable de la clase del EXE (0x86E96C) y sigue con ese cuerpo. Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama
// ScalarDeletingDestructor como el `call 0x63CB50` del EXE.
__declspec(noinline) void __fastcall CarDriveWanderDestructorBody(CTaskComplexCarDriveWander* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86E96C;
    static_cast<CTaskComplexCarDrive*>(self)->Destructor();
}
} // namespace

uintptr_t Lote140ACarDriveWanderDestructorBody() { return reinterpret_cast<uintptr_t>(&CarDriveWanderDestructorBody); } // para el banco Fn140A

// 0x643220 (lote 140A, banco Fn140A) : destructor con borrado de CTaskComplexCarDriveWander del EXE (ranura 0 de su vtable 0x86E96C; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x63CB50; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x63CB50, funcion propia), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskComplexCarDriveWander* CTaskComplexCarDriveWander::ScalarDeletingDestructor(uint8 flags) {
    CarDriveWanderDestructorBody(this, nullptr);
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}

void CTaskComplexCarDriveWander::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCarDriveWander, 0x86E96C, 14);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63CB10);
    RH_ScopedNamedGlobalInstall(CarDriveWanderDestructorBody, "Destructor", 0x63CB50); // lote 148A (banco Fn148A)
    RH_ScopedInstall(ScalarDeletingDestructor, 0x643220); // lote 140A (banco Fn140A)

    RH_ScopedVMTInstall(Clone, 0x63DD00);
    RH_ScopedVMTInstall(GetTaskType, 0x63CB40);
    RH_ScopedVMTInstall(SetUpCar, 0x63CB60);
    RH_ScopedVMTInstall(CreateSubTaskCannotGetInCar, 0x643240);
}

// 0x63CB10
CTaskComplexCarDriveWander::CTaskComplexCarDriveWander(CVehicle* vehicle, eCarDrivingStyle carDrivingStyle, float fSpeed)
    : CTaskComplexCarDrive(vehicle, fSpeed, MODEL_INVALID, carDrivingStyle)
{
    // NOP
}

// 0x63CB60
void CTaskComplexCarDriveWander::SetUpCar() {
    m_OriginalDrivingStyle = m_Veh->m_autoPilot.m_nCarDrivingStyle;
    m_OriginalMission      = m_Veh->m_autoPilot.m_nCarMission;
    m_OriginalSpeed        = m_Veh->m_autoPilot.m_nCruiseSpeed;
    m_bIsCarSetUp          = true;
    if (!CCarCtrl::bCarIsBeingCreated) {
        CCarCtrl::JoinCarWithRoadSystem(m_Veh);
        m_Veh->SetStatus(STATUS_PHYSICS);
        m_Veh->m_autoPilot.SetCarMission(MISSION_CRUISE);
        m_Veh->m_autoPilot.m_nCruiseSpeed = (uint8)(int64)m_CruiseSpeed; // Como el original: _ftol2 (truncado a 64 bits), byte bajo
        m_Veh->m_autoPilot.m_speed = m_Veh->m_autoPilot.m_nCruiseSpeed;
        m_Veh->m_autoPilot.m_nCarDrivingStyle = static_cast<eCarDrivingStyle>(m_CarDrivingStyle);
    }
    if (m_Veh->vehicleFlags.bEngineBroken) {
        m_Veh->vehicleFlags.bEngineOn = false; // Como el original: solo and cl,0EFh (no toca bIsLawEnforcer)
    } else {
        m_Veh->vehicleFlags.bEngineOn = true;
    }
    m_Veh->m_autoPilot.m_nTimeToStartMission = CTimer::GetTimeInMS();
}

// 0x643240
CTask* CTaskComplexCarDriveWander::CreateSubTaskCannotGetInCar(CPed* ped) {
    return CTaskComplexCarDrive::CreateSubTask(TASK_COMPLEX_WANDER, ped);
}
