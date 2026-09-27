/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CConversationForPed
 * Reconstrucción: SIN RECONSTRUIR — 0/2 funciones reconstruidas.
 * Pendientes de reconstruir: 2.
 * Funciones bloqueadas: 2.
 * Evidencia aprobada vinculada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   IsPlayerInPositionForConversation | 0x0043AC40 | S
 *   Update | 0x0043C190 | S
 * BLOQUEADAS (nombre | dirección | tipo):
 *   IsPlayerInPositionForConversation | 0x0043AC40 | S
 *   Update | 0x0043C190 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "ConversationForPed.h"
#include "Conversations.h"

/**
 * Registra los hooks de CConversationForPed.
 *
 * Update() está marcado explícitamente como no reconstruido porque todavía
 * redirige la ejecución al ejecutable original. IsPlayerInPositionForConversation()
 * también actúa como un wrapper delgado alrededor de la función original.
 */
void CConversationForPed::InjectHooks() {
    RH_ScopedClass(CConversationForPed);
    RH_ScopedCategory("Conversations");
    RH_ScopedInstall(Update, 0x43C190, {.reversed = false});
    RH_ScopedInstall(IsPlayerInPositionForConversation, 0x43AC40, {.reversed = false});
}

/**
 * Libera el estado almacenado por una entrada del pool de conversaciones.
 *
 * El grafo de nodos pertenece al pool global CConversations::m_Nodes y no a este
 * objeto. Por ello, el llamador puede decidir si ese grafo también debe eliminarse.
 *
 * @param dontClearNodes
 *     Si es false, limpia recursivamente todos los nodos alcanzables desde m_FirstNode.
 *     Si es true, el pool de nodos se deja intacto.
 *
 * @note Si se solicita limpiar los nodos, m_FirstNode debe ser un índice válido.
 */
void CConversationForPed::Clear(bool dontClearNodes) {
    // Al eliminar normalmente la conversación de un peatón también se libera su
    // grafo. CConversations::Clear() pasa true porque después limpia por separado
    // todo el pool global de nodos.
    if (!dontClearNodes) {
        CConversations::m_Nodes[m_FirstNode].ClearRecursively();
    }

    // Reiniciar los índices hace que el slot deje de referenciar cualquier nodo.
    m_FirstNode   = -1;
    m_CurrentNode = -1;

    // nullptr también funciona como marca de slot libre para CConversations.
    m_pPed = nullptr;

    // Se descarta el historial temporal y de proximidad de la conversación.
    m_LastChange                = 0;
    m_LastTimeWeWereCloseEnough = 0;
}

// 0x43C190
/**
 * Ejecuta la máquina de estados original de la conversación en cada frame.
 *
 * Esta función todavía no ha sido reconstruida en C++. Mantener esta llamada al
 * ejecutable original conserva el comportamiento del juego hasta que se revierta.
 */
void CConversationForPed::Update() {
    plugin::CallMethod<0x43C190, CConversationForPed*>(this);
}

// 0x43AC40
/**
 * Utiliza la comprobación espacial original del ejecutable para esta conversación.
 *
 * @param randomConversation true si se prueban las reglas de conversación aleatoria.
 * @return Indica si el jugador está colocado lo bastante cerca/correctamente para conversar.
 */
bool CConversationForPed::IsPlayerInPositionForConversation(bool randomConversation) {
    return plugin::CallMethodAndReturn<bool, 0x43AC40, CConversationForPed*, bool>(this, randomConversation);
}
