/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CConversationNode
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "ConversationNode.h"
#include "Conversations.h"

static void __fastcall ConversationNodeClearRecursively(CConversationNode* node) {
    node->ClearRecursively();
}

// Como el original: 0x43A710 solo toca eax, ecx y esi; sus llamadores del EXE (CConversations::RemoveConversationForPed
// 0x43A960, CConversationForPed::Clear 0x43A770) conservan el puntero del slot en edx a través de la llamada.
static __declspec(naked) void ConversationNodeClearRecursivelyKeepEdx() {
    __asm {
        push edx
        call ConversationNodeClearRecursively
        pop  edx
        ret
    }
}

/** Registra la rutina reconstruida que limpia nodos de forma recursiva. */
void CConversationNode::InjectHooks() {
    RH_ScopedClass(CConversationNode);
    RH_ScopedCategory("Conversations");
    // Antes 0x43A7A0: caía dentro de una instrucción de esta misma función. Envoltorio que conserva edx.
    RH_ScopedNamedGlobalInstall(ConversationNodeClearRecursivelyKeepEdx, "ClearRecursively", 0x43A710);
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
