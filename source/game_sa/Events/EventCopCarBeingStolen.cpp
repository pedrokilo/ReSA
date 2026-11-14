/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventCopCarBeingStolen
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventCopCarBeingStolen.h"


namespace {
// 0x4B17F0 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventCopCarBeingStolen que usan los destructores del EXE. Deja la vtable de CEventCopCarBeingStolen del EXE (0x85B548), libera las referencias de m_hijacker (+0xC) y despues de m_vehicle (+0x10) con CEntity::CleanUpOldReference (0x571A00) si no son nulas
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall CopCarBeingStolenDestructorBody(CEventCopCarBeingStolen* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B548;
    CEntity::SafeCleanUpRef(self->m_hijacker);
    CEntity::SafeCleanUpRef(self->m_vehicle);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
// 0x4B66B0 (lote 192A, sin validar): destructor con borrado de CEventCopCarBeingStolen del EXE (ranura 0 de su vtable 0x85B548; thiscall, un argumento de pila, `ret 4`, 76 bytes: `call 0x4B17F0; test byte [esp+..],1; je ..;`
// y, con el bit 0 del BYTE de banderas, el operator delete de CEvent EN LINEA (pool de eventos 0xB744AC, indice = (this - base) / 0x44 con signo, estado |= 0x80, cursor = min(cursor, indice)); devuelve `this`). El EXE conserva ECX tras la llamada al cuerpo (sin pila extra).
// Como el original: destruye con el cuerpo de la clase (0x4B17F0), libera `this` con CEvent::operator delete (0x4B5630, con hook; mismo efecto que la copia en linea) solo si el bit 0 esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventCopCarBeingStolen* __fastcall CopCarBeingStolenScalarDeletingDestructor(CEventCopCarBeingStolen* self, void* /*edx*/, uint8 flags) {
    CopCarBeingStolenDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventCopCarBeingStolen::InjectHooks() {
    RH_ScopedVirtualClass(CEventCopCarBeingStolen, 0x85B548, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(CopCarBeingStolenDestructorBody, "Destructor", 0x4B17F0);
    RH_ScopedNamedGlobalInstall(CopCarBeingStolenScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B66B0); // lote 192A (sin validar)

    RH_ScopedInstall(Constructor, 0x4B1740);
    RH_ScopedVMTInstall(GetEventType, 0x4B17C0);      // lote 138A (banco Fn138A): retorno de constante
    RH_ScopedVMTInstall(GetLifeTime, 0x4B17D0);       // lote 138A
    RH_ScopedVMTInstall(GetEventPriority, 0x4B17E0);  // lote 138A
    RH_ScopedVMTInstall(AffectsPed, 0x4B1860);
}

// 0x4B1740
CEventCopCarBeingStolen* CEventCopCarBeingStolen::Constructor(CPed* hijacker, CVehicle* vehicle) { this->CEventCopCarBeingStolen::CEventCopCarBeingStolen(hijacker, vehicle); return this; }

// 0x4B1740
CEventCopCarBeingStolen::CEventCopCarBeingStolen(CPed* hijacker, CVehicle* vehicle) : CEvent() {
    m_hijacker = hijacker;
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_hijacker);
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventCopCarBeingStolen::~CEventCopCarBeingStolen() {
    CEntity::SafeCleanUpRef(m_hijacker);
    CEntity::SafeCleanUpRef(m_vehicle);
}

// 0x4B1860
bool CEventCopCarBeingStolen::AffectsPed(CPed* ped) {
    if (m_hijacker && m_vehicle && m_hijacker->IsAlive() && ped->IsAlive() && ped->m_nPedType == PED_TYPE_COP) {
        if (m_vehicle == ped->m_pVehicle && ped->bInVehicle)
            return true;
    }
    return false;
}