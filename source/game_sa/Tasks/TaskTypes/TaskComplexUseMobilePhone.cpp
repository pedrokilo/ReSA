/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseMobilePhone
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexUseMobilePhone.h"

void CTaskComplexUseMobilePhone::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseMobilePhone, 0x86E454, 11); // vtable verificado en gta_sa.exe (GetTaskType en [4])
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Quit, 0x634A40);
    // El constructor esta declarado pero no definido: no hay mas funciones reconstruidas todavia.
}

// 0x634A40
void CTaskComplexUseMobilePhone::Quit(CPed* ped) {
    // Como el original: el byte +0x1D (m_bQuit) se prueba entero y se marca antes de abortar; la ranura 6 de la vtable es MakeAbortable (prioridad 0 = leisure, sin evento)
    if (!m_bQuit) {
        m_bQuit = true;
        MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
    }
}
