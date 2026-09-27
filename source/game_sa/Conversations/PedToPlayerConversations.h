#pragma once

class CPed;

/**
 * Gestor de conversaciones ambientales/directas iniciadas entre un peatón y el jugador.
 *
 * A diferencia de CConversationForPed, este subsistema utiliza un único conjunto
 * de variables globales de estado en vez de un pool de grafos de nodos. Update()
 * todavía se ejecuta mediante la rutina original del juego, por lo que los estados
 * se documentan a partir de sus nombres y del comportamiento visible en el código
 * de limpieza ya reconstruido.
 */
class CPedToPlayerConversations {
public:
    /** Máquina de estados utilizada por la rutina original de conversación peatón-jugador. */
    enum class eP2pState : uint32 {
        INACTIVE = 0,       //!< No hay ningún intercambio peatón-jugador activo.
        PEDHASOPENED,       //!< Un peatón ha iniciado/abierto un intercambio.
        WAITINGFORFINALWORD,//!< Se está esperando la etapa o frase final de cierre.
        WAITINGTOFINISH,    //!< Se está esperando a que termine el intercambio/audio activo.
    };

    // Estas referencias apuntan directamente a las variables globales del juego original.
    static inline auto& m_State                        = StaticRef<eP2pState>(0x969A20); //!< Estado actual de la máquina de estados.
    static inline auto& m_pPed                         = StaticRef<CPed*>(0x9691C0);     //!< Peatón implicado actualmente en el intercambio.
    static inline auto& m_Topic                        = StaticRef<int32>(0x9691BC);     //!< Selector de tema/voz utilizado por el Update() original.
    static inline auto& m_TimeOfLastPlayerConversation = StaticRef<uint32>(0x9691B4);  //!< Marca temporal usada para limitar/registrar conversaciones recientes.
    static inline auto& m_StartTime                    = StaticRef<uint32>(0x9691B8);   //!< Marca temporal en la que comenzó el intercambio actual.
    static inline auto& m_bPositiveReply               = StaticRef<bool>(0x9691B0);     //!< Existe en memoria original; actualmente no se usa en el código reconstruido.
    static inline auto& m_bPositiveOpening             = StaticRef<bool>(0x9691B1);     //!< Indica si la apertura seleccionada por la rutina original es positiva.

    /** Registra los hooks del subsistema de conversación peatón-jugador. */
    static void InjectHooks();

    /** Reinicia el estado del subsistema y libera cualquier recurso de conversación del jugador reservado. */
    static void Clear();

    /**
     * Avanza la máquina de estados original de conversación peatón-jugador.
     *
     * Todavía no está reconstruida; actualmente llama a la rutina del ejecutable
     * original ubicada en 0x43B0F0.
     */
    static void Update();

    /**
     * Finaliza el intercambio activo, libera el estado de voz y cancela el IK de
     * mirada de conversación tanto para el jugador como para el peatón participante.
     */
    static void EndConversation();
};
