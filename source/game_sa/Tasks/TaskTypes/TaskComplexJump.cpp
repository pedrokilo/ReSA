/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexJump
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

#include "TaskComplexJump.h"

#include "TaskSimpleJump.h"
#include "TaskSimpleHitHead.h"
#include "TaskComplexInAirAndLand.h"
#include "TaskSimpleClimb.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
}

namespace {
// 0x67A060 (lote 115A, banco Fn115A): cuerpo no virtual de ~CTaskComplexJump del EXE (thiscall, sin argumentos de pila, `ret`)
// Deja la vtable de CTaskComplexJump del EXE (0x870570) y salta a CTaskComplex::Destructor (0x61A3D0). El EXE lo envuelve en un marco SEH que solo desenrolla.
void __fastcall JumpDestructorBody(CTaskComplexJump* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x870570;
    CTaskComplexBody::Destructor(self, nullptr);
}

// 0x67D960 (lote 156A, banco Fn156A; el listado decia CTaskSimpleJump): destructor con borrado de CTaskComplexJump del EXE (ranura 0 de su vtable 0x870570; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x67A060; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x67A060, con hook),
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskComplexJump* __fastcall JumpScalarDeletingDestructor(CTaskComplexJump* self, void* /*edx*/, uint8 flags) {
    JumpDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskComplexJump::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexJump, 0x870570, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(JumpDestructorBody, "Destructor", 0x67A060);
    RH_ScopedNamedGlobalInstall(JumpScalarDeletingDestructor, "ScalarDeletingDestructor", 0x67D960); // lote 156A (banco Fn156A)
    RH_ScopedInstall(Constructor, 0x67A030);
    RH_ScopedInstall(CreateSubTask, 0x67D980);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x67FD10);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x67FC00);
    RH_ScopedVMTInstall(Clone, 0x67C5A0);
    RH_ScopedVMTInstall(MakeAbortable, 0x67A070);
}

CTaskComplexJump* CTaskComplexJump::Constructor(eForceClimb jumpType) {
    this->CTaskComplexJump::CTaskComplexJump(jumpType);
    return this;
}

// 0x67A030
CTaskComplexJump::CTaskComplexJump(eForceClimb forceClimb) : CTaskComplex() {
    m_ForceClimb = forceClimb;
    m_UsePlayerLaunchForce = false;
}

// 0x67C5A0
CTask* CTaskComplexJump::Clone() const {
    auto newTask = new CTaskComplexJump(m_ForceClimb);
    newTask->m_UsePlayerLaunchForce = this->m_UsePlayerLaunchForce;
    return newTask;
}

// 0x67FD10
CTask* CTaskComplexJump::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_JUMP, ped);
}

// 0x67FC00
CTask* CTaskComplexJump::CreateNextSubTask(CPed* ped) {
    eTaskType subTaskType = m_pSubTask->GetTaskType();

    if (subTaskType == TASK_SIMPLE_CLIMB)
        return CreateSubTask(ped->bIsInTheAir ? TASK_COMPLEX_IN_AIR_AND_LAND : TASK_FINISHED, ped);

    if (subTaskType == TASK_SIMPLE_HIT_HEAD || subTaskType == TASK_COMPLEX_IN_AIR_AND_LAND) {
        ped->bIsLanding = false;
        return CreateSubTask(TASK_FINISHED, ped);
    }

    if (subTaskType == TASK_SIMPLE_JUMP) {
        auto jumpTask = reinterpret_cast<CTaskSimpleJump*>(m_pSubTask);

        if (!jumpTask->m_bLaunchAnimStarted) {
            ped->bIsLanding = false;
            return CreateSubTask(TASK_FINISHED, ped);
        } if (jumpTask->m_bIsJumpBlocked) {
            ped->bIsLanding = true;
            return CreateSubTask(TASK_SIMPLE_HIT_HEAD, ped);
        } else if (jumpTask->m_pClimbEntity && m_ForceClimb != eForceClimb::DISABLE) {
            ped->bIsInTheAir = true;
            return CreateSubTask(TASK_SIMPLE_CLIMB, ped);
        } else {
            ped->bIsInTheAir = true;
            return CreateSubTask(TASK_COMPLEX_IN_AIR_AND_LAND, ped);
        }
    }

    return nullptr;
}

// 0x67A070
bool CTaskComplexJump::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_URGENT && event) {
        if (event->GetEventType() == EVENT_DAMAGE) {
            const auto pDamageEvent = static_cast<const CEventDamage*>(event);
            if (pDamageEvent->m_weaponType == WEAPON_FALL && pDamageEvent->m_damageResponse.m_bHealthZero && pDamageEvent->m_bAddToEventGroup) {
                ped->bIsInTheAir = false;
                ped->bIsLanding = false;
                return true;
            }
        } else if (event->GetEventType() == EVENT_DEATH) {
            ped->bIsInTheAir = false;
            ped->bIsLanding = false;
            return true;
        }
    }

    if (m_pSubTask->MakeAbortable(ped, priority, event)) {
        ped->bIsInTheAir = false;
        ped->bIsLanding = false;
        return true;
    }

    return false;
}

// 0x67D980
CTask* CTaskComplexJump::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_HIT_HEAD:
        return new CTaskSimpleHitHead();
    case TASK_FINISHED:
        ped->bIsInTheAir = false;
        return nullptr;
    case TASK_SIMPLE_JUMP: {
        auto t = new CTaskSimpleJump(m_ForceClimb == eForceClimb::FORCE);
        if (m_UsePlayerLaunchForce || CPedGroups::IsInPlayersGroup(ped))
            t->m_bHighJump = true;
        return t;
    }
    case TASK_SIMPLE_CLIMB: {
        if (const auto* const tJump = notsa::dyn_cast_if_present<CTaskSimpleJump>(m_pSubTask)) {
            return new CTaskSimpleClimb(
                tJump->m_pClimbEntity,
                tJump->m_vecClimbPos,
                tJump->m_fClimbAngle,
                tJump->m_nClimbSurfaceType,
                tJump->m_vecClimbPos.z - ped->GetPosition().z < CTaskSimpleClimb::ms_fMinForStretchGrab ? CLIMB_PULLUP : CLIMB_GRAB,
                m_ForceClimb == eForceClimb::FORCE
            );
        }
        return new CTaskComplexInAirAndLand(true, false);
    }
    case TASK_COMPLEX_IN_AIR_AND_LAND: {
        auto t = new CTaskComplexInAirAndLand(true, false);
        if (const auto tClimb = notsa::dyn_cast_if_present<CTaskSimpleClimb>(m_pSubTask)) {
            t->m_bInvalidClimb = tClimb->GetIsInvalidClimb();
        }
        return t;
    }
    default:
        return nullptr;
    }
}
