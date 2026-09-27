#pragma once

class CPed;

/**
 * Estado de ejecución de una conversación guionizada asociada a un peatón concreto.
 *
 * Una conversación se representa mediante un grafo de objetos CConversationNode
 * almacenados en CConversations::m_Nodes. Este objeto no es propietario de ese
 * almacenamiento; únicamente conserva el índice del primer nodo y del nodo que
 * se está procesando actualmente.
 *
 * El juego mantiene un pool de tamaño fijo de estos objetos en
 * CConversations::m_Conversations.
 */
class CConversationForPed {
public:
    /**
     * Máquina de estados principal utilizada mientras una conversación está activa.
     *
     * INACTIVE:
     *     No hay ningún intercambio ejecutándose en este momento.
     *
     * PLAYER_SPEAKING:
     *     Se está reproduciendo la línea del jugador asociada a la transición actual.
     *
     * PED_SPEAKING:
     *     Se está reproduciendo la línea/pregunta del peatón.
     *
     * WAITINGFORINPUT:
     *     La conversación está esperando la respuesta positiva o negativa del jugador.
     *
     * El momento exacto de las transiciones se gestiona en Update(), que todavía
     * delega su ejecución al ejecutable original.
     */
    enum eStatus : uint32 {
        INACTIVE = 0,
        PLAYER_SPEAKING,
        PED_SPEAKING,
        WAITINGFORINPUT
    };

    int32   m_FirstNode;                 //!< Índice del nodo raíz en CConversations::m_Nodes, o -1 si no está en uso.
    int32   m_CurrentNode;               //!< Índice del nodo que se está procesando, o -1 si no hay ninguno.
    CPed*   m_pPed;                      //!< Peatón asociado a esta conversación. nullptr indica un slot libre del pool.
    uint32  m_LastChange;                //!< Tiempo del juego (ms) en el que cambió por última vez el estado o nodo.
    uint32  m_LastTimeWeWereCloseEnough; //!< Último tiempo del juego (ms) en el que se cumplieron los requisitos de proximidad.
    eStatus m_Status;                    //!< Estado actual de la conversación.
    bool    m_Enabled;                   //!< Indica si actualmente se permite que la conversación avance.
    bool    m_SuppressSubtitles;         //!< Indica si deben ocultarse los subtítulos de esta conversación.

    /** Registra los hooks de esta clase, tanto reconstruidos como delegados al juego original. */
    static void InjectHooks();

    /**
     * Reinicia este slot de conversación.
     *
     * @param dontClearNodes
     *     true  -> limpia únicamente el objeto de conversación y conserva intacto el grafo de nodos.
     *     false -> limpia recursivamente el grafo que comienza en m_FirstNode antes de reiniciar el slot.
     *
     * Cuando dontClearNodes es false, se espera que m_FirstNode contenga un índice
     * válido dentro del pool de nodos.
     */
    void Clear(bool dontClearNodes);

    /**
     * Avanza la máquina de estados de la conversación asociada al peatón.
     *
     * Esta implementación todavía no ha sido reconstruida y actualmente llama a
     * la función original del juego ubicada en 0x43C190.
     */
    void Update();

    /**
     * Comprueba si el jugador cumple las condiciones de posición/proximidad
     * necesarias para conversar con este peatón.
     *
     * @param randomConversation
     *     Selecciona las reglas alternativas de posicionamiento utilizadas por las
     *     conversaciones aleatorias del juego original.
     *
     * @return true si la comprobación de posicionamiento del juego original tiene éxito.
     *
     * La implementación actual delega en la función original ubicada en 0x43AC40.
     */
    bool IsPlayerInPositionForConversation(bool randomConversation);
};

// Debe conservar compatibilidad binaria con la estructura en memoria del juego original.
VALIDATE_SIZE(CConversationForPed, 0x1C);
