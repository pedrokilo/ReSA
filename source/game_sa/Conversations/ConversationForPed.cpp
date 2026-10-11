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

// Este archivo se compila completo con /O2 /Oi /arch:IA32 /fp:fast /Zi, sin PCH ni unity (ver source/CMakeLists.txt):
// IsPlayerInPositionForConversation necesita las operaciones x87 sin guardados intermedios de float.
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include "StdInc.h"
#include "ConversationForPed.h"
#include "Conversations.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

extern "C" double __cdecl sqrt(double);
#pragma intrinsic(sqrt)

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

// GTA cambia la precisión de x87: ni float ni double conservan todos sus redondeos
// intermedios. Estas funciones son noinline porque, insertadas en línea con /fp:fast, el
// compilador sustituye sqrt(s) > 4 por s > 16 y elimina el FSQRT. Conservan la secuencia del
// EXE (FSQRT, FCOMP + FNSTSW + TEST AH,41h), el orden de las sumas y el resultado false con NaN.
static __declspec(noinline) bool ConversationTooFar(const CVector& ped, const CVector& player) {
    const float dx = player.x - ped.x;
    const float dy = player.y - ped.y;
    const float dz = player.z - ped.z;
    const float distance = float(sqrt(double((dz * dz + dy * dy) + dx * dx)));
    return distance > 4.f;
}

static __declspec(noinline) bool ConversationFacingAway(const CVector& ped, const CVector& player) {
    const float dot = (player.z * ped.z + player.y * ped.y) + player.x * ped.x;
    return dot > 0.f;
}

static __declspec(noinline) bool ConversationMovingTooFast(const CVector& speed) {
    const float magnitude = float(sqrt(double(speed.x * speed.x + speed.y * speed.y)));
    return magnitude > 0.01f;
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
    if (ConversationTooFar(pedPosition, playerPosition)) {
        return false;
    }

    const auto& pedForward = m_pPed->GetForward();
    const auto& playerForward = FindPlayerPed()->GetForward();
    if (ConversationFacingAway(pedForward, playerForward)) {
        return false;
    }
    if (ConversationMovingTooFast(FindPlayerPed()->m_vecMoveSpeed)) {
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
