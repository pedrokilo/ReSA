/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexObserveTrafficLights
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

#include "TaskComplexObserveTrafficLights.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleStandStill.h"
#include "TrafficLights.h"

namespace {
// 0x6317C0 (lote 157A, banco Fn157A): cuerpo del destructor de CTaskComplexObserveTrafficLights del EXE (con hook (Destructor)). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor con borrado como el `call 0x6317C0` del EXE.
__declspec(noinline) void __fastcall CTaskComplexObserveTrafficLightsDestructorBody(CTaskComplexObserveTrafficLights* self, void* /*edx*/) { self->CTaskComplexObserveTrafficLights::~CTaskComplexObserveTrafficLights(); }

// 0x637F60 (lote 157A, banco Fn157A): destructor con borrado de CTaskComplexObserveTrafficLights del EXE (ranura 0 de su vtable 0x86DF98; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x6317C0; test byte [esp+8],1; je ..; push esi;
// call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta
// puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexObserveTrafficLights* __fastcall CTaskComplexObserveTrafficLightsScalarDeletingDestructor(CTaskComplexObserveTrafficLights* self, void* /*edx*/, uint8 flags) {
    CTaskComplexObserveTrafficLightsDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote157ABody_CTaskComplexObserveTrafficLights() { return reinterpret_cast<uintptr_t>(&CTaskComplexObserveTrafficLightsDestructorBody); } // para el banco Fn157A

void CTaskComplexObserveTrafficLights::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexObserveTrafficLights, 0x86DF98, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexObserveTrafficLightsScalarDeletingDestructor, "ScalarDeletingDestructor", 0x637F60); // lote 157A (banco Fn157A)

    RH_ScopedInstall(Constructor, 0x631790);
    RH_ScopedInstall(Destructor, 0x6317C0);

    RH_ScopedVMTInstall(Clone, 0x636410);
    RH_ScopedVMTInstall(GetTaskType, 0x6317B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x6317D0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6317E0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6318D0);
    RH_ScopedVMTInstall(ControlSubTask, 0x6318E0);
}

// 0x6318E0
CTask* CTaskComplexObserveTrafficLights::ControlSubTask(CPed* ped) {
    if (CTrafficLights::LightForPeds() || !m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr)) {
        return m_pSubTask;
    } else {
        return nullptr;
    }
}

// 0x6317E0
CTask* CTaskComplexObserveTrafficLights::CreateNextSubTask(CPed* ped) {
    if (!m_pSubTask || m_pSubTask->GetTaskType() == TASK_SIMPLE_STAND_STILL) {
        return new CTaskSimpleRunAnim(ped->m_nAnimGroup, ANIM_ID_ROADCROSS, 4.0f, false);
    } else if (m_pSubTask->GetTaskType() == TASK_SIMPLE_ANIM) {
        // Como el original: 0x407180 ([3000, 5000) con 1/32768) antes de reservar la tarea
        const auto time = CGeneral::GetRandomIntInRange(3000, 5000);
        return new CTaskSimpleStandStill(time);
    }
    return nullptr;
}
