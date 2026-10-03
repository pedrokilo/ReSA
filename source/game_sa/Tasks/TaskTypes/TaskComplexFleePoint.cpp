/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexFleePoint
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexFleePoint.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskComplexLeaveAnyCar.h"
#include "TaskComplexSequence.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleTired.h"
#include "Ragdoll/IKChainManager.h"

void CTaskComplexFleePoint::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFleePoint, 0x86f6cc, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65B390);
    RH_ScopedInstall(Destructor, 0x65B410);

    RH_ScopedInstall(CreateSubTask, 0x65B460);
    RH_ScopedInstall(SetFleePosition, 0x65B7B0);
    RH_ScopedInstall(ComputeTargetPoint, 0x65B820);

    RH_ScopedVMTInstall(Clone, 0x65CDE0);
    RH_ScopedVMTInstall(GetTaskType, 0x65B400);
    RH_ScopedVMTInstall(MakeAbortable, 0x65B420);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65D150);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65D100);
    RH_ScopedVMTInstall(ControlSubTask, 0x65B660);
}

// 0x65B390
CTaskComplexFleePoint::CTaskComplexFleePoint(CVector const& fleeFromPos, bool pedScream, float safeDist, int32 durationMs) :
    m_fleeFromPos{fleeFromPos},
    m_bScream{pedScream},
    m_safeDist{safeDist},
    m_durationMs{durationMs}
{
    // Como el original: el temporizador arranca con cualquier duracion distinta de -1 (no solo >= 0)
    if (m_durationMs != -1) {
        m_timer.m_nStartTime = CTimer::GetTimeInMS();
        m_timer.m_nInterval  = m_durationMs;
        m_timer.m_bStarted   = true;
    }
}

CTaskComplexFleePoint::CTaskComplexFleePoint(const CTaskComplexFleePoint& o) :
    CTaskComplexFleePoint{o.m_fleeFromPos, o.m_bScream, o.m_safeDist, o.m_durationMs}
{
}

// 0x65B460
CTask * CTaskComplexFleePoint::CreateSubTask(eTaskType taskType) {
    switch (taskType) {
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        return new CTaskComplexGoToPointAndStandStill{ PEDMOVE_SPRINT, m_fleeToPos, 0.5f, 2.f, false, false };
    case TASK_SIMPLE_GO_TO_POINT:
        return new CTaskSimpleGoToPoint{ PEDMOVE_SPRINT, m_fleeToPos, 0.5f, false, false };
    case TASK_COMPLEX_LEAVE_ANY_CAR:
        return new CTaskComplexLeaveAnyCar{ 0, true, false };
    case TASK_COMPLEX_SEQUENCE: {
        const auto seq = new CTaskComplexSequence; // Como el original: sin inicializacion por valor (relleno 0x39-0x3B sin escribir)
        seq->AddTask(new CTaskSimpleRunAnim{ ANIM_GROUP_DEFAULT, ANIM_ID_FLEE_LKAROUND_01, 4.0f, false });
        seq->AddTask(new CTaskSimpleTired{ 2000u });
        return seq;
    }
    case TASK_FINISHED:
        return nullptr;
    default:
        return nullptr; // Como el original: tipos desconocidos devuelven nullptr
    }
}

/*!
* @addr 0x65B7B0
* @brief Set a new flee position
*/
void CTaskComplexFleePoint::SetFleePosition(CVector const& fleePos, float safeDistance, bool scream) {
    // Como el original: tambien cambia si solo cambia la distancia segura (comparaciones con NaN cuentan como distintas)
    if (m_fleeFromPos.x != fleePos.x || m_fleeFromPos.y != fleePos.y || m_fleeFromPos.z != fleePos.z || m_safeDist != safeDistance) {
        m_fleeFromPos = fleePos;
        m_safeDist = safeDistance;
        m_newFleePosWasSet = true;
    }
    m_bScream = scream;
}

/*!
* @addr 0x65B820
* @brief Compute random `m_fleeToPos`
*/
void CTaskComplexFleePoint::ComputeTargetPoint(CPed const* ped) {
    // Como el original: direccion 3D con z = 0 normalizada con CVector::Normalise, angulo y separacion con la version
    // float de GetRandomNumberInRange (en este orden), fcos/fsin x87 a precision completa y desplazamientos
    // ((x*cos + y*sin) * sep, (x*cos - y*sin) * sep)
    const auto& pedPos = ped->GetPosition();
    CVector dir{ pedPos.x - m_fleeFromPos.x, pedPos.y - m_fleeFromPos.y, 0.f };
    dir.Normalise();

    const float angle = CGeneral::GetRandomNumberInRange<float>(-0.33f, 0.33f);
    float cosAng, sinAng;
    __asm {
        fld angle
        fld st(0)
        fcos
        fstp cosAng
        fsin
        fstp sinAng
    }
    const float spread = CGeneral::GetRandomNumberInRange<float>(3.f, 6.f);

    const float ySin = dir.y * sinAng;
    const float xCos = dir.x * cosAng;
    const float offX = (xCos + ySin) * spread;
    const float offY = (xCos - ySin) * spread;

    const auto& pos = ped->GetPosition();
    m_fleeToPos.x = offX + pos.x;
    m_fleeToPos.y = offY + pos.y;
    m_fleeToPos.z = pos.z;
}

 // 0x65B420
