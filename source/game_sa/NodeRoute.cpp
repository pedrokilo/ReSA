/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CNodeRoute
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
#include "NodeRoute.h"

void CNodeRoute::InjectHooks() {
    RH_ScopedClass(CNodeRoute);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(operator new, 0x41B860);
    RH_ScopedInstall(operator delete, 0x41B870);
}

void* CNodeRoute::operator new(uint32 size) {
    return GetNodeRoutePool()->New();
}

void CNodeRoute::operator delete(void* ptr, size_t sz) {
    GetNodeRoutePool()->Delete(reinterpret_cast<CNodeRoute*>(ptr));
}
