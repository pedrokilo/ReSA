/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleSetCharIgnoreWeaponRangeFlag
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
#include "TaskSimpleSetCharIgnoreWeaponRangeFlag.h"

void CTaskSimpleSetCharIgnoreWeaponRangeFlag::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSetCharIgnoreWeaponRangeFlag, 0x859ebc, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x474620);
    RH_ScopedInstall(Destructor, 0x4748F0);

    RH_ScopedVMTInstall(Clone, 0x474640);
    RH_ScopedVMTInstall(GetTaskType, 0x4746A0);
    RH_ScopedVMTInstall(MakeAbortable, 0x4746B0);
    RH_ScopedVMTInstall(ProcessPed, 0x622B50);
}

// 0x474620
CTaskSimpleSetCharIgnoreWeaponRangeFlag::CTaskSimpleSetCharIgnoreWeaponRangeFlag(bool bIgnoreWeaponRange) :
    m_bIgnoreWeaponRange{ bIgnoreWeaponRange } {
}

// 0x474640
CTaskSimpleSetCharIgnoreWeaponRangeFlag::CTaskSimpleSetCharIgnoreWeaponRangeFlag(const CTaskSimpleSetCharIgnoreWeaponRangeFlag& o) :
    CTaskSimpleSetCharIgnoreWeaponRangeFlag{ o.m_bIgnoreWeaponRange } {
}
