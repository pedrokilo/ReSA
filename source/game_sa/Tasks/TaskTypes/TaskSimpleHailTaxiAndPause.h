#pragma once

#include "TaskSimple.h"

class CPed;
class CEvent;

// Plantilla: clase todavía no reconstruida. Sin constructor, campos ni
// VALIDATE_SIZE; no se registra en InjectHooksMain ni figura en hooks.csv.
class NOTSA_EXPORT_VTABLE CTaskSimpleHailTaxiAndPause : public CTaskSimple {
public:
    // TODO: tipo de tarea (eTaskType) sin identificar.

    static void InjectHooks();

    // TODO: constructor, destructor y campos.

    // eTaskType GetTaskType() const override;
    // CTask* Clone() const override;
    // bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    // bool ProcessPed(CPed* ped) override;
};
