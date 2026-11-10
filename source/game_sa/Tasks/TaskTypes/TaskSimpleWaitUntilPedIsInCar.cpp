/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleWaitUntilPedIsInCar
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleWaitUntilPedIsInCar.h"

namespace {
// 0x6496A0 (lote 146A, banco Fn146A): cuerpo del destructor de CTaskSimpleWaitUntilPedIsInCar que usa el destructor con borrado del EXE (thiscall, sin argumentos de pila; el hook Destructor de la clase). Funcion propia sin expansion
// en linea para que el banco pueda sustituirla; el destructor con borrado la llama como el `call 0x6496A0` del EXE.
__declspec(noinline) void __fastcall WaitUntilPedIsInCarDestructorBody(CTaskSimpleWaitUntilPedIsInCar* self, void* /*edx*/) { self->CTaskSimpleWaitUntilPedIsInCar::~CTaskSimpleWaitUntilPedIsInCar(); }

// 0x64D200 (lote 146A, banco Fn146A): destructor con borrado de CTaskSimpleWaitUntilPedIsInCar del EXE (ranura 0 de su vtable 0x86F0B8; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x6496A0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x6496A0), libera `this`
// con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleWaitUntilPedIsInCar* __fastcall WaitUntilPedIsInCarScalarDeletingDestructor(CTaskSimpleWaitUntilPedIsInCar* self, void* /*edx*/, uint8 flags) {
    WaitUntilPedIsInCarDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote146AWaitUntilPedIsInCarDestructorBody() { return reinterpret_cast<uintptr_t>(&WaitUntilPedIsInCarDestructorBody); } // para el banco Fn146A

void CTaskSimpleWaitUntilPedIsInCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleWaitUntilPedIsInCar, 0x86f0b8, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(WaitUntilPedIsInCarScalarDeletingDestructor, "ScalarDeletingDestructor", 0x64D200); // lote 146A (banco Fn146A)

    RH_ScopedInstall(Constructor, 0x649620);
    RH_ScopedInstall(Destructor, 0x6496A0);

    RH_ScopedVMTInstall(Clone, 0x64A520);
    RH_ScopedVMTInstall(GetTaskType, 0x649680);
    RH_ScopedVMTInstall(MakeAbortable, 0x649690);
    RH_ScopedVMTInstall(ProcessPed, 0x649700);
}

// 0x649620
CTaskSimpleWaitUntilPedIsInCar::CTaskSimpleWaitUntilPedIsInCar(CPed* pedToWaitFor) :
    m_pedToWaitFor{pedToWaitFor}
{
    CEntity::SafeRegisterRef(m_pedToWaitFor);
}

// 0x6496A0
CTaskSimpleWaitUntilPedIsInCar::~CTaskSimpleWaitUntilPedIsInCar() {
    CEntity::SafeCleanUpRef(m_pedToWaitFor);
}


// 0x649700
bool CTaskSimpleWaitUntilPedIsInCar::ProcessPed(CPed* ped) {
    ped->SetMoveState(PEDMOVE_STILL);

    if (!m_pedToWaitFor) {
        return true;
    }

    if (m_pedToWaitFor->GetTaskManager().Find<TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL>()) {
        m_bIsWalkingToDoor = true;
    } else {
        if (m_bIsWalkingToDoor) {
            return true;
        }
        if (m_pedToWaitFor->GetTaskManager().Find<TASK_SIMPLE_CAR_GET_IN>()) {
            return true;
        }
        if (!m_pedToWaitFor->IsPlayer() && m_pedToWaitFor->bStuckUnderCar) {
            return true;
        }
    }

    return m_pedToWaitFor->bInVehicle;
}
