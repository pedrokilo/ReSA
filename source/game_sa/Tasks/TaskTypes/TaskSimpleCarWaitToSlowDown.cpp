/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarWaitToSlowDown
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

#include "TaskSimpleCarWaitToSlowDown.h"

namespace {
// 0x646A00 (lote 150A, banco Fn150A): cuerpo del destructor de CTaskSimpleCarWaitToSlowDown del EXE (con hook propio (Destructor); vtable 0x86ED98). Funcion propia sin expansion en linea para que el banco pueda sustituirla;
// la llama el destructor con borrado como el `call 0x646A00` del EXE.
__declspec(noinline) void __fastcall CTaskSimpleCarWaitToSlowDownDestructorBody(CTaskSimpleCarWaitToSlowDown* self, void* /*edx*/) { self->CTaskSimpleCarWaitToSlowDown::~CTaskSimpleCarWaitToSlowDown(); }

// 0x64B440 (lote 150A, banco Fn150A): destructor con borrado de CTaskSimpleCarWaitToSlowDown del EXE (ranura 0 de su vtable 0x86ED98; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x646A00; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleCarWaitToSlowDown* __fastcall CTaskSimpleCarWaitToSlowDownScalarDeletingDestructor(CTaskSimpleCarWaitToSlowDown* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleCarWaitToSlowDownDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote150ABody_CTaskSimpleCarWaitToSlowDown() { return reinterpret_cast<uintptr_t>(&CTaskSimpleCarWaitToSlowDownDestructorBody); } // para el banco Fn150A

void CTaskSimpleCarWaitToSlowDown::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarWaitToSlowDown, 0x86ed98, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimpleCarWaitToSlowDownScalarDeletingDestructor, "ScalarDeletingDestructor", 0x64B440); // lote 150A (banco Fn150A)

    RH_ScopedInstall(Constructor, 0x646990);
    RH_ScopedInstall(Destructor, 0x646A00);
    RH_ScopedVMTInstall(Clone, 0x649CB0);
    RH_ScopedVMTInstall(GetTaskType, 0x6469F0);
    RH_ScopedVMTInstall(MakeAbortable, 0x646A60);
    RH_ScopedVMTInstall(ProcessPed, 0x646AD0);
    RH_ScopedVMTInstall(SetPedPosition, 0x646AB0); // lote 124A (banco Fn124A): ranura 8 de la vtable 0x86EDB8; el "crash" antiguo era de CPed::SetPedPositionInCar (0x5DF910), ya corregida y validada
}

// 0x646990
CTaskSimpleCarWaitToSlowDown::CTaskSimpleCarWaitToSlowDown(CVehicle* vehicle, SlowDownType type) :
    CTaskSimple(),
    m_TargetVehicle{ vehicle },
    m_SlowType{ type }
{
    CEntity::SafeRegisterRef(m_TargetVehicle);
}

// 0x646A00
CTaskSimpleCarWaitToSlowDown::~CTaskSimpleCarWaitToSlowDown() {
    CEntity::SafeCleanUpRef(m_TargetVehicle);
}

// 0x646A60
bool CTaskSimpleCarWaitToSlowDown::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE:
        return true;
    case ABORT_PRIORITY_URGENT: {
        if (event && event->GetEventType() == EVENT_DAMAGE) {
            const auto dmg = static_cast<const CEventDamage*>(event);
            if (dmg->m_damageResponse.m_bHealthZero && dmg->m_bAddToEventGroup) {
                return true;
            }
        }
        break;
    }
    }
    m_SlowType = SlowDownType::DONT_WAIT;
    return false;
}

// 0x646AD0
bool CTaskSimpleCarWaitToSlowDown::ProcessPed(CPed* ped) {
    if (!m_TargetVehicle || m_SlowType == SlowDownType::DONT_WAIT) {
        return true;
    }

    if (const auto driver = m_TargetVehicle->m_pDriver) {
        if (!driver->IsPlayer() && ped == driver) {
            // Start stopping the car completely
            auto& ap = m_TargetVehicle->m_autoPilot;
            ap.SetCruiseSpeed(0);
            ap.SetCarMission(MISSION_NONE);
        }
    }

    switch (m_SlowType) {
    case SlowDownType::SLOW_ENOUGH_TO_STEP:
        return m_TargetVehicle->CanPedStepOutCar(false);
    case SlowDownType::SLOW_ENOUGH_TO_STEP_OR_JUMP:
        return m_TargetVehicle->CanPedStepOutCar(false) || m_TargetVehicle->CanPedJumpOutCar(ped);
    default:
        return false;
    }
    return false;
}

// 0x646AB0 (thiscall, un argumento de pila, `ret 4`, 21 bytes): con vehiculo objetivo (+8) no nulo llama a CPed::SetPedPositionInCar (0x5DF910, ECX = ped); devuelve true (AL = 1)
bool CTaskSimpleCarWaitToSlowDown::SetPedPosition(CPed* ped) {
    if (m_TargetVehicle) {
        ped->SetPedPositionInCar();
    }
    return true;
}
