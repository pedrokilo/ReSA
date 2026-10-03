/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleSetStayInSamePlace
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleSetStayInSamePlace.h"

void CTaskSimpleSetStayInSamePlace::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSetStayInSamePlace, 0x86DD50, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62F590);

    RH_ScopedVMTInstall(Clone, 0x635BD0);
    RH_ScopedVMTInstall(GetTaskType, 0x62F5B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x62F5C0);
    RH_ScopedVMTInstall(ProcessPed, 0x62F5E0);
}

// 0x62F590
CTaskSimpleSetStayInSamePlace::CTaskSimpleSetStayInSamePlace(bool bStayInSamePlace) : CTaskSimple() {
    m_bStayInSamePlace = bStayInSamePlace;
}

// 0x62F5E0
bool CTaskSimpleSetStayInSamePlace::ProcessPed(CPed* ped) {
    ped->bStayInSamePlace = m_bStayInSamePlace;
    return true;
}
