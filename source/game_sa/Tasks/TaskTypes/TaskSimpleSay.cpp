/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleSay
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleSay.h"

void CTaskSimpleSay::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSay, 0x85a1dc, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x48E360);
    RH_ScopedInstall(Destructor, 0x48E4E0);

    RH_ScopedVMTInstall(Clone, 0x48E390);
    RH_ScopedVMTInstall(GetTaskType, 0x48E420);
    RH_ScopedVMTInstall(MakeAbortable, 0x48E430);
    RH_ScopedVMTInstall(ProcessPed, 0x48E440);
}

// 0x48E360
CTaskSimpleSay::CTaskSimpleSay(eGlobalSpeechContext gctx, int32 sayDuration) :
    m_Duration{sayDuration},
    m_GlobalSpeechContext{gctx}
{
}

// 0x48E440
bool CTaskSimpleSay::ProcessPed(CPed* ped) {
    m_Timer.StartIfNotAlready(m_Duration);
    ped->Say(m_GlobalSpeechContext);
    return m_Timer.IsOutOfTime();
}
