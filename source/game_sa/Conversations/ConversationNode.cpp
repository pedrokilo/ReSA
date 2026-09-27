#include "ConversationNode.h"
#include "Conversations.h"

/** Registra la rutina reconstruida que limpia nodos de forma recursiva. */
void CConversationNode::InjectHooks() {
    RH_ScopedClass(CConversationNode);
    RH_ScopedCategory("Conversations");
    RH_ScopedInstall(ClearRecursively, 0x43A7A0);
}

/**
 * Reinicia un solo nodo sin modificar los nodos referenciados por sus ramas.
 *
 * Asignar '\0' a m_Name[0] es importante: CConversations::DoneSettingUpConversation()
 * utiliza un nombre vacío para identificar entradas libres del pool global de nodos.
 */
void CConversationNode::Clear() {
    m_Name[0] = '\0';

    // -1 funciona como centinela para "sin sucesor" o final de esta rama.
    m_NodeYes = -1;
    m_NodeNo  = -1;

    // Elimina todos los identificadores de voz asociados al nodo.
    m_Speech  = 0;
    m_SpeechY = 0;
    m_SpeechN = 0;
}

// 0x43A710
/**
 * Libera todo el subárbol de conversación cuya raíz es este nodo.
 *
 * Las ramas se limpian antes que el nodo actual para que sus índices sigan
 * disponibles durante el recorrido recursivo.
 *
 * @note Se espera que los datos originales formen un árbol/grafo acíclico. Un
 * ciclo provocaría recursión indefinida porque esta rutina no mantiene un conjunto
 * de nodos ya visitados.
 */
void CConversationNode::ClearRecursively() {
    if (m_NodeYes >= 0) {
        CConversations::m_Nodes[m_NodeYes].ClearRecursively();
    }

    if (m_NodeNo >= 0) {
        CConversations::m_Nodes[m_NodeNo].ClearRecursively();
    }

    Clear();
}
