/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CInformGroupEvent
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "InformGroupEvent.h"

namespace {
// 0x4AC3B0 (lote 151A, banco Fn151A; sin llamadores directos; el listado decia CInformGroupEvent y lo es): constructor con argumentos de CInformGroupEvent (thiscall, cuatro dwords de pila, `ret 10h`, 51 bytes).
// Como el original: guarda el peaton en +0 y, si no es nulo, registra la referencia (CEntity::RegisterReference 0x571B70, ECX = peaton, argumento = la direccion de +0); despues guarda el grupo (+4), el evento (+8) y
// en +0xC el reloj de juego (0xB7CB84) mas el cuarto argumento (suma de 32 bits). Devuelve en EAX ese valor (m_Time), no `this`: el EXE deja EAX = reloj + retardo.
uint32 __fastcall InformGroupEventConstructorWithArgs(CInformGroupEvent* self, void* /*edx*/, CPed* ped, CPedGroup* pedGroup, CEvent* event, uint32 delayMs) {
    self->m_Ped = ped;
    CEntity::SafeRegisterRef(self->m_Ped);
    self->m_PedGroup = pedGroup;
    self->m_Event = event;
    self->m_Time = static_cast<int32>(CTimer::m_snTimeInMilliseconds + delayMs);
    return static_cast<uint32>(self->m_Time);
}
} // namespace

void CInformGroupEvent::InjectHooks() {
    RH_ScopedClass(CInformGroupEvent);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x4AC350);
    RH_ScopedNamedGlobalInstall(InformGroupEventConstructorWithArgs, "Constructor-2", 0x4AC3B0); // lote 151A (banco Fn151A)
    RH_ScopedInstall(Destructor, 0x4B29A0);
    RH_ScopedNamedInstall(Destructor, "Destructor-2", 0x4AC370); // lote 151A (banco Fn151A): copia byte a byte de 0x4B29A0 (sin llamadores directos)
    RH_ScopedInstall(Process, 0x4B29E0);
}

// 0x4B29A0
CInformGroupEvent::~CInformGroupEvent() {
    CEntity::SafeCleanUpRef(m_Ped);
    m_Ped = nullptr;
    delete std::exchange(m_Event, nullptr);
    m_Time = -1;
}

// 0x4B29E0
void CInformGroupEvent::Process() {
    if (m_Ped) {
        // FIXME: This cast isn't safe. Also in CInformFriendsEventQueue::Process.
        if (static_cast<uint32>(m_Time) >= CTimer::GetTimeInMS()) {
            return;
        }

        if (auto* group = CPedGroups::GetPedsGroup(m_Ped); group) {
            if (const auto id = group->GetId(); id >= 0 && CPedGroups::ms_activeGroups[id]) {
                group->m_groupIntelligence.AddEvent(CEventGroupEvent{ m_Ped, m_Event->Clone() });
            }
        }

        CEntity::SafeCleanUpRef(m_Ped);
        m_Ped = nullptr;
    }

    delete std::exchange(m_Event, nullptr);
    m_Time = -1;
}
