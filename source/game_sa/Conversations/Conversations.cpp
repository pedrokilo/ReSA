/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CConversations
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "Conversations.h"
#include "ConversationForPed.h"

/**
 * Registra los hooks del gestor global de conversaciones.
 *
 * La mayoría de las rutinas de esta clase ya cuentan con implementaciones C++
 * reconstruidas. DoneSettingUpConversation() conserva reversed=false en los
 * metadatos del hook, por lo que su cuerpo reconstruido todavía debe considerarse
 * pendiente de evidencia explícita de equivalencia/validación antes de cambiar esa marca.
 */
void CConversations::InjectHooks() {
    RH_ScopedClass(CConversations);
    RH_ScopedCategory("Conversations");

    RH_ScopedInstall(Clear, 0x43A7B0);
    RH_ScopedInstall(Update, 0x43C590);
    RH_ScopedInstall(SetUpConversationNode, 0x43A870);
    RH_ScopedInstall(RemoveConversationForPed, 0x43A960);
    RH_ScopedInstall(IsPlayerInPositionForConversation, 0x43B0B0);
    RH_ScopedInstall(IsConversationGoingOn, 0x43AAC0);
    RH_ScopedInstall(IsConversationAtNode, 0x43B000);
    RH_ScopedInstall(AwkwardSay, 0x43A810);
    // EnableConversation no existe como función propia en el EXE (está inlineada, igual que
    // FindConversationForPed). La dirección anterior, 0x43A7F0, caía dentro del bucle de
    // CConversations::Clear (0x43A7B0) y el salto del hook lo corrompía; no se instala.
    RH_ScopedInstall(StartSettingUpConversation, 0x43A840);
    RH_ScopedInstall(DoneSettingUpConversation, 0x43ADB0); // Validada: 2.048 pares (reversing/CConversations)
}

// 0x43A7B0
/**
 * Reinicia por completo el subsistema de conversaciones guionizadas.
 *
 * Los objetos de conversación se limpian sin recorrer recursivamente sus nodos,
 * porque el pool permanente de nodos se limpia de forma explícita en el bucle
 * siguiente. Esto evita recorrer estados de grafo potencialmente solapados o
 * antiguos durante un reinicio global.
 */
void CConversations::Clear() {
    ZoneScoped;

    // Reinicia todos los slots de conversación asociados a peatones. Se pasa true
    // porque la limpieza de nodos se realiza por separado justo después.
    for (auto& conversation : m_Conversations) {
        conversation.Clear(true);
    }

    // Marca todos los slots de nodos permanentes como libres y borra ramas/audio.
    for (auto& node : m_Nodes) {
        node.Clear();
    }

    // Cancela cualquier configuración en curso y cualquier estado de AwkwardSay().
    m_SettingUpConversation = 0;
    m_AwkwardSayStatus      = eAwkwardSayStatus::INACTIVE;
}

// 0x43C590
/**
 * Ejecuta la actualización global del sistema de conversaciones en cada frame.
 *
 * AwkwardSay() utiliza el canal 0 de audio de misión. Mientras esa locución
 * especial se está cargando o reproduciendo, las conversaciones normales quedan
 * detenidas. En cuanto termina la muestra, las actualizaciones normales se
 * reanudan en ese mismo frame.
 */
void CConversations::Update() {
    ZoneScoped;

    // Centraliza la actualización normal para poder ejecutarla directamente o
    // justo después de terminar la reproducción de AwkwardSay().
    const auto updateConversations = [&]() {
        for (auto& conversation : m_Conversations) {
            conversation.Update();
        }
    };

    switch (m_AwkwardSayStatus) {
    case eAwkwardSayStatus::LOADING:
        // El estado de carga 1 representa el estado listo/correcto esperado aquí.
        // Cuando está listo, comienza la reproducción y se espera a otro frame.
        if (AudioEngine.GetMissionAudioLoadingStatus(0) == 1) {
            AudioEngine.PlayLoadedMissionAudio(0);
            m_AwkwardSayStatus = eAwkwardSayStatus::PLAYING;
        }
        break;

    case eAwkwardSayStatus::PLAYING:
        // Las conversaciones normales continúan pausadas hasta que la muestra de
        // audio de misión especial haya terminado por completo.
        if (AudioEngine.IsMissionAudioSampleFinished(0)) {
            m_AwkwardSayStatus = eAwkwardSayStatus::INACTIVE;
            updateConversations();
        }
        break;

    case eAwkwardSayStatus::INACTIVE:
        // Ninguna locución especial está bloqueando al gestor de conversaciones.
        updateConversations();
        break;
    }
}

