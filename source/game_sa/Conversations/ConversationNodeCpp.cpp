// Compile separately: /O2 /Oi /arch:IA32 /fp:fast, ordinary /Zi and no RTC/PCH/unity.
// CConversationNode::Clear and ClearRecursively (0x43A710). The EXE routine touches only EAX, ECX and ESI,
// and its callers keep a pointer in EDX across the call, so the generated code must not write EDX.
#include <cstdint>
using int16 = int16_t;
using int32 = int32_t;
#define VALIDATE_SIZE(type, size) static_assert(sizeof(type) == size)
#include "ConversationNode.h"
#undef VALIDATE_SIZE

// CConversations::m_Nodes (0x969570); Conversations.h is not included to avoid the project-wide headers.
static CConversationNode* const Nodes = reinterpret_cast<CConversationNode*>(0x969570);

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
        Nodes[m_NodeYes].ClearRecursively();
    }

    if (m_NodeNo >= 0) {
        Nodes[m_NodeNo].ClearRecursively();
    }

    Clear();
}
