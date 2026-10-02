/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseEffectSprinting
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexUseEffectSprinting.h"

void CTaskComplexUseEffectSprinting::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseEffectSprinting, 0x86e5c0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(Clone, 0x6368A0);
    RH_ScopedVMTInstall(GetTaskType, 0x636890);
}

// 0x636780
CTaskComplexUseEffectSprinting::CTaskComplexUseEffectSprinting(C2dEffectPedAttractor* fx, CEntity* e) :
    CTaskComplexUseEffect{ fx, e }
{
    m_MoveState = PEDMOVE_RUN;
}

// 0x6367C0
CTaskComplexUseEffectSprinting::CTaskComplexUseEffectSprinting(const CTaskComplexUseEffectSprinting& o) :
    CTaskComplexUseEffectSprinting{ o.m_2dFx, o.m_Entity }
{
}
