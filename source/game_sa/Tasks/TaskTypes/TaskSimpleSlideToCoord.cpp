/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleSlideToCoord
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleSlideToCoord.h"
#include "TaskSimpleStandStill.h"
#include "General.h"
#include "X87Math.h"

void CTaskSimpleSlideToCoord::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSlideToCoord, 0x86FFEC, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    // + RH_ScopedOverloadedInstall(Constructor, "NoAnim", 0x66C3E0, CTaskSimpleSlideToCoord*(CTaskSimpleSlideToCoord::*)(CVector const&, float, float));
    // + RH_ScopedOverloadedInstall(Constructor, "Anim", 0x66C450, CTaskSimpleSlideToCoord*(CTaskSimpleSlideToCoord::*)(CVector const&, float, float, char const*, char const*, int32, float, bool, int32));
    // + RH_ScopedVmtInstall(MakeAbortable, 0x66C4D0);
    RH_ScopedVMTInstall(ProcessPed, 0x66C4E0);
}

// 0x66C3E0
CTaskSimpleSlideToCoord::CTaskSimpleSlideToCoord(const CVector& slideToPos, float aimingRotation, float speed) :
    CTaskSimpleRunNamedAnim(),
    m_SlideToPos{ slideToPos },
    m_fAimingRotation{ aimingRotation },
    m_Speed{ speed },
    m_bFirstTime{ true },
    m_bRunningAnim{ false }
{
    // m_Timer not initialized
}

// 0x66C450
CTaskSimpleSlideToCoord::CTaskSimpleSlideToCoord(const CVector& slideToPos, float aimingRotation, float speed, const char* animBlockName, const char* animGroupName, uint32 animFlags, float animBlendDelta, bool bRunInSequence, uint32 endTime) :
    CTaskSimpleRunNamedAnim{ animBlockName, animGroupName, animFlags, animBlendDelta, endTime, false, bRunInSequence, false, false },
    m_SlideToPos{ slideToPos },
    m_fAimingRotation{ aimingRotation },
    m_Speed{ speed },
    m_bFirstTime{ true },
    m_bRunningAnim{ false },
    m_Timer{ -1 }
{
}

// 0x66D300
CTask* CTaskSimpleSlideToCoord::Clone() const {
    return m_bRunningAnim
        ? new CTaskSimpleSlideToCoord(m_SlideToPos, m_fAimingRotation, m_Speed, m_animName, m_animGroupName, m_animFlags, m_fBlendDelta, !!m_bRunInSequence, m_Time)
        : new CTaskSimpleSlideToCoord(m_SlideToPos, m_fAimingRotation, m_Speed);
}

// 0x66C4D0
bool CTaskSimpleSlideToCoord::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return m_bRunningAnim ? CTaskSimpleAnim::MakeAbortable(ped, priority, event) : true;
}

// 0x66C4E0
// m_Timer (+0x7C) es el instante limite en ms (-1 = aun no fijado): 2000 ms tras el primer paso si no hay animacion y 500 ms tras
// la primera pasada correcta de la animacion. El resto desliza al ped hacia m_SlideToPos y termina al vencer el plazo o al
// llegar (a menos de 0.05) con el ritmo de rotacion dentro de 0.1.
bool CTaskSimpleSlideToCoord::ProcessPed(CPed* ped) {
    bool animOk = true;
    if (m_bRunningAnim) {
        animOk = CTaskSimpleRunNamedAnim::ProcessPed(ped);
    }

    if (m_Timer == -1) {
        if (!m_bRunningAnim) {
            m_Timer = int32(CTimer::GetTimeInMS() + 2000);
        } else if (animOk) {
            m_Timer = int32(CTimer::GetTimeInMS() + 500);
        }
    }

    // Mismo bloque de "quedarse quieto" que CTaskSimpleWaitUntilPedIsOutCar.
    if (m_bFirstTime) {
        CTaskSimpleStandStill standStillTask{ STAND_STILL_TIME };
        standStillTask.ProcessPed(ped);
        if (ped->IsPlayer()) {
            ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_DEFAULT)->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr);
        }
        ped->m_fAimingRotation = m_fAimingRotation;
        m_bFirstTime = false;
    }

    // Aritmetica x87 del EXE (24 bits, sin guardar intermedios): X87Math.cpp. Solo se toca la matriz si hay que deslizar.
    const CVector& pedPos = ped->GetPosition();
    float shift[2];
    const bool reached = SlideToCoordShiftCpp(
        &pedPos.x, &m_SlideToPos.x, m_Speed,
        ped->m_matrix ? &ped->m_matrix->GetRight().x : nullptr,
        ped->m_matrix ? &ped->m_matrix->GetForward().x : nullptr,
        shift
    );
    ped->m_vecAnimMovingShiftLocal.x = shift[0];
    ped->m_vecAnimMovingShiftLocal.y = shift[1];

    if (uint32(m_Timer) < CTimer::GetTimeInMS()) {
        return true;
    }

    if (!animOk || !reached) {
        return false;
    }

    return std::abs(CGeneral::LimitRadianAngle(SlideToCoordAngleDeltaCpp(ped->m_fCurrentRotation, ped->m_fAimingRotation))) < 0.1f;
}
