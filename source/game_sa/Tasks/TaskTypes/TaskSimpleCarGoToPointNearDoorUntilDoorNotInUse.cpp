/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleCarWaitForDoorNotToBeInUse.h"
#include "TaskSimpleStandStill.h"

namespace {
// 0x646E50 (lote 150A, banco Fn150A): cuerpo del destructor de CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse del EXE (con hook propio (Destructor); vtable 0x86EDE0). Funcion propia sin expansion en linea para que el banco pueda sustituirla;
// la llama el destructor con borrado como el `call 0x646E50` del EXE.
__declspec(noinline) void __fastcall CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUseDestructorBody(CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse* self, void* /*edx*/) { self->CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::~CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse(); }

// 0x64B480 (lote 150A, banco Fn150A): destructor con borrado de CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse del EXE (ranura 0 de su vtable 0x86EDE0; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x646E50; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse* __fastcall CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUseScalarDeletingDestructor(CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUseDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote150ABody_CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse() { return reinterpret_cast<uintptr_t>(&CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUseDestructorBody); } // para el banco Fn150A

void CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse, 0x86ede0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUseScalarDeletingDestructor, "ScalarDeletingDestructor", 0x64B480); // lote 150A (banco Fn150A)

    RH_ScopedInstall(Constructor, 0x646D70);
    RH_ScopedInstall(Destructor, 0x646E50);

    RH_ScopedVMTInstall(Clone, 0x646EC0);
    RH_ScopedVMTInstall(GetTaskType, 0x646E30);
    RH_ScopedVMTInstall(MakeAbortable, 0x646E40);
    RH_ScopedVMTInstall(ProcessPed, 0x64B4A0);
}

// 0x646D70
CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse(
    CVehicle*      vehicle,
    int32          targetDoor,
    const CVector& targetPt,
    eMoveState     moveState
) :
    m_TaskGoToPoint{ new CTaskSimpleGoToPoint{ moveState, targetPt } },
    m_TaskWaitForDoor{ new CTaskSimpleCarWaitForDoorNotToBeInUse{ vehicle, (uint32)targetDoor } }
{
}

// 0x646EC0
// Como el original: los datos salen de las subtareas del objeto copiado (antes se leian las del objeto nuevo, sin inicializar)
CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse(const CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse& o) :
    CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse{
        o.m_TaskWaitForDoor->m_veh,
        static_cast<int32>(o.m_TaskWaitForDoor->m_doorsToWaitFor[0]),
        o.m_TaskGoToPoint->m_vecTargetPoint,
        o.m_TaskGoToPoint->m_moveState
    }
{
}

// 0x646E50
CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::~CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse() {
    delete m_TaskGoToPoint;
    delete m_TaskWaitForDoor;
}

// 0x64B4A0
bool CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::ProcessPed(CPed* ped) {
    m_TaskGoToPoint->UpdatePoint(ComputeTarget());
    if (!m_TaskGoToPoint->ProcessPed(ped)) {
        return m_TaskWaitForDoor->ProcessPed(ped);
    }
    CTaskSimpleStandStill{-1}.ProcessPed(ped);
    return true;
}

// 0x646F40
CVector CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::ComputeTarget() const {
    const auto    veh    = m_TaskWaitForDoor->m_veh;
    const CVector vehPos = veh->GetPosition();
    return vehPos + (CCarEnterExit::GetPositionToOpenCarDoor(veh, m_TaskWaitForDoor->m_doorsToWaitFor[0]) - vehPos) * 2.f;
}
