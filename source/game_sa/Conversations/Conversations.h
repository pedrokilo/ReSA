#pragma once

#include "ConversationForPed.h"
#include "PedToPlayerConversations.h"
#include "ConversationNode.h"

class CPed;

// Capacidades fijas utilizadas por el juego original.
// Estos valores forman parte del modelo de memoria reconstruido y no deben
// modificarse sin revisar también el almacenamiento estático y la disposición binaria.
enum {
    MAX_NUM_CONVERSATIONS = 14,
    MAX_NUM_CONVERSATION_NODES = 50,
    MAX_NUM_TEMP_CONVERSATION_NODES = 12,
};

/**
 * Representación temporal de un nodo mientras se construye una conversación.
 *
 * Los scripts describen inicialmente las ramas mediante claves de texto cortas.
 * Mientras se ensambla la conversación, esas claves se almacenan aquí.
 * DoneSettingUpConversation() las resuelve primero a índices temporales y después
 * a índices del pool global de CConversationNode.
 */
class CTempConversationNode {
public:
    char  m_Name[8];        //!< Nombre/clave de este nodo temporal.
    char  m_NameNodeYes[8]; //!< Clave del sucesor para respuesta positiva, o vacío si no existe.
    char  m_NameNodeNo[8];  //!< Clave del sucesor para respuesta negativa, o vacío si no existe.
    int32 m_FinalSlot;      //!< Índice asignado finalmente dentro de CConversations::m_Nodes.
    int16 m_NodeYes;        //!< Índice resuelto en el array temporal para la rama positiva, o -1.
    int16 m_NodeNo;         //!< Índice resuelto en el array temporal para la rama negativa, o -1.
    int32 m_Speech;         //!< ID de voz principal/pregunta.
    int32 m_SpeechY;        //!< ID de voz para la respuesta positiva.
    int32 m_SpeechN;        //!< ID de voz para la respuesta negativa.
    // void Clear();
};

VALIDATE_SIZE(CTempConversationNode, 0x2C);

/**
 * Gestor global de los grafos de conversación guionizados.
 *
 * El juego original almacena todo este estado en direcciones globales fijas.
 * StaticRef vincula estas referencias de C++ directamente con dichas direcciones,
 * conservando ABI y comportamiento mientras las rutinas individuales se reconstruyen.
 */
class CConversations {
public:
    /**
     * Pequeña máquina de estados utilizada por la locución especial de audio de
     * misión iniciada mediante AwkwardSay().
     *
     * Las conversaciones normales se pausan mientras ese audio se carga o reproduce.
     */
    enum class eAwkwardSayStatus : int32 {
        INACTIVE = 0, //!< No hay una locución especial activa; las conversaciones pueden actualizarse normalmente.
        LOADING  = 1, //!< El audio de misión fue solicitado y se está esperando a que quede listo.
        PLAYING  = 2, //!< La locución precargada se está reproduciendo actualmente.
    };

    static inline auto& m_AwkwardSayStatus              = StaticRef<eAwkwardSayStatus>(0x9691C4); //!< Estado de reproducción del audio iniciado por AwkwardSay().
    static inline auto& m_SettingUpConversation         = StaticRef<bool>(0x9691D0);              //!< true mientras las llamadas de script están construyendo una conversación.
    static inline auto& m_Conversations                 = StaticRef<std::array<CConversationForPed, MAX_NUM_CONVERSATIONS>>(0x9691D8); //!< Pool fijo de conversaciones asociadas a peatones.
    static inline auto& m_Nodes                         = StaticRef<std::array<CConversationNode, MAX_NUM_CONVERSATION_NODES>>(0x969570); //!< Pool compartido de nodos permanentes.
    static inline auto& m_aTempNodes                    = StaticRef<std::array<CTempConversationNode, MAX_NUM_TEMP_CONVERSATION_NODES>>(0x969360); //!< Nodos temporales utilizados durante la configuración.
    static inline auto& m_SettingUpConversationNumNodes = StaticRef<int32>(0x9691C8);             //!< Número de entradas válidas actualmente dentro de m_aTempNodes.
    static inline auto& m_SettingUpConversationPed      = StaticRef<CPed*>(0x9691CC);             //!< Peatón para el que se está construyendo la conversación actual.

    /** Registra los hooks de las funciones del gestor de conversaciones. */
    static void InjectHooks();

    /** Reinicia todos los slots de conversación, todos los nodos permanentes y el estado del gestor. */
    static void Clear();

    /**
     * Elimina la conversación asociada a @p ped y libera recursivamente sus nodos.
     * No hace nada si ninguna entrada del pool hace referencia a ese peatón.
     */
    static void RemoveConversationForPed(CPed* ped);

    /** Ejecuta la actualización global de conversaciones y el bloqueo de audio de AwkwardSay(). */
    static void Update();

    /**
     * Añade una definición de nodo al buffer temporal de configuración.
     *
     * @param questionKey  Clave/nombre de este nodo.
     * @param answerYesKey Clave del nodo al que se avanza tras una respuesta positiva, o nullptr.
     * @param answerNoKey  Clave del nodo al que se avanza tras una respuesta negativa, o nullptr.
     * @param questionWAV  ID de voz principal/pregunta.
     * @param answerYesWAV ID de voz para la respuesta positiva.
     * @param answerNoWAV  ID de voz para la respuesta negativa.
     */
    static void SetUpConversationNode(const char* questionKey, const char* answerYesKey, const char* answerNoKey, int32 questionWAV, int32 answerYesWAV, int32 answerNoWAV);

    /** Reenvía la comprobación de posicionamiento del jugador a la conversación asociada a @p ped. */
    static bool IsPlayerInPositionForConversation(CPed* ped, bool randomConversation);

    /** @return true si al menos un slot de conversación se encuentra en un estado distinto de INACTIVE. */
    static bool IsConversationGoingOn();

    /**
     * Busca la entrada del pool fijo asociada a @p ped.
     * @return Puntero a la entrada encontrada, o nullptr si el peatón no tiene conversación.
     */
    static CConversationForPed* FindConversationForPed(CPed* ped);

    /**
     * Comprueba si la conversación de un peatón se encuentra actualmente en el nodo indicado.
     *
     * La implementación reconstruida realiza una comparación sin distinguir mayúsculas/minúsculas.
     */
    static bool IsConversationAtNode(const char* pName, CPed* pPed);

    /**
     * Reproduce una locución de audio de misión puntual asociada a @p speaker.
     * Mientras se carga o reproduce, las conversaciones guionizadas normales quedan pausadas.
     */
    static void AwkwardSay(int32 whatToSay, CPed* speaker);

    /** Habilita o deshabilita el avance de la conversación asociada a @p ped. */
    static void EnableConversation(CPed* ped, bool enabled);

    /**
     * Comienza a construir una nueva conversación para @p ped dentro de m_aTempNodes.
     * Debe ir seguida de llamadas a SetUpConversationNode() y DoneSettingUpConversation().
     */
    static void StartSettingUpConversation(CPed* ped);

    /**
     * Finaliza el grafo temporal, asigna slots de nodos permanentes y crea una
     * entrada CConversationForPed.
     *
     * @param bSuppressSubtitles Indica si deben ocultarse los subtítulos de la nueva conversación.
     */
    static void DoneSettingUpConversation(bool bSuppressSubtitles);

    /*
     * Helpers históricos presentes en notas de reversing pero que actualmente
     * no están expuestos como funciones reconstruidas:
     *
     * static void FindFreeNodeSlot();
     * static void FindFreeConversationSlot();
     */
};