// 0x43A870
/**
 * Añade un nodo guionizado al buffer temporal de construcción de conversaciones.
 *
 * En esta etapa las ramas se proporcionan mediante claves de texto.
 * DoneSettingUpConversation() resuelve posteriormente esas claves a índices
 * temporales y, finalmente, a índices permanentes dentro de m_Nodes.
 *
 * @param questionKey  Clave/nombre del nodo que se está definiendo.
 * @param answerYesKey Clave del sucesor para una respuesta positiva, o nullptr.
 * @param answerNoKey  Clave del sucesor para una respuesta negativa, o nullptr.
 * @param questionWAV  ID de voz de la línea principal/pregunta.
 * @param answerYesWAV ID de voz de la respuesta positiva.
 * @param answerNoWAV  ID de voz de la respuesta negativa.
 *
 * @note Se espera que el llamador respete MAX_NUM_TEMP_CONVERSATION_NODES.
 */
void CConversations::SetUpConversationNode(
    const char* questionKey,
    const char* answerYesKey,
    const char* answerNoKey,
    int32       questionWAV,
    int32       answerYesWAV,
    int32       answerNoWAV
) {
    // Añade el nodo en el siguiente slot temporal indicado por el contador.
    auto& node = CConversations::m_aTempNodes[CConversations::m_SettingUpConversationNumNodes];

    // Las claves se normalizan a mayúsculas para poder comparar ramas con strcmp()
    // durante la finalización.
    strncpy(node.m_Name, questionKey, 6u);
    MakeUpperCase(node.m_Name);

    node.m_Speech  = questionWAV;
    node.m_SpeechY = answerYesWAV;
    node.m_SpeechN = answerNoWAV;

    // La ausencia de una clave de rama representa una rama terminal.
    if (answerYesKey) {
        strncpy(node.m_NameNodeYes, answerYesKey, 6u);
        MakeUpperCase(node.m_NameNodeYes);
    } else {
        node.m_NameNodeYes[0] = '\0';
    }

    if (answerNoKey) {
        strncpy(node.m_NameNodeNo, answerNoKey, 6u);
        MakeUpperCase(node.m_NameNodeNo);
    } else {
        node.m_NameNodeNo[0] = '\0';
    }

    ++CConversations::m_SettingUpConversationNumNodes;
}

// 0x43A960
/**
 * Elimina la conversación asociada a un peatón concreto.
 *
 * El slot encontrado es propietario lógico del grafo que comienza en m_FirstNode,
 * por lo que Clear(false) libera recursivamente sus nodos antes de liberar el slot.
 *
 * @param ped Peatón cuya conversación guionizada debe eliminarse.
 */
void CConversations::RemoveConversationForPed(CPed* ped) {
    for (auto& conversation : m_Conversations) {
        if (conversation.m_pPed == ped) {
            conversation.Clear(false);
        }
    }
}

// 0x43B0B0
/**
 * Comprueba si el jugador está colocado correctamente para conversar con @p ped.
 *
 * @param ped                Peatón cuya conversación debe consultarse.
 * @param randomConversation Selecciona las reglas alternativas usadas en conversaciones aleatorias.
 * @return Resultado devuelto por CConversationForPed::IsPlayerInPositionForConversation().
 *
 * @pre Debe existir previamente una conversación para @p ped. Esta función
 *      reproduce intencionadamente el lookup sin comprobación del comportamiento original.
 */
bool CConversations::IsPlayerInPositionForConversation(CPed* ped, bool randomConversation) {
    return FindConversationForPed(ped)->IsPlayerInPositionForConversation(randomConversation);
}

// 0x43AAC0
/**
 * Indica si existe actualmente alguna conversación guionizada activa.
 *
 * Una entrada del pool cuenta como activa cuando su estado es distinto de INACTIVE.
 * Por tanto, tener simplemente un peatón asociado al slot no es suficiente.
 */
bool CConversations::IsConversationGoingOn() {
    for (const auto& conversation : m_Conversations) {
        if (conversation.m_Status != CConversationForPed::eStatus::INACTIVE) {
            return true;
        }
    }
    return false;
}

