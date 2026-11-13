/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventSeenPanickedPed
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventSeenPanickedPed.h"


namespace {
// 0x4B2130 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventSeenPanickedPed que usan los destructores del EXE. Deja la vtable de CEventSeenPanickedPed del EXE (0x85B778), libera la referencia de m_ped (+0x14) con CEntity::CleanUpOldReference (0x571A00) si no son nulas
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall SeenPanickedPedDestructorBody(CEventSeenPanickedPed* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B778;
    CEntity::SafeCleanUpRef(self->m_ped);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

namespace {
// 0x4B69E0 (lote 184A, sin validar; el listado decia CEventPotentialWalkIntoFire): destructor con borrado de CEventSeenPanickedPed del EXE (ranura 0 de su vtable 0x85B778; thiscall, un argumento de pila, `ret 4`, 76 bytes: `call 0x4B2130` (cuerpo del
// destructor, con hook propio); con el bit 0 del BYTE de banderas libera la ranura en el pool de eventos (0xB744AC) EN LINEA, igual que CEvent::operator delete (indice = (this - almacenamiento) / 0x44 con signo,
// estados[indice] |= 0x80, cursor = min con signo); devuelve `this`). Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0). Como CEventDamage (lote 176A), usa CEvent::operator delete (0x4B5630, con hook).
__declspec(noinline) CEventSeenPanickedPed* __fastcall SeenPanickedPedScalarDeletingDestructor(CEventSeenPanickedPed* self, void* /*edx*/, uint8 flags) {
    SeenPanickedPedDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventSeenPanickedPed::InjectHooks() {
    RH_ScopedVirtualClass(CEventSeenPanickedPed, 0x85B778, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(SeenPanickedPedDestructorBody, "Destructor", 0x4B2130);
    RH_ScopedNamedGlobalInstall(SeenPanickedPedScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B69E0); // lote 184A (sin validar)

    RH_ScopedInstall(Constructor, 0x4B2080);
    RH_ScopedVMTInstall(GetEventType, 0x4B20F0);      // lote 138A (banco Fn138A): retorno de constante
    RH_ScopedVMTInstall(GetLifeTime, 0x4B2100);       // lote 138A
    RH_ScopedVMTInstall(GetSourceEntity, 0x4B2110);   // lote 138A: devuelve m_ped (+0x14)
    RH_ScopedVMTInstall(GetEventPriority, 0x4B2120);  // lote 138A
    RH_ScopedVMTInstall(AffectsPed, 0x4B53C0);
}

// 0x4B2080
CEventSeenPanickedPed* CEventSeenPanickedPed::Constructor(CPed* ped) { this->CEventSeenPanickedPed::CEventSeenPanickedPed(ped); return this; }

// 0x4B2080
CEventSeenPanickedPed::CEventSeenPanickedPed(CPed* ped) : CEventEditableResponse() {
    m_ped = ped;
    CEntity::SafeRegisterRef(m_ped);
}

CEventSeenPanickedPed::~CEventSeenPanickedPed() {
    CEntity::SafeCleanUpRef(m_ped);
}

// 0x4B53C0
bool CEventSeenPanickedPed::AffectsPed(CPed* ped) {
    if (!ped->IsPlayer() && m_ped && m_ped != ped) {
        CEvent* currentEvent = m_ped->GetEventHandlerHistory().GetCurrentEvent();
        if (currentEvent && currentEvent->GetSourceEntity()) {
            CVector distance = ped->GetPosition() - m_ped->GetPosition();
            return distance.SquaredMagnitude() < 100.0f;
        }
    }
    return false;
}