#pragma once

#include "TaskSimple.h"
#include "TaskTimer.h"

class CPed;
class CEvent;

// Tarea simple que espera al repartidor de pizza (vtable 0x86E1DC del EXE): un temporizador de 2000 ms en +8 (CTaskTimer: inicio, intervalo, iniciado, detenido). Aun sin constructor de la DLL: el del EXE
// (0x632B30) esta enganchado como funcion libre que deja la vtable del EXE, como el cuerpo del destructor (0x632B70).
class NOTSA_EXPORT_VTABLE CTaskSimpleWaitForPizza : public CTaskSimple {
public:
    CTaskTimer m_timer;

public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_WAIT_FOR_PIZZA;

    static void InjectHooks();

    eTaskType GetTaskType() const override;                                                                                                // 0x632B50 (lote 171A, banco Fn171A)
    CTask* Clone() const override;                                                                                                         // 0x636B40 (lote 171A)
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;              // 0x632B60 (lote 171A)
    bool ProcessPed(CPed* ped) override;                                                                                                   // 0x632B80 (lote 171A)
};
VALIDATE_SIZE(CTaskSimpleWaitForPizza, 0x14);
