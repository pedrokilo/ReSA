/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexCrossRoadLookAndAchieveHeading
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

#include "TaskComplexCrossRoadLookAndAchieveHeading.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleAchieveHeading.h"

namespace {
// 0x631B50 (lote 157A, banco Fn157A): cuerpo del destructor de CTaskComplexCrossRoadLookAndAchieveHeading del EXE (con hook (Destructor)). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor con borrado como el `call 0x631B50` del EXE.
__declspec(noinline) void __fastcall CTaskComplexCrossRoadLookAndAchieveHeadingDestructorBody(CTaskComplexCrossRoadLookAndAchieveHeading* self, void* /*edx*/) { self->CTaskComplexCrossRoadLookAndAchieveHeading::~CTaskComplexCrossRoadLookAndAchieveHeading(); }

// 0x637FA0 (lote 157A, banco Fn157A): destructor con borrado de CTaskComplexCrossRoadLookAndAchieveHeading del EXE (ranura 0 de su vtable 0x86DFF0; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x631B50; test byte [esp+8],1; je ..; push esi;
// call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta
// puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexCrossRoadLookAndAchieveHeading* __fastcall CTaskComplexCrossRoadLookAndAchieveHeadingScalarDeletingDestructor(CTaskComplexCrossRoadLookAndAchieveHeading* self, void* /*edx*/, uint8 flags) {
    CTaskComplexCrossRoadLookAndAchieveHeadingDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote157ABody_CTaskComplexCrossRoadLookAndAchieveHeading() { return reinterpret_cast<uintptr_t>(&CTaskComplexCrossRoadLookAndAchieveHeadingDestructorBody); } // para el banco Fn157A

void CTaskComplexCrossRoadLookAndAchieveHeading::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCrossRoadLookAndAchieveHeading, 0x86DFF0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexCrossRoadLookAndAchieveHeadingScalarDeletingDestructor, "ScalarDeletingDestructor", 0x637FA0); // lote 157A (banco Fn157A)

    RH_ScopedInstall(Constructor, 0x631AE0);
    RH_ScopedInstall(Destructor, 0x631B50);
    RH_ScopedInstall(CreateSubTask, 0x631B60);

    RH_ScopedVMTInstall(Clone, 0x636500);
    RH_ScopedVMTInstall(GetTaskType, 0x631B10);
    RH_ScopedVMTInstall(MakeAbortable, 0x631B20);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x631C40);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x631C90);
    RH_ScopedVMTInstall(ControlSubTask, 0x631CB0);
}

// 0x631AE0
CTaskComplexCrossRoadLookAndAchieveHeading::CTaskComplexCrossRoadLookAndAchieveHeading(int32 durationInMs, float fTargetHeading) : CTaskComplex() {
    m_nDuration = durationInMs;
    m_fTargetHeading = fTargetHeading;
}

// 0x631B50
CTaskComplexCrossRoadLookAndAchieveHeading::~CTaskComplexCrossRoadLookAndAchieveHeading() {
    // NOP
}

// 0x631C40
CTask* CTaskComplexCrossRoadLookAndAchieveHeading::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_ANIM:
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateSubTask(TASK_SIMPLE_ANIM, ped);
    default:
        return nullptr;
    }
}

// 0x631B60
CTask* CTaskComplexCrossRoadLookAndAchieveHeading::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_ANIM:
        return new CTaskSimpleRunAnim(ped->m_nAnimGroup, ANIM_ID_ROADCROSS, 4.0f, false);
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return new CTaskSimpleAchieveHeading(m_fTargetHeading, 0.5f, 0.2f);
    case TASK_FINISHED:
        return nullptr;
    default:
        return nullptr;
    }
}

// 0x631B20
bool CTaskComplexCrossRoadLookAndAchieveHeading::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_URGENT || priority == ABORT_PRIORITY_IMMEDIATE)
        return m_pSubTask->MakeAbortable(ped, priority, event);

    return false;
}

CTaskComplexCrossRoadLookAndAchieveHeading* CTaskComplexCrossRoadLookAndAchieveHeading::Constructor(int32 durationInMs, float fTargetHeading) {
    this->CTaskComplexCrossRoadLookAndAchieveHeading::CTaskComplexCrossRoadLookAndAchieveHeading(durationInMs, fTargetHeading);
    return this;
}
