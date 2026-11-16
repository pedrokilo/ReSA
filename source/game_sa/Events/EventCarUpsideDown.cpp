/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventCarUpsideDown
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
#include "EventCarUpsideDown.h"


namespace {
// 0x4B1D50 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventCarUpsideDown que usan los destructores del EXE. Deja la vtable de CEventCarUpsideDown del EXE (0x85B660), registra (no libera: asi lo hace el original) la referencia de m_vehicle (+0xC) con CEntity::RegisterReference (0x571B70) si no es nula
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall CarUpsideDownDestructorBody(CEventCarUpsideDown* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B660;
    CEntity::SafeRegisterRef(self->m_vehicle);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
// 0x4B67F0 (lote 192A, sin validar): destructor con borrado de CEventCarUpsideDown del EXE (ranura 0 de su vtable 0x85B660; thiscall, un argumento de pila, `ret 4`, 76 bytes: `call 0x4B1D50; test byte [esp+..],1; je ..;`
// y, con el bit 0 del BYTE de banderas, el operator delete de CEvent EN LINEA (pool de eventos 0xB744AC, indice = (this - base) / 0x44 con signo, estado |= 0x80, cursor = min(cursor, indice)); devuelve `this`). El EXE conserva ECX tras la llamada al cuerpo (sin pila extra).
// Como el original: destruye con el cuerpo de la clase (0x4B1D50), libera `this` con CEvent::operator delete (0x4B5630, con hook; mismo efecto que la copia en linea) solo si el bit 0 esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventCarUpsideDown* __fastcall CarUpsideDownScalarDeletingDestructor(CEventCarUpsideDown* self, void* /*edx*/, uint8 flags) {
    CarUpsideDownDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventCarUpsideDown::InjectHooks() {
    RH_ScopedVirtualClass(CEventCarUpsideDown, 0x85B660, 16);
    RH_ScopedCategory("Events");
    RH_ScopedVMTInstall(Clone, 0x4B79D0); // lote 201A (sin validar)
    RH_ScopedNamedGlobalInstall(CarUpsideDownDestructorBody, "Destructor", 0x4B1D50);
    RH_ScopedNamedGlobalInstall(CarUpsideDownScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B67F0); // lote 192A (sin validar)

    RH_ScopedInstall(Constructor, 0x4B1CC0);
    RH_ScopedVMTInstall(GetEventType, 0x4B1D20);      // lote 138A (banco Fn138A): retorno de constante
    RH_ScopedVMTInstall(GetLifeTime, 0x4B1D30);       // lote 138A
    RH_ScopedVMTInstall(GetEventPriority, 0x4B1D40);  // lote 138A
    RH_ScopedVMTInstall(AffectsPed, 0x4B1DB0);
}

// 0x4B1CC0
CEventCarUpsideDown* CEventCarUpsideDown::Constructor(CVehicle* vehicle) { this->CEventCarUpsideDown::CEventCarUpsideDown(vehicle); return this; }

// 0x4B1CC0
CEventCarUpsideDown::CEventCarUpsideDown(CVehicle* vehicle) : CEvent() {
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventCarUpsideDown::~CEventCarUpsideDown() {
    // Como el original (0x4B1D50): el destructor vuelve a registrar la referencia en vez de limpiarla (fallo del EXE; la referencia queda colgando)
    CEntity::SafeRegisterRef(m_vehicle);
}

// 0x4B1DB0
bool CEventCarUpsideDown::AffectsPed(CPed* ped) {
    if (ped->IsPlayer()) return false;
    if (ped->bGetOutUpsideDownCar == 0u) return false;
    if (ped->bInVehicle == 0u) return false;
    if (ped->IsCreatedByMission() && (!ped->m_pVehicle || ped->m_pVehicle->IsBoat())) return false;
    if (ped->m_pVehicle->IsBike() || ped->m_pVehicle->IsQuad()) return false;
    return ped->IsAlive();
}

// 0x4B79D0 (lote 201A, sin validar; thiscall, sin argumentos, `ret`, ranura 4 de la vtable 0x85B660; el listado la atribuia a otra clase). Como el original: CEvent::operator new (0x4B5620, con hook; el EXE lo expande en linea con 0x4B5570 sobre
// el pool 0xB744AC) con CEventCarUpsideDown; sin memoria devuelve nulo (EAX = 0); si no, llama al constructor del EXE (0x4B1CC0, con hook; aqui Constructor) con el vehiculo guardado (+0xC) y devuelve lo que este deja en EAX (this). El marco SEH solo libera la
// memoria si el constructor lanza y se omite. No modifica el objeto copiado.
CEventCarUpsideDown* CEventCarUpsideDown::Clone() const noexcept {
    auto* const object = static_cast<CEventCarUpsideDown*>(CEvent::operator new(sizeof(CEventCarUpsideDown)));
    return object ? object->Constructor(m_vehicle) : nullptr;
}
