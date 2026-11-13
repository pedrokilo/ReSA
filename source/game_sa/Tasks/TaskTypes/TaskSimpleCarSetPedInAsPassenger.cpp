/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarSetPedInAsPassenger
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

#include "TaskSimpleCarSetPedInAsPassenger.h"
#include "TaskUtilityLineUpPedWithCar.h"
#include "TaskSimpleCarSetPedOut.h"
#include "TaskSimpleCarDrive.h"
#include "CarEnterExit.h"
#include "Crime.h"

namespace CTaskSimpleBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A390
}

namespace {
constexpr uint32 kSetPedInAsPassengerVTable = 0x86EE04;

// 0x646FE0 (lote 176A, validado con el banco Fn176A; el listado la atribuia a CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse: la clase sale de la vtable 0x86EE04): el constructor del EXE llama a CTaskSimple::CTaskSimple (0x61A390), deja
// vtable propia, m_bIsFinished (+8) y m_pAnim (+0xC) a cero, m_pTargetVehicle (+0x10), m_nTargetDoor (+0x14), m_pUtility (+0x18) y a cero m_bWarpingInToCar (+0x1C), m_nDoorFlagsToClear (+0x1D) y m_nNumGettingInToClear (+0x1E);
// por ultimo registra la referencia al vehiculo (CEntity::RegisterReference 0x571B70) solo si no es nulo. thiscall, tres argumentos de pila, `ret 0xC`; devuelve `this`.
// Como el original, en este orden: base CTaskSimple (0x61A390, con hook), puerta (+14h), vtable del EXE (0x86EE04: la de la DLL no vale para los objetos que construyen los 9 llamadores del EXE), +8 y +0Ch a cero, vehiculo (+10h),
// utilidad (+18h) y los tres bytes de estado a cero (+1Ch es el argumento NOTSA `warpingInToCar`, siempre falso aqui); por ultimo registra la referencia al vehiculo solo si no es nulo (0x571B70). Funcion propia sin expansion en linea.
__declspec(noinline) CTaskSimpleCarSetPedInAsPassenger* __fastcall SetPedInAsPassengerConstructorBody(CTaskSimpleCarSetPedInAsPassenger* self, void* /*edx*/, CVehicle* targetVehicle, eTargetDoor targetDoor, CTaskUtilityLineUpPedWithCar* utility) {
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    CTaskSimpleBody::Constructor(self, nullptr);
    self->m_nTargetDoor = targetDoor;
    *reinterpret_cast<uint32*>(bytes) = kSetPedInAsPassengerVTable;
    bytes[0x8] = 0;
    *reinterpret_cast<uint32*>(bytes + 0xC) = 0;
    self->m_pTargetVehicle = targetVehicle;
    self->m_pUtility = utility;
    bytes[0x1C] = 0;
    bytes[0x1D] = 0;
    bytes[0x1E] = 0;
    CEntity::SafeRegisterRef(self->m_pTargetVehicle);
    return self;
}
} // namespace

void CTaskSimpleCarSetPedInAsPassenger::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarSetPedInAsPassenger, 0x86EE04, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    // Lote 176A (sin validar): el constructor original (0x646FE0) recibe (vehiculo, puerta, utilidad); la firma de la DLL añade el argumento NOTSA `warpingInToCar`, que la envoltura deja a falso.
    RH_ScopedNamedGlobalInstall(SetPedInAsPassengerConstructorBody, "Constructor", 0x646FE0);
    RH_ScopedInstall(Destructor, 0x647080);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x64B5B0);

    RH_ScopedVMTInstall(Clone, 0x649D90);
    RH_ScopedVMTInstall(GetTaskType, 0x647060);
    RH_ScopedVMTInstall(MakeAbortable, 0x647070);
    RH_ScopedVMTInstall(ProcessPed, 0x64B5D0);
}

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
    // Como el original (0x649D90): tras el constructor copia los tres bytes de estado (aviso de puerta, puertas y entrantes).
    m_nDoorFlagsToClear    = o.m_nDoorFlagsToClear;
    m_nNumGettingInToClear = o.m_nNumGettingInToClear;
}

// 0x647080
CTaskSimpleCarSetPedInAsPassenger::~CTaskSimpleCarSetPedInAsPassenger() {
    CEntity::SafeCleanUpRef(m_pTargetVehicle);
}

