/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPedEnteredMyVehicle
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
#include "EventPedEnteredMyVehicle.h"

namespace {
// Cuerpo no virtual de ~CEventPedEnteredMyVehicle del EXE (thiscall, sin argumentos de pila, `ret`): deja la vtable de CEventPedEnteredMyVehicle del EXE (0x85AFE8), libera con CEntity::CleanUpOldReference (0x571A00) las referencias no nulas
// en el orden del EXE y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 119A, banco Fn119A.)
void __fastcall PedEnteredMyVehicleDestructorBody(CEventPedEnteredMyVehicle* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AFE8;
    CEntity::SafeCleanUpRef(self->m_PedThatEntered);
    CEntity::SafeCleanUpRef(self->m_Vehicle);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
// 0x4B6030 (lote 204A, sin validar; el listado decia CEventDamage; ranura 0 de la vtable de CEventPedEnteredMyVehicle; thiscall, un argumento de pila, `ret 4`, 76 bytes): destructor con borrado del EXE. Como el original: llama al cuerpo del destructor
// (0x4AEB90, con hook propio) y, con el bit 0 del BYTE de banderas, libera la ranura en el pool de eventos (0xB744AC) EN LINEA, igual que CEvent::operator delete (0x4B5630, con hook: indice = (this - almacenamiento) / 0x44 con signo,
// estados[indice] |= 0x80, cursor = min con signo); devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
__declspec(noinline) CEventPedEnteredMyVehicle* __fastcall PedEnteredMyVehicleScalarDeletingDestructor(CEventPedEnteredMyVehicle* self, void* /*edx*/, uint8 flags) {
    PedEnteredMyVehicleDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventPedEnteredMyVehicle::InjectHooks() {
    RH_ScopedVirtualClass(CEventPedEnteredMyVehicle, 0x85AFE8, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(PedEnteredMyVehicleDestructorBody, "Destructor", 0x4AEB90);
    RH_ScopedNamedGlobalInstall(PedEnteredMyVehicleScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B6030); // lote 204A (sin validar)

    RH_ScopedInstall(Constructor, 0x4AEAC0);
    RH_ScopedVMTInstall(AffectsPed, 0x4AEC00);     // lote 199A (sin validar): ranura 5
    RH_ScopedVMTInstall(CloneEditable, 0x4B7170);  // lote 199A (sin validar): ranura 16
}



CEventPedEnteredMyVehicle::CEventPedEnteredMyVehicle(CPed* pedThatEntered, CVehicle* vehicle, eTargetDoor targetDoor) :
    m_Vehicle{vehicle},
    m_PedThatEntered{pedThatEntered},
    m_TargetDoor{targetDoor}
{
    // 0x4AEAC0: el EXE registra las dos referencias sin comprobar que sean no nulas (con `this` nulo 0x571B70 lee [this+0x36] y falla); se conserva (Lote 119A, banco Fn119A).
    m_PedThatEntered->RegisterReference(reinterpret_cast<CEntity**>(&m_PedThatEntered));
    m_Vehicle->RegisterReference(reinterpret_cast<CEntity**>(&m_Vehicle));
}

CEventPedEnteredMyVehicle::~CEventPedEnteredMyVehicle() {
    CEntity::SafeCleanUpRef(m_PedThatEntered);
    CEntity::SafeCleanUpRef(m_Vehicle);
}

// 0x4AEC00 (lote 199A, sin validar; thiscall, un argumento de pila, `ret 4`, 65 bytes; ranura 5 de la vtable 0x85AFE8): en este orden y cortando en el primer fallo: CPed::IsAlive (0x5E0170) del peaton, vehiculo del peaton
// (+0x58C) no nulo, bit 8 de la palabra +0x46C (bInVehicle), ese vehiculo igual al de +0x18 del evento y el peaton que entro (+0x14) no nulo. Devuelve AL (0 o 1).
bool CEventPedEnteredMyVehicle::AffectsPed(CPed* ped) {
    return ped->IsAlive()
        && ped->m_pVehicle
        && ped->bInVehicle
        && ped->m_pVehicle == m_Vehicle
        && m_PedThatEntered;
}

// 0x4B7170 (lote 199A, sin validar; thiscall, sin argumentos, `ret`, 105 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 16 de la vtable 0x85AFE8).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x20 bytes; sin memoria devuelve nulo sin leer campos; si no, el constructor 0x4AEAC0 (Constructor, con hook) con el peaton (+0x14), el vehiculo (+0x18)
// y la puerta (+0x1C), en ese orden de argumentos, y devuelve la memoria.
CEventEditableResponse* CEventPedEnteredMyVehicle::CloneEditable() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventPedEnteredMyVehicle));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CEventPedEnteredMyVehicle*>(memory)->Constructor(m_PedThatEntered, m_Vehicle, m_TargetDoor);
}

// 0x4AEAC0
CEventPedEnteredMyVehicle* CEventPedEnteredMyVehicle::Constructor(CPed* pedThatEntered, CVehicle* vehicle, eTargetDoor targetDoor) {
    this->CEventPedEnteredMyVehicle::CEventPedEnteredMyVehicle(pedThatEntered, vehicle, targetDoor);
    return this;
}
