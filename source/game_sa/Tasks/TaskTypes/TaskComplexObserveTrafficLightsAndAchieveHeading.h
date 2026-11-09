/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexObserveTrafficLightsAndAchieveHeading
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#pragma once

#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexObserveTrafficLightsAndAchieveHeading : public CTaskComplex {
public:
    int32 m_nDurationInMs;
    float m_fTargetHeading;

public:
    static constexpr auto Type = TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS_AND_ACHIEVE_HEADING;

    CTaskComplexObserveTrafficLightsAndAchieveHeading(int32 durationInMs, float targetHeading);
    ~CTaskComplexObserveTrafficLightsAndAchieveHeading() override = default; // 0x631980

    eTaskType GetTaskType() const override { return Type; } // 0x631940
    CTask* Clone() const override;
    CTask* ControlSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;

    CTask* CreateSubTask(eTaskType taskType) const;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexObserveTrafficLightsAndAchieveHeading* Constructor(int32 durationInMs, float fTargetHeading) {
        this->CTaskComplexObserveTrafficLightsAndAchieveHeading::CTaskComplexObserveTrafficLightsAndAchieveHeading(durationInMs, fTargetHeading);
        return this;
    }

};

VALIDATE_SIZE(CTaskComplexObserveTrafficLightsAndAchieveHeading, 0x14);
