/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexPartnerChat
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
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

namespace CTaskComplexPartnerBody {
void __fastcall Destructor(CTaskComplexPartner* self, void* edx); // TaskComplexPartner.cpp: 0x683A40
}

namespace {
// 0x684320 (lote 153A, banco Fn153A): cuerpo del destructor de CTaskComplexPartnerChat del EXE (thiscall, sin argumentos de pila, `ret`, 93 bytes con marco SEH que solo desenrolla): deja la vtable de la clase (0x8707C4);
// si m_conversationEnabled (+0x74) y m_pedConversationLoaded (+0x76) no son cero (cada byte entero) llama a CAEPedSpeechAudioEntity::ReleasePedConversation (0x4E52A0); y llama al cuerpo de ~CTaskComplexPartner
// (0x683A40, con hook). El destructor generado por el compilador dejaria la vtable de la DLL. El marco SEH se omite.
void __fastcall CTaskComplexPartnerChatDestructorBodyExe(CTaskComplexPartnerChat* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x8707C4;
    if (self->m_conversationEnabled && self->m_pedConversationLoaded) {
        CAEPedSpeechAudioEntity::ReleasePedConversation();
    }
    CTaskComplexPartnerBody::Destructor(self, nullptr);
}
// 0x684FC0 (lote 185A, sin validar; el listado decia CTaskComplexPartnerGreet): destructor con borrado de CTaskComplexPartnerChat del EXE (ranura 0 de la vtable 0x8707C4; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x684320;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). 0x684320 es el cuerpo del destructor de la clase (CTaskComplexPartnerChatDestructorBodyExe, con hook). Como el original: libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas
// esta puesto y devuelve `this`. Se engancha como S.
CTaskComplexPartnerChat* __fastcall PartnerChatScalarDeletingDestructor(CTaskComplexPartnerChat* self, void* /*edx*/, uint8 flags) {
    CTaskComplexPartnerChatDestructorBodyExe(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskComplexPartnerChat::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexPartnerChat, 0x8707C4, 14);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexPartnerChatDestructorBodyExe, "DestructorBody", 0x684320); // lote 153A (banco Fn153A)
    RH_ScopedInstall(Constructor, 0x684290);
    RH_ScopedNamedGlobalInstall(PartnerChatScalarDeletingDestructor, "ScalarDeletingDestructor", 0x684FC0); // lote 185A (sin validar)
    RH_ScopedVMTInstall(GetTaskType, 0x681E10); // lote 185A (sin validar): ranura 4
    RH_ScopedVMTInstall(Clone, 0x684E30);       // lote 185A (sin validar): ranura 1
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

// 0x684E30 (lote 185A, sin validar; el listado decia CTaskComplexPartnerGreet; thiscall, sin argumentos, `ret`, 154 bytes con marco SEH que solo libera la memoria si el constructor lanza y se omite): Clone del EXE. Reserva 0x78 bytes con
// CTask::operator new (0x61A5A0); si devuelve nulo, devuelve nulo. Si no, llama al constructor (0x684290) sobre la memoria con (nombre de comando +0x14, companero +0x38, lider +0x58 sin signo, multiplicador +0x3C, cuenta de
// direcciones +0x5B con signo, conversacion +0x74 sin signo, byte +0x75 sin signo y el punto +0x40 por valor) y devuelve lo que el constructor deja en EAX.
CTask* CTaskComplexPartnerChat::Clone() const {
    auto* const memory = static_cast<CTaskComplexPartnerChat*>(CTask::operator new(sizeof(CTaskComplexPartnerChat)));
    if (!memory) {
        return nullptr;
    }
    return memory->Constructor(m_commandName, m_partner, m_leadSpeaker, m_distanceMultiplier, m_updateDirectionCount, m_conversationEnabled, field_75, m_point);
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
