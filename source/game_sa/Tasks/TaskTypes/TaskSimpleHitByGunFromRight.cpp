/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleHitByGunFromRight
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleHitByGunFromRight.h"

void CTaskSimpleHitByGunFromRight::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitByGunFromRight, 0x86dcc4, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62F080);

    RH_ScopedVMTInstall(Clone, 0x62F0B0);
    RH_ScopedVMTInstall(IsInterruptable, 0x62F120);
}

// 0x62F080
CTaskSimpleHitByGunFromRight::CTaskSimpleHitByGunFromRight() :
    CTaskSimpleRunAnim::CTaskSimpleRunAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_SHOT_RIGHTP,
        4.0f,
        Type,
        "HitByGunFromRight",
        false
    }
{
}

CTaskSimpleHitByGunFromRight::CTaskSimpleHitByGunFromRight(const CTaskSimpleHitByGunFromRight&) :
    CTaskSimpleHitByGunFromRight{}
{
}
