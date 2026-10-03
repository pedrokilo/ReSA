/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWanderFlee
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
#include "TaskComplexWanderFlee.h"

void CTaskComplexWanderFlee::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderFlee, 0x86f690, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65B320);
    RH_ScopedInstall(Destructor, 0x65B370);

    RH_ScopedVMTInstall(Clone, 0x65CD70);
    RH_ScopedVMTInstall(ControlSubTask, 0x65B380);
    RH_ScopedVMTInstall(GetWanderType, 0x65B350);
    RH_ScopedVMTInstall(ScanForStuff, 0x65B360);
}

// 0x65B320
CTaskComplexWanderFlee::CTaskComplexWanderFlee(eMoveState moveState, uint8 dir) :
    CTaskComplexWander{moveState, dir, false}
{
}

CTaskComplexWanderFlee::CTaskComplexWanderFlee(const CTaskComplexWanderFlee& o) :
    CTaskComplexWanderFlee{o.m_nMoveState, o.m_nDir}
{
}
