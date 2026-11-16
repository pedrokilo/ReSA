/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventStuckInAir
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventStuckInAir.h"


namespace {
// 0x4B1520 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventStuckInAir que usan los destructores del EXE. Deja la vtable de CEventStuckInAir del EXE (0x85B508), libera la referencia de m_ped (+0xC) con CEntity::CleanUpOldReference (0x571A00) si no son nulas
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall StuckInAirDestructorBody(CEventStuckInAir* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B508;
    CEntity::SafeCleanUpRef(self->m_ped);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
// 0x4B6660 (lote 192A, sin validar): destructor con borrado de CEventStuckInAir del EXE (ranura 0 de su vtable 0x85B508; thiscall, un argumento de pila, `ret 4`, 76 bytes: `call 0x4B1520; test byte [esp+..],1; je ..;`
// y, con el bit 0 del BYTE de banderas, el operator delete de CEvent EN LINEA (pool de eventos 0xB744AC, indice = (this - base) / 0x44 con signo, estado |= 0x80, cursor = min(cursor, indice)); devuelve `this`). El EXE conserva ECX tras la llamada al cuerpo (sin pila extra).
// Como el original: destruye con el cuerpo de la clase (0x4B1520), libera `this` con CEvent::operator delete (0x4B5630, con hook; mismo efecto que la copia en linea) solo si el bit 0 esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventStuckInAir* __fastcall StuckInAirScalarDeletingDestructor(CEventStuckInAir* self, void* /*edx*/, uint8 flags) {
    StuckInAirDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventStuckInAir::InjectHooks()
{
    RH_ScopedVirtualClass(CEventStuckInAir, 0x85B508, 16);
    RH_ScopedCategory("Events");
    RH_ScopedVMTInstall(Clone, 0x4B7840); // lote 201A (sin validar)
    RH_ScopedNamedGlobalInstall(StuckInAirDestructorBody, "Destructor", 0x4B1520);
    RH_ScopedNamedGlobalInstall(StuckInAirScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B6660); // lote 192A (sin validar)

    RH_ScopedInstall(Constructor, 0x4B1490);
    RH_ScopedVMTInstall(GetEventType, 0x4B14F0);                    // lote 153A (banco Fn153A)
    RH_ScopedVMTInstall(GetLifeTime, 0x4B1500);                     // lote 153A
    RH_ScopedVMTInstall(CanBeInterruptedBySameEvent, 0x4B1510);     // lote 153A
    RH_ScopedVMTInstall(GetEventPriority, 0x4B1600);
    RH_ScopedVMTInstall(AffectsPed, 0x4B1580);
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4B15B0);
}

// 0x4B14F0 (lote 153A, banco Fn153A; thiscall, `ret`, 6 bytes: `mov eax,5Dh`): EVENT_STUCK_IN_AIR (93); no lee el objeto.
eEventType CEventStuckInAir::GetEventType() const {
    return EVENT_STUCK_IN_AIR;
}

// 0x4B1500 (lote 153A, banco Fn153A; thiscall, `ret`, 3 bytes: `xor eax,eax`): vida util 0; no lee el objeto.
int32 CEventStuckInAir::GetLifeTime() {
    return 0;
}

// 0x4B1510 (lote 153A, banco Fn153A; thiscall, `ret`, 3 bytes: `mov al,1`): siempre verdadero; no lee el objeto.
bool CEventStuckInAir::CanBeInterruptedBySameEvent() {
    return true;
}

// 0x4B1490
CEventStuckInAir::CEventStuckInAir(CPed* ped)
{
    m_ped = ped;
    CEntity::SafeRegisterRef(m_ped);
}

CEventStuckInAir::~CEventStuckInAir()
{
    CEntity::SafeCleanUpRef(m_ped);
}

// 0x4B1600
int32 CEventStuckInAir::GetEventPriority() const {
    // Como el original: 75 solo si hay mas de un evento en cola (comparacion con signo, jg)
    return m_ped && m_ped->GetIntelligence()->GetEventHandler().GetCurrentEventType() != EVENT_STUCK_IN_AIR && (int32)m_ped->GetIntelligence()->GetEventGroup().GetNumEventsInQueue() > 1
        ? 75
        : 63;
}

// 0x4B1490
CEventStuckInAir* CEventStuckInAir::Constructor(CPed* ped)
{
    this->CEventStuckInAir::CEventStuckInAir(ped);
    return this;
}

// 0x4B1580
bool CEventStuckInAir::AffectsPed(CPed* ped)
{
    if (ped->bIsStanding)
        return false;

    return ped->GetStuckChecker().m_state != PED_STUCK_STATE_NONE;
}

// 0x4B15B0
bool CEventStuckInAir::TakesPriorityOver(const CEvent& refEvent)
{
    if (refEvent.GetEventPriority() < 74 && refEvent.GetEventType() != EVENT_STUCK_IN_AIR)
        return true;

    return CEvent::TakesPriorityOver(refEvent);
}

// 0x4B7840 (lote 201A, sin validar; thiscall, sin argumentos, `ret`, ranura 4 de la vtable 0x85B508; el listado la atribuia a otra clase). Como el original: CEvent::operator new (0x4B5620, con hook; el EXE lo expande en linea con 0x4B5570 sobre
// el pool 0xB744AC) con CEventStuckInAir; sin memoria devuelve nulo (EAX = 0); si no, llama al constructor del EXE (0x4B1490, con hook; aqui Constructor) con el ped guardado (+0xC) y devuelve lo que este deja en EAX (this). El marco SEH solo libera la
// memoria si el constructor lanza y se omite. No modifica el objeto copiado.
CEvent* CEventStuckInAir::Clone() const noexcept {
    auto* const object = static_cast<CEventStuckInAir*>(CEvent::operator new(sizeof(CEventStuckInAir)));
    return object ? object->Constructor(m_ped) : nullptr;
}
