/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexDieInCar
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexDieInCar.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexLeaveCarAndDie.h"
#include "TaskSimpleCarDrive.h"
#include "TaskSimpleDieInCar.h"
#include "CarCtrl.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
}

namespace {
// 0x62FC80 (lote 181A, sin validar; el listado decia CTaskSimpleDieInCar): cuerpo del constructor de CTaskComplexDieInCar del EXE (thiscall, un argumento de pila, `ret 4`, 41 bytes: `push esi; mov esi,ecx; call 0x61A3B0;
// mov eax,[esp+8]; mov [esi+0Ch],eax; xor eax,eax; mov [esi],86DE04h; mov [esi+10h],eax; mov [esi+14h],eax; mov [esi+18h],al; mov [esi+19h],al; mov eax,esi; pop esi; ret 4`). Base CTaskComplex (0x61A3B0, con hook), arma (+0xC),
// vtable de la clase del EXE (0x86DE04), tiempo (+0x10) y desplazamiento (+0x14) a cero y los dos bytes +0x18 y +0x19 a cero; devuelve this. Clone (0x635F90) lo lleva expandido en linea; lo llaman ademas otros
// constructores del EXE (sin llamadores directos de call en este lote). Funcion propia (la clase usa el constructor generado por el compilador).
__declspec(noinline) CTaskComplexDieInCar* __fastcall CTaskComplexDieInCarConstructorBody(CTaskComplexDieInCar* self, void* /*edx*/, eWeaponType weaponType) {
    CTaskComplexBody::Constructor(self, nullptr);
    self->m_nWeaponType = weaponType;
    *reinterpret_cast<uint32*>(self) = 0x86DE04;
    self->m_nTimeMS           = 0;
    self->m_nOffset           = 0;
    self->m_bPreparedForDeath = false;
    self->m_bUpdateTime       = false;
    return self;
}

// 0x62FCF0 (lote 163A, banco Fn163A; thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],86DE04h; jmp 0x61A3D0`): cuerpo del destructor de CTaskComplexDieInCar del EXE. Deja la vtable de la clase
// del EXE (0x86DE04) y sigue con el cuerpo de ~CTaskComplex (0x61A3D0, con hook). Antes la DLL usaba el destructor generado de la clase (que deja la vtable de la DLL). Funcion propia sin expansion en linea
// para que el banco pueda sustituirla; la llama el destructor con borrado como el `call 0x62FCF0` del EXE.
__declspec(noinline) void __fastcall CTaskComplexDieInCarDestructorBody(CTaskComplexDieInCar* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86DE04;
    CTaskComplexBody::Destructor(self, nullptr);
}

// 0x6375D0 (lote 159A, banco Fn159A): destructor con borrado de CTaskComplexDieInCar del EXE (ranura 0 de su vtable; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x62FCF0; test byte [esp+8],1;
// je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0
// del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexDieInCar* __fastcall CTaskComplexDieInCarScalarDeletingDestructor(CTaskComplexDieInCar* self, void* /*edx*/, uint8 flags) {
    CTaskComplexDieInCarDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote159ABody_CTaskComplexDieInCar() { return reinterpret_cast<uintptr_t>(&CTaskComplexDieInCarDestructorBody); } // para el banco Fn159A

void CTaskComplexDieInCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDieInCar, 0x86DE04, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexDieInCarConstructorBody, "Constructor", 0x62FC80); // lote 181A (sin validar)
    RH_ScopedNamedGlobalInstall(CTaskComplexDieInCarDestructorBody, "DestructorBody", 0x62FCF0); // lote 163A (banco Fn163A)
    RH_ScopedVMTInstall(GetTaskType, 0x62FCB0); // lote 181A (sin validar): ranura 4
    RH_ScopedVMTInstall(Clone, 0x635F90);       // lote 181A (sin validar): ranura 1
    RH_ScopedNamedGlobalInstall(CTaskComplexDieInCarScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6375D0); // lote 159A (banco Fn159A)

    RH_ScopedVMTInstall(ControlSubTask, 0x6377B0);
    RH_ScopedInstall(CreateSubTask, 0x62FD50);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6375F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x637850);
    RH_ScopedVMTInstall(MakeAbortable, 0x62FCC0);
    RH_ScopedInstall(PreparePedVehicleForPedDeath, 0x62FD00);
}

// 0x62FC80
CTaskComplexDieInCar::CTaskComplexDieInCar(eWeaponType weaponType) : CTaskComplex() {
    m_nWeaponType       = weaponType;
    m_nTimeMS           = 0;
    m_nOffset           = 0;
    m_bPreparedForDeath = false;
    m_bUpdateTime       = false;
}

// 0x62FCB0 (lote 181A, sin validar; el listado decia CTaskSimpleDieInCar; thiscall, `ret`, 6 bytes: `mov eax,0D7h`): TASK_COMPLEX_DIE_IN_CAR (215); no lee el objeto (ranura 4 de la vtable 0x86DE04).
eTaskType CTaskComplexDieInCar::GetTaskType() const {
    return Type;
}

// 0x635F90 (lote 181A, sin validar; el listado decia CTaskSimpleDieInCar; ranura 1 de la vtable 0x86DE04; thiscall, `ret`, 124 bytes con marco SEH que solo desenrolla la reserva): reserva 0x1C bytes con CTask::operator new
// (0x61A5A0, pool de tareas) y, si no es nulo, construye con el constructor del EXE (0x62FC80, que el EXE lleva en linea) copiando el arma (+0xC, leida despues de la reserva); devuelve la tarea o nulo. Deja la vtable del EXE (0x86DE04).
CTask* CTaskComplexDieInCar::Clone() const {
    void* const memory = CTask::operator new(sizeof(CTaskComplexDieInCar));
    if (!memory) {
        return nullptr;
    }
    return CTaskComplexDieInCarConstructorBody(static_cast<CTaskComplexDieInCar*>(memory), nullptr, m_nWeaponType);
}

