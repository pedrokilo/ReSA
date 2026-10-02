/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventScriptedAttractor
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
#include "EventScriptedAttractor.h"


void CEventScriptedAttractor::InjectHooks()
{
    RH_ScopedVirtualClass(CEventScriptedAttractor, 0x86C9F0, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x5FEF40);
}

// 0x5FEF40
CEventScriptedAttractor::CEventScriptedAttractor(C2dEffectPedAttractor* the2dEffect, CEntity* entity) :
    CEventAttractor(the2dEffect, entity, false)
{
}

// 0x5FEF40
// Como el original: dos argumentos (ret 8); con el tercero la DLL desequilibraba la pila de los llamadores del EXE
CEventScriptedAttractor* CEventScriptedAttractor::Constructor(C2dEffectPedAttractor* the2dEffect, CEntity* entity)
{
    this->CEventScriptedAttractor::CEventScriptedAttractor(the2dEffect, entity);
    return this;
}

