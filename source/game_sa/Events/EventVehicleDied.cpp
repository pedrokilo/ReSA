/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventVehicleDied
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

#include "EventVehicleDied.h"

namespace {
// 0x4B0DA0 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventVehicleDied que usan los destructores del EXE. Deja la vtable de CEventVehicleDied del EXE (0x85B400), libera la referencia de m_vehicle (+0xC) con CEntity::CleanUpOldReference (0x571A00) si no son nulas
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall VehicleDiedDestructorBody(CEventVehicleDied* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B400;
    CEntity::SafeCleanUpRef(self->m_vehicle);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
// 0x4B6520 (lote 192A, sin validar): destructor con borrado de CEventVehicleDied del EXE (ranura 0 de su vtable 0x85B400; thiscall, un argumento de pila, `ret 4`, 76 bytes: `call 0x4B0DA0; test byte [esp+..],1; je ..;`
// y, con el bit 0 del BYTE de banderas, el operator delete de CEvent EN LINEA (pool de eventos 0xB744AC, indice = (this - base) / 0x44 con signo, estado |= 0x80, cursor = min(cursor, indice)); devuelve `this`). El EXE conserva ECX tras la llamada al cuerpo (sin pila extra).
// Como el original: destruye con el cuerpo de la clase (0x4B0DA0), libera `this` con CEvent::operator delete (0x4B5630, con hook; mismo efecto que la copia en linea) solo si el bit 0 esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventVehicleDied* __fastcall VehicleDiedScalarDeletingDestructor(CEventVehicleDied* self, void* /*edx*/, uint8 flags) {
    VehicleDiedDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventVehicleDied::InjectHooks()
{
    RH_ScopedVirtualClass(CEventVehicleDied, 0x85B400, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(VehicleDiedDestructorBody, "Destructor", 0x4B0DA0);
    RH_ScopedNamedGlobalInstall(VehicleDiedScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B6520); // lote 192A (sin validar)

    RH_ScopedInstall(Constructor, 0x4B0D10);
    RH_ScopedVMTInstall(GetEventType, 0x4B0D70);      // lote 139A (banco Fn139A): retorno de constante (35)
    RH_ScopedVMTInstall(GetLifeTime, 0x4B0D80);       // lote 139A: 0
    RH_ScopedVMTInstall(GetEventPriority, 0x4B0D90);  // lote 139A: 72
    RH_ScopedVMTInstall(Clone, 0x4B76D0);
    RH_ScopedVMTInstall(AffectsPed, 0x4B0E00);
}

CEventVehicleDied::CEventVehicleDied (CVehicle* vehicle)
{
    m_vehicle = vehicle;
    m_IsOccupantLeavingCar = false;
    m_vehicle->RegisterReference(reinterpret_cast<CEntity**>(&m_vehicle)); // todo: possible nullptr
}

CEventVehicleDied::~CEventVehicleDied ()
{
    CEntity::SafeCleanUpRef(m_vehicle);
}

// 0x4B0D10
CEventVehicleDied* CEventVehicleDied::Constructor(CVehicle* vehicle)
{
    this->CEventVehicleDied::CEventVehicleDied(vehicle);
    return this;
}

// 0x4B0E00
bool CEventVehicleDied::AffectsPed(CPed* ped)
{
    if (!ped->IsAlive())
        return false;

    // Como el original: si la bandera ya estaba puesta no se vuelve a buscar la tarea y se pasa directamente a
    // comparar el vehículo.
    if (!m_IsOccupantLeavingCar) {
        m_IsOccupantLeavingCar = ped->GetTaskManager().Has<TASK_COMPLEX_LEAVE_CAR>();
        if (!m_IsOccupantLeavingCar && !ped->bInVehicle)
            return false;
    }
    return m_vehicle && ped->m_pVehicle == m_vehicle;
}
