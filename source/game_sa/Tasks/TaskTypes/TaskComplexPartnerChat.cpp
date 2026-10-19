/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexPartnerChat
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexPartnerChat.h"
#include "TaskComplexChat.h"
#include "TaskComplexSequence.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleChat.h"
#include "TaskSimpleStandStill.h"

void CTaskComplexPartnerChat::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexPartnerChat, 0x8707C4, 14);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x684290);
    RH_ScopedVMTInstall(MakeAbortable, 0x682C60);
    RH_ScopedVMTInstall(GetPartnerSequence, 0x684380);
}

// 0x684290
CTaskComplexPartnerChat::CTaskComplexPartnerChat(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, int8 updateDirectionCount, bool conversationEnabled, bool a8, CVector point) :
    CTaskComplexPartner(commandName, partner, leadSpeaker, distanceMultiplier, false, updateDirectionCount, point)
{
    m_taskId = TASK_COMPLEX_PARTNER_CHAT;
    m_pedConversationLoaded = 0;
    m_conversationEnabled = conversationEnabled;
    if (conversationEnabled) {
        m_updateDirectionCount = 4;
    }
    field_75 = a8;
    // Como el original: copia byte a byte hasta el terminador (strcpy); strcpy_s rellena el resto con 0xFE en depuración
    strcpy(m_commandName, commandName);
}

// 0x684320
CTaskComplexPartnerChat::~CTaskComplexPartnerChat() {
    if (m_conversationEnabled && m_pedConversationLoaded) {
        CAEPedSpeechAudioEntity::ReleasePedConversation();
    }
}

CTaskComplexPartnerChat* CTaskComplexPartnerChat::Constructor(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, int8 updateDirectionCount, bool conversationEnabled, bool a8, CVector point)
{
    this->CTaskComplexPartnerChat::CTaskComplexPartnerChat(commandName, partner, leadSpeaker, distanceMultiplier, updateDirectionCount, conversationEnabled, a8, point);
    return this;
}

// 0x682C60
bool CTaskComplexPartnerChat::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (!m_pSubTask->MakeAbortable(ped, priority, event)) { // Como el original: sin comprobar nullptr
        return false;
    }
    // Como el original: ambos bytes se prueban enteros (cualquier valor distinto de 0 cuenta como verdadero)
    if (m_conversationEnabled && m_pedConversationLoaded) {
        CAEPedSpeechAudioEntity::ReleasePedConversation();
        m_pedConversationLoaded = false;
    }
    return true;
}

// 0x684380
// Como el original: la semilla se calcula en x87 (suma en float, en orden, redondeada a 24 bits) y se trunca con _ftol (0x821B40);
// /fp:fast no puede reordenar las sumas.
#pragma float_control(push)
#pragma float_control(precise, on)
CTaskComplexSequence* CTaskComplexPartnerChat::GetPartnerSequence() {
    auto* seq = new CTaskComplexSequence; // Como el original: sin inicialización por valor
    seq->AddTask(new CTaskComplexTurnToFaceEntityOrCoord(m_partner, 0.5f, 0.001f));

    // Semilla: (int)((((n << 4) + z) + y) + x) & 0x7F, por 1/128 (0x858B88); r = semilla/128, 1 - r (0x858624).
    // Duraciones: (int)(r * 3000 + 3500) (0x859A94, 0x8707FC) y (int)((1 - r) * 3000 + 3500).
    const int32 n0 = (int32)m_updateDirectionCount; // movsx
    float seedF    = (float)(n0 << 4);
    seedF += m_point.z;
    seedF += m_point.y;
    seedF += m_point.x;
    const int32 seed = (int32)seedF & 0x7F;
    const float r    = (float)seed * 0.0078125f;
    const float inv  = 1.0f - r;
    const int32 t1   = (int32)(r * 3000.0f + 3500.0f);
    const int32 t2   = (int32)(inv * 3000.0f + 3500.0f);

    if (!m_leadSpeaker) {
        // Como el original: CPedIntelligence::FindTaskByType (0x600EE0) sin comprobar nullptr (ni del compañero ni de la tarea)
        const auto* const other = static_cast<CTaskComplexPartnerChat*>(m_partner->GetIntelligence()->FindTaskByType(TASK_COMPLEX_PARTNER_CHAT));
        m_conversationEnabled   = other->m_conversationEnabled;
    }

    if (m_conversationEnabled) {
        // Como el original: contextos (palabras sin signo) s_Conversation[6 - 2n] y [7 - 2n] (0xB613F8 - 4n y 0xB613FA - 4n; con n = 4
        // se leen antes del array: 0xB613E8 = s_ConversationLength y 0xB613EA), n = (int8)m_updateDirectionCount
        const int32 n       = (int32)m_updateDirectionCount;
        const auto  convAt  = reinterpret_cast<uintptr_t>(CAEPedSpeechAudioEntity::s_Conversation.data());
        const auto  ctxA    = (int16)*reinterpret_cast<const uint16*>(convAt + (size_t)(ptrdiff_t)((6 - 2 * n) * 2));
        const auto  ctxB    = (int16)*reinterpret_cast<const uint16*>(convAt + (size_t)(ptrdiff_t)((7 - 2 * n) * 2));
        if (3 - n == (int32)CAEPedSpeechAudioEntity::s_ConversationLength) {
            m_updateDirectionCount = 0;
        }
        if (m_leadSpeaker) {
            seq->AddTask(new CTaskComplexChat(true, m_partner, (int32)m_updateDirectionCount, ctxA));
            seq->AddTask(new CTaskComplexChat(false, m_partner, (int32)m_updateDirectionCount, ctxA));
        } else {
            seq->AddTask(new CTaskComplexChat(false, m_partner, (int32)m_updateDirectionCount, ctxB));
            seq->AddTask(new CTaskComplexChat(true, m_partner, (int32)m_updateDirectionCount, ctxB));
        }
    } else if (m_leadSpeaker) {
        seq->AddTask(new CTaskSimpleChat(t1));
        seq->AddTask(new CTaskSimpleStandStill(t2, false, false, 8.0f));
    } else {
        seq->AddTask(new CTaskSimpleStandStill(t1, false, false, 8.0f));
        seq->AddTask(new CTaskSimpleChat(t2));
    }
    return seq;
}
#pragma float_control(pop)
