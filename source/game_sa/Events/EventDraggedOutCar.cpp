/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventDraggedOutCar
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

#include "EventDraggedOutCar.h"

namespace {
// Cuerpo no virtual de ~CEventDraggedOutCar del EXE (thiscall, sin argumentos de pila, `ret`): deja la vtable de CEventDraggedOutCar del EXE (0x85AD30), libera con CEntity::CleanUpOldReference (0x571A00) las referencias no nulas
// en el orden del EXE y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 117A, banco Fn117A.)
void __fastcall DraggedOutCarDestructorBody(CEventDraggedOutCar* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AD30;
    CEntity::SafeCleanUpRef(self->m_Vehicle);
    CEntity::SafeCleanUpRef(self->m_CarJacker);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}

// 0x4B5A70 (lote 190A, sin validar): destructor con borrado de CEventDraggedOutCar del EXE (ranura 0 de su vtable 0x85AD30; thiscall, un argumento de pila, `ret 4`, 76 bytes: `push esi; push edi; mov edi,ecx; call 0x4AD330; test byte [esp+0Ch],1; je ..;`
// y, con el bit 0 del BYTE de banderas, el operator delete de CEvent EN LINEA (pool de eventos 0xB744AC, como 0x4B5630); devuelve `this`). Como el original: destruye con el cuerpo de la clase (0x4AD330), libera `this`
// con CEvent::operator delete (0x4B5630, con hook; mismo efecto que la copia en linea) solo si el bit 0 esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventDraggedOutCar* __fastcall DraggedOutCarScalarDeletingDestructor(CEventDraggedOutCar* self, void* /*edx*/, uint8 flags) {
    DraggedOutCarDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventDraggedOutCar::InjectHooks() {
    RH_ScopedVirtualClass(CEventDraggedOutCar, 0x85AD30, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(DraggedOutCarDestructorBody, "Destructor", 0x4AD330);
    RH_ScopedNamedGlobalInstall(DraggedOutCarScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B5A70); // lote 190A (sin validar)

    RH_ScopedInstall(Constructor, 0x4AD250);
    RH_ScopedVMTInstall(AffectsPed, 0x4AD3A0); // lote 149A (banco Fn149A)
}

// 0x4AD250
CEventDraggedOutCar::CEventDraggedOutCar(CVehicle* vehicle, CPed* carjacker, bool IsDriverSeat) : CEventEditableResponse() {
    m_CarJacker = carjacker;
    m_Vehicle   = vehicle;
    m_IsDriverSeat = IsDriverSeat;
    CEntity::SafeRegisterRef(m_Vehicle);
    CEntity::SafeRegisterRef(m_CarJacker);
}

// 0x4AD330
CEventDraggedOutCar::~CEventDraggedOutCar() {
    CEntity::SafeCleanUpRef(m_Vehicle);
    CEntity::SafeCleanUpRef(m_CarJacker);
}

// 0x4AD250
CEventDraggedOutCar* CEventDraggedOutCar::Constructor(CVehicle* vehicle, CPed* carjacker, bool IsDriverSeat) {
    this->CEventDraggedOutCar::CEventDraggedOutCar(vehicle, carjacker, IsDriverSeat);
    return this;
}

// 0x4AD3A0 (lote 149A, banco Fn149A; thiscall, un argumento de pila, `ret 4`, 32 bytes): llama a CPed::IsAlive (0x5E0170, con hook) con el ped y, solo si el AL devuelto no es cero, devuelve si el atacante (+0x14) no es
// nulo; solo se define AL.
bool CEventDraggedOutCar::AffectsPed(CPed* ped) {
    return ped->IsAlive() && m_CarJacker;
}

// 0x4AD3C0
bool CEventDraggedOutCar::AffectsPedGroup(CPedGroup* pedGroup) {
    return FindPlayerPed() == pedGroup->GetMembership().GetLeader();
}

// 0x4B6DC0
CEventEditableResponse* CEventDraggedOutCar::CloneEditable() const noexcept {
    return new CEventDraggedOutCar(m_Vehicle, m_CarJacker, m_IsDriverSeat);
}