// 0x6377B0


CTask* CTaskComplexDieInCar::ControlSubTask(CPed* ped) {
    if (m_pSubTask->GetTaskType() != TASK_SIMPLE_CAR_DRIVE || !m_bPreparedForDeath)
        return m_pSubTask;

    if (m_bUpdateTime) {
        m_nTimeMS = CTimer::GetTimeInMS();
        m_bUpdateTime = false;
    }

    if (CTimer::GetTimeInMS() < m_nTimeMS + m_nOffset)
        return m_pSubTask;

    if (ped->GetCreatedBy() == PED_MISSION || ped->m_pVehicle->CanPedStepOutCar(false))
        return CreateSubTask(TASK_COMPLEX_LEAVE_CAR_AND_DIE, ped);

    return CreateSubTask(TASK_SIMPLE_DIE_IN_CAR, ped);
}

// 0x62FD50
CTask* CTaskComplexDieInCar::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_LEAVE_CAR_AND_DIE:
        return new CTaskComplexLeaveCarAndDie(ped->m_pVehicle, 0, 0); // Como el original: 0x62F1A0 (vtable de CTaskComplexLeaveCarAndDie)
    case TASK_SIMPLE_CAR_DRIVE:
        return new CTaskSimpleCarDrive(ped->m_pVehicle, nullptr, false);
    case TASK_SIMPLE_DIE_IN_CAR:
        return new CTaskSimpleDieInCar(ANIM_GROUP_DEFAULT, ped->m_pVehicle->IsDriver(ped) ? ANIM_ID_CAR_DEAD_LHS : ANIM_ID_CAR_DEAD_RHS);
    default:
        return nullptr;
    }
}

// 0x6375F0


CTask* CTaskComplexDieInCar::CreateFirstSubTask(CPed* ped) {
    assert(ped);
    ped->SetPedState(PEDSTATE_DIE);

    auto currentEvent = ped->GetEventHandlerHistory().GetCurrentEvent();
    if (currentEvent) {
        if (currentEvent->GetEventType() == EVENT_DAMAGE) {
            if (const auto driver = ped->m_pVehicle->m_pDriver) {
                if (driver != ped) {
                    auto* event = static_cast<CEventDamage*>(currentEvent->Clone());
                    event->m_bAddToEventGroup = false;
                    driver->GetEventGroup().Add(event, false);
                    delete event;
                }
            }

            for (const auto passenger : ped->m_pVehicle->GetPassengers()) {
                if (!passenger)
                    continue;

                if (passenger != ped) {
                    auto* event = static_cast<CEventDamage*>(currentEvent->Clone());
                    event->m_bAddToEventGroup = false;
                    passenger->GetEventGroup().Add(event, false);
                    delete event;
                }
            }
        }
    }

    if (ped->bForceDieInCar)
        return CreateSubTask(TASK_SIMPLE_DIE_IN_CAR, ped);

    if (m_nWeaponType == WEAPON_EXPLOSION)
        return CreateSubTask(TASK_SIMPLE_DIE_IN_CAR, ped);

    if (ped->m_pVehicle->IsSubPlane() || ped->m_pVehicle->IsSubHeli())
        return CreateSubTask(TASK_SIMPLE_DIE_IN_CAR, ped);

    if (ped->m_pVehicle->CanPedStepOutCar(false))
        return CreateSubTask(TASK_COMPLEX_LEAVE_CAR_AND_DIE, ped);

    if (ped->m_pVehicle->GetVehicleAppearance() != VEHICLE_APPEARANCE_AUTOMOBILE || ped->m_pVehicle->m_pDriver != ped)
        return CreateSubTask(TASK_SIMPLE_DIE_IN_CAR, ped);

    PreparePedVehicleForPedDeath(ped->m_pVehicle);
    m_nTimeMS = CTimer::GetTimeInMS();
    m_nOffset = 2000;
    m_bPreparedForDeath = true;

    return CreateSubTask(TASK_SIMPLE_CAR_DRIVE, ped);
}

// 0x62FD00
void CTaskComplexDieInCar::PreparePedVehicleForPedDeath(CVehicle *vehicle) {
    if (vehicle->GetStatus() == STATUS_SIMPLE) {
        CCarCtrl::SwitchVehicleToRealPhysics(vehicle);
    }
    const auto ap      = &vehicle->m_autoPilot;
    ap->SetCruiseSpeed(0);
    ap->m_nCarMission  = eCarMission::MISSION_NONE;
    ap->SetTempAction(TEMPACT_HANDBRAKESTRAIGHT, 2'000);
}

// 0x637850


CTask* CTaskComplexDieInCar::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_DIE_IN_CAR:
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_COMPLEX_LEAVE_CAR_AND_DIE:
        return CreateSubTask(ped->m_pVehicle && ped->bInVehicle ? TASK_SIMPLE_DIE_IN_CAR : TASK_FINISHED, ped);
    default:
        return nullptr;
    }
}

// 0x62FCC0

// 0x0
bool CTaskComplexDieInCar::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    switch (priority) {
    case ABORT_PRIORITY_URGENT:
    case ABORT_PRIORITY_IMMEDIATE:
        return m_pSubTask->MakeAbortable(ped, priority, event);
    default:
        return false;
    }
}
