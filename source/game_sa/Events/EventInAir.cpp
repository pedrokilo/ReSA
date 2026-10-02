/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventInAir
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventInAir.h"


void CEventInAir::InjectHooks()
{
    RH_ScopedVirtualClass(CEventInAir, 0x85B3C0, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B0CB0);
    RH_ScopedVMTInstall(AffectsPed, 0x4B0C00);
}

// 0x4B0CB0
CEventInAir* CEventInAir::Constructor()
{
    this->CEventInAir::CEventInAir();
    return this;
}

// 0x4B0C00
bool CEventInAir::AffectsPed(CPed* ped)
{
    CTask* activeTask = ped->GetTaskManager().GetActiveTask();
    if (activeTask
        && (activeTask->GetTaskType() == TASK_COMPLEX_USE_SWAT_ROPE
            || activeTask->GetTaskType() == TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP
            ||
                       activeTask->GetTaskType() == TASK_COMPLEX_IN_AIR_AND_LAND))
    {
        return false;
    }
    CTask* simplestActiveTask = ped->GetTaskManager().GetSimplestActiveTask();
    if (simplestActiveTask && simplestActiveTask->GetTaskType() == TASK_SIMPLE_FALL ||
        ped->m_pDamageEntity ||
        !ped->GetUsesCollision() ||
        ped->m_pAttachedTo ||
        !CPedGeometryAnalyser::IsInAir(*ped)
    ) {
        return false;
    }
    return ped->IsAlive();
}