// 0x64B5D0
bool CTaskSimpleCarSetPedInAsPassenger::ProcessPed(CPed* ped) {
    // Como el original: llamada virtual (GetTaskType) a la tarea padre con el resultado descartado.
    if (ped->bInVehicle && GetParent()) {
        GetParent()->GetTaskType();
    }

    CEntity::SafeCleanUpRef(ped->m_pVehicle);
    ped->m_pVehicle = m_pTargetVehicle;
    CEntity::RegisterReference(ped->m_pVehicle);

    ped->m_fAimingRotation = ped->m_fCurrentRotation;
    ped->bInVehicle = true;

    if (ped->IsPlayer()) {
        ped->GetPlayerData()->m_bPlayersGangActive = true;
        ped->AsPlayer()->ClearAdrenaline();
    }

    if (ped->IsPlayer() && !m_pTargetVehicle->vehicleFlags.bHasBeenOwnedByPlayer) {
        m_pTargetVehicle->vehicleFlags.bHasBeenOwnedByPlayer = true;
        CCrime::ReportCrime(CRIME_CAR_STEAL, m_pTargetVehicle, FindPlayerPed(-1));
    }

    int32 door = -1;
    if (!m_pTargetVehicle->vehicleFlags.bIsBus) {
        door = CCarEnterExit::ComputePassengerIndexFromCarDoor(m_pTargetVehicle, m_nTargetDoor);
        if (door != -1) {
            // Como el original: se vuelve a leer el ocupante de ese asiento en cada comprobación (sin comprobar el rango).
            const auto Occupant = [&] { return m_pTargetVehicle->m_apPassengers.data()[door]; };
            if (Occupant() != ped && Occupant()) {
                if (!Occupant()->GetIntelligence()->m_TaskMgr.FindActiveTaskByType(TASK_COMPLEX_LEAVE_CAR)
                    && !Occupant()->GetIntelligence()->m_TaskMgr.FindActiveTaskByType(TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT_AND_STAND_UP)
                    && !Occupant()->GetIntelligence()->m_TaskMgr.FindActiveTaskByType(TASK_COMPLEX_LEAVE_CAR_AND_DIE)
                    && !Occupant()->GetIntelligence()->m_TaskMgr.FindActiveTaskByType(TASK_SIMPLE_BIKE_JACKED)
                ) {
                    CTaskSimpleCarSetPedOut task{
                        m_pTargetVehicle,
                        static_cast<eTargetDoor>(CCarEnterExit::ComputeTargetDoorToExit(m_pTargetVehicle, Occupant())),
                        true,
                        true
                    };
                    task.ProcessPed(Occupant());
                }
            }
        }
    }

    if (m_bWarpingInToCar) {
        const CVector pos = m_pTargetVehicle->GetPosition();
        ped->GetPosition() = pos;
    }

    if (door != -1) {
        m_pTargetVehicle->AddPassenger(ped, static_cast<uint8>(door));
    } else {
        m_pTargetVehicle->AddPassenger(ped);
    }

    ped->UpdateStatEnteringVehicle();
    ped->SetMoveState(PEDMOVE_NONE);
    ped->SetMoveAnim();
    ped->m_bUsesCollision = false;

    if (m_pTargetVehicle->m_nAlarmState == 0xFFFF) {
        m_pTargetVehicle->m_nAlarmState = 0x3A98;
    }

    ped->SetPedState(PEDSTATE_DRIVING);
    if (m_nDoorFlagsToClear) {
        m_pTargetVehicle->ClearGettingInFlags(m_nDoorFlagsToClear);
    }
    if (m_nNumGettingInToClear) {
        m_pTargetVehicle->m_nNumGettingIn -= m_nNumGettingInToClear;
    }
    ped->RemoveWeaponWhenEnteringVehicle(0);
    ped->bRenderPedInCar = !m_pTargetVehicle->vehicleFlags.bIsBus;

    CCarEnterExit::RemoveGetInAnims(ped);
    CCarEnterExit::AddInCarAnim(m_pTargetVehicle, ped, false);

    if (!m_bWarpingInToCar) {
        m_pUtility->ProcessPed(ped, m_pTargetVehicle, nullptr);
    }

    ped->GetIntelligence()->m_TaskMgr.SetTask(new CTaskSimpleCarDrive(m_pTargetVehicle, m_pUtility, false), TASK_PRIMARY_DEFAULT, false);
    return true;
}

// 0x64B5B0 (lote 176A, sin validar; el listado la atribuia a CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse): destructor con borrado de CTaskSimpleCarSetPedInAsPassenger del EXE (ranura 0 de su vtable 0x86EE04; thiscall,
// un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x647080; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la
// clase (0x647080, con hook), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleCarSetPedInAsPassenger* CTaskSimpleCarSetPedInAsPassenger::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
