/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexDragPedFromCar
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexDragPedFromCar.h"

namespace {
// 0x6404D0 (lote 165A, banco Fn165A): cuerpo no virtual de ~CTaskComplexDragPedFromCar del EXE (thiscall, sin argumentos de pila, `ret`, 90 bytes con marco SEH que solo desenrolla y aqui se omite). Deja la vtable de la
// clase del EXE (0x86EB6C), libera con CEntity::CleanUpOldReference (0x571A00) la referencia no nula en +0x50 (m_Ped) y llama a CTaskComplexEnterCar::Destructor (0x63DFA0, con hook). Sin hook propio (no es
// de este lote): es la funcion que llama el destructor con borrado; el destructor generado por el compilador dejaria la vtable de la DLL.
__declspec(noinline) void __fastcall DragPedFromCarDestructorBodyExe(CTaskComplexDragPedFromCar* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86EB6C;
    CEntity::SafeCleanUpRef(*reinterpret_cast<CEntity**>(reinterpret_cast<uint8*>(self) + 0x50));
    self->CTaskComplexEnterCar::Destructor();
}

// 0x643CE0 (lote 165A, banco Fn165A): destructor con borrado de CTaskComplexDragPedFromCar del EXE (ranura 0 de su vtable 0x86EB6C; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x6404D0; test byte
// [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x6404D0, con hook), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo
// si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexDragPedFromCar* __fastcall DragPedFromCarScalarDeletingDestructor(CTaskComplexDragPedFromCar* self, void* /*edx*/, uint8 flags) {
    DragPedFromCarDestructorBodyExe(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote165ABody_CTaskComplexDragPedFromCar() { return reinterpret_cast<uintptr_t>(&DragPedFromCarDestructorBodyExe); } // para el banco Fn165A

void CTaskComplexDragPedFromCar__InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDragPedFromCar, 0x86EB6C, 12); // 12 huecos, como CTaskComplexEnterCar
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(DragPedFromCarScalarDeletingDestructor, "ScalarDeletingDestructor", 0x643CE0); // lote 165A (banco Fn165A)
    RH_ScopedVMTInstall(ControlSubTask, 0x640530);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x643D00);
}

// 0x640430
CTaskComplexDragPedFromCar::CTaskComplexDragPedFromCar(CPed* ped, int32 draggedPedDownTime) : CTaskComplexEnterCar(nullptr, false, false, true, false) {
    m_Ped = ped;
    CEntity::SafeRegisterRef(m_Ped);
    m_DraggedPedDownTime = draggedPedDownTime;
}

// 0x6404D0
CTaskComplexDragPedFromCar::~CTaskComplexDragPedFromCar() {
    CEntity::SafeCleanUpRef(m_Ped);
}

// 0x640530


CTask* CTaskComplexDragPedFromCar::ControlSubTask(CPed* ped) {
    if (m_NumGettingInSet)
        return CTaskComplexEnterCar::ControlSubTask(ped);

    if (!m_Ped || m_Ped->bInVehicle || !m_pSubTask->MakeAbortable(ped))
        return CTaskComplexEnterCar::ControlSubTask(ped);

    return CTaskComplexEnterCar::CreateSubTask(TASK_NONE, nullptr);
}

// 0x643D00

CTask* CTaskComplexDragPedFromCar::CreateFirstSubTask(CPed* ped) {
    if (!m_Ped || !m_Ped->m_pVehicle || !m_Ped->bInVehicle ||
        (!m_Ped->m_pVehicle->IsPassenger(m_Ped) && !m_Ped->m_pVehicle->IsDriver(m_Ped))) {
        return CTaskComplexEnterCar::CreateSubTask(TASK_NONE, ped);
    }
    CEntity::SafeCleanUpRef(m_Car);
    m_Car = m_Ped->m_pVehicle;
    CEntity::SafeRegisterRef(m_Car);
    m_bAsDriver = m_Car->m_pDriver == m_Ped;
    m_bQuitAfterOpeningDoor = false;
    m_bQuitAfterDraggingPedOut = true;
    if (m_Car->m_pHandlingData->m_bTandemSeats || m_Car->IsBike() || m_Car->IsSubQuad()) {
        m_TargetSeat = 0;
    } else {
        m_TargetSeat = CCarEnterExit::ComputeTargetDoorToExit(m_Car, m_Ped);
    }
    return CTaskComplexEnterCar::CreateFirstSubTask(ped);
}