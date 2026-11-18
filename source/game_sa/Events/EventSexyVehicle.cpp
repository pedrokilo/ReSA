/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventSexyVehicle
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
#include "EventSexyVehicle.h"

namespace {
// 0x4AF0A0
// Cuerpo no virtual de ~CEventSexyVehicle del EXE (thiscall, sin argumentos de pila, `ret`): deja la vtable de CEventSexyVehicle del EXE (0x85B0F8), libera con CEntity::CleanUpOldReference (0x571A00) las referencias no nulas
// en el orden del EXE y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 119A, banco Fn119A.)
void __fastcall SexyVehicleDestructorBody(CEventSexyVehicle* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B0F8;
    CEntity::SafeCleanUpRef(self->m_vehicle);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
// 0x4B6170 (lote 205A, sin validar; el listado decia CEventDamage; ranura 0 de la vtable 0x85B0F8 de CEventSexyVehicle; thiscall, un argumento de pila, `ret 4`, 76 bytes: `push esi; push edi; mov edi,ecx; call 0x4AF0A0; test byte [esp+0Ch],1; je ..;` y, con el bit 0 del BYTE de banderas,
// el operator delete de CEvent EN LINEA (pool de eventos 0xB744AC, indice = (this - base) / 0x44 con signo, estado |= 0x80, cursor = min(cursor, indice) con signo); devuelve `this`): destructor con borrado de CEventSexyVehicle del EXE.
// Como el original: destruye con el cuerpo de la clase (0x4AF0A0), libera `this` con CEvent::operator delete (0x4B5630, con hook; mismo efecto que la copia en linea) solo si el bit 0 esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventSexyVehicle* __fastcall SexyVehicleScalarDeletingDestructor(CEventSexyVehicle* self, void* /*edx*/, uint8 flags) {
    SexyVehicleDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventSexyVehicle::InjectHooks() {
    RH_ScopedVirtualClass(CEventSexyVehicle, 0x85B0F8, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(SexyVehicleDestructorBody, "Destructor", 0x4AF0A0);
    RH_ScopedNamedGlobalInstall(SexyVehicleScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B6170); // lote 205A (sin validar)

    RH_ScopedInstall(Constructor, 0x4AF010);
    RH_ScopedVMTInstall(AffectsPed, 0x4AF100); // lote 199A (sin validar): ranura 5
    RH_ScopedVMTInstall(Clone, 0x4B72F0);      // lote 199A (sin validar): ranura 4
}



CEventSexyVehicle::CEventSexyVehicle(CVehicle* vehicle) : CEvent() {
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventSexyVehicle::~CEventSexyVehicle() {
    CEntity::SafeCleanUpRef(m_vehicle);
}

// 0x4AF100 (lote 199A, sin validar; thiscall, un argumento de pila, `ret 4`, 35 bytes; ranura 5 de la vtable 0x85B0F8): CPed::IsAlive (0x5E0170) del peaton y vehiculo (+0xC) no nulo. Devuelve AL (0 o 1).
bool CEventSexyVehicle::AffectsPed(CPed* ped) {
    return ped->IsAlive() && m_vehicle;
}

// 0x4B72F0 (lote 199A, sin validar; thiscall, sin argumentos, `ret`, 97 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 4 de la vtable 0x85B0F8).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x10 bytes; sin memoria devuelve nulo sin leer campos; si no, el constructor 0x4AF010 (Constructor, con hook) con el vehiculo (+0xC) y devuelve la memoria.
CEventSexyVehicle* CEventSexyVehicle::Clone() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventSexyVehicle));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CEventSexyVehicle*>(memory)->Constructor(m_vehicle);
}

// 0x4AF010
CEventSexyVehicle* CEventSexyVehicle::Constructor(CVehicle* vehicle) {
    this->CEventSexyVehicle::CEventSexyVehicle(vehicle);
    return this;
}
