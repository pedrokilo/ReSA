/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventHitByWaterCannon
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventHitByWaterCannon.h"


void CEventHitByWaterCannon::InjectHooks()
{
    RH_ScopedVirtualClass(CEventHitByWaterCannon, 0x85B488, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B1290);
    RH_ScopedVMTInstall(AffectsPed, 0x4B1330);
}

// 0x4B1290
CEventHitByWaterCannon::CEventHitByWaterCannon(const CVector& point, const CVector& moveSpeed)
{
    m_point = point;
    m_moveSpeed = moveSpeed;
}

// 0x4B1290
CEventHitByWaterCannon* CEventHitByWaterCannon::Constructor(const CVector& point, const CVector& moveSpeed)
{
    this->CEventHitByWaterCannon::CEventHitByWaterCannon(point, moveSpeed);
    return this;
}

// 0x4B1330
bool CEventHitByWaterCannon::AffectsPed(CPed* ped)
{
    if (ped->IsAlive()) {
        CTask* task = ped->GetTaskManager().GetActiveTask();
        if (!task || task->GetTaskType() != TASK_COMPLEX_FALL_AND_GET_UP)
            return true;
    }
    return false;
}