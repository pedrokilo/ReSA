#pragma once

#include "EntryInfoNode.h"

/**
 * Lista intrusiva de CEntryInfoNode asociados a una entidad física.
 *
 * Cada nodo enlaza la entidad con estructuras espaciales del mundo. La lista sólo
 * es propietaria de sus CEntryInfoNode, cuyos bloques proceden del pool dedicado.
 */
class CEntryInfoList {
public:
    CEntryInfoNode* m_node; //!< Cabeza de la lista, o nullptr cuando está vacía.

public:
    static void InjectHooks();

    // Desenlaza y devuelve al pool todos los nodos.
    void Flush();                          // 0x536E10
    // Elimina un nodo concreto sin destruir los objetos a los que hace referencia.
    void DeleteNode(CEntryInfoNode* node); // Most likely inlined in final exe
    bool IsEmpty() const noexcept { return !m_node; }

    // inlined
    CEntryInfoNode* GetNodePtr() { return m_node; } // AKA = GetHeadPtr
};

VALIDATE_SIZE(CEntryInfoList, 0x4);
