/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleUseAtm
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleUseAtm.h"


void CTaskSimpleUseAtm::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleUseAtm, 0x85a134, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x48DFE0);
    RH_ScopedInstall(Destructor, 0x48E960);

    RH_ScopedVMTInstall(Clone, 0x48E010);
    RH_ScopedVMTInstall(IsInterruptable, 0x48E080);
}

// 0x48DFE0
CTaskSimpleUseAtm::CTaskSimpleUseAtm() :
    CTaskSimpleRunAnim { ANIM_GROUP_DEFAULT, ANIM_ID_ATM, 4.f, Type, "UseAtm", false }
{
}

// NOTSA
CTaskSimpleUseAtm::CTaskSimpleUseAtm(const CTaskSimpleUseAtm&) :
    CTaskSimpleUseAtm{}
{
}
