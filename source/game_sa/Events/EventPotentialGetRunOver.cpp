/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPotentialGetRunOver
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
#include "EventPotentialGetRunOver.h"

namespace {
// 0x4AE270
// Cuerpo no virtual de ~CEventPotentialGetRunOver del EXE (thiscall, sin argumentos de pila, `ret`): deja la vtable de CEventPotentialGetRunOver del EXE (0x85AE88), libera con CEntity::CleanUpOldReference (0x571A00) las referencias no nulas
// en el orden del EXE y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 117A, banco Fn117A.)
void __fastcall PotentialGetRunOverDestructorBody(CEventPotentialGetRunOver* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AE88;
    CEntity::SafeCleanUpRef(self->m_Vehicle);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
// 0x4B5EA0 (lote 204A, sin validar; el listado decia CEventDamage; ranura 0 de la vtable de CEventPotentialGetRunOver; thiscall, un argumento de pila, `ret 4`, 76 bytes): destructor con borrado del EXE. Como el original: llama al cuerpo del destructor
// (0x4AE270, con hook propio) y, con el bit 0 del BYTE de banderas, libera la ranura en el pool de eventos (0xB744AC) EN LINEA, igual que CEvent::operator delete (0x4B5630, con hook: indice = (this - almacenamiento) / 0x44 con signo,
// estados[indice] |= 0x80, cursor = min con signo); devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
__declspec(noinline) CEventPotentialGetRunOver* __fastcall PotentialGetRunOverScalarDeletingDestructor(CEventPotentialGetRunOver* self, void* /*edx*/, uint8 flags) {
    PotentialGetRunOverDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventPotentialGetRunOver::InjectHooks() {
    RH_ScopedVirtualClass(CEventPotentialGetRunOver, 0x85AE88, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(PotentialGetRunOverDestructorBody, "Destructor", 0x4AE270);
    RH_ScopedNamedGlobalInstall(PotentialGetRunOverScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B5EA0); // lote 204A (sin validar)

    RH_ScopedInstall(Constructor, 0x4AE1A0);
    RH_ScopedVMTInstall(CloneEditable, 0x4B6F40); // lote 202A (sin validar): ranura 16
    RH_ScopedVMTInstall(AffectsPed, 0x4AE2D0);    // lote 204A (sin validar): ranura 5
}

// 0x4AE1A0
CEventPotentialGetRunOver::CEventPotentialGetRunOver(CVehicle* vehicle) {
    m_Vehicle = vehicle;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x4AE1A0
CEventPotentialGetRunOver* CEventPotentialGetRunOver::Constructor(CVehicle* vehicle) {
    this->CEventPotentialGetRunOver::CEventPotentialGetRunOver(vehicle);
    return this;
}

// 0x4B6F40 (lote 202A, sin validar; el listado decia CEventVehicleCollision; ranura 16 (CloneEditable) de la vtable 0x85AE88; thiscall, sin argumentos, `ret`, 97 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite)).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x18 bytes; sin memoria devuelve nulo sin leer campos; si no, el constructor 0x4AE1A0 (Constructor, con hook) con el vehiculo (+0x14) y devuelve el evento nuevo.
CEventEditableResponse* CEventPotentialGetRunOver::CloneEditable() const noexcept {
    auto* const object = static_cast<CEventPotentialGetRunOver*>(CEvent::operator new(sizeof(CEventPotentialGetRunOver)));
    if (!object) {
        return nullptr;
    }
    return object->Constructor(m_Vehicle);
}

// 0x4AE270
CEventPotentialGetRunOver::~CEventPotentialGetRunOver() {
    CEntity::SafeCleanUpRef(m_Vehicle);
}

// 0x4AE2D0 (lote 204A, sin validar; el listado decia CEventDamage; ranura 5 de la vtable 0x85AE88; thiscall, un argumento de pila, `ret 4`, 69 bytes). Como el original y en este orden, cortando al primer fallo: CPed::IsAlive (0x5E0170);
// el peaton no esta anclado a otro cuerpo (dword +0xFC nulo); no esta en vehiculo (bit 8 de +0x46C); el vehiculo del evento (+0x14) no es nulo; y su tipo (+0x590) no es VEHICLE_TYPE_BOAT (5). Devuelve el resultado del ultimo SETNE.
bool CEventPotentialGetRunOver::AffectsPed(CPed* ped) {
    return ped->IsAlive()
       && !ped->m_pAttachedTo
       && !ped->bInVehicle
       && m_Vehicle
       && !m_Vehicle->IsBoat();
}

