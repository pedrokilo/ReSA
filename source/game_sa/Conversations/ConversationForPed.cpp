/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CConversationForPed
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "ConversationForPed.h"
#include "Conversations.h"
#include "ConversationMathCpp.h"

/** Registra la actualización y la comprobación espacial de la conversación. */
void CConversationForPed::InjectHooks() {
    RH_ScopedClass(CConversationForPed);
    RH_ScopedCategory("Conversations");
    RH_ScopedInstall(Update, 0x43C190);
    RH_ScopedInstall(IsPlayerInPositionForConversation, 0x43AC40);
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
 * Avanza las preguntas y respuestas, conservando los plazos unsigned del juego.
 */
void CConversationForPed::Update() {
    if (!m_pPed || !m_Enabled || !IsPlayerInPositionForConversation(false)) {
        m_Status = INACTIVE;
        return;
    }

    const auto Speak = [](CPed* speaker, int32 speech) {
        if (speech > 0) {
            speaker->Say(static_cast<eGlobalSpeechContext>(static_cast<uint16>(speech)));
        } else if (speech < 0) {
            CConversations::AwkwardSay(static_cast<int32>(0u - static_cast<uint32>(speech)), speaker);
        }
    };
    const auto Reply = [&](bool yes) {
        const auto& node = CConversations::m_Nodes[m_CurrentNode];
        if (!m_SuppressSubtitles) {
            CMessages::ClearSmallMessagesOnly();
            strcpy(gString, node.m_Name);
            strcat(gString, yes ? "Y" : "N");
            CMessages::AddMessageJump(TheText.Get(gString), 4000, 3, true);
        }
        const auto speech = yes ? node.m_SpeechY : node.m_SpeechN;
        if (speech != 0) {
            Speak(FindPlayerPed(), speech);
        }
        m_CurrentNode = yes ? node.m_NodeYes : node.m_NodeNo;
        m_LastChange = CTimer::GetTimeInMS();
        if (!m_SuppressSubtitles) {
            CMessages::AddMessageQ(TheText.Get(CConversations::m_Nodes[m_CurrentNode].m_Name), 4000, 1, true);
        }
        m_Status = PLAYER_SPEAKING;
    };

    if (CTimer::GetTimeInMS() > m_LastTimeWeWereCloseEnough + 10000u
        || (m_Status == PLAYER_SPEAKING
            && (CTimer::GetTimeInMS() > m_LastChange + 4000u
                || CPad::GetPad(0)->ConversationNoJustDown()
                || CPad::GetPad(0)->ConversationYesJustDown()))) {
        const auto& node = CConversations::m_Nodes[m_CurrentNode];
        if (!m_SuppressSubtitles) {
            CMessages::ClearSmallMessagesOnly();
            CMessages::AddMessageJump(TheText.Get(node.m_Name), 4000, 1, true);
        }
        Speak(m_pPed, node.m_Speech);
        m_Status = PED_SPEAKING;
        m_LastChange = CTimer::GetTimeInMS();
    } else if (CTimer::GetTimeInMS() > m_LastChange + 400u
        && (m_Status == PED_SPEAKING || m_Status == WAITINGFORINPUT || m_Status == INACTIVE)) {
        // El original prueba primero la rama negativa, incluso si ambas teclas bajan.
        if (CPad::GetPad(0)->ConversationNoJustDown() && CConversations::m_Nodes[m_CurrentNode].m_NodeNo >= 0) {
            Reply(false);
        } else if (CPad::GetPad(0)->ConversationYesJustDown() && CConversations::m_Nodes[m_CurrentNode].m_NodeYes >= 0) {
            Reply(true);
        }
    }

    m_LastTimeWeWereCloseEnough = CTimer::GetTimeInMS();
    if (m_Status == PED_SPEAKING && CTimer::GetTimeInMS() > m_LastChange + 4000u) {
        m_Status = WAITINGFORINPUT;
    }
}

// 0x43AC40
/**
 * Comprueba distancia, orientación, movimiento y disponibilidad de los participantes.
 *
 * @param randomConversation true si se prueban las reglas de conversación aleatoria.
 * @return Indica si el jugador está colocado lo bastante cerca/correctamente para conversar.
 */
bool CConversationForPed::IsPlayerInPositionForConversation(bool randomConversation) {
    const auto& pedPosition = m_pPed->GetPosition();
    const auto playerPosition = FindPlayerCoors();
    // GTA cambia la precisión de x87: ni float ni double conservan todos sus
    // redondeos intermedios. El cálculo está en ConversationMathCpp.cpp, con las
    // mismas operaciones x87 y la comparación que conserva los casos NaN.
    const bool tooFar = ConversationTooFarCpp(&pedPosition.x, &playerPosition.x);
    if (tooFar) {
        return false;
    }

    const auto& pedForward = m_pPed->GetForward();
    const auto& playerForward = FindPlayerPed()->GetForward();
    const bool facingAway = ConversationFacingAwayCpp(&pedForward.x, &playerForward.x);
    if (facingAway) {
        return false;
    }
    const auto& speed = FindPlayerPed()->m_vecMoveSpeed;
    const bool movingTooFast = ConversationMovingTooFastCpp(&speed.x);
    if (movingTooFast) {
        return false;
    }
    if (m_pPed->m_pLastEntityDamage == FindPlayerPed()
        && CTimer::GetTimeInMS() < static_cast<uint32>(m_pPed->field_768) + 6000u) {
        return false;
    }
    if (randomConversation && FindPlayerPed()->GetPlayerGroup().GetMembership().CountMembersExcludingLeader() >= 1) {
        return false;
    }
    return FindPlayerPed()->PedIsReadyForConversation(randomConversation)
        && m_pPed->PedIsReadyForConversation(randomConversation);
}
