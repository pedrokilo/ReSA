/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarWaitForDoorNotToBeInUse
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
#include "TaskSimpleCarWaitForDoorNotToBeInUse.h"

namespace {
// 0x646C00 (lote 150A, banco Fn150A): cuerpo del destructor de CTaskSimpleCarWaitForDoorNotToBeInUse del EXE (con hook propio (Destructor); vtable 0x86EDBC). Funcion propia sin expansion en linea para que el banco pueda sustituirla;
// la llama el destructor con borrado como el `call 0x646C00` del EXE.
__declspec(noinline) void __fastcall CTaskSimpleCarWaitForDoorNotToBeInUseDestructorBody(CTaskSimpleCarWaitForDoorNotToBeInUse* self, void* /*edx*/) { self->CTaskSimpleCarWaitForDoorNotToBeInUse::~CTaskSimpleCarWaitForDoorNotToBeInUse(); }

// 0x64B460 (lote 150A, banco Fn150A): destructor con borrado de CTaskSimpleCarWaitForDoorNotToBeInUse del EXE (ranura 0 de su vtable 0x86EDBC; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x646C00; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleCarWaitForDoorNotToBeInUse* __fastcall CTaskSimpleCarWaitForDoorNotToBeInUseScalarDeletingDestructor(CTaskSimpleCarWaitForDoorNotToBeInUse* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleCarWaitForDoorNotToBeInUseDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote150ABody_CTaskSimpleCarWaitForDoorNotToBeInUse() { return reinterpret_cast<uintptr_t>(&CTaskSimpleCarWaitForDoorNotToBeInUseDestructorBody); } // para el banco Fn150A

void CTaskSimpleCarWaitForDoorNotToBeInUse::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarWaitForDoorNotToBeInUse, 0x86edbc, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimpleCarWaitForDoorNotToBeInUseScalarDeletingDestructor, "ScalarDeletingDestructor", 0x64B460); // lote 150A (banco Fn150A)

    RH_ScopedInstall(Constructor, 0x646B70);

    RH_ScopedInstall(Destructor, 0x646C00);

    RH_ScopedVMTInstall(Clone, 0x649D20);
    RH_ScopedVMTInstall(GetTaskType, 0x646BE0);
    RH_ScopedVMTInstall(MakeAbortable, 0x646BF0);
    RH_ScopedVMTInstall(ProcessPed, 0x646C90);
    RH_ScopedVMTInstall(SetPedPosition, 0x646CC0);
}

/*!
* @addr 0x646B70
* @brief Wait until either of the 2 doors are ready
* @param veh            The vehicle whose door we have to wait for
* @param doorToWaitFor1 Main door to wait for
* @param doorToWaitFor2 Alternative door
*/
CTaskSimpleCarWaitForDoorNotToBeInUse::CTaskSimpleCarWaitForDoorNotToBeInUse(CVehicle* veh, uint32 doorToWaitFor1, uint32 doorToWaitFor2) :
    m_veh{veh},
    m_doorsToWaitFor{doorToWaitFor1, doorToWaitFor2}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskSimpleCarWaitForDoorNotToBeInUse::CTaskSimpleCarWaitForDoorNotToBeInUse(const CTaskSimpleCarWaitForDoorNotToBeInUse& o)
    : CTaskSimpleCarWaitForDoorNotToBeInUse{o.m_veh, o.m_doorsToWaitFor[0], o.m_doorsToWaitFor[1]} {}

// 0x646C00
CTaskSimpleCarWaitForDoorNotToBeInUse::~CTaskSimpleCarWaitForDoorNotToBeInUse() {
    CEntity::SafeCleanUpRef(m_veh);
}

// 0x646BF0
bool CTaskSimpleCarWaitForDoorNotToBeInUse::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return true;
}

// 0x646C90
bool CTaskSimpleCarWaitForDoorNotToBeInUse::ProcessPed(CPed* ped) {
    return !m_veh || !CCarEnterExit::IsCarDoorInUse(m_veh, m_doorsToWaitFor[0], m_doorsToWaitFor[1]);
}

// 0x646CC0
bool CTaskSimpleCarWaitForDoorNotToBeInUse::SetPedPosition(CPed* ped) {
    if (m_veh && ped->IsInVehicle(m_veh)) { // Como el original: sin vehiculo no se toca al peaton (test eax,eax / je)
        ped->SetPedPositionInCar();
    }
    return true;
}
