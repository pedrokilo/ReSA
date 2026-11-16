/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexKillPedOnFootStealth
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
#include "TaskComplexKillPedOnFootStealth.h"
#include "TaskComplexInvestigateDisturbance.h"
#include "TaskComplexSequence.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleScratchHead.h"

// 0x626A80 (lote 193A, sin validar; el listado decia CTaskSimpleThrowProjectile: es la ranura 0 de la vtable 0x86D8C0 de CTaskComplexKillPedOnFootStealth; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x620FF0;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x620FF0, el destructor de C++ con el hook Destructor), libera `this` con
// CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskComplexKillPedOnFootStealth* __fastcall KillPedOnFootStealthScalarDeletingDestructor(CTaskComplexKillPedOnFootStealth* self, void* edx, uint8 flags) {
    self->Destructor(); // el destructor enganchado (para que el banco lo sustituya por el mismo falso en el EXE y en la DLL)
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}

void CTaskComplexKillPedOnFootStealth::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillPedOnFootStealth, 0x86d8c0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x620F70);
    RH_ScopedInstall(Destructor, 0x620FF0);
    RH_ScopedNamedGlobalInstall(KillPedOnFootStealthScalarDeletingDestructor, "ScalarDeletingDestructor", 0x626A80); // lote 193A (sin validar)

    RH_ScopedVMTInstall(Clone, 0x623400);
    RH_ScopedVMTInstall(GetTaskType, 0x620FE0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62B790);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62B810);
    RH_ScopedVMTInstall(ControlSubTask, 0x62B840);
}

// 0x620F70
CTaskComplexKillPedOnFootStealth::CTaskComplexKillPedOnFootStealth(CPed* target) :
    CTaskComplexKillPedOnFoot{target}
{
}

// NOTSA (For 0x623400)
// Como el original: Clone construye la copia con el objetivo del original (o.m_target)
CTaskComplexKillPedOnFootStealth::CTaskComplexKillPedOnFootStealth(const CTaskComplexKillPedOnFootStealth& o) :
    CTaskComplexKillPedOnFootStealth{o.m_target}
{
}

// 0x62B790
CTask* CTaskComplexKillPedOnFootStealth::CreateNextSubTask(CPed* ped) {
    // Como el original: dos llamadas a GetTaskType (SEQUENCE primero) y después la prueba del tiempo
    if (m_pSubTask) {
        if (m_pSubTask->GetTaskType() == TASK_COMPLEX_SEQUENCE || m_pSubTask->GetTaskType() == TASK_COMPLEX_INVESTIGATE_DISTURBANCE) {
            if (m_targetUnreachableTimeMs == (uint32)-1) {
                return CTaskComplexKillPedOnFoot::CreateFirstSubTask(ped);
            }
        }
    }
    return CreateSubTask(TASK_FINISHED, ped);
}

// 0x62B810
CTask* CTaskComplexKillPedOnFootStealth::CreateFirstSubTask(CPed* ped) {
    ped->Say(CTX_GLOBAL_STEALTH_DEF_SIGHTING);
    return CTaskComplexKillPedOnFoot::CreateFirstSubTask(ped);
}

// 0x62B840
CTask* CTaskComplexKillPedOnFootStealth::ControlSubTask(CPed* ped) {
    if (!m_target) {
        return CTaskComplexKillPedOnFoot::ControlSubTask(ped);
    }

    const bool canTarget = CPedGeometryAnalyser::CanPedTargetPed(*ped, *m_target, true);
    if (m_targetUnreachableTimeMs == (uint32)-1) {
        if (canTarget) {
            m_targetUnreachableTimeMs = (uint32)-1;
            m_targetPos               = m_target->GetPosition();
        } else {
            m_targetUnreachableTimeMs = CTimer::GetTimeInMS(); // State is changing (Can't target the target anymore)
        }
    } else if (canTarget) {
        // Como el original: dirección objetivo - peatón, normalizada; producto (z + y) + x;
        // CanSeeEntityWithLights se llama antes de comparar el producto y ambas pruebas son estrictas
        CVector dir = m_target->GetPosition() - ped->GetPosition();
        dir.Normalise();
        const auto& fwd = ped->GetForward();
        const float dot = dir.z * fwd.z + dir.y * fwd.y + dir.x * fwd.x;
        if (ped->GetIntelligence()->CanSeeEntityWithLights(m_target, true) > 0.f && dot > CPedAcquaintanceScanner::ms_fThresholdDotProduct) {
            m_targetUnreachableTimeMs = (uint32)-1;
            m_targetPos               = m_target->GetPosition();
        }
    }

    if (m_targetUnreachableTimeMs == (uint32)-1) { // 0x62b98d
        if (m_pSubTask) {
            if (m_pSubTask->GetTaskType() == TASK_COMPLEX_INVESTIGATE_DISTURBANCE || m_pSubTask->GetTaskType() == TASK_COMPLEX_SEQUENCE) {
                if (m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
                    return CTaskComplexKillPedOnFoot::CreateFirstSubTask(ped);
                }
            }
        }
        return m_pSubTask;
    }

    // Como el original: diferencia con signo (jle)
    if ((int32)(CTimer::GetTimeInMS() - m_targetUnreachableTimeMs) <= 5'000) {
        return m_pSubTask;
    }

    ped->Say(CTX_GLOBAL_STEALTH_NOTHING_THERE);

    // Como el original: con z > -9999,9 investiga; con z <= -9999,9 (no NaN) secuencia si la subtarea no es ya una secuencia
    const float z = m_targetPos.z;
    if (z > -9999.9004f) {
        if (m_pSubTask && m_pSubTask->GetTaskType() != TASK_COMPLEX_INVESTIGATE_DISTURBANCE && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
            return new CTaskComplexInvestigateDisturbance{ m_targetPos, m_target };
        }
    } else if (z <= -9999.9004f) {
        if (m_pSubTask && m_pSubTask->GetTaskType() != TASK_COMPLEX_SEQUENCE && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
            auto* seq = new CTaskComplexSequence; // Como el original: sin inicialización por valor
            seq->AddTask(new CTaskSimpleStandStill{1000, false, false, 8.f});
            seq->AddTask(new CTaskSimpleRunAnim{ped->m_nAnimGroup, ANIM_ID_ROADCROSS, 4.f, false});
            seq->AddTask(new CTaskSimpleScratchHead{});
            return seq;
        }
    }

    return m_pSubTask;
}