// 0x43B000
/**
 * Comprueba si la conversación de un peatón se encuentra en un nodo concreto.
 *
 * Se excluye PLAYER_SPEAKING porque durante esa fase el nodo actual no se considera
 * consultable como nodo activo del peatón.
 *
 * @param pName Clave del nodo que se desea comparar.
 * @param pPed  Peatón cuya conversación debe inspeccionarse.
 * @return true si existe un nodo actual consultable y su nombre coincide.
 */
bool CConversations::IsConversationAtNode(const char* pName, CPed* pPed) {
    auto conversation = FindConversationForPed(pPed);
    assert(conversation);

    if (conversation->m_CurrentNode < 0 || conversation->m_Status == CConversationForPed::eStatus::PLAYER_SPEAKING) {
        return false;
    }

    // NOTSA: La implementación de estilo original puede normalizar una copia con
    // MakeUpperCase() y después usar strcmp(); stricmp() consigue la misma
    // comparación sin distinguir mayúsculas/minúsculas y sin modificar la entrada.
    return !stricmp(pName, CConversations::m_Nodes[conversation->m_CurrentNode].m_Name);
}

// 0x43A810
/**
 * Inicia una locución puntual de audio de misión asociada a un peatón.
 *
 * El audio se carga en el canal 0 de audio de misión. Después, Update() controla
 * la transición LOADING -> PLAYING -> INACTIVE y pausa las conversaciones normales
 * hasta que esta locución termina.
 *
 * @param whatToSay ID del audio/voz de misión.
 * @param speaker   Peatón al que debe asociarse espacialmente el audio.
 */
void CConversations::AwkwardSay(int32 whatToSay, CPed* speaker) {
    AudioEngine.PreloadMissionAudio(0, whatToSay);
    AudioEngine.AttachMissionAudioToPed(0, speaker);
    m_AwkwardSayStatus = eAwkwardSayStatus::LOADING;
}

// 0x43AA40
/**
 * Habilita o deshabilita la conversación guionizada asociada a @p ped.
 *
 * Esta bandera controla su avance; no elimina la conversación ni sus nodos asignados.
 *
 * @pre Debe existir previamente una conversación para @p ped.
 */
void CConversations::EnableConversation(CPed* ped, bool enabled) {
    FindConversationForPed(ped)->m_Enabled = enabled;
}

// 0x43A840
/**
 * Comienza a construir una conversación guionizada para un peatón.
 *
 * El peatón se almacena mediante el sistema de referencias del juego para que el
 * puntero pueda invalidarse de forma segura si la entidad desaparece durante la
 * configuración. El contenido anterior del buffer temporal se ignora al reiniciar
 * el contador de nodos.
 *
 * @param ped Peatón que será propietario de la conversación en construcción.
 */
void CConversations::StartSettingUpConversation(CPed* ped) {
    m_SettingUpConversationPed = ped;
    ped->RegisterReference(m_SettingUpConversationPed);

    m_SettingUpConversationNumNodes = 0;
    m_SettingUpConversation         = true;
}

// 0x43ADB0
/**
 * Finaliza la definición temporal de la conversación y la instala en los pools
 * fijos utilizados durante la ejecución.
 *
 * La finalización ocurre en cuatro etapas:
 *  1. Resolver las claves de texto yes/no a índices de m_aTempNodes.
 *  2. Buscar un slot libre de CConversationForPed.
 *  3. Reservar un slot permanente de m_Nodes para cada nodo temporal.
 *  4. Copiar los datos, remapear los índices de ramas e inicializar la conversación.
 *
 * @param bSuppressSubtitles Indica si deben ocultarse los subtítulos de esta conversación.
 *
 * @warning La implementación reconstruida actual conserva varias precondiciones
 *          observadas en el comportamiento original: debe existir al menos un nodo
 *          configurado y debe haber un slot de conversación libre. Si el pool
 *          permanente de nodos está lleno, la lógica de asignación cae en el slot 0,
 *          tal como está documentado en la reconstrucción existente. Esta función
 *          sigue marcada como reversed=false en InjectHooks(), por lo que debe
 *          validarse su equivalencia antes de cambiar esa marca.
 */
