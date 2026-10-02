/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleLookAbout
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
#include "TaskSimpleLookAbout.h"

void CTaskSimpleLookAbout::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleLookAbout, 0x0085A164, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x48E0A0);

    RH_ScopedVMTInstall(Clone, 0x48E0F0);
    RH_ScopedVMTInstall(IsInterruptable, 0x48E160);
}

// 0x48E0A0
CTaskSimpleLookAbout::CTaskSimpleLookAbout(uint32 durationMs) :
    CTaskSimpleRunTimedAnim{
        ANIM_GROUP_DEFAULT,
        ANIM_ID_IDLE_HBHB_1,
        4.0f,
        -4.0f,
        durationMs,
        Type,
        "LookAbout",
        false
    }
{
}

CTaskSimpleLookAbout::CTaskSimpleLookAbout(const CTaskSimpleLookAbout& o) :
    CTaskSimpleLookAbout{o.m_durationMs}
{
}
