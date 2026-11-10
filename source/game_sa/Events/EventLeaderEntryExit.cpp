/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventLeaderEntryExit
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventLeaderEntryExit.h"


void CEventLeaderEntryExit::InjectHooks()
{
    RH_ScopedVirtualClass(CEventLeaderEntryExit, 0x859540, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x43E1C0);
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4B23A0);     // lote 144A (banco Fn144A): ranura 6 de la vtable 0x859540 (el listado la atribuia a CEventAreaCodes)
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4B2390);   // lote 144A: `mov al,1; ret 4` (verdadero siempre), ranura 11
}

// 0x43E1C0
CEventLeaderEntryExit* CEventLeaderEntryExit::Constructor(CPed* ped)
{
    this->CEventLeaderEntryExit::CEventLeaderEntryExit(ped);
    return this;
}

