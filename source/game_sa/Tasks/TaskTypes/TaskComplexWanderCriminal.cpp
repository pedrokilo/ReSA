/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWanderCriminal
 * Reconstrucción: INCOMPLETA — 3/6 funciones reconstruidas.
 * Pendientes de reconstruir: 3.
 * Funciones bloqueadas: 3.
 * Validación registrada: 0/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   Clone | 0x0048E650 | V
 *   LookForCarsToSteal | 0x0066B4F0 | S
 *   ScanForStuff | 0x00670350 | V
 * BLOQUEADAS (nombre | dirección | tipo):
 *   Clone | 0x0048E650 | V
 *   LookForCarsToSteal | 0x0066B4F0 | S
 *   ScanForStuff | 0x00670350 | V
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexWanderCriminal.h"

void CTaskComplexWanderCriminal::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderCriminal, 0x85A23C, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x48E610);
    RH_ScopedInstall(Destructor, 0x48E720);
    RH_ScopedInstall(LookForCarsToSteal, 0x66B4F0, { .reversed = false });

    // El original (0x48E650) pasa también m_bWanderSensibly; el Clone inline usa el valor por defecto.
    RH_ScopedVMTInstall(Clone, 0x48E650, { .reversed = false });
    RH_ScopedVMTInstall(GetWanderType, 0x48E6F0);
    RH_ScopedVMTInstall(ScanForStuff, 0x670350, { .reversed = false });
}

// 0x48E610
CTaskComplexWanderCriminal::CTaskComplexWanderCriminal(eMoveState MoveState, uint8 Dir, bool bWanderSensibly) : CTaskComplexWander(MoveState, Dir, bWanderSensibly) { }

// 0x670350
void CTaskComplexWanderCriminal::ScanForStuff(CPed* ped) {
    plugin::CallMethod<0x670350, CTaskComplexWanderCriminal*, CPed*>(this, ped);
    return;

    // todo: weird
    if (m_TaskTimer.m_bStarted
        || (
               m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS(),
               m_TaskTimer.m_nInterval = 50,
               m_TaskTimer.m_bStarted = true,
               m_nMinNextScanTime = CTimer::GetTimeInMS() + (uint32)CGeneral::GetRandomNumberInRange(0.0f, 30'000.0f),
               m_TaskTimer.m_bStarted
           )
    ) {
        if (m_TaskTimer.m_bStopped) {
            m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_TaskTimer.m_bStopped = false;
        }

        if (CTimer::GetTimeInMS() >= m_TaskTimer.m_nStartTime + m_TaskTimer.m_nInterval) {
            m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_TaskTimer.m_nInterval = 50;
            m_TaskTimer.m_bStarted = true;
            if (CTimer::GetTimeInMS() >= m_nMinNextScanTime) {
                LookForCarsToSteal(ped);
            }
        }
    }
}

// 0x66B4F0
void CTaskComplexWanderCriminal::LookForCarsToSteal(CPed* ped) {
    plugin::CallMethod<0x66B4F0, CTaskComplexWanderCriminal*, CPed*>(this, ped);
}
