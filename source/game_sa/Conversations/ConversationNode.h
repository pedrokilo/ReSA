#pragma once

/**
 * Representa un nodo de un grafo de conversación guionizada entre peatón y jugador.
 *
 * Los nodos viven en el pool global fijo CConversations::m_Nodes. Los dos miembros
 * de ramificación almacenan índices dentro de ese mismo pool, no punteros, lo que
 * permite elegir el siguiente nodo después de una respuesta positiva o negativa.
 */
class CConversationNode {
public:
    char  m_Name[8]; //!< Clave corta del nodo/script. Un primer byte vacío indica que el slot no está en uso.
    int16 m_NodeYes; //!< Índice del siguiente nodo para la rama positiva, o -1 si la rama termina.
    int16 m_NodeNo;  //!< Índice del siguiente nodo para la rama negativa, o -1 si la rama termina.
    int32 m_Speech;  //!< ID de audio/evento para la línea principal o pregunta del nodo.
    int32 m_SpeechY; //!< ID de audio/evento utilizado por la ruta de respuesta positiva ("sí").
    int32 m_SpeechN; //!< ID de audio/evento utilizado por la ruta de respuesta negativa ("no").

    /** Registra el hook reconstruido de ClearRecursively(). */
    static void InjectHooks();

    /** Marca este nodo como libre y limpia sus metadatos de ramas y audio. */
    void Clear();

    /**
     * Limpia recursivamente todos los nodos alcanzables mediante m_NodeYes y
     * m_NodeNo, y finalmente limpia este propio nodo.
     *
     * La rutina supone que el grafo de conversación es acíclico, como se espera
     * de los datos de conversación guionizados del juego original.
     */
    void ClearRecursively();
};

// Debe coincidir exactamente con la disposición binaria de la estructura original.
VALIDATE_SIZE(CConversationNode, 0x18);
