/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventSeenCop
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
#include "EventSeenCop.h"


void CEventSeenCop::InjectHooks()
{
    RH_ScopedVirtualClass(CEventSeenCop, 0x86CB58, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor1, 0x5FF380);
    RH_ScopedVMTInstall(GetEventPriority, 0x5FF880);                                                       // lote 197A (sin validar): retorno de constante
}

// 0x5FF380
CEventSeenCop* CEventSeenCop::Constructor1(CPed* cop)
{
    this->CEventSeenCop::CEventSeenCop(cop);
    return this;
}

// 0x5FF880 (lote 197A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,15h`): prioridad 21; no lee el objeto (ranura 2 de la vtable 0x86CB58).
int32 CEventSeenCop::GetEventPriority() const {
    return 21;
}
