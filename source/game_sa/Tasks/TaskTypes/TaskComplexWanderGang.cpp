/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWanderGang
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexWanderGang.h"

void CTaskComplexWanderGang::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWanderGang, 0x8700D4, 15);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x66F5C0);

    RH_ScopedVMTInstall(Clone, 0x671470);
    RH_ScopedVMTInstall(GetWanderType, 0x66F630);
    RH_ScopedVMTInstall(ScanForStuff, 0x66F640);
}

// 0x66F5C0
CTaskComplexWanderGang::CTaskComplexWanderGang(eMoveState moveState, uint8 dir, uint32 scanTime, bool bWanderSensibly, float fTargetRadius)
    : CTaskComplexWander(moveState, dir, bWanderSensibly, fTargetRadius)
{
    m_NextScanTime = scanTime;
    m_TaskTimer.m_nInterval = scanTime;
    m_TaskTimer.m_nStartTime = CTimer::GetTimeInMS();
    m_TaskTimer.m_bStarted = true;
}

// 0x66F640
void CTaskComplexWanderGang::ScanForStuff(CPed* ped) {
    // NOP
}
