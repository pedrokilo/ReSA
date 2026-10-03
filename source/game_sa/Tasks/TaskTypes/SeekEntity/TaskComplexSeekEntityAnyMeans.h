#pragma once

#include <type_traits>
#include <concepts>

#include "Base.h"
#include "TaskComplex.h"
#include "PosCalculators/EntitySeekPosCalculator.h"
#include "TaskComplexDriveToPoint.h"
#include "TaskComplexLeaveAnyCar.h"
#include "TaskTimer.h"
#include "Entity.h"

class CPed;

/*!
* Follows an entity using a car.
* When the entity is destroyed the ped leaves the car.
* Do not use directly, use the derived classes instead, 
* such as `CTaskComplexSeekEntityAnyMeansXYOffset`
*
* @tparam T The position calculator to use
*/
template<std::derived_from<CEntitySeekPosCalculator> T>
class NOTSA_EXPORT_VTABLE CTaskComplexSeekEntityAnyMeans : public CTaskComplex {
    CEntity*   m_entity{};        /// The entity to seek
    int32      m_scanInterval{};  /// Entity pos seek interval 
    float      m_targetRadius{};  /// The "tolerance"
    CTaskTimer m_scanTimer;       /// Como el original: sin value-init (el ctor 0x695600 no escribe el relleno +0x22..+0x23). Timer which on expiration updates the subtasks' goto point and is re-started (Uses `m_scanInterval` as the interval)
    T          m_posCalculator{}; /// The position calculator
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_SEEK_ENTITY_ANY_MEANS;

protected:
    CTaskComplexSeekEntityAnyMeans(CEntity* entity, int32 period, float radius, T posCalculator) :
        m_targetRadius{ radius },
        m_entity{ entity },
        m_scanInterval{ period },
        m_posCalculator{ posCalculator }
    {
        CEntity::SafeRegisterRef(m_entity);
    }

public:
    CTaskComplexSeekEntityAnyMeans(const CTaskComplexSeekEntityAnyMeans& o) :
        m_targetRadius{ 8.f },
        m_entity{ o.m_entity },
        m_scanInterval{ 1000 },
        m_posCalculator{ o.m_posCalculator }
    {
        CEntity::SafeRegisterRef(m_entity); // Como el original: Clone (0x696030) usa el ctor 0x695600, que registra la referencia
    }

    ~CTaskComplexSeekEntityAnyMeans() {
        CEntity::SafeCleanUpRef(m_entity);
    }

    CTask* Clone() const override {
        return new CTaskComplexSeekEntityAnyMeans{ *this };
    }

    eTaskType GetTaskType() const override {
        return Type;
    }

    CTask*    CreateNextSubTask(CPed* ped) override {
        return nullptr;
    }

    CTask* CreateFirstSubTask(CPed* ped) override {
        // Como el original (0x6960B0): calcula la posición, arranca el temporizador (sin comprobar intervalo < 0) y después reserva la subtarea
        const CVector seekPos = GetSeekPos(ped);
        StartScanTimer();
        return new CTaskComplexDriveToPoint{ nullptr, seekPos, 30.f, 0, MODEL_INVALID, m_targetRadius, DRIVING_STYLE_STOP_FOR_CARS };
    }

    CTask* ControlSubTask(CPed* ped) override {
        if (m_entity) {
            if (m_scanTimer.IsOutOfTime()) {
                StartScanTimer();
                // Como el original (0x696232): escribe el punto en +0x24 de la subtarea sin comprobar su tipo
                static_cast<CTaskComplexDriveToPoint*>(m_pSubTask)->GoToPoint(GetSeekPos(ped));
            }
            return m_pSubTask;
        }
        return new CTaskComplexLeaveAnyCar{ 0, true, false };
    }

    // Como el original: Start inlineado, escribe siempre inicio/intervalo/arrancado (no toca m_bStopped)
    void StartScanTimer() {
        m_scanTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_scanTimer.m_nInterval  = m_scanInterval;
        m_scanTimer.m_bStarted   = true;
    }

    CVector GetSeekPos(CPed* ped) {
        CVector seekPos{};
        m_posCalculator.ComputeEntitySeekPos(*ped, *m_entity, seekPos);
        return seekPos;
    }
};
