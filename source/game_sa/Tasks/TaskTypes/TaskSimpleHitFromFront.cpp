/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleHitFromFront
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleHitFromFront.h"

void CTaskSimpleHitFromFront::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitFromFront, 0x86e5ec, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x637250);

    RH_ScopedVMTInstall(Clone, 0x637290);
    RH_ScopedVMTInstall(IsInterruptable, 0x637280);
}

// 0x637250
CTaskSimpleHitFromFront::CTaskSimpleHitFromFront() :
    CTaskSimpleRunAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_HIT_FRONT,
        4.0f,
        Type,
        "HitFromFront",
        false
    }
{
}

CTaskSimpleHitFromFront::CTaskSimpleHitFromFront(const CTaskSimpleHitFromFront& o) :
    CTaskSimpleHitFromFront{}
{
}
