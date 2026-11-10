/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleWaitUntilPedIsOutCar
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
#include "TaskSimpleStandStill.h"
#include "TaskSimpleWaitUntilPedIsOutCar.h"

namespace {
// 0x6494F0 (lote 146A, banco Fn146A): cuerpo del destructor de CTaskSimpleWaitUntilPedIsOutCar que usa el destructor con borrado del EXE (thiscall, sin argumentos de pila; el hook Destructor de la clase). Funcion propia sin expansion
// en linea para que el banco pueda sustituirla; el destructor con borrado la llama como el `call 0x6494F0` del EXE.
__declspec(noinline) void __fastcall WaitUntilPedIsOutCarDestructorBody(CTaskSimpleWaitUntilPedIsOutCar* self, void* /*edx*/) { self->CTaskSimpleWaitUntilPedIsOutCar::~CTaskSimpleWaitUntilPedIsOutCar(); }

// 0x64D1E0 (lote 146A, banco Fn146A): destructor con borrado de CTaskSimpleWaitUntilPedIsOutCar del EXE (ranura 0 de su vtable 0x86F094; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x6494F0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x6494F0), libera `this`
// con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleWaitUntilPedIsOutCar* __fastcall WaitUntilPedIsOutCarScalarDeletingDestructor(CTaskSimpleWaitUntilPedIsOutCar* self, void* /*edx*/, uint8 flags) {
    WaitUntilPedIsOutCarDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote146AWaitUntilPedIsOutCarDestructorBody() { return reinterpret_cast<uintptr_t>(&WaitUntilPedIsOutCarDestructorBody); } // para el banco Fn146A

void CTaskSimpleWaitUntilPedIsOutCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleWaitUntilPedIsOutCar, 0x86F094, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(WaitUntilPedIsOutCarScalarDeletingDestructor, "ScalarDeletingDestructor", 0x64D1E0); // lote 146A (banco Fn146A)

    RH_ScopedInstall(Constructor, 0x649450);
    RH_ScopedInstall(Destructor, 0x6494F0);
    RH_ScopedVMTInstall(Clone, 0x64A4B0);
    RH_ScopedVMTInstall(GetTaskType, 0x6494D0);
    RH_ScopedVMTInstall(MakeAbortable, 0x6494E0);
    RH_ScopedVMTInstall(ProcessPed, 0x649550);
}

// 0x649450
CTaskSimpleWaitUntilPedIsOutCar::CTaskSimpleWaitUntilPedIsOutCar(CPed* ped, CVector const& pos) : CTaskSimple(),
    m_PedToWaitFor{ ped },
    m_Pos{ pos },
    m_bIsNotGoingTowardsPos{ true }
{
    CEntity::SafeRegisterRef(m_PedToWaitFor);
}

// 0x6494F0
CTaskSimpleWaitUntilPedIsOutCar::~CTaskSimpleWaitUntilPedIsOutCar() {
    CEntity::SafeCleanUpRef(m_PedToWaitFor);
}

// 0x649550

// 0x0
bool CTaskSimpleWaitUntilPedIsOutCar::ProcessPed(CPed* ped) {
    if (!m_PedToWaitFor) {
        return true;
    }

    if (m_bIsNotGoingTowardsPos) {
        m_bIsNotGoingTowardsPos = false;

        CTaskSimpleStandStill standStillTask;
        standStillTask.ProcessPed(ped);

        // Como el original: ángulo en radianes (0x53CBE0), no en grados
        ped->m_fAimingRotation = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(m_Pos.x, m_Pos.y, 0.f, 0.f));
    }

    return !m_PedToWaitFor->bInVehicle;
}

