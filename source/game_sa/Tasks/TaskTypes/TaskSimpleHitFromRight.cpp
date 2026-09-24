/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleHitFromRight
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
#include "TaskSimpleHitFromRight.h"

void CTaskSimpleHitFromRight::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitFromRight, 0x86dbd8, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62EDC0);

    RH_ScopedVMTInstall(Clone, 0x62EDF0);
    RH_ScopedVMTInstall(IsInterruptable, 0x62EE60);
}

// 0x62EDC0
CTaskSimpleHitFromRight::CTaskSimpleHitFromRight() :
    CTaskSimpleRunAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_HIT_R,
        4.0f,
        Type,
        "HitFromRight",
        false
    }
{
}

CTaskSimpleHitFromRight::CTaskSimpleHitFromRight(const CTaskSimpleHitFromRight& o) :
    CTaskSimpleHitFromRight{}
{
}
