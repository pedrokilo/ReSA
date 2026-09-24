/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleClearLookAt
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleClearLookAt.h"
#include "Ragdoll/IKChainManager.h"

void CTaskSimpleClearLookAt::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleClearLookAt, 0x86E404, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x634670);
    RH_ScopedInstall(Destructor, 0x6346A0);

    RH_ScopedVMTInstall(Clone, 0x636F20);
    RH_ScopedVMTInstall(GetTaskType, 0x634690);
    RH_ScopedVMTInstall(MakeAbortable, 0x6346B0);
    RH_ScopedVMTInstall(ProcessPed, 0x6346C0);
}

// 0x6346C0
bool CTaskSimpleClearLookAt::ProcessPed(CPed* ped) {
    if (g_ikChainMan.IsLooking(ped)) {
        g_ikChainMan.AbortLookAt(ped, 500u);
    }
    return true;
}
