/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexLeaveBoat
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexLeaveBoat.h"
#include "TaskSimpleCarSetPedOut.h"

void CTaskComplexLeaveBoat::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexLeaveBoat, 0x86e854, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63BB60);
    // Como el original: 0x642250 es el destructor con borrado (slot 0, ret 4); el destructor es 0x63BBD0
    RH_ScopedInstall(Destructor, 0x63BBD0);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x642250); // lote 186A (sin validar)
    
    RH_ScopedInstall(CreateSubTask, 0x63BC50);

    RH_ScopedVMTInstall(Clone, 0x63DA60);
    RH_ScopedVMTInstall(GetTaskType, 0x63BBC0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x63BC30);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x642270);
    RH_ScopedVMTInstall(ControlSubTask, 0x63BC40);
}

// 0x63BB60
CTaskComplexLeaveBoat::CTaskComplexLeaveBoat(CVehicle* vehicle, uint32 delay) :
    m_veh{vehicle},
    m_delayTimeMs{delay}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskComplexLeaveBoat::CTaskComplexLeaveBoat(const CTaskComplexLeaveBoat& o) :
    CTaskComplexLeaveBoat{o.m_veh, o.m_delayTimeMs}
{
}

// 0x63BBD0
CTaskComplexLeaveBoat::~CTaskComplexLeaveBoat() {
    CEntity::SafeCleanUpRef(m_veh);
}

// 0x642250 (lote 186A, sin validar; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x63BBD0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`;
// el listado decia CTaskComplexStealCar): destructor con borrado de CTaskComplexLeaveBoat del EXE (ranura 0 de su vtable 0x86E854). Como el original: destruye con el cuerpo de la clase (0x63BBD0), libera `this` con CTask::operator delete
// (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskComplexLeaveBoat* CTaskComplexLeaveBoat::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}

// 0x63BC50
CTask* CTaskComplexLeaveBoat::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_SIMPLE_CAR_SET_PED_OUT: {
        return new CTaskSimpleCarSetPedOut{ m_veh, TARGET_DOOR_FRONT_LEFT, true };
    }
    }
    return nullptr;
}

// 0x63BC30
CTask* CTaskComplexLeaveBoat::CreateNextSubTask(CPed* ped) {
    return nullptr;
}

// 0x642270
CTask* CTaskComplexLeaveBoat::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_CAR_SET_PED_OUT);
}

// 0x63BC40
CTask* CTaskComplexLeaveBoat::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}