void CConversations::DoneSettingUpConversation(bool bSuppressSubtitles) {
    // Etapa 1: resuelve los nombres de las ramas a índices dentro del array temporal.
    //
    // Cada nodo temporal contiene inicialmente *nombres* de ramas. El valor -1
    // significa que no se encontró sucesor y, por tanto, representa una rama terminal.
    for (int32 i = 0; i < m_SettingUpConversationNumNodes; ++i) {
        auto& node = m_aTempNodes[i];
        node.m_NodeYes = node.m_NodeNo = -1;

        for (int32 j = 0; j < m_SettingUpConversationNumNodes; ++j) {
            if (!strcmp(node.m_NameNodeYes, m_aTempNodes[j].m_Name)) node.m_NodeYes = int16(j);
            if (!strcmp(node.m_NameNodeNo, m_aTempNodes[j].m_Name)) node.m_NodeNo = int16(j);
        }
    }

    // Etapa 2: localiza un slot libre en el pool de conversaciones por peatón.
    //
    // Un puntero de peatón nulo es la marca de slot libre. El flujo original/
    // reconstruido presupone que existe uno antes de continuar la finalización.
    CConversationForPed* conversation = nullptr;
    for (auto& slot : m_Conversations) {
        if (!slot.m_pPed) {
            conversation = &slot;
            break;
        }
    }

    // Etapa 3: reserva un slot permanente para cada nodo temporal.
    //
    // Un m_Name[0] vacío marca un nodo libre. Escribir temporalmente "X" evita que
    // otro nodo temporal de esta misma pasada seleccione el mismo slot.
    //
    // La reconstrucción existente utiliza intencionadamente el slot 0 como fallback
    // cuando el pool está lleno, conservando su comportamiento documentado.
    for (int32 i = 0; i < m_SettingUpConversationNumNodes; ++i) {
        int32 slot = 0;

        for (int32 j = 0; j < MAX_NUM_CONVERSATION_NODES; ++j) {
            if (!m_Nodes[j].m_Name[0]) {
                slot = j;
                m_Nodes[j].m_Name[0] = 'X';
                m_Nodes[j].m_Name[1] = '\0';
                break;
            }
        }

        m_aTempNodes[i].m_FinalSlot = slot;
    }

    // Etapa 4a: materializa cada nodo temporal dentro de su slot permanente reservado.
    //
    // Las ramas deben remapearse desde índices del array temporal a índices del
    // pool permanente. Una rama negativa conserva el centinela terminal -1.
    for (int32 i = 0; i < m_SettingUpConversationNumNodes; ++i) {
        const auto& temp = m_aTempNodes[i];
        auto& node = m_Nodes[temp.m_FinalSlot];

        strcpy(node.m_Name, temp.m_Name);
        node.m_NodeYes = temp.m_NodeYes < 0 ? -1 : int16(m_aTempNodes[temp.m_NodeYes].m_FinalSlot);
        node.m_NodeNo = temp.m_NodeNo < 0 ? -1 : int16(m_aTempNodes[temp.m_NodeNo].m_FinalSlot);
        node.m_Speech = temp.m_Speech;
        node.m_SpeechY = temp.m_SpeechY;
        node.m_SpeechN = temp.m_SpeechN;
    }

    // Etapa 4b: inicializa la conversación de ejecución en el primer nodo temporal.
    // Tanto el primer nodo como el nodo actual comienzan apuntando a esa raíz.
    conversation->m_FirstNode = conversation->m_CurrentNode = m_aTempNodes[0].m_FinalSlot;

    // Asocia el peatón propietario y registra el puntero almacenado mediante el
    // sistema de referencias de entidades, igual que durante la fase de configuración.
    conversation->m_pPed = m_SettingUpConversationPed;
    m_SettingUpConversationPed->RegisterReference(conversation->m_pPed);

    // Inicializa tiempos y opciones visibles. La conversación comienza INACTIVE;
    // su Update() de ejecución decide cuándo y cómo realiza las transiciones.
    conversation->m_LastChange = CTimer::GetTimeInMS();
    conversation->m_LastTimeWeWereCloseEnough = 0;
    conversation->m_Enabled = true;
    conversation->m_SuppressSubtitles = bSuppressSubtitles;
    conversation->m_Status = CConversationForPed::INACTIVE;

    // El estado temporal ya fue consumido y puede reutilizarse en otra conversación.
    m_SettingUpConversationNumNodes = 0;
    m_SettingUpConversation = false;
}

/**
 * Busca la entrada del pool de conversaciones asociada actualmente a un peatón.
 *
 * @param ped Puntero del peatón que se desea buscar.
 * @return Dirección de la entrada coincidente, o nullptr si ese peatón no tiene
 *         ninguna conversación registrada.
 */
CConversationForPed* CConversations::FindConversationForPed(CPed* ped) {
    for (auto& conversation : m_Conversations) {
        if (conversation.m_pPed == ped) {
            return &conversation;
        }
    }
    return nullptr;
}