bool CTaskComplexFleePoint::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == ABORT_PRIORITY_LEISURE) {
        m_durationMs = -1;

        // Leads to running `TASK_SEQUENCE` => `TASK_FINISHED`
        // TODO: Below is probably something inlined, so unline it...
        m_timer.m_nStartTime = CTimer::GetTimeInMS();
        m_timer.m_nInterval = -1;
        m_timer.m_bStarted = 1;
    }
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x65D150
CTask* CTaskComplexFleePoint::CreateNextSubTask(CPed* ped) {
    (void)m_pSubTask->GetTaskType(); // Como el original: GetTaskType se consulta dos veces
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL:
        return CreateSubTask(TASK_COMPLEX_SEQUENCE);
    case TASK_COMPLEX_SEQUENCE:
        return CreateSubTask(TASK_FINISHED);
    case TASK_COMPLEX_LEAVE_ANY_CAR:
        ComputeTargetPoint(ped); // Como el original: se calcula el destino antes de ir al punto
        return CreateSubTask(TASK_SIMPLE_GO_TO_POINT);
    case TASK_SIMPLE_GO_TO_POINT: {
        ComputeTargetPoint(ped);
        // Como el original: distancia del destino al punto del que se huye, (z*z + y*y) + x*x; solo si supera
        // estrictamente la distancia segura se para (empate y NaN siguen huyendo)
        const float dx     = m_fleeToPos.x - m_fleeFromPos.x;
        const float dy     = m_fleeToPos.y - m_fleeFromPos.y;
        const float dz     = m_fleeToPos.z - m_fleeFromPos.z;
        const float distSq = dz * dz + dy * dy + dx * dx;
        return CreateSubTask(
            m_safeDist * m_safeDist < distSq
                ? TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL // This will eventually create a `TASK_COMPLEX_SEQUENCE` which then leads to `TASK_FINISHED`
                : TASK_SIMPLE_GO_TO_POINT                  // Go to `m_fleeToPos`
        );
    }
    default:
        return nullptr;
    }
}

// 0x65D100
CTask* CTaskComplexFleePoint::CreateFirstSubTask(CPed* ped) {
    if (ped->IsInVehicle()) {
        return CreateSubTask(TASK_COMPLEX_LEAVE_ANY_CAR);
    }

    ComputeTargetPoint(ped);
    return CreateSubTask(TASK_SIMPLE_GO_TO_POINT);
}

// 0x65B660
CTask* CTaskComplexFleePoint::ControlSubTask(CPed* ped) {
    CTask* subTask = m_pSubTask;
    // Como el original: GetTaskType se consulta dos veces y solo se aborta la mirada si el peaton esta mirando
    if (m_pSubTask->GetTaskType() != TASK_SIMPLE_GO_TO_POINT && m_pSubTask->GetTaskType() != TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) {
        if (g_ikChainMan.IsLooking(ped)) {
            g_ikChainMan.AbortLookAt(ped, 250);
        }
        return m_pSubTask;
    }

    if (m_bScream) {
        ped->Say(CTX_GLOBAL_PAIN_PANIC, 0, 0.1f);
    }

    // Como el original: primero el posible cambio de subtarea y despues la mirada (el orden de rand importa)
    if (m_newFleePosWasSet) {
        m_newFleePosWasSet = false;
        // Como el original: el temporizador arranca sin comprobar la duracion
        m_timer.m_nStartTime = CTimer::GetTimeInMS();
        m_timer.m_nInterval  = m_durationMs;
        m_timer.m_bStarted   = true;
        subTask = CreateFirstSubTask(ped); // Do it all over again :D
    } else if (m_timer.IsOutOfTime()) {
        subTask = CreateSubTask(TASK_COMPLEX_SEQUENCE);
    }

    if (!g_ikChainMan.IsLooking(ped)) {
        // Como el original: (int)(rand() * (1/32768) * 100) <= 5
        const auto chance = (int32)((float)CGeneral::GetRandomNumber() * (1.f / 32768.f) * 100.f);
        if (chance <= 5) {
            g_ikChainMan.LookAt("TaskFleePoint", ped, nullptr, 2000, eBoneTag::BONE_UNKNOWN, &m_fleeFromPos, false, 0.25f, 500, 3, false);
        }
    }

    return subTask;
}
