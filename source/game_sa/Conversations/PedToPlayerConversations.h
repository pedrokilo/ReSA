#pragma once
#include "DatosExe/DataBssE2.h"
#include "DatosExe/DataBssE1.h"

class CPed;
class CVehicle;

/**
 * Gestor de conversaciones ambientales/directas iniciadas entre un peatón y el jugador.
 *
 * A diferencia de CConversationForPed, este subsistema utiliza un único conjunto
 * de variables globales de estado en vez de un pool de grafos de nodos. Update()
 * explora hasta ocho slots del pool, selecciona una apertura según el tema y
 * procesa las respuestas del jugador y los plazos de cierre.
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
    static inline auto& m_Topic                        = DatosExe::Bss::b9691BC;     //!< Selector de tema/voz utilizado por el Update() original.
    static inline auto& m_TimeOfLastPlayerConversation = DatosExe::Bss::b9691B4;  //!< Marca temporal usada para limitar/registrar conversaciones recientes.
    static inline auto& m_StartTime                    = DatosExe::Bss::b9691B8;   //!< Marca temporal en la que comenzó el intercambio actual.
    static inline auto& m_bPositiveReply               = DatosExe::Bss::b9691B0;     //!< Existe en memoria original; actualmente no se usa en el código reconstruido.
    static inline auto& m_bPositiveOpening             = DatosExe::Bss::b9691B1;     //!< Indica si la apertura seleccionada por la rutina original es positiva.
    static inline auto& m_PedPoolIndex                 = DatosExe::Bss::b969A3C;    //!< Exploración circular: hasta ocho slots por actualización.
    static inline auto& m_LastPlayerVehicle            = StaticRef<CVehicle*>(0x969A40); //!< Último vehículo del jugador, registrado como referencia de entidad.

    /** Registra los hooks del subsistema de conversación peatón-jugador. */
    static void InjectHooks();

    /** Reinicia el estado del subsistema y libera cualquier recurso de conversación del jugador reservado. */
    static void Clear();

    /**
     * Avanza la máquina de estados reconstruida desde 0x43B0F0. Conserva las
     * dependencias del motor para voz, IK, referencias y tareas.
     */
    static void Update();

    /**
     * Finaliza el intercambio activo, libera el estado de voz y cancela el IK de
     * mirada de conversación tanto para el jugador como para el peatón participante.
     */
    static void EndConversation();
};
