/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleHitFromBack
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
#include "TaskSimpleHitFromBack.h"

void CTaskSimpleHitFromBack::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitFromBack, 0x86db70, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62EC60);

    RH_ScopedVMTInstall(Clone, 0x62EC90);
    RH_ScopedVMTInstall(IsInterruptable, 0x62ED00);
}

// 0x62EC60
CTaskSimpleHitFromBack::CTaskSimpleHitFromBack() :
    CTaskSimpleRunAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_HIT_BACK,
        4.0f,
        Type,
        "HitFromBack",
        false
    }
{
}

CTaskSimpleHitFromBack::CTaskSimpleHitFromBack(const CTaskSimpleHitFromBack& o) :
    CTaskSimpleHitFromBack{}
{
}
