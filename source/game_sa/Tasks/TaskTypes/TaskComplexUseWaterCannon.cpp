/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseWaterCannon
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexUseWaterCannon.h"

#include "TaskSimpleCarDrive.h"
#include "TaskSimpleCarDriveTimed.h"

void CTaskComplexUseWaterCannon::InjectHooks()
{
    RH_ScopedVirtualClass(CTaskComplexUseWaterCannon, 0x86F5D0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x659780);
    //VTABLE
    RH_ScopedVMTInstall(Clone, 0x659D10);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6597D0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6597C0);
    RH_ScopedVMTInstall(ControlSubTask, 0x65A640);
}

CTaskComplexUseWaterCannon* CTaskComplexUseWaterCannon::Constructor(CFire* pFire)
{
    this->CTaskComplexUseWaterCannon::CTaskComplexUseWaterCannon(pFire);
    return this;
}

// 0x659780
CTaskComplexUseWaterCannon::CTaskComplexUseWaterCannon(CFire* pFire)
{
    m_pFire = pFire;
}

// 0x65A620
CTaskComplexUseWaterCannon::~CTaskComplexUseWaterCannon()
{
}

// 0x659D10
CTask* CTaskComplexUseWaterCannon::Clone() const {
    return new CTaskComplexUseWaterCannon(m_pFire);
}

// 0x6597C0
CTask* CTaskComplexUseWaterCannon::CreateNextSubTask(CPed* ped)
{
    return nullptr;
}

// 0x6597D0
CTask* CTaskComplexUseWaterCannon::CreateFirstSubTask(CPed* ped)
{
    return new CTaskSimpleCarDrive(ped->m_pVehicle, nullptr, false);
}

// 0x65A640
CTask* CTaskComplexUseWaterCannon::ControlSubTask(CPed* ped)
{
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_CAR_DRIVE)
    {
        if (ped->m_pVehicle->GetStatus() == STATUS_PHYSICS && m_pFire->IsActive())
            ped->m_pVehicle->AsAutomobile()->FireTruckControl(m_pFire);
        else
            return new CTaskSimpleCarDriveTimed(ped->m_pVehicle, 0);
    }

    return m_pSubTask;
}
