/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEntryInfoList
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EntryInfoList.h"

void CEntryInfoList::InjectHooks()
{
    RH_ScopedClass(CEntryInfoList);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(Flush, 0x536E10);
}

void CEntryInfoList::Flush()
{
    CEntryInfoNode* curNode = m_node;
    while (curNode) {
        auto nextNode = curNode->m_next;
        CEntryInfoList::DeleteNode(curNode);
        curNode = nextNode;
    }
}

// 0x?
void CEntryInfoList::DeleteNode(CEntryInfoNode* node)
{
    if (m_node == node)
        m_node = node->m_next;

    if (node->m_previous)
        node->m_previous->m_next = node->m_next;

    if (node->m_next)
        node->m_next->m_previous = node->m_previous;

    delete node;
}
