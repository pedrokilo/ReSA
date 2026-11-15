/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGoToPointAndStandStillTimed
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

#include "TaskComplexGoToPointAndStandStillTimed.h"
#include "PedPlacement.h"

void GoToPointAndStandStillExeDestructorBody(CTaskComplexGoToPointAndStandStill* self); // TaskComplexGoToPointAndStandStill.cpp: 0x668630

namespace {
// 0x66DC70 (lote 198A, sin validar; el listado decia CTaskComplexGoToPointAndStandStill: es la ranura 0 de la vtable 0x86FDD4 de CTaskComplexGoToPointAndStandStillTimed; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx;
// call 0x668630; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: la clase no tiene cuerpo de destructor propio y llama directamente al de la base (0x668630, con hook:
// deja la vtable 0x86FDA8 y sigue con ~CTaskComplex), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S.
CTaskComplexGoToPointAndStandStillTimed* __fastcall GoToPointAndStandStillTimedScalarDeletingDestructor(CTaskComplexGoToPointAndStandStillTimed* self, void* /*edx*/, uint8 flags) {
    GoToPointAndStandStillExeDestructorBody(self);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskComplexGoToPointAndStandStillTimed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToPointAndStandStillTimed, 0x86FDD4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x6685E0);
    RH_ScopedNamedGlobalInstall(GoToPointAndStandStillTimedScalarDeletingDestructor, "ScalarDeletingDestructor", 0x66DC70); // lote 198A (sin validar)
    RH_ScopedVMTInstall(Clone, 0x66CF30);
    RH_ScopedVMTInstall(StopTimer, 0x6686A0);
    RH_ScopedVMTInstall(MakeAbortable, 0x668640);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x66DC90);
    RH_ScopedVMTInstall(ControlSubTask, 0x66DCE0);
}

CTaskComplexGoToPointAndStandStillTimed::CTaskComplexGoToPointAndStandStillTimed(eMoveState moveState, const CVector& targetPoint, float fRadius, float fMoveStateRadius, int32 time)
    : CTaskComplexGoToPointAndStandStill(moveState, targetPoint, fRadius, fMoveStateRadius, false, false)
{
    m_nTime = time;
}

CTaskComplexGoToPointAndStandStillTimed::~CTaskComplexGoToPointAndStandStillTimed() {
    // nothing here
}

// 0x66CF30
CTask* CTaskComplexGoToPointAndStandStillTimed::Clone() const {
    return new CTaskComplexGoToPointAndStandStillTimed(m_moveState, m_vecTargetPoint, m_fRadius, m_fMoveStateRadius, m_nTime);
}

// 0x6686A0
void CTaskComplexGoToPointAndStandStillTimed::StopTimer(const CEvent* event) {
    if (!CEventHandler::IsTemporaryEvent(*event))
        m_timer.Pause();
}

// 0x668640
bool CTaskComplexGoToPointAndStandStillTimed::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    bool bSubTaskAbortable = m_pSubTask->MakeAbortable(ped, priority, event);
    if (bSubTaskAbortable && priority == ABORT_PRIORITY_URGENT && (!event || !CEventHandler::IsTemporaryEvent(*event))) {
        m_timer.Pause();
    }
    return bSubTaskAbortable;
}

// 0x66DC90
CTask* CTaskComplexGoToPointAndStandStillTimed::CreateFirstSubTask(CPed* ped) {
    m_timer.Start(m_nTime);
    return CTaskComplexGoToPointAndStandStill::CreateFirstSubTask(ped);
}

// 0x66DCE0
CTask* CTaskComplexGoToPointAndStandStillTimed::ControlSubTask(CPed* ped) {
    if (m_timer.Reset()) {
        if (m_timer.IsOutOfTime() && m_pSubTask->GetTaskType() != TASK_SIMPLE_STAND_STILL) {
            const auto [pos, foundZ] = CPedPlacement::FindZCoorForPed(m_vecTargetPoint);
            if (foundZ) {
                m_vecTargetPoint = pos;
                ped->SetPosn(m_vecTargetPoint);
            }
        }
    }
    return CTaskComplexGoToPointAndStandStill::ControlSubTask(ped);
}
