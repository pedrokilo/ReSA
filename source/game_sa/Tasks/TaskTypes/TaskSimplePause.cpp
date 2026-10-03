/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimplePause
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

#include "TaskSimplePause.h"

void CTaskSimplePause::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimplePause, 0x85A278, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x48E750);
    RH_ScopedVMTInstall(ProcessPed, 0x48E830);
    RH_ScopedVMTInstall(Clone, 0x48E780);
    RH_ScopedVMTInstall(MakeAbortable, 0x48E810);
}

// 0x48E750
CTaskSimplePause::CTaskSimplePause(int32 pauseIntervalMs)  :
    m_PauseIntervalMs{pauseIntervalMs}
{
}

// For 0x48E780
CTaskSimplePause::CTaskSimplePause(const CTaskSimplePause& o) :
    CTaskSimplePause{o.m_PauseIntervalMs}
{
}

CTaskSimplePause* CTaskSimplePause::Constructor(int32 time) {
    this->CTaskSimplePause::CTaskSimplePause(time);
    return this;
}

// 0x48E810
bool CTaskSimplePause::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    m_Timer.SetAsOutOfTime();
    return true;
}

// 0x48E830
bool CTaskSimplePause::ProcessPed(CPed* ped) {
    if (!m_Timer.m_bStarted) {
        // Como el original: arranca el temporizador sin comprobar que el intervalo sea >= 0
        m_Timer.m_nStartTime = CTimer::GetTimeInMS();
        m_Timer.m_nInterval  = m_PauseIntervalMs;
        m_Timer.m_bStarted   = true;
        if (!m_Timer.m_bStarted) {
            return false;
        }
    }
    m_Timer.Reset();
    return m_Timer.IsOutOfTime();
}
