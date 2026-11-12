/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CNodeRoute
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
#include "NodeRoute.h"

void CNodeRoute::InjectHooks() {
    RH_ScopedClass(CNodeRoute);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(operator new, 0x41B860);
    RH_ScopedInstall(operator delete, 0x41B870);
    RH_ScopedInstall(Reverse, 0x667900); // lote 170A (banco Fn170A)
}

// 0x667900 (lote 170A, banco Fn170A; el listado lo atribuia a CTaskSimpleRunNamedAnim): thiscall, sin argumentos de pila, `ret`, 43 bytes. Como el original: ultimo = contador - 1 (con signo); si es > 0 intercambia las
// entradas (un dword cada una, copiado en crudo) i y ultimo mientras i < ultimo, subiendo i y bajando ultimo en cada vuelta. Con un contador mayor que las 8 entradas de la ruta recorre la memoria contigua, como el EXE.
void CNodeRoute::Reverse() {
    uint32* const entries = reinterpret_cast<uint32*>(m_Entries.data());
    int32 last = int32(m_NumEntries) - 1;
    for (int32 i = 0; i < last; i++, last--) {
        const uint32 tmp = entries[i];
        entries[i] = entries[last];
        entries[last] = tmp;
    }
}

void* CNodeRoute::operator new(uint32 size) {
    return GetNodeRoutePool()->New();
}

void CNodeRoute::operator delete(void* ptr, size_t sz) {
    GetNodeRoutePool()->Delete(reinterpret_cast<CNodeRoute*>(ptr));
}
