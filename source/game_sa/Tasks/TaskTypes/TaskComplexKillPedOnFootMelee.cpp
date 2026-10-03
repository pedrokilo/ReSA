/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexKillPedOnFootMelee
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexKillPedOnFootMelee.h"
#include <InterestingEvents.h>
#include "TaskSimpleFight.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.h"
#include "SeekEntity/TaskComplexSeekEntity.h"
#include "TaskSimpleFightingControl.h"
#include "TaskSimplePause.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexSequence.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleShakeFist.h"

void CTaskComplexKillPedOnFootMelee::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillPedOnFootMelee, 0x86d8ec, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x621000);
    RH_ScopedInstall(Destructor, 0x621090);

    RH_ScopedInstall(CalculateSearchPositionAndRanges, 0x621100);
    RH_ScopedInstall(CreateSubTask, 0x626AC0);

    RH_ScopedVMTInstall(Clone, 0x623460);
    RH_ScopedVMTInstall(GetTaskType, 0x621080);
    RH_ScopedVMTInstall(MakeAbortable, 0x6210F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62BC10);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62BE30);
    RH_ScopedVMTInstall(ControlSubTask, 0x626D90);
}

// 0x621000
CTaskComplexKillPedOnFootMelee::CTaskComplexKillPedOnFootMelee(CPed* target) :
    m_target{target}
{
    CEntity::SafeRegisterRef(m_target);
}

// NOTSA (For 0x623460)
CTaskComplexKillPedOnFootMelee::CTaskComplexKillPedOnFootMelee(const CTaskComplexKillPedOnFootMelee& o) :
    CTaskComplexKillPedOnFootMelee{o.m_target}
{
}

// 0x621090
CTaskComplexKillPedOnFootMelee::~CTaskComplexKillPedOnFootMelee() {
    CEntity::SafeCleanUpRef(m_target);
}

// 0x6210F0
bool CTaskComplexKillPedOnFootMelee::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x621100
void CTaskComplexKillPedOnFootMelee::CalculateSearchPositionAndRanges(CPed* ped) {
    m_attackRange = 8.f;
    const auto fight = ped->GetIntelligence()->GetTaskFighting();
    // Como el original: indice max(0, combo - 4) sin limite superior (.data() evita la asercion de std::array)
    m_arriveRange = CTaskSimpleFight::m_aComboData.data()[fight ? std::max(0, fight->m_nComboSet - 4) : 0].m_fRanges;
    if (m_target) {
        m_targetPos = m_target->GetPosition();
    }
}

// 0x626AC0
CTask* CTaskComplexKillPedOnFootMelee::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_FIGHT_CTRL: {
        m_pauseTimer = -1;
        return new CTaskSimpleFightingControl{
            m_target,
            m_attackAngle,
            m_attackRange
        };
    }
    case TASK_COMPLEX_SEEK_ENTITY:
        m_pauseTimer = -1; // Como el original
        return new CTaskComplexSeekEntity<CEntitySeekPosCalculatorStandard>{
            m_target,
            50'000,
            1'000,
            1.f,
            2.f,
            2.f,
            true,
            true
        };
    case TASK_SIMPLE_PAUSE: {
        CTaskSimpleStandStill{}.ProcessPed(ped); // Como el original: se ignora el resultado
        if (m_pauseTimer == -1) {
            m_pauseTimer = (int32)CTimer::GetTimeInMS();
        }
        return new CTaskSimplePause{ 100 }; // Como el original: 100 ms
    }
    case TASK_SIMPLE_STAND_STILL:
        m_pauseTimer = -1; // Como el original
        return new CTaskSimpleStandStill{2000};
    case TASK_COMPLEX_SEQUENCE: {
        m_pauseTimer = -1; // Como el original
        // Como el original: secuencia sin inicializacion por valor; girarse y agitar el puno se crean y se anaden en orden
        auto* const sequence = new CTaskComplexSequence;
        sequence->AddTask(new CTaskComplexTurnToFaceEntityOrCoord{m_target});
        sequence->AddTask(new CTaskSimpleShakeFist{});
        return sequence;
    }
    case TASK_FINISHED:
        m_pauseTimer = -1; // Como el original
        return nullptr;
    default: // Como el original: tipos desconocidos devuelven nullptr
        return nullptr;
    }
}

// Como el original: distancia al cuadrado (0x406DA0, suma x, y, z) de m_targetPos al peaton
static float KillMeleeTargetDistance(CPed* ped, const CVector& targetPos) {
    return (targetPos - ped->GetPosition()).SquaredMagnitude();
}

// Como el original: distancia al cuadrado de m_targetPos al peaton sumando z, x, y
static float KillMeleeTargetDistanceSq(CPed* ped, const CVector& targetPos) {
    const CVector delta = targetPos - ped->GetPosition();
    return delta.z * delta.z + delta.x * delta.x + delta.y * delta.y;
}

