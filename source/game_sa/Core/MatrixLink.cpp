/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CMatrixLink
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

#include "MatrixLink.h"

/**
 * Registra las operaciones que enlazan y desenlazan matrices dentro de las listas
 * intrusivas administradas por CMatrixLinkList.
 */
void CMatrixLink::InjectHooks()
{
    RH_ScopedClass(CMatrixLink);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(Insert, 0x54E8F0);
    RH_ScopedInstall(Remove, 0x54E910);
}

/**
 * Inserta @p pWhere inmediatamente después de este nodo.
 *
 * La lista es intrusiva: los enlaces anterior/siguiente pertenecen a la propia
 * CMatrixLink, por lo que no se crea ni destruye memoria durante la operación.
 *
 * @pre Esta matriz ya debe formar parte de una lista bien enlazada.
 */
void CMatrixLink::Insert(CMatrixLink* pWhere)
{
    pWhere->m_pNext = m_pNext;
    m_pNext->m_pPrev = pWhere;

    pWhere->m_pPrev = this;
    m_pNext = pWhere;
}

/**
 * Desconecta esta matriz de su lista actual reparando los enlaces de sus vecinos.
 *
 * Los punteros del propio nodo no se limpian; normalmente el llamador lo inserta
 * inmediatamente en otra de las listas de CMatrixLinkList.
 */
void CMatrixLink::Remove()
{
    m_pNext->m_pPrev = m_pPrev;
    m_pPrev->m_pNext = m_pNext;
}
