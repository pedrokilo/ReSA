/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWaitForBus
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
#include "TaskComplexWaitForBus.h"
#include "TaskSimpleWaitForBus.h"
#include "TaskComplexEnterCarAsPassenger.h"

void CTaskComplexWaitForBus::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWaitForBus, 0x86e18c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x632900);

    RH_ScopedInstall(Destructor, 0x632930);

    RH_ScopedVMTInstall(Clone, 0x636A60);
    RH_ScopedVMTInstall(GetTaskType, 0x632920);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x632940);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x638940);
    RH_ScopedVMTInstall(ControlSubTask, 0x632A40);
}

CTaskComplexWaitForBus::CTaskComplexWaitForBus(const CTaskComplexWaitForBus& o) :
    CTaskComplexWaitForBus{}
{
}

// 0x632940
CTask* CTaskComplexWaitForBus::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_WAIT_FOR_BUS: {
        m_bus = notsa::cast<CTaskSimpleWaitForBus>(m_pSubTask)->m_bus;
        return new CTaskComplexEnterCarAsPassenger{ m_bus, 0, false };
    }
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: {
        ped->m_bRemoveFromWorld = true;
        ped->bDontDragMeOutCar = true;


        auto* player = FindPlayerPed();
        if ((player && m_bus->IsPassenger(player)) || m_bus->IsDriver(player)) {
            player->GetPlayerData()->m_nBusFaresCollected += 5;
        }

        return nullptr; // Got on the bus, so task is finito
    }
    default:
        return nullptr;
    }
}

// 0x638940
CTask* CTaskComplexWaitForBus::CreateFirstSubTask(CPed* ped) {
    return new CTaskSimpleWaitForBus{};
}

CTask* CTaskComplexWaitForBus::Clone() const { return new CTaskComplexWaitForBus{ *this }; }
eTaskType CTaskComplexWaitForBus::GetTaskType() const { return Type; }
CTask* CTaskComplexWaitForBus::ControlSubTask(CPed* ped) { return m_pSubTask; }