// 0x62BC10
CTask* CTaskComplexKillPedOnFootMelee::CreateNextSubTask(CPed* ped) {
    if (!m_target) {
        return nullptr;
    }
    const auto Create = [&](eTaskType tt) { return CreateSubTask(tt, ped); };
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_SEEK_ENTITY:
        return Create(TASK_SIMPLE_FIGHT_CTRL);
    case TASK_SIMPLE_STAND_STILL:
    case TASK_COMPLEX_SEQUENCE: // 0x62BC4A
        // Como el original: bajo el agua se queda quieto; si no, vuelve a empezar
        return m_target->physicalFlags.bSubmergedInWater
            ? Create(TASK_SIMPLE_STAND_STILL)
            : CreateFirstSubTask(ped);
    case TASK_SIMPLE_PAUSE: { // 0x62BC98
        CalculateSearchPositionAndRanges(ped);
        // Como el original: distancia al cuadrado (0x406DA0) frente al rango al cuadrado; NaN pelea
        const float range = m_arriveRange;
        if (!(range * range < KillMeleeTargetDistance(ped, m_targetPos))) {
            return Create(TASK_SIMPLE_FIGHT_CTRL);
        }
        if (!ped->bStayInSamePlace) {
            return Create(TASK_COMPLEX_SEEK_ENTITY);
        }
        return Create(
            m_pauseTimer != -1 && CTimer::GetTimeInMS() - (uint32)m_pauseTimer > 3000
                ? TASK_FINISHED
                : TASK_SIMPLE_PAUSE
        );
    }
    case TASK_FINISHED: { // 0x62BD75
        return nullptr;
    }
    case TASK_SIMPLE_FIGHT_CTRL: { // 0x62BD8B
        if (ped->bStayInSamePlace) {
            return Create(TASK_SIMPLE_PAUSE);
        }
        CalculateSearchPositionAndRanges(ped);
        const float range = m_arriveRange;
        return Create(
            range * range < KillMeleeTargetDistanceSq(ped, m_targetPos)
                ? TASK_COMPLEX_SEEK_ENTITY
                : TASK_SIMPLE_PAUSE
        );
    }
    default: // Como el original: tipos desconocidos devuelven nullptr
        return nullptr;
    }
}

// 0x62BE30
CTask* CTaskComplexKillPedOnFootMelee::CreateFirstSubTask(CPed* ped) {
    g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_19, ped);
    m_bNewTarget = false;
    if (!m_target) {
        return nullptr;
    }
    CalculateSearchPositionAndRanges(ped);
    // Como el original: suma z, x, y; NaN pelea
    const float range = m_arriveRange;
    if (!(range * range < KillMeleeTargetDistanceSq(ped, m_targetPos))) {
        return CreateSubTask(TASK_SIMPLE_FIGHT_CTRL, ped);
    }
    return CreateSubTask(ped->bStayInSamePlace ? TASK_SIMPLE_PAUSE : TASK_COMPLEX_SEEK_ENTITY, ped);
}

// 0x626D90
CTask* CTaskComplexKillPedOnFootMelee::ControlSubTask(CPed* ped) {
    CTask* const subTask = m_pSubTask;
    ped->Say(CTX_GLOBAL_FIGHT);

    if (m_bNewTarget) {
        return CreateFirstSubTask(ped);
    }

    if (!m_target || m_target->m_fHealth <= 0.f) {
        return nullptr;
    }

    const auto TryAbortAndCreate = [&, this](eTaskType taskToCreate) {
        return subTask->MakeAbortable(ped)
            ? CreateSubTask(taskToCreate, ped)
            : subTask;
    };

    switch (subTask->GetTaskType()) {
    case TASK_SIMPLE_STAND_STILL: // Como el original: igual que la secuencia
    case TASK_COMPLEX_SEQUENCE: { // 0x626dfe
        return m_target->physicalFlags.bSubmergedInWater
            ? subTask
            : TryAbortAndCreate(TASK_SIMPLE_FIGHT_CTRL);
    }
    case TASK_SIMPLE_PAUSE: // 0x626e05
    case TASK_COMPLEX_SEEK_ENTITY: {  // 0x626f0e
        if (m_target->physicalFlags.bSubmergedInWater) {
            return TryAbortAndCreate(TASK_COMPLEX_SEQUENCE);
        }
        CalculateSearchPositionAndRanges(ped);
        // Como el original: distancia al cuadrado (0x406DA0) frente al rango al cuadrado; NaN sigue
        const float range = m_arriveRange;
        if (!(range * range > KillMeleeTargetDistance(ped, m_targetPos))) {
            return subTask;
        }
        return TryAbortAndCreate(TASK_SIMPLE_FIGHT_CTRL);
    }
    case TASK_SIMPLE_FIGHT_CTRL: { // 0x626E5C
        if (m_target->physicalFlags.bSubmergedInWater) {
            return TryAbortAndCreate(TASK_COMPLEX_SEQUENCE);
        }
        CalculateSearchPositionAndRanges(ped);
        // Como el original: distancia al cuadrado (0x406DA0) frente al rango al cuadrado; NaN sigue
        const float range = m_attackRange;
        if (!(range * range < KillMeleeTargetDistance(ped, m_targetPos))) {
            return subTask;
        }
        return TryAbortAndCreate(
            ped->bStayInSamePlace
                ? TASK_SIMPLE_PAUSE
                : TASK_COMPLEX_SEEK_ENTITY
        );
    }
    default:
        return subTask;
    }
}
