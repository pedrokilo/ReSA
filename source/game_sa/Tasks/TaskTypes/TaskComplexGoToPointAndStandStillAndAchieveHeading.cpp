/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGoToPointAndStandStillAndAchieveHeading
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

#include "TaskComplexGoToPointAndStandStillAndAchieveHeading.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleGoToPoint.h"

namespace {
// 0x668D40 (lote 147A, banco Fn147A): cuerpo del destructor de CTaskComplexGoToPointAndStandStillAndAchieveHeading del EXE (con hook propio (Destructor); vtable 0x86FE58). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor
// con borrado como el `call 0x668D40` del EXE.
__declspec(noinline) void __fastcall CTaskComplexGoToPointAndStandStillAndAchieveHeadingDestructorBody(CTaskComplexGoToPointAndStandStillAndAchieveHeading* self, void* /*edx*/) { self->CTaskComplexGoToPointAndStandStillAndAchieveHeading::~CTaskComplexGoToPointAndStandStillAndAchieveHeading(); }

// 0x66DFB0 (lote 147A, banco Fn147A): destructor con borrado de CTaskComplexGoToPointAndStandStillAndAchieveHeading del EXE (ranura 0 de su vtable 0x86FE58; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x668D40; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexGoToPointAndStandStillAndAchieveHeading* __fastcall CTaskComplexGoToPointAndStandStillAndAchieveHeadingScalarDeletingDestructor(CTaskComplexGoToPointAndStandStillAndAchieveHeading* self, void* /*edx*/, uint8 flags) {
    CTaskComplexGoToPointAndStandStillAndAchieveHeadingDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote147ABody_CTaskComplexGoToPointAndStandStillAndAchieveHeading() { return reinterpret_cast<uintptr_t>(&CTaskComplexGoToPointAndStandStillAndAchieveHeadingDestructorBody); } // para el banco Fn147A

void CTaskComplexGoToPointAndStandStillAndAchieveHeading::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToPointAndStandStillAndAchieveHeading, 0x86FE58, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexGoToPointAndStandStillAndAchieveHeadingScalarDeletingDestructor, "ScalarDeletingDestructor", 0x66DFB0); // lote 147A (banco Fn147A)

    RH_ScopedInstall(Constructor, 0x668CD0);
    RH_ScopedInstall(Destructor, 0x668D40);
    RH_ScopedInstall(CreateSubTask, 0x668D50); // lote 180A (sin validar)

    RH_ScopedVMTInstall(Clone, 0x66CFD0);
    RH_ScopedVMTInstall(GetTaskType, 0x668D30);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66DFD0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x66E030);
    RH_ScopedVMTInstall(ControlSubTask, 0x668E80);
}

// 0x668CD0
CTaskComplexGoToPointAndStandStillAndAchieveHeading::CTaskComplexGoToPointAndStandStillAndAchieveHeading(eMoveState moveState, const CVector& targetPos, float angle, float radius, float changeRateMult, float maxHeading)
    : CTaskComplex()
{
    m_MoveState = moveState;
    m_TargetPos = targetPos;
    m_Radius = radius;
    m_MaxHeading = maxHeading;
    m_nFlags = m_nFlags | 1; // todo: flags
    m_Angle = angle;
    m_ChangeRateMult = changeRateMult;
}

// 0x66DFD0
CTask* CTaskComplexGoToPointAndStandStillAndAchieveHeading::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_GO_TO_POINT:
        return CreateSubTask(TASK_SIMPLE_STAND_STILL);
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return CreateSubTask(TASK_FINISHED);
    case TASK_SIMPLE_STAND_STILL:
        return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING);
    default:
        return nullptr;
    }
}

// 0x668D50 (lote 180A, sin validar; thiscall, un argumento de pila, `ret 4`, 292 bytes con marco SEH que solo desenrolla y aqui se omite): crea la subtarea segun el tipo; en todos los casos reserva en el pool de tareas (0x61A5A0) y, si la
// reserva es nula, devuelve nulo sin llamar al constructor. TASK_SIMPLE_GO_TO_POINT (0x384): 0x30 bytes, CTaskSimpleGoToPoint (0x667CD0) con (m_MoveState +0xC, &m_TargetPos +0x10, m_Radius +0x1C, false, false). TASK_SIMPLE_STAND_STILL (0xCB):
// 0x20 bytes, CTaskSimpleStandStill (0x62F310) con (0, false, false, 8.0f). TASK_SIMPLE_ACHIEVE_HEADING (0x386): 0x18 bytes, CTaskSimpleAchieveHeading (0x667E20) con (m_Angle +0x20, m_ChangeRateMult +0x24, m_MaxHeading +0x28). Cualquier otro
// tipo (TASK_FINISHED 0x516 incluido) devuelve nulo.
CTask* CTaskComplexGoToPointAndStandStillAndAchieveHeading::CreateSubTask(eTaskType taskType) {
    // Como el original: cada rama reserva con CTask::operator new (0x61A5A0); sin memoria devuelve nulo sin construir; si no, devuelve el EAX del constructor enganchado de la subtarea
    switch (taskType) {
    case TASK_SIMPLE_ACHIEVE_HEADING: {
        auto* const memory = static_cast<CTaskSimpleAchieveHeading*>(CTask::operator new(sizeof(CTaskSimpleAchieveHeading)));
        return memory ? memory->Constructor(m_Angle, m_ChangeRateMult, m_MaxHeading) : nullptr;
    }
    case TASK_SIMPLE_STAND_STILL: {
        auto* const memory = static_cast<CTaskSimpleStandStill*>(CTask::operator new(sizeof(CTaskSimpleStandStill)));
        return memory ? memory->Constructor(0, false, false, 8.0f) : nullptr;
    }
    case TASK_SIMPLE_GO_TO_POINT: {
        auto* const memory = static_cast<CTaskSimpleGoToPoint*>(CTask::operator new(sizeof(CTaskSimpleGoToPoint)));
        return memory ? memory->Constructor(m_MoveState, m_TargetPos, m_Radius, false, false) : nullptr;
    }
    default:
        return nullptr;
    }
}

// 0x66E030
CTask* CTaskComplexGoToPointAndStandStillAndAchieveHeading::CreateFirstSubTask(CPed* ped) {
    // Como el original: apaga el bit 0 de las banderas y crea el desplazamiento
    m_nFlags &= 0xFE; // todo: flags
    return CreateSubTask(TASK_SIMPLE_GO_TO_POINT);
}

// 0x668E80
CTask* CTaskComplexGoToPointAndStandStillAndAchieveHeading::ControlSubTask(CPed* ped) {
    // Como el original: con el bit 0 pendiente se crea la primera subtarea (hueco 9 de la vtable)
    if (m_nFlags & 1) { // todo: flags
        return CreateFirstSubTask(ped);
    }
    // Como el original: la subtarea de desplazamiento sigue el estado de movimiento de esta tarea
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_GO_TO_POINT) {
        static_cast<CTaskSimpleGoToPoint*>(m_pSubTask)->m_moveState = m_MoveState;
    }
    return m_pSubTask;
}
