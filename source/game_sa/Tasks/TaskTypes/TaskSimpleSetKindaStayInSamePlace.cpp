/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleSetKindaStayInSamePlace
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleSetKindaStayInSamePlace.h"

void CTaskSimpleSetKindaStayInSamePlace::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSetKindaStayInSamePlace, 0x86dd74, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62F610);
    RH_ScopedInstall(Destructor, 0x62F650);

    RH_ScopedVMTInstall(Clone, 0x635C30);
    RH_ScopedVMTInstall(GetTaskType, 0x62F630);
    RH_ScopedVMTInstall(MakeAbortable, 0x62F640);
    RH_ScopedVMTInstall(ProcessPed, 0x62F660);
}

// 0x62F610
CTaskSimpleSetKindaStayInSamePlace::CTaskSimpleSetKindaStayInSamePlace(bool bKindaStayInSamePlace) :
    m_bKindaStayInSamePlace{ bKindaStayInSamePlace }
{
}

// 0x635C30
CTaskSimpleSetKindaStayInSamePlace::CTaskSimpleSetKindaStayInSamePlace(const CTaskSimpleSetKindaStayInSamePlace& o) :
    CTaskSimpleSetKindaStayInSamePlace{ o.m_bKindaStayInSamePlace }
{
}
