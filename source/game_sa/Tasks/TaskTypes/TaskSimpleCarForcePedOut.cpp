#include "StdInc.h"

#include "TaskSimpleCarForcePedOut.h"

#include "Vehicle.h"
#include "Ped.h"
#include "CarEnterExit.h"

// 0x647710
CTaskSimpleCarForcePedOut::CTaskSimpleCarForcePedOut(CVehicle* vehicle, int32 door) :
    CTaskSimple() // 0x61A390
{
    m_pVehicle = vehicle; // Como el original: no pone a cero nada mas; registra la referencia solo si no es nula
    m_nDoor    = door;
    CEntity::SafeRegisterRef(m_pVehicle);
}

// 0x647790: como el original, limpia la referencia al vehiculo; el campo no se pone a nulo
CTaskSimpleCarForcePedOut::~CTaskSimpleCarForcePedOut() {
    CEntity::SafeCleanUpRef(m_pVehicle);
}

// 0x649EE0: reserva con CTask::operator new (0x61A5A0) y, si el pool esta agotado (nulo), devuelve nulo sin construir; sin inicializacion por valor
CTask* CTaskSimpleCarForcePedOut::Clone() const {
    void* const memory = CTask::operator new(sizeof(CTaskSimpleCarForcePedOut));
    if (!memory) {
        return nullptr;
    }
    return ::new (memory) CTaskSimpleCarForcePedOut(m_pVehicle, m_nDoor);
}

// 0x647780 (`xor al, al; ret 0Ch`)
bool CTaskSimpleCarForcePedOut::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return false;
}

static_assert(offsetof(CVehicle, m_pDriver) == 0x460 && offsetof(CVehicle, m_apPassengers) == 0x464 && offsetof(CVehicle, m_nGettingOutFlags) == 0x487);

// 0x6477F0 (`ret 4`): sin vehiculo termina; si el ped es el conductor, lo saca de la colision; si no, y la puerta opuesta (ComputeOppositeDoorFlag) esta siendo usada para salir, solo
// continua si no hay conductor ni pasajero en los asientos anteriores (ComputePassengerIndexFromCarDoor; la lectura de los asientos es cruda, sin limite de m_nMaxPassengers)
bool CTaskSimpleCarForcePedOut::ProcessPed(CPed* ped) {
    if (!m_pVehicle) {
        return true;
    }
    if (m_pVehicle->IsDriver(ped)) {
        ped->PositionPedOutOfCollision(m_nDoor, m_pVehicle, true);
        return true;
    }
    const int32 oppositeDoorFlag = CCarEnterExit::ComputeOppositeDoorFlag(m_pVehicle, m_nDoor, false);
    if (uint8(oppositeDoorFlag) & m_pVehicle->m_nGettingOutFlags) {
        if (m_pVehicle->m_pDriver) {
            return false;
        }
        const int32 seats = CCarEnterExit::ComputePassengerIndexFromCarDoor(m_pVehicle, m_nDoor);
        for (int32 i = 0; i < seats; i++) {
            if (m_pVehicle->m_apPassengers.data()[i]) {
                return false;
            }
        }
    }
    ped->PositionPedOutOfCollision(m_nDoor, m_pVehicle, true);
    return true;
}

void CTaskSimpleCarForcePedOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarForcePedOut, 0x86EE94, 9); // vtable verificado en gta_sa.exe (GetTaskType en [4]); N por confirmar
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x647710);
    RH_ScopedInstall(Destructor, 0x647790);
    RH_ScopedVMTInstall(Clone, 0x649EE0);
    RH_ScopedVMTInstall(MakeAbortable, 0x647780);
    RH_ScopedVMTInstall(ProcessPed, 0x6477F0);
}
