/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexDestroyCar
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

#include "TaskComplexDestroyCar.h"
#include "TaskComplexDestroyCarArmed.h"
#include "TaskComplexDestroyCarMelee.h"
#include "TaskComplexLeaveCar.h"

namespace {
// 0x621CB0 (lote 145A, banco Fn145A): cuerpo del destructor de CTaskComplexDestroyCar del EXE (con hook propio (Destructor); vtable 0x86D968). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama
// el destructor con borrado como el `call 0x621CB0` del EXE.
__declspec(noinline) void __fastcall CTaskComplexDestroyCarDestructorBody(CTaskComplexDestroyCar* self, void* /*edx*/) { self->CTaskComplexDestroyCar::~CTaskComplexDestroyCar(); }

// 0x628780 (lote 145A, banco Fn145A): destructor con borrado de CTaskComplexDestroyCar del EXE (ranura 0 de su vtable 0x86D968; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x621CB0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexDestroyCar* __fastcall CTaskComplexDestroyCarScalarDeletingDestructor(CTaskComplexDestroyCar* self, void* /*edx*/, uint8 flags) {
    CTaskComplexDestroyCarDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote145ABody_CTaskComplexDestroyCar() { return reinterpret_cast<uintptr_t>(&CTaskComplexDestroyCarDestructorBody); } // para el banco Fn145A

void CTaskComplexDestroyCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDestroyCar, 0x86d968, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexDestroyCarScalarDeletingDestructor, "ScalarDeletingDestructor", 0x628780); // lote 145A (banco Fn145A)

    RH_ScopedInstall(Constructor, 0x621C00);
    RH_ScopedInstall(Destructor, 0x621CB0);

    RH_ScopedInstall(CreateSubTask, 0x6287A0);

    RH_ScopedVMTInstall(Clone, 0x623530);
    RH_ScopedVMTInstall(GetTaskType, 0x621C70);
    RH_ScopedVMTInstall(MakeAbortable, 0x621C80);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62D9E0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62DA90);
    RH_ScopedVMTInstall(ControlSubTask, 0x6288C0);
}

// 0x621C00
CTaskComplexDestroyCar::CTaskComplexDestroyCar(CVehicle* vehicleToDestroy, uint32 unused, uint32 unused2, uint32 unused3) :
      m_VehicleToDestroy{ vehicleToDestroy },
      m_unused{ unused },
      m_unused2{ unused2 },
      m_unused3{ unused3 }
{
    CEntity::SafeRegisterRef(m_VehicleToDestroy);
}

// 0x621CB0
CTaskComplexDestroyCar::~CTaskComplexDestroyCar() {
    CEntity::SafeCleanUpRef(m_VehicleToDestroy);
}

// 0x621C80
bool CTaskComplexDestroyCar::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    switch (priority) {
    case ABORT_PRIORITY_URGENT:
    case ABORT_PRIORITY_IMMEDIATE:
        return m_pSubTask->MakeAbortable(ped, priority, event);
    default:
        return false;
    }
}

// 0x62DA90
CTask* CTaskComplexDestroyCar::CreateFirstSubTask(CPed* ped) {
    m_arg0 = false;

    if (!m_VehicleToDestroy)
        return CreateSubTask(TASK_FINISHED);

    if (ped->m_pVehicle && ped->bInVehicle)
        return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);

    if (ped->GetActiveWeapon().IsTypeMelee())
        return CreateSubTask(TASK_COMPLEX_DESTROY_CAR_MELEE);

    return CreateSubTask(TASK_COMPLEX_DESTROY_CAR_ARMED);
}

// 0x62D9E0
CTask* CTaskComplexDestroyCar::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_BE_HIT:
    case TASK_COMPLEX_FALL_AND_GET_UP:
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_COMPLEX_LEAVE_CAR:
    case TASK_COMPLEX_DRAG_PED_FROM_CAR:
        return CreateSubTask(ped->GetActiveWeapon().IsTypeMelee() ? TASK_COMPLEX_DESTROY_CAR_MELEE : TASK_COMPLEX_DESTROY_CAR_ARMED, ped);
    default:
        return nullptr;
    }
}

// 0x6287A0
CTask* CTaskComplexDestroyCar::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_DESTROY_CAR_ARMED:
        return new CTaskComplexDestroyCarArmed(m_VehicleToDestroy, m_unused, m_unused2, m_unused3);
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar(ped->m_pVehicle, 0, 0, true, false);
    case TASK_COMPLEX_DESTROY_CAR_MELEE:
        return new CTaskComplexDestroyCarMelee(m_VehicleToDestroy);
    default:
        return nullptr;
    }
}

// 0x6288C0
CTask* CTaskComplexDestroyCar::ControlSubTask(CPed* ped) {
    if (!m_VehicleToDestroy || m_VehicleToDestroy->m_fHealth <= 0.f) {
        return nullptr;
    }
    if (m_arg0) {
        return CreateFirstSubTask(ped);
    }

    eTaskType nextType;
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_DESTROY_CAR_ARMED:
        if (ped->GetActiveWeapon().m_TotalAmmo != 0 || ped->IsPlayer()) {
            return m_pSubTask;
        }
        for (int32 slot = 0; slot < NUM_WEAPON_SLOTS; ++slot) {
            // The original uses a signed comparison on this unsigned field.
            if (static_cast<int32>(ped->m_aWeapons[slot].m_TotalAmmo) > 0) {
                ped->SetCurrentWeapon(slot);
                return m_pSubTask;
            }
        }
        ped->SetCurrentWeapon(0);
        nextType = TASK_COMPLEX_DESTROY_CAR_MELEE;
        break;
    case TASK_COMPLEX_DESTROY_CAR_MELEE:
        if (ped->GetActiveWeapon().IsTypeMelee()) {
            return m_pSubTask;
        }
        nextType = TASK_COMPLEX_DESTROY_CAR_ARMED;
        break;
    default:
        return m_pSubTask;
    }
    return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)
        ? CreateSubTask(nextType, ped)
        : m_pSubTask;
}
